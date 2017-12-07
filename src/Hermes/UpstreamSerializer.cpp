// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"
#include "UpstreamSerializer.h"

#include "Network.h"
#include <HermesData.hpp>
#include "IService.h"
#include "Serializer.h"

namespace Hermes
{
    namespace Upstream
    {
        struct Serializer : Upstream::ISerializer, ISocketCallback
        {
            unsigned m_sessionId;
            IAsioService& m_service;
            IClientSocket& m_socket;
            ISerializerCallback* m_pCallback = nullptr;
            std::string m_receiveBuffer;

            Serializer(unsigned sessionId, IAsioService& service, IClientSocket& socket) :
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
                    if (const auto* pData = boost::get<BoardAvailableData>(&variant))
                    {
                        m_service.Log(m_sessionId, "BoardAvailableData=", *pData);
                        m_pCallback->On(*pData);
                        continue;
                    }
                    if (const auto* pData = boost::get<RevokeBoardAvailableData>(&variant))
                    {
                        m_service.Log(m_sessionId, "RevokeBoardAvailableData=", *pData);
                        m_pCallback->On(*pData);
                        continue;
                    }
                    if (const auto* pData = boost::get<TransportFinishedData>(&variant))
                    {
                        m_service.Log(m_sessionId, "TransportFinishedData=", *pData);
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

            //============== IUpstreamSerialization ================================
            void Connect(std::weak_ptr<void> wpOwner, ISerializerCallback& callback) override
            {
                assert(!m_pCallback);
                m_pCallback = &callback;
                m_socket.Connect(std::move(wpOwner), *this);
            }

            void Signal(const ServiceDescription& data) override
            {
                const auto& xmlString = Serialize(data);
                Send_(xmlString);
            }

            void Signal(const MachineReadyData& data) override
            {
                const auto& xmlString = Serialize(data);
                Send_(xmlString);
            }

            void Signal(const RevokeMachineReadyData& data) override
            {
                const auto& xmlString = Serialize(data);
                Send_(xmlString);
            }

            void Signal(const StartTransportData& data) override
            {
                const auto& xmlString = Serialize(data);
                Send_(xmlString);
            }

            void Signal(const StopTransportData& data) override
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

    using namespace Upstream;

    std::unique_ptr<ISerializer> Upstream::CreateSerializer(unsigned sessionId,
        IAsioService& service, IClientSocket& socket)
    {
        return std::make_unique<Serializer>(sessionId, service, socket);
    }
}
