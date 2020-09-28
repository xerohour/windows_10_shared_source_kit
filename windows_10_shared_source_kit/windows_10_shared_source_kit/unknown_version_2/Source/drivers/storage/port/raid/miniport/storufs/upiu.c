/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    upiu.c

Abstract:

    Handles all I/O related operations (such as StartIO).

Author:

    Erik Schmidt (eriksch) - October 2012

Environment:



Revision History:

--*/

#include "storufs.h"

BOOLEAN
UfsBuildIo (
    _In_ PVOID AdapterExtension,
    _In_ PSCSI_REQUEST_BLOCK Srb
    )
/*+++

Routine Description:

    This routine will be used to perform any operations that can be done
    asynchronously when building an I/O request. Will be implemented once
    driver is stabilized and performance is required.

Arguments:

    AdapterExtension - The UFS Adapter Extension structure

    Srb - The SCSI Request Block to be passed onto the UFS Device

Return Value:

    Returns TRUE if the I/O was built successfully, FALSE otherwise.

--*/

{
    UNREFERENCED_PARAMETER(AdapterExtension);
    UNREFERENCED_PARAMETER(Srb);

    return TRUE;
}


BOOLEAN
UfsStartIo(
    _In_reads_bytes_(sizeof(UFS_ADAPTER_EXTENSION)) PVOID DeviceExtension,
    _In_ PSCSI_REQUEST_BLOCK Srb
    )
/*+++

Routine Description:

    Dispatcher function for handling an I/O request.

Arguments:

    DeviceExtension - The UFS Adapter Extension structure

    Srb - The SCSI Request Block to be passed onto the UFS Device

Return Value:

    Returns TRUE if the I/O was processed successfully, FALSE otherwise.

--*/
{
    UCHAR function = Srb->Function, success;
    PUFS_ADAPTER_EXTENSION adapterExtension = (PUFS_ADAPTER_EXTENSION)DeviceExtension;
    ULONG i;
    BOOLEAN sendNotification = FALSE;
    PUFS_TRANSFER_REQUEST_LIST trl = &adapterExtension->Trl;
    STOR_PNP_ACTION pnpAction;
    PSCSI_PNP_REQUEST_BLOCK pnpSrb;
    PSTOR_DEVICE_CAPABILITIES_EX pStorCaps;
    ULONG status;
    PUFS_DUMP_CONTEXT dumpContext;
    PMINIPORT_DUMP_POINTERS dumpPointers;

    switch (function) {

    case SRB_FUNCTION_EXECUTE_SCSI:

        //
        // Execute a SCSI operation by adding an entry to the TRL
        //

        if (UfsPreProcessSrb(adapterExtension, Srb) == FALSE) {
            sendNotification = TRUE;
            break;
        }

        if (InterlockedAnd(&adapterExtension->BusReset, UfsBusResetMode) == 0) {
            //
            // Build the UTP Transfer Request Descriptor and send it to the Host
            // Controller to be executed.
            //

            success = UfsSendTransferRequest(adapterExtension, Srb);
        } else {
            success = SRB_STATUS_BUSY;
        }

        if (success != SRB_STATUS_SUCCESS) {
            Srb->SrbStatus = success;
            // LogError: "No Transfer Request Descriptors available for SRB:0x%0IX\n"
            sendNotification = TRUE;
        }

        break;

    case SRB_FUNCTION_IO_CONTROL:

        Srb->SrbStatus = SRB_STATUS_INVALID_REQUEST;
        sendNotification = TRUE;
        break;

    case SRB_FUNCTION_ABORT_COMMAND:
    case SRB_FUNCTION_TERMINATE_IO:

        NT_ASSERT(FALSE);

        success = SRB_STATUS_ERROR;

        for (i = 0; i < adapterExtension->Trl.ListSize; i++){
            if ((trl->ActiveSrb[i] != NULL) &&
                (Srb->Length == trl->ActiveSrb[i]->Length)) {

                if (RtlCompareMemory(Srb->NextSrb,
                                     trl->ActiveSrb[i],
                                     Srb->Length) == Srb->Length) {

                    success = UfsSendTaskManRequest(adapterExtension,
                                                    i,
                                                    Srb->NextSrb->Lun,
                                                    UPIU_TM_FUNCTION_ABORT,
                                                    Srb);
                    break;
                }
            }
        }

        if (success != SRB_STATUS_SUCCESS) {
            Srb->SrbStatus = success;
            sendNotification = TRUE;

            // LogError: "No Task Management Request Descriptors available for SRB:0x%0IX\n"
        }

        break;

    case SRB_FUNCTION_RESET_DEVICE:

        NT_ASSERT(FALSE);
        UfsResetBus(adapterExtension, 0);
        Srb->SrbStatus = SRB_STATUS_SUCCESS;
        sendNotification = TRUE;

        break;

    case SRB_FUNCTION_DUMP_POINTERS:

        status = STOR_STATUS_SUCCESS;
        dumpContext = NULL;
        dumpPointers = (PMINIPORT_DUMP_POINTERS)Srb->DataBuffer;

        if ((dumpPointers == NULL) ||
            (Srb->DataTransferLength < sizeof(MINIPORT_DUMP_POINTERS))) {

            Srb->SrbStatus = SRB_STATUS_INVALID_REQUEST;
            sendNotification = TRUE;
            break;
        }

        status = StorPortAllocatePool(adapterExtension,
                                      sizeof(UFS_DUMP_CONTEXT),
                                      UFS_POOL_TAG,
                                      (PVOID*)&dumpContext);

        if ((status != STOR_STATUS_SUCCESS) || (dumpContext == NULL)) {
            Srb->SrbStatus = SRB_STATUS_ERROR;
            sendNotification = TRUE;
            break;
        }

        dumpPointers->Version = DUMP_MINIPORT_VERSION_1;
        dumpPointers->Size = sizeof(MINIPORT_DUMP_POINTERS);

        dumpPointers->MiniportPrivateDumpData = (PVOID)dumpContext;

        dumpContext->VendorID = adapterExtension->PciVendorID;
        dumpContext->DeviceID = adapterExtension->PciDeviceID;
        dumpContext->RevisionID = adapterExtension->PciRevisionID;
        dumpContext->UfsBar = adapterExtension->UfsBar;
        dumpContext->Trd = adapterExtension->Trl.List;
        dumpContext->Tmrd = adapterExtension->Tmrl.List;
        dumpContext->TrlAddr = adapterExtension->Trl.PhysicalAddr;
        dumpContext->TmrlAddr = adapterExtension->Tmrl.PhysicalAddr;

        NT_ASSERT(trl->ListSize <= MAX_REQUEST_LIST_SIZE);
        RtlCopyMemory(&dumpContext->UcdList[0],
                      &trl->UcdList[0],
                      sizeof(UFS_UCD_ADDRESS) * trl->ListSize);

        dumpContext->FeatureFlags = adapterExtension->FeatureFlags;
        
        RtlCopyMemory(&dumpContext->RegValues,
                      &adapterExtension->RegValues,
                      sizeof(UFS_REG_VALUES));

        sendNotification = TRUE;
        Srb->SrbStatus = SRB_STATUS_SUCCESS;
        break;

    case SRB_FUNCTION_FREE_DUMP_POINTERS:

        dumpPointers = (PMINIPORT_DUMP_POINTERS)Srb->DataBuffer;

        if ((dumpPointers == NULL) ||
            (Srb->DataTransferLength < sizeof(MINIPORT_DUMP_POINTERS))) {

            Srb->SrbStatus = SRB_STATUS_INVALID_REQUEST;
            sendNotification = TRUE;
            break;
        }

        UfsFreeContiguousMemory(adapterExtension,
                                sizeof(UFS_DUMP_CONTEXT),
                                dumpPointers->MiniportPrivateDumpData);
        Srb->SrbStatus = SRB_STATUS_SUCCESS;
        sendNotification = TRUE;
        break;

    case SRB_FUNCTION_RESET_LOGICAL_UNIT:

        NT_ASSERT(FALSE);

        UfsClearOutstandingRequests(adapterExtension,
                                    Srb->Lun);

        success = UfsSendTaskManRequest(adapterExtension,
                                        0,
                                        Srb->Lun,
                                        UPIU_TM_FUNCTION_LU_RESET,
                                        Srb);
        if (success == FALSE) {
            Srb->SrbStatus = SRB_STATUS_BUSY;
            sendNotification = TRUE;

            // LogError: "Error Resetting Lun:0x%08x for SRB:0x%0IX\n"
        }

        break;

    case SRB_FUNCTION_SHUTDOWN:

        Srb->SrbStatus = SRB_STATUS_SUCCESS;
        sendNotification = TRUE;
        break;

    case SRB_FUNCTION_PNP:

        pnpSrb = (PSCSI_PNP_REQUEST_BLOCK)Srb;
        pStorCaps = (PSTOR_DEVICE_CAPABILITIES_EX)pnpSrb->DataBuffer;
        pnpAction = pnpSrb->PnPAction;
        Srb->SrbStatus = SRB_STATUS_INVALID_REQUEST;

        if (((pnpSrb->SrbPnPFlags & SRB_PNP_FLAGS_ADAPTER_REQUEST) == 0) &&
            (pnpAction == StorQueryCapabilities))
        {
            pStorCaps->Removable = 0;
            pStorCaps->DeviceD1 = 0;
            pStorCaps->DeviceD2 = 0;
            pStorCaps->LockSupported = 0;
            pStorCaps->EjectSupported = 0;
            pStorCaps->DockDevice = 0;
            pStorCaps->UniqueID = 0;
            pStorCaps->SilentInstall = 1;
            pStorCaps->SurpriseRemovalOK = 0;
            if (Srb->Lun != 0) {
                pStorCaps->NoDisplayInUI = 1;
            } else {
                pStorCaps->NoDisplayInUI = 0;
            }

            Srb->SrbStatus = SRB_STATUS_SUCCESS;
        }
        else if ((pnpSrb->SrbPnPFlags & SRB_PNP_FLAGS_ADAPTER_REQUEST) &&
                 ((pnpAction == StorRemoveDevice) ||
                  (pnpAction == StorSurpriseRemoval))) {
            //
            // The adapter is going to be removed, so release all the resources
            // allocated for it, and free all allocated memory.
            //

            UfsFreeResources(adapterExtension);
            Srb->SrbStatus = SRB_STATUS_SUCCESS;
        } else if ((pnpSrb->SrbPnPFlags & SRB_PNP_FLAGS_ADAPTER_REQUEST) &&
                   (pnpAction == StorStopDevice)) {

            UfsStopAdapter(adapterExtension);
            Srb->SrbStatus = SRB_STATUS_SUCCESS;
        } else {
            Srb->SrbStatus = SRB_STATUS_INVALID_REQUEST;
        }

        sendNotification = TRUE;
        break;

    default:

        sendNotification = TRUE;
        // LogError: "Illegal Function:%08x for SRB:0x%0IX\n"
        Srb->SrbStatus = SRB_STATUS_INVALID_REQUEST;
    }

    if (sendNotification != FALSE) {
        StorPortNotification(RequestComplete,
                             adapterExtension,
                             Srb);
    }

    return TRUE;
}

