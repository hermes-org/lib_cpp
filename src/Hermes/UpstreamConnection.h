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

#pragma once
#include <HermesData.hpp>

namespace Hermes
{

    class UpstreamConnection
    {
    public:
        UpstreamConnection(IAsioServiceSp, const UpstreamSettings&, const UpstreamConfiguration&);

        SignalResult Connect(const ServiceDescription&);
        SignalResult Signal(const MachineReadyData&);
        SignalResult Signal(const RevokeMachineReadyData&);
        SignalResult Signal(const StartTransportData&);
        SignalResult Signal(const StopTransportData&);
        SignalResult SendNotification(const NotificationData&);
        SignalResult Disconnect(EHermesNotificationCode, EHermesSeverity);

    };

    struct IUpstreamConnectionCallback
    {
        virtual void OnConnected(UpstreamConnection&, EHermesState, const ServiceDescription&) = 0;
        virtual void OnBoardAvailable(UpstreamConnection&, EHermesState, const BoardAvailableData&) = 0;
        virtual void OnRevokeBoardAvailable(UpstreamConnection&, EHermesState, const RevokeBoardAvailableData&) = 0;
        virtual void OnTransportFinished(UpstreamConnection&, EHermesState, const TransportFinishedData&) = 0;
        virtual void OnDisconnected(UpstreamConnection&, EHermesState) = 0;

        virtual void (UpstreamConnection&, EHermesState, const NotificationData&) = 0;
    };


}
