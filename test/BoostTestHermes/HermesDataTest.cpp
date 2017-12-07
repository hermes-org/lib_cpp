// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include <HermesDataConversion.hpp>
#include "HermesDataGenerators.h"

#include <boost/test/data/test_case.hpp>

BOOST_AUTO_TEST_SUITE(DataTestSuite);

BOOST_AUTO_TEST_CASE_TEMPLATE(HermesDataEqualityTest, T, HermesDataTypes)
{
    auto samples = GenerateSamples<T>();

    BOOST_TEST(samples[0] == samples[0]);
    for (std::size_t i = 1U; i < samples.size(); ++i)
    {
        BOOST_TEST(samples[i] == samples[i]);
        BOOST_TEST(samples[i] != samples[i-1]);
        BOOST_TEST(samples[0] != samples[i]);
    }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(HermesCApiTest, T, HermesDataTypes)
{
    auto samples = GenerateSamples<T>();

    for (const auto& sample: samples)
    {
        auto apiData = ToC(sample);
        auto roundTrippedSample = Hermes::ToCpp(*(&apiData));
        BOOST_TEST(sample == roundTrippedSample);
        BOOST_TEST(!(sample != roundTrippedSample));
    }
}

BOOST_AUTO_TEST_SUITE_END();



