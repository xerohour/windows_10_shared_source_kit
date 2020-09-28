/*****************************************************************************
* perf.cpp - services provided by the port class driver to miniports
*****************************************************************************
* Copyright (c) Microsoft.  All rights reserved.
*/

#include "private.h"
#include <wmikm.h>

extern PUNICODE_STRING g_RegistryPath;

#define PORTCLS_TRACE_MOF_NAME	L"PortclsMof"
#define PORTCLS_DEFAULT_WMI_LOG_FLAGS	0x03

ULONG		PortClsWmiLogEnable = 0;
TRACEHANDLE	PortClsWmiLoggerHandle = 0;
ULONG		PortClsWmiLogFlags = 0;
ULONG		PortClsWmiLogLevel = 0;
WCHAR		PortClsBaseInstanceName[] = L"PortCls";

WMIGUIDREGINFO PcWmiGuidList[] =
{
	{
		&PortClsEvent_GUID,
	    0,
		WMIREG_FLAG_TRACED_GUID | WMIREG_FLAG_TRACE_CONTROL_GUID
	},
	{
		&PortCls_PinState_GUID,
		1,
		WMIREG_FLAG_EVENT_ONLY_GUID
	},
	{
		&PortCls_IrpProcessing_GUID,
	    1,
		WMIREG_FLAG_EVENT_ONLY_GUID
	},
	{
		&PortCls_PowerState_GUID,
		1,
		WMIREG_FLAG_EVENT_ONLY_GUID
	},
	{
		&PortCls_AudioEngine_GUID,
		1,
		WMIREG_FLAG_EVENT_ONLY_GUID
	},
    {
		&PortCls_PowerPolicy_GUID,
		1,
		WMIREG_FLAG_EVENT_ONLY_GUID
	},
};

#define PC_CONTROL_INDEX		0
#define PC_PINSTATE_INDEX		1
#define PC_IRPPROCESSING_INDEX	2
#define PC_POWERSTATE_INDEX		3

