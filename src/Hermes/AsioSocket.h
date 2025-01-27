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

#include "Network.h"
#include "IService.h"
#include "Network.h"
#include "MessageSerialization.h"
#include "StringBuilder.h"

#include <HermesData.hpp>

#include <boost/asio.hpp>

#include <memory>

namespace asio = boost::asio;

namespace Hermes
{
    template<class... Ts>
    Error Alarm(IAsioService& service, unsigned sessionId, const boost::system::error_code& ec, const Ts&... trace)
    {
        return service.Alarm(sessionId, EErrorCode::eNETWORK_ERROR, trace..., ": ", ec.message(), '(', ec.value(), ')');
    }

    template<class... Ts>
    void Info(IAsioService& service, unsigned sessionId, const boost::system::error_code& ec, const Ts&... trace)
    {
        service.Inform(sessionId, trace..., ": ", ec.message(), '(', ec.value(), ')');
    }

    struct AsioSocket
    {
        unsigned m_sessionId;
        std::weak_ptr<void> m_wpOwner;
        IAsioService& m_service;
        std::array<char, 1024> m_receivedData;
        asio::io_service& m_asioService{m_service.GetUnderlyingService()};
        asio::ip::tcp::socket m_socket{m_asioService};
        asio::deadline_timer m_timer{m_asioService};
        ISocketCallback* m_pCallback{nullptr};
        NetworkConfiguration m_configuration;
        ConnectionInfo m_connectionInfo;
        bool m_closed{false};

        explicit AsioSocket(unsigned sessionId,
            const NetworkConfiguration& configuration, IAsioService& service) :
            m_sessionId(sessionId),
            m_service(service),
            m_configuration(configuration)
        {}

        AsioSocket(const AsioSocket&) = delete;
        AsioSocket& operator=(const AsioSocket&) = delete;

        ~AsioSocket()
        {
            Close_();
        }

        std::shared_ptr<AsioSocket> shared_from_this() { return std::shared_ptr<AsioSocket>(m_wpOwner.lock(), this); }

        void StartReceiving()
        {
            assert(m_pCallback);
            AsyncReceive_();
        }

        void Send(StringView message)
        {
            if (m_closed)
                return m_service.Log(m_sessionId, "Already closed on Send: ", message);

            boost::system::error_code ec;
            asio::write(m_socket, asio::buffer(message.data(), message.size()), ec);
            if (ec)
                return DisconnectOnError_(ec, "Cannot write ", message);

            m_service.Trace(ETraceType::eSENT, m_sessionId, message);
            RestartCheckAliveTimer_();
        }

        void Close() 
        { 
            Close_(); 
        }

        bool Closed() const 
        { 
            return m_closed; 
        }

        template<class... Ts>
        Error Alarm(const boost::system::error_code& ec, const Ts&... trace)
        {
            return Hermes::Alarm(m_service, m_sessionId, ec, trace...);
        }

        template<class... Ts>
        Error Info(const boost::system::error_code& ec, const Ts&... trace)
        {
            Hermes::Info(m_service, m_sessionId, ec, trace...);
            return Error{};
        }

    private:

        template<class... Ts>
        void DisconnectOnError_(const boost::system::error_code& ec, const Ts&... trace)
        {
            if (m_closed)
                return;
            
            // consider closed connections not to be an error:
            bool isError = (asio::error::eof != ec) && (asio::error::connection_reset != ec);
            auto error = isError ? Alarm(ec, trace...) : Info(ec, trace...);

            auto* pCallback = Close_();
            if (!pCallback)
                return;

            m_service.Inform(m_sessionId, "Disconnected");
            pCallback->OnDisconnected(error);
        }

        ISocketCallback* Close_()
        {
            if (m_closed)
                return nullptr;
            m_closed = true;
            m_service.Log(m_sessionId, "Close socket");

            boost::system::error_code ecDummy;
            m_socket.shutdown(asio::socket_base::shutdown_both, ecDummy);
            m_socket.close(ecDummy);
            m_timer.cancel(ecDummy);

            auto* pCallback = m_pCallback;
            m_pCallback = nullptr;
            return pCallback;
        }

        //================ internally used methods, must all be called from the asio service thread =====================
        void AsyncReceive_()
        {
            if (m_closed)
                return;

            m_service.Log(m_sessionId, "async_receive");
            m_socket.async_receive(asio::buffer(m_receivedData), [spThis = shared_from_this()](const boost::system::error_code& ec, std::size_t size)
            {
                spThis->OnReceive_(ec, size);
            });
        }

        void OnReceive_(const boost::system::error_code& ec, std::size_t size)
        {
            StringSpan data(&m_receivedData.front(), size);

            if (size)
            {
                m_service.Trace(ETraceType::eRECEIVED, m_sessionId, data);
            }

            if (m_closed)
            {
                m_service.Log(m_sessionId, "Received data, but already closed");
                return;
            }

            if (ec)
                return DisconnectOnError_(ec, "OnReceive");

            assert(m_pCallback);
            if (!m_pCallback)
            {
                m_service.Alarm(m_sessionId, Hermes::EErrorCode::eIMPLEMENTATION_ERROR, "Received data, but no callback");
                return;
            }

            m_pCallback->OnReceived(data);
            AsyncReceive_();
        }

        void RestartCheckAliveTimer_()
        {
            if (m_closed)
                return;

            if (!m_configuration.m_checkAlivePeriodInSeconds)
                return;

            m_timer.expires_from_now(boost::posix_time::milliseconds(static_cast<int>(1000.0 * m_configuration.m_checkAlivePeriodInSeconds)));
            m_timer.async_wait([spThis = shared_from_this()](const boost::system::error_code& ec)
            {
                spThis->OnCheckAliveTrigger_(ec);
            });
        }

        void OnCheckAliveTrigger_(const boost::system::error_code& ec)
        {
            if (m_closed)
                return;

            if (ec) // cancelled or whatever
                return;

            Send(Serialize(CheckAliveData()));
        }


    };

}

