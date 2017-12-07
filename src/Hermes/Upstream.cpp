// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include <HermesDataConversion.hpp>
#include "ApiCallback.h"
#include "Service.h"
#include "UpstreamSession.h"

#include <cassert>
#include <memory>

using namespace Hermes;
using namespace Hermes::Upstream;

struct HermesUpstream : ISessionCallback
{
    unsigned m_laneId = 0U;
    Service m_service;
    asio::deadline_timer m_timer{m_service.GetUnderlyingService()};
    UpstreamSettings m_configuration;

    unsigned m_sessionId{0U};
    unsigned m_connectedSessionId{0U};

    ApiCallback<HermesConnectedCallback> m_connectedCallback;
    ApiCallback<HermesServiceDescriptionCallback> m_serviceDescriptionCallback;
    ApiCallback<HermesBoardAvailableCallback> m_boardAvailableCallback;
    ApiCallback<HermesRevokeBoardAvailableCallback> m_revokeBoardAvailableCallback;
    ApiCallback<HermesTransportFinishedCallback> m_transportFinishedCallback;
    ApiCallback<HermesNotificationCallback> m_notificationCallback;
    ApiCallback<HermesCheckAliveCallback> m_checkAliveCallback;
    ApiCallback<HermesStateCallback> m_stateCallback;
    ApiCallback<HermesDisconnectedCallback> m_disconnectedCallback;

    std::unique_ptr<Session> m_upSession;

    bool m_enabled{false};

    HermesUpstream(unsigned laneId, const HermesUpstreamCallbacks& callbacks) :
        m_laneId(laneId),
        m_service(callbacks.m_traceCallback),
        m_connectedCallback(callbacks.m_connectedCallback),
        m_serviceDescriptionCallback(callbacks.m_serviceDescriptionCallback),
        m_boardAvailableCallback(callbacks.m_boardAvailableCallback),
        m_revokeBoardAvailableCallback(callbacks.m_revokeBoardAvailableCallback),
        m_transportFinishedCallback(callbacks.m_transportFinishedCallback),
        m_notificationCallback(callbacks.m_notificationCallback),
        m_checkAliveCallback(callbacks.m_checkAliveCallback),
        m_stateCallback(callbacks.m_stateCallback),
        m_disconnectedCallback(callbacks.m_disconnectedCallback)
    {
        m_service.Inform(0U, "Created");
    }

    ~HermesUpstream()
    {
        m_service.Inform(0U, "Deleted");
    }

    void Enable(const UpstreamSettings& configuration)
    {
        m_service.Log(0U, "Enable(", configuration, "); m_enabled=", m_enabled, ", m_configuration=", m_configuration);

        if (m_enabled && configuration == m_configuration)
            return;

        m_enabled = true;
        RemoveSession_(NotificationData(ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION,
            ESeverity::eINFO, "ConfigurationChanged"));

        m_configuration = configuration;
        CreateNewSession_();
    }

    void Disable(const NotificationData& notificationData)
    {
        m_service.Log(0U, "Disable(", notificationData, "); m_enabled=", m_enabled);

        if (!m_enabled)
            return;

        m_enabled = false;
        RemoveSession_(notificationData);
    }

    template<class DataT>
    void Signal(unsigned sessionId, const DataT& data)
    {
        m_service.Log(sessionId, "Signal(", data, ')');

        auto pSession = Session_(sessionId);
        if (!pSession)
            return;

        pSession->Signal(data);
    }

    void Reset(const NotificationData& notificationData)
    {
        m_service.Log(0U, "Reset(", notificationData, ')');

        RemoveSession_(notificationData);
        CreateNewSession_();
    }

    void Stop()
    {
        m_service.Log(0U, "Stop()");

        NotificationData notificationData(ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "Upstream service stopped by application");
        RemoveSession_(notificationData);
        m_service.Stop();
    }

    //================= ISessionCallback =========================
    void OnSocketConnected(unsigned sessionId, EState state, const ConnectionInfo& connectionInfo) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        m_connectedSessionId = pSession->Id();
        auto apiConnectionInfo = ToC(connectionInfo);
        m_connectedCallback(pSession->Id(), ToC(state), &apiConnectionInfo);
    }

    void On(unsigned sessionId, EState state, const ServiceDescription& serviceDescription) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiServiceDescription = ToC(serviceDescription);
        m_serviceDescriptionCallback(pSession->Id(), ToC(state), &apiServiceDescription);
    }

    void On(unsigned sessionId, EState state, const BoardAvailableData& in_data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_boardAvailableCallback(sessionId, ToC(state), &apiData);
    }

    void On(unsigned sessionId, EState state, const RevokeBoardAvailableData& data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(data);
        m_revokeBoardAvailableCallback(sessionId, ToC(state), &apiData);
    }

    void On(unsigned sessionId, EState state, const TransportFinishedData& data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(data);
        m_transportFinishedCallback(sessionId, ToC(state), &apiData);
    }

    void On(unsigned sessionId, EState, const NotificationData& data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(data);
        m_notificationCallback(sessionId, &apiData);
    }

    void On(unsigned sessionId, EState, const CheckAliveData& data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(data);
        m_checkAliveCallback(sessionId, &apiData);
    }

    void OnState(unsigned sessionId, EState state) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        m_stateCallback(sessionId, ToC(state));
    }

    void OnDisconnected(unsigned sessionId, EState state, const Error& error) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        if (pSession->OptionalPeerServiceDescription())
        {
            m_service.Post([this]() { CreateNewSession_(); });
        }
        else
        {
            DelayCreateNewSession_();
        }

        m_upSession.reset();
        auto apiError = ToC(error);
        m_disconnectedCallback(sessionId, ToC(state), &apiError);
    }

    //============ internal impplementation ==============
    Session* Session_(unsigned id)
    {
        if (m_upSession && m_upSession->Id() == id)
            return m_upSession.get();

        m_service.Warn(id, "Session ID no longer valid; current session ID=", m_upSession ? m_upSession->Id() : 0U);
        return nullptr;
    }

    void RemoveSession_(const NotificationData& notification)
    {
        if (!m_upSession)
            return;

        auto sessionId = m_upSession->Id();
        m_service.Log(sessionId, "Disconnect_(", notification, ')');

        m_upSession->Disconnect(notification);
        m_upSession.reset();

        if (sessionId != m_connectedSessionId)
            return;

        m_connectedSessionId = 0U;
        Error error;
        auto apiError = ToC(error);
        m_disconnectedCallback(sessionId, eHERMES_DISCONNECTED, &apiError);

    }

    void CreateNewSession_()
    {
        if (m_upSession)
            return;

        if (!m_enabled)
        {
            m_service.Log(0U, "CreateNewSession_, but disabled");
            return;
        }

        if (!++m_sessionId) { ++m_sessionId; } // avoid zero sessionId

        m_upSession = std::make_unique<Session>(m_sessionId, m_service, m_configuration);
        m_upSession->Connect(*this);
    }

    void DelayCreateNewSession_()
    {
        m_service.Log(0U, "DelayCreateNewSession_");

        m_timer.expires_from_now(boost::posix_time::seconds(m_configuration.m_reconnectWaitTimeInSeconds));
        m_timer.async_wait([this](const boost::system::error_code& ec)
        {
            if (ec) // timer cancelled or whatever
                return;
            CreateNewSession_();
        });
    }
};

