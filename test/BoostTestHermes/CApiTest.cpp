// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include <Hermes.h>
#include <Windows.h>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

// some infrastructure to aid testing:
namespace
{
    class Mutex
    {
    public:

        Mutex() { ::InitializeCriticalSection(&m_criticalSection); }
        ~Mutex() { ::DeleteCriticalSection(&m_criticalSection); }

    private:
        friend class Lock;
        CRITICAL_SECTION m_criticalSection;
        Mutex(const Mutex&);
        Mutex& operator=(const Mutex&);
    };

    class Lock
    {
    public:
        Lock(Mutex& mutex) : m_mutex(mutex) { ::EnterCriticalSection(&m_mutex.m_criticalSection); }
        ~Lock() { ::LeaveCriticalSection(&m_mutex.m_criticalSection); }

    private:
        Lock(const Lock&);
        Lock& operator=(const Lock&);
        Mutex& m_mutex;
    };


    std::string sHermesVersion = "1.0";
    std::string sLocalHost = "localhost";
    Mutex sTraceMutex;
    const unsigned cCHECK_ALIVE_PERIOD = 60U;
    const unsigned cRECONNECT_TIMEOUT = 5U;
    const unsigned short cPORT = 50101;
    const unsigned cMAX_LANE_COUNT = 2U;

    void Trace(const std::string& in_name, unsigned sessionId, EHermesTraceType type, HermesStringView trace)
    {
        Lock lock(sTraceMutex);
        std::cout << in_name << '#' << sessionId << '(';
        switch (type)
        {
        case eHERMES_TRACE_SENT: std::cout << "Sent: "; break;
        case eHERMES_TRACE_RECEIVED: std::cout << "Received: "; break;
        case eHERMES_TRACE_DEBUG: std::cout << "Debug: "; break;
        case eHERMES_TRACE_INFO: std::cout << "Info: "; break;
        case eHERMES_TRACE_WARNING: std::cout << "Warning: "; break;
        case eHERMES_TRACE_ERROR: std::cout << "Error: "; break;
        default: std::cout << "Unknown: ";
        }
        std::cout.write(trace.m_pData, trace.m_size);
    }
}

#ifdef BOOST_CHECK_EQUAL
#define CHECK_EQUAL(lhs, rhs) BOOST_CHECK_EQUAL(lhs, rhs)
#else
#define CHECK_EQUAL(lhs, rhs) assert((lhs) == (rhs));
#endif

class DownstreamProxy
{
public:

    DownstreamProxy(unsigned laneId) :
        m_laneId(laneId),
        m_pHermes(0),
        m_threadHandle(0), 
        m_state(eHERMES_NOT_CONNECTED), 
        m_sessionId(0U),
        m_bottomBarcode("ABCDEF"),
        m_width(210.0),
        m_configuration()
    {
        m_configuration.m_downstreamLaneId = m_laneId;
        HermesDownstreamCallbacks callbacks = {
            {&DownstreamProxy::OnConnected_, this},
            {&DownstreamProxy::OnServiceDescription_, this},
            {&DownstreamProxy::OnMachineReady_, this},
            {0, 0}, // OnRevokeMachineReady
            {&DownstreamProxy::OnStartTransport_, this},
            {&DownstreamProxy::OnStopTransport_, this},
            {0, 0}, // OnNotification
            {0, 0}, // OnState
            {0, 0}, // OnCheckAlive
            {0, 0}, // OnDisconnected
            {&DownstreamProxy::OnTrace_, this}
        };

        m_pHermes = ::CreateHermesDownstream(1U, &callbacks);
        m_completedEventHandle = ::CreateEvent(NULL, TRUE, FALSE, 0);
        m_threadHandle = ::CreateThread(0, 0, &DownstreamProxy::Run_, this, 0, 0);
    }

