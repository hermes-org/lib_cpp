// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once


#include <HermesData.hpp>
#include <memory>

namespace Hermes
{
    struct IAsioService;
    struct IServerSocket;

    namespace Downstream
    {
        struct ISerializerCallback;

        struct ISerializer
        {
            virtual void Connect(std::weak_ptr<void> wpOwner, ISerializerCallback&) = 0;
            virtual void Signal(const ServiceDescription&) = 0;
            virtual void Signal(const BoardAvailableData&) = 0;
            virtual void Signal(const RevokeBoardAvailableData&) = 0;
            virtual void Signal(const TransportFinishedData&) = 0;
            virtual void Signal(const NotificationData&) = 0;
            virtual void Signal(const CheckAliveData&) = 0;
            virtual void Disconnect() = 0;

            virtual ~ISerializer() = default;
        };

        struct ISerializerCallback
        {
            virtual void OnSocketConnected(const ConnectionInfo&) = 0;
            virtual void On(const ServiceDescription&) = 0;
            virtual void On(const MachineReadyData&) = 0;
            virtual void On(const RevokeMachineReadyData&) = 0;
            virtual void On(const StartTransportData&) = 0;
            virtual void On(const StopTransportData&) = 0;
            virtual void On(const NotificationData&) = 0;
            virtual void On(const CheckAliveData&) = 0;
            virtual void OnDisconnected(const Error&) = 0;

        protected:
            ~ISerializerCallback() = default;
        };

        std::unique_ptr<ISerializer> CreateSerializer(unsigned sessionId,
            IAsioService&, IServerSocket&);
    }
}

