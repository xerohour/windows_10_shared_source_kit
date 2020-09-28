/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

Dot11Oids.cpp

Abstract:

Implementation for Dot11 OID request handler

Environment:

Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "Dot11Oids.tmh"

NDIS_STATUS CDummySetJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    UNREFERENCED_PARAMETER(pInformationBuffer);
    UNREFERENCED_PARAMETER(pBytesNeeded);

    WFCActError(
        "Warning: Auto-completing dummy set for OID %x.",
        m_pOidRequest->DATA.QUERY_INFORMATION.Oid);

    *pBytesRead = InformationBufferLength;

    return NDIS_STATUS_SUCCESS;
}

//==============================================================================================
NDIS_STATUS CDummyQueryReturnTrueJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    BOOLEAN* pBool = (BOOLEAN*)pInformationBuffer;

    *pBytesWritten = 0;
    *pBytesNeeded = sizeof(BOOLEAN);

    if (InformationBufferLength < *pBytesNeeded)
    {
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    //
    // TODO: This is a stub. Need to actually implement this.
    //
    *pBool = TRUE;
    *pBytesWritten = *pBytesNeeded;

    return NDIS_STATUS_SUCCESS;
}

//==============================================================================================
//
//++ CSetOperationModeJob : job to set operation mode
//
//
//++ CSetOperationModeJob   implementation =====
//
NDIS_STATUS CSetOperationModeJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS     ndisStatus = NDIS_STATUS_SUCCESS;

    TraceEntry();

    if ( SET_OID_BUFFER_LENGTH(pOidRequest) < sizeof(DOT11_CURRENT_OPERATION_MODE) )
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto Exit;
    }

    PDOT11_CURRENT_OPERATION_MODE pOpMode = ((PDOT11_CURRENT_OPERATION_MODE)pOidRequest->DATA.SET_INFORMATION.InformationBuffer);
    if (pOpMode->uCurrentOpMode & (DOT11_OPERATION_MODE_AP | DOT11_OPERATION_MODE_EXTENSIBLE_AP | DOT11_OPERATION_MODE_NETWORK_MONITOR | DOT11_OPERATION_MODE_MANUFACTURING))
    {
        ndisStatus = NDIS_STATUS_BAD_VERSION;
        WFCActError( "Unsupported Dot11OpMode mask %x",
            pOpMode->uCurrentOpMode );
        goto Exit;
    }

    m_OpmodeMask = CDot11ToWabiConverter::MapOperationMode( pOpMode );
    if (0 == m_OpmodeMask)
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        WFCActError( "Invalid Dot11OpMode mask %x", pOpMode->uCurrentOpMode );
        goto Exit;
    }

Exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}



VOID CSetOperationModeJob::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceEntry();
    TraceExit(NdisStatus);
}

VOID CSetOperationModeJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);

    TraceActExit( NdisStatus );
}


void CSetOperationModeJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CPort *pPort;
    ULONG uOpmodeMask;
    DeviceCommand *pDeviceCommand;
    WDI_TASK_CHANGE_OPERATION_MODE_PARAMETERS changeOperationModeParameters;

    TraceActEntry();

    pPort = m_pAdapter->GetPortFromNdisPortNumber(get_NdisPortNumber());

    //
    // Check for the following:
    //  Is the Adapter already in this mode (i.e. a dup) ?
    //  Is this a STA port ? If this is not a STA, do not allow the operation mode to be changed to STA
    //
    status = GetPortPropertyCache()->GetPropertyULong(WfcPortCurrentOperationalMode, &uOpmodeMask);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to find WfcPortCurrentOperationalMode property in port Property Cache. Error = 0x%08x",
            status);
        goto CompleteJob;
    }

    if (m_OpmodeMask == (WDI_OPERATION_MODE) uOpmodeMask)
    {
        WFCActInfo("Not changing Opmode=%x for PortType=%d as it is a duplicate notification",
            m_OpmodeMask, pPort->GetPortType());
        goto CompleteJob;
    }

    //
    // Currently, the driver crashes if the STAPort is set to STA mode, se need to work around it for now
    //
#ifdef  DRIVER_BUG_PENDING
    if ((pPort->GetPortType() != WfcPortTypeExtSTA) &&
       (m_OpmodeMask & WDI_OPERATION_MODE_STA))
#else
    if (m_OpmodeMask & WDI_OPERATION_MODE_STA)
#endif // DRIVER_BUG_PENDING
    {
        WFCActInfo("Not calling driver to change Opmode to 0x%x=STA for PortType <%d>\n",
            m_OpmodeMask, pPort->GetPortType());
        status = GetPortPropertyCache()->SetPropertyULong(WfcPortCurrentOperationalMode, m_OpmodeMask);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Unable to set WfcPortCurrentOperationalMode property in port Property Cache. Error = 0x%08x",
                status);
            goto CompleteJob;
        }

        m_OpmodeMask = (WDI_OPERATION_MODE) uOpmodeMask;

        goto CompleteJob;
    }

   //
   // initialize task
   //
   status = m_Task.Initialize(
               m_pAdapter->GetNotificationManager(),
               m_pAdapter->GetDeviceCommandScheduler(),
               m_pAdapter->GetEventQueue());
   if ( status != NDIS_STATUS_SUCCESS )
   {
       WFCActErrorStatus("Failed to initialize OperationMode task.");
       goto CompleteJob;
   }

    //
    // init task and  command
    //
    status = m_Task.get_TaskDeviceCommand( &pDeviceCommand );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't get device command for OperationMode task.");
        goto CompleteJob;
    }

    WFCActInfo("Changing Opmode to 0x%x for PortType <%d>\n", m_OpmodeMask, pPort->GetPortType());

    changeOperationModeParameters.OperationMode = m_OpmodeMask;
    status = GenerateWdiTaskChangeOperationMode(
        &changeOperationModeParameters,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_TlvLength,
        &m_pTlvBuffer
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to generate Change Operation Mode params tlv. Error = 0x%08x", status);
        goto CompleteJob;
    }

    status = pDeviceCommand->Initialize(
            get_PortId(),
            WDI_TASK_CHANGE_OPERATION_MODE,
            m_TlvLength,
            m_pTlvBuffer );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize Device Command for OperationMode 0x%x", status);
        goto CompleteJob;
    }

    //
    // rock and roll
    //
    status = StartTask( &m_Task );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't start OperationMode task.");
        goto CompleteJob;
    }

    goto Exit;

CompleteJob:
    CompleteJob( status );

Exit:
    TraceActExit(status);
}


void CSetOperationModeJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    CPort * pPort;

    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);

    TraceActEntry();

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Previous job failed, error = 0x%08x", OperationStatus);
        goto Exit;
    }

    pPort = m_pAdapter->GetPortFromNdisPortNumber(get_NdisPortNumber());
    OperationStatus = GetPortPropertyCache()->SetPropertyULong(WfcPortCurrentOperationalMode, m_OpmodeMask);
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to set WfcPortCurrentOperationalMode property in port Property Cache. Error = 0x%08x",
            OperationStatus);
        goto Exit;
    }

    m_pAdapter->m_MiniportDataHandlers.TalTxRxSetPortOpModeHandler(m_pAdapter->GetMiniportTalTxRxContext(),
        get_PortId(),
        m_OpmodeMask);

    WFCActInfo("Set datapath port opmode = 0x%08x for PortType=%d", m_OpmodeMask, pPort->GetPortType());

Exit:
    CompleteJob(OperationStatus);
    TraceActExit( OperationStatus );
}

void CSetOperationModeJob::OnJobCancelled()
{
    // We are not cancellable

    TraceActEntry();
    TraceActExit( STATUS_SUCCESS );
}


NDIS_STATUS CQueryOperationModeJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded)
{
    UNREFERENCED_PARAMETER(pBytesNeeded);
    UNREFERENCED_PARAMETER(InformationBufferLength);

    NDIS_STATUS status;
    ULONG uOpmodeMask;
    ULONG uNdisOpmodeMask = 0;
    PDOT11_CURRENT_OPERATION_MODE pDot11CurrentOperationMode;

    TraceActEntry();

    status = GetPortPropertyCache()->GetPropertyULong(WfcPortCurrentOperationalMode,
        &uOpmodeMask);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to find WfcPortCurrentOperationalMode property in port Property Cache. Error = 0x%08x", status);
        goto exit;
    }

    if ((uOpmodeMask & WDI_OPERATION_MODE_STA) != 0)
    {
        uNdisOpmodeMask |= DOT11_OPERATION_MODE_EXTENSIBLE_STATION;
    }

    if ((uOpmodeMask & WDI_OPERATION_MODE_P2P_DEVICE) != 0)
    {
        uNdisOpmodeMask |= DOT11_OPERATION_MODE_WFD_DEVICE;
    }

    if ((uOpmodeMask & WDI_OPERATION_MODE_P2P_GO) != 0)
    {
        uNdisOpmodeMask |= DOT11_OPERATION_MODE_WFD_GROUP_OWNER;
    }

    if ((uOpmodeMask & WDI_OPERATION_MODE_P2P_CLIENT) != 0)
    {
        uNdisOpmodeMask |= DOT11_OPERATION_MODE_WFD_CLIENT;
    }

    // Size is already verified 
    pDot11CurrentOperationMode = PDOT11_CURRENT_OPERATION_MODE(pInformationBuffer);

    RtlZeroMemory(pDot11CurrentOperationMode, sizeof(*pDot11CurrentOperationMode));

    NdisMoveMemory(&(pDot11CurrentOperationMode->uCurrentOpMode), &(uNdisOpmodeMask), sizeof(ULONG));

    *pBytesWritten = sizeof(DOT11_CURRENT_OPERATION_MODE);

    WFCActInfo("reporting 0x%08x", uNdisOpmodeMask);


exit:

    return status;
}

//==============================================================================================
//
//++ CSetDesirerdBssTypeJob : job to set desired BSS type
//
//
//++ CSetDesirerdBssTypeJob   implementation =====
//
NDIS_STATUS CSetDesirerdBssTypeJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    // We disallow dot11_BSS_type_independent (IBSS)
    // It's right int he middle of the enum, so the normal min/max
    // range checks won't suffice
    if ((InformationBufferLength >= sizeof( ULONG )) &&
        ((*((ULONG*)pInformationBuffer) == dot11_BSS_type_independent)))
    {
        status = NDIS_STATUS_NOT_SUPPORTED;
        WFCActError( "dot11_BSS_type_independent not supported" );
        goto exit;
    }

    // Call the base to do all the real work
    status = CSimplePropertyCacheULongSetOidJob::SetData( pInformationBuffer, InformationBufferLength, pBytesRead, pBytesNeeded );

exit:
    TraceActExit( status );
    return status;
}

//==============================================================================================

NDIS_STATUS CQueryOperationalRateSet::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    DOT11_RATE_SET* pRateSet = (DOT11_RATE_SET*)pInformationBuffer;

    *pBytesWritten = 0;
    *pBytesNeeded = FIELD_OFFSET(DOT11_RATE_SET, ucRateSet) + 1;

    if (InformationBufferLength < *pBytesNeeded)
    {
        return NDIS_STATUS_BUFFER_OVERFLOW;
    }

    //
    // TODO: This is a stub. Need to actually implement this.
    //
    pRateSet->uRateSetLength = 1;
    pRateSet->ucRateSet[0] = 0;

    *pBytesWritten = *pBytesNeeded;

    return NDIS_STATUS_SUCCESS;
}

//==============================================================================================
NDIS_STATUS CSetPrivacyExemptionListJob::GetDeviceCommandData(
    _In_ PVOID             pInformationBuffer,
    _In_ UINT              InformationBufferLength,
    _Inout_ PUINT             pBytesRead,
    _Inout_ PUINT             pBytesNeeded,
    _In_ UINT              ReservedHeaderLength,
    _Out_ ULONG*           pCommandId,
    _Out_ ULONG*           pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE**           pCommandBuffer)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_PRIVACY_EXEMPTION_LIST   pExemptionList;
    ULONG i;

    WDI_SET_PRIVACY_EXEMPTION_LIST_PARAMETERS privacyExemptionListParameters;
    RtlZeroMemory(&privacyExemptionListParameters, sizeof(privacyExemptionListParameters));

    TraceActEntry();

    *pBytesRead = 0;
    *pBytesNeeded = 0;

    pExemptionList = (PDOT11_PRIVACY_EXEMPTION_LIST)pInformationBuffer;

    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pExemptionList->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_PRIVACY_EXEMPTION_LIST_REVISION_1,
        sizeof(DOT11_PRIVACY_EXEMPTION_LIST)))
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid NDIS object header. Expected (T:%d R:%d S:%d) Received (T:%d R:%d S:%d)",
            NDIS_OBJECT_TYPE_DEFAULT, DOT11_PRIVACY_EXEMPTION_LIST_REVISION_1,
            sizeof(DOT11_PRIVACY_EXEMPTION_LIST), pExemptionList->Header.Type,
            pExemptionList->Header.Revision, pExemptionList->Header.Size
            );

        goto exit;
    }

    *pBytesNeeded = FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries) +
        pExemptionList->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION);
    if (InformationBufferLength < (*pBytesNeeded))
    {
        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
        WFCActError("Insufficient data. Expected %d Received %d",
            *pBytesNeeded, InformationBufferLength);
        goto exit;
    }
    privacyExemptionListParameters.PrivacyExemptionEntry.ElementCount = pExemptionList->uNumOfEntries;
    if (pExemptionList->uNumOfEntries != 0)
    {
        privacyExemptionListParameters.Optional.PrivacyExemptionEntry_IsPresent = true;
        privacyExemptionListParameters.PrivacyExemptionEntry.AllocateElements(
            privacyExemptionListParameters.PrivacyExemptionEntry.ElementCount,
            0);

        if (privacyExemptionListParameters.PrivacyExemptionEntry.pElements == NULL)
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            WFCActError("Failed to allocate memory for the WFC exemption list entries");
            goto exit;
        }
    }

    *pBytesRead = *pBytesNeeded;
    *pCommandId = WDI_SET_PRIVACY_EXEMPTION_LIST;

    for (i = 0; i < pExemptionList->uNumOfEntries; i++)
    {
        privacyExemptionListParameters.PrivacyExemptionEntry.pElements[i].EtherType = pExemptionList->PrivacyExemptionEntries[i].usEtherType;
        privacyExemptionListParameters.PrivacyExemptionEntry.pElements[i].ExemptionActionType =
            CDot11ToWabiConverter::MapExemptionActionType(
            pExemptionList->PrivacyExemptionEntries[i].usExemptionActionType
            );
        privacyExemptionListParameters.PrivacyExemptionEntry.pElements[i].ExemptionPacketType =
            CDot11ToWabiConverter::MapExemptionPacketType(
            pExemptionList->PrivacyExemptionEntries[i].usExemptionPacketType
            );

    }

    ndisStatus = GenerateWdiSetPrivacyExemptionList(
        &privacyExemptionListParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer
        );

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to generate privacy exemption params tlv. Error = 0x%08x", ndisStatus);
        goto exit;
    }

exit:
    TraceActExit(ndisStatus);
    return ndisStatus;
}


