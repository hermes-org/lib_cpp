// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include "Runner.h"
#include "Sinks.h"

#include <HermesSerialization.hpp>

using namespace Hermes;


BOOST_AUTO_TEST_CASE(TestRawXml)
{
    TestCaseScope scope("TestRawXml");

    std::string upstreamMachineId{"UpstreamMachineId"};
    std::string downstreamMachineId{"DownstreamMachineId"};

    std::string boardId{"123e4567-e89b-12d3-a456-426655440000"};

    UpstreamSink upstreamSink;
    Hermes::Upstream upstream(1U, upstreamSink);
    Runner<Hermes::Upstream> upstreamRunner(upstream);
    upstream.Enable(Hermes::UpstreamSettings(downstreamMachineId, "localhost", 50101));

    DownstreamSink  downstreamSink;
    Hermes::Downstream downstream(1U, downstreamSink);
    Runner<Hermes::Downstream> downstreamRunner(downstream);
    downstream.Enable(Hermes::DownstreamSettings(downstreamMachineId, 50101));

    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSOCKET_CONNECTED; });
    WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eSOCKET_CONNECTED; });
    upstream.Signal(upstreamSink.m_sessionId, ToXml(Hermes::ServiceDescriptionData(downstreamMachineId, 1U)));
    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eSERVICE_DESCRIPTION_DOWNSTREAM; });
    downstream.Signal(downstreamSink.m_sessionId, ToXml(Hermes::ServiceDescriptionData(upstreamMachineId, 1U)));
    WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
    WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });

    {
        NotificationData data{ENotificationCode::eCONFIGURATION_ERROR, ESeverity::eINFO, "myNotification"};
        auto rawXml = ToXml(data);
        upstream.Signal(upstreamSink.m_sessionId, rawXml);
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_notificationData == data; });
    }
    {
        CheckAliveData data{};
        auto rawXml = ToXml(data);
        upstream.Signal(upstreamSink.m_sessionId, rawXml);
    }

    {
        NotificationData data{ENotificationCode::eCONFIGURATION_ERROR, ESeverity::eINFO, "myNotification"};
        auto rawXml = ToXml(data);
        downstream.Signal(downstreamSink.m_sessionId, rawXml);
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_notificationData == data; });
    }
    {
        CheckAliveData data{};
        auto rawXml = ToXml(data);
        downstream.Signal(downstreamSink.m_sessionId, rawXml);
    }
    {
        MachineReadyData data{};
        auto rawXml = ToXml(data);
        upstream.Signal(upstreamSink.m_sessionId, rawXml);
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eMACHINE_READY; });
    }
    {
        RevokeMachineReadyData data{};
        auto rawXml = ToXml(data);
        upstream.Signal(upstreamSink.m_sessionId, rawXml);
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
    }
    {
        BoardAvailableData data{boardId, upstreamMachineId, Hermes::EBoardQuality::eANY, Hermes::EFlippedBoard::eSIDE_UP_IS_UNKNOWN};
        auto rawXml = ToXml(data);
        downstream.Signal(downstreamSink.m_sessionId, rawXml);
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eBOARD_AVAILABLE; });
    }
    {
        RevokeBoardAvailableData data{};
        auto rawXml = ToXml(data);
        downstream.Signal(downstreamSink.m_sessionId, rawXml);
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eBOARD_AVAILABLE; });
    }
    {
        MachineReadyData data{};
        auto rawXml = ToXml(data);
        upstream.Signal(upstreamSink.m_sessionId, rawXml);
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eMACHINE_READY; });
    }
    {
        BoardAvailableData data{boardId, upstreamMachineId, Hermes::EBoardQuality::eGOOD, Hermes::EFlippedBoard::eTOP_SIDE_IS_UP};
        auto rawXml = ToXml(data);
        downstream.Signal(downstreamSink.m_sessionId, rawXml);
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eAVAILABLE_AND_READY; });
    }
    {
        StartTransportData data{boardId};
        auto rawXml = ToXml(data);
        upstream.Signal(upstreamSink.m_sessionId, rawXml);
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eTRANSPORTING; });
    }
    {
        TransportFinishedData data{ETransferState::eCOMPLETE, boardId};
        auto rawXml = ToXml(data);
        downstream.Signal(downstreamSink.m_sessionId, rawXml);
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eTRANSPORT_FINISHED; });
    }
    {
        StopTransportData data{ETransferState::eCOMPLETE, boardId};
        auto rawXml = ToXml(data);
        upstream.Signal(upstreamSink.m_sessionId, rawXml);
        WaitFor(upstreamSink, [&]() { return upstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
        WaitFor(downstreamSink, [&]() { return downstreamSink.m_state == EState::eNOT_AVAILABLE_NOT_READY; });
    }
    {
        NotificationData data{ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "myShutdown"};
        upstream.Reset(data);
    }
    {
        NotificationData data{ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "myShutdown"};
        downstream.Reset(data);
    }
}

BOOST_AUTO_TEST_CASE(TestVerticalRawXml)
{
    TestCaseScope scope("TestVerticalRawXml");

    std::string serviceId{ "VerticalServiceId" };
    std::string clientId{ "VerticalClientId" };
    uint16_t verticalPort{ 50201 };

    std::string boardId{ "123e4567-e89b-12d3-a456-426655440000" };

    VerticalServiceSink serviceSink;
    Hermes::VerticalService service{ serviceSink };
    Runner<Hermes::VerticalService> serviceRunner{ service };
    VerticalServiceSettings serviceSettings{ serviceId, verticalPort };
    serviceSettings.m_checkAlivePeriodInSeconds = 0.0;
    service.Enable(serviceSettings);

    VerticalClientSink clientSink;
    Hermes::VerticalClient client(clientSink);
    Runner<Hermes::VerticalClient> clientRunner(client);
    Hermes::VerticalClientSettings clientSettings{ clientId, "localhost", verticalPort };
    clientSettings.m_checkAlivePeriodInSeconds = 0.0;
    client.Enable(clientSettings);

    WaitFor(clientSink, [&]() { return clientSink.m_state == EVerticalState::eSOCKET_CONNECTED; });
    WaitFor(serviceSink, [&]() { return !serviceSink.m_sessionMap.empty(); });

    auto& serviceSession = serviceSink.m_sessionMap.begin()->second;
    auto serviceSessionId = serviceSink.m_sessionMap.begin()->first;
    auto clientSessionId = clientSink.m_sessionId;

    client.Signal(clientSessionId, ToXml(Hermes::SupervisoryServiceDescriptionData(clientId)));
    WaitFor(serviceSink, [&]() { return serviceSession.m_state == EVerticalState::eSUPERVISORY_SERVICE_DESCRIPTION; });
    service.Signal(serviceSessionId, Hermes::SupervisoryServiceDescriptionData(serviceId));
    WaitFor(clientSink, [&]() { return clientSink.m_state == EVerticalState::eCONNECTED; });

    {
        NotificationData data{ ENotificationCode::eCONFIGURATION_ERROR, ESeverity::eINFO, "myNotification" };
        auto rawXml = ToXml(data);
        client.Signal(clientSessionId, rawXml);
        WaitFor(serviceSink, [&]() { return serviceSession.m_notificationData == data; });
    }
    {
        CheckAliveData data{};
        auto rawXml = ToXml(data);
        client.Signal(clientSessionId, rawXml);
        WaitFor(serviceSink, [&]() { return serviceSession.m_checkAliveData == data; });
    }
    {
        GetConfigurationData data{};
        auto rawXml = ToXml(data);
        client.Signal(clientSessionId, rawXml);
        WaitFor(serviceSink, [&]() { return serviceSession.m_getConfigurationData == data; });
    }
    {
        SetConfigurationData data{};
        data.m_machineId = "MyId";
        data.m_downstreamConfigurations.emplace_back(2U, verticalPort);
        data.m_upstreamConfigurations.emplace_back(2U, "MyAddress", verticalPort);
        auto rawXml = ToXml(data);
        client.Signal(clientSessionId, rawXml);
        WaitFor(serviceSink, [&]() { return serviceSession.m_setConfigurationData == data; });
    }
    {
        SendWorkOrderInfoData data{};
        data.m_optionalBottomBarcode = "BottomBarcode";
        auto rawXml = ToXml(data);
        client.Signal(clientSessionId, rawXml);
        WaitFor(serviceSink, [&]() { return serviceSession.m_sendWorkOrderInfoData == data; });
    }
    {
        NotificationData data{ ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "myShutdown" };
        client.Reset(ToXml(data));
    }
}
