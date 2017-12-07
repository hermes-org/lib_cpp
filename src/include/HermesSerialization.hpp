// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include "HermesSerialization.h"
#include "HermesDataConversion.hpp"


namespace Hermes
{
    class StringHandle
    {
    public:
        StringHandle() = default;
        StringHandle(HermesStringHandle* pHandle) : m_pHandle(pHandle) {}
       
        StringHandle(const StringHandle&) = delete;
        StringHandle& operator=(const StringHandle&) = delete;

        StringHandle(StringHandle&& rhs) noexcept: m_pHandle(rhs.m_pHandle)
        {
            rhs.m_pHandle = nullptr;
        }

        StringHandle& operator=(StringHandle&& rhs) noexcept
        {
            StringHandle(std::move(rhs)).swap(*this);
            return *this;
        }

        std::string value() const { return view(); }

        StringView view() const
        {
            auto stringView = ::HermesStringViewFromHandle(m_pHandle);
            return{stringView.m_pData, stringView.m_size};
        }

        explicit operator bool() const { return m_pHandle ? true : false; }
        bool operator!() const { return !m_pHandle; }

        void swap(StringHandle& rhs) noexcept { std::swap(m_pHandle, rhs.m_pHandle); }
        friend void swap(StringHandle& lhs, StringHandle& rhs) noexcept { lhs.swap(rhs); }

    private:
        HermesStringHandle* m_pHandle{nullptr};
    };

    inline std::string ToXml(const ServiceDescription& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromServiceDescription(&apiData)).value();
    }
    inline std::string ToXml(const BoardAvailableData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromBoardAvailableData(&apiData)).value();
    }
    inline std::string ToXml(const RevokeBoardAvailableData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromRevokeBoardAvailableData(&apiData)).value();
    }
    inline std::string ToXml(const MachineReadyData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromMachineReadyData(&apiData)).value();
    }
    inline std::string ToXml(const RevokeMachineReadyData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromRevokeMachineReadyData(&apiData)).value();
    }
    inline std::string ToXml(const StartTransportData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromStartTransportData(&apiData)).value();
    }
    inline std::string ToXml(const StopTransportData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromStopTransportData(&apiData)).value();
    }
    inline std::string ToXml(const TransportFinishedData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromTransportFinishedData(&apiData)).value();
    }
    inline std::string ToXml(const NotificationData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromNotificationData(&apiData)).value();
    }
    inline std::string ToXml(const CheckAliveData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromCheckAliveData(&apiData)).value();
    }
    inline std::string ToXml(const SetConfigurationData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromSetConfigurationData(&apiData)).value();
    }
    inline std::string ToXml(const GetConfigurationData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromGetConfigurationData(&apiData)).value();
    }
    inline std::string ToXml(const CurrentConfigurationData& data)
    {
        auto apiData = ToC(data);
        return StringHandle(::HermesXmlFromCurrentConfigurationData(&apiData)).value();
    }

    namespace Detail
    {
        template<class> struct OptionalGetter;
        template<> struct OptionalGetter<ServiceDescription> { static const HermesServiceDescription* Get(HermesDataHandle* pHandle) { return ::HermesServiceDescriptionFromHandle(pHandle); } };
        template<> struct OptionalGetter<BoardAvailableData> { static const HermesBoardAvailableData* Get(HermesDataHandle* pHandle) { return ::HermesBoardAvailableDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<RevokeBoardAvailableData> { static const HermesRevokeBoardAvailableData* Get(HermesDataHandle* pHandle) { return ::HermesRevokeBoardAvailableDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<MachineReadyData> { static const HermesMachineReadyData* Get(HermesDataHandle* pHandle) { return ::HermesMachineReadyDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<RevokeMachineReadyData> { static const HermesRevokeMachineReadyData* Get(HermesDataHandle* pHandle) { return ::HermesRevokeMachineReadyDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<StartTransportData> { static const HermesStartTransportData* Get(HermesDataHandle* pHandle) { return ::HermesStartTransportDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<StopTransportData> { static const HermesStopTransportData* Get(HermesDataHandle* pHandle) { return ::HermesStopTransportDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<TransportFinishedData> { static const HermesTransportFinishedData* Get(HermesDataHandle* pHandle) { return ::HermesTransportFinishedDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<NotificationData> { static const HermesNotificationData* Get(HermesDataHandle* pHandle) { return ::HermesNotificationDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<CheckAliveData> { static const HermesCheckAliveData* Get(HermesDataHandle* pHandle) { return ::HermesCheckAliveDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<GetConfigurationData> { static const HermesGetConfigurationData* Get(HermesDataHandle* pHandle) { return ::HermesGetConfigurationDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<SetConfigurationData> { static const HermesSetConfigurationData* Get(HermesDataHandle* pHandle) { return ::HermesSetConfigurationDataFromHandle(pHandle); } };
        template<> struct OptionalGetter<CurrentConfigurationData> { static const HermesCurrentConfigurationData* Get(HermesDataHandle* pHandle) { return ::HermesCurrentConfigurationDataFromHandle(pHandle); } };

    }

    class DataHandle
    {
    public:
        DataHandle() = default;
        DataHandle(HermesDataHandle* pHandle) : m_pHandle(pHandle) {}

        DataHandle(const DataHandle&) = delete;
        DataHandle& operator=(const DataHandle&) = delete;

        DataHandle(DataHandle&& rhs) noexcept: m_pHandle(rhs.m_pHandle)
        {
            rhs.m_pHandle = nullptr;
        }

        DataHandle& operator=(DataHandle&& rhs) noexcept
        {
            DataHandle(std::move(rhs)).swap(*this);
            return *this;
        }

        HermesDataHandle* get() const { return m_pHandle; }

        explicit operator bool() const { return m_pHandle ? true : false; }
        bool operator!() const { return !m_pHandle; }

        void swap(DataHandle& rhs) noexcept { std::swap(m_pHandle, rhs.m_pHandle); }
        friend void swap(DataHandle& lhs, DataHandle& rhs) noexcept { lhs.swap(rhs); }

    private:
        HermesDataHandle* m_pHandle{ nullptr };
    };

    template<class T>
    Optional<T> ToOptionalData(StringView view)
    {
        DataHandle handle(::HermesDataFromXml(ToC(view)));
        if (!handle)
            return{};

        if (auto* pData = Detail::OptionalGetter<T>::Get(handle.get()))
            return ToCpp(*pData);

        return{};
    }
}