NDIS_STATUS CSetPrivacyExemptionListJob::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader)
{
    UNREFERENCED_PARAMETER(MessageLength);
    UNREFERENCED_PARAMETER(pMessageHeader);

    if (OperationStatus == NDIS_STATUS_SUCCESS)
    {
        PDOT11_PRIVACY_EXEMPTION_LIST pExemptionList
            = (PDOT11_PRIVACY_EXEMPTION_LIST)m_pInformationBuffer;

        DtPrint( "caching PrivacyExemption listCount=%d\n", pExemptionList->uNumOfEntries );
        for ( UINT16 i=0; i < pExemptionList->uNumOfEntries;i++ )
        {
            WFCInfo( "Caching [%d] EtherType=0x%4x ActionType=0x%4x PacketType=0x%4x ",
                i, 
                pExemptionList->PrivacyExemptionEntries[i].usEtherType,
                pExemptionList->PrivacyExemptionEntries[i].usExemptionActionType,
                pExemptionList->PrivacyExemptionEntries[i].usExemptionPacketType );
        }

        GetPortPropertyCache()->SetPropertyList(
            (WFC_PROPERTY_NAME)WfcPortPropertyNamePrivacyExemptionList,
            pExemptionList->uNumOfEntries * sizeof(DOT11_PRIVACY_EXEMPTION),
            (UINT16)pExemptionList->uNumOfEntries,
            (BYTE *)&(pExemptionList->PrivacyExemptionEntries[0]));
    }
    else
    {
        WFCError("WORKAROUND: HCK: Fake succeeding PRIVACY_EXEMPTION_LIST set (Original Failure = 0x%08x)", OperationStatus);
        OperationStatus = NDIS_STATUS_SUCCESS;
    }

    return OperationStatus;
}
//==============================================================================================
VOID CSetDefaultKeyJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus )
/*++

    Overwrite so we can cache default keys

--*/
{
    PNDIS_OID_REQUEST pOidRequest = GetOidRequest();
    PDOT11_CIPHER_DEFAULT_KEY_VALUE pDefaultKeyValue;
    PDOT11_CIPHER_DEFAULT_KEY_VALUE pMyKeyValue;
    ULONG  ulBuffer = SET_OID_BUFFER_LENGTH( pOidRequest );
    DOT11_CIPHER_ALGORITHM algorithmId;
    ULONG keyIndex;

    //
    // caching if default port and WEP
    //
    pDefaultKeyValue = ( PDOT11_CIPHER_DEFAULT_KEY_VALUE )SET_OID_BUFFER( pOidRequest );
    ASSERT ( NdisStatus == NDIS_STATUS_SUCCESS );
    ASSERT ( ulBuffer >= sizeof(DOT11_CIPHER_DEFAULT_KEY_VALUE)); // assume key length > 0
    algorithmId = pDefaultKeyValue->AlgorithmId;
    keyIndex =  pDefaultKeyValue->uKeyIndex;

    if ( get_NdisPortNumber() == NDIS_DEFAULT_PORT_NUMBER && 
        keyIndex < MAX_DEFAULT_KEYS &&
        ( algorithmId == DOT11_CIPHER_ALGO_WEP || 
          algorithmId == DOT11_CIPHER_ALGO_WEP40 ||
          algorithmId == DOT11_CIPHER_ALGO_WEP104 ) )
    {
        if ( pDefaultKeyValue->bDelete == FALSE )
        {
            //++
            // Remember for Resume from hibernation; check if the slot is occupied
            //
            pMyKeyValue = m_pAdapter->m_DefaultKeyValues[keyIndex];
            if ( pMyKeyValue != NULL )
            {
                //
                // slot is used; free it first
                //
                DtPrint( "Default key[%d] is NOT empty\n", keyIndex);
                WFCInfo( "Default key[%d] is NOT empty", keyIndex);
                RtlSecureZeroMemory( pMyKeyValue, m_pAdapter->m_DefaultKeyLengths[keyIndex]);
                ExFreePoolWithTag( pMyKeyValue, WDI_POOL_TAG_KEY );
                m_pAdapter->m_DefaultKeyValues[keyIndex] = NULL;
            }

            pMyKeyValue = (PDOT11_CIPHER_DEFAULT_KEY_VALUE)ExAllocatePoolWithTag( NonPagedPool,
                                                ulBuffer,
                                                WDI_POOL_TAG_KEY);
            if ( pMyKeyValue )
            {
                RtlCopyMemory( (PVOID)pMyKeyValue, 
                            SET_OID_BUFFER( pOidRequest ),
                            ulBuffer );

                m_pAdapter->m_DefaultKeyValues[keyIndex] = pMyKeyValue;
                m_pAdapter->m_DefaultKeyLengths[keyIndex] = ulBuffer;
                DtPrint( "Cached Added default key[%d]\n", keyIndex );
                WFCInfo( "Cached Added default key[%d]", keyIndex );
            }
            else
            {
                WFCError( "CSetDefaultKeyJob failed alloc memory for key" );
            }
            //--
        }
        else
        {
            //++
            // if deleting default key; free it and remove it from the cache
            //
            pMyKeyValue = m_pAdapter->m_DefaultKeyValues[keyIndex];
            if ( pMyKeyValue )
            {
                RtlSecureZeroMemory( pMyKeyValue, m_pAdapter->m_DefaultKeyLengths[keyIndex]);
                ExFreePoolWithTag( pMyKeyValue, WDI_POOL_TAG_KEY );
                m_pAdapter->m_DefaultKeyValues[keyIndex] = NULL;
                DtPrint( "Deleted cached default key[%d]\n", keyIndex );
                WFCInfo( "Deleted cached default key[%d]", keyIndex );
            }
            else
            {
                ASSERT(0);
                WFCError( "Deleting default key cache that does not exist" );
            }
            //--
        }
    }

    //
    // Complete the OID back to NDIS
    //
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);
}


NDIS_STATUS CSetDefaultKeyJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_CIPHER_DEFAULT_KEY_VALUE pDefaultKeyValue;
    UINT        informationBufferLength;

    // We override this so that we can call the data path in the
    // the context of the original OID request.
    ndisStatus = CSimpleSetDeviceCommandOidJob::InitializeFromOidRequest(pOidRequest);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    // Check if we are adding keys
    informationBufferLength = SET_OID_BUFFER_LENGTH(pOidRequest);
    if (informationBufferLength >= sizeof(DOT11_CIPHER_DEFAULT_KEY_VALUE))
    {
        pDefaultKeyValue = (PDOT11_CIPHER_DEFAULT_KEY_VALUE)SET_OID_BUFFER(pOidRequest);

        if (pDefaultKeyValue->bDelete == FALSE)
        {
            // Key is being added, notify data path about it so that it
            // can now enable encryption of packets

            // Here we will look at the pPort. Note that normally this is not OK
            // However we being called in the context of the OID, so we know that there
            // is no other OID currently deleteing the port or adding a new one.
            // So as long as we just using within this API, its OK.
            CPort* pPort = m_pAdapter->GetPortFromNdisPortNumber(
                get_NdisPortNumber());

            pPort->SetKeyState(TRUE);
        }
    }

exit:
    return ndisStatus;
}

NDIS_STATUS CSetDefaultKeyJob::GetDeviceCommandData(
    _In_ PVOID              pInformationBuffer,
    _In_ UINT               InformationBufferLength,
    _Inout_ PUINT           pBytesRead,
    _Inout_ PUINT           pBytesNeeded,
    _In_ UINT               ReservedHeaderLength,
    _Out_ ULONG*            pCommandId,
    _Out_ ULONG*            pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE**           pCommandBuffer)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_CIPHER_DEFAULT_KEY_VALUE pDefaultKeyValue;
    WDI_CIPHER_KEY_TYPE keyType = WDI_CIPHER_KEY_TYPE_GROUP_KEY;
    WDI_SET_ADD_CIPHER_KEYS_PARAMETERS AddCipherKeysParameters;
    WDI_SET_DELETE_CIPHER_KEYS_PARAMETERS DeleteCipherKeysParameters;
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());

    TraceActEntry();

    *pBytesRead = 0;
    *pBytesNeeded = 0;

    pDefaultKeyValue = (PDOT11_CIPHER_DEFAULT_KEY_VALUE)pInformationBuffer;

    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pDefaultKeyValue->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_CIPHER_DEFAULT_KEY_VALUE_REVISION_1,
        sizeof(DOT11_CIPHER_DEFAULT_KEY_VALUE)))
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid NDIS object header. Expected (T:%d R:%d S:%d) Received (T:%d R:%d S:%d)",
            NDIS_OBJECT_TYPE_DEFAULT, DOT11_CIPHER_DEFAULT_KEY_VALUE_REVISION_1,
            sizeof(DOT11_CIPHER_DEFAULT_KEY_VALUE), pDefaultKeyValue->Header.Type,
            pDefaultKeyValue->Header.Revision, pDefaultKeyValue->Header.Size
            );
        goto exit;
    }

    *pBytesNeeded = FIELD_OFFSET(DOT11_CIPHER_DEFAULT_KEY_VALUE, ucKey) +
        pDefaultKeyValue->usKeyLength;
    if (InformationBufferLength < (*pBytesNeeded))
    {
        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
        WFCActError("Insufficient data. Expected %d Received %d",
            *pBytesNeeded, InformationBufferLength);
        goto exit;
    }

    *pBytesRead = *pBytesNeeded;

    // Check if this is a BIP key set
    if ((pDefaultKeyValue->AlgorithmId == DOT11_CIPHER_ALGO_BIP) ||
        (pDefaultKeyValue->uKeyIndex == 4) ||
        (pDefaultKeyValue->uKeyIndex == 5))
    {
        WFCActInfo("Setting BIP key (Algo = %d, Index = %d)", pDefaultKeyValue->AlgorithmId,
            pDefaultKeyValue->uKeyIndex);

        keyType = WDI_CIPHER_KEY_TYPE_IGTK;
    }

    if (!pDefaultKeyValue->bDelete)
    {
        // Add key
        AddCipherKeysParameters.SetCipherKey.ElementCount = 1;
        AddCipherKeysParameters.SetCipherKey.pElements = new WDI_SET_ADD_CIPHER_KEYS_CONTAINER[1];

        AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.PeerMacAddress_IsPresent = false;

        AddCipherKeysParameters.SetCipherKey.pElements[0].CipherKeyTypeInfo.KeyType = keyType;
        AddCipherKeysParameters.SetCipherKey.pElements[0].CipherKeyTypeInfo.CipherAlgorithm = CDot11ToWabiConverter::MapCipherAlgorithm(pDefaultKeyValue->AlgorithmId);
        AddCipherKeysParameters.SetCipherKey.pElements[0].CipherKeyTypeInfo.Direction = CDot11ToWabiConverter::MapCipherKeyDirection(DOT11_DIR_BOTH);
        AddCipherKeysParameters.SetCipherKey.pElements[0].CipherKeyTypeInfo.Static = pDefaultKeyValue->bStatic;
        AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.CipherKeyID_IsPresent = true;
        AddCipherKeysParameters.SetCipherKey.pElements[0].CipherKeyID.CipherKeyID = pDefaultKeyValue->uKeyIndex;

        AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.BIPKey_IsPresent = false;
        AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.CCMPKey_IsPresent = false;
        AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.GCMPKey_IsPresent = false;
        AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.TKIPInfo_IsPresent = false;
        AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.WEPKey_IsPresent = false;
        AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.IHVKey_IsPresent = false;
        
        switch (pDefaultKeyValue->AlgorithmId)
            {
                case DOT11_CIPHER_ALGO_WEP40:
                case DOT11_CIPHER_ALGO_WEP104:
                case DOT11_CIPHER_ALGO_WEP:
                {
                     AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.WEPKey_IsPresent = true;
                     AddCipherKeysParameters.SetCipherKey.pElements[0].WEPKey.ElementCount = pDefaultKeyValue->usKeyLength;
                     AddCipherKeysParameters.SetCipherKey.pElements[0].WEPKey.pElements = pDefaultKeyValue->ucKey;
                }
                break;

                case DOT11_CIPHER_ALGO_TKIP:
                {
                    PDOT11_KEY_ALGO_TKIP_MIC pTkipKey =  (PDOT11_KEY_ALGO_TKIP_MIC)pDefaultKeyValue->ucKey;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.ReceiveSequenceCount_IsPresent = true;
                    RtlCopyMemory(AddCipherKeysParameters.SetCipherKey.pElements[0].ReceiveSequenceCount.ReceiveSequenceCount,
                        pTkipKey->ucIV48Counter,
                        6);

                    // The TKIP key starts at ucTKIPMICKeys [0]. The MIC key starts at ucTKIPMICKeys [ ulTKIPKeyLength ].
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.TKIPInfo_IsPresent = true;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].TKIPInfo.TKIPKey.ElementCount = (UINT16)pTkipKey->ulTKIPKeyLength;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].TKIPInfo.TKIPKey.pElements = pTkipKey->ucTKIPMICKeys;

                    AddCipherKeysParameters.SetCipherKey.pElements[0].TKIPInfo.TKIPMIC.ElementCount = (UINT16)pTkipKey->ulMICKeyLength;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].TKIPInfo.TKIPMIC.pElements = &pTkipKey->ucTKIPMICKeys[pTkipKey->ulTKIPKeyLength];
                }
                break;

                case DOT11_CIPHER_ALGO_CCMP:
                {
                    PDOT11_KEY_ALGO_CCMP pCcmpKey =  (PDOT11_KEY_ALGO_CCMP)pDefaultKeyValue->ucKey;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.ReceiveSequenceCount_IsPresent = true;
                    RtlCopyMemory(AddCipherKeysParameters.SetCipherKey.pElements[0].ReceiveSequenceCount.ReceiveSequenceCount,
                        pCcmpKey->ucIV48Counter,
                        6);
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.CCMPKey_IsPresent = true;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].CCMPKey.ElementCount = (UINT16)pCcmpKey->ulCCMPKeyLength;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].CCMPKey.pElements = pCcmpKey->ucCCMPKey;
                }
                break;

                case DOT11_CIPHER_ALGO_GCMP:
                {
                    PDOT11_KEY_ALGO_GCMP pGcmpKey =  (PDOT11_KEY_ALGO_GCMP)pDefaultKeyValue->ucKey;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.ReceiveSequenceCount_IsPresent = true;
                    RtlCopyMemory(AddCipherKeysParameters.SetCipherKey.pElements[0].ReceiveSequenceCount.ReceiveSequenceCount,
                        pGcmpKey->ucIV48Counter,
                        6);
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.GCMPKey_IsPresent = true;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].GCMPKey.ElementCount = (UINT16)pGcmpKey->ulGCMPKeyLength;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].GCMPKey.pElements = pGcmpKey->ucGCMPKey;
                }
                break;

                case DOT11_CIPHER_ALGO_BIP:
                {
                    PDOT11_KEY_ALGO_BIP pBipKey =  (PDOT11_KEY_ALGO_BIP)pDefaultKeyValue->ucKey;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.ReceiveSequenceCount_IsPresent = true;
                    RtlCopyMemory(AddCipherKeysParameters.SetCipherKey.pElements[0].ReceiveSequenceCount.ReceiveSequenceCount,
                        pBipKey->ucIPN,
                        6);

                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.BIPKey_IsPresent = true;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].BIPKey.ElementCount = (UINT16)pBipKey->ulBIPKeyLength;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].BIPKey.pElements = pBipKey->ucBIPKey;
                }
                break;

                default:
                {
                    if ((pDefaultKeyValue->AlgorithmId >= DOT11_CIPHER_ALGO_IHV_START) &&
                        (pDefaultKeyValue->AlgorithmId <= DOT11_CIPHER_ALGO_IHV_END))
                    {
                        AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.IHVKey_IsPresent = true;
                        AddCipherKeysParameters.SetCipherKey.pElements[0].IHVKey.ElementCount = pDefaultKeyValue->usKeyLength;
                        AddCipherKeysParameters.SetCipherKey.pElements[0].IHVKey.pElements = pDefaultKeyValue->ucKey;
                    }
                    else
                    {
                        NT_ASSERT(FALSE);
                        ndisStatus = NDIS_STATUS_INVALID_DATA;
                        goto exit;
                    }
                }
            }

        *pCommandId = WDI_SET_ADD_CIPHER_KEYS;
        ndisStatus = GenerateWdiSetAddCipherKeys(
            &AddCipherKeysParameters,
            ReservedHeaderLength,
            m_pAdapter->GetTlvContext( ),
            pCommandBufferLength,
            pCommandBuffer
            );

        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to generate add default key params tlv. Error = 0x%08x", ndisStatus);
            goto exit;
        }
    }
    else
    {
        // Delete key
        *pCommandId = WDI_SET_DELETE_CIPHER_KEYS;

        DeleteCipherKeysParameters.CipherKeyInfo.ElementCount = 1;
        DeleteCipherKeysParameters.CipherKeyInfo.pElements = new WDI_SET_DELETE_CIPHER_KEYS_CONTAINER[1];

        DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].CipherKeyID.CipherKeyID = pDefaultKeyValue->uKeyIndex;
        DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].CipherKeyTypeInfo.CipherAlgorithm = CDot11ToWabiConverter::MapCipherAlgorithm(pDefaultKeyValue->AlgorithmId);
        DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].CipherKeyTypeInfo.Direction = CDot11ToWabiConverter::MapCipherKeyDirection(DOT11_DIR_BOTH);
        DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].CipherKeyTypeInfo.KeyType = keyType;
        DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].CipherKeyTypeInfo.Static = pDefaultKeyValue->bStatic;

        RtlCopyMemory(DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].PeerMacAddress.Address, pDefaultKeyValue->MacAddr, sizeof(WDI_MAC_ADDRESS));

        ndisStatus = GenerateWdiSetDeleteCipherKeys(
            &DeleteCipherKeysParameters,
            ReservedHeaderLength,
            m_pAdapter->GetTlvContext( ),
            pCommandBufferLength,
            pCommandBuffer
            );

        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to generate delete default key params tlv. Error = 0x%08x", ndisStatus);
            goto exit;
        }
    }


    if (pPort->m_pRoamTraceLoggingData && pPort->m_pRoamTraceLoggingData->roamOccured)
    {
        pPort->m_pRoamTraceLoggingData->setCipherKeysTime = CSystem::get_CurrentTime();
        pPort->TraceLoggingRoamFinished(NDIS_STATUS_SUCCESS);
    }

