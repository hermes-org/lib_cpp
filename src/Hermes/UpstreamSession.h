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
