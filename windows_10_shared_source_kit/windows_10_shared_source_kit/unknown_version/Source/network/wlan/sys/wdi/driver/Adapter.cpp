/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Adapter.cpp

Abstract:

    Implementation for most of the adapter class functionality

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"

#include "Adapter.hpp"
#include "Registry.hpp"
#include "Adapter.tmh"

#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(array) (sizeof(array)/sizeof(array[0]))
#endif

WFC_REG_ENTRY g_AdapterRegTable[] = {
    {
        NDIS_STRING_CONST("Test_FeatureMask"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegTest_FeatureMask,
        0,
        0,
        0xFFFFFFFF
    },
    {
        NDIS_STRING_CONST("*PacketCoalescing"),
        NdisParameterHexInteger,
        WfcPropertyTypeBoolean,
        WfcAdapterPacketCoalescing,
        0,
        0,
        (UINT)TRUE,
    },
    {
        NDIS_STRING_CONST("SoftwareRadioOff"), 
        NdisParameterHexInteger,
        WfcPropertyTypeBoolean,
        WfcAdapterDesiredSoftwareRadioOff,
        0,
        0,
        (UINT)TRUE,
    },
    {
        NDIS_STRING_CONST( "ResetRecoveryOff" ),
        NdisParameterHexInteger,
        WfcPropertyTypeBoolean,
        WfcAdapterResetRecoveryOff,
        0,
        0,
        (UINT)TRUE,
    },
    {
        NDIS_STRING_CONST( "TaskTimeout" ),
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterTaskTimeout,
        DEFAULT_TASK_TIMEOUT,
        MIN_TASK_TIMEOUT,
        MAX_TASK_TIMEOUT
    },
    {
        NDIS_STRING_CONST("CommandTimeout"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterCommandTimeout,
        DEFAULT_COMMAND_TIMEOUT,
        MIN_COMMAND_TIMEOUT,
        MAX_COMMAND_TIMEOUT
    },
    {
        NDIS_STRING_CONST("UnreachableThreshold"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegUnreachableThreshold,
        3000,
        500,
        10000
    },
    {
        NDIS_STRING_CONST("Test_BSSChannel"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegTest_BSSChannel,
        0,
        0,
        200
    },
    {
        NDIS_STRING_CONST("RoamControl"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegRoamControlParametersIndex,
        ROAM_CONTROL_LEVEL::WfcNormalRoam,
        ROAM_CONTROL_LEVEL::WfcNeverRoam,
        ROAM_CONTROL_LEVEL::WfcMaxRoamControlLevel    
    },
    {
        NDIS_STRING_CONST("Roam_LQThreshold"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegRoamLQThreshold,
        50,
        0,
        100   
    },
    {
        NDIS_STRING_CONST("Roam_LQChangeBar"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegRoamLQChangeBar,
        10,
        0,
        100 
    },
    {
        NDIS_STRING_CONST("Roam_TimeGap"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegRoamTimeGap,
        10,
        1,
        10000
    },
    {
        NDIS_STRING_CONST("Roam_AllowScanCancel"),
        NdisParameterHexInteger,
        WfcPropertyTypeBoolean,
        WfcAdapterPropertyRegRoamAllowScanCancel,
        0,
        0,
        (UINT)TRUE
    },
    {
        NDIS_STRING_CONST("Roam_BoostParams"),
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegRoamFiveGhzParams,
        0x00503205, //high_lq = 80, low_lq = 50, boost_low = 5
        0,
        0x00FFFFFF // max is 
    },
    {
        NDIS_STRING_CONST("Roam_FullPassiveScan"),
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegRoamFullPassiveScan,
        0,
        0,
        1
    },    
    {
        NDIS_STRING_CONST("ThrottleRoamingNeeded"),
        NdisParameterHexInteger,
        WfcPropertyTypeBoolean,
        WfcAdapterPropertyRegRoamThrottleRoamingNeeded,
        0,
        0,
        (UINT)FALSE
    },    
    {
        NDIS_STRING_CONST("MaxBSSEntryCount"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegMaxBssEntryCount,
        512,
        8,
        4096
    },
    {
        NDIS_STRING_CONST("MaxBackgroundScanSkipCount"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegMaxBackgroundScanSkipCount,
        1,
        0,
        32767
    },
    //
    // The follwoing two values control ResetRecovery injections.
    //
    {
        NDIS_STRING_CONST("HungAtWdiCommandPasses"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegHungAtWdiCommandPasses,
        32,
        0,
        32767
    },
    {
        NDIS_STRING_CONST("HungResetRecoveryIterations"),
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegHungResetRecoveryIterations,
        0,
        0,
        32767
    },
    {   // this key overwrite PLDR in OEM systemCapabilities
        NDIS_STRING_CONST( "TestEnableResetRecovery" ),
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyTestEnableResetRecovery,
        (ULONG)-1,
        0,
        1
    },
    {
        NDIS_STRING_CONST("StaLastConnectedChannel"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyStaLastConnectedChannel,
        6,
        0,
        165
    },
    {
        NDIS_STRING_CONST("StaLastConnectedBandId"),
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyStaLastConnectedBandId,
        0,
        0,
        3
    },
    {
        NDIS_STRING_CONST("RRDiagnoseLevel"),
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRRDiagnoseLevel,
        (UINT)DiagnoseLevelHardwareRegisters,
        (UINT)DiagnoseLevelNone,
        (UINT)DiagnoseLevelDriverStateDump
    },
    {
        NDIS_STRING_CONST("ScanOptimizationChannelCount"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegScanOptChannelCount,
        3,
        0,
        WFC_CONSTANT::MaxNetworkChannelHistorySize
    },
    {
        NDIS_STRING_CONST("AdapterCapabilityOverride"), 
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegAdapterCapabilityOverride,
        0,
        0,
        0xFFFFFFFF      // This is filtered further later
    },
    {
        NDIS_STRING_CONST("DisableOpportunisticRoam"),
        NdisParameterHexInteger,
        WfcPropertyTypeBoolean,
        WfcAdapterPropertyRegDisableOpportunisticRoam,
        0,
        0,
        (UINT)TRUE,
    },
    {
        NDIS_STRING_CONST("OpportunisticRoamDelay"),
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegOpportunisticRoamDelay,
        WFC_CONSTANT::OpportunisticRoamDelayMs,
        WFC_CONSTANT::OpportunisticRoamDelayMinMs,
        WFC_CONSTANT::OpportunisticRoamDelayMaxMs,
    },
    {
        NDIS_STRING_CONST("MaxRoamDeferCount"),
        NdisParameterHexInteger,
        WfcPropertyTypeULong,
        WfcAdapterPropertyRegMaxRoamDeferCount,
        WFC_CONSTANT::MaxRoamDeferCountDefault,
        WFC_CONSTANT::MaxRoamDeferCountMin,
        WFC_CONSTANT::MaxRoamDeferCountMax,
    }
};


OID_HANDLER_ENTRY g_UnrecognizedOidForwardHandlerInfo = 
    {
        0,                                                 // OID
        TRUE,                                              // Queryable
        TRUE,                                              // Settable
        TRUE,                                              // Method
        FALSE,                                             // QuerySerialization
        FALSE,                                             // Set/MethodSerialization
        FALSE,                                             // IsPreemptable
        FALSE,                                             // CanPreempt
        FALSE,                                             // IsDirectOID
        WiFiPriorityScopeAdapter,                          // PriorityScope
        WiFiSerializedJobPriority_Lowest,                  // Priority
        WiFiJobUnrecognizedOidHandler,                     // QueryHandlerJobType
        WiFiJobUnrecognizedOidHandler,                     // Set/MethodHandlerJobType
        0,                                                 // MinBufferSize
        WfcPortTypeAll,                                    // AllowedPortType
    };

ULONG g_Test_FeatureMask = 0;

#define REG_VALUE_RRHISTORY L"ResetRecoveryHistory"
#define FIVE_MINUTES        (3000000000L)
#define RESET_RECOVERY_MAX 5
#define RESET_RECOVERY_PERIOD FIVE_MINUTES


//==============================================================================================

CAdapter::CAdapter(
    _In_ CMiniportDriver* MiniportDriver
    ) :
    m_EventQueue(),
    m_CommandScheduler(),
    m_NotificationManager(),
    m_ActiveJobsList(),
    m_SerializedJobList(),
    m_OidJobNotificationHandler(),
    m_DebugLastOidRequest(NULL),
    m_DebugLastOidSubmitTime(0),
    m_DebugLastOidValue(0),
    m_DebugLastOidStatus(0),
    m_MiniportDataHandlers(),
    m_MiniportDataApi(),
    m_TxMgr(),
    m_FrameIdTable(),
    m_PeerTable(),
#if WDI_DISABLED    
    m_DataPathFunctions(),
    m_DataPathCallbacks(),
#endif // WDI_DISABLED    
    m_ExtStaBSSList(),
    m_DevicePortBSSList(),
    m_ServicesManager(),
    m_AdapterPowerState(NdisDeviceStateD0),
    m_MiniportPauseJob(NULL),
    m_MiniportRestartJob(NULL),
    m_IsRecoverable(FALSE),
    m_MiniportDriver(MiniportDriver),
    m_MiniportAdapterContext(NULL),
    m_MiniportAdapterHandle(NULL),
    m_MiniportTalTxRxContext(NULL),
    m_InitializationState(WfcAdapterInitializeNotStarted),
    m_lResetRecovery(0),
    m_lResetTriggerCount(0),
    m_pWorkItemDeactivatePort(NULL),
    m_pWorkItemActivatePort(NULL),
    m_DebugDiagnosticsTriggerCommand(0),
    m_DebugDiagnosticsTriggerTime(0),
    m_WfcFrameLookasideInitialized(false)
{
    for (UINT8 i = 0; i < c_MaxPorts; ++i)
    {
        m_pPortList[i] = NULL;
    }
    for (UINT8 i = 0; i < MAX_DEFAULT_KEYS; i++)
    {
        m_DefaultKeyValues[i] = NULL;
    }

    InitializeCppListEntry(this, &m_Link);
    
    m_TlvContext.PeerVersion = MiniportDriver->GetWdiCharacteristics()->WdiVersion;
    m_TlvContext.AllocationContext = 0;

    RtlZeroMemory(&m_WfcFrameLookaside, sizeof(NPAGED_LOOKASIDE_LIST));
}

CAdapter::~CAdapter()
{
    TraceEntry();

    //++
    // free cached default key values
    //
    for ( ULONG i=0;  i < MAX_DEFAULT_KEYS; i++ )
    {
        if ( m_DefaultKeyValues[i] )
        {
            ASSERT( m_DefaultKeyLengths[i] );
            RtlSecureZeroMemory( m_DefaultKeyValues[i],
                                m_DefaultKeyLengths[i]);
            ExFreePoolWithTag( (PVOID)m_DefaultKeyValues[i], WDI_POOL_TAG_KEY );
            m_DefaultKeyValues[i] = NULL;
        }
    }
    //--

    if (m_MiniportPauseJob)
    {
        delete m_MiniportPauseJob;
    }

    if (m_MiniportRestartJob)
    {
        delete m_MiniportRestartJob;
    }

    if (m_MiniportSurpriseRemovalJob)
    {
        delete m_MiniportSurpriseRemovalJob;
    }

    //
    // before adapter is gone, de-register adapter event callback
    //
    if ( m_EventRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID )
    {
        m_NotificationManager.DeregisterNotificationHandler( m_EventRegistrationToken );
        m_EventRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
    }

    Halt(NdisHaltDeviceInitializationFailed);

    BOOLEAN isClosed;
    BOOLEAN status;
#pragma warning(suppress: 4189)
    for (UINT8 i = 0; i < c_MaxPorts; ++i)
    {
        //
        //  the port gate must be open
        //
        isClosed = m_PortLockList[i].IsClosed();
        ASSERT (!isClosed);

        //
        // Must be able to close synchronously because there should not be an pending data path activity
        //
        status = m_PortLockList[i].CloseAsync(NULL);
        ASSERT (status);
    }

    if ( m_pWorkItemDeactivatePort )
    {
        m_pWorkItemDeactivatePort->WaitForCompletion();
        delete m_pWorkItemDeactivatePort;
    }

    if ( m_pWorkItemActivatePort )
    {
        m_pWorkItemActivatePort->WaitForCompletion();
        delete m_pWorkItemActivatePort;
    }

    if ( m_ISpinLockDelayDeactivation )
    {
        delete m_ISpinLockDelayDeactivation;
    }

    if ( m_ISpinLockDelayActivation )
    {
        delete m_ISpinLockDelayActivation;
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}


NDIS_STATUS CAdapter::ControlResetRecovery()
/*++

    Reset Recovery will be contraint at 5 times in 5 minutes at most.
    We persist the history of RR. If there are 5 of them (within 5 minues span)
    We will fail Init call, so RR won't repeat too many times.

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CRegistryHelper regHelper(GetMiniportAdapterHandle());

    ULONGLONG sysTimes[RESET_RECOVERY_MAX] = {0};
    ULONG cb = sizeof(sysTimes);
    ULONG entries;

    TraceEntry();
    ndisStatus = regHelper.ReadAdapterRegistryBinaries( 
                    REG_VALUE_RRHISTORY,
                    &cb,
                    (BYTE*)&sysTimes[0]);

    if ( ndisStatus != NDIS_STATUS_SUCCESS || cb == 0 || (cb % sizeof(LARGE_INTEGER)))
    {
        //
        // not-present, 0 length or not multiples of sysTime. Thorw it away.
        //
        ndisStatus = NDIS_STATUS_SUCCESS; // return success so the code continues mormally
        goto exit;
    }
    else
    {
        entries = cb / sizeof(LARGE_INTEGER );
    }

    if ( entries >= RESET_RECOVERY_MAX)
    {
        WFCError( "Too many Reset Recovery in 5 minutes. Will fail MPInit" );

        //
        // wipe the reg record, wrie 1 bye which is invalid system time.
        //
        ndisStatus = regHelper.WriteAdapterRegistryBinaries( 
                        REG_VALUE_RRHISTORY,
                        1,
                        (BYTE*)&sysTimes[0]);
        ndisStatus = NDIS_STATUS_REQUEST_ABORTED; // return error to fail MPInit
        goto exit;
    }

exit:
    TraceExit( ndisStatus );
    return ndisStatus;
}


NDIS_STATUS CAdapter::SimulateFirmwareHang(
    _In_ ULONG HungResetRecoveryIterations )
/*++

    Setup firmware hang simulation. The main logics happen in EvenQueue.

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CRegistryHelper regHelper(GetMiniportAdapterHandle());

    HungResetRecoveryIterations; // ref for free build

    TraceEntry();
    
    NT_ASSERT( HungResetRecoveryIterations );
    NT_ASSERT( HungResetRecoveryIterations == m_EventQueue.m_HungResetRecoveryIterations);

    m_AdapterPropertyCache.GetPropertyULong(
        (WFC_PROPERTY_NAME)WfcAdapterPropertyRegHungAtWdiCommandPasses,
        &m_EventQueue.m_HungAtWdiCommandPasses );
    
    //
    // sum it to avoid repeated adding
    //
    m_EventQueue.m_HungAtWdiCommandPasses += m_EventQueue.m_HungResetRecoveryIterations;
    m_EventQueue.m_CountCommandsToLe = 0;
    
    //
    // persist the iteration count down
    //
    WFC_REG_ENTRY regEntry;
    int i;
    
    for ( i=0; i < SIZEOF_ARRAY(g_AdapterRegTable); i++ ) 
    {
        if ( g_AdapterRegTable[i].PropertyName == 
            WfcAdapterPropertyRegHungResetRecoveryIterations )
        {
            break;
        }
    }
    
    if ( i < SIZEOF_ARRAY(g_AdapterRegTable) )
    {
        //
        // Found our entry
        //
        regEntry = g_AdapterRegTable[i];    // make a copy so we can write
        //
        // deafult field is overloaded for write to reg
        //
        regEntry.Default = m_EventQueue.m_HungResetRecoveryIterations-1;
        ndisStatus = regHelper.WriteAdapterRegistryEntry( &regEntry );
        if ( !NT_SUCCESS(ndisStatus) )
        {
            WFCError( "Failed to persist HungResetRecoveryIterations 0x%x", ndisStatus);
            ndisStatus = NTSTATUS2NDISSTATUS( ndisStatus );
            goto exit;
        }

        WFCInfo( "Simulate FW hang iterations=%d Effective WDIPasses=%d",
            m_EventQueue.m_HungResetRecoveryIterations,
            m_EventQueue.m_HungAtWdiCommandPasses);
    }

exit:
    TraceExit( ndisStatus );
    return ndisStatus;
}


NDIS_STATUS CAdapter::Initialize(
    _In_ NDIS_HANDLE MiniportAdapterHandle,
    _In_ PNDIS_MINIPORT_INIT_PARAMETERS     MiniportInitParameters,
    _In_ PNDIS_WDI_INIT_PARAMETERS          NdisWdiInitParameters
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BOOLEAN controlPathInitialized = FALSE;

    CMiniportInitializeJob *pInitializeJob = NULL;
    CJobCompleteNotifier *pInitializeCompleteHandler = NULL;
    IEventContext *pCompleteEvent = NULL;
    CMiniportPauseJob *pMiniportPauseJob = NULL;
    CMiniportRestartJob *pMiniportRestartJob = NULL;
    CMiniportSurpriseRemovalJob *pMiniportSurpriseRemovalJob = NULL;
    NDIS_STATUS completeStatus = NDIS_STATUS_SUCCESS;
    CRegistryHelper* pRegistryHelper = NULL;

    TraceEntry();

    m_MiniportAdapterHandle = MiniportAdapterHandle;
    m_pCtlDevice = g_pWdiDriver->GetControlDevice();

    ndisStatus = ConvertInterfaceLuidToGuid( &MiniportInitParameters->NetLuid, &m_NetGuid );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError( "Failed to convert Interface Luid (0x%llx) to Guid status = 0x%08X", MiniportInitParameters->NetLuid.Value, ndisStatus );
        goto Exit;
    }

    ndisStatus = m_AdapterPropertyCache.Initialize();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize m_AdapterPropertyCache");
        goto Exit;
    }

    pRegistryHelper = new CRegistryHelper(MiniportAdapterHandle);
    if (pRegistryHelper == NULL)
    {
        WFCError("Failed to create registry helper");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    // Load values from the registry
    ndisStatus = pRegistryHelper->LoadPropertiesFromRegistry(
        &m_AdapterPropertyCache,
        g_AdapterRegTable,
        (((ULONG)sizeof (g_AdapterRegTable)) / ((ULONG)sizeof(WFC_REG_ENTRY))));
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to read registry/setup defaults for properties. Error = 0x%08x", ndisStatus);
        goto Exit;
    }

    //
    // read oem PLDR and FLDR settings
    //
    {
        // Changed to get info from NDIS config which is obtained from ACPI Reset Interface; reuse m_OemPLDRSupported
        //(void) ReadRegDword( REG_KEY_WLANSVC_OEM, REG_VALUE_OEM_FLDR, &m_OemFLDRSupported );
        NDIS_STRING PldrCapabilityStr = NDIS_STRING_CONST( "PldrCapability" );
        ndisStatus = pRegistryHelper->ReadAdapterRegistryDword( &PldrCapabilityStr, &m_OemPLDRSupported );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            m_OemPLDRSupported = 0;
        }
    }

    m_OemFLDRSupported = 0; // this is not supported by NDIS

    WFCInfo( "OEM PLDRSupported=%u FLDRSupported=%u", m_OemPLDRSupported, m_OemFLDRSupported );

    //
    // Check the old key to see if the OEM wants to override PLDR.
    // Note that we only support disabling PLDR if the bus supports it.  There is no
    // point in enabling it if the bus/system doesn't support it.
    //
    if (m_OemPLDRSupported)
    {
        ULONG data = 0;
        ndisStatus = ReadRegDword( REG_KEY_WLANSVC_OEM, REG_VALUE_OEM_PLDR, &data );
        if ((ndisStatus == NDIS_STATUS_SUCCESS) && (data == 0))
        {
            WFCError( "PLDRSupported disabled by Old OEM regkey" );
            m_OemPLDRSupported = 0;
        }
    }

    //
    // Now check for a testing override
    //
    {
        ULONG data = 0;
        ndisStatus = m_AdapterPropertyCache.GetPropertyULong(
            (WFC_PROPERTY_NAME)WfcAdapterPropertyTestEnableResetRecovery,
            &data );
        // If the override exists, is valid (i.e. not -1), and is different from the OEM/bus setting
        // then override!
        if ((ndisStatus == NDIS_STATUS_SUCCESS) && (data != (ULONG)-1) && (!!data != !!m_OemPLDRSupported))
        {
            WFCError( "PLDRSupported testing override! New PLDRSupported=%u", data );
            m_OemPLDRSupported = data;  // use it regardless of oem settings
        }
    }

    // TODO: Remove this property and the data from it. 
    // This is test code. Dont define new globals for stuff from registry. Use property cache
    m_AdapterPropertyCache.GetPropertyULong(WfcAdapterPropertyRegTest_FeatureMask,
        &g_Test_FeatureMask);
        
    // create locks for CPort objects
    for (UINT8 i = 0; i < c_MaxPorts; ++i)
    {
        m_PortLockList[i].Initialize(this);

        m_PortLockList[i].Open();
    }
    
    m_AdapterPropertyCache.SetPropertyULong((WFC_PROPERTY_NAME)WfcAdapterCurrentPhyID,
                                            0xFFFFFFFFul);

    ndisStatus = InitializeControlPath();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize control path (0x%x)", ndisStatus);
        goto Exit;
    }
    controlPathInitialized = TRUE;

    //
    //if configured, setup to simulate firmware hang; or control the RR frequency
    //
    m_AdapterPropertyCache.GetPropertyULong(
            (WFC_PROPERTY_NAME)WfcAdapterPropertyRegHungResetRecoveryIterations,
            &m_EventQueue.m_HungResetRecoveryIterations );

    if ( m_EventQueue.m_HungResetRecoveryIterations )
    {
        SimulateFirmwareHang( m_EventQueue.m_HungResetRecoveryIterations );
    }
    else
    {
        ndisStatus = ControlResetRecovery();
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError( "Failed Adapter Init due to control of RR" );
            goto Exit;
        }
    }

    (VOID)m_ExtStaBSSList.Initialize(this, &m_AdapterPropertyCache, WfcPortTypeExtSTA);
    (VOID)m_DevicePortBSSList.Initialize(this, &m_AdapterPropertyCache, WfcPortTypeWFDDevice);
    (VOID)m_ServicesManager.Initialize(this, &m_AdapterPropertyCache);

    SetDataPathApis();

    //
    // inint pause job
    //
    pMiniportPauseJob = new CMiniportPauseJob(
        IActivityId::get_NextActivityId() );
    if (pMiniportPauseJob == NULL)
    {
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCError("Failed to allocate CMiniportPauseJob object");
        goto Exit;
    }
    ndisStatus = pMiniportPauseJob->Initialize(this, WiFiJobMiniportPause);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize MiniportPauseJob job");
        goto Exit;
    }

    //
    // inint restart job
    //
    pMiniportRestartJob = new CMiniportRestartJob(
        IActivityId::get_NextActivityId() );
    if (pMiniportRestartJob == NULL)
    {
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCError("Failed to allocate CMiniportRestartJob object");
        goto Exit;
    }
    ndisStatus = pMiniportRestartJob->Initialize(this, WiFiJobMiniportRestart);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize MiniportPauseJob job");
        goto Exit;
    }
    
    //
    // init pause job
    //
    pMiniportSurpriseRemovalJob = new CMiniportSurpriseRemovalJob(
        IActivityId::get_NextActivityId() );
    if (pMiniportSurpriseRemovalJob == NULL)
    {
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCError("Failed to allocate CMiniportSurpriseRemovalJob object");
        goto Exit;
    }
    ndisStatus = pMiniportSurpriseRemovalJob->Initialize(this, WiFiJobMiniportSurpriseRemoval);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize MiniportSurpriseRemovalJob");
        goto Exit;
    }

    // Create the initialization job
    pInitializeJob = new CMiniportInitializeJob(
        IActivityId::get_NextActivityId() );
    if (pInitializeJob == NULL)
    {
        WFCError("Failed to allocate MiniportInitialize job");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    ndisStatus = pInitializeJob->Initialize(
        this, 
        MiniportInitParameters,
        NdisWdiInitParameters);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize MiniportInitialize job");
            goto Exit;
    }

    // Create the completion handler. We will wait on it
    pInitializeCompleteHandler = new CJobCompleteNotifier();
    if (pInitializeCompleteHandler == NULL)
    {
        WFCError("Failed to allocate MiniportInitialize complete notifier");
        ndisStatus = NDIS_STATUS_RESOURCES;
            goto Exit;
    }

    ndisStatus = IEventContext::CreateInstance(&pCompleteEvent);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize complete event (0x%x)", ndisStatus);
        goto Exit;
    }
    
    ndisStatus = pInitializeCompleteHandler->Initialize(
        &completeStatus,
        pCompleteEvent);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize MiniportInitialize complete notifier");
            goto Exit;
    }

    m_PendingInitHaltJob = pInitializeJob;
    
    // Queue the MiniportInitializeJob to do all the real work. Even if this fails
    // we will call cleanup
    ndisStatus = m_SerializedJobList.QueueSerializedJob(
        pInitializeJob,
        pInitializeCompleteHandler);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to queue MiniportInitialize job");
            goto Exit;
    }

    // if we succeeded in queuing the job, the complete would clean itself up
    pInitializeCompleteHandler = NULL;
    
    // Wait for the job to complete. 
    pCompleteEvent->Wait( 0 );

    // Copy the status
    if ( m_lResetRecovery ) 
    {
        //
        // Firmware stalled detected; return success so we can do RnR
        //
        ASSERT( completeStatus != NDIS_STATUS_SUCCESS );
        completeStatus = NDIS_STATUS_SUCCESS;
    }

    ndisStatus = completeStatus;
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError(" MiniportInitialize job completed with failure (0x%x)", ndisStatus);
        goto Exit;
    }
    
    m_MiniportPauseJob = pMiniportPauseJob;
    pMiniportPauseJob = NULL;

    m_MiniportRestartJob = pMiniportRestartJob;
    pMiniportRestartJob = NULL;

    m_MiniportSurpriseRemovalJob = pMiniportSurpriseRemovalJob;
    pMiniportSurpriseRemovalJob = NULL;

    //
    // delay workitems for NdisPort deactivation and Activation
    //
    for (UINT8 i=0; i < c_MaxPorts; i++ )
    {
        m_NdisPortNumToActivate[i] = 0;
        m_NdisPortNumToDeactivate[i] = 0;
    }
    m_bDeactivateWorkerActive = FALSE;
    m_bActivateWorkerActive = FALSE;

    ndisStatus = CNdisSpinLock::CreateInstance( & m_ISpinLockDelayDeactivation );
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't create Deactivation Spinlock");
        goto Exit;
    }

    ndisStatus = CNdisSpinLock::CreateInstance( & m_ISpinLockDelayActivation );
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't create Activation Spinlock");
        goto Exit;
    }

    ndisStatus = IWorkItemContext::CreateInstance( &m_pWorkItemDeactivatePort );
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't create Deactivation work item.");
        goto Exit;
    }

    ndisStatus = m_pWorkItemDeactivatePort->Initialize( m_pCtlDevice, this, m_pWorkItemDeactivatePort );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't initialize preallocated deactivation workitem.");
        goto Exit;
    }

    ndisStatus = IWorkItemContext::CreateInstance( &m_pWorkItemActivatePort );
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't create Activation work item.");
        goto Exit;
    }

    ndisStatus = m_pWorkItemActivatePort->Initialize( m_pCtlDevice, this, m_pWorkItemActivatePort );
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't initialize preallocated ctivation workitem.");
        goto Exit;
    }

    WFCInfo("Adapter Initialize complete (0x%x)", ndisStatus);
    
Exit:
    if (pInitializeJob)
    {
        delete pInitializeJob;
    }

    if (pInitializeCompleteHandler)
    {
        delete pInitializeCompleteHandler;
    }

    if (pCompleteEvent)
    {
        delete pCompleteEvent;
    }
    
    if (pMiniportPauseJob)
    {
        delete pMiniportPauseJob;
    }

    if (pMiniportSurpriseRemovalJob)
    {
        delete pMiniportSurpriseRemovalJob;
    }
        
    if (pMiniportRestartJob)
    {
        delete pMiniportRestartJob;
    }

    if (pRegistryHelper)
    {
        delete pRegistryHelper;
    }

    m_PendingInitHaltJob = NULL;
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        DtErrMsg( "MPInit Failed status=0x%x\n", ndisStatus );
        // On failure, we will call halt if we got past the control
        // path initialization
        if (controlPathInitialized)
        {
            Halt(NdisHaltDeviceInitializationFailed);
        }
    }
    
    m_IsRecoverable = TRUE; // Mark end of initialization

    if ( m_lResetRecovery )
    {
        // Do Diagnose too <ToDo> if we can
        ReenumerateFailedAdapter();
    }
    TraceExit(ndisStatus);

    return ndisStatus;
}

VOID CAdapter::Halt(
    _In_  NDIS_HALT_ACTION        HaltAction        
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    
    CMiniportHaltJob *pHaltJob = NULL;
    CJobCompleteNotifier *pHaltCompleteHandler = NULL;
    IEventContext *pCompleteEvent = NULL;
    NDIS_STATUS completeStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    UNREFERENCED_PARAMETER(HaltAction);
    
    if ( !IsInitComplete(WfcAdapterInitializeControlPathInitialized) )
    {
        WFCInfo("Note: Control path was never fully initialized, so we are going to skip work in MiniportHalt");
        goto Exit;
    }

    m_IsRecoverable = FALSE; // Mark that device is not recoverable from this point
    // Create the halt job
    pHaltJob = new CMiniportHaltJob(
        IActivityId::get_NextActivityId() );
    if (pHaltJob == NULL)
    {
        WFCError("Failed to allocate MiniportHalt job");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    ndisStatus = pHaltJob->Initialize(this);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize MiniportHalt job");
        goto Exit;
    }

    // Create the completion handler. We will wait on it
    pHaltCompleteHandler = new CJobCompleteNotifier();
    if (pHaltCompleteHandler == NULL)
    {
        WFCError("Failed to allocate MiniportHalt complete notifier");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    ndisStatus = IEventContext::CreateInstance(&pCompleteEvent);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize complete event (0x%x)", ndisStatus);
        goto Exit;
    }


    ndisStatus = pHaltCompleteHandler->Initialize(
        &completeStatus,
        pCompleteEvent);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize MiniportHalt complete notifier");
        goto Exit;
    }

    m_PendingInitHaltJob = pHaltJob;

    // Queue the MiniportHaltJob to do all the real work
    ndisStatus = m_SerializedJobList.QueueSerializedJob(
        pHaltJob,
        pHaltCompleteHandler);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to queue MiniportHalt job");
        goto Exit;
    }

    // if we succeeded in queuing the job, the complete would clean itself up
    pHaltCompleteHandler = NULL;
    
    // Wait for the job to complete. 
    pCompleteEvent->Wait(0);

    // Copy the status
    ndisStatus = completeStatus;
    WFCInfo("MiniportHalt job complete (0x%x)", ndisStatus);
    
Exit:
    if (pHaltJob)
    {
        delete pHaltJob;
    }

    if (pHaltCompleteHandler)
    {
        delete pHaltCompleteHandler;
    }

    if (pCompleteEvent)
    {
        delete pCompleteEvent;
    }

    m_PendingInitHaltJob = NULL;
    
    // Do control path cleanup. Note that we are never
    // called from the event queue (else we would have
    // hung on the Wait(0) above)
    CleanupControlPath(true);
        
    TraceExit(ndisStatus);
}

// Helpers to help cleanup steps
VOID CAdapter::MarkInitComplete(
    _In_ WFC_ADAPTER_INITIALIZE_STATE CompletedState
    )
{
    m_InitializationState |= CompletedState;
}

VOID CAdapter::UnmarkInitComplete(
    _In_ WFC_ADAPTER_INITIALIZE_STATE RollbackState
    )
{
    m_InitializationState &= (~RollbackState);
}

BOOLEAN CAdapter::IsInitComplete(
    _In_ WFC_ADAPTER_INITIALIZE_STATE StateToCheck
    )
{
    if (m_InitializationState & StateToCheck)
        return TRUE;
    else 
        return FALSE;
}

NDIS_STATUS 
CAdapter::ReadRegDword( 
    _In_ PWSTR Path,
    _In_ PWSTR Value,
    _Inout_ DWORD * pData )  // input default return default if unsuccessful
/*++

    Read dword value in Registry from arbitray path and Value name.
    Data type must be REG_DWORD - caller's caveat

--*/
{
    NDIS_STATUS ndisStatus;
    HANDLE hKey = NULL;
    OBJECT_ATTRIBUTES nameAttributes;
    UNICODE_STRING uniPath, uniValue;
    DWORD data = *pData;
    ULONG needLength;
    struct {
        KEY_VALUE_PARTIAL_INFORMATION PartialInfo;
        DWORD dwValue;
    } valueInfo;

    TraceEntry();

    RtlInitUnicodeString(&uniPath, Path); 

    InitializeObjectAttributes(&nameAttributes,
                               &uniPath,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               NULL, //  ParentHandle
                               (PSECURITY_DESCRIPTOR)NULL);

    ndisStatus = ZwOpenKey( &hKey, KEY_READ, &nameAttributes );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError( "Failed to open key %S status=0x%x\n", Path, ndisStatus );
        goto exit;
    }

    RtlInitUnicodeString(&uniValue, Value);
    ndisStatus = ZwQueryValueKey( hKey,
                                &uniValue,
                                KeyValuePartialInformation,
                                &valueInfo,
                                sizeof(valueInfo),
                                &needLength );

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError( "Failed to query value %S status=0x%x\n", Value, ndisStatus );
        goto exit;
    }

    data = * (DWORD*) valueInfo.PartialInfo.Data;

exit:
    if ( hKey )
    {
        ZwClose( hKey );
    }

    *pData = data;

    TraceExit( ndisStatus );
    return ndisStatus;
}


void CAdapter::OnOpenAdapterCompleted(
    _In_ NDIS_STATUS              CompletionStatus
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    TraceEntry();
    
    NT_ASSERTMSG("Open adapter completed when it was not initiated", m_PendingInitHaltJob != NULL);

    WFCTrace("OpenAdapter completed with status 0x%08x", ndisStatus);
    
    // Forward this to the requesting job
    ndisStatus = m_EventQueue.DeferOperationCompletion(m_PendingInitHaltJob,
                        CompletionStatus,
                        this,
                        NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)    
    {
        // Fatal error
        NT_ASSERT(FALSE);
        WFCError("EventQueue->DeferOperationCompletion(PendingOpenCompleteCallback) failed %u", ndisStatus);
    }           

    TraceExit(ndisStatus);    
}

void CAdapter::OnCloseAdapterCompleted(
    _In_ NDIS_STATUS              CompletionStatus
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    TraceEntry();
    
    NT_ASSERTMSG("Close adapter completed when it was not initiated", m_PendingInitHaltJob != NULL);

    WFCTrace("CloseAdapter completed with status 0x%08x", ndisStatus);
    
    // Forward this to the requesting job
    ndisStatus = m_EventQueue.DeferOperationCompletion(m_PendingInitHaltJob,
                        CompletionStatus,
                        this,
                        NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)    
    {
        // Fatal error
        NT_ASSERT(FALSE);
        WFCError("EventQueue->DeferOperationCompletion(PendingCloseCompleteCallback) failed %u", ndisStatus);
    }           

    TraceExit(ndisStatus);    
}

NDIS_STATUS CAdapter::ConvertNdisPortNumberToPortId(
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _Out_ WFC_PORT_ID *pPortId)
{
    NT_ASSERT(
        m_EventQueue.get_ActiveThreadHandle() ==
        CDebugMetadata::get_CurrentThreadHandle() );

    *pPortId = WDI_PORT_ID_ADAPTER;
    
    for (UINT8 i =0; i < c_MaxPorts; i++)
    {
        if ((m_pPortList[i] != NULL) && 
            (m_pPortList[i]->GetNdisPortNumber() == NdisPortNumber))
        {
            *pPortId = m_pPortList[i]->GetPortId();
            return NDIS_STATUS_SUCCESS;
        }
    }

    return NDIS_STATUS_FILE_NOT_FOUND;
}

NDIS_STATUS CAdapter::ConvertPortIdToNdisPortNumber(
    _In_ WFC_PORT_ID PortId,
    _Out_ NDIS_PORT_NUMBER *pNdisPortNumber)
{
    NT_ASSERT(
        m_EventQueue.get_ActiveThreadHandle() ==
        CDebugMetadata::get_CurrentThreadHandle() );

    *pNdisPortNumber = INVALID_NDIS_PORT_NUMBER;
    
    for (UINT8 i =0; i < c_MaxPorts; i++)
    {
        if ((m_pPortList[i] != NULL) && 
            (m_pPortList[i]->GetPortId() == PortId))
        {
            *pNdisPortNumber = m_pPortList[i]->GetNdisPortNumber();
            return NDIS_STATUS_SUCCESS;
        }
    }

    return NDIS_STATUS_FILE_NOT_FOUND;
}

NDIS_STATUS CAdapter::GetNdisPortNumberForPortType(
    _In_ WFC_PORT_TYPE PortType,
    _Out_ NDIS_PORT_NUMBER *pNdisPortNumber)
{
    NT_ASSERT(
        m_EventQueue.get_ActiveThreadHandle() ==
        CDebugMetadata::get_CurrentThreadHandle() );

    *pNdisPortNumber = INVALID_NDIS_PORT_NUMBER;
    
    for (UINT8 i =0; i < c_MaxPorts; i++)
    {
        if ((m_pPortList[i] != NULL) && 
            (m_pPortList[i]->GetPortType() == PortType))
        {
            *pNdisPortNumber = m_pPortList[i]->GetNdisPortNumber();
            return NDIS_STATUS_SUCCESS;
        }
    }

    return NDIS_STATUS_FILE_NOT_FOUND;
}

NDIS_STATUS CAdapter::GetPortTypeFromPortId(
    _In_ WFC_PORT_ID PortId,
    _Out_ WFC_PORT_TYPE *pPortType)
{
    NT_ASSERT(
        m_EventQueue.get_ActiveThreadHandle() ==
        CDebugMetadata::get_CurrentThreadHandle() );

    for (UINT8 i =0; i < c_MaxPorts; i++)
    {
        if ((m_pPortList[i] != NULL) && 
            (m_pPortList[i]->GetPortId() == PortId))
        {
            *pPortType = m_pPortList[i]->GetPortType();
            return NDIS_STATUS_SUCCESS;
        }
    }

    return NDIS_STATUS_FILE_NOT_FOUND;
}

NDIS_STATUS CAdapter::GetPortIdForPortType(
    _In_ WFC_PORT_TYPE PortType,
    _Out_ WFC_PORT_ID *pPortId)
{
    NT_ASSERT(
        m_EventQueue.get_ActiveThreadHandle() ==
        CDebugMetadata::get_CurrentThreadHandle() );

    for (UINT8 i =0; i < c_MaxPorts; i++)
    {
        if ((m_pPortList[i] != NULL) && 
            (m_pPortList[i]->GetPortType() == PortType))
        {
            *pPortId = m_pPortList[i]->GetPortId();
            return NDIS_STATUS_SUCCESS;
        }
    }

    return NDIS_STATUS_FILE_NOT_FOUND;
}

BOOLEAN CAdapter::IsOperationalPowerState()
{
    return ( m_AdapterPowerState == NdisDeviceStateD0 );
}

CPort* CAdapter::GetPortFromPortId(
    _In_ WFC_PORT_ID PortId)
{
    for (UINT i =0; i < c_MaxPorts; i++)
    {
        if ((m_pPortList[i] != NULL) && 
            (m_pPortList[i]->GetPortId() == PortId))
        {
            return m_pPortList[i];
        }
    }

    return NULL;
}
CPort* CAdapter::GetPortFromPortIdAndLock(
    _In_ WFC_PORT_ID PortId,
    _Out_ CWFCExclusiveAsyncGate **ppPortLockOut)
/*
If the function succeeded, then the caller should to call CWFCExclusiveAsyncGate::Leave() 
to release the lock on the port
*/
{
    for (UINT i = 0; i < c_MaxPorts; i++)
    {
        if (m_PortLockList[i].Enter())
        {
            if ((m_pPortList[i] != NULL) && 
                (m_pPortList[i]->GetPortId() == PortId))
            {
                *ppPortLockOut = &m_PortLockList[i];
                return m_pPortList[i];
            }
            m_PortLockList[i].Leave();
        }
    }

    return NULL;
}

CPort* CAdapter::GetPortFromNdisPortNumber(
    _In_ NDIS_PORT_NUMBER NdisPortNumber)
{
    for (UINT i = 0; i < c_MaxPorts; i++)
    {
        if ((m_pPortList[i] != NULL) && 
            (m_pPortList[i]->GetNdisPortNumber() == NdisPortNumber))
        {
            return m_pPortList[i];
        }
    }

    return NULL;
}

CPort* CAdapter::GetPortFromNdisPortNumberAndLock(
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _Out_ CWFCExclusiveAsyncGate **ppPortLockOut)
/*
If the function succeeded, then the caller should to call CWFCExclusiveAsyncGate::Leave() 
to release the lock on the port
*/
{
    for (UINT i = 0; i < c_MaxPorts; i++)
    {
        if (m_PortLockList[i].Enter())
        {
            if ((m_pPortList[i] != NULL) && 
                (m_pPortList[i]->GetNdisPortNumber() == NdisPortNumber))
            {
                *ppPortLockOut = &m_PortLockList[i];
                return m_pPortList[i];
            }
            m_PortLockList[i].Leave();
        }
    }

    return NULL;
}


NDIS_STATUS 
CAdapter::PersistStateInRegistry()
/*++

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_FAILURE;
    CRegistryHelper regHelper(GetMiniportAdapterHandle());

    ndisStatus = regHelper.SavePropertyToRegistry(
        WfcAdapterPropertyStaLastConnectedChannel,
        &m_AdapterPropertyCache,
        g_AdapterRegTable,
        SIZEOF_ARRAY(g_AdapterRegTable)
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to save last connected channel to registry. Error = 0x%08x", ndisStatus);    
    }
    
    ndisStatus = regHelper.SavePropertyToRegistry(
        WfcAdapterPropertyStaLastConnectedBandId,
        &m_AdapterPropertyCache,
        g_AdapterRegTable,
        SIZEOF_ARRAY(g_AdapterRegTable)
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to save last connected band id to registry. Error = 0x%08x", ndisStatus);    
    }

    return ndisStatus;
}


IConnectedPeerList* CAdapter::GetConnectedPeerList(_In_ WFC_PORT_ID WfcPortId)
{
    return GetPortFromPortId(WfcPortId)->GetConnectedPeerList();
}


NDIS_STATUS CAdapter::InitializeControlPath()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NTSTATUS ntStatus = STATUS_SUCCESS;

    TraceEntry();

    ntStatus = m_CtlPlane.Initialize(
        this ); // core's adapter struct pointer

    if (!NT_SUCCESS(ntStatus))
    {
        WFCError("CtlPlane Initialization failed (0x%x)", ntStatus);
        ndisStatus = NTSTATUS2NDISSTATUS(ntStatus);
        goto Exit;
    }

    // Global event queue
    ndisStatus = m_EventQueue.Initialize(
        m_pCtlDevice
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("EventQueue Initialize failed (0x%x)", ndisStatus);
        goto Exit;
    }    

    // Command scheduler
    ndisStatus = m_CommandScheduler.Initialize(
        &m_EventQueue,
        &m_CtlPlane,
        this);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError(" DeviceCommandScheduler Initialize failed (0x%x)", ndisStatus);
        goto Exit;
    }    

    // Notification manager
    ndisStatus = m_NotificationManager.Initialize(
        &m_EventQueue,
        &m_CtlPlane );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("NotificationManager Initialize failed (0x%x)", ndisStatus);
        goto Exit;
    }
    

    // Active Jobs list
    ndisStatus = m_ActiveJobsList.Initialize(
                    &m_EventQueue,
                    &m_CommandScheduler );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("ActiveJobsList Initialize failed (0x%x)", ndisStatus);
        goto Exit;
    }

    // Serialized Job list
    ndisStatus = m_SerializedJobList.Initialize(
        &m_EventQueue,
        &m_ActiveJobsList );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("SerializedJobList Initialize failed (0x%x)", ndisStatus);
        goto Exit;
    }

    MarkInitComplete(WfcAdapterInitializeControlPathInitialized);    
Exit:

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        CleanupControlPath(false);
    }
    
    TraceExit(ndisStatus);

    return ndisStatus;
}


VOID CAdapter::CleanupControlPath(
    _In_ bool   WaitForEventQueueToEmpty
    )
{
    TraceEntry();   

    if (IsInitComplete(WfcAdapterInitializeControlPathInitialized))
    {
        // Stop any new Serialized jobs from getting queued.
        // This is just precautionary
        m_SerializedJobList.set_AllowNewJobs(FALSE);    // don't allow
    
        if (WaitForEventQueueToEmpty)
        {
            // Wait for the event queue thread to finish. This 
            // means we must not be called from the event queue thread

            m_EventQueue.WaitForQueueToEmpty();
        }            
    }
    
    UnmarkInitComplete(WfcAdapterInitializeControlPathInitialized);

    TraceExit(NDIS_STATUS_SUCCESS);    
}

VOID CAdapter::CompleteOidRequest(
    _In_  PNDIS_OID_REQUEST pNdisOidRequest,
    _In_  NDIS_STATUS       CompletionStatus
    )
{
    NT_ASSERT(m_DebugLastOidRequest == pNdisOidRequest);
    
    m_DebugLastOidStatus = CompletionStatus;
    m_DebugLastOidRequest = NULL;

    WFCInfo("[OID] Completed OID 0x%08x on NDIS Port %d with status 0x%x", 
        pNdisOidRequest->DATA.SET_INFORMATION.Oid,
        pNdisOidRequest->PortNumber,
        CompletionStatus);

    //
    // Special handling of return code when RR/diagnose has been invoked.
    //
    if ( m_lResetRecovery == 0 )
    {
        goto complete;
    }

    //
    // Only care state changing OIDs.
    //
    if ( pNdisOidRequest->RequestType !=NdisRequestSetInformation &&
        pNdisOidRequest->RequestType != NdisRequestMethod )
    {
        goto complete;
    }

    //
    // some OIDs must be succeeded to release the referernce on the MP
    //
    switch ( pNdisOidRequest->DATA.QUERY_INFORMATION.Oid )
    {
        //
        // Patch some return fields?
        //
        case OID_DOT11_DELETE_MAC:
        case OID_PM_REMOVE_WOL_PATTERN:
        case OID_PM_REMOVE_PROTOCOL_OFFLOAD:
        case OID_RECEIVE_FILTER_CLEAR_FILTER:
            CompletionStatus = NDIS_STATUS_SUCCESS;
            break;
    }

complete:
    g_pWdiDriver->GetNdisHook()->CompleteOidRequestToNdis(
        this->m_MiniportAdapterHandle,
        pNdisOidRequest,
        CompletionStatus
        );

}

VOID
CAdapter::IndicateStatus(
    _In_  NDIS_PORT_NUMBER  NdisPortNumber,
    _In_  NDIS_STATUS       StatusCode,
    _In_opt_  PVOID         RequestID,
    _In_  PVOID             pStatusBuffer,
    _In_  ULONG             StatusBufferSize
    )
{
    NDIS_STATUS_INDICATION  statusIndication;
    
    RtlZeroMemory(&statusIndication, sizeof(NDIS_STATUS_INDICATION));
    
    //
    // Fill in object header
    //
    statusIndication.Header.Type = NDIS_OBJECT_TYPE_STATUS_INDICATION;
    statusIndication.Header.Revision = NDIS_STATUS_INDICATION_REVISION_1;
    statusIndication.Header.Size = sizeof(NDIS_STATUS_INDICATION);
    
    //
    // Fill in the rest of the field
    // 
    statusIndication.StatusCode = StatusCode;
    statusIndication.SourceHandle = m_MiniportAdapterHandle;
    statusIndication.DestinationHandle = NULL;
    statusIndication.RequestId = RequestID;
    statusIndication.PortNumber = NdisPortNumber;
    
    statusIndication.StatusBuffer = pStatusBuffer;
    statusIndication.StatusBufferSize = StatusBufferSize;

    WFCInfo("[Status] Indicating Status 0x%08x on NDIS Port %d, StatusBufferSize %d", 
        StatusCode,
        statusIndication.PortNumber,
        StatusBufferSize);

    //
    // Indicate the status to NDIS
    //
    g_pWdiDriver->GetNdisHook()->IndicateStatusToNdis(
        m_MiniportAdapterHandle,
        &statusIndication
        );
}  

void CAdapter::TriggerControlPathDiagnostics(
    _In_ ULONG TriggeringCommand)
{
    NDIS_STATUS     status = NDIS_STATUS_SUCCESS;

    TraceEntry();

    //
    // For debugging hangs, etc save the time at which we triggered diagnostics
    // and which command triggered it
    //
    m_DebugDiagnosticsTriggerTime = CSystem::get_CurrentTime();
    m_DebugDiagnosticsTriggerCommand = TriggeringCommand;

    MINIPORT_WDI_HANG_DIAGNOSE_HANDLER pDiagnoseHandler = GetMiniportDriver()->
            GetWdiCharacteristics()->HangDiagnoseHandler;

    eDiagnoseLevel diagnoseLevel = DiagnoseLevelHardwareRegisters;
    status = m_AdapterPropertyCache.GetPropertyT(
            WfcAdapterPropertyRRDiagnoseLevel,
            &diagnoseLevel);
    
    if ( status != NDIS_STATUS_SUCCESS )
    {
        diagnoseLevel = DiagnoseLevelHardwareRegisters;
    }

    if ( pDiagnoseHandler )
    {
        UINT32 lOutputSize;
        WFCTrace("Invoking LE Diagnose handler level %d", DiagnoseLevelHardwareRegisters );

        ULONGLONG ullBeginTime = CSystem::get_CurrentTime();
        ULONGLONG ullEndTime;

        pDiagnoseHandler( 
            GetMiniportAdapterContext(),
            diagnoseLevel,
            MAX_DIAGNOSTIC_FIRMWARE_BUFFER_SIZE,
            (UINT8*)m_DebugFirmwareBuffer,
            &lOutputSize );

        ullEndTime = CSystem::get_CurrentTime();

        // take delta and convert it to ms
        TraceLoggingDiagnosisDuration( (ULONG)(ullEndTime - ullBeginTime)/10000 );
    }
    else 
    {
        WFCError("LE does not have Diagnose handler" );
        DbgPrintEx(0,0,"LE does not have Diagnose handler, level=%d\n", diagnoseLevel );
    }

    TraceExit(status);
}


VOID CAdapter::TraceLoggingDiagnosisDuration(
    _In_ ULONG ulMs )
{
#ifndef WDI_WIN81_COMPLIANT
    if ( !g_RegisteredWithTraceLogging )
    {
        //
        // not registered with TraceLogging, include WDI_WIN81)COMPLIANT
        //
        goto exit; // do nothing
    }

    TraceLoggingWrite( g_hProvider,
                "ResetRecovery",
                TraceLoggingGuid(GetNetGuid(), "InterfaceGuid"),
                TraceLoggingUInt32( ulMs, "DiagnosisDurationMs" ),
                TraceLoggingKeyword(MICROSOFT_KEYWORD_TELEMETRY) );

#else
            // reference these to silent OACR
            ulMs;
#endif

exit:
    return;
}

VOID CAdapter::TraceLoggingResetRecovery( 
    _In_ ULONG EventCode,
    _In_ ULONG CommandCode,
    _In_ ULONG rrCount,
    _In_ ULONG OemPldrSupported,
    _In_ BOOLEAN bResetRecoveryOff )
{
#ifndef WDI_WIN81_COMPLIANT
    BOOLEAN bIHVMPResetSupported = FALSE;
    if ( !g_RegisteredWithTraceLogging )
    {
        //
        // not registered with TraceLogging, include WDI_WIN81)COMPLIANT
        //
        goto exit; // do nothing
    }

    if (GetMiniportDriver()->GetNdisCharacteristics()->ResetHandlerEx)    
    {
        bIHVMPResetSupported = TRUE;
    }

    TraceLoggingWrite( g_hProvider,
                    "WdiResetRecovery",
                    TraceLoggingGuid(GetNetGuid(), "InterfaceGuid"),
                    TraceLoggingUInt32( EventCode, "EventCode" ),
                    TraceLoggingUInt32( CommandCode, "WdiCommand" ),
                    TraceLoggingUInt32( rrCount, "ResetRecoveryCount"),
                    TraceLoggingUInt32( OemPldrSupported, "OemPldrSupported"),
                    TraceLoggingBoolean( bResetRecoveryOff, "ResetRecoveryOff" ),
                    TraceLoggingBoolean( bIHVMPResetSupported, "IHVMiniportResetSupported" ),
                    TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES) );

#else
    // reference these to silent OACR
    EventCode, CommandCode, rrCount, OemPldrSupported, bResetRecoveryOff;
#endif
exit:
    return;
}


NDIS_STATUS CAdapter::PreprocessOid(
    _In_  PNDIS_OID_REQUEST     NdisOidRequest,
    _Out_ POID_HANDLER_ENTRY    *ppOidHandler,
    _Out_ WFC_JOB_TYPE         *pHandlerJobType,
    _Out_ BOOLEAN               *pSerializedJob
    )
{
    NDIS_STATUS        ndisStatus    = NDIS_STATUS_SUCCESS;
    PULONG             bytesNeededLocation;
    NDIS_OID           oid;
    ULONG              infoBufferLength;
    POID_HANDLER_ENTRY oidTableEntry = NULL;
    CPort*             pTargetPort   = NULL;

    TraceEntry();

    *ppOidHandler = NULL;
    *pSerializedJob = FALSE;
    *pHandlerJobType = WiFiJobTypeInvalid;
    
    oid = NdisOidRequest->DATA.QUERY_INFORMATION.Oid; // Oid is at same offset for all RequestTypes

    // Find the OID in the oid table
    oidTableEntry = COidJobBase::s_FindOidHandlerEntry(oid);
    if (oidTableEntry == NULL)
    {
        //
        // We do not have data to verify this OID. We let it through
        //
        WFCError("Failed to find handler entry for OID 0x%08x", oid);
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;

        // Workaround OS expectation that we have cleared fields
        switch(NdisOidRequest->RequestType)
        {
            case NdisRequestQueryInformation:
            case NdisRequestQueryStatistics:
                // Initialize this to zero to avoid returning random data
                QUERY_OID_BYTES_WRITTEN(NdisOidRequest) = 0;
                break;

            case NdisRequestSetInformation:
                // Initialize this to zero to avoid returning random data
                SET_OID_BYTES_READ(NdisOidRequest) = 0;
                break;

            case NdisRequestMethod:
                // Initialize this to zero to avoid returning random data
                METHOD_OID_BYTES_READ(NdisOidRequest) = 0;
                METHOD_OID_BYTES_WRITTEN(NdisOidRequest) = 0;
                break;
        }
        
        goto Exit;        
    }

    // Verify if the OID is supported in supported
    switch(NdisOidRequest->RequestType)
    {
        case NdisRequestQueryInformation:
        case NdisRequestQueryStatistics:
            if (!oidTableEntry->Queryable)
            {
                WFCError("Query of OID 0x%08x not supported ", oid);
                ndisStatus = NDIS_STATUS_INVALID_STATE;
                goto Exit;
            }
            
            // Determine buffer length that will be used later
            bytesNeededLocation = (PULONG)&QUERY_OID_BYTES_NEEDED(NdisOidRequest);
            infoBufferLength = QUERY_OID_BUFFER_LENGTH(NdisOidRequest);

            // Initialize this to zero to avoid returning random data
            QUERY_OID_BYTES_WRITTEN(NdisOidRequest) = 0;

            if (oidTableEntry->QuerySerialized)
            {
                *pSerializedJob = TRUE;
            }

            *pHandlerJobType = oidTableEntry->QueryJobType;
            break;

        case NdisRequestSetInformation:
            if (!oidTableEntry->Settable)
            {
                WFCError("Set of OID 0x%08x not supported ", oid);
                ndisStatus = NDIS_STATUS_INVALID_STATE;
                goto Exit;
            }

            // Determine buffer length that will be used later
            bytesNeededLocation = (PULONG)&SET_OID_BYTES_NEEDED(NdisOidRequest);
            infoBufferLength = SET_OID_BUFFER_LENGTH(NdisOidRequest);

            // Initialize this to zero to avoid returning random data
            SET_OID_BYTES_READ(NdisOidRequest) = 0;

            if (oidTableEntry->SetMethodSerialized)
            {
                *pSerializedJob = TRUE;
            }

            *pHandlerJobType = oidTableEntry->SetMethodJobType;
            break;

        case NdisRequestMethod:
            if (!oidTableEntry->Method)
            {
                WFCError("Method request on OID 0x%08x not supported ", oid);
                ndisStatus = NDIS_STATUS_INVALID_STATE;
                goto Exit;
            }

            // Determine buffer length that will be used later
            bytesNeededLocation = (PULONG)&METHOD_OID_BYTES_NEEDED(NdisOidRequest);
            
            // Larger of the two buffer lengths
            infoBufferLength = MAX(METHOD_OID_INPUT_LENGTH(NdisOidRequest),
                                METHOD_OID_OUTPUT_LENGTH(NdisOidRequest));

            // Initialize this to zero to avoid returning random data
            METHOD_OID_BYTES_READ(NdisOidRequest) = 0;
            METHOD_OID_BYTES_WRITTEN(NdisOidRequest) = 0;

            if (oidTableEntry->SetMethodSerialized)
            {
                *pSerializedJob = TRUE;
            }
            *pHandlerJobType = oidTableEntry->SetMethodJobType;                                
            break;

        default:
            WFCError("NDIS_OID_REQUEST for OID 0x%08x contains unknown request type %d", 
                oid, NdisOidRequest->RequestType);
            ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
            goto Exit;
    }


    // Verify length is big enough
    if (infoBufferLength < oidTableEntry->MinBufferSize)
    {
        WFCError("InformationBufferLength %d for OID 0x%08x less than minimum required %d\n", 
            infoBufferLength,
            oid,
            oidTableEntry->MinBufferSize);

        // Set the bytes needed value
        *bytesNeededLocation = oidTableEntry->MinBufferSize;
        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
        goto Exit;
    }

    pTargetPort = this->GetPortFromNdisPortNumber(
        NdisOidRequest->PortNumber );

    if ( pTargetPort != NULL &&
         (pTargetPort->GetPortType() & oidTableEntry->AllowedPortTypes) == 0  )
    {
        WFCError(
            "OID not allowed on port (Port Number: 0x%x) of type 0x%x (Allowed 0x%x)",
            NdisOidRequest->PortNumber,
            pTargetPort->GetPortType(),
            oidTableEntry->AllowedPortTypes );

        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;
    }
    
Exit:
    *ppOidHandler = oidTableEntry;
    
    TraceExit(ndisStatus);    

    // OID must be at this time and has enough buffer to hold minimum data
    return ndisStatus;
}

VOID
CAdapter::OnOidRequestCompleteFromMiniport(
    _In_  PNDIS_OID_REQUEST       Request,
    _In_  NDIS_STATUS             Status
    )
{
    m_CtlPlane.OnOidRequestCompleteFromMiniport(
        Request,
        Status
        );
}

VOID
CAdapter::OnStatusIndicationFromMiniport(
    _In_ PNDIS_STATUS_INDICATION  StatusIndication
    )
{
    m_CtlPlane.OnStatusIndicationFromMiniport(
        StatusIndication
        );
}


NDIS_STATUS 
CAdapter::SetPersistentSoftwareRadioOffState(
    _In_ BOOLEAN    IsRadioOff )
/*++

--*/
{
    NDIS_STATUS status = NDIS_STATUS_FAILURE;
    WFC_REG_ENTRY regEntry;
    
    int i;
    CRegistryHelper regHelper(GetMiniportAdapterHandle());
    
    for ( i=0; i < SIZEOF_ARRAY(g_AdapterRegTable); i++ ) 
    {
        if ( g_AdapterRegTable[i].PropertyName == WfcAdapterDesiredSoftwareRadioOff )
        {
            break;
        }
    }

    if ( i < SIZEOF_ARRAY(g_AdapterRegTable) )
    {
        //
        // Found our entry
        //
        regEntry = g_AdapterRegTable[i];    // make a copy so we can write
        regEntry.Default = IsRadioOff; // deafult field is overloaded for write to reg
        status = regHelper.WriteAdapterRegistryEntry( &regEntry );
    }

    return status;
}



void CAdapter::OnOperationCompleted(
      _In_ NDIS_STATUS OperationStatus,
      _In_opt_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);

    // Only jobs can be sending these. One job is the CIhvTaskRequestJob
    CJobBase *pJobBase = (CJobBase *)SenderObject;
    
    delete pJobBase;
}

void CAdapter::OnDeviceIndicationArrived(
    _In_ WDI_INDICATION_TYPE                 IndicationType,
    _In_ WFC_COMMAND_TOKEN                   CommandToken,
    _In_ WFC_PORT_ID                         PortId,
    _In_ ULONG                               EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    UNREFERENCED_PARAMETER(CommandToken);
    UNREFERENCED_PARAMETER(PortId);

    if (PortId != WDI_PORT_ID_ADAPTER)
    {
        // Dont care about non-Adapter notification
        return;
    }

    switch( IndicationType )
    {
        case WiFiIndicationRadioStatus:
            OnRadioStateIndication(
                EventBufferLength,
                EventBuffer );
            break;

        case WiFiIndicationFirmwareStalled:
            OnFirmwareStalledIndication(
                EventBufferLength,
                EventBuffer );
            break;

        case WiFiIndicationIhvTaskRequest:
            OnIhvTaskRequestReceived(
                EventBufferLength,
                EventBuffer );
            break;
    }
}


NDIS_STATUS CAdapter::RegisterForAdapterNotifications()
{
    NDIS_STATUS ndisStatus;
    //
    // Register callback to listen to unsolicit message on the adapter
    //
    NT_ASSERT( m_EventRegistrationToken == WFC_REGISTRATION_TOKEN_INVALID );
    ndisStatus = m_NotificationManager.RegisterForNotifications(
                    WiFiIndicationInvalid,      // Any notification,
                    WFC_COMMAND_TOKEN_INVALID,  // unsolicit
                    WDI_PORT_ID_ADAPTER,
                    this,
                    &m_EventRegistrationToken );

    if(ndisStatus != NDIS_STATUS_SUCCESS)
    {
            WFCError("Failed to register for unsolicited notifications");
    }
    return ndisStatus;
}

NDIS_STATUS CAdapter::DeregisterFromAdapterNotifications()
{
    NDIS_STATUS ndisStatus;
    //
    // Register callback to listen to unsolicit message on the adapter
    //

    NT_ASSERT( m_EventRegistrationToken != WFC_REGISTRATION_TOKEN_INVALID );
    ndisStatus = m_NotificationManager.DeregisterNotificationHandler(
                    m_EventRegistrationToken );

    if(ndisStatus == NDIS_STATUS_SUCCESS)
    {
        m_EventRegistrationToken = WFC_REGISTRATION_TOKEN_INVALID;
    }
    else
    {
        WFCError("Failed to deregister for unsolicited notifications");
    }
    return ndisStatus;
}


void CAdapter::OnRadioStateIndication(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
/*++

    Handle Radio state change indication.


   NB: Per internal discussion
        1. We will always set radio to all bands, i.e. WDI_BAND_ID_ANY
        2. Assume FW always indicates radio state with all bands.
        3. We maintain radio state at adapter level.
          If there is request to indcaite per Band, we have to deal with them.
          Then we also have to fix the GetPropertyPropertyCacheList() for 
          WfcAdapterPropertyBandInfo

--*/
{
    DOT11_PHY_STATE_PARAMETERS phyStateParams;
    NDIS_STATUS status;
    WDI_INDICATION_RADIO_STATUS_PARAMETERS radioStatus;
    CPort * pPort = NULL;
    UINT i = 0;

    TraceEntry();
    EventBufferLength, EventBuffer;

    WFCTrace("[Radio] Got radio state indication from device.");

    //
    // Parse the indication
    //
    status = ParseWdiIndicationRadioStatus(
        EventBufferLength- sizeof( WFC_MESSAGE_METADATA),
        EventBuffer + sizeof( WFC_MESSAGE_METADATA ),
        GetTlvContext( ),
        &radioStatus);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError( "[Radio] Failed to parse radio state indication. Status = 0x%08x", status );
        goto Exit;
    }

    //
    // get the current states; assume falses first.
    //
    BOOLEAN bCurrHwRadioOff = FALSE;
    BOOLEAN bCurrSwRadioOff = FALSE;
    BOOLEAN bDesiredSwRadioOff = FALSE;

    //
    // from cached current states
    //
    CAdapterPropertyCache* propertyCache = GetAdapterPropertyCache();
    
    status = propertyCache->GetPropertyBoolean(
                WfcAdapterHardwareRadioOff,
                &bCurrHwRadioOff);
    if ( status != STATUS_SUCCESS )
    {
        NT_ASSERT(0);
        WFCError( "[Radio] Failed to read cache for Previous Hardware Radio Off 0x%x", status );
    }

    status = propertyCache->GetPropertyBoolean(
                WfcAdapterSoftwareRadioOff,
                &bCurrSwRadioOff);
    if ( status != STATUS_SUCCESS )
    {
        NT_ASSERT(0);
        WFCError( "[Radio] Failed to read cache for Previous Software Radio Off 0x%x", status );
    }


    status = propertyCache->GetPropertyBoolean(
                WfcAdapterDesiredSoftwareRadioOff,
                &bDesiredSwRadioOff);
    if ( status != STATUS_SUCCESS )
    {
        NT_ASSERT(0);
        WFCError( "[Radio] Failed to read cache for Desired Software Radio Off 0x%x", status );
    }
    
    WFCTrace( "[Radio] Received RadioState Indication Previous Hardware=%d Previous Software=%d New Hardware=%d New Software=%d Desired Software = %d",
                !bCurrHwRadioOff,
                !bCurrSwRadioOff,
                radioStatus.RadioState.HardwareState,
                radioStatus.RadioState.SoftwareState,
                !bDesiredSwRadioOff);

    //
    // set the new state in the cache
    //
    status = propertyCache->SetPropertyBoolean(
                WfcAdapterHardwareRadioOff,
                (radioStatus.RadioState.HardwareState == 0));
    if ( status != STATUS_SUCCESS )
    {
        NT_ASSERT(0);
        WFCError( "[Radio] Failed to write cache for New Hardware Radio Off 0x(%x)", status);
    }

    status = propertyCache->SetPropertyBoolean(
                WfcAdapterSoftwareRadioOff,
                (radioStatus.RadioState.SoftwareState == 0));
    if ( status != STATUS_SUCCESS )
    {
        NT_ASSERT(0);
        WFCError( "[Radio] Failed to write cache for New Software Radio Off 0x(%x)", status);
    }

    //
    // If we are expecting ourselves to be in this state, persist the software radio state 
    // in the registry. We don't want to do this just based on indications from the IHV
    //    
    if (bDesiredSwRadioOff == ((radioStatus.RadioState.SoftwareState == 0) ? TRUE : FALSE))
    {
        // We had received a request and we have ended up in the state we expected
        // to be in. Persist this state
        status = SetPersistentSoftwareRadioOffState(bDesiredSwRadioOff);
        if ( status != STATUS_SUCCESS )
        {
            //NT_ASSERT(0);
            WFCError( "[Radio] Failed to persist Desired Software Radio Off state (%x)", status );
        }
    }        

    //
    // indicate only when state changed. 

    //
    // try to always indicate for HCK wlan power test
    //if ( (!bCurrHwRadioOff) != pRadioState->hwRadioState ||
    //    (!bCurrSwRadioOff) != pRadioState->swRadioState )

    {
        phyStateParams.uPhyId = DOT11_PHY_ID_ANY;

        phyStateParams.bHardwarePhyState = radioStatus.RadioState.HardwareState;
        phyStateParams.bSoftwarePhyState = radioStatus.RadioState.SoftwareState;

        ASSIGN_NDIS_OBJECT_HEADER(
            phyStateParams.Header, 
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_PHY_STATE_PARAMETERS_REVISION_1,
            sizeof(DOT11_PHY_STATE_PARAMETERS) );

        //
        // Indication needs to be sent for all ports
        //
        for (i = 0; i < c_MaxPorts; i++)
        {
            pPort = this->m_pPortList[i];
            if (pPort != NULL)
            {
                WFCInfo( "[Radio] Indicating Radio State on NDIS Port %d Hardware=%d Software=%d",
                            pPort->GetNdisPortNumber(),
                            phyStateParams.bHardwarePhyState,
                            phyStateParams.bSoftwarePhyState);

                IndicateStatus(
                    pPort->GetNdisPortNumber(),
                    NDIS_STATUS_DOT11_PHY_STATE_CHANGED,
                    NULL,
                    &phyStateParams,
                    sizeof(phyStateParams) );

            }
        }

    }

Exit:
    TraceExit(NDIS_STATUS_SUCCESS);
}


void CAdapter::OnFirmwareStalledIndication(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
/*++

    LE reports detection of firmware stalled. LE may receives the report from Firmware
    detecting hang itself, e.g. by watchdog timer. But it does not matter. This status come
    from LE. We will log and start Reset/Recovery process.

--*/
{
    DeviceCommand * pActiveCommand;
    ULONG commandId = 0;

    TraceEntry();
    //
    // assume we dont have a flag, there is no payload
    //
    ASSERT( EventBufferLength >= sizeof( WFC_MESSAGE_METADATA ));

    //
    /// code coverage
    /// __debugbreak();

    EventBufferLength, EventBuffer;

    //
    // try best to record active command if one exists.
    //
    pActiveCommand = m_CommandScheduler.GetActiveCommand();
    if ( pActiveCommand ) 
    {
        pActiveCommand->get_CommandType( & commandId );
    }

    NdisWriteErrorLogEntry( GetMiniportAdapterHandle(),
                        NDIS_ERROR_CODE_HARDWARE_FAILURE,
                        2,
                        RESET_RECOVERY_FIRMWARE_STALLED,
                        commandId);

    InterlockedIncrement(&m_lResetTriggerCount);
    
    BOOLEAN bResetRecoveryOff = GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(
        WfcAdapterResetRecoveryOff, FALSE );

    TraceLoggingResetRecovery( RESET_RECOVERY_FIRMWARE_STALLED,
                                commandId,
                                m_lResetTriggerCount,
                                m_OemPLDRSupported,
                                bResetRecoveryOff);

    // We dont check for MiniportResetHandler here because this operation will
    // was triggered by IHV sending us an indication
    
    if ( InterlockedIncrement( &m_lResetRecovery ) > 1 )
    {
        //
        // RR has already occurred, be done.
        //
        WFCInfo( "Processing Reset Recovery OID when it has already occurred, done" );
        goto exit;
    }

    //
    // Go get Diagnose info, this should be RR OID self
    //
    if ( pActiveCommand )
    {
        TriggerControlPathDiagnostics( commandId );
        CollectDebugData( pActiveCommand,
            NULL,
            pActiveCommand->get_ParentJob());
    }
    else
    {
        TriggerControlPathDiagnostics( 0 );
        CollectDebugData( NULL, NULL, NULL);
    }

    //
    // Diagnosis taken, check if OEM does not enable UEFI support
    //
    if ( m_OemPLDRSupported == 0 ) 
    {
        WFCError( "Firmware indicates hang but OEM disables PLDR; bail out" );
        goto exit;
    }

    //
    // if RR is disabled, dont request it.
    //
    if (bResetRecoveryOff == FALSE)
    {
        ASSERT(FALSE);
        ReenumerateFailedAdapter();
    }

exit:
    TraceExit( 0 );

}


void CAdapter::OnIhvTaskRequestReceived(
    _In_ ULONG EventBufferLength,
    _In_reads_bytes_(EventBufferLength) BYTE* EventBuffer )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    bool bAllocatedJob = false;
    
    CIhvTaskRequestJob *pIhvTaskJob = new CIhvTaskRequestJob(
            IActivityId::get_NextActivityId());
    if (pIhvTaskJob == NULL)
    {
        ndisStatus = NDIS_STATUS_RESOURCES;
        WFCError("[IHVTask] Unable to allocate IHV task request job. Error = 0x%08x", ndisStatus);
        goto exit;
    }
    bAllocatedJob = true;

    // Call a serialized initialization function
    ndisStatus = pIhvTaskJob->InitializeFromJob(
        this,
        0,
        WDI_PORT_ID_ADAPTER,
        EventBufferLength- sizeof( WFC_MESSAGE_METADATA),
        EventBuffer + sizeof( WFC_MESSAGE_METADATA )
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[IHVTask] Failed to initialize IHV task request job. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    // Queue the serialized job
    ndisStatus = GetSerializedJobList()->QueueSerializedJob(
        pIhvTaskJob,
        this
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[IHVTask] Failed to queue serialized IHV task request job. Error = 0x08%x",
            ndisStatus
            );
        goto exit;
    }

    // Job is queued, dont free it
    bAllocatedJob = false;

    // It will fire on its own later
exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (bAllocatedJob)
        {
            delete pIhvTaskJob;
            pIhvTaskJob = NULL;
        }
    }

    TraceExit(ndisStatus);
}
    
NDIS_STATUS CAdapter::HandleOidRequest(
    _In_ PNDIS_OID_REQUEST  NdisRequest,
    _In_ BOOLEAN            DirectOid)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    COidJobBase* pOidJob = NULL;
    POID_HANDLER_ENTRY pHandlerInfo = NULL;
    BOOLEAN     serializedJob = FALSE;
    WFC_JOB_TYPE jobType;

    TraceEntry();

    if (!DirectOid)
    {
        // Remember this for debugging purposes. We dont remember direct OID since
        // they are not serialized
        m_DebugLastOidRequest = NdisRequest;
        m_DebugLastOidStatus = NDIS_STATUS_SUCCESS;
        m_DebugLastOidValue = NdisRequest->DATA.QUERY_INFORMATION.Oid;
        m_DebugLastOidSubmitTime = CSystem::get_CurrentTime();
    }

    WFCInfo("[OID] Received OID 0x%08x on NDIS Port %d RequestType = %d", NdisRequest->DATA.QUERY_INFORMATION.Oid, 
        NdisRequest->PortNumber,
        NdisRequest->RequestType);

    // Preprocess the OID
    ndisStatus = PreprocessOid(NdisRequest, 
                    &pHandlerInfo,
                    &jobType,
                    &serializedJob);
    if ((ndisStatus == NDIS_STATUS_NOT_SUPPORTED) && (pHandlerInfo == NULL))
    {
        // OID unsupported by us. Check if this is something we will be forwarding down
        if (GetAdapterPropertyCache()->GetPropertyBooleanOrDefault(WfcAdapterPropertyNonWdiOidsSupported, FALSE))
        {
            pHandlerInfo = &g_UnrecognizedOidForwardHandlerInfo;
            jobType = WiFiJobUnrecognizedOidHandler;
            serializedJob = FALSE;
            pHandlerInfo->IsDirectOid = DirectOid;
            ndisStatus = NDIS_STATUS_SUCCESS;
        }
    }
    
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[OID] Pre-processing of OID request failed (0x%x)", ndisStatus); 
        pHandlerInfo = NULL;
        goto Exit;
    }

    NT_ASSERT(pHandlerInfo);

    // Create the OID handler job
    ndisStatus = COidJobBase::s_CreateOidJob(
        jobType,
        this,
        pHandlerInfo,
        NdisRequest,
        &pOidJob);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("[OID] Failed to create Job for OID 0x%08x (0x%x)", 
            pHandlerInfo->Oid,
            ndisStatus);
        goto Exit;
    }

    if (serializedJob)
    {
        // Sign up for the job started event
        pOidJob->EventJobStarted = (INotifyOperationStarted*)(&m_OidJobNotificationHandler);
        
        // Queue the serialized job
        ndisStatus = m_SerializedJobList.QueueSerializedJob(
            pOidJob,
            &m_OidJobNotificationHandler
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("[OID] Failed to queue serialized job for OID 0x%08x (0x%x)",
                pHandlerInfo->Oid,
                ndisStatus
                );
            goto Exit;
        }
    }
    else
    {
        // Queue the non-serialized job
        ndisStatus = m_ActiveJobsList.StartAddNewJob(
            pOidJob,
            &m_OidJobNotificationHandler
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("[OID] Failed to queue non-serialized job for OID 0x%08x (0x%x)",
                pHandlerInfo->Oid,
                ndisStatus
                );
            goto Exit;
        }
    }

Exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pOidJob != NULL)
        {
            delete pOidJob;
        }
        
        WFCInfo("[OID] Synchronously Completed OID 0x%08x with status 0x%x", 
            NdisRequest->DATA.QUERY_INFORMATION.Oid,
            ndisStatus);

        if (!DirectOid)
        {
            m_DebugLastOidRequest = NULL;
            m_DebugLastOidStatus = ndisStatus;
        }
    }
    else
    {
        // Status success means we will complete this asynchronously 
        ndisStatus = NDIS_STATUS_PENDING;

        if ( pHandlerInfo->IsDirectOid )
        {
            WFCTrace(
                "[OID] Synchronously Completing Direct OID request 0x%08x with NDIS_STATUS_SUCCESS. Job queue will handle further OS notification.",
                NdisRequest->DATA.QUERY_INFORMATION.Oid );

            ndisStatus = NDIS_STATUS_SUCCESS;
        }
    }

    TraceExit(ndisStatus);

    return ndisStatus;
}


VOID CAdapter::CleanupDataPath()
{

    TraceEntry();

    //TODO proper datapath cleanup
    if (IsInitComplete(WfcAdapterInitializeDataPathStarted))
    {
        //Stop the old frame detection timer
        m_TxMgr.StopOldFrameDetectionTimer();

        m_MiniportDataHandlers.TalTxRxStopHandler(GetMiniportTalTxRxContext());
        if (m_WfcFrameLookasideInitialized)
        {
            NdisDeleteNPagedLookasideList(&m_WfcFrameLookaside);
            m_WfcFrameLookasideInitialized = false;
        }
        UnmarkInitComplete( WfcAdapterInitializeDataPathStarted );
    }

    if (IsInitComplete(WfcAdapterInitializeDataPathInitialized))
    {
        m_MiniportDriver->GetWdiCharacteristics()->TalTxRxDeinitializeHandler(
            GetMiniportTalTxRxContext());
        m_RxMgr.Deinitialize();
        UnmarkInitComplete(WfcAdapterInitializeDataPathInitialized);        
    }
#if WDI_DISABLED
    if (m_DataPathFunctions.NICDeinitialize)
    {
        m_DataPathFunctions.NICDeinitialize(m_NICContext);
    }
#endif

    TraceExit(NDIS_STATUS_SUCCESS);    
}

NDIS_STATUS CAdapter::InitializeDataPath()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    TAL_TXRX_HANDLE miniportTalTxRxContext = NULL;

    TraceEntry();
    
    RtlZeroMemory( &m_MiniportDataHandlers, sizeof( m_MiniportDataHandlers ) );
    ASSIGN_NDIS_OBJECT_HEADER( m_MiniportDataHandlers.Header, 
        NDIS_OBJECT_TYPE_MINIPORT_WDI_DATA_HANDLERS,
        NDIS_OBJECT_TYPE_MINIPORT_WDI_DATA_HANDLERS_REVISION_1,
        NDIS_SIZEOF_MINIPORT_WDI_DATA_HANDLERS_REVISION_1 );

    ndisStatus = m_MiniportDriver->GetWdiCharacteristics()->TalTxRxInitializeHandler(
        GetMiniportAdapterContext(),
        this,
        &m_MiniportDataApi,
        &miniportTalTxRxContext,
        &m_MiniportDataHandlers,
        &m_MiniportWdiFrameMetadataExtraSpace );

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Tal TxRx initialization failed (0x%x)", ndisStatus);
        goto exit;
    }

    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT( m_MiniportDataHandlers.Header,
            NDIS_OBJECT_TYPE_MINIPORT_WDI_DATA_HANDLERS,
            NDIS_OBJECT_TYPE_MINIPORT_WDI_DATA_HANDLERS_REVISION_1,
            NDIS_SIZEOF_MINIPORT_WDI_DATA_HANDLERS_REVISION_1 ))
    {
        WFCError( "Tal TxRx initialization failed, wrong data handlers: Type=0x%02x, Revision=%u, Size=%u",
            m_MiniportDataHandlers.Header.Type,
            m_MiniportDataHandlers.Header.Revision,
            m_MiniportDataHandlers.Header.Size );
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    }

    SetMiniportTalTxRxContext(miniportTalTxRxContext);

    ASSERT( m_MiniportWdiFrameMetadataExtraSpace < 256 ); // dont use excessive space

exit:
    TraceExit(ndisStatus);

    return ndisStatus;
}

NDIS_STATUS CAdapter::StartDataPath()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_TXRX_TARGET_CONFIGURATION wifiTxRxConfig;
    TAL_TXRX_PARAMETERS talTxRxParams;
    PWFC_DATAPATH_CAPABILITIES pDatapathCapabilities = NULL;

    TraceEntry();

    pDatapathCapabilities = GetDatapathCapabilities();
    if (pDatapathCapabilities == NULL)
    {
        WFCError("Datapath capabilities not present");
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }
    //
    // Create WDI frame Lookaside
    //
    m_WfcFrameSize = sizeof( WFC_FRAME ) + m_MiniportWdiFrameMetadataExtraSpace;
    InitializeNPagedLookaside(m_WfcFrameSize, &m_WfcFrameLookaside);
    m_WfcFrameLookasideInitialized = true;

    //initialize datapath components
    ndisStatus = m_FrameIdTable.Initialize();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize FrameIdTable");
        goto exit;
    }

    ndisStatus = m_PeerTable.Initialize(pDatapathCapabilities->MaxNumPeers);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Unable to initialize PeerTable (0x%x)", ndisStatus);
        goto exit;
    }

    ndisStatus = m_TxMgr.Initialize(this,pDatapathCapabilities,WFC_CONSTANT::MaximumNumberOfPorts);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Unable to initialize TxMgr (0x%x)", ndisStatus);
        goto exit;
    }

    ndisStatus = m_RxMgr.Initialize(this, &m_DatapathCapabilities, &m_MiniportDataHandlers);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Unable to initialize RxMgr (0x%x)", ndisStatus);
        goto exit;
    }

    wifiTxRxConfig.MaxNumPorts = WFC_CONSTANT::MaximumNumberOfPorts;
    wifiTxRxConfig.MaxNumPeers = pDatapathCapabilities->MaxNumPeers;
    wifiTxRxConfig.TxRxParams.TxRxCapabilities.InterconnectType = pDatapathCapabilities->InterconnectType;
    wifiTxRxConfig.TxRxParams.TxRxCapabilities.TransmitCapabilities.TargetPriorityQueueing = pDatapathCapabilities->TxTargetPriorityQueueing;
    wifiTxRxConfig.TxRxParams.TxRxCapabilities.TransmitCapabilities.ExplicitSendCompleteFlagRequired = pDatapathCapabilities->TxExplicitSendCompleteFlagRequired;
    wifiTxRxConfig.TxRxParams.TxRxCapabilities.TransmitCapabilities.MaxScatterGatherElementsPerFrame = pDatapathCapabilities->TxMaxScatterGatherElementsPerFrame;
    wifiTxRxConfig.TxRxParams.TxRxCapabilities.TransmitCapabilities.MinEffectiveSize = pDatapathCapabilities->TxMinEffectiveFrameSize;
    wifiTxRxConfig.TxRxParams.TxRxCapabilities.TransmitCapabilities.FrameSizeGranularity = pDatapathCapabilities->TxFrameSizeGranularity;

    wifiTxRxConfig.TxRxParams.TxRxCapabilities.ReceiveCapabilities.RxTxForwarding = pDatapathCapabilities->RxTxForwarding;
    wifiTxRxConfig.TxRxParams.TxRxCapabilities.ReceiveCapabilities.MaxThroughput = pDatapathCapabilities->RxMaxThroughput;

    //TODO flatten wifiTxRxconfig by at least one layer
    ndisStatus = m_MiniportDataHandlers.TalTxRxStartHandler(GetMiniportTalTxRxContext(), &wifiTxRxConfig,&talTxRxParams);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Tal datapath initialization failed (0x%x)", ndisStatus);
        goto exit;
    }

    m_TxMgr.SetMaxOutstandingTransfers(talTxRxParams.MaxOutstandingTransfers);

