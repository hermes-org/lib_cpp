// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include <HermesData.hpp>

#include <memory>

namespace Hermes
{
    struct IAsioService;

    namespace Upstream
    {
        struct ISessionCallback;
        class Session
        {
        public:
            Session(unsigned id, IAsioService&, const UpstreamSettings&);
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
            void Signal(const MachineReadyData&);
            void Signal(const RevokeMachineReadyData&);
            void Signal(const StartTransportData&);
            void Signal(const StopTransportData&);
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
            virtual void On(unsigned id, EState, const BoardAvailableData&) = 0;
            virtual void On(unsigned id, EState, const RevokeBoardAvailableData&) = 0;
            virtual void On(unsigned id, EState, const TransportFinishedData&) = 0;
            virtual void On(unsigned id, EState, const NotificationData&) = 0;
            virtual void On(unsigned id, EState, const CheckAliveData&) = 0;
            virtual void OnState(unsigned id, EState) = 0;
            virtual void OnDisconnected(unsigned id, EState, const Error&) = 0;
        };


    }
}
