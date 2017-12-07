#pragma once

#include <HermesData.hpp>

#include <memory>

namespace Hermes
{
    struct IAsioService;
    struct IServerSocket;

    namespace Downstream
    {
        struct ISessionCallback;

        class Session
        {
        public:
            Session(std::unique_ptr<IServerSocket>&&, IAsioService&, const DownstreamSettings&);
            Session(const Session&) = default;
            Session& operator=(const Session&) = default;
            Session(Session&&) = default;
            Session& operator=(Session&&) = default;
            ~Session();

            explicit operator bool() const { return bool(m_spImpl); }
            unsigned Id() const;
            const Optional<ServiceDescription>& OptionalPeerServiceDescription() const;
            const ConnectionInfo& PeerConnectionInfo() const;

            void Connect(ISessionCallback&);
            void Signal(const ServiceDescription&);
            void Signal(const BoardAvailableData&);
            void Signal(const RevokeBoardAvailableData&);
            void Signal(const TransportFinishedData&);
            void Signal(const NotificationData&);
            void Signal(const CheckAliveData&);
            void Disconnect(const NotificationData&);

        private:
            struct Impl;
            std::shared_ptr<Impl> m_spImpl;

        };

        struct ISessionCallback
        {
            virtual void OnSocketConnected(unsigned id, EState, const ConnectionInfo&) = 0;
            virtual void On(unsigned id, EState, const ServiceDescription&) = 0;
            virtual void On(unsigned id, EState, const MachineReadyData&) = 0;
            virtual void On(unsigned id, EState, const RevokeMachineReadyData&) = 0;
            virtual void On(unsigned id, EState, const StartTransportData&) = 0;
            virtual void On(unsigned id, EState, const StopTransportData&) = 0;
            virtual void On(unsigned id, EState, const NotificationData&) = 0;
            virtual void On(unsigned id, EState, const CheckAliveData&) = 0;
            virtual void OnState(unsigned id, EState) = 0;
            virtual void OnDisconnected(unsigned id, EState, const Error&) = 0;
        };


    }
}