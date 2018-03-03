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
