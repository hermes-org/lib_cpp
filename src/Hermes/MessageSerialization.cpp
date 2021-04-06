#include "stdafx.h"

#include "MessageSerialization.h"

#include "BasicPugiSerialization.h"
#include "SenderEnvelope.h"

#include <chrono>
#include <time.h>
#include <iomanip>

namespace Hermes
{
    template<> struct PugiSerializer<FeatureBoardForecast>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureBoardForecast&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureBoardForecast&) {}
    };

    template<> struct PugiSerializer<FeatureCheckAliveResponse>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureCheckAliveResponse&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureCheckAliveResponse&) {}
    };

    template<> struct PugiSerializer<FeatureQueryBoardInfo>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureQueryBoardInfo&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureQueryBoardInfo&) {}
    };

    template<> struct PugiSerializer<FeatureSendBoardInfo>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureSendBoardInfo&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureSendBoardInfo&) {}
    };

    template<> struct PugiSerializer<SupportedFeatures>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node parent, const SupportedFeatures& data)
        {
            Serialize(parent, "FeatureBoardForecast", data.m_optionalFeatureBoardForecast);
            Serialize(parent, "FeatureCheckAliveResponse", data.m_optionalFeatureCheckAliveResponse);
            Serialize(parent, "FeatureQueryBoardInfo", data.m_optionalFeatureQueryBoardInfo);
            Serialize(parent, "FeatureSendBoardInfo", data.m_optionalFeatureSendBoardInfo);
        }

        static void ReadElement(pugi::xml_node parent, Error& error, SupportedFeatures& data)
        {
            Deserialize(parent, "FeatureBoardForecast", error, data.m_optionalFeatureBoardForecast);
            Deserialize(parent, "FeatureCheckAliveResponse", error, data.m_optionalFeatureCheckAliveResponse);
            Deserialize(parent, "FeatureQueryBoardInfo", error, data.m_optionalFeatureQueryBoardInfo);
            Deserialize(parent, "FeatureSendBoardInfo", error, data.m_optionalFeatureSendBoardInfo);
        }
    };

    template<> struct PugiSerializer<UpstreamConfiguration>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node parent, const UpstreamConfiguration& data) 
        {
            Serialize(parent, "UpstreamLaneId", data.m_upstreamLaneId);
            Serialize(parent, "UpstreamInterfaceId", data.m_optionalUpstreamInterfaceId);
            Serialize(parent, "HostAddress", data.m_hostAddress);
            Serialize(parent, "Port", data.m_port);
        }

        static void ReadElement(pugi::xml_node parent, Error& error, UpstreamConfiguration& data) 
        {
            Deserialize(parent, "UpstreamLaneId", error, data.m_upstreamLaneId);
            Deserialize(parent, "UpstreamInterfaceId", error, data.m_optionalUpstreamInterfaceId);
            Deserialize(parent, "HostAddress", error, data.m_hostAddress);
            Deserialize(parent, "Port", error, data.m_port);
        }
    };

    template<> struct PugiSerializer<DownstreamConfiguration>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node parent, const DownstreamConfiguration& data)
        {
            Serialize(parent, "DownstreamLaneId", data.m_downstreamLaneId);
            Serialize(parent, "DownstreamInterfaceId", data.m_optionalDownstreamInterfaceId);
            Serialize(parent, "ClientAddress", data.m_optionalClientAddress);
            Serialize(parent, "Port", data.m_port);
        }

        static void ReadElement(pugi::xml_node parent, Error& error, DownstreamConfiguration& data)
        {
            Deserialize(parent, "DownstreamLaneId", error, data.m_downstreamLaneId);
            Deserialize(parent, "DownstreamInterfaceId", error, data.m_optionalDownstreamInterfaceId);
            Deserialize(parent, "ClientAddress", error, data.m_optionalClientAddress);
            Deserialize(parent, "Port", error, data.m_port);
        }
    };

    template<> struct PugiSerializer<UpstreamConfigurations>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node parent, const UpstreamConfigurations& data)
        {
            for (const auto& item : data)
            {
                Serialize(parent, "UpstreamConfiguration", item);
            }
        }
        static void ReadElement(pugi::xml_node parent, Error& error, UpstreamConfigurations& data)
        {
            for (const auto& node : parent.children("UpstreamConfiguration"))
            {
                data.emplace_back();
                PugiSerializer<UpstreamConfiguration>::ReadElement(node, error, data.back());
            }
        }
    };

    template<> struct PugiSerializer<DownstreamConfigurations>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node parent, const DownstreamConfigurations& data)
        {
            for (const auto& item : data)
            {
                Serialize(parent, "DownstreamConfiguration", item);
            }
        }
        static void ReadElement(pugi::xml_node parent, Error& error, DownstreamConfigurations& data)
        {
            for (const auto& node : parent.children("DownstreamConfiguration"))
            {
                data.emplace_back();
                PugiSerializer<DownstreamConfiguration>::ReadElement(node, error, data.back());
            }
        }
    };

    template<> struct PugiSerializer<FeatureConfiguration>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureConfiguration&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureConfiguration&) {}
    };

    template<> struct PugiSerializer<FeatureBoardTracking>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureBoardTracking&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureBoardTracking&) {}
    };

    template<> struct PugiSerializer<FeatureQueryWorkOrderInfo>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureQueryWorkOrderInfo&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureQueryWorkOrderInfo&) {}
    };

    template<> struct PugiSerializer<FeatureSendWorkOrderInfo>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureSendWorkOrderInfo&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureSendWorkOrderInfo&) {}
    };

    template<> struct PugiSerializer<SupervisoryFeatures>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node parent, const SupervisoryFeatures& data)
        {
            Serialize(parent, "FeatureConfiguration", data.m_optionalFeatureConfiguration);
            Serialize(parent, "FeatureCheckAliveResponse", data.m_optionalFeatureCheckAliveResponse);
            Serialize(parent, "FeatureBoardTracking", data.m_optionalFeatureBoardTracking);
            Serialize(parent, "FeatureQueryWorkOrderInfo", data.m_optionalFeatureQueryWorkOrderInfo);
            Serialize(parent, "FeatureSendWorkOrderInfo", data.m_optionalFeatureSendWorkOrderInfo);
        }

        static void ReadElement(pugi::xml_node parent, Error& error, SupervisoryFeatures& data)
        {
            Deserialize(parent, "FeatureConfiguration", error, data.m_optionalFeatureConfiguration);
            Deserialize(parent, "FeatureCheckAliveResponse", error, data.m_optionalFeatureCheckAliveResponse);
            Deserialize(parent, "FeatureBoardTracking", error, data.m_optionalFeatureBoardTracking);
            Deserialize(parent, "FeatureQueryWorkOrderInfo", error, data.m_optionalFeatureQueryWorkOrderInfo);
            Deserialize(parent, "FeatureSendWorkOrderInfo", error, data.m_optionalFeatureSendWorkOrderInfo);
        }
    };
}

