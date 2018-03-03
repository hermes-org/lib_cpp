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

#include "Serializer.h"

#include <chrono>
#include <time.h>
#include <iomanip>

namespace Hermes
{
    namespace
    {
        const StringView sServiceDescriptionTag{"ServiceDescription"};
        const StringView sBoardAvailableDataTag{"BoardAvailable"};
        const StringView sRevokeBoardAvailableTag{"RevokeBoardAvailable"};
        const StringView sMachineReadyTag{"MachineReady"};
        const StringView sRevokeMachineReadyTag{"RevokeMachineReady"};
        const StringView sStartTransportTag{"StartTransport"};
        const StringView sTransportFinishedTag{"TransportFinished"};
        const StringView sStopTransportTag{"StopTransport"};
        const StringView sNotificationTag{"Notification"};
        const StringView sCheckAliveTag{"CheckAlive"};
        const StringView sGetConfigurationTag{"GetConfiguration"};
        const StringView sSetConfigurationTag{"SetConfiguration"};
        const StringView sCurrentConfigurationTag{"CurrentConfiguration"};

        const StringView Tag_(const ServiceDescription&) { return sServiceDescriptionTag; }
        const StringView Tag_(const BoardAvailableData&) { return sBoardAvailableDataTag; }
        const StringView Tag_(const RevokeBoardAvailableData&) { return sRevokeBoardAvailableTag; }
        const StringView Tag_(const MachineReadyData&) { return sMachineReadyTag; }
        const StringView Tag_(const RevokeMachineReadyData&) { return sRevokeMachineReadyTag; }
        const StringView Tag_(const StartTransportData&) { return sStartTransportTag; }
        const StringView Tag_(const TransportFinishedData&) { return sTransportFinishedTag; }
        const StringView Tag_(const StopTransportData&) { return sStopTransportTag; }
        const StringView Tag_(const NotificationData&) { return sNotificationTag; }
        const StringView Tag_(const CheckAliveData&) { return sCheckAliveTag; }
        const StringView Tag_(const GetConfigurationData&) { return sGetConfigurationTag; }
        const StringView Tag_(const SetConfigurationData&) { return sSetConfigurationTag; }
        const StringView Tag_(const CurrentConfigurationData&) { return sCurrentConfigurationTag; }

        const std::string cHERMES = "Hermes";
        const std::size_t cHERMES_SIZE = 6U;
        const std::string cHERMES_END_TAG = "</Hermes";

        std::string _GetTimeStamp()
        {
            auto now = std::chrono::system_clock::now();
            auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
            auto fraction = now - seconds;
            time_t cnow = std::chrono::system_clock::to_time_t(now);
            tm local_tm;
#ifdef _WINDOWS
            localtime_s(&local_tm, &cnow);
#else
            localtime_r(&cnow, &local_tm);
#endif
            std::stringstream ss;
            ss << std::put_time(&local_tm, "%Y-%m-%dT%H:%M:%S.");
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(fraction);
            ss << milliseconds.count();
            return ss.str();
        }

        std::string Envelope_(const pugi::xml_node &element)
        {
            pugi::xml_document domDocument;
            auto root = domDocument.append_child(cHERMES.c_str());
            root.append_attribute("Timestamp").set_value(_GetTimeStamp().c_str());
            root.append_copy(element);
            std::stringstream ss;
            domDocument.save(ss, PUGIXML_TEXT(" "), pugi::format_indent | pugi::format_no_declaration, pugi::encoding_utf8);
            return ss.str();
        }
    }

    bool TakeMessage(std::string &out_message, std::string &inout_buffer)
    {
        // find "<Hermes" removing everything that is encountered until then:
        for (;;)
        {
            std::size_t startIndex = inout_buffer.find('<');
            if (startIndex == std::string::npos)
            {
                inout_buffer.clear();
                return false;
            }
            if (startIndex != 0)
            {
                inout_buffer = inout_buffer.substr(startIndex);
            }

            if (inout_buffer.size() < 1U + cHERMES_SIZE)
                return false;

            if (inout_buffer.compare(1U, cHERMES_SIZE, cHERMES) == 0)
                break;

            inout_buffer = inout_buffer.substr(1U);
        }

        // find the ending "</Hermes"
        std::size_t endTagIndex = inout_buffer.find(cHERMES_END_TAG);
        if (endTagIndex == std::string::npos)
            return false;

        // ... and the final '>'
        std::size_t endIndex = inout_buffer.find('>', endTagIndex + cHERMES_END_TAG.size());
        if (endIndex == std::string::npos)
            return false;

        out_message = inout_buffer.substr(0U, endIndex + 1U);
        inout_buffer = inout_buffer.substr(endIndex + 1U);
        return true;
    }

