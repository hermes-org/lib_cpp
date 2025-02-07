/***********************************************************************
Copyright ASMPT Assembly Systems GmbH & Co. KG

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
    static std::vector<std::string> Generate(const std::string& defaultValue)
    {
        if (defaultValue == std::string{"00000000-0000-0000-0000-000000000000"})
            return
        {
            std::string{"00000000-0000-0000-0000-000000000000"},
            std::string{"90abb804-ef8d-11e7-8c3f-9a214cf093ae"},
            std::string{"0e4c18f9-5f69-4dbe-9b1c-e705bf7d680f"}
        };
        return
        {
            std::string(),
            std::string("A"),
            std::string("Hello, world"),
            std::string(10000, 'X'),
            std::string("\x48\xE2\x82\xAC\x72\xC2\xB5\x65\xC3\x9F")
        };
    }
};

template<class E>
struct SimpleSampleGenerator<E, std::enable_if_t<std::is_enum<E>::value>>
{
    static std::vector<E> Generate(E)
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

template<>
struct SimpleSampleGenerator<Hermes::ETransferState>
{
    static std::vector<Hermes::ETransferState> Generate(Hermes::ETransferState)
    {
        return{Hermes::ETransferState::eNOT_STARTED, Hermes::ETransferState::eINCOMPLETE, Hermes::ETransferState::eCOMPLETE};
    }
};

template<>
struct SimpleSampleGenerator<Hermes::ESeverity>
{
    static std::vector<Hermes::ESeverity> Generate(Hermes::ESeverity)
    {
        return{Hermes::ESeverity::eFATAL, Hermes::ESeverity::eERROR, Hermes::ESeverity::eWARNING, Hermes::ESeverity::eINFO};
    }
};

template<class T>
struct SimpleSampleGenerator<T, std::enable_if_t<std::is_empty<T>::value>>
{
    static std::vector<T> Generate(const T&)
    {
        return{T{}};
    }
};

template<class T>
struct SimpleSampleGenerator<Hermes::Optional<T>>
{
    static std::vector<Hermes::Optional<T>> Generate(const Hermes::Optional<T>&)
    {
        auto values = SimpleSampleGenerator<T>::Generate(T());
        return{values.begin(), values.end()};
    }
};

template<class T>
struct SimpleSampleGenerator<T, std::enable_if_t<boost::fusion::traits::is_sequence<T>::value>>
{
    static std::vector<T> Generate(const T&)
    {
        return GenerateSamples<T>();
    }
};

template<class T>
struct SimpleSampleGenerator<std::vector<T>>
{
    static std::vector<std::vector<T>> Generate(const std::vector<T>&)
    {
        return{SimpleSampleGenerator<T>::Generate(T())};
    }
};

template<class I>
struct SimpleSampleGenerator<I, std::enable_if_t<std::is_signed<I>::value && std::is_integral<I>::value>>
{
    static std::vector<I> Generate(I)
    {
        return{std::numeric_limits<I>::min(), -1, 0, 1, std::numeric_limits<I>::max()};
    }
};

template<class U>
struct SimpleSampleGenerator<U, std::enable_if_t<std::is_unsigned<U>::value && std::is_integral<U>::value>>
{
    static std::vector<U> Generate(U defaultValue)
    {
        return{ defaultValue, defaultValue + 1U, std::numeric_limits<U>::max()};
    }
};

template<>
struct SimpleSampleGenerator<unsigned>
{
    static std::vector<unsigned> Generate(unsigned defaultValue)
    {
        return{ defaultValue, defaultValue + 1, static_cast<unsigned>(std::numeric_limits<int>::max())};
    }
};

template<>
struct SimpleSampleGenerator<double>
{
    static std::vector<double> Generate(double)
    {
        return{0.0, 0.375, 1.0, 10000.125};
    };
};

template<>
struct SimpleSampleGenerator<bool>
{
    static std::list<bool> Generate(bool)
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


template<class T>
std::vector<T> GenerateNonDefaultSamples()
{
    std::vector<T> result;
    T sample;
    boost::fusion::for_each(sample, [&](auto& member)
    {
        using MemberType = std::remove_reference_t<decltype(member)>;
        auto defaultValue = member;
        for (const auto& sampleValue : SimpleSampleGenerator<MemberType>::Generate(defaultValue))
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
        for (const auto& sampleValue : SimpleSampleGenerator<MemberType>::Generate(defaultValue))
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
