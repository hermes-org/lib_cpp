/***********************************************************************
Copyright ASM Assembly Systems GmbH & Co. KG

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
        ServiceDescriptionData m_serviceDescription;
        MachineReadyData m_machineReadyData;
        StartTransportData m_startTransportData;
        StopTransportData m_stopTransportData;
        QueryBoardInfoData m_queryBoardInfoData;
        NotificationData m_notificationData;
        CheckAliveData m_checkAliveData;

        void OnConnected(unsigned sessionId, EState state, const Hermes::ConnectionInfo& connectionInfo) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_connectionInfo = connectionInfo;
        }

        void On(unsigned sessionId, EState state, const Hermes::ServiceDescriptionData& serviceDescription) override
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

        void On(unsigned sessionId, const Hermes::CheckAliveData& data) override
        {
            ChangeLock lock(this);
            m_checkAliveData = data;
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

        void On(unsigned sessionId, const Hermes::QueryBoardInfoData& data) override
        {
            ChangeLock lock(this);
            m_queryBoardInfoData = data;
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
        ServiceDescriptionData m_serviceDescription;
        BoardAvailableData m_boardAvailableData;
        TransportFinishedData m_transportFinishedData;
        BoardForecastData m_boardForecastData;
        SendBoardInfoData m_sendBoardInfoData;
        NotificationData m_notificationData;
        CheckAliveData m_checkAliveData;

        void OnConnected(unsigned sessionId, EState state, const Hermes::ConnectionInfo& connectionInfo) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_connectionInfo = connectionInfo;
        }

        void On(unsigned sessionId, EState state, const Hermes::ServiceDescriptionData& serviceDescription) override
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

        void On(unsigned sessionId, const Hermes::CheckAliveData& data) override
        {
            ChangeLock lock(this);
            m_checkAliveData = data;
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

        void On(unsigned sessionId, EState state, const Hermes::BoardForecastData& data) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_boardForecastData = data;
        }

        void On(unsigned sessionId, const Hermes::SendBoardInfoData& data) override
        {
            ChangeLock lock(this);
            m_sessionId = sessionId;
            m_sendBoardInfoData = data;
        }

        void OnTrace(unsigned sessionId, ETraceType type, StringView trace) override
        {
            TestTrace("UpstreamTrace", type, sessionId, trace);
        }

    };

    struct VerticalServiceSink : IVerticalServiceCallback
    {
        Mutex m_mutex;
        Cv m_cv;
        using ChangeLock = ::ChangeLock<VerticalServiceSink>;

        struct Session
        {
            unsigned m_sessionId{ 0U };
            EVerticalState m_state{ EVerticalState::eNOT_CONNECTED };
            Hermes::Optional<ConnectionInfo> m_connectionInfo;
            Hermes::Optional<SupervisoryServiceDescriptionData> m_serviceDescription;
            Hermes::Optional<GetConfigurationData> m_getConfigurationData;
            Hermes::Optional<SetConfigurationData> m_setConfigurationData;
            Hermes::Optional<SendWorkOrderInfoData> m_sendWorkOrderInfoData;
            Hermes::Optional<NotificationData> m_notificationData;
            Hermes::Optional<CheckAliveData> m_checkAliveData;
        };
        std::map<unsigned int, Session> m_sessionMap;

        void OnConnected(unsigned sessionId, EVerticalState state, const Hermes::ConnectionInfo& connectionInfo) override
        {
            ChangeLock lock(this);
            auto& session = m_sessionMap[sessionId];
            session.m_state = state;
            session.m_sessionId = sessionId;
            session.m_connectionInfo = connectionInfo;
        }

        void On(unsigned sessionId, EVerticalState state, const Hermes::SupervisoryServiceDescriptionData& serviceDescription) override
        {
            ChangeLock lock(this);
            auto& session = m_sessionMap[sessionId];
            session.m_state = state;
            session.m_serviceDescription = serviceDescription;
        }

        void On(unsigned sessionId, const Hermes::GetConfigurationData& data, const ConnectionInfo&) override
        {
            ChangeLock lock(this);
            m_sessionMap[sessionId].m_getConfigurationData = data;
        }

        void On(unsigned sessionId, const Hermes::SetConfigurationData& data, const ConnectionInfo&) override
        {
            ChangeLock lock(this);
            m_sessionMap[sessionId].m_setConfigurationData = data;
        }

        void On(unsigned sessionId, const Hermes::SendWorkOrderInfoData& data) override
        {
            ChangeLock lock(this);
            m_sessionMap[sessionId].m_sendWorkOrderInfoData = data;
        }

        void On(unsigned sessionId, const Hermes::NotificationData& data) override
        {
            ChangeLock lock(this);
            m_sessionMap[sessionId].m_notificationData = data;
        }

        void On(unsigned sessionId, const Hermes::CheckAliveData& data) override
        {
            ChangeLock lock(this);
            m_sessionMap[sessionId].m_checkAliveData = data;
        }

        void OnDisconnected(unsigned sessionId, EVerticalState state, const Hermes::Error&) override
        {
            ChangeLock lock(this);
            m_sessionMap[sessionId].m_state = state;
        }

        void OnTrace(unsigned sessionId, ETraceType type, StringView trace) override
        {
            TestTrace("VerticalServiceTrace", type, sessionId, trace);
        }

    };

    struct VerticalClientSink : IVerticalClientCallback
    {
        Mutex m_mutex;
        Cv m_cv;
        using ChangeLock = ::ChangeLock<VerticalClientSink>;

        unsigned m_sessionId{ 0U };
        EVerticalState m_state{ EVerticalState::eNOT_CONNECTED };
        Hermes::Optional<ConnectionInfo> m_connectionInfo;
        Hermes::Optional<SupervisoryServiceDescriptionData> m_serviceDescription;
        Hermes::Optional<CurrentConfigurationData> m_currentConfigurationData;
        Hermes::Optional<BoardArrivedData> m_boardArrivedData;
        Hermes::Optional<BoardDepartedData> m_boardDepartedData;
        Hermes::Optional<QueryWorkOrderInfoData> m_queryWorkOrderInfoData;
        Hermes::Optional<NotificationData> m_notificationData;
        Hermes::Optional<CheckAliveData> m_checkAliveData;

        void OnConnected(unsigned sessionId, EVerticalState state, const Hermes::ConnectionInfo& connectionInfo) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_connectionInfo = connectionInfo;
        }

        void On(unsigned sessionId, EVerticalState state, const Hermes::SupervisoryServiceDescriptionData& serviceDescription) override
        {
            ChangeLock lock(this);
            m_state = state;
            m_sessionId = sessionId;
            m_serviceDescription = serviceDescription;
        }

        void On(unsigned sessionId, const Hermes::CurrentConfigurationData& data) override
        {
            ChangeLock lock(this);
            m_sessionId = sessionId;
            m_currentConfigurationData = data;
        }

        void On(unsigned sessionId, const Hermes::BoardArrivedData& data) override
        {
            ChangeLock lock(this);
            m_sessionId = sessionId;
            m_boardArrivedData = data;
        }

        void On(unsigned sessionId, const Hermes::BoardDepartedData& data) override
        {
            ChangeLock lock(this);
            m_sessionId = sessionId;
            m_boardDepartedData = data;
        }

        void On(unsigned sessionId, const Hermes::QueryWorkOrderInfoData& data) override
        {
            ChangeLock lock(this);
            m_sessionId = sessionId;
            m_queryWorkOrderInfoData = data;
        }

        void On(unsigned sessionId, const Hermes::NotificationData& data) override
        {
            ChangeLock lock(this);
            m_sessionId = sessionId;
            m_notificationData = data;
        }

        void On(unsigned sessionId, const Hermes::CheckAliveData& data) override
        {
            ChangeLock lock(this);
            m_sessionId = sessionId;
            m_checkAliveData = data;
        }

        void OnDisconnected(unsigned sessionId, EVerticalState state, const Hermes::Error&) override
        {
            ChangeLock lock(this);
            m_sessionId = sessionId;
            m_state = state;
        }


        void OnTrace(unsigned sessionId, ETraceType type, StringView trace) override
        {
            TestTrace("VerticalClientTrace", type, sessionId, trace);
        }

    };

}