//------------------------------------------------------------------------------
// Description:
//
//  This function takes miniport's WMIContext and merge it with portcls
//  own WMIConext before re-registering WMI provider 
//
// Parameters:
//
//  PDEVICE_OBJECT _pDeviceObject [in] this is a FDO 
//  IN PVOID _MiniortWmiContext   [in] miniport's WMIConext 
//
//
// Return Values: appropriate NTSTATUS code
//
//
// Note: Here is how this function is expected to be used by miniport driver
//       
//    PcNewPort (&pPort, CLSID_PortWaveCyclic);
//    status = pPort->QueryInterface (IID_IPortWMIRegistration, (PVOID *)&pPortWMIRegistration);
//    if (NT_SUCCESS (status))
//    {
//        status = pPortWMIRegistration->RegisterWMIProvider(_Fdo, (PVOID)_pWmiLibContext);
//    }
//    pPortWMIRegistration->Release();
//
NTSTATUS
RegisterMiniportWMIProvider(IN PDEVICE_OBJECT _pDeviceObject,
                            IN PVOID _MiniortWmiContext)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    ULONG ulGuidCount = 0;
    ULONG ulMiniportGuidCount = 0;
    PWMIGUIDREGINFO pGuidList = NULL;
    PDEVICE_CONTEXT pDeviceContext = NULL;
    PPC_WMI_PWMILIB_CONTEXT pWmiLibContext = NULL;
    PWMILIB_CONTEXT pMiniPortWmiLibContext = (PWMILIB_CONTEXT)_MiniortWmiContext;
    ULONG i=0;
    LPCGUID pControlGuidMiniport = NULL;

    IF_TRUE_ACTION_JUMP((!_pDeviceObject) || (!pMiniPortWmiLibContext), 
                        status = STATUS_INVALID_PARAMETER, exit);

	pDeviceContext = (PDEVICE_CONTEXT)(_pDeviceObject->DeviceExtension);
    IF_TRUE_ACTION_JUMP((!pDeviceContext) || (!pDeviceContext->WmiLibContext), 
                        status = STATUS_INVALID_DEVICE_REQUEST, exit);

    pWmiLibContext = pDeviceContext->WmiLibContext;
    // RegisterMiniportWMIProvider and UnregisterMiniportWMIProvider need to be called
    // in pair. This following checking is to reject any multiple RegisterMiniportWMIProvider calls 
    // in a row before any UnregisterMiniportWMIProvider is invoked.
    IF_TRUE_ACTION_JUMP(pWmiLibContext->MiniPortWmiLibContext, 
                        status = STATUS_INVALID_DEVICE_REQUEST, exit);
    //
    // make sure pMiniPortWmiLibContext->GuidCount is not zero 
    //
    IF_TRUE_ACTION_JUMP((pMiniPortWmiLibContext->GuidCount==0), 
                        status = STATUS_UNSUCCESSFUL, exit);

    ulGuidCount = pWmiLibContext->PcWmiLibContext.GuidCount;
    ulMiniportGuidCount = pMiniPortWmiLibContext->GuidCount;

    // check to see if miniport is using the same GUID as portcls
    //
    pWmiLibContext->MiniPortControlGuidIndex = 0;
    for (i=0; i<ulMiniportGuidCount; i++)
    {
        if (pMiniPortWmiLibContext->GuidList[i].Flags & WMIREG_FLAG_TRACE_CONTROL_GUID)
        {
            pControlGuidMiniport = pMiniPortWmiLibContext->GuidList[i].Guid;
            pWmiLibContext->MiniPortControlGuidIndex = i;
            break;
        }
    }
    // miniport context is invalid if it doesn't contain a control guid
    IF_TRUE_ACTION_JUMP(!pControlGuidMiniport, status = STATUS_INVALID_PARAMETER, exit);
    if (IsEqualGUIDAligned(*pControlGuidMiniport,*PcWmiGuidList[0].Guid))
    {
        pWmiLibContext->SameGUIDUsed = TRUE;
    }
    else
    {
        pWmiLibContext->SameGUIDUsed = FALSE;
    }

    //
    // Combine PcWmiGuidList and GuidList from pMiniPortWmiLibContext
    //   
    ulGuidCount = ulGuidCount + ulMiniportGuidCount;

    //
    // The following memory will be released when InternalPcWmiRegisterProvider.
    //
    pGuidList = (WMIGUIDREGINFO*)ExAllocatePoolWithTag( NonPagedPoolNx, 
                                                sizeof(WMIGUIDREGINFO)*ulGuidCount, 'CWcP' );

    IF_TRUE_ACTION_JUMP(!pGuidList, status = STATUS_INSUFFICIENT_RESOURCES, exit);

    RtlCopyMemory( (VOID*)(pGuidList),  
                   (VOID*)(pMiniPortWmiLibContext->GuidList),
                    sizeof(WMIGUIDREGINFO)*ulMiniportGuidCount);

    RtlCopyMemory( (VOID*)(pGuidList+ulMiniportGuidCount),  
                   (VOID*)(PcWmiGuidList),
                    sizeof(WMIGUIDREGINFO)*(sizeof(PcWmiGuidList) / sizeof(WMIGUIDREGINFO)));

    pWmiLibContext->PcWmiLibContext.GuidList = pGuidList;
    pWmiLibContext->PcWmiLibContext.GuidCount = ulGuidCount;
   	status = IoWMIRegistrationControl (_pDeviceObject, WMIREG_ACTION_REGISTER);
    if(NT_SUCCESS(status))
    {
        pWmiLibContext->MiniPortWmiLibContext = pMiniPortWmiLibContext;
    }

