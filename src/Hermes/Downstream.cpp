// Copyright (c) ASM Assembly Systems GmbH & Co. KG
#include "stdafx.h"

#include <HermesDataConversion.hpp>
#include "ApiCallback.h"
#include "Network.h"
#include "DownstreamSession.h"
#include "Serializer.h"
#include "Service.h"
#include "StringBuilder.h"

#include <algorithm>
#include <memory>
#include <vector>

using namespace Hermes;
using namespace Hermes::Downstream;

struct HermesDownstream : IAcceptorCallback, ISessionCallback
{
    unsigned m_laneId = 0U;
    Service m_service;
    DownstreamSettings m_settings;

    std::unique_ptr<Session> m_upSession;
    std::unique_ptr<IAcceptor>  m_upAcceptor{CreateAcceptor(m_service, *this)};

    ApiCallback<HermesConnectedCallback> m_connectedCallback;
    ApiCallback<HermesServiceDescriptionCallback> m_serviceDescriptionCallback;
    ApiCallback<HermesMachineReadyCallback> m_machineReadyCallback;
    ApiCallback<HermesRevokeMachineReadyCallback> m_revokeMachineReadyCallback;
    ApiCallback<HermesStartTransportCallback> m_startTransportCallback;
    ApiCallback<HermesStopTransportCallback> m_stopTransportCallback;
    ApiCallback<HermesNotificationCallback> m_notificationCallback;
    ApiCallback<HermesStateCallback> m_stateCallback;
    ApiCallback<HermesCheckAliveCallback> m_checkAliveCallback;
    ApiCallback<HermesDisconnectedCallback> m_disconnectedCallback;

    bool m_enabled{false};

    HermesDownstream(unsigned laneId, const HermesDownstreamCallbacks& callbacks) :
        m_laneId(laneId),
        m_service(callbacks.m_traceCallback),
        m_connectedCallback(callbacks.m_connectedCallback),
        m_serviceDescriptionCallback(callbacks.m_serviceDescriptionCallback),
        m_machineReadyCallback(callbacks.m_machineReadyCallback),
        m_revokeMachineReadyCallback(callbacks.m_revokeMachineReadyCallback),
        m_startTransportCallback(callbacks.m_startTransportCallback),
        m_stopTransportCallback(callbacks.m_stopTransportCallback),
        m_notificationCallback(callbacks.m_notificationCallback),
        m_stateCallback(callbacks.m_stateCallback),
        m_checkAliveCallback(callbacks.m_checkAliveCallback),
        m_disconnectedCallback(callbacks.m_disconnectedCallback)
    {
        m_service.Inform(0U, "Created");
    }

    virtual ~HermesDownstream()
    {
        m_service.Inform(0U, "Deleted");
    }

    //================= forwarding calls =========================
    void Enable(const DownstreamSettings& settings)
    {
        m_service.Log(0U, "Enable(", settings, "); m_enabled=", m_enabled, ", m_settings=", m_settings);

        if (m_enabled && m_settings == settings)
            return;

        m_enabled = true;
        RemoveCurrentSession_(NotificationData(ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION,
            ESeverity::eINFO, "ConfigurationChanged"));

        m_settings = settings;
        NetworkConfiguration networkConfiguration;
        networkConfiguration.m_hostName = m_settings.m_optionalClientAddress;
        networkConfiguration.m_port = m_settings.m_port;
        networkConfiguration.m_checkAlivePeriodInSeconds = m_settings.m_checkAlivePeriodInSeconds;
        networkConfiguration.m_retryDelayInSeconds = m_settings.m_reconnectWaitTimeInSeconds;
        
        m_upAcceptor->StartListening(networkConfiguration);
    }

    template<class DataT>
    void Signal(unsigned sessionId, const DataT& data)
    {
        m_service.Log(sessionId, "Signal(", data, ')');

        auto pSession = Session_(sessionId);
        if (!pSession)
            return m_service.Log(sessionId, "No matching session");

        pSession->Signal(data);
    }

    void Reset(const NotificationData& notificationData)
    {
        m_service.Log(0U, "Reset(", notificationData, ')');
        RemoveCurrentSession_(notificationData);
    }

    void Disable(const NotificationData& notificationData)
    {
        m_service.Log(0U, "Disable(", notificationData, "); m_enabled=", m_enabled);

        if (!m_enabled)
            return;

        m_enabled = false;
        m_upAcceptor->StopListening();
        RemoveCurrentSession_(notificationData);
    }

