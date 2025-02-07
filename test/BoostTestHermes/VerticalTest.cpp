// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include "Runner.h"
#include "Sinks.h"

#include <boost/mp11.hpp>

#include <array>
#include <HermesSerialization.hpp>

using namespace Hermes;

BOOST_AUTO_TEST_CASE(VerticalServicePostTest)
{
    TestCaseScope scope("VerticalServicePostTest");

    std::string called1;
    bool called2 = false;
    std::mutex m_mutex;
    using Lock = std::unique_lock<std::mutex>;
    std::condition_variable cv;

    VerticalServiceSink verticalServiceSink;
    Hermes::VerticalService verticalService(verticalServiceSink);

    std::string postedValue = "Posted!";
    verticalService.Post([&, postedValue]()
    {
        Lock lock(m_mutex);
        called1 = postedValue;
    });

    {
        Runner<Hermes::VerticalService> runner(verticalService);
        verticalService.Post([&]()
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

BOOST_AUTO_TEST_CASE(VerticalClientPostTest)
{
    TestCaseScope scope("VerticalClientPostTest");

    std::string called1;
    bool called2 = false;
    std::mutex m_mutex;
    using Lock = std::unique_lock<std::mutex>;
    std::condition_variable cv;

    VerticalClientSink verticalClientSink;
    Hermes::VerticalClient verticalClient(verticalClientSink);

    std::string postedValue = "Posted!";
    verticalClient.Post([&, postedValue]()
    {
        Lock lock(m_mutex);
        called1 = postedValue;
    });

    {
        Runner<Hermes::VerticalClient> runner(verticalClient);
        verticalClient.Post([&]()
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

// helper structure to hold the stuff associated with a client
struct Client
{
    int m_id;
    std::string m_systemId{ "MyClient_" + std::to_string(m_id) };
    VerticalClientSink m_sink;
    Hermes::VerticalClient m_impl{ m_sink };
    Runner<Hermes::VerticalClient> m_runner{ m_impl };

    template<class T>
    void Signal(const T& data)
    {
        m_impl.Signal(m_sink.m_sessionId, data);
    }

    Client(int id) : m_id{ id } {}
};
using ClientUp = std::unique_ptr<Client>;
using Clients = std::vector<ClientUp>;

struct Service
{
    std::string m_systemId{ "MyService" };
    VerticalServiceSink m_sink;
    Hermes::VerticalService m_impl{ m_sink };
    Runner<Hermes::VerticalService> m_runner{ m_impl };

    template<class T>
    void Signal(unsigned sessionId, const T& data)
    {
        m_impl.Signal(sessionId, data);
    }

    template<class T>
    void Signal(const T& data)
    {
        m_impl.Signal(data);
    }
};

BOOST_AUTO_TEST_CASE(VerticalTest)
{
    TestCaseScope scope("VerticalTest");

    Service service;
    service.m_impl.Enable(Hermes::VerticalServiceSettings(service.m_systemId, 50100));

    Clients clients;

    Hermes::SupervisoryServiceDescriptionData serviceDescription{ service.m_systemId };
    serviceDescription.m_supportedFeatures.m_optionalFeatureCheckAliveResponse = Hermes::FeatureCheckAliveResponse{};
    serviceDescription.m_supportedFeatures.m_optionalFeatureSendWorkOrderInfo = Hermes::FeatureSendWorkOrderInfo{};
    serviceDescription.m_supportedFeatures.m_optionalFeatureQueryHermesCapabilities = Hermes::FeatureQueryHermesCapabilities{};

    Hermes::SupervisoryServiceDescriptionData clientDescription{ std::string{} };
    clientDescription.m_supportedFeatures.m_optionalFeatureBoardTracking = Hermes::FeatureBoardTracking{};
    clientDescription.m_supportedFeatures.m_optionalFeatureCheckAliveResponse = Hermes::FeatureCheckAliveResponse{};
    clientDescription.m_supportedFeatures.m_optionalFeatureQueryWorkOrderInfo = Hermes::FeatureQueryWorkOrderInfo{};
    clientDescription.m_supportedFeatures.m_optionalFeatureConfiguration = Hermes::FeatureConfiguration{};
    clientDescription.m_supportedFeatures.m_optionalFeatureSendHermesCapabilities = Hermes::FeatureSendHermesCapabilities{};

    const std::size_t cCLIENT_COUNT = 10;

    std::array<QueryWorkOrderInfoData, cCLIENT_COUNT> queryWorkOrderInfos;
    std::array<SendWorkOrderInfoData, cCLIENT_COUNT> sendWorkOrderInfos;
    std::array<ReplyWorkOrderInfoData, cCLIENT_COUNT> replyWorkOrderInfos;
    std::array<BoardArrivedData, cCLIENT_COUNT> boardsArrived;
    std::array<BoardDepartedData, cCLIENT_COUNT> boardsDeparted;
    std::array<SetConfigurationData, cCLIENT_COUNT> setConfigurations;
    std::array<GetConfigurationData, cCLIENT_COUNT> getConfigurations;
    std::array<CurrentConfigurationData, cCLIENT_COUNT> currentConfigurations;
    std::array<NotificationData, cCLIENT_COUNT> notifications;


    for (auto i = 0U; i < cCLIENT_COUNT; ++i)
    {
        auto sessionId = i + 1U;
        auto id = std::to_string(sessionId);
        queryWorkOrderInfos[i] = QueryWorkOrderInfoData{ "MyQuery_" + id };
        sendWorkOrderInfos[i] = SendWorkOrderInfoData{ "MyInfo_" + id };
        replyWorkOrderInfos[i] = ReplyWorkOrderInfoData{ "MyInfo_" + id, EReplyWorkOrderInfoStatus::eACCEPTED_AND_READY };
        boardsArrived[i] = BoardArrivedData{"ArrivedId_" + id, sessionId, EBoardArrivedTransfer::eINSERTED, "Id_" + id, "myself"
            , EBoardQuality::eGOOD, EFlippedBoard::eTOP_SIDE_IS_UP};
        boardsDeparted[i] = BoardDepartedData{ "DepartedId_" + id, sessionId, EBoardDepartedTransfer::eTRANSFERRED, "Id_" + id, "myself"
            , EBoardQuality::eGOOD, EFlippedBoard::eTOP_SIDE_IS_UP };
        setConfigurations[i] = SetConfigurationData{ "SetConfigId_" + id };
        getConfigurations[i] = GetConfigurationData{};
        currentConfigurations[i].m_optionalMachineId = "CurrentConfigId_" + id;
        notifications[i] = NotificationData{ ENotificationCode::eCONFIGURATION_ERROR, ESeverity::eWARNING, "MyNotification_" + id };

        clients.emplace_back(std::make_unique<Client>(i));
        auto& client = *clients.back();
        client.m_impl.Enable(Hermes::VerticalClientSettings(client.m_systemId, "127.0.0.1", 50100));

        // wait for connection:
        WaitFor(client.m_sink, [&]() { return client.m_sink.m_state == Hermes::EVerticalState::eSOCKET_CONNECTED; });
        WaitFor(service.m_sink, [&]() { return service.m_sink.m_sessionMap.size() == sessionId; });
        
        auto& serviceSession = service.m_sink.m_sessionMap.rbegin()->second;
        BOOST_TEST(serviceSession.m_state == Hermes::EVerticalState::eSOCKET_CONNECTED);

        // exchange service description:
        client.Signal(clientDescription);
        WaitFor(service.m_sink, [&]() { return serviceSession.m_state == Hermes::EVerticalState::eSUPERVISORY_SERVICE_DESCRIPTION; });
        service.Signal( serviceSession.m_sessionId, serviceDescription );

        WaitFor(client.m_sink, [&]() { return client.m_sink.m_state == Hermes::EVerticalState::eCONNECTED; });
    }

    for (auto i = 0U; i < cCLIENT_COUNT; ++i)
    {
        auto sessionId = i + 1U;
        auto& client = *clients[i];
        
        service.Signal(sessionId, boardsArrived[i]);
        service.Signal(sessionId, boardsDeparted[i]);
        service.Signal(sessionId, currentConfigurations[i]);
        service.Signal(sessionId, queryWorkOrderInfos[i]);
        service.Signal(sessionId, replyWorkOrderInfos[i]);
        service.Signal(sessionId, notifications[i]);

        client.Signal(sendWorkOrderInfos[i]);
        client.Signal(getConfigurations[i]);
        client.Signal(setConfigurations[i]);
        client.Signal(notifications[i]);
    }

    for (auto i = 0U; i < cCLIENT_COUNT; ++i)
    {
        auto sessionId = i + 1U;
        auto& serviceSessionSink = service.m_sink.m_sessionMap[sessionId];
        auto& clientSink = clients[i]->m_sink;

        WaitFor(clientSink, [&]() { return clientSink.m_notificationData == notifications[i]; });
        BOOST_TEST(clientSink.m_boardArrivedData == boardsArrived[i]);
        BOOST_TEST(clientSink.m_boardDepartedData == boardsDeparted[i]);
        BOOST_TEST(clientSink.m_currentConfigurationData == currentConfigurations[i]);
        BOOST_TEST(clientSink.m_queryWorkOrderInfoData == queryWorkOrderInfos[i]);
        BOOST_TEST(clientSink.m_replyWorkOrderInfoData == replyWorkOrderInfos[i]);

        WaitFor(service.m_sink, [&]() { return serviceSessionSink.m_notificationData == notifications[i]; });
        BOOST_TEST(serviceSessionSink.m_sendWorkOrderInfoData == sendWorkOrderInfos[i]);
        BOOST_TEST(serviceSessionSink.m_getConfigurationData == getConfigurations[i]);
        BOOST_TEST(serviceSessionSink.m_setConfigurationData == setConfigurations[i]);
    }

    // disconnect
    std::array<NotificationData, cCLIENT_COUNT> disableNotifications;
    for (auto i = 0U; i < cCLIENT_COUNT; ++i)
    {
        disableNotifications[i] = NotificationData{ ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "Disabled" };
        clients[i]->m_impl.Disable(disableNotifications[i]);
    }

    for (auto i = 0U; i < cCLIENT_COUNT; ++i)
    {
        auto sessionId = i + 1U;
        auto& serviceSessionSink = service.m_sink.m_sessionMap[sessionId];
        auto& clientSink = clients[i]->m_sink;

        WaitFor(service.m_sink, [&]() { return serviceSessionSink.m_state == EVerticalState::eDISCONNECTED; });
        BOOST_TEST(clientSink.m_notificationData == notifications[i]);
        BOOST_TEST(serviceSessionSink.m_notificationData == disableNotifications[i]);
    }

}

BOOST_AUTO_TEST_CASE(VerticalBoardTrackingTest)
{
    TestCaseScope scope("VerticalBoardTrackingTest");

    Service service;
    service.m_impl.Enable(Hermes::VerticalServiceSettings(service.m_systemId, 50100));

    Clients clients;

    Hermes::SupervisoryServiceDescriptionData serviceDescription{ service.m_systemId };
    serviceDescription.m_supportedFeatures.m_optionalFeatureCheckAliveResponse = Hermes::FeatureCheckAliveResponse{};
    serviceDescription.m_supportedFeatures.m_optionalFeatureSendWorkOrderInfo = Hermes::FeatureSendWorkOrderInfo{};

    Hermes::SupervisoryServiceDescriptionData clientDescription{ std::string{} };
    clientDescription.m_supportedFeatures.m_optionalFeatureBoardTracking = Hermes::FeatureBoardTracking{};
    clientDescription.m_supportedFeatures.m_optionalFeatureCheckAliveResponse = Hermes::FeatureCheckAliveResponse{};
    clientDescription.m_supportedFeatures.m_optionalFeatureQueryWorkOrderInfo = Hermes::FeatureQueryWorkOrderInfo{};
    clientDescription.m_supportedFeatures.m_optionalFeatureConfiguration = Hermes::FeatureConfiguration{};

    BoardArrivedData boardArrivedData{ "Arrived", 1, EBoardArrivedTransfer::eINSERTED, "ArrivedId", "myself"
        , EBoardQuality::eGOOD, EFlippedBoard::eTOP_SIDE_IS_UP };
    BoardDepartedData boardDepartedData{ "Departed", 2, EBoardDepartedTransfer::eTRANSFERRED, "DepartedId", "myself"
        , EBoardQuality::eBAD, EFlippedBoard::eBOTTOM_SIDE_IS_UP };

    const std::size_t cCLIENT_COUNT = 10;

    std::array<Optional<BoardArrivedData>, cCLIENT_COUNT> boardsArrived;
    std::array<Optional<BoardDepartedData>, cCLIENT_COUNT> boardsDeparted;

    for (auto i = 0U; i < cCLIENT_COUNT; ++i)
    {
        auto sessionId = i + 1U;
        auto id = std::to_string(sessionId);

        // only every second client is subsribed to the BoardTracking:
        if (i % 2 == 0)
        {
            boardsArrived[i] = boardArrivedData;
            boardsDeparted[i] = boardDepartedData;
            clientDescription.m_supportedFeatures.m_optionalFeatureBoardTracking = Hermes::FeatureBoardTracking{};
        }
        else
        {
            clientDescription.m_supportedFeatures.m_optionalFeatureBoardTracking.reset();
        }

        clients.emplace_back(std::make_unique<Client>(i));
        auto& client = *clients.back();
        client.m_impl.Enable(Hermes::VerticalClientSettings(client.m_systemId, "127.0.0.1", 50100));

        // wait for connection:
        WaitFor(client.m_sink, [&]() { return client.m_sink.m_state == Hermes::EVerticalState::eSOCKET_CONNECTED; });
        WaitFor(service.m_sink, [&]() { return service.m_sink.m_sessionMap.size() == sessionId; });

        auto& serviceSession = service.m_sink.m_sessionMap.rbegin()->second;
        BOOST_TEST(serviceSession.m_state == Hermes::EVerticalState::eSOCKET_CONNECTED);

        // exchange service description:
        client.Signal(clientDescription);
        WaitFor(service.m_sink, [&]() { return serviceSession.m_state == Hermes::EVerticalState::eSUPERVISORY_SERVICE_DESCRIPTION; });
        service.Signal(serviceSession.m_sessionId, serviceDescription);

        WaitFor(client.m_sink, [&]() { return client.m_sink.m_state == Hermes::EVerticalState::eCONNECTED; });
    }


    service.Signal(boardArrivedData);
    service.Signal(boardDepartedData);
    NotificationData notificationData{ ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "Synchronizer" };

    for (auto i = 0U; i < cCLIENT_COUNT; ++i)
    {
        // to sure that we've awaited each client, send a notification message
        auto sessionId = i + 1U;
        service.Signal(sessionId, notificationData);

        auto& clientSink = clients[i]->m_sink;
        WaitFor(clientSink, [&]() { return clientSink.m_notificationData == notificationData; });
        BOOST_TEST(clientSink.m_boardArrivedData == boardsArrived[i]);
        BOOST_TEST(clientSink.m_boardDepartedData == boardsDeparted[i]);
    }

    // disconnect
    NotificationData disableNotification{ ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "Disabled" };
    service.m_impl.Disable(disableNotification);

    // wait for all clients not connected
    for (auto i = 0U; i < cCLIENT_COUNT; ++i)
    {
        auto& clientSink = clients[i]->m_sink;
        WaitFor(clientSink, [&]() { return clientSink.m_state == Hermes::EVerticalState::eDISCONNECTED; });
        WaitFor(clientSink, [&]() { return clientSink.m_notificationData == disableNotification; });
    }

    for (const auto& entry : service.m_sink.m_sessionMap)
    {
        BOOST_TEST(entry.second.m_state == Hermes::EVerticalState::eDISCONNECTED);
    }

    clients.clear(); // desctruction should terminate all client threads
}

BOOST_AUTO_TEST_CASE(VerticalCheckAliveTest)
{
    TestCaseScope scope("VerticalCheckAliveTest");

    Service service;
    VerticalServiceSettings serviceSettings{ service.m_systemId, 50200 };
    serviceSettings.m_checkAlivePeriodInSeconds = 0.0;
    serviceSettings.m_checkAliveResponseMode = ECheckAliveResponseMode::eAUTO;
    service.m_impl.Enable(serviceSettings);

    Client client{ 1 };
    VerticalClientSettings clientSettings{ client.m_systemId, "localhost", 50200 };
    clientSettings.m_checkAlivePeriodInSeconds = 0.0;
    clientSettings.m_checkAliveResponseMode = ECheckAliveResponseMode::eAUTO;
    client.m_impl.Enable(clientSettings);

    // wait for connection:
    WaitFor(client.m_sink, [&]() { return client.m_sink.m_state == Hermes::EVerticalState::eSOCKET_CONNECTED; });
    WaitFor(service.m_sink, [&]() { return service.m_sink.m_sessionMap.size() == 1U; });

    auto& serviceSession = service.m_sink.m_sessionMap.begin()->second;
    BOOST_TEST(serviceSession.m_state == Hermes::EVerticalState::eSOCKET_CONNECTED);

    CheckAliveData ping;
    ping.m_optionalId = "Hi";
    ping.m_optionalType = ECheckAliveType::ePING;

    CheckAliveData pong;
    pong.m_optionalId = ping.m_optionalId;
    pong.m_optionalType = ECheckAliveType::ePONG;

    // client to service:
    client.m_impl.Signal(client.m_id, ping);
    WaitFor(service.m_sink, [&]() { return serviceSession.m_checkAliveData == ping; });
    // should auto-reply with pong:
    WaitFor(client.m_sink, [&]() { return client.m_sink.m_checkAliveData == pong; });

    // service to client:
    service.m_impl.Signal(serviceSession.m_sessionId, ping);
    WaitFor(client.m_sink, [&]() { return client.m_sink.m_checkAliveData == ping; });
    // should auto-reply with pong:
    WaitFor(service.m_sink, [&]() { return serviceSession.m_checkAliveData == pong; });
}

BOOST_AUTO_TEST_CASE(QueryAndSendHermesCapabilities)
{
    TestCaseScope scope("VerticalHermesCapabilitiesTest");

    Service service;
    VerticalServiceSettings serviceSettings{ service.m_systemId, 50200 };
    serviceSettings.m_checkAlivePeriodInSeconds = 0.0;
    serviceSettings.m_checkAliveResponseMode = ECheckAliveResponseMode::eAUTO;
    service.m_impl.Enable(serviceSettings);

    Client client{ 1 };
    VerticalClientSettings clientSettings{ client.m_systemId, "localhost", 50200 };
    clientSettings.m_checkAlivePeriodInSeconds = 0.0;
    clientSettings.m_checkAliveResponseMode = ECheckAliveResponseMode::eAUTO;
    client.m_impl.Enable(clientSettings);

    // wait for connection:
    WaitFor(client.m_sink, [&]() { return client.m_sink.m_state == Hermes::EVerticalState::eSOCKET_CONNECTED; });
    WaitFor(service.m_sink, [&]() { return service.m_sink.m_sessionMap.size() == 1U; });

    auto& serviceSession = service.m_sink.m_sessionMap.begin()->second;
    BOOST_TEST(serviceSession.m_state == Hermes::EVerticalState::eSOCKET_CONNECTED);

    QueryHermesCapabilitiesData query;
    // client to service:
    client.m_impl.Signal(client.m_id, query);

    WaitFor(service.m_sink, [&]() 
        { 
            return serviceSession.m_queryHermesCapabilitiesData.has_value() == true;
        });

    // Und jetzt in die andere Richtung
    SendHermesCapabilitiesData reply;
    reply.m_attributes.m_action = 1;
    reply.m_attributes.m_batchId = 5;
    reply.m_attributes.m_bottomBarcode = 4;
    // usw.
    reply.m_optionalMessages.m_optionalMessageBoardArrived = MessageBoardArrived();
    reply.m_optionalMessages.m_optionalMessageBoardForecast = MessageBoardForecast();
    // usw.
    
    auto zumAnschaun = ToXml(reply);

    service.m_impl.Signal(serviceSession.m_sessionId, reply);
    WaitFor(client.m_sink, [&]() { return client.m_sink.m_sendHermesCapabilitiesData == reply; });
}




