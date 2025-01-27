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
            const Optional<ServiceDescriptionData>& OptionalPeerServiceDescriptionData() const;
            const ConnectionInfo& PeerConnectionInfo() const;

            void Connect(ISessionCallback&);
            void Signal(const ServiceDescriptionData&, StringView rawXml);
            void Signal(const MachineReadyData&, StringView rawXml);
            void Signal(const RevokeMachineReadyData&, StringView rawXml);
            void Signal(const StartTransportData&, StringView rawXml);
            void Signal(const StopTransportData&, StringView rawXml);
            void Signal(const QueryBoardInfoData&, StringView rawXml);
            void Signal(const NotificationData&, StringView rawXml);
            void Signal(const CommandData&, StringView rawXml);
            void Signal(const CheckAliveData&, StringView rawXml);
            void Disconnect();

        private:
            struct Impl;
            std::shared_ptr<Impl> m_spImpl;

        };

        struct ISessionCallback
        {
            virtual void OnSocketConnected(unsigned id, EState, const ConnectionInfo&) = 0;
            virtual void On(unsigned id, EState, const ServiceDescriptionData&) = 0;
            virtual void On(unsigned id, EState, const BoardAvailableData&) = 0;
            virtual void On(unsigned id, EState, const RevokeBoardAvailableData&) = 0;
            virtual void On(unsigned id, EState, const TransportFinishedData&) = 0;
            virtual void On(unsigned id, EState, const BoardForecastData&) = 0;
            virtual void On(unsigned id, EState, const SendBoardInfoData&) = 0;
            virtual void On(unsigned id, EState, const NotificationData&) = 0;
            virtual void On(unsigned id, EState, const CommandData&) = 0;
            virtual void On(unsigned id, EState, const CheckAliveData&) = 0;
            virtual void OnState(unsigned id, EState) = 0;
            virtual void OnDisconnected(unsigned id, EState, const Error&) = 0;
        };


    }
}