exit:
    if (AddCipherKeysParameters.SetCipherKey.pElements)
    {
        delete[] AddCipherKeysParameters.SetCipherKey.pElements;
    }

    if (DeleteCipherKeysParameters.CipherKeyInfo.pElements)
    {
        delete[] DeleteCipherKeysParameters.CipherKeyInfo.pElements;
    }

    TraceActExit(ndisStatus);
    return ndisStatus;
}



//==============================================================================================
NDIS_STATUS CSetDefaultKeyIdJob::GetDeviceCommandData(
    _In_ PVOID             pInformationBuffer,
    _In_ UINT              InformationBufferLength,
    _Inout_ PUINT             pBytesRead,
    _Inout_ PUINT             pBytesNeeded,
    _In_ UINT              ReservedHeaderLength,
    _Out_ ULONG* pCommandId,
    _Out_ ULONG*           pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE**           pCommandBuffer)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId());
    ULONG newKeyId;
    WDI_SET_DEFAULT_KEY_ID_PARAMETERS defaultKeyIdParameters;
    RtlZeroMemory(&defaultKeyIdParameters, sizeof(defaultKeyIdParameters));

    TraceActEntry();

    *pBytesNeeded = sizeof(ULONG);

    if (InformationBufferLength < sizeof(ULONG))
    {
        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    *pBytesRead = InformationBufferLength;
    newKeyId = *((ULONG*)pInformationBuffer);

    if (newKeyId >= DOT11_MAX_NUM_DEFAULT_KEY)
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        WFCActError("Default Key ID %d > Max Count %d", newKeyId, DOT11_MAX_NUM_DEFAULT_KEY);
        goto exit;
    }

    // Cache the value to property
    pPort->GetPropertyCache()->SetPropertyULong(
        (WFC_PROPERTY_NAME)WfcPortPropertyNameDefaultKeyId,
        newKeyId);

    //++
    // cache default key id to adapter for resume from hibernation
    //
    m_pAdapter->m_DefaultKeyId = newKeyId;
    DtPrint( "Set Default Key id=%d\n", newKeyId );
    WFCInfo( "Set Default Key id=%d", newKeyId );
    //--

    *pCommandId = WDI_SET_DEFAULT_KEY_ID;

    defaultKeyIdParameters.DefaultKeyIdParameters.KeyID = newKeyId;

    ndisStatus = GenerateWdiSetDefaultKeyId(
        &defaultKeyIdParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer
        );

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to generate default key ID params tlv. Error = 0x%08x", ndisStatus);
        goto exit;
    }

exit:
    TraceActExit(ndisStatus);
    return ndisStatus;
}



//==============================================================================================

NDIS_STATUS CSetKeyMappingKeyJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE pKeyMappingKeyValue;
    UINT        informationBufferLength;

    // We override this so that we can call the data path in the
    // the context of the original OID request.
    ndisStatus = CSimpleSetDeviceCommandOidJob::InitializeFromOidRequest(pOidRequest);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    // Check if we are adding keys
    informationBufferLength = SET_OID_BUFFER_LENGTH(pOidRequest);
    if (informationBufferLength >= sizeof(DOT11_CIPHER_KEY_MAPPING_KEY_VALUE))
    {
        PDOT11_BYTE_ARRAY pArray = (PDOT11_BYTE_ARRAY)SET_OID_BUFFER(pOidRequest);
        // Must be a valid array with at least 1 element
        if (VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pArray->Header,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_CIPHER_KEY_MAPPING_KEY_VALUE_BYTE_ARRAY_REVISION_1,
            sizeof(DOT11_BYTE_ARRAY)) &&
            (pArray->uNumOfBytes >= sizeof(DOT11_CIPHER_KEY_MAPPING_KEY_VALUE)))
        {
            for (ULONG cbSize, i = 0; i + sizeof(DOT11_CIPHER_KEY_MAPPING_KEY_VALUE) < pArray->uNumOfBytes; i += cbSize)
            {
                pKeyMappingKeyValue = (PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE)(pArray->ucBuffer + i);
                cbSize = FIELD_OFFSET(DOT11_CIPHER_KEY_MAPPING_KEY_VALUE, ucKey) + pKeyMappingKeyValue->usKeyLength;

                if (pKeyMappingKeyValue->bDelete == FALSE)
                {
                    // Key is being added, notify data path about it so that it
                    // can now enable encryption of packets

                    // Here we will look at the pPort. Note that normally this is not OK
                    // However we being called in the context of the OID, so we know that there
                    // is no other OID currently deleting the port or adding a new one.
                    // So as long as we just using within this API, its OK.
                    CPort* pPort = m_pAdapter->GetPortFromNdisPortNumber(
                        get_NdisPortNumber());

                    pPort->SetKeyState(TRUE);
                    break;
                }
            }
        }
    }

exit:
    return ndisStatus;
}

NDIS_STATUS CSetKeyMappingKeyJob::GetDeviceCommandData(
    _In_ PVOID             pInformationBuffer,
    _In_ UINT              InformationBufferLength,
    _Inout_ PUINT             pBytesRead,
    _Inout_ PUINT             pBytesNeeded,
    _In_ UINT              ReservedHeaderLength,
    _Out_ ULONG* pCommandId,
    _Out_ ULONG*           pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE**           pCommandBuffer)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_BYTE_ARRAY keyData;
    PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE pKeyMappingKeys;
    ULONG keysListLength;
    ULONG keySize;
    WDI_SET_ADD_CIPHER_KEYS_PARAMETERS AddCipherKeysParameters;
    WDI_SET_DELETE_CIPHER_KEYS_PARAMETERS DeleteCipherKeysParameters;
    TraceActEntry();

    *pBytesRead = 0;
    *pBytesNeeded = 0;

    keyData = (PDOT11_BYTE_ARRAY)pInformationBuffer;

    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT(keyData->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_CIPHER_KEY_MAPPING_KEY_VALUE_BYTE_ARRAY_REVISION_1,
        sizeof(DOT11_BYTE_ARRAY)))
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid NDIS object header. Expected (T:%d R:%d S:%d) Received (T:%d R:%d S:%d)",
            NDIS_OBJECT_TYPE_DEFAULT, DOT11_CIPHER_KEY_MAPPING_KEY_VALUE_BYTE_ARRAY_REVISION_1,
            sizeof(DOT11_BYTE_ARRAY), keyData->Header.Type,
            keyData->Header.Revision, keyData->Header.Size
            );
        goto exit;
    }

    *pBytesNeeded = FIELD_OFFSET(DOT11_BYTE_ARRAY, ucBuffer) +
        keyData->uNumOfBytes;
    if (InformationBufferLength < *pBytesNeeded)
    {
        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
        WFCActError("Insufficient data. Expected %d Received %d",
            *pBytesNeeded, InformationBufferLength);
        goto exit;
    }

    pKeyMappingKeys = (PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE)keyData->ucBuffer;
    keysListLength = keyData->uNumOfBytes;

    if (keysListLength >= FIELD_OFFSET(DOT11_CIPHER_KEY_MAPPING_KEY_VALUE, ucKey))
    {
        keySize = FIELD_OFFSET(DOT11_CIPHER_KEY_MAPPING_KEY_VALUE, ucKey) +
            pKeyMappingKeys->usKeyLength;
        if (keysListLength < keySize)
        {
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            WFCActError("Invalid size for key mapping key value");
            goto exit;
        }

        if (!pKeyMappingKeys->bDelete)
        {
            // Add key
            m_IsKeyAdded = true;

            *pCommandId = WDI_SET_ADD_CIPHER_KEYS;

            AddCipherKeysParameters.SetCipherKey.ElementCount = 1;
            AddCipherKeysParameters.SetCipherKey.pElements = new WDI_SET_ADD_CIPHER_KEYS_CONTAINER[1];

            AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.PeerMacAddress_IsPresent = true;
            RtlCopyMemory(AddCipherKeysParameters.SetCipherKey.pElements[0].PeerMacAddress.Address, pKeyMappingKeys->PeerMacAddr, sizeof(WDI_MAC_ADDRESS));
            AddCipherKeysParameters.SetCipherKey.pElements[0].CipherKeyTypeInfo.KeyType = WDI_CIPHER_KEY_TYPE_PAIRWISE_KEY;
            AddCipherKeysParameters.SetCipherKey.pElements[0].CipherKeyTypeInfo.CipherAlgorithm = CDot11ToWabiConverter::MapCipherAlgorithm(pKeyMappingKeys->AlgorithmId);
            AddCipherKeysParameters.SetCipherKey.pElements[0].CipherKeyTypeInfo.Direction = CDot11ToWabiConverter::MapCipherKeyDirection(pKeyMappingKeys->Direction);
            AddCipherKeysParameters.SetCipherKey.pElements[0].CipherKeyTypeInfo.Static = pKeyMappingKeys->bStatic;

            AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.CipherKeyID_IsPresent = false;
            AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.BIPKey_IsPresent = false;
            AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.CCMPKey_IsPresent = false;
            AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.GCMPKey_IsPresent = false;
            AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.TKIPInfo_IsPresent = false;
            AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.WEPKey_IsPresent = false;
            AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.IHVKey_IsPresent = false;

            switch (pKeyMappingKeys->AlgorithmId)
            {
                case DOT11_CIPHER_ALGO_WEP40:
                case DOT11_CIPHER_ALGO_WEP104:
                case DOT11_CIPHER_ALGO_WEP:
                {
                     AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.WEPKey_IsPresent = true;
                     AddCipherKeysParameters.SetCipherKey.pElements[0].WEPKey.ElementCount = pKeyMappingKeys->usKeyLength;
                     AddCipherKeysParameters.SetCipherKey.pElements[0].WEPKey.pElements = pKeyMappingKeys->ucKey;
                }
                break;

                case DOT11_CIPHER_ALGO_TKIP:
                {
                    PDOT11_KEY_ALGO_TKIP_MIC pTkipKey =  (PDOT11_KEY_ALGO_TKIP_MIC)pKeyMappingKeys->ucKey;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.ReceiveSequenceCount_IsPresent = true;
                    RtlCopyMemory(AddCipherKeysParameters.SetCipherKey.pElements[0].ReceiveSequenceCount.ReceiveSequenceCount,
                        pTkipKey->ucIV48Counter,
                        6);

                    // The TKIP key starts at ucTKIPMICKeys [0]. The MIC key starts at ucTKIPMICKeys [ ulTKIPKeyLength ].
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.TKIPInfo_IsPresent = true;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].TKIPInfo.TKIPKey.ElementCount = (UINT32)pTkipKey->ulTKIPKeyLength;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].TKIPInfo.TKIPKey.pElements = pTkipKey->ucTKIPMICKeys;

                    AddCipherKeysParameters.SetCipherKey.pElements[0].TKIPInfo.TKIPMIC.ElementCount = (UINT32)pTkipKey->ulMICKeyLength;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].TKIPInfo.TKIPMIC.pElements = &pTkipKey->ucTKIPMICKeys[pTkipKey->ulTKIPKeyLength];                    
                }
                break;

                case DOT11_CIPHER_ALGO_CCMP:
                {
                    PDOT11_KEY_ALGO_CCMP pCcmpKey =  (PDOT11_KEY_ALGO_CCMP)pKeyMappingKeys->ucKey;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.ReceiveSequenceCount_IsPresent = true;

                    RtlCopyMemory(AddCipherKeysParameters.SetCipherKey.pElements[0].ReceiveSequenceCount.ReceiveSequenceCount,
                        pCcmpKey->ucIV48Counter,
                        6);
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.CCMPKey_IsPresent = true;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].CCMPKey.ElementCount = (UINT16)pCcmpKey->ulCCMPKeyLength;                    
                    AddCipherKeysParameters.SetCipherKey.pElements[0].CCMPKey.pElements = pCcmpKey->ucCCMPKey;
                }
                break;

                case DOT11_CIPHER_ALGO_GCMP:
                {
                    PDOT11_KEY_ALGO_GCMP pGcmpKey =  (PDOT11_KEY_ALGO_GCMP)pKeyMappingKeys->ucKey;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.ReceiveSequenceCount_IsPresent = true;

                    RtlCopyMemory(AddCipherKeysParameters.SetCipherKey.pElements[0].ReceiveSequenceCount.ReceiveSequenceCount,
                        pGcmpKey->ucIV48Counter,
                        6);
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.GCMPKey_IsPresent = true;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].GCMPKey.ElementCount = (UINT16)pGcmpKey->ulGCMPKeyLength;                    
                    AddCipherKeysParameters.SetCipherKey.pElements[0].GCMPKey.pElements = pGcmpKey->ucGCMPKey;
                }
                break;

                case DOT11_CIPHER_ALGO_BIP:
                {
                    PDOT11_KEY_ALGO_BIP pBipKey =  (PDOT11_KEY_ALGO_BIP)pKeyMappingKeys->ucKey;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.ReceiveSequenceCount_IsPresent = true;

                    RtlCopyMemory(AddCipherKeysParameters.SetCipherKey.pElements[0].ReceiveSequenceCount.ReceiveSequenceCount,
                        pBipKey->ucIPN,
                        6);

                    AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.BIPKey_IsPresent = true;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].BIPKey.ElementCount = (UINT16)pBipKey->ulBIPKeyLength;
                    AddCipherKeysParameters.SetCipherKey.pElements[0].BIPKey.pElements = pBipKey->ucBIPKey;
                }
                break;

                default:
                {
                    if ((pKeyMappingKeys->AlgorithmId >= DOT11_CIPHER_ALGO_IHV_START) &&
                        (pKeyMappingKeys->AlgorithmId <= DOT11_CIPHER_ALGO_IHV_END))
                    {
                        AddCipherKeysParameters.SetCipherKey.pElements[0].Optional.IHVKey_IsPresent = true;
                        AddCipherKeysParameters.SetCipherKey.pElements[0].IHVKey.ElementCount = pKeyMappingKeys->usKeyLength;
                        AddCipherKeysParameters.SetCipherKey.pElements[0].IHVKey.pElements = pKeyMappingKeys->ucKey;
                    }
                    else
                    {
                        NT_ASSERT(FALSE);
                        ndisStatus = NDIS_STATUS_INVALID_DATA;
                        goto exit;
                    }
                }
            }

            ndisStatus = GenerateWdiSetAddCipherKeys(
                &AddCipherKeysParameters,
                ReservedHeaderLength,
                m_pAdapter->GetTlvContext( ),
                pCommandBufferLength,
                pCommandBuffer
                );

            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCActError("Failed to generate add key mapping key params tlv. Error = 0x%08x", ndisStatus);
                goto exit;
            }
        }
        else
        {
            // Delete key
            *pCommandId = WDI_SET_DELETE_CIPHER_KEYS;
            DeleteCipherKeysParameters.CipherKeyInfo.ElementCount = 1;
            DeleteCipherKeysParameters.CipherKeyInfo.pElements = new WDI_SET_DELETE_CIPHER_KEYS_CONTAINER[1];

            DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].CipherKeyID.CipherKeyID = 0;
            DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].CipherKeyTypeInfo.CipherAlgorithm = CDot11ToWabiConverter::MapCipherAlgorithm(pKeyMappingKeys->AlgorithmId);
            DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].CipherKeyTypeInfo.Direction = CDot11ToWabiConverter::MapCipherKeyDirection(pKeyMappingKeys->Direction);
            DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].CipherKeyTypeInfo.KeyType = WDI_CIPHER_KEY_TYPE_PAIRWISE_KEY;
            DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].CipherKeyTypeInfo.Static = pKeyMappingKeys->bStatic;

            RtlCopyMemory(DeleteCipherKeysParameters.CipherKeyInfo.pElements[0].PeerMacAddress.Address, pKeyMappingKeys->PeerMacAddr, sizeof(WDI_MAC_ADDRESS));

            ndisStatus = GenerateWdiSetDeleteCipherKeys(
                &DeleteCipherKeysParameters,
                ReservedHeaderLength,
                m_pAdapter->GetTlvContext( ),
                pCommandBufferLength,
                pCommandBuffer
                );

            if (ndisStatus != NDIS_STATUS_SUCCESS)
            {
                WFCActError("Failed to generate delete key mapping key params tlv. Error = 0x%08x", ndisStatus);
                goto exit;
            }
        }

        keysListLength -= keySize;
        pKeyMappingKeys = (PDOT11_CIPHER_KEY_MAPPING_KEY_VALUE)((PUCHAR)pKeyMappingKeys + keySize);

        NT_ASSERT(keysListLength == 0);
    }
    else
    {
        WFCActError("OS did not provide sufficient buffer in set key command.");
        NT_ASSERT(FALSE);
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    *pBytesRead = *pBytesNeeded;

    //
    // Return success only if all data are consumed.
    //
    if (keysListLength != 0)
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }
exit:
    if (AddCipherKeysParameters.SetCipherKey.pElements)
    {
        delete[] AddCipherKeysParameters.SetCipherKey.pElements;
    }

    if (DeleteCipherKeysParameters.CipherKeyInfo.pElements)
    {
        delete[] DeleteCipherKeysParameters.CipherKeyInfo.pElements;
    }

    TraceActExit(ndisStatus);
    return ndisStatus;
}

