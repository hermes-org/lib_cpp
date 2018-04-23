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
#include "stdafx.h"

#include <HermesDataConversion.hpp>
#include "ApiCallback.h"
#include "MessageDispatcher.h"
#include "Service.h"
#include "StringBuilder.h"

#include <boost/asio.hpp>

#include <atomic>

namespace asio = boost::asio;

std::atomic<uint32_t> m_configurationTraceId{0};

using namespace Hermes;

namespace
{
    using TraceCallback = ApiCallback<HermesTraceCallback>;
    using ErrorCallback = ApiCallback<HermesErrorCallback>;
    using CurrentConfigurationCallback = ApiCallback<HermesCurrentConfigurationCallback>;
    using NotificationCallback = ApiCallback<HermesNotificationCallback>;

    // helper for both set and get:
    struct ConfigurationHelper
    {
        std::string m_traceName;
        std::string m_hostName;
        Service m_service;
        asio::io_service& m_asioService{m_service.GetUnderlyingService()};
        asio::ip::tcp::socket m_socket{m_asioService};
        MessageDispatcher m_dispatcher{0U, m_service};

        unsigned m_timeoutInSeconds = 10U;

        CurrentConfigurationCallback m_configurationCallback;
        NotificationCallback m_notificationCallback;
        ErrorCallback m_errorCallback;

        static const std::size_t cCHUNK_SIZE = 4096;
        std::array<char, cCHUNK_SIZE> m_receivedData;

        bool m_receiving = false;

        ConfigurationHelper(StringView traceName, StringView  hostName,
            unsigned timeoutInSeconds,
            const CurrentConfigurationCallback& configurationCallback,
            const NotificationCallback& notificationCallback,
            const ErrorCallback& errorCallback,
            const HermesTraceCallback& traceCallback) :
            m_traceName(traceName),
            m_hostName(hostName),
            m_service(traceCallback),
            m_timeoutInSeconds(timeoutInSeconds),
            m_configurationCallback(configurationCallback),
            m_notificationCallback(notificationCallback),
            m_errorCallback(errorCallback)
        {
            m_dispatcher.Add<CurrentConfigurationData>([this](const auto& data) -> Error
            {
                m_receiving = false;
                auto apiData = ToC(data);
                m_configurationCallback(0U, &apiData);
                return{};

            });
            m_dispatcher.Add<NotificationData>([this](const auto& data) -> Error
            {
                auto apiData = ToC(data);
                m_notificationCallback(0U, &apiData);
                return{};
            });
        }

        template<class... Ts>
        void GenerateError(EErrorCode errorCode, const Ts&... params)
        {
            auto error = m_service.Alarm(0U, errorCode, params...);
            auto apiError = ToC(error);
            m_errorCallback(&apiError);
        }

        bool Connect()
        {
            asio::ip::tcp::resolver resolver(m_asioService);
            asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), m_hostName, "");

            boost::system::error_code ec;
            auto itEndpoint = resolver.resolve(query, ec);
            if (ec)
            {
                GenerateError(EErrorCode::eNETWORK_ERROR, "asio::resolve: ", ec.message());
                return false;
            }

            asio::ip::tcp::endpoint endpoint(*itEndpoint);
            endpoint.port(cCONFIG_PORT);
            boost::system::error_code ecConnect = asio::error::would_block; // can never be returned from an async function
            asio::deadline_timer receiveTimer{m_asioService};
            receiveTimer.expires_from_now(boost::posix_time::seconds(m_timeoutInSeconds));
            receiveTimer.async_wait([](const boost::system::error_code&) {}); // leaves ecAccept as is
            m_socket.async_connect(endpoint, [&](boost::system::error_code in_ec) 
            { 
                ecConnect = in_ec; 
            });
            m_asioService.run_one(ec);
            if (ec)
            {
                GenerateError(EErrorCode::eIMPLEMENTATION_ERROR, "asio::run_one: ", ec.message());
                return false;
            }

