// Copyright(c) ASM Assembly Systems GmbH & Co.KG
#include "stdafx.h"
#include "UpstreamStateMachine.h"
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
                    m_forward.Signal(NotificationData(ENotificationCode::eUNSPECIFIC, ESeverity::eFATAL, "SoftwareError"));
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
                m_forward.Signal(NotificationData(ENotificationCode::ePROTOCOL_ERROR, ESeverity::eFATAL, error.m_text));
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

            void Signal(const ServiceDescription& data) override
            {
                switch (m_state)
                {
                case EState::eSOCKET_CONNECTED:
                    m_state = EState::eSERVICE_DESCRIPTION_DOWNSTREAM;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(data);
                    return;

                default:
                    if (DisconnectedDueToIllegalClientEvent_("ServiceDescription"))
                        return;
                    m_forward.Signal(data);
                }
            }

            void Signal(const MachineReadyData& data) override
            {
                switch (m_state)
                {
                case EState::eNOT_AVAILABLE_NOT_READY:
                    m_state = EState::eMACHINE_READY;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(data);
                    return;

                case EState::eBOARD_AVAILABLE:
                    m_state = EState::eAVAILABLE_AND_READY;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(data);
                    return;

                default:
                    if (DisconnectedDueToIllegalClientEvent_("MachineReady"))
                        return;
                    m_forward.Signal(data);
                }
            }

            void Signal(const RevokeMachineReadyData& data) override
            {
                switch (m_state)
                {
                case EState::eMACHINE_READY:
                    m_state = EState::eNOT_AVAILABLE_NOT_READY;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(data);
                    return;

                case EState::eAVAILABLE_AND_READY:
                    m_state = EState::eBOARD_AVAILABLE;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(data);
                    return;

                default:
                    if (DisconnectedDueToIllegalClientEvent_("RevokeMachineReady"))
                        return;
                    m_forward.Signal(data);
                }
            }

            void Signal(const StartTransportData& data) override
            {
                switch (m_state)
                {
                case EState::eAVAILABLE_AND_READY:
                    m_state = EState::eTRANSPORTING;
                    m_startTransportBoardId = data.m_boardId;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(data);
                    return;

                case EState::eMACHINE_READY:
                    m_state = EState::eTRANSPORTING;
                    m_startTransportBoardId = data.m_boardId;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(data);
                    return;

                default:
                    if (DisconnectedDueToIllegalClientEvent_("StartTransport"))
                        return;
                    m_forward.Signal(data);
                }
            }

            void Signal(const StopTransportData& data) override
            {
                switch (m_state)
                {
                case EState::eTRANSPORTING:
                    m_state = EState::eTRANSPORT_STOPPED;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(data);
                    return;

                case EState::eTRANSPORT_FINISHED:
                    m_state = EState::eNOT_AVAILABLE_NOT_READY;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(data);
                    return;

                default:
                    if (DisconnectedDueToIllegalClientEvent_("StopTransport"))
                        return;
                    m_forward.Signal(data);
                }
            }

            void Signal(const NotificationData& data) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_forward.Signal(data);
                    return;
                }
            }

            void Signal(const CheckAliveData& data) override
            {
                switch (m_state)
                {
                case EState::eNOT_CONNECTED:
                case EState::eDISCONNECTED:
                    return;

                default:
                    m_forward.Signal(data);
                    return;
                }
            }

            void Disconnect(const NotificationData& data) override
            {
                switch (m_state)
                {
                case EState::eDISCONNECTED:
                    return;

                case EState::eNOT_CONNECTED:
                    m_state = EState::eDISCONNECTED;
                    m_pCallback->OnState(m_state);
                    m_forward.Disconnect();
                    return;

                default:
                    m_state = EState::eDISCONNECTED;
                    m_pCallback->OnState(m_state);
                    m_forward.Signal(data);
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

            void On(const ServiceDescription& data) override
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
                    if (data.m_boardId != m_startTransportBoardId)
                        return StartAndStopBoardIdDoNotMatch_(m_startTransportBoardId, data.m_boardId);
                    m_state = EState::eNOT_AVAILABLE_NOT_READY;
                    m_pCallback->On(m_state, data);
                    return;
                }
                case EState::eTRANSPORTING:
                {
                    if (data.m_boardId != m_startTransportBoardId)
                        return StartAndStopBoardIdDoNotMatch_(m_startTransportBoardId, data.m_boardId);
                    m_state = EState::eTRANSPORT_FINISHED;
                    m_pCallback->On(m_state, data);
                    return;
                }
                default:
                    return OnUnexpectedPeerEvent_("TransportFinished");
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
                    return;
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
                    return;
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
