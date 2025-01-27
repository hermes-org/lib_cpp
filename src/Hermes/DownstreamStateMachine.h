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

    namespace Downstream
    {
        struct IStateMachineCallback;
        struct ISerializer;

        struct IStateMachine
        {
            virtual void Connect(std::weak_ptr<void> wpOwner, IStateMachineCallback&) = 0;
            virtual void Signal(const ServiceDescriptionData&, StringView rawXml) = 0;
            virtual void Signal(const BoardAvailableData&, StringView rawXml) = 0;
            virtual void Signal(const RevokeBoardAvailableData&, StringView rawXml) = 0;
            virtual void Signal(const TransportFinishedData&, StringView rawXml) = 0;
            virtual void Signal(const BoardForecastData&, StringView rawXml) = 0;
            virtual void Signal(const SendBoardInfoData&, StringView rawXml) = 0;
            virtual void Signal(const NotificationData&, StringView rawXml) = 0;
            virtual void Signal(const CommandData&, StringView rawXml) = 0;
            virtual void Signal(const CheckAliveData&, StringView rawXml) = 0;
            virtual void Disconnect() = 0;

            virtual ~IStateMachine() = default;
        };

        struct IStateMachineCallback
        {
            virtual void OnSocketConnected(EState, const ConnectionInfo&) = 0;
            virtual void On(EState, const ServiceDescriptionData&) = 0;
            virtual void On(EState, const MachineReadyData&) = 0;
            virtual void On(EState, const RevokeMachineReadyData&) = 0;
            virtual void On(EState, const StartTransportData&) = 0;
            virtual void On(EState, const StopTransportData&) = 0;
            virtual void On(EState, const QueryBoardInfoData&) = 0;
            virtual void On(EState, const NotificationData&) = 0;
            virtual void On(EState, const CommandData&) = 0;
            virtual void On(EState, const CheckAliveData&) = 0;
            virtual void OnState(EState) = 0;
            virtual void OnDisconnected(EState, const Error&) = 0;

        protected:
            ~IStateMachineCallback() = default;
        };

        std::unique_ptr<IStateMachine> CreateStateMachine(unsigned sessionId, IAsioService&, ISerializer&, ECheckState);
    }

}

