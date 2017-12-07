// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include "Hermes.h"
#include "HermesData.hpp"
#include "HermesOptional.hpp"
#include "HermesStringView.hpp"

namespace Hermes
{
    // check whether we got the constants right:
    static_assert(cCONFIG_PORT == cHERMES_CONFIG_PORT, "");
    static_assert(cBASE_PORT == cHERMES_BASE_PORT, "");
    static_assert(cMAX_MESSAGE_SIZE == cHERMES_MAX_MESSAGE_SIZE, "");

    // convert std::StringView from/to HermesStringView
    inline HermesStringView ToC(const std::string& str) { return{str.data(), static_cast<uint32_t>(str.size())}; }
    template<class T>
    const T* ToC(const Optional<T>& optional) { return optional ? &*optional : nullptr; }

    inline HermesStringView ToC(const Optional<std::string>& optional)
    {
        return optional ? ToC(*optional) : HermesStringView{0, 0};
    }
    inline HermesStringView ToC(StringView view) { return{view.data(), static_cast<uint32_t>(view.size())}; }
    inline StringView ToCpp(HermesStringView view) { return{view.m_pData, view.m_size}; }

    template<class T>
    Optional<T> ToOptional(const T* pValue)
    {
        if (!pValue)
            return{};
        return *pValue;
    }
    
    inline Optional<std::string> ToOptional(HermesStringView view) 
    {
        if (!view.m_pData)
            return{};
        return ToCpp(view);
    }

    // enums
    static_assert(size(EState()) == eHERMES_STATE_ENUM_SIZE, "enum mismatch");
    inline constexpr EHermesState ToC(EState e) { return static_cast<EHermesState>(e); }
    inline constexpr EState ToCpp(EHermesState e) { return static_cast<EState>(e); }

    static_assert(size(ETraceType()) == eHERMES_TRACE_TYPE_ENUM_SIZE, "enum mismatch");
    inline constexpr EHermesTraceType ToC(ETraceType e) { return static_cast<EHermesTraceType>(e); }
    inline constexpr ETraceType ToCpp(EHermesTraceType e) { return static_cast<ETraceType>(e); }

    static_assert(size(EBoardQuality()) == eHERMES_BOARD_QUALITY_ENUM_SIZE, "enum mismatch");
    inline constexpr EHermesBoardQuality ToC(EBoardQuality e) { return static_cast<EHermesBoardQuality>(e); }
    inline constexpr EBoardQuality ToCpp(EHermesBoardQuality e) { return static_cast<EBoardQuality>(e); }

    static_assert(size(EFlippedBoard()) == eHERMES_FLIPPED_BOARD_ENUM_SIZE, "enum mismatch");
    inline constexpr EHermesFlippedBoard ToC(EFlippedBoard e) { return static_cast<EHermesFlippedBoard>(e); }
    inline constexpr EFlippedBoard ToCpp(EHermesFlippedBoard e) { return static_cast<EFlippedBoard>(e); }

    static_assert(size(ETransferState()) == eHERMES_TRANSFER_STATE_ENUM_SIZE, "enum mismatch");
    inline constexpr EHermesTransferState ToC(ETransferState e) { return static_cast<EHermesTransferState>(e); }
    inline constexpr ETransferState ToCpp(EHermesTransferState e) { return static_cast<ETransferState>(e); }

    static_assert(size(ENotificationCode()) == eHERMES_NOTIFICATION_CODE_ENUM_SIZE, "enum mismatch");
    inline constexpr EHermesNotificationCode ToC(ENotificationCode e) { return static_cast<EHermesNotificationCode>(e); }
    inline constexpr ENotificationCode ToCpp(EHermesNotificationCode e) { return static_cast<ENotificationCode>(e); }

    static_assert(size(ESeverity()) == eHERMES_SEVERITY_ENUM_SIZE, "enum mismatch");
    inline constexpr EHermesSeverity ToC(ESeverity e) { return static_cast<EHermesSeverity>(e); }
    inline constexpr ESeverity ToCpp(EHermesSeverity e) { return static_cast<ESeverity>(e); }

