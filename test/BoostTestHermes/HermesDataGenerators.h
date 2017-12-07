// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include <HermesData.hpp>

#include "HermesDataReflection.h"

#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/is_sequence.hpp>


// forward declaration
template<class T>
std::vector<T> GenerateSamples();

// unspecialized template for sample generation
template<class T, class EnableT = void>
struct SimpleSampleGenerator;

template<>
struct SimpleSampleGenerator<std::string>
{
    static std::vector<std::string> Generate()
    {
        return
        {
            std::string(),
            std::string("A"),
            std::string("Hello, world"),
            std::string(10000, 'X')
        };
    }
};

template<class E>
struct SimpleSampleGenerator<E, std::enable_if_t<std::is_enum<E>::value>>
{
    static std::vector<E> Generate()
    {
        std::vector<E> result;
        result.reserve(size(E()));
        for (std::size_t e = 0U; e < size(E()); ++e)
        {
            result.push_back(static_cast<E>(e));
        }
        return result;
    }
};

template<class T>
struct SimpleSampleGenerator<Hermes::Optional<T>>
{
    static std::vector<Hermes::Optional<T>> Generate()
    {
        auto values = SimpleSampleGenerator<T>::Generate();
        return{values.begin(), values.end()};
    }
};

template<class T>
struct SimpleSampleGenerator<T, std::enable_if_t<boost::fusion::traits::is_sequence<T>::value>>
{
    static std::vector<T> Generate()
    {
        return GenerateSamples<T>();
    }
};

template<class T>
struct SimpleSampleGenerator<std::vector<T>>
{
    static std::vector<std::vector<T>> Generate()
    {
        return{SimpleSampleGenerator<T>::Generate()};
    }
};

template<class I>
struct SimpleSampleGenerator<I, std::enable_if_t<std::is_signed<I>::value && std::is_integral<I>::value>>
{
    static std::vector<I> Generate()
    {
        return{std::numeric_limits<I>::min(), -1, 0, 1, std::numeric_limits<I>::max()};
    }
};

template<class U>
struct SimpleSampleGenerator<U, std::enable_if_t<std::is_unsigned<U>::value && std::is_integral<U>::value>>
{
    static std::vector<U> Generate()
    {
        return{0, 1, std::numeric_limits<U>::max()};
    }
};

template<>
struct SimpleSampleGenerator<double>
{
    static std::vector<double> Generate()
    {
        return{-10000.0, -1.0, 0.0, 1.0, 10000.0};
    };
};

template<>
struct SimpleSampleGenerator<bool>
{
    static std::list<bool> Generate()
    {
        return{false, true};
    };
};


template< class SampleT, class MemberT, MemberT SampleT::*memberT>
void AddNonDefaultSamples(std::vector<SampleT>& samples)
{
    for (const auto& member : SimpleSampleGenerator<MemberT>::Generate())
    {
        SampleT sample;
        if (sample.*memberT == member)
            continue;
        sample.*memberT = member;
        samples.push_back(sample);
    }
}

template<class M>
struct MemberTraits;

template<class ClassT, class MemberT>
struct MemberTraits<MemberT ClassT::*>
{
    using Class = ClassT;
    using Member = MemberT;
};

//inline std::vector<Hermes::ServiceDescription> GenerateNonDefaultServiceDescription()
//{
//    std::vector<Hermes::ServiceDescription> result;
//    AddNonDefaultSamples<Hermes::ServiceDescription, std::string, &Hermes::ServiceDescription::m_machineId>(result);
//        
//    return result;
//}
//
//inline std::vector<Hermes::MachineReadyData> GenerateNonDefaultMachineReadyData()
//{
//    std::vector<Hermes::MachineReadyData> result;
//    AddNonDefaultSamples<Hermes::MachineReadyData, Hermes::EBoardQuality, &Hermes::MachineReadyData::m_boardQuality>(result);
//    return result;
//}

//#define HERMES_TEST_ADD_NON_DEFAULT_SAMPLES(samples, member) \
//AddNonDefaultSamples<MemberTraits<decltype(member)>::Class, MemberTraits<decltype(member)>::Member, member>(samples)
//
//void AddAllNonDefaultSamples(std::vector<Hermes::MachineReadyData>& samples)
//{
//    HERMES_TEST_ADD_NON_DEFAULT_SAMPLES(samples, &Hermes::MachineReadyData::m_boardQuality);
//}
//
//void AddAllNonDefaultSamples(std::vector<Hermes::ServiceDescription>& samples)
//{
//    HERMES_TEST_ADD_NON_DEFAULT_SAMPLES(samples, &Hermes::ServiceDescription::m_laneId);
//    HERMES_TEST_ADD_NON_DEFAULT_SAMPLES(samples, &Hermes::ServiceDescription::m_machineId);
//}

//HERMES_ADD_SAMPLES(Hermes::MachineReadyData, (m_boardQuality))
template<class T>
std::vector<T> GenerateNonDefaultSamples()
{
    std::vector<T> result;
    T sample;
    boost::fusion::for_each(sample, [&](auto& member)
    {
        using MemberType = std::remove_reference_t<decltype(member)>;
        auto defaultValue = member;
        for (const auto& sampleValue : SimpleSampleGenerator<MemberType>::Generate())
        {
            if (defaultValue == sampleValue)
                continue; // no defaults
            member = sampleValue;
            result.push_back(sample);
        }
        member = defaultValue;
    });

    return result;
}

template<class T>
std::vector<T> GenerateSamples()
{
    std::vector<T> result{T()};
    T sample;
    boost::fusion::for_each(sample, [&](auto& member)
    {
        using MemberType = std::remove_reference_t<decltype(member)>;
        auto defaultValue = member;
        for (const auto& sampleValue : SimpleSampleGenerator<MemberType>::Generate())
        {
            if (defaultValue == sampleValue)
                continue; // no defaults
            member = sampleValue;
            result.push_back(sample);
        }
        member = defaultValue;
    });

    return result;
}