    void SetConfiguration(const std::string& optionalClientAddress, unsigned short port,
        const std::string& machineId)
    {
        // do not bother if nothing has changed:
        if (m_optionalClientAddress == optionalClientAddress &&
            port == m_configuration.m_port && 
            m_machineId == machineId)
            return;

        m_configuration.m_port = port;
        m_optionalClientAddress = optionalClientAddress;
        m_configuration.m_optionalClientAddress.m_pData = m_optionalClientAddress.data();
        m_configuration.m_optionalClientAddress.m_size = static_cast<uint32_t>(m_optionalClientAddress.size());
        m_machineId = m_machineId;

        if (port) // enabled
        {
            HermesDownstreamSettings settings =
            {
                {m_machineId.data(), static_cast<uint32_t>(m_machineId.size())},
                m_configuration.m_optionalClientAddress,
                m_configuration.m_port,
                cCHECK_ALIVE_PERIOD,
                cRECONNECT_TIMEOUT,
                eHERMES_CHECK_SEND_AND_RECEIVE
            };
            ::EnableHermesDownstream(m_pHermes, &settings);
        }
        else
        {
            const char* cTEXT = "Connection was disabled";
            HermesNotificationData notification =
            {
                eHERMES_CONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION,
                eHERMES_INFO,
                {cTEXT, static_cast<uint32_t>(::strlen(cTEXT))}
            };
            ::DisableHermesDownstream(m_pHermes, &notification);
        }
    }

    void WaitForCompletion()
    {
        ::WaitForSingleObject(m_completedEventHandle, INFINITE);
    }

    void JoinThread()
    {
        if (!m_threadHandle)
            return;
        ::StopHermesDownstream(m_pHermes);
        ::WaitForSingleObject(m_threadHandle, INFINITE);
        m_threadHandle = 0;
    }

    ~DownstreamProxy()
    {
        JoinThread();
        ::DeleteHermesDownstream(m_pHermes);
    }

    std::string m_machineId;
    std::string m_boardId;
    std::string m_bottomBarcode;
    double m_width;
    HermesDownstreamConfiguration m_configuration;

private:
    unsigned m_laneId;
    HermesDownstream* m_pHermes;
    std::string m_optionalClientAddress; // this is the actual storage behind m_configuration.m_optionalClientAddress!

    HANDLE m_threadHandle;
    unsigned m_sessionId;
    EHermesState m_state;

    HANDLE m_completedEventHandle;

    static DWORD WINAPI Run_(void* pVoid)
    {
        DownstreamProxy* pThis = static_cast<DownstreamProxy*>(pVoid);
        ::RunHermesDownstream(pThis->m_pHermes);
        return 0;
    }

    static void OnConnected_(void* pVoid, unsigned sessionId, EHermesState state, const HermesConnectionInfo*)
    {
        DownstreamProxy* pThis = static_cast<DownstreamProxy*>(pVoid);
        pThis->m_sessionId = sessionId;
        pThis->m_state = state;
    }

    static void OnServiceDescription_(void* pVoid, unsigned sessionId, EHermesState state, const HermesServiceDescription*)
    {
        DownstreamProxy* pThis = static_cast<DownstreamProxy*>(pVoid);
        if (pThis->m_sessionId != sessionId)
            return;

        pThis->m_state = state;
        HermesServiceDescription serviceDescription = 
        {
            {pThis->m_machineId.data(), static_cast<uint32_t>(pThis->m_machineId.size())},
            pThis->m_laneId,
            {sHermesVersion.data(), static_cast<uint32_t>(sHermesVersion.size())}
        };
        ::SignalHermesDownstreamServiceDescription(pThis->m_pHermes, pThis->m_sessionId, &serviceDescription);
    }
    
    static void OnMachineReady_(void* pVoid, unsigned sessionId, EHermesState state, const HermesMachineReadyData*)
    {
        DownstreamProxy* pThis = static_cast<DownstreamProxy*>(pVoid);
        if (pThis->m_sessionId != sessionId)
            return;

        pThis->m_state = state;

        GUID guid;
        ::UuidCreateSequential(&guid);
        RPC_CSTR guidString;
        ::UuidToStringA(&guid, &guidString);
        pThis->m_boardId = reinterpret_cast<const char*>(guidString);
        ::RpcStringFreeA(&guidString);

        HermesBoardAvailableData boardAvailableData =
        {
            {pThis->m_boardId.data(), static_cast<uint32_t>(pThis->m_boardId.size())},
            {pThis->m_machineId.data(), static_cast<uint32_t>(pThis->m_machineId.size())},
            eHERMES_GOOD_BOARD_QUALITY,
            {0, 0},
            eHERMES_BOARD_TOP_SIDE_IS_UP,
            {0, 0},
            {pThis->m_bottomBarcode.data(), static_cast<uint32_t>(pThis->m_bottomBarcode.size())},
            0,
            &pThis->m_width,
        };

        ::SignalHermesBoardAvailable(pThis->m_pHermes, pThis->m_sessionId, &boardAvailableData);
    }