            if (ecConnect == asio::error::would_block)
            {
                GenerateError(EErrorCode::eTIMEOUT, "asio::accept: timeout");
                return false;
            }

            if (ecConnect)
            {
                GenerateError(EErrorCode::eNETWORK_ERROR, "asio::async_connect: ", ec.message());
                return false;
            }

            return true;
        }

        void GetConfiguration()
        {
            // write the get-message
            GetConfigurationData data;
            const auto& msgString = Serialize(data);
            boost::system::error_code ec;
            asio::write(m_socket, asio::buffer(msgString.data(), msgString.size()), ec);
            if (ec)
            {
                GenerateError(EErrorCode::eNETWORK_ERROR, "asio::write: ", ec.message());
                return;
            }
            m_service.Trace(ETraceType::eSENT, 0U, msgString);

            m_receiving = true;

            // parallel to receiving, run the timer:
            asio::deadline_timer receiveTimer{m_asioService};
            receiveTimer.expires_from_now(boost::posix_time::seconds(m_timeoutInSeconds));
            receiveTimer.async_wait([&](const boost::system::error_code&)
            {
                if (!m_receiving)
                    return;

                m_receiving = false;
                GenerateError(EErrorCode::eTIMEOUT, "asio::receive", ec.message());
            });

            while (m_receiving)
            {
                m_socket.async_receive(asio::buffer(m_receivedData), [this](const boost::system::error_code ecReceive, std::size_t size)
                {
                    OnAsyncReceive_(ecReceive, size);
                });
                m_asioService.reset();
                boost::system::error_code ecRun;
                m_asioService.run_one(ecRun);
                if (ecRun)
                    return GenerateError(EErrorCode::eNETWORK_ERROR, "asio::run_one", ecRun.message());
            }
        }

        void OnAsyncReceive_(const boost::system::error_code& ecReceive, std::size_t size)
        {
            if (ecReceive)
            {
                m_receiving = false;
                return GenerateError(EErrorCode::eNETWORK_ERROR, "asio::async_receive", ecReceive.message());
            }

            m_service.Trace(ETraceType::eRECEIVED, 0U, StringView{&m_receivedData[0], size});

            if (auto error = m_dispatcher.Dispatch(StringSpan{&m_receivedData[0], size}))
            {
                m_receiving = false;
                GenerateError(EErrorCode::ePEER_ERROR, m_traceName, "Maximum message size exceeded");
            }
        }
    };
}

void SetHermesConfiguration(HermesStringView hostName, const HermesSetConfigurationData* pConfiguration,
    unsigned timeoutInSeconds, const HermesSetConfigurationCallbacks* pCallbacks)
{
    ConfigurationHelper helper("SetHermesConfiguration", ToCpp(hostName), timeoutInSeconds,
        pCallbacks->m_currentConfigurationCallback, pCallbacks->m_notificationCallback,
        pCallbacks->m_errorCallback, pCallbacks->m_traceCallback);

    if (!helper.Connect())
        return;

    // send the set message:
    SetConfigurationData data(ToCpp(*pConfiguration));
    const auto& xmlString = Serialize(data);
    boost::system::error_code ec;
    asio::write(helper.m_socket, asio::buffer(xmlString.data(), xmlString.size()), ec);
    if (ec)
        return helper.GenerateError(EErrorCode::eNETWORK_ERROR, "asio::write", ec.message());
    helper.m_service.Log(eHERMES_TRACE_TYPE_SENT, 0U, xmlString);
    helper.GetConfiguration();
}

void GetHermesConfiguration(HermesStringView  hostName,
    unsigned timeoutInSeconds, const HermesGetConfigurationCallbacks* pCallbacks)
{
    ConfigurationHelper helper("GetHermesConfiguration", ToCpp(hostName), timeoutInSeconds,
        pCallbacks->m_currentConfigurationCallback, NotificationCallback(),
        pCallbacks->m_errorCallback, pCallbacks->m_traceCallback);

    if (!helper.Connect())
        return;

    helper.GetConfiguration();
}

