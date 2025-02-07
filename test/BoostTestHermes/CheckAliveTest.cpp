// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include "Runner.h"
#include "Sinks.h"

using namespace Hermes;

BOOST_AUTO_TEST_CASE(AutoCheckAliveResponseTest)
{
    TestCaseScope scope("AutoCheckAliveResponseTest");

    std::string upstreamMachineId{"UpstreamMachineId"};
    std::string downstreamMachineId{"DownstreamMachineId"};

    DownstreamSink downstreamSink;
    Hermes::Downstream downstream(1U, downstreamSink);
    Runner<Hermes::Downstream> downstreamRunner(downstream);

    UpstreamSink  upstreamSink;
    Hermes::Upstream upstream(1U, upstreamSink);
    Runner<Hermes::Upstream> upstreamRunner(upstream);

    // suppress periodic CheckAlive on both sides, but auto-response is enabled:
    DownstreamSettings downstreamSettings{upstreamMachineId, 50101};
    downstreamSettings.m_checkAlivePeriodInSeconds = 0;
    downstream.Enable(downstreamSettings);

    Hermes::UpstreamSettings upstreamSettings(downstreamMachineId, "127.0.0.1", 50101);
    upstreamSettings.m_checkAlivePeriodInSeconds = 0;
    upstream.Enable(upstreamSettings);

    // wait until connection is established
    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
    WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
    upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescriptionData(downstreamMachineId, 1U));
    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
    downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescriptionData(upstreamMachineId, 1U));

    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
    WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

    // Send CheckAlive from downstream to upstream:
    {
        downstream.Signal(downstreamSink.m_sessionId, CheckAliveData{ ECheckAliveType::ePING, std::string{"DownstreamId1"} });
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_checkAliveData.m_optionalId.has_value(); });
        BOOST_TEST(*downstreamSink.m_checkAliveData.m_optionalId == "DownstreamId1");
        BOOST_TEST(downstreamSink.m_checkAliveData.m_optionalType.value_or(ECheckAliveType::eUNKNOWN) == ECheckAliveType::ePONG);
        BOOST_TEST(upstreamSink.m_checkAliveData.m_optionalType.value_or(ECheckAliveType::eUNKNOWN) == ECheckAliveType::ePING);
        BOOST_TEST(upstreamSink.m_checkAliveData.m_optionalId.value_or("") == "DownstreamId1");

        // non-ping CheckAlive: no reaction:
        downstream.Signal(downstreamSink.m_sessionId, CheckAliveData{ ECheckAliveType::ePONG, std::string{"DownstreamId2"} });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_checkAliveData.m_optionalType.value_or(ECheckAliveType::eUNKNOWN) == ECheckAliveType::ePONG; });
        BOOST_TEST(upstreamSink.m_checkAliveData.m_optionalId.value_or("") == "DownstreamId2");

        downstream.Signal(downstreamSink.m_sessionId, CheckAliveData{});
        WaitFor(upstreamSink, [&]() { return !upstreamSink.m_checkAliveData.m_optionalType; });
        BOOST_TEST(!upstreamSink.m_checkAliveData.m_optionalId );

        // we should not have received an answer in the meantime:
        BOOST_TEST(downstreamSink.m_checkAliveData.m_optionalType.value_or(ECheckAliveType::eUNKNOWN) == ECheckAliveType::ePONG);
        BOOST_TEST(downstreamSink.m_checkAliveData.m_optionalId.value_or("") == "DownstreamId1");
    }

    // Send CheckAlive from upstream to downstream:
    {
        upstream.Signal(upstreamSink.m_sessionId, CheckAliveData{ ECheckAliveType::ePING, std::string{"UpstreamId1"} });
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_checkAliveData.m_optionalId.has_value(); });
        BOOST_TEST(*upstreamSink.m_checkAliveData.m_optionalId == "UpstreamId1");
        BOOST_TEST(upstreamSink.m_checkAliveData.m_optionalType.value_or(ECheckAliveType::eUNKNOWN) == ECheckAliveType::ePONG);
        BOOST_TEST(downstreamSink.m_checkAliveData.m_optionalType.value_or(ECheckAliveType::eUNKNOWN) == ECheckAliveType::ePING);
        BOOST_TEST(downstreamSink.m_checkAliveData.m_optionalId.value_or("") == "UpstreamId1");

        // non-ping CheckAlive: no reaction:
        upstream.Signal(upstreamSink.m_sessionId, CheckAliveData{ ECheckAliveType::ePONG, std::string{"UpstreamId2"} });
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_checkAliveData.m_optionalType.value_or(ECheckAliveType::eUNKNOWN) == ECheckAliveType::ePONG; });
        BOOST_TEST(downstreamSink.m_checkAliveData.m_optionalId.value_or("") == "UpstreamId2");

        upstream.Signal(upstreamSink.m_sessionId, CheckAliveData{});
        WaitFor(downstreamSink, [&]() { return !downstreamSink.m_checkAliveData.m_optionalType; });
        BOOST_TEST(!downstreamSink.m_checkAliveData.m_optionalId);

        // we should not have received an answer in the meantime:
        BOOST_TEST(upstreamSink.m_checkAliveData.m_optionalType.value_or(ECheckAliveType::eUNKNOWN) == ECheckAliveType::ePONG);
        BOOST_TEST(upstreamSink.m_checkAliveData.m_optionalId.value_or("") == "UpstreamId1");
    }
}

