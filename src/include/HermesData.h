// Copyright (c) ASM Assembly Systems GmbH & Co. KG
//
// C interface for an implementation of The Hermes Standard
//
#ifndef HERMESDATA_H
#define HERMESDATA_H

#include "HermesStringView.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* The Hermes Standard 3.3 */
    enum EHermesCheckAliveType
    {
        eHERMES_CHECK_ALIVE_TYPE_UNKNOWN,
        eHERMES_CHECK_ALIVE_TYPE_PING,
        eHERMES_CHECK_ALIVE_TYPE_PONG,
        cHERMES_CHECK_ALIVE_TYPE_ENUM_SIZE = 3
    };

    /* The Hermes Standard 3.5 */
    enum EHermesNotificationCode
    {
        eHERMES_NOTIFICATION_CODE_UNSPECIFIC,
        eHERMES_NOTIFICATION_CODE_PROTOCOL_ERROR,
        eHERMES_NOTIFICATION_CODE_CONNECTION_REFUSED_BECAUSE_OF_ESTABLISHED_CONNECTION,
        eHERMES_NOTIFICATION_CODE_CONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION,
        eHERMES_NOTIFICATION_CODE_CONFIGURATION_ERROR,
        eHERMES_NOTIFICATION_CODE_MACHINE_SHUTDOWN,
        eHERMES_NOTIFICATION_CODE_BOARD_FORECAST_ERROR,
        cHERMES_NOTIFICATION_CODE_ENUM_SIZE = 7
    };

    /* The Hermes Standard 3.5 */
    enum EHermesSeverity
    {
        eHERMES_SEVERITY_UNKNOWN,
        eHERMES_SEVERITY_FATAL,
        eHERMES_SEVERITY_ERROR,
        eHERMES_SEVERITY_WARNING,
        eHERMES_SEVERITY_INFO,
        cHERMES_SEVERITY_ENUM_SIZE = 5
    };

    /* The Hermes Standard 3.6 */
    enum EHermesBoardQuality
    {
        eHERMES_BOARD_QUALITY_ANY,
        eHERMES_BOARD_QUALITY_GOOD,
        eHERMES_BOARD_QUALITY_BAD,
        cHERMES_BOARD_QUALITY_ENUM_SIZE = 3
    };

    /* The Hermes Standard 3.6 */
    enum EHermesFlippedBoard
    {
        eHERMES_FLIPPED_BOARD_SIDE_UP_IS_UNKNOWN,
        eHERMES_FLIPPED_BOARD_TOP_SIDE_IS_UP,
        eHERMES_FLIPPED_BOARD_BOTTOM_SIDE_IS_UP,
        cHERMES_FLIPPED_BOARD_ENUM_SIZE = 3
    };

    /* The Hermes Standard 3.11 */
    enum EHermesTransferState
    {
        eHERMES_TRANSFER_STATE_UNKNOWN,
        eHERMES_TRANSFER_STATE_NOT_STARTED,
        eHERMES_TRANSFER_STATE_INCOMPLETE,
        eHERMES_TRANSFER_STATE_COMPLETE,
        cHERMES_TRANSFER_STATE_ENUM_SIZE = 4
    };

/* The Hermes Standard 3.23 */
enum EHermesBoardArrivedTransfer
{
    eHERMES_BOARD_ARRIVED_TRANSFER_TRANSFERRED,
    eHERMES_BOARD_ARRIVED_TRANSFER_LOADED,
    eHERMES_BOARD_ARRIVED_TRANSFER_INSERTED,
    cHERMES_BOARD_ARRIVED_TRANSFER_ENUM_SIZE = 3
};

