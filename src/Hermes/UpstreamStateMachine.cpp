/***********************************************************************
Copyright 2018 ASM Assembly Systems GmbH & Co. KG

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
************************************************************************/

#include "stdafx.h"
#include "UpstreamStateMachine.h"
#include "MessageSerialization.h"
#include "UpstreamSerializer.h"

#include "IService.h"


namespace Hermes
{
    namespace Upstream
    {
        struct StateMachine : IStateMachine, ISerializerCallback
        {
            unsigned m_sessionId;
            EState m_state{EState::eNOT_CONNECTED};
            IAsioService& m_service;
            ISerializer& m_forward;
            IStateMachineCallback* m_pCallback = nullptr;
            ECheckState m_checkState = ECheckState::eSEND_AND_RECEIVE;
            std::string m_startTransportBoardId;

            StateMachine(unsigned sessionId, IAsioService& service, ISerializer& forward, ECheckState checkStateConsistency) :
                m_sessionId(sessionId),
                m_service(service),
                m_forward(forward),
                m_checkState(checkStateConsistency)
            {}

            bool DisconnectedDueToIllegalClientEvent_(StringView event)
            {
                if (m_checkState != ECheckState::eSEND_AND_RECEIVE)
                {
                    m_service.Warn(m_sessionId, "Unexpected (but tolerated) client event in state ", m_state, ": ", event);
                    return false;
                }

                auto error = m_service.Alarm(m_sessionId, EErrorCode::eCLIENT_ERROR,
                    "Unexpected client event in state ", m_state, ": ", event);

                if (!m_pCallback)
                    return true;

                if (m_state == EState::eDISCONNECTED)
                    return true;

                if (m_state != EState::eNOT_CONNECTED)
                {
                    m_forward.Signal(Serialize(NotificationData(ENotificationCode::eUNSPECIFIC, ESeverity::eFATAL, "SoftwareError")));
                }

                m_state = EState::eDISCONNECTED;
                m_pCallback->OnDisconnected(m_state, error);
                m_forward.Disconnect();
                return true;
            }

            void OnUnexpectedPeerEvent_(StringView event)
            {
                auto error = m_service.Alarm(m_sessionId, EErrorCode::ePEER_ERROR,
                    "Unexpected peer event in state ", m_state, ": ", event);

                if (m_state == EState::eDISCONNECTED)
                    return;

                m_state = EState::eDISCONNECTED;
                m_pCallback->OnDisconnected(m_state, error);
                m_forward.Signal(Serialize(NotificationData(ENotificationCode::ePROTOCOL_ERROR, ESeverity::eFATAL, error.m_text)));
                m_forward.Disconnect();
            }

            //================ IUpstreamStateMachine =================================
            void Connect(std::weak_ptr<void> wpOwner, IStateMachineCallback& callback) override
            {
                assert(!m_pCallback);
                assert(m_state == EState::eNOT_CONNECTED);
                m_pCallback = &callback;
                m_forward.Connect(std::move(wpOwner), *this);
            }

            void Signal(const ServiceDescriptionData&, StringView rawXml) override
            {
                switch (m_state)
                {
                case EState::eSOCKET_CONNECTED:
                    m_state = EState::eSERVICE_DESCRIPTION_DOWNSTREAM;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(rawXml);
                    return;

                default:
                    if (DisconnectedDueToIllegalClientEvent_("ServiceDescription"))
                        return;
                    m_forward.Signal(rawXml);
                }
            }

            void Signal(const MachineReadyData&, StringView rawXml) override
            {
                switch (m_state)
                {
                case EState::eNOT_AVAILABLE_NOT_READY:
                    m_state = EState::eMACHINE_READY;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(rawXml);
                    return;

                case EState::eBOARD_AVAILABLE:
                    m_state = EState::eAVAILABLE_AND_READY;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(rawXml);
                    return;

                default:
                    if (DisconnectedDueToIllegalClientEvent_("MachineReady"))
                        return;
                    m_forward.Signal(rawXml);
                }
            }

            void Signal(const RevokeMachineReadyData&, StringView rawXml) override
            {
                switch (m_state)
                {
                case EState::eMACHINE_READY:
                    m_state = EState::eNOT_AVAILABLE_NOT_READY;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(rawXml);
                    return;

                case EState::eAVAILABLE_AND_READY:
                    m_state = EState::eBOARD_AVAILABLE;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(rawXml);
                    return;

                default:
                    if (DisconnectedDueToIllegalClientEvent_("RevokeMachineReady"))
                        return;
                    m_forward.Signal(rawXml);
                }
            }

            void Signal(const StartTransportData& data, StringView rawXml) override
            {
                switch (m_state)
                {
                case EState::eAVAILABLE_AND_READY:
                    m_state = EState::eTRANSPORTING;
                    m_startTransportBoardId = rawXml.empty() ? data.m_boardId : std::string();
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(rawXml);
                    return;

                case EState::eMACHINE_READY:
                    m_state = EState::eTRANSPORTING;
                    m_startTransportBoardId = rawXml.empty() ? data.m_boardId : std::string();
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(rawXml);
                    return;

                default:
                    if (DisconnectedDueToIllegalClientEvent_("StartTransport"))
                        return;
                    m_startTransportBoardId.clear();
                    m_forward.Signal(rawXml);
                }
            }

            void Signal(const StopTransportData&, StringView rawXml) override
            {
                switch (m_state)
                {
                case EState::eTRANSPORTING:
                    m_state = EState::eTRANSPORT_STOPPED;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(rawXml);
                    return;

                case EState::eTRANSPORT_FINISHED:
                    m_state = EState::eNOT_AVAILABLE_NOT_READY;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(rawXml);
                    return;

                default:
                    if (DisconnectedDueToIllegalClientEvent_("StopTransport"))
                        return;
                    m_forward.Signal(rawXml);
                }
            }