exit:
    if(!NT_SUCCESS(status))
    {
        //
        // Free memory and restore settings
        //
        if (pGuidList)
        {
            ExFreePool(pGuidList);
        }

        // Restore back to portcls only WMI information
        if (pWmiLibContext)
        {
            pWmiLibContext->PcWmiLibContext.GuidList = PcWmiGuidList;
            pWmiLibContext->PcWmiLibContext.GuidCount = sizeof(PcWmiGuidList) / sizeof(WMIGUIDREGINFO);
        }
    }
    return status;
}
//------------------------------------------------------------------------------
// Description:
//
//  This function removes miniport's WMIConext from portcls's current WMIConext 
//
// Parameters:
//
//  PDEVICE_OBJECT _pDeviceObject [in] this is a FDO 
//
//
// Return Values: appropriate NTSTATUS code
//
//
// Note: Here is how this function is expected to be used by miniport driver
//       
//    PcNewPort (&pPort, CLSID_PortWaveCyclic);
//    status = pPort->QueryInterface (IID_IPortWMIRegistration, (PVOID *)&pPortWMIRegistration);
//    if (NT_SUCCESS (status))
//    {
//         status = pPortWMIRegistration->UnregisterWMIProvider(_Fdo);
//    }
//    pPortWMIRegistration->Release();
//
NTSTATUS 
UnregisterMiniportWMIProvider (
    IN PDEVICE_OBJECT _pDeviceObject
    )
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
    IF_TRUE_ACTION_JUMP(!_pDeviceObject, status = STATUS_INVALID_PARAMETER, exit);

	PDEVICE_CONTEXT pDeviceContext = (PDEVICE_CONTEXT)(_pDeviceObject->DeviceExtension);
    IF_TRUE_ACTION_JUMP(!pDeviceContext->WmiLibContext, status = STATUS_INVALID_DEVICE_REQUEST, exit);

 	PPC_WMI_PWMILIB_CONTEXT pWmiLibContext = pDeviceContext->WmiLibContext;
    IF_TRUE_ACTION_JUMP(!pWmiLibContext, status = STATUS_INVALID_DEVICE_REQUEST, exit);
    IF_TRUE_ACTION_JUMP(!pWmiLibContext->MiniPortWmiLibContext, status = STATUS_INVALID_DEVICE_REQUEST, exit);

    //
    // Free dynamicall allocated GuidList
    //   
    if (pDeviceContext->WmiLibContext->PcWmiLibContext.GuidList)
    {
        ExFreePool(pDeviceContext->WmiLibContext->PcWmiLibContext.GuidList);
    }
    //
    // Restore guidlist back to portcls only list
    // 
    pWmiLibContext->PcWmiLibContext.GuidList = PcWmiGuidList;
    pWmiLibContext->PcWmiLibContext.GuidCount = sizeof(PcWmiGuidList) / sizeof(WMIGUIDREGINFO);
    //
    // Register WMI again, which will overwrite previously registered entry
    //
    status = IoWMIRegistrationControl (_pDeviceObject, WMIREG_ACTION_REGISTER);
    if(NT_SUCCESS(status))
    {
        pWmiLibContext->MiniPortWmiLibContext = NULL;
    }
exit:
    return status;
}
//
// InternalPcWmiRegisterProvider
//
NTSTATUS
InternalPcWmiRegisterProvider (
    IN PDEVICE_OBJECT DeviceObject
    )
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    IF_TRUE_ACTION_JUMP(!DeviceObject, status = STATUS_INVALID_PARAMETER, exit);

	PDEVICE_CONTEXT pDeviceContext = (PDEVICE_CONTEXT)(DeviceObject->DeviceExtension);
    IF_TRUE_ACTION_JUMP(!pDeviceContext, status = STATUS_INVALID_PARAMETER, exit);

	PPC_WMI_PWMILIB_CONTEXT pWmiLibContext = pDeviceContext->WmiLibContext;
    IF_TRUE_ACTION_JUMP(!pWmiLibContext, status = STATUS_INVALID_PARAMETER, exit);

	pWmiLibContext->PcWmiLibContext.GuidCount = sizeof(PcWmiGuidList) / sizeof(WMIGUIDREGINFO);
	pWmiLibContext->PcWmiLibContext.GuidList = PcWmiGuidList;
	pWmiLibContext->PcWmiLibContext.QueryWmiRegInfo = PcQueryWmiRegInfo;
	pWmiLibContext->PcWmiLibContext.QueryWmiDataBlock = PcQueryWmiDataBlock;
	pWmiLibContext->PcWmiLibContext.SetWmiDataBlock = PcSetWmiDataBlock;
	pWmiLibContext->PcWmiLibContext.SetWmiDataItem = PcSetWmiDataItem;
	pWmiLibContext->PcWmiLibContext.ExecuteWmiMethod = PcExecuteWmiMethod;
	pWmiLibContext->PcWmiLibContext.WmiFunctionControl = PcWmiFunctionControl;	
    pWmiLibContext->MiniPortWmiLibContext = NULL;
    pWmiLibContext->SameGUIDUsed = FALSE;
    status = IoWMIRegistrationControl (DeviceObject, WMIREG_ACTION_REGISTER);

exit:
    return status;
}