/* The Hermes Standard 3.24 */
enum EHermesBoardDepartedTransfer
{
    eHERMES_BOARD_DEPARTED_TRANSFER_TRANSFERRED,
    eHERMES_BOARD_DEPARTED_TRANSFER_UNLOADED,
    eHERMES_BOARD_DEPARTED_TRANSFER_REMOVED,
    cHERMES_BOARD_DEPARTED_TRANSFER_ENUM_SIZE = 3
};

    /* The Hermes Standard chapter 2.6 */
    enum EHermesState
    {
        eHERMES_STATE_NOT_CONNECTED,
        eHERMES_STATE_SOCKET_CONNECTED,
        eHERMES_STATE_SERVICE_DESCRIPTION_DOWNSTREAM,
        eHERMES_STATE_NOT_AVAILABLE_NOT_READY,
        eHERMES_STATE_BOARD_AVAILABLE,
        eHERMES_STATE_MACHINE_READY,
        eHERMES_STATE_AVAILABLE_AND_READY,
        eHERMES_STATE_TRANSPORTING,
        eHERMES_STATE_TRANSPORT_STOPPED,
        eHERMES_STATE_TRANSPORT_FINISHED,
        eHERMES_STATE_DISCONNECTED,
        cHERMES_STATE_ENUM_SIZE = 11
    };

    /* Trace levels of the implementation (not part of The Hermes Standard) */
    enum EHermesTraceType
    {
        eHERMES_TRACE_TYPE_SENT,
        eHERMES_TRACE_TYPE_RECEIVED,
        eHERMES_TRACE_TYPE_DEBUG,
        eHERMES_TRACE_TYPE_INFO,
        eHERMES_TRACE_TYPE_WARNING,
        eHERMES_TRACE_TYPE_ERROR,
        cHERMES_TRACE_TYPE_ENUM_SIZE = 6
    };

    /* Internal state check modes of the implementation (not part of The Hermes Standard) */
    enum EHermesCheckState
    {
        eHERMES_CHECK_STATE_SEND_AND_RECEIVE,
        eHERMES_CHECK_STATE_ONLY_RECEIVE,
        cHERMES_CHECK_STATE_ENUM_SIZE = 2
    };

    /* How to respond to a check alive ping) */
    enum EHermesCheckAliveResponseMode
    {
        eHERMES_CHECK_ALIVE_RESPONSE_MODE_AUTO,
        eHERMES_CHECK_ALIVE_RESPONSE_MODE_APPLICATION,
        cHERMES_CHECK_ALIVE_RESPONSE_MODE_ENUM_SIZE = 2
    };

    /* Error codes (not part of The Hermes Standard) */
    enum EHermesErrorCode
    {
        eHERMES_ERROR_CODE_SUCCESS,
        eHERMES_ERROR_CODE_IMPLEMENTATION_ERROR,
        eHERMES_ERROR_CODE_PEER_ERROR,
        eHERMES_ERROR_CODE_CLIENT_ERROR,
        eHERMES_ERROR_CODE_NETWORK_ERROR,
        eHERMES_ERROR_CODE_TIMEOUT,
        cHERMES_ERROR_CODE_ENUM_SIZE = 6
    };