    static void OnStartTransport_(void* pVoid, unsigned sessionId, EHermesState state, const HermesStartTransportData* data)
    {
        DownstreamProxy* pThis = static_cast<DownstreamProxy*>(pVoid);
        if (pThis->m_sessionId != sessionId)
            return;

        pThis->m_state = state;

        CHECK_EQUAL(pThis->m_boardId.compare(0U, std::string::npos, data->m_boardId.m_pData, data->m_boardId.m_size), 0);

        HermesTransportFinishedData transportFinishedData =
        {
            eHERMES_TRANSFER_COMPLETE,
            data->m_boardId,
        };
        ::SignalHermesTransportFinished(pThis->m_pHermes, pThis->m_sessionId, &transportFinishedData);
    }

    static void OnStopTransport_(void* pVoid, unsigned sessionId, EHermesState state, const HermesStopTransportData* data)
    {
        DownstreamProxy* pThis = static_cast<DownstreamProxy*>(pVoid);
        if (pThis->m_sessionId != sessionId)
            return;

        pThis->m_state = state;

        CHECK_EQUAL(pThis->m_boardId.compare(0U, std::string::npos, data->m_boardId.m_pData, data->m_boardId.m_size), 0);

        // finished:
        ::SetEvent(pThis->m_completedEventHandle);
    }

    static void OnTrace_(void*, unsigned sessionId, EHermesTraceType type, const HermesStringView trace)
    {
        Trace("Downstream", sessionId, type, trace);
    }
    
    // no copying etc.:
    DownstreamProxy(const DownstreamProxy&);
    DownstreamProxy& operator=(const DownstreamProxy&);

};

class UpstreamProxy
{
public:

    UpstreamProxy(unsigned laneId) :
        m_laneId(laneId),
        m_pHermes(0),
        m_threadHandle(0),
        m_state(eHERMES_NOT_CONNECTED),
        m_sessionId(0U),
        m_width(0.0),
        m_configuration()
    {
        m_configuration.m_upstreamLaneId = m_laneId;

        HermesUpstreamCallbacks callbacks = {
            {&UpstreamProxy::OnConnected_, this},
            {&UpstreamProxy::OnServiceDescription_, this},
            {&UpstreamProxy::OnBoardAvailable_, this},
            {0, 0}, // OnRevokeBoardAvailable
            {&UpstreamProxy::OnTransportFinished_, this},
            {0, 0}, // OnNotification
            {0, 0}, // OnState
            {0, 0}, // OnCheckAlive
            {0, 0}, // OnDisconnected
            {&UpstreamProxy::OnTrace_, this}
        };

        m_pHermes = ::CreateHermesUpstream(1U, &callbacks);

        m_completedEventHandle = ::CreateEvent(NULL, TRUE, FALSE, 0);
        m_threadHandle = ::CreateThread(0, 0, &UpstreamProxy::Run_, this, 0, 0);
    }

    void SetConfiguration(const std::string& serverAddress, unsigned short port,
        const std::string& machineId)
    {
        // do not bother if nothing has changed:
        if (m_serverAddress == serverAddress &&
            port == m_configuration.m_port &&
            m_machineId == machineId)
            return;

        m_configuration.m_port = port;
        m_serverAddress = serverAddress;
        m_configuration.m_hostAddress.m_pData = m_serverAddress.data();
        m_configuration.m_hostAddress.m_size = static_cast<uint32_t>(m_serverAddress.size());
        m_machineId = m_machineId;

        if (port) // enabled
        {
            HermesUpstreamSettings settings =
            {
                {m_machineId.data(), static_cast<uint32_t>(m_machineId.size())},
                m_configuration.m_hostAddress,
                m_configuration.m_port,
                cCHECK_ALIVE_PERIOD,
                cRECONNECT_TIMEOUT,
                eHERMES_CHECK_SEND_AND_RECEIVE
            };
            ::EnableHermesUpstream(m_pHermes, &settings);
        }
        else
        {
            const char* cTEXT = "Connection was disabled";
            HermesNotificationData notification =
            {
                eHERMES_CONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION,
                eHERMES_INFO,
                {cTEXT, static_cast<uint32_t>(::strlen(cTEXT))}
            };
            ::DisableHermesUpstream(m_pHermes, &notification);
        }
    }