BOOLEAN
UfsPreProcessSrb(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PSCSI_REQUEST_BLOCK Srb
    )
/*++

Routine Description:

    This routine handles any pre-processing of the SCSI Request Block. This
    includes checking whether the LUN Id is valid or whether a valid command
    is being sent to a Well-Defined LUN.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    Srb - the SCSI Request Block

Return Value:

    Returns FALSE if the SCSI Request Block should not be forwarded to a
    device, TRUE if no error is found. If false is returned, the SRB status is
    set to SRB_STATUS_ERROR.

--*/
{
    PCDB origCdb = (PCDB)&Srb->Cdb[0];

    UNREFERENCED_PARAMETER(AdapterExtension);

    //
    // Make sure that the Report Luns command will only return non
    // Well-Defined LUNs
    //

    if (origCdb->REPORT_LUNS.OperationCode == SCSIOP_REPORT_LUNS) {
        Srb->Cdb[2] = REPORT_LUNS_RESTRICTED;
    } 

    return TRUE;
}

VOID
UfsPostProcessSrb(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PSCSI_REQUEST_BLOCK Srb,
    _In_ ULONG Index
    )
/*++

Routine Description:

    This routine handles any post-processing of the SCSI Request Block. This
    includes removing the boot and RPMB LUNs from the Report LUNs command.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    Srb - the SCSI Request Block

Return Value:

    None

--*/