exit:

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        if (m_WfcFrameLookasideInitialized)
        {
            NdisDeleteNPagedLookasideList(&m_WfcFrameLookaside);
            m_WfcFrameLookasideInitialized = false;
        }
    }
    
    TraceExit(ndisStatus);

    return ndisStatus;
}


VOID CAdapter::SetDataPathApis()
{
    TraceEntry();
    RtlZeroMemory(&m_MiniportDataApi, sizeof(NDIS_WDI_DATA_API));

    m_MiniportDataApi.Header.Type = NDIS_OBJECT_TYPE_WDI_DATA_API;
    m_MiniportDataApi.Header.Size = NDIS_SIZEOF_WDI_DATA_API_REVISION_1;
    m_MiniportDataApi.Header.Revision = NDIS_OBJECT_TYPE_WDI_DATA_API_REVISION_1;

    m_MiniportDataApi.TxDequeueIndication = AdapterTxDequeueInd;
    m_MiniportDataApi.TxTransferCompleteIndication = AdapterTxTransferCompleteInd;
    m_MiniportDataApi.TxSendCompleteIndication = AdapterTxSendCompleteInd;
    m_MiniportDataApi.TxQueryRATIDState = AdapterTxQueryRaTidState;
    m_MiniportDataApi.TxSendPauseIndication = AdapterTxSendPauseInd;
    m_MiniportDataApi.TxSendRestartIndication = AdapterTxSendRestartInd;
    m_MiniportDataApi.TxReleaseFrameIndication = AdapterTxReleaseFramesInd;
    m_MiniportDataApi.TxInjectFrameIndication = AdapterTxInjectFrameInd;
    m_MiniportDataApi.TxAbortConfirm = AdapterTxAbortConfirm;

    m_MiniportDataApi.RxInorderDataIndication = AdapterRxInorderDataInd;
    m_MiniportDataApi.RxStopConfirm = AdapterRxStopConfirm;
    m_MiniportDataApi.RxFlushConfirm = AdapterRxFlushConfirm;

    m_MiniportDataApi.PeerCreateIndication = AdapterPeerCreateInd;
    m_MiniportDataApi.PeerDeleteIndication = AdapterPeerDeleteInd;

    m_MiniportDataApi.AllocateWiFiFrameMetaData = AdapterAllocateWifiFrameMetadata;
    m_MiniportDataApi.FreeWiFiFrameMetaData = AdapterFreeWifiFrameMetadata;

    TraceExit(NDIS_STATUS_SUCCESS);
}
#if WDI_DISABLED
//
// Datapath Inteface to Lower edge: datapath callbacks
//
NTSTATUS AdapterSendComplete(
        _In_ HANDLE AdapterContext,
        _In_ WFC_PORT_ID PortId,
        _In_ PTX_DESCRIPTOR pTxD)
{
    TraceEntry();

    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    CAdapter *pAdapter = (CAdapter*) AdapterContext;

    ndisStatus = pAdapter->SendTxDCompleted(PortId, pTxD);

    TraceExit(ndisStatus);

    return ndisStatus;
}

