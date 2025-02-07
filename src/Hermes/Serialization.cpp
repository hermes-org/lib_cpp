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

#include <boost/asio.hpp>

#include <HermesSerialization.h>
#include <HermesDataConversion.hpp>

#include "MessageDispatcher.h"
#include "Service.h"


void HermesSerializeServiceDescription(const HermesServiceDescriptionData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeBoardAvailable(const HermesBoardAvailableData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeRevokeBoardAvailable(const HermesRevokeBoardAvailableData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeMachineReady(const HermesMachineReadyData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeRevokeMachineReady(const HermesRevokeMachineReadyData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeStartTransport(const HermesStartTransportData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeStopTransport(const HermesStopTransportData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeTransportFinished(const HermesTransportFinishedData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeBoardForecast(const HermesBoardForecastData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeQueryBoardInfo(const HermesQueryBoardInfoData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeSendBoardInfo(const HermesSendBoardInfoData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeNotification(const HermesNotificationData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeCheckAlive(const HermesCheckAliveData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeGetConfiguration(const HermesGetConfigurationData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeSetConfiguration(const HermesSetConfigurationData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeCurrentConfiguration(const HermesCurrentConfigurationData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeSupervisoryServiceDescription(const HermesSupervisoryServiceDescriptionData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeBoardArrived(const HermesBoardArrivedData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeBoardDeparted(const HermesBoardDepartedData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeQueryWorkOrderInfo(const HermesQueryWorkOrderInfoData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeSendWorkOrderInfo(const HermesSendWorkOrderInfoData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeReplyWorkOrderInfo(const HermesReplyWorkOrderInfoData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeCommand(const HermesCommandData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeQueryHermesCapabilities(const HermesQueryHermesCapabilitiesData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
void HermesSerializeSendHermesCapabilities(const HermesSendHermesCapabilitiesData* pData, HermesSerializationCallback callback)
{
    callback.m_pCall(callback.m_pData, Hermes::ToC(Hermes::Serialize(Hermes::ToCpp(*pData))));
}
namespace
{
    template<class T, class CallbackT>
    void Add_(Hermes::MessageDispatcher& dispatcher, CallbackT callback)
    {
        if (!callback.m_pCall)
            return;

        dispatcher.Add<T>([callback](const auto& data)
        {
            Hermes::Converter2C<T> converter(data);
            callback.m_pCall(callback.m_pData, converter.CPointer());
        });
    }
}

void HermesDeserialize(HermesStringView stringView, const HermesDeserializationCallbacks* pCallbacks)
{
    HermesTraceCallback traceCallback{};
    Hermes::Service service{traceCallback};
    Hermes::MessageDispatcher dispatcher{0U, service};

    Add_<Hermes::ServiceDescriptionData>(dispatcher, pCallbacks->m_serviceDescriptionCallback);
    Add_<Hermes::BoardAvailableData>(dispatcher, pCallbacks->m_boardAvailableCallback);
    Add_<Hermes::RevokeBoardAvailableData>(dispatcher, pCallbacks->m_revokeBoardAvailableCallback);
    Add_<Hermes::MachineReadyData>(dispatcher, pCallbacks->m_machineReadyCallback);
    Add_<Hermes::RevokeMachineReadyData>(dispatcher, pCallbacks->m_revokeMachineReadyCallback);
    Add_<Hermes::StartTransportData>(dispatcher, pCallbacks->m_startTransportCallback);
    Add_<Hermes::StopTransportData>(dispatcher, pCallbacks->m_stopTransportCallback);
    Add_<Hermes::TransportFinishedData>(dispatcher, pCallbacks->m_transportFinishedCallback);
    Add_<Hermes::BoardForecastData>(dispatcher, pCallbacks->m_boardForecastCallback);
    Add_<Hermes::QueryBoardInfoData>(dispatcher, pCallbacks->m_queryBoardInfoCallback);
    Add_<Hermes::SendBoardInfoData>(dispatcher, pCallbacks->m_sendBoardInfoCallback);
    Add_<Hermes::CheckAliveData>(dispatcher, pCallbacks->m_checkAliveCallback);
    Add_<Hermes::NotificationData>(dispatcher, pCallbacks->m_notificationCallback);
    Add_<Hermes::GetConfigurationData>(dispatcher, pCallbacks->m_getConfigurationCallback);
    Add_<Hermes::SetConfigurationData>(dispatcher, pCallbacks->m_setConfigurationCallback);
    Add_<Hermes::CurrentConfigurationData>(dispatcher, pCallbacks->m_currentConfigurationCallback);
    Add_<Hermes::SupervisoryServiceDescriptionData>(dispatcher, pCallbacks->m_supervisoryServiceDescriptionCallback);
    Add_<Hermes::BoardArrivedData>(dispatcher, pCallbacks->m_boardArrivedCallback);
    Add_<Hermes::BoardDepartedData>(dispatcher, pCallbacks->m_boardDepartedCallback);
    Add_<Hermes::QueryWorkOrderInfoData>(dispatcher, pCallbacks->m_queryWorkOrderInfoCallback);
    Add_<Hermes::SendWorkOrderInfoData>(dispatcher, pCallbacks->m_sendWorkOrderInfoCallback);
    Add_<Hermes::ReplyWorkOrderInfoData>(dispatcher, pCallbacks->m_replyWorkOrderInfoCallback);
    Add_<Hermes::CommandData>(dispatcher, pCallbacks->m_commandCallback);
    Add_<Hermes::QueryHermesCapabilitiesData>(dispatcher, pCallbacks->m_queryHermesCapabilitiesCallback);
    Add_<Hermes::SendHermesCapabilitiesData>(dispatcher, pCallbacks->m_sendHermesCapabilitiesCallback);

    std::string xmlParseString{Hermes::ToCpp(stringView)};
    auto error = dispatcher.Dispatch(xmlParseString);
    if (!error)
        return;
    
    if (!pCallbacks->m_deserializationErrorCallback.m_pCall)
        return;

    const Hermes::Converter2C<Hermes::Error> converter(error);
    pCallbacks->m_deserializationErrorCallback.m_pCall(pCallbacks->m_deserializationErrorCallback.m_pData, 
        converter.CPointer());
}


