
/*++

Copyright (c) Microsoft Corporation

Module Name:

    connector.c

Abstract:

    Port to connector mapping services

Author:



Environment:

    Kernel mode only.

--*/

#include "pch.h"
#include "connector.tmh"


#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, HUBCONNECTOR_LookupConnectorId)
#pragma alloc_text (PAGE, HUBCONNECTOR_UnMapHubPorts)
#pragma alloc_text (PAGE, HUBCONNECTOR_MapHubPorts)
#pragma alloc_text (PAGE, HUBCONNECTOR_MapAcpiPorts)
#pragma alloc_text (PAGE, HUBCONNECTOR_MapExternalHubPorts)
#pragma alloc_text (PAGE, HUBCONNECTOR_MapIntegratedHubPorts)
#pragma alloc_text (PAGE, HUBCONNECTOR_MapRootHubPorts)
#pragma alloc_text (PAGE, HUBCONNECTOR_GetCompanionPort)
#pragma alloc_text (PAGE, HUBCONNECTOR_DeRegisterPort)
#pragma alloc_text (PAGE, HUBCONNECTOR_RegisterPort)
#pragma alloc_text (PAGE, HUBCONNECTOR_LockConnectorMap)
#pragma alloc_text (PAGE, HUBCONNECTOR_UnlockConnectorMap)
#pragma alloc_text (PAGE, HUBCONNECTOR_GetConnectorMapNodeForPort)
#endif

FORCEINLINE
__drv_maxIRQL(PASSIVE_LEVEL)
PCONNECTOR_MAP_NODE
HUBCONNECTOR_LookupConnectorId(
    __in
        PCONNECTOR_ID       ConnectorId
        )
/*++

Routine Description:

    Finds a connector node (if it exists) in the connector map for a specified connector ID.

    NOTE: It is assumed that the connector node list is locked on entry to this function.

Arguments:
    
    ConnectorId - Pointer to the connector ID to find the corresponding connector node.

Return Value:

    Pointer to the matching connector map node if found, NULL otherwise.

--*/
{
    PHUB_GLOBALS            hubGlobals;
    PCONNECTOR_MAP_NODE     connectorMapNode;
    
    PAGED_CODE ();

    TRY {

        hubGlobals = GetHubGlobals(WdfGetDriver());

        //
        // Walk list of global connector map nodes
        //
        FOR_ALL_IN_LIST(CONNECTOR_MAP_NODE,
                        &hubGlobals->ConnectorMapHead,
                        ListEntry,
                        connectorMapNode) {

            if (RtlCompareMemory(&connectorMapNode->ConnectorId,
                                 ConnectorId,
                                 sizeof(CONNECTOR_ID)) ==
                sizeof(CONNECTOR_ID)) {
                    LEAVE;
            }
        }

        connectorMapNode = NULL;
    
    } FINALLY {

    }

    return connectorMapNode;
}


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBCONNECTOR_RegisterPort(
    __in
        PPORT_CONTEXT       PortContext
        )
