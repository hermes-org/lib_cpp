// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include <HermesData.hpp>

#include <memory>

namespace Hermes
{
    struct IAsioService;

    namespace Upstream
    {
        struct ISerializer;
        struct IStateMachineCallback;
        struct IStateMachine
        {
            virtual void Connect(std::weak_ptr<void> wpOwner, IStateMachineCallback&) = 0;
            virtual void Signal(const ServiceDescription&) = 0;
            virtual void Signal(const MachineReadyData&) = 0;
            virtual void Signal(const RevokeMachineReadyData&) = 0;
            virtual void Signal(const StartTransportData&) = 0;
            virtual void Signal(const StopTransportData&) = 0;
            virtual void Signal(const NotificationData&) = 0;
            virtual void Signal(const CheckAliveData&) = 0;
            virtual void Disconnect(const NotificationData&) = 0;

            virtual ~IStateMachine() = default;
        };

        struct IStateMachineCallback
        {
            virtual void OnSocketConnected(EState, const ConnectionInfo&) = 0;
            virtual void On(EState, const ServiceDescription&) = 0;
            virtual void On(EState, const BoardAvailableData&) = 0;
            virtual void On(EState, const RevokeBoardAvailableData&) = 0;
            virtual void On(EState, const TransportFinishedData&) = 0;
            virtual void On(EState, const NotificationData&) = 0;
            virtual void On(EState, const CheckAliveData&) = 0;
            virtual void OnState(EState) = 0;
            virtual void OnDisconnected(EState, const Error&) = 0;

        protected:
            ~IStateMachineCallback() = default;
        };

        std::unique_ptr<IStateMachine> CreateStateMachine(unsigned sessionId, IAsioService&, ISerializer&, ECheckState);
    }

}

