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
#include "VerticalClientSerializer.h"

#include "Network.h"
#include "IService.h"
#include "MessageDispatcher.h"

namespace Hermes
{
    namespace VerticalClient
    {
        struct Serializer : ISerializer, ISocketCallback
        {
            unsigned m_sessionId;
            IAsioService& m_service;
            IClientSocket& m_socket;
            ISerializerCallback* m_pCallback = nullptr;
            MessageDispatcher m_dispatcher{ m_sessionId, m_service };

            Serializer(unsigned sessionId, IAsioService& service, IClientSocket& socket) :
                m_sessionId(sessionId),
                m_service(service),
                m_socket(socket)
            {
                m_dispatcher.Add<SupervisoryServiceDescriptionData>([this](const auto& data) { m_pCallback->On(data); });
                m_dispatcher.Add<CheckAliveData>([this](const auto& data) { m_pCallback->On(data); });
                m_dispatcher.Add<NotificationData>([this](const auto& data) { m_pCallback->On(data); });
                m_dispatcher.Add<CurrentConfigurationData>([this](const auto& data) { m_pCallback->On(data); });
                m_dispatcher.Add<BoardArrivedData>([this](const auto& data) { m_pCallback->On(data); });
                m_dispatcher.Add<BoardDepartedData>([this](const auto& data) { m_pCallback->On(data); });
                m_dispatcher.Add<QueryWorkOrderInfoData>([this](const auto& data) { m_pCallback->On(data); });
            }

            // ISocketCallback
            void OnConnected(const ConnectionInfo& connectionInfo) override
            {
                m_pCallback->OnSocketConnected(connectionInfo);
            }

            void OnReceived(StringSpan xmlData) override
            {
                auto error = m_dispatcher.Dispatch(xmlData);
                if (!error)
                    return;

                error = m_service.Alarm(m_sessionId, EErrorCode::ePEER_ERROR, error.m_text);
                Signal(Serialize(NotificationData(ENotificationCode::ePROTOCOL_ERROR, ESeverity::eFATAL, error.m_text)));
                m_socket.Close();
                m_pCallback->OnDisconnected(error);
            }

            void OnDisconnected(const Error& error) override
            {
                m_pCallback->OnDisconnected(error);
            }


            //============== VerticalService::ISerializer ================================
            void Connect(std::weak_ptr<void> wpOwner, ISerializerCallback& callback) override
            {
                assert(!m_pCallback);
                m_pCallback = &callback;
                m_socket.Connect(wpOwner, *this);
            }

            void Signal(StringView rawXml)
            {
                m_socket.Send(rawXml);
            }

            void Disconnect() override
            {
                m_socket.Close();
            }
        };
    }

    std::unique_ptr<VerticalClient::ISerializer> VerticalClient::CreateSerializer(unsigned sessionId, IAsioService& service,
        IClientSocket& socket)
    {
        return std::make_unique<VerticalClient::Serializer>(sessionId, service, socket);
    }
}