void CSetKeyMappingKeyJob::OnJobStarted()
{
    // Call the parent's implementation first (where m_IsKeyAdded gets populated).
    // We don't care if the job started successfully or not.
    CSimpleSetDeviceCommandOidJob::OnJobStarted();

    if (m_IsKeyAdded == true)
    {
        // Key is being added, queue neighbor report request and opportunistic roam.
        // It is safe to queue the timers here since this is called in EventQueue's context
        CPort* pPort = m_pAdapter->GetPortFromNdisPortNumber(
            get_NdisPortNumber());

        pPort->QueueNeighborReportRequest();
        // now that keys are being added queue opportunistic roam timer for ExtSTA
        if (pPort->GetPortType() == WfcPortTypeExtSTA)
        {
            pPort->QueueOpportunisticRoam();
        }
    }
}


//==============================================================================================
NDIS_STATUS CSetDesiredBSSIDListJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    DOT11_BSSID_LIST* pInBssidList = (DOT11_BSSID_LIST*)pInformationBuffer;

    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pInBssidList->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_BSSID_LIST_REVISION_1,
        sizeof(DOT11_BSSID_LIST)))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid NDIS object header. Expected (T:%d R:%d S:%d) Received (T:%d R:%d S:%d)",
            NDIS_OBJECT_TYPE_DEFAULT, DOT11_BSSID_LIST_REVISION_1,
            sizeof(DOT11_BSSID_LIST), pInBssidList->Header.Type,
            pInBssidList->Header.Revision, pInBssidList->Header.Size
            );

        goto exit;
    }

    if (pInBssidList->uNumOfEntries > 0)
    {
        // Verify length/number of entries match up
        *pBytesNeeded = pInBssidList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS)+
            FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs);
        if (InformationBufferLength < *pBytesNeeded)
        {
            status = NDIS_STATUS_BUFFER_OVERFLOW;
            WFCActError("Insufficient data. Expected %d Received %d",
                *pBytesNeeded, InformationBufferLength);
            goto exit;
        }

        if (pInBssidList->uNumOfEntries > WFC_CONSTANT::DesiredBSSIDListSize)
        {
            status = NDIS_STATUS_INVALID_LENGTH;
            WFCActError("Unsupported number of BSSID list entries. Limt %d Received %d",
                WFC_CONSTANT::DesiredBSSIDListSize, pInBssidList->uNumOfEntries);
            goto exit;
        }
    }

    status = GetPortPropertyCache()->SetPropertyList(
        WfcPortPropertyDesiredBSSIDList,
        pInBssidList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS),
        (UINT16)pInBssidList->uNumOfEntries,
        (BYTE *)pInBssidList->BSSIDs
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save BSSID list property in Property Cache. Error = 0x%08x", status);
        goto exit;
    }

    *pBytesRead = *pBytesNeeded;

    //
    // If only the broadcast MAC is present, accept any BSSID
    //
    if ((pInBssidList->uNumOfEntries == 1) &&
        (ETH_IS_BROADCAST(pInBssidList->BSSIDs[0]) == TRUE)
        )
    {
        status = GetPortPropertyCache()->SetPropertyBoolean(
            WfcPortPropertyAnyBSSIDAllowed,
            TRUE
            );
    }
    else
    {
        status = GetPortPropertyCache()->SetPropertyBoolean(
            WfcPortPropertyAnyBSSIDAllowed,
            FALSE
            );
    }

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save Any BSSID property in Property Cache. Error = 0x%08x", status);;
        goto exit;
    }
exit:
    TraceActExit(status);

    return status;
}


//==============================================================================================
NDIS_STATUS CSetStaAssociationIEsJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(pBytesNeeded);

    TraceActEntry();

    DOT11_ASSOCIATION_PARAMS* pInAssocParams = (DOT11_ASSOCIATION_PARAMS*)pInformationBuffer;

    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pInAssocParams->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_ASSOCIATION_PARAMS_REVISION_1,
        sizeof(DOT11_ASSOCIATION_PARAMS)))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid NDIS object header. Expected (T:%d R:%d S:%d) Received (T:%d R:%d S:%d)",
            NDIS_OBJECT_TYPE_DEFAULT, DOT11_ASSOCIATION_PARAMS_REVISION_1,
            sizeof(DOT11_ASSOCIATION_PARAMS), pInAssocParams->Header.Type,
            pInAssocParams->Header.Revision, pInAssocParams->Header.Size
            );
        goto exit;
    }

    if (pInAssocParams->uAssocRequestIEsOffset >
        (pInAssocParams->uAssocRequestIEsOffset + pInAssocParams->uAssocRequestIEsLength))
    {
        // Integer overflow
        status = NDIS_STATUS_INVALID_DATA;
        WFCActError("Integer overflow. %d + %d",
            pInAssocParams->uAssocRequestIEsOffset, pInAssocParams->uAssocRequestIEsLength);
        goto exit;
    }

    //
    // Verify IE blob length
    //
    *pBytesNeeded = pInAssocParams->uAssocRequestIEsOffset + pInAssocParams->uAssocRequestIEsLength;
    if (InformationBufferLength < *pBytesNeeded)
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        WFCActError("Insufficient data. Expected %d Received %d",
            *pBytesNeeded, InformationBufferLength);
        goto exit;
    }

    status = GetPortPropertyCache()->SetPropertyBuffer(
        WfcPortPropertyExtraAssociationRequestIEs,
        pInAssocParams->uAssocRequestIEsLength,
        (BYTE *)pInAssocParams + pInAssocParams->uAssocRequestIEsOffset
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save Exta Assocation IEs property in Property Cache. Error = 0x%08x", status);
        goto exit;
    }

    *pBytesRead = *pBytesNeeded;

exit:
    TraceActExit(status);

    return status;
}

//==============================================================================================
NDIS_STATUS CEnumAssociationInfoJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PDOT11_ASSOCIATION_INFO_LIST    pAssocInfoList = (PDOT11_ASSOCIATION_INFO_LIST)pInformationBuffer;
    CConnectedPeer *pPeer = NULL;
    DOT11_MAC_ADDRESS ZeroMac = { 0 };

    UNREFERENCED_PARAMETER(ZeroMac);

    CPort* pPort = m_pAdapter->GetPortFromNdisPortNumber(
        get_NdisPortNumber());

    TraceActEntry();

    NT_ASSERT(pInformationBuffer);

    RtlZeroMemory(pInformationBuffer, InformationBufferLength);

    ASSIGN_NDIS_OBJECT_HEADER(pAssocInfoList->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_ASSOCIATION_INFO_LIST_REVISION_1,
        sizeof(DOT11_ASSOCIATION_INFO_LIST));

    pAssocInfoList->uNumOfEntries = 0;
    pAssocInfoList->uTotalNumOfEntries = 0;

    *pBytesNeeded = sizeof(DOT11_ASSOCIATION_INFO_LIST);

    if (InformationBufferLength < *pBytesNeeded)
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    pPeer = pPort->GetConnectedPeerList()->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);
    if (pPeer != NULL)
    {
        PDOT11_ASSOCIATION_INFO_EX  pAssocInfo = NULL;
        pAssocInfoList->uNumOfEntries = 1;

        pAssocInfo = &(pAssocInfoList->dot11AssocInfo[0]);

        NdisMoveMemory(pAssocInfo->PeerMacAddress,
            pPeer->GetMacAddress(),
            sizeof(DOT11_MAC_ADDRESS));

        NT_ASSERT(sizeof(DOT11_MAC_ADDRESS) >
            RtlCompareMemory(pAssocInfo->PeerMacAddress, ZeroMac, sizeof(DOT11_MAC_ADDRESS)));

        NdisMoveMemory(pAssocInfo->BSSID,
            pPeer->GetMacAddress(),
            sizeof(DOT11_MAC_ADDRESS));

        NT_ASSERT(sizeof(DOT11_MAC_ADDRESS) >
            RtlCompareMemory(pAssocInfo->BSSID, ZeroMac, sizeof(DOT11_MAC_ADDRESS)));

        pAssocInfo->dot11AssociationState = dot11_assoc_state_auth_assoc;

        // TODO: Fix this to get the right value
        pAssocInfo->usCapabilityInformation = DOT11_CAPABILITY_INFO_ESS;
        /*
        pAssocInfo->usCapabilityInformation = pAPEntry->Dot11Capability.usValue;
        pAssocInfo->usListenInterval = pStation->Config.ListenInterval;

        pAssocInfo->usAssociationID = pAPEntry->AssocID;
        pAssocInfo->liAssociationUpTime.QuadPart = pAPEntry->AssociationUpTime.QuadPart;
        */

    }
    else
    {
        // Not connected
        pAssocInfoList->uNumOfEntries = 0;
    }


    *pBytesWritten = *pBytesNeeded;

exit:
    TraceActExit(status);
    return status;
}

#if WDI_DISABLED

//==============================================================================================

NDIS_STATUS CSetNicPowerStateJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    DOT11_PHY_STATE_PARAMETERS  phyStateParams;

    TraceActEntry();

    if (InformationBufferLength < sizeof(BOOLEAN))
    {
        *pBytesNeeded = sizeof(BOOLEAN);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    status = GetPortPropertyCache()->SetPropertyBoolean(
        WfcPortPropertyRadioState_Temp,
        *((BOOLEAN*)pInformationBuffer)
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save Radio State property in Property Cache. Error = 0x%08x", status);;
        goto exit;
    }
    *pBytesRead = sizeof(BOOLEAN);

    NdisZeroMemory(&phyStateParams, sizeof(DOT11_PHY_STATE_PARAMETERS));

    //
    // Fill in object header
    //
    phyStateParams.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    phyStateParams.Header.Revision = DOT11_PHY_STATE_PARAMETERS_REVISION_1;
    phyStateParams.Header.Size = sizeof(DOT11_PHY_STATE_PARAMETERS);

    //
    // Phy state buffer
    //
    phyStateParams.uPhyId = DOT11_PHY_ID_ANY;
    phyStateParams.bHardwarePhyState = TRUE;
    phyStateParams.bSoftwarePhyState = *((BOOLEAN*)pInformationBuffer);

    m_pAdapter->IndicateStatus(
        GetNdisPortNumber(),
        NDIS_STATUS_DOT11_PHY_STATE_CHANGED,
        0,
        &phyStateParams,
        sizeof(DOT11_PHY_STATE_PARAMETERS));
exit:
    TraceActExit(status);
    return status;
}


NDIS_STATUS CQueryNicPowerStateJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if (InformationBufferLength < sizeof(BOOLEAN))
    {
        *pBytesNeeded = sizeof(BOOLEAN);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    status = GetPortPropertyCache()->GetPropertyBoolean(
        WfcPortPropertyRadioState_Temp,
        (BOOLEAN*)pInformationBuffer
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to find Radio State property in Property Cache. Error = 0x%08x", status);;
        goto exit;
    }

    *pBytesWritten = sizeof(BOOLEAN);
exit:
    TraceActExit(status);
    return status;
}
#endif // WDI_DISABLED

//==============================================================================================
NDIS_STATUS CQueryOperationModeCapabilityJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PDOT11_OPERATION_MODE_CAPABILITY    pDot11OpModeCapability = (PDOT11_OPERATION_MODE_CAPABILITY)pInformationBuffer;

    TraceActEntry();

    NT_ASSERT(pInformationBuffer);

    *pBytesNeeded = sizeof(DOT11_OPERATION_MODE_CAPABILITY);
    if (InformationBufferLength < *pBytesNeeded)
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }


    RtlZeroMemory(pInformationBuffer, InformationBufferLength);

    pDot11OpModeCapability->uReserved = 0;
    pDot11OpModeCapability->uMajorVersion = 2;  // Native 802.11 version 2.0
    pDot11OpModeCapability->uMinorVersion = 0;

    pDot11OpModeCapability->uNumOfTXBuffers = WFC_CONSTANT::NumOfTXBuffers;
    pDot11OpModeCapability->uNumOfRXBuffers = WFC_CONSTANT::NumOfRXBuffers;

    status = get_AdapterPropertyCache()->GetPropertyULong(
        (WFC_PROPERTY_NAME)WfcAdapterPropertySupportedOpModes,
        &pDot11OpModeCapability->uOpModeCapability);

    if (NDIS_STATUS_SUCCESS != status)
    {
        WFCError("Couldn't get operation mode capability property");
    }

    *pBytesWritten = *pBytesNeeded;

