// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include <HermesData.hpp>
#include <HermesSerialization.hpp>

#include "HermesDataGenerators.h"

#include <boost/test/data/test_case.hpp>

#include <boost/mpl/vector.hpp>

using EmptyHermesDataTypes = boost::mpl::vector<
    Hermes::CheckAliveData,
    Hermes::RevokeBoardAvailableData,
    Hermes::RevokeMachineReadyData,
    Hermes::GetConfigurationData>;

using NonEmptyHermesDataTypes = boost::mpl::vector<
    Hermes::ServiceDescription,
    Hermes::NotificationData,
    Hermes::BoardAvailableData,
    Hermes::MachineReadyData,
    Hermes::StartTransportData,
    Hermes::StopTransportData,
    Hermes::TransportFinishedData,
    Hermes::SetConfigurationData,
    Hermes::CurrentConfigurationData>;


BOOST_AUTO_TEST_CASE_TEMPLATE(TestEmptyHermesDataTypes, DataT, EmptyHermesDataTypes)
{
    DataT data;
    std::string xml = ToXml(data);
    auto optionalData = Hermes::ToOptionalData<DataT>(xml);
    BOOST_CHECK(optionalData);
    if (optionalData)
    {
        BOOST_CHECK_EQUAL(data, *optionalData);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(TestNonEmptyHermesDataTypes, DataT, NonEmptyHermesDataTypes)
{
    auto samples = GenerateSamples<DataT>();
    for (const auto& data : samples)
    {
        std::string xml = ToXml(data);
        auto optionalData = Hermes::ToOptionalData<DataT>(xml);
        BOOST_CHECK(optionalData);
        if (optionalData)
        {
            BOOST_CHECK_EQUAL(data, *optionalData);
        }
    }
}




