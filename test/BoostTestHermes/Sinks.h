#pragma once

#include <Hermes.hpp>

#include "Trace.h"

#include <condition_variable>
#include <iostream>
#include <mutex>

namespace
{
    using Mutex = std::mutex;
    using Lock = std::unique_lock<Mutex>;
    using Cv = std::condition_variable;

    template<class Host>
    struct ChangeLock
    {
        Host* m_pHost;
        Lock m_lock{m_pHost->m_mutex};

        explicit ChangeLock(Host* pHost) : m_pHost(pHost) {}
        ~ChangeLock() { m_pHost->m_cv.notify_all(); }
    };

    template<class HostT, class PredicateT>
    void WaitFor(HostT& host, PredicateT p)
    {
        Lock lock(host.m_mutex);
        while (!p())
            host.m_cv.wait(lock);
    }
}


namespace Hermes
{

    struct DownstreamSink : IDownstreamCallback
    {
        Mutex m_mutex;
        Cv m_cv;
        using ChangeLock = ::ChangeLock<DownstreamSink>;
        unsigned m_sessionId{0U};
        EState m_state{EState::eNOT_CONNECTED};
        ConnectionInfo m_connectionInfo;
        ServiceDescription m_serviceDescription;
        MachineReadyData m_machineReadyData;
        StartTransportData m_startTransportData;
        StopTransportData m_stopTransportData;
        NotificationData m_notificationData;

        void OnConnected(unsigned sessionId, EState state, const Hermes::ConnectionInfo& connectionInfo) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_connectionInfo = connectionInfo;
        }

        void On(unsigned sessionId, EState state, const Hermes::ServiceDescription& serviceDescription) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_serviceDescription = serviceDescription;
        }

        void On(unsigned sessionId, const Hermes::NotificationData& data) override
        {
            ChangeLock lock(this);
            m_notificationData = data;
            m_sessionId = sessionId;
        }

        void OnState(unsigned sessionId, EState state) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
        }

        void OnDisconnected(unsigned sessionId, EState state, const Hermes::Error&) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
        }


        void On(unsigned sessionId, EState state, const Hermes::MachineReadyData& data) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_machineReadyData = data;
            m_sessionId = sessionId;
        }

        void On(unsigned sessionId, EState state, const RevokeMachineReadyData&) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
        }

        void On(unsigned sessionId, EState state, const Hermes::StartTransportData& data) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_startTransportData = data;
            m_sessionId = sessionId;
        }

        void On(unsigned sessionId, EState state, const Hermes::StopTransportData& data) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_stopTransportData = data;
            m_sessionId = sessionId;
        }


        void OnTrace(unsigned sessionId, ETraceType type, StringView trace) override
        {
            TestTrace("DownstreamTrace", type, sessionId, trace);
        }

    };

    struct UpstreamSink : IUpstreamCallback
    {
        Mutex m_mutex;
        Cv m_cv;
        using ChangeLock = ::ChangeLock<UpstreamSink>;
        unsigned m_sessionId{0U};
        EState m_state{EState::eNOT_CONNECTED};
        ConnectionInfo m_connectionInfo;
        ServiceDescription m_serviceDescription;
        BoardAvailableData m_boardAvailableData;
        TransportFinishedData m_transportFinishedData;
        NotificationData m_notificationData;

        void OnConnected(unsigned sessionId, EState state, const Hermes::ConnectionInfo& connectionInfo) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_connectionInfo = connectionInfo;
        }

        void On(unsigned sessionId, EState state, const Hermes::ServiceDescription& serviceDescription) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_serviceDescription = serviceDescription;
        }

        void On(unsigned sessionId, const Hermes::NotificationData& data) override
        {
            ChangeLock lock(this);
            m_sessionId = sessionId;
            m_notificationData = data;
        }

        void OnState(unsigned sessionId, EState state) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
        }

        void OnDisconnected(unsigned sessionId, EState state, const Hermes::Error&) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
        }


        void On(unsigned sessionId, EState state, const Hermes::BoardAvailableData& data) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_boardAvailableData = data;
        }

        void On(unsigned sessionId, EState state, const Hermes::RevokeBoardAvailableData&) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
        }

        void On(unsigned sessionId, EState state, const Hermes::TransportFinishedData& data) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_transportFinishedData = data;
        }

        void OnTrace(unsigned sessionId, ETraceType type, StringView trace) override
        {
            TestTrace("UpstreamTrace", type, sessionId, trace);
        }

    };



}