exit:
    TraceActExit(status);
    return status;
}

//==============================================================================================
NDIS_STATUS CQueryMacAddressJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CPort* pPort = m_pAdapter->GetPortFromNdisPortNumber(
        get_NdisPortNumber());
    DOT11_MAC_ADDRESS ZeroMac = { 0 };

    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(ZeroMac);

    TraceActEntry();

    NT_ASSERT(pInformationBuffer);

    *pBytesNeeded = sizeof(DOT11_MAC_ADDRESS);

    RtlCopyMemory(pInformationBuffer,
        pPort->GetMacAddress(),
        sizeof(DOT11_MAC_ADDRESS)
        );

    NT_ASSERT(sizeof(DOT11_MAC_ADDRESS) >
        RtlCompareMemory(pPort->GetMacAddress(), ZeroMac, sizeof(DOT11_MAC_ADDRESS)));

    *pBytesWritten = *pBytesNeeded;

    TraceActExit(status);
    return status;
}

//==============================================================================================
NDIS_STATUS CQueryHardwareAddressJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    UNREFERENCED_PARAMETER(InformationBufferLength);
    TraceActEntry();

    NT_ASSERT(pInformationBuffer);

    *pBytesNeeded = sizeof(DOT11_MAC_ADDRESS);
    *pBytesWritten = 0;

    if (InformationBufferLength < sizeof(DOT11_MAC_ADDRESS))
    {
        status = NDIS_STATUS_BUFFER_TOO_SHORT;
        goto Exit;
    }

    m_pAdapter->GetAdapterPropertyCache()->GetPermanentMacAddress(
        (PDOT11_MAC_ADDRESS)pInformationBuffer);

    *pBytesWritten = *pBytesNeeded;

Exit:
    TraceActExit(status);
    return status;
}

//==============================================================================================
NDIS_STATUS CSetExcludedMacAddressListJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PDOT11_MAC_ADDRESS_LIST pInMacAddressList = (PDOT11_MAC_ADDRESS_LIST)pInformationBuffer;
    BOOLEAN triggerRoam = FALSE;

    TraceActEntry();

    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pInMacAddressList->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_MAC_ADDRESS_LIST_REVISION_1,
        sizeof(DOT11_MAC_ADDRESS_LIST)))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid NDIS object header. Expected (T:%d R:%d S:%d) Received (T:%d R:%d S:%d)",
            NDIS_OBJECT_TYPE_DEFAULT, DOT11_MAC_ADDRESS_LIST_REVISION_1,
            sizeof(DOT11_MAC_ADDRESS_LIST), pInMacAddressList->Header.Type,
            pInMacAddressList->Header.Revision, pInMacAddressList->Header.Size
            );
        goto exit;
    }

    if (pInMacAddressList->uNumOfEntries > 0)
    {
        // Verify length/number of entries match up
        *pBytesNeeded = pInMacAddressList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS)+
            FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs);
        if (InformationBufferLength < *pBytesNeeded)
        {
            status = NDIS_STATUS_BUFFER_OVERFLOW;
            WFCActError("Insufficient data. Expected %d Received %d",
                *pBytesNeeded, InformationBufferLength);
            goto exit;
        }
    }

    status = GetPortPropertyCache()->SetPropertyList(
        WfcPortPropertyExcludedMacAddressList,
        pInMacAddressList->uNumOfEntries * sizeof(DOT11_MAC_ADDRESS),
        (UINT16)pInMacAddressList->uNumOfEntries,
        (BYTE *)pInMacAddressList->MacAddrs
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save Excluded Mac Address list property in Property Cache. Error = 0x%08x", status);
        goto exit;
    }

    *pBytesRead = *pBytesNeeded;

    //
    // If only the broadcast MAC is present, ignore all MAC address
    //
    if ((pInMacAddressList->uNumOfEntries == 1) &&
        (ETH_IS_BROADCAST(pInMacAddressList->MacAddrs[0]) == TRUE)
        )
    {
        status = GetPortPropertyCache()->SetPropertyBoolean(
            WfcPortPropertyIgnoreAllMacAddresses,
            TRUE
            );

        triggerRoam = TRUE;
    }
    else
    {
        status = GetPortPropertyCache()->SetPropertyBoolean(
            WfcPortPropertyIgnoreAllMacAddresses,
            FALSE
            );
    }

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save All Mac Addresses property in Property Cache. Error = 0x%08x", status);;
        goto exit;
    }

    // Trigger roam if necessary
    if (pInMacAddressList->uNumOfEntries != 0)
    {
        CPort* pPort = m_pAdapter->GetPortFromNdisPortNumber(
            get_NdisPortNumber());
        CConnectedPeer *pPeer = pPort->GetConnectedPeerList()
            ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);

        if (pPeer)
        {
            PDOT11_MAC_ADDRESS associatedMacAddress = pPeer->GetMacAddress();

            for (ULONG i = 0; i < pInMacAddressList->uNumOfEntries; i++)
            {
                if (ARE_MAC_ADDRESS_EQUAL(associatedMacAddress, pInMacAddressList->MacAddrs[i]))
                {
                    triggerRoam = TRUE;
                    break;
                }
            }

            if (triggerRoam)
            {
                WFCActInfo("Excluded MAC address list triggering roam from %!MACADDR!", *associatedMacAddress);
                status = pPort->TriggerDisconnectAndReconnect(
                    WDI_ASSOC_STATUS_DISASSOCIATED_BY_HOST,
                    DOT11_MGMT_REASON_DEAUTH_LEAVE_SS);
                if (status != NDIS_STATUS_SUCCESS)
                {
                    WFCActError("Failed to trigger ROAM due to excluded MAC address list. Error = 0x%08x", status);;
                    goto exit;
                }
            }
        }

    }

exit:
    TraceActExit(status);

    return status;
}

//==============================================================================================
NDIS_STATUS CQuerySupportedAlgorithmPairJobBase::QuerySupportedAlgorithmPairData(
    _In_ WFC_ADAPTER_PROPERTY_NAME AlgoPropertyName,
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
    ULONG BssType;
    ULONG bytesNeeded;
    PDOT11_AUTH_CIPHER_PAIR_LIST authCipherList;
    ULONG numOfAlgoPairs;
    PDOT11_AUTH_CIPHER_PAIR pAlgoPairs;

    authCipherList = (PDOT11_AUTH_CIPHER_PAIR_LIST)pInformationBuffer;
    ASSIGN_NDIS_OBJECT_HEADER(authCipherList->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_AUTH_CIPHER_PAIR_LIST_REVISION_1,
        sizeof(DOT11_AUTH_CIPHER_PAIR_LIST));
    authCipherList->uNumOfEntries = 0;
    authCipherList->uTotalNumOfEntries = 0;

    ndisStatus = GetPortPropertyCache()->GetPropertyULong(WfcPortBssType,
        &BssType);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save ULONG property %d in Property Cache. Error = 0x%08x", WfcPortBssType, ndisStatus);
        goto exit;
    }
    if (BssType != dot11_BSS_type_infrastructure)
    {
        goto exit;
    }

    ndisStatus = m_pAdapter->GetAdapterPropertyCache()->GetAuthAlgorithmPairs(AlgoPropertyName,
        &numOfAlgoPairs,
        &pAlgoPairs);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't get supported algo pair property");
        goto exit;
    }

    if (numOfAlgoPairs == 0)
    {
        goto exit;
    }

    authCipherList->uTotalNumOfEntries = numOfAlgoPairs;

    //
    // Ensure enough space for one entry
    //
    bytesNeeded = FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs) + numOfAlgoPairs * sizeof(DOT11_AUTH_CIPHER_PAIR);
    if (InformationBufferLength < bytesNeeded)
    {
        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    authCipherList->uNumOfEntries = numOfAlgoPairs;


    NdisMoveMemory(authCipherList->AuthCipherPairs, pAlgoPairs, numOfAlgoPairs * sizeof(DOT11_AUTH_CIPHER_PAIR));

exit:

    *pBytesWritten = authCipherList->uNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR)+
        FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs);

    *pBytesNeeded = authCipherList->uTotalNumOfEntries * sizeof(DOT11_AUTH_CIPHER_PAIR)+
        FIELD_OFFSET(DOT11_AUTH_CIPHER_PAIR_LIST, AuthCipherPairs);

    return ndisStatus;

}

//==============================================================================================
NDIS_STATUS CQuerySupportedMulticastAlgorithmPairJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    return QuerySupportedAlgorithmPairData(WfcAdapterPropertyInfraSupportedMcastAlgoPairs,
        pInformationBuffer,
        InformationBufferLength,
        pBytesWritten,
        pBytesNeeded);
}

//==============================================================================================
NDIS_STATUS CQuerySupportedUnicastAlgorithmPairJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    return QuerySupportedAlgorithmPairData(WfcAdapterPropertyInfraSupportedUcastAlgoPairs,
        pInformationBuffer,
        InformationBufferLength,
        pBytesWritten,
        pBytesNeeded);
}

//==============================================================================================
NDIS_STATUS CQueryExtSTACapabilityJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
    PDOT11_EXTSTA_CAPABILITY pDot11ExtStaCap = (PDOT11_EXTSTA_CAPABILITY)pInformationBuffer;
    CAdapterPropertyCache* adapterPropertyCache;

    UNREFERENCED_PARAMETER(InformationBufferLength);

    *pBytesNeeded = sizeof(DOT11_EXTSTA_CAPABILITY);

    RtlZeroMemory(pInformationBuffer, sizeof(DOT11_EXTSTA_CAPABILITY));
    WFC_ASSIGN_NDIS_OBJECT_HEADER(pDot11ExtStaCap->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_EXTSTA_CAPABILITY_REVISION_1,
        sizeof(DOT11_EXTSTA_CAPABILITY));

    adapterPropertyCache = m_pAdapter->GetAdapterPropertyCache();

    ndisStatus = adapterPropertyCache->GetPropertyULong(WfcAdapterPropertyScanSSIDListSize,
        &pDot11ExtStaCap->uScanSSIDListSize);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save ULONG property %d in Property Cache. Error = 0x%08x", WfcAdapterPropertyScanSSIDListSize, ndisStatus);
        goto exit;
    }

    ndisStatus = adapterPropertyCache->GetPropertyULong(WfcAdapterPropertyDesiredBSSIDListSize,
        &pDot11ExtStaCap->uDesiredBSSIDListSize);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save ULONG property %d in Property Cache. Error = 0x%08x", WfcAdapterPropertyDesiredBSSIDListSize, ndisStatus);
        goto exit;
    }
    ndisStatus = adapterPropertyCache->GetPropertyULong(WfcAdapterPropertyDesiredSSIDListSize,
        &pDot11ExtStaCap->uDesiredSSIDListSize);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save ULONG property %d in Property Cache. Error = 0x%08x", WfcAdapterPropertyDesiredSSIDListSize, ndisStatus);
        goto exit;
    }

    pDot11ExtStaCap->uExcludedMacAddressListSize = WFC_CONSTANT::ExcludedMacAddressListSize;

    ndisStatus = adapterPropertyCache->GetPropertyULong(WfcAdapterPropertyPrivacyExemptionListSize,
        &pDot11ExtStaCap->uPrivacyExemptionListSize);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save ULONG property %d in Property Cache. Error = 0x%08x", WfcAdapterPropertyPrivacyExemptionListSize, ndisStatus);
        goto exit;
    }

    ndisStatus = adapterPropertyCache->GetPropertyULong(WfcAdapterPropertyKeyMappingTableSize,
        &pDot11ExtStaCap->uKeyMappingTableSize);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save ULONG property %d in Property Cache. Error = 0x%08x", WfcAdapterPropertyKeyMappingTableSize, ndisStatus);
        goto exit;
    }

    ndisStatus = adapterPropertyCache->GetPropertyULong(WfcAdapterPropertyDefaultKeyTableSize,
        &pDot11ExtStaCap->uDefaultKeyTableSize);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save ULONG property %d in Property Cache. Error = 0x%08x", WfcAdapterPropertyDefaultKeyTableSize, ndisStatus);
        goto exit;
    }

    ndisStatus = adapterPropertyCache->GetPropertyULong(WfcAdapterPropertyWEPKeyValueMaxLength,
        &pDot11ExtStaCap->uWEPKeyValueMaxLength);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save ULONG property %d in Property Cache. Error = 0x%08x", WfcAdapterPropertyWEPKeyValueMaxLength, ndisStatus);
        goto exit;
    }

    pDot11ExtStaCap->uPMKIDCacheSize = WFC_CONSTANT::PMKIDCacheSize;

    ndisStatus = adapterPropertyCache->GetPropertyULong(WfcAdapterPropertyMaxNumPerSTADefaultKeyTables,
        &pDot11ExtStaCap->uMaxNumPerSTADefaultKeyTables);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to save ULONG property %d in Property Cache. Error = 0x%08x", WfcAdapterPropertyMaxNumPerSTADefaultKeyTables, ndisStatus);
        goto exit;
    }

    *pBytesWritten = *pBytesNeeded;

exit:

    return ndisStatus;
}

//==============================================================================================

NDIS_STATUS CQueryStatisticsJob::GetDeviceCommandId(
    _Out_ ULONG* pCommandId
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    *pCommandId = WDI_GET_STATISTICS;

    TraceActExit(ndisStatus);
    return ndisStatus;
}