NTSTATUS AdapterRequestReset(_In_ HANDLE AdapterContext)
{
    UNREFERENCED_PARAMETER(AdapterContext);

    return NDIS_STATUS_SUCCESS;
}
#endif //WDI_DISABLED
NDIS_STATUS CAdapter::SendCompleteNbl(
    _In_ PNET_BUFFER_LIST pNbl)
/*
We do not need to lock the port because the port deletion will 
happen only after successful pause. Which means 
there won't be any SendCompleteNbl calls afterward for that port.
*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PNET_BUFFER_LIST pCurNbl = NULL;
    PNET_BUFFER_LIST pNextNbl = NULL;
    PNET_BUFFER_LIST pCurPortNbl = NULL;

    PWDI_FRAME_METADATA pWifiFrame = NULL;
    WDI_PORT_ID lastPortId = WDI_PORT_ANY;
    WDI_PORT_ID curPortId = WDI_PORT_ANY;

    CPort*  pPort = NULL;

    TraceEntry();

    ASSERT(pNbl);

    pCurNbl = pNbl;

    while(pCurNbl != NULL)
    {

        pNextNbl = NET_BUFFER_LIST_NEXT_NBL(pCurNbl);
        NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = NULL;

        pWifiFrame = GetWiFiFrameMetadataFromNbl(pCurNbl);
        ASSERT(pWifiFrame);

        NET_BUFFER_LIST_STATUS(pCurNbl) = NDIS_STATUS_SUCCESS;

        curPortId = pWifiFrame->u.txMetaData.PortID;

        if(lastPortId != curPortId)
        {
            if (pCurPortNbl != NULL &&
                pPort != NULL)
            {
                pPort->SendCompleteNetBufferLists( pCurPortNbl );
            }
            pCurPortNbl = NULL;
            pPort = GetPortFromPortId( curPortId );
            if(!pPort)
            {
                // this shouldn't happen
                WFCError( "ERROR: Couldn't find [Port:%x] pNbl:%p pWifiFrame:%p", curPortId, pCurNbl, pWifiFrame );
                NT_ASSERTMSG_NOASSUME( "Invalid PortId in Tx path SendCompleteNbl!", FALSE );
            }
        }

        NET_BUFFER_LIST_NEXT_NBL(pCurNbl) = pCurPortNbl;
        pCurPortNbl = pCurNbl;

        lastPortId = curPortId;
        pCurNbl = pNextNbl;
    }

    //the port should be assigned
    ASSERT(pPort);

    if (pCurPortNbl != NULL &&
        pPort != NULL)
    {
        pPort->SendCompleteNetBufferLists( pCurPortNbl );
        pCurPortNbl = NULL;
    }

    TraceExit(ndisStatus);

    return ndisStatus;
}

VOID CAdapter::SendNetBufferLists(
    _In_ PNET_BUFFER_LIST   NetBufferList,
    NDIS_PORT_NUMBER   PortNumber,
    ULONG              SendFlags)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CPort*  pPort = NULL;
    CWFCExclusiveAsyncGate *pPortLock = NULL;

    TraceEntry();

    if ( PortNumber != INVALID_NDIS_PORT_NUMBER )
    {
        pPort = GetPortFromNdisPortNumberAndLock (PortNumber, 
                                                &pPortLock);
    }

    if (!pPort)
    {
        WFCError("Couldn't convert NDIS Port number to Port ID.");
        goto exit;
    }

    pPort->SendNetBufferLists(NetBufferList, SendFlags);

exit:

    if (pPortLock)
    {
        pPortLock->Leave();
    }

    TraceExit(ndisStatus);

}

VOID CAdapter::CancelSendNetBufferLists(
    _In_  PVOID  CancelId)
{
    UNREFERENCED_PARAMETER(CancelId);
    TraceEntry();
}


NDIS_STATUS CAdapter::IndicateFrames(
    _In_ WFC_PORT_ID PortId,
    _In_ PNET_BUFFER_LIST pNblChain,
    _In_ UINT32 NumFrames,
    _In_ ULONG ReceiveFlags )
/*++

    Redispatched from RxMgr to indicate up. This function de-mux into CPort
    while holding port lock

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CPort*  pPort = NULL;
    CWFCExclusiveAsyncGate *pPortLock = NULL;

    TraceEntry();

    pPort = GetPortFromPortIdAndLock (PortId, &pPortLock);
    if (!pPort)
    {
        ndisStatus = NDIS_STATUS_INVALID_PORT;
        WFCError("Couldn't convert WFC_PORT_ID to Port ID.");
        goto exit;
    }

    ndisStatus = pPort->IndicateFrames( pNblChain, NumFrames, ReceiveFlags );

exit:

    if (pPortLock)
    {
        pPortLock->Leave();
    }

    TraceExit(ndisStatus);
    return ndisStatus;
}


VOID
CAdapter::ReturnNetBufferLists(
    _In_  PNET_BUFFER_LIST    NetBufferLists,
    _In_  ULONG               ReturnFlags)
{

    PNET_BUFFER_LIST pNbl, pNblNext=NULL;
    PWDI_FRAME_METADATA pWiFiFrameMetadata;
    PWFC_FRAME pWfcFrame;
    CPort *pPort= NULL;
    WDI_PORT_ID curPortId = 0;
    WDI_PORT_ID lastPortId = WDI_PORT_ANY;
    LONG curNumFrames = 0;

    TraceEntry();

    UNREFERENCED_PARAMETER(ReturnFlags); // unused, this only says if Dispatch_level call

    for ( pNbl = NetBufferLists; pNbl != NULL; pNbl = pNblNext )
    {
        pNblNext = NET_BUFFER_LIST_NEXT_NBL(pNbl);
        pWiFiFrameMetadata = GetWiFiFrameMetadataFromNbl( pNbl );
        pWfcFrame = WFC_FRAME_FROM_WDI_FRAME(pWiFiFrameMetadata);

        curPortId = pWfcFrame->u.Rx.PortId;

        if (curPortId != lastPortId)
        {
            if (pPort)
            {
                //if the portid changes in this chain decrement the count for the previous frames 
                // in the chain
                // on the first time pPort is NULL and won't take this path.
                pPort->ReturnNBLs(curNumFrames);
            }
            //get the port for this portId
            pPort = GetPortFromPortId(curPortId);

            if (!pPort)
            {
                //this shouldn't happen as we don't delete ports when there are frames outstanding
                // but might as well sound alarm here.
                ASSERT(FALSE);
                WFCError("Couldn't find a port with this Port ID.");
            }

            //reset count and update lastPortId
            curNumFrames = 0;
            lastPortId = curPortId;
        }

        curNumFrames++;
    }
    //after loop update the port with the number of frames returned
    if (pPort)
    {
        pPort->ReturnNBLs(curNumFrames);
    }

    m_RxMgr.OnNdisReturnNetBufferLists( NetBufferLists, ReturnFlags );

    TraceExit( 0 );
}

NDIS_STATUS CAdapter::CopyNetBufferLists(
    _In_                          PNET_BUFFER_LIST  NetBufferLists,
    _Outptr_result_nullonfailure_ PNET_BUFFER_LIST *pCopiedNetBufferLists,
    _In_                          ULONG             DataBackFill,
    _In_                          bool              bAllocateWfcFrame)
{
    PNET_BUFFER_LIST CopiedNetBufferLists = NULL;
    NDIS_STATUS ndisStatus = NDIS_STATUS_INVALID_PARAMETER;

    TraceEntry( );

    //
    // Clone each NBL
    //
    for (PNET_BUFFER_LIST pNbl = NetBufferLists, *ppNblCopy = &CopiedNetBufferLists;
        pNbl;
        pNbl = NET_BUFFER_LIST_NEXT_NBL( pNbl ))
    {
        //
        // We're going to manually copy the data and create new MDLs, so no point asking the API
        // to allocate MDLs we don't want.
        //
        PNET_BUFFER_LIST pNblCopy = NdisAllocateCloneNetBufferList( pNbl, NULL, NULL, NDIS_CLONE_FLAGS_USE_ORIGINAL_MDLS );
        if (pNblCopy == NULL)
        {
            WFCError( "NdisAllocateCloneNetBufferList failed" );
            ndisStatus = NDIS_STATUS_RESOURCES;
            goto EXIT;
        }

        *ppNblCopy = pNblCopy;
        ppNblCopy = &NET_BUFFER_LIST_NEXT_NBL( pNblCopy );
        NET_BUFFER_LIST_MINIPORT_RESERVED( pNblCopy )[0] = NULL;

        //
        // Clear out the cloned MDL pointers so if the copy fails part way through, cleanup is easy
        // !! NO FAILING ALLOWED HERE !!
        //
        for (PNET_BUFFER pNbCopy = NET_BUFFER_LIST_FIRST_NB( pNblCopy );
            pNbCopy;
            pNbCopy = NET_BUFFER_NEXT_NB( pNbCopy ))
        {
            NET_BUFFER_CURRENT_MDL( pNbCopy ) = NULL;
            NET_BUFFER_CURRENT_MDL_OFFSET( pNbCopy ) = 0;
            NET_BUFFER_DATA_LENGTH( pNbCopy ) = 0;
            NET_BUFFER_DATA_OFFSET( pNbCopy ) = 0;
            NET_BUFFER_FIRST_MDL( pNbCopy ) = NULL;
        }

        //
        // Create new MDLs (via retreat) and copy the data over
        //
        for (PNET_BUFFER pNbCurr = NET_BUFFER_LIST_FIRST_NB( pNbl ), pNbCurrCopy = NET_BUFFER_LIST_FIRST_NB( pNblCopy );
            pNbCurr;
            pNbCurr = NET_BUFFER_NEXT_NB( pNbCurr ), pNbCurrCopy = NET_BUFFER_NEXT_NB( pNbCurrCopy ))
        {
            ULONG bytesCopied;

            // If we have the original, we should also have the clone
            NT_ASSERT( pNbCurrCopy );
            __analysis_assume( pNbCurrCopy );

            // If the clone's next NB is NULL, the orginal should be too
            NT_ASSERT( (NET_BUFFER_NEXT_NB( pNbCurrCopy ) == NULL) == (NET_BUFFER_NEXT_NB( pNbCurr ) == NULL) );

            //Since the clone NB has data length of zero the retreat call will allocate the backfill
            ndisStatus = NdisRetreatNetBufferDataStart( pNbCurrCopy, NET_BUFFER_DATA_LENGTH( pNbCurr ), DataBackFill, NULL );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError( "NdisRetreatNetBufferDataStart failed (0x%08X)", ndisStatus );
                goto EXIT;
            }
            ndisStatus = NdisCopyFromNetBufferToNetBuffer( pNbCurrCopy, 0, NET_BUFFER_DATA_LENGTH( pNbCurr ), pNbCurr, 0, &bytesCopied );
            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCError( "NdisCopyFromNetBufferToNetBuffer failed (0x%08X)", ndisStatus );
                goto EXIT;
            }
            if (bytesCopied != NET_BUFFER_DATA_LENGTH( pNbCurr ))
            {
                WFCError( "NdisCopyFromNetBufferToNetBuffer failed to copy everything (expected = %u, actual = %u)",
                    NET_BUFFER_DATA_LENGTH( pNbCurr ), bytesCopied );
                ndisStatus = NDIS_STATUS_INVALID_DATA;
                goto EXIT;
            }
        }

        //
        // Succeeded in copying all of the NET_BUFFERs and MDLs and data, so now
        // copy the frame and metadata if requested
        //
        if (bAllocateWfcFrame)
        {
            PWFC_FRAME pWfcFrame = AllocateWfcFrame();
            if (pWfcFrame == NULL)
            {
                WFCError("AllocateWfcFrame failed");
                ndisStatus = NDIS_STATUS_RESOURCES;
                goto EXIT;
            }
            *pWfcFrame = *WFC_FRAME_FROM_WDI_FRAME(GetWiFiFrameMetadataFromNbl(pNbl));

            //
            // Fixup the NBL/Metadata pointers
            //
            NET_BUFFER_LIST_MINIPORT_RESERVED(pNblCopy)[0] = &pWfcFrame->WiFiFrameMetadata;
            pWfcFrame->WiFiFrameMetadata.pNBL = pNblCopy;
        }
    }

EXIT:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        FreeCopiedNetBufferLists( CopiedNetBufferLists );
        *pCopiedNetBufferLists = NULL;

        //
        // OACR thinks we might get here with a non-NDIS_STATUS_SUCCESS code that is still
        // a success code (i.e. non-negative), and thus doesn't like us returning NULL.
        // so fix the code.
        //
        if (ndisStatus > NDIS_STATUS_SUCCESS)
        {
            WFCInfo( "Unexpected success code (0x%08X) treated as error", ndisStatus );
            ndisStatus = NDIS_STATUS_FAILURE;
        }
    }
    else
    {
        *pCopiedNetBufferLists = CopiedNetBufferLists;
    }

    TraceExit( ndisStatus );

    return ndisStatus;
}

VOID CAdapter::FreeCopiedNetBufferLists(
    _In_ PNET_BUFFER_LIST  CopiedNetBufferLists )
{

    TraceEntry();

    //
    // Free any MDLs/memory or any frame metadata we might have allocated
    //
    for (PNET_BUFFER_LIST pNbl = CopiedNetBufferLists, pNblNext;
        pNbl;
        pNbl = pNblNext)
    {
        pNblNext = NET_BUFFER_LIST_NEXT_NBL( pNbl );

        if (GetWiFiFrameMetadataFromNbl( pNbl ) != NULL)
        {
            FreeWfcFrame( WFC_FRAME_FROM_WDI_FRAME( GetWiFiFrameMetadataFromNbl( pNbl ) ) );
        }
        for (PNET_BUFFER pNb = NET_BUFFER_LIST_FIRST_NB( pNbl );
            pNb;
            pNb = NET_BUFFER_NEXT_NB( pNb ))
        {
            if (NET_BUFFER_FIRST_MDL( pNb ) != NULL)
            {
                NdisAdvanceNetBufferDataStart( pNb, NET_BUFFER_DATA_LENGTH( pNb ), TRUE, NULL );
            }
            NT_ASSERT( NET_BUFFER_DATA_LENGTH( pNb ) == 0 );
            NT_ASSERT( NET_BUFFER_DATA_OFFSET( pNb ) == 0 );
            NT_ASSERT( NET_BUFFER_FIRST_MDL( pNb ) == NULL );
        }

        //
        // Now we can free the NBL and NBs, flags don't matter as there's no MDLs
        // NOTE: this will clear the Next pointer, so we cached it above
        // 
        NdisFreeCloneNetBufferList( pNbl, 0 );
    }

    TraceExit( 0 );
}


NDIS_STATUS CAdapter::Pause(
    _In_  PNDIS_MINIPORT_PAUSE_PARAMETERS MiniportPauseParameters 
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    

    TraceEntry();

    m_MiniportPauseJob->SetPauseParameters( MiniportPauseParameters );

    // Queue the m_MiniportPauseJob to do the pause work.
    ndisStatus = m_MiniportPauseJob->QueueSerialized();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to queue m_MiniportPauseJob->QueueSerialized job");
        goto Exit;
    }

    ndisStatus = NDIS_STATUS_PENDING;

    TraceExit(ndisStatus);

Exit:
    return ndisStatus;
}

// Surprise removal is the same as pause
// except we don't call NdisMComplete and 
NDIS_STATUS CAdapter::HandlePnPEventNotify(
    _In_  PNET_DEVICE_PNP_EVENT   NetDevicePnPEvent
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    
    CJobCompleteNotifier *pInitializeCompleteHandler = NULL;
    IEventContext *pCompleteEvent = NULL;
    NDIS_STATUS completeStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    if (m_MiniportDriver->GetNdisCharacteristics()->DevicePnPEventNotifyHandler)
    {
        WFCTrace("Invoking IHV's DevicePnPEvent handler for %d", NetDevicePnPEvent->DevicePnPEvent);    
        // First we notify the IHV component about the device PNP event
        m_MiniportDriver->GetNdisCharacteristics()->DevicePnPEventNotifyHandler(
            m_MiniportAdapterContext,
            NetDevicePnPEvent
            );
    }
    
    // If this is not surprise removal, we dont care
    if (NetDevicePnPEvent->DevicePnPEvent != NdisDevicePnPEventSurpriseRemoved)
    {
        WFCTrace("No additional processing necessary for DevicePnPEvent %d", NetDevicePnPEvent->DevicePnPEvent);    
        return NDIS_STATUS_SUCCESS;
    }

    InterlockedIncrement( &m_lSurpriseRemove );

    // Create the completion handler. We will wait on it
    pInitializeCompleteHandler = new CJobCompleteNotifier();
    if (pInitializeCompleteHandler == NULL)
    {
        WFCError("Failed to allocate HandleSurpriseRemoval complete notifier");
        ndisStatus = NDIS_STATUS_RESOURCES;
            goto Exit;
    }

    ndisStatus = IEventContext::CreateInstance(&pCompleteEvent);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize complete event (0x%x)", ndisStatus);
        goto Exit;
    }
    
    ndisStatus = pInitializeCompleteHandler->Initialize(
        &completeStatus,
        pCompleteEvent);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to initialize MiniportInitialize complete notifier");
            goto Exit;
    }

    // Queue the m_MiniportPauseJob to do the pause work.
    // This job is not synchronized with the rest of the queue
    // as there could be a job stuck due to a hang.
    // Since the only action being done is to pause the data, we don't need to synchronize
    ndisStatus = m_MiniportSurpriseRemovalJob->QueueJob(
        NetDevicePnPEvent,
        pInitializeCompleteHandler);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to queue m_MiniportSurpriseRemovalJob->QueueSerialized job");
        goto Exit;
    }

    // if we succeeded in queuing the job, the complete would clean itself up
    pInitializeCompleteHandler = NULL;
    
    // Wait for the job to complete. 
    #define MPSurpriseRemovalWaitMs 0 // Infinite wait
    if ( !pCompleteEvent->Wait( MPSurpriseRemovalWaitMs ))
    {
        //
        // Wait time out
        //
        ndisStatus = NTSTATUS2NDISSTATUS(STATUS_IO_TIMEOUT);
        DtErrMsg( "MPInit Timeout\n" );
        goto Exit;
    }
    // this sets the allow jobs  in case the command that got cleared was for power OID
    GetSerializedJobList()->set_AllowNewJobs(TRUE);    // allow jobs
    ndisStatus = completeStatus;



Exit:
    if (pCompleteEvent != nullptr)
    {
        delete pCompleteEvent;
    }
    if (pInitializeCompleteHandler != nullptr)
    {
        delete pInitializeCompleteHandler;
    }
    TraceExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CAdapter::Restart(
    _In_ PNDIS_MINIPORT_RESTART_PARAMETERS   MiniportRestartParameters
    )
{   
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;    

    TraceEntry();

    m_MiniportRestartJob->SetRestartParameters(MiniportRestartParameters);

    // Queue the m_MiniportRestartJob to do the pause work.
    ndisStatus = m_MiniportRestartJob->QueueSerialized();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to queue MiniportRestartJob->QueueSerialized job");
        goto Exit;
    }

    ndisStatus = NDIS_STATUS_PENDING;
    TraceExit(ndisStatus);

Exit:

    //
    //Test UpdateRrHistory-rto remove
    //
    //for ( int i=0; i < 6; i++ )
    //{
    //    __debugbreak();
    //    UpdateRrHistory();
    //}
    return ndisStatus;
}

void CAdapter::UpdateRrHistory()
/*++

    Update Rr history to persisten store (registry) so we can control
    run-away RR

--*/
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CRegistryHelper regHelper( GetMiniportAdapterHandle());
    ULONGLONG now = CSystem::get_CurrentTime();
    ULONGLONG sysTimes[RESET_RECOVERY_MAX+1];  // extra to hold new time, if needed
    ULONGLONG newTimes[RESET_RECOVERY_MAX];     // always trim <= max
    ULONG cb;
    ULONG entries;
    ULONG purges;
    ULONG newEntries;
    ULONG newCb;

    TraceEntry();
    //
    // record the history to control the frequency of RR
    //
    cb = sizeof( ULONGLONG ) * RESET_RECOVERY_MAX;
    ndisStatus = regHelper.ReadAdapterRegistryBinaries( REG_VALUE_RRHISTORY,
                &cb,
                (PBYTE)&sysTimes[0]);

    if ( ndisStatus != NDIS_STATUS_SUCCESS || cb == 0 || (cb % sizeof(ULONGLONG)))
    {
        //
        // not-present, 0 length or not multiples of sysTime. Thorw it away.
        //
        entries = 0;
    }
    else
    {
        entries = cb / sizeof(LARGE_INTEGER );
        NT_ASSERT( entries <= RESET_RECOVERY_MAX );
        if ( entries > RESET_RECOVERY_MAX )
        {
            entries = 0;
        }
    }

    //
    // Count entries older than 5 minutes to purge
    //
    for ( purges=0; purges < entries; purges++ )
    {
        if ( sysTimes[purges]+RESET_RECOVERY_PERIOD > now)
        {
            break;
        }
    }

    //
    // Keep at most RESET_RECOVERY_MAX entries.
    //
    sysTimes[entries] = now;               // last entry is now
    newEntries = entries - purges + 1;     // +1 for a new entry
    if ( newEntries > RESET_RECOVERY_MAX )
    {
        // must trim
        newEntries--; // table was full and purges is 0, dec by one for the new
        purges = 1; 
    }
    newCb = sizeof(ULONGLONG)*(newEntries);   // new total bytes
    
    RtlCopyMemory( (PVOID)&newTimes[0], (PVOID)&sysTimes[purges], newCb);

    //
    // update history
    //
    ndisStatus = regHelper.WriteAdapterRegistryBinaries( REG_VALUE_RRHISTORY,
                   newCb,
                   (PBYTE)&newTimes[0]);
    NT_ASSERT( ndisStatus == NDIS_STATUS_SUCCESS );

    TraceExit( ndisStatus );
}


