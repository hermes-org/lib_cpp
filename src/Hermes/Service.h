#pragma once

#include <HermesDataConversion.hpp>
#include "ApiCallback.h"
#include "IService.h"

#include <boost/asio.hpp>

#include <memory>

namespace asio = boost::asio;

namespace Hermes
{
    struct Service : IAsioService
    {
        asio::io_service m_asioService;
        asio::io_service::work m_asioWork{m_asioService};
        ApiCallback<HermesTraceCallback> m_traceCallback;

        explicit Service(HermesTraceCallback traceCallback) :
            m_traceCallback(traceCallback)
        {}

        ~Service()
        {}

        void Run()
        {
            boost::system::error_code ec;
            m_asioService.run(ec);
            if (!ec)
                return;
            Trace(ETraceType::eERROR, 0U, BuildString("m_asioService.Run: ", ec.message()));
        }

        void Stop()
        {
            m_asioService.stop();
        }

        //============== IAsioService ==========================
        void Post(std::function<void()>&& f) override
        {
            m_asioService.post(std::move(f));
        }

        void Trace(ETraceType type, unsigned sessionId, StringView trace) override
        {
            m_traceCallback(sessionId, ToC(type), ToC(trace));
        }

        boost::asio::io_service& GetUnderlyingService() override
        {
            return m_asioService;
        }
    };
}
