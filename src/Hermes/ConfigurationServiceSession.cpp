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

#include "ConfigurationServiceSession.h"

#include "Network.h"
#include "ConfigurationServiceSerializer.h"
#include <HermesData.hpp>
#include "IService.h"

#include "Network.h"


namespace Hermes
{
    struct ConfigurationServiceSession::Impl : IConfigurationServiceSerializerCallback, 
        IGetConfigurationResponse, ISetConfigurationResponse,
        std::enable_shared_from_this<Impl>
    {
        unsigned m_id;
        IAsioService& m_service;

        std::unique_ptr<IServerSocket> m_upSocket;
        std::unique_ptr<IConfigurationServiceSerializer> m_upSerializer;
        ConnectionInfo m_peerConnectionInfo;

        IConfigurationServiceSessionCallback* m_pCallback = nullptr;

        Impl(std::unique_ptr<IServerSocket>&& upSocket, IAsioService& service) :
            m_id(upSocket->SessionId()),
            m_service(service),
            m_upSocket(std::move(upSocket))
        {
            m_service.Log(m_id, "ConfigurationServiceSession()");
        }

        ~Impl()
        {
            m_service.Log(m_id, "~ConfigurationServiceSession()");
        }

        //============= implementation of IConfigurationSessionSerializerCallback ============
        void OnSocketConnected(const ConnectionInfo& connectionInfo) override
        {
            m_peerConnectionInfo = connectionInfo;
            if (!m_pCallback)
                return;
            m_pCallback->OnSocketConnected(m_id, connectionInfo);
        }

        void On(const GetConfigurationData& data) override
        {
            if (!m_pCallback)
                return;

            m_pCallback->OnGet(m_id, m_peerConnectionInfo, data, *this);
        }

        void On(const SetConfigurationData& configuration) override
        {
            if (!m_pCallback)
                return;

            m_pCallback->OnSet(m_id, m_peerConnectionInfo, configuration, *this);
        }
           
        void OnDisconnected(const Error& data) override
        {
            if (!m_pCallback)
                return;

            auto* pCallback = m_pCallback;
            m_pCallback = nullptr;
            pCallback->OnDisconnected(m_id, data);
        }

        //========== IGetConfigurationResponse =============
        void Signal(const CurrentConfigurationData& data) override
        {
            m_upSerializer->Signal(data);
        }

        //========== ISetConfigurationResponse =============
        void Signal(const NotificationData& data) override
        {
            m_upSerializer->Signal(data);
        }
        
        unsigned Id() const override { return m_id; }
    };


    ConfigurationServiceSession::ConfigurationServiceSession(std::unique_ptr<IServerSocket>&& upSocket, IAsioService& service,
        const ConfigurationServiceSettings&)
    {
        auto id = upSocket->SessionId();
        m_spImpl = std::make_shared<Impl>(std::move(upSocket), service);
        m_spImpl->m_upSerializer = CreateConfigurationServiceSerializer(id, service, *m_spImpl->m_upSocket);
    }

    ConfigurationServiceSession::~ConfigurationServiceSession()
    {
        if (!m_spImpl)
            return;

        m_spImpl->m_pCallback = nullptr;
    }

    unsigned ConfigurationServiceSession::Id() const { return m_spImpl->m_id; }
    const ConnectionInfo& ConfigurationServiceSession::PeerConnectionInfo() const { return m_spImpl->m_peerConnectionInfo; }


    void ConfigurationServiceSession::Connect(IConfigurationServiceSessionCallback& callback)
    {
        m_spImpl->m_pCallback = &callback;
        m_spImpl->m_upSerializer->Connect(m_spImpl, *m_spImpl);
    }

    void ConfigurationServiceSession::Disconnect(const NotificationData& data) { m_spImpl->m_upSerializer->Disconnect(data); }

}