    void WaitForCompletion()
    {
        ::WaitForSingleObject(m_completedEventHandle, INFINITE);
    }

    void JoinThread()
    {
        if (!m_threadHandle)
            return;
        ::StopHermesUpstream(m_pHermes);
        ::WaitForSingleObject(m_threadHandle, INFINITE);
        m_threadHandle = 0;
    }

    ~UpstreamProxy()
    {
        JoinThread();
        ::DeleteHermesUpstream(m_pHermes);
    }

    std::string m_machineId;
    std::string m_boardId;
    std::string m_boardIdCreatedBy;
    std::string m_bottomBarcode;
    double m_width;
    HermesUpstreamConfiguration m_configuration;

private:
    unsigned m_laneId;
    HermesUpstream* m_pHermes;
    std::string m_serverAddress; // this is the actual storage behind m_configuration.m_hostAddress!
    HANDLE m_threadHandle;
    unsigned m_sessionId;
    EHermesState m_state;
    HANDLE m_completedEventHandle;

    static DWORD WINAPI Run_(void* pVoid)
    {
        UpstreamProxy* pThis = static_cast<UpstreamProxy*>(pVoid);
        ::RunHermesUpstream(pThis->m_pHermes);
        return 0;
    }

    static void OnConnected_(void* pVoid, unsigned sessionId, EHermesState state, const HermesConnectionInfo*)
    {
        UpstreamProxy* pThis = static_cast<UpstreamProxy*>(pVoid);
        pThis->m_sessionId = sessionId;
        pThis->m_state = state;

        HermesServiceDescription serviceDescription =
        {
            {pThis->m_machineId.data(), static_cast<uint32_t>(pThis->m_machineId.size())},
            pThis->m_laneId,
            {sHermesVersion.data(), static_cast<uint32_t>(sHermesVersion.size())}
        };
        ::SignalHermesUpstreamServiceDescription(pThis->m_pHermes, pThis->m_sessionId, &serviceDescription);
    }

    static void OnServiceDescription_(void* pVoid, unsigned sessionId, EHermesState state, const HermesServiceDescription*)
    {
        UpstreamProxy* pThis = static_cast<UpstreamProxy*>(pVoid);
        if (pThis->m_sessionId != sessionId)
            return;

        pThis->m_state = state;

        HermesMachineReadyData machineReadyData = {eHERMES_ANY_BOARD_QUALITY};
        ::SignalHermesMachineReady(pThis->m_pHermes, pThis->m_sessionId, &machineReadyData);
    }

    static void OnBoardAvailable_(void* pVoid, unsigned sessionId, EHermesState state, const HermesBoardAvailableData* data)
    {
        UpstreamProxy* pThis = static_cast<UpstreamProxy*>(pVoid);
        if (pThis->m_sessionId != sessionId)
            return;

        pThis->m_state = state;

        pThis->m_boardId.assign(data->m_boardId.m_pData, data->m_boardId.m_size);
        pThis->m_boardIdCreatedBy.assign(data->m_boardIdCreatedBy.m_pData, data->m_boardIdCreatedBy.m_size);
        pThis->m_bottomBarcode.assign(data->m_optionalBottomBarcode.m_pData, data->m_optionalBottomBarcode.m_size);
        if (data->m_pOptionalWidthInMM)
            pThis->m_width = *data->m_pOptionalWidthInMM;

        HermesStartTransportData startTransportData = {data->m_boardId};
        ::SignalHermesStartTransport(pThis->m_pHermes, pThis->m_sessionId, &startTransportData);
    }

    static void OnTransportFinished_(void* pVoid, unsigned sessionId, EHermesState state, const HermesTransportFinishedData* data)
    {
        UpstreamProxy* pThis = static_cast<UpstreamProxy*>(pVoid);
        if (pThis->m_sessionId != sessionId)
            return;

        pThis->m_state = state;

        CHECK_EQUAL(pThis->m_boardId.compare(0U, std::string::npos, data->m_boardId.m_pData, data->m_boardId.m_size), 0);

        HermesStopTransportData stopTransportData =
        {
            eHERMES_TRANSFER_COMPLETE,
            data->m_boardId
        };
        ::SignalHermesStopTransport(pThis->m_pHermes, pThis->m_sessionId, &stopTransportData);

        // finished
        ::SetEvent(pThis->m_completedEventHandle);
    }

