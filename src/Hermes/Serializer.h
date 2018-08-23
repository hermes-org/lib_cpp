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

#pragma once

#include <HermesDataConversion.hpp>

#include "pugixml/pugixml.hpp"

#include <boost/variant.hpp>

#include <memory>

#pragma warning( push )  
#pragma warning( disable : 4251 )  

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

#pragma warning( pop )  