{
    UNREFERENCED_PARAMETER(Index);
    UNREFERENCED_PARAMETER(AdapterExtension);

    if ((Srb->SrbStatus != SRB_STATUS_SUCCESS) ||
        (Srb->ScsiStatus != SCSISTAT_GOOD)) {

        return;
    }

    //
    // TODO: Figure out the LUN layout/ access requirements
    //

#if 0
    PCDB origCdb = (PCDB)&Srb->Cdb[0];
    PUFS_UPIU_PRDT prdt;
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    PUFS_HCI_TRD trd;
    PVOID cmdBaseAddr;
    UCHAR maxLunID = 0;
    ULONG i, trlResources;
    PSCSI_REPORT_LUNS_DATA reportLuns;
    UFS_QUERY_OPCODE opcode = {0};
    STOR_PHYSICAL_ADDRESS physAddr;
    UFS_UNIT_DESCRIPTOR unitDescriptor = {0};
    ULONGLONG maxLunSize = 0, curLunSize;

    //
    // Make sure that the Report Luns command will only return the Data LUN
    //

    if (origCdb->REPORT_LUNS.OperationCode == SCSIOP_REPORT_LUNS) {
        trd = &trl->List[Index];

        cmdBaseAddr = trl->UcdList[Index].VirtualAddress;
        prdt = (PUFS_UPIU_PRDT)((PUCHAR)cmdBaseAddr + (trd->PRDTOff * sizeof(ULONG)));

        NT_ASSERT(trd->PRDTLen == 1);

        physAddr.LowPart = prdt->BaseAddr;
        physAddr.HighPart = prdt->BaseAddrUpper;

        reportLuns = (PSCSI_REPORT_LUNS_DATA)StorPortGetVirtualAddress(AdapterExtension,
                                                                       physAddr);

        if (_byteswap_ulong(reportLuns->ListLength) == 8) {
            return;
        }

        //
        // Send a Query Unit Descriptor UPIU for LUNs 1-7 to determine the
        // largest enabled LUN
        //

        for (i = 0; i < MAX_NUM_LU; i++) {
            opcode.ReadDescOpcode.Opcode = UFS_UPIU_QUERY_OPCODE_READ_DESC;
            opcode.ReadDescOpcode.DescriptorIdn = UFS_DESC_UNIT_IDN;
            opcode.ReadDescOpcode.Index = (UCHAR)i;
            opcode.ReadDescOpcode.Selector = 0;
            opcode.ReadDescOpcode.Length = _byteswap_ushort(sizeof(UFS_UNIT_DESCRIPTOR));

            trlResources = UfsSendQueryUpiu(AdapterExtension,
                                            UFS_UPIU_QUERY_FUNCTION_READ,
                                            &opcode,
                                            sizeof(UFS_UNIT_DESCRIPTOR),
                                            (PUCHAR)&unitDescriptor);

            if (trlResources == 0) {
                NT_ASSERT(FALSE);
            }

            if (unitDescriptor.bLUEnable == 1) {
                curLunSize = _byteswap_uint64(*(PULONGLONG)&unitDescriptor.qLogicalBlockCount[0]);

                if (curLunSize > maxLunSize) {
                    maxLunID = unitDescriptor.bUnitIndex;
                    maxLunSize = curLunSize;
                }
            }
        }

        //
        // Fill out the Report Luns structure with the Data LUN
        //

        RtlZeroMemory(reportLuns, prdt->DataByteCount + 1);
        reportLuns->ListLength = _byteswap_ulong(sizeof(SCSI_LUN_RECORD));
        reportLuns->Record[0].Lun = maxLunID;

        Srb->DataTransferLength = sizeof(SCSI_LUN_RECORD) + 8;
    }
#endif
}

#pragma warning(push)
#pragma warning(disable: 26019)
UCHAR
UfsSendTaskManRequest(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG TaskTag,
    _In_ ULONG Lun,
    _In_ ULONG Function,
    _In_ PSCSI_REQUEST_BLOCK Srb
    )
/*++

Routine Description:

    Build a Task Management Request Descriptor. The function first locates an
    empty slot on the TMRL, and builds a request from there. It sends the
    request by ringing the doorbell register.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    TaskTag - The Task Tag of the task in the TRL (which is just the index in
              the TRL) that the task management operation will be performed on

    Lun - The Lun which the operation will be performed on

    Function - the type of task management operation (Abort, Reset LUN, etc.)

    Srb - the relevant SRB of the operation

Return Value:

    TRUE on success, FALSE on failure

Required IRQL:

    Dispatch

--*/
{
    ULONG index;
    PUFS_HCI_TMRD tmrd;
    PUFS_UPIU_TASK_MAN taskman;
    PUFS_TASKMAN_REQUEST_LIST tmrl = &AdapterExtension->Tmrl;
#if DBG
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_UTMRLRSR rsr;
#endif

    index = UfsAcquireTaskManBuildIndex(AdapterExtension);

    if (index == INVALID_INDEX) {
        return SRB_STATUS_BUSY;
    }

    NT_ASSERT(index < MAX_TASK_MAN_LIST_SIZE);

    tmrd = &tmrl->List[index];

    if (Srb != NULL) {
        tmrl->ActiveSrb[index] = Srb;
    } else {
        tmrl->ActiveSrb[index] = NULL;
    }

    //
    // Build the TMRD Header
    //

    tmrd->OverallCmdStatus = TMRD_OCS_INVALID_OCS_VALUE;
    tmrd->Interrupt = 1;

    //
    // Build the Task Management Request UPIU
    //

    taskman = &tmrd->Request;
    taskman->Header.TransactionType = UPIU_TASK_MAN_REQ;
    taskman->Header.Flags = 0;
    taskman->Header.LUN = (UCHAR)Lun;
    taskman->Header.TaskTag = (UCHAR)index;
    taskman->Header.QueryFunction = (UCHAR)Function;
    taskman->Header.EhsLength = 0;
    taskman->Header.DataSegmentLength = 0;
    taskman->Parameters[0] = _byteswap_ulong(Lun);
    if ((Function == UPIU_TM_FUNCTION_ABORT) ||
        (Function == UPIU_TM_FUNCTION_QUERY)) {
        taskman->Parameters[1] = _byteswap_ulong(TaskTag);
    }

#if DBG
    //
    // Check that the Task Man Request List Run-Stop Register is set to 1
    //
    rsr.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->TaskMgmt.RunStop.AsUlong);
    NT_ASSERT(rsr.TaskMgmtReqListRunStop == 1);
#endif

    UfsAddDbgLog(AdapterExtension,
                 'TMSe',
                 (ULONG_PTR)Srb,
                 index,
                 Function);

    //
    // Ring the Doorbell Register to indicate that the Transfer Request is
    // ready to be processed
    //

    UfsReleaseTaskManBuildIndex(AdapterExtension,
                                index);

    return SRB_STATUS_SUCCESS;
}

UCHAR
UfsSendTransferRequest(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PSCSI_REQUEST_BLOCK Srb
    )
