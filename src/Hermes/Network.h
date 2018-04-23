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

// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include <HermesStringView.hpp>
#include "StringSpan.h"

#include <memory>
#include <string>

#include <boost/system/error_code.hpp>


namespace Hermes
{
    struct ConnectionInfo;
    struct Error;
    struct IAsioService; using IAsioServiceSp = std::shared_ptr<IAsioService>;
    struct IDataForward;
    struct NetworkConfiguration;

    struct ISocketCallback;
    struct ISocket
    {
        virtual unsigned SessionId() const = 0;
        virtual const ConnectionInfo& GetConnectionInfo() const = 0;
        virtual const NetworkConfiguration& GetConfiguration() const = 0;

        virtual void Connect(std::weak_ptr<void> wpOwner, ISocketCallback&) = 0;
        virtual void Send(StringView message) = 0;
        virtual void Close() = 0;

        virtual ~ISocket() {}
    };

    struct ISocketCallback
    {
        virtual void OnConnected(const ConnectionInfo&) = 0;
        virtual void OnReceived(StringSpan data) = 0;
        virtual void OnDisconnected(const Error&) = 0;

    protected:
        ~ISocketCallback() {}
    };

    struct IClientSocket : ISocket
    {};


    struct IServerSocket : ISocket
    {};


    struct IAcceptorCallback;
    struct IAcceptor
    {
        virtual void StartListening(const NetworkConfiguration&) = 0;
        virtual void StopListening() = 0;

        virtual ~IAcceptor() {}
    };

    struct IAcceptorCallback
    {
        virtual void OnAccepted(std::unique_ptr<IServerSocket>&&) = 0;
    };

    struct NetworkConfiguration
    {
        std::string m_hostName;
        uint16_t m_port = 0U;
        double m_retryDelayInSeconds = 10.0;
        double m_checkAlivePeriodInSeconds = 60.0;

        friend bool operator==(const NetworkConfiguration& lhs, const NetworkConfiguration& rhs)
        {
            return lhs.m_hostName == rhs.m_hostName
                && lhs.m_port == rhs.m_port
                && lhs.m_retryDelayInSeconds == rhs.m_retryDelayInSeconds
                && lhs.m_checkAlivePeriodInSeconds == rhs.m_checkAlivePeriodInSeconds;
        }
        friend bool operator!=(const NetworkConfiguration& lhs, const NetworkConfiguration& rhs)
        {
            return !operator==(lhs, rhs);
        }

        template<class S>
        friend S& operator<<(S& s, const NetworkConfiguration& config)
        {
            s << "{m_hostName=" << config.m_hostName
                << ",m_port=" << config.m_port
                << ",m_retryDelayInSeconds=" << config.m_retryDelayInSeconds
                << ",m_checkAlivePeriodInSeconds=" << config.m_checkAlivePeriodInSeconds
                << '}';
            return s;
        }
    };

    std::unique_ptr<IClientSocket> CreateClientSocket(unsigned sessionId,
        const NetworkConfiguration&, IAsioService& service);

    std::unique_ptr<IAcceptor> CreateAcceptor(IAsioService& service, IAcceptorCallback&);
}