    static_assert(size(ECheckState()) == eHERMES_CHECK_STATE_ENUM_SIZE, "enum mismatch");
    inline constexpr EHermesCheckState ToC(ECheckState e) { return static_cast<EHermesCheckState>(e); }
    inline constexpr ECheckState ToCpp(EHermesCheckState e) { return static_cast<ECheckState>(e); }

    static_assert(size(EErrorCode()) == eHERMES_ERROR_CODE_ENUM_SIZE, "enum mismatch");
    inline constexpr EHermesErrorCode ToC(EErrorCode e) { return static_cast<EHermesErrorCode>(e); }
    inline constexpr EErrorCode ToCpp(EHermesErrorCode e) { return static_cast<EErrorCode>(e); }

    // UpstreamConfiguration
    inline HermesUpstreamConfiguration ToC(const UpstreamConfiguration& in_data)
    {
        return{in_data.m_upstreamLaneId, Hermes::ToC(in_data.m_hostAddress), in_data.m_port};
    }
    inline UpstreamConfiguration ToCpp(const HermesUpstreamConfiguration& in_data)
    {
        return UpstreamConfiguration(in_data.m_upstreamLaneId, ToCpp(in_data.m_hostAddress), in_data.m_port);
    }

    // DownstreamConfiguration
    inline HermesDownstreamConfiguration ToC(const DownstreamConfiguration& in_data)
    {
        return{in_data.m_downstreamLaneId, Hermes::ToC(in_data.m_optionalClientAddress), in_data.m_port};
    }
    inline DownstreamConfiguration ToCpp(const HermesDownstreamConfiguration& in_data)
    {
        return DownstreamConfiguration(in_data.m_downstreamLaneId, ToCpp(in_data.m_optionalClientAddress), in_data.m_port);
    }


    // helper template for converting a vector of C++ types to C
    template<class C>
    struct VectorHolder
    {
        std::vector<C> m_values;
        std::vector<const C*> m_pointers;

        template<class CppT>
        VectorHolder(const std::vector<CppT>& in_data)
        {
            m_values.reserve(in_data.size());
            for (const auto& data : in_data)
            {
                m_values.emplace_back(ToC(data));
            }
        }
    };

    template<class C>
    const C** ToC(VectorHolder<C>& holder, uint32_t* pSize)
    {
        uint32_t size = static_cast<uint32_t>(holder.m_values.size());
        holder.m_pointers.resize(size);
        for (uint32_t i = 0U; i < size; ++i)
        {
            holder.m_pointers[i] = &(holder.m_values[i]);
        }
        *pSize = size;
        return holder.m_pointers.data();
    }

    template<class CppT, class C>
    std::vector<CppT> ToCpp(const C** ppData, uint32_t size)
    {
        std::vector<CppT> data;
        data.reserve(size);
        for (uint32_t i = 0U; i < size; ++i)
        {
            data.emplace_back(ToCpp(*(ppData[i])));
        }
        return data;
    }

    // SetConfigurationData
    class SetConfigurationDataHolder
    {
    public:
        explicit SetConfigurationDataHolder(const SetConfigurationData& in_data) :
            m_upstreamConfigurations(in_data.m_upstreamConfigurations),
            m_downstreamConfigurations(in_data.m_downstreamConfigurations)
        {
            m_data.m_machineId = ToC(in_data.m_machineId);
        }

        const HermesSetConfigurationData* operator&()
        {
            m_data.m_upstreamConfigurations = ToC(m_upstreamConfigurations, &m_data.m_upstreamConfigurationCount);
            m_data.m_downstreamConfigurations = ToC(m_downstreamConfigurations, &m_data.m_downstreamConfigurationCount);
            return &m_data;
        }

    private:
        VectorHolder<HermesUpstreamConfiguration> m_upstreamConfigurations;
        VectorHolder<HermesDownstreamConfiguration> m_downstreamConfigurations;
        mutable HermesSetConfigurationData m_data;
    };