std::string Hermes::Serialize(const ServiceDescriptionData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<ServiceDescriptionData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "LaneId", data.m_laneId);
    Serialize(envelope.DataNode(), "MachineId", data.m_machineId);
    Serialize(envelope.DataNode(), "InterfaceId", data.m_optionalInterfaceId);
    Serialize(envelope.DataNode(), "Version", data.m_version);
    Serialize(envelope.DataNode(), "SupportedFeatures", data.m_supportedFeatures);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const BoardAvailableData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<BoardAvailableData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "BoardId", data.m_boardId);
    Serialize(envelope.DataNode(), "BoardIdCreatedBy", data.m_boardIdCreatedBy);
    Serialize(envelope.DataNode(), "FailedBoard", data.m_failedBoard);
    Serialize(envelope.DataNode(), "ProductTypeId", data.m_optionalProductTypeId);
    Serialize(envelope.DataNode(), "FlippedBoard", data.m_flippedBoard);
    Serialize(envelope.DataNode(), "TopBarcode", data.m_optionalTopBarcode);
    Serialize(envelope.DataNode(), "BottomBarcode", data.m_optionalBottomBarcode);
    Serialize(envelope.DataNode(), "Length", data.m_optionalLengthInMM);
    Serialize(envelope.DataNode(), "Width", data.m_optionalWidthInMM);
    Serialize(envelope.DataNode(), "Thickness", data.m_optionalThicknessInMM);
    Serialize(envelope.DataNode(), "ConveyorSpeed", data.m_optionalConveyorSpeedInMMPerSecs);
    Serialize(envelope.DataNode(), "TopClearanceHeight", data.m_optionalTopClearanceHeightInMM);
    Serialize(envelope.DataNode(), "BottomClearanceHeight", data.m_optionalBottomClearanceHeightInMM);
    Serialize(envelope.DataNode(), "Weight", data.m_optionalWeightInGrams);
    Serialize(envelope.DataNode(), "WorkOrderId", data.m_optionalWorkOrderId);
    Serialize(envelope.DataNode(), "BatchId", data.m_optionalBatchId);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const RevokeBoardAvailableData&)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<RevokeBoardAvailableData>::cTAG_VIEW);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const MachineReadyData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<MachineReadyData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "FailedBoard", data.m_failedBoard);
    Serialize(envelope.DataNode(), "ForecastId", data.m_optionalForecastId);
    Serialize(envelope.DataNode(), "BoardId", data.m_optionalBoardId);
    Serialize(envelope.DataNode(), "ProductTypeId", data.m_optionalProductTypeId);
    Serialize(envelope.DataNode(), "FlippedBoard", data.m_optionalFlippedBoard);
    Serialize(envelope.DataNode(), "TopBarcode", data.m_optionalTopBarcode);
    Serialize(envelope.DataNode(), "BottomBarcode", data.m_optionalBottomBarcode);
    Serialize(envelope.DataNode(), "Length", data.m_optionalLengthInMM);
    Serialize(envelope.DataNode(), "Width", data.m_optionalWidthInMM);
    Serialize(envelope.DataNode(), "Thickness", data.m_optionalThicknessInMM);
    Serialize(envelope.DataNode(), "ConveyorSpeed", data.m_optionalConveyorSpeedInMMPerSecs);
    Serialize(envelope.DataNode(), "TopClearanceHeight", data.m_optionalTopClearanceHeightInMM);
    Serialize(envelope.DataNode(), "BottomClearanceHeight", data.m_optionalBottomClearanceHeightInMM);
    Serialize(envelope.DataNode(), "Weight", data.m_optionalWeightInGrams);
    Serialize(envelope.DataNode(), "WorkOrderId", data.m_optionalWorkOrderId);
    Serialize(envelope.DataNode(), "BatchId", data.m_optionalBatchId);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const RevokeMachineReadyData&)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<RevokeMachineReadyData>::cTAG_VIEW);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const StartTransportData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<StartTransportData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "BoardId", data.m_boardId);
    Serialize(envelope.DataNode(), "ConveyorSpeed", data.m_optionalConveyorSpeedInMMPerSecs);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const TransportFinishedData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<TransportFinishedData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "TransferState", data.m_transferState);
    Serialize(envelope.DataNode(), "BoardId", data.m_boardId);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const StopTransportData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<StopTransportData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "TransferState", data.m_transferState);
    Serialize(envelope.DataNode(), "BoardId", data.m_boardId);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const NotificationData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<NotificationData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "NotificationCode", data.m_notificationCode);
    Serialize(envelope.DataNode(), "Severity", data.m_severity);
    Serialize(envelope.DataNode(), "Description", data.m_description);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const CheckAliveData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<CheckAliveData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "Type", data.m_optionalType);
    Serialize(envelope.DataNode(), "Id", data.m_optionalId);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const GetConfigurationData&)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<GetConfigurationData>::cTAG_VIEW);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const SetConfigurationData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<SetConfigurationData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "MachineId", data.m_machineId);
    Serialize(envelope.DataNode(), "SupervisorySystemPort", data.m_optionalSupervisorySystemPort);
    Serialize(envelope.DataNode(), "UpstreamConfigurations", data.m_upstreamConfigurations);
    Serialize(envelope.DataNode(), "DownstreamConfigurations", data.m_downstreamConfigurations);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const CurrentConfigurationData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<CurrentConfigurationData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "MachineId", data.m_optionalMachineId);
    Serialize(envelope.DataNode(), "SupervisorySystemPort", data.m_optionalSupervisorySystemPort);
    Serialize(envelope.DataNode(), "UpstreamConfigurations", data.m_upstreamConfigurations);
    Serialize(envelope.DataNode(), "DownstreamConfigurations", data.m_downstreamConfigurations);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const BoardForecastData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<BoardForecastData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "ForecastId", data.m_optionalForecastId);
    Serialize(envelope.DataNode(), "TimeUntilAvailable", data.m_optionalTimeUntilAvailableInSeconds);    
    Serialize(envelope.DataNode(), "BoardId", data.m_optionalBoardId);
    Serialize(envelope.DataNode(), "BoardIdCreatedBy", data.m_optionalBoardIdCreatedBy);
    Serialize(envelope.DataNode(), "FailedBoard", data.m_failedBoard);
    Serialize(envelope.DataNode(), "ProductTypeId", data.m_optionalProductTypeId);
    Serialize(envelope.DataNode(), "FlippedBoard", data.m_flippedBoard);
    Serialize(envelope.DataNode(), "TopBarcode", data.m_optionalTopBarcode);
    Serialize(envelope.DataNode(), "BottomBarcode", data.m_optionalBottomBarcode);
    Serialize(envelope.DataNode(), "Length", data.m_optionalLengthInMM);
    Serialize(envelope.DataNode(), "Width", data.m_optionalWidthInMM);
    Serialize(envelope.DataNode(), "Thickness", data.m_optionalThicknessInMM);
    Serialize(envelope.DataNode(), "ConveyorSpeed", data.m_optionalConveyorSpeedInMMPerSecs);
    Serialize(envelope.DataNode(), "TopClearanceHeight", data.m_optionalTopClearanceHeightInMM);
    Serialize(envelope.DataNode(), "BottomClearanceHeight", data.m_optionalBottomClearanceHeightInMM);
    Serialize(envelope.DataNode(), "Weight", data.m_optionalWeightInGrams);
    Serialize(envelope.DataNode(), "WorkOrderId", data.m_optionalWorkOrderId);
    Serialize(envelope.DataNode(), "BatchId", data.m_optionalBatchId);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const QueryBoardInfoData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<QueryBoardInfoData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "TopBarcode", data.m_optionalTopBarcode);
    Serialize(envelope.DataNode(), "BottomBarcode", data.m_optionalBottomBarcode);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const SendBoardInfoData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<SendBoardInfoData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "BoardId", data.m_optionalBoardId);
    Serialize(envelope.DataNode(), "BoardIdCreatedBy", data.m_optionalBoardIdCreatedBy);
    Serialize(envelope.DataNode(), "FailedBoard", data.m_optionalFailedBoard);
    Serialize(envelope.DataNode(), "ProductTypeId", data.m_optionalProductTypeId);
    Serialize(envelope.DataNode(), "FlippedBoard", data.m_optionalFlippedBoard);
    Serialize(envelope.DataNode(), "TopBarcode", data.m_optionalTopBarcode);
    Serialize(envelope.DataNode(), "BottomBarcode", data.m_optionalBottomBarcode);
    Serialize(envelope.DataNode(), "Length", data.m_optionalLengthInMM);
    Serialize(envelope.DataNode(), "Width", data.m_optionalWidthInMM);
    Serialize(envelope.DataNode(), "Thickness", data.m_optionalThicknessInMM);
    Serialize(envelope.DataNode(), "ConveyorSpeed", data.m_optionalConveyorSpeedInMMPerSecs);
    Serialize(envelope.DataNode(), "TopClearanceHeight", data.m_optionalTopClearanceHeightInMM);
    Serialize(envelope.DataNode(), "BottomClearanceHeight", data.m_optionalBottomClearanceHeightInMM);
    Serialize(envelope.DataNode(), "Weight", data.m_optionalWeightInGrams);
    Serialize(envelope.DataNode(), "WorkOrderId", data.m_optionalWorkOrderId);
    Serialize(envelope.DataNode(), "BatchId", data.m_optionalBatchId);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const SupervisoryServiceDescriptionData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<SupervisoryServiceDescriptionData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "SystemId", data.m_systemId);
    Serialize(envelope.DataNode(), "Version", data.m_version);
    Serialize(envelope.DataNode(), "SupportedFeatures", data.m_supportedFeatures);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const BoardArrivedData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<BoardArrivedData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "MachineId", data.m_machineId);
    Serialize(envelope.DataNode(), "UpstreamLaneId", data.m_upstreamLaneId);
    Serialize(envelope.DataNode(), "UpstreamInterfaceId", data.m_optionalUpstreamInterfaceId);
    Serialize(envelope.DataNode(), "MagazineId", data.m_optionalMagazineId);
    Serialize(envelope.DataNode(), "SlotId", data.m_optionalSlotId);
    Serialize(envelope.DataNode(), "BoardTransfer", data.m_boardTransfer);
    Serialize(envelope.DataNode(), "BoardId", data.m_boardId);
    Serialize(envelope.DataNode(), "BoardIdCreatedBy", data.m_boardIdCreatedBy);
    Serialize(envelope.DataNode(), "FailedBoard", data.m_failedBoard);
    Serialize(envelope.DataNode(), "ProductTypeId", data.m_optionalProductTypeId);
    Serialize(envelope.DataNode(), "FlippedBoard", data.m_flippedBoard);
    Serialize(envelope.DataNode(), "TopBarcode", data.m_optionalTopBarcode);
    Serialize(envelope.DataNode(), "BottomBarcode", data.m_optionalBottomBarcode);
    Serialize(envelope.DataNode(), "Length", data.m_optionalLengthInMM);
    Serialize(envelope.DataNode(), "Width", data.m_optionalWidthInMM);
    Serialize(envelope.DataNode(), "Thickness", data.m_optionalThicknessInMM);
    Serialize(envelope.DataNode(), "ConveyorSpeed", data.m_optionalConveyorSpeedInMMPerSecs);
    Serialize(envelope.DataNode(), "TopClearanceHeight", data.m_optionalTopClearanceHeightInMM);
    Serialize(envelope.DataNode(), "BottomClearanceHeight", data.m_optionalBottomClearanceHeightInMM);
    Serialize(envelope.DataNode(), "Weight", data.m_optionalWeightInGrams);
    Serialize(envelope.DataNode(), "WorkOrderId", data.m_optionalWorkOrderId);
    Serialize(envelope.DataNode(), "BatchId", data.m_optionalBatchId);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const BoardDepartedData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<BoardDepartedData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "MachineId", data.m_machineId);
    Serialize(envelope.DataNode(), "DownstreamLaneId", data.m_downstreamLaneId);
    Serialize(envelope.DataNode(), "DownstreamInterfaceId", data.m_optionalDownstreamInterfaceId);
    Serialize(envelope.DataNode(), "MagazineId", data.m_optionalMagazineId);
    Serialize(envelope.DataNode(), "SlotId", data.m_optionalSlotId);
    Serialize(envelope.DataNode(), "BoardTransfer", data.m_boardTransfer);
    Serialize(envelope.DataNode(), "BoardId", data.m_boardId);
    Serialize(envelope.DataNode(), "BoardIdCreatedBy", data.m_boardIdCreatedBy);
    Serialize(envelope.DataNode(), "FailedBoard", data.m_failedBoard);
    Serialize(envelope.DataNode(), "ProductTypeId", data.m_optionalProductTypeId);
    Serialize(envelope.DataNode(), "FlippedBoard", data.m_flippedBoard);
    Serialize(envelope.DataNode(), "TopBarcode", data.m_optionalTopBarcode);
    Serialize(envelope.DataNode(), "BottomBarcode", data.m_optionalBottomBarcode);
    Serialize(envelope.DataNode(), "Length", data.m_optionalLengthInMM);
    Serialize(envelope.DataNode(), "Width", data.m_optionalWidthInMM);
    Serialize(envelope.DataNode(), "Thickness", data.m_optionalThicknessInMM);
    Serialize(envelope.DataNode(), "ConveyorSpeed", data.m_optionalConveyorSpeedInMMPerSecs);
    Serialize(envelope.DataNode(), "TopClearanceHeight", data.m_optionalTopClearanceHeightInMM);
    Serialize(envelope.DataNode(), "BottomClearanceHeight", data.m_optionalBottomClearanceHeightInMM);
    Serialize(envelope.DataNode(), "Weight", data.m_optionalWeightInGrams);
    Serialize(envelope.DataNode(), "WorkOrderId", data.m_optionalWorkOrderId);
    Serialize(envelope.DataNode(), "BatchId", data.m_optionalBatchId);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const QueryWorkOrderInfoData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<QueryWorkOrderInfoData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "QueryId", data.m_optionalQueryId);
    Serialize(envelope.DataNode(), "MachineId", data.m_machineId);
    Serialize(envelope.DataNode(), "MagazineId", data.m_optionalMagazineId);
    Serialize(envelope.DataNode(), "SlotId", data.m_optionalSlotId);
    Serialize(envelope.DataNode(), "Barcode", data.m_optionalBarcode);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const SendWorkOrderInfoData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<SendWorkOrderInfoData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "QueryId", data.m_optionalQueryId);
    Serialize(envelope.DataNode(), "WorkOrderId", data.m_optionalWorkOrderId);
    Serialize(envelope.DataNode(), "BatchId", data.m_optionalBatchId);
    Serialize(envelope.DataNode(), "BoardId", data.m_optionalBoardId);
    Serialize(envelope.DataNode(), "BoardIdCreatedBy", data.m_optionalBoardIdCreatedBy);
    Serialize(envelope.DataNode(), "FailedBoard", data.m_optionalFailedBoard);
    Serialize(envelope.DataNode(), "ProductTypeId", data.m_optionalProductTypeId);
    Serialize(envelope.DataNode(), "FlippedBoard", data.m_optionalFlippedBoard);
    Serialize(envelope.DataNode(), "TopBarcode", data.m_optionalTopBarcode);
    Serialize(envelope.DataNode(), "BottomBarcode", data.m_optionalBottomBarcode);
    Serialize(envelope.DataNode(), "Length", data.m_optionalLengthInMM);
    Serialize(envelope.DataNode(), "Width", data.m_optionalWidthInMM);
    Serialize(envelope.DataNode(), "Thickness", data.m_optionalThicknessInMM);
    Serialize(envelope.DataNode(), "ConveyorSpeed", data.m_optionalConveyorSpeedInMMPerSecs);
    Serialize(envelope.DataNode(), "TopClearanceHeight", data.m_optionalTopClearanceHeightInMM);
    Serialize(envelope.DataNode(), "BottomClearanceHeight", data.m_optionalBottomClearanceHeightInMM);
    Serialize(envelope.DataNode(), "Weight", data.m_optionalWeightInGrams);
    return envelope.ToXmlString();
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, ServiceDescriptionData& data)
{
    Error error;
    Deserialize(xmlNode, "LaneId", error, data.m_laneId);
    Deserialize(xmlNode, "MachineId", error, data.m_machineId);
    Deserialize(xmlNode, "InterfaceId", error, data.m_optionalInterfaceId);
    Deserialize(xmlNode, "Version", error, data.m_version);
    Deserialize(xmlNode, "SupportedFeatures", error, data.m_supportedFeatures);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, BoardAvailableData& data)
{
    Error error;
    Deserialize(xmlNode, "BoardId", error, data.m_boardId);
    Deserialize(xmlNode, "BoardIdCreatedBy", error, data.m_boardIdCreatedBy);
    Deserialize(xmlNode, "FailedBoard", error, data.m_failedBoard);
    Deserialize(xmlNode, "ProductTypeId", error, data.m_optionalProductTypeId);
    Deserialize(xmlNode, "FlippedBoard", error, data.m_flippedBoard);
    Deserialize(xmlNode, "TopBarcode", error, data.m_optionalTopBarcode);
    Deserialize(xmlNode, "BottomBarcode", error, data.m_optionalBottomBarcode);
    Deserialize(xmlNode, "Length", error, data.m_optionalLengthInMM);
    Deserialize(xmlNode, "Width", error, data.m_optionalWidthInMM);
    Deserialize(xmlNode, "Thickness", error, data.m_optionalThicknessInMM);
    Deserialize(xmlNode, "ConveyorSpeed", error, data.m_optionalConveyorSpeedInMMPerSecs);
    Deserialize(xmlNode, "TopClearanceHeight", error, data.m_optionalTopClearanceHeightInMM);
    Deserialize(xmlNode, "BottomClearanceHeight", error, data.m_optionalBottomClearanceHeightInMM);
    Deserialize(xmlNode, "Weight", error, data.m_optionalWeightInGrams);
    Deserialize(xmlNode, "WorkOrderId", error, data.m_optionalWorkOrderId);
    Deserialize(xmlNode, "BatchId", error, data.m_optionalBatchId);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node, RevokeBoardAvailableData&)
{
    Error error;
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, MachineReadyData& data)
{
    Error error;
    Deserialize(xmlNode, "FailedBoard", error, data.m_failedBoard);
    Deserialize(xmlNode, "ForecastId", error, data.m_optionalForecastId);
    Deserialize(xmlNode, "BoardId", error, data.m_optionalBoardId);
    Deserialize(xmlNode, "ProductTypeId", error, data.m_optionalProductTypeId);
    Deserialize(xmlNode, "FlippedBoard", error, data.m_optionalFlippedBoard);
    Deserialize(xmlNode, "TopBarcode", error, data.m_optionalTopBarcode);
    Deserialize(xmlNode, "BottomBarcode", error, data.m_optionalBottomBarcode);
    Deserialize(xmlNode, "Length", error, data.m_optionalLengthInMM);
    Deserialize(xmlNode, "Width", error, data.m_optionalWidthInMM);
    Deserialize(xmlNode, "Thickness", error, data.m_optionalThicknessInMM);
    Deserialize(xmlNode, "ConveyorSpeed", error, data.m_optionalConveyorSpeedInMMPerSecs);
    Deserialize(xmlNode, "TopClearanceHeight", error, data.m_optionalTopClearanceHeightInMM);
    Deserialize(xmlNode, "BottomClearanceHeight", error, data.m_optionalBottomClearanceHeightInMM);
    Deserialize(xmlNode, "Weight", error, data.m_optionalWeightInGrams);
    Deserialize(xmlNode, "WorkOrderId", error, data.m_optionalWorkOrderId);
    Deserialize(xmlNode, "BatchId", error, data.m_optionalBatchId);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node, RevokeMachineReadyData&)
{
    Error error;
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, StartTransportData& data)
{
    Error error;
    Deserialize(xmlNode, "BoardId", error, data.m_boardId);
    Deserialize(xmlNode, "ConveyorSpeed", error, data.m_optionalConveyorSpeedInMMPerSecs);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, StopTransportData& data)
{
    Error error;
    Deserialize(xmlNode, "TransferState", error, data.m_transferState);
    Deserialize(xmlNode, "BoardId", error, data.m_boardId);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, TransportFinishedData& data)
{
    Error error;
    Deserialize(xmlNode, "TransferState", error, data.m_transferState);
    Deserialize(xmlNode, "BoardId", error, data.m_boardId);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, NotificationData& data)
{
    Error error;
    Deserialize(xmlNode, "NotificationCode", error, data.m_notificationCode);
    Deserialize(xmlNode, "Severity", error, data.m_severity);
    Deserialize(xmlNode, "Description", error, data.m_description);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, CheckAliveData& data)
{
    Error error;
    Deserialize(xmlNode, "Type", error, data.m_optionalType);
    Deserialize(xmlNode, "Id", error, data.m_optionalId);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, SetConfigurationData& data)
{
    Error error;
    Deserialize(xmlNode, "MachineId", error, data.m_machineId);
    Deserialize(xmlNode, "SupervisorySystemPort", error, data.m_optionalSupervisorySystemPort);
    Deserialize(xmlNode, "UpstreamConfigurations", error, data.m_upstreamConfigurations);
    Deserialize(xmlNode, "DownstreamConfigurations", error, data.m_downstreamConfigurations);
    return{};
}

