#include "stdafx.h"

#include "MessageSerialization.h"

#include "BasicPugiSerialization.h"
#include "SenderEnvelope.h"

#include <chrono>
#include <time.h>
#include <iomanip>

namespace Hermes
{
    template<> struct PugiSerializer<SubBoard>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node parent, const SubBoard& data)
        {
            Serialize(parent, "Pos", data.m_pos);
            Serialize(parent, "Bc", data.m_optionalBc);
            Serialize(parent, "St", data.m_st);
        }

        static void ReadElement(pugi::xml_node parent, Error& error, SubBoard& data)
        {
            Deserialize(parent, "Pos", error, data.m_pos);
            Deserialize(parent, "Bc", error, data.m_optionalBc);
            Deserialize(parent, "St", error, data.m_st);
        }
    };

    // SubBoards are special in that they are optional vectors
    void Serialize(pugi::xml_node parent, const char* name, const SubBoards& subBoards)
    {
        if (subBoards.empty())
            return;

        auto subBoardsNode = parent.append_child(name);

        for (const auto& item : subBoards)
        {
            Serialize(subBoardsNode, "SB", item);
        }
    }

    void Deserialize(pugi::xml_node parent, const char* name, Error& error, SubBoards& subBoards)
    {
        const auto subBoardsNode = parent.child(name);
        if (!subBoardsNode)
            return;

        for (const auto& node : subBoardsNode.children("SB"))
        {
            subBoards.emplace_back();
            PugiSerializer<SubBoard>::ReadElement(node, error, subBoards.back());
        }
    }

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

    template<> struct PugiSerializer<FeatureCommand>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureCommand&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureCommand&) {}
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
            Serialize(parent, "FeatureCommand", data.m_optionalFeatureCommand);
        }

        static void ReadElement(pugi::xml_node parent, Error& error, SupportedFeatures& data)
        {
            Deserialize(parent, "FeatureBoardForecast", error, data.m_optionalFeatureBoardForecast);
            Deserialize(parent, "FeatureCheckAliveResponse", error, data.m_optionalFeatureCheckAliveResponse);
            Deserialize(parent, "FeatureQueryBoardInfo", error, data.m_optionalFeatureQueryBoardInfo);
            Deserialize(parent, "FeatureSendBoardInfo", error, data.m_optionalFeatureSendBoardInfo);
            Deserialize(parent, "FeatureCommand", error, data.m_optionalFeatureCommand);
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

    template<> struct PugiSerializer<FeatureReplyWorkOrderInfo>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureReplyWorkOrderInfo&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureReplyWorkOrderInfo&) {}
    };

    template<> struct PugiSerializer<FeatureQueryHermesCapabilities>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureQueryHermesCapabilities&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureQueryHermesCapabilities&) {}
    };

    template<> struct PugiSerializer<FeatureSendHermesCapabilities>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const FeatureSendHermesCapabilities&) {}
        static void ReadElement(pugi::xml_node, Error&, FeatureSendHermesCapabilities&) {}
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
            Serialize(parent, "FeatureReplyWorkOrderInfo", data.m_optionalFeatureReplyWorkOrderInfo);
            Serialize(parent, "FeatureQueryHermesCapabilities", data.m_optionalFeatureQueryHermesCapabilities);
            Serialize(parent, "FeatureSendHermesCapabilities", data.m_optionalFeatureSendHermesCapabilities);
        }

        static void ReadElement(pugi::xml_node parent, Error& error, SupervisoryFeatures& data)
        {
            Deserialize(parent, "FeatureConfiguration", error, data.m_optionalFeatureConfiguration);
            Deserialize(parent, "FeatureCheckAliveResponse", error, data.m_optionalFeatureCheckAliveResponse);
            Deserialize(parent, "FeatureBoardTracking", error, data.m_optionalFeatureBoardTracking);
            Deserialize(parent, "FeatureQueryWorkOrderInfo", error, data.m_optionalFeatureQueryWorkOrderInfo);
            Deserialize(parent, "FeatureSendWorkOrderInfo", error, data.m_optionalFeatureSendWorkOrderInfo);
            Deserialize(parent, "FeatureReplyWorkOrderInfo", error, data.m_optionalFeatureReplyWorkOrderInfo);
            Deserialize(parent, "FeatureQueryHermesCapabilities", error, data.m_optionalFeatureQueryHermesCapabilities);
            Deserialize(parent, "FeatureSendHermesCapabilities", error, data.m_optionalFeatureSendHermesCapabilities);
        }
    };

    template<> struct PugiSerializer<MessageCheckAliveResponse>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const MessageCheckAliveResponse&) {}
        static void ReadElement(pugi::xml_node, Error&, MessageCheckAliveResponse&) {}
    };

    template<> struct PugiSerializer<MessageBoardForecast>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const MessageBoardForecast&) {}
        static void ReadElement(pugi::xml_node, Error&, MessageBoardForecast&) {}
    };

    template<> struct PugiSerializer<MessageQueryBoardInfo>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const MessageQueryBoardInfo&) {}
        static void ReadElement(pugi::xml_node, Error&, MessageQueryBoardInfo&) {}
    };

    template<> struct PugiSerializer<MessageSendBoardInfo>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const MessageSendBoardInfo&) {}
        static void ReadElement(pugi::xml_node, Error&, MessageSendBoardInfo&) {}
    };
    template<> struct PugiSerializer<MessageBoardArrived>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const MessageBoardArrived&) {}
        static void ReadElement(pugi::xml_node, Error&, MessageBoardArrived&) {}
    };

    template<> struct PugiSerializer<MessageBoardDeparted>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const MessageBoardDeparted&) {}
        static void ReadElement(pugi::xml_node, Error&, MessageBoardDeparted&) {}
    };

    template<> struct PugiSerializer<MessageQueryWorkOrderInfo>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const MessageQueryWorkOrderInfo&) {}
        static void ReadElement(pugi::xml_node, Error&, MessageQueryWorkOrderInfo&) {}
    };

    template<> struct PugiSerializer<MessageReplyWorkOrderInfo>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const MessageReplyWorkOrderInfo&) {}
        static void ReadElement(pugi::xml_node, Error&, MessageReplyWorkOrderInfo&) {}
    };

    template<> struct PugiSerializer<MessageCommand>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node, const MessageCommand&) {}
        static void ReadElement(pugi::xml_node, Error&, MessageCommand&) {}
    };

    template<> struct PugiSerializer<OptionalMessages>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node parent, const OptionalMessages& data)
        {
            Serialize(parent, "MessageCheckAliveResponse", data.m_optionalMessageCheckAliveResponse);
            Serialize(parent, "MessageBoardForecast", data.m_optionalMessageBoardForecast);
            Serialize(parent, "MessageQueryBoardInfo", data.m_optionalMessageQueryBoardInfo);
            Serialize(parent, "MessageSendBoardInfo", data.m_optionalMessageSendBoardInfo);
            Serialize(parent, "MessageBoardArrived", data.m_optionalMessageBoardArrived);
            Serialize(parent, "MessageBoardDeparted", data.m_optionalMessageBoardDeparted);
            Serialize(parent, "MessageQueryWorkOrderInfo", data.m_optionalMessageQueryWorkOrderInfo);
            Serialize(parent, "MessageReplyWorkOrderInfo", data.m_optionalMessageReplyWorkOrderInfo);
            Serialize(parent, "MessageCommand", data.m_optionalMessageCommand);
        }

        static void ReadElement(pugi::xml_node parent, Error& error, OptionalMessages& data)
        {
            Deserialize(parent, "MessageCheckAliveResponse", error, data.m_optionalMessageCheckAliveResponse);
            Deserialize(parent, "MessageBoardForecast", error, data.m_optionalMessageBoardForecast);
            Deserialize(parent, "MessageQueryBoardInfo", error, data.m_optionalMessageQueryBoardInfo);
            Deserialize(parent, "MessageSendBoardInfo", error, data.m_optionalMessageSendBoardInfo);
            Deserialize(parent, "MessageBoardArrived", error, data.m_optionalMessageBoardArrived);
            Deserialize(parent, "MessageBoardDeparted", error, data.m_optionalMessageBoardDeparted);
            Deserialize(parent, "MessageQueryWorkOrderInfo", error, data.m_optionalMessageQueryWorkOrderInfo);
            Deserialize(parent, "MessageReplyWorkOrderInfo", error, data.m_optionalMessageReplyWorkOrderInfo);
            Deserialize(parent, "MessageCommand", error, data.m_optionalMessageCommand);
        }
    };

    template<> struct PugiSerializer<Attributes>
    {
        using AsElementTag = int;

        static void WriteElement(pugi::xml_node parent, const Attributes& data)
        {
            Serialize(parent, "ProductTypeId", data.m_productTypeId);
            Serialize(parent, "TopBarcode", data.m_topBarcode);
            Serialize(parent, "BottomBarcode", data.m_bottomBarcode);
            Serialize(parent, "Length", data.m_length);
            Serialize(parent, "Width", data.m_width);
            Serialize(parent, "Thickness", data.m_thickness);
            Serialize(parent, "ConveyorSpeed", data.m_conveyorSpeed);
            Serialize(parent, "TopClearanceHeight", data.m_topClearanceHeight);
            Serialize(parent, "BottomClearanceHeight", data.m_bottomClearanceHeight);
            Serialize(parent, "Weight", data.m_weight);
            Serialize(parent, "WorkOrderId", data.m_workOrderId);
            Serialize(parent, "BatchId", data.m_batchId);
            Serialize(parent, "Route", data.m_route);
            Serialize(parent, "Action", data.m_action);
            Serialize(parent, "SubBoards", data.m_subBoards);
        }

        static void ReadElement(pugi::xml_node parent, Error& error, Attributes& data)
        {
            Deserialize(parent, "ProductTypeId", error, data.m_productTypeId);
            Deserialize(parent, "TopBarcode", error, data.m_topBarcode);
            Deserialize(parent, "BottomBarcode", error, data.m_bottomBarcode);
            Deserialize(parent, "Length", error, data.m_length);
            Deserialize(parent, "Width", error, data.m_width);
            Deserialize(parent, "Thickness", error, data.m_thickness);
            Deserialize(parent, "ConveyorSpeed", error, data.m_conveyorSpeed);
            Deserialize(parent, "TopClearanceHeight", error, data.m_topClearanceHeight);
            Deserialize(parent, "BottomClearanceHeight", error, data.m_bottomClearanceHeight);
            Deserialize(parent, "Weight", error, data.m_weight);
            Deserialize(parent, "WorkOrderId", error, data.m_workOrderId);
            Deserialize(parent, "BatchId", error, data.m_batchId);
            Deserialize(parent, "Route", error, data.m_route);
            Deserialize(parent, "Action", error, data.m_action);
            Deserialize(parent, "SubBoards", error, data.m_subBoards);
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


std::string Serialize_(const Hermes::BoardAvailableData& data, bool includeSubBoards)
{
    using namespace Hermes;
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
    Serialize(envelope.DataNode(), "Route", data.m_optionalRoute);
    Serialize(envelope.DataNode(), "Action", data.m_optionalAction);
    if (includeSubBoards)
    {
        Serialize(envelope.DataNode(), "SubBoards", data.m_optionalSubBoards);
    }
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const BoardAvailableData& data)
{
    auto str{ Serialize_(data, true) };
    if (str.size() <= cMAX_MESSAGE_SIZE)
        return str;

    return Serialize_(data, false);
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

std::string Serialize_(const Hermes::SendBoardInfoData& data, bool includeSubBoards)
{
    using namespace Hermes;

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
    Serialize(envelope.DataNode(), "Route", data.m_optionalRoute);
    Serialize(envelope.DataNode(), "Action", data.m_optionalAction);
    if (includeSubBoards)
    {
        Serialize(envelope.DataNode(), "SubBoards", data.m_optionalSubBoards);
    }
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const SendBoardInfoData& data)
{
    auto str{ Serialize_(data, true) };
    if (str.size() <= cMAX_MESSAGE_SIZE)
        return str;

    return Serialize_(data, false);
}

std::string Hermes::Serialize(const SupervisoryServiceDescriptionData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<SupervisoryServiceDescriptionData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "SystemId", data.m_systemId);
    Serialize(envelope.DataNode(), "Version", data.m_version);
    Serialize(envelope.DataNode(), "SupportedFeatures", data.m_supportedFeatures);
    return envelope.ToXmlString();
}

std::string Serialize_(const Hermes::BoardArrivedData& data, bool includeSubBoards)
{
    using namespace Hermes;
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
    Serialize(envelope.DataNode(), "Route", data.m_optionalRoute);
    Serialize(envelope.DataNode(), "Action", data.m_optionalAction);
    if (includeSubBoards)
    {
        Serialize(envelope.DataNode(), "SubBoards", data.m_optionalSubBoards);
    }
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const BoardArrivedData& data)
{
    auto str{ Serialize_(data, true) };
    if (str.size() <= cMAX_MESSAGE_SIZE)
        return str;

    return Serialize_(data, false);
}

std::string Serialize_(const Hermes::BoardDepartedData& data, bool includeSubBoards)
{
    using namespace Hermes;
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
    Serialize(envelope.DataNode(), "Route", data.m_optionalRoute);
    Serialize(envelope.DataNode(), "Action", data.m_optionalAction);
    if (includeSubBoards)
    {
        Serialize(envelope.DataNode(), "SubBoards", data.m_optionalSubBoards);
    }
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const BoardDepartedData& data)
{
    auto str{ Serialize_(data, true) };
    if (str.size() <= cMAX_MESSAGE_SIZE)
        return str;

    return Serialize_(data, false);
}

std::string Hermes::Serialize(const QueryWorkOrderInfoData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<QueryWorkOrderInfoData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "QueryId", data.m_optionalQueryId);
    Serialize(envelope.DataNode(), "MachineId", data.m_machineId);
    Serialize(envelope.DataNode(), "MagazineId", data.m_optionalMagazineId);
    Serialize(envelope.DataNode(), "SlotId", data.m_optionalSlotId);
    Serialize(envelope.DataNode(), "Barcode", data.m_optionalBarcode);
    Serialize(envelope.DataNode(), "WorkOrderId", data.m_optionalWorkOrderId);
    Serialize(envelope.DataNode(), "BatchId", data.m_optionalBatchId);
    return envelope.ToXmlString();
}

std::string Serialize_(const Hermes::SendWorkOrderInfoData& data, bool includeSubBoards)
{
    using namespace Hermes;
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
    Serialize(envelope.DataNode(), "Route", data.m_optionalRoute);
    if (includeSubBoards)
    {
        Serialize(envelope.DataNode(), "SubBoards", data.m_optionalSubBoards);
    }
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const SendWorkOrderInfoData& data)
{
    auto str{ Serialize_(data, true) };
    if (str.size() <= cMAX_MESSAGE_SIZE)
        return str;

    return Serialize_(data, false);
}

std::string Hermes::Serialize(const ReplyWorkOrderInfoData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<ReplyWorkOrderInfoData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "WorkOrderId", data.m_workOrderId);
    Serialize(envelope.DataNode(), "BatchId", data.m_optionalBatchId);
    Serialize(envelope.DataNode(), "Status", data.m_status);
    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const QueryHermesCapabilitiesData&)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<QueryHermesCapabilitiesData>::cTAG_VIEW);

    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const SendHermesCapabilitiesData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<SendHermesCapabilitiesData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "OptionalMessages", data.m_optionalMessages);
    Serialize(envelope.DataNode(), "Attributes", data.m_attributes);

    return envelope.ToXmlString();
}

