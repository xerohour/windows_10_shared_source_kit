/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Pnp.cpp

Abstract:

    Implementation for Pnp related jobs

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "Pnp.tmh"

//
// Scan mode that the IHV needs to use (Active/Passive)
//
#define REG_WLAN_CUSTOM_SCAN_MODE                     L"\\REGISTRY\\MACHINE\\Software\\Microsoft\\WlanSvc\\Parameters\\CustomScanParameters"
#define REG_WLAN_CUSTOM_SCAN_MODE_REG_VALUE           L"WlanScanMode"    //Dword value
#define WLAN_SCAN_MODE_LOW                            500                //Low frequency scan
//==============================================================================================

CJobCompleteNotifier::CJobCompleteNotifier()
{
    m_pCompletionEvent = NULL;
    m_pCompletionStatus = NULL;
}

NDIS_STATUS CJobCompleteNotifier::Initialize(
    _In_ PNDIS_STATUS pCompletionStatus,
    _In_ IEventContext* pCompletionEvent)
{
    m_pCompletionEvent = pCompletionEvent;
    m_pCompletionStatus = pCompletionStatus;

    return NDIS_STATUS_SUCCESS;
}

void CJobCompleteNotifier::OnOperationCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_opt_ WFC_SENDER_OBJECT SenderObject )
{
    // The caller should already have the SenderObject (it queued
    // the operation), so if it really needed, it can get the
    // detailed results.
    UNREFERENCED_PARAMETER(SenderObject);

    // Save the status for the caller and signal completion.
    *m_pCompletionStatus = OperationStatus;
    m_pCompletionEvent->Set();

    // Done with the objects we were saving
    m_pCompletionStatus = NULL;
    m_pCompletionEvent = NULL;

    // Delete self
    delete this;
}


//==============================================================================================

CCreatePortJob::CCreatePortJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    CJobBase(ActivityId),
    m_pAdapter(NULL),
    m_CreateTask(ActivityId),
    m_ResetJob(ActivityId),
    m_JobState(CreatePortJobNotStarted),
    m_PortType(WfcPortTypeUnspecified),
    m_uOpmodeMask(WDI_OPERATION_MODE_STA),
    m_CreatedPortId(WDI_PORT_ID_ADAPTER),
    m_CreatedNdisPortNumber(INVALID_NDIS_PORT_NUMBER)
{
    RtlZeroMemory(&m_CreatePortIndicationParameters,
        sizeof(m_CreatePortIndicationParameters)
        );
}

CCreatePortJob::~CCreatePortJob()
{
    if (m_CommandBuffer)
    {
        delete[] m_CommandBuffer;
        m_CommandBuffer = NULL;
    }
}


NDIS_STATUS CCreatePortJob::InitializeFromJob(
    _In_ CAdapter* pAdapter,
    _In_ WFC_PORT_TYPE PortType,
    _In_ UINT16 uOpmodeMask,
    _In_ NDIS_PORT_NUMBER CreatedNdisPortNumber,
    _In_ BOOLEAN bCreateForResume )
{
    m_pAdapter = pAdapter;
    m_PortType = PortType;
    m_uOpmodeMask = (WDI_OPERATION_MODE) uOpmodeMask;
    m_CreatedNdisPortNumber = CreatedNdisPortNumber;
    m_bCreateForResume = bCreateForResume;

    if ( bCreateForResume )
    {
        m_pPortToResume = m_pAdapter->GetPortFromNdisPortNumber(m_CreatedNdisPortNumber);
        ASSERT( m_pPortToResume != NULL );
    }
    else
    {
        m_pPortToResume = NULL;
    }

    // Some jobs reuse without deleting and cleaning up
    if (m_CommandBuffer)
    {
        delete[] m_CommandBuffer;
        m_CommandBuffer = NULL;
    }

    return CJobBase::Initialize(
            m_pAdapter->GetEventQueue(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetActiveJobsList(),
            m_pAdapter->GetSerializedJobList(),
            WiFiJobCreatePort,
            FALSE,
            FALSE,
            WiFiPriorityScopeAdapter,
            WiFiSerializedJobPriority_1,
            (IPropertyCacheDirectory *)pAdapter
            );
}

NDIS_STATUS CCreatePortJob::StartCreatePortTask()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    DeviceCommand* pDeviceCommand = NULL;
    WDI_TASK_CREATE_PORT_PARAMETERS CommandData;

    TraceActEntry();

    // Send the create port job to the device
    ndisStatus = m_CreateTask.Initialize(
            m_pAdapter->GetNotificationManager(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetEventQueue()
            );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Create Port Task (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = m_CreateTask.get_TaskDeviceCommand(&pDeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to get Device Command for Create Port Task (0x%x)", ndisStatus);
        goto Exit;
    }

    ULONG CommandLength;
    RtlZeroMemory(&CommandData, sizeof(WDI_TASK_CREATE_PORT_PARAMETERS));
    CommandData.CreatePortParameters.OpModeMask = (UINT16)m_uOpmodeMask;
    CommandData.CreatePortParameters.NdisPortNumber = m_CreatedNdisPortNumber;
    if ( !m_bCreateForResume )
    {
        CommandData.Optional.MacAddress_IsPresent = FALSE;
    }
    else
    {
        CommandData.Optional.MacAddress_IsPresent = TRUE;
        RtlCopyMemory( &CommandData.MacAddress, 
                        m_pPortToResume->GetMacAddress(),
                        sizeof( DOT11_MAC_ADDRESS));
        WFCInfo( "Re-Create Port for NdisPortNumer %d PortId %d MacAddr %!MACADDR!",
            m_CreatedNdisPortNumber,
            m_pPortToResume->GetPortId(),
            CommandData.MacAddress.Address);
    }

    if ( m_PortType != WfcPortTypeExtSTA )
    {
        WFCActTrace("Port type is not ExtSTA, yanking out WDI_OPERATION_MODE_STA from opmode mask sent to device.");
        CommandData.CreatePortParameters.OpModeMask = 
            CommandData.CreatePortParameters.OpModeMask & ~(WDI_OPERATION_MODE_STA);
    }

    ndisStatus = GenerateWdiTaskCreatePort(&CommandData,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &CommandLength,
        (UINT8**)&m_CommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to Generate TLV for WDI_TASK_CREATE_PORT (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = pDeviceCommand->Initialize(
        WDI_PORT_ID_ADAPTER,
        WDI_TASK_CREATE_PORT,
        CommandLength,
        m_CommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for Create Port Task (0x%x)", ndisStatus);
        goto Exit;
    }

    // Save the start state so that when complete happens, we know where we are
    m_JobState = CreatePortJobWaitingForPortCreate;

    ndisStatus = StartTask(&m_CreateTask);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Create Port Task (0x%x)", ndisStatus);
        goto Exit;
    }
    
Exit:

    TraceActExit(ndisStatus); 
    
    return ndisStatus;
}

NDIS_STATUS CCreatePortJob::CompleteCreatePortTask(
    _In_ NDIS_STATUS OperationStatus)
{
    NDIS_STATUS taskStatus;    
    CPort  *pCreatedPort = NULL;    
    ULONG bufferLength;
    BYTE   *pBuffer = NULL;
    
    TraceActEntry();

    WFCActInfo("Create Port Task completed (0x%x)", OperationStatus);
    
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        goto Exit;
    }

    // Get the results from the TLV
    OperationStatus = CMessageHelper::GetStatusFromTaskOutput(
        &m_CreateTask,
        &taskStatus
        );
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        // TODO: workaround: ignore the error and assign some PortID
        if (m_CreatedNdisPortNumber != NDIS_DEFAULT_PORT_NUMBER)
        {
            m_CreatedPortId = (WFC_PORT_ID) m_CreatedNdisPortNumber;
            goto workaround;
        }

        WFCActError("Create Port Task failed with status (0x%x) %d", 
            OperationStatus, taskStatus);
        goto Exit;
    }

    OperationStatus = m_CreateTask.get_OutputBuffer(
        &bufferLength, 
        &pBuffer);
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to obtain task result from create port task (0x%x)", OperationStatus);
        goto Exit;
    }

    pBuffer += sizeof(WFC_MESSAGE_METADATA);
    bufferLength -= sizeof(WFC_MESSAGE_METADATA);

    //
    // Parse the capabilities. We dont do anything with it just yet
    //
    OperationStatus = ParseWdiIndicationCreatePortComplete(bufferLength,
        pBuffer,
        m_pAdapter->GetTlvContext( ),
        &m_CreatePortIndicationParameters);

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to parse port attributes from task (0x%x)", OperationStatus);
        goto Exit;
    }

    m_CreatedPortId = m_CreatePortIndicationParameters.PortAttributes.PortNumber;

workaround:
    //
    // Create the port object
    //

    if ( m_bCreateForResume )
    {
        //
        // if this is re-creating port at Resume, skip parsing cap because we still have them.
        //

        //
        // update port ID in case it may change
        //
        WFCActInfo( "Resume port id=%d re-create portid=%d", 
            m_pPortToResume->GetPortId(),
            m_CreatedPortId);
        m_pPortToResume->SetPortId( m_CreatedPortId );

        //
        // The following block is for debug info only
        //
        DOT11_MAC_ADDRESS * pOldMacAddr;
        DOT11_MAC_ADDRESS * pNewMacAddr;

        pOldMacAddr = m_pPortToResume->GetMacAddress();
        pNewMacAddr =(PDOT11_MAC_ADDRESS) 
            &m_CreatePortIndicationParameters.PortAttributes.MacAddress.Address[0];
        NT_ASSERT( ARE_MAC_ADDRESS_EQUAL( pOldMacAddr, pNewMacAddr ));
        WFCActInfo( "Resume re-createPort oldMac=%!MACADDR! newMac=%!MACADDR!",
            *pOldMacAddr,
            *pNewMacAddr );
    }
    else
    {
        pCreatedPort = new CPort();
        if (pCreatedPort == NULL)
        {
            WFCActError("Failed to create Port object ");
            OperationStatus = NDIS_STATUS_RESOURCES;
            goto Exit;
        }
        
        OperationStatus = pCreatedPort->Initialize(
                m_pAdapter,
                m_PortType,
                m_CreatedNdisPortNumber,
                m_CreatedPortId,
                (UCHAR*)m_CreatePortIndicationParameters.PortAttributes.MacAddress.Address);
        if (OperationStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to initialize Port object (0x%x)", OperationStatus);
            goto Exit;
        }

        pCreatedPort->SetCurrentOpMode (m_uOpmodeMask);
        
        WFCActInfo("Port %p created with NDIS Port Number = %d Port ID = %d", 
            pCreatedPort, 
            pCreatedPort->GetNdisPortNumber(),
            pCreatedPort->GetPortId());

        // Save it in the adapter list so that additional commands on the port work OK
        OperationStatus = m_pAdapter->AddPort(pCreatedPort);
        if (OperationStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to save create port(0x%x)", OperationStatus);
            goto Exit;
        }

        //Create port on datapath
        m_pAdapter->m_TxMgr.AddPortId(pCreatedPort->GetPortId());
        //TODO what should the opmode be?
        m_pAdapter->m_MiniportDataHandlers.TalTxRxAddPortHandler(m_pAdapter->GetMiniportTalTxRxContext(),
                                                                 pCreatedPort->GetPortId(),
                                                                 m_uOpmodeMask);
    }

    // Adapter is responsible for cleaning this up
    pCreatedPort = NULL;

Exit:
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        if (pCreatedPort)
        {
            delete pCreatedPort;
        }
    }

    TraceActExit(OperationStatus); 

    return OperationStatus;
}

NDIS_STATUS CCreatePortJob::StartPortResetJob()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CPort* pPort;
    
    TraceActEntry();

    pPort = m_pAdapter->GetPortFromNdisPortNumber(m_CreatedNdisPortNumber);
    if (pPort == NULL)
    {
        NT_ASSERT(FALSE);
        ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;        
        WFCActError("Failed to find created port in adapter");
        goto Exit;        
    }
    
    // Launch the child job to create the default port
    ndisStatus = m_ResetJob.InitializeFromJob(
        m_pAdapter,
        m_CreatedNdisPortNumber,
        NULL
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Dot11 Reset Port Job (0x%x)", ndisStatus);
        goto Exit;
    }

    // Save the start state so that when complete happens, we know where we are
    m_JobState = CreatePortJobWaitingForPortReset;

    ndisStatus = StartChildJob(&m_ResetJob);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Dot11 Reset Port Job (0x%x)", ndisStatus);
        goto Exit;
    }
    
Exit:
    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CCreatePortJob::CompletePortResetJob(
    _In_ NDIS_STATUS OperationStatus)
{
    // Job returned the final code
    WFCActInfo("Dot11 Reset Port Job completed (0x%x)", OperationStatus);
    
    return OperationStatus;
}

void CCreatePortJob::FinishJob(
    _In_ NDIS_STATUS JobCompleteStatus)
{
    CompleteJob(JobCompleteStatus);
}

void CCreatePortJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    // Launch the create port task. Only if it succeeds, would we do anything
    ndisStatus = StartCreatePortTask();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Create Port Task (0x%x)", ndisStatus);
        goto Exit;
    }
    
Exit:
    // If anything failed, we will complete the job right now
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        FinishJob(ndisStatus);
    }

    TraceActExit(ndisStatus);    
}

void CCreatePortJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    BOOLEAN completeJob = FALSE;
    
    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    switch(m_JobState)
    {
        case CreatePortJobWaitingForPortCreate:
            {
                OperationStatus = CompleteCreatePortTask(OperationStatus);

                if ( m_bCreateForResume )
                {
                    // for resume, we are done. Dont do reset
                    completeJob = TRUE;
                    break;
                }

                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    OperationStatus = StartPortResetJob();
                }
            }
            break;

        case CreatePortJobWaitingForPortReset:
            {
                OperationStatus = CompletePortResetJob(OperationStatus);
                
                completeJob = TRUE;
            }
            break;
        default:
            NT_ASSERT(FALSE);
            break;
    }

    if ((OperationStatus != NDIS_STATUS_SUCCESS) ||
        (completeJob == TRUE))
    {
        FinishJob(OperationStatus);
    }
    
    TraceActExit(OperationStatus);
}


void CCreatePortJob::OnJobCancelled()
{
    // Do nothing for Create Port task
}

//==============================================================================================

ULONG g_tmpZeroAutoPSMNloPm = 0;

CMiniportInitializeJob::CMiniportInitializeJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    CJobBase(ActivityId),
    m_pAdapter(NULL),
    m_Task(ActivityId),
    m_FirmwareCapabilitiesCommand(ActivityId),
    m_SimpleCommandBuffer(),
#if WDI_DISABLED    
    m_DataPathQueryInterfaceCommand(ActivityId),
    m_SetRadioStateOffJob(ActivityId),
    m_DataPathQICommandBuffer(),
#endif    
    m_FirmwareConfigurationCommand(ActivityId),
    m_pFirmwareConfigurationCommandBuffer(NULL),
    m_JobState(MiniportInitializeJobNotStarted),
    m_CreatePortJob(ActivityId),
    m_MiniportInitParameters(NULL),
    m_pRadioTlvBuffer(NULL),
    m_bInitToResumeFirmware(false)
{
    CJobBase::set_RequiresPassiveIrql(true);

}

CMiniportInitializeJob::~CMiniportInitializeJob()
{
    if (m_pRadioTlvBuffer)
    {
        delete[] m_pRadioTlvBuffer;
    }
    if (m_AdapterCapabilities.PhyInfo.pElements)
    {
        delete[] m_AdapterCapabilities.PhyInfo.pElements;
        m_AdapterCapabilities.PhyInfo.pElements = NULL;
    }

    if (m_AdapterCapabilities.BandInfo.pElements)
    {
        delete[] m_AdapterCapabilities.BandInfo.pElements;
        m_AdapterCapabilities.BandInfo.pElements = NULL;
    }
}

NDIS_STATUS CMiniportInitializeJob::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ PNDIS_MINIPORT_INIT_PARAMETERS     MiniportInitParameters,
    _In_ PNDIS_WDI_INIT_PARAMETERS          NdisWdiInitParameters
    )
{
    m_pAdapter = pAdapter;
    m_MiniportInitParameters = MiniportInitParameters;
    m_NdisWdiInitParameters = NdisWdiInitParameters;
    //InitializeDataPathQICommandBuffer();

    return CJobBase::Initialize(
            m_pAdapter->GetEventQueue(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetActiveJobsList(),
            m_pAdapter->GetSerializedJobList(),
            WiFiJobMiniportInitialize,
            FALSE,
            FALSE,
            WiFiPriorityScopeAdapter,
            WiFiSerializedJobPriority_1,
            (IPropertyCacheDirectory *)pAdapter
            );
}

NDIS_STATUS CMiniportInitializeJob::PerformAllocateAdapter()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_MINIPORT_ADAPTER_ATTRIBUTES    miniportAttributes;            

    TraceActEntry();

    m_JobState = MiniportInitializeJobPerformingAllocateAdapter;
    
    //
    // First we we set default registration attributes
    //
    RtlZeroMemory(&miniportAttributes, sizeof(miniportAttributes));
    miniportAttributes.RegistrationAttributes.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES;
    miniportAttributes.RegistrationAttributes.Header.Revision = NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES_REVISION_1;
    miniportAttributes.RegistrationAttributes.Header.Size = sizeof(NDIS_MINIPORT_ADAPTER_REGISTRATION_ATTRIBUTES);

    // Default port gets automatically activated by NDIS
    miniportAttributes.RegistrationAttributes.AttributeFlags = NDIS_MINIPORT_ATTRIBUTES_NO_PAUSE_ON_SUSPEND;
    miniportAttributes.RegistrationAttributes.CheckForHangTimeInSeconds = 0;
    miniportAttributes.RegistrationAttributes.InterfaceType = NdisInterfaceInternal;
    

    // Call the IHV's code
    ndisStatus = m_pAdapter->GetMiniportDriver()->GetWdiCharacteristics()->AllocateAdapterHandler(
        m_pAdapter->GetMiniportAdapterHandle(),
        m_pAdapter->GetMiniportDriver()->GetIHVMiniportDriverContext(),
        m_MiniportInitParameters,       
        m_NdisWdiInitParameters,
        &miniportAttributes.RegistrationAttributes
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("IHV's AllocateAdapterHandler failed. Error = 0x%08x", ndisStatus);        
        goto Exit;
    }

    ndisStatus = NdisMSetMiniportAttributes(
        m_pAdapter->GetMiniportAdapterHandle(),
        &miniportAttributes
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to set miniport registration attributes (0x%08x)", ndisStatus);
        goto Exit;
    }
    
    // Save the context
    m_pAdapter->SetMiniportAdapterContext(miniportAttributes.RegistrationAttributes.MiniportAdapterContext);

    m_pAdapter->MarkInitComplete(WfcAdapterInitializeIhvAdapterAllocated);
    
Exit:

    TraceActExit(ndisStatus);    
    return ndisStatus;
}

NDIS_STATUS CMiniportInitializeJob::StartOpenTask()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Save the start state so that when complete happens, we know where we are
    m_JobState = MiniportInitializeJobWaitingForOpenTaskComplete;

    // Call the IHV code
    ndisStatus = m_pAdapter->GetMiniportDriver()->GetWdiCharacteristics()->OpenAdapterHandler(
        m_pAdapter->GetMiniportAdapterContext(),
        m_MiniportInitParameters
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        // Failed
        WFCError("WDI Miniport's OpenAdapterHandler failed. Status = 0x%08x", ndisStatus);
        OnJobStepCompleted(ndisStatus, this);
    }
    else
    {
        // Set the step as pending as its going to get completed asynchronously
        SetPending();
    }

    TraceActExit(ndisStatus);

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CMiniportInitializeJob::CompleteOpenTask(
    _In_ NDIS_STATUS OperationStatus)
{
    TraceActEntry();
    
    WFCActInfo("Open Task completed (0x%x)", OperationStatus);
    
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        goto Exit;
    }

    m_pAdapter->SetFirmwareOpenState( TRUE );

