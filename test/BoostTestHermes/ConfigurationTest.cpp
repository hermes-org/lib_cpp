// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include <Hermes.hpp>

#include "Runner.h"
#include "Trace.h"

#include <iostream>
#include <mutex>

struct ConfigurationClient : Hermes::ITraceCallback
{
    void OnTrace(unsigned sessionId, Hermes::ETraceType type, Hermes::StringView trace) override
    {
        TestTrace("ConfigurationClient", type, sessionId, trace);
    }
};

struct ConfigurationProvider : Hermes::IConfigurationServiceCallback
{
    Hermes::CurrentConfigurationData m_currentConfiguration;
    Hermes::ConnectionInfo m_info;
    unsigned m_sessionId = 0U;
    Hermes::ConnectionInfo m_connectionInfo;
    Hermes::ConfigurationService m_service{*this};

    void OnConnected(unsigned sessionId, const Hermes::ConnectionInfo& info) override
    {
        m_sessionId = sessionId;
        m_connectionInfo = info;
    }

    Hermes::Error OnSetConfiguration(unsigned sessionId, const Hermes::ConnectionInfo& info, const Hermes::SetConfigurationData& configuration) override
    {
        std::ostringstream oss;
        oss << "Configuration::OnSetConfiguration(" << sessionId << ',' << configuration << ',' << info << ')';
        TestTrace("ConfigurationTest", Hermes::ETraceType::eDEBUG, m_sessionId, oss.str());

        m_currentConfiguration.m_optionalMachineId = configuration.m_machineId;
        m_currentConfiguration.m_downstreamConfigurations = configuration.m_downstreamConfigurations;
        m_currentConfiguration.m_upstreamConfigurations = configuration.m_upstreamConfigurations;
        m_info = info;
        BOOST_TEST(m_sessionId == sessionId);
        BOOST_TEST(m_connectionInfo == info);
        return{};
    }

    Hermes::CurrentConfigurationData OnGetConfiguration(unsigned sessionId, const Hermes::ConnectionInfo& info) override
    {
        std::ostringstream oss;
        oss << "Configuration::OnGetConfiguration(" << sessionId << ',' << info << "), m_currentConfiguration=" << m_currentConfiguration;
        TestTrace("ConfigurationTest", Hermes::ETraceType::eDEBUG, m_sessionId, oss.str());

        BOOST_TEST(m_sessionId == sessionId);
        return m_currentConfiguration;
    }

    void OnDisconnected(unsigned sessionId, const Hermes::Error& error) override
    {
        BOOST_TEST(!error);
        BOOST_TEST(m_sessionId == sessionId);
    }

    void OnTrace(unsigned sessionId, Hermes::ETraceType type, Hermes::StringView trace) override
    {
        TestTrace("ConfigurationService", type, sessionId, trace);
    }
};

BOOST_AUTO_TEST_CASE(ConfigurationServicePostTest)
{
    TestCaseScope scope("DownstreamPostTest");

    std::string called1;
    bool called2 = false;
    std::mutex m_mutex;
    using Lock = std::unique_lock<std::mutex>;
    std::condition_variable cv;

    ConfigurationProvider configurationProvider;

    std::string postedValue = "Posted!";
    configurationProvider.m_service.Post([&, postedValue]()
    {
        Lock lock(m_mutex);
        called1 = postedValue;
    });

    {
        Runner<Hermes::ConfigurationService> runner(configurationProvider.m_service);
        configurationProvider.m_service.Post([&]()
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

BOOST_AUTO_TEST_CASE(GetEmptyHermesConfigurationTest)
{
    TestCaseScope scope("GetEmptyHermesConfigurationTest");

    ConfigurationProvider configurationProvider;
    ConfigurationClient configurationClient;
    Runner<Hermes::ConfigurationService> runner(configurationProvider.m_service);

    Hermes::ConfigurationServiceSettings settings;
    configurationProvider.m_service.Enable(settings);

    const auto& result = Hermes::GetConfiguration("localhost", 10000U, &configurationClient);
    BOOST_TEST(result.first == Hermes::CurrentConfigurationData());
    BOOST_TEST(!result.second);


}

BOOST_AUTO_TEST_CASE(SetHermesConfigurationTest)
{
    TestCaseScope scope("SetHermesConfigurationTest");

    ConfigurationProvider configurationProvider;
    ConfigurationClient configurationClient;
    Runner<Hermes::ConfigurationService> runner(configurationProvider.m_service);

    Hermes::ConfigurationServiceSettings settings;
    configurationProvider.m_service.Enable(settings);

    // build up a configuration:
    Hermes::SetConfigurationData sentConfig;
    sentConfig.m_machineId = "TX_5";

    sentConfig.m_downstreamConfigurations.emplace_back(1U, 0); // on lane 1, take default port
    sentConfig.m_downstreamConfigurations.emplace_back(2U, "localhost", 50101);

    sentConfig.m_upstreamConfigurations.emplace_back(1U, "localhost", 50101);
    sentConfig.m_upstreamConfigurations.emplace_back(2U, "127.0.0.1", 50102);

    // send it to a machine:
    Hermes::CurrentConfigurationData receivedConfig;
    std::vector<Hermes::NotificationData> notifications;
    auto error = Hermes::SetConfiguration("localhost", sentConfig, 60, &receivedConfig, &notifications, &configurationClient);
    BOOST_TEST(!error);
    BOOST_TEST(bool(receivedConfig.m_optionalMachineId));
    if (receivedConfig.m_optionalMachineId) { BOOST_TEST(sentConfig.m_machineId == *receivedConfig.m_optionalMachineId); }
    BOOST_TEST(sentConfig.m_downstreamConfigurations == receivedConfig.m_downstreamConfigurations);
    BOOST_TEST(sentConfig.m_upstreamConfigurations == receivedConfig.m_upstreamConfigurations);

    // get it back from the machine:
    auto result = Hermes::GetConfiguration("127.0.0.1", 60, &configurationClient);
    BOOST_TEST(!result.second);
    BOOST_TEST(result.first == receivedConfig);

}