/*++

Routine Description:

    Build a Transfer Request Descriptor. The function first locates an
    empty slot on the TRL, and builds a request from there. It sends the
    request by ringing the doorbell register.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    Srb - the SCSI Request Block

Return Value:

    TRUE on success, FALSE on failure

Required IRQL:

    Dispatch

--*/
{
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    ULONG index;
    PUFS_HCI_TRD trd;
    UFS_HCI_UTRLRSR rsr;
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    ULONG cmdLen, respLen, numScatterGatherEntries;
    PUFS_UPIU_COMMAND cmd;
    PUFS_UPIU_PRDT prdt;
    PSTOR_SCATTER_GATHER_LIST scatterGather;
    PUFS_UCD_ADDRESS ucd;

    NT_ASSERT(Srb != NULL);

#ifndef DBG
    UNREFERENCED_PARAMETER(rsr);
    UNREFERENCED_PARAMETER(reg);
#endif

    NT_ASSERT(AdapterExtension->Power.LinkState == UfsLinkActive);

    index = UfsAcquireTransferBuildIndex(AdapterExtension);

    if ((index == INVALID_INDEX) || (index >= AdapterExtension->Trl.ListSize)){
        return SRB_STATUS_BUSY;
    }

    NT_ASSERT(index < MAX_REQUEST_LIST_SIZE);

    trl->ActiveSrb[index] = Srb;

    //
    // Build a Transfer Request Descriptor at the empty slot
    //

    trd = &trl->List[index];

    RtlZeroMemory(trd, sizeof(UFS_HCI_TRD));

    trd->CmdType = TRD_CT_UFS_STORAGE;

    if ((Srb->SrbFlags & SRB_FLAGS_UNSPECIFIED_DIRECTION) == SRB_FLAGS_UNSPECIFIED_DIRECTION) {
        trd->DataDirection = TRD_DD_TARGET2SYS;
    } else if ((Srb->SrbFlags & SRB_FLAGS_DATA_IN) != 0) {
        trd->DataDirection = TRD_DD_TARGET2SYS;
    } else if ((Srb->SrbFlags & SRB_FLAGS_DATA_OUT) != 0) {
        trd->DataDirection = TRD_DD_SYS2TARGET;
    } else {
        trd->DataDirection = TRD_DD_NODATA;
    }

    //
    // Mark the I (Interrupt) Bit to true to be notified by interrupt of command
    // completion, rather than by interrupt aggregation
    //

    trd->Interrupt = 1;

    //
    // Initialize OCS with 0xF. If this is read back after completion, there was
    // an error handling the transfer request.
    //

    trd->OverallCmdStatus = TRD_OCS_INVALID_OCS_VALUE;

    //
    // Fill out the UFS Command Descriptor (UCD) for the Target SCSI command by
    // creating a Command UPIU, Response UPIU, and Physical Region Description
    // Table.
    //

    ucd = &trl->UcdList[index];

    scatterGather = StorPortGetScatterGatherList(AdapterExtension,
                                                 Srb);

    if (scatterGather == NULL) {
        numScatterGatherEntries = 0;
    } else {
        numScatterGatherEntries = scatterGather->NumberOfElements;
    }

    cmdLen = SIZEOF_UFS_UPIU_COMMAND;
    respLen = SIZEOF_UFS_UPIU_RESPONSE;

    //
    // Program the UCD Base Address/ Base Address Upper, Response Offset/Length,
    // and PRDT Offset/ Length Fields in the TRD
    //

    trd->CmdDescriptorBaseAddr = ucd->PhysicalAddress.LowPart;
    trd->CmdDescriptorBaseAddrUpper32 = ucd->PhysicalAddress.HighPart;
    trd->RespUPIULen = (respLen + sizeof(ULONG) - 1) / sizeof(ULONG);
    trd->RespUPIUOff = DWORD_ALIGNED(cmdLen) / sizeof(ULONG);
    trd->PRDTLen = numScatterGatherEntries;
    trd->PRDTOff = DWORD_ALIGNED(cmdLen + respLen) / sizeof(ULONG);

    RtlZeroMemory(ucd->VirtualAddress, MAX_SIZEOF_UFS_UPIU_UCD);

    cmd = (PUFS_UPIU_COMMAND)ucd->VirtualAddress;
    prdt = (PUFS_UPIU_PRDT)((PUCHAR)ucd->VirtualAddress + (trd->PRDTOff * sizeof(ULONG)));

    //
    // Initialize and fill in the Command UPIU
    //

    cmd->Header.TransactionType = UPIU_COMMAND;
    cmd->Header.LUN = Srb->Lun;
    cmd->Header.TaskTag = (UCHAR)index;
    cmd->Header.CmdSetType = UPIU_CMD_SET_SCSI;
    cmd->Header.EhsLength = 0;
    cmd->Header.DataSegmentLength = 0;
    cmd->Header.Flags = 0;
    if (trd->DataDirection == TRD_DD_SYS2TARGET) {
        cmd->Header.Flags |= UPIU_HEADER_FLAGS_W_MASK;
    } else if (trd->DataDirection == TRD_DD_TARGET2SYS) {
        cmd->Header.Flags |= UPIU_HEADER_FLAGS_R_MASK;
    }
    cmd->ExpectedDataLength = _byteswap_ulong(Srb->DataTransferLength);

    NT_ASSERT(Srb->CdbLength <= sizeof(CDB));
    RtlCopyMemory(cmd->Cdb,
                  Srb->Cdb,
                  Srb->CdbLength);

    //
    // We can ignore the Response UPIU for now (we allocated the memory
    // already), so we are going to fill in the PRDT
    //

    if (numScatterGatherEntries != 0) {
        UfsCreatePrdt(AdapterExtension, prdt, scatterGather);
    }

    //
    // Check that the Transfer Request List Run-Stop Register is set to 1
    //

#if DBG
    rsr.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->TransReq.RunStop.AsUlong);
    NT_ASSERT(rsr.TransferReqListRunStop == 1);
#endif

    UfsAddDbgLog(AdapterExtension,
                 'TrSc',
                 (ULONG_PTR)Srb,
                 index,
                 Srb->Lun);

    //
    // Ring the Doorbell Register to indicate that the Transfer Request is
    // ready to be processed
    //

    UfsReleaseTransferBuildIndex(AdapterExtension,
                                 index, 
                                 FALSE);

    return SRB_STATUS_SUCCESS;
}

NTSTATUS
UfsSendStartStopUnitTransferRequest(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG TargetState
    )
/*++

Routine Description:

    Send a Start Stop Unit Request to the UFS device to change its power state.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    Target State - The target state for the power transition

Return Value:

    TRUE is returned if the UTP Transfer Request was built successfully.
    Otherwise, a return value of false indicates an error (like an invalid
    state transition).

Required IRQL:

    Dispatch

--*/
{
    ULONG index;
    PUFS_HCI_TRD trd;
    PUFS_TRANSFER_REQUEST_LIST trl;
    ULONG cmdLen, respLen;
    PUFS_UPIU_COMMAND cmd;
    PUFS_UCD_ADDRESS ucd;
    PSCSI_START_STOP_UNIT_CDB cdb;
#if DBG
    UFS_HCI_UTRLRSR rsr;
    PUFS_HCI_REG reg;
#endif

    index = UfsAcquireTransferBuildIndex(AdapterExtension);

    if ((index == INVALID_INDEX) || (index >= AdapterExtension->Trl.ListSize)){
        return STATUS_DEVICE_BUSY;
    }

    NT_ASSERT(index < MAX_REQUEST_LIST_SIZE);

    trl = &AdapterExtension->Trl;
    trl->ActiveSrb[index] = NULL;

    //
    // Build a Transfer Request Descriptor at the empty slot
    //

    trd = &trl->List[index];
    RtlZeroMemory(trd, sizeof(UFS_HCI_TRD));

    trd->CmdType = TRD_CT_UFS_STORAGE;
    trd->DataDirection = TRD_DD_NODATA;
    trd->Interrupt = 1;
    trd->OverallCmdStatus = TRD_OCS_INVALID_OCS_VALUE;

    //
    // Initialize the UCD
    //

    ucd = &trl->UcdList[index];
    cmdLen = SIZEOF_UFS_UPIU_COMMAND;
    respLen = SIZEOF_UFS_UPIU_RESPONSE;

    //
    // Program the UCD Base Address/ Base Address Upper, Response Offset/Length,
    // and PRDT Offset/ Length Fields in the TRD
    //

    trd->CmdDescriptorBaseAddr = ucd->PhysicalAddress.LowPart;
    trd->CmdDescriptorBaseAddrUpper32 = ucd->PhysicalAddress.HighPart;
    trd->RespUPIULen = (respLen + sizeof(ULONG) - 1) / sizeof(ULONG);
    trd->RespUPIUOff = DWORD_ALIGNED(cmdLen) / sizeof(ULONG);
    trd->PRDTLen = 0;
    trd->PRDTOff = DWORD_ALIGNED(cmdLen + respLen) / sizeof(ULONG);

    RtlZeroMemory(ucd->VirtualAddress, MAX_SIZEOF_UFS_UPIU_UCD);

    //
    // Initialize and fill in the Command UPIU
    //

    cmd = (PUFS_UPIU_COMMAND)ucd->VirtualAddress;

    cmd->Header.TransactionType = UPIU_COMMAND;
    cmd->Header.LUN = UFS_POWER_LUN_ID;
    cmd->Header.TaskTag = (UCHAR)index;
    cmd->Header.CmdSetType = UPIU_CMD_SET_SCSI;
    cmd->Header.EhsLength = 0;
    cmd->Header.DataSegmentLength = 0;
    cmd->Header.Flags = 0;
    cmd->ExpectedDataLength = 0;

    //
    // Fill out the Start Stop Unit CDB
    //

    cdb = (PSCSI_START_STOP_UNIT_CDB)&cmd->Cdb[0];

    cdb->Opcode = SCSIOP_START_STOP_UNIT;
    cdb->Immed = 0;
    cdb->NoFlush = 0;
    cdb->PowerCondition = (UCHAR)TargetState;
    cdb->Control = 0;

    //
    // Check that the Transfer Request List Run-Stop Register is set to 1
    //

#if DBG
    reg = AdapterExtension->UfsRegisters;
    rsr.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->TransReq.RunStop.AsUlong);
    NT_ASSERT(rsr.TransferReqListRunStop == 1);