Exit:                                   
    TraceActExit(OperationStatus);
    return OperationStatus;
}

NDIS_STATUS CMiniportInitializeJob::StartGetFirmwareCapabilities()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();
    
    // Launch the child job to get the firmware capabilities
    ndisStatus = m_FirmwareCapabilitiesCommand.Initialize(
        WDI_PORT_ID_ADAPTER,
        WDI_GET_ADAPTER_CAPABILITIES,
        sizeof(WFC_MESSAGE_METADATA),
        (BYTE *)&m_SimpleCommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Get Firmware Capabilities command(0x%x)", ndisStatus);
        goto Exit;
    }
    // Try again if the buffer is too small
    m_SimpleCommandBuffer.RetryOnSmallBuffer = TRUE;    

    // Save the start state so that when complete happens, we know where we are
    m_JobState = MiniportInitializeJobWaitingForFirmwareCapabilities;

    ndisStatus = QueueDeviceCommand(&m_FirmwareCapabilitiesCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Get Firmware Capabilites command (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:
    TraceActExit(ndisStatus);
    
    return ndisStatus;
}

NDIS_STATUS CMiniportInitializeJob::CompleteGetFirmwareCapabilities(
    _In_ NDIS_STATUS OperationStatus)
{       
    NDIS_STATUS getStatus;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG bufferLength;
    BYTE   *pBuffer = NULL;

    TraceActEntry();
    WFCActInfo("Get Firmware Capabilities completed (0x%x)", OperationStatus);

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        goto Exit;
    }

    // Get the results from the TLV
    OperationStatus = CMessageHelper::GetStatusFromCommandResult(
        &m_FirmwareCapabilitiesCommand,
        &getStatus
        );
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Get Firmware Capabilities failed with status (0x%x) %d", 
            OperationStatus, getStatus);
        goto Exit;
    }


    ndisStatus = m_FirmwareCapabilitiesCommand.get_CommandResult(
        &bufferLength, 
        &pBuffer);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to obtain command result from firmware capabilities command (0x%x)", ndisStatus);
        goto Exit;
    }

    pBuffer += sizeof(WFC_MESSAGE_METADATA);
    bufferLength -= sizeof(WFC_MESSAGE_METADATA);

    OperationStatus = ParseWdiGetAdapterCapabilities(
        (ULONG)bufferLength,
        pBuffer,
        m_pAdapter->GetTlvContext( ),
        &m_AdapterCapabilities);
    
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to parse command result from command (0x%x)", ndisStatus);
        goto Exit;
    }

    OperationStatus = ApplyDriverCapabilities();
    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't apply Driver Capabilites to capabilities. Error = 0x%08x", ndisStatus);
        goto Exit;
    }

    OperationStatus = ApplyAdapterCapabilitiesOverride();
    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't override adapter capabilities.");
        goto Exit;
    }

    //
    // populate adapter FirmwareCapabilities properties
    //
    ndisStatus = m_pAdapter->SetFirmwareCapabilitiesProperties(&m_AdapterCapabilities);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("m_pAdapter->SetFirmwareCapabilitiesProperties failed (0x%08x)", ndisStatus);
        goto Exit;
    }

    
Exit:
    TraceActExit(OperationStatus);

    return OperationStatus;
}

NDIS_STATUS CMiniportInitializeJob::ApplyDriverCapabilities()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    m_AdapterCapabilities.StationAttributes.StationCapabilities.DesiredBSSIDListSize =
        WFC_CONSTANT::DesiredBSSIDListSize;     
    
    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CMiniportInitializeJob::StartSetFirmwareConfiguration()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BYTE* pCommandBuffer = NULL;
    ULONG CommandBufferLength;
    WDI_SET_FIRMWARE_CONFIGURATION_PARAMETERS firmwareConfigParameters;
    DWORD scanModeWlanSvc = 0;

    TraceActEntry();

    RtlZeroMemory(&firmwareConfigParameters, sizeof(firmwareConfigParameters));

    // Get the NetworkAddress from the registry
    ndisStatus = CheckForNetworkAddressChange(
        &firmwareConfigParameters.MacAddress
        );
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        firmwareConfigParameters.Optional.MacAddress_IsPresent = TRUE;
    }
    else
    {
        // We could use the permanent address & send it down
        firmwareConfigParameters.Optional.MacAddress_IsPresent = FALSE;
    }

    ndisStatus = get_AdapterPropertyCache()->GetPropertyT(WfcAdapterPropertyRegUnreachableThreshold, 
                           &(firmwareConfigParameters.UnreachableDetectionThreshold));
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        NT_ASSERT(FALSE);
        
    }
    firmwareConfigParameters.Optional.UnreachableDetectionThreshold_IsPresent = TRUE;

    // TODO: Get the real bar map
    firmwareConfigParameters.LinkQualityBarMap.AllocateElements( 2, NULL );
    firmwareConfigParameters.LinkQualityBarMap.pElements[0].BarNumber = 0;
    firmwareConfigParameters.LinkQualityBarMap.pElements[0].StartRange = 0;
    firmwareConfigParameters.LinkQualityBarMap.pElements[0].EndRange = 49;
    firmwareConfigParameters.LinkQualityBarMap.pElements[1].BarNumber = 1;
    firmwareConfigParameters.LinkQualityBarMap.pElements[1].StartRange = 50;
    firmwareConfigParameters.LinkQualityBarMap.pElements[1].EndRange = 100;

    firmwareConfigParameters.Optional.AdapterNloScanMode_IsPresent = true;
    firmwareConfigParameters.AdapterNloScanMode = WDI_SCAN_TYPE_AUTO;

    ndisStatus = m_pAdapter->ReadRegDword(REG_WLAN_CUSTOM_SCAN_MODE, REG_WLAN_CUSTOM_SCAN_MODE_REG_VALUE, &scanModeWlanSvc);
    if (ndisStatus == NDIS_STATUS_SUCCESS &&
        scanModeWlanSvc == WLAN_SCAN_MODE_LOW)
    {
        ndisStatus = get_AdapterPropertyCache()->SetPropertyBoolean(WfcAdapterPropertyScanModePassiveOnly, TRUE);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to set WfcAdapterPropertyScanModePassiveOnly - Status:.(0x%x)", ndisStatus);
        }
        firmwareConfigParameters.AdapterNloScanMode = WDI_SCAN_TYPE_PASSIVE_ONLY;
    }

    firmwareConfigParameters.PLDRSupport =
        (!get_AdapterPropertyCache()->GetPropertyBooleanOrDefault( WfcAdapterResetRecoveryOff, FALSE )) &&
            m_pAdapter->m_OemPLDRSupported;

    // Generate the TLV
    ndisStatus = GenerateWdiSetAdapterConfiguration(
        &firmwareConfigParameters,
        sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &CommandBufferLength,
        &pCommandBuffer
        );
    
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't generate WDI_SET_FIRMWARE_CONFIGURATION TLV - Status:.(0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = m_FirmwareConfigurationCommand.Initialize(
        WDI_PORT_ID_ADAPTER,
        WDI_SET_ADAPTER_CONFIGURATION,
        CommandBufferLength,
        pCommandBuffer);
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Get Firmware Capabilities command(0x%x)", ndisStatus);
        goto Exit;
    }

    firmwareConfigParameters.Optional.GroupOwnerResetPolicy_IsPresent = TRUE;
    firmwareConfigParameters.GroupOwnerResetPolicy = WDI_P2P_GO_INTERNAL_RESET_POLICY_USE_LAST_CHANNEL;

    // Save the start state so that when complete happens, we know where we are
    m_JobState = MiniportInitializeJobWaitingForFirmwareConfiguration;

    ndisStatus = QueueDeviceCommand((DeviceCommand*)&m_FirmwareConfigurationCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Set Firmware Configuration command (0x%x)", ndisStatus);
        goto Exit;
    }

    m_pFirmwareConfigurationCommandBuffer = pCommandBuffer;
    pCommandBuffer = NULL;
    
Exit:
    if(pCommandBuffer)
    {
        delete pCommandBuffer;
    }
    TraceActExit(ndisStatus);
    
    return ndisStatus;
}

NDIS_STATUS CMiniportInitializeJob::CompleteSetFirmwareConfiguration(
    _In_ NDIS_STATUS OperationStatus)
{
    WFCActInfo("SetFirmwareConfiguration completed with Status (0x%x)", OperationStatus);

    // TODO: WDI_DISABLED need to look at the completion status code
    if(m_pFirmwareConfigurationCommandBuffer)
    {
        delete m_pFirmwareConfigurationCommandBuffer;
        m_pFirmwareConfigurationCommandBuffer = NULL;
    }

    return OperationStatus;
}