//
// InternalPcWmiUnregisterProvider
//
NTSTATUS
InternalPcWmiUnregisterProvider (
    IN PDEVICE_OBJECT DeviceObject
    )
{
    NTSTATUS status;
    status = IoWMIRegistrationControl (DeviceObject, WMIREG_ACTION_DEREGISTER);
    return status;
}

//
// PcWmiDispatch
//
NTSTATUS
PcWmiSystemControl (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
	SYSCTL_IRP_DISPOSITION irpDisposition;
	PDEVICE_CONTEXT pDeviceContext;
	NTSTATUS ntStatus = STATUS_SUCCESS;

	pDeviceContext = (PDEVICE_CONTEXT)(DeviceObject->DeviceExtension);

	if( pDeviceContext->DeviceRemoveState == DeviceRemoved )
	{
		ntStatus = STATUS_DELETE_PENDING;

		Irp->IoStatus.Status = ntStatus;
		Irp->IoStatus.Information = 0;

		IoCompleteRequest(Irp, IO_NO_INCREMENT);

		return ntStatus;
	}

    ntStatus = AcquireRemoveLock( pDeviceContext, Irp );
    if(!NT_SUCCESS(ntStatus))
    {
        Irp->IoStatus.Status = ntStatus;
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        return ntStatus;
    }
	ntStatus = WmiSystemControl( &(pDeviceContext->WmiLibContext->PcWmiLibContext),
								 DeviceObject,
								 Irp,
								 &irpDisposition );

	switch(irpDisposition)
	{
		case IrpProcessed:
            ReleaseRemoveLock( pDeviceContext, Irp );
			break;

		case IrpNotCompleted:
			IoCompleteRequest(Irp, IO_NO_INCREMENT);
            ReleaseRemoveLock( pDeviceContext, Irp );
			break;

		case IrpForward:
		case IrpNotWmi:
			ntStatus = ForwardIrpAsynchronous( pDeviceContext, Irp );
			break;

		default:
			ntStatus = ForwardIrpAsynchronous( pDeviceContext, Irp );
			break;
	}

	return ntStatus;
}

NTSTATUS
PcQueryWmiRegInfo (
	IN PDEVICE_OBJECT DeviceObject,
	OUT ULONG *RegFlags,
	OUT PUNICODE_STRING InstanceName,
	OUT PUNICODE_STRING *RegistryPath,
	OUT PUNICODE_STRING MofResourceName,
	OUT PDEVICE_OBJECT *Pdo
	)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;

	*RegFlags = WMIREG_FLAG_INSTANCE_BASENAME;

	C_ASSERT(sizeof(PortClsBaseInstanceName) < MAXUSHORT);
	InstanceName->Length = (USHORT)(wcslen(PortClsBaseInstanceName) * sizeof(WCHAR));
	InstanceName->MaximumLength = InstanceName->Length + sizeof(UNICODE_NULL);
	InstanceName->Buffer = (PWSTR)ExAllocatePoolWithTag( PagedPool,
												         InstanceName->MaximumLength,
												         'sUcP' );
	if( NULL != InstanceName->Buffer )
	{
		RtlCopyMemory( InstanceName->Buffer, PortClsBaseInstanceName, InstanceName->Length );
		InstanceName->Buffer[InstanceName->Length / sizeof(WCHAR)] = UNICODE_NULL;

		*RegistryPath = g_RegistryPath;
		RtlInitUnicodeString(MofResourceName, PORTCLS_TRACE_MOF_NAME);
	} else
	{
		ntStatus = STATUS_INSUFFICIENT_RESOURCES;
	}

	return ntStatus;
}

