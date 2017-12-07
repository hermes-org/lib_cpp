// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include "Runner.h"
#include "Sinks.h"

using namespace Hermes;

BOOST_AUTO_TEST_CASE(DownstreamPostTest)
{
    TestCaseScope scope("DownstreamPostTest");

    std::string called1;
    bool called2 = false;
    std::mutex m_mutex;
    using Lock = std::unique_lock<std::mutex>;
    std::condition_variable cv;

    DownstreamSink downstreamSink;
    Hermes::Downstream downstream(1U, downstreamSink);

    std::string postedValue = "Posted!";
    downstream.Post([&, postedValue]()
    {
        Lock lock(m_mutex);
        called1 = postedValue;
    });

    {
        Runner<Hermes::Downstream> runner(downstream);
        downstream.Post([&]()
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

BOOST_AUTO_TEST_CASE(DownstreamTest)
{
    TestCaseScope scope("DownstreamTest");

    DownstreamSink downstreamSink;
    std::string upstreamMachineId{"UpstreamMachineId"};
    std::string downstreamMachineId{"DownstreamMachineId"};

    Hermes::Downstream downstream(1U, downstreamSink);
    Runner<Hermes::Downstream> downstreamRunner(downstream);
    downstream.Enable(Hermes::DownstreamSettings(upstreamMachineId, 50101));

    for (auto i = 0; i < 10; ++i)
    {
        UpstreamSink  upstreamSink;
        Hermes::Upstream upstream(1U, upstreamSink);
        Runner<Hermes::Upstream> upstreamRunner(upstream);
        upstream.Enable(Hermes::UpstreamSettings(downstreamMachineId, "127.0.0.1", 50101));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescription(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescription(upstreamMachineId, 1U));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
    }

    for (auto i = 0; i < 10; ++i)
    {
        UpstreamSink  upstreamSink;
        Hermes::Upstream upstream(1U, upstreamSink);
        Runner<Hermes::Upstream> upstreamRunner(upstream);
        upstream.Enable(Hermes::UpstreamSettings(upstreamMachineId, "127.0.0.1", 50101));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescription(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescription(upstreamMachineId, 1U));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

        upstream.Signal(upstreamSink.m_sessionId, MachineReadyData());
        downstream.Signal(downstreamSink.m_sessionId, BoardAvailableData());
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eAVAILABLE_AND_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eAVAILABLE_AND_READY; });

        upstream.Signal(upstreamSink.m_sessionId, RevokeMachineReadyData());
        downstream.Signal(downstreamSink.m_sessionId, RevokeBoardAvailableData());
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

        upstream.Signal(upstreamSink.m_sessionId, MachineReadyData());
        downstream.Signal(downstreamSink.m_sessionId, BoardAvailableData());
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eAVAILABLE_AND_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eAVAILABLE_AND_READY; });

        upstream.Signal(upstreamSink.m_sessionId, StartTransportData());
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eTRANSPORTING; });

        downstream.Signal(downstreamSink.m_sessionId, TransportFinishedData());
        upstream.Signal(upstreamSink.m_sessionId, StopTransportData());
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

        NotificationData downstreamNotification(ENotificationCode::eUNSPECIFIC, ESeverity::eINFO, std::to_string(i));
        NotificationData upstreamNotification(ENotificationCode::eUNSPECIFIC, ESeverity::eWARNING, std::to_string(i));
        downstream.Signal(downstreamSink.m_sessionId, downstreamNotification);
        upstream.Signal(upstreamSink.m_sessionId, upstreamNotification);
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_notificationData == upstreamNotification; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_notificationData == downstreamNotification; });
    }

    {
        UpstreamSink  upstreamSink;
        Hermes::Upstream upstream(1U, upstreamSink);
        Runner<Hermes::Upstream> upstreamRunner(upstream);
        upstream.Enable(Hermes::UpstreamSettings(upstreamMachineId, "127.0.0.1", 50101));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescription(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescription(upstreamMachineId, 1U));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

        auto upstreamSessionId = upstreamSink.m_sessionId + 1;
        auto downstreamSessionId = downstreamSink.m_sessionId + 1;
        upstream.Reset(NotificationData(ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION, ESeverity::eINFO, "ResetUpstream"));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescription(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescription(upstreamMachineId, 1U));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY && downstreamSink.m_sessionId == downstreamSessionId; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY && upstreamSink.m_sessionId == upstreamSessionId; });

        upstreamSessionId = upstreamSink.m_sessionId + 1;
        downstreamSessionId = downstreamSink.m_sessionId + 1;
        downstream.Reset(NotificationData(ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION, ESeverity::eINFO, "ResetDownstream"));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescription(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescription(upstreamMachineId, 1U));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY && downstreamSink.m_sessionId == downstreamSessionId; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY && upstreamSink.m_sessionId == upstreamSessionId; });

    }

}

BOOST_AUTO_TEST_CASE(DownstreamAllowedHostTest)
{
    TestCaseScope scope("DownstreamAllowedHostTest");

    DownstreamSink downstreamSink;
    std::string upstreamMachineId{"UpstreamMachineId"};
    std::string downstreamMachineId{"DownstreamMachineId"};

    Hermes::Downstream downstream(1U, downstreamSink);
    Runner<Hermes::Downstream> downstreamRunner(downstream);
    Hermes::DownstreamSettings downstreamConfig(upstreamMachineId, 50101);
    downstreamConfig.m_optionalClientAddress = "127.0.0.1"; // should work

    downstream.Enable(downstreamConfig);

    for (auto i = 0; i < 10; ++i)
    {
        UpstreamSink  upstreamSink;
        Hermes::Upstream upstream(1U, upstreamSink);
        Runner<Hermes::Upstream> upstreamRunner(upstream);
        upstream.Enable(Hermes::UpstreamSettings(upstreamMachineId, "127.0.0.1", 50101));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescription(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescription(upstreamMachineId, 1U));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
    }

}

BOOST_AUTO_TEST_CASE(DownstreamHostNotAlloweHostTest)
{
    TestCaseScope scope("DownstreamHostNotAlloweHostTest");

    DownstreamSink downstreamSink;
    std::string upstreamMachineId{"UpstreamMachineId"};
    std::string downstreamMachineId{"DownstreamMachineId"};

    Hermes::Downstream downstream(1U, downstreamSink);
    Runner<Hermes::Downstream> downstreamRunner(downstream);
    Hermes::DownstreamSettings downstreamConfig(upstreamMachineId, 50101);
    downstreamConfig.m_optionalClientAddress = "127.0.0.2"; // should not accept from localhost

    downstream.Enable(downstreamConfig);

    {
        UpstreamSink  upstreamSink;
        Hermes::Upstream upstream(1U, upstreamSink);
        Runner<Hermes::Upstream> upstreamRunner(upstream);
        upstream.Enable(Hermes::UpstreamSettings(upstreamMachineId, "127.0.0.1", 50101));

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eDISCONNECTED; });

        downstreamConfig.m_optionalClientAddress.clear(); // should now accept anything
        downstream.Enable(downstreamConfig);

        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
        upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescription(downstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
        downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescription(upstreamMachineId, 1U));
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
    }



}