NDIS_STATUS CMiniportInitializeJob::CheckForNetworkAddressChange(
    PWDI_MAC_ADDRESS pConfiguredMacAddress
    )
{
    NDIS_STATUS Status = NDIS_STATUS_SUCCESS;
    BYTE* pMacAddress = NULL;
    UINT MacAddressLength;
    NDIS_HANDLE hNdisConfig =NULL;
    BOOLEAN fCloseHandle = FALSE;
    NDIS_CONFIGURATION_OBJECT config = {0};
    DOT11_MAC_ADDRESS ZeroMac = 
        {0x0, 0x0, 0x0,
        0x0, 0x0, 0x0};

    config.Header.Type = NDIS_OBJECT_TYPE_CONFIGURATION_OBJECT;
    config.Header.Revision =  NDIS_CONFIGURATION_OBJECT_REVISION_1;
    config.Header.Size = NDIS_SIZEOF_CONFIGURATION_OBJECT_REVISION_1;
    config.NdisHandle = m_pAdapter->GetMiniportAdapterHandle();

    Status = NdisOpenConfigurationEx(
        &config,
        &hNdisConfig);

    if(Status != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    fCloseHandle = TRUE;

    NdisReadNetworkAddress(
        &Status,
        (PVOID *)&pMacAddress,
        &MacAddressLength,
        hNdisConfig);

    if(Status != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    if(MacAddressLength != sizeof(WDI_MAC_ADDRESS))
    {
        Status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if((pMacAddress[0] & 0x03) != 0x02 ||
        sizeof(DOT11_MAC_ADDRESS) == RtlCompareMemory(ZeroMac, pMacAddress, sizeof(DOT11_MAC_ADDRESS)))
    {
        Status = NDIS_STATUS_INVALID_PARAMETER;
        goto exit;
    }

    RtlCopyMemory(pConfiguredMacAddress, pMacAddress, sizeof(WDI_MAC_ADDRESS));

exit:
    if(fCloseHandle)
    {
        NdisCloseConfiguration(hNdisConfig);
    }
    return Status;
}



NDIS_STATUS CMiniportInitializeJob::StartCreatePortJob( 
    _In_ BOOLEAN bCreateForResume )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();
    
    // Launch the child job to create the default port
    ndisStatus = m_CreatePortJob.InitializeFromJob(
        m_pAdapter,
        WfcPortTypeExtSTA,
        WDI_OPERATION_MODE_STA,
        NDIS_DEFAULT_PORT_NUMBER,
        bCreateForResume );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Create Port Job (0x%x)", ndisStatus);
        goto Exit;
    }

    // Save the start state so that when complete happens, we know where we are
    m_JobState = MiniportInitializeJobWaitingForCreatePortComplete;

    ndisStatus = StartChildJob(&m_CreatePortJob);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Create Port Job (0x%x)", ndisStatus);
        goto Exit;
    }
    
Exit:
    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CMiniportInitializeJob::CompleteCreatePortJob(
    _In_ NDIS_STATUS OperationStatus)
{
    // Job returned the final code
    WFCActInfo("Create Port Job completed (0x%x)", OperationStatus);
    return OperationStatus;
}

NDIS_STATUS CMiniportInitializeJob::StartDataPathQueryInterface()
/*++
    Issues DataPath QI command to exchange function and callback pointers with lower edge
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

#if WDI_DISABLED    
    ndisStatus = m_DataPathQueryInterfaceCommand.Initialize(
        WDI_PORT_ID_ADAPTER,
        WiFiIoctlCodeDataPathQueryInterface, 
        sizeof(DATAPATH_QI_CMD_BUFFER),
        (BYTE*)&(m_DataPathQICommandBuffer));

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize DataPathQI command(0x%x)", ndisStatus);
        goto Exit;
    }

    // Save the start state so that when complete happens, we know where we are
    m_JobState = MiniportInitializeJobWaitingForDataPathQueryInterfaceComplete;

    ndisStatus = QueueDeviceCommand(&m_DataPathQueryInterfaceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Datapath QI command (0x%x)", ndisStatus);
        goto Exit;
    }
Exit:
#endif // WDI_DISABLED

    TraceActExit(ndisStatus);
    
    return ndisStatus;

}

NDIS_STATUS CMiniportInitializeJob:: StartSetRadioStateTask(_In_ BOOLEAN bRadioOff)

/*++

    Init job calls this function when persistent Softwarer radio state is not 
    matching current radio state

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    DeviceCommand* pDeviceCommand = NULL;
    WDI_SET_RADIO_STATE_PARAMETERS    setParameters;


    TraceActEntry();
    
    // Send the SetRadioOff task to the device
    ndisStatus = m_Task.Initialize(
            m_pAdapter->GetNotificationManager(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetEventQueue()
            );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize SetRadioOff Task (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = m_Task.get_TaskDeviceCommand(&pDeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to get Device Command for Open Task (0x%x)", ndisStatus);
        goto Exit;
    }

    RtlZeroMemory(&setParameters, sizeof(WDI_SET_RADIO_STATE_PARAMETERS));
    setParameters.SoftwareRadioState = !bRadioOff;
    
    ndisStatus = GenerateWdiTaskSetRadioState(&setParameters,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_RadioTlvLength,
        &m_pRadioTlvBuffer);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to generate TLV for SetRadioState task. Status = 0x%08x", ndisStatus);
        goto Exit;
    }    

    ndisStatus = pDeviceCommand->Initialize(
        get_PortId(),
        WDI_TASK_SET_RADIO_STATE,
        m_RadioTlvLength,
        m_pRadioTlvBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize command for SetRadioState task. Status = 0x%08x", ndisStatus);
        goto Exit;
    }    

    ndisStatus = StartTask(&m_Task);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start SetRadioOff Task (0x%x)", ndisStatus);
        goto Exit;
    }
    
Exit:
    TraceActExit(ndisStatus);    
    return ndisStatus;
}

NDIS_STATUS CMiniportInitializeJob::CompleteDataPathQueryInterface(
 _In_ NDIS_STATUS OperationStatus)
{
#if WDI_DISABLED    
    ULONG bufferLength = 0;
    BYTE *pOutBuffer = NULL;

    
    IDatapathFunctions *pDatapathFunctions = NULL;
    DATAPATH_QI_RESPONSE_BUFFER* pQIResponse;
    
    TraceActEntry();

    WFCActInfo("DataPath QI completed (0x%x)", OperationStatus);

    //if this function fails, neither LE or UE interfaces have been set
  
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        goto Exit;
    }

    OperationStatus = m_DataPathQueryInterfaceCommand.get_CommandResult(
        &bufferLength, 
        (BYTE**)&pOutBuffer);

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError(
            "Failed to get results from DataPath QI  with status (0x%x). bufferLength (%ld)", 
            OperationStatus,
            (LONG)bufferLength);
        
        goto Exit;
    }

    NT_ASSERT(bufferLength >= sizeof(DATAPATH_QI_RESPONSE_BUFFER));

    pQIResponse = (DATAPATH_QI_RESPONSE_BUFFER*) pOutBuffer;
    
    pDatapathFunctions = pQIResponse->pFunctions;

    NT_ASSERT(pDatapathFunctions != NULL && pQIResponse->functionTableSize != 0);
    if(pDatapathFunctions == NULL || pQIResponse->functionTableSize == 0)
    {
        OperationStatus = STATUS_INVALID_PARAMETER;
        WFCActError(
            "Invalid datapath function table returned by LE: size (%d)", 
            pQIResponse->functionTableSize);
        
        goto Exit;
    }
    m_pAdapter->m_DataPathFunctions = *pDatapathFunctions;

    m_pAdapter->m_NICContext = pQIResponse->hNICContext;
Exit:
    
#endif // WDI_DISABLED    

    TraceActExit(OperationStatus);

    return OperationStatus;
}

NDIS_STATUS CMiniportInitializeJob::PerformIHVOperationsStart()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    
    MINIPORT_WDI_START_OPERATION_HANDLER pStartHandler = m_pAdapter->GetMiniportDriver()->GetWdiCharacteristics()->StartOperationHandler;

    if (pStartHandler)
    {        
        // Call the IHV's code
        ndisStatus = pStartHandler(
            m_pAdapter->GetMiniportAdapterContext() 
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("IHV's StartOperationHandler failed. Error = 0x%08x", ndisStatus);        
        }

        m_pAdapter->MarkInitComplete(WfcAdapterInitializeIhvOperationStarted);
    }

    return ndisStatus;
}

void CMiniportInitializeJob::FinishJob(
    _In_ MINIPORT_INITIALIZE_JOB_STATE CompletingStep,
    _In_ NDIS_STATUS JobCompleteStatus)
{
    NDIS_ERROR_CODE errorLogEvent;

    if (JobCompleteStatus != NDIS_STATUS_SUCCESS)
    {
        // Log an event log message to help diagnostics
        switch(CompletingStep)
        {
            case MiniportInitializeJobNotStarted:
                errorLogEvent = NDIS_ERROR_CODE_OUT_OF_RESOURCES;
                break;            
            case MiniportInitializeJobPerformingAllocateAdapter:
                // Most likely IHV couldn't get enough info on adapter
                errorLogEvent = NDIS_ERROR_CODE_ADAPTER_NOT_FOUND;
                break;                
            case MiniportInitializeJobWaitingForOpenTaskComplete:
                // Most likely firmware version mismatch
                errorLogEvent = NDIS_ERROR_CODE_BAD_VERSION;
                break;                
            case MiniportInitializeJobWaitingForFirmwareCapabilities:
                // Most likely got bad capabilities
                errorLogEvent = NDIS_ERROR_CODE_INVALID_VALUE_FROM_ADAPTER;
                break;                
            case MiniportInitializeJobWaitingForFirmwareConfiguration:
                // Most likely bad address
                errorLogEvent = NDIS_ERROR_CODE_NETWORK_ADDRESS;
                break;                
            case MiniportInitializeJobWaitingForDataPathQueryInterfaceComplete:
                // Most likely Data path error
                errorLogEvent = NDIS_ERROR_CODE_RECEIVE_SPACE_SMALL;
                break;                
            case MiniportInitializeJobWaitingForCreatePortComplete:
                // Most likely Port creation failure
                errorLogEvent = NDIS_ERROR_CODE_RESOURCE_CONFLICT;
                break;                
            case MiniportInitializeJobWaitingForSetRadioStateComplete:
                // Most likely Invalid port state setting
                errorLogEvent = NDIS_ERROR_CODE_MISSING_CONFIGURATION_PARAMETER;
                break;                
            case MiniportInitializeJobSettingMiniportAttributes:
                // Most likely NDIS didnt like attributes
                errorLogEvent = NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;
                break;                
            case MiniportInitializeJobStartingIHVOperations:
                // Most likely IHV failed
                errorLogEvent = NDIS_ERROR_CODE_HARDWARE_FAILURE;
                break;                
            default:
                // Generic error
                errorLogEvent = NDIS_ERROR_CODE_DRIVER_FAILURE;
                break;                
        };

        // We can only write 2 ULONGs to the event log
        NdisWriteErrorLogEntry(
            m_pAdapter->GetMiniportAdapterHandle(),
            errorLogEvent,
            2,
            ((WiFiJobMiniportInitialize << 16) | CompletingStep),  // Job Type | Job Step
            JobCompleteStatus           // NDIS STATUS
            );
    }    
    
    // We dont do cleanup here. Caller needs to do all the cleanup
    CompleteJob(JobCompleteStatus);
}

void CMiniportInitializeJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    if ( !m_bInitToResumeFirmware )
    {
        ndisStatus = PerformAllocateAdapter();
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("IHV's AllocateAdapterHandler failed. Error = 0x%08x", ndisStatus);
            goto Exit;
        }

        ndisStatus = m_pAdapter->RegisterForAdapterNotifications();
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError( "Failed to reister adapter notification (%x)", ndisStatus );
            goto Exit;
        }
    }

    // Call StartOpen
    ndisStatus = StartOpenTask();  // for ResumeFirmware, this sends Open Task
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Open Task (0x%x)", ndisStatus);
        goto Exit;
    }
    
Exit:

    // If anything failed, we will complete the job right now
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        FinishJob(m_JobState, 
            ndisStatus);
    }

    TraceActExit(ndisStatus);    
}

void CMiniportInitializeJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    BOOLEAN completeJob = FALSE;
    BOOLEAN runCompletionSteps = FALSE;    
    BOOLEAN bDesiredSoftwareRadioOff = FALSE;
    MINIPORT_INITIALIZE_JOB_STATE lastAttemptedState = m_JobState;
    
    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    switch(m_JobState)
    {
        case MiniportInitializeJobWaitingForOpenTaskComplete:
            {
                OperationStatus = CompleteOpenTask(OperationStatus);
                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    m_pAdapter->MarkInitComplete(WfcAdapterInitializeOpenTaskCompleted);
                    OperationStatus = m_pAdapter->InitializeDataPath();
                }
                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    m_pAdapter->MarkInitComplete(WfcAdapterInitializeDataPathInitialized);
                    lastAttemptedState = MiniportInitializeJobWaitingForFirmwareCapabilities;
                    OperationStatus = StartGetFirmwareCapabilities();
                }
            }
            break;

        case MiniportInitializeJobWaitingForFirmwareCapabilities:
            {
                OperationStatus = CompleteGetFirmwareCapabilities(OperationStatus);

                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    lastAttemptedState = MiniportInitializeJobWaitingForFirmwareConfiguration;
                    OperationStatus = StartSetFirmwareConfiguration();
                }
            }
            break;

        case MiniportInitializeJobWaitingForFirmwareConfiguration:
            {
                OperationStatus = CompleteSetFirmwareConfiguration(OperationStatus);

                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    OperationStatus = m_pAdapter->StartDataPath();
                }

                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    m_pAdapter->MarkInitComplete(WfcAdapterInitializeDataPathStarted);                
                
#if WDI_DISABLED                
                    lastAttemptedState = MiniportInitializeJobWaitingForDataPathQueryInterfaceComplete;
                    OperationStatus = StartDataPathQueryInterface();
                }
            }
            break;

        case MiniportInitializeJobWaitingForDataPathQueryInterfaceComplete:
            {
                OperationStatus = CompleteDataPathQueryInterface(OperationStatus);
                
                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
#endif WDI_DISABLED
                    lastAttemptedState = MiniportInitializeJobWaitingForCreatePortComplete;
                    OperationStatus = StartCreatePortJob();
                }
            }

            break;

        case MiniportInitializeJobWaitingForCreatePortComplete:
            {
                OperationStatus = CompleteCreatePortJob(OperationStatus);
                if ( OperationStatus != NDIS_STATUS_SUCCESS )
                {
                    //completeJob = TRUE; implied by the failed status
                    break;
                }
                lastAttemptedState = MiniportInitializeJobWaitingForSetRadioStateComplete;

                WFCActInfo( "[Radio] Initial adapter Radio State reported as Hardware=%d Software=%d",
                            m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.HardwareRadioState,
                            m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.SoftwareRadioState);

                //
                // Save the Hardware and Software radio state as reported by the device
                //
                OperationStatus = m_pAdapter->GetAdapterPropertyCache()->SetPropertyBoolean(
                            WfcAdapterHardwareRadioOff,
                            (m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.HardwareRadioState == 0));
                if ( OperationStatus != STATUS_SUCCESS )
                {
                    WFCActError( "[Radio] Failed to persist Hardware radio state in cache (0x%x)", OperationStatus);
                    break;
                }
                
                OperationStatus = m_pAdapter->GetAdapterPropertyCache()->SetPropertyBoolean(
                            WfcAdapterSoftwareRadioOff,
                            (m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.SoftwareRadioState == 0));
                if ( OperationStatus != STATUS_SUCCESS )
                {
                    WFCActError( "[Radio] Failed to persist Software radio state in cache (0x%x)", OperationStatus);
                    break;
                }
                
                //
                // get desired sw radio off setting. If it doesnt match the firmware state, we  must set radio off                
                //
                OperationStatus = m_pAdapter->GetAdapterPropertyCache()->
                    GetPropertyBoolean( WfcAdapterDesiredSoftwareRadioOff, &bDesiredSoftwareRadioOff);
                
                if ( OperationStatus != NDIS_STATUS_SUCCESS )
                {
                    WFCActError("[Radio] Failed to read Desired Software Radio Off property in the cache");
                    //completeJob = TRUE; implied by the failed status
                    break;
                }

                // If we are not matching, perform the action. Note:
                //      The capability reports 1 for ON, 0 for OFF
                //      WfcAdapterDesiredSoftwareRadioOff caches 0 for ON, 1 for OFF
                if ( m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.SoftwareRadioState == 
                    bDesiredSoftwareRadioOff)
                {
                    WFCActInfo("[Radio] Persisted Software radio OFF state %d and current Radio ON state %d mismatch", 
                        bDesiredSoftwareRadioOff,
                        m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.SoftwareRadioState                        
                        );
                
                    OperationStatus = StartSetRadioStateTask(bDesiredSoftwareRadioOff);
                    if ( OperationStatus != NDIS_STATUS_SUCCESS )
                    {
                        WFCError("[Radio] Failed to StartSetRadioStateTask");
                        //completeJob = TRUE; implied by the failed status
                        break;
                    }
                    else 
                    {
                        m_JobState = MiniportInitializeJobWaitingForSetRadioStateComplete;
                    }
                }
                else
                {
                    //
                    // persisted software radio power is correct, no needs to change FW
                    //
                    runCompletionSteps = TRUE;

                    //
                    // Reported hardware state is already saved
                    //
                    break;
                }
            }
            break;
            
        case MiniportInitializeJobWaitingForSetRadioStateComplete:
            {
                if (OperationStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCActError("Ignoring failure of SetRadio Job (0x%x)", OperationStatus);
                    OperationStatus = NDIS_STATUS_SUCCESS;
                }

                runCompletionSteps = TRUE;
            }
            break;


        default:
            NT_ASSERT(FALSE);
            break;
    }

    // Common steps that we will always run at the end
    if (runCompletionSteps)
    {
        if (OperationStatus == NDIS_STATUS_SUCCESS)
        {
            m_JobState = MiniportInitializeJobSettingMiniportAttributes;
            OperationStatus = SetMiniportAttributes(false);
        }

        if (OperationStatus == NDIS_STATUS_SUCCESS)
        {
            m_JobState = MiniportInitializeJobStartingIHVOperations;
            OperationStatus = PerformIHVOperationsStart();
        }

        if (OperationStatus == NDIS_STATUS_SUCCESS)
        {
            m_JobState = MiniportInitializeJobSucceeded;
        }
        
        // Irrespective of what happened above, we will start
        completeJob = TRUE;
    }
    else if ( m_pAdapter->m_lResetRecovery )
    {
        //
        // needs to register Attributes and return success so we can call NDIS RnR
        //
        SetMiniportAttributes(true);
        OperationStatus = NDIS_STATUS_SUCCESS;
    }

    if ((OperationStatus != NDIS_STATUS_SUCCESS) ||
        (completeJob == TRUE))
    {
        FinishJob(lastAttemptedState, OperationStatus);
    }
    
    TraceActExit(OperationStatus);
}

void CMiniportInitializeJob::OnJobCancelled()
{
    // Do nothing for Initialize task
}

NDIS_STATUS CMiniportInitializeJob::SetHardwareAssistAttributes(
    )
{
    NDIS_STATUS ndisStatus=NDIS_STATUS_SUCCESS;
    NDIS_MINIPORT_ADAPTER_ATTRIBUTES    miniportAttributes;
    PNDIS_MINIPORT_ADAPTER_HARDWARE_ASSIST_ATTRIBUTES pHardwareAssistAttribs;
    NDIS_RECEIVE_FILTER_CAPABILITIES hardwareReceiveFilterCaps;

    pHardwareAssistAttribs = &miniportAttributes.HardwareAssistAttributes;
    
    RtlZeroMemory( &miniportAttributes, sizeof(miniportAttributes));
    WFC_ASSIGN_NDIS_OBJECT_HEADER(
        miniportAttributes.Header,
        NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_HARDWARE_ASSIST_ATTRIBUTES,
        NDIS_MINIPORT_ADAPTER_HARDWARE_ASSIST_ATTRIBUTES_REVISION_3,
        NDIS_SIZEOF_MINIPORT_ADAPTER_HARDWARE_ASSIST_ATTRIBUTES_REVISION_3 );

    RtlCopyMemory( 
        &hardwareReceiveFilterCaps,
        m_pAdapter->GetReceiveFilterCapabilities(),
        sizeof(NDIS_RECEIVE_FILTER_CAPABILITIES));

    WFC_ASSIGN_NDIS_OBJECT_HEADER(
        hardwareReceiveFilterCaps.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        NDIS_RECEIVE_FILTER_CAPABILITIES_REVISION_2,
        NDIS_SIZEOF_RECEIVE_FILTER_CAPABILITIES_REVISION_2 );

    pHardwareAssistAttribs->HardwareReceiveFilterCapabilities = &hardwareReceiveFilterCaps;

    CPropertyCache * propertyCache = m_pAdapter->GetAdapterPropertyCache();
    BOOLEAN bPacketCoalescing = TRUE;
    propertyCache->GetPropertyBoolean(WfcAdapterPacketCoalescing, &bPacketCoalescing );

    if ( bPacketCoalescing )
    {
        pHardwareAssistAttribs->CurrentReceiveFilterCapabilities = &hardwareReceiveFilterCaps;
    }
    
    ndisStatus = NdisMSetMiniportAttributes(
        m_pAdapter->GetMiniportAdapterHandle(),
        &miniportAttributes );
        
    return ndisStatus;
}

NDIS_STATUS CMiniportInitializeJob::SetMiniportAttributes(
    _In_ bool bResetRecoveryRegistration)
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_MINIPORT_ADAPTER_ATTRIBUTES    miniportAttributes;
    PDOT11_PHY_ATTRIBUTES       pPhyAttributes = NULL;
    NDIS_OFFLOAD                offloadAttributes;
    NDIS_PM_CAPABILITIES        pmCapabilities;
    CPort*                      pDefaultPort = NULL;
    PNDIS_OID                   supportedOidsList = NULL;
    ULONG                       supportedOidsCount = 0;
    ULONG                       vwifiCombinations, vwifiAttributesLength;
    DOT11_EXTAP_ATTRIBUTES APAttributes;
    DOT11_EXTSTA_ATTRIBUTES StationAttributes;
    PDOT11_VWIFI_ATTRIBUTES pVWiFiAttributes = NULL;
    DOT11_WFD_ATTRIBUTES WfdAttributes;

    TraceActEntry();

    RtlZeroMemory(&StationAttributes, sizeof(DOT11_EXTSTA_ATTRIBUTES));
    WFC_ASSIGN_NDIS_OBJECT_HEADER(StationAttributes.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_EXTSTA_ATTRIBUTES_REVISION_3,
        sizeof(DOT11_EXTSTA_ATTRIBUTES));

    RtlZeroMemory(&APAttributes, sizeof(DOT11_EXTAP_ATTRIBUTES));
    WFC_ASSIGN_NDIS_OBJECT_HEADER(APAttributes.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_EXTAP_ATTRIBUTES_REVISION_1,
        sizeof(DOT11_EXTAP_ATTRIBUTES));

    RtlZeroMemory(&WfdAttributes, sizeof(DOT11_WFD_ATTRIBUTES));
    WFC_ASSIGN_NDIS_OBJECT_HEADER(WfdAttributes.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_WFD_ATTRIBUTES_REVISION_1,
        DOT11_SIZEOF_WFD_ATTRIBUTES_REVISION_1);

    RtlZeroMemory(&pmCapabilities, sizeof(NDIS_PM_CAPABILITIES));
    WFC_ASSIGN_NDIS_OBJECT_HEADER(pmCapabilities.Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        NDIS_PM_CAPABILITIES_REVISION_2,
        sizeof(NDIS_PM_CAPABILITIES));

    RtlZeroMemory(&offloadAttributes, sizeof(NDIS_OFFLOAD));

    //
    // Next we set the miniport general attributes
    // 
    RtlZeroMemory(&miniportAttributes, sizeof(miniportAttributes));

    
    miniportAttributes.GeneralAttributes.MaxXmitLinkSpeed = m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.MaxTxRate * 1000;
    miniportAttributes.GeneralAttributes.XmitLinkSpeed = NDIS_LINK_SPEED_UNKNOWN;
    miniportAttributes.GeneralAttributes.MaxRcvLinkSpeed = m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.MaxRxRate * 1000;
    miniportAttributes.GeneralAttributes.RcvLinkSpeed = NDIS_LINK_SPEED_UNKNOWN;
    
    miniportAttributes.GeneralAttributes.MaxMulticastListSize = m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.MaxMultiCastListSize;
    miniportAttributes.GeneralAttributes.DataBackFillSize = m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.BackFillSize;
    miniportAttributes.GeneralAttributes.MtuSize = m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.MTUSize;

    RtlCopyMemory(
        &miniportAttributes.GeneralAttributes.PermanentMacAddress,
        (void*)&m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.Address,
        sizeof(miniportAttributes.GeneralAttributes.PermanentMacAddress)
        );

    miniportAttributes.GeneralAttributes.Flags = 0;

    miniportAttributes.GeneralAttributes.MediaType = NdisMediumNative802_11;
    miniportAttributes.GeneralAttributes.PhysicalMediumType = NdisPhysicalMediumNative802_11;
    miniportAttributes.GeneralAttributes.RecvScaleCapabilities = NULL;
    miniportAttributes.GeneralAttributes.AccessType = NET_IF_ACCESS_BROADCAST;
    miniportAttributes.GeneralAttributes.DirectionType = NET_IF_DIRECTION_SENDRECEIVE;
    miniportAttributes.GeneralAttributes.ConnectionType = (NET_IF_CONNECTION_TYPE)IF_CONNECTION_DEDICATED;
    miniportAttributes.GeneralAttributes.IfType = IF_TYPE_IEEE80211;
    miniportAttributes.GeneralAttributes.IfConnectorPresent = TRUE; // RFC 2665 TRUE if physical adapter
    miniportAttributes.GeneralAttributes.SupportedStatistics = 0;
    miniportAttributes.GeneralAttributes.SupportedPauseFunctions = 0; // IEEE 802.3 37.2.1
    miniportAttributes.GeneralAttributes.ContextBackFillSize = 0;
    miniportAttributes.GeneralAttributes.SupportedOidList = NULL;
    miniportAttributes.GeneralAttributes.SupportedOidListLength = 0;
    miniportAttributes.GeneralAttributes.AutoNegotiationFlags = 0;
    miniportAttributes.GeneralAttributes.LookaheadSize = (DOT11_MIN_PDU_SIZE - (sizeof(DOT11_DATA_SHORT_HEADER) + 12));
    miniportAttributes.GeneralAttributes.PowerManagementCapabilitiesEx = 0;

    miniportAttributes.GeneralAttributes.MediaConnectState = MediaConnectStateConnected;
    miniportAttributes.GeneralAttributes.MediaDuplexState = MediaDuplexStateHalf;

    miniportAttributes.GeneralAttributes.MacOptions = NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
        NDIS_MAC_OPTION_TRANSFERS_NOT_PEND |
        NDIS_MAC_OPTION_NO_LOOPBACK;

    miniportAttributes.GeneralAttributes.SupportedPacketFilters = NDIS_PACKET_TYPE_DIRECTED |
        NDIS_PACKET_TYPE_MULTICAST |
        NDIS_PACKET_TYPE_ALL_MULTICAST |
        NDIS_PACKET_TYPE_BROADCAST;

    miniportAttributes.GeneralAttributes.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES;
    miniportAttributes.GeneralAttributes.Header.Revision = NDIS_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES_REVISION_2;
    miniportAttributes.GeneralAttributes.Header.Size = NDIS_SIZEOF_MINIPORT_ADAPTER_GENERAL_ATTRIBUTES_REVISION_2;

    //
    // D0 Coalescing. Enable via *PackageCoalescing in inf.
    ndisStatus = SetHardwareAssistAttributes();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to SetHardwareAssistAttributes (0x%x)", ndisStatus);
        goto Exit;
    }

    //
    // Fill in the PM capabilities
    //
    if (m_AdapterCapabilities.Optional.PmCapabilities_IsPresent)
    {
        pmCapabilities.Flags = m_AdapterCapabilities.PmCapabilities.Flags;
        pmCapabilities.SupportedWoLPacketPatterns = m_AdapterCapabilities.PmCapabilities.SupportedWoLPacketPatterns;
        pmCapabilities.NumTotalWoLPatterns = m_AdapterCapabilities.PmCapabilities.NumTotalWoLPatterns;
        pmCapabilities.MaxWoLPatternSize = m_AdapterCapabilities.PmCapabilities.MaxWoLPatternSize;
        pmCapabilities.MaxWoLPatternOffset = m_AdapterCapabilities.PmCapabilities.MaxWoLPatternOffset;
        pmCapabilities.MaxWoLPacketSaveBuffer = m_AdapterCapabilities.PmCapabilities.MaxWoLPacketSaveBuffer;
        pmCapabilities.SupportedProtocolOffloads = m_AdapterCapabilities.PmCapabilities.SupportedProtocolOffloads;
        pmCapabilities.NumArpOffloadIPv4Addresses = m_AdapterCapabilities.PmCapabilities.NumArpOffloadIPv4Addresses;
        pmCapabilities.NumNSOffloadIPv6Addresses = m_AdapterCapabilities.PmCapabilities.NumNSOffloadIPv6Addresses;
        pmCapabilities.MinMagicPacketWakeUp = (NDIS_DEVICE_POWER_STATE)m_AdapterCapabilities.PmCapabilities.MinMagicPacketWakeUp;
        pmCapabilities.MinPatternWakeUp = (NDIS_DEVICE_POWER_STATE)m_AdapterCapabilities.PmCapabilities.MinPatternWakeUp;
        pmCapabilities.MinLinkChangeWakeUp = (NDIS_DEVICE_POWER_STATE)m_AdapterCapabilities.PmCapabilities.MinLinkChangeWakeUp;
        pmCapabilities.SupportedWakeUpEvents = m_AdapterCapabilities.PmCapabilities.SupportedWakeUpEvents;
        pmCapabilities.MediaSpecificWakeUpEvents = m_AdapterCapabilities.PmCapabilities.MediaSpecificWakeUpEvents;

        miniportAttributes.GeneralAttributes.PowerManagementCapabilitiesEx = &pmCapabilities;
    }
    else
    {
        miniportAttributes.GeneralAttributes.PowerManagementCapabilitiesEx = NULL;
    }

    if (g_tmpZeroAutoPSMNloPm)
    {
        RtlZeroMemory(&pmCapabilities, sizeof(pmCapabilities));
    }

    // Get the port that was created. We use its MAC address
    pDefaultPort = m_pAdapter->GetPortFromNdisPortNumber(NDIS_DEFAULT_PORT_NUMBER);
    NT_ASSERT((pDefaultPort != NULL) || bResetRecoveryRegistration);
    // The port may not yet be created in the case of reset recovery. Handle that
    if (pDefaultPort != NULL)
    {
        m_pAdapter->GetAdapterPropertyCache()->SetPropertyBuffer(
            WfcAdapterPropertyPermanentMacAddress,
            sizeof(DOT11_MAC_ADDRESS),
            miniportAttributes.GeneralAttributes.PermanentMacAddress);

        RtlCopyMemory(
            &miniportAttributes.GeneralAttributes.CurrentMacAddress,
            pDefaultPort->GetMacAddress(),
            sizeof(DOT11_MAC_ADDRESS)
            );            
    }
    
    miniportAttributes.GeneralAttributes.MacAddressLength = sizeof(DOT11_MAC_ADDRESS);


    ndisStatus = COidJobBase::s_GetSupportedOidsList(
        m_AdapterCapabilities.InterfaceAttributes.NonWdiOidsList.pElements,
        (m_AdapterCapabilities.InterfaceAttributes.Optional.NonWdiOidsList_IsPresent) ? 
            m_AdapterCapabilities.InterfaceAttributes.NonWdiOidsList.ElementCount : 0,
        &supportedOidsList,
        &supportedOidsCount);
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to create supported OIDs list(0x%08x)", ndisStatus);
        goto Exit;
    }

    miniportAttributes.GeneralAttributes.SupportedOidList = supportedOidsList;
    miniportAttributes.GeneralAttributes.SupportedOidListLength = supportedOidsCount * sizeof(NDIS_OID);

    ndisStatus = NdisMSetMiniportAttributes(
        m_pAdapter->GetMiniportAdapterHandle(),
        &miniportAttributes
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to set miniport general attributes (0x%08x)", ndisStatus);
        goto Exit;
    }

    if (bResetRecoveryRegistration)
    {
        WFCActError("Not doing full NDIS registration since we are in reset/recovery path");
        goto Exit;
    }

    //
    // Next we set the miniport Native 802.11 attributes
    // 
    RtlZeroMemory(&miniportAttributes, sizeof(miniportAttributes));

    StationAttributes.uScanSSIDListSize = m_AdapterCapabilities.StationAttributes.StationCapabilities.ScanSSIDListSize;
    StationAttributes.uDesiredBSSIDListSize = m_AdapterCapabilities.StationAttributes.StationCapabilities.DesiredBSSIDListSize;
    StationAttributes.uDesiredSSIDListSize = m_AdapterCapabilities.StationAttributes.StationCapabilities.DesiredSSIDListSize;
    StationAttributes.uExcludedMacAddressListSize = WFC_CONSTANT::ExcludedMacAddressListSize;
    StationAttributes.uPrivacyExemptionListSize = m_AdapterCapabilities.StationAttributes.StationCapabilities.PrivacyExemptionListSize;
    StationAttributes.uKeyMappingTableSize = m_AdapterCapabilities.StationAttributes.StationCapabilities.KeyMappingTableSize;
    StationAttributes.uDefaultKeyTableSize = m_AdapterCapabilities.StationAttributes.StationCapabilities.DefaultKeyTableSize;
    StationAttributes.uWEPKeyValueMaxLength = m_AdapterCapabilities.StationAttributes.StationCapabilities.WEPKeyValueMaxLength;
    StationAttributes.uPMKIDCacheSize = WFC_CONSTANT::PMKIDCacheSize;
    StationAttributes.uMaxNumPerSTADefaultKeyTables = m_AdapterCapabilities.StationAttributes.StationCapabilities.MaxNumPerSTA;
    StationAttributes.ucSupportedQoSProtocolFlags = m_AdapterCapabilities.StationAttributes.StationCapabilities.SupportedQOSFlags;
    StationAttributes.bSafeModeImplemented = m_AdapterCapabilities.StationAttributes.StationCapabilities.HostFIPSModeImplemented;
    StationAttributes.bMFPCapable = m_AdapterCapabilities.StationAttributes.StationCapabilities.MFPCapable;
    StationAttributes.bAutoPowerSaveMode = m_AdapterCapabilities.StationAttributes.StationCapabilities.AutoPowerSaveMode;
    StationAttributes.uMaxNetworkOffloadListSize = m_AdapterCapabilities.StationAttributes.StationCapabilities.uMaxNetworkOffloadListSize;
    // Neighbor Reports are only supported if Action Frames are supported
    // We will need to expose this differently from miniport or station attributes
    StationAttributes.bNeighborReportSupported = m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.ActionFramesSupported;
    StationAttributes.bHESSIDConnectionSupported = m_AdapterCapabilities.StationAttributes.StationCapabilities.HESSIDConnectionSupported;

    // Unicast algorithm capabilities
    StationAttributes.uInfraNumSupportedUcastAlgoPairs = m_AdapterCapabilities.StationAttributes.UnicastAlgorithms.ElementCount;

    if (StationAttributes.uInfraNumSupportedUcastAlgoPairs > 0)
    {
        StationAttributes.pInfraSupportedUcastAlgoPairs = new DOT11_AUTH_CIPHER_PAIR[StationAttributes.uInfraNumSupportedUcastAlgoPairs];
    }
    else
    {
        StationAttributes.pInfraSupportedUcastAlgoPairs = NULL;
    }

    for (UINT32 i = 0; i < StationAttributes.uInfraNumSupportedUcastAlgoPairs; i++)
    {
        StationAttributes.pInfraSupportedUcastAlgoPairs[i].AuthAlgoId = CWabiToDot11Converter::MapAuthAlgorithm(m_AdapterCapabilities.StationAttributes.UnicastAlgorithms.pElements[i].AuthAlgorithm);
        StationAttributes.pInfraSupportedUcastAlgoPairs[i].CipherAlgoId = CWabiToDot11Converter::MapCipherAlgorithm(m_AdapterCapabilities.StationAttributes.UnicastAlgorithms.pElements[i].CipherAlgorithm);
    }

    // Multicast algorithm capabilities
    StationAttributes.uInfraNumSupportedMcastAlgoPairs = m_AdapterCapabilities.StationAttributes.MulticastDataAlgorithms.ElementCount;

    if (StationAttributes.uInfraNumSupportedMcastAlgoPairs)
    {
        StationAttributes.pInfraSupportedMcastAlgoPairs = new DOT11_AUTH_CIPHER_PAIR[StationAttributes.uInfraNumSupportedMcastAlgoPairs];
    }
    else
    {
        StationAttributes.pInfraSupportedMcastAlgoPairs = NULL;
    }

    for (UINT32 i = 0; i < StationAttributes.uInfraNumSupportedMcastAlgoPairs; i++)
    {
        StationAttributes.pInfraSupportedMcastAlgoPairs[i].AuthAlgoId = CWabiToDot11Converter::MapAuthAlgorithm(m_AdapterCapabilities.StationAttributes.MulticastDataAlgorithms.pElements[i].AuthAlgorithm);
        StationAttributes.pInfraSupportedMcastAlgoPairs[i].CipherAlgoId = CWabiToDot11Converter::MapCipherAlgorithm(m_AdapterCapabilities.StationAttributes.MulticastDataAlgorithms.pElements[i].CipherAlgorithm);
    }

    // Multicast algorithm capabilities
    StationAttributes.uInfraNumSupportedMcastMgmtAlgoPairs = m_AdapterCapabilities.StationAttributes.MulticastManagementAlgorithms.ElementCount;
    
    if (StationAttributes.uInfraNumSupportedMcastMgmtAlgoPairs > 0)
    {
        StationAttributes.pInfraSupportedMcastMgmtAlgoPairs = new DOT11_AUTH_CIPHER_PAIR[StationAttributes.uInfraNumSupportedMcastMgmtAlgoPairs];
    }
    else
    {
        StationAttributes.pInfraSupportedMcastMgmtAlgoPairs = NULL; 
    }

    for (UINT32 i = 0; i < StationAttributes.uInfraNumSupportedMcastMgmtAlgoPairs; i++)
    {
        StationAttributes.pInfraSupportedMcastMgmtAlgoPairs[i].AuthAlgoId = CWabiToDot11Converter::MapAuthAlgorithm(m_AdapterCapabilities.StationAttributes.MulticastManagementAlgorithms.pElements[i].AuthAlgorithm);
        StationAttributes.pInfraSupportedMcastMgmtAlgoPairs[i].CipherAlgoId = CWabiToDot11Converter::MapCipherAlgorithm(m_AdapterCapabilities.StationAttributes.MulticastManagementAlgorithms.pElements[i].CipherAlgorithm);
    }
    miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes = &StationAttributes;

    if (!m_AdapterCapabilities.Optional.VirtualizationAttributes_IsPresent)
    {
        miniportAttributes.Native_802_11_Attributes.VWiFiAttributes = NULL;
    }
    else
    {
        //
        // We will support only a single combination
        //
        vwifiCombinations = 1;
        vwifiAttributesLength = FIELD_OFFSET(DOT11_VWIFI_ATTRIBUTES, Combinations[0]) +
                                 (vwifiCombinations * DOT11_SIZEOF_VWIFI_COMBINATION_REVISION_3);
        pVWiFiAttributes = (PDOT11_VWIFI_ATTRIBUTES) new BYTE[vwifiAttributesLength];
        if (pVWiFiAttributes)
        {
            RtlZeroMemory(pVWiFiAttributes, vwifiAttributesLength);
            WFC_ASSIGN_NDIS_OBJECT_HEADER(pVWiFiAttributes->Header,
                NDIS_OBJECT_TYPE_DEFAULT,
                DOT11_VWIFI_ATTRIBUTES_REVISION_1,
                sizeof(DOT11_VWIFI_ATTRIBUTES));

            pVWiFiAttributes->uTotalNumOfEntries = vwifiCombinations;
            for (UINT32 i = 0; i < vwifiCombinations; i++)
            {
                PDOT11_VWIFI_COMBINATION_V3 pVWIFIComb_v3 = (PDOT11_VWIFI_COMBINATION_V3) (&pVWiFiAttributes->Combinations[i]);
                WFC_ASSIGN_NDIS_OBJECT_HEADER (pVWIFIComb_v3->Header, 
                                            NDIS_OBJECT_TYPE_DEFAULT,
                                            DOT11_VWIFI_COMBINATION_REVISION_3,
                                            DOT11_SIZEOF_VWIFI_COMBINATION_REVISION_3);

                pVWIFIComb_v3->uNumInfrastructure = m_AdapterCapabilities.VirtualizationAttributes.VirtualizationCapabilities.ExtSTACount;
                pVWIFIComb_v3->uNumAdhoc = 0;
                pVWIFIComb_v3->uNumSoftAP = m_AdapterCapabilities.VirtualizationAttributes.VirtualizationCapabilities.APCount;
                pVWIFIComb_v3->uNumVirtualStation = 0;
                pVWIFIComb_v3->uNumWFDGroup = m_AdapterCapabilities.VirtualizationAttributes.VirtualizationCapabilities.WiFiDirectGroupCount;
            }

            miniportAttributes.Native_802_11_Attributes.VWiFiAttributes = pVWiFiAttributes;
        }
    }

    if (!m_AdapterCapabilities.Optional.P2PAttributes_IsPresent)
    {
        miniportAttributes.Native_802_11_Attributes.WFDAttributes = NULL;
    }
    else
    {
        WfdAttributes.uNumConcurrentGORole = m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ConcurrentGOCount;
        WfdAttributes.uNumConcurrentClientRole = m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ConcurrentClientCount;
        WfdAttributes.WPSVersionsSupported = m_AdapterCapabilities.P2PAttributes.P2PCapabilities.WPSVersionSupport;
        WfdAttributes.bServiceDiscoverySupported = m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ServiceDiscoverySupported;
        WfdAttributes.bClientDiscoverabilitySupported = m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ClientDiscoverabilitySupported;
        WfdAttributes.bInfrastructureManagementSupported = m_AdapterCapabilities.P2PAttributes.P2PCapabilities.InfrastructureManagementSupported;
        WfdAttributes.uMaxSecondaryDeviceTypeListSize = m_AdapterCapabilities.P2PAttributes.P2PCapabilities.MaxSecondaryAdapterTypeListSize;
        RtlCopyMemory(&WfdAttributes.DeviceAddress,
            m_AdapterCapabilities.P2PAttributes.P2PCapabilities.DeviceAddress,
            sizeof(WfdAttributes.DeviceAddress)
            );

        if (m_AdapterCapabilities.P2PAttributes.InterfaceAddressList.ElementCount > 0)
        {
            WfdAttributes.pInterfaceAddressList = new DOT11_MAC_ADDRESS[m_AdapterCapabilities.P2PAttributes.InterfaceAddressList.ElementCount];

            if (WfdAttributes.pInterfaceAddressList)
            {
                for (UINT32 i = 0; i < m_AdapterCapabilities.P2PAttributes.InterfaceAddressList.ElementCount; i++)
                {
                    RtlCopyMemory(&WfdAttributes.pInterfaceAddressList[i],
                        (void*)&m_AdapterCapabilities.P2PAttributes.InterfaceAddressList.pElements[i],
                        sizeof(WfdAttributes.pInterfaceAddressList[i]));
                }
                WfdAttributes.uInterfaceAddressListCount = m_AdapterCapabilities.P2PAttributes.InterfaceAddressList.ElementCount;
            }
        }
        else
        {
            WfdAttributes.pInterfaceAddressList = NULL;
        }
        WfdAttributes.uDiscoveryFilterListSize = m_AdapterCapabilities.P2PAttributes.P2PCapabilities.DiscoveryFilterListSize;
        WfdAttributes.uGORoleClientTableSize = m_AdapterCapabilities.P2PAttributes.P2PCapabilities.GOClientTableSize;
        miniportAttributes.Native_802_11_Attributes.WFDAttributes = &WfdAttributes;
    }

    if (!m_AdapterCapabilities.Optional.APAttributes_IsPresent)
    {
        miniportAttributes.Native_802_11_Attributes.ExtAPAttributes = NULL;
    }
    else
    {
        APAttributes.uScanSSIDListSize = m_AdapterCapabilities.APAttributes.APCapabilities.ScanSSIDListSize;
        APAttributes.uDesiredSSIDListSize = m_AdapterCapabilities.APAttributes.APCapabilities.DesiredSSIDListSize;
        APAttributes.uPrivacyExemptionListSize = m_AdapterCapabilities.APAttributes.APCapabilities.PrivacyExemptionListSize;
        APAttributes.uAssociationTableSize = m_AdapterCapabilities.APAttributes.APCapabilities.AssociationTableSize;;
        APAttributes.uDefaultKeyTableSize = m_AdapterCapabilities.APAttributes.APCapabilities.DefaultKeyTableSize;
        APAttributes.uWEPKeyValueMaxLength = m_AdapterCapabilities.APAttributes.APCapabilities.WEPKeyValueMaxLength;

        // Unicast algorithm capabilities
        APAttributes.uInfraNumSupportedUcastAlgoPairs = m_AdapterCapabilities.APAttributes.UnicastAlgorithms.ElementCount;

        if (APAttributes.uInfraNumSupportedUcastAlgoPairs > 0)
        {
            APAttributes.pInfraSupportedUcastAlgoPairs = new DOT11_AUTH_CIPHER_PAIR[APAttributes.uInfraNumSupportedUcastAlgoPairs];

            if (APAttributes.pInfraSupportedUcastAlgoPairs)
            {
                for (UINT32 i = 0; i < APAttributes.uInfraNumSupportedUcastAlgoPairs; i++)
                {
                    APAttributes.pInfraSupportedUcastAlgoPairs[i].AuthAlgoId = CWabiToDot11Converter::MapAuthAlgorithm(m_AdapterCapabilities.APAttributes.UnicastAlgorithms.pElements[i].AuthAlgorithm);
                    APAttributes.pInfraSupportedUcastAlgoPairs[i].CipherAlgoId = CWabiToDot11Converter::MapCipherAlgorithm(m_AdapterCapabilities.APAttributes.UnicastAlgorithms.pElements[i].CipherAlgorithm);
                }
            }
        }
        else
        {
            APAttributes.pInfraSupportedUcastAlgoPairs = NULL;
        }

        // Multicast algorithm capabilities                                                                                                   capabilities
        APAttributes.uInfraNumSupportedMcastAlgoPairs = m_AdapterCapabilities.APAttributes.MulticastDataAlgorithms.ElementCount;

        if (APAttributes.uInfraNumSupportedMcastAlgoPairs > 0)
        {
            APAttributes.pInfraSupportedMcastAlgoPairs = new DOT11_AUTH_CIPHER_PAIR[APAttributes.uInfraNumSupportedMcastAlgoPairs];

            if (APAttributes.pInfraSupportedMcastAlgoPairs)
            {
                for (UINT32 i = 0; i < APAttributes.uInfraNumSupportedMcastAlgoPairs; i++)
                {
                    APAttributes.pInfraSupportedMcastAlgoPairs[i].AuthAlgoId = CWabiToDot11Converter::MapAuthAlgorithm(m_AdapterCapabilities.APAttributes.MulticastDataAlgorithms.pElements[i].AuthAlgorithm);
                    APAttributes.pInfraSupportedMcastAlgoPairs[i].CipherAlgoId = CWabiToDot11Converter::MapCipherAlgorithm(m_AdapterCapabilities.APAttributes.MulticastDataAlgorithms.pElements[i].CipherAlgorithm);
                }
            }
        }
        else
        {
            APAttributes.pInfraSupportedMcastAlgoPairs = NULL;
        }

        APAttributes.bStrictlyOrderedServiceClassImplemented = miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->bStrictlyOrderedServiceClassImplemented;
        APAttributes.uNumSupportedCountryOrRegionStrings = miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->uNumSupportedCountryOrRegionStrings;
        APAttributes.pSupportedCountryOrRegionStrings = miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->pSupportedCountryOrRegionStrings;

        miniportAttributes.Native_802_11_Attributes.ExtAPAttributes = &APAttributes;
    }

    miniportAttributes.Native_802_11_Attributes.Header.Type = NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES;
    miniportAttributes.Native_802_11_Attributes.Header.Revision = NDIS_MINIPORT_ADAPTER_802_11_ATTRIBUTES_REVISION_3;
    miniportAttributes.Native_802_11_Attributes.Header.Size = NDIS_SIZEOF_MINIPORT_ADAPTER_NATIVE_802_11_ATTRIBUTES_REVISION_3;

    miniportAttributes.Native_802_11_Attributes.NumOfTXBuffers = WFC_CONSTANT::NumOfTXBuffers;
    miniportAttributes.Native_802_11_Attributes.NumOfRXBuffers = WFC_CONSTANT::NumOfRXBuffers;

    miniportAttributes.Native_802_11_Attributes.OpModeCapability = DOT11_OPERATION_MODE_EXTENSIBLE_STATION;

    // TODO: Disable Virtualization support
    if (IS_FEATURE_ENABLED(TEST_FEATUREMASK_ENABLE_VWIFI)) //HACKHACK: Logic flipped
    {
        miniportAttributes.Native_802_11_Attributes.VWiFiAttributes = NULL;
        miniportAttributes.Native_802_11_Attributes.ExtAPAttributes = NULL;
        miniportAttributes.Native_802_11_Attributes.WFDAttributes = NULL;
    }
    else
    {
        if (miniportAttributes.Native_802_11_Attributes.WFDAttributes)
        {
            miniportAttributes.Native_802_11_Attributes.OpModeCapability |= DOT11_OPERATION_MODE_WFD_DEVICE;

            if (miniportAttributes.Native_802_11_Attributes.WFDAttributes->uNumConcurrentGORole > 0)
            {
                miniportAttributes.Native_802_11_Attributes.OpModeCapability |= DOT11_OPERATION_MODE_WFD_GROUP_OWNER;
            }
            if (miniportAttributes.Native_802_11_Attributes.WFDAttributes->uNumConcurrentClientRole > 0)
            {
                miniportAttributes.Native_802_11_Attributes.OpModeCapability |= DOT11_OPERATION_MODE_WFD_CLIENT;
            }
        }
    }

#ifndef ARM
    // BEGIN WORKAROUND for x86Stub

    // Report support for Adhoc otherwise the OS would not reject this interface    
    miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->uAdhocNumSupportedUcastAlgoPairs
        = 1;
    miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->pAdhocSupportedUcastAlgoPairs
        = miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->pInfraSupportedUcastAlgoPairs;


    miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->uAdhocNumSupportedMcastAlgoPairs
        = 1;
    miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->pAdhocSupportedMcastAlgoPairs
        = miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->pInfraSupportedMcastAlgoPairs;

    // END OF WORKAROUND
#endif

    if (g_tmpZeroAutoPSMNloPm)
    {
        miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->bAutoPowerSaveMode = 0;
        miniportAttributes.Native_802_11_Attributes.ExtSTAAttributes->uMaxNetworkOffloadListSize = 0;
    }

    // Allocate and fill space for the PHY attributes

    if ((!m_AdapterCapabilities.Optional.PhyInfo_IsPresent) || (m_AdapterCapabilities.PhyInfo.ElementCount == 0))
    {
        WFCActError("Failed to find Phy Info in firmware capabilities");
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto Exit;
    }

    // Convert the WFC_PHY_INFO to DOT11_PHY_ATTTRIBUTES
    pPhyAttributes = new DOT11_PHY_ATTRIBUTES[m_AdapterCapabilities.PhyInfo.ElementCount];
    if (pPhyAttributes == NULL)
    {
        WFCError("Failed to allocate memory for DOT11_PHY_ATTRIBUTES");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    RtlZeroMemory(pPhyAttributes, sizeof(DOT11_PHY_ATTRIBUTES)* m_AdapterCapabilities.PhyInfo.ElementCount);

    // init ndis headers
    for (ULONG i = 0; i < m_AdapterCapabilities.PhyInfo.ElementCount; ++i)
    {
        PDOT11_PHY_ATTRIBUTES pCurrentPhyAttributes = &pPhyAttributes[i];
        ULONG txIndex = 0;
        ULONG rxIndex = 0;
        UINT8 lastNonStandardIndex = 0;

        WFC_ASSIGN_NDIS_OBJECT_HEADER(pCurrentPhyAttributes->Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_PHY_ATTRIBUTES_REVISION_1,
            sizeof(DOT11_PHY_ATTRIBUTES));


        pCurrentPhyAttributes->PhyType = (DOT11_PHY_TYPE)m_AdapterCapabilities.PhyInfo.pElements[i].PhyCapabilities.PhyType;

        // Use data from cache to report to OS
        pCurrentPhyAttributes->bHardwarePhyState = !(m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(
            WfcAdapterHardwareRadioOff,
            !m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.HardwareRadioState));
        pCurrentPhyAttributes->bSoftwarePhyState = !(m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(
            WfcAdapterSoftwareRadioOff,
            !m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.SoftwareRadioState));

        pCurrentPhyAttributes->bCFPollable = m_AdapterCapabilities.PhyInfo.pElements[i].PhyCapabilities.SupportsCFPoll;
        pCurrentPhyAttributes->uMPDUMaxLength = m_AdapterCapabilities.PhyInfo.pElements[i].PhyCapabilities.MPDUMaxLength;
        pCurrentPhyAttributes->TempType = (DOT11_TEMP_TYPE)m_AdapterCapabilities.PhyInfo.pElements[i].PhyCapabilities.TemperatureClass;
        pCurrentPhyAttributes->DiversitySupport = (DOT11_DIVERSITY_SUPPORT)m_AdapterCapabilities.PhyInfo.pElements[i].PhyCapabilities.DiversitySupport;

        pCurrentPhyAttributes->uNumberSupportedPowerLevels = m_AdapterCapabilities.PhyInfo.pElements[i].TxPowerLevelList.ElementCount;

        RtlCopyMemory(pCurrentPhyAttributes->TxPowerLevels, m_AdapterCapabilities.PhyInfo.pElements[i].TxPowerLevelList.pElements, sizeof(UINT32)* pCurrentPhyAttributes->uNumberSupportedPowerLevels);

        pCurrentPhyAttributes->uNumDataRateMappingEntries = m_AdapterCapabilities.PhyInfo.pElements[i].DataRateList.ElementCount;
        if (pCurrentPhyAttributes->uNumDataRateMappingEntries > DOT11_RATE_SET_MAX_LENGTH)
            pCurrentPhyAttributes->uNumDataRateMappingEntries = DOT11_RATE_SET_MAX_LENGTH;

        WDI_DATA_RATE_ENTRY DataRateEntry;
        for (ULONG j = 0; j < pCurrentPhyAttributes->uNumDataRateMappingEntries; j++)
        {
            // Fill the data rate mapping table
            DataRateEntry.DataRateFlag = m_AdapterCapabilities.PhyInfo.pElements[i].DataRateList.pElements[j].DataRateFlag;
            DataRateEntry.DataRateValue = m_AdapterCapabilities.PhyInfo.pElements[i].DataRateList.pElements[j].DataRateValue;

            CWabiToDot11Converter::MapDataRateEntry(
                &DataRateEntry,
                &pCurrentPhyAttributes->DataRateMappingEntries[j],
                lastNonStandardIndex);

            if (pCurrentPhyAttributes->DataRateMappingEntries[j].ucDataRateFlag & DOT11_DATA_RATE_NON_STANDARD)
            {
                lastNonStandardIndex = pCurrentPhyAttributes->DataRateMappingEntries[j].ucDataRateIndex;
            }

            if (DataRateEntry.DataRateFlag & WDI_DATA_RATE_TX_RATE)
            {
                pCurrentPhyAttributes->SupportedDataRatesValue.ucSupportedTxDataRatesValue[txIndex] =
                    pCurrentPhyAttributes->DataRateMappingEntries[j].ucDataRateIndex;
                txIndex++;
            }

            if (DataRateEntry.DataRateFlag & WDI_DATA_RATE_RX_RATE)
            {
                pCurrentPhyAttributes->SupportedDataRatesValue.ucSupportedRxDataRatesValue[rxIndex] =
                    pCurrentPhyAttributes->DataRateMappingEntries[j].ucDataRateIndex;
                rxIndex++;
            }
        }
    }

    miniportAttributes.Native_802_11_Attributes.SupportedPhyAttributes = pPhyAttributes;
    miniportAttributes.Native_802_11_Attributes.NumSupportedPhys = m_AdapterCapabilities.PhyInfo.ElementCount;
    
    ndisStatus = NdisMSetMiniportAttributes(
        m_pAdapter->GetMiniportAdapterHandle(),
        &miniportAttributes
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to set miniport Native 802.11 attributes (0x%08x)", ndisStatus);
        goto Exit;
    }

    RtlZeroMemory(&miniportAttributes, sizeof(miniportAttributes));

    if (m_AdapterCapabilities.Optional.TcpOffloadCapabilities_IsPresent)
    {
        WFC_ASSIGN_NDIS_OBJECT_HEADER(offloadAttributes.Header,
            NDIS_OBJECT_TYPE_OFFLOAD,
            NDIS_OFFLOAD_REVISION_3,
            NDIS_SIZEOF_NDIS_OFFLOAD_REVISION_3);

        WFC_ASSIGN_NDIS_OBJECT_HEADER(miniportAttributes.OffloadAttributes.Header,
            NDIS_OBJECT_TYPE_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES,
            NDIS_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES_REVISION_1,
            NDIS_SIZEOF_MINIPORT_ADAPTER_OFFLOAD_ATTRIBUTES_REVISION_1);

        CWabiToDot11Converter::MapTcpOffloadCapabilitiesToNdisOffload(m_AdapterCapabilities.TcpOffloadCapabilities, &offloadAttributes);
        miniportAttributes.OffloadAttributes.HardwareOffloadCapabilities = &offloadAttributes;
        miniportAttributes.OffloadAttributes.DefaultOffloadConfiguration = &offloadAttributes;

        miniportAttributes.OffloadAttributes.DefaultTcpConnectionOffloadConfiguration = NULL;
        miniportAttributes.OffloadAttributes.TcpConnectionOffloadHardwareCapabilities = NULL;

        ndisStatus = NdisMSetMiniportAttributes(
            m_pAdapter->GetMiniportAdapterHandle(),
            &miniportAttributes
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to set miniport offload attributes (0x%08x)", ndisStatus);
            goto Exit;
        }

        m_pAdapter->GetAdapterPropertyCache()->PopulateTcpOffloadCapabilities(
            m_AdapterCapabilities.TcpOffloadCapabilities);
    }

Exit:
    if (supportedOidsList)
    {
        delete[]supportedOidsList;
    }

    if (pPhyAttributes)
    {
        delete[] pPhyAttributes;
    }

    if (StationAttributes.pInfraSupportedMcastMgmtAlgoPairs)
    {
        delete[] StationAttributes.pInfraSupportedMcastMgmtAlgoPairs;
        StationAttributes.pInfraSupportedMcastMgmtAlgoPairs = NULL;
    }

    if (StationAttributes.pInfraSupportedMcastAlgoPairs)
    {
        delete[] StationAttributes.pInfraSupportedMcastAlgoPairs;
        StationAttributes.pInfraSupportedMcastAlgoPairs = NULL;
    }

    if (StationAttributes.pInfraSupportedUcastAlgoPairs)
    {
        delete[] StationAttributes.pInfraSupportedUcastAlgoPairs;
        StationAttributes.pInfraSupportedUcastAlgoPairs = NULL;
    }

    if (APAttributes.pInfraSupportedUcastAlgoPairs)
    {
        delete[] APAttributes.pInfraSupportedUcastAlgoPairs;
        APAttributes.pInfraSupportedUcastAlgoPairs = NULL;
    }

    if (APAttributes.pInfraSupportedMcastAlgoPairs)
    {
        delete[] APAttributes.pInfraSupportedMcastAlgoPairs;
        APAttributes.pInfraSupportedMcastAlgoPairs = NULL;
    }

    if (WfdAttributes.pInterfaceAddressList)
    {
        delete[] WfdAttributes.pInterfaceAddressList;
        WfdAttributes.pInterfaceAddressList = NULL;
    }

    if (pVWiFiAttributes)
    {
        delete[] pVWiFiAttributes;
        pVWiFiAttributes = NULL;
    }

    TraceActExit(ndisStatus);

    return ndisStatus;
}


NDIS_STATUS
CMiniportInitializeJob::ApplyAdapterCapabilitiesOverride()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG adapterCapabilitiesOverride = 0;
    
    TraceActEntry();

    // These are capabilities that we may have to disable on specific machines
    // in the field for compability with existing infrastructure. These should not be 
    // used by IHVs to disable capabilites, they should stop reporting
    // those instead.

    // These are private override settings, so not put in a global header
    enum _WFC_ADAPTER_CAPABILITY_OVERRIDE_FLAGS {
        WFC_ADAPTER_CAPABILITY_OVERRIDE_MTU_SIZE                            = 0x00000001,
        WFC_ADAPTER_CAPABILITY_OVERRIDE_ACTION_FRAME_SUPPORT                = 0x00000002,
        WFC_ADAPTER_CAPABILITY_OVERRIDE_FAST_TRANSITION_SUPPORT             = 0x00000004,
        WFC_ADAPTER_CAPABILITY_OVERRIDE_MFP_CAPABLE                         = 0x00000008,
        WFC_ADAPTER_CAPABILITY_OVERRIDE_P2P_SERVICE_DISCOVERY_SUPPORT       = 0x00000010,
        WFC_ADAPTER_CAPABILITY_OVERRIDE_BSS_TRANSITION                      = 0x00000020,
        WFC_ADAPTER_CAPABILITY_OVERRIDE_MAX                                 = 0x0000002F
        // MAC ADDRESS Randomization disable support is through the OS
    };
    
    adapterCapabilitiesOverride = get_AdapterPropertyCache()->GetPropertyULongOrDefault(
                    WfcAdapterPropertyRegAdapterCapabilityOverride, 0);
    if (!adapterCapabilitiesOverride)
    {
        goto exit;
    }    

    // To avoid someone from setting random bits to disabling future
    // capabilities, limit to only allow those capabilities to be disabled that are supported
    // in this driver. The side effect is that this disallows a newer regkey from being used on older OS.
    // For debugging purposes allow -1 for all capabilities disable
    if ((adapterCapabilitiesOverride != ((ULONG)-1)) && 
        (adapterCapabilitiesOverride > WFC_ADAPTER_CAPABILITY_OVERRIDE_MAX))
    
    {
        WFCActError("Invalid AdapterCapabilitiesOverride setting specified (Set: 0x%x , Max Allowed: 0x%x", 
            adapterCapabilitiesOverride , WFC_ADAPTER_CAPABILITY_OVERRIDE_MAX);
        goto exit;
    }

    if (adapterCapabilitiesOverride & WFC_ADAPTER_CAPABILITY_OVERRIDE_MTU_SIZE)
    {
        // Limit MTU size
        WFCActInfo("Overriding adapter capability for MTU size from %d to %d", 
            m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.MTUSize,
            WFC_CONSTANT::LegacyMtuSize);

        m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.MTUSize = WFC_CONSTANT::LegacyMtuSize;
    }

    if (adapterCapabilitiesOverride & WFC_ADAPTER_CAPABILITY_OVERRIDE_ACTION_FRAME_SUPPORT)
    {
        // This disables all action frame based capabilities like (11k Neighbor reports, ANQP Service discovery)
        WFCActInfo("Overriding adapter capability for Action frame support from %d to %d", 
            m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.ActionFramesSupported,
            0);

        m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.ActionFramesSupported = 0;
    }

    if (adapterCapabilitiesOverride & WFC_ADAPTER_CAPABILITY_OVERRIDE_FAST_TRANSITION_SUPPORT)
    {
        // 802.11R
        WFCActInfo("Overriding adapter capability for Fast Transition Support support from %d to %d", 
            m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.FastTransitionSupported,
            0);

        m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.FastTransitionSupported = 0;
    }
    
    if (adapterCapabilitiesOverride & WFC_ADAPTER_CAPABILITY_OVERRIDE_MFP_CAPABLE)
    {
        // 802.11W
        WFCActInfo("Overriding adapter capability for MFP support from %d to %d", 
            m_AdapterCapabilities.StationAttributes.StationCapabilities.MFPCapable,
            0);

        m_AdapterCapabilities.StationAttributes.StationCapabilities.MFPCapable = 0;
    }

    if (adapterCapabilitiesOverride & WFC_ADAPTER_CAPABILITY_OVERRIDE_BSS_TRANSITION)
    {
        // 802.11v
        WFCActInfo("Overriding adapter capability for 11v BSS Transition from %d to %d",
            m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.BSSTransitionSupported,
            0);

        m_AdapterCapabilities.InterfaceAttributes.InterfaceCapabilities.BSSTransitionSupported = 0;
    }

    if (adapterCapabilitiesOverride & WFC_ADAPTER_CAPABILITY_OVERRIDE_P2P_SERVICE_DISCOVERY_SUPPORT)
    {
        // Disable all the P2P services related capabilities
        
        //
        // m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ServiceDiscoverySupported
        // is actually used for reporting non-Service P2PDiscovery, dont overwrite that
        //
        // Action frame support is separately controlled
        //

        
        WFCActInfo("Overriding adapter capability for P2P Service Name support from %d to %d", 
            m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ServiceNameDiscoverySupported,
            0);

        m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ServiceNameDiscoverySupported = 0;

        WFCActInfo("Overriding adapter capability for P2P Service Information support from %d to %d", 
            m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ServiceInformationDiscoverySupported,
            0);

        m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ServiceInformationDiscoverySupported = 0;

        WFCActInfo("Overriding adapter capability for P2P Service Name Advertisement Bytes from %d to %d", 
            m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ServiceNameAdvertisementsMaxBytesSupported,
            0);

        m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ServiceNameAdvertisementsMaxBytesSupported = 0;

        WFCActInfo("Overriding adapter capability for P2P Service Information Advertisement Bytes from %d to %d", 
            m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ServiceInformationAdvertisementsMaxBytesSupported,
            0);

        m_AdapterCapabilities.P2PAttributes.P2PCapabilities.ServiceInformationAdvertisementsMaxBytesSupported = 0;

        // This will disable both WFDS services and non-WFDS devices discovery
        WFCActInfo("Overriding adapter capability for P2P Background Discovery support from %d to %d", 
            m_AdapterCapabilities.P2PAttributes.P2PCapabilities.BackgroundDiscoverySupported,
            0);

        m_AdapterCapabilities.P2PAttributes.P2PCapabilities.BackgroundDiscoverySupported = 0;
    }

        
exit:
    TraceActExit(status);
    return status;
}

#if WDI_DISABLED    
void CMiniportInitializeJob::InitializeDataPathQICommandBuffer()
{
    m_DataPathQICommandBuffer.callbackTableSize = 
        sizeof(IDatapathCallbacks)/sizeof(PVOID);

    m_DataPathQICommandBuffer.hAdapterContext = (HANDLE) m_pAdapter;

    m_DataPathQICommandBuffer.pCallbacks = &(m_pAdapter->m_DataPathCallbacks);
}
#endif // WDI_DISABLED    


//==============================================================================================
CDeletePortJob::CDeletePortJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    CJobBase(ActivityId),
    m_pAdapter(NULL),
    m_Task(ActivityId),
    m_pPortToDelete(NULL),
    m_JobState(DeletePortJobNotStarted),
    m_PortNumberToDelete(INVALID_NDIS_PORT_NUMBER),
    m_pNotifyOperationStarted(NULL),
    m_StopBackgroundDiscoveryJob(ActivityId)
{
    CJobBase::set_RequiresPassiveIrql(true);

}

CDeletePortJob::~CDeletePortJob()
{
    if (m_CommandBuffer)
    {
        delete[] m_CommandBuffer;
        m_CommandBuffer = NULL;
    }
}

NDIS_STATUS CDeletePortJob::InitializeFromJob(
    _In_ CAdapter*  pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_opt_ INotifyOperationStarted *pNotifyOperationStarted)
{
    m_PortNumberToDelete = NdisPortNumber;
    m_pAdapter = pAdapter;
    m_pNotifyOperationStarted = pNotifyOperationStarted;

    // Some jobs reuse without deleting and cleaning up
    if (m_CommandBuffer)
    {
        delete[] m_CommandBuffer;
        m_CommandBuffer = NULL;
    }

    return CJobBase::Initialize(
            m_pAdapter->GetEventQueue(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetActiveJobsList(),
            m_pAdapter->GetSerializedJobList(),
            WiFiJobDeletePort,
            FALSE,
            FALSE,
            WiFiPriorityScopeAdapter,
            WiFiSerializedJobPriority_1,
            (IPropertyCacheDirectory *)pAdapter
            );
}

NDIS_STATUS CDeletePortJob::StartDeletePortTask()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    DeviceCommand* pDeviceCommand = NULL;
    
    WDI_TASK_DELETE_PORT_PARAMETERS deletePortParametersTlv;
    RtlZeroMemory(&deletePortParametersTlv, sizeof(WDI_TASK_DELETE_PORT_PARAMETERS));

    TraceActEntry();

    // Send the delete port job to the device
    ndisStatus = m_Task.Initialize(
            m_pAdapter->GetNotificationManager(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetEventQueue()
            );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Delete Port Task (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = m_Task.get_TaskDeviceCommand(&pDeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to get Device Command for Delete Port Task (0x%x)", ndisStatus);
        goto Exit;
    }

    deletePortParametersTlv.DeletePortParameters.PortNumber = m_pPortToDelete->GetPortId();
    ULONG CommandLength;

    ndisStatus = GenerateWdiTaskDeletePort(&deletePortParametersTlv,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &CommandLength,
        (UINT8**)&m_CommandBuffer
        );

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to Generate TLV for WDI_TASK_DELETE_PORT (0x%x)", ndisStatus);
        goto Exit;
    }

    ndisStatus = pDeviceCommand->Initialize(
        WDI_PORT_ID_ADAPTER,
        WDI_TASK_DELETE_PORT,
        CommandLength,
        m_CommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for Delete Port Task (0x%x)", ndisStatus);
        goto Exit;
    }

    // Save the start state so that when complete happens, we know where we are
    m_JobState = DeletePortJobWaitingForPortDelete;

    ndisStatus = StartTask(&m_Task);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Delete Port Task (0x%x)", ndisStatus);
        goto Exit;
    }
    
Exit:

    TraceActExit(ndisStatus); 
    
    return ndisStatus;
}

NDIS_STATUS CDeletePortJob::CompleteDeletePortTask(
    _In_ NDIS_STATUS OperationStatus)
{
    NDIS_STATUS taskStatus;

    TraceActEntry();
    WFCActInfo("Delete Port Task completed (0x%x)", OperationStatus);
    
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        goto Exit;
    }

    // Get the results from the buffer

    // Get the results from the TLV
    OperationStatus = CMessageHelper::GetStatusFromTaskOutput(
        &m_Task,
        &taskStatus
        );
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Delete Port Task failed with status (0x%x) %d", 
                    OperationStatus, taskStatus);
        goto Exit;
    }

Exit:
    //Even if the task failed we still need to clean up the datapath appropriately.
    taskStatus = m_pAdapter->m_TxMgr.RemovePortId(m_pPortToDelete->GetPortId());
    if (taskStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("TxMgr failed to delete port(0x%x)", taskStatus);
        if (OperationStatus == NDIS_STATUS_SUCCESS)
        {
            OperationStatus = taskStatus;
        }
    }
    //TODO make sure TxMgr/RxMgr and port states are ok
    m_pAdapter->m_MiniportDataHandlers.TalTxRxDeletePortHandler(m_pAdapter->GetMiniportTalTxRxContext(),
        m_pPortToDelete->GetPortId());

    TraceActExit(OperationStatus); 

    return OperationStatus;
}

NDIS_STATUS CDeletePortJob::StartPortJobCleanup()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    m_JobState = DeletePortJobWaitingForPortJobsCleanup;

    WFCActInfo("Cleaning up port queued jobs");

    // If there is an IHV triggered roam job queued prevent the job from being requeued by setting
    // the WfcPortPropertyIhvRoamRequested to false.
    ndisStatus = m_pPortToDelete->GetPropertyCache()->SetPropertyBoolean(
        WfcPortPropertyIhvRoamRequested,
        FALSE
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Couldn't clear flag for IHV Roam requested. Error = 0x%08x", ndisStatus);
        // Continue on
    }

    // Flush all other jobs for this port
    m_pAdapter->GetSerializedJobList()->FlushQueuedJobs(m_PortNumberToDelete);

    ndisStatus = m_pPortToDelete->WaitForPendingJobCompletion(this);
    if (ndisStatus == NDIS_STATUS_PENDING)
    {
        SetPending();
    }

    if (ndisStatus != NDIS_STATUS_PENDING)
    {
        // Since the model is always sync, call
        // OnJobStepCompleted when we are doing a sync complete
        CDeletePortJob::OnJobStepCompleted(
            ndisStatus,
            this);
    }
    else
    {
        // This indicates that the start was successful to prevent the job from 
        //  completing too soon
        ndisStatus = NDIS_STATUS_SUCCESS;
    }

    TraceActExit(ndisStatus);

    return ndisStatus;
}

void CDeletePortJob::FinishJob(
    _In_ NDIS_STATUS JobCompleteStatus)
{    
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    // Remove the port from the adapter's list
    ndisStatus = m_pAdapter->RemovePort(m_pPortToDelete);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to forget Port to delete (0x%x)", ndisStatus);

        // This is a problem
        NT_ASSERT(FALSE);
        goto exit;
    }

    // If the port was present, delete it
    if (m_pPortToDelete)
    {
        delete m_pPortToDelete;
        m_pPortToDelete = NULL;
    }

    if (m_PortNumberToDelete != NDIS_DEFAULT_PORT_NUMBER && 
        m_PortNumberToDelete != INVALID_NDIS_PORT_NUMBER)
    {
        // Deactivate and Free need to be async
        //
        m_pAdapter->m_ISpinLockDelayDeactivation->EnterCriticalSection();

        //
        // put the ndis port in the table
        //
        int i;
        for ( i=0; i < m_pAdapter->c_MaxPorts; i++ )
        {
            if ( m_pAdapter->m_NdisPortNumToDeactivate[i] == 0 )
            {
                break;
            }
        }

        if ( i < m_pAdapter->c_MaxPorts )
        {
            m_pAdapter->m_NdisPortNumToDeactivate[i] = m_PortNumberToDelete;
        }
        else 
        {
            WFCError("Deactivate Port table full");
        }

        //
        // if worker is not running, trigger it
        //
        if ( !m_pAdapter->m_bDeactivateWorkerActive )
        {
            m_pAdapter->m_pWorkItemDeactivatePort->QueueWorkItem();
            m_pAdapter->m_bDeactivateWorkerActive = TRUE;
        }

        m_pAdapter->m_ISpinLockDelayDeactivation->LeaveCriticalSection();
    }

exit:

    CompleteJob(JobCompleteStatus);
}

void CDeletePortJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    TraceActEntry();

    // Convert the port number to CPort
    m_pPortToDelete = m_pAdapter->GetPortFromNdisPortNumber(m_PortNumberToDelete);
    if (m_pPortToDelete == NULL)
    {
        WFCActError("Unable to find the port (%d) to delete", 
            m_PortNumberToDelete);
        ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
        goto Exit;
    }

    // If the P2P BackgroundDiscovery task is running, it should be stopped
    if ((m_pPortToDelete->GetPortType() == WfcPortTypeWFDDevice) &&
        (m_pAdapter->GetBSSListManager(WfcPortTypeWFDDevice)->IsBackgroundDiscoveryEnabled()))
    {
        DOT11_WFD_BACKGROUND_DISCOVERY_REQUEST Dot11WFDDiscovery = {0};

        ndisStatus = m_StopBackgroundDiscoveryJob.InitializeFromJob(
            m_pAdapter,
            m_PortNumberToDelete );
        if ( ndisStatus == NDIS_STATUS_SUCCESS )
        {
            // Save the start state so that when complete happens, we know where we are
            m_JobState = DeletePortJobWaitingForStopBackgroundDiscovery;

            ndisStatus = StartChildJob(&m_StopBackgroundDiscoveryJob);
            if (ndisStatus == NDIS_STATUS_SUCCESS)
            {
                // We started the Job, so we need to just exit
                WFCActError("Started StopBackgroundDiscovery Job ...");
                goto Exit;
            }

            WFCActError("Failed to start StopBackgroundDiscovery Job (0x%x)", ndisStatus);
        }
        else
        {
            WFCActError("Failed to initialize StopBackgroundDiscovery Job (0x%x)", ndisStatus);
        }

        //
        // Do best-effort to tell the Services manager to stop tracking the services
        //
        m_pAdapter->GetServicesManager()->SetNewBackgroundDiscoveryContexts(
            FALSE,                                    // Stop indicating device / service changes
            sizeof(Dot11WFDDiscovery),
            (PBYTE) &Dot11WFDDiscovery);
    }

    // Launch the delete port task
    ndisStatus = StartDeletePortTask();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Delete Port Task (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:

    //
    // notify any parent that the job has started
    //
    if (m_pNotifyOperationStarted)
    {
        m_pNotifyOperationStarted->OnOperationStarted (ndisStatus, this);
    }

    // If anything failed, we will complete the job right now
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        FinishJob(ndisStatus);
    }

    TraceActExit(ndisStatus);    
}

void CDeletePortJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    BOOLEAN completeJob = FALSE;
    
    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    switch(m_JobState)
    {
        case DeletePortJobWaitingForStopBackgroundDiscovery:
            {
                OperationStatus = StartDeletePortTask();
            }
            break;
        case DeletePortJobWaitingForPortDelete:
            {
                OperationStatus = CompleteDeletePortTask(OperationStatus);
                //Regardless of CompleteDeletePortTask status StartPortJobCleanup needs to run
                // since the port will be deleted anyways
                OperationStatus = StartPortJobCleanup();
            }
            break;
        case DeletePortJobWaitingForPortJobsCleanup:
            {
                completeJob = TRUE;
            }
            break;
        default:
            NT_ASSERT(FALSE);
            break;
    }

    if ((OperationStatus != NDIS_STATUS_SUCCESS) ||
        (completeJob == TRUE))
    {
        FinishJob(OperationStatus);
    }
    
    TraceActExit(OperationStatus);
}


void CDeletePortJob::OnJobCancelled()
{
    // Do nothing for Delete Port task
}

//==============================================================================================

CMiniportHaltJob::CMiniportHaltJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    CJobBase(ActivityId),
    m_pAdapter(NULL),
    m_Task(ActivityId),
    m_CommandBuffer(),    
#if WDI_DISABLED    
    m_DataPathQICommandBuffer(),
    m_DataPathReleaseInterfaceCommand(ActivityId),
#endif // WDI_DISABLED    
    m_JobState(MiniportHaltJobNotStarted),
    m_DeletePortJob(ActivityId)
{
    CJobBase::set_RequiresPassiveIrql(true);

}

NDIS_STATUS CMiniportHaltJob::Initialize(
    _In_ CAdapter* pAdapter
    )
{
    m_pAdapter = pAdapter;

#if WDI_DISABLED
    InitializeDataPathRICommandBuffer();
#endif // WDI_DISABLED
    return CJobBase::Initialize(
            m_pAdapter->GetEventQueue(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetActiveJobsList(),
            m_pAdapter->GetSerializedJobList(),
            WiFiJobMiniportHalt,
            FALSE,
            FALSE,
            WiFiPriorityScopeAdapter,
            WiFiSerializedJobPriority_1,
            (IPropertyCacheDirectory *)pAdapter
            );
}

NDIS_STATUS CMiniportHaltJob::StartDeleteAllPorts()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CPort* pPort = NULL;

    TraceActEntry();

    m_JobState = MiniportHaltJobDeletingAllPorts;
    
    // If there is a port present, launch the job to delete it
    for (INT8 i = CAdapter::c_MaxPorts; i > 0; i--)
    {
        if (m_pAdapter->m_pPortList[i-1] != NULL)
        {
            // This is the port we want to delete
            pPort = m_pAdapter->m_pPortList[i-1];
            break;
        }
    }

    if (pPort == NULL)
    {
        // Found no port to delete. We are done with the delete work
        m_bAreAllPortsDeleted = TRUE;
        goto Exit;
    }

    ndisStatus = StartDeletePortJob(
        pPort
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to intiate Delete Port Job (0x%x)", ndisStatus);
        goto Exit;
    }
Exit:
    if (m_bAreAllPortsDeleted || (ndisStatus != NDIS_STATUS_SUCCESS))
    {
        // If all ports are deleted or we had a failure, we continue the next
        // steps
        OnJobStepCompleted(NDIS_STATUS_SUCCESS, this);
    }
    
    TraceActExit(ndisStatus);

    // We need to continue cleanup
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CMiniportHaltJob::StartDeletePortJob(
    _In_ CPort* pPort
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Launch the child job to delete the port
    ndisStatus = m_DeletePortJob.InitializeFromJob(
        m_pAdapter,
        pPort->GetNdisPortNumber(),
        NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Delete Port Job (0x%x)", ndisStatus);
        goto Exit;
    }

    // Save the start state so that when complete happens, we know where we are
    m_JobState = MiniportHaltJobWaitingForDeletePortComplete;

    ndisStatus = StartChildJob(&m_DeletePortJob);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Delete Port Job (0x%x)", ndisStatus);
        goto Exit;
    }
    
Exit:

    // Caller handles failures
    TraceActExit(ndisStatus);
    return ndisStatus;
}


NDIS_STATUS CMiniportHaltJob::CompleteDeletePortJob(
    _In_ NDIS_STATUS OperationStatus)
{
    WFCActInfo("Delete Port Job completed (0x%x)", OperationStatus);

    // regardless of what the status of the previous port we should still
    // delete the next port. Note that delete port can fail if the underlying hardware
    // was surprise removed. However we should still go ahead and free up the memory
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CMiniportHaltJob::DataPathReleaseInterface()
/*++
    1. Calls into lower edge to deinit the datapath adapter
    2. Clear the ptrs to LE datapath functions
--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    m_pAdapter->CleanupDataPath();

    RtlZeroMemory(
        &m_pAdapter->m_MiniportDataApi,
        sizeof(NDIS_WDI_DATA_API));

    RtlZeroMemory(
        &m_pAdapter->m_MiniportDataHandlers,
        sizeof(NDIS_MINIPORT_WDI_DATA_HANDLERS));

    //Replace the TxPeerBacklogHandler with the stub to prevent AV.
    m_pAdapter->m_MiniportDataHandlers.TxPeerBacklogHandler = TxPeerBacklogStub;

    TraceActExit(ndisStatus);

    return ndisStatus;

}


NDIS_STATUS CMiniportHaltJob::StartCloseTask()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Save the start state so that when complete happens, we know where we are
    m_JobState = MiniportHaltJobWaitingForCloseTaskComplete;

    // Call the IHV code
    ndisStatus = m_pAdapter->GetMiniportDriver()->GetWdiCharacteristics()->CloseAdapterHandler(
        m_pAdapter->GetMiniportAdapterContext()
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        // Failed
        WFCError("WDI Miniport's CloseAdapterHandler failed. Status = 0x%08x", ndisStatus);
        // If this step fails to start, we continue the next steps and mask the failure
        OnJobStepCompleted(NDIS_STATUS_SUCCESS, this);
    }
    else
    {
        // Set the step as pending as its going to get completed asynchronously
        SetPending();
    }

    TraceActExit(ndisStatus);

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CMiniportHaltJob::CompleteCloseTask(
    _In_ NDIS_STATUS OperationStatus)
{
    TraceActEntry();
    WFCActInfo("Close Task completed (0x%x)", OperationStatus);
    
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        goto Exit;
    }

    m_pAdapter->SetFirmwareOpenState( FALSE );

Exit:
    m_pAdapter->UnmarkInitComplete(WfcAdapterInitializeOpenTaskCompleted);

    TraceActExit(OperationStatus);

    // regardless of what the status of the close task is, we continue
    // Note that close task can fail if the underlying hardware
    // was surprise removed. However we should still go ahead and free up the memory
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS CMiniportHaltJob::PerformIHVOperationsStop()
{
    MINIPORT_WDI_STOP_OPERATION_HANDLER pStopHandler = m_pAdapter->GetMiniportDriver()->GetWdiCharacteristics()->StopOperationHandler;

    if (pStopHandler && m_pAdapter->IsInitComplete(WfcAdapterInitializeIhvOperationStarted))
    {        
        // Call the IHV's code
        pStopHandler(
            m_pAdapter->GetMiniportAdapterContext() 
            );
        m_pAdapter->UnmarkInitComplete(WfcAdapterInitializeIhvOperationStarted);    
    }

    return NDIS_STATUS_SUCCESS;
}


void CMiniportHaltJob::PerformFreeAdapter()
{
    // Call the IHV's code
    if (m_pAdapter->IsInitComplete(WfcAdapterInitializeIhvAdapterAllocated))
    {
        m_pAdapter->GetMiniportDriver()->GetWdiCharacteristics()->FreeAdapterHandler(
            m_pAdapter->GetMiniportAdapterContext()
            );
        m_pAdapter->UnmarkInitComplete(WfcAdapterInitializeIhvAdapterAllocated);    
    }
    
}

void CMiniportHaltJob::FinishJob(
    _In_ NDIS_STATUS JobCompleteStatus)
{
    CompleteJob(JobCompleteStatus);
}

void CMiniportHaltJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Save any values we would like to persist across disable/enable
    m_pAdapter->PersistStateInRegistry();

    // Start by stopping all IHV operations
    ndisStatus = PerformIHVOperationsStop();
    ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);

    // Flush all other serialized jobs. We dont want anything
    // running after this job
    m_pAdapter->GetSerializedJobList()->FlushQueuedJobs(INVALID_NDIS_PORT_NUMBER);    

    // Then start deleting all existing ports. It will trigger the next step
    ndisStatus = StartDeleteAllPorts();
    ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);

    TraceActExit(ndisStatus);    
}

void CMiniportHaltJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    BOOLEAN completeJob = FALSE;
    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    switch(m_JobState)
    {
        case MiniportHaltJobWaitingForDeletePortComplete:
            {
                OperationStatus = CompleteDeletePortJob(OperationStatus);
                ASSERT(OperationStatus == NDIS_STATUS_SUCCESS);

                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    // Continue the delete port jobs.  If all ports are
                    // deleted m_bAreAllPortsDelete will be true
                    OperationStatus = StartDeleteAllPorts();
                    ASSERT(OperationStatus == NDIS_STATUS_SUCCESS);
                }
            }
            break;

        case MiniportHaltJobDeletingAllPorts:
            {
                // if all ports are deleted call Datapath release interface
                ASSERT(m_bAreAllPortsDeleted  == TRUE);
                OperationStatus = DataPathReleaseInterface();
                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {
                    if (m_pAdapter->IsInitComplete(WfcAdapterInitializeOpenTaskCompleted))
                    {
                        // Stuff is open, we continue
                        OperationStatus = StartCloseTask();
                        ASSERT(OperationStatus == NDIS_STATUS_SUCCESS);
                        
                    }
                    else
                    {
                        // Move to the next step directly
                        PerformFreeAdapter();
                        completeJob = TRUE;
                    }                
                }
            }
            break;
            
        case MiniportHaltJobWaitingForCloseTaskComplete:
            {
                OperationStatus = CompleteCloseTask(OperationStatus);
                ASSERT(OperationStatus == NDIS_STATUS_SUCCESS);
                
                if (OperationStatus == NDIS_STATUS_SUCCESS)
                {            
                    PerformFreeAdapter();
                    completeJob = TRUE;
                }
                
            }
            break;            

        default:
            NT_ASSERT(FALSE);
            break;
    }

    if ((OperationStatus != NDIS_STATUS_SUCCESS) ||
        (completeJob == TRUE))
    {
        FinishJob(OperationStatus);
    }
    
    TraceActExit(OperationStatus);
}

void CMiniportHaltJob::OnJobCancelled()
{
    // Do nothing for Halt task
}


//
//=====================================================================================================================
//
CMiniportJobBase::CMiniportJobBase(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    CJobBase(ActivityId),
    m_pAdapter(NULL),   
    m_PortJobNotifier(NULL),
    m_PortIndexNext(0)
{

}

CMiniportJobBase::~CMiniportJobBase()
{
    if (m_PortJobNotifier)
    {
        delete m_PortJobNotifier;
    }
}

NDIS_STATUS CMiniportJobBase::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ WFC_JOB_TYPE eJobType)
{
    m_pAdapter = pAdapter;

    m_PortJobNotifier = new CPortJobNotifier(this);
    if (m_PortJobNotifier == NULL)
    {
        WFCActError("Failed to create CPortJobNotifier object ");
        return NDIS_STATUS_RESOURCES;
    }

    return CJobBase::Initialize(m_pAdapter->GetEventQueue(),
                                m_pAdapter->GetDeviceCommandScheduler(),
                                m_pAdapter->GetActiveJobsList(),
                                m_pAdapter->GetSerializedJobList(),
                                eJobType,
                                FALSE,
                                FALSE,
                                WiFiPriorityScopeAdapter,
                                WiFiSerializedJobPriority_1,
                                (IPropertyCacheDirectory *)pAdapter
                                );
}

NDIS_STATUS CMiniportJobBase::QueueSerialized()
{
    // Queue the port job to do the work.
    return m_pAdapter->GetSerializedJobList()->QueueSerializedJob (this, 
                                                                    m_PortJobNotifier);
}

void CMiniportJobBase::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    //
    // start from the first port
    //
    m_PortIndexNext = 0;

    ndisStatus = StartPortJobNext();    
    if (ndisStatus != NDIS_STATUS_PENDING)
    {
        CompleteJob(ndisStatus);
    }
    else
    {
        SetPending();
    }

    TraceActExit(ndisStatus);
}

void CMiniportJobBase::OnJobStepCompleted (
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();
    
    UNREFERENCED_PARAMETER(SenderObject);
    UNREFERENCED_PARAMETER(OperationStatus);

    ndisStatus = StartPortJobNext ();
    if (ndisStatus != NDIS_STATUS_PENDING)
    {
        CompleteJob(ndisStatus);
    } 
    else
    {
        SetPending();
    }

    TraceActExit(OperationStatus);
}

void CMiniportJobBase::OnJobCancelled()
{
    // Do nothing for Halt task
}

NDIS_STATUS CMiniportJobBase::StartPortJobNext()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    //
    // If there is a port present, launch the job to pause it.
    // Pause one port at a time
    //
    for ( ; m_PortIndexNext < CAdapter::c_MaxPorts; 
          m_PortIndexNext++)
    {
        if (m_pAdapter->m_pPortList[m_PortIndexNext] == NULL)
        {
            continue;
        }

        //
        // do the job
        //
        ndisStatus = PortJobCallback();
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            continue;
        }
        else if (ndisStatus == NDIS_STATUS_PENDING)
        {
            // next time pick the next port
            m_PortIndexNext++;            
        }

        break;
    }
   
    TraceActExit(ndisStatus);

    return ndisStatus;
}

//----------------------
// pause job
//
CMiniportPauseJob::CMiniportPauseJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
        CMiniportJobBase(ActivityId)
{

}

NDIS_STATUS CMiniportPauseJob::PortJobCallback()
{
    NDIS_STATUS status;
    
    // A real NDIS MiniportPause requires waiting for all RX frames, but internal pauses must *NOT* wait
    const CPort::PauseWaitForRxReturn fWait = (m_PauseParameters != NULL) ? CPort::WaitForRxReturn : CPort::NoWaitForRxReturn;

    status = m_pAdapter->m_pPortList[m_PortIndexNext]->Pause( this, fWait );
    return status;
}

void CMiniportPauseJob::PortOperationCompletedCallback (
    _In_ NDIS_STATUS OperationStatus)
{
    MINIPORT_WDI_POST_PAUSE_HANDLER pPauseHandler = m_pAdapter->GetMiniportDriver()->GetWdiCharacteristics()->PostPauseHandler;

    TraceEntry();

    if (pPauseHandler)
    {
        if (OperationStatus == NDIS_STATUS_SUCCESS)
        {        
            WFCTrace("Invoking IHV's WDI PostPauseHandler handler");    
        
            // Call the IHV's code
            OperationStatus = pPauseHandler(
                m_pAdapter->GetMiniportAdapterContext(),
                m_PauseParameters
                );
            if (OperationStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError("IHV's PostPauseHandler failed. Error = 0x%08x", OperationStatus);        
                // We still continue with the pause completion
            }
        }
        else
        {
            WFCError("Internal Pause processing has failed. Error = 0x%08x. Not calling IHV handler", OperationStatus);        
        }
    }
    WFCTrace("Completing NdisMPauseComplete with status 0x%08x", OperationStatus);        

    // Clear the pointer
    m_PauseParameters = NULL;

    NdisMPauseComplete (m_pAdapter->GetMiniportAdapterHandle());

    TraceExit(OperationStatus);
}

//----------------------
// Surprise Removal job
//
NDIS_STATUS CMiniportSurpriseRemovalJob::QueueJob(
    _In_  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent,    
    _In_  IOperationCompletionCallback *pCompletionCallback)
{
    UNREFERENCED_PARAMETER(NetDevicePnPEvent);
    
    // Queue the port job to do the work.
    return m_pAdapter->GetActiveJobsList()->StartAddNewJob(this, pCompletionCallback);
}

CMiniportSurpriseRemovalJob::CMiniportSurpriseRemovalJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
        CMiniportJobBase(ActivityId)
{
    CJobBase::set_RequiresPassiveIrql(true);

}

NDIS_STATUS CMiniportSurpriseRemovalJob::PortJobCallback()
{
    m_pAdapter->m_pPortList[m_PortIndexNext]->SurpriseRemoved();
    return NDIS_STATUS_SUCCESS;
}

void CMiniportSurpriseRemovalJob::PortOperationCompletedCallback (
    _In_ NDIS_STATUS OperationStatus)
{
    UNREFERENCED_PARAMETER(OperationStatus);

    NT_ASSERT(FALSE);
    // This should not be called
}
//----------------------
// restart job
//
CMiniportRestartJob::CMiniportRestartJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
        CMiniportJobBase(ActivityId)
{

}

NDIS_STATUS CMiniportRestartJob::PortJobCallback()
{
    //
    // should not return pending
    //
    return m_pAdapter->m_pPortList[m_PortIndexNext]->Restart();
}

void CMiniportRestartJob::PortOperationCompletedCallback (
    _In_ NDIS_STATUS OperationStatus)
{
    MINIPORT_WDI_POST_RESTART_HANDLER pRestartHandler = m_pAdapter->GetMiniportDriver()->GetWdiCharacteristics()->PostRestartHandler;

    TraceEntry();
    if (pRestartHandler)
    {
        if (OperationStatus == NDIS_STATUS_SUCCESS)
        {        
            WFCTrace("Invoking IHV's WDI PostRestartHandler handler");    
        
            // Call the IHV's code
            OperationStatus = pRestartHandler(
                m_pAdapter->GetMiniportAdapterContext(),
                m_RestartParameters
                );
            if (OperationStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError("IHV's PostRestartHandler failed. Error = 0x%08x", OperationStatus);        
                // We still continue with the restart completion
            }
        }
        else
        {
            WFCError("Internal Restart processing has failed. Error = 0x%08x. Not calling IHV handler", OperationStatus);        
        }
    }
    WFCTrace("Completing NdisMRestartComplete with status 0x%08x", OperationStatus);    

    NdisMRestartComplete (m_pAdapter->GetMiniportAdapterHandle(), 
                        OperationStatus);

    TraceExit(OperationStatus);                        
}

//-----------------------------------------
// CCreatePortOidJob
NDIS_STATUS CCreateMacOidJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();
    
    pOidRequest->DATA.METHOD_INFORMATION.BytesWritten = 0;
    pOidRequest->DATA.METHOD_INFORMATION.BytesRead = 0;
    pOidRequest->DATA.METHOD_INFORMATION.BytesNeeded = 0;
        
    //
    // if the input buffer size is > 0, the input for this OID is formatted as a DOT11_MAC_PARAMETERS structure.
    //
    if (pOidRequest->DATA.METHOD_INFORMATION.InputBufferLength > 0)
    {
        if (pOidRequest->DATA.METHOD_INFORMATION.InputBufferLength < sizeof(DOT11_MAC_PARAMETERS)) 
        {            
            pOidRequest->DATA.METHOD_INFORMATION.BytesNeeded = sizeof(DOT11_MAC_PARAMETERS);
            WFCActError("Invalid input length");
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            goto Exit;
        } 

        m_Dot11MacParamsOpmodeMask = ((PDOT11_MAC_PARAMETERS)(pOidRequest->DATA.METHOD_INFORMATION.InformationBuffer))->uOpmodeMask;
    }
    else
    {
        m_Dot11MacParamsOpmodeMask = DOT11_OPERATION_MODE_UNKNOWN;
    }
    
    //
    // First make sure the output buffer is large enough to hold a DOT11_MAC_INFO
    //
    if (pOidRequest->DATA.METHOD_INFORMATION.OutputBufferLength < sizeof(DOT11_MAC_INFO)) 
    {
        pOidRequest->DATA.METHOD_INFORMATION.BytesNeeded = sizeof(DOT11_MAC_INFO);
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto Exit;
    }
   
    //
    // Save the start state so that when complete happens, we know where we are
    //
    m_CreatePortOidState = CreatePortOidStateInit;

Exit:

    TraceActExit(ndisStatus);

    return ndisStatus;
}


NDIS_STATUS CCreateMacOidJob::InitializeForResume(
    _In_ CPort * pPort )
/*++

    Create a dummmy OID and initialize from it.

--*/
{
    NDIS_STATUS ndisStatus;
    NDIS_OID oid = OID_DOT11_CREATE_MAC;
    POID_HANDLER_ENTRY pHandlerInfo;

    TraceActEntry();

    m_pPortToRecreate = pPort;
    m_pAdapter = pPort->GetAdapter();
    m_NdisPortNumber = pPort->GetNdisPortNumber();
    ndisStatus = pPort->GetPropertyCache()->GetPropertyULong( 
                    WfcPortPropertyCreateOpMask,
                    &m_Dot11MacParamsOpmodeMask);
    NT_ASSERT( ndisStatus == NDIS_STATUS_SUCCESS );

    //
    // Make the dummy OID
    //
    RtlZeroMemory( &m_NdisOidRequest, sizeof(NDIS_OID_REQUEST) );
    RtlZeroMemory( &m_u, sizeof( m_u ));
    m_NdisOidRequest.RequestType = NdisRequestMethod;
    m_NdisOidRequest.PortNumber = m_NdisPortNumber;
    m_NdisOidRequest.DATA.METHOD_INFORMATION.Oid = oid;
    m_NdisOidRequest.DATA.METHOD_INFORMATION.InformationBuffer = (PVOID) &m_u;
    m_NdisOidRequest.DATA.METHOD_INFORMATION.InputBufferLength = sizeof(DOT11_MAC_PARAMETERS);
    m_NdisOidRequest.DATA.METHOD_INFORMATION.OutputBufferLength = sizeof(DOT11_MAC_INFO);

    m_u.Dot11MacParams.uOpmodeMask = m_Dot11MacParamsOpmodeMask;

    pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(oid);
    NT_ASSERT( pHandlerInfo );

    ndisStatus = COidJobBase::InitializeForOid(
                        pHandlerInfo->SetMethodJobType,
                        m_pAdapter,
                        pHandlerInfo,
                        &m_NdisOidRequest);
    NT_ASSERT( NT_SUCCESS( ndisStatus ) );

    TraceActExit(ndisStatus);

    return ndisStatus;
}


void CCreateMacOidJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus;
    NDIS_STATUS ndisStatusFreePort;
    UINT16 opmodeMask = 0;
    WFC_PORT_TYPE PortType ;

    TraceActEntry();
        
    if ( (m_Dot11MacParamsOpmodeMask & DOT11_OPERATION_MODE_WFD_DEVICE) != 0 )
    {
        PortType = WfcPortTypeWFDDevice;
        opmodeMask |= WDI_OPERATION_MODE_P2P_DEVICE;
    }
    else if ( (m_Dot11MacParamsOpmodeMask & DOT11_OPERATION_MODE_WFD_GROUP_OWNER) != 0 ||
              (m_Dot11MacParamsOpmodeMask & DOT11_OPERATION_MODE_WFD_CLIENT) != 0 )
    {
        PortType = WfcPortTypeWFDRole;
        opmodeMask |= (WDI_OPERATION_MODE_P2P_CLIENT | WDI_OPERATION_MODE_P2P_GO);
    }
    else
    {
        PortType = WfcPortTypeExtSTA;
        opmodeMask |= WDI_OPERATION_MODE_STA;
    }

    
    //
    // allocating non-default port.
    //
    if ( m_pPortToRecreate == NULL )
    {
        ndisStatus = AllocateNdisPort(&m_CreatedNdisPortNumber);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("AllocateNdisPort failed (0x%x)", ndisStatus);
            goto Exit;
        } 
    }
    else
    {
        m_CreatedNdisPortNumber = m_pPortToRecreate->GetNdisPortNumber();
    }

    WFCActInfo ("calling m_CreatePortChildJob.InitializeFromJob: Dot11-Mac-Params-OpmodeMask(oid) 0x%x, port type %u, wabi uOpmodeMask 0x%x ndisPortNumber %u", 
            m_Dot11MacParamsOpmodeMask, 
            PortType, 
            opmodeMask,
            m_CreatedNdisPortNumber);
    //
    // Launch the child job to create the default port
    //
    ndisStatus = m_CreatePortChildJob.InitializeFromJob(m_pAdapter,
                                        PortType,
                                        opmodeMask,
                                        m_CreatedNdisPortNumber,
                                        (m_pPortToRecreate != NULL)? TRUE : FALSE );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Create Port Job (0x%x)", ndisStatus);
        goto Exit;
    }
    
    m_CreatePortOidState = CreatePortOidStateWaitingForCreatePortChildJob;

    ndisStatus = StartChildJob(&m_CreatePortChildJob);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to start Create Port Job (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:

    //
    // If anything failed, we will complete the job right now
    //
    if (ndisStatus != NDIS_STATUS_SUCCESS )
    {
        if (  m_pPortToRecreate == NULL )
        {
            //
            // Complete the OID back to NDIS
            //
            m_pAdapter->CompleteOidRequest(COidJobBase::GetOidRequest(),
                                        ndisStatus);
            m_OidIsCompleted = TRUE;

            //
            // free the allocated ndis port
            //
            if (m_CreatedNdisPortNumber != INVALID_NDIS_PORT_NUMBER)
            {
                ndisStatusFreePort = NdisMFreePort(m_pAdapter->GetMiniportAdapterHandle(), 
                                                m_CreatedNdisPortNumber);
                if (ndisStatusFreePort != NDIS_STATUS_SUCCESS)
                {
                    WFCActInfo("NdisMFreePort Failed (0x%x)", ndisStatusFreePort);
                }
                m_CreatedNdisPortNumber = INVALID_NDIS_PORT_NUMBER;
            }
        }
        else
        {
            // failed for Resume, should not fail or nothing much we can do
            NT_ASSERT( 0 );
        }

        CompleteJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
    return;
}

void CCreateMacOidJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(SenderObject);
    NDIS_STATUS ndisStatus;
    PNDIS_OID_REQUEST pOidRequest;
    PDOT11_MAC_INFO pDot11MacInfo;

    TraceActEntry();

    switch( m_CreatePortOidState )
    {
        case CreatePortOidStateWaitingForCreatePortChildJob:
            
            WFCActInfo ("CCreateMacOidJob->createPortChildJob step : OperationStatus 0x%x", OperationStatus);

            //
            // If the port create succeeded, then restart the port unless the default port is paused/pausing
            //
            if (OperationStatus == NDIS_STATUS_SUCCESS)
            {
                CPort* pPort = m_pAdapter->GetPortFromNdisPortNumber(m_CreatedNdisPortNumber);
                CPort* pDefaultPort = m_pAdapter->GetPortFromNdisPortNumber(0);

                ndisStatus = pPort->GetPropertyCache()->SetPropertyULong( 
                                WfcPortPropertyCreateOpMask,
                                m_Dot11MacParamsOpmodeMask);
                NT_ASSERT( ndisStatus == NDIS_STATUS_SUCCESS );

                NT_ASSERT(pPort != NULL);
                NT_ASSERT(pDefaultPort != NULL);

                // Restart this port since it comes up as Paused if it is NDISPortNumber 0 or if NDISPortNumber 0 is not paused/pausing
                if (m_CreatedNdisPortNumber == 0 || !TEST_FLAG(pDefaultPort->GetPnpState(), PORT_PAUSED | PORT_PAUSING))
                {
                    ndisStatus = pPort->Restart();
                }
                else
                {
                    WFCActInfo ("NDIS Port 0 is in paused/pausing state. Not restarting New Port (%u)", m_CreatedNdisPortNumber);
                    ndisStatus = NDIS_STATUS_SUCCESS;
                }
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCActError("Step Restart New Port Failed (0x%x)", ndisStatus);
                    OperationStatus = ndisStatus;
                }
            }


            if (OperationStatus == NDIS_STATUS_SUCCESS)
            {
                pOidRequest = COidJobBase::GetOidRequest();

                pDot11MacInfo = (PDOT11_MAC_INFO)pOidRequest->DATA.METHOD_INFORMATION.InformationBuffer;
                //
                // get the created ndis port number
                //
                pDot11MacInfo->uNdisPortNumber = m_CreatedNdisPortNumber;

                //
                // get the created ndis port mac address1
                //           
                NdisMoveMemory(pDot11MacInfo->MacAddr, (void*)m_CreatePortChildJob.GetMacAddress().Address, sizeof(DOT11_MAC_ADDRESS));
                pOidRequest->DATA.METHOD_INFORMATION.BytesWritten = sizeof(DOT11_MAC_INFO);

                WFCActInfo ("CCreateMacOidJob->createPortChildJob step : uNdisPortNumber %u, Mac=%!MACADDR!, BytesWritten=<%d>",
                    pDot11MacInfo->uNdisPortNumber, pDot11MacInfo->MacAddr, pOidRequest->DATA.METHOD_INFORMATION.BytesWritten);
            }
            else
            {
                NT_ASSERT (m_CreatedNdisPortNumber != INVALID_NDIS_PORT_NUMBER);
        
                ndisStatus = NdisMFreePort(m_pAdapter->GetMiniportAdapterHandle(), 
                                           m_CreatedNdisPortNumber);
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCActError("Step CreatePortChildJob: NdisMFreePort Failed (0x%x)", ndisStatus);
                } 
                m_CreatedNdisPortNumber = INVALID_NDIS_PORT_NUMBER;
            }

            m_OidIsCompleted = TRUE;
            //
            // Complete the OID back to NDIS, only do so if it's not a dummy
            //
            if ( m_pPortToRecreate == NULL )
            {
                m_pAdapter->CompleteOidRequest(COidJobBase::GetOidRequest(),
                                            OperationStatus);
            }

            //
            // make sure on failure goto exit
            //
            if (OperationStatus != NDIS_STATUS_SUCCESS)
            {
                ndisStatus = OperationStatus;
                goto Exit;
            }
            
            //-------------------------------

            //
            // Make the event queue call back on a work item thread
            //
            ndisStatus = m_pEventQueue->DeferToWorkItem(this,
                                                       &m_AsyncCallbackEvent);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                //
                // TODO: we are not goint to activate the port. probably we should free the allocated port.
                //
                // TODO: Should we start a job to delete the port in the device?
                //
                WFCActError("m_pEventQueue->DeferToWorkItem failed (0x%x)", ndisStatus);
                goto Exit;
            }
                        
            //
            // set pending and wait for the next step
            //
            ndisStatus = SetPending();

            m_CreatePortOidState = CreatePortOidStateWaitingForWorkItemCallback;
            
            break;

        case CreatePortOidStateWaitingForWorkItemCallback:

            WFCActInfo ("WorkItemCallback step: OperationStatus 0x%x", OperationStatus);

            //
            // TODO: if we are not goint to activate the port. probably we should free the allocated port.
            //
            // TODO: Should we start a job to delete the port in the device?
            //
            // make sure on failure goto exit
            //
            ndisStatus = OperationStatus;
            if (OperationStatus != NDIS_STATUS_SUCCESS)
            {
                goto Exit;
            }

            if ( m_pPortToRecreate == NULL ) // not for resume is the normal path
            {
                //
                // Activate needs to be async; out the ndis port in the table and schedule an workitem
                //
                m_pAdapter->m_ISpinLockDelayActivation->EnterCriticalSection();

                //
                // put the ndis port in the table
                //
                int i;
                for ( i=0; i < m_pAdapter->c_MaxPorts; i++ )
                {
                    if ( m_pAdapter->m_NdisPortNumToActivate[i] == 0 )
                    {
                        break;
                    }
                }
                
                if ( i < m_pAdapter->c_MaxPorts )
                {
                    m_pAdapter->m_NdisPortNumToActivate[i] = m_CreatedNdisPortNumber;
                }
                else 
                {
                    WFCError("Aactivate Port table full");
                    NT_ASSERT( FALSE );
                }
                
                //
                // if worker is not running, trigger it
                //
                if ( !m_pAdapter->m_bActivateWorkerActive )
                {
                    m_pAdapter->m_pWorkItemActivatePort->QueueWorkItem();
                    m_pAdapter->m_bActivateWorkerActive = TRUE;
                }
                m_pAdapter->m_ISpinLockDelayActivation->LeaveCriticalSection();
            }
            //
            // complete this job
            //
            CompleteJob(ndisStatus);

            goto Exit;

        default:
            ndisStatus = NDIS_STATUS_NOT_RECOGNIZED;
            NT_ASSERT(FALSE);
    }
      
