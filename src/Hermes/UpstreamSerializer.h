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

#pragma once

#include <HermesData.hpp>

#include <memory>

namespace Hermes
{
    struct IAsioService;
    struct IClientSocket;

    namespace Upstream
    {
        struct ISerializerCallback;
        struct ISerializer
        {
            virtual void Connect(std::weak_ptr<void> wpOwner, ISerializerCallback&) = 0;
            virtual void Signal(const ServiceDescription&) = 0;
            virtual void Signal(const MachineReadyData&) = 0;
            virtual void Signal(const RevokeMachineReadyData&) = 0;
            virtual void Signal(const StartTransportData&) = 0;
            virtual void Signal(const StopTransportData&) = 0;
            virtual void Signal(const NotificationData&) = 0;
            virtual void Signal(const CheckAliveData&) = 0;
            virtual void Disconnect() = 0;

            virtual ~ISerializer() = default;
        };

        struct ISerializerCallback
        {
            virtual void OnSocketConnected(const ConnectionInfo&) = 0;
            virtual void On(const ServiceDescription&) = 0;
            virtual void On(const BoardAvailableData&) = 0;
            virtual void On(const RevokeBoardAvailableData&) = 0;
            virtual void On(const TransportFinishedData&) = 0;
            virtual void On(const NotificationData&) = 0;
            virtual void On(const CheckAliveData&) = 0;
            virtual void OnDisconnected(const Error&) = 0;

        protected:
            ~ISerializerCallback() = default;
        };

        std::unique_ptr<ISerializer> CreateSerializer(unsigned sessionId, IAsioService&, IClientSocket&);
    }
}
