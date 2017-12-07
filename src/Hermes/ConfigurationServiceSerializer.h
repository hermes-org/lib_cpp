// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once


#include <HermesData.hpp>
#include <memory>

namespace Hermes
{
    struct IAsioService;
    struct IServerSocket;

    struct IConfigurationServiceSerializerCallback;

    struct IConfigurationServiceSerializer
    {
        virtual void Connect(std::weak_ptr<void> wpOwner, IConfigurationServiceSerializerCallback&) = 0;
        virtual void Signal(const CurrentConfigurationData&) = 0;
        virtual void Signal(const NotificationData&) = 0;
        virtual void Disconnect(const NotificationData&) = 0;

        virtual ~IConfigurationServiceSerializer() = default;
    };

    struct IConfigurationServiceSerializerCallback
    {
        virtual void OnSocketConnected(const ConnectionInfo&) = 0;
        virtual void On(const GetConfigurationData&) = 0;
        virtual void On(const SetConfigurationData&) = 0;
        virtual void OnDisconnected(const Error&) = 0;

    protected:
        ~IConfigurationServiceSerializerCallback() = default;
    };

    std::unique_ptr<IConfigurationServiceSerializer> CreateConfigurationServiceSerializer(unsigned sessionId,
        IAsioService&, IServerSocket&);
}


