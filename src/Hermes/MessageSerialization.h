#pragma once

#include <HermesData.hpp>

#ifdef _WINDOWS
# include <pugixml/pugixml.hpp>
#else
# include <pugixml.hpp>
#endif
#include <string>


namespace Hermes
{
    template<class> struct SerializationTraits;

    template<> struct SerializationTraits<ServiceDescriptionData>
    {
        using Data = ServiceDescriptionData;

        static constexpr const char cTAG[] = "ServiceDescription";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<BoardAvailableData>
    {
        using Data = BoardAvailableData;

        static constexpr const char cTAG[] = "BoardAvailable";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<RevokeBoardAvailableData>
    {
        using Data = RevokeBoardAvailableData;

        static constexpr const char cTAG[] = "RevokeBoardAvailable";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<MachineReadyData>
    {
        using Data = MachineReadyData;

        static constexpr const char cTAG[] = "MachineReady";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<RevokeMachineReadyData>
    {
        using Data = RevokeMachineReadyData;

        static constexpr const char cTAG[] = "RevokeMachineReady";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<StartTransportData>
    {
        using Data = StartTransportData;

        static constexpr const char cTAG[] = "StartTransport";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<TransportFinishedData>
    {
        using Data = TransportFinishedData;

        static constexpr const char cTAG[] = "TransportFinished";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<StopTransportData>
    {
        using Data = StopTransportData;

        static constexpr const char cTAG[] = "StopTransport";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<NotificationData>
    {
        using Data = NotificationData;

        static constexpr const char cTAG[] = "Notification";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<CheckAliveData>
    {
        using Data = CheckAliveData;

        static constexpr const char cTAG[] = "CheckAlive";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<GetConfigurationData>
    {
        using Data = GetConfigurationData;

        static constexpr const char cTAG[] = "GetConfiguration";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<SetConfigurationData>
    {
        using Data = ServiceDescriptionData;

        static constexpr const char cTAG[] = "SetConfiguration";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<CurrentConfigurationData>
    {
        using Data = CurrentConfigurationData;

        static constexpr const char cTAG[] = "CurrentConfiguration";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<BoardForecastData>
    {
        using Data = CurrentConfigurationData;

        static constexpr const char cTAG[] = "BoardForecast";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<QueryBoardInfoData>
    {
        using Data = CurrentConfigurationData;

        static constexpr const char cTAG[] = "QueryBoardInfo";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<SendBoardInfoData>
    {
        using Data = CurrentConfigurationData;

        static constexpr const char cTAG[] = "SendBoardInfo";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };
    
    template<> struct SerializationTraits<SupervisoryServiceDescriptionData>
    {
        using Data = CurrentConfigurationData;

        static constexpr const char cTAG[] = "SupervisoryServiceDescription";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<BoardArrivedData>
    {
        using Data = CurrentConfigurationData;

        static constexpr const char cTAG[] = "BoardArrived";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<BoardDepartedData>
    {
        using Data = CurrentConfigurationData;

        static constexpr const char cTAG[] = "BoardDeparted";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<QueryWorkOrderInfoData>
    {
        using Data = CurrentConfigurationData;

        static constexpr const char cTAG[] = "QueryWorkOrderInfo";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };

    template<> struct SerializationTraits<SendWorkOrderInfoData>
    {
        using Data = CurrentConfigurationData;

        static constexpr const char cTAG[] = "SendWorkOrderInfo";
        static constexpr StringView cTAG_VIEW = StringView{cTAG, sizeof(cTAG) - 1U};
    };
    
    std::string Serialize(const ServiceDescriptionData&);
    std::string Serialize(const BoardAvailableData&);
    std::string Serialize(const RevokeBoardAvailableData&);
    std::string Serialize(const MachineReadyData&);
    std::string Serialize(const RevokeMachineReadyData&);
    std::string Serialize(const StartTransportData&);
    std::string Serialize(const TransportFinishedData&);
    std::string Serialize(const StopTransportData&);
    std::string Serialize(const NotificationData&);
    std::string Serialize(const CheckAliveData&);
    std::string Serialize(const GetConfigurationData&);
    std::string Serialize(const SetConfigurationData&);
    std::string Serialize(const CurrentConfigurationData&);
    std::string Serialize(const BoardForecastData&);
    std::string Serialize(const QueryBoardInfoData&);
    std::string Serialize(const SendBoardInfoData&);
    std::string Serialize(const SupervisoryServiceDescriptionData&);
    std::string Serialize(const BoardArrivedData&);
    std::string Serialize(const BoardDepartedData&);
    std::string Serialize(const QueryWorkOrderInfoData&);
    std::string Serialize(const SendWorkOrderInfoData&);

    Error Deserialize(pugi::xml_node, ServiceDescriptionData&);
    Error Deserialize(pugi::xml_node, BoardAvailableData&);
    Error Deserialize(pugi::xml_node, RevokeBoardAvailableData&);
    Error Deserialize(pugi::xml_node, MachineReadyData&);
    Error Deserialize(pugi::xml_node, RevokeMachineReadyData&);
    Error Deserialize(pugi::xml_node, StartTransportData&);
    Error Deserialize(pugi::xml_node, TransportFinishedData&);
    Error Deserialize(pugi::xml_node, StopTransportData&);
    Error Deserialize(pugi::xml_node, NotificationData&);
    Error Deserialize(pugi::xml_node, CheckAliveData&);
    Error Deserialize(pugi::xml_node, GetConfigurationData&);
    Error Deserialize(pugi::xml_node, SetConfigurationData&);
    Error Deserialize(pugi::xml_node, CurrentConfigurationData&);
    Error Deserialize(pugi::xml_node, BoardForecastData&);
    Error Deserialize(pugi::xml_node, QueryBoardInfoData&);
    Error Deserialize(pugi::xml_node, SendBoardInfoData&);
    Error Deserialize(pugi::xml_node, SupervisoryServiceDescriptionData&);
    Error Deserialize(pugi::xml_node, BoardArrivedData&);
    Error Deserialize(pugi::xml_node, BoardDepartedData&);
    Error Deserialize(pugi::xml_node, QueryWorkOrderInfoData&);
    Error Deserialize(pugi::xml_node, SendWorkOrderInfoData&);

}