NDIS_STATUS CQueryStatisticsJob::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG CommandBufferLength,
    _In_reads_bytes_(CommandBufferLength) BYTE* pCommandBuffer,
    _In_ PVOID pInformationBuffer,
    _In_ UINT InformationBufferLength,
    _Inout_ PUINT pBytesWritten,
    _Inout_ PUINT pBytesNeeded)
{
    ULONG i = 0;

    UINT16 numPhyIds = 0;
    ULONG bufferSize;
    ULONG *pPhyIdMap;
    WDI_GET_STATISTICS_PARAMETERS statisticsParameters;

    TraceActEntry();

    NT_ASSERT(pInformationBuffer);

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        goto exit;
    }

    OperationStatus = get_AdapterPropertyCache()->GetPropertyList(WfcAdapterPhyTypeDot11PhyIDMap,
        &bufferSize,
        &numPhyIds,
        (BYTE **)&pPhyIdMap);
    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to retrieve expected number of entries in Phy ID list. Error = 0x%08x", OperationStatus);
        goto exit;
    }

    // make sure that buffer is large enough.
    *pBytesNeeded = sizeof(DOT11_STATISTICS)+(numPhyIds - 1)*sizeof(DOT11_PHY_FRAME_STATISTICS);

    if (InformationBufferLength < *pBytesNeeded)
    {
        OperationStatus = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    RtlZeroMemory(pInformationBuffer, InformationBufferLength);
    DOT11_STATISTICS* pOutStatistics = (DOT11_STATISTICS*)pInformationBuffer;

    ASSIGN_NDIS_OBJECT_HEADER(pOutStatistics->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_STATISTICS_REVISION_1,
        sizeof(DOT11_STATISTICS));

    pOutStatistics->ullFourWayHandshakeFailures = 0;
    pOutStatistics->ullTKIPCounterMeasuresInvoked = 0;

    pCommandBuffer += sizeof(WFC_MESSAGE_METADATA);
    CommandBufferLength -= sizeof(WFC_MESSAGE_METADATA);
    
    OperationStatus = ParseWdiGetStatistics(
        CommandBufferLength,
        pCommandBuffer,
        m_pAdapter->GetTlvContext( ),
        &statisticsParameters);

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to parse WiFi Statistics. Error = 0x%08x", OperationStatus);
        goto exit;
    }

    for (i = 0; i < statisticsParameters.PeerMACStatistics.ElementCount; i++)
    {
        PDOT11_MAC_FRAME_STATISTICS pDest = &pOutStatistics->MacUcastCounters;

        if (ETH_IS_BROADCAST(&statisticsParameters.PeerMACStatistics.pElements[i].MACAddress) == TRUE)
        {
            pDest = &pOutStatistics->MacMcastCounters;
        }

        pDest->ullTransmittedFrameCount += statisticsParameters.PeerMACStatistics.pElements[i].TransmittedFrameCount;
        pDest->ullReceivedFrameCount += statisticsParameters.PeerMACStatistics.pElements[i].ReceivedFrameCount;
        
        //TO DO pDest->ullTransmittedFailureFrameCount;
        //To DO pDest->ullReceivedFailureFrameCount

        pDest->ullWEPExcludedCount += statisticsParameters.PeerMACStatistics.pElements[i].WEPExcludedCount;
        pDest->ullTKIPLocalMICFailures += statisticsParameters.PeerMACStatistics.pElements[i].TKIPLocalMICFailures;
        pDest->ullTKIPReplays += statisticsParameters.PeerMACStatistics.pElements[i].TKIPReplays;
        pDest->ullTKIPICVErrorCount += statisticsParameters.PeerMACStatistics.pElements[i].TKIPICVErrorCount;
        pDest->ullCCMPReplays += statisticsParameters.PeerMACStatistics.pElements[i].CCMPReplays;
        pDest->ullCCMPDecryptErrors += statisticsParameters.PeerMACStatistics.pElements[i].CCMPDecryptErrors;
        pDest->ullWEPUndecryptableCount += statisticsParameters.PeerMACStatistics.pElements[i].DecryptFailureCount;
        pDest->ullWEPICVErrorCount += statisticsParameters.PeerMACStatistics.pElements[i].WEPICVErrorCount;
        pDest->ullDecryptSuccessCount += statisticsParameters.PeerMACStatistics.pElements[i].DecryptSuccessCount;
        pDest->ullDecryptFailureCount += statisticsParameters.PeerMACStatistics.pElements[i].DecryptFailureCount;
    }

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to get PHY Statistics. Error = 0x%08x", OperationStatus);
        goto exit;
    }

    for (i = 0; i < statisticsParameters.PhyStatistics.ElementCount; i++)
    {
        ULONG phyIndex = 0;

        OperationStatus = get_AdapterPropertyCache()->GetDot11PhyIDFromPhyType(statisticsParameters.PhyStatistics.pElements[i].PhyType,
            &phyIndex);

        if (OperationStatus != NDIS_STATUS_SUCCESS && statisticsParameters.PhyStatistics.pElements[i].PhyType != WDI_PHY_TYPE_ANY)
        {
            WFCActError("Unable to convert PhyType to PhyID. Error = 0x%08x", OperationStatus);
            goto exit;
        }

        if (statisticsParameters.PhyStatistics.pElements[i].PhyType == WDI_PHY_TYPE_ANY)
        {
            OperationStatus = GetPortPropertyCache()->GetPropertyULong(
                WfcPortPropertyMostDesiredPhyIndex,
                &phyIndex);
            if (OperationStatus != NDIS_STATUS_SUCCESS)
            {
                WFCActError("Phy Type was WDI_PHY_TYPE_ANY. Error while trying to get Most Desired Phy Index. Setting it to zero. Error = 0x%08x", OperationStatus);
                phyIndex = 0;
            }
        }

        PDOT11_PHY_FRAME_STATISTICS pDest = &pOutStatistics->PhyCounters[phyIndex];

        pDest->ullTransmittedFrameCount += statisticsParameters.PhyStatistics.pElements[i].TransmittedFrameCount;
        pDest->ullMulticastTransmittedFrameCount += statisticsParameters.PhyStatistics.pElements[i].GroupTransmittedFrameCount;
        pDest->ullFailedCount += statisticsParameters.PhyStatistics.pElements[i].FailedCount;
        pDest->ullRetryCount += statisticsParameters.PhyStatistics.pElements[i].RetryCount;
        pDest->ullMultipleRetryCount += statisticsParameters.PhyStatistics.pElements[i].MultipleRetryCount;
        pDest->ullMaxTXLifetimeExceededCount += statisticsParameters.PhyStatistics.pElements[i].MaxTXLifetimeExceededCount;

        pDest->ullTransmittedFragmentCount += statisticsParameters.PhyStatistics.pElements[i].TransmittedFragmentCount;
        pDest->ullRTSSuccessCount += statisticsParameters.PhyStatistics.pElements[i].RTSSuccessCount;
        pDest->ullRTSFailureCount += statisticsParameters.PhyStatistics.pElements[i].RTSFailureCount;
        pDest->ullACKFailureCount += statisticsParameters.PhyStatistics.pElements[i].ACKFailureCount;

        pDest->ullReceivedFrameCount += statisticsParameters.PhyStatistics.pElements[i].ReceivedFrameCount;
        pDest->ullMulticastReceivedFrameCount += statisticsParameters.PhyStatistics.pElements[i].GroupReceivedFrameCount;
        pDest->ullPromiscuousReceivedFrameCount += statisticsParameters.PhyStatistics.pElements[i].PromiscuousReceivedFrameCount;
        pDest->ullMaxRXLifetimeExceededCount += statisticsParameters.PhyStatistics.pElements[i].MaxRXLifetimeExceededCount;

        pDest->ullFrameDuplicateCount += statisticsParameters.PhyStatistics.pElements[i].FrameDuplicateCount;
        pDest->ullReceivedFragmentCount += statisticsParameters.PhyStatistics.pElements[i].ReceivedFragmentCount;
        pDest->ullPromiscuousReceivedFragmentCount += statisticsParameters.PhyStatistics.pElements[i].PromiscuousReceivedFragmentCount;
        pDest->ullFCSErrorCount += statisticsParameters.PhyStatistics.pElements[i].FCSErrorCount;
    }

    *pBytesWritten = *pBytesNeeded;

exit:
    TraceActExit(OperationStatus);
    return OperationStatus;
}

NDIS_STATUS CQueryPhyIdJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS ndisStatus;
    ULONG phyID;

    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(pBytesNeeded);


    ndisStatus = GetPortPropertyCache()->GetPropertyULong((WFC_PROPERTY_NAME)WfcPortOSConfiguredPhyID,
        &phyID);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Unable to find WfcPortOSConfiguredPhyID property in port Property Cache. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    //
    // Size is already verified 
    //
    NdisMoveMemory(pInformationBuffer, &(phyID), sizeof(ULONG));

    *pBytesWritten = sizeof(ULONG);

exit:
    return ndisStatus;
}

//==============================================================================================

NDIS_STATUS CQueryRegDomainsSupportJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_REG_DOMAINS_SUPPORT_VALUE pRegDomainsValue;

    UNREFERENCED_PARAMETER(pBytesNeeded);

    // Will only be filling single entry, so easy check
    if (InformationBufferLength < sizeof(DOT11_REG_DOMAINS_SUPPORT_VALUE))
    {
        ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    RtlZeroMemory(pInformationBuffer, sizeof(DOT11_REG_DOMAINS_SUPPORT_VALUE));
    pRegDomainsValue = (PDOT11_REG_DOMAINS_SUPPORT_VALUE)pInformationBuffer;

    // We support a single regulatory domain
    pRegDomainsValue->uNumOfEntries = 1;
    pRegDomainsValue->uTotalNumOfEntries = 1;

    pRegDomainsValue->dot11RegDomainValue[0].uRegDomainsSupportIndex = 0;
    pRegDomainsValue->dot11RegDomainValue[0].uRegDomainsSupportValue = WFC_CONSTANT::RegDomain;

    *pBytesWritten = sizeof(DOT11_REG_DOMAINS_SUPPORT_VALUE);

exit:
    return ndisStatus;
}

//==============================================================================================

NDIS_STATUS CQueryCurrentRegDomainJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    ULONG currentDomain = WFC_CONSTANT::RegDomain;

    // TODO: Placeholder code

    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(pBytesNeeded);

    //
    // Size is already verified 
    //
    NdisMoveMemory(pInformationBuffer, &(currentDomain), sizeof(ULONG));

    *pBytesWritten = sizeof(ULONG);

    return NDIS_STATUS_SUCCESS;
}

//==============================================================================================

void CSetMediaStreamingEnabledOidJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Initialize the Connection quality Job
    ndisStatus = m_ConnectionQualityJob.InitializeFromJob(
        m_pAdapter,
        m_NdisPortNumber,
        m_pAdapter->IsResumeRestoreInProgress(),
        m_PortId,
        WDI_CONNECTION_QUALITY_LOW_LATENCY,
        m_bEnableMediaStreaming);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize connection quality job from media streaming setting (0x%x)", ndisStatus);
        goto Exit;
    }

    // Queue the child job
    ndisStatus = CJobBase::StartChildJob(
        &m_ConnectionQualityJob
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to queue connection quality job (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        CompleteJob(ndisStatus);
    }

    TraceActExit(ndisStatus);
}

void CSetMediaStreamingEnabledOidJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    NDIS_STATUS status = OperationStatus;
    CPortPropertyCache* pPropertyCache;
    CAdapterPropertyCache* pAdapterPropertyCache;
    ULONG mediaStreamingRefCount;
    BOOLEAN mediaStreamingCurrentlyEnabled;

    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);
    
    // If this succeeds, then save it and update global state
    if (status == NDIS_STATUS_SUCCESS)
    {
        pPropertyCache = m_pAdapter->GetPortPropertyCache(
            get_PortId());

        if (pPropertyCache == NULL)
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Couldn't get port property cache.");
            goto exit;
        }

        pAdapterPropertyCache = m_pAdapter->GetAdapterPropertyCache();

        if (pAdapterPropertyCache == NULL)
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("Couldn't get adapter property cache.");
            goto exit;
        }

        status = pPropertyCache->GetPropertyBoolean(
            WfcPortPropertyMediaStreamingEnabled,
            &mediaStreamingCurrentlyEnabled);

        if (status != NDIS_STATUS_SUCCESS)
        {
            mediaStreamingCurrentlyEnabled = FALSE;
        }

        status = pPropertyCache->SetPropertyBoolean(
            WfcPortPropertyMediaStreamingEnabled,
            m_bEnableMediaStreaming);

        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't commit media streaming mode enabled port setting.");
            goto exit;
        }

        status = pAdapterPropertyCache->GetPropertyULong(
            WfcAdapterPropertyMediaStreamingRefcount,
            &mediaStreamingRefCount);

        if (status != NDIS_STATUS_SUCCESS)
        {
            mediaStreamingRefCount = 0;
        }

        if (!mediaStreamingCurrentlyEnabled &&
            m_bEnableMediaStreaming)
        {
            mediaStreamingRefCount++;
        }
        else if (mediaStreamingCurrentlyEnabled &&
            !m_bEnableMediaStreaming)
        {
            if (mediaStreamingRefCount > 0)
            {
                mediaStreamingRefCount--;
            }
            else
            {
                WFCActError("WARN: Ref count for media streaming would have dropped below zero. Not touching it.");
                NT_ASSERT(FALSE);
            }
        }

        WFCActInfo(
            "Setting media streaming mode to %d, refcount to %d",
            m_bEnableMediaStreaming,
            mediaStreamingRefCount);

        status = pAdapterPropertyCache->SetPropertyULong(
            WfcAdapterPropertyMediaStreamingRefcount,
            mediaStreamingRefCount);

        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't commit media streaming ref count.");
            goto exit;
        }
    }
exit:
    TraceActExit(status);

    CompleteJob(status);
}

NDIS_STATUS CSetMediaStreamingEnabledOidJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if (SET_OID_BUFFER_LENGTH(pOidRequest) < sizeof(BOOLEAN))
    {
        status = NDIS_STATUS_INVALID_DATA;
        goto exit;
    }

    m_bEnableMediaStreaming = *(BOOLEAN*)SET_OID_BUFFER(pOidRequest);

exit:
    TraceActExit(status);
    return status;
}

VOID CSetMediaStreamingEnabledOidJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);
}

//==============================================================================================
VOID CSetPMKIDList::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CPortPropertyCache* pPropertyCache;

    // PMKID is configured. Signal the roam scan job
    WFCActInfo("Signalling PMKID list configuration");

    pPropertyCache = m_pAdapter->GetPortPropertyCache(
        get_PortId());

    if (pPropertyCache != NULL)
    {
        WFC_MESSAGE_METADATA *pMessageHeader = NULL;

        status = pPropertyCache->SetPropertyBoolean(
            WfcPortPropertyPMKIDListUpdatePending,
            FALSE);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to clear the PMKID List Update pending property. Status = 0x%08x", status);
        }

        status = pPropertyCache->SetPropertyBoolean(
            WfcPortPropertyAssociationParameterPMKIDUpdatePending,
            FALSE);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActError("Failed to clear the Association Parameter PMKID List Update pending property. Status = 0x%08x", status);
        }

        // Send the indication that we have finished the update
        pMessageHeader = (WFC_MESSAGE_METADATA *)new BYTE[sizeof(WFC_MESSAGE_METADATA)];
        if (pMessageHeader == NULL)
        {
            WFCActError("Failed to allocate memory for WDI_INDICATION_PMKID_CANDIDATE_LIST_UPDATE");
        }

        RtlZeroMemory(pMessageHeader, sizeof(WFC_MESSAGE_METADATA));

        pMessageHeader->MessageId = WDI_INDICATION_PMKID_CANDIDATE_LIST_UPDATE;
        pMessageHeader->MessageLength = sizeof(WFC_MESSAGE_METADATA);
        pMessageHeader->Message.TransactionId = WDI_TRANSACTION_ID_UNSOLICIT;
        pMessageHeader->Message.PortId = get_PortId();
        pMessageHeader->Message.Status = NDIS_STATUS_SUCCESS;

        m_pAdapter->GetNotificationManager()->OnDeviceIndicationArrived(
            sizeof(WFC_MESSAGE_METADATA),
            pMessageHeader,
            NULL,
            TRUE
            );
    }


    CSimplePropertyCacheListSetOidJob::OnOidJobCompletedEvent(NdisStatus);
}


//==============================================================================================

NDIS_STATUS CP2PDiscoverCancelJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    SerializedJobList* serializedJobs;
    CJobBase* currentJob;

    UNREFERENCED_PARAMETER(pInformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(pBytesRead);
    UNREFERENCED_PARAMETER(pBytesNeeded);

    TraceActEntry();

    serializedJobs = m_pAdapter->GetSerializedJobList();

    if (serializedJobs)
    {
        currentJob = serializedJobs->GetActiveSerializedJob();
        if (currentJob &&
            currentJob->get_NdisPortNumber() == get_NdisPortNumber() &&
            currentJob->get_JobType() == WiFiJobP2PDiscover)
        {
            currentJob->CancelJob();
        }
        else
        {
            currentJob = serializedJobs->FindQueuedSerializedJob(
                get_NdisPortNumber(),
                WiFiJobP2PDiscover);

            if (currentJob)
            {
                serializedJobs->OnOperationCompleted(
                    NDIS_STATUS_REQUEST_ABORTED,
                    currentJob);
            }
        }
    }

    TraceActExit(ndisStatus);

    return ndisStatus;
}

