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

#include "DownstreamSession.h"

#include "Network.h"
#include "DownstreamSerializer.h"
#include "DownstreamStateMachine.h"
#include <HermesData.hpp>
#include "IService.h"
#include "StringBuilder.h"

#include "Network.h"

namespace Hermes
{
    namespace asio = boost::asio;

    namespace Downstream
    {
        struct Session::Impl : IStateMachineCallback, std::enable_shared_from_this<Impl>
        {
            unsigned m_id;
            IAsioService& m_service;
            DownstreamSettings m_configuration;

            std::unique_ptr<IServerSocket> m_upSocket;
            std::unique_ptr<ISerializer> m_upSerializer;
            std::unique_ptr<IStateMachine> m_upStateMachine;
            Optional<ServiceDescriptionData> m_optionalPeerServiceDescriptionData;
            ConnectionInfo m_peerConnectionInfo;

            ISessionCallback* m_pCallback{nullptr};
            bool m_hasServiceDescriptionData{false};

            Impl(std::unique_ptr<IServerSocket>&& upSocket, IAsioService& service, const DownstreamSettings& configuration) :
                m_id(upSocket->SessionId()),
                m_service(service),
                m_configuration(configuration),
                m_upSocket(std::move(upSocket))
            {
                m_service.Log(m_id, "DownstreamSession()");
            }

            ~Impl()
            {
                m_service.Log(m_id, "~DownstreamSession()");
            }

            //============= implementation of IStateMachineCallback ============
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

            void On(EState state, const MachineReadyData& data) override { On_(state, data); }
            void On(EState state, const RevokeMachineReadyData& data) override { On_(state, data); }
            void On(EState state, const StartTransportData& data) override { On_(state, data); }
            void On(EState state, const StopTransportData& data) override { On_(state, data); }
            void On(EState state, const QueryBoardInfoData& data) override { On_(state, data); }
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


        Session::Session(std::unique_ptr<IServerSocket>&& upSocket, IAsioService& service,
            const DownstreamSettings& configuration)
        {
            auto sessionId = upSocket->SessionId();
            m_spImpl = std::make_shared<Impl>(std::move(upSocket), service, configuration);
            m_spImpl->m_upSerializer = CreateSerializer(sessionId, service, *m_spImpl->m_upSocket);
            m_spImpl->m_upStateMachine = CreateStateMachine(sessionId, service, *m_spImpl->m_upSerializer, configuration.m_checkState);
        }

        Session::~Session()
        {
            if (!m_spImpl)
                return;

            m_spImpl->m_pCallback = nullptr;
        }

        unsigned Session::Id() const { return m_spImpl->m_id; }
        const Optional<ServiceDescriptionData>& Session::OptionalPeerServiceDescriptionData() const 
        { 
            return m_spImpl->m_optionalPeerServiceDescriptionData; 
        }
        
        const ConnectionInfo& Session::PeerConnectionInfo() const { return m_spImpl->m_peerConnectionInfo; }

        void Session::Connect(ISessionCallback& callback)
        {
            m_spImpl->m_pCallback = &callback;
            m_spImpl->m_upStateMachine->Connect(m_spImpl, *m_spImpl);
        }

        void Session::Signal(const ServiceDescriptionData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const BoardAvailableData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const RevokeBoardAvailableData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const TransportFinishedData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const BoardForecastData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
        void Session::Signal(const SendBoardInfoData& data, StringView rawXml) { m_spImpl->Signal_(data, rawXml); }
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