    inline SetConfigurationDataHolder ToC(const SetConfigurationData& in_data) { return SetConfigurationDataHolder(in_data); }
    inline SetConfigurationData ToCpp(const HermesSetConfigurationData& in_data)
    {
        SetConfigurationData data;
        data.m_machineId = ToCpp(in_data.m_machineId);
        data.m_upstreamConfigurations = ToCpp<Hermes::UpstreamConfiguration>(in_data.m_upstreamConfigurations, in_data.m_upstreamConfigurationCount);
        data.m_downstreamConfigurations = ToCpp<Hermes::DownstreamConfiguration>(in_data.m_downstreamConfigurations, in_data.m_downstreamConfigurationCount);
        return data;
    }

    // GetConfigurationData
    inline HermesGetConfigurationData ToC(const GetConfigurationData&) { return{}; }
    inline GetConfigurationData ToCpp(const HermesGetConfigurationData&) { return{}; }

    // CurrentConfigurationData
    class CurrentConfigurationDataHolder
    {
    public:
        explicit CurrentConfigurationDataHolder(const CurrentConfigurationData& in_data) :
            m_upstreamConfigurations(in_data.m_upstreamConfigurations),
            m_downstreamConfigurations(in_data.m_downstreamConfigurations)
        {
            m_data.m_optionalMachineId = ToC(in_data.m_optionalMachineId);
        }

        const HermesCurrentConfigurationData* operator&()
        {
            m_data.m_upstreamConfigurations = ToC(m_upstreamConfigurations, &m_data.m_upstreamConfigurationCount);
            m_data.m_downstreamConfigurations = ToC(m_downstreamConfigurations, &m_data.m_downstreamConfigurationCount);
            return &m_data;
        }

    private:
        VectorHolder<HermesUpstreamConfiguration> m_upstreamConfigurations;
        VectorHolder<HermesDownstreamConfiguration> m_downstreamConfigurations;
        mutable HermesCurrentConfigurationData m_data;
    };

    inline CurrentConfigurationDataHolder ToC(const CurrentConfigurationData& in_data) { return CurrentConfigurationDataHolder(in_data); }
    inline CurrentConfigurationData ToCpp(const HermesCurrentConfigurationData& in_data)
    {
        CurrentConfigurationData data;
        if (in_data.m_optionalMachineId.m_pData)
        {
            data.m_optionalMachineId = ToCpp(in_data.m_optionalMachineId);
        }
        data.m_upstreamConfigurations = ToCpp<Hermes::UpstreamConfiguration>(in_data.m_upstreamConfigurations, in_data.m_upstreamConfigurationCount);
        data.m_downstreamConfigurations = ToCpp<Hermes::DownstreamConfiguration>(in_data.m_downstreamConfigurations, in_data.m_downstreamConfigurationCount);
        return data;
    }

    // ConnectionInfo
    inline HermesConnectionInfo ToC(const ConnectionInfo& in_data) 
    { 
        return{ToC(in_data.m_address), in_data.m_port, ToC(in_data.m_hostName)};
    }
    inline ConnectionInfo ToCpp(const HermesConnectionInfo& in_data)
    {
        return{ToCpp(in_data.m_address), in_data.m_port, ToCpp(in_data.m_hostName)};
    }

    // ServiceDescription
    inline HermesServiceDescription ToC(const ServiceDescription& in_data)
    {
        return{ToC(in_data.m_machineId), in_data.m_laneId, ToC(in_data.m_version)};
    }
    inline ServiceDescription ToCpp(const HermesServiceDescription& in_data)
    {
        ServiceDescription data(ToCpp(in_data.m_machineId), in_data.m_laneId);
        data.m_version = ToCpp(in_data.m_version);
        return data;
    }

