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
            virtual void Signal(const ServiceDescription&) = 0;
            virtual void Signal(const BoardAvailableData&) = 0;
            virtual void Signal(const RevokeBoardAvailableData&) = 0;
            virtual void Signal(const TransportFinishedData&) = 0;
            virtual void Signal(const NotificationData&) = 0;
            virtual void Signal(const CheckAliveData&) = 0;
            virtual void Disconnect(const NotificationData&) = 0;

            virtual ~IStateMachine() = default;
        };

        struct IStateMachineCallback
        {
            virtual void OnSocketConnected(EState, const ConnectionInfo&) = 0;
            virtual void On(EState, const ServiceDescription&) = 0;
            virtual void On(EState, const MachineReadyData&) = 0;
            virtual void On(EState, const RevokeMachineReadyData&) = 0;
            virtual void On(EState, const StartTransportData&) = 0;
            virtual void On(EState, const StopTransportData&) = 0;
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

