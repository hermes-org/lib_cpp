// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"
#include <HermesData.hpp>

#include "ApiCallback.h"
#include "ConfigurationServiceSession.h"
#include "Network.h"
#include "Service.h"

#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace
{
    template<class CallbackT>
    class CallbackScope
    {
    public:
        CallbackScope(CallbackT*& pCallbackHolder, CallbackT& callback) :
            m_pHolder(pCallbackHolder)
        {
            m_pHolder = &callback;
        }

        ~CallbackScope()
        {
            m_pHolder = nullptr;
        }

    private:
        CallbackT*& m_pHolder;
    };
}

using namespace Hermes;

struct HermesConfigurationService : IAcceptorCallback, IConfigurationServiceSessionCallback
{
    Service m_service;
    ConfigurationServiceSettings m_settings;;

    // we only hold on to the accepting session
    std::unique_ptr<IAcceptor> m_upAcceptor{CreateAcceptor(m_service, *this)};
    std::map<unsigned, ConfigurationServiceSession> m_sessionMap;

    ApiCallback<HermesConnectedCallback> m_connectedCallback;
    ApiCallback<HermesSetConfigurationCallback> m_setConfigurationCallback;
    ApiCallback<HermesGetConfigurationCallback> m_getConfigurationCallback;
    ApiCallback<HermesDisconnectedCallback> m_disconnectedCallback;

    ISetConfigurationResponse* m_pSetConfigurationResponder = nullptr; // temporarily stored during the callback
    IGetConfigurationResponse* m_pGetConfigurationResponder = nullptr; // temporarily stored during the callback

    bool m_enabled{false};

    HermesConfigurationService(const HermesConfigurationServiceCallbacks& callbacks) :
        m_service(callbacks.m_traceCallback),
        m_connectedCallback(callbacks.m_connectedCallback),
        m_setConfigurationCallback(callbacks.m_setConfigurationCallback),
        m_getConfigurationCallback(callbacks.m_getConfigurationCallback),
        m_disconnectedCallback(callbacks.m_disconnectedCallback)
    {
        m_service.Inform(0U, "Created");
    }

    ~HermesConfigurationService()
    {
        m_service.Inform(0U, "Deleted");
    }

    //================= forwarding calls =========================
    void Enable(const ConfigurationServiceSettings& settings)
    {
        m_service.Log(0U, "Enable(", settings, "); m_enabled=", m_enabled, ", m_settings=", m_settings);

        if (m_enabled && m_settings == settings)
            return;

        m_enabled = true;
        m_settings = settings;

        NetworkConfiguration networkConfiguration;
        networkConfiguration.m_port = settings.m_port ? settings.m_port : cCONFIG_PORT;
        networkConfiguration.m_retryDelayInSeconds = settings.m_reconnectWaitTimeInSeconds;
        networkConfiguration.m_checkAlivePeriodInSeconds = 0U;

        m_upAcceptor->StartListening(networkConfiguration);
    }

    void Disable(const NotificationData& data)
    {
        m_service.Log(0U, "Disable(", data, "); m_enabled=", m_enabled);

        if (!m_enabled)
            return;

        m_enabled = false;
        m_upAcceptor->StopListening();
        RemoveSessions_(NotificationData(ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION, ESeverity::eINFO,
            "Configuration service disabled"));
    }

    void Stop()
    {
        m_service.Log(0U, "Stop(), sessionCount=", m_sessionMap.size());

        NotificationData notificationData(ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "Configuration service stopped by application");
        RemoveSessions_(notificationData);
        m_service.Stop();
    }

    void RemoveSessions_(const NotificationData& data)
    {
        for (auto& entry : m_sessionMap)
        {
            entry.second.Disconnect(data);
        }
        m_sessionMap.clear();
    }

    //================= IAcceptorCallback =========================
    void OnAccepted(std::unique_ptr<IServerSocket>&& upSocket) override
    {
        auto sessionId = upSocket->SessionId();
        m_service.Inform(sessionId, "OnAccepted: ", upSocket->GetConnectionInfo());
        auto result = m_sessionMap.try_emplace(sessionId, std::move(upSocket), m_service, m_settings);
        if (!result.second)
        {
            // should not really happen, unless someone launches a Denial of Service attack
            m_service.Warn(sessionId, "Duplicate session ID");
            return;
        }
        result.first->second.Connect(*this);
    }

    //================= IConfigurationServiceSessionCallback =========================
    void OnSocketConnected(unsigned sessionId, const ConnectionInfo& connectionInfo) override
    {
        auto apiConnectionInfo = ToC(connectionInfo);
        m_connectedCallback(sessionId, eHERMES_SOCKET_CONNECTED, &apiConnectionInfo);
    }