#endif

    UfsAddDbgLog(AdapterExtension,
                 'TrPo',
                 index,
                 TargetState,
                 0);

    //
    // Ring the Doorbell Register to indicate that the Transfer Request is
    // ready to be processed
    //

    UfsReleaseTransferBuildIndex(AdapterExtension, index, FALSE);

    return STATUS_SUCCESS;
}

ULONG
UfsSendQueryUpiu(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ UCHAR Function,
    _In_ PUFS_QUERY_OPCODE Opcode,
    _In_ ULONG DataLen,
    _Inout_updates_opt_(DataLen) PVOID Data
    )
/*++

Routine Description:

    Send a Query UPIU to the Host Controller. Depending on the value of the
    opcode, it can either read or write entire descriptors or single flags.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    Function - The function of the Query UPIU

    Opcode - The opcode of the desired operation (as defined per spec)

    DataLen - Size of optional data buffer

    Data - Pointer to data buffer for Query UPIU

Return Value:

    N/A

--*/
{
    ULONG index;
    PUFS_HCI_TRD trd;
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    PUFS_UCD_ADDRESS ucd;
    ULONG cmdLen, respLen, i, trlResources = 0;
    PUFS_UPIU_QUERY cmd;
    PUFS_UPIU_QUERY resp;
    PUCHAR respData;
#if DBG
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_UTRLRSR rsr;
#endif

    NT_ASSERT(AdapterExtension->Power.LinkState == UfsLinkActive);

    index = UfsAcquireTransferBuildIndex(AdapterExtension);

    if ((index == INVALID_INDEX) || (index >= AdapterExtension->Trl.ListSize)) {
        return 0;
    }

    NT_ASSERT(index < MAX_REQUEST_LIST_SIZE);

    trl->ActiveSrb[index] = NULL;

    //
    // Build a Transfer Request Descriptor at the empty slot and fill in the
    // appropriate fields
    //

    trd = &trl->List[index];
    RtlZeroMemory(trd, sizeof(UFS_HCI_TRD));

    trd->CmdType = TRD_CT_UFS_STORAGE;
    trd->DataDirection = TRD_DD_NODATA;
    trd->Interrupt = 1;
    trd->OverallCmdStatus = TRD_OCS_INVALID_OCS_VALUE;

    cmdLen = DWORD_ALIGNED(sizeof(UFS_UPIU_QUERY));
    respLen = DWORD_ALIGNED(sizeof(UFS_UPIU_QUERY));

    if ((Function == UFS_UPIU_QUERY_FUNCTION_WRITE) &&
        (Opcode->GenericOpcode.Opcode == UFS_UPIU_QUERY_OPCODE_WRITE_DESC)) {

        cmdLen += DWORD_ALIGNED(DataLen);
    } else if ((Function == UFS_UPIU_QUERY_FUNCTION_READ) &&
               (Opcode->GenericOpcode.Opcode == UFS_UPIU_QUERY_OPCODE_READ_DESC)) {

        respLen += DWORD_ALIGNED(DataLen);
    }

    ucd = &AdapterExtension->Trl.UcdList[index];

    //
    // Program the UCD Base Address/ Base Address Upper, Response Offset/Length,
    // and PRDT Offset/ Length Fields in the TRD
    //

    trd->CmdDescriptorBaseAddr = ucd->PhysicalAddress.LowPart;
    trd->CmdDescriptorBaseAddrUpper32 = ucd->PhysicalAddress.HighPart;
    trd->RespUPIULen = respLen / sizeof(ULONG);
    trd->RespUPIUOff = DWORD_ALIGNED(cmdLen) / sizeof(ULONG);
    trd->PRDTLen = 0;
    trd->PRDTOff = DWORD_ALIGNED(cmdLen + respLen) / sizeof(ULONG);

    RtlZeroMemory(ucd->VirtualAddress, MAX_SIZEOF_UFS_UPIU_UCD);

    cmd = (PUFS_UPIU_QUERY)ucd->VirtualAddress;
    cmd->Header.TransactionType = UPIU_QUERY_REQ;
    cmd->Header.QueryFunction = Function;
    cmd->Header.DataSegmentLength = _byteswap_ushort((USHORT)DWORD_ALIGNED(DataLen));
    cmd->Opcode.GenericOpcode.Opcode = Opcode->GenericOpcode.Opcode;
    cmd->Opcode.GenericOpcode.Idn = Opcode->GenericOpcode.Idn;
    cmd->Opcode.GenericOpcode.Index = Opcode->GenericOpcode.Index;
    cmd->Opcode.GenericOpcode.Selector = Opcode->GenericOpcode.Selector;
    RtlCopyMemory(&cmd->Opcode.GenericOpcode.Data[0],
                  &Opcode->GenericOpcode.Data[0],
                  sizeof(UFS_QUERY_OPCODE) - 4);

    if ((DataLen > 0) && (Function == UFS_UPIU_QUERY_FUNCTION_WRITE)) {

        respData = ((PUCHAR)cmd) + sizeof(UFS_UPIU_QUERY);
        NT_ASSERT(sizeof(UFS_UPIU_QUERY) + \
                  DataLen + \
                  SIZEOF_UFS_UPIU_RESPONSE < MAX_SIZEOF_UFS_UPIU_UCD);
        RtlCopyMemory(respData,
                      Data,
                      DataLen);

    }

#if DBG
    rsr.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->TransReq.RunStop.AsUlong);
    NT_ASSERT(rsr.TransferReqListRunStop == 1);
