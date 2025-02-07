/***********************************************************************
Copyright ASM Assembly Systems GmbH & Co. KG

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

#include "stdafx.h"

#include "Runner.h"
#include "Sinks.h"

using namespace Hermes;


BOOST_AUTO_TEST_CASE(UpstreamPostTest)
{
    TestCaseScope scope("UpstreamPostTest");

    std::string called1;
    bool called2 = false;
    std::mutex m_mutex;
    using Lock = std::unique_lock<std::mutex>;
    std::condition_variable cv;

    UpstreamSink upstreamSink;
    Hermes::Upstream upstream(1U, upstreamSink);

    std::string postedValue = "Posted!";
    upstream.Post([&, postedValue]()
    {
        Lock lock(m_mutex);
        called1 = postedValue;
    });

    {
        Runner<Hermes::Upstream> runner(upstream);
        upstream.Post([&]()
        {
            Lock lock(m_mutex);
            called2 = true;
            cv.notify_all();
        });
        Lock lock(m_mutex);
        while (!called2)
            cv.wait(lock);
    }

    BOOST_TEST(called1 == postedValue);
    BOOST_TEST(called2);
}

BOOST_AUTO_TEST_CASE(UpstreamTest)
{
    TestCaseScope scope("UpstreamTest");

    UpstreamSink upstreamSink;
    std::string upstreamMachineId{"UpstreamMachineId"};
    std::string downstreamMachineId{"DownstreamMachineId"};

    Hermes::Upstream upstream(1U, upstreamSink);
    Runner<Hermes::Upstream> upstreamRunner(upstream);
    upstream.Enable(Hermes::UpstreamSettings(downstreamMachineId, "localhost", 50101));

    for (auto i = 0; i < 10; ++i)
    {
        DownstreamSink  downstreamSink;
        Hermes::Downstream downstream(1U, downstreamSink);
        Runner<Hermes::Downstream> downstreamRunner(downstream);
        downstream.Enable(Hermes::DownstreamSettings(downstreamMachineId, 50101));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescriptionData(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescriptionData(upstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
    }

    for (auto i = 0; i < 10; ++i)
    {
        DownstreamSink  downstreamSink;
        Hermes::Downstream downstream(1U, downstreamSink);
        Runner<Hermes::Downstream> downstreamRunner(downstream);
        downstream.Enable(Hermes::DownstreamSettings(downstreamMachineId, 50101));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescriptionData(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescriptionData(upstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

        upstream.Signal(upstreamSink.m_sessionId, MachineReadyData());
        downstream.Signal(downstreamSink.m_sessionId, BoardAvailableData());
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eAVAILABLE_AND_READY; });
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eAVAILABLE_AND_READY; });

        upstream.Signal(upstreamSink.m_sessionId, RevokeMachineReadyData());
        downstream.Signal(downstreamSink.m_sessionId, RevokeBoardAvailableData());
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

        upstream.Signal(upstreamSink.m_sessionId, MachineReadyData());
        downstream.Signal(downstreamSink.m_sessionId, BoardAvailableData());
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eAVAILABLE_AND_READY; });
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eAVAILABLE_AND_READY; });

        upstream.Signal(upstreamSink.m_sessionId, StartTransportData());
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eTRANSPORTING; });

        downstream.Signal(downstreamSink.m_sessionId, TransportFinishedData());
        upstream.Signal(upstreamSink.m_sessionId, StopTransportData());
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

        NotificationData downstreamNotification(ENotificationCode::eUNSPECIFIC, ESeverity::eINFO, std::to_string(i));
        NotificationData upstreamNotification(ENotificationCode::eUNSPECIFIC, ESeverity::eWARNING, std::to_string(i));
        downstream.Signal(downstreamSink.m_sessionId, downstreamNotification);
        upstream.Signal(upstreamSink.m_sessionId, upstreamNotification);
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_notificationData == upstreamNotification; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_notificationData == downstreamNotification; });

        CommandData downstreamCommand(0);
        CommandData upstreamCommand(1);
        downstream.Signal(downstreamSink.m_sessionId, downstreamCommand);
        upstream.Signal(upstreamSink.m_sessionId, upstreamCommand);
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_commandData == upstreamCommand; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_commandData == downstreamCommand; });
    }

    {
        DownstreamSink  downstreamSink;
        Hermes::Downstream downstream(1U, downstreamSink);
        Runner<Hermes::Downstream> downstreamRunner(downstream);
        downstream.Enable(Hermes::DownstreamSettings(downstreamMachineId, 50101));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescriptionData(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescriptionData(upstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

        auto downstreamSessionId = downstreamSink.m_sessionId + 1;
        auto upstreamSessionId = upstreamSink.m_sessionId + 1;
        downstream.Reset(NotificationData(ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION, ESeverity::eINFO, "ResetDownstream"));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescriptionData(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescriptionData(upstreamMachineId, 1U));
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY && upstreamSink.m_sessionId == upstreamSessionId; });
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY && downstreamSink.m_sessionId == downstreamSessionId; });

        downstreamSessionId = downstreamSink.m_sessionId + 1;
        upstreamSessionId = upstreamSink.m_sessionId + 1;
        upstream.Reset(NotificationData(ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION, ESeverity::eINFO, "ResetUpstream"));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescriptionData(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescriptionData(upstreamMachineId, 1U));
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY && upstreamSink.m_sessionId == upstreamSessionId; });
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY && downstreamSink.m_sessionId == downstreamSessionId; });

    }
}