    // MachineReadyData
    inline HermesMachineReadyData ToC(const MachineReadyData& in_data) { return{ToC(in_data.m_failedBoard)}; }
    inline MachineReadyData ToCpp(const HermesMachineReadyData& in_data)
    {
        return MachineReadyData(ToCpp(in_data.m_failedBoard));
    }

    // RevokeMachineReadyData
    inline HermesRevokeMachineReadyData ToC(const RevokeMachineReadyData&) { return{}; }
    inline RevokeMachineReadyData ToCpp(const HermesRevokeMachineReadyData&) { return{}; }

    // BoardAvailableData
    inline HermesBoardAvailableData ToC(const BoardAvailableData& in_data)
    {
        return{ToC(in_data.m_boardId), ToC(in_data.m_boardIdCreatedBy), ToC(in_data.m_failedBoard),
            ToC(in_data.m_optionalProductTypeId), ToC(in_data.m_flippedBoard), 
            ToC(in_data.m_optionalTopBarcode), ToC(in_data.m_optionalBottomBarcode),
            ToC(in_data.m_optionalLengthInMM), ToC(in_data.m_optionalWidthInMM), ToC(in_data.m_optionalThicknessInMM), 
            ToC(in_data.m_optionalConveyorSpeedInMMPerSecs),
            ToC(in_data.m_optionalTopClearanceHeightInMM), ToC(in_data.m_optionalBottomClearanceHeightInMM)};
    }
    inline BoardAvailableData ToCpp(const HermesBoardAvailableData& in_data) 
    { 
        BoardAvailableData data(ToCpp(in_data.m_boardId), ToCpp(in_data.m_boardIdCreatedBy),
            ToCpp(in_data.m_failedBoard),
            ToCpp(in_data.m_flippedBoard));
        data.m_optionalProductTypeId = ToOptional(in_data.m_optionalProductTypeId);
        data.m_optionalTopBarcode = ToOptional(in_data.m_optionalTopBarcode);
        data.m_optionalBottomBarcode = ToOptional(in_data.m_optionalBottomBarcode);
        data.m_optionalLengthInMM = ToOptional(in_data.m_pOptionalLengthInMM);
        data.m_optionalWidthInMM = ToOptional(in_data.m_pOptionalWidthInMM);
        data.m_optionalThicknessInMM = ToOptional(in_data.m_pOptionalThicknessInMM);
        data.m_optionalConveyorSpeedInMMPerSecs = ToOptional(in_data.m_pOptionalConveyorSpeedInMMPerSecs);
        data.m_optionalTopClearanceHeightInMM = ToOptional(in_data.m_pOptionalTopClearanceHeightInMM);
        data.m_optionalBottomClearanceHeightInMM = ToOptional(in_data.m_pOptionalBottomClearanceHeightInMM);
        return data;
    }

    // RevokeBoardAvailableData
    inline HermesRevokeBoardAvailableData ToC(const RevokeBoardAvailableData&) { return{}; }
    inline RevokeBoardAvailableData ToCpp(const HermesRevokeBoardAvailableData&) { return{}; }
    
    // StartTransportData
    inline HermesStartTransportData ToC(const StartTransportData& in_data)
    {
        return{ToC(in_data.m_boardId), ToC(in_data.m_optionalConveyorSpeedInMMPerSecs)};
    }
    inline StartTransportData ToCpp(const HermesStartTransportData& in_data)
    {
        StartTransportData data(ToCpp(in_data.m_boardId));
        data.m_optionalConveyorSpeedInMMPerSecs = ToOptional(in_data.m_pOptionalConveyorSpeedInMMPerSecs);
        return data;
    }

    // StopTransportData
    inline HermesStopTransportData ToC(const StopTransportData& in_data)
    {
        return{ToC(in_data.m_transferState), ToC(in_data.m_boardId)};
    }
    inline StopTransportData ToCpp(const HermesStopTransportData& in_data)
    {
        return StopTransportData(ToCpp(in_data.m_transferState),
            ToCpp(in_data.m_boardId));
    }

