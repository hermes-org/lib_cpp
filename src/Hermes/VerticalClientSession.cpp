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

#include "VerticalClientSession.h"

#include "Network.h"
#include "VerticalClientSerializer.h"
#include "IService.h"
#include "MessageSerialization.h"
#include "StringBuilder.h"

#include "Network.h"

#include <HermesData.hpp>

namespace Hermes
{
    namespace asio = boost::asio;

    namespace VerticalClient
    {
        struct Session::Impl : ISerializerCallback
        {
            unsigned m_id;
            EVerticalState m_state{ EVerticalState::eNOT_CONNECTED };
            IAsioService& m_service;
            VerticalClientSettings m_configuration;

            std::unique_ptr<IClientSocket> m_upSocket;
            std::unique_ptr<ISerializer> m_upSerializer;
            Optional<SupervisoryServiceDescriptionData> m_optionalPeerServiceDescriptionData;
            ConnectionInfo m_peerConnectionInfo;

            ISessionCallback* m_pCallback{ nullptr };

            Impl(unsigned id, IAsioService& service, const VerticalClientSettings& configuration) :
                m_id(id),
                m_service(service),
                m_configuration(configuration)
            {
                m_service.Log(m_id, "VerticalClientSession()");
            }

            ~Impl()
            {
                m_service.Log(m_id, "~VerticalClientSession()");
            }

            bool DisconnectedDueToIllegalClientEvent_(StringView event)
            {
                auto error = m_service.Alarm(m_id, EErrorCode::eCLIENT_ERROR,
                    "Unexpected client event in state ", m_state, ": ", event);

                if (!m_pCallback)
                    return true;

                if (m_state == EVerticalState::eDISCONNECTED)
                    return true;

                if (m_state != EVerticalState::eNOT_CONNECTED)
                {
                    m_upSerializer->Signal(Serialize(NotificationData(ENotificationCode::eUNSPECIFIC, ESeverity::eFATAL, "SoftwareError")));
                }

                m_state = EVerticalState::eDISCONNECTED;
                m_pCallback->OnDisconnected(m_id, m_state, error);
                m_upSerializer->Disconnect();
                return true;
            }

            void OnUnexpectedPeerEvent_(StringView event)
            {
                auto error = m_service.Alarm(m_id, EErrorCode::ePEER_ERROR,
                    "Unexpected peer event in state ", m_state, ": ", event);

                if (m_state == EVerticalState::eDISCONNECTED)
                    return;

                m_state = EVerticalState::eDISCONNECTED;
                m_pCallback->OnDisconnected(m_id, m_state, error);
                m_upSerializer->Signal(Serialize(NotificationData(ENotificationCode::ePROTOCOL_ERROR, ESeverity::eFATAL, error.m_text)));
                m_upSerializer->Disconnect();
            }


            //============= implementation of ISerializerCallback ============
            void OnSocketConnected(const ConnectionInfo& connectionInfo) override
            {
                switch (m_state)
                {
                case EVerticalState::eNOT_CONNECTED:
                    m_state = EVerticalState::eSOCKET_CONNECTED;
                    m_peerConnectionInfo = connectionInfo;
                    m_pCallback->OnSocketConnected(m_id, m_state, connectionInfo);
                    return;

                default:
                    return OnUnexpectedPeerEvent_("Connected");
                }
            }

            void On(const SupervisoryServiceDescriptionData& data) override
            {
                switch (m_state)
                {
                case EVerticalState::eSUPERVISORY_SERVICE_DESCRIPTION:
                    m_state = EVerticalState::eCONNECTED;
                    m_optionalPeerServiceDescriptionData = data;
                    m_pCallback->On(m_id, m_state, data);
                    return;

                default:
                    return OnUnexpectedPeerEvent_("ServiceDescription");
                }
            }

            template<class DataT>
            void On_(const DataT& data, const char* name)
            {
                switch (m_state)
                {
                case EVerticalState::eNOT_CONNECTED:
                case EVerticalState::eDISCONNECTED:
                    return OnUnexpectedPeerEvent_(name);

                default:
                    m_pCallback->On(m_id, m_state, data);
                    return;
                }
            }

