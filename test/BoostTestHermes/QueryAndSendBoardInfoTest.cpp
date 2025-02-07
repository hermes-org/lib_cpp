// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include "Runner.h"
#include "Sinks.h"

using namespace Hermes;

BOOST_AUTO_TEST_CASE(QueryAndSendBoardInfoTest)
{
    TestCaseScope scope("QueryAndSendBoardInfoTest");

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
    upstreamServiceDescription.m_supportedFeatures.m_optionalFeatureSendBoardInfo = FeatureSendBoardInfo{};
    Hermes::ServiceDescriptionData downstreamServiceDescription{upstreamMachineId, 1U};
    downstreamServiceDescription.m_supportedFeatures.m_optionalFeatureQueryBoardInfo = FeatureQueryBoardInfo{};

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
    BOOST_TEST(!downstreamSink.m_queryBoardInfoData.m_optionalTopBarcode);
    BOOST_TEST(!upstreamSink.m_sendBoardInfoData.m_optionalTopBarcode);

    // Send QueryBoardInfo from downstream to upstream
    QueryBoardInfoData query;
    query.m_optionalTopBarcode = "Top";
    query.m_optionalBottomBarcode = "Bottom";
    upstream.Signal(upstreamSink.m_sessionId, query);
    ::WaitFor(downstreamSink, [&]() { return downstreamSink.m_queryBoardInfoData.m_optionalTopBarcode.has_value(); });
    BOOST_TEST(downstreamSink.m_queryBoardInfoData == query);

    // Respond with SendBoardInfo from upstream to downstream
    SendBoardInfoData info;
    info.m_optionalTopBarcode = "TopResponse";
    info.m_optionalBottomBarcode = "BottomResponse";
    info.m_optionalBoardId = "BoardId";
    info.m_optionalBoardIdCreatedBy = "CreatedBy";
    
    info.m_optionalSubBoards.emplace_back(Hermes::SubBoard{1, Hermes::ESubBoardState::eGOOD});
    info.m_optionalSubBoards.emplace_back(Hermes::SubBoard{2, Hermes::ESubBoardState::eSKIP});

    downstream.Signal(downstreamSink.m_sessionId, info);
    ::WaitFor(upstreamSink, [&]() { return upstreamSink.m_sendBoardInfoData.m_optionalTopBarcode.has_value(); });
    BOOST_TEST(upstreamSink.m_sendBoardInfoData == info);
}

