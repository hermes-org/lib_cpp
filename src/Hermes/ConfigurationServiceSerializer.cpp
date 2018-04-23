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
#include "ConfigurationServiceSerializer.h"

#include "Network.h"
#include "IService.h"
#include "MessageDispatcher.h"

namespace Hermes
{
        struct ConfigurationServiceSerializer : IConfigurationServiceSerializer, ISocketCallback
        {
            unsigned m_sessionId;
            IAsioService& m_service;
            IServerSocket& m_socket;
            IConfigurationServiceSerializerCallback* m_pCallback = nullptr;
            MessageDispatcher m_dispatcher{m_sessionId, m_service};
            bool m_connected = false;

            ConfigurationServiceSerializer(unsigned sessionId, IAsioService& service, 
                IServerSocket& socket) :
                m_sessionId(sessionId),
                m_service(service),
                m_socket(socket)
            {
                m_dispatcher.Add<GetConfigurationData>([this](const auto& data) { m_pCallback->On(data); });
                m_dispatcher.Add<SetConfigurationData>([this](const auto& data) { m_pCallback->On(data); });
            }

            // ISocketCallback
            void OnConnected(const ConnectionInfo& connectionInfo) override
            {
                m_connected = true;
                m_pCallback->OnSocketConnected(connectionInfo);
            }

            void OnReceived(StringSpan xmlData) override
            {
                auto error = m_dispatcher.Dispatch(xmlData);
                if (!error)
                    return;

                error = m_service.Alarm(m_sessionId, EErrorCode::ePEER_ERROR, error.m_text);
                Signal(NotificationData(ENotificationCode::ePROTOCOL_ERROR, ESeverity::eFATAL, error.m_text));
                m_socket.Close();
                m_pCallback->OnDisconnected(error);
            }

            void OnDisconnected(const Error& error) override
            {
                m_pCallback->OnDisconnected(error);
            }

            //============== IConfigurationServiceSerializer ================================
            void Connect(std::weak_ptr<void> wpOwner, IConfigurationServiceSerializerCallback& callback) override
            {
                assert(!m_pCallback);
                m_pCallback = &callback;
                m_socket.Connect(std::move(wpOwner), *this);
            }

            void Signal(const CurrentConfigurationData& data) override
            {
                if (!m_connected)
                    return;

                const auto& xmlString = Serialize(data);
                m_socket.Send(xmlString);
            }

            void Signal(const NotificationData& data) override
            {
                if (!m_connected)
                    return;

                const auto& xmlString = Serialize(data);
                m_socket.Send(xmlString);
            }

            void Disconnect(const NotificationData& data) override
            {
                Signal(data);
                m_socket.Close();
            }

        };

    std::unique_ptr<IConfigurationServiceSerializer> CreateConfigurationServiceSerializer(unsigned sessionId,
        IAsioService& service, IServerSocket& socket)
    {
        return std::make_unique<ConfigurationServiceSerializer>(sessionId, service, socket);
    }
}