/* The Hermes Standard chapter 2.5.3 */
enum EHermesVerticalState
{
    eHERMES_VERTICAL_STATE_NOT_CONNECTED,
    eHERMES_VERTICAL_STATE_SOCKET_CONNECTED,
    eHERMES_VERTICAL_STATE_SUPERVISORY_SERVICE_DESCRIPTION,
    eHERMES_VERTICAL_STATE_CONNECTED,
    eHERMES_VERTICAL_STATE_DISCONNECTED,
    cHERMES_VERTICAL_STATE_ENUM_SIZE = 5
};

    /* CheckAlive, The Hermes Standard 3.3 */
    struct HermesCheckAliveData
    {
        const EHermesCheckAliveType* m_pOptionalType;
        HermesStringView m_optionalId;
    };

    /* FeatureBoardForecast, The Hermes Standard 3.4 */
    struct HermesFeatureBoardForecast
{
};

    /* FeatureCheckAliveResponse, The Hermes Standard 3.4 */
    struct HermesFeatureCheckAliveResponse
{
};

    /* FeatureQueryBoardInfo, The Hermes Standard 3.4 */
    struct HermesFeatureQueryBoardInfo
{
};

    /* FeatureSendBoardInfo, The Hermes Standard 3.4 */
    struct HermesFeatureSendBoardInfo
{
};

    /* SupportedFeatures, The Hermes Standard 3.4 */
    struct HermesSupportedFeatures
    {
        const HermesFeatureBoardForecast* m_pOptionalFeatureBoardForecast;
        const HermesFeatureCheckAliveResponse* m_pOptionalFeatureCheckAliveResponse;
        const HermesFeatureQueryBoardInfo* m_pOptionalFeatureQueryBoardInfo;
        const HermesFeatureSendBoardInfo* m_pOptionalFeatureSendBoardInfo;
    };

    /* ServiceDescription, The Hermes Standard 3.4 */
    struct HermesServiceDescriptionData
    {
        HermesStringView m_machineId;
        uint32_t m_laneId;
        HermesStringView m_optionalInterfaceId;
        HermesStringView m_version;
        const HermesSupportedFeatures* m_pSupportedFeatures;
    };

    /* Notification, The Hermes Standard 3.5 */
    struct HermesNotificationData
    {
        EHermesNotificationCode m_notificationCode;
        EHermesSeverity m_severity;
        HermesStringView m_description;
    };

    /* BoardAvailable, The Hermes Standard 3.6 */
    struct HermesBoardAvailableData
    {
        HermesStringView m_boardId;
        HermesStringView m_boardIdCreatedBy;
        EHermesBoardQuality m_failedBoard;
        HermesStringView m_optionalProductTypeId;
        EHermesFlippedBoard m_flippedBoard;
        HermesStringView m_optionalTopBarcode;
        HermesStringView m_optionalBottomBarcode;
        const double* m_pOptionalLengthInMM;
        const double* m_pOptionalWidthInMM;
        const double* m_pOptionalThicknessInMM;
        const double* m_pOptionalConveyorSpeedInMMPerSecs;
        const double* m_pOptionalTopClearanceHeightInMM;
        const double* m_pOptionalBottomClearanceHeightInMM;
        const double* m_pOptionalWeightInGrams;
    HermesStringView m_optionalWorkOrderId;
    HermesStringView m_optionalBatchId;
    };

    /* RevokeBoardAvailable, The Hermes Standard 3.7 */
    struct HermesRevokeBoardAvailableData
{
};

    /* MachineReady, The Hermes Standard 3.8 */
    struct HermesMachineReadyData
    {
        EHermesBoardQuality m_failedBoard;
        HermesStringView m_optionalForecastId;
        HermesStringView m_optionalBoardId;
        HermesStringView m_optionalProductTypeId;
        const EHermesFlippedBoard* m_pOptionalFlippedBoard;
        HermesStringView m_optionalTopBarcode;
        HermesStringView m_optionalBottomBarcode;
        const double* m_pOptionalLengthInMM;
        const double* m_pOptionalWidthInMM;
        const double* m_pOptionalThicknessInMM;
        const double* m_pOptionalConveyorSpeedInMMPerSecs;
        const double* m_pOptionalTopClearanceHeightInMM;
        const double* m_pOptionalBottomClearanceHeightInMM;
        const double* m_pOptionalWeightInGrams;
    HermesStringView m_optionalWorkOrderId;
    HermesStringView m_optionalBatchId;
    };

    /* RevokeMachineReady, The Hermes Standard 3.9 */
    struct HermesRevokeMachineReadyData
{
};

    /* StartTransport, The Hermes Standard 3.10 */
    struct HermesStartTransportData
    {
        HermesStringView m_boardId;
        const double* m_pOptionalConveyorSpeedInMMPerSecs;
    };

    /* StopTransport, The Hermes Standard 3.11 */
    struct HermesStopTransportData
    {
        EHermesTransferState m_transferState;
        HermesStringView m_boardId;
    };

    /* TransportFinished, The Hermes Standard 3.12 */
    struct HermesTransportFinishedData
    {
        EHermesTransferState m_transferState;
        HermesStringView m_boardId;
    };

    /* UpstreamConfiguration, The Hermes Standard 3.13 */
    struct HermesUpstreamConfiguration
    {
        uint32_t m_upstreamLaneId;
        HermesStringView m_optionalUpstreamInterfaceId;
        HermesStringView m_hostAddress;
        uint16_t m_port;
    };

    /* The Hermes Standard 3.13 */
    struct HermesUpstreamConfigurations
    {
        const HermesUpstreamConfiguration** m_pData; // pointer to first element
        size_t m_size; // number of elements
    };

    /* DownstreamConfiguration, The Hermes Standard 3.13 */
    struct HermesDownstreamConfiguration
    {
        uint32_t m_downstreamLaneId;
        HermesStringView m_optionalDownstreamInterfaceId;
        HermesStringView m_optionalClientAddress;
        uint16_t m_port;
    };

    /* The Hermes Standard 3.13 */
    struct HermesDownstreamConfigurations
    {
        const HermesDownstreamConfiguration** m_pData; // pointer to first element
        size_t m_size; // number of elements
    };

    /* SetConfiguration, The Hermes Standard 3.13 */
    struct HermesSetConfigurationData
    {
        HermesStringView m_machineId;
    const uint16_t* m_pOptionalSupervisorySystemPort;
        HermesUpstreamConfigurations m_upstreamConfigurations;
        HermesDownstreamConfigurations m_downstreamConfigurations;
    };

    /* GetConfiguration, The Hermes Standard 3.14 */
    struct HermesGetConfigurationData
{
};

    /* CurrentConfiguration, The Hermes Standard 3.15 */
    struct HermesCurrentConfigurationData
    {
        HermesStringView m_optionalMachineId;
    const uint16_t* m_pOptionalSupervisorySystemPort;
        HermesUpstreamConfigurations m_upstreamConfigurations;
        HermesDownstreamConfigurations m_downstreamConfigurations;
    };

    /* BoardForecast, The Hermes Standard 3.16 */
    struct HermesBoardForecastData
    {
        HermesStringView m_optionalForecastId;
        const double* m_pOptionalTimeUntilAvailableInSeconds;
        HermesStringView m_optionalBoardId;
        HermesStringView m_optionalBoardIdCreatedBy;
        EHermesBoardQuality m_failedBoard;
        HermesStringView m_optionalProductTypeId;
        EHermesFlippedBoard m_flippedBoard;
        HermesStringView m_optionalTopBarcode;
        HermesStringView m_optionalBottomBarcode;
        const double* m_pOptionalLengthInMM;
        const double* m_pOptionalWidthInMM;
        const double* m_pOptionalThicknessInMM;
        const double* m_pOptionalConveyorSpeedInMMPerSecs;
        const double* m_pOptionalTopClearanceHeightInMM;
        const double* m_pOptionalBottomClearanceHeightInMM;
        const double* m_pOptionalWeightInGrams;
    HermesStringView m_optionalWorkOrderId;
    HermesStringView m_optionalBatchId;
    };

    /* QueryBoardInfo, The Hermes Standard 3.17 */
    struct HermesQueryBoardInfoData
    {
        HermesStringView m_optionalTopBarcode;
        HermesStringView m_optionalBottomBarcode;
    };

    /* SendBoardInfo, The Hermes Standard 3.18 */
    struct HermesSendBoardInfoData
    {
        HermesStringView m_optionalBoardId;
        HermesStringView m_optionalBoardIdCreatedBy;
        const EHermesBoardQuality* m_pOptionalFailedBoard;
        HermesStringView m_optionalProductTypeId;
        const EHermesFlippedBoard* m_pOptionalFlippedBoard;
        HermesStringView m_optionalTopBarcode;
        HermesStringView m_optionalBottomBarcode;
        const double* m_pOptionalLengthInMM;
        const double* m_pOptionalWidthInMM;
        const double* m_pOptionalThicknessInMM;
        const double* m_pOptionalConveyorSpeedInMMPerSecs;
        const double* m_pOptionalTopClearanceHeightInMM;
        const double* m_pOptionalBottomClearanceHeightInMM;
        const double* m_pOptionalWeightInGrams;
    HermesStringView m_optionalWorkOrderId;
    HermesStringView m_optionalBatchId;
};