    static void OnTrace_(void*, unsigned sessionId, EHermesTraceType type, const HermesStringView trace)
    {
        Trace("UpstreamProxy", sessionId, type, trace);
    }
};

class ConfigurationProxy
{
public:
    ConfigurationProxy() :
        m_upstream1(1U),
        m_upstream2(2U),
        m_downstream1(1U),
        m_downstream2(2U),
        m_pHermes(0),
        m_threadHandle(0)
    {
        HermesConfigurationServiceCallbacks callbacks = {
            {0, 0}, // OnConnected
            {&ConfigurationProxy::OnSetConfiguration_, this},
            {&ConfigurationProxy::OnGetConfiguration_, this},
            {0, 0}, // OnDisconnected
            {&ConfigurationProxy::OnTrace_, this}
        };

        m_pHermes = ::CreateHermesConfigurationService(&callbacks);
        m_threadHandle = ::CreateThread(0, 0, &ConfigurationProxy::Run_, this, 0, 0);

        HermesConfigurationServiceSettings settings = {0, 10U};
        ::EnableHermesConfigurationService(m_pHermes, &settings);
    }

    UpstreamProxy* GetUpstreamProxy(unsigned laneId)
    {
        switch (laneId)
        {
        case 1U: return &m_upstream1;
        case 2U: return &m_upstream2;
        default: return 0;
        }
    }
    DownstreamProxy* GetDownstreamProxy(unsigned laneId)
    {
        switch (laneId)
        {
        case 1U: return &m_downstream1;
        case 2U: return &m_downstream2;
        default: return 0;
        }
    }

    void JoinThread()
    {
        if (!m_threadHandle)
            return;

        ::StopHermesConfigurationService(m_pHermes);
        ::WaitForSingleObject(m_threadHandle, INFINITE);
        m_threadHandle = 0;
    }

    ~ConfigurationProxy()
    {
        JoinThread();
        ::DeleteHermesConfigurationService(m_pHermes);
    }

private:

    UpstreamProxy m_upstream1;
    UpstreamProxy m_upstream2;
    DownstreamProxy m_downstream1;
    DownstreamProxy m_downstream2;

    HermesConfigurationService* m_pHermes;
    HANDLE m_threadHandle;

    std::string m_machineId;


    static DWORD WINAPI Run_(void* pVoid)
    {
        ConfigurationProxy* pThis = static_cast<ConfigurationProxy*>(pVoid);
        ::RunHermesConfigurationService(pThis->m_pHermes);
        return 0;
    }

    static void OnTrace_(void*, unsigned sessionId, EHermesTraceType type, const HermesStringView trace)
    {
        Trace("ConfigurationProxy", sessionId, type, trace);
    }

    static void OnGetConfiguration_(void* pVoid, uint32_t sessionId, const HermesConnectionInfo*, const HermesGetConfigurationData*)
    {
        const HermesUpstreamConfiguration *upstreamConfigurations[cMAX_LANE_COUNT];
        const HermesDownstreamConfiguration *downstreamConfigurations[cMAX_LANE_COUNT];

        ConfigurationProxy* pThis = static_cast<ConfigurationProxy*>(pVoid);
        HermesCurrentConfigurationData data = {{pThis->m_machineId.data(), static_cast<uint32_t>(pThis->m_machineId.size())}};
        data.m_downstreamConfigurations = downstreamConfigurations;
        data.m_upstreamConfigurations = upstreamConfigurations;

        for (unsigned laneId = 1; laneId <= cMAX_LANE_COUNT; ++laneId)
        {
            UpstreamProxy& upstreamProxy = *pThis->GetUpstreamProxy(laneId);
            if (upstreamProxy.m_configuration.m_port)
            {
                upstreamConfigurations[data.m_upstreamConfigurationCount] = &upstreamProxy.m_configuration;
                ++data.m_upstreamConfigurationCount;
            }
            DownstreamProxy& downstreamProxy = *pThis->GetDownstreamProxy(laneId);
            if (downstreamProxy.m_configuration.m_port)
            {
                downstreamConfigurations[data.m_downstreamConfigurationCount] = &downstreamProxy.m_configuration;
                ++data.m_downstreamConfigurationCount;
            }
        }
        ::SignalHermesCurrentConfiguration(pThis->m_pHermes, sessionId, &data);
    }