//==============================================================================================
NDIS_STATUS CNicSpecificExtensionJob::GetDeviceCommandData(
    _In_reads_bytes_(InputBufferLength) PVOID             pInputBuffer,
    _In_ UINT              InputBufferLength,
    _Inout_ PUINT          pBytesRead,
    _Inout_ PUINT          pBytesNeeded,
    _In_ UINT              ReservedHeaderLength,
    _Out_ ULONG*           pCommandId,
    _Out_ ULONG*           pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_IHV_REQUEST_PARAMETERS requestData;

    TraceActEntry();

    *pBytesRead = InputBufferLength;
    *pBytesNeeded = 0;

    *pCommandId = WDI_IHV_REQUEST;
    *pCommandBufferLength = 0;
    *pCommandBuffer = NULL;

    if (InputBufferLength > 0)
    {
        requestData.Optional.IHVData_IsPresent = TRUE;

        // If the InputBufferLength is greater than 64K, we would 
        // need to map the original data into more than one TLVs
        status = CDot11ToWabiConverter::SplitToArrayOfByteArrays(
            (UINT8 *)pInputBuffer,
            InputBufferLength,
            &requestData.IHVData
            );
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCError("[IHVExt] Failed to split NIC specific extension event buffer into multiple TLVs. Error = 0x%08x", status);
            goto exit;
        }
    }
    else
    {
        requestData.Optional.IHVData_IsPresent = FALSE;
    }

    status = GenerateWdiIhvRequest(
        &requestData,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVExt] Failed to generated NIC specific extension job TLVs. Error = 0x%08x", status);
        goto exit;
    }

exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CNicSpecificExtensionJob::OnDeviceCommandCompletion(
    _Out_writes_bytes_(OutputBufferLength) PVOID             pOutputBuffer,
    _In_ UINT              OutputBufferLength,
    _Inout_ PUINT          pBytesWritten,
    _Inout_ PUINT          pBytesNeeded,
    _In_ NDIS_STATUS       OperationStatus,
    _In_ ULONG             MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_IHV_REQUEST_PARAMETERS responseData;
    BYTE* pIhvResponse = NULL;
    UINT ihvResponseLength = 0;
    bool bNeedsFree = false;

    TraceActEntry();

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVExt] NIC specific extension failed by device. Error = 0x%08x)", OperationStatus);
        goto exit;
    }

    // We will copy whatever data the IHV filled in
    
    status = ParseWdiIhvRequest(
        MessageLength - sizeof(WFC_MESSAGE_METADATA),
        ((PUCHAR)pMessageHeader) + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &responseData
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVExt] Failed to parse NIC specific extension job TLVs. Error = 0x%08x", status);
        goto exit;
    }

    if (responseData.Optional.IHVData_IsPresent)
    {
        // If the total data is greater than 64K, it would be
        // mapped into more than one TLV    
        status = CWabiToDot11Converter::CombineArrayOfByteArrays(
            &responseData.IHVData,            
            &pIhvResponse,
            &ihvResponseLength,
            &bNeedsFree
            );

        *pBytesNeeded = ihvResponseLength;
        if (OutputBufferLength < ihvResponseLength)
        {
            status = NDIS_STATUS_BUFFER_TOO_SHORT;
            WFCActError("[IHVExt] Not enough space to copy IHV extension data (%d bytes) to OID output buffer (%d bytes)",
                ihvResponseLength, OutputBufferLength
                );
            goto exit;
        }
        else
        {
            RtlCopyMemory(pOutputBuffer,
                pIhvResponse,
                ihvResponseLength
                );
            *pBytesWritten = ihvResponseLength;
        }
    }

exit:
    // Only if the operation succeeded, do we copy the new status over
    if (OperationStatus == NDIS_STATUS_SUCCESS)
        OperationStatus = status;


    if (bNeedsFree)
    {
        delete[] pIhvResponse;
    }
    
    TraceActExit(OperationStatus);
    return OperationStatus;
}


//==============================================================================================
CUpdateConnectionQualityJob::CUpdateConnectionQualityJob(
    _In_ WFC_ACTIVITY_ID ActivityId) :
    CJobBase(ActivityId),
    m_DeviceCommand(ActivityId)
{

}

CUpdateConnectionQualityJob::~CUpdateConnectionQualityJob()
{
    if (m_pCommandBuffer)
    {
        delete[] m_pCommandBuffer;
        m_pCommandBuffer = NULL;
    }
}

NDIS_STATUS CUpdateConnectionQualityJob::InitializeFromJob(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_ BOOLEAN   ForceUpdate,
    _In_ WFC_PORT_ID ParentWfcPortId,  // Port that triggered the update
    _In_ WDI_CONNECTION_QUALITY_HINT ConnectionQualityTrigger,  // Connection Quality being triggered
    _In_ BOOLEAN   EnablingTrigger     // Change 
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Some jobs reuse without deleting and cleaning up
    if (m_pCommandBuffer)
    {
        delete[] m_pCommandBuffer;
        m_pCommandBuffer = NULL;
    }

    m_pAdapter = pAdapter;
    m_NdisPortNumber = PortNumber;    
    m_ParentPortId = ParentWfcPortId;
    m_ParentConnectionQualityTrigger = ConnectionQualityTrigger;
    m_ParentEnablingTrigger = EnablingTrigger;
    m_ForceUpdate = ForceUpdate;

    WFCActTrace("[ConnQuality] Connection Quality hint %d being %s on port %d",
        ConnectionQualityTrigger, EnablingTrigger ? "ENABLED" : "DISABLED", m_ParentPortId);
    
    // Initialize the base
    ndisStatus = CJobBase::Initialize(
        m_pAdapter->GetEventQueue(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetActiveJobsList(),
        m_pAdapter->GetSerializedJobList(),
        WiFiJobUpdateConnectionQuality,
        false,
        false,
        WiFiPriorityScopePort,
        WiFiSerializedJobPriority_10,
        // If queued as child of non-serialized job, then its used
        (IPropertyCacheDirectory *)m_pAdapter,
        (INdisConversion *)pAdapter,
        PortNumber
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[ConnQuality] Failed to initialize base job for connection quality (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:

    TraceActExit(ndisStatus);

    return ndisStatus;
}

void CUpdateConnectionQualityJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_CONNECTION_QUALITY_HINT updatedConnectionQuality;
    bool bUpdateStarted = false;

    TraceActEntry();

    // Break this into multiple steps. One to determine the new connection quality
    // and another to do the update. This way if needed in the future, its less
    // effort to change this so that an update on one port can influence other ports

    // Determine the connection quality setting for the port    
    ndisStatus = DetermineConnectionQuality(m_ParentPortId, &updatedConnectionQuality);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[ConnQuality] Failed to determine updated connection quality for port %d. Error = 0x%08x", 
            m_ParentPortId, ndisStatus);
        goto exit;
    }

    // Configure the connection quality on the port. This may 
    // be a no-op
    ndisStatus = StartConnectionQualityUpdateOnPort(m_ParentPortId, 
        updatedConnectionQuality,
        &bUpdateStarted);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[ConnQuality] Failed to set updated connection quality for port %d. Error = 0x%08x", 
            m_ParentPortId, ndisStatus);
        goto exit;
    }

exit:
    if (!bUpdateStarted || (ndisStatus != NDIS_STATUS_SUCCESS))
    {
        // Finish the job
        FinishJob(ndisStatus);
    }

    TraceActExit(ndisStatus);    
}
void CUpdateConnectionQualityJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    UNREFERENCED_PARAMETER(SenderObject);

    OperationStatus = CompleteConnectionQualityUpdateOnPort(OperationStatus,
        m_CommandPortId,
        m_CommandConnectionQuality);
    
    // Complete the job
    FinishJob(OperationStatus);
}

void CUpdateConnectionQualityJob::OnJobCancelled()
{
    // Not cancellable
}
void CUpdateConnectionQualityJob::FinishJob(
    _In_ NDIS_STATUS OperationStatus)
{
    CompleteJob(OperationStatus);
}

NDIS_STATUS CUpdateConnectionQualityJob::DetermineConnectionQuality(
    _In_ WFC_PORT_ID WfcPortId,
    _Out_ WDI_CONNECTION_QUALITY_HINT *pConnectionQuality
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CPortPropertyCache *pPropertyCache = m_pAdapter->GetPortPropertyCache(WfcPortId);

    // Look at the property cache in the port to determine its new
    // connection quality setting

    // Currently we dont every do high availability, so its not looked at

    // Check for media streaming mode
    if ((m_ParentPortId == WfcPortId) && (m_ParentConnectionQualityTrigger == WDI_CONNECTION_QUALITY_LOW_LATENCY))
    {
        // Media streaming mode was being changed on this port. Determine if it is being enabled
        if (m_ParentEnablingTrigger)
        {
            WFCActTrace("[ConnQuality] Media Streaming is being enabled on port %d. Configuring WDI_CONNECTION_QUALITY_LOW_LATENCY", WfcPortId);
            *pConnectionQuality = WDI_CONNECTION_QUALITY_LOW_LATENCY;
            goto exit;            
        }

        // If being disabled other properties are looked at to make a determination
    }
    else
    {
        // This setting is not being configured
        if (pPropertyCache->GetPropertyBooleanOrDefault(
            WfcPortPropertyMediaStreamingEnabled,
            FALSE))
        {
            // Media streaming is enabled
            WFCActTrace("[ConnQuality] Media Streaming is enabled on port %d. Configuring WDI_CONNECTION_QUALITY_LOW_LATENCY", WfcPortId);
            *pConnectionQuality = WDI_CONNECTION_QUALITY_LOW_LATENCY;
            goto exit;
        }
    }
    
    // Check if auto-power save is turned on
    if ((m_ParentPortId == WfcPortId) && (m_ParentConnectionQualityTrigger == WDI_CONNECTION_QUALITY_AUTO_POWER_SAVE))
    {
        // Auto-power saving mode was being changed on this port. Determine if it is being enabled
        if (m_ParentEnablingTrigger)
        {
            WFCActTrace("[ConnQuality] Auto power Saving is being enabled on port %d. Configuring WDI_CONNECTION_QUALITY_AUTO_POWER_SAVE", WfcPortId);
            *pConnectionQuality = WDI_CONNECTION_QUALITY_AUTO_POWER_SAVE;
            goto exit;            
        }

        // If being disabled other properties are looked at to make a determination
    }
    else
    {
        // This setting is not being configured
        if (pPropertyCache->GetPropertyBooleanOrDefault(
            WfcPortPropertyAutoPsm,
            FALSE))
        {
            // Auto powersaving is enabled
            WFCActTrace("[ConnQuality] Auto power Saving is enabled on port %d. Configuring WDI_CONNECTION_QUALITY_AUTO_POWER_SAVE", WfcPortId);
            *pConnectionQuality = WDI_CONNECTION_QUALITY_AUTO_POWER_SAVE;
            goto exit;
        }
    }
    
    // Nothing special is configured
    WFCActTrace("[ConnQuality] No special settings enabled on port %d. Configuring WDI_CONNECTION_QUALITY_NO_POWER_SAVE", WfcPortId);
    *pConnectionQuality = WDI_CONNECTION_QUALITY_NO_POWER_SAVE;    

exit:

    return ndisStatus;
}

NDIS_STATUS CUpdateConnectionQualityJob::StartConnectionQualityUpdateOnPort(
    _In_ WFC_PORT_ID WfcPortId,
    _In_ WDI_CONNECTION_QUALITY_HINT ConnectionQuality,
    _Out_ bool* pbUpdateStarted
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_SET_CONNECTION_QUALITY_PARAMETERS setParameters;
    
    TraceActEntry();
    *pbUpdateStarted = false;

    if (!m_ForceUpdate)
    {
        // Check the cache if the new setting matches what is on the port.
        // This logic gets skiiped if we are resuming firmware after
        // hibernating
        CPortPropertyCache *pPropertyCache = m_pAdapter->GetPortPropertyCache(WfcPortId);
        WDI_CONNECTION_QUALITY_HINT previousConnectionQuality;

        ndisStatus = pPropertyCache->GetPropertyT(
            WfcPortPropertyConnectionQuality,
            &previousConnectionQuality);
        if (ndisStatus == NDIS_STATUS_SUCCESS)        
        {
            if (previousConnectionQuality == ConnectionQuality)
            {
                WFCActTrace("[ConnQuality] Connection quality %d already configured on port %d. Skipping new set", 
                    ConnectionQuality, WfcPortId);
                    
                goto Exit;                
            }
        }
        // Continue
        ndisStatus = NDIS_STATUS_SUCCESS;
    }

    // Configure the new value on the port
    WFCActTrace("[ConnQuality] Configuring connection quality %d on port %d", 
        ConnectionQuality, WfcPortId);

    // Remember this so that we can use it on the complete
    m_CommandPortId = WfcPortId;
    m_CommandConnectionQuality = ConnectionQuality;

    RtlZeroMemory(&setParameters, sizeof(WDI_SET_CONNECTION_QUALITY_PARAMETERS));
    setParameters.Quality = ConnectionQuality;

    // If we are configuring this to be low latency, set the values for the low latency
    // parameters
    if (ConnectionQuality == WDI_CONNECTION_QUALITY_LOW_LATENCY)
    {
        setParameters.LowLatencyParameters.MaximumOffChannelOperationTime = 
            (UINT8)WFC_CONSTANT::MediaStreamingOffChannelTimeLimit;
        setParameters.LowLatencyParameters.RoamingNeededLinkQualityThreshold = 
            (UINT8)g_RoamControlParametersTable[ROAM_CONTROL_LEVEL::WfcMediaStreamingRoam].BetterAPRoamLinkQualityThreshold;

        setParameters.Optional.LowLatencyParameters_IsPresent = true;
    }

    ndisStatus = GenerateWdiSetConnectionQuality(&setParameters,
        (ULONG)sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_CommandSize,
        &m_pCommandBuffer);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[ConnQuality] Failed to generate TLV for connection quality 0x%x", ndisStatus);
        goto Exit;
    }

    //
    // set up WDI command
    //
    ndisStatus = m_DeviceCommand.Initialize(
        WfcPortId,
        WDI_SET_CONNECTION_QUALITY,
        m_CommandSize,
        m_pCommandBuffer);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[ConnQuality] Failed to initialize Device Command for set connection quality (0x%x)", ndisStatus);
        goto Exit;
    }

    //
    // queue it to start
    //
    ndisStatus = QueueDeviceCommand(&m_DeviceCommand);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[ConnQuality] Failed to Queue set connection quality (0x%x)", ndisStatus);
        goto Exit;
    }

    *pbUpdateStarted = true;
    
Exit:
    TraceActExit(ndisStatus);

    return ndisStatus;
}


NDIS_STATUS CUpdateConnectionQualityJob::CompleteConnectionQualityUpdateOnPort(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_PORT_ID WfcPortId,
    _In_ WDI_CONNECTION_QUALITY_HINT ConnectionQuality
    )
{
    NDIS_STATUS ndisStatus;
    TraceActEntry();
    
    WFCActTrace("[ConnQuality] Completed command to set connection quality. Status = 0x%08x", 
        OperationStatus);
    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("[ConnQuality] Device command for connection quality failed with status %x.", OperationStatus);
        goto exit;
    }
    OperationStatus = CMessageHelper::GetStatusFromCommandResult(
        &m_DeviceCommand,
        &ndisStatus );

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("[ConnQuality] Failed get of connection quality status from command result 0x%08x", OperationStatus);
    }
    else
    {
        CPortPropertyCache *pPropertyCache = m_pAdapter->GetPortPropertyCache(WfcPortId);
    
        // Save this in the cache for debugging purpose
        ndisStatus = pPropertyCache->SetPropertyULong(
            WfcPortPropertyConnectionQuality,
            ConnectionQuality
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            // Port has already been configured, so this is a benign error
            WFCActError("[ConnQuality] Failed to cache updated connection quality on port %d. Error = 0x%08x", 
                WfcPortId,
                ndisStatus);
        }
    }

