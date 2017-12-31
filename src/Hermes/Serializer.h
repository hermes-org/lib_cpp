#pragma once

#include <HermesDataConversion.hpp>

#ifdef _WINDOWS
 #include "pugixml/pugixml.hpp"
#else
 #include "pugixml.hpp"
#endif

#include <boost/variant.hpp>

#include <memory>

#ifdef _WINDOWS
# pragma warning( push )  
# pragma warning( disable : 4251 )  
#endif

namespace Hermes
{
    std::string Serialize(const ServiceDescription&);
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

    bool TakeMessage(std::string& out_message, std::string& inout_buffer);

    using VariantData = boost::variant<boost::blank, Error,
        ServiceDescription,
        BoardAvailableData,
        RevokeBoardAvailableData,
        MachineReadyData,
        RevokeMachineReadyData,
        StartTransportData,
        TransportFinishedData,
        StopTransportData,
        NotificationData,
        CheckAliveData,
        GetConfigurationData,
        SetConfigurationData,
        CurrentConfigurationData>;
    
    VariantData Deserialize(StringView message);

}

#ifdef _WINDOWS
# pragma warning( pop )  
#endif