VOID CAdapter::ReenumerateFailedAdapter()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    if (m_AdapterFailed == true)
    {
        goto exit;
    }
    m_AdapterFailed = true;

    //
    // Unconditionally allow jobs to flow, in case we happen to fall in Dx when we stop
    // job flow. This is needed because RR can happen in Dx and we need to allow
    // clean up jobs to flow, e.g. Reset, Pause, DeletePort, Halt ...
    //
    GetSerializedJobList()->set_AllowNewJobs(TRUE);

    //
    // Update the history to control the frequency of RR
    //
    UpdateRrHistory();

    //
    // finally do the deed
    //
    if (g_NdisMReenumerateFailedAdapter != NULL)
    {
        (*g_NdisMReenumerateFailedAdapter)(m_MiniportAdapterHandle);
    }

exit:
    TraceExit( ndisStatus );
}


NDIS_STATUS CAdapter::GetBandInformation(
    _In_ WDI_GET_ADAPTER_CAPABILITIES_PARAMETERS* pAdapterParams,
    _Inout_ PULONG  puNumBands,
    _Inout_ PULONG  puBytesWritten,
    _Inout_ PULONG  puBufferSize,
    _Out_writes_bytes_opt_(*puBufferSize) PDOT11_BAND_CAPABILITIES pBandCapabilities
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG i, j, uNumBands;
    ULONG uBufferSizeIn = 0, uBufferSizeNeeded = 0, uNextDataOffset = 0, uOffsetBytesNeeded;
    ULONG *pBuffer;
    PWDI_BAND_INFO_CONTAINER pBandInfo;
    PDOT11_BAND_DESCRIPTOR pBandDescriptor;

    if (pBandCapabilities)
    {
        uBufferSizeIn = *puBufferSize;
        uNextDataOffset = FIELD_OFFSET(DOT11_BAND_CAPABILITIES, BandDescriptors) +
                          (*puNumBands * sizeof(DOT11_BAND_DESCRIPTOR));
        if (uBufferSizeIn < uNextDataOffset)
        {
            status = NDIS_STATUS_INVALID_PARAMETER;
            WFCError("NumBands=%d, INVALID Min BufferSizeIn=%d < BufferSizeNeeded=%d",
                *puNumBands, uBufferSizeIn, uBufferSizeNeeded);
            goto Exit;
        }
    }

    uNumBands = 0;
    uBufferSizeNeeded = FIELD_OFFSET(DOT11_BAND_CAPABILITIES, BandDescriptors);
    for (i = 0; i < pAdapterParams->BandInfo.ElementCount; i++)
    {
        pBandInfo = &pAdapterParams->BandInfo.pElements[i];

        uOffsetBytesNeeded = (pBandInfo->ValidPhyTypes.ElementCount * sizeof(ULONG)) +
                             (pBandInfo->ChannelWidthList.ElementCount * sizeof(ULONG)) +
                             (pBandInfo->ValidChannelTypes.ElementCount * sizeof(ULONG));
        uBufferSizeNeeded += (sizeof(DOT11_BAND_DESCRIPTOR) + uOffsetBytesNeeded);

        if (pBandCapabilities)
        {
            if ((*puNumBands < uNumBands) ||
                (uBufferSizeIn < FIELD_OFFSET( DOT11_BAND_CAPABILITIES, BandDescriptors ) +
                    (uNumBands * sizeof( DOT11_BAND_DESCRIPTOR ))))
            {
                status = NDIS_STATUS_INVALID_PARAMETER;
                WFCError( "\tInvalid BufferSizeIn=%d/NumBandsIn=%u < NumBandsNeeded=%d",
                    uBufferSizeIn, *puNumBands, uNumBands );
                goto Exit;
            }
            if (uBufferSizeIn < (uNextDataOffset + uOffsetBytesNeeded))
            {
                status = NDIS_STATUS_INVALID_PARAMETER;
                WFCError("\t[%d] Invalid BufferSizeIn=%d < BufferSizeNeeded=%d",
                    uNumBands, uBufferSizeIn, uBufferSizeNeeded);
                goto Exit;
            }

            pBandDescriptor = &pBandCapabilities->BandDescriptors[uNumBands];

            //
            // Get Band Common Frequency
            //
            pBandDescriptor->uBandID = pBandInfo->BandCapabilities.BandID;
            switch (pBandInfo->BandCapabilities.BandID)
            {
                case (WDI_BAND_ID_2400):
                {
                    pBandDescriptor->uBandCommonFrequency = WDI_BAND_FREQUENCY_2400;
                    break;
                }

                case (WDI_BAND_ID_5000):
                {
                    pBandDescriptor->uBandCommonFrequency = WDI_BAND_FREQUENCY_5000;
                    break;
                }

                case (WDI_BAND_ID_60000):
                {
                    pBandDescriptor->uBandCommonFrequency = WDI_BAND_FREQUENCY_60000;
                    break;
                }

                case (WDI_BAND_ID_900):
                {
                    pBandDescriptor->uBandCommonFrequency = WDI_BAND_FREQUENCY_900;
                    break;
                }

                default:
                {
                    pBandDescriptor->uBandCommonFrequency = 0;
                    WFCError("\t[%d] WARNING - Unknown BandID=%d\n", uNumBands, pBandInfo->BandCapabilities.BandID);
                }
            }

            pBandDescriptor->BandStateActive = pBandInfo->BandCapabilities.BandState ?
                dot11_tri_state_true : dot11_tri_state_false;

            C_ASSERT (sizeof(ULONG) == sizeof(UINT32));

            pBandDescriptor->uPhyTypeOffset = uNextDataOffset;
            pBandDescriptor->uPhyTypeCount = pBandInfo->ValidPhyTypes.ElementCount;
            RtlCopyMemory(
                (((PBYTE)pBandCapabilities) + uNextDataOffset),
                pBandInfo->ValidPhyTypes.pElements,
                (pBandInfo->ValidPhyTypes.ElementCount * sizeof(ULONG)));
            uNextDataOffset += (pBandInfo->ValidPhyTypes.ElementCount * sizeof(ULONG));

            pBandDescriptor->uChannelWidthOffset = uNextDataOffset;
            pBandDescriptor->uChannelWidthCount = pBandInfo->ChannelWidthList.ElementCount;
            RtlCopyMemory(
                (((PBYTE)pBandCapabilities) + uNextDataOffset),
                pBandInfo->ChannelWidthList.pElements,
                (pBandInfo->ChannelWidthList.ElementCount * sizeof(ULONG)));
            uNextDataOffset += (pBandInfo->ChannelWidthList.ElementCount * sizeof(ULONG));

            pBandDescriptor->uChannelFrequenciesOffset = uNextDataOffset;
            pBandDescriptor->uChannelFrequenciesCount = pBandInfo->ValidChannelTypes.ElementCount;
            pBuffer = (PULONG) (((PBYTE)pBandCapabilities) + uNextDataOffset);
            for (j=0; j<pBandInfo->ValidChannelTypes.ElementCount; j++)
            {
                pBuffer[j] = pBandInfo->ValidChannelTypes.pElements[j].ChannelCenterFrequency;
            }
            uNextDataOffset += (pBandInfo->ValidChannelTypes.ElementCount * sizeof(ULONG));
        }

        uNumBands++;
    }

    *puNumBands = uNumBands;
    *puBufferSize = uBufferSizeNeeded;
    if (pBandCapabilities)
    {
        pBandCapabilities->uBandCount = uNumBands;
        pBandCapabilities->uTotalSize = uBufferSizeNeeded;

        ASSERT (uBufferSizeNeeded == uNextDataOffset);
        *puBytesWritten = uNextDataOffset;
    }

Exit:
    return status;
}