/* FeatureConfiguration, The Hermes Standard 3.22 */
struct HermesFeatureConfiguration
{
};

/* FeatureBoardTracking, The Hermes Standard 3.22 */
struct HermesFeatureBoardTracking
{
};

/* FeatureQueryWorkOrderInfo, The Hermes Standard 3.22 */
struct HermesFeatureQueryWorkOrderInfo
{
};

/* FeatureSendWorkOrderInfo, The Hermes Standard 3.22 */
struct HermesFeatureSendWorkOrderInfo
{
};

/* SupervisoryFeatures, The Hermes Standard 3.22 */
struct HermesSupervisoryFeatures
{
    const HermesFeatureConfiguration* m_pOptionalFeatureConfiguration;
    const HermesFeatureCheckAliveResponse* m_pOptionalFeatureCheckAliveResponse;
    const HermesFeatureBoardTracking* m_pOptionalFeatureBoardTracking;
    const HermesFeatureQueryWorkOrderInfo* m_pOptionalFeatureQueryWorkOrderInfo;
    const HermesFeatureSendWorkOrderInfo* m_pOptionalFeatureSendWorkOrderInfo;
};

/* SupervisoryServiceDescription, The Hermes Standard 3.22 */
struct HermesSupervisoryServiceDescriptionData
{
    HermesStringView m_systemId;
    HermesStringView m_version;
    const HermesSupervisoryFeatures* m_pSupportedFeatures;
};

