/*++

Copyright (c) Microsoft Corporation

Module Name:

    connector.h

Abstract:

    Connector mapping services for ports

Author:



Environment:

    Kernel mode only.

--*/


#pragma once


typedef struct _CONNECTOR_MAP_NODE {
    CONNECTOR_ID        ConnectorId;
    PPORT_CONTEXT       Usb20Port;
    PPORT_CONTEXT       Usb30Port;
    PPORT_CONTEXT       Usb30SecondPort;
    LIST_ENTRY          ListEntry;
} CONNECTOR_MAP_NODE, *PCONNECTOR_MAP_NODE;


FORCEINLINE
__drv_maxIRQL(PASSIVE_LEVEL)
PCONNECTOR_MAP_NODE
HUBCONNECTOR_LookupConnectorId(
    __in
        PCONNECTOR_ID       ConnectorId
        );


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBCONNECTOR_DeRegisterPort(
    __in
        PPORT_CONTEXT       PortContext
        );


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBCONNECTOR_RegisterPort(
    __in
        PPORT_CONTEXT       PortContext
        );


__drv_maxIRQL(PASSIVE_LEVEL)
PPORT_CONTEXT
HUBCONNECTOR_GetCompanionPort(
    __in
        PPORT_CONTEXT       PortContext,
    __in
        USHORT              CompanionIndex
        );


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_MapRootHubPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        );


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_MapIntegratedHubPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        );


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_MapExternalHubPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        );


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBCONNECTOR_MapAcpiPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        );


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_MapHubPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_UnMapHubPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        );

FORCEINLINE
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_LockConnectorMap(
        );

FORCEINLINE
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_UnlockConnectorMap(
        );

__drv_maxIRQL(PASSIVE_LEVEL)
PCONNECTOR_MAP_NODE
HUBCONNECTOR_GetConnectorMapNodeForPort(
    __in
        PPORT_CONTEXT       PortContext
        );
