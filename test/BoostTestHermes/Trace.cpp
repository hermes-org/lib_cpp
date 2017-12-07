// Copyright(c) ASM Assembly Systems GmbH & Co.KG
#include "stdafx.h"

#include "Trace.h"

using Mutex = std::mutex;
using Lock = std::unique_lock<Mutex>;
Mutex s_traceMutex;

void TestTrace(Hermes::StringView sender, Hermes::ETraceType type, unsigned sessionId, Hermes::StringView trace)
{
    Lock lock(s_traceMutex);
    std::cout << sender << '#' << sessionId << '(' << type << "): " << trace << '\n';
}

TestCaseScope::TestCaseScope(Hermes::StringView testCase) : m_testCase(testCase)
{
    Lock lock(s_traceMutex);
    std::cout << "=========BEGIN " << m_testCase << " ===========================\n";
}

TestCaseScope::~TestCaseScope()
{
    Lock lock(s_traceMutex);
    std::cout << "===========END " << m_testCase << " ===========================\n";
}