/* BoardArrived, The Hermes Standard 3.23 */
struct HermesBoardArrivedData
{
    HermesStringView m_machineId;
    uint32_t m_upstreamLaneId;
    HermesStringView m_optionalUpstreamInterfaceId;
    HermesStringView m_optionalMagazineId;
    const uint32_t* m_pOptionalSlotId;
    EHermesBoardArrivedTransfer m_boardTransfer;
    HermesStringView m_boardId;
    HermesStringView m_boardIdCreatedBy;
    EHermesBoardQuality m_failedBoard;
    HermesStringView m_optionalProductTypeId;
    EHermesFlippedBoard m_flippedBoard;
    HermesStringView m_optionalTopBarcode;
    HermesStringView m_optionalBottomBarcode;
    const double* m_pOptionalLengthInMM;
    const double* m_pOptionalWidthInMM;
    const double* m_pOptionalThicknessInMM;
    const double* m_pOptionalConveyorSpeedInMMPerSecs;
    const double* m_pOptionalTopClearanceHeightInMM;
    const double* m_pOptionalBottomClearanceHeightInMM;
    const double* m_pOptionalWeightInGrams;
    HermesStringView m_optionalWorkOrderId;
    HermesStringView m_optionalBatchId;
};

/* BoardDeparted, The Hermes Standard 3.24 */
struct HermesBoardDepartedData
{
    HermesStringView m_machineId;
    uint32_t m_downstreamLaneId;
    HermesStringView m_optionalDownstreamInterfaceId;
    HermesStringView m_optionalMagazineId;
    const uint32_t* m_pOptionalSlotId;
    EHermesBoardDepartedTransfer m_boardTransfer;
    HermesStringView m_boardId;
    HermesStringView m_boardIdCreatedBy;
    EHermesBoardQuality m_failedBoard;
    HermesStringView m_optionalProductTypeId;
    EHermesFlippedBoard m_flippedBoard;
    HermesStringView m_optionalTopBarcode;
    HermesStringView m_optionalBottomBarcode;
    const double* m_pOptionalLengthInMM;
    const double* m_pOptionalWidthInMM;
    const double* m_pOptionalThicknessInMM;
    const double* m_pOptionalConveyorSpeedInMMPerSecs;
    const double* m_pOptionalTopClearanceHeightInMM;
    const double* m_pOptionalBottomClearanceHeightInMM;
    const double* m_pOptionalWeightInGrams;
    HermesStringView m_optionalWorkOrderId;
    HermesStringView m_optionalBatchId;
};