Exit:
    
    //
    // If anything failed, we will complete the job right now
    //
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (!m_OidIsCompleted)
        {
            //
            // Complete the OID back to NDIS
            //
            m_pAdapter->CompleteOidRequest(COidJobBase::GetOidRequest(),
                                        ndisStatus);
            m_OidIsCompleted = TRUE;
        }

        CompleteJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
    
}

NDIS_STATUS CCreateMacOidJob::AllocateNdisPort(
    _Out_ PNDIS_PORT_NUMBER  AllocatedPortNumber)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_PORT_CHARACTERISTICS   portChar;
    
    TraceActEntry();

    // Call NDIS to allocate the port
    NdisZeroMemory(&portChar, sizeof(NDIS_PORT_CHARACTERISTICS));

    ASSIGN_NDIS_OBJECT_HEADER(portChar.Header, NDIS_OBJECT_TYPE_DEFAULT,
        NDIS_PORT_CHARACTERISTICS_REVISION_1, sizeof(NDIS_PORT_CHARACTERISTICS));

    portChar.Flags = NDIS_PORT_CHAR_USE_DEFAULT_AUTH_SETTINGS;
    portChar.Type = NdisPortTypeUndefined;
    portChar.MediaConnectState = MediaConnectStateConnected;
    portChar.XmitLinkSpeed = NDIS_LINK_SPEED_UNKNOWN;
    portChar.RcvLinkSpeed = NDIS_LINK_SPEED_UNKNOWN;
    portChar.Direction = NET_IF_DIRECTION_SENDRECEIVE;
    //
    // TODO: check these 4 lines!
    //
    portChar.SendControlState = NdisPortControlStateUnknown;
    portChar.RcvControlState = NdisPortControlStateUnknown;
    portChar.SendAuthorizationState = NdisPortAuthorizationUnknown; 
    portChar.RcvAuthorizationState = NdisPortAuthorizationUnknown; 
               
    ndisStatus = NdisMAllocatePort(m_pAdapter->GetMiniportAdapterHandle(), &portChar);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to allocate NDIS port. Status = 0x%08x\n",
                ndisStatus);
        *AllocatedPortNumber = 0xFFFFFFFFU;
    }
    else
    {
        WFCActInfo ("Associated Port Number %d with allocated port\n", 
                portChar.PortNumber);
             
        //
        // Return the NDIS port number that has been allocated to this port
        //
        *AllocatedPortNumber = portChar.PortNumber;
    }
    
    TraceActExit(ndisStatus); 

    return ndisStatus;
}