    static void OnSetConfiguration_(void* pVoid, uint32_t sessionId, const HermesConnectionInfo*, const HermesSetConfigurationData* pData)
    {
        const char* cINVALID_LANE = "Invalid lane";
        HermesUpstreamConfiguration upstreamConfigs[cMAX_LANE_COUNT] = {{1U}, {2U}};
        HermesDownstreamConfiguration downstreamConfigs[cMAX_LANE_COUNT] = {{1U}, {2U}};

        ConfigurationProxy* pThis = static_cast<ConfigurationProxy*>(pVoid);
        pThis->m_machineId.assign(pData->m_machineId.m_pData, pData->m_machineId.m_size);

        for (unsigned i = 0U; i < pData->m_upstreamConfigurationCount; ++i)
        {
            const HermesUpstreamConfiguration* pConfig = pData->m_upstreamConfigurations[i];
            UpstreamProxy* pProxy = pThis->GetUpstreamProxy(pConfig->m_upstreamLaneId);
            if (!pProxy)
            {
                HermesNotificationData notification = 
                {
                    eHERMES_CONFIGURATION_ERROR,
                    eHERMES_INFO,
                    {cINVALID_LANE, static_cast<uint32_t>(::strlen(cINVALID_LANE))}
                };
                ::SignalHermesConfigurationNotification(pThis->m_pHermes, sessionId, &notification);
                return;
            }
            upstreamConfigs[pConfig->m_upstreamLaneId - 1U].m_hostAddress = pConfig->m_hostAddress;
            upstreamConfigs[pConfig->m_upstreamLaneId - 1U].m_port = pConfig->m_port;
        }

        for (unsigned i = 0U; i < pData->m_downstreamConfigurationCount; ++i)
        {
            const HermesDownstreamConfiguration* pConfig = pData->m_downstreamConfigurations[i];
            DownstreamProxy* pProxy = pThis->GetDownstreamProxy(pConfig->m_downstreamLaneId);
            if (!pProxy)
            {
                HermesNotificationData notification =
                {
                    eHERMES_CONFIGURATION_ERROR,
                    eHERMES_INFO,
                    {cINVALID_LANE, static_cast<uint32_t>(::strlen(cINVALID_LANE))}
                };
                ::SignalHermesConfigurationNotification(pThis->m_pHermes, sessionId, &notification);
                return;
            }
            downstreamConfigs[pConfig->m_downstreamLaneId - 1U].m_optionalClientAddress = pConfig->m_optionalClientAddress;
            downstreamConfigs[pConfig->m_downstreamLaneId - 1U].m_port = pConfig->m_port;
        }

        for (unsigned i = 0; i < cMAX_LANE_COUNT; ++i)
        {
            UpstreamProxy& upstreamProxy = *pThis->GetUpstreamProxy(upstreamConfigs[i].m_upstreamLaneId);
            std::string serverAddress(upstreamConfigs[i].m_hostAddress.m_pData, upstreamConfigs[i].m_hostAddress.m_size);
            upstreamProxy.SetConfiguration(serverAddress, upstreamConfigs[i].m_port, pThis->m_machineId);
            
            DownstreamProxy& downstreamProxy = *pThis->GetDownstreamProxy(downstreamConfigs[i].m_downstreamLaneId);
            std::string optionalClientAddress(downstreamConfigs[i].m_optionalClientAddress.m_pData, downstreamConfigs[i].m_optionalClientAddress.m_size);
            downstreamProxy.SetConfiguration(optionalClientAddress, downstreamConfigs[i].m_port, pThis->m_machineId);
        }

    }
};


struct LaneConfiguration
{
    unsigned m_laneId;
    std::string m_ipAddress;
    unsigned short m_port;
};


struct Configuration
{
    std::string m_machineId;
    std::vector<LaneConfiguration> m_upstreamConfigurations;
    std::vector<LaneConfiguration> m_downstreamConfigurations;
};


class ConfigurationClient
{
public:

