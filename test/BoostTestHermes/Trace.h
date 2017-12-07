// Copyright(c) ASM Assembly Systems GmbH & Co.KG
#pragma once

#include <HermesData.hpp>

#include <iostream>
#include <mutex>

void TestTrace(Hermes::StringView sender, Hermes::ETraceType type, unsigned sessionId, Hermes::StringView trace);

class TestCaseScope
{
public:
    TestCaseScope(Hermes::StringView testCase);
    ~TestCaseScope();

private:
    std::string m_testCase;
};