//-------------------------------------------------------

CDeleteMacOidJob::~CDeleteMacOidJob()
{
    //
    // If for some reason the child job failed to open the gate open it here
    //
    if (m_pPortAccessGate != NULL && m_pPortAccessGate->IsClosed())
    {
        m_pPortAccessGate->Open();
    }
}

// CDeleteMactOidJob
NDIS_STATUS CDeleteMacOidJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_MAC_INFO dot11MacInfo;

    TraceActEntry();
    
    pOidRequest->DATA.METHOD_INFORMATION.BytesRead = 0;
    pOidRequest->DATA.METHOD_INFORMATION.BytesNeeded = 0;
      
    if (pOidRequest->RequestType != NdisRequestSetInformation)
    {
        WFCActError ("Invalid request type %d for OID_DOT11_DELETE_MAC",  pOidRequest->RequestType);
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    }
    
    //
    // First make sure the input buffer is large enough to hold a DOT11_MAC_INFO
    //  
    if (pOidRequest->DATA.SET_INFORMATION.InformationBufferLength < sizeof(DOT11_MAC_INFO))
    {
        WFCActError ("The buffer being passed into OID_DOT11_DELETE_MAC is too small(%d)", 
                    pOidRequest->DATA.SET_INFORMATION.InformationBufferLength);
        pOidRequest->DATA.SET_INFORMATION.BytesNeeded = sizeof(DOT11_MAC_INFO);
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto exit;
    }

    dot11MacInfo = (PDOT11_MAC_INFO)pOidRequest->DATA.SET_INFORMATION.InformationBuffer;

    if (dot11MacInfo->uNdisPortNumber > CAdapter::c_MaxPorts ||
        dot11MacInfo->uNdisPortNumber == NDIS_DEFAULT_PORT_NUMBER) 
    {
        WFCActError ("The port number (%d) being passed in is invalid", dot11MacInfo->uNdisPortNumber);
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }
    
    m_DeleteNdisPortNumber = dot11MacInfo->uNdisPortNumber;
    
    WFCActInfo ("ndis The port number to delete (%d)", m_DeleteNdisPortNumber);

