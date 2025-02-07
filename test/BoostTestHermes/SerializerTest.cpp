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

template<class T>
void TestSubBoardsCutoff_(T& data, const uint16_t maxSubBoards)
{
    auto& subBoards = data.m_optionalSubBoards;

    data.m_optionalSubBoards.reserve(maxSubBoards + 1);
    for (uint16_t i{ 0 }; i < maxSubBoards; ++i)
    {
        subBoards.emplace_back();
        subBoards.back().m_pos = i + 1;
        subBoards.back().m_optionalBc = std::to_string(i);
    }

    // maxCount and contents were carefully tweaked to yield slightly less or equal to max message size:
    auto xml = ToXml(data);
    BOOST_CHECK(xml.size() <= cHERMES_MAX_MESSAGE_SIZE);
    auto optionalData = Hermes::FromXml<T>(xml);
    BOOST_CHECK(optionalData == data);

    subBoards.emplace_back();
    subBoards.back().m_optionalBc = "AVeryLongBarcodeStringThatWillSurelyExceedTheMaximumHermesMessageSizeOf65536Bytes";
    xml = ToXml(data);
    BOOST_CHECK(xml.size() < cHERMES_MAX_MESSAGE_SIZE/2); // size should have become a lot smaller
    optionalData = Hermes::FromXml<T>(xml);
    BOOST_CHECK(optionalData != data);

    subBoards.clear();
    BOOST_CHECK(optionalData == data);
}

BOOST_AUTO_TEST_CASE(TestBoardAvailableSubBoardInfoSerializationCutOff)
{
    Hermes::BoardAvailableData data{ "0e4c18f9-5f69-4dbe-9b1c-e705bf7d680f", "123456", Hermes::EBoardQuality::eGOOD,
        Hermes::EFlippedBoard::eTOP_SIDE_IS_UP };

    TestSubBoardsCutoff_(data, 1777);
}

BOOST_AUTO_TEST_CASE(TestBoardArrivedSubBoardInfoSerializationCutOff)
{
    Hermes::BoardArrivedData data{ "M", 1, Hermes::EBoardArrivedTransfer::eINSERTED,
        "0e4c18f9-4dbe-9b1c-e705bf7d680f", "Cr", Hermes::EBoardQuality::eGOOD, Hermes::EFlippedBoard::eTOP_SIDE_IS_UP };

    TestSubBoardsCutoff_(data, 1776);
}

BOOST_AUTO_TEST_CASE(TestBoardDepartedSubBoardInfoSerializationCutOff)
{
    Hermes::BoardDepartedData data{ "M", 1, Hermes::EBoardDepartedTransfer::eREMOVED,
        "0e4c18f9-9b1c-e705bf7d680f", "Cr", Hermes::EBoardQuality::eGOOD, Hermes::EFlippedBoard::eTOP_SIDE_IS_UP };

    TestSubBoardsCutoff_(data, 1776);
}

BOOST_AUTO_TEST_CASE(TestSendWorkOrderInfoSubBoardInfoSerializationCutOff)
{
    Hermes::SendWorkOrderInfoData data{std::string{"MyVeryLongQueryId"}};

    TestSubBoardsCutoff_(data, 1778);
}