    // helpers for both CurrentConfigurationMessage and SetConfigurationMessage
    void FromXml(std::vector<DownstreamConfiguration>& configs, const pugi::xml_node in_node)
    {
        configs.clear();
        for (auto const& node : in_node.child("DownstreamConfigurations").children("DownstreamConfiguration"))
        {
            configs.emplace_back();
            auto& config = configs.back();
            config.m_downstreamLaneId = node.attribute("DownstreamLaneId").as_uint();
            if (const auto& attribute = node.attribute("ClientAddress"))
            {
                config.m_optionalClientAddress = attribute.as_string();
            }
            config.m_port = static_cast<uint16_t>(node.attribute("Port").as_uint());
        }
    }

    void FromXml(std::vector<UpstreamConfiguration>& configs, const pugi::xml_node in_node)
    {
        configs.clear();
        for (auto const& node : in_node.child("UpstreamConfigurations").children("UpstreamConfiguration"))
        {
            configs.emplace_back();
            auto& config = configs.back();
            config.m_upstreamLaneId = node.attribute("UpstreamLaneId").as_uint();
            config.m_hostAddress = node.attribute("HostAddress").as_string();
            config.m_port = static_cast<uint16_t>(node.attribute("Port").as_uint());
        }
    }

    void ToXml(const std::vector<UpstreamConfiguration>& configs, pugi::xml_node root)
    {
        pugi::xml_node parentNode = root.append_child("UpstreamConfigurations");
        for (auto const& config : configs)
        {
            pugi::xml_node node = parentNode.append_child("UpstreamConfiguration");
            node.append_attribute("UpstreamLaneId").set_value(config.m_upstreamLaneId);
            node.append_attribute("HostAddress").set_value(config.m_hostAddress.c_str());
            node.append_attribute("Port").set_value(config.m_port);
        }
    }

    void ToXml(const std::vector<DownstreamConfiguration>& configs, pugi::xml_node root)
    {
        pugi::xml_node parentNode = root.append_child("DownstreamConfigurations");
        for (auto const& config : configs)
        {
            pugi::xml_node node = parentNode.append_child("DownstreamConfiguration");
            node.append_attribute("DownstreamLaneId").set_value(config.m_downstreamLaneId);
            if (!config.m_optionalClientAddress.empty())
            {
                node.append_attribute("ClientAddress").set_value(config.m_optionalClientAddress.c_str());
            }
            node.append_attribute("Port").set_value(config.m_port);

        }
    }

}

std::string Hermes::Serialize(const ServiceDescription& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    rootElement.append_attribute("LaneId").set_value(data.m_laneId);
    rootElement.append_attribute("MachineId").set_value(data.m_machineId.c_str());
    rootElement.append_attribute("Version").set_value(data.m_version.c_str());
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const BoardAvailableData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    //required attributes
    rootElement.append_attribute("BoardId").set_value(data.m_boardId.c_str());
    rootElement.append_attribute("BoardIdCreatedBy").set_value(data.m_boardIdCreatedBy.c_str());
    rootElement.append_attribute("FailedBoard").set_value(static_cast<int>(data.m_failedBoard));
    if (const auto& attr = data.m_optionalProductTypeId)
    {
        rootElement.append_attribute("ProductTypeId").set_value(attr->c_str());
    }
    rootElement.append_attribute("FlippedBoard").set_value(static_cast<int>(data.m_flippedBoard));
    if (const auto& attr = data.m_optionalTopBarcode)
    {
        rootElement.append_attribute("TopBarcode").set_value(attr->c_str());
    }
    if (const auto& attr = data.m_optionalBottomBarcode)
    {
        rootElement.append_attribute("BottomBarcode").set_value(attr->c_str());
    }
    if (const auto& attr = data.m_optionalLengthInMM)
        rootElement.append_attribute("Length").set_value(*attr);
    if (const auto& attr = data.m_optionalWidthInMM)
        rootElement.append_attribute("Width").set_value(*attr);
    if (const auto& attr = data.m_optionalThicknessInMM)
        rootElement.append_attribute("Thickness").set_value(*attr);
    if (const auto& attr = data.m_optionalConveyorSpeedInMMPerSecs)
        rootElement.append_attribute("ConveyorSpeed").set_value(*attr);
    if (const auto& attr = data.m_optionalTopClearanceHeightInMM)
        rootElement.append_attribute("TopClearanceHeight").set_value(*attr);
    if (const auto& attr = data.m_optionalBottomClearanceHeightInMM)
        rootElement.append_attribute("BottomClearanceHeight").set_value(*attr);
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const RevokeBoardAvailableData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const MachineReadyData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    rootElement.append_attribute("FailedBoard").set_value(static_cast<int>(data.m_failedBoard));
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const RevokeMachineReadyData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const StartTransportData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    rootElement.append_attribute("BoardId").set_value(data.m_boardId.c_str());
    if (auto attr = data.m_optionalConveyorSpeedInMMPerSecs)
        rootElement.append_attribute("ConveyorSpeed").set_value(*attr);
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const TransportFinishedData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    rootElement.append_attribute("TransferState").set_value(static_cast<int>(data.m_transferState));
    rootElement.append_attribute("BoardId").set_value(data.m_boardId.c_str());
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const StopTransportData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    rootElement.append_attribute("TransferState").set_value(static_cast<int>(data.m_transferState));
    rootElement.append_attribute("BoardId").set_value(data.m_boardId.c_str());

    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const NotificationData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    rootElement.append_attribute("NotificationCode").set_value(static_cast<int>(data.m_notificationCode));
    rootElement.append_attribute("Severity").set_value(static_cast<int>(data.m_severity));
    rootElement.append_attribute("Description").set_value(data.m_description.c_str());
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const CheckAliveData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const GetConfigurationData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const SetConfigurationData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    rootElement.append_attribute("MachineId").set_value(data.m_machineId.c_str());
    ToXml(data.m_upstreamConfigurations, rootElement);
    ToXml(data.m_downstreamConfigurations, rootElement);
    return Envelope_(rootElement);
}