BOOST_AUTO_TEST_CASE(ApplicationCheckAliveResponseTest)
{
    TestCaseScope scope("ApplicationCheckAliveResponseTest");

    std::string upstreamMachineId{"UpstreamMachineId"};
    std::string downstreamMachineId{"DownstreamMachineId"};

    DownstreamSink downstreamSink;
    Hermes::Downstream downstream(1U, downstreamSink);
    Runner<Hermes::Downstream> downstreamRunner(downstream);

    UpstreamSink  upstreamSink;
    Hermes::Upstream upstream(1U, upstreamSink);
    Runner<Hermes::Upstream> upstreamRunner(upstream);

    // suppress periodic CheckAlive on both sides, but auto-response is enabled:
    DownstreamSettings downstreamSettings{upstreamMachineId, 50101};
    downstreamSettings.m_checkAlivePeriodInSeconds = 0;
    downstreamSettings.m_checkAliveResponseMode = ECheckAliveResponseMode::eAPPLICATION;
    downstream.Enable(downstreamSettings);

    Hermes::UpstreamSettings upstreamSettings(downstreamMachineId, "127.0.0.1", 50101);
    upstreamSettings.m_checkAlivePeriodInSeconds = 0;
    upstreamSettings.m_checkAliveResponseMode = ECheckAliveResponseMode::eAPPLICATION;
    upstream.Enable(upstreamSettings);

    // wait until connection is established
    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
    WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
    upstream.Signal(upstreamSink.m_sessionId, Hermes::ServiceDescriptionData(downstreamMachineId, 1U));
    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
    downstream.Signal(downstreamSink.m_sessionId, Hermes::ServiceDescriptionData(upstreamMachineId, 1U));

    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
    WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

    // Send CheckAlive from downstream to upstream:
    downstream.Signal(downstreamSink.m_sessionId, CheckAliveData{ ECheckAliveType::ePING, std::string{"DownstreamId"} });
    WaitFor(upstreamSink, [&]() { return upstreamSink.m_checkAliveData.m_optionalType.value_or(ECheckAliveType::eUNKNOWN) == ECheckAliveType::ePING; });

    upstream.Signal(upstreamSink.m_sessionId, CheckAliveData{ ECheckAliveType::ePING, std::string{"UpstreamId"} });
    WaitFor(downstreamSink, [&]() { return downstreamSink.m_checkAliveData.m_optionalType.value_or(ECheckAliveType::eUNKNOWN) == ECheckAliveType::ePING; });

    // no autoresponse should have been received:
    BOOST_TEST(*upstreamSink.m_checkAliveData.m_optionalId == "DownstreamId");
    BOOST_TEST(*downstreamSink.m_checkAliveData.m_optionalId == "UpstreamId");

}