//===================== implementation of public C functions =====================

HermesUpstream* CreateHermesUpstream(uint32_t laneId, const HermesUpstreamCallbacks* pCallbacks)
{
    return new HermesUpstream(laneId, *pCallbacks);
}

void RunHermesUpstream(HermesUpstream* pUpstream)
{
    pUpstream->m_service.Log(0U, "RunHermesUpstream");

    pUpstream->m_service.Run();
}

void PostHermesUpstream(HermesUpstream* pUpstream, HermesVoidCallback voidCallback)
{
    pUpstream->m_service.Log(0U, "PostHermesUpstream");

    pUpstream->m_service.Post([voidCallback]
    {
        voidCallback.m_pCall(voidCallback.m_pData);
    });
}

void EnableHermesUpstream(HermesUpstream* pUpstream, const HermesUpstreamSettings* pSettings)
{
    pUpstream->m_service.Log(0U, "EnableHermesUpstream");
    auto settings = ToCpp(*pSettings);
    if (!settings.m_port)
    {
        settings.m_port = static_cast<uint16_t>(cBASE_PORT + pUpstream->m_laneId);
    }

    pUpstream->m_service.Post([pUpstream, settings = std::move(settings)]()
    {
        pUpstream->Enable(settings);
    });
}

void SignalHermesUpstreamServiceDescription(HermesUpstream* pUpstream, uint32_t sessionId, const HermesServiceDescription* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesUpstreamServiceDescription");
    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data);
    });
}

void SignalHermesMachineReady(HermesUpstream* pUpstream, uint32_t sessionId, const HermesMachineReadyData* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesMachineReady");
    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data);
    });
}

void SignalHermesRevokeMachineReady(HermesUpstream* pUpstream, uint32_t sessionId, const HermesRevokeMachineReadyData* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesRevokeMachineReady");
    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data);
    });
}

void SignalHermesStartTransport(HermesUpstream* pUpstream, uint32_t sessionId, const HermesStartTransportData* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesStartTransport");

    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data);
    });
}

void SignalHermesStopTransport(HermesUpstream* pUpstream, uint32_t sessionId, const HermesStopTransportData* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesStopTransport");

    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data);
    });
}

void SignalHermesUpstreamNotification(HermesUpstream* pUpstream, uint32_t sessionId, const HermesNotificationData* pData)
{
    pUpstream->m_service.Log(0U, "SignalHermesUpstreamNotification");

    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data);
    });
}

void SignalHermesUpstreamCheckAlive(HermesUpstream* pUpstream, uint32_t sessionId, const HermesCheckAliveData* pData)
{
    pUpstream->m_service.Log(0U, "SignalHermesUpstreamCheckAlive");

    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data);
    });
}

void ResetHermesUpstream(HermesUpstream* pUpstream, const HermesNotificationData* pData)
{
    pUpstream->m_service.Log(0U, "ResetHermesUpstream");

    pUpstream->m_service.Post([pUpstream, data = ToCpp(*pData)]()
    {
        pUpstream->Reset(data);
    });
}

void DisableHermesUpstream(HermesUpstream* pUpstream, const HermesNotificationData* pData)
{
    pUpstream->m_service.Log(0U, "DisableHermesUpstream");

    pUpstream->m_service.Post([pUpstream, data = ToCpp(*pData)]()
    {
        pUpstream->Disable(data);
    });
}

void StopHermesUpstream(HermesUpstream* pUpstream)
{
    pUpstream->m_service.Log(0U, "StopHermesUpstream");

    pUpstream->m_service.Post([pUpstream]()
    {
        pUpstream->Stop();
    });
}

void DeleteHermesUpstream(HermesUpstream* pUpstream)
{
    if (!pUpstream)
        return;

    pUpstream->m_service.Log(0U, "DeleteHermesUpstream");

    pUpstream->m_service.Stop();
    delete pUpstream;
}