std::string Hermes::Serialize(const CurrentConfigurationData& data)
{
    pugi::xml_document doc;
    pugi::xml_node rootElement = doc.append_child(Tag_(data).data());
    if (data.m_optionalMachineId)
    {
        rootElement.append_attribute("MachineId").set_value(data.m_optionalMachineId->c_str());
    }
    ToXml(data.m_upstreamConfigurations, rootElement);
    ToXml(data.m_downstreamConfigurations, rootElement);
    return Envelope_(rootElement);
}

namespace Hermes
{
    namespace
    {
        ServiceDescription ServiceDescriptionFromXml(pugi::xml_node xmlNode)
        {
            ServiceDescription data;
            data.m_laneId = xmlNode.attribute("LaneId").as_uint();
            data.m_machineId = xmlNode.attribute("MachineId").as_string();
            data.m_version = xmlNode.attribute("Version").as_string();
            return data;
        }

        BoardAvailableData BoardAvailableDataFromXml(pugi::xml_node xmlNode)
        {
            BoardAvailableData data;
            //required attributes
            data.m_boardId = xmlNode.attribute("BoardId").as_string();
            data.m_boardIdCreatedBy = xmlNode.attribute("BoardIdCreatedBy").as_string();
            data.m_failedBoard = static_cast<EBoardQuality>(xmlNode.attribute("FailedBoard").as_int());

            if (auto attr = xmlNode.attribute("ProductTypeId"))
            {
                data.m_optionalProductTypeId = attr.as_string();
            }

            data.m_flippedBoard = static_cast<EFlippedBoard>(xmlNode.attribute("FlippedBoard").as_int());
            if (auto attr = xmlNode.attribute("TopBarcode"))
            {
                data.m_optionalTopBarcode = attr.as_string();
            }
            if (auto attr = xmlNode.attribute("BottomBarcode"))
            {
                data.m_optionalBottomBarcode = attr.as_string();
            }
            if (auto attr = xmlNode.attribute("Length"))
            {
                data.m_optionalLengthInMM = attr.as_double();
            }
            if (auto attr = xmlNode.attribute("Width"))
            {
                data.m_optionalWidthInMM = attr.as_double();
            }
            if (auto attr = xmlNode.attribute("Thickness"))
            {
                data.m_optionalThicknessInMM = attr.as_double();
            }
            if (auto attr = xmlNode.attribute("ConveyorSpeed"))
            {
                data.m_optionalConveyorSpeedInMMPerSecs = attr.as_double();
            }
            if (auto attr = xmlNode.attribute("TopClearanceHeight"))
            {
                data.m_optionalTopClearanceHeightInMM = attr.as_double();
            }
            if (auto attr = xmlNode.attribute("BottomClearanceHeight"))
            {
                data.m_optionalBottomClearanceHeightInMM = attr.as_double();
            }
            return data;
        }

        RevokeBoardAvailableData RevokeBoardAvailableDataFromXml(pugi::xml_node)
        {
            RevokeBoardAvailableData data;
            return data;
        }

        MachineReadyData MachineReadyDataFromXml(pugi::xml_node xmlNode)
        {
            MachineReadyData data;
            data.m_failedBoard = static_cast<EBoardQuality>(xmlNode.attribute("FailedBoard").as_int());
            return data;
        }