    // TransportFinishedData
    inline HermesTransportFinishedData ToC(const TransportFinishedData& in_data)
    {
        return{ToC(in_data.m_transferState), ToC(in_data.m_boardId)};
    }
    inline TransportFinishedData ToCpp(const HermesTransportFinishedData& in_data)
    {
        return TransportFinishedData(ToCpp(in_data.m_transferState),
            ToCpp(in_data.m_boardId));
    }

    // NotificationData
    inline HermesNotificationData ToC(const NotificationData& in_data)
    { 
        return{ToC(in_data.m_notificationCode), ToC(in_data.m_severity), ToC(in_data.m_description)};
    }
    inline NotificationData ToCpp(const HermesNotificationData& in_data)
    {
        return NotificationData(ToCpp(static_cast<EHermesNotificationCode>(in_data.m_notificationCode)), 
            ToCpp(static_cast<EHermesSeverity>(in_data.m_severity)),
            ToCpp(in_data.m_description));
    }

    // CheckAliveData
    inline HermesCheckAliveData ToC(const CheckAliveData&) { return{}; }
    inline CheckAliveData ToCpp(const HermesCheckAliveData&) { return{}; }

    // UpstreamSettings
    inline HermesUpstreamSettings ToC(const UpstreamSettings& in_data)
    {
        return{Hermes::ToC(in_data.m_machineId), Hermes::ToC(in_data.m_hostAddress), in_data.m_port, 
            in_data.m_checkAlivePeriodInSeconds, in_data.m_reconnectWaitTimeInSeconds, 
            ToC(in_data.m_checkState)};
    }
    inline UpstreamSettings ToCpp(const HermesUpstreamSettings& in_data)
    {
        UpstreamSettings data(ToCpp(in_data.m_machineId), ToCpp(in_data.m_hostAddress), in_data.m_port);
        data.m_checkAlivePeriodInSeconds = in_data.m_checkAlivePeriodInSeconds;
        data.m_reconnectWaitTimeInSeconds = in_data.m_reconnectWaitTimeInSeconds;
        data.m_checkState = ToCpp(in_data.m_checkState);
        return data; 
    }

    // DownstreamSettings
    inline HermesDownstreamSettings ToC(const DownstreamSettings& in_data)
    {
        return{Hermes::ToC(in_data.m_machineId), Hermes::ToC(in_data.m_optionalClientAddress), in_data.m_port,
            in_data.m_checkAlivePeriodInSeconds, in_data.m_reconnectWaitTimeInSeconds,
            ToC(in_data.m_checkState)};
    }
    inline DownstreamSettings ToCpp(const HermesDownstreamSettings& in_data)
    {
        DownstreamSettings data(ToCpp(in_data.m_machineId), in_data.m_port);
        data.m_optionalClientAddress = ToCpp(in_data.m_optionalClientAddress);
        data.m_checkAlivePeriodInSeconds = in_data.m_checkAlivePeriodInSeconds;
        data.m_reconnectWaitTimeInSeconds = in_data.m_reconnectWaitTimeInSeconds;
        data.m_checkState = ToCpp(in_data.m_checkState);
        return data;
    }

    // ConfigurationServiceSettings
    inline HermesConfigurationServiceSettings ToC(const ConfigurationServiceSettings& in_data)
    {
        return{in_data.m_port, in_data.m_reconnectWaitTimeInSeconds};
    }
    inline ConfigurationServiceSettings ToCpp(const HermesConfigurationServiceSettings& in_data)
    {
        ConfigurationServiceSettings data;
        data.m_port = in_data.m_port;
        data.m_reconnectWaitTimeInSeconds = in_data.m_reconnectWaitTimeInSeconds;
        return data;
    }

    // HermesError
    inline HermesError ToC(const Error& in_data) 
    { 
        return{ToC(in_data.m_code), ToC(in_data.m_text)};
    }
    inline Error ToCpp(const HermesError& in_data) 
    { 
        return Error(ToCpp(in_data.m_code), ToCpp(in_data.m_text)); 
    }


}
