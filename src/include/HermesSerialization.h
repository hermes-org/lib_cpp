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

// Copyright (c) ASM Assembly Systems GmbH & Co. KG
//
// C interface for (de)serializing The Hermes Standard messages
//
#ifndef HERMESSERIALIZATION_H
#define HERMESSERIALIZATION_H

#include "Hermes.h"

#ifdef __cplusplus
extern "C" {
#endif

    // Serialize (just for unit testing)
    struct HermesSerializationCallback
    {
        void(*m_pCall)(void* /*m_pData*/, HermesStringView);
        void* m_pData;
    };
    HERMESPROTOCOL_API void HermesSerializeServiceDescription(const HermesServiceDescriptionData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeBoardAvailable(const HermesBoardAvailableData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeRevokeBoardAvailable(const HermesRevokeBoardAvailableData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeMachineReady(const HermesMachineReadyData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeRevokeMachineReady(const HermesRevokeMachineReadyData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeStartTransport(const HermesStartTransportData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeStopTransport(const HermesStopTransportData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeTransportFinished(const HermesTransportFinishedData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeBoardForecast(const HermesBoardForecastData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeQueryBoardInfo(const HermesQueryBoardInfoData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeSendBoardInfo(const HermesSendBoardInfoData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeNotification(const HermesNotificationData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeCheckAlive(const HermesCheckAliveData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeGetConfiguration(const HermesGetConfigurationData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeSetConfiguration(const HermesSetConfigurationData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeCurrentConfiguration(const HermesCurrentConfigurationData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeSupervisoryServiceDescription(const HermesSupervisoryServiceDescriptionData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeBoardArrived(const HermesBoardArrivedData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeBoardDeparted(const HermesBoardDepartedData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeQueryWorkOrderInfo(const HermesQueryWorkOrderInfoData*, HermesSerializationCallback);
    HERMESPROTOCOL_API void HermesSerializeSendWorkOrderInfo(const HermesSendWorkOrderInfoData*, HermesSerializationCallback);

    // Deserialize (just for unit testing)
    struct HermesDeserializationErrorCallback
    {
        void(*m_pCall)(void*, const HermesError*);
        void* m_pData;
    };
    struct HermesDeserializedServiceDescriptionCallback
    {
        void(*m_pCall)(void*, const HermesServiceDescriptionData*);
        void* m_pData;
    };
    struct HermesDeserializedBoardAvailableCallback
    {
        void(*m_pCall)(void*, const HermesBoardAvailableData*);
        void* m_pData;
    };
    struct HermesDeserializedRevokeBoardAvailableCallback
    {
        void(*m_pCall)(void*, const HermesRevokeBoardAvailableData*);
        void* m_pData;
    };
    struct HermesDeserializedMachineReadyCallback
    {
        void(*m_pCall)(void*, const HermesMachineReadyData*);
        void* m_pData;
    };
    struct HermesDeserializedRevokeMachineReadyCallback
    {
        void(*m_pCall)(void*, const HermesRevokeMachineReadyData*);
        void* m_pData;
    };
    struct HermesDeserializedStartTransportCallback
    {
        void(*m_pCall)(void*, const HermesStartTransportData*);
        void* m_pData;
    };
    struct HermesDeserializedStopTransportCallback
    {
        void(*m_pCall)(void*, const HermesStopTransportData*);
        void* m_pData;
    };
    struct HermesDeserializedTransportFinishedCallback
    {
        void(*m_pCall)(void*, const HermesTransportFinishedData*);
        void* m_pData;
    };
    struct HermesDeserializedBoardForecastCallback
    {
        void(*m_pCall)(void*, const HermesBoardForecastData*);
        void* m_pData;
    };
    struct HermesDeserializedQueryBoardInfoCallback
    {
        void(*m_pCall)(void*, const HermesQueryBoardInfoData*);
        void* m_pData;
    };
    struct HermesDeserializedSendBoardInfoCallback
    {
        void(*m_pCall)(void*, const HermesSendBoardInfoData*);
        void* m_pData;
    };
    struct HermesDeserializedNotificationCallback
    {
        void(*m_pCall)(void*, const HermesNotificationData*);
        void* m_pData;
    };
    struct HermesDeserializedCheckAliveCallback
    {
        void(*m_pCall)(void*, const HermesCheckAliveData*);
        void* m_pData;
    };
    struct HermesDeserializedGetConfigurationCallback
    {
        void(*m_pCall)(void*, const HermesGetConfigurationData*);
        void* m_pData;
    };
    struct HermesDeserializedSetConfigurationCallback
    {
        void(*m_pCall)(void*, const HermesSetConfigurationData*);
        void* m_pData;
    };
    struct HermesDeserializedCurrentConfigurationCallback
    {
        void(*m_pCall)(void*, const HermesCurrentConfigurationData*);
        void* m_pData;
    };
    struct HermesDeserializedSupervisoryServiceDescriptionCallback
    {
        void(*m_pCall)(void*, const HermesSupervisoryServiceDescriptionData*);
        void* m_pData;
    };
    struct HermesDeserializedBoardArrivedCallback
    {
        void(*m_pCall)(void*, const HermesBoardArrivedData*);
        void* m_pData;
    };
    struct HermesDeserializedBoardDepartedCallback
    {
        void(*m_pCall)(void*, const HermesBoardDepartedData*);
        void* m_pData;
    };
    struct HermesDeserializedQueryWorkOrderInfoCallback
    {
        void(*m_pCall)(void*, const HermesQueryWorkOrderInfoData*);
        void* m_pData;
    };
    struct HermesDeserializedSendWorkOrderInfoCallback
    {
        void(*m_pCall)(void*, const HermesSendWorkOrderInfoData*);
        void* m_pData;
    };

    struct HermesDeserializationCallbacks
    {
        HermesDeserializationErrorCallback m_deserializationErrorCallback;
        HermesDeserializedServiceDescriptionCallback m_serviceDescriptionCallback;
        HermesDeserializedBoardAvailableCallback m_boardAvailableCallback;
        HermesDeserializedRevokeBoardAvailableCallback m_revokeBoardAvailableCallback;
        HermesDeserializedMachineReadyCallback m_machineReadyCallback;
        HermesDeserializedRevokeMachineReadyCallback m_revokeMachineReadyCallback;
        HermesDeserializedStartTransportCallback m_startTransportCallback;
        HermesDeserializedStopTransportCallback m_stopTransportCallback;
        HermesDeserializedTransportFinishedCallback m_transportFinishedCallback;
        HermesDeserializedBoardForecastCallback m_boardForecastCallback;
        HermesDeserializedQueryBoardInfoCallback m_queryBoardInfoCallback;
        HermesDeserializedSendBoardInfoCallback m_sendBoardInfoCallback;
        HermesDeserializedNotificationCallback m_notificationCallback;
        HermesDeserializedCheckAliveCallback m_checkAliveCallback;
        HermesDeserializedGetConfigurationCallback m_getConfigurationCallback;
        HermesDeserializedSetConfigurationCallback m_setConfigurationCallback;
        HermesDeserializedCurrentConfigurationCallback m_currentConfigurationCallback;
        HermesDeserializedSupervisoryServiceDescriptionCallback m_supervisoryServiceDescriptionCallback;
        HermesDeserializedBoardArrivedCallback m_boardArrivedCallback;
        HermesDeserializedBoardDepartedCallback m_boardDepartedCallback;
        HermesDeserializedQueryWorkOrderInfoCallback m_queryWorkOrderInfoCallback;
        HermesDeserializedSendWorkOrderInfoCallback m_sendWorkOrderInfoCallback;

    };
    HERMESPROTOCOL_API void HermesDeserialize(HermesStringView, const HermesDeserializationCallbacks*);

#ifdef __cplusplus
}
#endif

#endif // HERMESSERIALIZATION_H