/*++

Routine Description:

    Registers a port with the connector map.

Arguments:
    
    PortContext - Pointer to the port context structure.  It is assumed that the connector ID
                  structure within the port context is valid.

Return Value:

    NTSTATUS

--*/
{
    PHUB_GLOBALS            hubGlobals;
    PCONNECTOR_MAP_NODE     connectorMapNode;
    NTSTATUS                status;
    
    PAGED_CODE ();

    TRY {

        hubGlobals = GetHubGlobals(WdfGetDriver());

        status = STATUS_SUCCESS;

        //
        // Lock the list of connector map nodes
        //
        HUBCONNECTOR_LockConnectorMap();

        connectorMapNode = HUBCONNECTOR_LookupConnectorId(&PortContext->ConnectorId);

        if (connectorMapNode == NULL) {
            //
            // Did not find an existing connector map node matching the connector ID, so allocate one.
            //
            connectorMapNode = ExAllocatePoolWithTag(NonPagedPool,
                                                     sizeof(CONNECTOR_MAP_NODE),
                                                     USBHUB3_PORT_TAG);

            if (connectorMapNode == NULL) {
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           Port,
                           "Unable to allocate memory for connector node");
                
                status = STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            RtlZeroMemory(connectorMapNode, sizeof(CONNECTOR_MAP_NODE));
            
            RtlCopyMemory(&connectorMapNode->ConnectorId,
                          &PortContext->ConnectorId,
                          sizeof(CONNECTOR_ID));    

            InsertTailList(&hubGlobals->ConnectorMapHead, &connectorMapNode->ListEntry);

        }

        switch (PortContext->PortInformation.PortProtocol) {
        case PortProtocolUsb20:
            if (connectorMapNode->Usb20Port != NULL) {
                DbgTraceTo(PortContext->IfrLog,
                           TL_ERROR,
                           HwcPlatform,
                           "HW_COMPLIANCE_PLATFORM: Multiple USB 2.0 ports mapped to the same connector!");
                NT_ASSERTMSG("Multiple USB 2.0 ports mapped to the same connector!", 0);
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            if (PortContext->PortInformation.PortFlags.PortFlagTypeCWithoutSwitch == 1) {
                if (connectorMapNode->Usb30Port != NULL) {
                    //
                    // USB 2.0 Type C port without switch.   Make sure the corresponding USB 3.0 port is the same type.
                    //
                    if (connectorMapNode->Usb30Port->PortInformation.PortFlags.PortFlagTypeCWithoutSwitch == 0) {
                        DbgTraceTo(PortContext->IfrLog,
                                    TL_ERROR,
                                    HwcPlatform,
                                    "HW_COMPLIANCE_PLATFORM: USB 2.0 Type C without switch port mapped to a different type of USB 3.0 port %x!",
                                    connectorMapNode->Usb30Port->AcpiUpcPackage.PortConnectorType);
                        NT_ASSERTMSG("USB 2.0 Type C without switch port mapped to a different type of USB 3.0 port!", 0);
                        status = STATUS_UNSUCCESSFUL;
                        LEAVE;
                    }
                }

                PortContext->PortInformation.PortNumberForInstanceId = PortContext->PortInformation.PortNumber;
            }
           
            connectorMapNode->Usb20Port = PortContext;
            break;

        case PortProtocolUsb30:
            if (connectorMapNode->Usb30Port != NULL) {
                
                if (PortContext->PortInformation.PortFlags.PortFlagTypeCWithoutSwitch == 1) {
                    
                    USHORT PortNumberForInstanceId;

                    if (connectorMapNode->Usb30SecondPort != NULL) {
                        DbgTraceTo(PortContext->IfrLog,
                                   TL_ERROR,
                                   HwcPlatform,
                                   "HW_COMPLIANCE_PLATFORM: More than two Type C USB 3.0 ports mapped to the same connector!");
                        NT_ASSERTMSG("More than two Type C USB 3.0 ports mapped to the same connector!", 0);
                        status = STATUS_UNSUCCESSFUL;
                        LEAVE;
                    }
                    
                    //
                    // Second USB 3.0 Type C port without switch.  Make sure the first USB 3.0 port is the same type.
                    //
                    if (connectorMapNode->Usb30Port->PortInformation.PortFlags.PortFlagTypeCWithoutSwitch == 0) {
                        DbgTraceTo(PortContext->IfrLog,
                                   TL_ERROR,
                                   HwcPlatform,
                                   "HW_COMPLIANCE_PLATFORM: USB 3.0 Type C without switch port mapped to a different type of USB 3.0 port %x!",
                                   connectorMapNode->Usb30Port->AcpiUpcPackage.PortConnectorType);
                        NT_ASSERTMSG("USB 3.0 Type C without switch port mapped to a different type of USB 3.0 port!", 0);
                        status = STATUS_UNSUCCESSFUL;
                        LEAVE;
                    }

                    //
                    // The instance ID for a device plugged into either of the two USB 3.0 ports on a Type C connectors
                    // should be the same.  Use the lowest of the two port numbers regardless of which port the device
                    // is plugged into.
                    //
                    PortNumberForInstanceId = min(connectorMapNode->Usb30Port->PortInformation.PortNumber,
                                                  PortContext->PortInformation.PortNumber);

                    PortContext->PortInformation.PortNumberForInstanceId = PortNumberForInstanceId;
                    connectorMapNode->Usb30Port->PortInformation.PortNumberForInstanceId = PortNumberForInstanceId;

                    connectorMapNode->Usb30SecondPort = PortContext;
                    break;

                } else {
                    DbgTraceTo(PortContext->IfrLog,
                               TL_ERROR,
                               HwcPlatform,
                               "HW_COMPLIANCE_PLATFORM: Multiple USB 3.0 ports mapped to the same connector!");
                    NT_ASSERTMSG("Multiple USB 3.0 ports mapped to the same connector!", 0);
                    status = STATUS_UNSUCCESSFUL;
                    LEAVE;
                }
            }

            if (PortContext->PortInformation.PortFlags.PortFlagTypeCWithoutSwitch == 1) {
                if (connectorMapNode->Usb20Port != NULL) {
                
                    //
                    // First USB 3.0 Type C port without switch.  Make sure the corresponding USB 2.0 port is the same type.
                    //
                    if (connectorMapNode->Usb20Port->PortInformation.PortFlags.PortFlagTypeCWithoutSwitch == 0) {
                        DbgTraceTo(PortContext->IfrLog,
                                    TL_ERROR,
                                    HwcPlatform,
                                    "HW_COMPLIANCE_PLATFORM: USB 2.0 Type C with switch port mapped to a different type of USB 3.0 port %x!",
                                    connectorMapNode->Usb20Port->AcpiUpcPackage.PortConnectorType);
                        NT_ASSERTMSG("USB 2.0 Type C with switch port mapped to a different type of USB 3.0 port!", 0);
                        status = STATUS_UNSUCCESSFUL;
                        LEAVE;
                    }
                }

                PortContext->PortInformation.PortNumberForInstanceId = PortContext->PortInformation.PortNumber;
            }
            
            connectorMapNode->Usb30Port = PortContext;
            break;
        
        default:
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       HwcPlatform,
                       "HW_COMPLIANCE_PLATFORM: Unknown USB protocol %d specified for port!",
                       PortContext->PortInformation.PortProtocol);
            NT_ASSERTMSG("Unknown protocol specified for port", FALSE);
            LEAVE;
        }

        INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &PortContext->PortInformation.PortFlags, PortFlagRegisteredWithConnectorMap);

    } FINALLY {
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Failed to register port with connector map %!STATUS!",
                       status);
        }        
        
        HUBCONNECTOR_UnlockConnectorMap();
    }

    return status;
}

        
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBCONNECTOR_DeRegisterPort(
    __in
        PPORT_CONTEXT       PortContext
        )