std::string Hermes::Serialize(const CommandData& data)
{
    SenderEnvelope envelope(Hermes::SerializationTraits<CommandData>::cTAG_VIEW);
    Serialize(envelope.DataNode(), "Command", data.m_command);
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
    Deserialize(xmlNode, "Route", error, data.m_optionalRoute);
    Deserialize(xmlNode, "Action", error, data.m_optionalAction);
    Deserialize(xmlNode, "SubBoards", error, data.m_optionalSubBoards);
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
    Deserialize(xmlNode, "Route", error, data.m_optionalRoute);
    Deserialize(xmlNode, "Action", error, data.m_optionalAction);
    Deserialize(xmlNode, "SubBoards", error, data.m_optionalSubBoards);
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
    Deserialize(xmlNode, "Route", error, data.m_optionalRoute);
    Deserialize(xmlNode, "Action", error, data.m_optionalAction);
    Deserialize(xmlNode, "SubBoards", error, data.m_optionalSubBoards);
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
    Deserialize(xmlNode, "Route", error, data.m_optionalRoute);
    Deserialize(xmlNode, "Action", error, data.m_optionalAction);
    Deserialize(xmlNode, "SubBoards", error, data.m_optionalSubBoards);
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
    Deserialize(xmlNode, "WorkOrderId", error, data.m_optionalWorkOrderId);
    Deserialize(xmlNode, "BatchId", error, data.m_optionalBatchId);
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
    Deserialize(xmlNode, "Route", error, data.m_optionalRoute);
    Deserialize(xmlNode, "SubBoards", error, data.m_optionalSubBoards);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, ReplyWorkOrderInfoData& data)
{
    Error error;
    Deserialize(xmlNode, "WorkOrderId", error, data.m_workOrderId);
    Deserialize(xmlNode, "BatchId", error, data.m_optionalBatchId);
    Deserialize(xmlNode, "Status", error, data.m_status);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, CommandData& data)
{
    Error error;
    Deserialize(xmlNode, "Command", error, data.m_command);
    return error;
}

Hermes::Error Hermes::Deserialize(pugi::xml_node /*xmlNode*/, QueryHermesCapabilitiesData& /*data*/)
{
    return { EErrorCode::eSUCCESS, "" };
}

Hermes::Error Hermes::Deserialize(pugi::xml_node xmlNode, SendHermesCapabilitiesData& data)
{
    Error error;
    Deserialize(xmlNode, "OptionalMessages", error, data.m_optionalMessages);
    Deserialize(xmlNode, "Attributes", error, data.m_attributes);
    return error;
}
