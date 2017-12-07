// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include <HermesData.hpp>
#include <HermesStringView.hpp>

#include "StringBuilder.h"

#include <functional>
#include <memory>

namespace boost
{
    namespace asio { class io_service; }
}


namespace Hermes
{
    struct IAsioService
    {
        virtual void Post(std::function<void()>&&) = 0;
        virtual void Trace(ETraceType, unsigned sessionId, StringView trace) = 0;
        virtual boost::asio::io_service& GetUnderlyingService() = 0;

        template<class... Ts>
        void Log(unsigned sessionId, const Ts&... params)
        {
            Trace(ETraceType::eDEBUG, sessionId, BuildString(params...));
        }

        template<class... Ts>
        void Inform(unsigned sessionId, const Ts&... params)
        {
            Trace(ETraceType::eINFO, sessionId, BuildString(params...));
        }

        template<class... Ts>
        void Warn(unsigned sessionId, const Ts&... params)
        {
            Trace(ETraceType::eWARNING, sessionId, BuildString(params...));
        }

        template<class... Ts>
        Error Alarm(unsigned sessionId, EErrorCode errorCode, const Ts&... params)
        {
            Error error(errorCode, BuildString(params...));
            Trace(ETraceType::eERROR, sessionId, error.m_text);
            return error;
        }


        virtual ~IAsioService() = default;
    };
    using IAsioServiceSp = std::shared_ptr<IAsioService>;
}

struct HermesService
{
    Hermes::IAsioServiceSp m_spService;
};