NDIS_STATUS CAdapter::SaveEnhancedCapabilities(
    _In_ WDI_GET_ADAPTER_CAPABILITIES_PARAMETERS* pAdapterParams)
{
    NDIS_STATUS status;
    ULONG uBytes, uNumBands, uBandsSize, uBytesWritten;
    DOT11_ADAPTER_CAPABILITIES  AdapterCapabilities = { 0 };
    PDOT11_BAND_CAPABILITIES pBandCapabilities = NULL;
    CAdapterPropertyCache* propertyCache = GetAdapterPropertyCache();

    TraceEntry();

    //
    // First, save the Adapter Enhanced Capabilities
    //
    uBytes = min(pAdapterParams->InterfaceAttributes.FirmwareVersion.ElementCount,
        WDI_MAX_FIRMWARE_VERSION_LENGTH);
    RtlCopyMemory(&AdapterCapabilities.FirmwareVersion,
        pAdapterParams->InterfaceAttributes.FirmwareVersion.pElements,
        uBytes);
    AdapterCapabilities.uIhvWdiVersion =
        GetMiniportDriver()->GetWdiCharacteristics()->WdiVersion;
    AdapterCapabilities.uWindowsWdiVersion = WDI_VERSION_LATEST;

    AdapterCapabilities.uOpModesSupported = DOT11_OPERATION_MODE_EXTENSIBLE_STATION;
    AdapterCapabilities.PromiscuousModeSupported = dot11_tri_state_false;
    AdapterCapabilities.IBSSSupported = dot11_tri_state_false;
    AdapterCapabilities.DiversityAntennaSupported =
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.AntennaDiversity ?
            dot11_tri_state_true : dot11_tri_state_false;
    AdapterCapabilities.uNumRxSpatialStreams =
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.NumRxStreams;
    AdapterCapabilities.uNumTxSpatialStreams =
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.NumTxStreams;

    if (pAdapterParams->InterfaceAttributes.InterfaceCapabilities.ActionFramesSupported)
    {
        AdapterCapabilities.Dot11ActionFrameTxRxSupported = dot11_tri_state_true;
        AdapterCapabilities.Dot11k_NeighborReportSupported = dot11_tri_state_true;
        AdapterCapabilities.Dot11k_APChannelReportSupported = dot11_tri_state_true;
        AdapterCapabilities.ANQPServiceInformationDiscoverySupported = dot11_tri_state_true;
    }
    else
    {
        AdapterCapabilities.Dot11ActionFrameTxRxSupported = dot11_tri_state_false;
        AdapterCapabilities.Dot11k_NeighborReportSupported = dot11_tri_state_false;
        AdapterCapabilities.Dot11k_APChannelReportSupported = dot11_tri_state_false;
        AdapterCapabilities.ANQPServiceInformationDiscoverySupported = dot11_tri_state_false;
    }
    AdapterCapabilities.Dot11w_ProtectedMgmtFramesSupported =
        pAdapterParams->StationAttributes.StationCapabilities.MFPCapable ?
            dot11_tri_state_true : dot11_tri_state_false;

    //
    // Hotspot capabilities
    //
    AdapterCapabilities.HotspotHESSIDConnectionSupported =
        pAdapterParams->StationAttributes.StationCapabilities.HESSIDConnectionSupported ?
            dot11_tri_state_true : dot11_tri_state_false;

    //
    // P2P capabilities
    //
    AdapterCapabilities.uNumConcurrentChannelOperations =
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.NumChannels;
    if (pAdapterParams->Optional.P2PAttributes_IsPresent)
    {
        AdapterCapabilities.uOpModesSupported |= DOT11_OPERATION_MODE_WFD_DEVICE;
        if (pAdapterParams->P2PAttributes.P2PCapabilities.ConcurrentGOCount > 0)
        {
            AdapterCapabilities.uOpModesSupported |= DOT11_OPERATION_MODE_WFD_GROUP_OWNER;
        }
        if (pAdapterParams->P2PAttributes.P2PCapabilities.ConcurrentClientCount > 0)
        {
            AdapterCapabilities.uOpModesSupported |= DOT11_OPERATION_MODE_WFD_CLIENT;
        }

        AdapterCapabilities.uP2PConcurrentGOPortsCount =
            pAdapterParams->P2PAttributes.P2PCapabilities.ConcurrentGOCount;
        AdapterCapabilities.uP2PConcurrentClientPortsCount =
            pAdapterParams->P2PAttributes.P2PCapabilities.ConcurrentClientCount;
        AdapterCapabilities.uP2PMaxMobileAPClientsAllowed =
            pAdapterParams->P2PAttributes.P2PCapabilities.GOClientTableSize;

        AdapterCapabilities.P2PGOon5GHzBandSupported =
            pAdapterParams->P2PAttributes.P2PCapabilities.GOon5GHzBandSupported ?
                dot11_tri_state_true : dot11_tri_state_false;;

        //
        // Services Advertisement and Discovery
        //
        AdapterCapabilities.P2PPassiveListenAvailabilitySupported =
            pAdapterParams->P2PAttributes.P2PCapabilities.PassiveAvailabilityListenStateSupported ?
                dot11_tri_state_true : dot11_tri_state_false;
        AdapterCapabilities.P2PDeviceDiscoverySupported =
            pAdapterParams->P2PAttributes.P2PCapabilities.ServiceDiscoverySupported ?
                dot11_tri_state_true : dot11_tri_state_false;
        AdapterCapabilities.P2PServiceNameDiscoverySupported =
            pAdapterParams->P2PAttributes.P2PCapabilities.ServiceNameDiscoverySupported ?
                dot11_tri_state_true : dot11_tri_state_false;
        AdapterCapabilities.P2PServiceInformationDiscoverySupported =
            pAdapterParams->P2PAttributes.P2PCapabilities.ServiceInformationDiscoverySupported ?
                dot11_tri_state_true : dot11_tri_state_false;
        AdapterCapabilities.P2PBackgroundDiscoverySupported =
            pAdapterParams->P2PAttributes.P2PCapabilities.BackgroundDiscoverySupported ?
                dot11_tri_state_true : dot11_tri_state_false;

        AdapterCapabilities.ASP20ServiceNameDiscoverySupported =
            pAdapterParams->P2PAttributes.P2PCapabilities.ASP20ServiceNameDiscoverySupported ?
                dot11_tri_state_true : dot11_tri_state_false;
        AdapterCapabilities.ASP20ServiceInformationDiscoverySupported =
            pAdapterParams->P2PAttributes.P2PCapabilities.ASP20ServiceInformationDiscoverySupported ?
                dot11_tri_state_true : dot11_tri_state_false;

        AdapterCapabilities.uServiceNameAdvertisementsMaxBytesSupported =
            pAdapterParams->P2PAttributes.P2PCapabilities.ServiceNameAdvertisementsMaxBytesSupported;
        AdapterCapabilities.uServiceInformationAdvertisementsMaxBytesSupported =
            pAdapterParams->P2PAttributes.P2PCapabilities.ServiceInformationAdvertisementsMaxBytesSupported;
    }
    else
    {
        AdapterCapabilities.uP2PMaxMobileAPClientsAllowed = 0;
        AdapterCapabilities.P2PPassiveListenAvailabilitySupported = dot11_tri_state_false;
        AdapterCapabilities.P2PDeviceDiscoverySupported = dot11_tri_state_false;
        AdapterCapabilities.P2PServiceNameDiscoverySupported = dot11_tri_state_false;
        AdapterCapabilities.P2PServiceInformationDiscoverySupported = dot11_tri_state_false;
        AdapterCapabilities.P2PBackgroundDiscoverySupported = dot11_tri_state_false;
        AdapterCapabilities.ASP20ServiceNameDiscoverySupported = dot11_tri_state_false;
        AdapterCapabilities.ASP20ServiceInformationDiscoverySupported = dot11_tri_state_false;
        AdapterCapabilities.uServiceNameAdvertisementsMaxBytesSupported = 0;
        AdapterCapabilities.uServiceInformationAdvertisementsMaxBytesSupported = 0;
    }

    //
    // Other Capabilities
    //
    AdapterCapabilities.D0_NLOSupported = dot11_tri_state_false;
    AdapterCapabilities.Dx_StandbyNLOSupported = dot11_tri_state_unknown;    // Set in nwifi
    AdapterCapabilities.InstantConnectSupported = dot11_tri_state_unknown;   // Set in nwifi

    AdapterCapabilities.FIPSSupported =
        pAdapterParams->StationAttributes.StationCapabilities.HostFIPSModeImplemented ?
            dot11_tri_state_true : dot11_tri_state_false;
    AdapterCapabilities.eCSASupported =
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.Support_eCSA ?
            dot11_tri_state_true : dot11_tri_state_false;
    AdapterCapabilities.FunctionalLevelResetRecoverySupported =
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.SupportFLR ?
            dot11_tri_state_true : dot11_tri_state_false;
    AdapterCapabilities.PlatformLevelResetRecoverySupported =
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.SupportPLR ?
            dot11_tri_state_true : dot11_tri_state_false;

    //only report support for MAC randomization if NDIS version >= 6.50.  During registration there is a check
    // to protect against older versions of the OS so only the adjusted NDIS version needs to be checked here.
    if (!IS_DRIVER_VERSION_650_OR_ABOVE(GetMiniportDriver()->GetNdisCharacteristics()->MajorNdisVersion,
        GetMiniportDriver()->GetNdisCharacteristics()->MinorNdisVersion))
    {
        AdapterCapabilities.MACAddressRandomizationSupported = dot11_tri_state_false;
        WFCInfo("IHV miniport indicated support for MAC randomization of %d but due to the NDIS version WDI is reporting %d",
            pAdapterParams->InterfaceAttributes.InterfaceCapabilities.MACAddressRandomization,
            AdapterCapabilities.MACAddressRandomizationSupported);
    }
    else
    {
        AdapterCapabilities.MACAddressRandomizationSupported =
            pAdapterParams->InterfaceAttributes.InterfaceCapabilities.MACAddressRandomization ?
                dot11_tri_state_true : dot11_tri_state_false;
        RtlCopyMemory(&AdapterCapabilities.MACAddressRandomizationMask,
            &pAdapterParams->InterfaceAttributes.InterfaceCapabilities.MACAddressRandomizationMask,
            sizeof(AdapterCapabilities.MACAddressRandomizationMask));
    }

    C_ASSERT(WDI_BLUETOOTH_COEXISTENCE_MAX == DOT11_BLUETOOTH_COEXISTENCE_MAX);
    AdapterCapabilities.BluetoothCoexistenceSupport = (DOT11_BLUETOOTH_COEXISTENCE_SUPPORT)
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.BluetoothCoexistenceSupport;

    AdapterCapabilities.FastTransitionSupported =
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.FastTransitionSupported ?
            dot11_tri_state_true : dot11_tri_state_false;

    AdapterCapabilities.MU_MIMOSupported =
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.MU_MIMOSupported ?
            dot11_tri_state_true : dot11_tri_state_false;

    AdapterCapabilities.CannotSupportMiracastSink =
        pAdapterParams->InterfaceAttributes.InterfaceCapabilities.CannotSupportMiracastSink ?
            dot11_tri_state_true : dot11_tri_state_false;

    AdapterCapabilities.BSSTransitionSupported = pAdapterParams->InterfaceAttributes.InterfaceCapabilities.BSSTransitionSupported ?
        dot11_tri_state_true : dot11_tri_state_false;

    //
    // IP Docking capabilities
    //
    AdapterCapabilities.IPDockingCapable = pAdapterParams->InterfaceAttributes.InterfaceCapabilities.IPDockingSupported ?
            dot11_tri_state_true : dot11_tri_state_false;


    //
    // The following value will be filled in by the Msm module since it needs to query the Ihv module
    //
    AdapterCapabilities.IHVExtensibilityModuleConfigured = dot11_tri_state_unknown;

    status = propertyCache->SetPropertyBuffer(
                WfcAdapterPropertyEnhancedCapabilities,
                sizeof(DOT11_ADAPTER_CAPABILITIES),
                (PBYTE) &AdapterCapabilities);
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError( "SetPropertyBuffer[WfcAdapterPropertyEnhancedCapabilities] FAILed 0x(%x)", status);
        goto Exit;
    }

    //
    // Now, get the band information.  First, compute the length required
    //
    uNumBands = 0;
    uBandsSize = 0;
    status = GetBandInformation( pAdapterParams,
                                 &uNumBands,
                                 NULL,
                                 &uBandsSize,
                                 NULL );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError("GetBandInformation[1] FAILed - Error = 0x%08x", status);
        goto Exit;
    }

    pBandCapabilities = (PDOT11_BAND_CAPABILITIES) new BYTE[uBandsSize];
    if (!pBandCapabilities)
    {
        WFCError("FAILed to allocate %d bytes for BandCapabilities", uBandsSize);
        status = NDIS_STATUS_RESOURCES;
        goto Exit;
    }
    RtlZeroMemory(pBandCapabilities, uBandsSize);

    uBytesWritten = 0;
    status = GetBandInformation( pAdapterParams,
                                 &uNumBands,
                                 &uBytesWritten,
                                 &uBandsSize,
                                 pBandCapabilities);
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError("GetBandInformation[2] FAILed - Error = 0x%08x", status);
        goto Exit;
    }

    status = propertyCache->SetPropertyBuffer(
                WfcAdapterPropertyBandCapabilities,
                uBandsSize,
                (PBYTE) pBandCapabilities);
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError( "SetPropertyBuffer[WfcAdapterPropertyBandCapabilities] FAILed 0x(%x)", status);
        goto Exit;
    }

