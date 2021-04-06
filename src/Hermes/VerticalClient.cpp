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
#include "VerticalClientSession.h"

#include <cassert>
#include <memory>

using namespace Hermes;
using namespace Hermes::VerticalClient;

struct HermesVerticalClient : ISessionCallback
{
    Service m_service;
    asio::deadline_timer m_timer{ m_service.GetUnderlyingService() };
    VerticalClientSettings m_settings;

    unsigned m_sessionId{ 0U };
    unsigned m_connectedSessionId{ 0U };

    ApiCallback<HermesVerticalConnectedCallback> m_connectedCallback;
    ApiCallback<HermesSupervisoryServiceDescriptionCallback> m_serviceDescriptionCallback;
    ApiCallback<HermesBoardArrivedCallback> m_boardArrivedCallback;
    ApiCallback<HermesBoardDepartedCallback> m_boardDepartedCallback;
    ApiCallback<HermesQueryWorkOrderInfoCallback> m_queryWorkOrderInfoCallback;
    ApiCallback<HermesCurrentConfigurationCallback> m_currentConfigurationCallback;
    ApiCallback<HermesNotificationCallback> m_notificationCallback;
    ApiCallback<HermesCheckAliveCallback> m_checkAliveCallback;
    ApiCallback<HermesVerticalDisconnectedCallback> m_disconnectedCallback;

    std::unique_ptr<Session> m_upSession;

    bool m_enabled{ false };

    HermesVerticalClient(const HermesVerticalClientCallbacks& callbacks) :
        m_service(callbacks.m_traceCallback),
        m_connectedCallback(callbacks.m_connectedCallback),
        m_serviceDescriptionCallback(callbacks.m_serviceDescriptionCallback),
        m_boardArrivedCallback(callbacks.m_boardArrivedCallback),
        m_boardDepartedCallback(callbacks.m_boardDepartedCallback),
        m_queryWorkOrderInfoCallback(callbacks.m_queryWorkOrderInfoCallback),
        m_currentConfigurationCallback(callbacks.m_currentConfigurationCallback),
        m_notificationCallback(callbacks.m_notificationCallback),
        m_checkAliveCallback(callbacks.m_checkAliveCallback),
        m_disconnectedCallback(callbacks.m_disconnectedCallback)
    {
        m_service.Inform(0U, "Created");
    }

    virtual ~HermesVerticalClient()
    {
        m_service.Inform(0U, "Deleted");
    }

    void Enable(const VerticalClientSettings& settings)
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

        auto pSession = Session_(sessionId);
        if (!pSession)
            return;