/* QueryWorkOrderInfo, The Hermes Standard 3.25 */
struct HermesQueryWorkOrderInfoData
{
    HermesStringView m_optionalQueryId;
    HermesStringView m_machineId;
    HermesStringView m_optionalMagazineId;
    const uint32_t* m_pOptionalSlotId;
    HermesStringView m_optionalBarcode;
};

/* SendWorkOrderInfo, The Hermes Standard 3.26 */
struct HermesSendWorkOrderInfoData
{
    HermesStringView m_optionalQueryId;
    HermesStringView m_optionalWorkOrderId;
    HermesStringView m_optionalBatchId;
    HermesStringView m_optionalBoardId;
    HermesStringView m_optionalBoardIdCreatedBy;
    const EHermesBoardQuality* m_pOptionalFailedBoard;
    HermesStringView m_optionalProductTypeId;
    const EHermesFlippedBoard* m_pOptionalFlippedBoard;
    HermesStringView m_optionalTopBarcode;
    HermesStringView m_optionalBottomBarcode;
    const double* m_pOptionalLengthInMM;
    const double* m_pOptionalWidthInMM;
    const double* m_pOptionalThicknessInMM;
    const double* m_pOptionalConveyorSpeedInMMPerSecs;
    const double* m_pOptionalTopClearanceHeightInMM;
    const double* m_pOptionalBottomClearanceHeightInMM;
    const double* m_pOptionalWeightInGrams;
    };

    /* UpstreamSettings, Configuration of upstream interface (not part of The Hermes Standard) */
    struct HermesUpstreamSettings
    {
        HermesStringView m_machineId;
        HermesStringView m_hostAddress;
        uint16_t m_port;
        double m_checkAlivePeriodInSeconds;
        double m_reconnectWaitTimeInSeconds;
        EHermesCheckAliveResponseMode m_checkAliveResponseMode;
        EHermesCheckState m_checkState;
    };

    /* DownstreamSettings, Configuration of downstream interface (not part of The Hermes Standard) */
    struct HermesDownstreamSettings
    {
        HermesStringView m_machineId;
        HermesStringView m_optionalClientAddress;
        uint16_t m_port;
        double m_checkAlivePeriodInSeconds;
        double m_reconnectWaitTimeInSeconds;
        EHermesCheckAliveResponseMode m_checkAliveResponseMode;
        EHermesCheckState m_checkState;
    };

    /* ConfigurationServiceSettings, Configuration of configuration service interface (not part of The Hermes Standard) */
    struct HermesConfigurationServiceSettings
    {
        uint16_t m_port;
        double m_reconnectWaitTimeInSeconds;
    };

/* VerticalServiceSettings, Configuration of vertical service interface (not part of The Hermes Standard) */
struct HermesVerticalServiceSettings
{
    HermesStringView m_systemId;
    uint16_t m_port;
    double m_reconnectWaitTimeInSeconds;
    double m_checkAlivePeriodInSeconds;
    EHermesCheckAliveResponseMode m_checkAliveResponseMode;
};

/* VerticalClientSettings, Configuration of vertical client interface (not part of The Hermes Standard) */
struct HermesVerticalClientSettings
{
    HermesStringView m_systemId;
    HermesStringView m_hostAddress;
    uint16_t m_port;
    double m_reconnectWaitTimeInSeconds;
    double m_checkAlivePeriodInSeconds;
    EHermesCheckAliveResponseMode m_checkAliveResponseMode;
};

    /* Error, Error object (not part of The Hermes Standard) */
    struct HermesError
    {
        EHermesErrorCode m_code;
        HermesStringView m_text;
    };

    /* ConnectionInfo, Attributes for the established connection (not part of The Hermes Standard) */
    struct HermesConnectionInfo
    {
        HermesStringView m_address;
        uint16_t m_port;
        HermesStringView m_hostName;
    };

#ifdef __cplusplus
}
#endif

#endif //#define HERMESDATA_H