#endif

    UfsAddDbgLog(AdapterExtension,
                 'TrQu',
                 index,
                 Opcode->GenericOpcode.Opcode,
                 Opcode->GenericOpcode.Idn);

    //
    // Ring the Doorbell Register to indicate that the Transfer Request is
    // ready to be processed
    //

    UfsReleaseTransferBuildIndex(AdapterExtension, index, TRUE);

    //
    // Poll for completion of the request
    //

    for (i = 0; i < UFS_COMMAND_TIMEOUT / 10; i++) {
        UfsDelay(10);

        UfsPollForCompletion(AdapterExtension, &trlResources);
        if ((trlResources & (1 << index)) != 0){
            break;
        }
    }

    resp = (PUFS_UPIU_QUERY)((PUCHAR)cmd + (trd->RespUPIUOff * sizeof(ULONG)));

    //
    // Copy the response into the Data Buffer for Read Requests
    //

    if ((DataLen > 0) &&
        (Function == UFS_UPIU_QUERY_FUNCTION_READ)) {

        if (Opcode->GenericOpcode.Opcode == UFS_UPIU_QUERY_OPCODE_READ_DESC) {
            
            //
            // Copy the Entire Descriptor
            //

            NT_ASSERT(sizeof(UFS_UPIU_QUERY) + 
                      DataLen + 
                      SIZEOF_UFS_UPIU_RESPONSE < MAX_SIZEOF_UFS_UPIU_UCD);
            NT_ASSERT(Data != NULL);
            RtlCopyMemory(Data,
                          (PUCHAR)resp + 32,
                          DataLen);
        } else if (Opcode->GenericOpcode.Opcode == UFS_UPIU_QUERY_OPCODE_READ_ATTR) {

            //
            // Copy the ULONG Attribute
            //

            NT_ASSERT(DataLen == sizeof(ULONG));
            NT_ASSERT(Data != NULL);
            RtlCopyMemory(Data,
                          resp->Opcode.AttributeOpcode.Value,
                          DataLen);
        } else if (Opcode->GenericOpcode.Opcode == UFS_UPIU_QUERY_OPCODE_READ_FLAG) {

            //
            // Copy the Individual Flag Value
            //

            NT_ASSERT(DataLen == sizeof(UCHAR));
            NT_ASSERT(Data != NULL);
            *(PUCHAR)Data = resp->Opcode.FlagOpcode.Value;
        }
    }

    return trlResources;
}

VOID
UfsSendNopOutUpiu(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension
    )
/*++

Routine Description:

    Send a NOP OUT UPIU. This is sent during controller initialization, and
    the initialization will wait until a NOP IN UPIU is returned before
    continuing.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

Return Value:

    N/A

--*/
{
    ULONG index;
    PUFS_HCI_TRD trd;
    PUFS_TRANSFER_REQUEST_LIST trl = &AdapterExtension->Trl;
    PUFS_UCD_ADDRESS ucd;
    ULONG cmdLen, respLen;
#if DBG
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_UTRLRSR rsr;
#endif

    NT_ASSERT(AdapterExtension->Power.LinkState == UfsLinkActive);

    index = UfsAcquireTransferBuildIndex(AdapterExtension);

    if ((index == INVALID_INDEX) || (index >= AdapterExtension->Trl.ListSize)){
        return;
    }

    NT_ASSERT(index < MAX_REQUEST_LIST_SIZE);

    trl->ActiveSrb[index] = NULL;

    //
    // Build a Transfer Request Descriptor at the empty slot and fill in the
    // appropriate fields
    //

    trd = &trl->List[index];
    RtlZeroMemory(trd, sizeof(UFS_HCI_TRD));
    trd->CmdType = TRD_CT_UFS_STORAGE;
    trd->DataDirection = TRD_DD_NODATA;
    trd->Interrupt = 1;
    trd->OverallCmdStatus = TRD_OCS_INVALID_OCS_VALUE;

    cmdLen = sizeof(UFS_UPIU_COMMAND);
    respLen = SIZEOF_UFS_UPIU_RESPONSE;

    ucd = &AdapterExtension->Trl.UcdList[index];

    //
    // Program the UCD Base Address/ Base Address Upper, Response Offset/Length,
    // and PRDT Offset/ Length Fields in the TRD
    //

    trd->CmdDescriptorBaseAddr = ucd->PhysicalAddress.LowPart;
    trd->CmdDescriptorBaseAddrUpper32 = ucd->PhysicalAddress.HighPart;
    trd->RespUPIULen = respLen / sizeof(ULONG);
    trd->RespUPIUOff = DWORD_ALIGNED(cmdLen) / sizeof(ULONG);
    trd->PRDTLen = 0;
    trd->PRDTOff = DWORD_ALIGNED(cmdLen + respLen) / sizeof(ULONG);

    //
    // Command UPIU is all zeros
    //

    RtlZeroMemory(ucd->VirtualAddress, MAX_SIZEOF_UFS_UPIU_UCD);

#if DBG
    rsr.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->TransReq.RunStop.AsUlong);
    NT_ASSERT(rsr.TransferReqListRunStop == 1);
#endif

    UfsAddDbgLog(AdapterExtension,
                 'TrNo',
                 index,
                 0,
                 0);

    //
    // Ring the Doorbell Register to indicate that the Transfer Request is
    // ready to be processed
    //

    UfsReleaseTransferBuildIndex(AdapterExtension, index, TRUE);
}
#pragma warning(pop)

NTSTATUS
UfsSendUicCommand(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG Opcode,
    _In_reads_(3) PULONG Parameters,
    _In_ BOOLEAN Synchronous
    )
/*++

Routine Description:

    Execute a UFS Interconnect Layer (UIC) command. These commands are
    typically power related operations, and are usually performed for internal
    operations. Only one UIC command can be sent to the Host Controller at a
    time, and the commands are sent by setting the UIC Command (UICCMD) and
    UIC Command Argument 1/2/3 (UICCMD1/2/3) registers.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    Opcode - the opcode for the UIC command to be sent

    Parameters - the parameters for the UIC command

    Synchronous - identifies whether this operation is synchronous (TRUE) or
                  asynchronous (FALSE)

Return Value:

    True on success, false on failure.

--*/
{
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_UICCMD cmd;
    UFS_HCI_UICCMDARG arg;
    ULONG i;

    NT_ASSERT(Parameters != NULL);

    if (InterlockedCompareExchange(&AdapterExtension->ActiveUicCmd, 1, 0) == 1) {
        // LogError: "Sending UIC Command failed (pending command), for UIC Opcode 0x%08x\n"
        return STATUS_UNSUCCESSFUL;
    }

    NT_ASSERT(AdapterExtension->ActiveUicCmd == 1);

    AdapterExtension->UicSync = Synchronous;

    if (Parameters != NULL){
        for (i = 0; i < 3; i++){
            arg.Argument = Parameters[i];

            StorPortWriteRegisterUlong(AdapterExtension,
                                       &reg->UicCmd.Args[i].AsUlong,
                                       arg.AsUlong);
        }
    }

    UfsAddDbgLog(AdapterExtension,
                 'UICS',
                 Opcode,
                 Parameters[0],
                 Parameters[2]);

    cmd.AsUlong = Opcode;

    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->UicCmd.Cmd.AsUlong,
                               cmd.AsUlong);

    return STATUS_SUCCESS;
}