            void Signal(const QueryBoardInfoData&, StringView rawXml) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_forward.Signal(rawXml);
                    return;
                }
            }

            void Signal(const NotificationData&, StringView rawXml) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_forward.Signal(rawXml);
                    return;
                }
            }

            void Signal(const CommandData&, StringView rawXml) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_forward.Signal(rawXml);
                    return;
                }
            }

            void Signal(const CheckAliveData&, StringView rawXml) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_forward.Signal(rawXml);
                    return;
                }
            }

            void Disconnect() override
            {
                switch (m_state)
                {
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_state = EState::eDISCONNECTED;
                    m_pCallback->OnState(m_state);
                    m_forward.Disconnect();

                }
            }

            //============== IUpstreamDeserializerCallback ===================
            void OnSocketConnected(const ConnectionInfo& connectionInfo) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                    m_state = EState::eSOCKET_CONNECTED;
                    m_pCallback->OnSocketConnected(m_state, connectionInfo);
                    return;

                default:
                    return OnUnexpectedPeerEvent_("Connected");
                }
            }

            void On(const ServiceDescriptionData& data) override
            {
                switch (m_state)
                {
                case EState::eSERVICE_DESCRIPTION_DOWNSTREAM:
                {
                    m_state = EState::eNOT_AVAILABLE_NOT_READY;
                    m_pCallback->On(m_state, data);
                    return;
                }
                default:
                    return OnUnexpectedPeerEvent_("ServiceDescription");
                }
            }

            void On(const BoardAvailableData& data) override
            {
                switch (m_state)
                {
                case EState::eNOT_AVAILABLE_NOT_READY:
                {
                    m_state = EState::eBOARD_AVAILABLE;
                    m_pCallback->On(m_state, data);
                    return;
                }
                case EState::eMACHINE_READY:
                {
                    m_state = EState::eAVAILABLE_AND_READY;
                    m_pCallback->On(m_state, data);
                    return;
                }
                case EState::eTRANSPORTING:
                case EState::eTRANSPORT_STOPPED:
                    return;

                default:
                    return OnUnexpectedPeerEvent_("BoardAvailable");
                }
            }

            void On(const RevokeBoardAvailableData& data) override
            {
                switch (m_state)
                {
                case EState::eBOARD_AVAILABLE:
                {
                    m_state = EState::eNOT_AVAILABLE_NOT_READY;
                    m_pCallback->On(m_state, data);
                    return;
                }
                case EState::eAVAILABLE_AND_READY:
                {
                    m_state = EState::eMACHINE_READY;
                    m_pCallback->On(m_state, data);
                    return;
                }
                case EState::eTRANSPORTING:
                case EState::eTRANSPORT_STOPPED:
                    return;

                default:
                    return OnUnexpectedPeerEvent_("RevokeBoardAvailable");
                }
            }
            
            void StartAndStopBoardIdDoNotMatch_(const std::string& startId, const std::string& stopId)
            {
                std::ostringstream oss;
                oss << "StartTransport BoardId={" << startId << "} does not match StopTransport BoardId={" << stopId << '}';
                OnUnexpectedPeerEvent_(oss.str());
            }

            void On(const TransportFinishedData& data) override
            {
                switch (m_state)
                {
                case EState::eTRANSPORT_STOPPED:
                {
                    if (data.m_boardId != m_startTransportBoardId && !m_startTransportBoardId.empty())
                        return StartAndStopBoardIdDoNotMatch_(m_startTransportBoardId, data.m_boardId);
                    m_state = EState::eNOT_AVAILABLE_NOT_READY;
                    m_pCallback->On(m_state, data);
                    return;
                }
                case EState::eTRANSPORTING:
                {
                    if (data.m_boardId != m_startTransportBoardId && !m_startTransportBoardId.empty())
                        return StartAndStopBoardIdDoNotMatch_(m_startTransportBoardId, data.m_boardId);
                    m_state = EState::eTRANSPORT_FINISHED;
                    m_pCallback->On(m_state, data);
                    return;
                }
                default:
                    return OnUnexpectedPeerEvent_("TransportFinished");
                }
            }

            void On(const BoardForecastData& data) override
            {
                switch (m_state)
                {
                case EState::eNOT_AVAILABLE_NOT_READY:
                {
                    m_pCallback->On(m_state, data);
                    return;
                }
                case EState::eMACHINE_READY:
                {
                    m_pCallback->On(m_state, data);
                    return;
                }
                case EState::eTRANSPORTING:
                case EState::eTRANSPORT_STOPPED:
                    return;

                default:
                    return OnUnexpectedPeerEvent_("BoardForeast");
                }
            }

            void On(const SendBoardInfoData& data) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_pCallback->On(m_state, data);
                }
            }

            void On(const NotificationData& data) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_pCallback->On(m_state, data);
                }
            }

            void On(const CommandData& data) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_pCallback->On(m_state, data);
                }
            }

            void On(const CheckAliveData& data) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_pCallback->On(m_state, data);
                }
            }

            void OnDisconnected(const Error& error) override
            {
                switch (m_state)
                {
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_state = EState::eDISCONNECTED;
                    m_pCallback->OnDisconnected(m_state, error);
                }
            }

        };

    }

    using namespace Upstream;
    std::unique_ptr<IStateMachine> Upstream::CreateStateMachine(unsigned sessionId,
        IAsioService& service, ISerializer& forward, ECheckState checkStateConsistency)
    {
        return std::make_unique<StateMachine>(sessionId, service, forward, checkStateConsistency);
    }

}
