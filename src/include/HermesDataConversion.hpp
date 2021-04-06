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

    inline void CppToC(const std::string& data, HermesStringView& result) { result = {data.data(), data.size()}; }
    inline void CToCpp(HermesStringView data, std::string& result) { result.assign(data.m_pData, data.m_size); }
    inline HermesStringView ToC(StringView data) { return{data.data(), data.size()}; }
    inline StringView ToCpp(HermesStringView data) { return{data.m_pData, data.m_size}; }

    inline void CppToC(const Optional<std::string>& data, HermesStringView& result)
    { 
        result = data ? HermesStringView{data->data(), data->size()} : HermesStringView{};
    }
    inline void CToCpp(HermesStringView data, Optional<std::string>& result)
    {
        result = data.m_pData ? Optional<std::string>{data.m_pData, data.m_size} : Optional<std::string>{};
    }

    inline void CppToC(double data, double& result) { result = data; }
    inline void CToCpp(double data, double& result) { result = data; }

    inline void CppToC(unsigned data, unsigned& result) { result = data; }
    inline void CToCpp(unsigned data, unsigned& result) { result = data; }

    inline void CppToC(uint16_t data, uint16_t& result) { result = data; }
    inline void CToCpp(uint16_t data, uint16_t& result) { result = data; }

    template<class CT, class CppT>
    void CToCpp(const CT* pData, CppT& result)
    {
        if (!pData)
            return;

        CToCpp(*pData, result);
    }

    template<class T>
    void CppToC(const Optional<T>& data, const T*& result)
    {
        result = data ? &*data : nullptr;
    }

    template<class CT, class CppT>
    void CToCpp(const CT* pData, Optional<CppT>& result)
    {
        if (!pData)
            return;

        result.emplace();
        CToCpp(*pData, *result);
    }

    template<class CppT, class CT>
    void CppToC(const Optional<CppT>& data, Optional<CT>& result)
    {
        if (!data)
            return;

        result.emplace(CT{});
        CppToC(*data, *result);
    }

    template<class CT>
    void Adjust(const Optional<CT>& data, const CT*& result)
    {
        result = data ? &(*data) : nullptr;
    }

    // struct for converting vom cpp to c vector:
    template<class CT>
    struct VectorHolder
    {
        std::vector<CT> m_values;
        std::vector<const CT*> m_pointers;
    };

    template<class CppT, class CT>
    void CppToC(const std::vector<CppT>& data, VectorHolder<CT>& result)
    {
        auto size = data.size();
        result.m_values.resize(size, CT{});
        for (uint32_t i = 0; i < size; ++i)
        {
            CppToC(data[i], result.m_values[i]);
        }
    }

    template<class CT, class CVector>
    void Adjust(VectorHolder<CT>& holder, CVector& result)
    {
        auto size = holder.m_values.size();
        holder.m_pointers.resize(size);
        for (uint32_t i = 0; i < size; ++i)
        {
            holder.m_pointers[i] = &holder.m_values[i];
        }
        result.m_pData = holder.m_pointers.data();
        result.m_size = size;
    }

    template<class CVector, class CppT>
    void CToCpp(const CVector& data, std::vector<CppT>& result)
    {
        result.resize(data.m_size);
        for (uint32_t i = 0; i < data.m_size; ++i)
        {
            CToCpp(*data.m_pData[i], result[i]);
        }
    }

    // enums
    static_assert(size(EState()) == cHERMES_STATE_ENUM_SIZE, "enum mismatch");
    inline EHermesState ToC(EState data) { return static_cast<EHermesState>(data); }
    inline EState ToCpp(EHermesState data) { return static_cast<EState>(data); }

    static_assert(size(ETraceType()) == cHERMES_TRACE_TYPE_ENUM_SIZE, "enum mismatch");
    inline EHermesTraceType ToC(ETraceType data) { return static_cast<EHermesTraceType>(data); }
    inline ETraceType ToCpp(EHermesTraceType data) { return static_cast<ETraceType>(data); }

    static_assert(size(ECheckAliveType()) == cHERMES_CHECK_ALIVE_TYPE_ENUM_SIZE, "enum mismatch");
    inline void CppToC(ECheckAliveType data, EHermesCheckAliveType& result) { result = static_cast<EHermesCheckAliveType>(data); }
    inline void CToCpp(EHermesCheckAliveType data, ECheckAliveType& result) { result = static_cast<ECheckAliveType>(data); }

    static_assert(size(EBoardQuality()) == cHERMES_BOARD_QUALITY_ENUM_SIZE, "enum mismatch");    
    inline void CppToC(EBoardQuality data, EHermesBoardQuality& result) { result = static_cast<EHermesBoardQuality>(data); }
    inline void CToCpp(EHermesBoardQuality data, EBoardQuality& result) { result = static_cast<EBoardQuality>(data); }

    static_assert(size(EFlippedBoard()) == cHERMES_FLIPPED_BOARD_ENUM_SIZE, "enum mismatch");    
    inline void CppToC(EFlippedBoard data, EHermesFlippedBoard& result) { result = static_cast<EHermesFlippedBoard>(data); }
    inline void CToCpp(EHermesFlippedBoard data, EFlippedBoard& result) { result = static_cast<EFlippedBoard>(data); }

    static_assert(size(ETransferState()) == cHERMES_TRANSFER_STATE_ENUM_SIZE, "enum mismatch");    
    inline void CppToC(ETransferState data, EHermesTransferState& result) { result = static_cast<EHermesTransferState>(data); }
    inline void CToCpp(EHermesTransferState data, ETransferState& result) { result = static_cast<ETransferState>(data); }

    static_assert(size(ENotificationCode()) == cHERMES_NOTIFICATION_CODE_ENUM_SIZE, "enum mismatch");    
    inline void CppToC(ENotificationCode data, EHermesNotificationCode& result) { result = static_cast<EHermesNotificationCode>(data); }
    inline void CToCpp(EHermesNotificationCode data, ENotificationCode& result) { result = static_cast<ENotificationCode>(data); }

    static_assert(size(ESeverity()) == cHERMES_SEVERITY_ENUM_SIZE, "enum mismatch");    
    inline void CppToC(ESeverity data, EHermesSeverity& result) { result = static_cast<EHermesSeverity>(data); }
    inline void CToCpp(EHermesSeverity data, ESeverity& result) { result = static_cast<ESeverity>(data); }

    static_assert(size(ECheckState()) == cHERMES_CHECK_STATE_ENUM_SIZE, "enum mismatch");    
    inline void CppToC(ECheckState data, EHermesCheckState& result) { result = static_cast<EHermesCheckState>(data); }
    inline void CToCpp(EHermesCheckState data, ECheckState& result) { result = static_cast<ECheckState>(data); }

    static_assert(size(EErrorCode()) == cHERMES_ERROR_CODE_ENUM_SIZE, "enum mismatch");    
    inline void CppToC(EErrorCode data, EHermesErrorCode& result) { result = static_cast<EHermesErrorCode>(data); }
    inline void CToCpp(EHermesErrorCode data, EErrorCode& result) { result = static_cast<EErrorCode>(data); }

    static_assert(size(ECheckAliveResponseMode()) == cHERMES_CHECK_ALIVE_RESPONSE_MODE_ENUM_SIZE, "enum mismatch");
    inline void CppToC(ECheckAliveResponseMode data, EHermesCheckAliveResponseMode& result) { result = static_cast<EHermesCheckAliveResponseMode>(data); }
    inline void CToCpp(EHermesCheckAliveResponseMode data, ECheckAliveResponseMode& result) { result = static_cast<ECheckAliveResponseMode>(data); }

    static_assert(size(EBoardArrivedTransfer()) == cHERMES_BOARD_ARRIVED_TRANSFER_ENUM_SIZE, "enum mismatch");
    inline void CppToC(EBoardArrivedTransfer data, EHermesBoardArrivedTransfer& result) { result = static_cast<EHermesBoardArrivedTransfer>(data); }
    inline void CToCpp(EHermesBoardArrivedTransfer data, EBoardArrivedTransfer& result) { result = static_cast<EBoardArrivedTransfer>(data); }

    static_assert(size(EBoardDepartedTransfer()) == cHERMES_BOARD_DEPARTED_TRANSFER_ENUM_SIZE, "enum mismatch");
    inline void CppToC(EBoardDepartedTransfer data, EHermesBoardDepartedTransfer& result) { result = static_cast<EHermesBoardDepartedTransfer>(data); }
    inline void CToCpp(EHermesBoardDepartedTransfer data, EBoardDepartedTransfer& result) { result = static_cast<EBoardDepartedTransfer>(data); }

    static_assert(size(EVerticalState()) == cHERMES_VERTICAL_STATE_ENUM_SIZE, "enum mismatch");
    inline EHermesVerticalState ToC(EVerticalState data) { return static_cast<EHermesVerticalState>(data); }
    inline EVerticalState ToCpp(EHermesVerticalState data) { return static_cast<EVerticalState>(data); }

    // UpstreamConfiguration
    inline void CppToC(const UpstreamConfiguration& data, HermesUpstreamConfiguration& result)
    {
        CppToC(data.m_upstreamLaneId, result.m_upstreamLaneId);
        CppToC(data.m_optionalUpstreamInterfaceId, result.m_optionalUpstreamInterfaceId);
        CppToC(data.m_hostAddress, result.m_hostAddress);
        CppToC(data.m_port, result.m_port);
    }

    inline void CToCpp(const HermesUpstreamConfiguration& data, UpstreamConfiguration& result)
    {
        CToCpp(data.m_upstreamLaneId, result.m_upstreamLaneId);
        CToCpp(data.m_optionalUpstreamInterfaceId, result.m_optionalUpstreamInterfaceId);
        CToCpp(data.m_port, result.m_port);
        CToCpp(data.m_hostAddress, result.m_hostAddress);
    }

    // DownstreamConfiguration
    inline void CppToC(const DownstreamConfiguration& data, HermesDownstreamConfiguration& result)
    {
        CppToC(data.m_downstreamLaneId, result.m_downstreamLaneId);
        CppToC(data.m_optionalDownstreamInterfaceId, result.m_optionalDownstreamInterfaceId);
        CppToC(data.m_optionalClientAddress, result.m_optionalClientAddress);
        CppToC(data.m_port, result.m_port);
    }

    inline void CToCpp(const HermesDownstreamConfiguration& data, DownstreamConfiguration& result)
    {
        CToCpp(data.m_downstreamLaneId, result.m_downstreamLaneId);
        CToCpp(data.m_optionalDownstreamInterfaceId, result.m_optionalDownstreamInterfaceId);
        CToCpp(data.m_optionalClientAddress, result.m_optionalClientAddress);
        CToCpp(data.m_port, result.m_port);
    }


    // SetConfigurationData
    class SetConfigurationDataHolder
    {
    public:
        explicit SetConfigurationDataHolder(const SetConfigurationData& data) 
        {
            CppToC(data.m_machineId, m_data.m_machineId);
            CppToC(data.m_optionalSupervisorySystemPort, m_data.m_pOptionalSupervisorySystemPort);
            CppToC(data.m_upstreamConfigurations, m_upstreamConfigurations);
            CppToC(data.m_downstreamConfigurations, m_downstreamConfigurations);
        }

        const HermesSetConfigurationData* operator&()
        {
            Adjust(m_upstreamConfigurations, m_data.m_upstreamConfigurations);
            Adjust(m_downstreamConfigurations, m_data.m_downstreamConfigurations);
            return &m_data;
        }

    private:
        VectorHolder<HermesUpstreamConfiguration> m_upstreamConfigurations;
        VectorHolder<HermesDownstreamConfiguration> m_downstreamConfigurations;
        mutable HermesSetConfigurationData m_data{};
    };

    inline SetConfigurationDataHolder ToC(const SetConfigurationData& data) { return SetConfigurationDataHolder(data); }
    inline SetConfigurationData ToCpp(const HermesSetConfigurationData& data)
    {
        SetConfigurationData result;
        CToCpp(data.m_machineId, result.m_machineId);
        CToCpp(data.m_pOptionalSupervisorySystemPort, result.m_optionalSupervisorySystemPort);
        CToCpp(data.m_upstreamConfigurations, result.m_upstreamConfigurations);
        CToCpp(data.m_downstreamConfigurations, result.m_downstreamConfigurations);
        return result;
    }

    // GetConfigurationData
    inline HermesGetConfigurationData ToC(const GetConfigurationData&) { return{}; }
    inline GetConfigurationData ToCpp(const HermesGetConfigurationData&) { return{}; }

    // CurrentConfigurationData
    class CurrentConfigurationDataHolder
    {
    public:
        explicit CurrentConfigurationDataHolder(const CurrentConfigurationData& data)
        {
            CppToC(data.m_optionalMachineId, m_data.m_optionalMachineId);
            CppToC(data.m_optionalSupervisorySystemPort, m_data.m_pOptionalSupervisorySystemPort);
            CppToC(data.m_upstreamConfigurations, m_upstreamConfigurations);
            CppToC(data.m_downstreamConfigurations, m_downstreamConfigurations);
        }

        const HermesCurrentConfigurationData* operator&()
        {
            Adjust(m_upstreamConfigurations, m_data.m_upstreamConfigurations);
            Adjust(m_downstreamConfigurations, m_data.m_downstreamConfigurations);
            return &m_data;
        }

    private:
        VectorHolder<HermesUpstreamConfiguration> m_upstreamConfigurations;
        VectorHolder<HermesDownstreamConfiguration> m_downstreamConfigurations;
        HermesCurrentConfigurationData m_data{};
    };

    inline CurrentConfigurationDataHolder ToC(const CurrentConfigurationData& data) { return CurrentConfigurationDataHolder(data); }
    inline CurrentConfigurationData ToCpp(const HermesCurrentConfigurationData& data)
    {
        CurrentConfigurationData result;
        CToCpp(data.m_optionalMachineId, result.m_optionalMachineId);
        CToCpp(data.m_pOptionalSupervisorySystemPort, result.m_optionalSupervisorySystemPort);
        CToCpp(data.m_upstreamConfigurations, result.m_upstreamConfigurations);
        CToCpp(data.m_downstreamConfigurations, result.m_downstreamConfigurations);
        return result;
    }

    // ConnectionInfo
    inline HermesConnectionInfo ToC(const ConnectionInfo& data) 
    {
        HermesConnectionInfo result{};
        CppToC(data.m_address, result.m_address);
        CppToC(data.m_port, result.m_port);
        CppToC(data.m_hostName, result.m_hostName);
        return result;
    }
    inline ConnectionInfo ToCpp(const HermesConnectionInfo& data)
    {
        ConnectionInfo result;
        CToCpp(data.m_address, result.m_address);
        CToCpp(data.m_port, result.m_port);
        CToCpp(data.m_hostName, result.m_hostName);
        return result;
    }

    // Features
    inline void CppToC(const FeatureBoardForecast&, HermesFeatureBoardForecast&) {};
    inline void CppToC(const FeatureCheckAliveResponse&, HermesFeatureCheckAliveResponse&) {};
    inline void CppToC(const FeatureQueryBoardInfo&, HermesFeatureQueryBoardInfo&) {};
    inline void CppToC(const FeatureSendBoardInfo&, HermesFeatureSendBoardInfo&) {};

    inline void CToCpp(const HermesFeatureBoardForecast&, FeatureBoardForecast&) {};
    inline void CToCpp(const HermesFeatureCheckAliveResponse&, FeatureCheckAliveResponse&) {};
    inline void CToCpp(const HermesFeatureQueryBoardInfo&, FeatureQueryBoardInfo&) {};
    inline void CToCpp(const HermesFeatureSendBoardInfo&, FeatureSendBoardInfo&) {};

    // SupportedFeatures
    struct SupportedFeaturesHolder
    {
        Optional<HermesFeatureBoardForecast> m_optionalFeatureBoardForecast{};
        Optional<HermesFeatureCheckAliveResponse> m_optionalFeatureCheckAliveResponse{};
        Optional<HermesFeatureQueryBoardInfo> m_optionalFeatureQueryBoardInfo{};
        Optional<HermesFeatureSendBoardInfo> m_optionalFeatureSendBoardInfo{};
        HermesSupportedFeatures m_data{};
    };

    inline void CppToC(const SupportedFeatures& data, SupportedFeaturesHolder& result)
    {
        CppToC(data.m_optionalFeatureBoardForecast, result.m_optionalFeatureBoardForecast);
        CppToC(data.m_optionalFeatureCheckAliveResponse, result.m_optionalFeatureCheckAliveResponse);
        CppToC(data.m_optionalFeatureQueryBoardInfo, result.m_optionalFeatureQueryBoardInfo);
        CppToC(data.m_optionalFeatureSendBoardInfo, result.m_optionalFeatureSendBoardInfo);
    }

    inline void CToCpp(const HermesSupportedFeatures& data, SupportedFeatures& result)
    {
        CToCpp(data.m_pOptionalFeatureBoardForecast, result.m_optionalFeatureBoardForecast);
        CToCpp(data.m_pOptionalFeatureCheckAliveResponse, result.m_optionalFeatureCheckAliveResponse);
        CToCpp(data.m_pOptionalFeatureQueryBoardInfo, result.m_optionalFeatureQueryBoardInfo);
        CToCpp(data.m_pOptionalFeatureSendBoardInfo, result.m_optionalFeatureSendBoardInfo);
    }

    inline void Adjust(SupportedFeaturesHolder& data, const HermesSupportedFeatures*& pResult)
    {
        Adjust(data.m_optionalFeatureBoardForecast, data.m_data.m_pOptionalFeatureBoardForecast);
        Adjust(data.m_optionalFeatureCheckAliveResponse, data.m_data.m_pOptionalFeatureCheckAliveResponse);
        Adjust(data.m_optionalFeatureQueryBoardInfo, data.m_data.m_pOptionalFeatureQueryBoardInfo);
        Adjust(data.m_optionalFeatureSendBoardInfo, data.m_data.m_pOptionalFeatureSendBoardInfo);
        pResult = &data.m_data;
    }

    class ServiceDescriptionDataHolder
    {
    public:
        explicit ServiceDescriptionDataHolder(const ServiceDescriptionData& data)
        {
            CppToC(data.m_machineId, m_data.m_machineId);
            CppToC(data.m_laneId, m_data.m_laneId);
            CppToC(data.m_optionalInterfaceId, m_data.m_optionalInterfaceId);
            CppToC(data.m_version, m_data.m_version);
            CppToC(data.m_supportedFeatures, m_supportedFeatures);
        }

        const HermesServiceDescriptionData* operator&()
        {
            Adjust(m_supportedFeatures, m_data.m_pSupportedFeatures);
            return &m_data;
        }

    private:
        SupportedFeaturesHolder m_supportedFeatures;
        HermesServiceDescriptionData m_data{};
    };

    // ServiceDescriptionData
    inline ServiceDescriptionDataHolder ToC(const ServiceDescriptionData& data)
    {
        return ServiceDescriptionDataHolder(data);
    }
    inline ServiceDescriptionData ToCpp(const HermesServiceDescriptionData& data)
    {
        ServiceDescriptionData result;
        CToCpp(data.m_machineId, result.m_machineId);
        CToCpp(data.m_laneId, result.m_laneId);
        CToCpp(data.m_optionalInterfaceId, result.m_optionalInterfaceId);
        CToCpp(data.m_version, result.m_version);
        CToCpp(data.m_pSupportedFeatures, result.m_supportedFeatures);
        return result;
    }

    // MachineReadyData
    class MachineReadyDataHolder
    {
    public:
        explicit MachineReadyDataHolder(const MachineReadyData& data)
        {
            CppToC(data.m_optionalFlippedBoard, m_optionalFlippedBoard);

            CppToC(data.m_failedBoard, m_data.m_failedBoard);
            CppToC(data.m_optionalForecastId, m_data.m_optionalForecastId);
            CppToC(data.m_optionalBoardId, m_data.m_optionalBoardId);
            CppToC(data.m_optionalProductTypeId, m_data.m_optionalProductTypeId);
            CppToC(data.m_optionalTopBarcode, m_data.m_optionalTopBarcode);
            CppToC(data.m_optionalBottomBarcode, m_data.m_optionalBottomBarcode);
            CppToC(data.m_optionalLengthInMM, m_data.m_pOptionalLengthInMM);
            CppToC(data.m_optionalWidthInMM, m_data.m_pOptionalWidthInMM);
            CppToC(data.m_optionalThicknessInMM, m_data.m_pOptionalThicknessInMM);
            CppToC(data.m_optionalConveyorSpeedInMMPerSecs, m_data.m_pOptionalConveyorSpeedInMMPerSecs);
            CppToC(data.m_optionalTopClearanceHeightInMM, m_data.m_pOptionalTopClearanceHeightInMM);
            CppToC(data.m_optionalBottomClearanceHeightInMM, m_data.m_pOptionalBottomClearanceHeightInMM);
            CppToC(data.m_optionalWeightInGrams, m_data.m_pOptionalWeightInGrams);
            CppToC(data.m_optionalWorkOrderId, m_data.m_optionalWorkOrderId);
            CppToC(data.m_optionalBatchId, m_data.m_optionalBatchId);
        }


        const HermesMachineReadyData* operator&()
        {
            Adjust(m_optionalFlippedBoard, m_data.m_pOptionalFlippedBoard);
            return &m_data;
        }

    private:
        Optional<EHermesFlippedBoard> m_optionalFlippedBoard;
        HermesMachineReadyData m_data{};
    };

    inline MachineReadyDataHolder ToC(const MachineReadyData& data) 
    {
        return MachineReadyDataHolder{data};
    }
    inline MachineReadyData ToCpp(const HermesMachineReadyData& data)
    {
        MachineReadyData result;
        CToCpp(data.m_failedBoard, result.m_failedBoard);
        CToCpp(data.m_optionalForecastId, result.m_optionalForecastId);
        CToCpp(data.m_optionalBoardId, result.m_optionalBoardId);
        CToCpp(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CToCpp(data.m_pOptionalFlippedBoard, result.m_optionalFlippedBoard);
        CToCpp(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CToCpp(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CToCpp(data.m_pOptionalLengthInMM, result.m_optionalLengthInMM);
        CToCpp(data.m_pOptionalWidthInMM, result.m_optionalWidthInMM);
        CToCpp(data.m_pOptionalThicknessInMM, result.m_optionalThicknessInMM);
        CToCpp(data.m_pOptionalConveyorSpeedInMMPerSecs, result.m_optionalConveyorSpeedInMMPerSecs);
        CToCpp(data.m_pOptionalTopClearanceHeightInMM, result.m_optionalTopClearanceHeightInMM);
        CToCpp(data.m_pOptionalBottomClearanceHeightInMM, result.m_optionalBottomClearanceHeightInMM);
        CToCpp(data.m_pOptionalWeightInGrams, result.m_optionalWeightInGrams);
        CToCpp(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CToCpp(data.m_optionalBatchId, result.m_optionalBatchId);
        return result;
    }

    // RevokeMachineReadyData
    inline HermesRevokeMachineReadyData ToC(const RevokeMachineReadyData&) { return{}; }
    inline RevokeMachineReadyData ToCpp(const HermesRevokeMachineReadyData&) { return{}; }

    // BoardAvailableData
    inline HermesBoardAvailableData ToC(const BoardAvailableData& data)
    {
        HermesBoardAvailableData result{};
        CppToC(data.m_boardId, result.m_boardId);
        CppToC(data.m_boardIdCreatedBy, result.m_boardIdCreatedBy);
        CppToC(data.m_failedBoard, result.m_failedBoard);
        CppToC(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CppToC(data.m_flippedBoard, result.m_flippedBoard);
        CppToC(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CppToC(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CppToC(data.m_optionalLengthInMM, result.m_pOptionalLengthInMM);
        CppToC(data.m_optionalWidthInMM, result.m_pOptionalWidthInMM);
        CppToC(data.m_optionalThicknessInMM, result.m_pOptionalThicknessInMM);
        CppToC(data.m_optionalConveyorSpeedInMMPerSecs, result.m_pOptionalConveyorSpeedInMMPerSecs);
        CppToC(data.m_optionalTopClearanceHeightInMM, result.m_pOptionalTopClearanceHeightInMM);
        CppToC(data.m_optionalBottomClearanceHeightInMM, result.m_pOptionalBottomClearanceHeightInMM);
        CppToC(data.m_optionalWeightInGrams, result.m_pOptionalWeightInGrams);
        CppToC(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CppToC(data.m_optionalBatchId, result.m_optionalBatchId);
        return result;
    }
    inline BoardAvailableData ToCpp(const HermesBoardAvailableData& data) 
    { 
        BoardAvailableData result;
        CToCpp(data.m_boardId, result.m_boardId);
        CToCpp(data.m_boardIdCreatedBy, result.m_boardIdCreatedBy);
        CToCpp(data.m_failedBoard, result.m_failedBoard);
        CToCpp(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CToCpp(data.m_flippedBoard, result.m_flippedBoard);
        CToCpp(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CToCpp(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CToCpp(data.m_pOptionalLengthInMM, result.m_optionalLengthInMM);
        CToCpp(data.m_pOptionalWidthInMM, result.m_optionalWidthInMM);
        CToCpp(data.m_pOptionalThicknessInMM, result.m_optionalThicknessInMM);
        CToCpp(data.m_pOptionalConveyorSpeedInMMPerSecs, result.m_optionalConveyorSpeedInMMPerSecs);
        CToCpp(data.m_pOptionalTopClearanceHeightInMM, result.m_optionalTopClearanceHeightInMM);
        CToCpp(data.m_pOptionalBottomClearanceHeightInMM, result.m_optionalBottomClearanceHeightInMM);
        CToCpp(data.m_pOptionalWeightInGrams, result.m_optionalWeightInGrams);
        CToCpp(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CToCpp(data.m_optionalBatchId, result.m_optionalBatchId);
        return result;
    }

    // RevokeBoardAvailableData
    inline HermesRevokeBoardAvailableData ToC(const RevokeBoardAvailableData&) { return{}; }
    inline RevokeBoardAvailableData ToCpp(const HermesRevokeBoardAvailableData&) { return{}; }
    
    // StartTransportData
    inline HermesStartTransportData ToC(const StartTransportData& data)
    {
        HermesStartTransportData result{};
        CppToC(data.m_boardId, result.m_boardId);
        CppToC(data.m_optionalConveyorSpeedInMMPerSecs, result.m_pOptionalConveyorSpeedInMMPerSecs);
        return result;
    }
    inline StartTransportData ToCpp(const HermesStartTransportData& data)
    {
        StartTransportData result;
        CToCpp(data.m_boardId, result.m_boardId);
        CToCpp(data.m_pOptionalConveyorSpeedInMMPerSecs, result.m_optionalConveyorSpeedInMMPerSecs);
        return result;
    }

    // StopTransportData
    inline HermesStopTransportData ToC(const StopTransportData& data)
    {
        HermesStopTransportData result{};
        CppToC(data.m_transferState, result.m_transferState);
        CppToC(data.m_boardId, result.m_boardId);
        return result;
    }
    inline StopTransportData ToCpp(const HermesStopTransportData& data)
    {
        StopTransportData result;
        CToCpp(data.m_transferState, result.m_transferState);
        CToCpp(data.m_boardId, result.m_boardId);
        return result;
    }

    // TransportFinishedData
    inline HermesTransportFinishedData ToC(const TransportFinishedData& data)
    {
        HermesTransportFinishedData result{};
        CppToC(data.m_transferState, result.m_transferState);
        CppToC(data.m_boardId, result.m_boardId);
        return result;
    }
    inline TransportFinishedData ToCpp(const HermesTransportFinishedData& data)
    {
        TransportFinishedData result;
        CToCpp(data.m_transferState, result.m_transferState);
        CToCpp(data.m_boardId, result.m_boardId);
        return result;
    }

    // NotificationData
    inline HermesNotificationData ToC(const NotificationData& data)
    { 
        HermesNotificationData result{};
        CppToC(data.m_notificationCode, result.m_notificationCode);
        CppToC(data.m_severity, result.m_severity);
        CppToC(data.m_description, result.m_description);
        return result;
    }
    inline NotificationData ToCpp(const HermesNotificationData& data)
    {
        NotificationData result;
        CToCpp(data.m_notificationCode, result.m_notificationCode);
        CToCpp(data.m_severity, result.m_severity);
        CToCpp(data.m_description, result.m_description);
        return result;
    }

    // CheckAliveData
    class CheckAliveDataHolder
    {
    public:
        explicit CheckAliveDataHolder(const CheckAliveData& data)
        {
            CppToC(data.m_optionalType, m_optionalType);
            CppToC(data.m_optionalId, m_data.m_optionalId);
        }

        const HermesCheckAliveData* operator&()
        {
            Adjust(m_optionalType, m_data.m_pOptionalType);
            return &m_data;
        }

    private:
        Optional<EHermesCheckAliveType> m_optionalType;
        HermesCheckAliveData m_data{};
    };

    inline CheckAliveDataHolder ToC(const CheckAliveData& data)
    {
        return CheckAliveDataHolder{data};
    }
    inline CheckAliveData ToCpp(const HermesCheckAliveData& data)
    {
        CheckAliveData result;
        CToCpp(data.m_pOptionalType, result.m_optionalType);
        CToCpp(data.m_optionalId, result.m_optionalId);
        return result;
    }

    // BoardForecastData
    inline HermesBoardForecastData ToC(const BoardForecastData& data)
    {
        HermesBoardForecastData result{};
        CppToC(data.m_optionalForecastId, result.m_optionalForecastId);
        CppToC(data.m_optionalTimeUntilAvailableInSeconds, result.m_pOptionalTimeUntilAvailableInSeconds);
        CppToC(data.m_optionalBoardId, result.m_optionalBoardId);
        CppToC(data.m_optionalBoardIdCreatedBy, result.m_optionalBoardIdCreatedBy);
        CppToC(data.m_failedBoard, result.m_failedBoard);
        CppToC(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CppToC(data.m_flippedBoard, result.m_flippedBoard);
        CppToC(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CppToC(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CppToC(data.m_optionalLengthInMM, result.m_pOptionalLengthInMM);
        CppToC(data.m_optionalWidthInMM, result.m_pOptionalWidthInMM);
        CppToC(data.m_optionalThicknessInMM, result.m_pOptionalThicknessInMM);
        CppToC(data.m_optionalConveyorSpeedInMMPerSecs, result.m_pOptionalConveyorSpeedInMMPerSecs);
        CppToC(data.m_optionalTopClearanceHeightInMM, result.m_pOptionalTopClearanceHeightInMM);
        CppToC(data.m_optionalBottomClearanceHeightInMM, result.m_pOptionalBottomClearanceHeightInMM);
        CppToC(data.m_optionalWeightInGrams, result.m_pOptionalWeightInGrams);
        CppToC(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CppToC(data.m_optionalBatchId, result.m_optionalBatchId);
        return result;
    }
    inline BoardForecastData ToCpp(const HermesBoardForecastData& data)
    {
        BoardForecastData result;
        CToCpp(data.m_optionalForecastId, result.m_optionalForecastId);
        CToCpp(data.m_pOptionalTimeUntilAvailableInSeconds, result.m_optionalTimeUntilAvailableInSeconds);
        CToCpp(data.m_optionalBoardId, result.m_optionalBoardId);
        CToCpp(data.m_optionalBoardIdCreatedBy, result.m_optionalBoardIdCreatedBy);
        CToCpp(data.m_failedBoard, result.m_failedBoard);
        CToCpp(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CToCpp(data.m_flippedBoard, result.m_flippedBoard);
        CToCpp(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CToCpp(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CToCpp(data.m_pOptionalLengthInMM, result.m_optionalLengthInMM);
        CToCpp(data.m_pOptionalWidthInMM, result.m_optionalWidthInMM);
        CToCpp(data.m_pOptionalThicknessInMM, result.m_optionalThicknessInMM);
        CToCpp(data.m_pOptionalConveyorSpeedInMMPerSecs, result.m_optionalConveyorSpeedInMMPerSecs);
        CToCpp(data.m_pOptionalTopClearanceHeightInMM, result.m_optionalTopClearanceHeightInMM);
        CToCpp(data.m_pOptionalBottomClearanceHeightInMM, result.m_optionalBottomClearanceHeightInMM);
        CToCpp(data.m_pOptionalWeightInGrams, result.m_optionalWeightInGrams);
        CToCpp(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CToCpp(data.m_optionalBatchId, result.m_optionalBatchId);
        return result;
    }

    // QueryBoardInfoData
    inline HermesQueryBoardInfoData ToC(const QueryBoardInfoData& data)
    {
        HermesQueryBoardInfoData result{};
        CppToC(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CppToC(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        return result;
    }
    inline QueryBoardInfoData ToCpp(const HermesQueryBoardInfoData& data)
    {
        QueryBoardInfoData result;
        CToCpp(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CToCpp(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        return result;
    }

    // SendBoardInfo
    class SendBoardInfoDataHolder
    {
    public:
        explicit SendBoardInfoDataHolder(const SendBoardInfoData& data)
        {
            CppToC(data.m_optionalFailedBoard, m_optionalFailedBoard);
            CppToC(data.m_optionalFlippedBoard, m_optionalFlippedBoard);

            CppToC(data.m_optionalBoardId, m_data.m_optionalBoardId);
            CppToC(data.m_optionalBoardIdCreatedBy, m_data.m_optionalBoardIdCreatedBy);
            CppToC(data.m_optionalProductTypeId, m_data.m_optionalProductTypeId);
            CppToC(data.m_optionalTopBarcode, m_data.m_optionalTopBarcode);
            CppToC(data.m_optionalBottomBarcode, m_data.m_optionalBottomBarcode);
            CppToC(data.m_optionalLengthInMM, m_data.m_pOptionalLengthInMM);
            CppToC(data.m_optionalWidthInMM, m_data.m_pOptionalWidthInMM);
            CppToC(data.m_optionalThicknessInMM, m_data.m_pOptionalThicknessInMM);
            CppToC(data.m_optionalConveyorSpeedInMMPerSecs, m_data.m_pOptionalConveyorSpeedInMMPerSecs);
            CppToC(data.m_optionalTopClearanceHeightInMM, m_data.m_pOptionalTopClearanceHeightInMM);
            CppToC(data.m_optionalBottomClearanceHeightInMM, m_data.m_pOptionalBottomClearanceHeightInMM);
            CppToC(data.m_optionalWeightInGrams, m_data.m_pOptionalWeightInGrams);
            CppToC(data.m_optionalWorkOrderId, m_data.m_optionalWorkOrderId);
            CppToC(data.m_optionalBatchId, m_data.m_optionalBatchId);
        }


        const HermesSendBoardInfoData* operator&()
        {
            Adjust(m_optionalFailedBoard, m_data.m_pOptionalFailedBoard);
            Adjust(m_optionalFlippedBoard, m_data.m_pOptionalFlippedBoard);
            return &m_data;
        }

    private:
        Optional<EHermesBoardQuality> m_optionalFailedBoard;
        Optional<EHermesFlippedBoard> m_optionalFlippedBoard;
        HermesSendBoardInfoData m_data{};
    };

    inline SendBoardInfoDataHolder ToC(const SendBoardInfoData& data)
    {
        return SendBoardInfoDataHolder{data};
    }
    inline SendBoardInfoData ToCpp(const HermesSendBoardInfoData& data)
    {
        SendBoardInfoData result;
        CToCpp(data.m_optionalBoardId, result.m_optionalBoardId);
        CToCpp(data.m_optionalBoardIdCreatedBy, result.m_optionalBoardIdCreatedBy);
        CToCpp(data.m_pOptionalFailedBoard, result.m_optionalFailedBoard);
        CToCpp(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CToCpp(data.m_pOptionalFlippedBoard, result.m_optionalFlippedBoard);
        CToCpp(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CToCpp(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CToCpp(data.m_pOptionalLengthInMM, result.m_optionalLengthInMM);
        CToCpp(data.m_pOptionalWidthInMM, result.m_optionalWidthInMM);
        CToCpp(data.m_pOptionalThicknessInMM, result.m_optionalThicknessInMM);
        CToCpp(data.m_pOptionalConveyorSpeedInMMPerSecs, result.m_optionalConveyorSpeedInMMPerSecs);
        CToCpp(data.m_pOptionalTopClearanceHeightInMM, result.m_optionalTopClearanceHeightInMM);
        CToCpp(data.m_pOptionalBottomClearanceHeightInMM, result.m_optionalBottomClearanceHeightInMM);
        CToCpp(data.m_pOptionalWeightInGrams, result.m_optionalWeightInGrams);
        CToCpp(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CToCpp(data.m_optionalBatchId, result.m_optionalBatchId);
        return result;
    }

    // SupervisoryFeatures
    inline void CppToC(const FeatureConfiguration&, HermesFeatureConfiguration&) {};
    inline void CppToC(const FeatureBoardTracking&, HermesFeatureBoardTracking&) {};
    inline void CppToC(const FeatureQueryWorkOrderInfo&, HermesFeatureQueryWorkOrderInfo&) {};
    inline void CppToC(const FeatureSendWorkOrderInfo&, HermesFeatureSendWorkOrderInfo&) {};

    inline void CToCpp(const HermesFeatureConfiguration&, FeatureConfiguration&) {};
    inline void CToCpp(const HermesFeatureBoardTracking&, FeatureBoardTracking&) {};
    inline void CToCpp(const HermesFeatureQueryWorkOrderInfo&, FeatureQueryWorkOrderInfo&) {};
    inline void CToCpp(const HermesFeatureSendWorkOrderInfo&, FeatureSendWorkOrderInfo&) {};

    // SupervisoryFeatures
    struct SupervisoryFeaturesHolder
    {
        Optional<HermesFeatureConfiguration> m_optionalFeatureConfiguration{};
        Optional<HermesFeatureCheckAliveResponse> m_optionalFeatureCheckAliveResponse{};
        Optional<HermesFeatureBoardTracking> m_optionalFeatureBoardTracking{};
        Optional<HermesFeatureQueryWorkOrderInfo> m_optionalFeatureQueryWorkOrderInfo{};
        Optional<HermesFeatureSendWorkOrderInfo> m_optionalFeatureSendWorkOrderInfo{};
        HermesSupervisoryFeatures m_data{};
    };

    inline void CppToC(const SupervisoryFeatures& data, SupervisoryFeaturesHolder& result)
    {
        CppToC(data.m_optionalFeatureConfiguration, result.m_optionalFeatureConfiguration);
        CppToC(data.m_optionalFeatureCheckAliveResponse, result.m_optionalFeatureCheckAliveResponse);
        CppToC(data.m_optionalFeatureBoardTracking, result.m_optionalFeatureBoardTracking);
        CppToC(data.m_optionalFeatureQueryWorkOrderInfo, result.m_optionalFeatureQueryWorkOrderInfo);
        CppToC(data.m_optionalFeatureSendWorkOrderInfo, result.m_optionalFeatureSendWorkOrderInfo);
    }

    inline void CToCpp(const HermesSupervisoryFeatures& data, SupervisoryFeatures& result)
    {
        CToCpp(data.m_pOptionalFeatureConfiguration, result.m_optionalFeatureConfiguration);
        CToCpp(data.m_pOptionalFeatureCheckAliveResponse, result.m_optionalFeatureCheckAliveResponse);
        CToCpp(data.m_pOptionalFeatureBoardTracking, result.m_optionalFeatureBoardTracking);
        CToCpp(data.m_pOptionalFeatureQueryWorkOrderInfo, result.m_optionalFeatureQueryWorkOrderInfo);
        CToCpp(data.m_pOptionalFeatureSendWorkOrderInfo, result.m_optionalFeatureSendWorkOrderInfo);
    }

    inline void Adjust(SupervisoryFeaturesHolder& data, const HermesSupervisoryFeatures*& pResult)
    {
        Adjust(data.m_optionalFeatureConfiguration, data.m_data.m_pOptionalFeatureConfiguration);
        Adjust(data.m_optionalFeatureCheckAliveResponse, data.m_data.m_pOptionalFeatureCheckAliveResponse);
        Adjust(data.m_optionalFeatureBoardTracking, data.m_data.m_pOptionalFeatureBoardTracking);
        Adjust(data.m_optionalFeatureQueryWorkOrderInfo, data.m_data.m_pOptionalFeatureQueryWorkOrderInfo);
        Adjust(data.m_optionalFeatureSendWorkOrderInfo, data.m_data.m_pOptionalFeatureSendWorkOrderInfo);
        pResult = &data.m_data;
    }

    class SupervisoryServiceDescriptionDataHolder
    {
    public:
        explicit SupervisoryServiceDescriptionDataHolder(const SupervisoryServiceDescriptionData& data)
        {
            CppToC(data.m_systemId, m_data.m_systemId);
            CppToC(data.m_version, m_data.m_version);
            CppToC(data.m_supportedFeatures, m_supervisoryFeatures);
        }

        const HermesSupervisoryServiceDescriptionData* operator&()
        {
            Adjust(m_supervisoryFeatures, m_data.m_pSupportedFeatures);
            return &m_data;
        }

    private:
        SupervisoryFeaturesHolder m_supervisoryFeatures;
        HermesSupervisoryServiceDescriptionData m_data{};
    };

    // SupervisoryServiceDescriptionData
    inline SupervisoryServiceDescriptionDataHolder ToC(const SupervisoryServiceDescriptionData& data)
    {
        return SupervisoryServiceDescriptionDataHolder(data);
    }
    inline SupervisoryServiceDescriptionData ToCpp(const HermesSupervisoryServiceDescriptionData& data)
    {
        SupervisoryServiceDescriptionData result;
        CToCpp(data.m_systemId, result.m_systemId);
        CToCpp(data.m_version, result.m_version);
        CToCpp(data.m_pSupportedFeatures, result.m_supportedFeatures);
        return result;
    }

    inline HermesBoardArrivedData ToC(const BoardArrivedData& data)
    {
        HermesBoardArrivedData result{};
        CppToC(data.m_machineId, result.m_machineId);
        CppToC(data.m_upstreamLaneId, result.m_upstreamLaneId);
        CppToC(data.m_optionalUpstreamInterfaceId, result.m_optionalUpstreamInterfaceId);
        CppToC(data.m_optionalMagazineId, result.m_optionalMagazineId);
        CppToC(data.m_optionalSlotId, result.m_pOptionalSlotId);
        CppToC(data.m_boardTransfer, result.m_boardTransfer);
        CppToC(data.m_boardId, result.m_boardId);
        CppToC(data.m_boardIdCreatedBy, result.m_boardIdCreatedBy);
        CppToC(data.m_failedBoard, result.m_failedBoard);
        CppToC(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CppToC(data.m_flippedBoard, result.m_flippedBoard);
        CppToC(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CppToC(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CppToC(data.m_optionalLengthInMM, result.m_pOptionalLengthInMM);
        CppToC(data.m_optionalWidthInMM, result.m_pOptionalWidthInMM);
        CppToC(data.m_optionalThicknessInMM, result.m_pOptionalThicknessInMM);
        CppToC(data.m_optionalConveyorSpeedInMMPerSecs, result.m_pOptionalConveyorSpeedInMMPerSecs);
        CppToC(data.m_optionalTopClearanceHeightInMM, result.m_pOptionalTopClearanceHeightInMM);
        CppToC(data.m_optionalBottomClearanceHeightInMM, result.m_pOptionalBottomClearanceHeightInMM);
        CppToC(data.m_optionalWeightInGrams, result.m_pOptionalWeightInGrams);
        CppToC(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CppToC(data.m_optionalBatchId, result.m_optionalBatchId);

        return result;
    }
    inline BoardArrivedData ToCpp(const HermesBoardArrivedData& data)
    {
        BoardArrivedData result;

        CToCpp(data.m_machineId, result.m_machineId);
        CToCpp(data.m_upstreamLaneId, result.m_upstreamLaneId);
        CToCpp(data.m_optionalUpstreamInterfaceId, result.m_optionalUpstreamInterfaceId);
        CToCpp(data.m_optionalMagazineId, result.m_optionalMagazineId);
        CToCpp(data.m_pOptionalSlotId, result.m_optionalSlotId);
        CToCpp(data.m_boardTransfer, result.m_boardTransfer);
        CToCpp(data.m_boardId, result.m_boardId);
        CToCpp(data.m_boardIdCreatedBy, result.m_boardIdCreatedBy);
        CToCpp(data.m_failedBoard, result.m_failedBoard);
        CToCpp(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CToCpp(data.m_flippedBoard, result.m_flippedBoard);
        CToCpp(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CToCpp(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CToCpp(data.m_pOptionalLengthInMM, result.m_optionalLengthInMM);
        CToCpp(data.m_pOptionalWidthInMM, result.m_optionalWidthInMM);
        CToCpp(data.m_pOptionalThicknessInMM, result.m_optionalThicknessInMM);
        CToCpp(data.m_pOptionalConveyorSpeedInMMPerSecs, result.m_optionalConveyorSpeedInMMPerSecs);
        CToCpp(data.m_pOptionalTopClearanceHeightInMM, result.m_optionalTopClearanceHeightInMM);
        CToCpp(data.m_pOptionalBottomClearanceHeightInMM, result.m_optionalBottomClearanceHeightInMM);
        CToCpp(data.m_pOptionalWeightInGrams, result.m_optionalWeightInGrams);
        CToCpp(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CToCpp(data.m_optionalBatchId, result.m_optionalBatchId);
        return result;
    }

    inline HermesBoardDepartedData ToC(const BoardDepartedData& data)
    {
        HermesBoardDepartedData result{};
        CppToC(data.m_machineId, result.m_machineId);
        CppToC(data.m_downstreamLaneId, result.m_downstreamLaneId);
        CppToC(data.m_optionalDownstreamInterfaceId, result.m_optionalDownstreamInterfaceId);
        CppToC(data.m_optionalMagazineId, result.m_optionalMagazineId);
        CppToC(data.m_optionalSlotId, result.m_pOptionalSlotId);
        CppToC(data.m_boardTransfer, result.m_boardTransfer);
        CppToC(data.m_boardId, result.m_boardId);
        CppToC(data.m_boardIdCreatedBy, result.m_boardIdCreatedBy);
        CppToC(data.m_failedBoard, result.m_failedBoard);
        CppToC(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CppToC(data.m_flippedBoard, result.m_flippedBoard);
        CppToC(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CppToC(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CppToC(data.m_optionalLengthInMM, result.m_pOptionalLengthInMM);
        CppToC(data.m_optionalWidthInMM, result.m_pOptionalWidthInMM);
        CppToC(data.m_optionalThicknessInMM, result.m_pOptionalThicknessInMM);
        CppToC(data.m_optionalConveyorSpeedInMMPerSecs, result.m_pOptionalConveyorSpeedInMMPerSecs);
        CppToC(data.m_optionalTopClearanceHeightInMM, result.m_pOptionalTopClearanceHeightInMM);
        CppToC(data.m_optionalBottomClearanceHeightInMM, result.m_pOptionalBottomClearanceHeightInMM);
        CppToC(data.m_optionalWeightInGrams, result.m_pOptionalWeightInGrams);
        CppToC(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CppToC(data.m_optionalBatchId, result.m_optionalBatchId);
        return result;
    }
    inline BoardDepartedData ToCpp(const HermesBoardDepartedData& data)
    {
        BoardDepartedData result;

        CToCpp(data.m_machineId, result.m_machineId);
        CToCpp(data.m_downstreamLaneId, result.m_downstreamLaneId);
        CToCpp(data.m_optionalDownstreamInterfaceId, result.m_optionalDownstreamInterfaceId);
        CToCpp(data.m_optionalMagazineId, result.m_optionalMagazineId);
        CToCpp(data.m_pOptionalSlotId, result.m_optionalSlotId);
        CToCpp(data.m_boardTransfer, result.m_boardTransfer);

        CToCpp(data.m_boardId, result.m_boardId);
        CToCpp(data.m_boardIdCreatedBy, result.m_boardIdCreatedBy);
        CToCpp(data.m_failedBoard, result.m_failedBoard);
        CToCpp(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CToCpp(data.m_flippedBoard, result.m_flippedBoard);
        CToCpp(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CToCpp(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CToCpp(data.m_pOptionalLengthInMM, result.m_optionalLengthInMM);
        CToCpp(data.m_pOptionalWidthInMM, result.m_optionalWidthInMM);
        CToCpp(data.m_pOptionalThicknessInMM, result.m_optionalThicknessInMM);
        CToCpp(data.m_pOptionalConveyorSpeedInMMPerSecs, result.m_optionalConveyorSpeedInMMPerSecs);
        CToCpp(data.m_pOptionalTopClearanceHeightInMM, result.m_optionalTopClearanceHeightInMM);
        CToCpp(data.m_pOptionalBottomClearanceHeightInMM, result.m_optionalBottomClearanceHeightInMM);
        CToCpp(data.m_pOptionalWeightInGrams, result.m_optionalWeightInGrams);
        CToCpp(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CToCpp(data.m_optionalBatchId, result.m_optionalBatchId);
        return result;
    }

    // QueryWorkOrderInfoData
    inline HermesQueryWorkOrderInfoData ToC(const QueryWorkOrderInfoData& data)
    {
        HermesQueryWorkOrderInfoData result{};
        CppToC(data.m_optionalQueryId, result.m_optionalQueryId);
        CppToC(data.m_machineId, result.m_machineId);
        CppToC(data.m_optionalMagazineId, result.m_optionalMagazineId);
        CppToC(data.m_optionalSlotId, result.m_pOptionalSlotId);
        CppToC(data.m_optionalBarcode, result.m_optionalBarcode);
        return result;
    }
    inline QueryWorkOrderInfoData ToCpp(const HermesQueryWorkOrderInfoData& data)
    {
        QueryWorkOrderInfoData result;
        CToCpp(data.m_optionalQueryId, result.m_optionalQueryId);
        CToCpp(data.m_machineId, result.m_machineId);
        CToCpp(data.m_optionalMagazineId, result.m_optionalMagazineId);
        CToCpp(data.m_pOptionalSlotId, result.m_optionalSlotId);
        CToCpp(data.m_optionalBarcode, result.m_optionalBarcode);
        return result;
    }

    // SendWorkOrderInfo
    class SendWorkOrderInfoDataHolder
    {
    public:
        explicit SendWorkOrderInfoDataHolder(const SendWorkOrderInfoData& data)
        {
            CppToC(data.m_optionalQueryId, m_data.m_optionalQueryId);
            CppToC(data.m_optionalWorkOrderId, m_data.m_optionalWorkOrderId);
            CppToC(data.m_optionalBatchId, m_data.m_optionalBatchId);
            CppToC(data.m_optionalBoardId, m_data.m_optionalBoardId);
            CppToC(data.m_optionalBoardIdCreatedBy, m_data.m_optionalBoardIdCreatedBy);
            CppToC(data.m_optionalFailedBoard, m_optionalFailedBoard);
            CppToC(data.m_optionalProductTypeId, m_data.m_optionalProductTypeId);
            CppToC(data.m_optionalFlippedBoard, m_optionalFlippedBoard);
            CppToC(data.m_optionalTopBarcode, m_data.m_optionalTopBarcode);
            CppToC(data.m_optionalBottomBarcode, m_data.m_optionalBottomBarcode);
            CppToC(data.m_optionalLengthInMM, m_data.m_pOptionalLengthInMM);
            CppToC(data.m_optionalWidthInMM, m_data.m_pOptionalWidthInMM);
            CppToC(data.m_optionalThicknessInMM, m_data.m_pOptionalThicknessInMM);
            CppToC(data.m_optionalConveyorSpeedInMMPerSecs, m_data.m_pOptionalConveyorSpeedInMMPerSecs);
            CppToC(data.m_optionalTopClearanceHeightInMM, m_data.m_pOptionalTopClearanceHeightInMM);
            CppToC(data.m_optionalBottomClearanceHeightInMM, m_data.m_pOptionalBottomClearanceHeightInMM);
            CppToC(data.m_optionalWeightInGrams, m_data.m_pOptionalWeightInGrams);
        }


        const HermesSendWorkOrderInfoData* operator&()
        {
            Adjust(m_optionalFailedBoard, m_data.m_pOptionalFailedBoard);
            Adjust(m_optionalFlippedBoard, m_data.m_pOptionalFlippedBoard);
            return &m_data;
        }

    private:
        Optional<EHermesBoardQuality> m_optionalFailedBoard;
        Optional<EHermesFlippedBoard> m_optionalFlippedBoard;
        HermesSendWorkOrderInfoData m_data{};
    };

    inline SendWorkOrderInfoDataHolder ToC(const SendWorkOrderInfoData& data)
    {
        return SendWorkOrderInfoDataHolder{data};
    }
    inline SendWorkOrderInfoData ToCpp(const HermesSendWorkOrderInfoData& data)
    {
        SendWorkOrderInfoData result;
        CToCpp(data.m_optionalQueryId, result.m_optionalQueryId);
        CToCpp(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CToCpp(data.m_optionalBatchId, result.m_optionalBatchId);
        CToCpp(data.m_optionalBoardId, result.m_optionalBoardId);
        CToCpp(data.m_optionalBoardIdCreatedBy, result.m_optionalBoardIdCreatedBy);
        CToCpp(data.m_pOptionalFailedBoard, result.m_optionalFailedBoard);
        CToCpp(data.m_optionalProductTypeId, result.m_optionalProductTypeId);
        CToCpp(data.m_pOptionalFlippedBoard, result.m_optionalFlippedBoard);
        CToCpp(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CToCpp(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        CToCpp(data.m_pOptionalLengthInMM, result.m_optionalLengthInMM);
        CToCpp(data.m_pOptionalWidthInMM, result.m_optionalWidthInMM);
        CToCpp(data.m_pOptionalThicknessInMM, result.m_optionalThicknessInMM);
        CToCpp(data.m_pOptionalConveyorSpeedInMMPerSecs, result.m_optionalConveyorSpeedInMMPerSecs);
        CToCpp(data.m_pOptionalTopClearanceHeightInMM, result.m_optionalTopClearanceHeightInMM);
        CToCpp(data.m_pOptionalBottomClearanceHeightInMM, result.m_optionalBottomClearanceHeightInMM);
        CToCpp(data.m_pOptionalWeightInGrams, result.m_optionalWeightInGrams);
        return result;
    }

    // UpstreamSettings
    inline HermesUpstreamSettings ToC(const UpstreamSettings& data)
    {
        HermesUpstreamSettings result{};
        CppToC(data.m_machineId, result.m_machineId);
        CppToC(data.m_hostAddress, result.m_hostAddress);
        CppToC(data.m_port, result.m_port);
        CppToC(data.m_checkAlivePeriodInSeconds, result.m_checkAlivePeriodInSeconds);
        CppToC(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        CppToC(data.m_checkAliveResponseMode, result.m_checkAliveResponseMode);
        CppToC(data.m_checkState, result.m_checkState);
        return result;
    }
    inline UpstreamSettings ToCpp(const HermesUpstreamSettings& data)
    {
        UpstreamSettings result;
        CToCpp(data.m_machineId, result.m_machineId);
        CToCpp(data.m_hostAddress, result.m_hostAddress);
        CToCpp(data.m_port, result.m_port);
        CToCpp(data.m_checkAlivePeriodInSeconds, result.m_checkAlivePeriodInSeconds);
        CToCpp(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        CToCpp(data.m_checkAliveResponseMode, result.m_checkAliveResponseMode);
        CToCpp(data.m_checkState, result.m_checkState);
        return result;
    }

    // DownstreamSettings
    inline HermesDownstreamSettings ToC(const DownstreamSettings& data)
    {
        HermesDownstreamSettings result{};
        CppToC(data.m_machineId, result.m_machineId);
        CppToC(data.m_optionalClientAddress, result.m_optionalClientAddress);
        CppToC(data.m_port, result.m_port);
        CppToC(data.m_checkAlivePeriodInSeconds, result.m_checkAlivePeriodInSeconds);
        CppToC(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        CppToC(data.m_checkAliveResponseMode, result.m_checkAliveResponseMode);
        CppToC(data.m_checkState, result.m_checkState);
        return result;
    }
    inline DownstreamSettings ToCpp(const HermesDownstreamSettings& data)
    {
        DownstreamSettings result;
        CToCpp(data.m_machineId, result.m_machineId);
        CToCpp(data.m_optionalClientAddress, result.m_optionalClientAddress);
        CToCpp(data.m_port, result.m_port);
        CToCpp(data.m_checkAlivePeriodInSeconds, result.m_checkAlivePeriodInSeconds);
        CToCpp(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        CToCpp(data.m_checkAliveResponseMode, result.m_checkAliveResponseMode);
        CToCpp(data.m_checkState, result.m_checkState);
        return result;
    }

    // ConfigurationServiceSettings
    inline HermesConfigurationServiceSettings ToC(const ConfigurationServiceSettings& data)
    {
        HermesConfigurationServiceSettings result{};
        CppToC(data.m_port, result.m_port);
        CppToC(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        return result;
    }
    inline ConfigurationServiceSettings ToCpp(const HermesConfigurationServiceSettings& data)
    {
        ConfigurationServiceSettings result;
        CToCpp(data.m_port, result.m_port);
        CToCpp(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        return result;
    }

    // VerticServiceSettings
    inline HermesVerticalServiceSettings ToC(const VerticalServiceSettings& data)
    {
        HermesVerticalServiceSettings result{};
        CppToC(data.m_systemId, result.m_systemId);
        CppToC(data.m_port, result.m_port);
        CppToC(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        CppToC(data.m_checkAlivePeriodInSeconds, result.m_checkAlivePeriodInSeconds);
        CppToC(data.m_checkAliveResponseMode, result.m_checkAliveResponseMode);
        return result;
    }
    inline VerticalServiceSettings ToCpp(const HermesVerticalServiceSettings& data)
    {
        VerticalServiceSettings result;
        CToCpp(data.m_systemId, result.m_systemId);
        CToCpp(data.m_port, result.m_port);
        CToCpp(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        CToCpp(data.m_checkAlivePeriodInSeconds, result.m_checkAlivePeriodInSeconds);
        CToCpp(data.m_checkAliveResponseMode, result.m_checkAliveResponseMode);
        return result;
    }

    // VerticClientSettings
    inline HermesVerticalClientSettings ToC(const VerticalClientSettings& data)
    {
        HermesVerticalClientSettings result{};
        CppToC(data.m_systemId, result.m_systemId);
        CppToC(data.m_hostAddress, result.m_hostAddress);
        CppToC(data.m_port, result.m_port);
        CppToC(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        CppToC(data.m_checkAlivePeriodInSeconds, result.m_checkAlivePeriodInSeconds);
        CppToC(data.m_checkAliveResponseMode, result.m_checkAliveResponseMode);
        return result;
    }
    inline VerticalClientSettings ToCpp(const HermesVerticalClientSettings& data)
    {
        VerticalClientSettings result;
        CToCpp(data.m_systemId, result.m_systemId);
        CToCpp(data.m_hostAddress, result.m_hostAddress);
        CToCpp(data.m_port, result.m_port);
        CToCpp(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        CToCpp(data.m_checkAlivePeriodInSeconds, result.m_checkAlivePeriodInSeconds);
        CToCpp(data.m_checkAliveResponseMode, result.m_checkAliveResponseMode);
        return result;
    }

    // HermesError
    inline HermesError ToC(const Error& data)
    { 
        HermesError result{};
        CppToC(data.m_code, result.m_code);
        CppToC(data.m_text, result.m_text);
        return result;
    }
    inline Error ToCpp(const HermesError& data)
    { 
        Error result;
        CToCpp(data.m_code, result.m_code);
        CToCpp(data.m_text, result.m_text);
        return result;
    }


}
