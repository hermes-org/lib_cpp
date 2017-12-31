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

#define HERMES_VERSION "1.0"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    // The header is organized to follow the documentation of The Hermes Standard 
    // as closely as possible: https://www.the-hermes-standard.info/wp-content/uploads/pdf/The_Hermes_Standard_Version_1.0.pdf
    // It maintains the order and the naming as far
    // as the C syntax and the coding guidelines allow it.

    // Not part of The Hermes Standard, but used extensively: a non-owning string type, in the spirit of std::string_view.
    // This relieves us from the need to terminate all strings with \0.
    // Note that all the C interface structures are non-owning and need to be backed up by actual storage.
    struct HermesStringView
    {
        const char* m_pData; // if nullptr then we have no string at all - not even an empty string
        uint32_t m_size;
    };

    // base port number, The Hermes Standard 2.1
    static const uint16_t cHERMES_BASE_PORT = 50100U;

    // configuration port number, The Hermes Standard 2.2
    static const uint16_t cHERMES_CONFIG_PORT = 1248U;

    // protocol states, The Hermes Standard 2.6
    enum EHermesState
    {
        eHERMES_NOT_CONNECTED,
        eHERMES_SOCKET_CONNECTED, // not part of The Hermes Standard statechart, but helpful to differentiate an intermediate step
        eHERMES_SERVICE_DESCRIPTION_DOWNSTREAM,
        eHERMES_NOT_AVAILABLE_NOT_READY,
        eHERMES_BOARD_AVAILABLE,
        eHERMES_MACHINE_READY,
        eHERMES_AVAILABLE_AND_READY,
        eHERMES_TRANSPORTING,
        eHERMES_TRANSPORT_STOPPED,
        eHERMES_TRANSPORT_FINISHED,
        eHERMES_DISCONNECTED, // not part of the The Hermes Standard, but implicitly present as the terminal state
        eHERMES_STATE_ENUM_SIZE // not part of The Hermes Standard; only for programming convenience
    };

    // maximum message size, The Hermes Standard 3.1
    static const unsigned cHERMES_MAX_MESSAGE_SIZE = 65536U;

    // CheckAlive, The Hermes Standard 3.3
    struct HermesCheckAliveData
    {};

    // ServiceDescription, The Hermes Standard 3.4
    struct HermesServiceDescription
    {
        HermesStringView m_machineId;
        uint32_t m_laneId;
        HermesStringView m_version;
        // SupportedFeatures is omitted because V1.0 does not specify any
    };

    // Notification, The  Hermes Standard 3.5
    enum EHermesNotificationCode
    {
        eHERMES_UNSPECIFIC_NOTIFICATION = 0,
        eHERMES_PROTOCOL_ERROR = 1,
        eHERMES_CONNECTION_REFUSED_BECAUSE_OF_ESTABLISHED_CONNECTION = 2,
        eHERMES_CONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION = 3,
        eHERMES_CONFIGURATION_ERROR = 4,
        eHERMES_MACHINE_SHUTDOWN = 5,
        eHERMES_NOTIFICATION_CODE_ENUM_SIZE // not part of The Hermes Standard; only for programming convenience
    };

    enum EHermesSeverity
    {
        eHERMES_UNKNOWN_SEVERITY = 0,
        eHERMES_FATAL = 1,
        eHERMES_ERROR = 2,
        eHERMES_eWARNING = 3,
        eHERMES_INFO = 4,
        eHERMES_SEVERITY_ENUM_SIZE // not part of The Hermes Standard; only for programming convenience
    };

    struct HermesNotificationData
    {
        int32_t m_notificationCode; // must be in the range of EHermesNotificationCode
        int32_t m_severity; // must be in the range of EHermesSeverity
        HermesStringView m_description;
    };

    // BoardAvailable, The Hermes Standard 3.6
    enum EHermesBoardQuality
    {
        eHERMES_ANY_BOARD_QUALITY = 0,
        eHERMES_GOOD_BOARD_QUALITY = 1,
        eHERMES_FAILED_BOARD_QUALITY = 2,
        eHERMES_BOARD_QUALITY_ENUM_SIZE // not part of The Hermes Standard; only for programming convenience
    };

    enum EHermesFlippedBoard
    {
        eHERMES_SIDE_UP_IS_UNKNOWN,
        eHERMES_BOARD_TOP_SIDE_IS_UP = 1,
        eHERMES_BOARD_BOTTOM_SIDE_IS_UP = 2,
        eHERMES_FLIPPED_BOARD_ENUM_SIZE // not part of The Hermes Standard; only for programming convenience
    };

    struct HermesBoardAvailableData
    {
        HermesStringView m_boardId;
        HermesStringView m_boardIdCreatedBy;
        EHermesBoardQuality m_failedBoard;
        HermesStringView m_optionalProductTypeId;
        EHermesFlippedBoard m_flippedBoard;
        HermesStringView m_optionalTopBarcode;
        HermesStringView m_optionalBottomBarcode;
        const double* m_pOptionalLengthInMM; // possibly nullptr since optional
        const double* m_pOptionalWidthInMM; // possibly nullptr since optional
        const double* m_pOptionalThicknessInMM; // possibly nullptr since optional
        const double* m_pOptionalConveyorSpeedInMMPerSecs; // possibly nullptr since optional
        const double* m_pOptionalTopClearanceHeightInMM; // possibly nullptr since optional
        const double* m_pOptionalBottomClearanceHeightInMM; // possibly nullptr since optional
    };

    // RevokeBoardAvailable, The Hermes Standard 3.7
    struct HermesRevokeBoardAvailableData
    {};

    // MachineReady, The Hermes Standard 3.8
    struct HermesMachineReadyData
    {
        EHermesBoardQuality m_failedBoard;
    };

    // RevokeMachineReady, The Hermes Standard 3.9
    struct HermesRevokeMachineReadyData
    {};

    // StartTransport, The Hermes Standard 3.10
    struct HermesStartTransportData
    {
        HermesStringView m_boardId;
        const double* m_pOptionalConveyorSpeedInMMPerSecs; // possibly nullptr since optional
    };

    // StopTransport, The Hermes Standard 3.11
    enum EHermesTransferState
    {
        eHERMES_TRANSFER_NOT_STARTED = 1,
        eHERMES_TRANSFER_INCOMPLETE = 2,
        eHERMES_TRANSFER_COMPLETE = 3,
        eHERMES_TRANSFER_STATE_ENUM_SIZE // not part of The Hermes Standard; only for programming convenience
    };

    struct HermesStopTransportData
    {
        EHermesTransferState m_transferState; // must be in the range of EHermesTransferState
        HermesStringView m_boardId;
    };

    // TransportFinished, The Hermes Standard 3.12
    struct HermesTransportFinishedData
    {
        EHermesTransferState m_transferState; // must be in the range of EHermesTransferState
        HermesStringView m_boardId;
    };

    // SetConfiguration, The Hermes Standard 3.13
    struct HermesUpstreamConfiguration
    {
        uint32_t m_upstreamLaneId;
        HermesStringView m_hostAddress;
        uint16_t m_port; // if 0 then HermesProtocol will use the default port number for this lane
    };

    struct HermesDownstreamConfiguration
    {
        uint32_t m_downstreamLaneId;
        HermesStringView m_optionalClientAddress; // if 0 then requests from any client will be accepted
        uint16_t m_port; // if 0, then HermesProcol will use the default port for this lane
    };

    struct HermesSetConfigurationData
    {
        HermesStringView m_machineId;
        const HermesUpstreamConfiguration** m_upstreamConfigurations; //pointer to first element of array
        uint32_t m_upstreamConfigurationCount; //count of array elements
        const HermesDownstreamConfiguration** m_downstreamConfigurations; //pointer to first element of array
        uint32_t m_downstreamConfigurationCount; //count of array elements
    };

    struct HermesGetConfigurationData
    {};

    struct HermesCurrentConfigurationData
    {
        HermesStringView m_optionalMachineId;
        const HermesUpstreamConfiguration** m_upstreamConfigurations; //pointer to first element of array
        uint32_t m_upstreamConfigurationCount; //count of array elements
        const HermesDownstreamConfiguration** m_downstreamConfigurations; //pointer to first element of array
        uint32_t m_downstreamConfigurationCount; //count of array elements
    };

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

    // For an established connection, an info about the connected foreign host and the foreign port is provided: 
    struct HermesConnectionInfo
    {
        HermesStringView m_address;
        uint16_t m_port;
        HermesStringView m_hostName;
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

    // The remote host has sent its service description
    struct HermesServiceDescriptionCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, EHermesState, const HermesServiceDescription*);
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

    // Disconnections might well be due to an error condition.
    enum EHermesErrorCode
    {
        eHERMES_SUCCESS,
        eHERMES_IMPLEMENTATION_ERROR, // error inside the Hermes DLL
        eHERMES_PEER_ERROR,  // the remote host has misbehaved
        eHERMES_CLIENT_ERROR, // the client code making the API calls has misbehaved
        eHERMES_NETWORK_ERROR, // something is wrong with the network or its configuration
        eHERMES_TIMEOUT, // a specified timeout has been reached
        eHERMES_ERROR_CODE_ENUM_SIZE
    };
    struct HermesError
    {
        EHermesErrorCode m_code;
        HermesStringView m_text;
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
    enum EHermesTraceType
    {
        eHERMES_TRACE_SENT, // raw message sent
        eHERMES_TRACE_RECEIVED, // raw message received
        eHERMES_TRACE_DEBUG, // debug trace from implementation internals
        eHERMES_TRACE_INFO,
        eHERMES_TRACE_WARNING,
        eHERMES_TRACE_ERROR,
        eHERMES_TRACE_TYPE_ENUM_SIZE
    };
    struct HermesTraceCallback
    {
        void(*m_pCall)(void* /*m_pData*/, unsigned /*sessionId*/, EHermesTraceType, HermesStringView /*trace*/);
        void *m_pData;
    };

    enum EHermesCheckState
    {
        eHERMES_CHECK_SEND_AND_RECEIVE,
        eHERMES_CHECK_ONLY_RECEIVE,
        eHERMES_CHECK_STATE_ENUM_SIZE
    };

    // The interface for the connection to the downstream machine.
    // Configuration settings for our connection:
    struct HermesDownstreamSettings
    {
        HermesStringView m_machineId; // required for the service description
        HermesStringView m_optionalClientAddress; // if empty then requests from any client will be accepted
        uint16_t m_port; // if 0, then HermesProcol will use the default port for this lane
        uint32_t m_checkAlivePeriodInSeconds; // if 0 then no check alive messages will be sent
        uint32_t m_reconnectWaitTimeInSeconds;
        EHermesCheckState m_checkState;
    };

    // These are the callbacks to deal with:
    struct HermesDownstreamCallbacks
    {
        HermesConnectedCallback m_connectedCallback;
        HermesServiceDescriptionCallback m_serviceDescriptionCallback;
        HermesMachineReadyCallback m_machineReadyCallback;
        HermesRevokeMachineReadyCallback m_revokeMachineReadyCallback;
        HermesStartTransportCallback m_startTransportCallback;
        HermesStopTransportCallback m_stopTransportCallback;
        HermesNotificationCallback m_notificationCallback;
        HermesStateCallback m_stateCallback;
        HermesCheckAliveCallback m_checkAliveCallback;
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

    HERMESPROTOCOL_API void SignalHermesDownstreamServiceDescription(HermesDownstream*, uint32_t sessionId, const HermesServiceDescription*);
    HERMESPROTOCOL_API void SignalHermesBoardAvailable(HermesDownstream*, uint32_t sessionId, const HermesBoardAvailableData*);
    HERMESPROTOCOL_API void SignalHermesRevokeBoardAvailable(HermesDownstream*, uint32_t sessionId, const HermesRevokeBoardAvailableData*);
    HERMESPROTOCOL_API void SignalHermesTransportFinished(HermesDownstream*, uint32_t sessionId, const HermesTransportFinishedData*);
    HERMESPROTOCOL_API void SignalHermesDownstreamNotification(HermesDownstream*, uint32_t sessionId, const HermesNotificationData*);
    HERMESPROTOCOL_API void SignalHermesDownstreamCheckAlive(HermesDownstream*, uint32_t sessionId, const HermesCheckAliveData*);
    HERMESPROTOCOL_API void ResetHermesDownstream(HermesDownstream*, const HermesNotificationData*);

    HERMESPROTOCOL_API void DisableHermesDownstream(HermesDownstream*, const HermesNotificationData*);
    HERMESPROTOCOL_API void StopHermesDownstream(HermesDownstream*);
    HERMESPROTOCOL_API void DeleteHermesDownstream(HermesDownstream*);

    // The interface for the connection to the upstream machine.
    // Configuration settings for our connection:
    struct HermesUpstreamSettings
    {
        HermesStringView m_machineId; // required for the service description
        HermesStringView m_hostAddress;
        uint16_t m_port; // if 0 then HermesProtocol will use the default port number for this lane
        uint32_t m_checkAlivePeriodInSeconds;
        uint32_t m_reconnectWaitTimeInSeconds;
        EHermesCheckState m_checkState;
    };

    // These are the callbacks to deal with:
    struct HermesUpstreamCallbacks
    {
        HermesConnectedCallback m_connectedCallback;
        HermesServiceDescriptionCallback m_serviceDescriptionCallback;
        HermesBoardAvailableCallback m_boardAvailableCallback;
        HermesRevokeBoardAvailableCallback m_revokeBoardAvailableCallback;
        HermesTransportFinishedCallback m_transportFinishedCallback;
        HermesNotificationCallback m_notificationCallback;
        HermesStateCallback m_stateCallback;
        HermesCheckAliveCallback m_checkAliveCallback;
        HermesDisconnectedCallback m_disconnectedCallback;
        HermesTraceCallback m_traceCallback;
    };

    // The calling API:
    struct HermesUpstream; // the opaque handle to the upstream service
    HERMESPROTOCOL_API HermesUpstream* CreateHermesUpstream(uint32_t laneId, const HermesUpstreamCallbacks*);
    HERMESPROTOCOL_API void RunHermesUpstream(HermesUpstream*);
    HERMESPROTOCOL_API void PostHermesUpstream(HermesUpstream*, HermesVoidCallback);
    HERMESPROTOCOL_API void EnableHermesUpstream(HermesUpstream*, const HermesUpstreamSettings*);

    HERMESPROTOCOL_API void SignalHermesUpstreamServiceDescription(HermesUpstream*, uint32_t sessionId, const HermesServiceDescription*);
    HERMESPROTOCOL_API void SignalHermesMachineReady(HermesUpstream*, uint32_t sessionId, const HermesMachineReadyData*);
    HERMESPROTOCOL_API void SignalHermesRevokeMachineReady(HermesUpstream*, uint32_t sessionId, const HermesRevokeMachineReadyData*);
    HERMESPROTOCOL_API void SignalHermesStartTransport(HermesUpstream*, uint32_t sessionId, const HermesStartTransportData*);
    HERMESPROTOCOL_API void SignalHermesStopTransport(HermesUpstream*, uint32_t sessionId, const HermesStopTransportData*);
    HERMESPROTOCOL_API void SignalHermesUpstreamNotification(HermesUpstream*, uint32_t sessionId, const HermesNotificationData*);
    HERMESPROTOCOL_API void SignalHermesUpstreamCheckAlive(HermesUpstream*, uint32_t sessionId, const HermesCheckAliveData*);
    HERMESPROTOCOL_API void ResetHermesUpstream(HermesUpstream*, const HermesNotificationData*);

    HERMESPROTOCOL_API void DisableHermesUpstream(HermesUpstream*, const HermesNotificationData*);
    HERMESPROTOCOL_API void StopHermesUpstream(HermesUpstream*);
    HERMESPROTOCOL_API void DeleteHermesUpstream(HermesUpstream*);

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
        void(*m_pCall)(void* /*m_pData*/, uint32_t /*sessionId*/, const HermesConnectionInfo*, const HermesSetConfigurationData*);
        void* m_pData;
    };

    struct HermesGetConfigurationCallback
    {
        void(*m_pCall)(void* /*m_pData*/, uint32_t/*sessionId*/, const HermesConnectionInfo*, const HermesGetConfigurationData*);
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
    struct HermesConfigurationServiceSettings
    {
        uint16_t m_port; // if 0 then use default
        uint32_t m_reconnectWaitTimeInSeconds;
    };

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


#ifdef __cplusplus
}
#endif

#endif //HERMES_H


