// Copyright (c) ASM Assembly Systems GmbH & Co. KG
//
// C++ interface for an implementation of The Hermes Standard
//
#pragma once

#include "HermesOptional.hpp"
#include "HermesStringView.hpp"

#include <string>
#include <type_traits>
#include <vector>

namespace Hermes {

static const uint32_t cMAX_MESSAGE_SIZE = 65536U; // The Hermes Standard 3.1
static const uint16_t cBASE_PORT = 50100U; // The Hermes Standard 2.3.1
static const uint16_t cCONFIG_PORT = 1248U; // The Hermes Standard 2.4.1
static const uint16_t cROUTE_RETURN_BOARD = 900U; // The Hermes Standard 3.6
static const uint16_t cROUTE_UNDEFINED = 999U; // The Hermes Standard 3.6
static const uint16_t cROUTE_MANUALLY_REMOVE_BOARD = 999U; // The Hermes Standard 3.6
static const uint16_t cACTION_UNDEFINED = 1U; // The Hermes Standard 3.6
static const uint16_t cACTION_PROCESS_BOARD = 1U; // The Hermes Standard 3.6
static const uint16_t cACTION_PASS_THROUGH_BOARD = 2U; // The Hermes Standard 3.6


//========== The Hermes Standard 3.3 ==========
enum class ECheckAliveType
{
    eUNKNOWN,
    ePING,
    ePONG
};
template<class S>
S& operator<<(S& s, ECheckAliveType e)
{
   switch(e)
   {
        case ECheckAliveType::eUNKNOWN: s << "eUNKNOWN"; return s;
        case ECheckAliveType::ePING: s << "ePING"; return s;
        case ECheckAliveType::ePONG: s << "ePONG"; return s;
        default: s << "INVALID_CHECK_ALIVE_TYPE: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(ECheckAliveType) { return 3; }

//========== The Hermes Standard 3.5 ==========
enum class ENotificationCode
{
    eUNSPECIFIC,
    ePROTOCOL_ERROR,
    eCONNECTION_REFUSED_BECAUSE_OF_ESTABLISHED_CONNECTION,
    eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION,
    eCONFIGURATION_ERROR,
    eMACHINE_SHUTDOWN,
    eBOARD_FORECAST_ERROR
};
template<class S>
S& operator<<(S& s, ENotificationCode e)
{
   switch(e)
   {
        case ENotificationCode::eUNSPECIFIC: s << "eUNSPECIFIC"; return s;
        case ENotificationCode::ePROTOCOL_ERROR: s << "ePROTOCOL_ERROR"; return s;
        case ENotificationCode::eCONNECTION_REFUSED_BECAUSE_OF_ESTABLISHED_CONNECTION: s << "eCONNECTION_REFUSED_BECAUSE_OF_ESTABLISHED_CONNECTION"; return s;
        case ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION: s << "eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION"; return s;
        case ENotificationCode::eCONFIGURATION_ERROR: s << "eCONFIGURATION_ERROR"; return s;
        case ENotificationCode::eMACHINE_SHUTDOWN: s << "eMACHINE_SHUTDOWN"; return s;
        case ENotificationCode::eBOARD_FORECAST_ERROR: s << "eBOARD_FORECAST_ERROR"; return s;
        default: s << "INVALID_NOTIFICATION_CODE: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(ENotificationCode) { return 7; }

//========== The Hermes Standard 3.5 ==========
enum class ESeverity
{
    eUNKNOWN,
    eFATAL,
    eERROR,
    eWARNING,
    eINFO
};
template<class S>
S& operator<<(S& s, ESeverity e)
{
   switch(e)
   {
        case ESeverity::eUNKNOWN: s << "eUNKNOWN"; return s;
        case ESeverity::eFATAL: s << "eFATAL"; return s;
        case ESeverity::eERROR: s << "eERROR"; return s;
        case ESeverity::eWARNING: s << "eWARNING"; return s;
        case ESeverity::eINFO: s << "eINFO"; return s;
        default: s << "INVALID_SEVERITY: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(ESeverity) { return 5; }

//========== The Hermes Standard 3.6 ==========
enum class EBoardQuality
{
    eANY,
    eGOOD,
    eBAD
};
template<class S>
S& operator<<(S& s, EBoardQuality e)
{
   switch(e)
   {
        case EBoardQuality::eANY: s << "eANY"; return s;
        case EBoardQuality::eGOOD: s << "eGOOD"; return s;
        case EBoardQuality::eBAD: s << "eBAD"; return s;
        default: s << "INVALID_BOARD_QUALITY: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(EBoardQuality) { return 3; }

//========== The Hermes Standard 3.6 ==========
enum class EFlippedBoard
{
    eSIDE_UP_IS_UNKNOWN,
    eTOP_SIDE_IS_UP,
    eBOTTOM_SIDE_IS_UP
};
template<class S>
S& operator<<(S& s, EFlippedBoard e)
{
   switch(e)
   {
        case EFlippedBoard::eSIDE_UP_IS_UNKNOWN: s << "eSIDE_UP_IS_UNKNOWN"; return s;
        case EFlippedBoard::eTOP_SIDE_IS_UP: s << "eTOP_SIDE_IS_UP"; return s;
        case EFlippedBoard::eBOTTOM_SIDE_IS_UP: s << "eBOTTOM_SIDE_IS_UP"; return s;
        default: s << "INVALID_FLIPPED_BOARD: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(EFlippedBoard) { return 3; }

//========== The Hermes Standard 3.6 ==========
enum class ESubBoardState
{
    eUNKNOWN,
    eGOOD,
    eFAILED,
    eMISSING,
    eSKIP
};
template<class S>
S& operator<<(S& s, ESubBoardState e)
{
   switch(e)
   {
        case ESubBoardState::eUNKNOWN: s << "eUNKNOWN"; return s;
        case ESubBoardState::eGOOD: s << "eGOOD"; return s;
        case ESubBoardState::eFAILED: s << "eFAILED"; return s;
        case ESubBoardState::eMISSING: s << "eMISSING"; return s;
        case ESubBoardState::eSKIP: s << "eSKIP"; return s;
        default: s << "INVALID_SUB_BOARD_STATE: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(ESubBoardState) { return 5; }

//========== The Hermes Standard 3.11 ==========
enum class ETransferState
{
    eUNKNOWN,
    eNOT_STARTED,
    eINCOMPLETE,
    eCOMPLETE
};
template<class S>
S& operator<<(S& s, ETransferState e)
{
   switch(e)
   {
        case ETransferState::eUNKNOWN: s << "eUNKNOWN"; return s;
        case ETransferState::eNOT_STARTED: s << "eNOT_STARTED"; return s;
        case ETransferState::eINCOMPLETE: s << "eINCOMPLETE"; return s;
        case ETransferState::eCOMPLETE: s << "eCOMPLETE"; return s;
        default: s << "INVALID_TRANSFER_STATE: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(ETransferState) { return 4; }

//========== The Hermes Standard 3.23 ==========
enum class EBoardArrivedTransfer
{
    eUNKNOWN,
    eTRANSFERRED,
    eLOADED,
    eINSERTED
};
template<class S>
S& operator<<(S& s, EBoardArrivedTransfer e)
{
   switch(e)
   {
        case EBoardArrivedTransfer::eUNKNOWN: s << "eUNKNOWN"; return s;
        case EBoardArrivedTransfer::eTRANSFERRED: s << "eTRANSFERRED"; return s;
        case EBoardArrivedTransfer::eLOADED: s << "eLOADED"; return s;
        case EBoardArrivedTransfer::eINSERTED: s << "eINSERTED"; return s;
        default: s << "INVALID_BOARD_ARRIVED_TRANSFER: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(EBoardArrivedTransfer) { return 4; }

//========== The Hermes Standard 3.24 ==========
enum class EBoardDepartedTransfer
{
    eUNKNOWN,
    eTRANSFERRED,
    eUNLOADED,
    eREMOVED
};
template<class S>
S& operator<<(S& s, EBoardDepartedTransfer e)
{
   switch(e)
   {
        case EBoardDepartedTransfer::eUNKNOWN: s << "eUNKNOWN"; return s;
        case EBoardDepartedTransfer::eTRANSFERRED: s << "eTRANSFERRED"; return s;
        case EBoardDepartedTransfer::eUNLOADED: s << "eUNLOADED"; return s;
        case EBoardDepartedTransfer::eREMOVED: s << "eREMOVED"; return s;
        default: s << "INVALID_BOARD_DEPARTED_TRANSFER: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(EBoardDepartedTransfer) { return 4; }

//========== The Hermes Standard 3.27 ==========
enum class EReplyWorkOrderInfoStatus
{
    eREJECTED,
    eACCEPTED_AND_READY,
    eACCEPTED_AND_QUEUED
};
template<class S>
S& operator<<(S& s, EReplyWorkOrderInfoStatus e)
{
   switch(e)
   {
        case EReplyWorkOrderInfoStatus::eREJECTED: s << "eREJECTED"; return s;
        case EReplyWorkOrderInfoStatus::eACCEPTED_AND_READY: s << "eACCEPTED_AND_READY"; return s;
        case EReplyWorkOrderInfoStatus::eACCEPTED_AND_QUEUED: s << "eACCEPTED_AND_QUEUED"; return s;
        default: s << "INVALID_REPLY_WORK_ORDER_INFO_STATUS: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(EReplyWorkOrderInfoStatus) { return 3; }

//========== The Hermes Standard chapter 2.6 ==========
enum class EState
{
    eNOT_CONNECTED,
    eSOCKET_CONNECTED,
    eSERVICE_DESCRIPTION_DOWNSTREAM,
    eNOT_AVAILABLE_NOT_READY,
    eBOARD_AVAILABLE,
    eMACHINE_READY,
    eAVAILABLE_AND_READY,
    eTRANSPORTING,
    eTRANSPORT_STOPPED,
    eTRANSPORT_FINISHED,
    eDISCONNECTED
};
template<class S>
S& operator<<(S& s, EState e)
{
   switch(e)
   {
        case EState::eNOT_CONNECTED: s << "eNOT_CONNECTED"; return s;
        case EState::eSOCKET_CONNECTED: s << "eSOCKET_CONNECTED"; return s;
        case EState::eSERVICE_DESCRIPTION_DOWNSTREAM: s << "eSERVICE_DESCRIPTION_DOWNSTREAM"; return s;
        case EState::eNOT_AVAILABLE_NOT_READY: s << "eNOT_AVAILABLE_NOT_READY"; return s;
        case EState::eBOARD_AVAILABLE: s << "eBOARD_AVAILABLE"; return s;
        case EState::eMACHINE_READY: s << "eMACHINE_READY"; return s;
        case EState::eAVAILABLE_AND_READY: s << "eAVAILABLE_AND_READY"; return s;
        case EState::eTRANSPORTING: s << "eTRANSPORTING"; return s;
        case EState::eTRANSPORT_STOPPED: s << "eTRANSPORT_STOPPED"; return s;
        case EState::eTRANSPORT_FINISHED: s << "eTRANSPORT_FINISHED"; return s;
        case EState::eDISCONNECTED: s << "eDISCONNECTED"; return s;
        default: s << "INVALID_STATE: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(EState) { return 11; }

//========== Trace levels of the implementation (not part of The Hermes Standard) ==========
enum class ETraceType
{
    eSENT, // raw message sent
    eRECEIVED, // raw message received
    eDEBUG,
    eINFO,
    eWARNING,
    eERROR
};
template<class S>
S& operator<<(S& s, ETraceType e)
{
   switch(e)
   {
        case ETraceType::eSENT: s << "eSENT"; return s;
        case ETraceType::eRECEIVED: s << "eRECEIVED"; return s;
        case ETraceType::eDEBUG: s << "eDEBUG"; return s;
        case ETraceType::eINFO: s << "eINFO"; return s;
        case ETraceType::eWARNING: s << "eWARNING"; return s;
        case ETraceType::eERROR: s << "eERROR"; return s;
        default: s << "INVALID_TRACE_TYPE: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(ETraceType) { return 6; }

//========== Internal state check modes of the implementation (not part of The Hermes Standard) ==========
enum class ECheckState
{
    eSEND_AND_RECEIVE, // check sent and received messages for conformance with state machine
    eONLY_RECEIVE // check only received message for conformance with state machine
};
template<class S>
S& operator<<(S& s, ECheckState e)
{
   switch(e)
   {
        case ECheckState::eSEND_AND_RECEIVE: s << "eSEND_AND_RECEIVE"; return s;
        case ECheckState::eONLY_RECEIVE: s << "eONLY_RECEIVE"; return s;
        default: s << "INVALID_CHECK_STATE: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(ECheckState) { return 2; }

//========== How to respond to a check alive ping) ==========
enum class ECheckAliveResponseMode
{
    eAUTO, // automatically respond to a check alive ping with a pong
    eAPPLICATION // let the application respond with a pong
};
template<class S>
S& operator<<(S& s, ECheckAliveResponseMode e)
{
   switch(e)
   {
        case ECheckAliveResponseMode::eAUTO: s << "eAUTO"; return s;
        case ECheckAliveResponseMode::eAPPLICATION: s << "eAPPLICATION"; return s;
        default: s << "INVALID_CHECK_ALIVE_RESPONSE_MODE: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(ECheckAliveResponseMode) { return 2; }

//========== Error codes (not part of The Hermes Standard) ==========
enum class EErrorCode
{
    eSUCCESS,
    eIMPLEMENTATION_ERROR, // error inside the Hermes DLL
    ePEER_ERROR, // the remote peer has misbehaved
    eCLIENT_ERROR, // the client code making the API calls has misbehaved
    eNETWORK_ERROR, // something is wrong with the network or its configuration
    eTIMEOUT // the specified timeout has been exceeded
};
template<class S>
S& operator<<(S& s, EErrorCode e)
{
   switch(e)
   {
        case EErrorCode::eSUCCESS: s << "eSUCCESS"; return s;
        case EErrorCode::eIMPLEMENTATION_ERROR: s << "eIMPLEMENTATION_ERROR"; return s;
        case EErrorCode::ePEER_ERROR: s << "ePEER_ERROR"; return s;
        case EErrorCode::eCLIENT_ERROR: s << "eCLIENT_ERROR"; return s;
        case EErrorCode::eNETWORK_ERROR: s << "eNETWORK_ERROR"; return s;
        case EErrorCode::eTIMEOUT: s << "eTIMEOUT"; return s;
        default: s << "INVALID_ERROR_CODE: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(EErrorCode) { return 6; }

//========== The Hermes Standard chapter 2.5.3 ==========
enum class EVerticalState
{
    eNOT_CONNECTED,
    eSOCKET_CONNECTED,
    eSUPERVISORY_SERVICE_DESCRIPTION,
    eCONNECTED,
    eDISCONNECTED
};
template<class S>
S& operator<<(S& s, EVerticalState e)
{
   switch(e)
   {
        case EVerticalState::eNOT_CONNECTED: s << "eNOT_CONNECTED"; return s;
        case EVerticalState::eSOCKET_CONNECTED: s << "eSOCKET_CONNECTED"; return s;
        case EVerticalState::eSUPERVISORY_SERVICE_DESCRIPTION: s << "eSUPERVISORY_SERVICE_DESCRIPTION"; return s;
        case EVerticalState::eCONNECTED: s << "eCONNECTED"; return s;
        case EVerticalState::eDISCONNECTED: s << "eDISCONNECTED"; return s;
        default: s << "INVALID_VERTICAL_STATE: " << static_cast<int>(e); return s;
    }
}
inline constexpr std::size_t size(EVerticalState) { return 5; }

//========== The Hermes Standard 3.3 ==========
struct CheckAliveData
{
    Optional<ECheckAliveType> m_optionalType;
    Optional<std::string> m_optionalId;

    friend bool operator==(const CheckAliveData& lhs, const CheckAliveData& rhs)
    {
        return lhs.m_optionalType == rhs.m_optionalType
            && lhs.m_optionalId == rhs.m_optionalId;
    }
    friend bool operator!=(const CheckAliveData& lhs, const CheckAliveData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const CheckAliveData& data) 
    {
        s << '{';
        if (data.m_optionalType) { s << " Type=" << *data.m_optionalType; }
        if (data.m_optionalId) { s << " Id=" << *data.m_optionalId; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.4 ==========
struct FeatureBoardForecast
{
    friend bool operator==(const FeatureBoardForecast&, const FeatureBoardForecast&) { return true; }
    friend bool operator!=(const FeatureBoardForecast&, const FeatureBoardForecast&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureBoardForecast&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.4 ==========
struct FeatureCheckAliveResponse
{
    friend bool operator==(const FeatureCheckAliveResponse&, const FeatureCheckAliveResponse&) { return true; }
    friend bool operator!=(const FeatureCheckAliveResponse&, const FeatureCheckAliveResponse&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureCheckAliveResponse&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.4 ==========
struct FeatureQueryBoardInfo
{
    friend bool operator==(const FeatureQueryBoardInfo&, const FeatureQueryBoardInfo&) { return true; }
    friend bool operator!=(const FeatureQueryBoardInfo&, const FeatureQueryBoardInfo&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureQueryBoardInfo&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.4 ==========
struct FeatureSendBoardInfo
{
    friend bool operator==(const FeatureSendBoardInfo&, const FeatureSendBoardInfo&) { return true; }
    friend bool operator!=(const FeatureSendBoardInfo&, const FeatureSendBoardInfo&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureSendBoardInfo&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.4 ==========
struct FeatureCommand
{
    friend bool operator==(const FeatureCommand&, const FeatureCommand&) { return true; }
    friend bool operator!=(const FeatureCommand&, const FeatureCommand&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureCommand&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.4 ==========
struct SupportedFeatures
{
    Optional<FeatureBoardForecast> m_optionalFeatureBoardForecast;
    Optional<FeatureCheckAliveResponse> m_optionalFeatureCheckAliveResponse;
    Optional<FeatureQueryBoardInfo> m_optionalFeatureQueryBoardInfo;
    Optional<FeatureSendBoardInfo> m_optionalFeatureSendBoardInfo;
    Optional<FeatureCommand> m_optionalFeatureCommand;

    friend bool operator==(const SupportedFeatures& lhs, const SupportedFeatures& rhs)
    {
        return lhs.m_optionalFeatureBoardForecast == rhs.m_optionalFeatureBoardForecast
            && lhs.m_optionalFeatureCheckAliveResponse == rhs.m_optionalFeatureCheckAliveResponse
            && lhs.m_optionalFeatureQueryBoardInfo == rhs.m_optionalFeatureQueryBoardInfo
            && lhs.m_optionalFeatureSendBoardInfo == rhs.m_optionalFeatureSendBoardInfo
            && lhs.m_optionalFeatureCommand == rhs.m_optionalFeatureCommand;
    }
    friend bool operator!=(const SupportedFeatures& lhs, const SupportedFeatures& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const SupportedFeatures& data) 
    {
        s << '{';
        if (data.m_optionalFeatureBoardForecast) { s << " FeatureBoardForecast=" << *data.m_optionalFeatureBoardForecast; }
        if (data.m_optionalFeatureCheckAliveResponse) { s << " FeatureCheckAliveResponse=" << *data.m_optionalFeatureCheckAliveResponse; }
        if (data.m_optionalFeatureQueryBoardInfo) { s << " FeatureQueryBoardInfo=" << *data.m_optionalFeatureQueryBoardInfo; }
        if (data.m_optionalFeatureSendBoardInfo) { s << " FeatureSendBoardInfo=" << *data.m_optionalFeatureSendBoardInfo; }
        if (data.m_optionalFeatureCommand) { s << " FeatureCommand=" << *data.m_optionalFeatureCommand; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.4 ==========
struct ServiceDescriptionData
{
    std::string m_machineId;
    unsigned m_laneId{0};
    Optional<std::string> m_optionalInterfaceId;
    std::string m_version{"1.5"};
    SupportedFeatures m_supportedFeatures;

    ServiceDescriptionData() = default;
    ServiceDescriptionData(StringView machineId,
        unsigned laneId) :
        m_machineId(machineId),
        m_laneId(laneId)
    {}

    friend bool operator==(const ServiceDescriptionData& lhs, const ServiceDescriptionData& rhs)
    {
        return lhs.m_machineId == rhs.m_machineId
            && lhs.m_laneId == rhs.m_laneId
            && lhs.m_optionalInterfaceId == rhs.m_optionalInterfaceId
            && lhs.m_version == rhs.m_version
            && lhs.m_supportedFeatures == rhs.m_supportedFeatures;
    }
    friend bool operator!=(const ServiceDescriptionData& lhs, const ServiceDescriptionData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const ServiceDescriptionData& data) 
    {
        s << '{';
        s << " MachineId=" << data.m_machineId;
        s << " LaneId=" << data.m_laneId;
        if (data.m_optionalInterfaceId) { s << " InterfaceId=" << *data.m_optionalInterfaceId; }
        s << " Version=" << data.m_version;
        s << " SupportedFeatures=" << data.m_supportedFeatures;
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.5 ==========
struct NotificationData
{
    ENotificationCode m_notificationCode{ENotificationCode::eUNSPECIFIC};
    ESeverity m_severity{ESeverity::eUNKNOWN};
    std::string m_description;

    NotificationData() = default;
    NotificationData(ENotificationCode notificationCode,
        ESeverity severity,
        StringView description) :
        m_notificationCode(notificationCode),
        m_severity(severity),
        m_description(description)
    {}

    friend bool operator==(const NotificationData& lhs, const NotificationData& rhs)
    {
        return lhs.m_notificationCode == rhs.m_notificationCode
            && lhs.m_severity == rhs.m_severity
            && lhs.m_description == rhs.m_description;
    }
    friend bool operator!=(const NotificationData& lhs, const NotificationData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const NotificationData& data) 
    {
        s << '{';
        s << " NotificationCode=" << data.m_notificationCode;
        s << " Severity=" << data.m_severity;
        s << " Description=" << data.m_description;
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.6 ==========
struct SubBoard
{
    uint16_t m_pos{0};
    Optional<std::string> m_optionalBc;
    ESubBoardState m_st{ESubBoardState::eUNKNOWN};

    SubBoard() = default;
    SubBoard(uint16_t pos,
        ESubBoardState st) :
        m_pos(pos),
        m_st(st)
    {}

    friend bool operator==(const SubBoard& lhs, const SubBoard& rhs)
    {
        return lhs.m_pos == rhs.m_pos
            && lhs.m_optionalBc == rhs.m_optionalBc
            && lhs.m_st == rhs.m_st;
    }
    friend bool operator!=(const SubBoard& lhs, const SubBoard& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const SubBoard& data) 
    {
        s << '{';
        s << " Pos=" << data.m_pos;
        if (data.m_optionalBc) { s << " Bc=" << *data.m_optionalBc; }
        s << " St=" << data.m_st;
        s << " }";
        return s;
    }
};

/* The Hermes Standard 3.6 */
using SubBoards = std::vector<SubBoard>;
inline std::ostream& operator<<(std::ostream& s, const SubBoards& data) 
{
    s << '[';
    if (!data.empty()) { s << ' '; }
    for (const auto& item : data)
    {
        s << item << ' ';
    }
    s << ']';
    return s;
}

//========== The Hermes Standard 3.6 ==========
struct BoardAvailableData
{
    std::string m_boardId{"00000000-0000-0000-0000-000000000000"};
    std::string m_boardIdCreatedBy;
    EBoardQuality m_failedBoard{EBoardQuality::eANY};
    Optional<std::string> m_optionalProductTypeId;
    EFlippedBoard m_flippedBoard{EFlippedBoard::eSIDE_UP_IS_UNKNOWN};
    Optional<std::string> m_optionalTopBarcode;
    Optional<std::string> m_optionalBottomBarcode;
    Optional<double> m_optionalLengthInMM;
    Optional<double> m_optionalWidthInMM;
    Optional<double> m_optionalThicknessInMM;
    Optional<double> m_optionalConveyorSpeedInMMPerSecs;
    Optional<double> m_optionalTopClearanceHeightInMM;
    Optional<double> m_optionalBottomClearanceHeightInMM;
    Optional<double> m_optionalWeightInGrams;
    Optional<std::string> m_optionalWorkOrderId;
    Optional<std::string> m_optionalBatchId;
    Optional<uint16_t> m_optionalRoute;
    Optional<uint16_t> m_optionalAction;
    SubBoards m_optionalSubBoards;

    BoardAvailableData() = default;
    BoardAvailableData(StringView boardId,
        StringView boardIdCreatedBy,
        EBoardQuality failedBoard,
        EFlippedBoard flippedBoard) :
        m_boardId(boardId),
        m_boardIdCreatedBy(boardIdCreatedBy),
        m_failedBoard(failedBoard),
        m_flippedBoard(flippedBoard)
    {}

    friend bool operator==(const BoardAvailableData& lhs, const BoardAvailableData& rhs)
    {
        return lhs.m_boardId == rhs.m_boardId
            && lhs.m_boardIdCreatedBy == rhs.m_boardIdCreatedBy
            && lhs.m_failedBoard == rhs.m_failedBoard
            && lhs.m_optionalProductTypeId == rhs.m_optionalProductTypeId
            && lhs.m_flippedBoard == rhs.m_flippedBoard
            && lhs.m_optionalTopBarcode == rhs.m_optionalTopBarcode
            && lhs.m_optionalBottomBarcode == rhs.m_optionalBottomBarcode
            && lhs.m_optionalLengthInMM == rhs.m_optionalLengthInMM
            && lhs.m_optionalWidthInMM == rhs.m_optionalWidthInMM
            && lhs.m_optionalThicknessInMM == rhs.m_optionalThicknessInMM
            && lhs.m_optionalConveyorSpeedInMMPerSecs == rhs.m_optionalConveyorSpeedInMMPerSecs
            && lhs.m_optionalTopClearanceHeightInMM == rhs.m_optionalTopClearanceHeightInMM
            && lhs.m_optionalBottomClearanceHeightInMM == rhs.m_optionalBottomClearanceHeightInMM
            && lhs.m_optionalWeightInGrams == rhs.m_optionalWeightInGrams
            && lhs.m_optionalWorkOrderId == rhs.m_optionalWorkOrderId
            && lhs.m_optionalBatchId == rhs.m_optionalBatchId
            && lhs.m_optionalRoute == rhs.m_optionalRoute
            && lhs.m_optionalAction == rhs.m_optionalAction
            && lhs.m_optionalSubBoards == rhs.m_optionalSubBoards;
    }
    friend bool operator!=(const BoardAvailableData& lhs, const BoardAvailableData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const BoardAvailableData& data) 
    {
        s << '{';
        s << " BoardId=" << data.m_boardId;
        s << " BoardIdCreatedBy=" << data.m_boardIdCreatedBy;
        s << " FailedBoard=" << data.m_failedBoard;
        if (data.m_optionalProductTypeId) { s << " ProductTypeId=" << *data.m_optionalProductTypeId; }
        s << " FlippedBoard=" << data.m_flippedBoard;
        if (data.m_optionalTopBarcode) { s << " TopBarcode=" << *data.m_optionalTopBarcode; }
        if (data.m_optionalBottomBarcode) { s << " BottomBarcode=" << *data.m_optionalBottomBarcode; }
        if (data.m_optionalLengthInMM) { s << " Length=" << *data.m_optionalLengthInMM; }
        if (data.m_optionalWidthInMM) { s << " Width=" << *data.m_optionalWidthInMM; }
        if (data.m_optionalThicknessInMM) { s << " Thickness=" << *data.m_optionalThicknessInMM; }
        if (data.m_optionalConveyorSpeedInMMPerSecs) { s << " ConveyorSpeed=" << *data.m_optionalConveyorSpeedInMMPerSecs; }
        if (data.m_optionalTopClearanceHeightInMM) { s << " TopClearanceHeight=" << *data.m_optionalTopClearanceHeightInMM; }
        if (data.m_optionalBottomClearanceHeightInMM) { s << " BottomClearanceHeight=" << *data.m_optionalBottomClearanceHeightInMM; }
        if (data.m_optionalWeightInGrams) { s << " Weight=" << *data.m_optionalWeightInGrams; }
        if (data.m_optionalWorkOrderId) { s << " WorkOrderId=" << *data.m_optionalWorkOrderId; }
        if (data.m_optionalBatchId) { s << " BatchId=" << *data.m_optionalBatchId; }
        if (data.m_optionalRoute) { s << " Route=" << *data.m_optionalRoute; }
        if (data.m_optionalAction) { s << " Action=" << *data.m_optionalAction; }
        if (!data.m_optionalSubBoards.empty()) { s << " SubBoards=" << data.m_optionalSubBoards; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.7 ==========
struct RevokeBoardAvailableData
{
    friend bool operator==(const RevokeBoardAvailableData&, const RevokeBoardAvailableData&) { return true; }
    friend bool operator!=(const RevokeBoardAvailableData&, const RevokeBoardAvailableData&) { return false; }
    template <class S> friend S& operator<<(S& s, const RevokeBoardAvailableData&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.8 ==========
struct MachineReadyData
{
    EBoardQuality m_failedBoard{EBoardQuality::eANY};
    Optional<std::string> m_optionalForecastId;
    Optional<std::string> m_optionalBoardId;
    Optional<std::string> m_optionalProductTypeId;
    Optional<EFlippedBoard> m_optionalFlippedBoard;
    Optional<std::string> m_optionalTopBarcode;
    Optional<std::string> m_optionalBottomBarcode;
    Optional<double> m_optionalLengthInMM;
    Optional<double> m_optionalWidthInMM;
    Optional<double> m_optionalThicknessInMM;
    Optional<double> m_optionalConveyorSpeedInMMPerSecs;
    Optional<double> m_optionalTopClearanceHeightInMM;
    Optional<double> m_optionalBottomClearanceHeightInMM;
    Optional<double> m_optionalWeightInGrams;
    Optional<std::string> m_optionalWorkOrderId;
    Optional<std::string> m_optionalBatchId;

    MachineReadyData() = default;
    explicit MachineReadyData(EBoardQuality failedBoard) :
        m_failedBoard(failedBoard)
    {}

    friend bool operator==(const MachineReadyData& lhs, const MachineReadyData& rhs)
    {
        return lhs.m_failedBoard == rhs.m_failedBoard
            && lhs.m_optionalForecastId == rhs.m_optionalForecastId
            && lhs.m_optionalBoardId == rhs.m_optionalBoardId
            && lhs.m_optionalProductTypeId == rhs.m_optionalProductTypeId
            && lhs.m_optionalFlippedBoard == rhs.m_optionalFlippedBoard
            && lhs.m_optionalTopBarcode == rhs.m_optionalTopBarcode
            && lhs.m_optionalBottomBarcode == rhs.m_optionalBottomBarcode
            && lhs.m_optionalLengthInMM == rhs.m_optionalLengthInMM
            && lhs.m_optionalWidthInMM == rhs.m_optionalWidthInMM
            && lhs.m_optionalThicknessInMM == rhs.m_optionalThicknessInMM
            && lhs.m_optionalConveyorSpeedInMMPerSecs == rhs.m_optionalConveyorSpeedInMMPerSecs
            && lhs.m_optionalTopClearanceHeightInMM == rhs.m_optionalTopClearanceHeightInMM
            && lhs.m_optionalBottomClearanceHeightInMM == rhs.m_optionalBottomClearanceHeightInMM
            && lhs.m_optionalWeightInGrams == rhs.m_optionalWeightInGrams
            && lhs.m_optionalWorkOrderId == rhs.m_optionalWorkOrderId
            && lhs.m_optionalBatchId == rhs.m_optionalBatchId;
    }
    friend bool operator!=(const MachineReadyData& lhs, const MachineReadyData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const MachineReadyData& data) 
    {
        s << '{';
        s << " FailedBoard=" << data.m_failedBoard;
        if (data.m_optionalForecastId) { s << " ForecastId=" << *data.m_optionalForecastId; }
        if (data.m_optionalBoardId) { s << " BoardId=" << *data.m_optionalBoardId; }
        if (data.m_optionalProductTypeId) { s << " ProductTypeId=" << *data.m_optionalProductTypeId; }
        if (data.m_optionalFlippedBoard) { s << " FlippedBoard=" << *data.m_optionalFlippedBoard; }
        if (data.m_optionalTopBarcode) { s << " TopBarcode=" << *data.m_optionalTopBarcode; }
        if (data.m_optionalBottomBarcode) { s << " BottomBarcode=" << *data.m_optionalBottomBarcode; }
        if (data.m_optionalLengthInMM) { s << " Length=" << *data.m_optionalLengthInMM; }
        if (data.m_optionalWidthInMM) { s << " Width=" << *data.m_optionalWidthInMM; }
        if (data.m_optionalThicknessInMM) { s << " Thickness=" << *data.m_optionalThicknessInMM; }
        if (data.m_optionalConveyorSpeedInMMPerSecs) { s << " ConveyorSpeed=" << *data.m_optionalConveyorSpeedInMMPerSecs; }
        if (data.m_optionalTopClearanceHeightInMM) { s << " TopClearanceHeight=" << *data.m_optionalTopClearanceHeightInMM; }
        if (data.m_optionalBottomClearanceHeightInMM) { s << " BottomClearanceHeight=" << *data.m_optionalBottomClearanceHeightInMM; }
        if (data.m_optionalWeightInGrams) { s << " Weight=" << *data.m_optionalWeightInGrams; }
        if (data.m_optionalWorkOrderId) { s << " WorkOrderId=" << *data.m_optionalWorkOrderId; }
        if (data.m_optionalBatchId) { s << " BatchId=" << *data.m_optionalBatchId; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.9 ==========
struct RevokeMachineReadyData
{
    friend bool operator==(const RevokeMachineReadyData&, const RevokeMachineReadyData&) { return true; }
    friend bool operator!=(const RevokeMachineReadyData&, const RevokeMachineReadyData&) { return false; }
    template <class S> friend S& operator<<(S& s, const RevokeMachineReadyData&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.10 ==========
struct StartTransportData
{
    std::string m_boardId{"00000000-0000-0000-0000-000000000000"};
    Optional<double> m_optionalConveyorSpeedInMMPerSecs;

    StartTransportData() = default;
    explicit StartTransportData(StringView boardId) :
        m_boardId(boardId)
    {}

    friend bool operator==(const StartTransportData& lhs, const StartTransportData& rhs)
    {
        return lhs.m_boardId == rhs.m_boardId
            && lhs.m_optionalConveyorSpeedInMMPerSecs == rhs.m_optionalConveyorSpeedInMMPerSecs;
    }
    friend bool operator!=(const StartTransportData& lhs, const StartTransportData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const StartTransportData& data) 
    {
        s << '{';
        s << " BoardId=" << data.m_boardId;
        if (data.m_optionalConveyorSpeedInMMPerSecs) { s << " ConveyorSpeed=" << *data.m_optionalConveyorSpeedInMMPerSecs; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.11 ==========
struct StopTransportData
{
    ETransferState m_transferState{ETransferState::eUNKNOWN};
    std::string m_boardId{"00000000-0000-0000-0000-000000000000"};

    StopTransportData() = default;
    StopTransportData(ETransferState transferState,
        StringView boardId) :
        m_transferState(transferState),
        m_boardId(boardId)
    {}

    friend bool operator==(const StopTransportData& lhs, const StopTransportData& rhs)
    {
        return lhs.m_transferState == rhs.m_transferState
            && lhs.m_boardId == rhs.m_boardId;
    }
    friend bool operator!=(const StopTransportData& lhs, const StopTransportData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const StopTransportData& data) 
    {
        s << '{';
        s << " TransferState=" << data.m_transferState;
        s << " BoardId=" << data.m_boardId;
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.12 ==========
struct TransportFinishedData
{
    ETransferState m_transferState{ETransferState::eUNKNOWN};
    std::string m_boardId{"00000000-0000-0000-0000-000000000000"};

    TransportFinishedData() = default;
    TransportFinishedData(ETransferState transferState,
        StringView boardId) :
        m_transferState(transferState),
        m_boardId(boardId)
    {}

    friend bool operator==(const TransportFinishedData& lhs, const TransportFinishedData& rhs)
    {
        return lhs.m_transferState == rhs.m_transferState
            && lhs.m_boardId == rhs.m_boardId;
    }
    friend bool operator!=(const TransportFinishedData& lhs, const TransportFinishedData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const TransportFinishedData& data) 
    {
        s << '{';
        s << " TransferState=" << data.m_transferState;
        s << " BoardId=" << data.m_boardId;
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.13 ==========
struct UpstreamConfiguration
{
    unsigned m_upstreamLaneId{0};
    Optional<std::string> m_optionalUpstreamInterfaceId;
    std::string m_hostAddress;
    uint16_t m_port{0};

    UpstreamConfiguration() = default;
    UpstreamConfiguration(unsigned upstreamLaneId,
        StringView hostAddress,
        uint16_t port) :
        m_upstreamLaneId(upstreamLaneId),
        m_hostAddress(hostAddress),
        m_port(port)
    {}

    friend bool operator==(const UpstreamConfiguration& lhs, const UpstreamConfiguration& rhs)
    {
        return lhs.m_upstreamLaneId == rhs.m_upstreamLaneId
            && lhs.m_optionalUpstreamInterfaceId == rhs.m_optionalUpstreamInterfaceId
            && lhs.m_hostAddress == rhs.m_hostAddress
            && lhs.m_port == rhs.m_port;
    }
    friend bool operator!=(const UpstreamConfiguration& lhs, const UpstreamConfiguration& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const UpstreamConfiguration& data) 
    {
        s << '{';
        s << " UpstreamLaneId=" << data.m_upstreamLaneId;
        if (data.m_optionalUpstreamInterfaceId) { s << " UpstreamInterfaceId=" << *data.m_optionalUpstreamInterfaceId; }
        s << " HostAddress=" << data.m_hostAddress;
        s << " Port=" << data.m_port;
        s << " }";
        return s;
    }
};

/* The Hermes Standard 3.13 */
using UpstreamConfigurations = std::vector<UpstreamConfiguration>;
inline std::ostream& operator<<(std::ostream& s, const UpstreamConfigurations& data) 
{
    s << '[';
    if (!data.empty()) { s << ' '; }
    for (const auto& item : data)
    {
        s << item << ' ';
    }
    s << ']';
    return s;
}

//========== The Hermes Standard 3.13 ==========
struct DownstreamConfiguration
{
    unsigned m_downstreamLaneId{0};
    Optional<std::string> m_optionalDownstreamInterfaceId;
    Optional<std::string> m_optionalClientAddress;
    uint16_t m_port{0};

    DownstreamConfiguration() = default;
    DownstreamConfiguration(unsigned downstreamLaneId,
        uint16_t port) :
        m_downstreamLaneId(downstreamLaneId),
        m_port(port)
    {}

    friend bool operator==(const DownstreamConfiguration& lhs, const DownstreamConfiguration& rhs)
    {
        return lhs.m_downstreamLaneId == rhs.m_downstreamLaneId
            && lhs.m_optionalDownstreamInterfaceId == rhs.m_optionalDownstreamInterfaceId
            && lhs.m_optionalClientAddress == rhs.m_optionalClientAddress
            && lhs.m_port == rhs.m_port;
    }
    friend bool operator!=(const DownstreamConfiguration& lhs, const DownstreamConfiguration& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const DownstreamConfiguration& data) 
    {
        s << '{';
        s << " DownstreamLaneId=" << data.m_downstreamLaneId;
        if (data.m_optionalDownstreamInterfaceId) { s << " DownstreamInterfaceId=" << *data.m_optionalDownstreamInterfaceId; }
        if (data.m_optionalClientAddress) { s << " ClientAddress=" << *data.m_optionalClientAddress; }
        s << " Port=" << data.m_port;
        s << " }";
        return s;
    }
};

/* The Hermes Standard 3.13 */
using DownstreamConfigurations = std::vector<DownstreamConfiguration>;
inline std::ostream& operator<<(std::ostream& s, const DownstreamConfigurations& data) 
{
    s << '[';
    if (!data.empty()) { s << ' '; }
    for (const auto& item : data)
    {
        s << item << ' ';
    }
    s << ']';
    return s;
}

//========== The Hermes Standard 3.13 ==========
struct SetConfigurationData
{
    std::string m_machineId;
    Optional<uint16_t> m_optionalSupervisorySystemPort;
    UpstreamConfigurations m_upstreamConfigurations;
    DownstreamConfigurations m_downstreamConfigurations;

    SetConfigurationData() = default;
    explicit SetConfigurationData(StringView machineId) :
        m_machineId(machineId)
    {}

    friend bool operator==(const SetConfigurationData& lhs, const SetConfigurationData& rhs)
    {
        return lhs.m_machineId == rhs.m_machineId
            && lhs.m_optionalSupervisorySystemPort == rhs.m_optionalSupervisorySystemPort
            && lhs.m_upstreamConfigurations == rhs.m_upstreamConfigurations
            && lhs.m_downstreamConfigurations == rhs.m_downstreamConfigurations;
    }
    friend bool operator!=(const SetConfigurationData& lhs, const SetConfigurationData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const SetConfigurationData& data) 
    {
        s << '{';
        s << " MachineId=" << data.m_machineId;
        if (data.m_optionalSupervisorySystemPort) { s << " SupervisorySystemPort=" << *data.m_optionalSupervisorySystemPort; }
        s << " UpstreamConfigurations=" << data.m_upstreamConfigurations;
        s << " DownstreamConfigurations=" << data.m_downstreamConfigurations;
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.14 ==========
struct GetConfigurationData
{
    friend bool operator==(const GetConfigurationData&, const GetConfigurationData&) { return true; }
    friend bool operator!=(const GetConfigurationData&, const GetConfigurationData&) { return false; }
    template <class S> friend S& operator<<(S& s, const GetConfigurationData&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.15 ==========
struct CurrentConfigurationData
{
    Optional<std::string> m_optionalMachineId;
    Optional<uint16_t> m_optionalSupervisorySystemPort;
    UpstreamConfigurations m_upstreamConfigurations;
    DownstreamConfigurations m_downstreamConfigurations;

    friend bool operator==(const CurrentConfigurationData& lhs, const CurrentConfigurationData& rhs)
    {
        return lhs.m_optionalMachineId == rhs.m_optionalMachineId
            && lhs.m_optionalSupervisorySystemPort == rhs.m_optionalSupervisorySystemPort
            && lhs.m_upstreamConfigurations == rhs.m_upstreamConfigurations
            && lhs.m_downstreamConfigurations == rhs.m_downstreamConfigurations;
    }
    friend bool operator!=(const CurrentConfigurationData& lhs, const CurrentConfigurationData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const CurrentConfigurationData& data) 
    {
        s << '{';
        if (data.m_optionalMachineId) { s << " MachineId=" << *data.m_optionalMachineId; }
        if (data.m_optionalSupervisorySystemPort) { s << " SupervisorySystemPort=" << *data.m_optionalSupervisorySystemPort; }
        s << " UpstreamConfigurations=" << data.m_upstreamConfigurations;
        s << " DownstreamConfigurations=" << data.m_downstreamConfigurations;
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.16 ==========
struct BoardForecastData
{
    Optional<std::string> m_optionalForecastId;
    Optional<double> m_optionalTimeUntilAvailableInSeconds;
    Optional<std::string> m_optionalBoardId;
    Optional<std::string> m_optionalBoardIdCreatedBy;
    EBoardQuality m_failedBoard{EBoardQuality::eANY};
    Optional<std::string> m_optionalProductTypeId;
    EFlippedBoard m_flippedBoard{EFlippedBoard::eSIDE_UP_IS_UNKNOWN};
    Optional<std::string> m_optionalTopBarcode;
    Optional<std::string> m_optionalBottomBarcode;
    Optional<double> m_optionalLengthInMM;
    Optional<double> m_optionalWidthInMM;
    Optional<double> m_optionalThicknessInMM;
    Optional<double> m_optionalConveyorSpeedInMMPerSecs;
    Optional<double> m_optionalTopClearanceHeightInMM;
    Optional<double> m_optionalBottomClearanceHeightInMM;
    Optional<double> m_optionalWeightInGrams;
    Optional<std::string> m_optionalWorkOrderId;
    Optional<std::string> m_optionalBatchId;

    BoardForecastData() = default;
    BoardForecastData(EBoardQuality failedBoard,
        EFlippedBoard flippedBoard) :
        m_failedBoard(failedBoard),
        m_flippedBoard(flippedBoard)
    {}

    friend bool operator==(const BoardForecastData& lhs, const BoardForecastData& rhs)
    {
        return lhs.m_optionalForecastId == rhs.m_optionalForecastId
            && lhs.m_optionalTimeUntilAvailableInSeconds == rhs.m_optionalTimeUntilAvailableInSeconds
            && lhs.m_optionalBoardId == rhs.m_optionalBoardId
            && lhs.m_optionalBoardIdCreatedBy == rhs.m_optionalBoardIdCreatedBy
            && lhs.m_failedBoard == rhs.m_failedBoard
            && lhs.m_optionalProductTypeId == rhs.m_optionalProductTypeId
            && lhs.m_flippedBoard == rhs.m_flippedBoard
            && lhs.m_optionalTopBarcode == rhs.m_optionalTopBarcode
            && lhs.m_optionalBottomBarcode == rhs.m_optionalBottomBarcode
            && lhs.m_optionalLengthInMM == rhs.m_optionalLengthInMM
            && lhs.m_optionalWidthInMM == rhs.m_optionalWidthInMM
            && lhs.m_optionalThicknessInMM == rhs.m_optionalThicknessInMM
            && lhs.m_optionalConveyorSpeedInMMPerSecs == rhs.m_optionalConveyorSpeedInMMPerSecs
            && lhs.m_optionalTopClearanceHeightInMM == rhs.m_optionalTopClearanceHeightInMM
            && lhs.m_optionalBottomClearanceHeightInMM == rhs.m_optionalBottomClearanceHeightInMM
            && lhs.m_optionalWeightInGrams == rhs.m_optionalWeightInGrams
            && lhs.m_optionalWorkOrderId == rhs.m_optionalWorkOrderId
            && lhs.m_optionalBatchId == rhs.m_optionalBatchId;
    }
    friend bool operator!=(const BoardForecastData& lhs, const BoardForecastData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const BoardForecastData& data) 
    {
        s << '{';
        if (data.m_optionalForecastId) { s << " ForecastId=" << *data.m_optionalForecastId; }
        if (data.m_optionalTimeUntilAvailableInSeconds) { s << " TimeUntilAvailable=" << *data.m_optionalTimeUntilAvailableInSeconds; }
        if (data.m_optionalBoardId) { s << " BoardId=" << *data.m_optionalBoardId; }
        if (data.m_optionalBoardIdCreatedBy) { s << " BoardIdCreatedBy=" << *data.m_optionalBoardIdCreatedBy; }
        s << " FailedBoard=" << data.m_failedBoard;
        if (data.m_optionalProductTypeId) { s << " ProductTypeId=" << *data.m_optionalProductTypeId; }
        s << " FlippedBoard=" << data.m_flippedBoard;
        if (data.m_optionalTopBarcode) { s << " TopBarcode=" << *data.m_optionalTopBarcode; }
        if (data.m_optionalBottomBarcode) { s << " BottomBarcode=" << *data.m_optionalBottomBarcode; }
        if (data.m_optionalLengthInMM) { s << " Length=" << *data.m_optionalLengthInMM; }
        if (data.m_optionalWidthInMM) { s << " Width=" << *data.m_optionalWidthInMM; }
        if (data.m_optionalThicknessInMM) { s << " Thickness=" << *data.m_optionalThicknessInMM; }
        if (data.m_optionalConveyorSpeedInMMPerSecs) { s << " ConveyorSpeed=" << *data.m_optionalConveyorSpeedInMMPerSecs; }
        if (data.m_optionalTopClearanceHeightInMM) { s << " TopClearanceHeight=" << *data.m_optionalTopClearanceHeightInMM; }
        if (data.m_optionalBottomClearanceHeightInMM) { s << " BottomClearanceHeight=" << *data.m_optionalBottomClearanceHeightInMM; }
        if (data.m_optionalWeightInGrams) { s << " Weight=" << *data.m_optionalWeightInGrams; }
        if (data.m_optionalWorkOrderId) { s << " WorkOrderId=" << *data.m_optionalWorkOrderId; }
        if (data.m_optionalBatchId) { s << " BatchId=" << *data.m_optionalBatchId; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.17 ==========
struct QueryBoardInfoData
{
    Optional<std::string> m_optionalTopBarcode;
    Optional<std::string> m_optionalBottomBarcode;

    friend bool operator==(const QueryBoardInfoData& lhs, const QueryBoardInfoData& rhs)
    {
        return lhs.m_optionalTopBarcode == rhs.m_optionalTopBarcode
            && lhs.m_optionalBottomBarcode == rhs.m_optionalBottomBarcode;
    }
    friend bool operator!=(const QueryBoardInfoData& lhs, const QueryBoardInfoData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const QueryBoardInfoData& data) 
    {
        s << '{';
        if (data.m_optionalTopBarcode) { s << " TopBarcode=" << *data.m_optionalTopBarcode; }
        if (data.m_optionalBottomBarcode) { s << " BottomBarcode=" << *data.m_optionalBottomBarcode; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.18 ==========
struct SendBoardInfoData
{
    Optional<std::string> m_optionalBoardId;
    Optional<std::string> m_optionalBoardIdCreatedBy;
    Optional<EBoardQuality> m_optionalFailedBoard;
    Optional<std::string> m_optionalProductTypeId;
    Optional<EFlippedBoard> m_optionalFlippedBoard;
    Optional<std::string> m_optionalTopBarcode;
    Optional<std::string> m_optionalBottomBarcode;
    Optional<double> m_optionalLengthInMM;
    Optional<double> m_optionalWidthInMM;
    Optional<double> m_optionalThicknessInMM;
    Optional<double> m_optionalConveyorSpeedInMMPerSecs;
    Optional<double> m_optionalTopClearanceHeightInMM;
    Optional<double> m_optionalBottomClearanceHeightInMM;
    Optional<double> m_optionalWeightInGrams;
    Optional<std::string> m_optionalWorkOrderId;
    Optional<std::string> m_optionalBatchId;
    Optional<uint16_t> m_optionalRoute;
    Optional<uint16_t> m_optionalAction;
    SubBoards m_optionalSubBoards;

    friend bool operator==(const SendBoardInfoData& lhs, const SendBoardInfoData& rhs)
    {
        return lhs.m_optionalBoardId == rhs.m_optionalBoardId
            && lhs.m_optionalBoardIdCreatedBy == rhs.m_optionalBoardIdCreatedBy
            && lhs.m_optionalFailedBoard == rhs.m_optionalFailedBoard
            && lhs.m_optionalProductTypeId == rhs.m_optionalProductTypeId
            && lhs.m_optionalFlippedBoard == rhs.m_optionalFlippedBoard
            && lhs.m_optionalTopBarcode == rhs.m_optionalTopBarcode
            && lhs.m_optionalBottomBarcode == rhs.m_optionalBottomBarcode
            && lhs.m_optionalLengthInMM == rhs.m_optionalLengthInMM
            && lhs.m_optionalWidthInMM == rhs.m_optionalWidthInMM
            && lhs.m_optionalThicknessInMM == rhs.m_optionalThicknessInMM
            && lhs.m_optionalConveyorSpeedInMMPerSecs == rhs.m_optionalConveyorSpeedInMMPerSecs
            && lhs.m_optionalTopClearanceHeightInMM == rhs.m_optionalTopClearanceHeightInMM
            && lhs.m_optionalBottomClearanceHeightInMM == rhs.m_optionalBottomClearanceHeightInMM
            && lhs.m_optionalWeightInGrams == rhs.m_optionalWeightInGrams
            && lhs.m_optionalWorkOrderId == rhs.m_optionalWorkOrderId
            && lhs.m_optionalBatchId == rhs.m_optionalBatchId
            && lhs.m_optionalRoute == rhs.m_optionalRoute
            && lhs.m_optionalAction == rhs.m_optionalAction
            && lhs.m_optionalSubBoards == rhs.m_optionalSubBoards;
    }
    friend bool operator!=(const SendBoardInfoData& lhs, const SendBoardInfoData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const SendBoardInfoData& data) 
    {
        s << '{';
        if (data.m_optionalBoardId) { s << " BoardId=" << *data.m_optionalBoardId; }
        if (data.m_optionalBoardIdCreatedBy) { s << " BoardIdCreatedBy=" << *data.m_optionalBoardIdCreatedBy; }
        if (data.m_optionalFailedBoard) { s << " FailedBoard=" << *data.m_optionalFailedBoard; }
        if (data.m_optionalProductTypeId) { s << " ProductTypeId=" << *data.m_optionalProductTypeId; }
        if (data.m_optionalFlippedBoard) { s << " FlippedBoard=" << *data.m_optionalFlippedBoard; }
        if (data.m_optionalTopBarcode) { s << " TopBarcode=" << *data.m_optionalTopBarcode; }
        if (data.m_optionalBottomBarcode) { s << " BottomBarcode=" << *data.m_optionalBottomBarcode; }
        if (data.m_optionalLengthInMM) { s << " Length=" << *data.m_optionalLengthInMM; }
        if (data.m_optionalWidthInMM) { s << " Width=" << *data.m_optionalWidthInMM; }
        if (data.m_optionalThicknessInMM) { s << " Thickness=" << *data.m_optionalThicknessInMM; }
        if (data.m_optionalConveyorSpeedInMMPerSecs) { s << " ConveyorSpeed=" << *data.m_optionalConveyorSpeedInMMPerSecs; }
        if (data.m_optionalTopClearanceHeightInMM) { s << " TopClearanceHeight=" << *data.m_optionalTopClearanceHeightInMM; }
        if (data.m_optionalBottomClearanceHeightInMM) { s << " BottomClearanceHeight=" << *data.m_optionalBottomClearanceHeightInMM; }
        if (data.m_optionalWeightInGrams) { s << " Weight=" << *data.m_optionalWeightInGrams; }
        if (data.m_optionalWorkOrderId) { s << " WorkOrderId=" << *data.m_optionalWorkOrderId; }
        if (data.m_optionalBatchId) { s << " BatchId=" << *data.m_optionalBatchId; }
        if (data.m_optionalRoute) { s << " Route=" << *data.m_optionalRoute; }
        if (data.m_optionalAction) { s << " Action=" << *data.m_optionalAction; }
        if (!data.m_optionalSubBoards.empty()) { s << " SubBoards=" << data.m_optionalSubBoards; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.22 ==========
struct FeatureConfiguration
{
    friend bool operator==(const FeatureConfiguration&, const FeatureConfiguration&) { return true; }
    friend bool operator!=(const FeatureConfiguration&, const FeatureConfiguration&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureConfiguration&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.22 ==========
struct FeatureBoardTracking
{
    friend bool operator==(const FeatureBoardTracking&, const FeatureBoardTracking&) { return true; }
    friend bool operator!=(const FeatureBoardTracking&, const FeatureBoardTracking&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureBoardTracking&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.22 ==========
struct FeatureQueryWorkOrderInfo
{
    friend bool operator==(const FeatureQueryWorkOrderInfo&, const FeatureQueryWorkOrderInfo&) { return true; }
    friend bool operator!=(const FeatureQueryWorkOrderInfo&, const FeatureQueryWorkOrderInfo&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureQueryWorkOrderInfo&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.22 ==========
struct FeatureSendWorkOrderInfo
{
    friend bool operator==(const FeatureSendWorkOrderInfo&, const FeatureSendWorkOrderInfo&) { return true; }
    friend bool operator!=(const FeatureSendWorkOrderInfo&, const FeatureSendWorkOrderInfo&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureSendWorkOrderInfo&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.22 ==========
struct FeatureReplyWorkOrderInfo
{
    friend bool operator==(const FeatureReplyWorkOrderInfo&, const FeatureReplyWorkOrderInfo&) { return true; }
    friend bool operator!=(const FeatureReplyWorkOrderInfo&, const FeatureReplyWorkOrderInfo&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureReplyWorkOrderInfo&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.22 ==========
struct FeatureQueryHermesCapabilities
{
    friend bool operator==(const FeatureQueryHermesCapabilities&, const FeatureQueryHermesCapabilities&) { return true; }
    friend bool operator!=(const FeatureQueryHermesCapabilities&, const FeatureQueryHermesCapabilities&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureQueryHermesCapabilities&) { s << "{}"; return s; }
};

//========== The Hermes Standard 3.22 ==========
struct FeatureSendHermesCapabilities
{
    friend bool operator==(const FeatureSendHermesCapabilities&, const FeatureSendHermesCapabilities&) { return true; }
    friend bool operator!=(const FeatureSendHermesCapabilities&, const FeatureSendHermesCapabilities&) { return false; }
    template <class S> friend S& operator<<(S& s, const FeatureSendHermesCapabilities&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct SupervisoryFeatures
{
    Optional<FeatureConfiguration> m_optionalFeatureConfiguration;
    Optional<FeatureCheckAliveResponse> m_optionalFeatureCheckAliveResponse;
    Optional<FeatureBoardTracking> m_optionalFeatureBoardTracking;
    Optional<FeatureQueryWorkOrderInfo> m_optionalFeatureQueryWorkOrderInfo;
    Optional<FeatureSendWorkOrderInfo> m_optionalFeatureSendWorkOrderInfo;
    Optional<FeatureReplyWorkOrderInfo> m_optionalFeatureReplyWorkOrderInfo;
    Optional<FeatureQueryHermesCapabilities> m_optionalFeatureQueryHermesCapabilities;
    Optional<FeatureSendHermesCapabilities> m_optionalFeatureSendHermesCapabilities;

    friend bool operator==(const SupervisoryFeatures& lhs, const SupervisoryFeatures& rhs)
    {
        return lhs.m_optionalFeatureConfiguration == rhs.m_optionalFeatureConfiguration
            && lhs.m_optionalFeatureCheckAliveResponse == rhs.m_optionalFeatureCheckAliveResponse
            && lhs.m_optionalFeatureBoardTracking == rhs.m_optionalFeatureBoardTracking
            && lhs.m_optionalFeatureQueryWorkOrderInfo == rhs.m_optionalFeatureQueryWorkOrderInfo
            && lhs.m_optionalFeatureSendWorkOrderInfo == rhs.m_optionalFeatureSendWorkOrderInfo
            && lhs.m_optionalFeatureReplyWorkOrderInfo == rhs.m_optionalFeatureReplyWorkOrderInfo
            && lhs.m_optionalFeatureQueryHermesCapabilities == rhs.m_optionalFeatureQueryHermesCapabilities
            && lhs.m_optionalFeatureSendHermesCapabilities == rhs.m_optionalFeatureSendHermesCapabilities;
    }
    friend bool operator!=(const SupervisoryFeatures& lhs, const SupervisoryFeatures& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const SupervisoryFeatures& data) 
    {
        s << '{';
        if (data.m_optionalFeatureConfiguration) { s << " FeatureConfiguration=" << *data.m_optionalFeatureConfiguration; }
        if (data.m_optionalFeatureCheckAliveResponse) { s << " FeatureCheckAliveResponse=" << *data.m_optionalFeatureCheckAliveResponse; }
        if (data.m_optionalFeatureBoardTracking) { s << " FeatureBoardTracking=" << *data.m_optionalFeatureBoardTracking; }
        if (data.m_optionalFeatureQueryWorkOrderInfo) { s << " FeatureQueryWorkOrderInfo=" << *data.m_optionalFeatureQueryWorkOrderInfo; }
        if (data.m_optionalFeatureSendWorkOrderInfo) { s << " FeatureSendWorkOrderInfo=" << *data.m_optionalFeatureSendWorkOrderInfo; }
        if (data.m_optionalFeatureReplyWorkOrderInfo) { s << " FeatureReplyWorkOrderInfo=" << *data.m_optionalFeatureReplyWorkOrderInfo; }
        if (data.m_optionalFeatureQueryHermesCapabilities) { s << " FeatureQueryHermesCapabilities=" << *data.m_optionalFeatureQueryHermesCapabilities; }
        if (data.m_optionalFeatureSendHermesCapabilities) { s << " FeatureSendHermesCapabilities=" << *data.m_optionalFeatureSendHermesCapabilities; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.22 ==========
struct SupervisoryServiceDescriptionData
{
    std::string m_systemId;
    std::string m_version{"1.5"};
    SupervisoryFeatures m_supportedFeatures;

    SupervisoryServiceDescriptionData() = default;
    explicit SupervisoryServiceDescriptionData(StringView systemId) :
        m_systemId(systemId)
    {}

    friend bool operator==(const SupervisoryServiceDescriptionData& lhs, const SupervisoryServiceDescriptionData& rhs)
    {
        return lhs.m_systemId == rhs.m_systemId
            && lhs.m_version == rhs.m_version
            && lhs.m_supportedFeatures == rhs.m_supportedFeatures;
    }
    friend bool operator!=(const SupervisoryServiceDescriptionData& lhs, const SupervisoryServiceDescriptionData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const SupervisoryServiceDescriptionData& data) 
    {
        s << '{';
        s << " SystemId=" << data.m_systemId;
        s << " Version=" << data.m_version;
        s << " SupportedFeatures=" << data.m_supportedFeatures;
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.23 ==========
struct BoardArrivedData
{
    std::string m_machineId;
    unsigned m_upstreamLaneId{0};
    Optional<std::string> m_optionalUpstreamInterfaceId;
    Optional<std::string> m_optionalMagazineId;
    Optional<unsigned> m_optionalSlotId;
    EBoardArrivedTransfer m_boardTransfer{EBoardArrivedTransfer::eUNKNOWN};
    std::string m_boardId{"00000000-0000-0000-0000-000000000000"};
    std::string m_boardIdCreatedBy;
    EBoardQuality m_failedBoard{EBoardQuality::eANY};
    Optional<std::string> m_optionalProductTypeId;
    EFlippedBoard m_flippedBoard{EFlippedBoard::eSIDE_UP_IS_UNKNOWN};
    Optional<std::string> m_optionalTopBarcode;
    Optional<std::string> m_optionalBottomBarcode;
    Optional<double> m_optionalLengthInMM;
    Optional<double> m_optionalWidthInMM;
    Optional<double> m_optionalThicknessInMM;
    Optional<double> m_optionalConveyorSpeedInMMPerSecs;
    Optional<double> m_optionalTopClearanceHeightInMM;
    Optional<double> m_optionalBottomClearanceHeightInMM;
    Optional<double> m_optionalWeightInGrams;
    Optional<std::string> m_optionalWorkOrderId;
    Optional<std::string> m_optionalBatchId;
    Optional<uint16_t> m_optionalRoute;
    Optional<uint16_t> m_optionalAction;
    SubBoards m_optionalSubBoards;

    BoardArrivedData() = default;
    BoardArrivedData(StringView machineId,
        unsigned upstreamLaneId,
        EBoardArrivedTransfer boardTransfer,
        StringView boardId,
        StringView boardIdCreatedBy,
        EBoardQuality failedBoard,
        EFlippedBoard flippedBoard) :
        m_machineId(machineId),
        m_upstreamLaneId(upstreamLaneId),
        m_boardTransfer(boardTransfer),
        m_boardId(boardId),
        m_boardIdCreatedBy(boardIdCreatedBy),
        m_failedBoard(failedBoard),
        m_flippedBoard(flippedBoard)
    {}

    friend bool operator==(const BoardArrivedData& lhs, const BoardArrivedData& rhs)
    {
        return lhs.m_machineId == rhs.m_machineId
            && lhs.m_upstreamLaneId == rhs.m_upstreamLaneId
            && lhs.m_optionalUpstreamInterfaceId == rhs.m_optionalUpstreamInterfaceId
            && lhs.m_optionalMagazineId == rhs.m_optionalMagazineId
            && lhs.m_optionalSlotId == rhs.m_optionalSlotId
            && lhs.m_boardTransfer == rhs.m_boardTransfer
            && lhs.m_boardId == rhs.m_boardId
            && lhs.m_boardIdCreatedBy == rhs.m_boardIdCreatedBy
            && lhs.m_failedBoard == rhs.m_failedBoard
            && lhs.m_optionalProductTypeId == rhs.m_optionalProductTypeId
            && lhs.m_flippedBoard == rhs.m_flippedBoard
            && lhs.m_optionalTopBarcode == rhs.m_optionalTopBarcode
            && lhs.m_optionalBottomBarcode == rhs.m_optionalBottomBarcode
            && lhs.m_optionalLengthInMM == rhs.m_optionalLengthInMM
            && lhs.m_optionalWidthInMM == rhs.m_optionalWidthInMM
            && lhs.m_optionalThicknessInMM == rhs.m_optionalThicknessInMM
            && lhs.m_optionalConveyorSpeedInMMPerSecs == rhs.m_optionalConveyorSpeedInMMPerSecs
            && lhs.m_optionalTopClearanceHeightInMM == rhs.m_optionalTopClearanceHeightInMM
            && lhs.m_optionalBottomClearanceHeightInMM == rhs.m_optionalBottomClearanceHeightInMM
            && lhs.m_optionalWeightInGrams == rhs.m_optionalWeightInGrams
            && lhs.m_optionalWorkOrderId == rhs.m_optionalWorkOrderId
            && lhs.m_optionalBatchId == rhs.m_optionalBatchId
            && lhs.m_optionalRoute == rhs.m_optionalRoute
            && lhs.m_optionalAction == rhs.m_optionalAction
            && lhs.m_optionalSubBoards == rhs.m_optionalSubBoards;
    }
    friend bool operator!=(const BoardArrivedData& lhs, const BoardArrivedData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const BoardArrivedData& data) 
    {
        s << '{';
        s << " MachineId=" << data.m_machineId;
        s << " UpstreamLaneId=" << data.m_upstreamLaneId;
        if (data.m_optionalUpstreamInterfaceId) { s << " UpstreamInterfaceId=" << *data.m_optionalUpstreamInterfaceId; }
        if (data.m_optionalMagazineId) { s << " MagazineId=" << *data.m_optionalMagazineId; }
        if (data.m_optionalSlotId) { s << " SlotId=" << *data.m_optionalSlotId; }
        s << " BoardTransfer=" << data.m_boardTransfer;
        s << " BoardId=" << data.m_boardId;
        s << " BoardIdCreatedBy=" << data.m_boardIdCreatedBy;
        s << " FailedBoard=" << data.m_failedBoard;
        if (data.m_optionalProductTypeId) { s << " ProductTypeId=" << *data.m_optionalProductTypeId; }
        s << " FlippedBoard=" << data.m_flippedBoard;
        if (data.m_optionalTopBarcode) { s << " TopBarcode=" << *data.m_optionalTopBarcode; }
        if (data.m_optionalBottomBarcode) { s << " BottomBarcode=" << *data.m_optionalBottomBarcode; }
        if (data.m_optionalLengthInMM) { s << " Length=" << *data.m_optionalLengthInMM; }
        if (data.m_optionalWidthInMM) { s << " Width=" << *data.m_optionalWidthInMM; }
        if (data.m_optionalThicknessInMM) { s << " Thickness=" << *data.m_optionalThicknessInMM; }
        if (data.m_optionalConveyorSpeedInMMPerSecs) { s << " ConveyorSpeed=" << *data.m_optionalConveyorSpeedInMMPerSecs; }
        if (data.m_optionalTopClearanceHeightInMM) { s << " TopClearanceHeight=" << *data.m_optionalTopClearanceHeightInMM; }
        if (data.m_optionalBottomClearanceHeightInMM) { s << " BottomClearanceHeight=" << *data.m_optionalBottomClearanceHeightInMM; }
        if (data.m_optionalWeightInGrams) { s << " Weight=" << *data.m_optionalWeightInGrams; }
        if (data.m_optionalWorkOrderId) { s << " WorkOrderId=" << *data.m_optionalWorkOrderId; }
        if (data.m_optionalBatchId) { s << " BatchId=" << *data.m_optionalBatchId; }
        if (data.m_optionalRoute) { s << " Route=" << *data.m_optionalRoute; }
        if (data.m_optionalAction) { s << " Action=" << *data.m_optionalAction; }
        if (!data.m_optionalSubBoards.empty()) { s << " SubBoards=" << data.m_optionalSubBoards; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.24 ==========
struct BoardDepartedData
{
    std::string m_machineId;
    unsigned m_downstreamLaneId{0};
    Optional<std::string> m_optionalDownstreamInterfaceId;
    Optional<std::string> m_optionalMagazineId;
    Optional<unsigned> m_optionalSlotId;
    EBoardDepartedTransfer m_boardTransfer{EBoardDepartedTransfer::eUNKNOWN};
    std::string m_boardId{"00000000-0000-0000-0000-000000000000"};
    std::string m_boardIdCreatedBy;
    EBoardQuality m_failedBoard{EBoardQuality::eANY};
    Optional<std::string> m_optionalProductTypeId;
    EFlippedBoard m_flippedBoard{EFlippedBoard::eSIDE_UP_IS_UNKNOWN};
    Optional<std::string> m_optionalTopBarcode;
    Optional<std::string> m_optionalBottomBarcode;
    Optional<double> m_optionalLengthInMM;
    Optional<double> m_optionalWidthInMM;
    Optional<double> m_optionalThicknessInMM;
    Optional<double> m_optionalConveyorSpeedInMMPerSecs;
    Optional<double> m_optionalTopClearanceHeightInMM;
    Optional<double> m_optionalBottomClearanceHeightInMM;
    Optional<double> m_optionalWeightInGrams;
    Optional<std::string> m_optionalWorkOrderId;
    Optional<std::string> m_optionalBatchId;
    Optional<uint16_t> m_optionalRoute;
    Optional<uint16_t> m_optionalAction;
    SubBoards m_optionalSubBoards;

    BoardDepartedData() = default;
    BoardDepartedData(StringView machineId,
        unsigned downstreamLaneId,
        EBoardDepartedTransfer boardTransfer,
        StringView boardId,
        StringView boardIdCreatedBy,
        EBoardQuality failedBoard,
        EFlippedBoard flippedBoard) :
        m_machineId(machineId),
        m_downstreamLaneId(downstreamLaneId),
        m_boardTransfer(boardTransfer),
        m_boardId(boardId),
        m_boardIdCreatedBy(boardIdCreatedBy),
        m_failedBoard(failedBoard),
        m_flippedBoard(flippedBoard)
    {}

    friend bool operator==(const BoardDepartedData& lhs, const BoardDepartedData& rhs)
    {
        return lhs.m_machineId == rhs.m_machineId
            && lhs.m_downstreamLaneId == rhs.m_downstreamLaneId
            && lhs.m_optionalDownstreamInterfaceId == rhs.m_optionalDownstreamInterfaceId
            && lhs.m_optionalMagazineId == rhs.m_optionalMagazineId
            && lhs.m_optionalSlotId == rhs.m_optionalSlotId
            && lhs.m_boardTransfer == rhs.m_boardTransfer
            && lhs.m_boardId == rhs.m_boardId
            && lhs.m_boardIdCreatedBy == rhs.m_boardIdCreatedBy
            && lhs.m_failedBoard == rhs.m_failedBoard
            && lhs.m_optionalProductTypeId == rhs.m_optionalProductTypeId
            && lhs.m_flippedBoard == rhs.m_flippedBoard
            && lhs.m_optionalTopBarcode == rhs.m_optionalTopBarcode
            && lhs.m_optionalBottomBarcode == rhs.m_optionalBottomBarcode
            && lhs.m_optionalLengthInMM == rhs.m_optionalLengthInMM
            && lhs.m_optionalWidthInMM == rhs.m_optionalWidthInMM
            && lhs.m_optionalThicknessInMM == rhs.m_optionalThicknessInMM
            && lhs.m_optionalConveyorSpeedInMMPerSecs == rhs.m_optionalConveyorSpeedInMMPerSecs
            && lhs.m_optionalTopClearanceHeightInMM == rhs.m_optionalTopClearanceHeightInMM
            && lhs.m_optionalBottomClearanceHeightInMM == rhs.m_optionalBottomClearanceHeightInMM
            && lhs.m_optionalWeightInGrams == rhs.m_optionalWeightInGrams
            && lhs.m_optionalWorkOrderId == rhs.m_optionalWorkOrderId
            && lhs.m_optionalBatchId == rhs.m_optionalBatchId
            && lhs.m_optionalRoute == rhs.m_optionalRoute
            && lhs.m_optionalAction == rhs.m_optionalAction
            && lhs.m_optionalSubBoards == rhs.m_optionalSubBoards;
    }
    friend bool operator!=(const BoardDepartedData& lhs, const BoardDepartedData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const BoardDepartedData& data) 
    {
        s << '{';
        s << " MachineId=" << data.m_machineId;
        s << " DownstreamLaneId=" << data.m_downstreamLaneId;
        if (data.m_optionalDownstreamInterfaceId) { s << " DownstreamInterfaceId=" << *data.m_optionalDownstreamInterfaceId; }
        if (data.m_optionalMagazineId) { s << " MagazineId=" << *data.m_optionalMagazineId; }
        if (data.m_optionalSlotId) { s << " SlotId=" << *data.m_optionalSlotId; }
        s << " BoardTransfer=" << data.m_boardTransfer;
        s << " BoardId=" << data.m_boardId;
        s << " BoardIdCreatedBy=" << data.m_boardIdCreatedBy;
        s << " FailedBoard=" << data.m_failedBoard;
        if (data.m_optionalProductTypeId) { s << " ProductTypeId=" << *data.m_optionalProductTypeId; }
        s << " FlippedBoard=" << data.m_flippedBoard;
        if (data.m_optionalTopBarcode) { s << " TopBarcode=" << *data.m_optionalTopBarcode; }
        if (data.m_optionalBottomBarcode) { s << " BottomBarcode=" << *data.m_optionalBottomBarcode; }
        if (data.m_optionalLengthInMM) { s << " Length=" << *data.m_optionalLengthInMM; }
        if (data.m_optionalWidthInMM) { s << " Width=" << *data.m_optionalWidthInMM; }
        if (data.m_optionalThicknessInMM) { s << " Thickness=" << *data.m_optionalThicknessInMM; }
        if (data.m_optionalConveyorSpeedInMMPerSecs) { s << " ConveyorSpeed=" << *data.m_optionalConveyorSpeedInMMPerSecs; }
        if (data.m_optionalTopClearanceHeightInMM) { s << " TopClearanceHeight=" << *data.m_optionalTopClearanceHeightInMM; }
        if (data.m_optionalBottomClearanceHeightInMM) { s << " BottomClearanceHeight=" << *data.m_optionalBottomClearanceHeightInMM; }
        if (data.m_optionalWeightInGrams) { s << " Weight=" << *data.m_optionalWeightInGrams; }
        if (data.m_optionalWorkOrderId) { s << " WorkOrderId=" << *data.m_optionalWorkOrderId; }
        if (data.m_optionalBatchId) { s << " BatchId=" << *data.m_optionalBatchId; }
        if (data.m_optionalRoute) { s << " Route=" << *data.m_optionalRoute; }
        if (data.m_optionalAction) { s << " Action=" << *data.m_optionalAction; }
        if (!data.m_optionalSubBoards.empty()) { s << " SubBoards=" << data.m_optionalSubBoards; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.25 ==========
struct QueryWorkOrderInfoData
{
    Optional<std::string> m_optionalQueryId;
    std::string m_machineId;
    Optional<std::string> m_optionalMagazineId;
    Optional<unsigned> m_optionalSlotId;
    Optional<std::string> m_optionalBarcode;
    Optional<std::string> m_optionalWorkOrderId;
    Optional<std::string> m_optionalBatchId;

    QueryWorkOrderInfoData() = default;
    explicit QueryWorkOrderInfoData(StringView machineId) :
        m_machineId(machineId)
    {}

    friend bool operator==(const QueryWorkOrderInfoData& lhs, const QueryWorkOrderInfoData& rhs)
    {
        return lhs.m_optionalQueryId == rhs.m_optionalQueryId
            && lhs.m_machineId == rhs.m_machineId
            && lhs.m_optionalMagazineId == rhs.m_optionalMagazineId
            && lhs.m_optionalSlotId == rhs.m_optionalSlotId
            && lhs.m_optionalBarcode == rhs.m_optionalBarcode
            && lhs.m_optionalWorkOrderId == rhs.m_optionalWorkOrderId
            && lhs.m_optionalBatchId == rhs.m_optionalBatchId;
    }
    friend bool operator!=(const QueryWorkOrderInfoData& lhs, const QueryWorkOrderInfoData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const QueryWorkOrderInfoData& data) 
    {
        s << '{';
        if (data.m_optionalQueryId) { s << " QueryId=" << *data.m_optionalQueryId; }
        s << " MachineId=" << data.m_machineId;
        if (data.m_optionalMagazineId) { s << " MagazineId=" << *data.m_optionalMagazineId; }
        if (data.m_optionalSlotId) { s << " SlotId=" << *data.m_optionalSlotId; }
        if (data.m_optionalBarcode) { s << " Barcode=" << *data.m_optionalBarcode; }
        if (data.m_optionalWorkOrderId) { s << " WorkOrderId=" << *data.m_optionalWorkOrderId; }
        if (data.m_optionalBatchId) { s << " BatchId=" << *data.m_optionalBatchId; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.26 ==========
struct SendWorkOrderInfoData
{
    Optional<std::string> m_optionalQueryId;
    Optional<std::string> m_optionalWorkOrderId;
    Optional<std::string> m_optionalBatchId;
    Optional<std::string> m_optionalBoardId;
    Optional<std::string> m_optionalBoardIdCreatedBy;
    Optional<EBoardQuality> m_optionalFailedBoard;
    Optional<std::string> m_optionalProductTypeId;
    Optional<EFlippedBoard> m_optionalFlippedBoard;
    Optional<std::string> m_optionalTopBarcode;
    Optional<std::string> m_optionalBottomBarcode;
    Optional<double> m_optionalLengthInMM;
    Optional<double> m_optionalWidthInMM;
    Optional<double> m_optionalThicknessInMM;
    Optional<double> m_optionalConveyorSpeedInMMPerSecs;
    Optional<double> m_optionalTopClearanceHeightInMM;
    Optional<double> m_optionalBottomClearanceHeightInMM;
    Optional<double> m_optionalWeightInGrams;
    Optional<uint16_t> m_optionalRoute;
    SubBoards m_optionalSubBoards;

    friend bool operator==(const SendWorkOrderInfoData& lhs, const SendWorkOrderInfoData& rhs)
    {
        return lhs.m_optionalQueryId == rhs.m_optionalQueryId
            && lhs.m_optionalWorkOrderId == rhs.m_optionalWorkOrderId
            && lhs.m_optionalBatchId == rhs.m_optionalBatchId
            && lhs.m_optionalBoardId == rhs.m_optionalBoardId
            && lhs.m_optionalBoardIdCreatedBy == rhs.m_optionalBoardIdCreatedBy
            && lhs.m_optionalFailedBoard == rhs.m_optionalFailedBoard
            && lhs.m_optionalProductTypeId == rhs.m_optionalProductTypeId
            && lhs.m_optionalFlippedBoard == rhs.m_optionalFlippedBoard
            && lhs.m_optionalTopBarcode == rhs.m_optionalTopBarcode
            && lhs.m_optionalBottomBarcode == rhs.m_optionalBottomBarcode
            && lhs.m_optionalLengthInMM == rhs.m_optionalLengthInMM
            && lhs.m_optionalWidthInMM == rhs.m_optionalWidthInMM
            && lhs.m_optionalThicknessInMM == rhs.m_optionalThicknessInMM
            && lhs.m_optionalConveyorSpeedInMMPerSecs == rhs.m_optionalConveyorSpeedInMMPerSecs
            && lhs.m_optionalTopClearanceHeightInMM == rhs.m_optionalTopClearanceHeightInMM
            && lhs.m_optionalBottomClearanceHeightInMM == rhs.m_optionalBottomClearanceHeightInMM
            && lhs.m_optionalWeightInGrams == rhs.m_optionalWeightInGrams
            && lhs.m_optionalRoute == rhs.m_optionalRoute
            && lhs.m_optionalSubBoards == rhs.m_optionalSubBoards;
    }
    friend bool operator!=(const SendWorkOrderInfoData& lhs, const SendWorkOrderInfoData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const SendWorkOrderInfoData& data) 
    {
        s << '{';
        if (data.m_optionalQueryId) { s << " QueryId=" << *data.m_optionalQueryId; }
        if (data.m_optionalWorkOrderId) { s << " WorkOrderId=" << *data.m_optionalWorkOrderId; }
        if (data.m_optionalBatchId) { s << " BatchId=" << *data.m_optionalBatchId; }
        if (data.m_optionalBoardId) { s << " BoardId=" << *data.m_optionalBoardId; }
        if (data.m_optionalBoardIdCreatedBy) { s << " BoardIdCreatedBy=" << *data.m_optionalBoardIdCreatedBy; }
        if (data.m_optionalFailedBoard) { s << " FailedBoard=" << *data.m_optionalFailedBoard; }
        if (data.m_optionalProductTypeId) { s << " ProductTypeId=" << *data.m_optionalProductTypeId; }
        if (data.m_optionalFlippedBoard) { s << " FlippedBoard=" << *data.m_optionalFlippedBoard; }
        if (data.m_optionalTopBarcode) { s << " TopBarcode=" << *data.m_optionalTopBarcode; }
        if (data.m_optionalBottomBarcode) { s << " BottomBarcode=" << *data.m_optionalBottomBarcode; }
        if (data.m_optionalLengthInMM) { s << " Length=" << *data.m_optionalLengthInMM; }
        if (data.m_optionalWidthInMM) { s << " Width=" << *data.m_optionalWidthInMM; }
        if (data.m_optionalThicknessInMM) { s << " Thickness=" << *data.m_optionalThicknessInMM; }
        if (data.m_optionalConveyorSpeedInMMPerSecs) { s << " ConveyorSpeed=" << *data.m_optionalConveyorSpeedInMMPerSecs; }
        if (data.m_optionalTopClearanceHeightInMM) { s << " TopClearanceHeight=" << *data.m_optionalTopClearanceHeightInMM; }
        if (data.m_optionalBottomClearanceHeightInMM) { s << " BottomClearanceHeight=" << *data.m_optionalBottomClearanceHeightInMM; }
        if (data.m_optionalWeightInGrams) { s << " Weight=" << *data.m_optionalWeightInGrams; }
        if (data.m_optionalRoute) { s << " Route=" << *data.m_optionalRoute; }
        if (!data.m_optionalSubBoards.empty()) { s << " SubBoards=" << data.m_optionalSubBoards; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.27 ==========
struct ReplyWorkOrderInfoData
{
    std::string m_workOrderId;
    Optional<std::string> m_optionalBatchId;
    EReplyWorkOrderInfoStatus m_status{EReplyWorkOrderInfoStatus::eREJECTED};

    ReplyWorkOrderInfoData() = default;
    ReplyWorkOrderInfoData(StringView workOrderId,
        EReplyWorkOrderInfoStatus status) :
        m_workOrderId(workOrderId),
        m_status(status)
    {}

    friend bool operator==(const ReplyWorkOrderInfoData& lhs, const ReplyWorkOrderInfoData& rhs)
    {
        return lhs.m_workOrderId == rhs.m_workOrderId
            && lhs.m_optionalBatchId == rhs.m_optionalBatchId
            && lhs.m_status == rhs.m_status;
    }
    friend bool operator!=(const ReplyWorkOrderInfoData& lhs, const ReplyWorkOrderInfoData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const ReplyWorkOrderInfoData& data) 
    {
        s << '{';
        s << " WorkOrderId=" << data.m_workOrderId;
        if (data.m_optionalBatchId) { s << " BatchId=" << *data.m_optionalBatchId; }
        s << " Status=" << data.m_status;
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 6 ==========
struct QueryHermesCapabilitiesData
{
    friend bool operator==(const QueryHermesCapabilitiesData&, const QueryHermesCapabilitiesData&) { return true; }
    friend bool operator!=(const QueryHermesCapabilitiesData&, const QueryHermesCapabilitiesData&) { return false; }
    template <class S> friend S& operator<<(S& s, const QueryHermesCapabilitiesData&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct MessageCheckAliveResponse
{
    friend bool operator==(const MessageCheckAliveResponse&, const MessageCheckAliveResponse&) { return true; }
    friend bool operator!=(const MessageCheckAliveResponse&, const MessageCheckAliveResponse&) { return false; }
    template <class S> friend S& operator<<(S& s, const MessageCheckAliveResponse&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct MessageBoardForecast
{
    friend bool operator==(const MessageBoardForecast&, const MessageBoardForecast&) { return true; }
    friend bool operator!=(const MessageBoardForecast&, const MessageBoardForecast&) { return false; }
    template <class S> friend S& operator<<(S& s, const MessageBoardForecast&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct MessageQueryBoardInfo
{
    friend bool operator==(const MessageQueryBoardInfo&, const MessageQueryBoardInfo&) { return true; }
    friend bool operator!=(const MessageQueryBoardInfo&, const MessageQueryBoardInfo&) { return false; }
    template <class S> friend S& operator<<(S& s, const MessageQueryBoardInfo&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct MessageSendBoardInfo
{
    friend bool operator==(const MessageSendBoardInfo&, const MessageSendBoardInfo&) { return true; }
    friend bool operator!=(const MessageSendBoardInfo&, const MessageSendBoardInfo&) { return false; }
    template <class S> friend S& operator<<(S& s, const MessageSendBoardInfo&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct MessageBoardArrived
{
    friend bool operator==(const MessageBoardArrived&, const MessageBoardArrived&) { return true; }
    friend bool operator!=(const MessageBoardArrived&, const MessageBoardArrived&) { return false; }
    template <class S> friend S& operator<<(S& s, const MessageBoardArrived&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct MessageBoardDeparted
{
    friend bool operator==(const MessageBoardDeparted&, const MessageBoardDeparted&) { return true; }
    friend bool operator!=(const MessageBoardDeparted&, const MessageBoardDeparted&) { return false; }
    template <class S> friend S& operator<<(S& s, const MessageBoardDeparted&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct MessageQueryWorkOrderInfo
{
    friend bool operator==(const MessageQueryWorkOrderInfo&, const MessageQueryWorkOrderInfo&) { return true; }
    friend bool operator!=(const MessageQueryWorkOrderInfo&, const MessageQueryWorkOrderInfo&) { return false; }
    template <class S> friend S& operator<<(S& s, const MessageQueryWorkOrderInfo&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct MessageReplyWorkOrderInfo
{
    friend bool operator==(const MessageReplyWorkOrderInfo&, const MessageReplyWorkOrderInfo&) { return true; }
    friend bool operator!=(const MessageReplyWorkOrderInfo&, const MessageReplyWorkOrderInfo&) { return false; }
    template <class S> friend S& operator<<(S& s, const MessageReplyWorkOrderInfo&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct MessageCommand
{
    friend bool operator==(const MessageCommand&, const MessageCommand&) { return true; }
    friend bool operator!=(const MessageCommand&, const MessageCommand&) { return false; }
    template <class S> friend S& operator<<(S& s, const MessageCommand&) { s << "{}"; return s; }
};

//========== The Hermes Standard 6 ==========
struct OptionalMessages
{
    Optional<MessageCheckAliveResponse> m_optionalMessageCheckAliveResponse;
    Optional<MessageBoardForecast> m_optionalMessageBoardForecast;
    Optional<MessageQueryBoardInfo> m_optionalMessageQueryBoardInfo;
    Optional<MessageSendBoardInfo> m_optionalMessageSendBoardInfo;
    Optional<MessageBoardArrived> m_optionalMessageBoardArrived;
    Optional<MessageBoardDeparted> m_optionalMessageBoardDeparted;
    Optional<MessageQueryWorkOrderInfo> m_optionalMessageQueryWorkOrderInfo;
    Optional<MessageReplyWorkOrderInfo> m_optionalMessageReplyWorkOrderInfo;
    Optional<MessageCommand> m_optionalMessageCommand;

    friend bool operator==(const OptionalMessages& lhs, const OptionalMessages& rhs)
    {
        return lhs.m_optionalMessageCheckAliveResponse == rhs.m_optionalMessageCheckAliveResponse
            && lhs.m_optionalMessageBoardForecast == rhs.m_optionalMessageBoardForecast
            && lhs.m_optionalMessageQueryBoardInfo == rhs.m_optionalMessageQueryBoardInfo
            && lhs.m_optionalMessageSendBoardInfo == rhs.m_optionalMessageSendBoardInfo
            && lhs.m_optionalMessageBoardArrived == rhs.m_optionalMessageBoardArrived
            && lhs.m_optionalMessageBoardDeparted == rhs.m_optionalMessageBoardDeparted
            && lhs.m_optionalMessageQueryWorkOrderInfo == rhs.m_optionalMessageQueryWorkOrderInfo
            && lhs.m_optionalMessageReplyWorkOrderInfo == rhs.m_optionalMessageReplyWorkOrderInfo
            && lhs.m_optionalMessageCommand == rhs.m_optionalMessageCommand;
    }
    friend bool operator!=(const OptionalMessages& lhs, const OptionalMessages& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const OptionalMessages& data) 
    {
        s << '{';
        if (data.m_optionalMessageCheckAliveResponse) { s << " MessageCheckAliveResponse=" << *data.m_optionalMessageCheckAliveResponse; }
        if (data.m_optionalMessageBoardForecast) { s << " MessageBoardForecast=" << *data.m_optionalMessageBoardForecast; }
        if (data.m_optionalMessageQueryBoardInfo) { s << " MessageQueryBoardInfo=" << *data.m_optionalMessageQueryBoardInfo; }
        if (data.m_optionalMessageSendBoardInfo) { s << " MessageSendBoardInfo=" << *data.m_optionalMessageSendBoardInfo; }
        if (data.m_optionalMessageBoardArrived) { s << " MessageBoardArrived=" << *data.m_optionalMessageBoardArrived; }
        if (data.m_optionalMessageBoardDeparted) { s << " MessageBoardDeparted=" << *data.m_optionalMessageBoardDeparted; }
        if (data.m_optionalMessageQueryWorkOrderInfo) { s << " MessageQueryWorkOrderInfo=" << *data.m_optionalMessageQueryWorkOrderInfo; }
        if (data.m_optionalMessageReplyWorkOrderInfo) { s << " MessageReplyWorkOrderInfo=" << *data.m_optionalMessageReplyWorkOrderInfo; }
        if (data.m_optionalMessageCommand) { s << " MessageCommand=" << *data.m_optionalMessageCommand; }
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 6 ==========
struct Attributes
{
    uint16_t m_productTypeId{0};
    uint16_t m_topBarcode{0};
    uint16_t m_bottomBarcode{0};
    uint16_t m_length{0};
    uint16_t m_width{0};
    uint16_t m_thickness{0};
    uint16_t m_conveyorSpeed{0};
    uint16_t m_topClearanceHeight{0};
    uint16_t m_bottomClearanceHeight{0};
    uint16_t m_weight{0};
    uint16_t m_workOrderId{0};
    uint16_t m_batchId{0};
    uint16_t m_route{0};
    uint16_t m_action{0};
    uint16_t m_subBoards{0};

    Attributes() = default;
    Attributes(uint16_t productTypeId,
        uint16_t topBarcode,
        uint16_t bottomBarcode,
        uint16_t length,
        uint16_t width,
        uint16_t thickness,
        uint16_t conveyorSpeed,
        uint16_t topClearanceHeight,
        uint16_t bottomClearanceHeight,
        uint16_t weight,
        uint16_t workOrderId,
        uint16_t batchId,
        uint16_t route,
        uint16_t action,
        uint16_t subBoards) :
        m_productTypeId(productTypeId),
        m_topBarcode(topBarcode),
        m_bottomBarcode(bottomBarcode),
        m_length(length),
        m_width(width),
        m_thickness(thickness),
        m_conveyorSpeed(conveyorSpeed),
        m_topClearanceHeight(topClearanceHeight),
        m_bottomClearanceHeight(bottomClearanceHeight),
        m_weight(weight),
        m_workOrderId(workOrderId),
        m_batchId(batchId),
        m_route(route),
        m_action(action),
        m_subBoards(subBoards)
    {}

    friend bool operator==(const Attributes& lhs, const Attributes& rhs)
    {
        return lhs.m_productTypeId == rhs.m_productTypeId
            && lhs.m_topBarcode == rhs.m_topBarcode
            && lhs.m_bottomBarcode == rhs.m_bottomBarcode
            && lhs.m_length == rhs.m_length
            && lhs.m_width == rhs.m_width
            && lhs.m_thickness == rhs.m_thickness
            && lhs.m_conveyorSpeed == rhs.m_conveyorSpeed
            && lhs.m_topClearanceHeight == rhs.m_topClearanceHeight
            && lhs.m_bottomClearanceHeight == rhs.m_bottomClearanceHeight
            && lhs.m_weight == rhs.m_weight
            && lhs.m_workOrderId == rhs.m_workOrderId
            && lhs.m_batchId == rhs.m_batchId
            && lhs.m_route == rhs.m_route
            && lhs.m_action == rhs.m_action
            && lhs.m_subBoards == rhs.m_subBoards;
    }
    friend bool operator!=(const Attributes& lhs, const Attributes& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const Attributes& data) 
    {
        s << '{';
        s << " ProductTypeId=" << data.m_productTypeId;
        s << " TopBarcode=" << data.m_topBarcode;
        s << " BottomBarcode=" << data.m_bottomBarcode;
        s << " Length=" << data.m_length;
        s << " Width=" << data.m_width;
        s << " Thickness=" << data.m_thickness;
        s << " ConveyorSpeed=" << data.m_conveyorSpeed;
        s << " TopClearanceHeight=" << data.m_topClearanceHeight;
        s << " BottomClearanceHeight=" << data.m_bottomClearanceHeight;
        s << " Weight=" << data.m_weight;
        s << " WorkOrderId=" << data.m_workOrderId;
        s << " BatchId=" << data.m_batchId;
        s << " Route=" << data.m_route;
        s << " Action=" << data.m_action;
        s << " SubBoards=" << data.m_subBoards;
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 6 ==========
struct SendHermesCapabilitiesData
{
    OptionalMessages m_optionalMessages;
    Attributes m_attributes;

    friend bool operator==(const SendHermesCapabilitiesData& lhs, const SendHermesCapabilitiesData& rhs)
    {
        return lhs.m_optionalMessages == rhs.m_optionalMessages
            && lhs.m_attributes == rhs.m_attributes;
    }
    friend bool operator!=(const SendHermesCapabilitiesData& lhs, const SendHermesCapabilitiesData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const SendHermesCapabilitiesData& data) 
    {
        s << '{';
        s << " OptionalMessages=" << data.m_optionalMessages;
        s << " Attributes=" << data.m_attributes;
        s << " }";
        return s;
    }
};

//========== The Hermes Standard 3.28 ==========
struct CommandData
{
    uint16_t m_command{0};

    CommandData() = default;
    explicit CommandData(uint16_t command) :
        m_command(command)
    {}

    friend bool operator==(const CommandData& lhs, const CommandData& rhs)
    {
        return lhs.m_command == rhs.m_command;
    }
    friend bool operator!=(const CommandData& lhs, const CommandData& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const CommandData& data) 
    {
        s << '{';
        s << " Command=" << data.m_command;
        s << " }";
        return s;
    }
};

//========== Configuration of upstream interface (not part of The Hermes Standard) ==========
struct UpstreamSettings
{
    std::string m_machineId;
    std::string m_hostAddress;
    uint16_t m_port{0};
    double m_checkAlivePeriodInSeconds{60};
    double m_reconnectWaitTimeInSeconds{10};
    ECheckAliveResponseMode m_checkAliveResponseMode{ECheckAliveResponseMode::eAUTO};
    ECheckState m_checkState{ECheckState::eSEND_AND_RECEIVE};

    UpstreamSettings() = default;
    UpstreamSettings(StringView machineId,
        StringView hostAddress,
        uint16_t port) :
        m_machineId(machineId),
        m_hostAddress(hostAddress),
        m_port(port)
    {}

    friend bool operator==(const UpstreamSettings& lhs, const UpstreamSettings& rhs)
    {
        return lhs.m_machineId == rhs.m_machineId
            && lhs.m_hostAddress == rhs.m_hostAddress
            && lhs.m_port == rhs.m_port
            && lhs.m_checkAlivePeriodInSeconds == rhs.m_checkAlivePeriodInSeconds
            && lhs.m_reconnectWaitTimeInSeconds == rhs.m_reconnectWaitTimeInSeconds
            && lhs.m_checkAliveResponseMode == rhs.m_checkAliveResponseMode
            && lhs.m_checkState == rhs.m_checkState;
    }
    friend bool operator!=(const UpstreamSettings& lhs, const UpstreamSettings& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const UpstreamSettings& data) 
    {
        s << '{';
        s << " MachineId=" << data.m_machineId;
        s << " HostAddress=" << data.m_hostAddress;
        s << " Port=" << data.m_port;
        s << " CheckAlivePeriod=" << data.m_checkAlivePeriodInSeconds;
        s << " ReconnectWaitTime=" << data.m_reconnectWaitTimeInSeconds;
        s << " CheckAliveResponseMode=" << data.m_checkAliveResponseMode;
        s << " CheckState=" << data.m_checkState;
        s << " }";
        return s;
    }
};

//========== Configuration of downstream interface (not part of The Hermes Standard) ==========
struct DownstreamSettings
{
    std::string m_machineId;
    Optional<std::string> m_optionalClientAddress;
    uint16_t m_port{0};
    double m_checkAlivePeriodInSeconds{60};
    double m_reconnectWaitTimeInSeconds{10};
    ECheckAliveResponseMode m_checkAliveResponseMode{ECheckAliveResponseMode::eAUTO};
    ECheckState m_checkState{ECheckState::eSEND_AND_RECEIVE};

    DownstreamSettings() = default;
    DownstreamSettings(StringView machineId,
        uint16_t port) :
        m_machineId(machineId),
        m_port(port)
    {}

    friend bool operator==(const DownstreamSettings& lhs, const DownstreamSettings& rhs)
    {
        return lhs.m_machineId == rhs.m_machineId
            && lhs.m_optionalClientAddress == rhs.m_optionalClientAddress
            && lhs.m_port == rhs.m_port
            && lhs.m_checkAlivePeriodInSeconds == rhs.m_checkAlivePeriodInSeconds
            && lhs.m_reconnectWaitTimeInSeconds == rhs.m_reconnectWaitTimeInSeconds
            && lhs.m_checkAliveResponseMode == rhs.m_checkAliveResponseMode
            && lhs.m_checkState == rhs.m_checkState;
    }
    friend bool operator!=(const DownstreamSettings& lhs, const DownstreamSettings& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const DownstreamSettings& data) 
    {
        s << '{';
        s << " MachineId=" << data.m_machineId;
        if (data.m_optionalClientAddress) { s << " ClientAddress=" << *data.m_optionalClientAddress; }
        s << " Port=" << data.m_port;
        s << " CheckAlivePeriod=" << data.m_checkAlivePeriodInSeconds;
        s << " ReconnectWaitTime=" << data.m_reconnectWaitTimeInSeconds;
        s << " CheckAliveResponseMode=" << data.m_checkAliveResponseMode;
        s << " CheckState=" << data.m_checkState;
        s << " }";
        return s;
    }
};

//========== Configuration of configuration service interface (not part of The Hermes Standard) ==========
struct ConfigurationServiceSettings
{
    uint16_t m_port{0};
    double m_reconnectWaitTimeInSeconds{10};

    ConfigurationServiceSettings() = default;
    explicit ConfigurationServiceSettings(uint16_t port) :
        m_port(port)
    {}

    friend bool operator==(const ConfigurationServiceSettings& lhs, const ConfigurationServiceSettings& rhs)
    {
        return lhs.m_port == rhs.m_port
            && lhs.m_reconnectWaitTimeInSeconds == rhs.m_reconnectWaitTimeInSeconds;
    }
    friend bool operator!=(const ConfigurationServiceSettings& lhs, const ConfigurationServiceSettings& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const ConfigurationServiceSettings& data) 
    {
        s << '{';
        s << " Port=" << data.m_port;
        s << " ReconnectWaitTime=" << data.m_reconnectWaitTimeInSeconds;
        s << " }";
        return s;
    }
};

//========== Configuration of vertical service interface (not part of The Hermes Standard) ==========
struct VerticalServiceSettings
{
    std::string m_systemId;
    uint16_t m_port{0};
    double m_reconnectWaitTimeInSeconds{10};
    double m_checkAlivePeriodInSeconds{60};
    ECheckAliveResponseMode m_checkAliveResponseMode{ECheckAliveResponseMode::eAUTO};

    VerticalServiceSettings() = default;
    VerticalServiceSettings(StringView systemId,
        uint16_t port) :
        m_systemId(systemId),
        m_port(port)
    {}

    friend bool operator==(const VerticalServiceSettings& lhs, const VerticalServiceSettings& rhs)
    {
        return lhs.m_systemId == rhs.m_systemId
            && lhs.m_port == rhs.m_port
            && lhs.m_reconnectWaitTimeInSeconds == rhs.m_reconnectWaitTimeInSeconds
            && lhs.m_checkAlivePeriodInSeconds == rhs.m_checkAlivePeriodInSeconds
            && lhs.m_checkAliveResponseMode == rhs.m_checkAliveResponseMode;
    }
    friend bool operator!=(const VerticalServiceSettings& lhs, const VerticalServiceSettings& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const VerticalServiceSettings& data) 
    {
        s << '{';
        s << " SystemId=" << data.m_systemId;
        s << " Port=" << data.m_port;
        s << " ReconnectWaitTime=" << data.m_reconnectWaitTimeInSeconds;
        s << " CheckAlivePeriod=" << data.m_checkAlivePeriodInSeconds;
        s << " CheckAliveResponseMode=" << data.m_checkAliveResponseMode;
        s << " }";
        return s;
    }
};

//========== Configuration of vertical client interface (not part of The Hermes Standard) ==========
struct VerticalClientSettings
{
    std::string m_systemId;
    std::string m_hostAddress;
    uint16_t m_port{0};
    double m_reconnectWaitTimeInSeconds{10};
    double m_checkAlivePeriodInSeconds{60};
    ECheckAliveResponseMode m_checkAliveResponseMode{ECheckAliveResponseMode::eAUTO};

    VerticalClientSettings() = default;
    VerticalClientSettings(StringView systemId,
        StringView hostAddress,
        uint16_t port) :
        m_systemId(systemId),
        m_hostAddress(hostAddress),
        m_port(port)
    {}

    friend bool operator==(const VerticalClientSettings& lhs, const VerticalClientSettings& rhs)
    {
        return lhs.m_systemId == rhs.m_systemId
            && lhs.m_hostAddress == rhs.m_hostAddress
            && lhs.m_port == rhs.m_port
            && lhs.m_reconnectWaitTimeInSeconds == rhs.m_reconnectWaitTimeInSeconds
            && lhs.m_checkAlivePeriodInSeconds == rhs.m_checkAlivePeriodInSeconds
            && lhs.m_checkAliveResponseMode == rhs.m_checkAliveResponseMode;
    }
    friend bool operator!=(const VerticalClientSettings& lhs, const VerticalClientSettings& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const VerticalClientSettings& data) 
    {
        s << '{';
        s << " SystemId=" << data.m_systemId;
        s << " HostAddress=" << data.m_hostAddress;
        s << " Port=" << data.m_port;
        s << " ReconnectWaitTime=" << data.m_reconnectWaitTimeInSeconds;
        s << " CheckAlivePeriod=" << data.m_checkAlivePeriodInSeconds;
        s << " CheckAliveResponseMode=" << data.m_checkAliveResponseMode;
        s << " }";
        return s;
    }
};

//========== Error object (not part of The Hermes Standard) ==========
struct Error
{
    EErrorCode m_code{EErrorCode::eSUCCESS};
    std::string m_text;

    Error() = default;
    Error(EErrorCode code,
        StringView text) :
        m_code(code),
        m_text(text)
    {}

    explicit operator bool() const { return m_code != EErrorCode::eSUCCESS; }

    friend bool operator==(const Error& lhs, const Error& rhs)
    {
        return lhs.m_code == rhs.m_code
            && lhs.m_text == rhs.m_text;
    }
    friend bool operator!=(const Error& lhs, const Error& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const Error& data) 
    {
        s << '{';
        s << " Code=" << data.m_code;
        s << " Text=" << data.m_text;
        s << " }";
        return s;
    }
};

//========== Attributes for the established connection (not part of The Hermes Standard) ==========
struct ConnectionInfo
{
    std::string m_address;
    uint16_t m_port{0};
    std::string m_hostName;

    ConnectionInfo() = default;
    ConnectionInfo(StringView address,
        uint16_t port,
        StringView hostName) :
        m_address(address),
        m_port(port),
        m_hostName(hostName)
    {}

    friend bool operator==(const ConnectionInfo& lhs, const ConnectionInfo& rhs)
    {
        return lhs.m_address == rhs.m_address
            && lhs.m_port == rhs.m_port
            && lhs.m_hostName == rhs.m_hostName;
    }
    friend bool operator!=(const ConnectionInfo& lhs, const ConnectionInfo& rhs) { return !operator==(lhs, rhs); }

    template <class S> friend S& operator<<(S& s, const ConnectionInfo& data) 
    {
        s << '{';
        s << " Address=" << data.m_address;
        s << " Port=" << data.m_port;
        s << " HostName=" << data.m_hostName;
        s << " }";
        return s;
    }
};


} // namespace Hermes