        RevokeMachineReadyData RevokeMachineReadyDataFromXml(pugi::xml_node)
        {
            RevokeMachineReadyData data;
            return data;
        }

        StartTransportData StartTransportDataFromXml(pugi::xml_node xmlNode)
        {
            StartTransportData data;
            data.m_boardId = xmlNode.attribute("BoardId").as_string();

            if (auto attr = xmlNode.attribute("ConveyorSpeed"))
            {
                data.m_optionalConveyorSpeedInMMPerSecs = attr.as_double();
            }
            return data;
        }

        StopTransportData StopTransportDataFromXml(pugi::xml_node xmlNode)
        {
            StopTransportData data;
            data.m_transferState = static_cast<ETransferState>(xmlNode.attribute("TransferState").as_int());
            data.m_boardId = xmlNode.attribute("BoardId").as_string();
            return data;
        }

        TransportFinishedData TransportFinishedDataFromXml(pugi::xml_node xmlNode)
        {
            TransportFinishedData data;
            data.m_transferState = static_cast<ETransferState>(xmlNode.attribute("TransferState").as_int());
            data.m_boardId = xmlNode.attribute("BoardId").as_string();
            return data;
        }

        NotificationData NotificationDataFromXml(pugi::xml_node xmlNode)
        {
            NotificationData data;
            data.m_notificationCode = static_cast<ENotificationCode>(xmlNode.attribute("NotificationCode").as_int());
            data.m_severity = static_cast<ESeverity>(xmlNode.attribute("Severity").as_int());
            data.m_description = xmlNode.attribute("Description").as_string();
            return data;
        }

        CheckAliveData CheckAliveDataFromXml(pugi::xml_node)
        {
            CheckAliveData data;
            return data;
        }

        SetConfigurationData SetConfigurationDataFromXml(pugi::xml_node xmlNode)
        {
            SetConfigurationData data;
            data.m_machineId = xmlNode.attribute("MachineId").as_string();
            FromXml(data.m_downstreamConfigurations, xmlNode);
            FromXml(data.m_upstreamConfigurations, xmlNode);
            return data;
        }

        GetConfigurationData GetConfigurationDataFromXml(pugi::xml_node)
        {
            GetConfigurationData data;
            return data;
        }

        CurrentConfigurationData CurrentConfigurationDataFromXml(pugi::xml_node xmlNode)
        {
            CurrentConfigurationData data;
            if (auto&& attr = xmlNode.attribute("MachineId"))
            {
                data.m_optionalMachineId = attr.as_string();
            }
            FromXml(data.m_downstreamConfigurations, xmlNode);
            FromXml(data.m_upstreamConfigurations, xmlNode);

            return data;
        }
    }
}

Hermes::VariantData Hermes::Deserialize(StringView message)
{
    pugi::xml_document doc;
    auto parseResult = doc.load_buffer(message.data(), message.size(), pugi::parse_default, pugi::encoding_utf8);
    if (!parseResult)
    {
        return Error{EErrorCode::eCLIENT_ERROR,
            std::string(parseResult.description()) + " at offset " + std::to_string(parseResult.offset)};
    }
    pugi::xml_node envelopeNode = doc.first_child();
    pugi::xml_node rootNode = envelopeNode.first_child();
    if (!rootNode)
    {
        return Error{EErrorCode::eCLIENT_ERROR, std::string("Missing message type node")};
    }

    const std::string& rootName = rootNode.name();
    if (rootName == Tag_(Hermes::ServiceDescription()))
        return Hermes::ServiceDescriptionFromXml(rootNode);
    else if (rootName == Tag_(Hermes::BoardAvailableData()))
        return Hermes::BoardAvailableDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::RevokeBoardAvailableData()))
        return Hermes::RevokeBoardAvailableDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::MachineReadyData()))
        return Hermes::MachineReadyDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::RevokeMachineReadyData()))
        return Hermes::RevokeMachineReadyDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::StartTransportData()))
        return Hermes::StartTransportDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::StopTransportData()))
        return Hermes::StopTransportDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::TransportFinishedData()))
        return Hermes::TransportFinishedDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::NotificationData()))
        return Hermes::NotificationDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::CheckAliveData()))
        return Hermes::CheckAliveDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::SetConfigurationData()))
        return Hermes::SetConfigurationDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::GetConfigurationData()))
        return Hermes::GetConfigurationDataFromXml(rootNode);
    else if (rootName == Tag_(Hermes::CurrentConfigurationData()))
        return Hermes::CurrentConfigurationDataFromXml(rootNode);
    else
        return boost::blank();
}


