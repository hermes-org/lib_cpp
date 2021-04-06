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

    namespace VerticalClient
    {
        struct ISessionCallback;

        class Session
        {
        public:
            Session(unsigned id, IAsioService&, const VerticalClientSettings&);
            Session(const Session&) = default;
            Session& operator=(const Session&) = default;
            Session(Session&&) = default;
            Session& operator=(Session&&) = default;
            ~Session();

            explicit operator bool() const { return bool(m_spImpl); }
            unsigned Id() const;
            const Optional<SupervisoryServiceDescriptionData>& OptionalPeerServiceDescriptionData() const;
            const ConnectionInfo& PeerConnectionInfo() const;

            void Connect(ISessionCallback&);
            void Signal(const SupervisoryServiceDescriptionData&, StringView rawXml);
            void Signal(const SendWorkOrderInfoData&, StringView rawXml);
            void Signal(const GetConfigurationData&, StringView rawXml);
            void Signal(const SetConfigurationData&, StringView rawXml);
            void Signal(const NotificationData&, StringView rawXml);
            void Signal(const CheckAliveData&, StringView rawXml);
            void Disconnect();

        private:
            struct Impl;
            std::shared_ptr<Impl> m_spImpl;

        };

        struct ISessionCallback
        {
            virtual void OnSocketConnected(unsigned id, EVerticalState, const ConnectionInfo&) = 0;
            virtual void On(unsigned id, EVerticalState, const SupervisoryServiceDescriptionData&) = 0;
            virtual void On(unsigned id, EVerticalState, const CurrentConfigurationData&) = 0;
            virtual void On(unsigned id, EVerticalState, const BoardArrivedData&) = 0;
            virtual void On(unsigned id, EVerticalState, const BoardDepartedData&) = 0;
            virtual void On(unsigned id, EVerticalState, const QueryWorkOrderInfoData&) = 0;
            virtual void On(unsigned id, EVerticalState, const NotificationData&) = 0;
            virtual void On(unsigned id, EVerticalState, const CheckAliveData&) = 0;
            virtual void OnDisconnected(unsigned id, EVerticalState, const Error&) = 0;
        };


    }
}
