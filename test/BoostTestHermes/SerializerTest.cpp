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

#include "stdafx.h"

#include <HermesData.hpp>
#include <HermesSerialization.hpp>

#include "HermesDataGenerators.h"

#include <boost/test/data/test_case.hpp>

#include <boost/mpl/vector.hpp>

#include <fstream>

using EmptyHermesDataTypes = boost::mpl::vector<
    Hermes::RevokeBoardAvailableData,
    Hermes::RevokeMachineReadyData,
    Hermes::GetConfigurationData>;

using NonEmptyHermesDataTypes = boost::mpl::vector<
    Hermes::CheckAliveData,
    Hermes::ServiceDescriptionData,
    Hermes::NotificationData,
    Hermes::BoardAvailableData,
    Hermes::MachineReadyData,
    Hermes::StartTransportData,
    Hermes::StopTransportData,
    Hermes::BoardForecastData,
    Hermes::QueryBoardInfoData,
    Hermes::SendBoardInfoData,
    Hermes::TransportFinishedData,
    Hermes::SetConfigurationData,
    Hermes::CurrentConfigurationData,
    Hermes::SupervisoryServiceDescriptionData,
    Hermes::BoardArrivedData,
    Hermes::BoardDepartedData,
    Hermes::QueryWorkOrderInfoData,
    Hermes::SendWorkOrderInfoData>;

template<class T>
void ToFile(unsigned counter, const std::string& xml)
{
    std::string typeName{typeid(T).name()};
    auto pos = typeName.find(':');
    if (pos != std::string::npos)
    {
        typeName.erase(0U, pos + 2U);
    }
    std::string fileName{"C:\\GeneratedHermesData\\" + typeName + std::to_string(counter) + ".xml"};
    std::ofstream file{fileName};
    file << xml;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(TestEmptyHermesDataTypes, DataT, EmptyHermesDataTypes)
{
    DataT data;
    std::string xml = ToXml(data);
    //ToFile<DataT>(1U, xml);
    auto optionalData = Hermes::FromXml<DataT>(xml);
    BOOST_CHECK(optionalData);
    if (optionalData)
    {
        BOOST_TEST(data == *optionalData);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(TestNonEmptyHermesDataTypes, DataT, NonEmptyHermesDataTypes)
{
    //auto counter = 0U;
    auto samples = GenerateSamples<DataT>();
    for (const auto& data : samples)
    {
        std::string xml = ToXml(data);
        //ToFile<DataT>(++counter, xml);

        auto optionalData = Hermes::FromXml<DataT>(xml);
        BOOST_CHECK(optionalData);
        if (optionalData)
        {
            BOOST_TEST(data == optionalData);
        }
    }
}




