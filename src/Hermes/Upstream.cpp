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

#include "stdafx.h"

#include <HermesDataConversion.hpp>
#include "ApiCallback.h"
#include "MessageDispatcher.h"
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
    UpstreamSettings m_settings;

    unsigned m_sessionId{0U};
    unsigned m_connectedSessionId{0U};

    ApiCallback<HermesConnectedCallback> m_connectedCallback;
    ApiCallback<HermesServiceDescriptionCallback> m_serviceDescriptionCallback;
    ApiCallback<HermesBoardAvailableCallback> m_boardAvailableCallback;
    ApiCallback<HermesRevokeBoardAvailableCallback> m_revokeBoardAvailableCallback;
    ApiCallback<HermesTransportFinishedCallback> m_transportFinishedCallback;
    ApiCallback<HermesBoardForecastCallback> m_boardForecastCallback;
    ApiCallback<HermesSendBoardInfoCallback> m_sendBoardInfoCallback;
    ApiCallback<HermesNotificationCallback> m_notificationCallback;
    ApiCallback<HermesCommandCallback> m_commandCallback;
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
        m_boardForecastCallback(callbacks.m_boardForecastCallback),
        m_sendBoardInfoCallback(callbacks.m_sendBoardInfoCallback),
        m_notificationCallback(callbacks.m_notificationCallback),
        m_commandCallback(callbacks.m_commandCallback),
        m_checkAliveCallback(callbacks.m_checkAliveCallback),
        m_stateCallback(callbacks.m_stateCallback),
        m_disconnectedCallback(callbacks.m_disconnectedCallback)
    {
        m_service.Inform(0U, "Created");
    }

    virtual ~HermesUpstream()
    {
        m_service.Inform(0U, "Deleted");
    }

    void Enable(const UpstreamSettings& settings)
    {
        m_service.Log(0U, "Enable(", settings, "); m_enabled=", m_enabled, ", m_settings=", m_settings);

        if (m_enabled && settings == m_settings)
            return;

        m_enabled = true;
        RemoveSession_(NotificationData(ENotificationCode::eCONNECTION_RESET_BECAUSE_OF_CHANGED_CONFIGURATION,
            ESeverity::eINFO, "ConfigurationChanged"));

        m_settings = settings;
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
    void Signal(unsigned sessionId, const DataT& data, StringView rawXml)
    {
        m_service.Log(sessionId, "Signal(", data, ',', rawXml, ')');

        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        pSession->Signal(data, rawXml);
    }

    void Stop()
    {
        m_service.Log(0U, "Stop()");

        NotificationData notificationData(ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "Upstream service stopped by application");
        RemoveSession_(notificationData);
        m_service.Stop();
    }

    //================= ISessionCallback =========================
    void OnSocketConnected(unsigned sessionId, EState state, const ConnectionInfo& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        m_connectedSessionId = pSession->Id();
        const Converter2C<ConnectionInfo> converter(in_data);
        m_connectedCallback(pSession->Id(), ToC(state), converter.CPointer());
    }

    void On(unsigned sessionId, EState state, const ServiceDescriptionData& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        const Converter2C<ServiceDescriptionData> converter(in_data);
        m_serviceDescriptionCallback(pSession->Id(), ToC(state), converter.CPointer());
    }

    void On(unsigned sessionId, EState state, const BoardAvailableData& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        const Converter2C<BoardAvailableData> converter(in_data);
        m_boardAvailableCallback(sessionId, ToC(state), converter.CPointer());
    }

    void On(unsigned sessionId, EState state, const RevokeBoardAvailableData& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        const Converter2C<RevokeBoardAvailableData> converter(in_data);
        m_revokeBoardAvailableCallback(sessionId, ToC(state), converter.CPointer());
    }

    void On(unsigned sessionId, EState state, const TransportFinishedData& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        const Converter2C<TransportFinishedData> converter(in_data);
        m_transportFinishedCallback(sessionId, ToC(state), converter.CPointer());
    }

    void On(unsigned sessionId, EState state, const BoardForecastData& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        const Converter2C<BoardForecastData> converter(in_data);
        m_boardForecastCallback(sessionId, ToC(state), converter.CPointer());
    }

    void On(unsigned sessionId, EState, const SendBoardInfoData& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        const Converter2C<SendBoardInfoData> converter(in_data);
        m_sendBoardInfoCallback(sessionId, converter.CPointer());
    }

    void On(unsigned sessionId, EState, const NotificationData& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        const Converter2C<NotificationData> converter(in_data);
        m_notificationCallback(sessionId, converter.CPointer());
    }

    void On(unsigned sessionId, EState, const CommandData& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        const Converter2C<CommandData> converter(in_data);
        m_commandCallback(sessionId, converter.CPointer());
    }

    void On(unsigned sessionId, EState, const CheckAliveData& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        if (in_data.m_optionalType
            && *in_data.m_optionalType == ECheckAliveType::ePING
            && m_settings.m_checkAliveResponseMode == ECheckAliveResponseMode::eAUTO)
        {
            CheckAliveData data{in_data};
            data.m_optionalType = ECheckAliveType::ePONG;
            m_service.Post([this, sessionId, data = std::move(data)]() { Signal(sessionId, data, Serialize(data)); });
        }
        const Converter2C<CheckAliveData> converter(in_data);
        m_checkAliveCallback(sessionId, converter.CPointer());
    }

    void OnState(unsigned sessionId, EState state) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        m_stateCallback(sessionId, ToC(state));
    }

    void OnDisconnected(unsigned sessionId, EState state, const Error& in_data) override
    {
        const auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        if (pSession->OptionalPeerServiceDescriptionData())
        {
            DelayCreateNewSession_(1.0);
        }
        else
        {
            DelayCreateNewSession_(m_settings.m_reconnectWaitTimeInSeconds);
        }

        m_upSession.reset();
        const Converter2C<Error> converter(in_data);
        m_disconnectedCallback(sessionId, ToC(state), converter.CPointer());
    }

    //============ internal impplementation ==============
    Session* Session_(unsigned id)
    {
        if (m_upSession && m_upSession->Id() == id)
            return m_upSession.get();

        m_service.Warn(id, "Session ID no longer valid; current session ID=", m_upSession ? m_upSession->Id() : 0U);
        return nullptr;
    }

    void RemoveSession_()
    {
        if (!m_upSession)
            return;

        auto sessionId = m_upSession->Id();
        m_service.Log(sessionId, "RemoveSession_()");

        m_upSession->Disconnect();
        m_upSession.reset();

        if (sessionId != m_connectedSessionId)
            return;

        m_connectedSessionId = 0U;
        Error error;
        const Converter2C<Error> converter(error);
        m_disconnectedCallback(sessionId, eHERMES_STATE_DISCONNECTED, converter.CPointer());
    }

    void RemoveSession_(const NotificationData& data)
    {
        if (!m_upSession)
            return;

        m_upSession->Signal(data, Serialize(data));
        RemoveSession_();
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

        m_upSession = std::make_unique<Session>(m_sessionId, m_service, m_settings);
        m_upSession->Connect(*this);
    }

    void DelayCreateNewSession_(double delay)
    {
        m_service.Log(0U, "DelayCreateNewSession_");

        m_timer.expires_from_now(boost::posix_time::millisec(static_cast<int>(1000.0 * delay)));
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

void SignalHermesUpstreamServiceDescription(HermesUpstream* pUpstream, uint32_t sessionId, const HermesServiceDescriptionData* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesUpstreamServiceDescription");
    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesMachineReady(HermesUpstream* pUpstream, uint32_t sessionId, const HermesMachineReadyData* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesMachineReady");
    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesRevokeMachineReady(HermesUpstream* pUpstream, uint32_t sessionId, const HermesRevokeMachineReadyData* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesRevokeMachineReady");
    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesStartTransport(HermesUpstream* pUpstream, uint32_t sessionId, const HermesStartTransportData* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesStartTransport");

    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesQueryBoardInfo(HermesUpstream* pUpstream, uint32_t sessionId, const HermesQueryBoardInfoData* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesQueryBoardInfo");

    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesStopTransport(HermesUpstream* pUpstream, uint32_t sessionId, const HermesStopTransportData* pData)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesStopTransport");

    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesUpstreamNotification(HermesUpstream* pUpstream, uint32_t sessionId, const HermesNotificationData* pData)
{
    pUpstream->m_service.Log(0U, "SignalHermesUpstreamNotification");

    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesUpstreamCommand(HermesUpstream* pUpstream, uint32_t sessionId, const HermesCommandData* pData)
{
    pUpstream->m_service.Log(0U, "SignalHermesUpstreamCommand");

    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesUpstreamCheckAlive(HermesUpstream* pUpstream, uint32_t sessionId, const HermesCheckAliveData* pData)
{
    pUpstream->m_service.Log(0U, "SignalHermesUpstreamCheckAlive");

    pUpstream->m_service.Post([pUpstream, sessionId, data = ToCpp(*pData)]()
    {
        pUpstream->Signal(sessionId, data, Serialize(data));
    });
}

void ResetHermesUpstream(HermesUpstream* pUpstream, const HermesNotificationData* pData)
{
    pUpstream->m_service.Log(0U, "ResetHermesUpstream");

    pUpstream->m_service.Post([pUpstream, data = ToCpp(*pData)]()
    {
        pUpstream->RemoveSession_(data);
        pUpstream->DelayCreateNewSession_(1.0);
    });
}

void SignalHermesUpstreamRawXml(HermesUpstream* pUpstream, uint32_t sessionId, HermesStringView rawXml)
{
    pUpstream->m_service.Log(sessionId, "SignalHermesUpstreamRawXml");
    pUpstream->m_service.Post([pUpstream, sessionId, xmlData = std::string(rawXml.m_pData, rawXml.m_size)]() mutable
    {
        MessageDispatcher dispatcher{sessionId, pUpstream->m_service};
        auto parseData = xmlData;

        bool wasDispatched = false;
        dispatcher.Add<ServiceDescriptionData>([&](const auto& data) { wasDispatched = true; pUpstream->Signal(sessionId, data, xmlData); });
        dispatcher.Add<MachineReadyData>([&](const auto& data) { wasDispatched = true; pUpstream->Signal(sessionId, data, xmlData); });
        dispatcher.Add<RevokeMachineReadyData>([&](const auto& data) { wasDispatched = true; pUpstream->Signal(sessionId, data, xmlData); });
        dispatcher.Add<StartTransportData>([&](const auto& data) { wasDispatched = true; pUpstream->Signal(sessionId, data, xmlData); });
        dispatcher.Add<StopTransportData>([&](const auto& data) { wasDispatched = true; pUpstream->Signal(sessionId, data, xmlData); });
        dispatcher.Add<QueryBoardInfoData>([&](const auto& data) { wasDispatched = true; pUpstream->Signal(sessionId, data, xmlData); });
        dispatcher.Add<NotificationData>([&](const auto& data) { wasDispatched = true; pUpstream->Signal(sessionId, data, xmlData); });
        dispatcher.Add<CheckAliveData>([&](const auto& data) { wasDispatched = true; pUpstream->Signal(sessionId, data, xmlData); });

        dispatcher.Dispatch(parseData);
        if (wasDispatched)
            return;

        pUpstream->Signal(sessionId, NotificationData{}, xmlData);
    });
}

void ResetHermesUpstreamRawXml(HermesUpstream* pUpstream, HermesStringView rawXml)
{
    pUpstream->m_service.Log(0U, "ResetHermesUpstreamRawXml");
    pUpstream->m_service.Post([pUpstream, data = std::string(rawXml.m_pData, rawXml.m_size)]()
    {
        if (!data.empty() && pUpstream->m_upSession)
        {
            pUpstream->m_upSession->Signal(NotificationData(), data);
        }
        pUpstream->RemoveSession_();
        pUpstream->DelayCreateNewSession_(1.0);
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
