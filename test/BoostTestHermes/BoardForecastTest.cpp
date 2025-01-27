// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include "Runner.h"
#include "Sinks.h"

using namespace Hermes;

BOOST_AUTO_TEST_CASE(BoardForecastTest)
{
    TestCaseScope scope("BoardForecastTest");

    std::string upstreamMachineId{"UpstreamMachineId"};
    std::string downstreamMachineId{"DownstreamMachineId"};

    DownstreamSink downstreamSink;
    Hermes::Downstream downstream(1U, downstreamSink);
    Runner<Hermes::Downstream> downstreamRunner(downstream);

    UpstreamSink  upstreamSink;
    Hermes::Upstream upstream(1U, upstreamSink);
    Runner<Hermes::Upstream> upstreamRunner(upstream);

    // suppress periodic CheckAlive on both sides
    DownstreamSettings downstreamSettings{upstreamMachineId, 50101};
    downstreamSettings.m_checkAlivePeriodInSeconds = 0;
    downstream.Enable(downstreamSettings);

    Hermes::UpstreamSettings upstreamSettings(downstreamMachineId, "127.0.0.1", 50101);
    upstreamSettings.m_checkAlivePeriodInSeconds = 0;
    upstream.Enable(upstreamSettings);

    // set ServiceDescription accordingly:
    Hermes::ServiceDescriptionData upstreamServiceDescription{downstreamMachineId, 1U};
    upstreamServiceDescription.m_supportedFeatures.m_optionalFeatureBoardForecast = FeatureBoardForecast{};
    Hermes::ServiceDescriptionData downstreamServiceDescription{upstreamMachineId, 1U};
    downstreamServiceDescription.m_supportedFeatures.m_optionalFeatureBoardForecast = FeatureBoardForecast{};

    // wait until connection is established
    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
    WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
    upstream.Signal(upstreamSink.m_sessionId, upstreamServiceDescription);
    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
    downstream.Signal(downstreamSink.m_sessionId, downstreamServiceDescription);

    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
    WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

    // verify ServiceDescription:
    BOOST_TEST(downstreamSink.m_serviceDescription == upstreamServiceDescription);
    BOOST_TEST(upstreamSink.m_serviceDescription == downstreamServiceDescription);

    // make sure, we have no data to start with:
    BOOST_TEST(!upstreamSink.m_boardForecastData.m_optionalBoardId);

    // Respond with SendBoardInfo from upstream to downstream
    BoardForecastData forecast{EBoardQuality::eGOOD, EFlippedBoard::eBOTTOM_SIDE_IS_UP};
    forecast.m_optionalTopBarcode = "Top";
    forecast.m_optionalBottomBarcode = "Bottom";
    forecast.m_optionalBoardId = "BoardId";
    forecast.m_optionalBoardIdCreatedBy = "CreatedBy";

    downstream.Signal(downstreamSink.m_sessionId, forecast);
    WaitFor(upstreamSink, [&]() { return upstreamSink.m_boardForecastData.m_optionalTopBarcode.has_value(); });
    BOOST_TEST(upstreamSink.m_boardForecastData == forecast);

}