            void On(const CurrentConfigurationData& data) override { On_(data, "CurrentConfiguration"); }
            void On(const BoardArrivedData& data) override { On_(data, "BoardArrived"); }
            void On(const BoardDepartedData& data) override { On_(data, "BoardDeparted"); }
            void On(const QueryWorkOrderInfoData& data) override { On_(data, "QueryWorkOrderInfo"); }
            void On(const NotificationData& data) override { On_(data, "Notification"); }
            void On(const CheckAliveData& data) override { On_(data, "CheckAlive"); }

            void Signal_(StringView rawXml)
            {
                switch (m_state)
                {
                case EVerticalState::eNOT_CONNECTED:
                case EVerticalState::eDISCONNECTED:
                    return;

                default:
                    m_upSerializer->Signal(rawXml);
                    return;
                }
            }

            void OnDisconnected(const Error& error) override
            {
                switch (m_state)
                {
                case EVerticalState::eDISCONNECTED:
                    return;

                default:
                    m_state = EVerticalState::eDISCONNECTED;
                    m_pCallback->OnDisconnected(m_id, m_state, error);
                }
            }
        };


        Session::Session(unsigned id, IAsioService& service,
            const VerticalClientSettings& configuration)
        {
            m_spImpl = std::make_shared<Impl>(id, service, configuration);

            NetworkConfiguration socketConfig;
            socketConfig.m_hostName = configuration.m_hostAddress;
            socketConfig.m_port = configuration.m_port;
            socketConfig.m_retryDelayInSeconds = configuration.m_reconnectWaitTimeInSeconds;
            socketConfig.m_checkAlivePeriodInSeconds = configuration.m_checkAlivePeriodInSeconds;

            m_spImpl->m_upSocket = CreateClientSocket(id, socketConfig, service);
            m_spImpl->m_upSerializer = CreateSerializer(id, service, *m_spImpl->m_upSocket);
        }

        Session::~Session()
        {
            if (!m_spImpl)
                return;

            m_spImpl->m_pCallback = nullptr;
        }

        unsigned Session::Id() const { return m_spImpl->m_id; }
        const Optional<SupervisoryServiceDescriptionData>& Session::OptionalPeerServiceDescriptionData() const
        {
            return m_spImpl->m_optionalPeerServiceDescriptionData;
        }

        const ConnectionInfo& Session::PeerConnectionInfo() const { return m_spImpl->m_peerConnectionInfo; }

        void Session::Connect(ISessionCallback& callback)
        {
            m_spImpl->m_pCallback = &callback;
            m_spImpl->m_upSerializer->Connect(m_spImpl, *m_spImpl);
        }

        void Session::Signal(const SupervisoryServiceDescriptionData&, StringView rawXml)
        {
            switch (m_spImpl->m_state)
            {
            case EVerticalState::eSOCKET_CONNECTED:
                m_spImpl->m_state = EVerticalState::eSUPERVISORY_SERVICE_DESCRIPTION;
                m_spImpl->m_upSerializer->Signal(rawXml);
                return;

            default:
                if (m_spImpl->DisconnectedDueToIllegalClientEvent_("ServiceDescription"))
                    return;
                m_spImpl->m_upSerializer->Signal(rawXml);
            }
        }

        void Session::Signal(const SendWorkOrderInfoData&, StringView rawXml) { m_spImpl->Signal_(rawXml); }
        void Session::Signal(const SetConfigurationData&, StringView rawXml) { m_spImpl->Signal_(rawXml); }
        void Session::Signal(const GetConfigurationData&, StringView rawXml) { m_spImpl->Signal_(rawXml); }
        void Session::Signal(const NotificationData&, StringView rawXml) { m_spImpl->Signal_(rawXml); }
        void Session::Signal(const CheckAliveData&, StringView rawXml) { m_spImpl->Signal_(rawXml); }

        void Session::Disconnect()
        {
            switch (m_spImpl->m_state)
            {
            case EVerticalState::eDISCONNECTED:
                return;

            default:
                m_spImpl->m_state = EVerticalState::eDISCONNECTED;
                m_spImpl->m_upSerializer->Disconnect();
            }
        }
    }
}