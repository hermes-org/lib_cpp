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
#include "UpstreamSession.h"

#include "Network.h"
#include "UpstreamSerializer.h"
#include "UpstreamStateMachine.h"
#include <HermesData.hpp>
#include "IService.h"

#include "Network.h"

namespace Hermes
{
    namespace asio = boost::asio;

    namespace Upstream
    {
        struct Session::Impl : IStateMachineCallback, std::enable_shared_from_this<Impl>
        {
            unsigned m_id;
            IAsioService& m_service;
            UpstreamSettings m_configuration;

            std::unique_ptr<IClientSocket> m_upSocket;
            std::unique_ptr<ISerializer> m_upSerializer;
            std::unique_ptr<IStateMachine> m_upStateMachine;
            Optional<ServiceDescriptionData> m_optionalPeerServiceDescriptionData;
            ConnectionInfo m_peerConnectionInfo;

            ISessionCallback* m_pCallback{nullptr};

            Impl(unsigned id, IAsioService& service, const UpstreamSettings& configuration) :
                m_id(id),
                m_service(service),
                m_configuration(configuration)
            {
                m_service.Log(m_id, "UpstreamSession()");
            }

            ~Impl()
            {
                m_service.Log(m_id, "~UpstreamSession()");
            }

            //============= implementation of ICallback ============
            void OnSocketConnected(EState state, const ConnectionInfo& connectionInfo) override
            {
                m_peerConnectionInfo = connectionInfo;
                if (!m_pCallback)
                    return;

                m_pCallback->OnSocketConnected(m_id, state, connectionInfo);
            }

            template<class DataT> void Signal_(const DataT& data, StringView rawXml)
            {
                m_upStateMachine->Signal(data, rawXml);
            }

            template<class DataT> void On_(EState state, const DataT& data)
            {
                if (!m_pCallback)
                    return;

                m_pCallback->On(m_id, state, data);
            }

            void On(EState state, const ServiceDescriptionData& data)
            {
                m_optionalPeerServiceDescriptionData = data;
                On_(state, data);
            }

            void On(EState state, const BoardAvailableData& data) override { On_(state, data); }
            void On(EState state, const RevokeBoardAvailableData& data) override { On_(state, data); }
            void On(EState state, const TransportFinishedData& data) override { On_(state, data); }
            void On(EState state, const BoardForecastData& data) override { On_(state, data); }
            void On(EState state, const SendBoardInfoData& data) override { On_(state, data); }
            void On(EState state, const NotificationData& data) override { On_(state, data); }
            void On(EState state, const CommandData& data) override { On_(state, data); }
            void On(EState state, const CheckAliveData& data) override { On_(state, data); }

            void OnState(EState state) override
            {
                if (!m_pCallback)
                    return;
                m_pCallback->OnState(m_id, state);
            }

            void OnDisconnected(EState state, const Error& data) override
            {
                if (!m_pCallback)
                    return;

                auto* pCallback = m_pCallback;
                m_pCallback = nullptr;
                pCallback->OnDisconnected(m_id, state, data);
            }
        };

        Session::Session(unsigned id, IAsioService& service, const UpstreamSettings& configuration)
        {
            m_spImpl = std::make_shared<Impl>(id, service, configuration);

            NetworkConfiguration socketConfig;
            socketConfig.m_hostName = configuration.m_hostAddress;
            socketConfig.m_port = configuration.m_port;
            socketConfig.m_retryDelayInSeconds = configuration.m_reconnectWaitTimeInSeconds;
            socketConfig.m_checkAlivePeriodInSeconds = configuration.m_checkAlivePeriodInSeconds;
            
            m_spImpl->m_upSocket = CreateClientSocket(id, socketConfig, service);
            m_spImpl->m_upSerializer = CreateSerializer(id, service, *m_spImpl->m_upSocket);
            m_spImpl->m_upStateMachine = CreateStateMachine(id, service, *m_spImpl->m_upSerializer, configuration.m_checkState);
        }

        Session::~Session()
        {
            if (!m_spImpl)
                return;

            m_spImpl->m_pCallback = nullptr;
        }

        unsigned Session::Id() const { return m_spImpl->m_id; }
        const Optional<ServiceDescriptionData>& Session::OptionalPeerServiceDescriptionData() const { return m_spImpl->m_optionalPeerServiceDescriptionData; }
        const ConnectionInfo& Session::PeerConnectionInfo() const { return m_spImpl->m_peerConnectionInfo; }

        void Session::Connect(ISessionCallback& callback)
        {
            m_spImpl->m_pCallback = &callback;
            m_spImpl->m_upStateMachine->Connect(m_spImpl, *m_spImpl);
        }

        void Session::Signal(const ServiceDescriptionData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const MachineReadyData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const RevokeMachineReadyData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const StartTransportData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const StopTransportData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const QueryBoardInfoData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const NotificationData&  data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const CommandData&  data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const CheckAliveData&  data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }

        void Session::Disconnect()
        {
            m_spImpl->m_pCallback = nullptr;
            m_spImpl->m_upStateMachine->Disconnect();
        }
    }
}