NTSTATUS
PcQueryWmiDataBlock(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp,
	IN ULONG GuidIndex,
	IN ULONG InstanceIndex,
	IN ULONG InstanceCount,
	IN OUT PULONG InstanceLengthArray,
	IN ULONG OutBufferSize,
	OUT PUCHAR Buffer
	)
{
	ULONG ulSize = 0;
	NTSTATUS status = STATUS_SUCCESS;
    ULONG ulPcGuidCount = 0;

	PDEVICE_CONTEXT pDeviceContext = (PDEVICE_CONTEXT)(DeviceObject->DeviceExtension);
    IF_TRUE_ACTION_JUMP(!pDeviceContext, status = STATUS_INVALID_PARAMETER, exit);

	PPC_WMI_PWMILIB_CONTEXT pWmiLibContext = pDeviceContext->WmiLibContext;
    IF_TRUE_ACTION_JUMP(!pWmiLibContext, status = STATUS_INVALID_PARAMETER, exit);

    ulPcGuidCount = sizeof(PcWmiGuidList) / sizeof(WMIGUIDREGINFO);
    IF_TRUE_ACTION_JUMP((GuidIndex>=pWmiLibContext->PcWmiLibContext.GuidCount), 
                        status = STATUS_WMI_GUID_NOT_FOUND, exit);

    //
    // GuidIndex is within range
    //
    if ((pWmiLibContext->MiniPortWmiLibContext)&& (GuidIndex<pWmiLibContext->MiniPortWmiLibContext->GuidCount))
    {
        // Miniport registers WmiLibContext
        // And the WMI is targeting on Minirport
        //
        status = pWmiLibContext->MiniPortWmiLibContext->QueryWmiDataBlock(DeviceObject, Irp, 
                                                                            GuidIndex, InstanceIndex,
                                                                            InstanceCount, InstanceLengthArray,
	                                                                        OutBufferSize, Buffer);
        return status;
    }
    //
    // portcls specific processing
    //
    // At this moment, we don't do anything here for PcQueryWmiDataBlock 
    //
exit:
    status = WmiCompleteRequest( DeviceObject,
								 Irp,
								 status,
								 ulSize,
								 IO_NO_INCREMENT );	
    return status;
}
NTSTATUS
PcSetWmiDataBlock(IN PDEVICE_OBJECT  DeviceObject,    
                  IN PIRP  Irp,    
                  IN ULONG  GuidIndex,    
                  IN ULONG  InstanceIndex,    
                  IN ULONG  BufferSize,    
                  IN PUCHAR  Buffer    )
{
	ULONG ulSize = 0;
	NTSTATUS status = STATUS_SUCCESS;
    ULONG ulPcGuidCount = 0;

	PDEVICE_CONTEXT pDeviceContext = (PDEVICE_CONTEXT)(DeviceObject->DeviceExtension);
    IF_TRUE_ACTION_JUMP(!pDeviceContext, status = STATUS_INVALID_PARAMETER, exit);

	PPC_WMI_PWMILIB_CONTEXT pWmiLibContext = pDeviceContext->WmiLibContext;
    IF_TRUE_ACTION_JUMP(!pWmiLibContext, status = STATUS_INVALID_DEVICE_REQUEST, exit);

    ulPcGuidCount = sizeof(PcWmiGuidList) / sizeof(WMIGUIDREGINFO);
    IF_TRUE_ACTION_JUMP((GuidIndex>=pWmiLibContext->PcWmiLibContext.GuidCount), 
                        status = STATUS_WMI_GUID_NOT_FOUND, exit);
    //
    // GuidIndex is within range
    //
    if ((pWmiLibContext->MiniPortWmiLibContext)&& (GuidIndex<pWmiLibContext->MiniPortWmiLibContext->GuidCount))
    {
        // Miniport registers WmiLibContext
        // And the WMI is targeting on Minirport
        //
        status = pWmiLibContext->MiniPortWmiLibContext->SetWmiDataBlock(DeviceObject,    
                                                                              Irp,    
                                                                              GuidIndex,    
                                                                              InstanceIndex,    
                                                                              BufferSize,    
                                                                              Buffer);
        return status;
    }
    //
    // portcls specific processing
    //
    // At this moment, we don't do anything here for  
    //
exit:
    status = WmiCompleteRequest( DeviceObject,
								   Irp,
								   status,
								   ulSize,
								   IO_NO_INCREMENT );

	return status;
}
//
//  SetWmiDataItem 
//
NTSTATUS PcSetWmiDataItem(IN PDEVICE_OBJECT  DeviceObject,    
                           IN PIRP  Irp,    
                           IN ULONG  GuidIndex,    
                           IN ULONG  InstanceIndex,    
                           IN ULONG  DataItemId,    
                           IN ULONG  BufferSize,    
                           IN PUCHAR Buffer    )
{
	ULONG ulSize = 0;
	NTSTATUS status = STATUS_SUCCESS;
    ULONG ulPcGuidCount = 0;

	PDEVICE_CONTEXT pDeviceContext = (PDEVICE_CONTEXT)(DeviceObject->DeviceExtension);
    IF_TRUE_ACTION_JUMP(!pDeviceContext, status = STATUS_INVALID_PARAMETER, exit);

	PPC_WMI_PWMILIB_CONTEXT pWmiLibContext = pDeviceContext->WmiLibContext;
    IF_TRUE_ACTION_JUMP(!pWmiLibContext, status = STATUS_INVALID_DEVICE_REQUEST, exit);

    ulPcGuidCount = sizeof(PcWmiGuidList) / sizeof(WMIGUIDREGINFO);
    IF_TRUE_ACTION_JUMP((GuidIndex>=pWmiLibContext->PcWmiLibContext.GuidCount), 
                        status = STATUS_WMI_GUID_NOT_FOUND, exit);

    //
    // GuidIndex is within range
    //
    if ((pWmiLibContext->MiniPortWmiLibContext)&& (GuidIndex<pWmiLibContext->MiniPortWmiLibContext->GuidCount))
    {
        // Miniport registers WmiLibContext
        // And the WMI is targeting on Minirport
        //
        status = pWmiLibContext->MiniPortWmiLibContext->SetWmiDataItem(DeviceObject,    
                                                                             Irp,    
                                                                             GuidIndex,    
                                                                             InstanceIndex,    
                                                                             DataItemId,    
                                                                             BufferSize,    
                                                                             Buffer);
        return status;
    }
    //
    // portcls specific processing
    //
    // At this moment, we don't do anything here for  
    //
exit:
    status = WmiCompleteRequest( DeviceObject,
								   Irp,
								   status,
								   ulSize,
								   IO_NO_INCREMENT );

	return status;
}
NTSTATUS PcExecuteWmiMethod(IN PDEVICE_OBJECT  DeviceObject,    
                            IN PIRP  Irp,    
                            IN ULONG  GuidIndex,    
                            IN ULONG  InstanceIndex,    
                            IN ULONG  MethodId,    
                            IN ULONG  InBufferSize,    
                            IN ULONG  OutBufferSize,    
                            IN OUT PUCHAR  Buffer)
{
	ULONG ulSize = 0;
	NTSTATUS status = STATUS_SUCCESS;
    ULONG ulPcGuidCount = 0;

	PDEVICE_CONTEXT pDeviceContext = (PDEVICE_CONTEXT)(DeviceObject->DeviceExtension);
    IF_TRUE_ACTION_JUMP(!pDeviceContext, status = STATUS_INVALID_PARAMETER, exit);

	PPC_WMI_PWMILIB_CONTEXT pWmiLibContext = pDeviceContext->WmiLibContext;
    IF_TRUE_ACTION_JUMP(!pWmiLibContext, status = STATUS_INVALID_DEVICE_REQUEST, exit);

    ulPcGuidCount = sizeof(PcWmiGuidList) / sizeof(WMIGUIDREGINFO);
    IF_TRUE_ACTION_JUMP((GuidIndex>=pWmiLibContext->PcWmiLibContext.GuidCount), 
                        status = STATUS_WMI_GUID_NOT_FOUND, exit);
    //
    // GuidIndex is within range
    //
    if ((pWmiLibContext->MiniPortWmiLibContext)&& (GuidIndex<pWmiLibContext->MiniPortWmiLibContext->GuidCount))
    {
        // Miniport registers WmiLibContext
        // And the WMI is targeting on Minirport
        //
        status = pWmiLibContext->MiniPortWmiLibContext->ExecuteWmiMethod(DeviceObject,    
                                                                               Irp,    
                                                                               GuidIndex,    
                                                                               InstanceIndex,    
                                                                               MethodId,    
                                                                               InBufferSize,    
                                                                               OutBufferSize,    
                                                                               Buffer);
        return status;
    }
    //
    // portcls specific processing
    //
    // At this moment, we don't do anything here for  
    //
exit:
    status = WmiCompleteRequest( DeviceObject,
								   Irp,
								   status,
								   ulSize,
								   IO_NO_INCREMENT );

	return status;
}
//------------------------------------------------------------------------------
// Description:
//
//  This function takes the miniport's WMIContext and merges it with the portcls
//  WMIConext before re-registering WMI provider 
//
// Parameters:
//
//	IN PDEVICE_OBJECT DeviceObject: Pointer to the driver's WDM DEVICE_OBJECT structure. 
//	IN PIRP Irp: Pointer to the IRP
//	IN ULONG GuidIndex: Specifies the data block by supplying a zero-based index into 
//                      the list of GUIDs that the driver provided in the WMILIB_CONTEXT structure 
//                      it passed to WmiSystemControl. 
//	IN WMIENABLEDISABLECONTROL Function: Specifies what to enable or disable. WmiEventControl 
//                      indicates an event, and WmiDataBlockControl indicates data collection 
//                      for a block that was registered as expensive to collect (that is, a block 
//                      for which the driver set WMIREG_FLAG_EXPENSIVE in Flags of the WMIGUIDREGINFO 
//                      structure used to register the block). 
//	IN BOOLEAN Enable: Specifies TRUE to enable the event or data collection, or FALSE to disable it.
//
//
// Return Values: appropriate NTSTATUS code
//
//
// Note: there are two possible cases
//       1. The underlying miniport driver does register its WMI Control GUIDs via RegisterMiniportWMIProvider.
//          In this case, the Guidindex is an index to the combined guid array (PcWmiGuidList + pMiniPortWmiLibContext->GuidList).
//          The necessary index adjustment needs to be done before passing on to miniport driver for enabling/disable.
//          We have a special case that needs to be handled when the miniport driver registers a control GUID that matches the portcls
//          control GUID.  The SameGUIDUsed field is created to handle this case - see RegisterMiniportWMIProvider and this function for details.
//       2. The second case is when there is no miniport registration. This is straight forward, just use the passed-in index.
NTSTATUS
PcWmiFunctionControl(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp,
	IN ULONG GuidIndex,
	IN WMIENABLEDISABLECONTROL Function,
	IN BOOLEAN Enable
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	PWNODE_HEADER Wnode;
	PIO_STACK_LOCATION IrpSp = IoGetCurrentIrpStackLocation(Irp);
    ULONG ulPcGuidCount = 0;
    ULONG ulIndex = 0;
	PDEVICE_CONTEXT pDeviceContext = (PDEVICE_CONTEXT)(DeviceObject->DeviceExtension);
    ULONG ulPortclsGuidStartingIndex = 0;
    ULONG ulPortclsGuidEndingingIndex = 0;

    IF_TRUE_ACTION_JUMP(!pDeviceContext, status = STATUS_INVALID_PARAMETER, exit);

	PPC_WMI_PWMILIB_CONTEXT pWmiLibContext = pDeviceContext->WmiLibContext;
    IF_TRUE_ACTION_JUMP(!pWmiLibContext, status = STATUS_INVALID_DEVICE_REQUEST, exit);

    IF_TRUE_ACTION_JUMP(IrpSp->Parameters.WMI.BufferSize < sizeof(WNODE_HEADER), 
                        status = STATUS_BUFFER_TOO_SMALL, exit);


    //
    // portcls specific processing
    //
    ulPcGuidCount = sizeof(PcWmiGuidList) / sizeof(WMIGUIDREGINFO);
    Wnode = (PWNODE_HEADER)IrpSp->Parameters.WMI.Buffer;
    ulIndex = GuidIndex;

    ulPortclsGuidStartingIndex = (pWmiLibContext->MiniPortWmiLibContext==NULL) ? 0 : pWmiLibContext->MiniPortWmiLibContext->GuidCount;
    ulPortclsGuidEndingingIndex = ulPortclsGuidStartingIndex + ulPcGuidCount - 1;
    
    if ( ((pWmiLibContext->SameGUIDUsed) && (GuidIndex == pWmiLibContext->MiniPortControlGuidIndex)) || 
        ((GuidIndex >= ulPortclsGuidStartingIndex) && (GuidIndex <= ulPortclsGuidEndingingIndex)))
    {
        if ((pWmiLibContext->SameGUIDUsed) && (GuidIndex == pWmiLibContext->MiniPortControlGuidIndex))
        {
            // Using the same GUID and the GuidIndex is for the miniport control GUID, thus we want to also
            // enable/disable the portcls control GUID.
            ulIndex = PC_CONTROL_INDEX;
        }
        else if (pWmiLibContext->MiniPortWmiLibContext)
        {
            // The access is to a portcls GUID, but there are miniport GUIDs too, so offset ulIndex
            // for relative portcls GUID access.
            ulIndex = ulIndex - pWmiLibContext->MiniPortWmiLibContext->GuidCount; 
        }

        //
        // portcls specific processing
        //
        if( Function == WmiEventControl )
        {
            switch(ulIndex)
            {
                case PC_CONTROL_INDEX:
                    // perform the enabling/disabling of portcls events
                    if(Enable)
				    {
					    InterlockedExchange((PLONG)&PortClsWmiLogEnable, 1);
					    PortClsWmiLoggerHandle = Wnode->HistoricalContext;
					    PortClsWmiLogLevel = (ULONG)WmiGetLoggerEnableLevel(PortClsWmiLoggerHandle);
					    PortClsWmiLogFlags = (ULONG)WmiGetLoggerEnableFlags(PortClsWmiLoggerHandle);

					    if( PortClsWmiLogLevel == 0 )
					    {
						    PortClsWmiLogLevel = 1;
					    }

					    if( PortClsWmiLogFlags == 0 )
					    {
						    PortClsWmiLogFlags = PORTCLS_DEFAULT_WMI_LOG_FLAGS;
					    }
				    } else
				    {
					    InterlockedExchange((PLONG)&PortClsWmiLogEnable, 0);
					    PortClsWmiLoggerHandle = 0;
					    PortClsWmiLogLevel = 0;
					    PortClsWmiLogFlags = 0;
				    }
				    break;
                default:
                    status = STATUS_WMI_GUID_NOT_FOUND;
		    		break;
			    }
		    }
            else
            {
			    status = STATUS_INVALID_DEVICE_REQUEST;
            }
    }

    // Pass the call along to the miniport if the miniport has registered GUIDs and GuidIndex points to a miniport GUID.
    if ((pWmiLibContext->MiniPortWmiLibContext) && (GuidIndex < pWmiLibContext->MiniPortWmiLibContext->GuidCount))
    {
        status = pWmiLibContext->MiniPortWmiLibContext->WmiFunctionControl(DeviceObject,
                                                                             Irp,
                                                                             GuidIndex,
                                                                             Function,
                                                                             Enable);
        return status;
    }

exit:
	status = WmiCompleteRequest( DeviceObject,
								   Irp,
								   status,
								   0,
								   IO_NO_INCREMENT );

	return status;
}
#pragma code_seg()
//
// PcWmiLogEvent
//
NTSTATUS
PcWmiLogEvent(
    IN UCHAR    Level,
	IN UCHAR	EventType,
	IN GUID		LogGUID,
	IN ULONGLONG ullData1,
	IN ULONGLONG ullData2,
	IN ULONGLONG ullData3,
	IN ULONGLONG ullData4,
	IN ULONGLONG ullData5)
{
	WMI_LOG_DATA WmiLogData = {0,0};
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PORTCLS_WMI_DATA PcWmiData;
	EVENT_TRACE_HEADER *Wnode;

    if (PortClsWmiLogEnable && PortClsWmiLoggerHandle && PortClsWmiLogLevel >= Level)
	{
		PcWmiData.EventType = (ULONGLONG)EventType;
		PcWmiData.Data1 = ullData1;
		PcWmiData.Data2 = ullData2;
		PcWmiData.Data3 = ullData3;
		PcWmiData.Data4 = ullData4;
		PcWmiData.Data5 = ullData5;

		WmiLogData.Data.DataPtr = (ULONG64)&PcWmiData;
		WmiLogData.Data.Length = sizeof(PORTCLS_WMI_DATA);
		WmiLogData.Header.Size = sizeof(WMI_LOG_DATA);
		WmiLogData.Header.Flags = WNODE_FLAG_TRACED_GUID | WNODE_FLAG_USE_MOF_PTR;
		WmiLogData.Header.Class.Type = EventType;
        WmiLogData.Header.Class.Level = Level;
		WmiLogData.Header.Guid = LogGUID;

		Wnode = &WmiLogData.Header;
		((PWNODE_HEADER)Wnode)->HistoricalContext = PortClsWmiLoggerHandle;

		ntStatus = IoWMIWriteEvent((PVOID)&(WmiLogData.Header));
	}

	return ntStatus;
}

