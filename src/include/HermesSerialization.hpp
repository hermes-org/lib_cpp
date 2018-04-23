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

#include "HermesSerialization.h"
#include "HermesDataConversion.hpp"


namespace Hermes
{

    template<class DataT, class SerializationFunctionT>
    std::string SerializeToXml_(const DataT& data, SerializationFunctionT serializationFunction)
    {
        auto apiData = ToC(data);
        std::string result;
        HermesSerializationCallback callback{[](void* pResult, HermesStringView stringView)
        {
            *static_cast<std::string*>(pResult) = ToCpp(stringView);
        }, &result};
        serializationFunction(&apiData, callback);
        return result;
    }

    inline std::string ToXml(const ServiceDescriptionData& data) { return SerializeToXml_(data, &::HermesSerializeServiceDescription); }
    inline std::string ToXml(const BoardAvailableData& data) { return SerializeToXml_(data, &::HermesSerializeBoardAvailable); }
    inline std::string ToXml(const RevokeBoardAvailableData& data) { return SerializeToXml_(data, &::HermesSerializeRevokeBoardAvailable); }
    inline std::string ToXml(const MachineReadyData& data) { return SerializeToXml_(data, &::HermesSerializeMachineReady); }
    inline std::string ToXml(const RevokeMachineReadyData& data) { return SerializeToXml_(data, &::HermesSerializeRevokeMachineReady); }
    inline std::string ToXml(const StartTransportData& data) { return SerializeToXml_(data, &::HermesSerializeStartTransport); }
    inline std::string ToXml(const StopTransportData& data) { return SerializeToXml_(data, &::HermesSerializeStopTransport); }
    inline std::string ToXml(const TransportFinishedData& data) { return SerializeToXml_(data, &::HermesSerializeTransportFinished); }
    inline std::string ToXml(const BoardForecastData& data) { return SerializeToXml_(data, &::HermesSerializeBoardForecast); }
    inline std::string ToXml(const QueryBoardInfoData& data) { return SerializeToXml_(data, &::HermesSerializeQueryBoardInfo); }
    inline std::string ToXml(const SendBoardInfoData& data) { return SerializeToXml_(data, &::HermesSerializeSendBoardInfo); }
    inline std::string ToXml(const NotificationData& data) { return SerializeToXml_(data, &::HermesSerializeNotification); }
    inline std::string ToXml(const CheckAliveData& data) { return SerializeToXml_(data, &::HermesSerializeCheckAlive); }
    inline std::string ToXml(const GetConfigurationData& data) { return SerializeToXml_(data, &::HermesSerializeGetConfiguration); }
    inline std::string ToXml(const SetConfigurationData& data) { return SerializeToXml_(data, &::HermesSerializeSetConfiguration); }
    inline std::string ToXml(const CurrentConfigurationData& data) { return SerializeToXml_(data, &::HermesSerializeCurrentConfiguration); }


    template<class DataT>
    void SetDeserializationCallback_(Optional<DataT>& optionalData, HermesDeserializationCallbacks& callbacks);