Exit:
    if (pBandCapabilities)
    {
        delete [] pBandCapabilities;
    }
    TraceExit(status);
    return status;
}


NDIS_STATUS CAdapter::SetFirmwareCapabilitiesProperties(
    _In_ WDI_GET_ADAPTER_CAPABILITIES_PARAMETERS* pAdapterCapabilities)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    if (pAdapterCapabilities->Optional.PmCapabilities_IsPresent)
    {
        //
        // Can't do straight memcpy. WDI_PM_CAPABILITIES_CONTAINER is
        // not the same as NDIS_PM_CAPABILITIES. The former is one header short.
        //
        RtlCopyMemory(&m_PmCapabilities.Flags,
                        &pAdapterCapabilities->PmCapabilities,
                        sizeof((pAdapterCapabilities->PmCapabilities)));
        m_PmCapabilities.Header.Type     = NDIS_OBJECT_TYPE_DEFAULT;
        m_PmCapabilities.Header.Revision = NDIS_PM_CAPABILITIES_REVISION_2;
        m_PmCapabilities.Header.Size     = NDIS_SIZEOF_NDIS_PM_CAPABILITIES_REVISION_2;
    }

    if (pAdapterCapabilities->Optional.DatapathAttributes_IsPresent)
    {
        RtlCopyMemory(&m_DatapathCapabilities, &pAdapterCapabilities->DatapathAttributes.DataPathCapabilities, sizeof((pAdapterCapabilities->DatapathAttributes.DataPathCapabilities)));
    }

    //
    // hardware assist receive filter capabilities
    //

    if (pAdapterCapabilities->Optional.ReceiveCoalescingCapabilities_IsPresent)
    {
        //
        // Can't do straight memcpy. WDI_RECEIVE_COALESCING_CAPABILITIES_CONTAINER is
        // not the same as NDIS_RECEIVE_FILTER_CAPABILITIES. The former is one header and one UINT32 flags short.
        //
        RtlCopyMemory(&m_ReceiveFilterCapabilities.EnabledFilterTypes,
                        &pAdapterCapabilities->ReceiveCoalescingCapabilities,
                        sizeof((pAdapterCapabilities->ReceiveCoalescingCapabilities)));
        m_ReceiveFilterCapabilities.Header.Type     = NDIS_OBJECT_TYPE_DEFAULT;
        m_ReceiveFilterCapabilities.Header.Revision = NDIS_RECEIVE_FILTER_CAPABILITIES_REVISION_2;
        m_ReceiveFilterCapabilities.Header.Size     = NDIS_SIZEOF_RECEIVE_FILTER_CAPABILITIES_REVISION_2;
        m_ReceiveFilterCapabilities.Flags = 0;
        m_ReceiveFilterCapabilities.NdisReserved = 0;
    }

    // populate firmware capabilities properties
    ndisStatus = m_AdapterPropertyCache.SetFirmwareCapabilitiesProperties(pAdapterCapabilities);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("m_AdapterPropertyCache.Initialize failed");
        goto Exit;
    }

    // 
    // Setup the BSS list appropriately
    //
    if (pAdapterCapabilities->StationAttributes.StationCapabilities.BSSListCachemanagement)
    {
        WFCInfo("[Scan] IHV driver would maintain BSS list. Switching BSS list behavior");
        m_ExtStaBSSList.SetBSSListCacheMode(true);
    }

    ndisStatus = SaveEnhancedCapabilities( pAdapterCapabilities );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("SaveEnhancedCapabilities FAILed - 0x%x", ndisStatus);
        goto Exit;
    }

