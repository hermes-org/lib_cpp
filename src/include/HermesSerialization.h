// Copyright (c) ASM Assembly Systems GmbH & Co. KG
//
// C interface for (de)serializing The Hermes Standard messages
//
#ifndef HERMESSERIALIZATION_H
#define HERMESSERIALIZATION_H

#include "Hermes.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct HermesStringHandle;
    HERMESPROTOCOL_API HermesStringView HermesStringViewFromHandle(HermesStringHandle*);

    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromServiceDescription(const HermesServiceDescription*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromBoardAvailableData(const HermesBoardAvailableData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromRevokeBoardAvailableData(const HermesRevokeBoardAvailableData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromMachineReadyData(const HermesMachineReadyData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromRevokeMachineReadyData(const HermesRevokeMachineReadyData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromStartTransportData(const HermesStartTransportData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromStopTransportData(const HermesStopTransportData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromTransportFinishedData(const HermesTransportFinishedData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromNotificationData(const HermesNotificationData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromCheckAliveData(const HermesCheckAliveData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromGetConfigurationData(const HermesGetConfigurationData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromSetConfigurationData(const HermesSetConfigurationData*);
    HERMESPROTOCOL_API HermesStringHandle* HermesXmlFromCurrentConfigurationData(const HermesCurrentConfigurationData*);

    HERMESPROTOCOL_API void FreeHermesString(HermesStringHandle*);

    struct HermesDataHandle;
    HERMESPROTOCOL_API HermesDataHandle* HermesDataFromXml(HermesStringView);

    HERMESPROTOCOL_API const HermesError* HermesErrorFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesServiceDescription* HermesServiceDescriptionFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesBoardAvailableData* HermesBoardAvailableDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesRevokeBoardAvailableData* HermesRevokeBoardAvailableDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesMachineReadyData* HermesMachineReadyDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesRevokeMachineReadyData* HermesRevokeMachineReadyDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesStartTransportData* HermesStartTransportDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesStopTransportData* HermesStopTransportDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesTransportFinishedData* HermesTransportFinishedDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesNotificationData* HermesNotificationDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesCheckAliveData* HermesCheckAliveDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesGetConfigurationData* HermesGetConfigurationDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesSetConfigurationData* HermesSetConfigurationDataFromHandle(HermesDataHandle*);
    HERMESPROTOCOL_API const HermesCurrentConfigurationData* HermesCurrentConfigurationDataFromHandle(HermesDataHandle*);

    HERMESPROTOCOL_API void FreeHermesData(HermesDataHandle*);

#ifdef __cplusplus
}
#endif

#endif // HERMESSERIALIZATION_H