ULONG
UfsSendUicDmeSet (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG MIBAttribute,
    _In_ ULONG GenSelectorIndex,
    _In_ ULONG Value
    )
/*++

Routine Description:

    Routine for sending a DME_SET command. Will send a successive DME_GET
    command if the driver was built in a checked build.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    MIBAtrribute - the specific attribute to set

    GenSelectorIndex - the index of the target data line

    Value - value to bet set

Return Value:

    The status returned by the UIC command

--*/
{
    ULONG args[3] = {0};
    ULONG errorStatus = 0;
    NTSTATUS status = STATUS_SUCCESS;

    args[0] = ((MIBAttribute & 0x0000FFFF) << 16) | (GenSelectorIndex & 0x0000FFFF);
    args[2] = Value;

    status = UfsExecuteUicCommandSynchronous(AdapterExtension,
                                             UIC_DME_SET,
                                             args,
                                             &errorStatus);

    if (!NT_SUCCESS(status) || (errorStatus != UIC_GENERIC_SUCCESS)) {
        NT_ASSERT(FALSE);
    }

    return errorStatus;
}

ULONG
UfsSendUicDmeGet (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG MIBAttribute,
    _In_ ULONG GenSelectorIndex,
    _Out_ PULONG Value
    )
/*++

Routine Description:

    Routine for sending a DME_GET command.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    MIBAtrribute - the specific attribute to set

    GenSelectorIndex - the index of the target data line

    Value - value returned by the UIC command

Return Value:

    The status returned by the UIC command

--*/
{
    ULONG args[3] = {0};
    ULONG errorStatus = 0;
    NTSTATUS status = STATUS_SUCCESS;

    args[0] = ((MIBAttribute & 0x0000FFFF) << 16) | (GenSelectorIndex & 0x0000FFFF);
    *Value = 0;

    status = UfsExecuteUicCommandSynchronous(AdapterExtension,
                                             UIC_DME_GET,
                                             args,
                                             &errorStatus);

    if (NT_SUCCESS(status)) {
        *Value = args[2];
    } else {
        errorStatus = UIC_GENERIC_FAILURE;
    }

    return errorStatus;
}

ULONG
UfsSendUicDmePeerSet (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG MIBAttribute,
    _In_ ULONG GenSelectorIndex,
    _In_ ULONG Value
    )
/*++

Routine Description:

    Routine for sending a DME_SET command. Will send a successive DME_GET
    command if the driver was built in a checked build.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    MIBAtrribute - the specific attribute to set

    GenSelectorIndex - the index of the target data line

    Value - value to bet set

Return Value:

    The status returned by the UIC command

--*/
{
    ULONG args[3] = {0};
    ULONG errorStatus = 0;
    NTSTATUS status = STATUS_SUCCESS;

    args[0] = ((MIBAttribute & 0x0000FFFF) << 16) | (GenSelectorIndex & 0x0000FFFF);
    args[2] = Value;

    status = UfsExecuteUicCommandSynchronous(AdapterExtension,
                                             UIC_DME_PEER_SET,
                                             args,
                                             &errorStatus);

    if (!NT_SUCCESS(status) || (errorStatus != UIC_GENERIC_SUCCESS)) {
        NT_ASSERT(FALSE);
    }

    return errorStatus;
}

NTSTATUS
UfsSendUicDmeHibernate (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG HibernateOpcode,
    _Out_ PULONG ErrorCode
    )
/*++


Routine Description:

    Routine for sending a DME_HIBERN8 command

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    HibernateOpcode - Opcode for DME_HIBERNATE_ENTER/EXIT command

    ErrorCode - Result of the hibernate operation

Return Value:

    NTSTATUS value of the operation

--*/
{
#if POFX_ENABLED == 1

    ULONG args[3] = {0};
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_HCS hcs;
    NTSTATUS status = STATUS_SUCCESS;

    //
    // If we've disabled Hibern8 or are registered for Auto-Hibern8,
    // we don't want to send any software hibern8 commands.
    //

    if (((AdapterExtension->FeatureFlags & UFS_FEATURE_DISABLE_H8) != 0) ||
        (AdapterExtension->AutoH8Enabled == TRUE)) {

        return STATUS_SUCCESS;
    }

#if DBG
    //
    // Check to see if the controller is ready to receive UIC commands
    //

    hcs.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->Runtime.Hcs.AsUlong);

    NT_ASSERT(hcs.UicCmdReady == 1);
#endif
    
    //
    // Send the Hibernate Command. UfsExecuteUicCommandSynchronous will
    // wait until both the IS.UCCS (UIC Command Completion) and IS.UHES/UHXS
    // interrupts are signalled
    //

    status = UfsExecuteUicCommandSynchronous(AdapterExtension,
                                             HibernateOpcode,
                                             args,
                                             ErrorCode);

    if ((!NT_SUCCESS(status)) || (*ErrorCode != UIC_GENERIC_SUCCESS)) {
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }

    //
    // Read the HCS register to ensure we've finished the operation.
    //

    hcs.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->Runtime.Hcs.AsUlong);

    if (hcs.UicPowerModeChangeReqStatus != UIC_PWR_LOCAL) {
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }

exit:
    return status;

#else
    UNREFERENCED_PARAMETER(AdapterExtension);
    UNREFERENCED_PARAMETER(HibernateOpcode);

    *ErrorCode = 0;

    return STATUS_SUCCESS;
#endif
}

NTSTATUS
UfsExecuteUicCommandSynchronous (
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ ULONG UicOpcode,
    _In_reads_(3) PULONG Args,
    _Out_ PULONG ErrorCode
    )
