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


