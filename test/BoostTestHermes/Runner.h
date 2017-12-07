// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#pragma once

#include <thread>

template<class T>
class Runner
{
public:
    
    explicit Runner(T& runnable) : m_runnable(runnable)
    { 
        m_thread = std::thread([this]() { m_runnable.Run(); });
    }

    ~Runner()
    {
        m_runnable.Stop();
        m_thread.join();
    }

private:
    T& m_runnable;
    std::thread m_thread;
};