Exit:

    TraceExit(ndisStatus);
    return ndisStatus;

}


CPortPropertyCache* CAdapter::GetPortPropertyCache(
    _In_ WFC_PORT_ID PortId
    )
{
    CPort* pPort = GetPortFromPortId(PortId);
    NT_ASSERT(pPort);

    return pPort->GetPropertyCache();
}

VOID 
CAdapter::CollectDebugData(_In_opt_ DeviceCommand *pDeviceCommand, _In_opt_ Task *pTask, _In_opt_ CJobBase *job)
{
    if (pDeviceCommand != NULL)
    {
        RtlCopyMemory(m_DebugHungDeviceCommand, pDeviceCommand, sizeof(DeviceCommand));
    }

    if (pTask != NULL)
    {
        RtlCopyMemory(m_DebugHungTask, pTask, sizeof(Task));
    }
    if (job != NULL)
    {
        RtlCopyMemory(m_DebugHungJob, job, sizeof(CJobBase));
    }
}

VOID 
CAdapter::CollectFirmwareDiagnosticData(
    _In_ ULONG                              BufferLength,
    _In_reads_bytes_(BufferLength)BYTE*     Buffer
    )
{
    ULONG length = MIN(BufferLength, MAX_DIAGNOSTIC_FIRMWARE_BUFFER_SIZE);
    RtlCopyMemory(m_DebugFirmwareBuffer, Buffer, length);
}