exit:

    TraceActExit(ndisStatus);

    return ndisStatus;
}

void CDeleteMacOidJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus;

    TraceActEntry();  

    //
    // Initialize the child job to delete the port
    //
    ndisStatus = m_DeletePortChildJob.InitializeFromJob(m_pAdapter,
                                                        m_DeleteNdisPortNumber,
                                                        this);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Delete Port Job (0x%x)", ndisStatus);
        goto exit;
    }

    //
    // try to defer the oid work
    //
    m_DeleteMacOidState = DeleteMacOidStateWaitingForWorkItem;
    
    ndisStatus = m_pEventQueue->DeferToWorkItem (this, &m_AsyncCallbackEvent);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("m_pEventQueue->DeferToWorkItem(this(%p)) returned = 0x%x. m_DeleteNdisPortNumber = %u", this, ndisStatus, m_DeleteNdisPortNumber);       
        goto exit;
    }

    SetPending();

exit:

    //
    // If anything failed, we will complete the job right now
    //
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        //
        // Complete the OID back to NDIS
        //
        m_pAdapter->CompleteOidRequest(COidJobBase::GetOidRequest(),
                                    ndisStatus);
        m_OidIsCompleted = TRUE;

        CompleteJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
    return;
}

void CDeleteMacOidJob::OnOperationStarted(
    _In_ NDIS_STATUS OperationStatus,
    _In_opt_ WFC_SENDER_OBJECT SenderObject)
