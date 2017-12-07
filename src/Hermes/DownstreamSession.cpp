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
            Optional<ServiceDescription> m_optionalPeerServiceDescription;
            ConnectionInfo m_peerConnectionInfo;

            ISessionCallback* m_pCallback{nullptr};
            bool m_hasServiceDescription{false};

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

            template<class DataT> void Signal_(const DataT& data)
            {
                m_upStateMachine->Signal(data);
            }

            template<class DataT> void On_(EState state, const DataT& data)
            {
                if (!m_pCallback)
                    return;

                m_pCallback->On(m_id, state, data);
            }

            void On(EState state, const ServiceDescription& data)
            { 
                m_optionalPeerServiceDescription = data;
                On_(state, data);
            }

            void On(EState state, const MachineReadyData& data) override { On_(state, data); }
            void On(EState state, const RevokeMachineReadyData& data) override { On_(state, data); }
            void On(EState state, const StartTransportData& data) override { On_(state, data); }
            void On(EState state, const StopTransportData& data) override { On_(state, data); }
            void On(EState state, const NotificationData& data) override { On_(state, data); }
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
        const Optional<ServiceDescription>& Session::OptionalPeerServiceDescription() const 
        { 
            return m_spImpl->m_optionalPeerServiceDescription; 
        }
        
        const ConnectionInfo& Session::PeerConnectionInfo() const { return m_spImpl->m_peerConnectionInfo; }

        void Session::Connect(ISessionCallback& callback)
        {
            m_spImpl->m_pCallback = &callback;
            m_spImpl->m_upStateMachine->Connect(m_spImpl, *m_spImpl);
        }

        void Session::Signal(const ServiceDescription& data) { m_spImpl->Signal_(data); }
        void Session::Signal(const BoardAvailableData& data) { m_spImpl->Signal_(data); }
        void Session::Signal(const RevokeBoardAvailableData& data) { m_spImpl->Signal_(data); }
        void Session::Signal(const TransportFinishedData& data) { m_spImpl->Signal_(data); }
        void Session::Signal(const NotificationData&  data) { m_spImpl->Signal_(data); }
        void Session::Signal(const CheckAliveData&  data) { m_spImpl->Signal_(data); }

        void Session::Disconnect(const NotificationData& data) 
        {
            m_spImpl->m_pCallback = nullptr;
            m_spImpl->m_upStateMachine->Disconnect(data); 
        }
    }
}