/*++

Routine Description:

    Unregisters a port with the connector map.

Arguments:
    
    PortContext - Pointer to the port context structure.  It is assumed that the connector ID
                  structure within the port context is valid.

Return Value:

    NTSTATUS

--*/
{
    PCONNECTOR_MAP_NODE     connectorMapNode;
    NTSTATUS                status;
    
    PAGED_CODE ();

    TRY {

        status = STATUS_SUCCESS;

        //
        // Lock our global list of connector map nodes
        //
        HUBCONNECTOR_LockConnectorMap();

        if (PortContext->PortInformation.PortFlags.PortFlagRegisteredWithConnectorMap == 0) {
            LEAVE;
        }

        //
        // Find the matching connector node
        //
        connectorMapNode = HUBCONNECTOR_LookupConnectorId(&PortContext->ConnectorId);

        if (connectorMapNode == NULL) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Unable to find connector for registered port");
 
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        //
        // Remove the hub context and port from the connector node
        //
        switch (PortContext->PortInformation.PortProtocol) {
        case PortProtocolUsb20:
            NT_ASSERT(connectorMapNode->Usb20Port != NULL);
            connectorMapNode->Usb20Port = NULL;
            break;

        case PortProtocolUsb30:
            NT_ASSERT(connectorMapNode->Usb30Port != NULL);
            connectorMapNode->Usb30Port = NULL;
            break;

        default:
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       HwcPlatform,
                       "HW_COMPLIANCE_PLATFORM: Unknown USB protocol %d specified for port!",
                       PortContext->PortInformation.PortProtocol);
            NT_ASSERTMSG("Unknown protocol specified for port", FALSE);
            LEAVE;
        }
            
        //
        // Free the connector node if ports are no longer registered
        //
        if ((connectorMapNode->Usb20Port == NULL) && 
            (connectorMapNode->Usb30Port == NULL)) {

            RemoveEntryList(&connectorMapNode->ListEntry);
            ExFreePoolWithTag(connectorMapNode, USBHUB3_PORT_TAG);
        }

        INTERLOCKED_CLEAR_BIT_FIELD(PORT_FLAGS, &PortContext->PortInformation.PortFlags, PortFlagRegisteredWithConnectorMap);

    } FINALLY {
        HUBCONNECTOR_UnlockConnectorMap();
    }

    return status;
}

