// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include "HermesOptional.hpp"
#include "HermesStringView.hpp"

#include <string>
#include <vector>

namespace Hermes
{
    static const uint16_t cCONFIG_PORT = 1248U;
    static const uint16_t cBASE_PORT = 50100U;
    static const std::size_t cMAX_MESSAGE_SIZE = 65536U;

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
        switch (e)
        {
        case EState::eNOT_CONNECTED: s << "eNOT_CONNECTED"; return s;
        case EState::eSOCKET_CONNECTED: s << "eSOCKET_CONNECTED"; return s;
        case EState::eSERVICE_DESCRIPTION_DOWNSTREAM: s << "eUPSTREAM_SERVICE_DESCRIPTION"; return s;
        case EState::eNOT_AVAILABLE_NOT_READY: s << "eNOT_AVAILABLE_NOT_READY"; return s;
        case EState::eBOARD_AVAILABLE: s << "eBOARD_AVAILABLE"; return s;
        case EState::eMACHINE_READY: s << "eMACHINE_READY"; return s;
        case EState::eAVAILABLE_AND_READY: s << "eAVAILABLE_AND_READY"; return s;
        case EState::eTRANSPORTING: s << "eTRANSPORTING"; return s;
        case EState::eTRANSPORT_STOPPED: s << "eTRANSPORT_STOPPED"; return s;
        case EState::eTRANSPORT_FINISHED: s << "eTRANSPORT_FINISHED"; return s;
        case EState::eDISCONNECTED: s << "eDISCONNECTED"; return s;
        default: s << "INVALID_STATE:" << static_cast<int>(e); return s;
        }
    }
    inline constexpr std::size_t size(EState) { return static_cast<std::size_t>(EState::eDISCONNECTED) + 1U; }

    enum class ETraceType
    {
        eSENT, // raw message sent
        eRECEIVED, // raw message received
        eDEBUG, // debug trace from implementation internals
        eINFO,
        eWARNING,
        eERROR
    };
    template<class S>
    S& operator<<(S& s, ETraceType e)
    {
        switch (e)
        {
        case ETraceType::eSENT: s << "eSENT"; return s;
        case ETraceType::eRECEIVED: s << "eRECEIVED"; return s;
        case ETraceType::eDEBUG: s << "eDEBUG"; return s;
        case ETraceType::eINFO: s << "eINFO"; return s;
        case ETraceType::eWARNING: s << "eWARNING"; return s;
        case ETraceType::eERROR: s << "eERROR"; return s;
        default: s << "INVALID_TRACE_TYPE:" << static_cast<int>(e); return s;
        }
    }
    inline constexpr std::size_t size(ETraceType) { return static_cast<std::size_t>(ETraceType::eERROR) + 1U; }

    enum class EBoardQuality
    {
        eANY,
        eGOOD,
        eFAILED
    };
    template<class S>
    S& operator<<(S& s, EBoardQuality e)
    {
        switch (e)
        {
        case EBoardQuality::eANY: s << "eANY"; return s;
        case EBoardQuality::eGOOD: s << "eGOOD"; return s;
        case EBoardQuality::eFAILED: s << "eFAILED"; return s;
        default: s << "INVALID_BOARD_QUALITY:" << static_cast<int>(e); return s;
        }
    }
    inline constexpr std::size_t size(EBoardQuality) { return static_cast<std::size_t>(EBoardQuality::eFAILED) + 1U; }

    enum class EFlippedBoard
    {
        eSIDE_UP_IS_UNKNOWN,
        eTOP_SIDE_IS_UP,
        eBOTTOM_SIDE_IS_UP
    };
    template<class S>
    S& operator<<(S& s, EFlippedBoard e)
    {
        switch (e)
        {
        case EFlippedBoard::eSIDE_UP_IS_UNKNOWN: s << "eSIDE_UP_IS_UNKNOWN"; return s;
        case EFlippedBoard::eTOP_SIDE_IS_UP: s << "eTOP_SIDE_IS_UP"; return s;
        case EFlippedBoard::eBOTTOM_SIDE_IS_UP: s << "eBOTTOM_SIDE_IS_UP"; return s;
        default: s << "INVALID_FLIPPED_BOARD:" << static_cast<int>(e); return s;
        }
    }
    inline constexpr std::size_t size(EFlippedBoard) { return static_cast<std::size_t>(EFlippedBoard::eBOTTOM_SIDE_IS_UP) + 1U; }

    enum class ETransferState
    {
        eNOT_STARTED = 1,
        eINCOMPLETE = 2,
        eCOMPLETE = 3
    };
    template<class S>
    S& operator<<(S& s, ETransferState e)
    {
        switch (e)
        {
        case ETransferState::eNOT_STARTED: s << "eNOT_STARTED"; return s;
        case ETransferState::eINCOMPLETE: s << "eINCOMPLETE"; return s;
        case ETransferState::eCOMPLETE: s << "eCOMPLETE"; return s;
        default: s << "INVALID_TRANSFER_STATE:" << static_cast<int>(e); return s;
        }
    }
    inline constexpr std::size_t size(ETransferState) { return static_cast<std::size_t>(ETransferState::eCOMPLETE) + 1U; }

    enum class ENotificationCode
    {
        eUNSPECIFIC,
        ePROTOCOL_ERROR,
        eCONNECTION_REFUSED_BECAUSE_OF_ESTABLISHED_CONNECTION,
        eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION,
        eCONFIGURATION_ERROR,
        eMACHINE_SHUTDOWN
    };
    template<class S>
    S& operator<<(S& s, ENotificationCode e)
    {
        switch (e)
        {
        case ENotificationCode::eUNSPECIFIC: s << "eUNSPECIFIC"; return s;
        case ENotificationCode::ePROTOCOL_ERROR: s << "ePROTOCOL_ERROR"; return s;
        case ENotificationCode::eCONNECTION_REFUSED_BECAUSE_OF_ESTABLISHED_CONNECTION: s << "eCONNECTION_REFUSED_BECAUSE_OF_ESTABLISHED_CONNECTION"; return s;
        case ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION: s << "eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION"; return s;
        case ENotificationCode::eCONFIGURATION_ERROR: s << "eCONFIGURATION_ERROR"; return s;
        case ENotificationCode::eMACHINE_SHUTDOWN: s << "eMACHINE_SHUTDOWN"; return s;
        default: s << "UNKNOWN_NOTIFICATION_CODE:" << static_cast<int>(e); return s;
        }
    }
    inline constexpr std::size_t size(ENotificationCode) { return static_cast<std::size_t>(ENotificationCode::eMACHINE_SHUTDOWN) + 1U; }

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
        switch (e)
        {
        case ESeverity::eUNKNOWN: s << "eUNKNOWN"; return s;
        case ESeverity::eFATAL: s << "eFATAL"; return s;
        case ESeverity::eERROR: s << "eERROR"; return s;
        case ESeverity::eWARNING: s << "eWARNING"; return s;
        case ESeverity::eINFO: s << "eINFO"; return s;
        default: s << "INVALID_SEVERITY:" << static_cast<int>(e); return s;
        }
    }
    inline constexpr std::size_t size(ESeverity) { return static_cast<std::size_t>(ESeverity::eINFO) + 1U; }

    enum class ECheckState
    {
        eSEND_AND_RECEIVE,
        eONLY_RECEIVE
    };
    template<class S>
    S& operator<<(S& s, ECheckState e)
    {
        switch (e)
        {
        case ECheckState::eSEND_AND_RECEIVE: s << "eSEND_AND_RECEIVE"; return s;
        case ECheckState::eONLY_RECEIVE: s << "eONLY_RECEIVE"; return s;
        default: s << "INVALID_CHECK_STATE:" << static_cast<int>(e); return s;
        }
    }
    inline constexpr std::size_t size(ECheckState) { return static_cast<std::size_t>(ECheckState::eONLY_RECEIVE) + 1U; }

    enum class EErrorCode
    {
        eSUCCESS,
        eIMPLEMENTATION_ERROR, // error inside the Hermes DLL
        ePEER_ERROR,  // the remote host has misbehaved
        eCLIENT_ERROR, // the client code making the API calls has misbehaved
        eNETWORK_ERROR, // something is wrong with the network or its configuration
        eTIMEOUT // a specified timeout has been reached
    };
    template<class S>
    S& operator<<(S& s, EErrorCode e)
    {
        switch (e)
        {
        case EErrorCode::eSUCCESS: s << "eSUCCESS"; return s;
        case EErrorCode::eIMPLEMENTATION_ERROR: s << "eIMPLEMENTATION_ERROR"; return s;
        case EErrorCode::ePEER_ERROR: s << "ePEER_ERROR"; return s;
        case EErrorCode::eCLIENT_ERROR: s << "eCLIENT_ERROR"; return s;
        case EErrorCode::eNETWORK_ERROR: s << "eNETWORK_ERROR"; return s;
        case EErrorCode::eTIMEOUT: s << "eTIMEOUT"; return s;
        default: s << "INVALID_ERROR_CODE:" << static_cast<int>(e); return s;
        }
    }
    inline constexpr std::size_t size(EErrorCode) { return static_cast<std::size_t>(EErrorCode::eTIMEOUT) + 1U; }

    struct UpstreamConfiguration
    {
        unsigned m_upstreamLaneId = 0;
        std::string m_hostAddress;
        uint16_t m_port = 0;

        UpstreamConfiguration() = default;
        UpstreamConfiguration(uint32_t upstreamLaneId, StringView hostName, uint16_t port = 0) :
            m_upstreamLaneId(upstreamLaneId),
            m_hostAddress(hostName),
            m_port(port)
        {}

        friend bool operator==(const UpstreamConfiguration& lhs, const UpstreamConfiguration& rhs)
        {
            return lhs.m_upstreamLaneId == rhs.m_upstreamLaneId
                && lhs.m_hostAddress == rhs.m_hostAddress
                && lhs.m_port == rhs.m_port;
        }
        friend bool operator!=(const UpstreamConfiguration& lhs, const UpstreamConfiguration& rhs) { return !operator==(lhs, rhs); }


        template<class S>
        friend S& operator<<(S& s, const UpstreamConfiguration& in_config)
        {
            s << "{m_upstreamLaneId=" << in_config.m_upstreamLaneId
                << "\n,m_hostAddress=" << in_config.m_hostAddress
                << "\n,m_port=" << in_config.m_port
                << '}';
            return s;
        }
    };

    struct DownstreamConfiguration
    {
        uint32_t m_downstreamLaneId = 0;
        std::string m_optionalClientAddress;
        uint16_t m_port = 0;

        DownstreamConfiguration() = default;
        DownstreamConfiguration(int32_t downstreamLaneId, uint16_t port = 0) :
            m_downstreamLaneId(downstreamLaneId),
            m_port(port)
        {}
        DownstreamConfiguration(uint32_t downstreamLaneId, const std::string& clientName, uint16_t port = 0) :
            m_downstreamLaneId(downstreamLaneId),
            m_optionalClientAddress(clientName),
            m_port(port)
        {}

        friend bool operator==(const DownstreamConfiguration& lhs, const DownstreamConfiguration& rhs)
        {
            return lhs.m_downstreamLaneId == rhs.m_downstreamLaneId
                && lhs.m_optionalClientAddress == rhs.m_optionalClientAddress
                && lhs.m_port == rhs.m_port;
        }
        friend bool operator!=(const DownstreamConfiguration& lhs, const DownstreamConfiguration& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const DownstreamConfiguration& in_config)
        {
            s << "{ m_downstreamLaneId=" << in_config.m_downstreamLaneId 
                << "\n,m_optionalClientAddress=" << in_config.m_optionalClientAddress
                << "\n,m_port=" << in_config.m_port << '}';
            return s;
        }
    };

    struct SetConfigurationData
    {
        std::string m_machineId;
        std::vector<UpstreamConfiguration> m_upstreamConfigurations;
        std::vector<DownstreamConfiguration> m_downstreamConfigurations;

        friend bool operator==(const SetConfigurationData& lhs, const SetConfigurationData& rhs)
        {
            return lhs.m_machineId == rhs.m_machineId
                && lhs.m_downstreamConfigurations == rhs.m_downstreamConfigurations
                && lhs.m_upstreamConfigurations == rhs.m_upstreamConfigurations;
        }
        friend bool operator!=(const SetConfigurationData& lhs, const SetConfigurationData& rhs) { return !operator==(lhs, rhs); }


        template<class S>
        friend S& operator<<(S& s, const SetConfigurationData& in_data)
        {
            s << "{m_machineId=" << in_data.m_machineId;
            for (const auto& config : in_data.m_upstreamConfigurations)
            {
                s << "\n,upstream=" << config;
            }
            for (const auto& config : in_data.m_downstreamConfigurations)
            {
                s << "\n,downstream=" << config;
            }
            s << '}';
            return s;
        }
    };

    struct GetConfigurationData // for uniformity and future extensions
    {
        friend bool operator==(const GetConfigurationData&, const GetConfigurationData&) { return true; }
        friend bool operator!=(const GetConfigurationData&, const GetConfigurationData&) { return false; }
        template<class S> friend S& operator<<(S& s, const GetConfigurationData&) 
        { 
            s << "{}";
            return s; 
        }
    };

    struct CurrentConfigurationData
    {
        Optional<std::string> m_optionalMachineId;
        std::vector<UpstreamConfiguration> m_upstreamConfigurations;
        std::vector<DownstreamConfiguration> m_downstreamConfigurations;

        CurrentConfigurationData() = default;

        friend bool operator==(const CurrentConfigurationData& lhs, const CurrentConfigurationData& rhs)
        {
            return lhs.m_optionalMachineId == rhs.m_optionalMachineId
                && lhs.m_downstreamConfigurations == rhs.m_downstreamConfigurations
                && lhs.m_upstreamConfigurations == rhs.m_upstreamConfigurations;
        }
        friend bool operator!=(const CurrentConfigurationData& lhs, const CurrentConfigurationData& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const CurrentConfigurationData& in_data)
        {
            s << "{m_optionalMachineId=" << in_data.m_optionalMachineId;
            for (const auto& upstreamConfig : in_data.m_upstreamConfigurations)
            {
                s << "\n,upstream=" << upstreamConfig;
            }
            for (const auto& downstreamConfig : in_data.m_downstreamConfigurations)
            {
                s << "\n,downstream=" << downstreamConfig;
            }
            s << '}';
            return s;
        }
    };

    struct ConnectionInfo
    {
        std::string m_address;
        uint16_t m_port = 0U;
        std::string m_hostName;

        ConnectionInfo() = default;
        ConnectionInfo(StringView address, uint16_t port, StringView hostName) :
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

        template<class S>
        friend S& operator<<(S& s, const ConnectionInfo& info)
        {
            s << "{m_address=" << info.m_address
                << "\n,m_port=" << info.m_port
                << "\n,m_hostName=" << info.m_hostName
                << '}';
            return s;
        }
    };

    struct ServiceDescription
    {
        std::string m_machineId;
        uint32_t m_laneId = 1;
        std::string m_version{"1.0"};

        ServiceDescription() = default;
        ServiceDescription(StringView machineId, uint32_t laneId) :
            m_machineId(machineId),
            m_laneId(laneId)
        {}

        friend bool operator==(const ServiceDescription& lhs, const ServiceDescription& rhs)
        {
            return lhs.m_machineId == rhs.m_machineId
                && lhs.m_laneId == rhs.m_laneId
                && lhs.m_version == rhs.m_version;
        }
        friend bool operator!=(const ServiceDescription& lhs, const ServiceDescription& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const ServiceDescription& in_data)
        {
            s << "{m_machineId=" << in_data.m_machineId
                << "\n,m_laneId=" << in_data.m_laneId
                << "\n,m_version=" << in_data.m_version
                << '}';
            return s;
        }
    };

    struct MachineReadyData
    {
        EBoardQuality m_failedBoard = EBoardQuality::eANY;

        MachineReadyData() = default;
        explicit MachineReadyData(EBoardQuality quality) : m_failedBoard(quality) {}

        friend bool operator==(const MachineReadyData& lhs, const MachineReadyData& rhs)
        {
            return lhs.m_failedBoard == rhs.m_failedBoard;
        }
        friend bool operator!=(const MachineReadyData& lhs, const MachineReadyData& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const MachineReadyData& in_data)
        {
            s << "{m_failedBoard=" << in_data.m_failedBoard << '}';
            return s;
        }
    };


    struct RevokeMachineReadyData // for uniformity and future extensions
    {
        friend bool operator==(const RevokeMachineReadyData&, const RevokeMachineReadyData&) { return true; }
        friend bool operator!=(const RevokeMachineReadyData&, const RevokeMachineReadyData&) { return false; }
        template<class S> friend S& operator<<(S& s, const RevokeMachineReadyData&)
        {
            s << "{}";
            return s;
        }
    };

    struct BoardAvailableData
    {
        std::string m_boardId;
        std::string m_boardIdCreatedBy;
        EBoardQuality m_failedBoard = EBoardQuality::eANY;
        Optional<std::string> m_optionalProductTypeId;
        EFlippedBoard m_flippedBoard = EFlippedBoard::eSIDE_UP_IS_UNKNOWN;
        Optional<std::string> m_optionalTopBarcode;
        Optional<std::string> m_optionalBottomBarcode;
        Optional<double> m_optionalLengthInMM;
        Optional<double> m_optionalWidthInMM;
        Optional<double> m_optionalThicknessInMM;
        Optional<double> m_optionalConveyorSpeedInMMPerSecs;
        Optional<double> m_optionalTopClearanceHeightInMM;
        Optional<double> m_optionalBottomClearanceHeightInMM;

        BoardAvailableData() = default;
        BoardAvailableData(StringView boardId, StringView boardIdCreatedBy, 
            EBoardQuality failedBoard = EBoardQuality::eANY,
            EFlippedBoard flippedBoard = EFlippedBoard::eSIDE_UP_IS_UNKNOWN) :
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
                && lhs.m_optionalBottomClearanceHeightInMM == rhs.m_optionalBottomClearanceHeightInMM;
        }
        friend bool operator!=(const BoardAvailableData& lhs, const BoardAvailableData& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const BoardAvailableData& in_data)
        {
            s << "{m_boardId=" << in_data.m_boardId
                << "\n,m_boardIdCreatedBy=" << in_data.m_boardIdCreatedBy
                << "\n,m_failedBoard=" << in_data.m_failedBoard
                << "\n,m_optionalProductTypeId=" << in_data.m_optionalProductTypeId
                << "\n,m_flippedBoard=" << in_data.m_flippedBoard
                << "\n,m_optionalTopBarcode=" << in_data.m_optionalTopBarcode
                << "\n,m_optionalBottomBarcode=" << in_data.m_optionalBottomBarcode
                << "\n,m_optionalLengthInMM=" << in_data.m_optionalLengthInMM
                << "\n,m_optionalWidthInMM=" << in_data.m_optionalWidthInMM
                << "\n,m_optionalThicknessInMM=" << in_data.m_optionalThicknessInMM
                << "\n,m_optionalConveyorSpeedInMMPerSecs=" << in_data.m_optionalConveyorSpeedInMMPerSecs
                << "\n,m_optionalTopClearanceHeightInMM=" << in_data.m_optionalTopClearanceHeightInMM
                << "\n,m_optionalBottomClearanceHeightInMM=" << in_data.m_optionalBottomClearanceHeightInMM
                << '}';
            return s;
        }
    };

    struct RevokeBoardAvailableData // for uniformity and future extensions
    {
        friend bool operator==(const RevokeBoardAvailableData&, const RevokeBoardAvailableData&) { return true; }
        friend bool operator!=(const RevokeBoardAvailableData&, const RevokeBoardAvailableData&) { return false; }
        template<class S> friend S& operator<<(S& s, const RevokeBoardAvailableData&)
        {
            s << "{}";
            return s;
        }
    };

    struct StartTransportData
    {
        std::string m_boardId;
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

        template<class S>
        friend S& operator<<(S& s, const StartTransportData& in_data)
        {
            s << "{m_boardId=" << in_data.m_boardId
                << "\n,m_optionalConveyorSpeedInMMPerSecs=" << in_data.m_optionalConveyorSpeedInMMPerSecs
                << '}';
            return s;
        }
    };

    struct StopTransportData
    {
        ETransferState m_transferState = ETransferState::eINCOMPLETE;
        std::string m_boardId;

        StopTransportData() = default;
        StopTransportData(ETransferState state, StringView boardId) :
            m_transferState(state),
            m_boardId(boardId)
        {}

        friend bool operator==(const StopTransportData& lhs, const StopTransportData& rhs)
        {
            return lhs.m_boardId == rhs.m_boardId
                && lhs.m_transferState == rhs.m_transferState;
        }
        friend bool operator!=(const StopTransportData& lhs, const StopTransportData& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const StopTransportData& in_data)
        {
            s << "{m_transferState=" << in_data.m_transferState
                << "\n,m_boardId=" << in_data.m_boardId
                << '}';
            return s;
        }
    };

    struct TransportFinishedData
    {
        ETransferState m_transferState = ETransferState::eINCOMPLETE;
        std::string m_boardId;

        TransportFinishedData() = default;
        TransportFinishedData(ETransferState state, StringView boardId) :
            m_transferState(state),
            m_boardId(boardId)
        {}

        friend bool operator==(const TransportFinishedData& lhs, const TransportFinishedData& rhs)
        {
            return lhs.m_boardId == rhs.m_boardId
                && lhs.m_transferState == rhs.m_transferState;
        }
        friend bool operator!=(const TransportFinishedData& lhs, const TransportFinishedData& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const TransportFinishedData& in_data)
        {
            s << "{m_transferState=" << in_data.m_transferState
                << "\n,m_boardId=" << in_data.m_boardId
                << '}';
            return s;
        }
    };

    struct NotificationData
    {
        ENotificationCode m_notificationCode = ENotificationCode::eUNSPECIFIC;
        ESeverity m_severity{};
        std::string m_description;

        NotificationData() = default;
        NotificationData(ENotificationCode code, ESeverity severity, StringView description) :
            m_notificationCode(code),
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

        template<class S>
        friend S& operator<<(S& s, const NotificationData& in_data)
        {
            s << "{m_notificationCode=" << in_data.m_notificationCode
                << "\n,m_severity=" << in_data.m_severity
                << "\n,m_description=" << in_data.m_description
                << '}';
            return s;
        }
    };

    struct CheckAliveData // for uniformity and future extensions
    {
        friend bool operator==(const CheckAliveData&, const CheckAliveData&) { return true; }
        friend bool operator!=(const CheckAliveData&, const CheckAliveData&) { return false; }
        template<class S> friend S& operator<<(S& s, const CheckAliveData&)
        {
            s << "{}";
            return s;
        }
    };

    struct UpstreamSettings
    {
        std::string m_machineId;
        std::string m_hostAddress;
        uint16_t m_port = 0U; // if 0, then HermesProcol will use the default port for this lane
        uint32_t m_checkAlivePeriodInSeconds = 60U;
        uint32_t m_reconnectWaitTimeInSeconds = 10U;
        ECheckState m_checkState = ECheckState::eSEND_AND_RECEIVE;

        UpstreamSettings() = default;
        UpstreamSettings(StringView machineId, StringView upstreamHostName, uint16_t upstreamPort) :
            m_machineId(machineId),
            m_hostAddress(upstreamHostName),
            m_port(upstreamPort)
        {}

        friend bool operator==(const UpstreamSettings& lhs, const UpstreamSettings& rhs)
        {
            return lhs.m_machineId == rhs.m_machineId
                && lhs.m_hostAddress == rhs.m_hostAddress
                && lhs.m_port == rhs.m_port
                && lhs.m_checkAlivePeriodInSeconds == rhs.m_checkAlivePeriodInSeconds
                && lhs.m_reconnectWaitTimeInSeconds == rhs.m_reconnectWaitTimeInSeconds
                && lhs.m_checkState == rhs.m_checkState;
        }
        friend bool operator!=(const UpstreamSettings& lhs, const UpstreamSettings& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const UpstreamSettings& in_data)
        {
            s << "{m_machineId=" << in_data.m_machineId
                << "\n,m_hostAddress=" << in_data.m_hostAddress
                << "\n,m_port=" << in_data.m_port
                << "\n,m_checkAlivePeriodInSeconds=" << in_data.m_checkAlivePeriodInSeconds
                << "\n,m_reconnectWaitTimeInSeconds=" << in_data.m_reconnectWaitTimeInSeconds
                << "\n,m_checkState=" << in_data.m_checkState
                << '}';
            return s;
        }
    };

    struct DownstreamSettings
    {
        std::string m_machineId;
        std::string m_optionalClientAddress; // if empty then requests from any client will be accepted
        uint16_t m_port = 0; // if 0, then HermesProcol will use the default port for this lane
        uint32_t m_checkAlivePeriodInSeconds = 60U;
        uint32_t m_reconnectWaitTimeInSeconds = 10U;
        ECheckState m_checkState = ECheckState::eSEND_AND_RECEIVE;

        DownstreamSettings() = default;
        explicit DownstreamSettings(StringView machineId, uint16_t myPort) :
            m_machineId(machineId),
            m_port(myPort)
        {}

        friend bool operator==(const DownstreamSettings& lhs, const DownstreamSettings& rhs)
        {
            return lhs.m_machineId == rhs.m_machineId
                && lhs.m_optionalClientAddress == rhs.m_optionalClientAddress
                && lhs.m_port == rhs.m_port
                && lhs.m_checkAlivePeriodInSeconds == rhs.m_checkAlivePeriodInSeconds
                && lhs.m_reconnectWaitTimeInSeconds == rhs.m_reconnectWaitTimeInSeconds
                && lhs.m_checkState == rhs.m_checkState;
        }
        friend bool operator!=(const DownstreamSettings& lhs, const DownstreamSettings& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const DownstreamSettings& in_data)
        {
            s << "{m_machineId=" << in_data.m_machineId
                << "\n,m_optionalClientAddress=" << in_data.m_optionalClientAddress
                << "\n,m_port=" << in_data.m_port
                << "\n,m_checkAlivePeriodInSeconds=" << in_data.m_checkAlivePeriodInSeconds
                << "\n,m_reconnectWaitTimeInSeconds=" << in_data.m_reconnectWaitTimeInSeconds
                << "\n,m_checkState=" << in_data.m_checkState
                << '}';
            return s;
        }
    };

    struct ConfigurationServiceSettings
    {
        uint16_t m_port = 0;
        uint32_t m_reconnectWaitTimeInSeconds = 10U;

        friend bool operator==(const ConfigurationServiceSettings& lhs, const ConfigurationServiceSettings& rhs)
        {
            return lhs.m_port == rhs.m_port
                && lhs.m_reconnectWaitTimeInSeconds == rhs.m_reconnectWaitTimeInSeconds;
        }
        friend bool operator!=(const ConfigurationServiceSettings& lhs, const ConfigurationServiceSettings& rhs) { return !operator==(lhs, rhs); }

        template<class S>
        friend S& operator<<(S& s, const ConfigurationServiceSettings& in_data)
        {
            s << "{m_port=" << in_data.m_port
                << "\n,m_reconnectWaitTimeInSeconds=" << in_data.m_reconnectWaitTimeInSeconds
                << '}';
            return s;
        }
    };

    class Error
    {
    public:
        EErrorCode m_code = EErrorCode::eSUCCESS;
        std::string m_text;

        Error() {}
        Error(EErrorCode code, StringView text) :
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

        template<class S>
        friend S& operator<<(S& s, const Error& error)
        {
            s << '{' << error.m_code;
            if (!error.m_text.empty()) { s << "\n," << error.m_text; }
            s << '}';
            return s;
        }

    };
}