/*
    completes the delete mac oid
*/
{
    UNREFERENCED_PARAMETER(SenderObject);

    TraceActEntry();  

    if (!m_OidIsCompleted)
    {
         WFCActInfo("Calling Adapter->CompleteOidRequest: OperationStatus 0x%x. m_DeleteNdisPortNumber = %u", 
                    OperationStatus, 
                    m_DeleteNdisPortNumber);       

        //
        // Complete the OID back to NDIS
        //
        m_pAdapter->CompleteOidRequest(COidJobBase::GetOidRequest(),
                                    OperationStatus);
        m_OidIsCompleted = TRUE;
    }
}

void CDeleteMacOidJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(SenderObject);
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CPort* pPort;
    
    TraceActEntry();

    switch (m_DeleteMacOidState)
    {
        case DeleteMacOidStateWaitingForWorkItem:   

            WFCActInfo("DeleteMac...WorkItem  step completed = 0x%x. m_DeleteNdisPortNumber = %u", 
                        OperationStatus, 
                        m_DeleteNdisPortNumber);    
            //
            // get the port.
            //
            pPort = m_pAdapter->GetPortFromNdisPortNumber(m_DeleteNdisPortNumber);
            if (!pPort)
            {
                WFCActError("Could not find a port for m_DeleteNdisPortNumber = %u", m_DeleteNdisPortNumber);
                ndisStatus = NDIS_STATUS_INVALID_PORT;
                goto exit;
            } 

            m_DeleteMacOidState = DeleteMacOidStateWaitingForPausePortChildJob;

            // Before we can delete the port, we *must* wait for all rx frames to be returned.
            ndisStatus = pPort->Pause( this, CPort::WaitForRxReturn );
            WFCActInfo("pPort(%p)->Pause returned = 0x%x. m_DeleteNdisPortNumber = %u", pPort, ndisStatus, m_DeleteNdisPortNumber);       
            if (ndisStatus == NDIS_STATUS_PENDING)
            {
                //
                // wait for pausing step complete
                //
                SetPending();
                ndisStatus = NDIS_STATUS_SUCCESS;
                goto exit;
            }
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {            
                WFCActError("pPort(%p)->Pause failed 0x%x", pPort, ndisStatus);
                goto exit;
            }

            //
            // if pause completed syncronously, just go to the next step
            /*
            break;
            */

        case DeleteMacOidStateWaitingForPausePortChildJob:
            
             WFCActInfo("DeleteMac..PausePortChildJob step completed = 0x%x. m_DeleteNdisPortNumber = %u", 
                        OperationStatus, 
                        m_DeleteNdisPortNumber);       

            //
            // make sure pause port succeeded
            //
            if (OperationStatus != NDIS_STATUS_SUCCESS)
            {
                ndisStatus = OperationStatus;
                goto exit;
            }

            //
            // get the port gate.
            //
            pPort = m_pAdapter->GetPortFromNdisPortNumberAndLock (m_DeleteNdisPortNumber,
                                                                &m_pPortAccessGate);
            if (!pPort)
            {
                WFCActError("Could not find a port for m_DeleteNdisPortNumber = %u", m_DeleteNdisPortNumber);
                ndisStatus = NDIS_STATUS_INVALID_PORT;
                goto exit;
            }    
            //
            // only this job is going to free the port so it must release current lock/gate first, before exclusively locking/closing the gate
            //
            m_pPortAccessGate->Leave();
    
            //-----------------------------------------------
            // 
            // next step, close the port gate
            //
            m_DeleteMacOidState = DeleteMacOidStateWaitingForClosePortGateStep;

            //
            // FALSE means CloseAsync will succeed asynchronously.
            //
            if (!m_pPortAccessGate->CloseAsync(this))
            {                 
                 WFCActInfo ("m_pPortAccessGate->CloseAsync went async");  
                 SetPending();
                 ndisStatus = NDIS_STATUS_SUCCESS;
                 goto exit;
            }
            
            //
            // the gate was successfully closed. we may just pass through to the next step. no need to defer a work item
            //
            /*
            break;
            */
        case DeleteMacOidStateWaitingForClosePortGateStep:

            WFCActInfo("DeleteMac..ClosePortGateStep step, m_DeleteNdisPortNumber = %u", m_DeleteNdisPortNumber);      

            m_DeleteMacOidState = DeleteMacOidStateWaitingForDeletePortJob;

            //
            // start the delete port job
            //
            ndisStatus = StartChildJob (&m_DeletePortChildJob);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCActError("Failed to start Delete Port Job (0x%x)", ndisStatus);
                goto exit;
            }

            break;

        case DeleteMacOidStateWaitingForDeletePortJob:

            WFCActInfo("DeleteMac..DeletePortJob step completed = 0x%x. m_DeleteNdisPortNumber = %u", 
                        OperationStatus, 
                        m_DeleteNdisPortNumber);       

            //
            //  open the port gate. must be closed 
            //
            NT_ASSERT (m_pPortAccessGate->IsClosed());
            
            m_pPortAccessGate->Open();
            m_pPortAccessGate = NULL;
            
            //
            // if for some reason the oid hasn't been completed
            //
            if (!m_OidIsCompleted)
            {
                //
                // Complete the OID back to NDIS
                //
                m_pAdapter->CompleteOidRequest(COidJobBase::GetOidRequest(),
                                            OperationStatus);
                m_OidIsCompleted = TRUE;
            }

            //
            // complete this job
            //
            CompleteJob (OperationStatus);

            break;
        default:
            ndisStatus = NDIS_STATUS_NOT_RECOGNIZED;

            NT_ASSERT(FALSE);
    }
      