    void Stop()
    {
        m_service.Log(0U, "Stop()");
        NotificationData notificationData(ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "Downstream service stopped by application");
        m_upAcceptor->StopListening();
        RemoveCurrentSession_(notificationData);
        m_service.Stop();
    }

    //================= IAcceptorCallback =========================
    void OnAccepted(std::unique_ptr<IServerSocket>&& upSocket) override
    {
        auto newSessionId = upSocket->SessionId();

        // refuse connection if we have an active one:
        if (m_upSession)
        {
            m_service.Warn(m_upSession->Id(), "Due to existing session, refusing to accept new session with id=", newSessionId);

            // assemble a notification
            const auto& connectionInfo = m_upSession->PeerConnectionInfo();
            std::ostringstream oss;
            oss << connectionInfo;
            NotificationData notification(ENotificationCode::eCONNECTION_REFUSED_BECAUSE_OF_ESTABLISHED_CONNECTION, ESeverity::eWARNING
                , oss.str());
            const auto& xmlString = Serialize(notification);
            upSocket->Send(xmlString);

            // send a check alive to the current connection to reduce time for timeout detection:
            m_upSession->Signal(CheckAliveData());
            return;
        }

        m_service.Log(newSessionId, "OnAccepted");
        m_upSession = std::make_unique<Session>(std::move(upSocket), m_service, m_settings);
        m_upSession->Connect(*this);
    }

    //================= ISessionCallback =========================
    void OnSocketConnected(unsigned sessionId, EState state, const ConnectionInfo& connectionInfo) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(connectionInfo);
        m_connectedCallback(sessionId, ToC(state), &apiData);
    }

    void On(unsigned sessionId, EState state, const ServiceDescription& in_data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_serviceDescriptionCallback(sessionId, ToC(state), &apiData);
    }

    void On(unsigned sessionId, EState state, const MachineReadyData& in_data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_machineReadyCallback(sessionId, ToC(state), &apiData);
    }

    void On(unsigned sessionId, EState state, const RevokeMachineReadyData& in_data) override
    {
        auto pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_revokeMachineReadyCallback(sessionId, ToC(state), &apiData);
    }

    void On(unsigned sessionId, EState state, const StartTransportData& in_data) override
    {
        auto pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_startTransportCallback(sessionId, ToC(state), &apiData);
    }

    void On(unsigned sessionId, EState state, const StopTransportData& in_data) override
    {
        auto pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_stopTransportCallback(sessionId, ToC(state), &apiData);
    }

    void On(unsigned sessionId, EState, const NotificationData& in_data) override
    {
        auto pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_notificationCallback(sessionId, &apiData);
    }

    void On(unsigned sessionId, EState, const CheckAliveData& in_data) override
    {
        auto pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_checkAliveCallback(sessionId, &apiData);
    }

    void OnState(unsigned sessionId, EState state) override
    {
        auto pSession = Session_(sessionId);
        if (!pSession)
            return;

        m_stateCallback(sessionId, ToC(state));
    }

    void OnDisconnected(unsigned sessionId, EState, const Error& error) override
    {
        auto pSession = Session_(sessionId);
        if (!pSession)
            return;
        
        m_upSession.reset();
        auto apiError = ToC(error);
        m_disconnectedCallback(sessionId, eHERMES_DISCONNECTED, &apiError);
    }

    //=================== internal =========================
    Session* Session_(unsigned id)
    {
        if (m_upSession && m_upSession->Id() == id)
            return m_upSession.get();

        m_service.Warn(id, "Session ID no longer valid; current session ID=", m_upSession ? m_upSession->Id() : 0U);
        return nullptr;
    }

    void RemoveCurrentSession_(const NotificationData& notificationData)
    {
        if (!m_upSession)
            return;

        m_upSession->Disconnect(notificationData);
        Error error;
        auto apiError = ToC(error);
        m_disconnectedCallback(m_upSession->Id(), eHERMES_DISCONNECTED, &apiError);
        m_upSession.reset();
    }

};

//===================== implementation of public C functions ====================
HermesDownstream* CreateHermesDownstream(uint32_t laneId, const HermesDownstreamCallbacks* pCallbacks)
{
    return new HermesDownstream(laneId, *pCallbacks);
}

void RunHermesDownstream(HermesDownstream* pDownstream)
{
    pDownstream->m_service.Log(0U, "RunHermesDownstream");

    pDownstream->m_service.Run();
}