    virtual void OnGet(unsigned sessionId, const ConnectionInfo& connectionInfo,
        const GetConfigurationData& data, IGetConfigurationResponse& responder) override
    {
        HermesConnectionInfo apiConnectionInfo = ToC(connectionInfo);
        HermesGetConfigurationData apiData = ToC(data);

        CallbackScope<IGetConfigurationResponse> scope(m_pGetConfigurationResponder, responder);
        m_getConfigurationCallback(sessionId, &apiConnectionInfo, &apiData);
    }

    virtual void OnSet(unsigned sessionId, const ConnectionInfo& connectionInfo,
        const SetConfigurationData& configuration, ISetConfigurationResponse& responder) override
    {
        auto apiConnectionInfo = ToC(connectionInfo);
        auto apiConfiguration = ToC(configuration);

        CallbackScope<ISetConfigurationResponse> scope(m_pSetConfigurationResponder, responder);
        m_setConfigurationCallback(sessionId, &apiConnectionInfo, &apiConfiguration);
    }

    virtual void OnDisconnected(unsigned sessionId, const Error& error) override
    {
        // only notify if this was signalled as connected:
        if (!m_sessionMap.erase(sessionId))
            return;

        auto apiError = ToC(error);
        m_disconnectedCallback(sessionId, eHERMES_DISCONNECTED, &apiError);
    }
};

HermesConfigurationService* CreateHermesConfigurationService(const HermesConfigurationServiceCallbacks* pCallbacks)
{
    return new HermesConfigurationService(*pCallbacks);
}

void RunHermesConfigurationService(HermesConfigurationService* pConfigurationService)
{
    pConfigurationService->m_service.Log(0U, "RunHermesConfigurationService");

    pConfigurationService->m_service.Run();
}

void PostHermesConfigurationService(HermesConfigurationService* pConfigurationService, HermesVoidCallback voidCallback)
{
    pConfigurationService->m_service.Log(0U, "EnableHermesDownstream");

    pConfigurationService->m_service.Post([voidCallback]
    {
        voidCallback.m_pCall(voidCallback.m_pData);
    });
}

void EnableHermesConfigurationService(HermesConfigurationService* pConfigurationService,
    const HermesConfigurationServiceSettings* pSettings)
{
    pConfigurationService->m_service.Log(0U, "EnableHermesConfigurationService");

    pConfigurationService->m_service.Post([pConfigurationService,
        settings = ToCpp(*pSettings)]()
    {
        pConfigurationService->Enable(settings);
    });
}

void SignalHermesCurrentConfiguration(HermesConfigurationService* pConfigurationService, uint32_t sessionId, 
    const HermesCurrentConfigurationData* pConfiguration)
{
    pConfigurationService->m_service.Log(sessionId, "SignalHermesCurrentConfiguration");

    auto* pResponder = pConfigurationService->m_pGetConfigurationResponder;
    assert(pResponder->Id() == sessionId);
    if (!pResponder)
        return;

    if (pResponder->Id() != sessionId)
        return;

    pResponder->Signal(ToCpp(*pConfiguration));
}

void SignalHermesConfigurationNotification(HermesConfigurationService* pConfigurationService, uint32_t sessionId, 
    const HermesNotificationData* pNotification)
{
    pConfigurationService->m_service.Log(sessionId, "SignalHermesConfigurationNotification");

    auto* pResponder = pConfigurationService->m_pSetConfigurationResponder;
    assert(pResponder->Id() == sessionId);
    if (!pResponder)
        return;

    if (pResponder->Id() != sessionId)
        return;

    pResponder->Signal(ToCpp(*pNotification));
}

void DisableHermesConfigurationService(HermesConfigurationService* pConfigurationService, const HermesNotificationData* pData)
{
    pConfigurationService->m_service.Log(0U, "DisableHermesConfigurationService");

    pConfigurationService->m_service.Post([pConfigurationService, data = ToCpp(*pData)]()
    {
        pConfigurationService->Disable(data);
    });
}

void StopHermesConfigurationService(HermesConfigurationService* pConfigurationService)
{
    pConfigurationService->m_service.Log(0U, "StopHermesConfigurationService");

    pConfigurationService->m_service.Post([pConfigurationService]()
    {
        pConfigurationService->Stop();
    });
}

void DeleteHermesConfigurationService(HermesConfigurationService* pConfigurationService)
{
    pConfigurationService->m_service.Log(0U, "DeleteHermesConfigurationService");

    pConfigurationService->m_service.Stop();
    delete pConfigurationService;
}