FORCEINLINE
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_LockConnectorMap(
        )
/*++

Routine Description:

    Locks the connector map.

--*/
{
    PAGED_CODE ();

    WdfWaitLockAcquire(GetHubGlobals(WdfGetDriver())->ConnectorMapLock, NULL);
}


FORCEINLINE
__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_UnlockConnectorMap(
        )
/*++

Routine Description:

    Unlocks the connector map.

--*/
{
    PAGED_CODE ();

    WdfWaitLockRelease(GetHubGlobals(WdfGetDriver())->ConnectorMapLock);
}

__drv_maxIRQL(PASSIVE_LEVEL)
PPORT_CONTEXT
HUBCONNECTOR_GetCompanionPort(
    __in
        PPORT_CONTEXT       PortContext,
    __in
        USHORT              CompanionIndex
        )
/*++

Routine Description:

    Returns the port that shares a connector with the specified port.

    It is assumed that the connector map lock has already been acquired
    by the caller.

Arguments:
    
    PortContext - Port context for the port.

    CompanionIndex - Index of the companion port to return.
    
Return Value:

    Pointer to the companion port's context if found, NULL otherwise

--*/
{
    PCONNECTOR_MAP_NODE     connectorMapNode;
    PPORT_CONTEXT           companionPort;
    
    PAGED_CODE();

    TRY {

        companionPort = NULL;

        if (PortContext->PortInformation.PortFlags.PortFlagRegisteredWithConnectorMap == 0) {
            LEAVE;
        }

        if (CompanionIndex > 1) {
            DbgTraceTo(PortContext->IfrLog,
                        TL_ERROR,
                        HwcPlatform,
                        "HW_COMPLIANCE_PLATFORM: Invalid companion index %d specified!",
                        PortContext->PortInformation.PortProtocol);
            NT_ASSERTMSG("Invalid companion index %d specified", FALSE);
            LEAVE;
        }

        connectorMapNode = HUBCONNECTOR_LookupConnectorId(&PortContext->ConnectorId);

        if (connectorMapNode == NULL) {
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       Port,
                       "Unable to find connector for registered port");

            LEAVE;
        }

        switch (PortContext->PortInformation.PortProtocol) {
        case PortProtocolUsb20:
            switch (CompanionIndex) {
            case 0:
                companionPort = connectorMapNode->Usb30Port;
                break;

            case 1:
                companionPort = connectorMapNode->Usb30SecondPort;
                break;
            }
            break;

        case PortProtocolUsb30:
            switch (CompanionIndex) {
            case 0:
                companionPort = connectorMapNode->Usb20Port;
                break;
            
            case 1:
                if (PortContext == connectorMapNode->Usb30Port) {
                    companionPort = connectorMapNode->Usb30SecondPort;
                } else {
                    companionPort = connectorMapNode->Usb30Port;
                }
                break;
            }
            break;

        default:
            DbgTraceTo(PortContext->IfrLog,
                       TL_ERROR,
                       HwcPlatform,
                       "HW_COMPLIANCE_PLATFORM: Unknown USB protocol %d specified for port!",
                       PortContext->PortInformation.PortProtocol);
            NT_ASSERTMSG("Unknown protocol specified for port", FALSE);
            LEAVE;
        }

    } FINALLY {
    }

    return companionPort;
}