void EnableHermesDownstream(HermesDownstream* pDownstream, const HermesDownstreamSettings* pSettings)
{
    pDownstream->m_service.Log(0U, "EnableHermesDownstream");
    auto settings = ToCpp(*pSettings);
    if (!settings.m_port)
    {
        settings.m_port = static_cast<uint16_t>(cBASE_PORT + pDownstream->m_laneId);
    }
    pDownstream->m_service.Post([pDownstream, settings = std::move(settings)]()
    {
        pDownstream->Enable(settings);
    });
}

void PostHermesDownstream(HermesDownstream* pDownstream, HermesVoidCallback voidCallback)
{
    pDownstream->m_service.Log(0U, "PostHermesDownstream");

    pDownstream->m_service.Post([voidCallback]
    {
        voidCallback.m_pCall(voidCallback.m_pData);
    });
}

void SignalHermesDownstreamServiceDescription(HermesDownstream* pDownstream, uint32_t sessionId, const HermesServiceDescription* pData)
{
    pDownstream->m_service.Log(sessionId, "SignalHermesDownstreamServiceDescription");
    pDownstream->m_service.Post([pDownstream, sessionId, data = ToCpp(*pData)]()
    {
        pDownstream->Signal(sessionId, data);
    });
}

void SignalHermesBoardAvailable(HermesDownstream* pDownstream, uint32_t sessionId, const HermesBoardAvailableData* pData)
{
    pDownstream->m_service.Log(sessionId, "SignalHermesBoardAvailable");
    pDownstream->m_service.Post([pDownstream, sessionId, data = ToCpp(*pData)]()
    {
        pDownstream->Signal(sessionId, data);
    });
}

void SignalHermesRevokeBoardAvailable(HermesDownstream* pDownstream, uint32_t sessionId, const HermesRevokeBoardAvailableData* pData)
{
    pDownstream->m_service.Log(sessionId, "SignalHermesRevokeBoardAvailable");
    pDownstream->m_service.Post([pDownstream, sessionId, data = ToCpp(*pData)]()
    {
        pDownstream->Signal(sessionId, data);
    });

}

void SignalHermesTransportFinished(HermesDownstream* pDownstream, uint32_t sessionId, const HermesTransportFinishedData* pData)
{
    pDownstream->m_service.Log(sessionId, "SignalHermesTransportFinished");

    pDownstream->m_service.Post([pDownstream, sessionId, data = ToCpp(*pData)]()
    {
        pDownstream->Signal(sessionId, data);
    });
}

void SignalHermesDownstreamNotification(HermesDownstream* pDownstream, uint32_t sessionId, const HermesNotificationData* pData)
{
    pDownstream->m_service.Log(0U, "SignalHermesDownstreamNotification");

    pDownstream->m_service.Post([pDownstream, sessionId, data = ToCpp(*pData)]()
    {
        pDownstream->Signal(sessionId, data);
    });
}

void SignalHermesDownstreamCheckAlive(HermesDownstream* pDownstream, uint32_t sessionId, const HermesCheckAliveData* pData)
{
    pDownstream->m_service.Log(0U, "SignalHermesDownstreamCheckAlive");

    pDownstream->m_service.Post([pDownstream, sessionId, data = ToCpp(*pData)]()
    {
        pDownstream->Signal(sessionId, data);
    });
}

void ResetHermesDownstream(HermesDownstream* pDownstream, const HermesNotificationData* pData)
{
    pDownstream->m_service.Log(0U, "ResetHermesDownstream");

    pDownstream->m_service.Post([pDownstream, data = ToCpp(*pData)]()
    {
        pDownstream->Reset(data);
    });
}

void DisableHermesDownstream(HermesDownstream* pDownstream, const HermesNotificationData* pData)
{
    pDownstream->m_service.Log(0U, "DisableHermesDownstream");

    pDownstream->m_service.Post([pDownstream, data = ToCpp(*pData)]()
    {
        pDownstream->Disable(data);
    });
}

void StopHermesDownstream(HermesDownstream* pDownstream)
{
    pDownstream->m_service.Log(0U, "StopHermesDownstream");

    pDownstream->m_service.Post([pDownstream]()
    {
        pDownstream->Stop();
    });
}

void DeleteHermesDownstream(HermesDownstream* pDownstream)
{
    if (!pDownstream)
        return;

    pDownstream->m_service.Log(0U, "DeleteHermesDownstream");

    pDownstream->m_service.Stop();
    delete pDownstream;
}
