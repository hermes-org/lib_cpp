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
    struct IServerSocket;

    namespace VerticalService
    {
        struct ISessionCallback;

        class Session
        {
        public:
            Session(std::unique_ptr<IServerSocket>&&, IAsioService&, const VerticalServiceSettings&);
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
            void Signal(const SupervisoryServiceDescriptionData&);
            void Signal(const BoardArrivedData&);
            void Signal(const BoardDepartedData&);
            void Signal(const QueryWorkOrderInfoData&);
            void Signal(const ReplyWorkOrderInfoData&);
            void Signal(const CurrentConfigurationData&);
            void Signal(const NotificationData&);
            void Signal(const CheckAliveData&);
            void Signal(const SendHermesCapabilitiesData&);
            void Disconnect();

        private:
            struct Impl;
            std::shared_ptr<Impl> m_spImpl;

        };

        struct ISessionCallback
        {
            virtual void OnSocketConnected(unsigned id, EVerticalState, const ConnectionInfo&) = 0;
            virtual void On(unsigned id, EVerticalState, const SupervisoryServiceDescriptionData&) = 0;
            virtual void On(unsigned id, EVerticalState, const GetConfigurationData&) = 0;
            virtual void On(unsigned id, EVerticalState, const SetConfigurationData&) = 0;
            virtual void On(unsigned id, EVerticalState, const SendWorkOrderInfoData&) = 0;
            virtual void On(unsigned id, EVerticalState, const QueryHermesCapabilitiesData&) = 0;
            virtual void On(unsigned id, EVerticalState, const NotificationData&) = 0;
            virtual void On(unsigned id, EVerticalState, const CheckAliveData&) = 0;
            virtual void OnDisconnected(unsigned id, EVerticalState, const Error&) = 0;
        };


    }
}