    template<>
    inline void SetDeserializationCallback_(Optional<ServiceDescriptionData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_serviceDescriptionCallback.m_pData = &optionalData;
        callbacks.m_serviceDescriptionCallback.m_pCall = [](void* pOptionalData, const HermesServiceDescriptionData* pData)
        {
            *static_cast<Optional<ServiceDescriptionData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<BoardAvailableData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_boardAvailableCallback.m_pData = &optionalData;
        callbacks.m_boardAvailableCallback.m_pCall = [](void* pOptionalData, const HermesBoardAvailableData* pData)
        {
            *static_cast<Optional<BoardAvailableData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<RevokeBoardAvailableData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_revokeBoardAvailableCallback.m_pData = &optionalData;
        callbacks.m_revokeBoardAvailableCallback.m_pCall = [](void* pOptionalData, const HermesRevokeBoardAvailableData* pData)
        {
            *static_cast<Optional<RevokeBoardAvailableData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<MachineReadyData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_machineReadyCallback.m_pData = &optionalData;
        callbacks.m_machineReadyCallback.m_pCall = [](void* pOptionalData, const HermesMachineReadyData* pData)
        {
            *static_cast<Optional<MachineReadyData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<RevokeMachineReadyData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_revokeMachineReadyCallback.m_pData = &optionalData;
        callbacks.m_revokeMachineReadyCallback.m_pCall = [](void* pOptionalData, const HermesRevokeMachineReadyData* pData)
        {
            *static_cast<Optional<RevokeMachineReadyData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<StartTransportData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_startTransportCallback.m_pData = &optionalData;
        callbacks.m_startTransportCallback.m_pCall = [](void* pOptionalData, const HermesStartTransportData* pData)
        {
            *static_cast<Optional<StartTransportData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<StopTransportData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_stopTransportCallback.m_pData = &optionalData;
        callbacks.m_stopTransportCallback.m_pCall = [](void* pOptionalData, const HermesStopTransportData* pData)
        {
            *static_cast<Optional<StopTransportData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<TransportFinishedData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_transportFinishedCallback.m_pData = &optionalData;
        callbacks.m_transportFinishedCallback.m_pCall = [](void* pOptionalData, const HermesTransportFinishedData* pData)
        {
            *static_cast<Optional<TransportFinishedData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<BoardForecastData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_boardForecastCallback.m_pData = &optionalData;
        callbacks.m_boardForecastCallback.m_pCall = [](void* pOptionalData, const HermesBoardForecastData* pData)
        {
            *static_cast<Optional<BoardForecastData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<QueryBoardInfoData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_queryBoardInfoCallback.m_pData = &optionalData;
        callbacks.m_queryBoardInfoCallback.m_pCall = [](void* pOptionalData, const HermesQueryBoardInfoData* pData)
        {
            *static_cast<Optional<QueryBoardInfoData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<SendBoardInfoData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_sendBoardInfoCallback.m_pData = &optionalData;
        callbacks.m_sendBoardInfoCallback.m_pCall = [](void* pOptionalData, const HermesSendBoardInfoData* pData)
        {
            *static_cast<Optional<SendBoardInfoData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<NotificationData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_notificationCallback.m_pData = &optionalData;
        callbacks.m_notificationCallback.m_pCall = [](void* pOptionalData, const HermesNotificationData* pData)
        {
            *static_cast<Optional<NotificationData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<CheckAliveData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_checkAliveCallback.m_pData = &optionalData;
        callbacks.m_checkAliveCallback.m_pCall = [](void* pOptionalData, const HermesCheckAliveData* pData)
        {
            *static_cast<Optional<CheckAliveData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<GetConfigurationData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_getConfigurationCallback.m_pData = &optionalData;
        callbacks.m_getConfigurationCallback.m_pCall = [](void* pOptionalData, const HermesGetConfigurationData* pData)
        {
            *static_cast<Optional<GetConfigurationData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<SetConfigurationData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_setConfigurationCallback.m_pData = &optionalData;
        callbacks.m_setConfigurationCallback.m_pCall = [](void* pOptionalData, const HermesSetConfigurationData* pData)
        {
            *static_cast<Optional<SetConfigurationData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }
    template<>
    inline void SetDeserializationCallback_(Optional<CurrentConfigurationData>& optionalData, HermesDeserializationCallbacks& callbacks)
    {
        callbacks.m_currentConfigurationCallback.m_pData = &optionalData;
        callbacks.m_currentConfigurationCallback.m_pCall = [](void* pOptionalData, const HermesCurrentConfigurationData* pData)
        {
            *static_cast<Optional<CurrentConfigurationData>*>(pOptionalData) = Hermes::ToCpp(*pData);
        };
    }

    template<class DataT>
    Optional<DataT> FromXml(StringView xml)
    {
        Optional<DataT> optionalData; 
        HermesDeserializationCallbacks callbacks{};
        SetDeserializationCallback_(optionalData, callbacks);
        ::HermesDeserialize(ToC(xml), &callbacks);
        return optionalData;
    }

}

