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
// C interface for an implementation of The Hermes Standard
//
#ifndef HERMES_H
#define HERMES_H

#ifdef _WINDOWS
# ifdef HERMES_EXPORTS
#  define  HERMESPROTOCOL_API  __declspec(dllexport) 
# else
#  define  HERMESPROTOCOL_API  __declspec(dllimport)
# endif // HERMES_EXPORTS
#else
# define HERMESPROTOCOL_API
#endif //_WINDOWS

#define HERMES_VERSION "1.6"

#include "HermesData.h"

#ifdef __cplusplus
extern "C" {
#endif

    // The header is organized to follow the documentation of The Hermes Standard 
    // as closely as possible: https://www.the-hermes-standard.info/wp-content/uploads/pdf/The_Hermes_Standard_Version_1.0.pdf
    // It maintains the order and the naming as far
    // as the C syntax and the coding guidelines allow it.

    // base port number, The Hermes Standard 2.1
    static const uint16_t cHERMES_BASE_PORT = 50100U;

    // configuration port number, The Hermes Standard 2.2
    static const uint16_t cHERMES_CONFIG_PORT = 1248U;

    // maximum message size, The Hermes Standard 3.1
    static const unsigned cHERMES_MAX_MESSAGE_SIZE = 65536U;

    // other data is specified in HermesData.h, which is separate since it is generated

    // What follows are the callbacks containing the various messages listed above.
    // This is more about API design and has no direct correspondence in The Hermes Standard.
    // Here, we introduce the notion of a session, corresponding to an established socket connection between upstream and downstream.
    // After each disconnect, the session ID is incremented (it starts with 1).
    // As a general principle, the client passes in a void pointer, with which it is called back as first parameter.
    // This provides him with additional information for dispatching the callback.
    // Please look at the C++ wrapper for a demonstration of this pattern!
    struct HermesNotificationCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesNotificationData*);
        void* m_pData;
    };

    // The remote host has connected
    struct HermesConnectedCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesConnectionInfo*);
        void* m_pData;
    };

    // CheckAlive received:
    struct HermesCheckAliveCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesCheckAliveData*);
        void* m_pData;
    };

    // Command received:
    struct HermesCommandCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesCommandData*);
        void* m_pData;
    };

    // The remote host has sent its service description
    struct HermesServiceDescriptionCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesServiceDescriptionData*);
        void* m_pData;
    };

    struct HermesBoardAvailableCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesBoardAvailableData*);
        void* m_pData;
    };

    struct HermesRevokeBoardAvailableCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesRevokeBoardAvailableData*);
        void* m_pData;
    };

    struct HermesMachineReadyCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesMachineReadyData*);
        void* m_pData;
    };

    struct HermesRevokeMachineReadyCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesRevokeMachineReadyData*);
        void* m_pData;
    };

    struct HermesStartTransportCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesStartTransportData*);
        void* m_pData;
    };

    struct HermesTransportFinishedCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesTransportFinishedData*);
        void* m_pData;
    };

    struct HermesStopTransportCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesStopTransportData*);
        void* m_pData;
    };

    struct HermesBoardForecastCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesBoardForecastData*);
        void* m_pData;
    };

    struct HermesQueryBoardInfoCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesQueryBoardInfoData*);
        void* m_pData;
    };

    struct HermesSendBoardInfoCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesSendBoardInfoData*);
        void* m_pData;
    };

    struct HermesDisconnectedCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesError*);
        void* m_pData;
    };

    // Hhe following callbacks do not directly relate to received messages from the remote host.
    // They are triggered through our own calls into the API.

    // The state has changed due to our own calls:
    struct HermesStateCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState);
        void* m_pData;
    };

    // Trace data from the implementation. Note that handling of the traces must be thread-safe.
    struct HermesTraceCallback
    {
        void(*m_pCall)(void* /*m_pData*/, unsigned /*sessionId*/, EHermesTraceType, HermesStringView /*trace*/);
        void *m_pData;
    };

    // The interface for the connection to the downstream machine.

    // These are the callbacks to deal with:
    struct HermesDownstreamCallbacks
    {
        HermesConnectedCallback m_connectedCallback;
        HermesServiceDescriptionCallback m_serviceDescriptionCallback;
        HermesMachineReadyCallback m_machineReadyCallback;
        HermesRevokeMachineReadyCallback m_revokeMachineReadyCallback;
        HermesStartTransportCallback m_startTransportCallback;
        HermesStopTransportCallback m_stopTransportCallback;
        HermesQueryBoardInfoCallback m_queryBoardInfoCallback;
        HermesNotificationCallback m_notificationCallback;
        HermesStateCallback m_stateCallback;
        HermesCheckAliveCallback m_checkAliveCallback;
        HermesCommandCallback m_commandCallback;
        HermesDisconnectedCallback m_disconnectedCallback;
        HermesTraceCallback m_traceCallback;
    };

    // it is sometimes convenient to carry out a task from within the Hermes message queue:
    struct HermesVoidCallback
    {
        void(*m_pCall)(void* /*m_pData*/);
        void *m_pData;
    };

    // The calling API
    struct HermesDownstream; // the opaque handle to the downstream service
    HERMESPROTOCOL_API HermesDownstream* CreateHermesDownstream(uint32_t laneId, const HermesDownstreamCallbacks*);
    HERMESPROTOCOL_API void RunHermesDownstream(HermesDownstream*); // blocks until ::StopHermesDownstream is called
    HERMESPROTOCOL_API void PostHermesDownstream(HermesDownstream*, HermesVoidCallback);
    HERMESPROTOCOL_API void EnableHermesDownstream(HermesDownstream*, const HermesDownstreamSettings*);

    HERMESPROTOCOL_API void SignalHermesDownstreamServiceDescription(HermesDownstream*, uint32_t sessionId, const HermesServiceDescriptionData*);
    HERMESPROTOCOL_API void SignalHermesBoardAvailable(HermesDownstream*, uint32_t sessionId, const HermesBoardAvailableData*);
    HERMESPROTOCOL_API void SignalHermesRevokeBoardAvailable(HermesDownstream*, uint32_t sessionId, const HermesRevokeBoardAvailableData*);
    HERMESPROTOCOL_API void SignalHermesTransportFinished(HermesDownstream*, uint32_t sessionId, const HermesTransportFinishedData*);
    HERMESPROTOCOL_API void SignalHermesBoardForecast(HermesDownstream*, uint32_t sessionId, const HermesBoardForecastData*);
    HERMESPROTOCOL_API void SignalHermesSendBoardInfo(HermesDownstream*, uint32_t sessionId, const HermesSendBoardInfoData*);
    HERMESPROTOCOL_API void SignalHermesDownstreamNotification(HermesDownstream*, uint32_t sessionId, const HermesNotificationData*);
    HERMESPROTOCOL_API void SignalHermesDownstreamCheckAlive(HermesDownstream*, uint32_t sessionId, const HermesCheckAliveData*);
    HERMESPROTOCOL_API void SignalHermesDownstreamCommand(HermesDownstream*, uint32_t sessionId, const HermesCommandData*);
    HERMESPROTOCOL_API void ResetHermesDownstream(HermesDownstream*, const HermesNotificationData*);

    HERMESPROTOCOL_API void DisableHermesDownstream(HermesDownstream*, const HermesNotificationData*);
    HERMESPROTOCOL_API void StopHermesDownstream(HermesDownstream*);
    HERMESPROTOCOL_API void DeleteHermesDownstream(HermesDownstream*);

    // interface for testing with raw or no xml:
    HERMESPROTOCOL_API void SignalHermesDownstreamRawXml(HermesDownstream*, uint32_t sessionId, HermesStringView);
    HERMESPROTOCOL_API void ResetHermesDownstreamRawXml(HermesDownstream*, HermesStringView);

    // The interface for the connection to the downstream machine.

    // These are the callbacks to deal with:
    struct HermesUpstreamCallbacks
    {
        HermesConnectedCallback m_connectedCallback;
        HermesServiceDescriptionCallback m_serviceDescriptionCallback;
        HermesBoardAvailableCallback m_boardAvailableCallback;
        HermesRevokeBoardAvailableCallback m_revokeBoardAvailableCallback;
        HermesTransportFinishedCallback m_transportFinishedCallback;
        HermesBoardForecastCallback m_boardForecastCallback;
        HermesSendBoardInfoCallback m_sendBoardInfoCallback;
        HermesNotificationCallback m_notificationCallback;
        HermesStateCallback m_stateCallback;
        HermesCheckAliveCallback m_checkAliveCallback;
        HermesCommandCallback m_commandCallback;
        HermesDisconnectedCallback m_disconnectedCallback;
        HermesTraceCallback m_traceCallback;
    };

    // The calling API:
    struct HermesUpstream; // the opaque handle to the upstream service
    HERMESPROTOCOL_API HermesUpstream* CreateHermesUpstream(uint32_t laneId, const HermesUpstreamCallbacks*);
    HERMESPROTOCOL_API void RunHermesUpstream(HermesUpstream*);
    HERMESPROTOCOL_API void PostHermesUpstream(HermesUpstream*, HermesVoidCallback);
    HERMESPROTOCOL_API void EnableHermesUpstream(HermesUpstream*, const HermesUpstreamSettings*);

    HERMESPROTOCOL_API void SignalHermesUpstreamServiceDescription(HermesUpstream*, uint32_t sessionId, const HermesServiceDescriptionData*);
    HERMESPROTOCOL_API void SignalHermesMachineReady(HermesUpstream*, uint32_t sessionId, const HermesMachineReadyData*);
    HERMESPROTOCOL_API void SignalHermesRevokeMachineReady(HermesUpstream*, uint32_t sessionId, const HermesRevokeMachineReadyData*);
    HERMESPROTOCOL_API void SignalHermesStartTransport(HermesUpstream*, uint32_t sessionId, const HermesStartTransportData*);
    HERMESPROTOCOL_API void SignalHermesStopTransport(HermesUpstream*, uint32_t sessionId, const HermesStopTransportData*);
    HERMESPROTOCOL_API void SignalHermesQueryBoardInfo(HermesUpstream*, uint32_t sessionId, const HermesQueryBoardInfoData*);
    HERMESPROTOCOL_API void SignalHermesUpstreamNotification(HermesUpstream*, uint32_t sessionId, const HermesNotificationData*);
    HERMESPROTOCOL_API void SignalHermesUpstreamCheckAlive(HermesUpstream*, uint32_t sessionId, const HermesCheckAliveData*);
    HERMESPROTOCOL_API void SignalHermesUpstreamCommand(HermesUpstream*, uint32_t sessionId, const HermesCommandData*);
    HERMESPROTOCOL_API void ResetHermesUpstream(HermesUpstream*, const HermesNotificationData*);

    HERMESPROTOCOL_API void DisableHermesUpstream(HermesUpstream*, const HermesNotificationData*);
    HERMESPROTOCOL_API void StopHermesUpstream(HermesUpstream*);
    HERMESPROTOCOL_API void DeleteHermesUpstream(HermesUpstream*);

    // interface for testing with raw xml:
    HERMESPROTOCOL_API void SignalHermesUpstreamRawXml(HermesUpstream*, uint32_t sessionId, HermesStringView);
    HERMESPROTOCOL_API void ResetHermesUpstreamRawXml(HermesUpstream*, HermesStringView);

    // For the remote connection configuration, another set of callbacks is required.
    // The following are the callbacks for the configuring client:
    struct HermesCurrentConfigurationCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesCurrentConfigurationData*);
        void* m_pData;
    };

    struct HermesErrorCallback
    {
        void(*m_pCall)(void* /*m_pData*/, const HermesError*);
        void* m_pData;
    };

    struct HermesSetConfigurationCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesSetConfigurationData*, const HermesConnectionInfo*);
        void* m_pData;
    };

    struct HermesGetConfigurationCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t/*sessionId*/, const HermesGetConfigurationData*, const HermesConnectionInfo*);
        void* m_pData;
    };

    // The API for the configuring client:
    // Set the The Hermes Standard configuration for a piece of equipment 
    // and obtain the resulting current configuration or an error as response:
    struct HermesSetConfigurationCallbacks
    {
        HermesCurrentConfigurationCallback m_currentConfigurationCallback;
        HermesErrorCallback m_errorCallback;
        HermesNotificationCallback m_notificationCallback;
        HermesTraceCallback m_traceCallback;
    };

    HERMESPROTOCOL_API void SetHermesConfiguration(HermesStringView hostName, const HermesSetConfigurationData*,
        unsigned timeoutInSeconds, const HermesSetConfigurationCallbacks*);

    // Get the Hermes configuration for a piece of equipment:
    struct HermesGetConfigurationCallbacks
    {
        HermesCurrentConfigurationCallback m_currentConfigurationCallback;
        HermesErrorCallback m_errorCallback;
        HermesTraceCallback m_traceCallback;
    };
    HERMESPROTOCOL_API void GetHermesConfiguration(HermesStringView  hostName,
        unsigned timeoutInSeconds, const HermesGetConfigurationCallbacks*);

    // The API to interface with The Hermes Standard service running on the equipment:
    struct HermesConfigurationServiceCallbacks
    {
        HermesConnectedCallback m_connectedCallback;
        HermesSetConfigurationCallback m_setConfigurationCallback;
        HermesGetConfigurationCallback m_getConfigurationCallback;
        HermesDisconnectedCallback m_disconnectedCallback;
        HermesTraceCallback m_traceCallback;
    };

    struct HermesConfigurationService; // the opaque handle to the configuration service
    HERMESPROTOCOL_API HermesConfigurationService* CreateHermesConfigurationService(const HermesConfigurationServiceCallbacks*);
    HERMESPROTOCOL_API void RunHermesConfigurationService(HermesConfigurationService*); // blocks until StopHermesConfigurationService is called
    HERMESPROTOCOL_API void PostHermesConfigurationService(HermesConfigurationService*, HermesVoidCallback);
    HERMESPROTOCOL_API void EnableHermesConfigurationService(HermesConfigurationService*, const HermesConfigurationServiceSettings*);

    // the following must be called from within the m_getConfigurationCallback so that the HermesConfigurationService can match them up
    HERMESPROTOCOL_API void SignalHermesCurrentConfiguration(HermesConfigurationService*, uint32_t sessionId, const HermesCurrentConfigurationData*);
    HERMESPROTOCOL_API void SignalHermesConfigurationNotification(HermesConfigurationService*, uint32_t sessionId, const HermesNotificationData*);

    HERMESPROTOCOL_API void DisableHermesConfigurationService(HermesConfigurationService*, const HermesNotificationData*);
    HERMESPROTOCOL_API void StopHermesConfigurationService(HermesConfigurationService*);
    HERMESPROTOCOL_API void DeleteHermesConfigurationService(HermesConfigurationService*);

    // vertical channel:
    struct HermesVerticalConnectedCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesVerticalState, const HermesConnectionInfo*);
        void* m_pData;
    };

    struct HermesSupervisoryServiceDescriptionCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesVerticalState, const HermesSupervisoryServiceDescriptionData*);
        void* m_pData;
    };

    struct HermesSendWorkOrderInfoCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesSendWorkOrderInfoData*);
        void* m_pData;
    };

    struct HermesQueryHermesCapabilitiesCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesQueryHermesCapabilitiesData*);
        void* m_pData;
    };

    struct HermesVerticalDisconnectedCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesVerticalState, const HermesError*);
        void* m_pData;
    };

    struct HermesVerticalServiceCallbacks
    {
        HermesVerticalConnectedCallback m_connectedCallback;
        HermesSupervisoryServiceDescriptionCallback m_serviceDescriptionCallback;
        HermesSendWorkOrderInfoCallback m_sendWorkOrderInfoCallback;
        HermesNotificationCallback m_notificationCallback;
        HermesCheckAliveCallback m_checkAliveCallback;
        HermesGetConfigurationCallback m_getConfigurationCallback;
        HermesSetConfigurationCallback m_setConfigurationCallback;
        HermesQueryHermesCapabilitiesCallback m_queryHermesCapabilitiesCallback;
        HermesVerticalDisconnectedCallback m_disconnectedCallback;
        HermesTraceCallback m_traceCallback;
    };

    // The calling API
    struct HermesVerticalService; // the opaque handle to the supervisor service
    HERMESPROTOCOL_API HermesVerticalService* CreateHermesVerticalService(const HermesVerticalServiceCallbacks*);
    HERMESPROTOCOL_API void RunHermesVerticalService(HermesVerticalService*); // blocks until ::StopHermesDownstream is called
    HERMESPROTOCOL_API void PostHermesVerticalService(HermesVerticalService*, HermesVoidCallback);
    HERMESPROTOCOL_API void EnableHermesVerticalService(HermesVerticalService*, const HermesVerticalServiceSettings*);

    HERMESPROTOCOL_API void SignalHermesVerticalServiceDescription(HermesVerticalService*, uint32_t sessionId, const HermesSupervisoryServiceDescriptionData*);
    HERMESPROTOCOL_API void SignalHermesQueryWorkOrderInfo(HermesVerticalService*, uint32_t sessionId, const HermesQueryWorkOrderInfoData*);
    HERMESPROTOCOL_API void SignalHermesReplyWorkOrderInfo(HermesVerticalService*, uint32_t sessionId, const HermesReplyWorkOrderInfoData*);
    HERMESPROTOCOL_API void SignalHermesVerticalServiceNotification(HermesVerticalService*, uint32_t sessionId, const HermesNotificationData*);
    HERMESPROTOCOL_API void SignalHermesVerticalServiceCheckAlive(HermesVerticalService*, uint32_t sessionId, const HermesCheckAliveData*);
    HERMESPROTOCOL_API void SignalHermesVerticalCurrentConfiguration(HermesVerticalService*, uint32_t sessionId, const HermesCurrentConfigurationData*);
    HERMESPROTOCOL_API void SignalHermesSendHermesCapabilities(HermesVerticalService*, uint32_t sessionId, const HermesSendHermesCapabilitiesData*);

    // if sessionId == 0, then the following two are propagated to all clients that have specified FeatureBoardTracking
    HERMESPROTOCOL_API void SignalHermesBoardArrived(HermesVerticalService*, uint32_t sessionId, const HermesBoardArrivedData*);
    HERMESPROTOCOL_API void SignalHermesBoardDeparted(HermesVerticalService*, uint32_t sessionId, const HermesBoardDepartedData*);

    HERMESPROTOCOL_API void ResetHermesVerticalServiceSession(HermesVerticalService*, uint32_t sessionId, const HermesNotificationData*);

    HERMESPROTOCOL_API void DisableHermesVerticalService(HermesVerticalService*, const HermesNotificationData*);
    HERMESPROTOCOL_API void StopHermesVerticalService(HermesVerticalService*);
    HERMESPROTOCOL_API void DeleteHermesVerticalService(HermesVerticalService*);

    struct HermesQueryWorkOrderInfoCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesQueryWorkOrderInfoData*);
        void* m_pData;
    };

    struct HermesReplyWorkOrderInfoCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesReplyWorkOrderInfoData*);
        void* m_pData;
    };

    struct HermesBoardArrivedCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesBoardArrivedData*);
        void* m_pData;
    };

    struct HermesBoardDepartedCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesBoardDepartedData*);
        void* m_pData;
    };
    
    struct HermesSendHermesCapabilitiesCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesSendHermesCapabilitiesData*);
        void* m_pData;
    };
    
    struct HermesVerticalClientCallbacks
    {
        HermesVerticalConnectedCallback m_connectedCallback;
        HermesSupervisoryServiceDescriptionCallback m_serviceDescriptionCallback;
        HermesBoardArrivedCallback m_boardArrivedCallback;
        HermesBoardDepartedCallback m_boardDepartedCallback;
        HermesSendHermesCapabilitiesCallback m_sendHermesCapabilitiesCallback;
        HermesQueryWorkOrderInfoCallback m_queryWorkOrderInfoCallback;
        HermesReplyWorkOrderInfoCallback m_replyWorkOrderInfoCallback;
        HermesNotificationCallback m_notificationCallback;
        HermesCurrentConfigurationCallback m_currentConfigurationCallback;
        HermesCheckAliveCallback m_checkAliveCallback;
        HermesVerticalDisconnectedCallback m_disconnectedCallback;
        HermesTraceCallback m_traceCallback;
    };


    // The calling API
    struct HermesVerticalClient; // the opaque handle to the supervisor service
    HERMESPROTOCOL_API HermesVerticalClient* CreateHermesVerticalClient(const HermesVerticalClientCallbacks*);
    HERMESPROTOCOL_API void RunHermesVerticalClient(HermesVerticalClient*); // blocks until ::StopHermesDownstream is called
    HERMESPROTOCOL_API void PostHermesVerticalClient(HermesVerticalClient*, HermesVoidCallback);
    HERMESPROTOCOL_API void EnableHermesVerticalClient(HermesVerticalClient*, const HermesVerticalClientSettings*);

    HERMESPROTOCOL_API void SignalHermesVerticalClientDescription(HermesVerticalClient*, uint32_t sessionId, const HermesSupervisoryServiceDescriptionData*);
    HERMESPROTOCOL_API void SignalHermesSendWorkOrderInfo(HermesVerticalClient*, uint32_t sessionId, const HermesSendWorkOrderInfoData*);
    HERMESPROTOCOL_API void SignalHermesVerticalGetConfiguration(HermesVerticalClient*, uint32_t sessionId, const HermesGetConfigurationData*);
    HERMESPROTOCOL_API void SignalHermesVerticalSetConfiguration(HermesVerticalClient*, uint32_t sessionId, const HermesSetConfigurationData*);
    HERMESPROTOCOL_API void SignalHermesVerticalQueryHermesCapabilities(HermesVerticalClient*, uint32_t sessionId, const HermesQueryHermesCapabilitiesData*);
    
    HERMESPROTOCOL_API void SignalHermesVerticalClientNotification(HermesVerticalClient*, uint32_t sessionId, const HermesNotificationData*);
    HERMESPROTOCOL_API void SignalHermesVerticalClientCheckAlive(HermesVerticalClient*, uint32_t sessionId, const HermesCheckAliveData*);
    
    HERMESPROTOCOL_API void ResetHermesVerticalClient(HermesVerticalClient*, const HermesNotificationData*);

    // interface for testing with raw xml:
    HERMESPROTOCOL_API void SignalHermesVerticalClientRawXml(HermesVerticalClient*, uint32_t sessionId, HermesStringView);
    HERMESPROTOCOL_API void ResetHermesVerticalClientRawXml(HermesVerticalClient*, HermesStringView);

    HERMESPROTOCOL_API void DisableHermesVerticalClient(HermesVerticalClient*, const HermesNotificationData*);
    HERMESPROTOCOL_API void StopHermesVerticalClient(HermesVerticalClient*);
    HERMESPROTOCOL_API void DeleteHermesVerticalClient(HermesVerticalClient*);





#ifdef __cplusplus
}
#endif

#endif //HERMES_H