exit:
    
    //
    // If anything failed, we will complete the job right now
    //
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (!m_OidIsCompleted)
        {
            //
            // Complete the OID back to NDIS
            //
            m_pAdapter->CompleteOidRequest(COidJobBase::GetOidRequest(),
                                        ndisStatus);
            m_OidIsCompleted = TRUE;
        }

        //
        // If for some reason the gate is still closed, open it here
        //
        if (m_pPortAccessGate != NULL && m_pPortAccessGate->IsClosed())
        {
            WFCActInfo("If for some reason the gate is still closed, open it here m_pPortAccessGate->Open()"); 

            m_pPortAccessGate->Open();
            m_pPortAccessGate = NULL;
        }

        //
        // complete this job
        //
        CompleteJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
    
}


//++ CResetRecovery ++
NDIS_STATUS CResetRecoveryJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
/*++

Simple job to handle OID_DOT11_ADAPTER_RESET_RECOVERY.
If this is queued behind a command, that will be timed out.
It's best to let the latter happen so debug information is collect on the command

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    DeviceCommandScheduler * commandScheduler;
    DeviceCommand * pActiveCommand;
    ULONG commandId = 0;
    ULONG rrCount = 0;

    /// code coverage
    /// __debugbreak();
    TraceEntry();

    pInformationBuffer, InformationBufferLength;

    *pBytesRead = sizeof( DOT11_ADAPTER_RESET_RECOVERY_INFO ); // checked by the OID table
    *pBytesNeeded = sizeof( DOT11_ADAPTER_RESET_RECOVERY_INFO );

    commandScheduler = m_pAdapter->GetDeviceCommandScheduler();
    pActiveCommand = commandScheduler->GetActiveCommand();
    if ( pActiveCommand ) 
    {
        pActiveCommand->get_CommandType( & commandId );
    }

    NdisWriteErrorLogEntry(m_pAdapter->GetMiniportAdapterHandle(),
                        NDIS_ERROR_CODE_HARDWARE_FAILURE,
                        2,
                        RESET_RECOVERY_OID_REQUEST,
                        commandId);
    
    InterlockedIncrement( &m_pAdapter->m_lResetTriggerCount );

    BOOLEAN bResetRecoveryOff = m_pAdapter->GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(
        WfcAdapterResetRecoveryOff, FALSE );

    m_pAdapter->TraceLoggingResetRecovery( RESET_RECOVERY_OID_REQUEST,
                                commandId,
                                m_pAdapter->m_lResetTriggerCount,
                                m_pAdapter->m_OemPLDRSupported,
                                bResetRecoveryOff );

    // We dont check for MiniportResetHandler here because this operation will
    // be failed with NDIS_STATUS_NOT_SUPPORTED if m_OemPLDRSupported is 0

    rrCount = InterlockedIncrement( &m_pAdapter->m_lResetRecovery );
    if ( rrCount > 1 )
    {
        //
        // RR has already occurred, be done.
        //
        // but give the same error code as last time, so that user mode can decide to
        // do disable/enable as appropriate.
        //
        if (m_pAdapter->m_OemPLDRSupported == 0)
        {
            ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        }
        else if (bResetRecoveryOff)
        {
            ndisStatus = NDIS_STATUS_REQUEST_ABORTED;
        }
        else
        {
            //
            // PLDR is fully enabled, the caller just needs to be more patient
            //
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
        WFCActInfo( "Processing Reset Recovery OID when it has already occurred, done (%x)", ndisStatus );
        goto exit;
    }
    
    //
    // Go get Diagnose info, this should be RR OID self
    //
    if ( pActiveCommand )
    {
        m_pAdapter->TriggerControlPathDiagnostics( commandId );
        m_pAdapter->CollectDebugData( pActiveCommand,
            NULL,
            pActiveCommand->get_ParentJob());
    }
    else
    {
        m_pAdapter->TriggerControlPathDiagnostics( 0 );
        m_pAdapter->CollectDebugData( NULL, NULL, NULL);
    }

    //
    // Diagnosis taken, check if OEM does not enable UEFI support
    //
    if ( m_pAdapter->m_OemPLDRSupported == 0 )
    {
        //
        // OEM disables RR support, return NOT_SUPPORTED
        //
        WFCError( "Return Reset Recovery OID STATUS_NOT_SUPPORTED becuase OEM disables it" );
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    }

    //
    // if RR is disabled, dont request it.
    //
    if (bResetRecoveryOff == FALSE)
    {
        ASSERT(FALSE);
        GetAdapter()->ReenumerateFailedAdapter();
    }
    else
    {
        WFCError( "Return Reset Recovery OID NDIS_STATUS_REQUEST_ABORTED because it is disabled" );
        ndisStatus = NDIS_STATUS_REQUEST_ABORTED;
    }

exit:
    TraceExit( ndisStatus );
    return ndisStatus;
}