/*++

Routine Description:

    Sit and wait for the UIC command to complete. This function should only be
    used in the device/ adapter initialization routines.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    UicOpcode - the opcode for the UIC command to be performed

    Args - arguments for the UIC command to be performed

    ErrorCode - pointer for the errorcode of the UIC Command to be returned
                to the caller

Return Value:

    Returns the NTSTATUS value of KeWaitForSingleObject

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    ULONG i;

    //
    // This delay isn't necessary for newer QC SoCs
    //

    if ((AdapterExtension->FeatureFlags & UFS_FEATURE_QUALCOMM_8996) != 0) {
        UfsDelay(1);
    }

    //
    // Clear the event and mark the command sequence as synchronous
    //

    AdapterExtension->UicSync = TRUE;

    ntStatus = UfsSendUicCommand(AdapterExtension,
                                 UicOpcode,
                                 Args,
                                 AdapterExtension->UicSync);

    if (!NT_SUCCESS(ntStatus)){
        return ntStatus;
    }

    //
    // Wait for completion of the UIC command by waiting for the event set
    // by the ISR
    //

    for (i = 0; i < UFS_COMMAND_TIMEOUT; i++) {
        UfsDelay(1);
        UfsPollForCompletion(AdapterExtension, NULL);

        //
        // If we're transitioning the link to/from hibernate, let's make sure
        // that the link has fully transitioned to the proper state before
        // exiting.
        //

        if (UicOpcode == UIC_DME_HIBERNATE_EXIT) {
            if (AdapterExtension->Power.LinkState != UfsLinkActive) {
                continue;
            }
        } else if (UicOpcode == UIC_DME_HIBERNATE_ENTER) {
            if (AdapterExtension->Power.LinkState != UfsLinkHibernate) {
                continue;
            }
        } 

        //
        // We don't want to bail until we know that the UIC command completed
        //

        if (AdapterExtension->ActiveUicCmd == 0) {
            break;
        }
    }

    if (AdapterExtension->ActiveUicCmd > 0) {
        ntStatus = STATUS_UNSUCCESSFUL;
    }

    if (!NT_SUCCESS(ntStatus)){
        // LogError: "KeWaitForSingleObject failed for UIC Command Opcode: 0x%08x, ntStatus = 0x%08x\n"
    }

    //
    // Retrieve the error status of the command
    //

    NT_ASSERT(Args != NULL);
    for (i = 0; i < 3; i++){
        Args[i] = StorPortReadRegisterUlong(AdapterExtension,
                                            &reg->UicCmd.Args[i].AsUlong);
    }

    if (ErrorCode) {
        *ErrorCode = Args[1] & UFS_UIC_ERROR_CODE_BITMASK;
    }

    //
    // Clear the Interrupt Status Bit, and mark the UIC command resources as
    // available
    //

    AdapterExtension->UicSync = FALSE;
    InterlockedCompareExchange(&AdapterExtension->ActiveUicCmd, 0, 1);
    NT_ASSERT(AdapterExtension->ActiveUicCmd == 0);

    return ntStatus;
}


UFS_HCI_IS
UfsPollForCompletion(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _Inout_opt_ PULONG TrlResources
    )
/*++

Routine Description:

    During initialization, or while in dump mode, interrupts won't be enabled.
    This function is used to poll the register space for I/O completions, and
    kicks off the appropriate completion routine.

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    TrlResources - bit vector for the completed IOs

Return Value:

    The IS register that's read in at the beginning of the function

--*/
{
    PUFS_HCI_REG reg = AdapterExtension->UfsRegisters;
    UFS_HCI_IS is, currentIs;
    UFS_HCI_IE ie;
    ULONG trlResources = 0, i, tmrlResources;
    BOOLEAN resetBus = FALSE;

    //
    // Read the current IS register
    //

    currentIs.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                                  &reg->Runtime.Is.AsUlong);

    //
    // We want only the IS bits that aren't being signalled in
    // the ISR
    //

    ie.AsUlong = StorPortReadRegisterUlong(AdapterExtension,
                                           &reg->Runtime.Ie.AsUlong);

    is.AsUlong = currentIs.AsUlong & ~ie.AsUlong;

    //
    // Clear the IS register
    //

    StorPortWriteRegisterUlong(AdapterExtension,
                               &reg->Runtime.Is.AsUlong,
                               is.AsUlong);

    //
    // If there were errors (noted by the IS register), handle the errors
    //

    resetBus = UfsHandleBusError(AdapterExtension, &is);

    //
    // Check to see if the UIC Link Lost Status bit is set
    //

    if (is.UicLinkLostStatus == 1){
        //resetBus = TRUE;
    }

    //
    // Check to see if a Transfer Request completed, and complete the request
    //

    trlResources = UfsAcquireTransferCompletionIndex(AdapterExtension,
                                                     TRUE);

    if (trlResources != 0){
        for (i = 0; i < AdapterExtension->Trl.ListSize; i++){
            if (trlResources & (1 << i)){
                UfsCompleteTransferRequest(AdapterExtension, i, is);
            }
        }

        //
        // Update the AdapterExtension variable to mark the TRL entries as
        // available
        //

        UfsReleaseTransferCompletionIndex(AdapterExtension,
                                          trlResources);
    }

    //
    // Check to see if a Task Management Request completed, and complete the
    // request
    //

    tmrlResources = UfsAcquireTaskManCompletionIndex(AdapterExtension);

    if (tmrlResources != 0) {
        for (i = 0; i < AdapterExtension->Tmrl.ListSize; i++){
            if (tmrlResources & (1 << i)){
                UfsCompleteTaskManRequest(AdapterExtension, i);
            }
        }

        //
        // Update the AdapterExtension variable to mark the TMRL entries as
        // available
        //

        UfsReleaseTaskManCompletionIndex(AdapterExtension,
                                         tmrlResources);
    }

    //
    // Check to see if a UIC Command Completed and complete the command
    //

    if (is.UicCmdCompStatus) {
        InterlockedCompareExchange(&AdapterExtension->ActiveUicCmd, 0, 1);
        NT_ASSERT(AdapterExtension->ActiveUicCmd == 0);
    }

    //
    // Check to see if we've transitioned a hibernate state
    //

    if (is.UicHibernateEnterStatus) {
        AdapterExtension->Power.LinkState = UfsLinkHibernate;
    }

    if (is.UicHibernateExitStatus) {
        AdapterExtension->Power.LinkState = UfsLinkActive;
    }

    if (TrlResources != 0) {
        *TrlResources = trlResources;
    }

    return is;
}

/* SSS_WARNINGS_OFF */ 
#pragma warning(push)
#pragma warning(disable:26010)

VOID
UfsCreatePrdt(
    _In_ PUFS_ADAPTER_EXTENSION AdapterExtension,
    _In_ PUFS_UPIU_PRDT Prdt,
    _In_ PSTOR_SCATTER_GATHER_LIST ScatterGather
    )
/*++

Routine Description:

    Translates the Scatter Gather List returned from the Storport Driver to
    the Physical Region Description Table (PRDT) format, and store it in the
    correct location for the UFS Command Descriptor (UCD).

Arguments:

    AdapterExtension - The UFS extension structure for the current adapter

    Prdt - Pointer to the Physical Region Description Table (PRDT)

    ScatterGather - Pointer to the Scatter Gather List

Return Value:

    N/A

--*/
{
    ULONG i;

    UNREFERENCED_PARAMETER(AdapterExtension);

    for (i = 0; i < ScatterGather->NumberOfElements; i++){
        NT_ASSERT(i < MAX_NUM_SCATTER_GATHER);
        NT_ASSERT((ScatterGather->List[i].PhysicalAddress.LowPart & PRDT_BASE_ADDR_MASK) == 0);

        Prdt[i].BaseAddr = ScatterGather->List[i].PhysicalAddress.LowPart;
        Prdt[i].BaseAddrUpper = ScatterGather->List[i].PhysicalAddress.HighPart;
        Prdt[i].DataByteCount = ScatterGather->List[i].Length-1;
    }
}

#pragma warning(pop)