Hermes::Error Hermes::Deserialize(pugi::xml_node, GetConfigurationData&)
{
    Error error;
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, CurrentConfigurationData& data)
{
    Error error;
    Deserialize(xmlNode, "MachineId", error, data.m_optionalMachineId);
    Deserialize(xmlNode, "SupervisorySystemPort", error, data.m_optionalSupervisorySystemPort);
    Deserialize(xmlNode, "UpstreamConfigurations", error, data.m_upstreamConfigurations);
    Deserialize(xmlNode, "DownstreamConfigurations", error, data.m_downstreamConfigurations);
    return{};
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, BoardForecastData& data)
{
    Error error;
    Deserialize(xmlNode, "ForecastId", error, data.m_optionalForecastId);
    Deserialize(xmlNode, "TimeUntilAvailable", error, data.m_optionalTimeUntilAvailableInSeconds);
    Deserialize(xmlNode, "BoardId", error, data.m_optionalBoardId);
    Deserialize(xmlNode, "BoardIdCreatedBy", error, data.m_optionalBoardIdCreatedBy);
    Deserialize(xmlNode, "FailedBoard", error, data.m_failedBoard);
    Deserialize(xmlNode, "ProductTypeId", error, data.m_optionalProductTypeId);
    Deserialize(xmlNode, "FlippedBoard", error, data.m_flippedBoard);
    Deserialize(xmlNode, "TopBarcode", error, data.m_optionalTopBarcode);
    Deserialize(xmlNode, "BottomBarcode", error, data.m_optionalBottomBarcode);
    Deserialize(xmlNode, "Length", error, data.m_optionalLengthInMM);
    Deserialize(xmlNode, "Width", error, data.m_optionalWidthInMM);
    Deserialize(xmlNode, "Thickness", error, data.m_optionalThicknessInMM);
    Deserialize(xmlNode, "ConveyorSpeed", error, data.m_optionalConveyorSpeedInMMPerSecs);
    Deserialize(xmlNode, "TopClearanceHeight", error, data.m_optionalTopClearanceHeightInMM);
    Deserialize(xmlNode, "BottomClearanceHeight", error, data.m_optionalBottomClearanceHeightInMM);
    Deserialize(xmlNode, "Weight", error, data.m_optionalWeightInGrams);
    Deserialize(xmlNode, "WorkOrderId", error, data.m_optionalWorkOrderId);
    Deserialize(xmlNode, "BatchId", error, data.m_optionalBatchId);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, QueryBoardInfoData& data)
{
    Error error;
    Deserialize(xmlNode, "TopBarcode", error, data.m_optionalTopBarcode);
    Deserialize(xmlNode, "BottomBarcode", error, data.m_optionalBottomBarcode);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, SendBoardInfoData& data)
{
    Error error;
    Deserialize(xmlNode, "BoardId", error, data.m_optionalBoardId);
    Deserialize(xmlNode, "BoardIdCreatedBy", error, data.m_optionalBoardIdCreatedBy);
    Deserialize(xmlNode, "FailedBoard", error, data.m_optionalFailedBoard);
    Deserialize(xmlNode, "ProductTypeId", error, data.m_optionalProductTypeId);
    Deserialize(xmlNode, "FlippedBoard", error, data.m_optionalFlippedBoard);
    Deserialize(xmlNode, "TopBarcode", error, data.m_optionalTopBarcode);
    Deserialize(xmlNode, "BottomBarcode", error, data.m_optionalBottomBarcode);
    Deserialize(xmlNode, "Length", error, data.m_optionalLengthInMM);
    Deserialize(xmlNode, "Width", error, data.m_optionalWidthInMM);
    Deserialize(xmlNode, "Thickness", error, data.m_optionalThicknessInMM);
    Deserialize(xmlNode, "ConveyorSpeed", error, data.m_optionalConveyorSpeedInMMPerSecs);
    Deserialize(xmlNode, "TopClearanceHeight", error, data.m_optionalTopClearanceHeightInMM);
    Deserialize(xmlNode, "BottomClearanceHeight", error, data.m_optionalBottomClearanceHeightInMM);
    Deserialize(xmlNode, "Weight", error, data.m_optionalWeightInGrams);
    Deserialize(xmlNode, "WorkOrderId", error, data.m_optionalWorkOrderId);
    Deserialize(xmlNode, "BatchId", error, data.m_optionalBatchId);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, SupervisoryServiceDescriptionData& data)
{
    Error error;
    Deserialize(xmlNode, "SystemId", error, data.m_systemId);
    Deserialize(xmlNode, "Version", error, data.m_version);
    Deserialize(xmlNode, "SupportedFeatures", error, data.m_supportedFeatures);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, BoardArrivedData& data)
{
    Error error;
    Deserialize(xmlNode, "MachineId", error, data.m_machineId);
    Deserialize(xmlNode, "UpstreamLaneId", error, data.m_upstreamLaneId);
    Deserialize(xmlNode, "UpstreamInterfaceId", error, data.m_optionalUpstreamInterfaceId);
    Deserialize(xmlNode, "MagazineId", error, data.m_optionalMagazineId);
    Deserialize(xmlNode, "SlotId", error, data.m_optionalSlotId);
    Deserialize(xmlNode, "BoardTransfer", error, data.m_boardTransfer);
    Deserialize(xmlNode, "BoardId", error, data.m_boardId);
    Deserialize(xmlNode, "BoardIdCreatedBy", error, data.m_boardIdCreatedBy);
    Deserialize(xmlNode, "FailedBoard", error, data.m_failedBoard);
    Deserialize(xmlNode, "ProductTypeId", error, data.m_optionalProductTypeId);
    Deserialize(xmlNode, "FlippedBoard", error, data.m_flippedBoard);
    Deserialize(xmlNode, "TopBarcode", error, data.m_optionalTopBarcode);
    Deserialize(xmlNode, "BottomBarcode", error, data.m_optionalBottomBarcode);
    Deserialize(xmlNode, "Length", error, data.m_optionalLengthInMM);
    Deserialize(xmlNode, "Width", error, data.m_optionalWidthInMM);
    Deserialize(xmlNode, "Thickness", error, data.m_optionalThicknessInMM);
    Deserialize(xmlNode, "ConveyorSpeed", error, data.m_optionalConveyorSpeedInMMPerSecs);
    Deserialize(xmlNode, "TopClearanceHeight", error, data.m_optionalTopClearanceHeightInMM);
    Deserialize(xmlNode, "BottomClearanceHeight", error, data.m_optionalBottomClearanceHeightInMM);
    Deserialize(xmlNode, "Weight", error, data.m_optionalWeightInGrams);
    Deserialize(xmlNode, "WorkOrderId", error, data.m_optionalWorkOrderId);
    Deserialize(xmlNode, "BatchId", error, data.m_optionalBatchId);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, BoardDepartedData& data)
{
    Error error;
    Deserialize(xmlNode, "MachineId", error, data.m_machineId);
    Deserialize(xmlNode, "DownstreamLaneId", error, data.m_downstreamLaneId);
    Deserialize(xmlNode, "DownstreamInterfaceId", error, data.m_optionalDownstreamInterfaceId);
    Deserialize(xmlNode, "MagazineId", error, data.m_optionalMagazineId);
    Deserialize(xmlNode, "SlotId", error, data.m_optionalSlotId);
    Deserialize(xmlNode, "BoardTransfer", error, data.m_boardTransfer);
    Deserialize(xmlNode, "BoardId", error, data.m_boardId);
    Deserialize(xmlNode, "BoardIdCreatedBy", error, data.m_boardIdCreatedBy);
    Deserialize(xmlNode, "FailedBoard", error, data.m_failedBoard);
    Deserialize(xmlNode, "ProductTypeId", error, data.m_optionalProductTypeId);
    Deserialize(xmlNode, "FlippedBoard", error, data.m_flippedBoard);
    Deserialize(xmlNode, "TopBarcode", error, data.m_optionalTopBarcode);
    Deserialize(xmlNode, "BottomBarcode", error, data.m_optionalBottomBarcode);
    Deserialize(xmlNode, "Length", error, data.m_optionalLengthInMM);
    Deserialize(xmlNode, "Width", error, data.m_optionalWidthInMM);
    Deserialize(xmlNode, "Thickness", error, data.m_optionalThicknessInMM);
    Deserialize(xmlNode, "ConveyorSpeed", error, data.m_optionalConveyorSpeedInMMPerSecs);
    Deserialize(xmlNode, "TopClearanceHeight", error, data.m_optionalTopClearanceHeightInMM);
    Deserialize(xmlNode, "BottomClearanceHeight", error, data.m_optionalBottomClearanceHeightInMM);
    Deserialize(xmlNode, "Weight", error, data.m_optionalWeightInGrams);
    Deserialize(xmlNode, "WorkOrderId", error, data.m_optionalWorkOrderId);
    Deserialize(xmlNode, "BatchId", error, data.m_optionalBatchId);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, QueryWorkOrderInfoData& data)
{
    Error error;
    Deserialize(xmlNode, "QueryId", error, data.m_optionalQueryId);
    Deserialize(xmlNode, "MachineId", error, data.m_machineId);
    Deserialize(xmlNode, "MagazineId", error, data.m_optionalMagazineId);
    Deserialize(xmlNode, "SlotId", error, data.m_optionalSlotId);
    Deserialize(xmlNode, "Barcode", error, data.m_optionalBarcode);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, SendWorkOrderInfoData& data)
{
    Error error;
    Deserialize(xmlNode, "QueryId", error, data.m_optionalQueryId);
    Deserialize(xmlNode, "WorkOrderId", error, data.m_optionalWorkOrderId);
    Deserialize(xmlNode, "BatchId", error, data.m_optionalBatchId);
    Deserialize(xmlNode, "BoardId", error, data.m_optionalBoardId);
    Deserialize(xmlNode, "BoardIdCreatedBy", error, data.m_optionalBoardIdCreatedBy);
    Deserialize(xmlNode, "FailedBoard", error, data.m_optionalFailedBoard);
    Deserialize(xmlNode, "ProductTypeId", error, data.m_optionalProductTypeId);
    Deserialize(xmlNode, "FlippedBoard", error, data.m_optionalFlippedBoard);
    Deserialize(xmlNode, "TopBarcode", error, data.m_optionalTopBarcode);
    Deserialize(xmlNode, "BottomBarcode", error, data.m_optionalBottomBarcode);
    Deserialize(xmlNode, "Length", error, data.m_optionalLengthInMM);
    Deserialize(xmlNode, "Width", error, data.m_optionalWidthInMM);
    Deserialize(xmlNode, "Thickness", error, data.m_optionalThicknessInMM);
    Deserialize(xmlNode, "ConveyorSpeed", error, data.m_optionalConveyorSpeedInMMPerSecs);
    Deserialize(xmlNode, "TopClearanceHeight", error, data.m_optionalTopClearanceHeightInMM);
    Deserialize(xmlNode, "BottomClearanceHeight", error, data.m_optionalBottomClearanceHeightInMM);
    Deserialize(xmlNode, "Weight", error, data.m_optionalWeightInGrams);
    return error;
}