exit:
    TraceActExit(OperationStatus);

    return OperationStatus;
}

//==============================================================================================

/*
  Handles the following jobs:
    CGetAdapterEnhancedCapabilitiesJob
    CGetAdapterBandCapabilitiesJob
*/

NDIS_STATUS CGetAdapterCapabilitiesJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG uBufferSize = 0;
    BYTE *pBuffer;
    WFC_ADAPTER_PROPERTY_NAME PropertyName;
    CAdapterPropertyCache *pAdapterPropertyCache = m_pAdapter->GetAdapterPropertyCache();

    UNREFERENCED_PARAMETER(InformationBufferLength);

    TraceActEntry();

    NT_ASSERT(pInformationBuffer);

    *pBytesWritten = 0;

    uBufferSize = 0;
    switch (m_JobType)
    {
        case (WiFiJobGetAdapterEnhancedCapabilities):
        {
            PropertyName = WfcAdapterPropertyEnhancedCapabilities;
            break;
        }

        case (WiFiJobGetAdapterBandCapabilities):
        {
            PropertyName = WfcAdapterPropertyBandCapabilities;
            break;
        }

        default:
        {
            WFCActError("Invalid JobType %d\n", m_JobType);
            status = NDIS_STATUS_INVALID_DATA;
            goto Exit;
        }
    }

    status = pAdapterPropertyCache->GetPropertyBuffer( PropertyName, &uBufferSize, &pBuffer);
    if (NDIS_STATUS_SUCCESS != status)
    {
        WFCActError("Unable to query Enhanced Capabilities, 0x%x\n", status);
        goto Exit;
    }

    *pBytesNeeded = uBufferSize;
    if (InformationBufferLength < *pBytesNeeded)
    {
        WFCActError("ERROR - Insufficient buffer size - %d < %d\n", InformationBufferLength, *pBytesNeeded);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto Exit;
    }

    RtlZeroMemory( pInformationBuffer, InformationBufferLength);
    RtlCopyMemory( pInformationBuffer, pBuffer, uBufferSize );

    *pBytesWritten = uBufferSize;

Exit:
    TraceActExit(status);
    return status;
}

//==============================================================================================
CIhvTaskRequestJob::CIhvTaskRequestJob(
    _In_ WFC_ACTIVITY_ID ActivityId) :
    CJobBase(ActivityId),
    m_Task(ActivityId),
    m_IsCancelled(false),
    m_pCommandBuffer(NULL)
{

}

CIhvTaskRequestJob::~CIhvTaskRequestJob()
{
    if (m_pCommandBuffer)
    {
        FreeGenerated(m_pCommandBuffer);
        m_pCommandBuffer = NULL;
    }
}

NDIS_STATUS CIhvTaskRequestJob::InitializeFromJob(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_ WFC_PORT_ID ParentWfcPortId,  // Port/Adapter that triggered the update
    _In_ ULONG RequestBufferLength,
    _In_reads_bytes_(RequestBufferLength) BYTE* RequestBuffer
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_INDICATION_IHV_TASK_REQUEST_PARAMETERS requestParams;
    POID_HANDLER_ENTRY pHandlerInfo;
    WDI_TASK_IHV_PARAMETERS taskParameters;

    TraceActEntry();

    m_pAdapter = pAdapter;
    m_NdisPortNumber = PortNumber;    
    m_ParentPortId = ParentWfcPortId;

    // Some jobs reuse without deleting and cleaning up
    if (m_pCommandBuffer)
    {
        FreeGenerated(m_pCommandBuffer);
        m_pCommandBuffer = NULL;
    }

    //
    // Parse the indication
    //
    ndisStatus = ParseWdiIndicationIhvTaskRequest(
        RequestBufferLength,
        RequestBuffer,
        pAdapter->GetTlvContext( ),
        &requestParams);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError( "[IHVTask] Failed to parse IHV task request indication. Status = 0x%08x", ndisStatus );
        goto Exit;
    }
    m_RequestedPriority = requestParams.RequestedPriority;
    WFCActInfo("[IHVTask] Requested IHV Task with Priority = %d on Port = %d", m_RequestedPriority, ParentWfcPortId);

    if (ParentWfcPortId == WDI_PORT_ID_ADAPTER)
    {
        m_IhvPriorityScope = WiFiPriorityScopeAdapter;
    }
    else
    {
        m_IhvPriorityScope = WiFiPriorityScopePort;
    }
    switch (m_RequestedPriority)
    {
        case WDI_IHV_TASK_PRIORITY_HIGH:
            {
                // To simplify things, use the adapter's table to find stuff
                pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(OID_DOT11_RESET_REQUEST);
                if (pHandlerInfo == NULL)
                {
                    WFCActError("[IHVTask] Failed to find handler entry for OID_DOT11_RESET_REQUEST");
                    ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
                    goto Exit;

                }
                m_IhvJobPriority = pHandlerInfo->Priority;
                m_IhvIsPreemptable = false;
                m_IhvCanPreempt = true;
            }
            break;

        case WDI_IHV_TASK_PRIORITY_MEDIUM:
            {
                // To simplify things, use the adapter's table to find stuff
                pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(OID_DOT11_CONNECT_REQUEST);
                if (pHandlerInfo == NULL)
                {
                    WFCActError("[IHVTask] Failed to find handler entry for OID_DOT11_CONNECT_REQUEST");
                    ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
                    goto Exit;

                }
                m_IhvJobPriority = pHandlerInfo->Priority;
                m_IhvIsPreemptable = true;
                m_IhvCanPreempt = true;
            }
            break;

        case WDI_IHV_TASK_PRIORITY_LOW:
            {
                // To simplify things, use the adapter's table to find stuff
                pHandlerInfo = COidJobBase::s_FindOidHandlerEntry(OID_DOT11_STATISTICS);
                if (pHandlerInfo == NULL)
                {
                    WFCActError("[IHVTask] Failed to find handler entry for OID_DOT11_STATISTICS");
                    ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
                    goto Exit;

                }
                m_IhvJobPriority = pHandlerInfo->Priority;
                m_IhvIsPreemptable = true;
                m_IhvCanPreempt = false;
            }
            break;
        default:
        {
            WFCError( "[IHVTask] Invalid priority level %d for IHV task request", m_RequestedPriority);
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            goto Exit;
        }
            
    }
    
    WFCActInfo("[IHVTask] Job Priority = %d", m_IhvJobPriority);
    WFCActInfo("[IHVTask] Priority Scope = %s", m_IhvPriorityScope == WiFiPriorityScopeAdapter ? "Adapter" : "Port");
    WFCActInfo("[IHVTask] Is Preemptable = %s", m_IhvIsPreemptable ? "Yes" : "No");
    WFCActInfo("[IHVTask] Can Preempty = %s", m_IhvCanPreempt ? "Yes" : "No");
    
    // Initialize the base
    ndisStatus = CJobBase::Initialize(
        m_pAdapter->GetEventQueue(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetActiveJobsList(),
        m_pAdapter->GetSerializedJobList(),
        WiFiJobIhvTaskRequest,
        m_IhvIsPreemptable,
        m_IhvCanPreempt,
        m_IhvPriorityScope,
        m_IhvJobPriority,
        // If queued as child of non-serialized job, then its used
        (IPropertyCacheDirectory *)m_pAdapter,
        (INdisConversion *)pAdapter,
        PortNumber
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVTask] Failed to initialize job for IHV task request (0x%x)", ndisStatus);
        goto Exit;
    }

    // Save the device context for later use. To make things easier, we just create the 
    // device command buffer right here
    taskParameters.Optional.IHVSpecificContext_IsPresent = requestParams.Optional.IHVSpecificContext_IsPresent;
    
    // Point to the request's object for the generation
    taskParameters.IHVSpecificContext.SimpleAssign(
        requestParams.IHVSpecificContext.pElements,
        requestParams.IHVSpecificContext.ElementCount);

    ndisStatus = GenerateWdiTaskIhv(
        &taskParameters,
        sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &m_CommandSize,
        &m_pCommandBuffer
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVTask] Failed to generate TLV for IHV task request (0x%x)", ndisStatus);
        goto Exit;
    }
    
Exit:
    CleanupParsedWdiIndicationIhvTaskRequest(&requestParams);
    
    TraceActExit(ndisStatus);

    return ndisStatus;
}

void CIhvTaskRequestJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Start the task
    ndisStatus = StartIhvTask();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVTask] Failed to initiate IHV task for port %d. Error = 0x%08x", 
            m_ParentPortId, ndisStatus);
        goto exit;
    }

exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        // Finish the job
        FinishJob(ndisStatus);
    }

    TraceActExit(ndisStatus);    
}

void CIhvTaskRequestJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    UNREFERENCED_PARAMETER(SenderObject);

    OperationStatus = CompleteIhvTask(OperationStatus);
    
    // Complete the job
    FinishJob(OperationStatus);
}

void CIhvTaskRequestJob::OnJobCancelled()
{
    TraceActEntry();

    if (m_IsCancelled)
    {
        goto exit;
    }
    WFCActInfo("[IHVTask] IHV task cancelled");

    m_IsCancelled = true;

    CancelActivePendingCommand();

exit:
    TraceActExit(NDIS_STATUS_SUCCESS);
}

void CIhvTaskRequestJob::FinishJob(
    _In_ NDIS_STATUS OperationStatus)
{
    CompleteJob(OperationStatus);
}

NDIS_STATUS CIhvTaskRequestJob::StartIhvTask()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    DeviceCommand *pTaskDeviceCommand;
    
    TraceActEntry();

    ndisStatus = m_Task.Initialize(
        m_pAdapter->GetNotificationManager(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetEventQueue());

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVTask] Failed to initialize IHV task. Status = 0x%08x", ndisStatus);
        goto exit;
    }

    ndisStatus = m_Task.get_TaskDeviceCommand(
        &pTaskDeviceCommand);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVTask] Couldn't get device command for IHV task. Status = 0x%08x", ndisStatus);
        goto exit;
    }
    
    ndisStatus = pTaskDeviceCommand->Initialize(
        get_PortId(),
        WDI_TASK_IHV,
        m_CommandSize,
        m_pCommandBuffer);

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVTask] Couldn't attach generated tlvs to device command. Status = 0x%08x", ndisStatus);
        goto exit;
    }

    ndisStatus = StartTask(&m_Task);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVTask] Couldn't start connect task. Status = 0x%08x", ndisStatus);
        goto exit;
    }
    
exit:
    TraceActExit(ndisStatus);

    return ndisStatus;
}


NDIS_STATUS CIhvTaskRequestJob::CompleteIhvTask(
    _In_ NDIS_STATUS OperationStatus
    )
{
    NDIS_STATUS commandStatus;
    TraceActEntry();
    
    WFCActTrace("[IHVTask] Completed IHV task with status = 0x%08x", OperationStatus);

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVTask] IHV Task async operation failed with status 0x%08x", OperationStatus);
        goto exit;
    }

    if (m_IsCancelled)
    {
        OperationStatus = NDIS_STATUS_FAILURE;
        WFCActError("[IHVTask] IHV Task job got cancelled.");
        goto exit;
    }

    OperationStatus = CMessageHelper::GetStatusFromTaskOutput(
        &m_Task,
        &commandStatus);

    if (OperationStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[IHVTask] IHV Task command has failed. Status = 0x%08x", OperationStatus);        
        goto exit;
    }

exit:
    TraceActExit(OperationStatus);

    return OperationStatus;
}

NDIS_STATUS CSetFTParametersJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_FT_PARAMETERS pFastTransitionParameters;
    BUFFER_DOT11_SSID SSIDList;
    ULONG bufferSize;
    CPort* pPort = m_pAdapter->GetPortFromNdisPortNumber(
        get_NdisPortNumber());

    if (InformationBufferLength < SIZEOF_DOT11_FT_PARAMETERS_REVISION_1)
    {
        WFCActError("Buffer too small. Minimum Expected: %x, Actual: %x", SIZEOF_DOT11_FT_PARAMETERS_REVISION_1, InformationBufferLength);
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto Exit;
    }
    
    pFastTransitionParameters = (PDOT11_FT_PARAMETERS)pInformationBuffer;
    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pFastTransitionParameters->Header,
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_FT_PARAMETERS_REVISION_1,
        SIZEOF_DOT11_FT_PARAMETERS_REVISION_1))
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        WFCActError("[11r] Invalid NDIS object header. Expected (T:%d R:%d S:%d) Received (T:%d R:%d S:%d)",
            NDIS_OBJECT_TYPE_DEFAULT, DOT11_FT_PARAMETERS_REVISION_1,
            SIZEOF_DOT11_FT_PARAMETERS_REVISION_1, pFastTransitionParameters->Header.Type,
            pFastTransitionParameters->Header.Revision, pFastTransitionParameters->Header.Size
            );

        goto Exit;
    }

    if (pFastTransitionParameters->uStatus != NDIS_STATUS_SUCCESS)
    {
        // This is a four way handshake failure. Block list this SSID for 11r for sometime.
        ndisStatus = GetPortPropertyCache()->GetPropertyList(WfcPortPropertySSID,
            &bufferSize,
            &SSIDList.ElementCount,
            (BYTE **)&SSIDList.pBuffer);
        if (ndisStatus == NDIS_STATUS_SUCCESS)
        {
            pPort->BlockSSID(
                SSIDList.pBuffer[0],
                WFC_BLOCKED_REASON_ASSOC_REJECTED_OR_PEER_DISCONNECTED
                );
        }

        pPort->ClearFTPropertyCache();

        //Disconnect from the currently connected AP, as the 4-way handshake has failed
        ndisStatus = pPort->TriggerDisconnectAndReconnect(
            WDI_ASSOC_STATUS_AUTH_EXCHANGE_FAILURE, 
            DOT11_MGMT_REASON_MIC_FAILURE
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCActError("[11r] Failed to send disconnect after 4-way handshake failure");
        }
        goto Exit;
    }

    *pBytesNeeded = sizeof(DOT11_FT_PARAMETERS) + pFastTransitionParameters->uR0KHIDSize;

    ndisStatus = GetPortPropertyCache()->SetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertyPMKR0Name, SIZEOF_PMKR0NAME,
        pFastTransitionParameters->pmkr0name);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[11r] Set property cache for PMKR0Name failed. Error = 0x%08x", ndisStatus);
        goto Exit;
    }

    ndisStatus = GetPortPropertyCache()->SetPropertyBuffer((WFC_PROPERTY_NAME)WfcPortPropertyR0KHID, pFastTransitionParameters->uR0KHIDSize,
        (BYTE*)(((PUCHAR)pFastTransitionParameters) + pFastTransitionParameters->uR0KHIDOffset));
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("[11r] Set property cache for R0KH-ID failed. Error = 0x%08x", ndisStatus);
        goto Exit;
    }

    //Mark this as a FT Connection
    GetPortPropertyCache()->SetPropertyBoolean((WFC_PROPERTY_NAME)WfcPortPropertyCurrentConnectionFT, TRUE);

    *pBytesRead = *pBytesNeeded;
Exit:
    TraceActExit(ndisStatus);
    return ndisStatus;
}