__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_MapRootHubPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        )
/*++

Routine Description:

    Creates connector mapping for ports on a root hub which ports do not have 
    valid _PLD packages in ACPI namespace.

Arguments:
    
    HubFdoContext - Context structure for the hub FDO object.
    
Return Value:

    VOID

--*/
{
    USHORT              current20PortNumber;
    USHORT              current30PortNumber;
    USHORT              last20PortNumber;
    USHORT              last30PortNumber;
    NTSTATUS            status;

    PAGED_CODE ();

    TRY {
        
        status = STATUS_SUCCESS;

        current20PortNumber = HubFdoContext->HubInformation.First20PortNumber;
        current30PortNumber = HubFdoContext->HubInformation.First30PortNumber;
        last20PortNumber = HubFdoContext->HubInformation.Last20PortNumber;
        last30PortNumber = HubFdoContext->HubInformation.Last30PortNumber;

        while ((current30PortNumber <= last30PortNumber) && 
               (current20PortNumber <= last20PortNumber)) {

            PPORT_CONTEXT  current20Port;
            PPORT_CONTEXT  current30Port;

            current20Port = HUBFDO_FindPortContextFromNumber(HubFdoContext, current20PortNumber);

            if (current20Port == NULL) {
               current20PortNumber++;
               current30PortNumber++;
               continue;
            }

            if (current20Port->PortInformation.PortFlags.PortFlagIntegratedHubImplemented == 1) {
                
                //
                // The USB 2.0 port has an integrated hub.  To reduce complexity of the connector
                // mapping code, we currently only support a single integrated hub on the root hub.
                // The integrated hub must reside on the last USB 2.0 port on the root hub.  This
                // allows us to know which USB 3.0 root ports will be mapped to the integrated
                // hub prior to it being started.  
                //
                if (current20PortNumber != last20PortNumber) {
                    DbgTraceTo(current20Port->IfrLog,
                               TL_ERROR,
                               HwcPlatform,
                               "HW_COMPLIANCE_PLATFORM: Integrated hub is on a port that is not the last USB 2.0 port on the root");

                    NT_ASSERTMSG("Integrated hub on port that is not the last USB 2.0 port on the root", FALSE);

                    LEAVE;
                }

                //
                // Store the range of 3.0 ports that will be mapped to the integrated hub
                // in the port context.  These will be propagated to the child hub via the
                // hub/parent interface.
                // 
                current20Port->PortInformation.FirstMapped30PortNumber = current30PortNumber;
                current20Port->PortInformation.LastMapped30PortNumber = last30PortNumber;

                //
                // Setup the base component of the connector ID, to be propagated to the child hub.
                //
                current20Port->ConnectorId.Type = RootConnectorTypeNonAcpi;
                current20Port->ConnectorId.HubNumber = HubFdoContext->HubInformation.HubNumber;

                //
                // Register the remaining 3.0 ports on the root with the connector map
                //                
                while (current30PortNumber <= last30PortNumber) {

                    //
                    // Find the port context for the 3.0 port
                    //
                    current30Port = HUBFDO_FindPortContextFromNumber(HubFdoContext, 
                                                                     current30PortNumber);

                    if (current30Port == NULL) {
                        current30PortNumber++;
                        continue;
                    }

                    current30Port->ConnectorId.Type = RootConnectorTypeNonAcpi;
                    current30Port->ConnectorId.HubNumber = HubFdoContext->HubInformation.HubNumber;
                    current30Port->ConnectorId.ConnectorPath[0] = current30PortNumber;
                    current30Port->ConnectorId.Depth = 0;

                    //
                    // Register the 3.0 port with the connector map
                    // 
                    status = HUBCONNECTOR_RegisterPort(current30Port);

                    if (!NT_SUCCESS(status)) {
                        LEAVE;
                    }

                    current30PortNumber++;
                }

                LEAVE;

            } else {

                //
                // No integrated hub, therefore a 1-to-1 mapping between 2.0 and 3.0 ports exists
                //
                current20Port->ConnectorId.Type = RootConnectorTypeNonAcpi;
                current20Port->ConnectorId.HubNumber = HubFdoContext->HubInformation.HubNumber;
                current20Port->ConnectorId.ConnectorPath[0] = current30PortNumber;
                current20Port->ConnectorId.Depth = 0;

                if ((current20Port->PortInformation.PortFlags.PortFlagRemovable == 1)) {

                    //
                    // Register the 2.0 port with the connector map
                    // 
                    status = HUBCONNECTOR_RegisterPort(current20Port);

                    if (!NT_SUCCESS(status)) {
                        LEAVE;
                    }

                    //
                    // Find the port context for the corresponding 3.0 port
                    //
                    current30Port = HUBFDO_FindPortContextFromNumber(HubFdoContext,
                                                                     current30PortNumber);

                    if (current30Port == NULL) {
                        DbgTraceTo(current20Port->IfrLog,
                                   TL_ERROR,
                                   HwcPlatform,
                                   "HW_COMPLIANCE_PLATFORM: USB 2.0 port %d should have a matching USB 3.0 companion port, but one could not be found!",
                                   current20PortNumber);
                        
                        current20PortNumber++;
                        current30PortNumber++;
                        continue;
                    }

                    current30Port->ConnectorId.Type = RootConnectorTypeNonAcpi;
                    current30Port->ConnectorId.HubNumber = HubFdoContext->HubInformation.HubNumber;
                    current30Port->ConnectorId.ConnectorPath[0] = current30PortNumber;
                    current30Port->ConnectorId.Depth = 0;

                    if(current30Port->PortInformation.PortFlags.PortFlagRemovable != 1) {
                        DbgTraceTo(current20Port->IfrLog,
                                   TL_ERROR,
                                   HwcPlatform,
                                   "HW_COMPLIANCE_PLATFORM: USB 2.0 port %d is marked as removable but its USB 3.0 companion port %d is not marked as removable!",
                                   current20PortNumber,
                                   current30PortNumber);
                    }
                    NT_ASSERT(current30Port->PortInformation.PortFlags.PortFlagRemovable == 1);
                    
                    //
                    // Register the 3.0 port with the connector map
                    // 
                    status = HUBCONNECTOR_RegisterPort(current30Port);

                    if (!NT_SUCCESS(status)) {
                        LEAVE;
                    }
                }
            }

            current20PortNumber++;
            current30PortNumber++;
        }
    
    } FINALLY {

    }
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_MapIntegratedHubPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        )
/*++

Routine Description:

    Creates connector mappings for a USB 2.0 integrated hub connected to the root hub.

Arguments:
    
    HubFdoContext - Context structure for the hub FDO object.
    
Return Value:

    VOID

--*/
{
    USHORT              i;
    USHORT              current30PortNumber;
    USHORT              last30PortNumber;
    NTSTATUS            status;
    PPORT_CONTEXT       current20Port;

    PAGED_CODE ();

    TRY {
        
        //
        // Retrieve the range of 3.0 ports to be mapped to this integrated hub
        //
        current30PortNumber = HubFdoContext->HubParentInterface.First30PortToMap;
        last30PortNumber = HubFdoContext->HubParentInterface.Last30PortToMap;
        
        for (i=1; 
             (i <= HubFdoContext->HubInformation.NumberOfPorts) && 
               (current30PortNumber <= last30PortNumber);
             i++, current30PortNumber++) {

            current20Port = HUBFDO_FindPortContextFromNumber(HubFdoContext, i);

            if (current20Port == NULL) {
               LEAVE;
            }

            RtlCopyMemory(&current20Port->ConnectorId,
                          HubFdoContext->HubParentInterface.ConnectorId,
                          sizeof(CONNECTOR_ID));

            current20Port->ConnectorId.ConnectorPath[0] = current30PortNumber;

            if ((current20Port->PortInformation.PortFlags.PortFlagRemovable == 1)) {

                //
                // Register the 2.0 port with the connector map
                // 
                status = HUBCONNECTOR_RegisterPort(current20Port);
                
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }
            }
        }
    } FINALLY {

    }
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_MapExternalHubPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        )
/*++

Routine Description:

    Creates connector mappings for an external USB hub.

Arguments:
    
    HubFdoContext - Context structure for the hub FDO object.
    
Return Value:

    VOID

--*/
{
    USHORT              i;
    NTSTATUS            status;

    PAGED_CODE ();

    TRY {
        
        for (i=1; 
             (i <= HubFdoContext->HubInformation.NumberOfPorts);
             i++) {

            PPORT_CONTEXT  currentPort;

            currentPort = HUBFDO_FindPortContextFromNumber(HubFdoContext, i);

            if (currentPort == NULL) {
               LEAVE;
            }

            RtlCopyMemory(&currentPort->ConnectorId,
                          HubFdoContext->HubParentInterface.ConnectorId,
                          sizeof(CONNECTOR_ID));
            
            currentPort->ConnectorId.Depth++;
            currentPort->ConnectorId.ConnectorPath[currentPort->ConnectorId.Depth] = i;

            if ((currentPort->PortInformation.PortFlags.PortFlagRemovable == 1)) {
                //
                // Register the port with the connector map
                // 
                status = HUBCONNECTOR_RegisterPort(currentPort);

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }
            }
        }
    } FINALLY {

    }
}


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBCONNECTOR_MapAcpiPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        )
/*++

Routine Description:

Return Value:

    NTSTATUS

--*/
{
    PPORT_CONTEXT       currentPort;
    NTSTATUS            status;

    PAGED_CODE ();

    TRY {
    
        status = STATUS_SUCCESS;
        
        FOR_ALL_IN_LIST(PORT_CONTEXT,
                        &HubFdoContext->MuxContext.PSMListHead,
                        MuxContext.ListEntry,
                        currentPort) {

            if ((currentPort->PortInformation.PortFlags.PortFlagAcpiPldValid == 1) && 
                (currentPort->PortInformation.PortFlags.PortFlagRemovable == 1)) {

                //
                // Any ports on this hub with visible connectors is considered a "root connector".
                // Their connector ID is always a root type of ACPI, along with their 128-bit _PLD package.
                //
                currentPort->ConnectorId.Type = RootConnectorTypeAcpi;
                currentPort->ConnectorId.AcpiPldBuffer = currentPort->AcpiPldPackage;

                //
                // Derive a USB Connector Manager connector ID for this port in case it is
                // associated with a Type-C connector.
                //
                currentPort->ConnectorId.UcmConnectorId =
                    UCM_CONNECTOR_ID_FROM_ACPI_PLD(&currentPort->ConnectorId.AcpiPldBuffer);

                status = HUBCONNECTOR_RegisterPort(currentPort);

                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }
            }
        }
    } FINALLY {
 
    }

    return status;
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_MapHubPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        )
/*++

Routine Description:

Return Value:

    VOID

--*/
{
    NTSTATUS    status;

    PAGED_CODE();

    TRY {

        if (HubFdoContext->HubInformation.HubFlags.InAcpiNamespace == 1) {

            //
            // Map Ports For an ACPI Hub
            //
            status = HUBCONNECTOR_MapAcpiPorts(HubFdoContext);

            //
            // If the ACPI description of the ports is not valid, un-map whatever ports
            // got mapped and fallback to the default mapping for non-ACPI hubs.
            //
            if (!NT_SUCCESS(status)) {
                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           HwcPlatform,
                           "HW_COMPLIANCE_PLATFORM: ACPI Port Description is Invalid (%!STATUS!)",
                           status);

                HUBCONNECTOR_UnMapHubPorts(HubFdoContext);

                INTERLOCKED_CLEAR_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, InAcpiNamespace);

            } else {
                LEAVE;
            }
        }
        
        if (HubFdoContext->HubParentInterface.HubDepth == 0) {

            //
            // Map ports for a non-ACPI root hub
            //
            HUBCONNECTOR_MapRootHubPorts(HubFdoContext);
            LEAVE;
        }

        if (HubFdoContext->HubParentInterface.First30PortToMap != 0) {

            //
            // Map ports for an integrated USB 2.0 hub on the root hub
            //
            HUBCONNECTOR_MapIntegratedHubPorts(HubFdoContext);
            LEAVE;
        }

        //
        // Map ports for an external hub
        //
        HUBCONNECTOR_MapExternalHubPorts(HubFdoContext);
    
    } FINALLY {

    }

}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBCONNECTOR_UnMapHubPorts(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext
        )
