/***********************************************************************
Copyright 2018 ASM Assembly Systems GmbH & Co. KG

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

// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"
#include <HermesData.hpp>

#include "ApiCallback.h"
#include "VerticalServiceSession.h"
#include "Network.h"
#include "Service.h"

#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

using namespace Hermes;
using namespace Hermes::VerticalService;

struct HermesVerticalService : IAcceptorCallback, ISessionCallback
{
    Service m_service;
    VerticalServiceSettings m_settings;;

    // we only hold on to the accepting session
    std::unique_ptr<IAcceptor> m_upAcceptor{ CreateAcceptor(m_service, *this) };
    std::map<unsigned, Session> m_sessionMap;

    ApiCallback<HermesVerticalConnectedCallback> m_connectedCallback;
    ApiCallback<HermesSupervisoryServiceDescriptionCallback> m_serviceDescriptionCallback;
    ApiCallback<HermesSendWorkOrderInfoCallback> m_sendWorkOrderInfoCallback;
    ApiCallback<HermesNotificationCallback> m_notificationCallback;
    ApiCallback<HermesCheckAliveCallback> m_checkAliveCallback;
    ApiCallback<HermesSetConfigurationCallback> m_setConfigurationCallback;
    ApiCallback<HermesGetConfigurationCallback> m_getConfigurationCallback;
    ApiCallback<HermesVerticalDisconnectedCallback> m_disconnectedCallback;

    bool m_enabled{ false };

    HermesVerticalService(const HermesVerticalServiceCallbacks& callbacks) :
        m_service(callbacks.m_traceCallback),
        m_connectedCallback(callbacks.m_connectedCallback),
        m_serviceDescriptionCallback(callbacks.m_serviceDescriptionCallback),
        m_sendWorkOrderInfoCallback(callbacks.m_sendWorkOrderInfoCallback),
        m_notificationCallback(callbacks.m_notificationCallback),
        m_checkAliveCallback(callbacks.m_checkAliveCallback),
        m_setConfigurationCallback(callbacks.m_setConfigurationCallback),
        m_getConfigurationCallback(callbacks.m_getConfigurationCallback),
        m_disconnectedCallback(callbacks.m_disconnectedCallback)
    {
        m_service.Inform(0U, "Created");
    }

    virtual ~HermesVerticalService()
    {
        m_service.Inform(0U, "Deleted");
    }

    //================= forwarding calls =========================
    void Enable(const VerticalServiceSettings& settings)
    {
        m_service.Log(0U, "Enable(", settings, "); m_enabled=", m_enabled, ", m_settings=", m_settings);

        if (m_enabled && m_settings == settings)
            return;

        RemoveSessions_(NotificationData(ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION,
            ESeverity::eINFO, "ConfigurationChanged"));

        m_enabled = true;
        m_settings = settings;

        NetworkConfiguration networkConfiguration;
        networkConfiguration.m_port = settings.m_port ? settings.m_port : cCONFIG_PORT;
        networkConfiguration.m_retryDelayInSeconds = settings.m_reconnectWaitTimeInSeconds;
        networkConfiguration.m_checkAlivePeriodInSeconds = settings.m_checkAlivePeriodInSeconds;

        m_upAcceptor->StartListening(networkConfiguration);
    }

    void Disable(const NotificationData& data)
    {
        m_service.Log(0U, "Disable(", data, "); m_enabled=", m_enabled);

        if (!m_enabled)
            return;

        m_enabled = false;
        m_upAcceptor->StopListening();
        RemoveSessions_(data);
    }

    void Stop()
    {
        m_service.Log(0U, "Stop(), sessionCount=", m_sessionMap.size());

        NotificationData notificationData(ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "Vertical service stopped by application");
        RemoveSessions_(notificationData);
        m_service.Stop();
    }

    void RemoveSessions_(const NotificationData& data)
    {
        auto sessionMap = std::move(m_sessionMap);
        m_sessionMap.clear();

        Error error{};
        auto apiError = ToC(error);
        for (auto& entry : sessionMap)
        {
            entry.second.Signal(data);
            entry.second.Disconnect();
            m_disconnectedCallback(entry.second.Id(), eHERMES_VERTICAL_STATE_DISCONNECTED, &apiError);
        }
    }

    void RemoveSession_(unsigned sessionId, const NotificationData& data)
    {
        auto itFound = m_sessionMap.find(sessionId);
        if (itFound == m_sessionMap.end())
            return;

        auto session = std::move(itFound->second);
        m_sessionMap.erase(itFound);

        session.Signal(data);
        session.Disconnect();
        Error error{};
        auto apiError = ToC(error);
        m_disconnectedCallback(session.Id(), eHERMES_VERTICAL_STATE_DISCONNECTED, &apiError);
    }

    Session* Session_(unsigned id)
    {
        auto itFound = m_sessionMap.find(id);
        if (itFound == m_sessionMap.end())
        {
            m_service.Warn(id, "Session ID no longer valid");
            return nullptr;
        }

        return &itFound->second;
    }

    template<class DataT>
    void Signal_(unsigned sessionId, const DataT& data)
    {
        m_service.Log(sessionId, "Signal(", data, ')');

        auto pSession = Session_(sessionId);
        if (!pSession)
            return m_service.Log(sessionId, "No matching session to signal to");

        pSession->Signal(data);
    }

    template<class DataT>
    void SignalTrackingData_(const DataT& data)
    {
        m_service.Log(0U, "Signal(", data, ')');

        for (auto& entry : m_sessionMap)
        {
            auto& session = entry.second;
            if (!session.OptionalPeerServiceDescriptionData() ||
                !session.OptionalPeerServiceDescriptionData()->m_supportedFeatures.m_optionalFeatureBoardTracking)
                continue;
            session.Signal(data);
        }
    }

    //================= IAcceptorCallback =========================
    void OnAccepted(std::unique_ptr<IServerSocket>&& upSocket) override
    {
        auto sessionId = upSocket->SessionId();
        m_service.Inform(sessionId, "OnAccepted: ", upSocket->GetConnectionInfo());
#ifdef _WINDOWS
        auto result = m_sessionMap.try_emplace(sessionId, std::move(upSocket), m_service, m_settings);
#else
        auto result = m_sessionMap.emplace(sessionId, VerticalService::Session(std::move(upSocket), m_service, m_settings));
#endif
        if (!result.second)
        {
            // should not really happen, unless someone launches a Denial of Service attack
            m_service.Warn(sessionId, "Duplicate session ID");
            return;
        }
        result.first->second.Connect(*this);
    }

    //================= VerticalService::ISessionCallback =========================
    void OnSocketConnected(unsigned sessionId, EVerticalState state, const ConnectionInfo& connectionInfo) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiConnectionInfo = ToC(connectionInfo);
        m_connectedCallback(sessionId, ToC(state), &apiConnectionInfo);
    }

    void On(unsigned sessionId, EVerticalState state, const SupervisoryServiceDescriptionData& data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(data);
        m_serviceDescriptionCallback(sessionId, ToC(state), &apiData);
    }

    void On(unsigned sessionId, EVerticalState, const GetConfigurationData& data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(data);
        auto apiConnectionInfo = ToC(pSession->PeerConnectionInfo());
        m_getConfigurationCallback(sessionId, &apiData, &apiConnectionInfo);
    }

    void On(unsigned sessionId, EVerticalState, const SetConfigurationData& data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(data);
        auto apiConnectionInfo = ToC(pSession->PeerConnectionInfo());
        m_setConfigurationCallback(sessionId, &apiData, &apiConnectionInfo);
    }

    void On(unsigned sessionId, EVerticalState, const SendWorkOrderInfoData& data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(data);
        m_sendWorkOrderInfoCallback(sessionId, &apiData);
    }

    void On(unsigned sessionId, EVerticalState, const NotificationData& data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(data); 
        m_notificationCallback(sessionId, &apiData);
    }

    void On(unsigned sessionId, EVerticalState, const CheckAliveData& in_data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        if (in_data.m_optionalType
            && *in_data.m_optionalType == ECheckAliveType::ePING
            && m_settings.m_checkAliveResponseMode == ECheckAliveResponseMode::eAUTO)
        {
            CheckAliveData data{ in_data };
            data.m_optionalType = ECheckAliveType::ePONG;
            m_service.Post([this, sessionId, data = std::move(data)]() { Signal_(sessionId, data); });
        }
        auto apiData = ToC(in_data);
        m_checkAliveCallback(sessionId, &apiData);
    }

    void OnDisconnected(unsigned sessionId, EVerticalState state, const Error& error) override
    {
        // only notify if this was signalled as connected:
        if (!m_sessionMap.erase(sessionId))
            return;

        auto apiError = ToC(error);
        m_disconnectedCallback(sessionId, ToC(state), &apiError);
    }
};

HermesVerticalService* CreateHermesVerticalService(const HermesVerticalServiceCallbacks* pCallbacks)
{
    return new HermesVerticalService(*pCallbacks);
}

void RunHermesVerticalService(HermesVerticalService* pVerticalService)
{
    pVerticalService->m_service.Log(0U, "RunHermesVerticalService");

    pVerticalService->m_service.Run();
}

void PostHermesVerticalService(HermesVerticalService* pVerticalService, HermesVoidCallback voidCallback)
{
    pVerticalService->m_service.Log(0U, "EnableHermesDownstream");

    pVerticalService->m_service.Post([voidCallback]
    {
        voidCallback.m_pCall(voidCallback.m_pData);
    });
}

void EnableHermesVerticalService(HermesVerticalService* pVerticalService,
    const HermesVerticalServiceSettings* pSettings)
{
    pVerticalService->m_service.Log(0U, "EnableHermesVerticalService");

    pVerticalService->m_service.Post([pVerticalService,
        settings = ToCpp(*pSettings)]()
    {
        pVerticalService->Enable(settings);
    });
}

void SignalHermesVerticalServiceDescription(HermesVerticalService* pVerticalService, uint32_t sessionId,
    const HermesSupervisoryServiceDescriptionData* pData)
{
    pVerticalService->m_service.Log(sessionId, "SignalHermesVerticalServiceDescription");
    pVerticalService->m_service.Post([pVerticalService, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalService->Signal_(sessionId, data);
    });
}

void SignalHermesBoardArrived(HermesVerticalService* pVerticalService, uint32_t sessionId,
    const HermesBoardArrivedData* pData)
{
    pVerticalService->m_service.Log(sessionId, "SignalHermesBoardArrived");
    pVerticalService->m_service.Post([pVerticalService, sessionId, data = ToCpp(*pData)]()
    {
        if (sessionId == 0U)
        {
            pVerticalService->SignalTrackingData_(data);
        }
        else
        {
            pVerticalService->Signal_(sessionId, data);
        }
    });
}

void SignalHermesBoardDeparted(HermesVerticalService* pVerticalService, uint32_t sessionId,
    const HermesBoardDepartedData* pData)
{
    pVerticalService->m_service.Log(sessionId, "SignalHermesBoardDeparted");
    pVerticalService->m_service.Post([pVerticalService, sessionId, data = ToCpp(*pData)]()
    {
        if (sessionId == 0U)
        {
            pVerticalService->SignalTrackingData_(data);
        }
        else
        {
            pVerticalService->Signal_(sessionId, data);
        }
    });
}

void ResetHermesVerticalServiceSession(HermesVerticalService* pVerticalService, uint32_t sessionId, 
    const HermesNotificationData* pData)
{
    pVerticalService->m_service.Log(sessionId, "ResetHermesVerticalServiceSession");
    pVerticalService->m_service.Post([pVerticalService, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalService->RemoveSession_(sessionId, data);
    });
}

void SignalHermesQueryWorkOrderInfo(HermesVerticalService* pVerticalService, uint32_t sessionId,
    const HermesQueryWorkOrderInfoData* pData)
{
    pVerticalService->m_service.Log(sessionId, "SignalHermesQueryWorkOrderInfo");
    pVerticalService->m_service.Post([pVerticalService, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalService->Signal_(sessionId, data);
    });
}

void SignalHermesVerticalServiceNotification(HermesVerticalService* pVerticalService, uint32_t sessionId,
    const HermesNotificationData* pData)
{
    pVerticalService->m_service.Log(sessionId, "SignalHermesVerticalServiceNotification");
    pVerticalService->m_service.Post([pVerticalService, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalService->Signal_(sessionId, data);
    });
}

void SignalHermesVerticalServiceCheckAlive(HermesVerticalService* pVerticalService, uint32_t sessionId,
    const HermesCheckAliveData* pData)
{
    pVerticalService->m_service.Log(sessionId, "SignalHermesVerticalCheckAlive");
    pVerticalService->m_service.Post([pVerticalService, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalService->Signal_(sessionId, data);
    });
}

void SignalHermesVerticalCurrentConfiguration(HermesVerticalService* pVerticalService, uint32_t sessionId,
    const HermesCurrentConfigurationData* pData)
{
    pVerticalService->m_service.Log(sessionId, "SignalVerticalCurrentConfiguration");
    pVerticalService->m_service.Post([pVerticalService, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalService->Signal_(sessionId, data);
    });
}

void DisableHermesVerticalService(HermesVerticalService* pVerticalService, const HermesNotificationData* pData)
{
    pVerticalService->m_service.Log(0U, "DisableHermesVerticalService");

    pVerticalService->m_service.Post([pVerticalService, data = ToCpp(*pData)]()
    {
        pVerticalService->Disable(data);
    });
}

void StopHermesVerticalService(HermesVerticalService* pVerticalService)
{
    pVerticalService->m_service.Log(0U, "StopHermesVerticalService");

    pVerticalService->m_service.Post([pVerticalService]()
    {
        pVerticalService->Stop();
    });
}

void DeleteHermesVerticalService(HermesVerticalService* pVerticalService)
{
    pVerticalService->m_service.Log(0U, "DeleteHermesVerticalService");

    pVerticalService->m_service.Stop();
    delete pVerticalService;
}
