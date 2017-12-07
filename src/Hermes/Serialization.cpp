// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"
#include <HermesSerialization.h>

#include <HermesDataConversion.hpp>
#include "Serializer.h"

#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <atomic>


struct HermesStringHandle
{
    explicit HermesStringHandle(std::string&& rhs) : m_string(std::move(rhs)) {}
    explicit HermesStringHandle(const std::string& rhs) : m_string(rhs) {}

    std::string m_string;
};

HermesStringView HermesStringViewFromHandle(HermesStringHandle* pHandle)
{
    return Hermes::ToC(pHandle->m_string);
}

HermesStringHandle* HermesXmlFromServiceDescription(const HermesServiceDescription* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromBoardAvailableData(const HermesBoardAvailableData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromRevokeBoardAvailableData(const HermesRevokeBoardAvailableData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromMachineReadyData(const HermesMachineReadyData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromRevokeMachineReadyData(const HermesRevokeMachineReadyData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromStartTransportData(const HermesStartTransportData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromStopTransportData(const HermesStopTransportData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromTransportFinishedData(const HermesTransportFinishedData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromNotificationData(const HermesNotificationData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromCheckAliveData(const HermesCheckAliveData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromGetConfigurationData(const HermesGetConfigurationData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromSetConfigurationData(const HermesSetConfigurationData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}
HermesStringHandle* HermesXmlFromCurrentConfigurationData(const HermesCurrentConfigurationData* pData)
{
    return new HermesStringHandle(Hermes::Serialize(Hermes::ToCpp(*pData)));
}

void FreeHermesString(HermesStringHandle* pHandle)
{
    delete pHandle;
}

namespace Hermes
{
    namespace
    {
        template<class T>
        struct DataHolder
        { 
            DataHolder(const T& cppData) : m_cppData(cppData) {}
            DataHolder(const DataHolder&) = delete;
            DataHolder& operator=(const DataHolder&) = delete;
            DataHolder(DataHolder&& rhs) : m_cppData(std::move(rhs.m_cppData)), m_convertedData(ToC(m_cppData)) {}
            DataHolder& operator=(DataHolder&& rhs)
            {
                m_cppData = std::move(rhs.m_cppData);
                m_convertedData = ToC(m_cppData);
                return *this;
            }

            T m_cppData;
            using ConvertedData = decltype(ToC(m_cppData));
            ConvertedData m_convertedData = ToC(m_cppData);
        };

        template<>
        struct DataHolder<boost::blank>
        {
            DataHolder() = default;
            DataHolder(boost::blank) {}
        };

        template<class... Ts>
        struct VariantDataHolder : boost::static_visitor<>
        {
            using Variant = boost::variant<DataHolder<Ts>...>;
            Variant m_variant;

            //template<class T>
            //VariantDataHolder(T&& m_value) : m_variant(std::forward<T>(m_value)) {}

            template<class T>
            void operator()(T&& value)
            {
                m_variant = Variant(std::forward<T>(value));
            }
        };

        using MyVariantDataHolder = VariantDataHolder<boost::blank, Error,
            ServiceDescription,
            BoardAvailableData,
            RevokeBoardAvailableData,
            MachineReadyData,
            RevokeMachineReadyData,
            StartTransportData,
            TransportFinishedData,
            StopTransportData,
            NotificationData,
            CheckAliveData,
            GetConfigurationData,
            SetConfigurationData,
            CurrentConfigurationData>;

    }
}

struct HermesDataHandle
{
    HermesDataHandle(Hermes::StringView message)
    {
        auto variant = Hermes::Deserialize(message);
        boost::apply_visitor(m_holder, variant);
    }

    Hermes::MyVariantDataHolder m_holder;
};

HermesDataHandle* HermesDataFromXml(HermesStringView message)
{
    return new HermesDataHandle(Hermes::ToCpp(message));
}

const HermesError* HermesErrorFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::Error>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesServiceDescription* HermesServiceDescriptionFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::ServiceDescription>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesBoardAvailableData* HermesBoardAvailableDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::BoardAvailableData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesRevokeBoardAvailableData* HermesRevokeBoardAvailableDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::RevokeBoardAvailableData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesMachineReadyData* HermesMachineReadyDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::MachineReadyData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesRevokeMachineReadyData* HermesRevokeMachineReadyDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::RevokeMachineReadyData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesStartTransportData* HermesStartTransportDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::StartTransportData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesStopTransportData* HermesStopTransportDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::StopTransportData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesTransportFinishedData* HermesTransportFinishedDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::TransportFinishedData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesNotificationData* HermesNotificationDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::NotificationData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesCheckAliveData* HermesCheckAliveDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::CheckAliveData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesGetConfigurationData* HermesGetConfigurationDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::GetConfigurationData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesSetConfigurationData* HermesSetConfigurationDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::SetConfigurationData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}
const HermesCurrentConfigurationData* HermesCurrentConfigurationDataFromHandle(HermesDataHandle* pHandle)
{
    if (auto p = boost::get<Hermes::DataHolder<Hermes::CurrentConfigurationData>>(&pHandle->m_holder.m_variant))
        return &p->m_convertedData;
    return nullptr;
}

void FreeHermesData(HermesDataHandle* pHandle)
{
    delete pHandle;
}