    static Configuration GetConfiguration(bool* pSuccess)
    {
        Configuration currentConfiguration;
        bool success = true;
        HermesGetConfigurationCallbacks callbacks = {
            {&ConfigurationClient::OnCurrentConfiguration_, &currentConfiguration},
            {&ConfigurationClient::OnError_, &success},
            {&ConfigurationClient::OnTrace_, nullptr}
        };
        HermesStringView hostName = {sLocalHost.data(), static_cast<uint32_t>(sLocalHost.size())};
        ::GetHermesConfiguration(hostName, 60U, &callbacks);
        if (pSuccess)
        {
            *pSuccess = success;
        }
        return currentConfiguration;
    }

    static Configuration SetConfiguration(const Configuration& configuration, bool* pSuccess)
    {
        Configuration resultingConfiguration;
        bool success = true;
        HermesSetConfigurationCallbacks callbacks = {
            {&ConfigurationClient::OnCurrentConfiguration_, &resultingConfiguration},
            {&ConfigurationClient::OnError_, &success},
            {0, 0}, // notification callback
            {&ConfigurationClient::OnTrace_, nullptr}
        };

        HermesSetConfigurationData data = {configuration.m_machineId.data(), static_cast<uint32_t>(configuration.m_machineId.size())};

        std::size_t upstreamCount = configuration.m_upstreamConfigurations.size();
        std::vector<HermesUpstreamConfiguration> upstreamConfigs(upstreamCount);
        std::vector<const HermesUpstreamConfiguration*> upstreamPointers(upstreamCount);
        data.m_upstreamConfigurationCount = static_cast<uint32_t>(upstreamCount);
        data.m_upstreamConfigurations = upstreamPointers.data();
        for (std::size_t i = 0U; i < upstreamCount; ++i)
        {
            const LaneConfiguration& myConfig = configuration.m_upstreamConfigurations[i];
            HermesUpstreamConfiguration upstreamConfig = {myConfig.m_laneId, 
                {myConfig.m_ipAddress.data(), static_cast<uint32_t>(myConfig.m_ipAddress.size())}, 
                myConfig.m_port
            };
            upstreamConfigs[i] = upstreamConfig;
            upstreamPointers[i] = &upstreamConfigs[i];
        }

        std::size_t downstreamCount = configuration.m_downstreamConfigurations.size();
        std::vector<HermesDownstreamConfiguration> downstreamConfigs(downstreamCount);
        std::vector<const HermesDownstreamConfiguration*> downstreamPointers(downstreamCount);
        data.m_downstreamConfigurationCount = static_cast<uint32_t>(downstreamCount);
        data.m_downstreamConfigurations = downstreamPointers.data();
        for (std::size_t i = 0U; i < downstreamCount; ++i)
        {
            const LaneConfiguration& myConfig = configuration.m_downstreamConfigurations[i];
            HermesDownstreamConfiguration downstreamConfig = {myConfig.m_laneId, {0, 0}, myConfig.m_port};
            if (!myConfig.m_ipAddress.empty())
            {
                downstreamConfig.m_optionalClientAddress.m_pData = myConfig.m_ipAddress.data();
                downstreamConfig.m_optionalClientAddress.m_size = static_cast<uint32_t>(myConfig.m_ipAddress.size());
            }
            downstreamConfigs[i] = downstreamConfig;
            downstreamPointers[i] = &downstreamConfigs[i];
        }

        HermesStringView hostName = {sLocalHost.data(), static_cast<uint32_t>(sLocalHost.size())};
        ::SetHermesConfiguration(hostName, &data, 60U, &callbacks);
        if (pSuccess)
        {
            *pSuccess = success;
        }
        return resultingConfiguration;
    }

private:
    static void OnTrace_(void*, unsigned sessionId, EHermesTraceType type, const HermesStringView trace)
    {
        Trace("ConfigurationClient", sessionId, type, trace);
    }

    static void OnError_(void* pBoolAsVoid, const HermesError*)
    {
        *static_cast<bool*>(pBoolAsVoid) = false;
    }