/*++

Routine Description:

Return Value:

    VOID

--*/
{
    USHORT          portNumber;
    PPORT_CONTEXT   currentPort;
    
    PAGED_CODE();

    TRY {
        
        for (portNumber = HubFdoContext->HubInformation.First20PortNumber;
             portNumber <= HubFdoContext->HubInformation.Last20PortNumber;
             portNumber++) {

            currentPort = HUBFDO_FindPortContextFromNumber(HubFdoContext, portNumber);

            if (currentPort == NULL) {
                continue;
            }

            HUBCONNECTOR_DeRegisterPort(currentPort);
        }
            

        for (portNumber = HubFdoContext->HubInformation.First30PortNumber;
             portNumber <= HubFdoContext->HubInformation.Last30PortNumber;
             portNumber++) {

            currentPort = HUBFDO_FindPortContextFromNumber(HubFdoContext, portNumber);
            
            if (currentPort == NULL) {
                continue;
            }

            HUBCONNECTOR_DeRegisterPort(currentPort);
        }
    
    } FINALLY {

    }

}


__drv_maxIRQL(PASSIVE_LEVEL)
PCONNECTOR_MAP_NODE
HUBCONNECTOR_GetConnectorMapNodeForPort(
    __in
        PPORT_CONTEXT       PortContext
        )
/*++

Routine Description:

    Returns the connector map node for a given port.  

Arguments:
    
    PortContext - Pointer to the port context structure.

Return Value:

    Connector map node address for the port, or NULL if there is none.

--*/
{
    PCONNECTOR_MAP_NODE     connectorMapNode;
    
    PAGED_CODE ();

    TRY {

        connectorMapNode = NULL;

        if (PortContext->PortInformation.PortFlags.PortFlagRegisteredWithConnectorMap == 0) {
            LEAVE;
        }

        HUBCONNECTOR_LockConnectorMap();

        connectorMapNode = HUBCONNECTOR_LookupConnectorId(&PortContext->ConnectorId);

        HUBCONNECTOR_UnlockConnectorMap();

    } FINALLY {
    
    }

    return connectorMapNode;
}