void 
CAdapter::DelayDeactivateAndFreePort()
{
    NDIS_STATUS ndisStatus;
    NDIS_PORT_NUMBER ndisPortNumber;
    int i;

    TraceEntry();

    for (;;)
    {
        m_ISpinLockDelayDeactivation->EnterCriticalSection();

        //
        // Service ports in the table
        //
        ndisPortNumber = NDIS_DEFAULT_PORT_NUMBER;

        for ( i=0; i < c_MaxPorts; i++ )
        {
            if ( m_NdisPortNumToDeactivate[i] != NDIS_DEFAULT_PORT_NUMBER )
            {
                ndisPortNumber = m_NdisPortNumToDeactivate[i];
                m_NdisPortNumToDeactivate[i] = 0;
                break;
            }
        }

        if ( ndisPortNumber == NDIS_DEFAULT_PORT_NUMBER )
        {
            m_bDeactivateWorkerActive = FALSE;
            m_ISpinLockDelayDeactivation->LeaveCriticalSection();
            break;
        }

        m_ISpinLockDelayDeactivation->LeaveCriticalSection();

        //
        //1. deactivate ndis port
        //
        //++ this block is copied from CDeletePortJob method
        //
        NET_PNP_EVENT_NOTIFICATION  netPnpEventNotification;
        NDIS_PORT_NUMBER            portNumberArray[1];

        NdisZeroMemory(&netPnpEventNotification, sizeof(NET_PNP_EVENT_NOTIFICATION));

        ASSIGN_NDIS_OBJECT_HEADER(netPnpEventNotification.Header, 
                                NDIS_OBJECT_TYPE_DEFAULT,
                                NET_PNP_EVENT_NOTIFICATION_REVISION_1, 
                                sizeof(NET_PNP_EVENT_NOTIFICATION));

        netPnpEventNotification.NetPnPEvent.NetEvent = NetEventPortDeactivation;

        // We need to save a pointer to the NDIS_PORT_NUMBER in the NetPnPEvent::Buffer field
        portNumberArray[0] = ndisPortNumber;
        netPnpEventNotification.NetPnPEvent.Buffer = (PVOID)portNumberArray;
        netPnpEventNotification.NetPnPEvent.BufferLength = sizeof(NDIS_PORT_NUMBER);

        ndisStatus = NdisMNetPnPEvent (m_MiniportAdapterHandle,
                                    &netPnpEventNotification);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError ("Failed to deactivate NDIS port %d. Status = 0x%08x\n", 
                         ndisPortNumber, ndisStatus);
        }
        else
        {
            WFCInfo ("Deactivated Port Number %d\n", ndisPortNumber);
        }

        //2. Free ndis port
        ndisStatus = NdisMFreePort (m_MiniportAdapterHandle,
                                    ndisPortNumber);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("this(%p). NdisMFreePort() returned = 0x%x. m_DeleteNdisPortNumber = %u", 
                        this, 
                        ndisStatus, 
                        ndisPortNumber);
        }
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}

void 
CAdapter::DelayActivatePort()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_PORT_NUMBER ndisPortNumber;
    int i;

    TraceEntry();

    for (;;)
    {
        m_ISpinLockDelayActivation->EnterCriticalSection();

        //
        // Service ports in the table
        //
        ndisPortNumber = NDIS_DEFAULT_PORT_NUMBER;
        
        for ( i=0; i < c_MaxPorts; i++ )
        {
            if ( m_NdisPortNumToActivate[i] != NDIS_DEFAULT_PORT_NUMBER )
            {
                ndisPortNumber = m_NdisPortNumToActivate[i];
                m_NdisPortNumToActivate[i] = 0;
                break;
            }
        }

        if ( ndisPortNumber == NDIS_DEFAULT_PORT_NUMBER )
        {
            m_bActivateWorkerActive = FALSE;
            m_ISpinLockDelayActivation->LeaveCriticalSection();
            break;
        }

        m_ISpinLockDelayActivation->LeaveCriticalSection();

        NET_PNP_EVENT_NOTIFICATION  netPnpEventNotification;
        NDIS_PORT                   ndisPort;
        PNDIS_PORT_CHARACTERISTICS  portChar;

        NdisZeroMemory(&netPnpEventNotification, sizeof(NET_PNP_EVENT_NOTIFICATION));
        NdisZeroMemory(&ndisPort, sizeof(NDIS_PORT));

        ASSIGN_NDIS_OBJECT_HEADER(netPnpEventNotification.Header, NDIS_OBJECT_TYPE_DEFAULT,
            NET_PNP_EVENT_NOTIFICATION_REVISION_1, sizeof(NET_PNP_EVENT_NOTIFICATION));

        netPnpEventNotification.NetPnPEvent.NetEvent = NetEventPortActivation;

        // Refill the characteristics structure for the port
        portChar = &(ndisPort.PortCharacteristics);
        ASSIGN_NDIS_OBJECT_HEADER(portChar->Header, NDIS_OBJECT_TYPE_DEFAULT,
            NDIS_PORT_CHARACTERISTICS_REVISION_1, sizeof(NDIS_PORT_CHARACTERISTICS));

        portChar->Flags = NDIS_PORT_CHAR_USE_DEFAULT_AUTH_SETTINGS;
        portChar->Type = NdisPortTypeUndefined;
        portChar->MediaConnectState = MediaConnectStateConnected;
        portChar->XmitLinkSpeed = NDIS_LINK_SPEED_UNKNOWN;
        portChar->RcvLinkSpeed = NDIS_LINK_SPEED_UNKNOWN;
        portChar->Direction = NET_IF_DIRECTION_SENDRECEIVE;
        //
        // TODO: check these 4 lines!
        //
        portChar->SendControlState = NdisPortControlStateUnknown;
        portChar->RcvControlState = NdisPortControlStateUnknown;
        portChar->SendAuthorizationState = NdisPortAuthorizationUnknown; // Ignored
        portChar->RcvAuthorizationState = NdisPortAuthorizationUnknown; // Ignored
        portChar->PortNumber = ndisPortNumber;

        // Single port is being activated
        ndisPort.Next = NULL;

        // We need to save a pointer to the NDIS_PORT in the NetPnPEvent::Buffer field
        netPnpEventNotification.NetPnPEvent.Buffer = (PVOID)&ndisPort;
        netPnpEventNotification.NetPnPEvent.BufferLength = sizeof(NDIS_PORT);

        ndisStatus = NdisMNetPnPEvent(m_MiniportAdapterHandle, 
                                    &netPnpEventNotification);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError ("Failed to activate NDIS port %d. Status = 0x%08x\n", 
                ndisPortNumber, ndisStatus);
        }
        else
        {
            WFCInfo ("Activated Port Number %d\n", ndisPortNumber);
        }
    }

    TraceExit(NDIS_STATUS_SUCCESS);
}

void CAdapter::InitializeNPagedLookaside(
    _In_ ULONG ulSize,
    _In_ PNPAGED_LOOKASIDE_LIST pLookaside)
/*++
    This function initializes a NDIS lookaside list
    Previously WDF lookaside lists were used however the overhead was measurable (especially on Rx path) and using NDIS lookaside list
    improved performance.  WDF lookaside lists have some nice properties (parents/refcounts) however for datapath they are not giving a benefit
    as WDI Rx and Tx paths will both keep track of packets (and therefore WFC Frames) on their own.  NDIS lookaside lists are used for the lower overhead.
--*/
{
    TraceEntry();

    //Initialize NDIS NPagedLookasideList
    NdisInitializeNPagedLookasideList(pLookaside,
        NULL,
        NULL,
        0,
        ulSize,
        WDI_TXRX_POOL_TAG, //Pool tag
        0);

    TraceExit(0);

}

__inline PWFC_FRAME CAdapter::AllocateWfcFrame()
{
    PWFC_FRAME pWfcFrame = reinterpret_cast<WFC_FRAME *>(NdisAllocateFromNPagedLookasideList(&m_WfcFrameLookaside));
    if (pWfcFrame)
    {
        RtlZeroMemory(pWfcFrame, m_WfcFrameSize);
        pWfcFrame->ulSizeOfThis = m_WfcFrameSize;
    }
    else
    {
        WFCError("Unable to allocate WfcFrame");
    }
    return pWfcFrame;
}

__inline void CAdapter::FreeWfcFrame(
    _In_ PWFC_FRAME pWfcFrame)
{
    TraceEntry();

    if (pWfcFrame)
    {
        NdisFreeToNPagedLookasideList(&m_WfcFrameLookaside, pWfcFrame);
    }
    else
    {
        NT_ASSERTMSG("FreeWfcFrame NULL pointer!", pWfcFrame != NULL);
        goto exit;
    }
exit:
    TraceExit(0);
}