        pSession->Signal(data, rawXml);
    }

    void Stop()
    {
        m_service.Log(0U, "Stop()");

        NotificationData notificationData(ENotificationCode::eMACHINE_SHUTDOWN, ESeverity::eINFO, "Vertial client stopped by application");
        RemoveSession_(notificationData);
        m_service.Stop();
    }

    //================= ISessionCallback =========================
    void OnSocketConnected(unsigned sessionId, EVerticalState state, const ConnectionInfo& connectionInfo) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        m_connectedSessionId = pSession->Id();
        auto apiConnectionInfo = ToC(connectionInfo);
        m_connectedCallback(pSession->Id(), ToC(state), &apiConnectionInfo);
    }

    void On(unsigned sessionId, EVerticalState state, const SupervisoryServiceDescriptionData& serviceDescription) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiServiceDescriptionData = ToC(serviceDescription);
        m_serviceDescriptionCallback(pSession->Id(), ToC(state), &apiServiceDescriptionData);
    }

    void On(unsigned sessionId, EVerticalState, const BoardArrivedData& in_data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_boardArrivedCallback(sessionId, &apiData);
    }

    void On(unsigned sessionId, EVerticalState, const BoardDepartedData& in_data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_boardDepartedCallback(sessionId, &apiData);
    }

    void On(unsigned sessionId, EVerticalState, const QueryWorkOrderInfoData& in_data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_queryWorkOrderInfoCallback(sessionId, &apiData);
    }

    void On(unsigned sessionId, EVerticalState, const CurrentConfigurationData& in_data) override
    {
        auto* pSession = Session_(sessionId);
        if (!pSession)
            return;

        auto apiData = ToC(in_data);
        m_currentConfigurationCallback(sessionId, &apiData);
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
        auto pSession = Session_(sessionId);
        if (!pSession)
            return;

        if (in_data.m_optionalType
            && *in_data.m_optionalType == ECheckAliveType::ePING
            && m_settings.m_checkAliveResponseMode == ECheckAliveResponseMode::eAUTO)
        {
            CheckAliveData data{ in_data };
            data.m_optionalType = ECheckAliveType::ePONG;
            m_service.Post([this, sessionId, data = std::move(data)]() { Signal(sessionId, data, Serialize(data)); });
        }
        auto apiData = ToC(in_data);
        m_checkAliveCallback(sessionId, &apiData);
    }

    void OnDisconnected(unsigned sessionId, EVerticalState state, const Error& error) override
    {
        auto* pSession = Session_(sessionId);
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
        auto apiError = ToC(error);
        m_disconnectedCallback(sessionId, eHERMES_VERTICAL_STATE_DISCONNECTED, &apiError);
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

HermesVerticalClient* CreateHermesVerticalClient(const HermesVerticalClientCallbacks* pCallbacks)
{
    return new HermesVerticalClient(*pCallbacks);
}

void RunHermesVerticalClient(HermesVerticalClient* pVerticalClient)
{
    pVerticalClient->m_service.Log(0U, "RunHermesVerticalClient");

    pVerticalClient->m_service.Run();
}

void PostHermesVerticalClient(HermesVerticalClient* pVerticalClient, HermesVoidCallback voidCallback)
{
    pVerticalClient->m_service.Log(0U, "PostHermesVerticalClient");

    pVerticalClient->m_service.Post([voidCallback]
    {
        voidCallback.m_pCall(voidCallback.m_pData);
    });
}

void EnableHermesVerticalClient(HermesVerticalClient* pVerticalClient, const HermesVerticalClientSettings* pSettings)
{
    pVerticalClient->m_service.Log(0U, "EnableHermesVerticalClient");
    auto settings = ToCpp(*pSettings);
    if (!settings.m_port)
    {
        settings.m_port = static_cast<uint16_t>(cBASE_PORT);
    }

    pVerticalClient->m_service.Post([pVerticalClient, settings = std::move(settings)]()
    {
        pVerticalClient->Enable(settings);
    });
}

void SignalHermesVerticalClientDescription(HermesVerticalClient* pVerticalClient, uint32_t sessionId, const HermesSupervisoryServiceDescriptionData* pData)
{
    pVerticalClient->m_service.Log(sessionId, "SignalHermesVerticalClientDescription");
    pVerticalClient->m_service.Post([pVerticalClient, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalClient->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesSendWorkOrderInfo(HermesVerticalClient* pVerticalClient, uint32_t sessionId, const HermesSendWorkOrderInfoData* pData)
{
    pVerticalClient->m_service.Log(sessionId, "SignalHermesSendWorkOrderInfo");
    pVerticalClient->m_service.Post([pVerticalClient, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalClient->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesVerticalGetConfiguration(HermesVerticalClient* pVerticalClient, uint32_t sessionId, const HermesGetConfigurationData* pData)
{
    pVerticalClient->m_service.Log(sessionId, "SignalHermesVerticalGetConfiguration");
    pVerticalClient->m_service.Post([pVerticalClient, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalClient->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesVerticalSetConfiguration(HermesVerticalClient* pVerticalClient, uint32_t sessionId, const HermesSetConfigurationData* pData)
{
    pVerticalClient->m_service.Log(sessionId, "SignalHermesVerticalSetConfiguration");
    pVerticalClient->m_service.Post([pVerticalClient, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalClient->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesVerticalClientNotification(HermesVerticalClient* pVerticalClient, uint32_t sessionId, const HermesNotificationData* pData)
{
    pVerticalClient->m_service.Log(0U, "SignalHermesVerticalClientNotification");

    pVerticalClient->m_service.Post([pVerticalClient, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalClient->Signal(sessionId, data, Serialize(data));
    });
}

void SignalHermesVerticalClientCheckAlive(HermesVerticalClient* pVerticalClient, uint32_t sessionId, const HermesCheckAliveData* pData)
{
    pVerticalClient->m_service.Log(0U, "SignalHermesVerticalClientCheckAlive");

    pVerticalClient->m_service.Post([pVerticalClient, sessionId, data = ToCpp(*pData)]()
    {
        pVerticalClient->Signal(sessionId, data, Serialize(data));
    });
}

void ResetHermesVerticalClient(HermesVerticalClient* pVerticalClient, const HermesNotificationData* pData)
{
    pVerticalClient->m_service.Log(0U, "ResetHermesVerticalClient");

    pVerticalClient->m_service.Post([pVerticalClient, data = ToCpp(*pData)]()
    {
        pVerticalClient->RemoveSession_(data);
        pVerticalClient->DelayCreateNewSession_(1.0);
    });
}


void DisableHermesVerticalClient(HermesVerticalClient* pVerticalClient, const HermesNotificationData* pData)
{
    pVerticalClient->m_service.Log(0U, "DisableHermesVerticalClient");
    pVerticalClient->m_service.Post([pVerticalClient, data = ToCpp(*pData)]()
    {
        pVerticalClient->Disable(data);
    });
}

void StopHermesVerticalClient(HermesVerticalClient* pVerticalClient)
{
    pVerticalClient->m_service.Log(0U, "StopHermesVerticalClient");

    pVerticalClient->m_service.Post([pVerticalClient]()
    {
        pVerticalClient->Stop();
    });
}

void DeleteHermesVerticalClient(HermesVerticalClient* pVerticalClient)
{
    if (!pVerticalClient)
        return;

    pVerticalClient->m_service.Log(0U, "DeleteHermesVerticalClient");

    pVerticalClient->m_service.Stop();
    delete pVerticalClient;
}

void SignalHermesVerticalClientRawXml(HermesVerticalClient* pVerticalClient, uint32_t sessionId, HermesStringView rawXml)
{
    pVerticalClient->m_service.Log(sessionId, "SignalHermesVerticalClientRawXml");
    pVerticalClient->m_service.Post([pVerticalClient, sessionId, xmlData = std::string(rawXml.m_pData, rawXml.m_size)]() mutable
    {
        MessageDispatcher dispatcher{ sessionId, pVerticalClient->m_service };
        auto parseData = xmlData;

        bool wasDispatched = false;
        dispatcher.Add<SupervisoryServiceDescriptionData>([&](const auto& data) { wasDispatched = true; pVerticalClient->Signal(sessionId, data, xmlData); });
        dispatcher.Add<SendWorkOrderInfoData>([&](const auto& data) { wasDispatched = true; pVerticalClient->Signal(sessionId, data, xmlData); });
        dispatcher.Add<GetConfigurationData>([&](const auto& data) { wasDispatched = true; pVerticalClient->Signal(sessionId, data, xmlData); });
        dispatcher.Add<SetConfigurationData>([&](const auto& data) { wasDispatched = true; pVerticalClient->Signal(sessionId, data, xmlData); });
        dispatcher.Add<NotificationData>([&](const auto& data) { wasDispatched = true; pVerticalClient->Signal(sessionId, data, xmlData); });
        dispatcher.Add<CheckAliveData>([&](const auto& data) { wasDispatched = true; pVerticalClient->Signal(sessionId, data, xmlData); });

        dispatcher.Dispatch(parseData);
        if (wasDispatched)
            return;

        pVerticalClient->Signal(sessionId, NotificationData{}, xmlData);
    });
}

void ResetHermesVerticalClientRawXml(HermesVerticalClient* pVerticalClient, HermesStringView rawXml)
{
    pVerticalClient->m_service.Log(0U, "ResetHermesVerticalClientRawXml");
    pVerticalClient->m_service.Post([pVerticalClient, data = std::string(rawXml.m_pData, rawXml.m_size)]()
    {
        if (!data.empty() && pVerticalClient->m_upSession)
        {
            pVerticalClient->m_upSession->Signal(NotificationData(), data);
        }
        pVerticalClient->RemoveSession_();
        pVerticalClient->DelayCreateNewSession_(1.0);
    });
}
