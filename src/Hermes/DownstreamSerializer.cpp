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
#include "DownstreamSerializer.h"

#include "Network.h"
#include "IService.h"
#include "Serializer.h"
#include "StringBuilder.h"

namespace Hermes
{
    namespace Downstream
    {
        struct Serializer : Downstream::ISerializer, ISocketCallback
        {
            unsigned m_sessionId;
            IAsioService& m_service;
            IServerSocket& m_socket;
            ISerializerCallback* m_pCallback = nullptr;
            std::string m_receiveBuffer;

            Serializer(unsigned sessionId, IAsioService& service, IServerSocket& socket) :
                m_sessionId(sessionId),
  
                m_service(service),
                m_socket(socket)
            {}

            // ISocketCallback
            void OnConnected(const ConnectionInfo& connectionInfo) override
            {
                m_pCallback->OnSocketConnected(connectionInfo);
            }

            void OnReceived(StringView dataView) override
            {
                m_receiveBuffer.append(dataView.data(), dataView.size());
                m_service.Log(m_sessionId, "m_receiveBuffer[", dataView.size(), "]=", m_receiveBuffer);

                std::string messageXml;
                while (TakeMessage(messageXml, m_receiveBuffer))
                {
                    m_service.Log(m_sessionId, "messageXml[", messageXml.size(), "]=", messageXml,
                        "\nm_receiveBuffer[", m_receiveBuffer.size(), "]=", m_receiveBuffer);

                    const auto& variant = Deserialize(messageXml);

                    if (const auto* pData = boost::get<Error>(&variant))
                    {
                        auto error = m_service.Alarm(m_sessionId, EErrorCode::ePEER_ERROR, pData->m_text);
                        Signal(NotificationData(ENotificationCode::ePROTOCOL_ERROR, ESeverity::eFATAL, error.m_text));
                        m_socket.Close();
                        m_pCallback->OnDisconnected(error);
                        return;
                    }

                    if (const auto* pData = boost::get<CheckAliveData>(&variant))
                    {
                        m_service.Log(m_sessionId, "CheckAliveData=", *pData);
                        m_pCallback->On(*pData);
                        continue;
                    }
                    if (const auto* pData = boost::get<ServiceDescription>(&variant))
                    {
                        m_service.Log(m_sessionId, "ServiceDescription=", *pData);
                        m_pCallback->On(*pData);
                        continue;
                    }
                    if (const auto* pData = boost::get<NotificationData>(&variant))
                    {
                        m_service.Log(m_sessionId, "NotificationData=", *pData);
                        m_pCallback->On(*pData);
                        continue;
                    }
                    if (const auto* pData = boost::get<MachineReadyData>(&variant))
                    {
                        m_service.Log(m_sessionId, "MachineReadyData=", *pData);
                        m_pCallback->On(*pData);
                        continue;
                    }
                    if (const auto* pData = boost::get<RevokeMachineReadyData>(&variant))
                    {
                        m_service.Log(m_sessionId, "RevokeMachineReadyData=", *pData);
                        m_pCallback->On(*pData);
                        continue;
                    }
                    if (const auto* pData = boost::get<StartTransportData>(&variant))
                    {
                        m_service.Log(m_sessionId, "StartTransportData=", *pData);
                        m_pCallback->On(*pData);
                        continue;
                    }
                    if (const auto* pData = boost::get<StopTransportData>(&variant))
                    {
                        m_service.Log(m_sessionId, "StopTransportData=", *pData);
                        m_pCallback->On(*pData);
                        continue;
                    }

                    m_service.Warn(m_sessionId, "Unexpected message");
                }

                if (m_receiveBuffer.size() <= cMAX_MESSAGE_SIZE)
                    return;

                auto error = m_service.Alarm(m_sessionId, EErrorCode::ePEER_ERROR, "Maximum message size exceeded");
                Signal(NotificationData(ENotificationCode::ePROTOCOL_ERROR, ESeverity::eFATAL, error.m_text));
                m_socket.Close();
                m_pCallback->OnDisconnected(error);
            }

            void OnDisconnected(const Error& error) override
            {
                m_pCallback->OnDisconnected(error);
            }


            //============== Downstream::ISerializer ================================
            void Connect(std::weak_ptr<void> wpOwner, ISerializerCallback& callback) override
            {
                assert(!m_pCallback);
                m_pCallback = &callback;
                m_socket.Connect(wpOwner, *this);
            }

            void Signal(const ServiceDescription& data) override
            {
                const auto& xmlString = Serialize(data);
                Send_(xmlString);
            }

            void Signal(const BoardAvailableData& data) override
            {
                const auto& xmlString = Serialize(data);
                Send_(xmlString);
            }

            void Signal(const RevokeBoardAvailableData& data) override
            {
                const auto& xmlString = Serialize(data);
                Send_(xmlString);
            }

            void Signal(const TransportFinishedData& data) override
            {
                const auto& xmlString = Serialize(data);
                Send_(xmlString);
            }

            void Signal(const NotificationData& data) override
            {
                const auto& xmlString = Serialize(data);
                Send_(xmlString);
            }

            void Signal(const CheckAliveData& data) override
            {
                const auto& xmlString = Serialize(data);
                Send_(xmlString);
            }

            void Disconnect() override
            {
                m_socket.Close();
            }

            void Send_(StringView message)
            {
                m_socket.Send(message);
            }
        };
    }

    using namespace Downstream;

    std::unique_ptr<ISerializer> Downstream::CreateSerializer(unsigned sessionId, IAsioService& service, 
        IServerSocket& socket)
    {
        return std::make_unique<Serializer>(sessionId, service, socket);
    }
}