    static void OnCurrentConfiguration_(void* pConfigurationAsVoid, unsigned, const HermesCurrentConfigurationData* pData)
    {
        Configuration* pConfiguration = static_cast<Configuration*>(pConfigurationAsVoid);
        pConfiguration->m_machineId.assign(pData->m_optionalMachineId.m_pData, pData->m_optionalMachineId.m_size);
        pConfiguration->m_upstreamConfigurations.clear();
        pConfiguration->m_downstreamConfigurations.clear();
        for (unsigned i = 0; i < pData->m_downstreamConfigurationCount; ++i)
        {
            const HermesDownstreamConfiguration* pConfig = pData->m_downstreamConfigurations[i];
            LaneConfiguration laneConfig =
            {
                pConfig->m_downstreamLaneId,
                std::string(pConfig->m_optionalClientAddress.m_pData, pConfig->m_optionalClientAddress.m_size),
                pConfig->m_port
            };
            pConfiguration->m_downstreamConfigurations.push_back(laneConfig);
        }
        for (unsigned i = 0; i < pData->m_upstreamConfigurationCount; ++i)
        {
            const HermesUpstreamConfiguration* pConfig = pData->m_upstreamConfigurations[i];
            LaneConfiguration laneConfig =
            {
                pConfig->m_upstreamLaneId,
                std::string(pConfig->m_hostAddress.m_pData, pConfig->m_hostAddress.m_size),
                pConfig->m_port
            };
            pConfiguration->m_upstreamConfigurations.push_back(laneConfig);
        }
    }

};


#ifdef BOOST_AUTO_TEST_CASE
BOOST_AUTO_TEST_CASE(TestHermesCApi)
#else
int main()
#endif
{
    ConfigurationProxy proxy;


    // retrieve configuration which should be empty:
    bool success = false;
    Configuration currentConfiguration = ConfigurationClient::GetConfiguration(&success);
    CHECK_EQUAL(success, true);
    CHECK_EQUAL(currentConfiguration.m_machineId , std::string());
    CHECK_EQUAL(currentConfiguration.m_upstreamConfigurations.size(), 0U);
    CHECK_EQUAL(currentConfiguration.m_downstreamConfigurations.size(), 0U);

    // set configuration:
    currentConfiguration.m_machineId = "MyMachineId";
    for (unsigned short i = 0U; i < cMAX_LANE_COUNT; ++i)
    {
        LaneConfiguration laneConfig = {i + 1U, std::string(), 50501U + i};
        currentConfiguration.m_downstreamConfigurations.push_back(laneConfig);
        laneConfig.m_ipAddress = sLocalHost;
        currentConfiguration.m_upstreamConfigurations.push_back(laneConfig);
    }

    Configuration resultingConfiguration = ConfigurationClient::SetConfiguration(currentConfiguration, &success);
    CHECK_EQUAL(success, true);
    CHECK_EQUAL(resultingConfiguration.m_machineId, currentConfiguration.m_machineId);
    for (unsigned i = 0U; i < cMAX_LANE_COUNT; ++i)
    {
        const LaneConfiguration& upResult = resultingConfiguration.m_upstreamConfigurations[i];
        const LaneConfiguration& downResult = resultingConfiguration.m_downstreamConfigurations[i];
        const LaneConfiguration& upSetting = currentConfiguration.m_upstreamConfigurations[i];
        const LaneConfiguration& downSetting = currentConfiguration.m_downstreamConfigurations[i];

        CHECK_EQUAL(upResult.m_laneId, upSetting.m_laneId);
        CHECK_EQUAL(upResult.m_ipAddress, upSetting.m_ipAddress);
        CHECK_EQUAL(upResult.m_port, upSetting.m_port);
        CHECK_EQUAL(downResult.m_laneId, downSetting.m_laneId);
        CHECK_EQUAL(downResult.m_ipAddress, downSetting.m_ipAddress);
        CHECK_EQUAL(downResult.m_port, downSetting.m_port);
    }

    // wait until the handover of one board has completed:
    for (unsigned i = 0U; i < cMAX_LANE_COUNT; ++i)
    {
        DownstreamProxy& downstreamProxy = *proxy.GetDownstreamProxy(i + 1U);
        UpstreamProxy& upstreamProxy = *proxy.GetUpstreamProxy(i + 1U);

        downstreamProxy.WaitForCompletion();
        upstreamProxy.WaitForCompletion();

        CHECK_EQUAL(upstreamProxy.m_boardId, downstreamProxy.m_boardId);
        CHECK_EQUAL(upstreamProxy.m_boardIdCreatedBy, downstreamProxy.m_machineId);
        CHECK_EQUAL(upstreamProxy.m_width, downstreamProxy.m_width);
        CHECK_EQUAL(upstreamProxy.m_bottomBarcode, downstreamProxy.m_bottomBarcode);
    }

#ifndef BOOST_AUTO_TEST_CASE
return 0;
#endif
}


