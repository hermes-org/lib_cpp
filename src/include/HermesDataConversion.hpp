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

    template<class CppT, class CT>
    void CppToC(const Optional<CppT>& data, CT& intermediate, const CT*& result)
    {
        if (!data)
            return;

        CppToC(*data, intermediate);
        result = &intermediate;
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

    template<class CppT, class CT, class CVector>
    void CppToC(const std::vector<CppT>& data, VectorHolder<CT>& intermediate, CVector& result)
    {
        auto size = data.size();
        intermediate.m_values.resize(size, CT{});
        intermediate.m_pointers.resize(size);
        for (uint32_t i = 0; i < size; ++i)
        {
            CppToC(data[i], intermediate.m_values[i]);
            intermediate.m_pointers[i] = &intermediate.m_values[i];
        }
        result.m_pData = size == 0 ? nullptr : intermediate.m_pointers.data();
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

    static_assert(size(ESubBoardState()) == cHERMES_SUB_BOARD_STATE_ENUM_SIZE, "enum mismatch");
    inline void CppToC(ESubBoardState data, EHermesSubBoardState& result) { result = static_cast<EHermesSubBoardState>(data); }
    inline void CToCpp(EHermesSubBoardState data, ESubBoardState& result) { result = static_cast<ESubBoardState>(data); }

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

    static_assert(size(EReplyWorkOrderInfoStatus()) == cHERMES_REPLY_WORK_ORDER_INFO_STATUS_ENUM_SIZE, "enum mismatch");
    inline void CppToC(EReplyWorkOrderInfoStatus data, EHermesReplyWorkOrderInfoStatus& result) { result = static_cast<EHermesReplyWorkOrderInfoStatus>(data); }
    inline void CToCpp(EHermesReplyWorkOrderInfoStatus data, EReplyWorkOrderInfoStatus& result) { result = static_cast<EReplyWorkOrderInfoStatus>(data); }

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

    template<class T>
    struct Converter2C;

    template<class T>
    struct ConverterBase
    {
        ConverterBase() = default;
        ConverterBase(const ConverterBase&) = delete;
        ConverterBase(ConverterBase&&) = delete;
        ConverterBase& operator=(const ConverterBase&) = delete;
        ConverterBase& operator=(ConverterBase&&) = delete;

        const T* CPointer() const { return &m_data; }
        T m_data{};
    };

    template<>
    struct Converter2C<SetConfigurationData> : ConverterBase<HermesSetConfigurationData>
    {
        explicit Converter2C(const SetConfigurationData& data)
        {
            CppToC(data.m_machineId, m_data.m_machineId);
            CppToC(data.m_optionalSupervisorySystemPort, m_data.m_pOptionalSupervisorySystemPort);
            CppToC(data.m_upstreamConfigurations, m_upstreamConfigurations, m_data.m_upstreamConfigurations);
            CppToC(data.m_downstreamConfigurations, m_downstreamConfigurations, m_data.m_downstreamConfigurations);
        }

    private:
        VectorHolder<HermesUpstreamConfiguration> m_upstreamConfigurations;
        VectorHolder<HermesDownstreamConfiguration> m_downstreamConfigurations;
    };

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
    template<> struct Converter2C<GetConfigurationData> : ConverterBase<HermesGetConfigurationData>
    {
        explicit Converter2C(const GetConfigurationData&) {}
    };
    inline GetConfigurationData ToCpp(const HermesGetConfigurationData&) { return{}; }

    // CurrentConfigurationData
    template<>
    struct Converter2C<CurrentConfigurationData> : ConverterBase<HermesCurrentConfigurationData>
    {
        explicit Converter2C(const CurrentConfigurationData& data)
        {
            CppToC(data.m_optionalMachineId, m_data.m_optionalMachineId);
            CppToC(data.m_optionalSupervisorySystemPort, m_data.m_pOptionalSupervisorySystemPort);
            CppToC(data.m_upstreamConfigurations, m_upstreamConfigurations, m_data.m_upstreamConfigurations);
            CppToC(data.m_downstreamConfigurations, m_downstreamConfigurations, m_data.m_downstreamConfigurations);
        }

    private:
        VectorHolder<HermesUpstreamConfiguration> m_upstreamConfigurations;
        VectorHolder<HermesDownstreamConfiguration> m_downstreamConfigurations;
    };

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
    template<>
    struct Converter2C<ConnectionInfo> : ConverterBase<HermesConnectionInfo>
    {
        Converter2C(const ConnectionInfo& data)
        {
            CppToC(data.m_address, m_data.m_address);
            CppToC(data.m_port, m_data.m_port);
            CppToC(data.m_hostName, m_data.m_hostName);
        }
    };
    inline ConnectionInfo ToCpp(const HermesConnectionInfo& data)
    {
        ConnectionInfo result;
        CToCpp(data.m_address, result.m_address);
        CToCpp(data.m_port, result.m_port);
        CToCpp(data.m_hostName, result.m_hostName);
        return result;
    }

    // Features
    inline void CppToC(const FeatureBoardForecast&, HermesFeatureBoardForecast&) {}
    inline void CppToC(const FeatureCheckAliveResponse&, HermesFeatureCheckAliveResponse&) {}
    inline void CppToC(const FeatureQueryBoardInfo&, HermesFeatureQueryBoardInfo&) {}
    inline void CppToC(const FeatureSendBoardInfo&, HermesFeatureSendBoardInfo&) {}
    inline void CppToC(const FeatureCommand&, HermesFeatureCommand&) {}

    inline void CToCpp(const HermesFeatureBoardForecast&, FeatureBoardForecast&) {}
    inline void CToCpp(const HermesFeatureCheckAliveResponse&, FeatureCheckAliveResponse&) {}
    inline void CToCpp(const HermesFeatureQueryBoardInfo&, FeatureQueryBoardInfo&) {}
    inline void CToCpp(const HermesFeatureSendBoardInfo&, FeatureSendBoardInfo&) {}
    inline void CToCpp(const HermesFeatureCommand&, FeatureCommand&) {}

    // SupportedFeatures
    struct SupportedFeaturesHolder : ConverterBase<HermesSupportedFeatures>
    {
        HermesFeatureBoardForecast m_optionalFeatureBoardForecast;
        HermesFeatureCheckAliveResponse m_optionalFeatureCheckAliveResponse;
        HermesFeatureQueryBoardInfo m_optionalFeatureQueryBoardInfo;
        HermesFeatureSendBoardInfo m_optionalFeatureSendBoardInfo;
        HermesFeatureCommand m_optionalFeatureCommand;
    };

    inline void CppToC(const SupportedFeatures& data, SupportedFeaturesHolder& result)
    {
        auto& hermesData = result.m_data;
        CppToC(data.m_optionalFeatureBoardForecast, result.m_optionalFeatureBoardForecast, hermesData.m_pOptionalFeatureBoardForecast);
        CppToC(data.m_optionalFeatureCheckAliveResponse, result.m_optionalFeatureCheckAliveResponse, hermesData.m_pOptionalFeatureCheckAliveResponse);
        CppToC(data.m_optionalFeatureQueryBoardInfo, result.m_optionalFeatureQueryBoardInfo, hermesData.m_pOptionalFeatureQueryBoardInfo);
        CppToC(data.m_optionalFeatureSendBoardInfo, result.m_optionalFeatureSendBoardInfo, hermesData.m_pOptionalFeatureSendBoardInfo);
        CppToC(data.m_optionalFeatureCommand, result.m_optionalFeatureCommand, hermesData.m_pOptionalFeatureCommand);
    }

    inline void CToCpp(const HermesSupportedFeatures& data, SupportedFeatures& result)
    {
        CToCpp(data.m_pOptionalFeatureBoardForecast, result.m_optionalFeatureBoardForecast);
        CToCpp(data.m_pOptionalFeatureCheckAliveResponse, result.m_optionalFeatureCheckAliveResponse);
        CToCpp(data.m_pOptionalFeatureQueryBoardInfo, result.m_optionalFeatureQueryBoardInfo);
        CToCpp(data.m_pOptionalFeatureSendBoardInfo, result.m_optionalFeatureSendBoardInfo);
        CToCpp(data.m_pOptionalFeatureCommand, result.m_optionalFeatureCommand);
    }

    // ServiceDescriptionData
    template<>
    struct Converter2C<ServiceDescriptionData> : ConverterBase<HermesServiceDescriptionData>
    {
        explicit Converter2C(const ServiceDescriptionData& data)
        {
            CppToC(data.m_machineId, m_data.m_machineId);
            CppToC(data.m_laneId, m_data.m_laneId);
            CppToC(data.m_optionalInterfaceId, m_data.m_optionalInterfaceId);
            CppToC(data.m_version, m_data.m_version);
            CppToC(data.m_supportedFeatures, m_supportedFeatures);
            m_data.m_pSupportedFeatures = &m_supportedFeatures.m_data;
        }

    private:
        SupportedFeaturesHolder m_supportedFeatures;
    };
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
    template<>
    struct Converter2C<MachineReadyData> : ConverterBase<HermesMachineReadyData>
    {
        explicit Converter2C(const MachineReadyData& data)
        {
            CppToC(data.m_optionalFlippedBoard, m_flippedBoard, m_data.m_pOptionalFlippedBoard);
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

    private:
        EHermesFlippedBoard m_flippedBoard;
    };

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
    template<> struct Converter2C<RevokeMachineReadyData> : ConverterBase<HermesRevokeMachineReadyData>
    {
        explicit Converter2C(const RevokeMachineReadyData&) {}
    };
    inline RevokeMachineReadyData ToCpp(const HermesRevokeMachineReadyData&) { return{}; }

    // SubBoards
    inline void CppToC(const SubBoard& data, HermesSubBoard& result)
    {
        CppToC(data.m_pos, result.m_pos);
        CppToC(data.m_optionalBc, result.m_optionalBc);
        CppToC(data.m_st, result.m_st);
    }

    inline void CToCpp(const HermesSubBoard& data, SubBoard& result)
    {
        CToCpp(data.m_pos, result.m_pos);
        CToCpp(data.m_optionalBc, result.m_optionalBc);
        CToCpp(data.m_st, result.m_st);
    }

    // BoardAvailableData
    template<>
    struct Converter2C<BoardAvailableData> : ConverterBase<HermesBoardAvailableData>
    {
        Converter2C(const BoardAvailableData& data)
        {
            CppToC(data.m_boardId, m_data.m_boardId);
            CppToC(data.m_boardIdCreatedBy, m_data.m_boardIdCreatedBy);
            CppToC(data.m_failedBoard, m_data.m_failedBoard);
            CppToC(data.m_optionalProductTypeId, m_data.m_optionalProductTypeId);
            CppToC(data.m_flippedBoard, m_data.m_flippedBoard);
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
            CppToC(data.m_optionalRoute, m_data.m_pOptionalRoute);
            CppToC(data.m_optionalAction, m_data.m_pOptionalAction);
            CppToC(data.m_optionalSubBoards, m_subBoards, m_data.m_optionalSubBoards);
        }

    private:
        VectorHolder<HermesSubBoard> m_subBoards;
    };
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
        CToCpp(data.m_pOptionalRoute, result.m_optionalRoute);
        CToCpp(data.m_pOptionalAction, result.m_optionalAction);
        CToCpp(data.m_optionalSubBoards, result.m_optionalSubBoards);
        return result;
    }

    // RevokeBoardAvailableData
    template<> struct Converter2C<RevokeBoardAvailableData> : ConverterBase<HermesRevokeBoardAvailableData>
    {
        explicit Converter2C(const RevokeBoardAvailableData&) {}
    };
    inline RevokeBoardAvailableData ToCpp(const HermesRevokeBoardAvailableData&) { return{}; }
    
    // StartTransportData
    template<>
    struct Converter2C<StartTransportData> : ConverterBase<HermesStartTransportData>
    {
        Converter2C(const StartTransportData& data)
        {
            CppToC(data.m_boardId, m_data.m_boardId);
            CppToC(data.m_optionalConveyorSpeedInMMPerSecs, m_data.m_pOptionalConveyorSpeedInMMPerSecs);
        }
    };
    inline StartTransportData ToCpp(const HermesStartTransportData& data)
    {
        StartTransportData result;
        CToCpp(data.m_boardId, result.m_boardId);
        CToCpp(data.m_pOptionalConveyorSpeedInMMPerSecs, result.m_optionalConveyorSpeedInMMPerSecs);
        return result;
    }

    // StopTransportData
    template<>
    struct Converter2C<StopTransportData> : ConverterBase<HermesStopTransportData>
    {
        Converter2C(const StopTransportData& data)
        {
            CppToC(data.m_transferState, m_data.m_transferState);
            CppToC(data.m_boardId, m_data.m_boardId);
        }
    };
    inline StopTransportData ToCpp(const HermesStopTransportData& data)
    {
        StopTransportData result;
        CToCpp(data.m_transferState, result.m_transferState);
        CToCpp(data.m_boardId, result.m_boardId);
        return result;
    }

    // TransportFinishedData
    template<>
    struct Converter2C<TransportFinishedData> : ConverterBase<HermesTransportFinishedData>
    {
        Converter2C(const TransportFinishedData& data)
        {
            CppToC(data.m_transferState, m_data.m_transferState);
            CppToC(data.m_boardId, m_data.m_boardId);
        }
    };
    inline TransportFinishedData ToCpp(const HermesTransportFinishedData& data)
    {
        TransportFinishedData result;
        CToCpp(data.m_transferState, result.m_transferState);
        CToCpp(data.m_boardId, result.m_boardId);
        return result;
    }

    // NotificationData
    template<>
    struct Converter2C<NotificationData> : ConverterBase<HermesNotificationData>
    {
        Converter2C(const NotificationData& data)
        {
            CppToC(data.m_notificationCode, m_data.m_notificationCode);
            CppToC(data.m_severity, m_data.m_severity);
            CppToC(data.m_description, m_data.m_description);
        }
    };
    inline NotificationData ToCpp(const HermesNotificationData& data)
    {
        NotificationData result;
        CToCpp(data.m_notificationCode, result.m_notificationCode);
        CToCpp(data.m_severity, result.m_severity);
        CToCpp(data.m_description, result.m_description);
        return result;
    }

    // CheckAliveData
    template<>
    struct Converter2C<CheckAliveData> : ConverterBase<HermesCheckAliveData>
    {
        Converter2C(const CheckAliveData& data)
        {
            CppToC(data.m_optionalType, m_optionalType, m_data.m_pOptionalType);
            CppToC(data.m_optionalId, m_data.m_optionalId);
        }

    private:
        EHermesCheckAliveType m_optionalType;
    };
    inline CheckAliveData ToCpp(const HermesCheckAliveData& data)
    {
        CheckAliveData result;
        CToCpp(data.m_pOptionalType, result.m_optionalType);
        CToCpp(data.m_optionalId, result.m_optionalId);
        return result;
    }

    // BoardForecastData
    template<>
    struct Converter2C<BoardForecastData> : ConverterBase<HermesBoardForecastData>
    {
        Converter2C(const BoardForecastData& data)
        {
            CppToC(data.m_optionalForecastId, m_data.m_optionalForecastId);
            CppToC(data.m_optionalTimeUntilAvailableInSeconds, m_data.m_pOptionalTimeUntilAvailableInSeconds);
            CppToC(data.m_optionalBoardId, m_data.m_optionalBoardId);
            CppToC(data.m_optionalBoardIdCreatedBy, m_data.m_optionalBoardIdCreatedBy);
            CppToC(data.m_failedBoard, m_data.m_failedBoard);
            CppToC(data.m_optionalProductTypeId, m_data.m_optionalProductTypeId);
            CppToC(data.m_flippedBoard, m_data.m_flippedBoard);
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
    };
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
    template<>
    struct Converter2C<QueryBoardInfoData> : ConverterBase<HermesQueryBoardInfoData>
    {
        Converter2C(const QueryBoardInfoData& data)
        {
            CppToC(data.m_optionalTopBarcode, m_data.m_optionalTopBarcode);
            CppToC(data.m_optionalBottomBarcode, m_data.m_optionalBottomBarcode);
        }
    };
    inline QueryBoardInfoData ToCpp(const HermesQueryBoardInfoData& data)
    {
        QueryBoardInfoData result;
        CToCpp(data.m_optionalTopBarcode, result.m_optionalTopBarcode);
        CToCpp(data.m_optionalBottomBarcode, result.m_optionalBottomBarcode);
        return result;
    }

    // SendBoardInfoData
    template<>
    struct Converter2C<SendBoardInfoData> : ConverterBase<HermesSendBoardInfoData>
    {
        Converter2C(const SendBoardInfoData& data)
        {
            CppToC(data.m_optionalFailedBoard, m_optionalFailedBoard, m_data.m_pOptionalFailedBoard);
            CppToC(data.m_optionalFlippedBoard, m_optionalFlippedBoard, m_data.m_pOptionalFlippedBoard);

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
            CppToC(data.m_optionalRoute, m_data.m_pOptionalRoute);
            CppToC(data.m_optionalAction, m_data.m_pOptionalAction);
            CppToC(data.m_optionalSubBoards, m_subBoards, m_data.m_optionalSubBoards);
        }

    private:
        EHermesBoardQuality m_optionalFailedBoard;
        EHermesFlippedBoard m_optionalFlippedBoard;
        VectorHolder<HermesSubBoard> m_subBoards;
    };

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
        CToCpp(data.m_pOptionalRoute, result.m_optionalRoute);
        CToCpp(data.m_pOptionalAction, result.m_optionalAction);
        CToCpp(data.m_optionalSubBoards, result.m_optionalSubBoards);
        return result;
    }

    // SupervisoryFeatures
    inline void CppToC(const FeatureConfiguration&, HermesFeatureConfiguration&) {}
    inline void CppToC(const FeatureBoardTracking&, HermesFeatureBoardTracking&) {}
    inline void CppToC(const FeatureQueryWorkOrderInfo&, HermesFeatureQueryWorkOrderInfo&) {}
    inline void CppToC(const FeatureSendWorkOrderInfo&, HermesFeatureSendWorkOrderInfo&) {}
    inline void CppToC(const FeatureReplyWorkOrderInfo&, HermesFeatureReplyWorkOrderInfo&) {}
    inline void CppToC(const FeatureQueryHermesCapabilities&, HermesFeatureQueryHermesCapabilities&) {}
    inline void CppToC(const FeatureSendHermesCapabilities&, HermesFeatureSendHermesCapabilities&) {}

    inline void CToCpp(const HermesFeatureConfiguration&, FeatureConfiguration&) {}
    inline void CToCpp(const HermesFeatureBoardTracking&, FeatureBoardTracking&) {}
    inline void CToCpp(const HermesFeatureQueryWorkOrderInfo&, FeatureQueryWorkOrderInfo&) {}
    inline void CToCpp(const HermesFeatureSendWorkOrderInfo&, FeatureSendWorkOrderInfo&) {}
    inline void CToCpp(const HermesFeatureReplyWorkOrderInfo&, FeatureReplyWorkOrderInfo&) {}
    inline void CToCpp(const HermesFeatureQueryHermesCapabilities&, FeatureQueryHermesCapabilities&) {}
    inline void CToCpp(const HermesFeatureSendHermesCapabilities&, FeatureSendHermesCapabilities&) {}

    // SupervisoryFeatures
    struct SupervisoryFeaturesHolder : ConverterBase<HermesSupervisoryFeatures>
    {
        HermesFeatureConfiguration m_optionalFeatureConfiguration;
        HermesFeatureCheckAliveResponse m_optionalFeatureCheckAliveResponse;
        HermesFeatureBoardTracking m_optionalFeatureBoardTracking;
        HermesFeatureQueryWorkOrderInfo m_optionalFeatureQueryWorkOrderInfo;
        HermesFeatureSendWorkOrderInfo m_optionalFeatureSendWorkOrderInfo;
        HermesFeatureReplyWorkOrderInfo m_optionalFeatureReplyWorkOrderInfo;
        HermesFeatureQueryHermesCapabilities m_optionalFeatureQueryHermesCapabilities;
        HermesFeatureSendHermesCapabilities m_optionalFeatureSendHermesCapabilities;
    };

    inline void CppToC(const SupervisoryFeatures& data, SupervisoryFeaturesHolder& intermediate)
    {
        auto& hermesData = intermediate.m_data;
        CppToC(data.m_optionalFeatureConfiguration, intermediate.m_optionalFeatureConfiguration, hermesData.m_pOptionalFeatureConfiguration);
        CppToC(data.m_optionalFeatureCheckAliveResponse, intermediate.m_optionalFeatureCheckAliveResponse, hermesData.m_pOptionalFeatureCheckAliveResponse);
        CppToC(data.m_optionalFeatureBoardTracking, intermediate.m_optionalFeatureBoardTracking, hermesData.m_pOptionalFeatureBoardTracking);
        CppToC(data.m_optionalFeatureQueryWorkOrderInfo, intermediate.m_optionalFeatureQueryWorkOrderInfo, hermesData.m_pOptionalFeatureQueryWorkOrderInfo);
        CppToC(data.m_optionalFeatureSendWorkOrderInfo, intermediate.m_optionalFeatureSendWorkOrderInfo, hermesData.m_pOptionalFeatureSendWorkOrderInfo);
        CppToC(data.m_optionalFeatureReplyWorkOrderInfo, intermediate.m_optionalFeatureReplyWorkOrderInfo, hermesData.m_pOptionalFeatureReplyWorkOrderInfo);
        CppToC(data.m_optionalFeatureQueryHermesCapabilities, intermediate.m_optionalFeatureQueryHermesCapabilities, hermesData.m_pOptionalFeatureQueryHermesCapabilities);
        CppToC(data.m_optionalFeatureSendHermesCapabilities, intermediate.m_optionalFeatureSendHermesCapabilities, hermesData.m_pOptionalFeatureSendHermesCapabilities);
    }

    inline void CToCpp(const HermesSupervisoryFeatures& data, SupervisoryFeatures& result)
    {
        CToCpp(data.m_pOptionalFeatureConfiguration, result.m_optionalFeatureConfiguration);
        CToCpp(data.m_pOptionalFeatureCheckAliveResponse, result.m_optionalFeatureCheckAliveResponse);
        CToCpp(data.m_pOptionalFeatureBoardTracking, result.m_optionalFeatureBoardTracking);
        CToCpp(data.m_pOptionalFeatureQueryWorkOrderInfo, result.m_optionalFeatureQueryWorkOrderInfo);
        CToCpp(data.m_pOptionalFeatureSendWorkOrderInfo, result.m_optionalFeatureSendWorkOrderInfo);
        CToCpp(data.m_pOptionalFeatureReplyWorkOrderInfo, result.m_optionalFeatureReplyWorkOrderInfo);
        CToCpp(data.m_pOptionalFeatureQueryHermesCapabilities, result.m_optionalFeatureQueryHermesCapabilities);
        CToCpp(data.m_pOptionalFeatureSendHermesCapabilities, result.m_optionalFeatureSendHermesCapabilities);
    }

    // SupervisoryServiceDescriptionData
    template<>
    struct Converter2C<SupervisoryServiceDescriptionData> : ConverterBase<HermesSupervisoryServiceDescriptionData>
    {
        Converter2C(const SupervisoryServiceDescriptionData& data)
        {
            CppToC(data.m_systemId, m_data.m_systemId);
            CppToC(data.m_version, m_data.m_version);
            CppToC(data.m_supportedFeatures, m_supervisoryFeatures);
            m_data.m_pSupportedFeatures = &m_supervisoryFeatures.m_data;
        }
    private:
        SupervisoryFeaturesHolder m_supervisoryFeatures;
    };

    // SupervisoryServiceDescriptionData
    inline SupervisoryServiceDescriptionData ToCpp(const HermesSupervisoryServiceDescriptionData& data)
    {
        SupervisoryServiceDescriptionData result;
        CToCpp(data.m_systemId, result.m_systemId);
        CToCpp(data.m_version, result.m_version);
        CToCpp(data.m_pSupportedFeatures, result.m_supportedFeatures);
        return result;
    }

    // BoardArrivedData
    template<>
    struct Converter2C<BoardArrivedData> : ConverterBase<HermesBoardArrivedData>
    {
        Converter2C(const BoardArrivedData& data)
        {
            CppToC(data.m_machineId, m_data.m_machineId);
            CppToC(data.m_upstreamLaneId, m_data.m_upstreamLaneId);
            CppToC(data.m_optionalUpstreamInterfaceId, m_data.m_optionalUpstreamInterfaceId);
            CppToC(data.m_optionalMagazineId, m_data.m_optionalMagazineId);
            CppToC(data.m_optionalSlotId, m_data.m_pOptionalSlotId);
            CppToC(data.m_boardTransfer, m_data.m_boardTransfer);
            CppToC(data.m_boardId, m_data.m_boardId);
            CppToC(data.m_boardIdCreatedBy, m_data.m_boardIdCreatedBy);
            CppToC(data.m_failedBoard, m_data.m_failedBoard);
            CppToC(data.m_optionalProductTypeId, m_data.m_optionalProductTypeId);
            CppToC(data.m_flippedBoard, m_data.m_flippedBoard);
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
            CppToC(data.m_optionalRoute, m_data.m_pOptionalRoute);
            CppToC(data.m_optionalAction, m_data.m_pOptionalAction);
            CppToC(data.m_optionalSubBoards, m_subBoards, m_data.m_optionalSubBoards);
        }
    private:
        VectorHolder<HermesSubBoard> m_subBoards;
    };
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
        CToCpp(data.m_pOptionalRoute, result.m_optionalRoute);
        CToCpp(data.m_pOptionalAction, result.m_optionalAction);
        CToCpp(data.m_optionalSubBoards, result.m_optionalSubBoards);
        return result;
    }

    // BoardDepartedData
    template<>
    struct Converter2C<BoardDepartedData> : ConverterBase<HermesBoardDepartedData>
    {
        Converter2C(const BoardDepartedData& data)
        {
            CppToC(data.m_machineId, m_data.m_machineId);
            CppToC(data.m_downstreamLaneId, m_data.m_downstreamLaneId);
            CppToC(data.m_optionalDownstreamInterfaceId, m_data.m_optionalDownstreamInterfaceId);
            CppToC(data.m_optionalMagazineId, m_data.m_optionalMagazineId);
            CppToC(data.m_optionalSlotId, m_data.m_pOptionalSlotId);
            CppToC(data.m_boardTransfer, m_data.m_boardTransfer);
            CppToC(data.m_boardId, m_data.m_boardId);
            CppToC(data.m_boardIdCreatedBy, m_data.m_boardIdCreatedBy);
            CppToC(data.m_failedBoard, m_data.m_failedBoard);
            CppToC(data.m_optionalProductTypeId, m_data.m_optionalProductTypeId);
            CppToC(data.m_flippedBoard, m_data.m_flippedBoard);
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
            CppToC(data.m_optionalRoute, m_data.m_pOptionalRoute);
            CppToC(data.m_optionalAction, m_data.m_pOptionalAction);
            CppToC(data.m_optionalSubBoards, m_subBoards, m_data.m_optionalSubBoards);
        }
    private:
        VectorHolder<HermesSubBoard> m_subBoards;
    };

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
        CToCpp(data.m_pOptionalRoute, result.m_optionalRoute);
        CToCpp(data.m_pOptionalAction, result.m_optionalAction);
        CToCpp(data.m_optionalSubBoards, result.m_optionalSubBoards);
        return result;
    }

    // QueryWorkOrderInfoData
    template<>
    struct Converter2C<QueryWorkOrderInfoData> : ConverterBase<HermesQueryWorkOrderInfoData>
    {
        Converter2C(const QueryWorkOrderInfoData& data)
        {
            CppToC(data.m_optionalQueryId, m_data.m_optionalQueryId);
            CppToC(data.m_machineId, m_data.m_machineId);
            CppToC(data.m_optionalMagazineId, m_data.m_optionalMagazineId);
            CppToC(data.m_optionalSlotId, m_data.m_pOptionalSlotId);
            CppToC(data.m_optionalBarcode, m_data.m_optionalBarcode);
            CppToC(data.m_optionalWorkOrderId, m_data.m_optionalWorkOrderId);
            CppToC(data.m_optionalBatchId, m_data.m_optionalBatchId);
        }
    };
    inline QueryWorkOrderInfoData ToCpp(const HermesQueryWorkOrderInfoData& data)
    {
        QueryWorkOrderInfoData result;
        CToCpp(data.m_optionalQueryId, result.m_optionalQueryId);
        CToCpp(data.m_machineId, result.m_machineId);
        CToCpp(data.m_optionalMagazineId, result.m_optionalMagazineId);
        CToCpp(data.m_pOptionalSlotId, result.m_optionalSlotId);
        CToCpp(data.m_optionalBarcode, result.m_optionalBarcode);
        CToCpp(data.m_optionalWorkOrderId, result.m_optionalWorkOrderId);
        CToCpp(data.m_optionalBatchId, result.m_optionalBatchId);
        return result;
    }

    // SendWorkOrderInfoData
    template<>
    struct Converter2C<SendWorkOrderInfoData> : ConverterBase<HermesSendWorkOrderInfoData>
    {
        Converter2C(const SendWorkOrderInfoData& data)
        {
            CppToC(data.m_optionalQueryId, m_data.m_optionalQueryId);
            CppToC(data.m_optionalWorkOrderId, m_data.m_optionalWorkOrderId);
            CppToC(data.m_optionalBatchId, m_data.m_optionalBatchId);
            CppToC(data.m_optionalBoardId, m_data.m_optionalBoardId);
            CppToC(data.m_optionalBoardIdCreatedBy, m_data.m_optionalBoardIdCreatedBy);
            CppToC(data.m_optionalFailedBoard, m_optionalFailedBoard, m_data.m_pOptionalFailedBoard);
            CppToC(data.m_optionalProductTypeId, m_data.m_optionalProductTypeId);
            CppToC(data.m_optionalFlippedBoard, m_optionalFlippedBoard, m_data.m_pOptionalFlippedBoard);
            CppToC(data.m_optionalTopBarcode, m_data.m_optionalTopBarcode);
            CppToC(data.m_optionalBottomBarcode, m_data.m_optionalBottomBarcode);
            CppToC(data.m_optionalLengthInMM, m_data.m_pOptionalLengthInMM);
            CppToC(data.m_optionalWidthInMM, m_data.m_pOptionalWidthInMM);
            CppToC(data.m_optionalThicknessInMM, m_data.m_pOptionalThicknessInMM);
            CppToC(data.m_optionalConveyorSpeedInMMPerSecs, m_data.m_pOptionalConveyorSpeedInMMPerSecs);
            CppToC(data.m_optionalTopClearanceHeightInMM, m_data.m_pOptionalTopClearanceHeightInMM);
            CppToC(data.m_optionalBottomClearanceHeightInMM, m_data.m_pOptionalBottomClearanceHeightInMM);
            CppToC(data.m_optionalWeightInGrams, m_data.m_pOptionalWeightInGrams);
            CppToC(data.m_optionalRoute, m_data.m_pOptionalRoute);
            CppToC(data.m_optionalSubBoards, m_subBoards, m_data.m_optionalSubBoards);
        }
    private:
        VectorHolder<HermesSubBoard> m_subBoards;
        EHermesBoardQuality m_optionalFailedBoard;
        EHermesFlippedBoard m_optionalFlippedBoard;
    };
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
        CToCpp(data.m_pOptionalRoute, result.m_optionalRoute);
        CToCpp(data.m_optionalSubBoards, result.m_optionalSubBoards);
        return result;
    }

    // ReplyWorkOrderInfoData
    template<>
    struct Converter2C<ReplyWorkOrderInfoData> : ConverterBase<HermesReplyWorkOrderInfoData>
    {
        Converter2C(const ReplyWorkOrderInfoData& data)
        {
            CppToC(data.m_workOrderId, m_data.m_workOrderId);
            CppToC(data.m_optionalBatchId, m_data.m_optionalBatchId);
            CppToC(data.m_status, m_data.m_status);
        }
    };
    inline ReplyWorkOrderInfoData ToCpp(const HermesReplyWorkOrderInfoData& data)
    {
        ReplyWorkOrderInfoData result;
        CToCpp(data.m_workOrderId, result.m_workOrderId);
        CToCpp(data.m_optionalBatchId, result.m_optionalBatchId);
        CToCpp(data.m_status, result.m_status);
        return result;
    }

    // CommandData
    template<>
    struct Converter2C<CommandData> : ConverterBase<HermesCommandData>
    {
        Converter2C(const CommandData& data)
        {
            CppToC(data.m_command, m_data.m_command);
        }
    };
    inline CommandData ToCpp(const HermesCommandData& data)
    {
        CommandData result;
        CToCpp(data.m_command, result.m_command);
        return result;
    }

    
    // QueryHermesCapabilitiesData
    template<>
    struct Converter2C<QueryHermesCapabilitiesData> : ConverterBase<HermesQueryHermesCapabilitiesData>
    {
        Converter2C(const QueryHermesCapabilitiesData&)
        {
        }
    };
    inline QueryHermesCapabilitiesData ToCpp(const HermesQueryHermesCapabilitiesData&)
    {
        QueryHermesCapabilitiesData result;
        return result;
    }
    
    // OptionalMessages
    inline void CppToC(const MessageCheckAliveResponse&, HermesMessageCheckAliveResponse&) {}
    inline void CppToC(const MessageBoardForecast&, HermesMessageBoardForecast&) {}
    inline void CppToC(const MessageQueryBoardInfo&, HermesMessageQueryBoardInfo&) {}
    inline void CppToC(const MessageSendBoardInfo&, HermesMessageSendBoardInfo&) {}
    inline void CppToC(const MessageBoardArrived&, HermesMessageBoardArrived&) {}
    inline void CppToC(const MessageBoardDeparted&, HermesMessageBoardDeparted&) {}
    inline void CppToC(const MessageQueryWorkOrderInfo&, HermesMessageQueryWorkOrderInfo&) {}
    inline void CppToC(const MessageReplyWorkOrderInfo&, HermesMessageReplyWorkOrderInfo&) {}
    inline void CppToC(const MessageCommand&, HermesMessageCommand&) {}

    inline void CToCpp(const HermesMessageCheckAliveResponse&, MessageCheckAliveResponse&) {}
    inline void CToCpp(const HermesMessageBoardForecast&, MessageBoardForecast&) {}
    inline void CToCpp(const HermesMessageQueryBoardInfo&, MessageQueryBoardInfo&) {}
    inline void CToCpp(const HermesMessageSendBoardInfo&, MessageSendBoardInfo&) {}
    inline void CToCpp(const HermesMessageBoardArrived&, MessageBoardArrived&) {}
    inline void CToCpp(const HermesMessageBoardDeparted&, MessageBoardDeparted&) {}
    inline void CToCpp(const HermesMessageQueryWorkOrderInfo&, MessageQueryWorkOrderInfo&) {}
    inline void CToCpp(const HermesMessageReplyWorkOrderInfo&, MessageReplyWorkOrderInfo&) {}
    inline void CToCpp(const HermesMessageCommand&, MessageCommand&) {}
   
    // OptionalMessages
    struct OptionalMessagesHolder : ConverterBase<HermesOptionalMessages>
    {
        HermesMessageCheckAliveResponse m_optionalMessageCheckAliveResponse;
        HermesMessageBoardForecast m_optionalMessageBoardForecast;
        HermesMessageQueryBoardInfo m_optionalMessageQueryBoardInfo;
        HermesMessageSendBoardInfo m_optionalMessageSendBoardInfo;
        HermesMessageBoardArrived m_optionalMessageBoardArrived;
        HermesMessageBoardDeparted m_optionalMessageBoardDeparted;
        HermesMessageQueryWorkOrderInfo m_optionalMessageQueryWorkOrderInfo;
        HermesMessageReplyWorkOrderInfo m_optionalMessageReplyWorkOrderInfo;
        HermesMessageCommand m_optionalMessageCommand;
    };
    
    inline void CppToC(const OptionalMessages& data, OptionalMessagesHolder& result)
    {
        auto& hermesData = result.m_data;
        CppToC(data.m_optionalMessageCheckAliveResponse, result.m_optionalMessageCheckAliveResponse, hermesData.m_pOptionalMessageCheckAliveResponse);
        CppToC(data.m_optionalMessageBoardForecast, result.m_optionalMessageBoardForecast, hermesData.m_pOptionalMessageBoardForecast);
        CppToC(data.m_optionalMessageQueryBoardInfo, result.m_optionalMessageQueryBoardInfo, hermesData.m_pOptionalMessageQueryBoardInfo);
        CppToC(data.m_optionalMessageSendBoardInfo, result.m_optionalMessageSendBoardInfo, hermesData.m_pOptionalMessageSendBoardInfo);
        CppToC(data.m_optionalMessageBoardArrived, result.m_optionalMessageBoardArrived, hermesData.m_pOptionalMessageBoardArrived);
        CppToC(data.m_optionalMessageBoardDeparted, result.m_optionalMessageBoardDeparted, hermesData.m_pOptionalMessageBoardDeparted);
        CppToC(data.m_optionalMessageQueryWorkOrderInfo, result.m_optionalMessageQueryWorkOrderInfo, hermesData.m_pOptionalMessageQueryWorkOrderInfo);
        CppToC(data.m_optionalMessageReplyWorkOrderInfo, result.m_optionalMessageReplyWorkOrderInfo, hermesData.m_pOptionalMessageReplyWorkOrderInfo);
        CppToC(data.m_optionalMessageCommand, result.m_optionalMessageCommand, hermesData.m_pOptionalMessageCommand);
    }
 
    inline void CToCpp(const HermesOptionalMessages& data, OptionalMessages& result)
    {
        CToCpp(data.m_pOptionalMessageCheckAliveResponse, result.m_optionalMessageCheckAliveResponse);
        CToCpp(data.m_pOptionalMessageBoardForecast, result.m_optionalMessageBoardForecast);
        CToCpp(data.m_pOptionalMessageQueryBoardInfo, result.m_optionalMessageQueryBoardInfo);
        CToCpp(data.m_pOptionalMessageSendBoardInfo, result.m_optionalMessageSendBoardInfo);
        CToCpp(data.m_pOptionalMessageBoardArrived, result.m_optionalMessageBoardArrived);
        CToCpp(data.m_pOptionalMessageBoardDeparted, result.m_optionalMessageBoardDeparted);
        CToCpp(data.m_pOptionalMessageQueryWorkOrderInfo, result.m_optionalMessageQueryWorkOrderInfo);
        CToCpp(data.m_pOptionalMessageReplyWorkOrderInfo, result.m_optionalMessageReplyWorkOrderInfo);
        CToCpp(data.m_pOptionalMessageCommand, result.m_optionalMessageCommand);
    }
    
    // Attributes
    inline void CppToC(const Attributes& data, HermesAttributes& result)
    {
        CppToC(data.m_productTypeId, result.m_productTypeId);
        CppToC(data.m_topBarcode, result.m_topBarcode);
        CppToC(data.m_bottomBarcode, result.m_bottomBarcode);
        CppToC(data.m_length, result.m_length);
        CppToC(data.m_width, result.m_width);
        CppToC(data.m_thickness, result.m_thickness);
        CppToC(data.m_conveyorSpeed, result.m_conveyorSpeed);
        CppToC(data.m_topClearanceHeight, result.m_topClearanceHeight);
        CppToC(data.m_bottomClearanceHeight, result.m_bottomClearanceHeight);
        CppToC(data.m_weight, result.m_weight);
        CppToC(data.m_workOrderId, result.m_workOrderId);
        CppToC(data.m_batchId, result.m_batchId);
        CppToC(data.m_route, result.m_route);
        CppToC(data.m_action, result.m_action);
        CppToC(data.m_subBoards, result.m_subBoards);
    }
    
    inline void CToCpp(const HermesAttributes& data, Attributes& result)
    {
        CToCpp(data.m_productTypeId, result.m_productTypeId);
        CToCpp(data.m_topBarcode, result.m_topBarcode);
        CToCpp(data.m_bottomBarcode, result.m_bottomBarcode);
        CToCpp(data.m_length, result.m_length);
        CToCpp(data.m_width, result.m_width);
        CToCpp(data.m_thickness, result.m_thickness);
        CToCpp(data.m_conveyorSpeed, result.m_conveyorSpeed);
        CToCpp(data.m_topClearanceHeight, result.m_topClearanceHeight);
        CToCpp(data.m_bottomClearanceHeight, result.m_bottomClearanceHeight);
        CToCpp(data.m_weight, result.m_weight);
        CToCpp(data.m_workOrderId, result.m_workOrderId);
        CToCpp(data.m_batchId, result.m_batchId);
        CToCpp(data.m_route, result.m_route);
        CToCpp(data.m_action, result.m_action);
        CToCpp(data.m_subBoards, result.m_subBoards);
    }

    // SendHermesCapabilitiesData
    template<>
    struct Converter2C<SendHermesCapabilitiesData> : ConverterBase<HermesSendHermesCapabilitiesData>
    {
        Converter2C(const SendHermesCapabilitiesData& data)
        {
            CppToC(data.m_optionalMessages, m_optionalMessages);
            m_data.m_pOptionalMessages = &m_optionalMessages.m_data;
            CppToC(data.m_attributes, m_hermesAttributes);
            m_data.m_pAttributes = &m_hermesAttributes;
        }
    private:
        OptionalMessagesHolder m_optionalMessages;
        HermesAttributes m_hermesAttributes;
    };
   
    inline SendHermesCapabilitiesData ToCpp(const HermesSendHermesCapabilitiesData& data)
    {
        SendHermesCapabilitiesData result;
        CToCpp(data.m_pOptionalMessages, result.m_optionalMessages);
        CToCpp(data.m_pAttributes, result.m_attributes);
        return result;
    }
    

    // UpstreamSettings
    template<>
    struct Converter2C<UpstreamSettings> : ConverterBase<HermesUpstreamSettings>
    {
        Converter2C(const UpstreamSettings& data)
        {
            CppToC(data.m_machineId, m_data.m_machineId);
            CppToC(data.m_hostAddress, m_data.m_hostAddress);
            CppToC(data.m_port, m_data.m_port);
            CppToC(data.m_checkAlivePeriodInSeconds, m_data.m_checkAlivePeriodInSeconds);
            CppToC(data.m_reconnectWaitTimeInSeconds, m_data.m_reconnectWaitTimeInSeconds);
            CppToC(data.m_checkAliveResponseMode, m_data.m_checkAliveResponseMode);
            CppToC(data.m_checkState, m_data.m_checkState);
        }
    };
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
    template<>
    struct Converter2C<DownstreamSettings> : ConverterBase<HermesDownstreamSettings>
    {
        Converter2C(const DownstreamSettings& data)
        {
            CppToC(data.m_machineId, m_data.m_machineId);
            CppToC(data.m_optionalClientAddress, m_data.m_optionalClientAddress);
            CppToC(data.m_port, m_data.m_port);
            CppToC(data.m_checkAlivePeriodInSeconds, m_data.m_checkAlivePeriodInSeconds);
            CppToC(data.m_reconnectWaitTimeInSeconds, m_data.m_reconnectWaitTimeInSeconds);
            CppToC(data.m_checkAliveResponseMode, m_data.m_checkAliveResponseMode);
            CppToC(data.m_checkState, m_data.m_checkState);
        }
    };
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
    template<>
    struct Converter2C<ConfigurationServiceSettings> : ConverterBase<HermesConfigurationServiceSettings>
    {
        Converter2C(const ConfigurationServiceSettings& data)
        {
            CppToC(data.m_port, m_data.m_port);
            CppToC(data.m_reconnectWaitTimeInSeconds, m_data.m_reconnectWaitTimeInSeconds);
        }
    };
    inline ConfigurationServiceSettings ToCpp(const HermesConfigurationServiceSettings& data)
    {
        ConfigurationServiceSettings result;
        CToCpp(data.m_port, result.m_port);
        CToCpp(data.m_reconnectWaitTimeInSeconds, result.m_reconnectWaitTimeInSeconds);
        return result;
    }

    // VerticalServiceSettings
    template<>
    struct Converter2C<VerticalServiceSettings> : ConverterBase<HermesVerticalServiceSettings>
    {
        Converter2C(const VerticalServiceSettings& data)
        {
            CppToC(data.m_systemId, m_data.m_systemId);
            CppToC(data.m_port, m_data.m_port);
            CppToC(data.m_reconnectWaitTimeInSeconds, m_data.m_reconnectWaitTimeInSeconds);
            CppToC(data.m_checkAlivePeriodInSeconds, m_data.m_checkAlivePeriodInSeconds);
            CppToC(data.m_checkAliveResponseMode, m_data.m_checkAliveResponseMode);
        }
    };
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

    // VerticalClientSettings
    template<>
    struct Converter2C<VerticalClientSettings> : ConverterBase<HermesVerticalClientSettings>
    {
        Converter2C(const VerticalClientSettings& data)
        {
            CppToC(data.m_systemId, m_data.m_systemId);
            CppToC(data.m_hostAddress, m_data.m_hostAddress);
            CppToC(data.m_port, m_data.m_port);
            CppToC(data.m_reconnectWaitTimeInSeconds, m_data.m_reconnectWaitTimeInSeconds);
            CppToC(data.m_checkAlivePeriodInSeconds, m_data.m_checkAlivePeriodInSeconds);
            CppToC(data.m_checkAliveResponseMode, m_data.m_checkAliveResponseMode);
        }
    };
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

    // Error
    template<>
    struct Converter2C<Error> : ConverterBase<HermesError>
    {
        Converter2C(const Error& data)
        {
            CppToC(data.m_code, m_data.m_code);
            CppToC(data.m_text, m_data.m_text);
        }
    };
    inline Error ToCpp(const HermesError& data)
    { 
        Error result;
        CToCpp(data.m_code, result.m_code);
        CToCpp(data.m_text, result.m_text);
        return result;
    }


}
