#include "Precomp.hpp"
#include "WfdGroupOwner.hpp"
#include "WfdGroupOwner.tmh"

NDIS_STATUS CStartAPJob::GetCipherForGO(
    _In_ UINT32 elementCount,
    _In_reads_(elementCount) WDI_CIPHER_ALGORITHM *pCipherAlgoList,
    _Out_ WDI_CIPHER_ALGORITHM *pCipherAlgo )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WDI_CIPHER_ALGORITHM CipherAlgo = WDI_CIPHER_ALGO_CCMP;     // default cipher

    for (UINT16 i=0; i<elementCount; i++)
    {
        if (WDI_CIPHER_ALGO_CCMP == pCipherAlgoList[i])
        {
            CipherAlgo = WDI_CIPHER_ALGO_CCMP;
            break;
        }

        if (WDI_CIPHER_ALGO_GCMP == pCipherAlgoList[i])
        {
            CipherAlgo = WDI_CIPHER_ALGO_GCMP;
        }
    }

    *pCipherAlgo = CipherAlgo;
    return status;
}

NDIS_STATUS CStartAPJob::QueryNextCommandInfo(
    _In_ ULONG ReservedHeaderLength,
    _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
    _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
    _Out_ UINT32* pMessageID,
    _Out_ ULONG* pCommandBufferLength,
    _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    CPortPropertyCache* pPropertyCache = GetPortPropertyCache();
    NDIS_OBJECT_HEADER *pNdisHeader;
    ULONG bufferLength;
    UINT16 elementCount;
    ULONG uOpmodeMask;

    TraceActEntry();

    *pCommandToken = NULL;
    *pCommandType = WfcDeviceCommandTypeTask;
    *pMessageID = WDI_TASK_START_AP;
    *pCommandBufferLength = 0;
    *ppCommandBuffer = NULL;

    if (m_LegacyAP)
    {
        DOT11_SSID* pSsid;
        status = pPropertyCache->GetPropertyList(WfcPortPropertySSID, 
                        &bufferLength,
                        &elementCount,
                        (BYTE **)&pSsid);
        if ((status != NDIS_STATUS_SUCCESS) || 
            (bufferLength != sizeof(DOT11_SSID)))
        {
            NT_ASSERT(FALSE);
            WFCActErrorStatus("Could not retrieve Desired SSID from property cache.");
            status = NDIS_STATUS_INVALID_STATE;
            goto exit;
        }

        // Above buffer length check ensures this, but OACR can't figure it out
        _Analysis_assume_( bufferLength >= (FIELD_OFFSET( DOT11_SSID, ucSSID ) + pSsid->uSSIDLength) );

        m_TaskParameters.DesiredSSID.ElementCount = pSsid->uSSIDLength;
        m_TaskParameters.DesiredSSID.pElements = pSsid->ucSSID;
    }
    else
    {
        DOT11_WFD_GROUP_ID* pGroupID;
        
        status = pPropertyCache->GetPropertyBuffer(
            WfcPortPropertyP2PDesiredGroupID,
            &bufferLength,
            (BYTE**)&pGroupID );

        if ( (status != NDIS_STATUS_SUCCESS) ||
             (bufferLength != sizeof(DOT11_WFD_GROUP_ID)) )
        {
            NT_ASSERT(FALSE);
            WFCActErrorStatus("Could not retrieve P2P Desired Group ID from property cache.");
            status = NDIS_STATUS_INVALID_STATE;
            goto exit;
        }

        // Above buffer length check ensures this, but OACR can't figure it out
        _Analysis_assume_( bufferLength >= (FIELD_OFFSET( DOT11_WFD_GROUP_ID, SSID.ucSSID ) + pGroupID->SSID.uSSIDLength) );

        m_TaskParameters.DesiredSSID.pElements = pGroupID->SSID.ucSSID;
        m_TaskParameters.DesiredSSID.ElementCount = pGroupID->SSID.uSSIDLength;
    }
    
    status = pPropertyCache->GetPropertyT(
        WfcPortPropertyBeaconPeriod,
        &m_TaskParameters.StaticParameters.BeaconPeriod );

    status = pPropertyCache->GetPropertyBoolean(
        WfcPortProperty802_11bDataRateSupport,
        (BOOLEAN*)&m_TaskParameters.StaticParameters.DataRateSupport802_11b);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        NT_ASSERT(FALSE);
        WFCActErrorStatus("Could not retrieve 802.11b rate support from property cache.");
        goto exit;
    }

    status = pPropertyCache->GetPropertyT(
        WfcPortPropertyDTIMPeriod,
        &m_TaskParameters.StaticParameters.DTIMPeriod );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        NT_ASSERT(FALSE);
        WFCActErrorStatus("Could not retrieve DTIM Period from property cache.");
        goto exit;
    }

    status = pPropertyCache->GetPropertyList(
        WfcPortPropertyAuthAlgoList, 
        &bufferLength,
        &elementCount,
        (BYTE**)&m_TaskParameters.AuthenticationAlgorithms.pElements );

    _Analysis_assume_( elementCount * sizeof(WDI_AUTH_ALGORITHM) <= bufferLength );

    m_TaskParameters.AuthenticationAlgorithms.ElementCount = elementCount;

    if ( status != NDIS_STATUS_SUCCESS ||
         m_TaskParameters.AuthenticationAlgorithms.ElementCount == 0 )
    {
        NT_ASSERT(FALSE);
        WFCActErrorStatus("Could not retrieve Auth Algorithm List from property cache.");
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    status = pPropertyCache->GetPropertyList(
        WfcPortPropertyUnicastCipherAlgoList, 
        &bufferLength,
        &elementCount,
        (BYTE **)&m_TaskParameters.UnicastCipherAlgorithms.pElements );

    _Analysis_assume_( elementCount * sizeof(WDI_CIPHER_ALGORITHM) <= bufferLength );

    m_TaskParameters.UnicastCipherAlgorithms.ElementCount = elementCount;

    if ( status != NDIS_STATUS_SUCCESS ||
         m_TaskParameters.UnicastCipherAlgorithms.ElementCount == 0 )
    {
        WFCActErrorStatus("Could not retrieve Unicast Cipher Algorithm List from property cache.");
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    status = pPropertyCache->GetPropertyList(
        WfcPortPropertyMulticastCipherAlgoList, 
        &bufferLength,
        &elementCount,
        (BYTE **)&m_TaskParameters.MulticastCipherAlgorithms.pElements );

    _Analysis_assume_( elementCount * sizeof(WDI_CIPHER_ALGORITHM) <= bufferLength );

    m_TaskParameters.MulticastCipherAlgorithms.ElementCount = elementCount;

    if ( status != NDIS_STATUS_SUCCESS ||
         m_TaskParameters.MulticastCipherAlgorithms.ElementCount == 0 )
    {
        NT_ASSERT(FALSE);
        WFCActErrorStatus("Could not retrieve Multicast Cipher Algorithm List from property cache.");
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    status = pPropertyCache->GetPropertyULong(WfcPortCurrentOperationalMode, &uOpmodeMask);
    if (status != NDIS_STATUS_SUCCESS)
    {
        NT_ASSERT(FALSE);
        WFCActErrorStatus("GetPropertyULong (WfcPortCurrentOperationalMode) failed ");
        goto exit;
    }

    WFCInfo ("[Port][PropCache] opMode is = 0x%x, m_NdisPortNumber =  %u", (ULONG) uOpmodeMask, m_NdisPortNumber);
    NT_ASSERT ((uOpmodeMask & WDI_OPERATION_MODE_P2P_GO) != 0);

    //
    // This is a temporary solution until the op mode is passed down to the device for OID_DOT11_CURRENT_OPERATION_MODE oid
    //
    if ((uOpmodeMask & WDI_OPERATION_MODE_P2P_GO) != 0)
    {
        //
        // We only support DOT11_AUTH_ALGO_RSNA_PSK for auth and DOT11_CIPHER_ALGO_CCMP/DOT11_CIPHER_ALGO_GCMP for cipher
        //
        status = GetCipherForGO(
            m_TaskParameters.MulticastCipherAlgorithms.ElementCount,
            (WDI_CIPHER_ALGORITHM *) m_TaskParameters.MulticastCipherAlgorithms.pElements,
            &m_multicastCipherAlgoList[0] );
        if ( status != NDIS_STATUS_SUCCESS )
        {
            NT_ASSERT(FALSE);
            WFCActErrorStatus("Could not retrieve Multicast Cipher Algorithm List from property cache.");
            goto exit;
        }
        m_TaskParameters.MulticastCipherAlgorithms.ElementCount = 1;
        m_TaskParameters.MulticastCipherAlgorithms.pElements = m_multicastCipherAlgoList;

        status = GetCipherForGO(
            m_TaskParameters.UnicastCipherAlgorithms.ElementCount,
            (WDI_CIPHER_ALGORITHM *) m_TaskParameters.UnicastCipherAlgorithms.pElements,
            &m_unicastCipherAlgoList[0] );
        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Could not retrieve Unicast Cipher Algorithm List from property cache.");
            goto exit;
        }
        m_TaskParameters.UnicastCipherAlgorithms.ElementCount  = 1;
        m_TaskParameters.UnicastCipherAlgorithms.pElements = m_unicastCipherAlgoList;

        m_TaskParameters.AuthenticationAlgorithms.ElementCount = 1;
        m_TaskParameters.AuthenticationAlgorithms.pElements = m_authAlgoList;
    }

    status = pPropertyCache->GetPropertyBuffer(
        WfcPortPropertyP2PGroupStartParameters,
        &bufferLength,
        (BYTE**)&pNdisHeader );

    if ( status != NDIS_STATUS_SUCCESS ||
         bufferLength < sizeof(NDIS_OBJECT_HEADER) )
    {
        WFCActInfo("Could not retrieve group start parameters from property cache status=%d, bufferLength=%d. Ignoring advertised operating channel.", status, bufferLength);
        status = NDIS_STATUS_SUCCESS;
    }
    else
    {
        switch (pNdisHeader->Revision)
        {
            case (DOT11_WFD_GROUP_START_PARAMETERS_REVISION_1):
            {
                PDOT11_WFD_GROUP_START_PARAMETERS pChannelInfo = (PDOT11_WFD_GROUP_START_PARAMETERS) pNdisHeader;

                if ((bufferLength >= sizeof(DOT11_WFD_GROUP_START_PARAMETERS)) &&
                    (pChannelInfo->AdvertisedOperatingChannel.ChannelNumber != 0))
                {
                    WFCActInfo(
                        "Rev1: Setting advertised operating channel to OpClass=0x%x, ChannelNumber=0x%x",
                        pChannelInfo->AdvertisedOperatingChannel.OperatingClass,
                        pChannelInfo->AdvertisedOperatingChannel.ChannelNumber );

                    m_TaskParameters.Optional.AdvertisedOperatingChannel_IsPresent = 1;

                    m_TaskParameters.AdvertisedOperatingChannel.OperatingClass = pChannelInfo->AdvertisedOperatingChannel.OperatingClass;
                    m_TaskParameters.AdvertisedOperatingChannel.ChannelNumber = pChannelInfo->AdvertisedOperatingChannel.ChannelNumber;

                    C_ASSERT( sizeof( m_TaskParameters.AdvertisedOperatingChannel.CountryOrRegionString ) == sizeof( pChannelInfo->AdvertisedOperatingChannel.CountryRegionString ) );
                    RtlCopyMemory(
                        m_TaskParameters.AdvertisedOperatingChannel.CountryOrRegionString,
                        pChannelInfo->AdvertisedOperatingChannel.CountryRegionString,
                        sizeof( pChannelInfo->AdvertisedOperatingChannel.CountryRegionString ) );

                }
                else
                {
                    WFCActInfo("Rev1: Channel number is set to zero, not passing down advertised operating channel.");
                }
                break;
            }

            case (DOT11_WFD_GROUP_START_PARAMETERS_REVISION_2):
            {
                PDOT11_WFD_GROUP_START_PARAMETERS_V2 pChannelInfo = (PDOT11_WFD_GROUP_START_PARAMETERS_V2) pNdisHeader;

                if (bufferLength >= sizeof(DOT11_WFD_GROUP_START_PARAMETERS_V2))
                {
                    m_TaskParameters.StaticParameters.AcceptNonP2PClients = pChannelInfo->bAcceptNonP2PClients;
                    m_TaskParameters.StaticParameters.MustUseSpecifiedChannels = pChannelInfo->bMustUseSpecifiedChannels;

                    if ((pChannelInfo->uNumBandChannels) &&
                        (pChannelInfo->uBandChannelsOffset >= sizeof(DOT11_WFD_GROUP_START_PARAMETERS_V2)) &&
                        (pChannelInfo->uBandChannelsLength >= FIELD_OFFSET(DOT11_WFD_BAND_CHANNELS,Channels)) &&
                        (bufferLength >= (pChannelInfo->uBandChannelsOffset + pChannelInfo->uBandChannelsLength)))
                    {
                        status = m_TaskParameters.APBandChannelList.AllocateElements(pChannelInfo->uNumBandChannels, 0);
                        if (status == NDIS_STATUS_SUCCESS)
                        {
                            DWORD i, dwOffset = pChannelInfo->uBandChannelsOffset;

                            WFCActInfo("Rev2: Setting %d operating channels", pChannelInfo->uNumBandChannels);

                            for (i=0; i<pChannelInfo->uNumBandChannels; i++)
                            {
                                PDOT11_WFD_BAND_CHANNELS pDot11BandChannels = (DOT11_WFD_BAND_CHANNELS *) (((PBYTE)pChannelInfo) + dwOffset);
                                PWDI_AP_BAND_CHANNEL_LIST_CONTAINER pWdiBandChannels = &m_TaskParameters.APBandChannelList.pElements[i];

                                if ((bufferLength < (dwOffset + FIELD_OFFSET(DOT11_WFD_BAND_CHANNELS,Channels))) ||
                                    (bufferLength < (dwOffset + FIELD_OFFSET(DOT11_WFD_BAND_CHANNELS,Channels) +
                                                                (pDot11BandChannels->uNumChannels * sizeof(ULONG)))))
                                {
                                    WFCError("\t\t[%d] Invalid bufferLength - bufferLength=%d < %d + %d + %d",
                                        i, bufferLength, dwOffset, FIELD_OFFSET(DOT11_WFD_BAND_CHANNELS,Channels),
                                        bufferLength < (dwOffset + FIELD_OFFSET(DOT11_WFD_BAND_CHANNELS,Channels)) ?
                                            0 : (pDot11BandChannels->uNumChannels * sizeof(ULONG)));
                                    break;
                                }

                                dwOffset += FIELD_OFFSET(DOT11_WFD_BAND_CHANNELS,Channels) + (pDot11BandChannels->uNumChannels * sizeof(ULONG));

                                WFCInfo ("\t\t[%d] Band=<%d>, NumChannels=<%d>, Channels=<%d ...>\n",
                                    i, pDot11BandChannels->uBandID, pDot11BandChannels->uNumChannels,
                                    (pDot11BandChannels->uNumChannels > 0 ? pDot11BandChannels->Channels[0] : 0));

                                pWdiBandChannels->BandID = pDot11BandChannels->uBandID;

                                if (pDot11BandChannels->uNumChannels)
                                {
                                    pWdiBandChannels->Optional.ChannelList_IsPresent = 1;
                                    pWdiBandChannels->ChannelList.SimpleAssign(
                                        (UINT32 *) pDot11BandChannels->Channels,
                                        pDot11BandChannels->uNumChannels);
                                }
                            }

                            if (i >= pChannelInfo->uNumBandChannels)
                            {
                                m_TaskParameters.Optional.APBandChannelList_IsPresent = 1;
                            }
                        }
                    }
                    else
                    {
                        WFCError("Rev2: Not sending down APBandChannelList bufferLength=%d, NumBandChannels=%d, Offset=%d, Length=%d",
                            bufferLength, pChannelInfo->uNumBandChannels,
                            pChannelInfo->uBandChannelsOffset, pChannelInfo->uBandChannelsLength);
                    }

                    if (pChannelInfo->AdvertisedOperatingChannel.ChannelNumber != 0)
                    {
                        WFCActInfo(
                            "Rev2: Setting advertised operating channel to OpClass=0x%x, ChannelNumber=0x%x",
                            pChannelInfo->AdvertisedOperatingChannel.OperatingClass,
                            pChannelInfo->AdvertisedOperatingChannel.ChannelNumber );

                        m_TaskParameters.Optional.AdvertisedOperatingChannel_IsPresent = 1;

                        m_TaskParameters.AdvertisedOperatingChannel.OperatingClass = pChannelInfo->AdvertisedOperatingChannel.OperatingClass;
                        m_TaskParameters.AdvertisedOperatingChannel.ChannelNumber = pChannelInfo->AdvertisedOperatingChannel.ChannelNumber;

                        C_ASSERT( sizeof( m_TaskParameters.AdvertisedOperatingChannel.CountryOrRegionString ) == sizeof( pChannelInfo->AdvertisedOperatingChannel.CountryRegionString ) );
                        RtlCopyMemory(
                            m_TaskParameters.AdvertisedOperatingChannel.CountryOrRegionString,
                            pChannelInfo->AdvertisedOperatingChannel.CountryRegionString,
                            sizeof( pChannelInfo->AdvertisedOperatingChannel.CountryRegionString ) );
                    }
                    else
                    {
                        WFCActInfo("Rev2: Advertised Operating Channel not specified.");
                    }
                }
                else
                {
                    WFCError("Rev2: Invalid buffer size %d < %d",
                        bufferLength, sizeof(DOT11_WFD_GROUP_START_PARAMETERS_V2));
                }

                break;
            }

            default:
            {
                WFCActErrorStatus("Invalid Revision version %d.", pNdisHeader->Revision);
            }
        }
    }

    status = GenerateWdiTaskStartAp(
        &m_TaskParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        ppCommandBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't generate command TLV.");
        goto exit;
    }

exit:
    return status;
}

void CStartAPJob::OnNotifyJobCompletion(
    _In_ WFC_DEVICE_COMMAND_TOKEN,
    _In_ NDIS_STATUS OperationStatus )
{
    NDIS_STATUS status = OperationStatus;
    CPort* pPort = m_pAdapter->GetPortFromPortId(
        get_PortId() );

    TraceActEntry();

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Start AP job was not successful.");
        goto exit;
    }

    WFCActInfo("AP Started Successfully.");
    pPort->SetDot11State(WfcPortDot11StateOp);
    pPort->SetAPStarted(true);
    
exit:
    TraceActExit(status);
}

NDIS_STATUS CStartAPJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST )
{
    return NDIS_STATUS_SUCCESS;
}


void CStartAPJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus );

    TraceActExit(NdisStatus);    
}


NDIS_STATUS CSendAssociationResponseJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();
    ULONG eventBufferLength;
    BYTE* pEventBuffer;
    DOT11_INCOMING_ASSOC_DECISION_V2* pAssocDecision = (DOT11_INCOMING_ASSOC_DECISION_V2*)SET_OID_BUFFER(pOidRequest);
    ULONG bufferLengthCheck;

    m_OidRequestId = pOidRequest->RequestId;
    m_OidPortNumber = pOidRequest->PortNumber;

    if ( !NT_SUCCESS(
             RtlULongAdd(
                 pAssocDecision->uAssocResponseIEsOffset,
                 pAssocDecision->uAssocResponseIEsLength,
                 &bufferLengthCheck ) ) ||
         bufferLengthCheck > SET_OID_BUFFER_LENGTH(pOidRequest) )
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActErrorStatus("Could not validate Association Response IEs length within association decision buffer.");
        goto exit;
    }

    status = m_pAdapter->GetAdapterPropertyCache()->GetPropertyBuffer(
        WfcAdapterProperty_LastAssociationRequestEventBuffer,
        &eventBufferLength,
        &pEventBuffer );

    if ( status != NDIS_STATUS_SUCCESS ||
         pEventBuffer == NULL ||
         eventBufferLength == 0 )
    {
        WFCActErrorStatus("Could not retrieve information about last association frame indication.");
        // NT_ASSERT(FALSE); Cannot assert here since this can happpen if we got two indications & two OIDs 
        status = NDIS_STATUS_INVALID_STATE;
        goto exit;
    }

    if ( m_pAdapter->GetAdapterPropertyCache()->SetPropertyBuffer(
             WfcAdapterProperty_LastAssociationRequestEventBuffer,
             0,
             NULL ) != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Could not reset last association frame indication property.");
        status = NDIS_STATUS_INVALID_STATE;
        NT_ASSERT(FALSE);
        goto exit;
    }

    {
        WDI_INDICATION_AP_ASSOCIATION_REQUEST_PARAMETERS incomingFrameInfo;

        status = ParseWdiIndicationApAssociationRequestReceived(
            eventBufferLength - sizeof(WFC_MESSAGE_METADATA),
            pEventBuffer + sizeof(WFC_MESSAGE_METADATA),
            m_pAdapter->GetTlvContext( ),
            &incomingFrameInfo );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't parse previous incoming association request frame info.");
            NT_ASSERT(FALSE);
            goto exit;
        }

        // Take ownership of all data in parsed FrameInfo structure
        RtlCopyMemory(
            &m_ResponseParameters.IncomingRequestInfo,
            &incomingFrameInfo.IncomingRequestInfo,
            sizeof(m_ResponseParameters.IncomingRequestInfo) );

        // Zero out memory so incomingFrameInfo doesn't cause double-free
        RtlZeroMemory(
            &incomingFrameInfo.IncomingRequestInfo,
            sizeof(incomingFrameInfo.IncomingRequestInfo) );
    }

    m_ResponseParameters.AssocResponseParameters.AcceptRequest = pAssocDecision->bAccept ? 1 : 0;
    m_ResponseParameters.AssocResponseParameters.ReasonCode = pAssocDecision->usReasonCode;
    if (VERIFY_NDIS_OBJECT_HEADER_DEFAULT(pAssocDecision->Header,
                                          NDIS_OBJECT_TYPE_DEFAULT,
                                          DOT11_INCOMING_ASSOC_DECISION_REVISION_2,
                                          DOT11_SIZEOF_INCOMING_ASSOC_DECISION_REVISION_2))
    {
        m_ResponseParameters.Optional.WFDAssociationStatus_IsPresent = 1;
        m_ResponseParameters.WFDAssociationStatus = pAssocDecision->WFDStatus;
    }

    if ( pAssocDecision->uAssocResponseIEsLength > 0 )
    {
        m_ResponseParameters.VendorIEs.ElementCount = pAssocDecision->uAssocResponseIEsLength;
        m_ResponseParameters.VendorIEs.pElements = new BYTE[pAssocDecision->uAssocResponseIEsLength];

        if ( m_ResponseParameters.VendorIEs.pElements == NULL )
        {
            WFCActErrorStatus("Couldn't allocate copy of Vendor IEs blob.");
            status = NDIS_STATUS_RESOURCES;
            goto exit;
        }

        m_ResponseParameters.VendorIEs.MemoryInternallyAllocated = true;
        m_ResponseParameters.Optional.VendorIEs_IsPresent = 1;

        RtlCopyMemory(
            m_ResponseParameters.VendorIEs.pElements,
            (BYTE*)pAssocDecision + pAssocDecision->uAssocResponseIEsOffset,
            m_ResponseParameters.VendorIEs.ElementCount );          
    }
    else
    {
        m_ResponseParameters.Optional.VendorIEs_IsPresent = 0;
    }

exit:
    TraceActExit(status);
    return status;
}

void CSendAssociationResponseJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus )
{
    NDIS_STATUS status = NdisStatus;
    WFCActInfo("Indicating send action frame complete to NDIS with status 0x%x", NdisStatus);

    if ( NdisStatus == NDIS_STATUS_SUCCESS &&
         m_pSendCompleteIndication == NULL )
    {
        WFCActErrorStatus("Completed with success, but did not set send complete indication contents.");
        status = NDIS_STATUS_INVALID_STATE;
    }

    if ( status == NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Start AP successful. Blocking background scan for 30 seconds");
        m_pAdapter->SetNextAllowedScanTime(30000);

        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            NDIS_STATUS_DOT11_INCOMING_ASSOC_COMPLETION,
            m_OidRequestId,
            m_pSendCompleteIndication,
            m_SendCompleteIndicationLength );
    }
    else
    {
        DOT11_INCOMING_ASSOC_COMPLETION_PARAMETERS sendCompleteParams = {0};

        ASSIGN_NDIS_OBJECT_HEADER(
            sendCompleteParams.Header, 
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_INCOMING_ASSOC_COMPLETION_PARAMETERS_REVISION_1,
            sizeof(sendCompleteParams));

        sendCompleteParams.ucErrorSource = DOT11_ASSOC_ERROR_SOURCE_OS;
        sendCompleteParams.uStatus = status;
        RtlCopyMemory(
            &sendCompleteParams.PeerMacAddr,
            &m_ResponseParameters.IncomingRequestInfo.AssocRequestParams.PeerMacAddress,
            sizeof(sendCompleteParams.PeerMacAddr) );

        m_pAdapter->IndicateStatus(
            m_NdisPortNumber,
            NDIS_STATUS_DOT11_INCOMING_ASSOC_COMPLETION,
            m_OidRequestId,
            &sendCompleteParams,
            sizeof(sendCompleteParams) );
    }
}

NDIS_STATUS CSendAssociationResponseJob::QueryNextCommandInfo(
    _In_ ULONG ReservedHeaderLength,
    _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
    _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
    _Out_ UINT32* pMessageID,
    _Out_ ULONG* pCommandBufferLength,
    _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    status = GenerateWdiTaskSendApAssociationResponse(
        &m_ResponseParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        ppCommandBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't generate send association response frame settings.");
        *pMessageID = 0;
        *pCommandType = WfcDeviceCommandTypeInvalid;
        goto exit;
    }

    *pCommandToken = NULL;
    *pCommandType = WfcDeviceCommandTypeTask;
    *pMessageID = WDI_TASK_SEND_AP_ASSOCIATION_RESPONSE;

exit:
    return status;
}

NDIS_STATUS CSendAssociationResponseJob::HandleCommandCompletion(
    _In_ WFC_DEVICE_COMMAND_TOKEN,
    _In_ NDIS_STATUS OperationStatus,
    _In_ NDIS_STATUS WiFiStatus,
    _In_ ULONG CommandCompletionBufferLength,
    _In_reads_bytes_(CommandCompletionBufferLength) BYTE* pCommandCompletionBuffer,
    _Out_ bool* pQueryNextCommand )
{
    NDIS_STATUS status = OperationStatus;
    NTSTATUS ntstatus;
    WDI_INDICATION_SEND_AP_ASSOCIATION_RESPONSE_COMPLETE_PARAMETERS completionParameters;
    DOT11_ASSOC_RESPONSE_FRAME* pResponseFrame;
    DOT11_INCOMING_ASSOC_COMPLETION_PARAMETERS* pCompletionParameters = NULL;
    BYTE* allocatedSpace = NULL;
    ULONG requiredIndicationSize;
    ULONG currentOffset;

    TraceActEntry();

    *pQueryNextCommand = false;

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Send association response task failed - OperationStatus.");
        goto exit;
    }

    if ( WiFiStatus != NDIS_STATUS_SUCCESS )
    {
        status = WiFiStatus;
        WFCActErrorStatus("Send association response task failed - WiFiStatus.");
        goto exit;
    }

    status = ParseWdiIndicationSendApAssociationResponseComplete(
        CommandCompletionBufferLength - sizeof(WFC_MESSAGE_METADATA),
        pCommandCompletionBuffer + sizeof(WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &completionParameters );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't parse send complete parameters.");
        goto exit;
    }

    requiredIndicationSize = sizeof(*pCompletionParameters);
    currentOffset = sizeof(*pCompletionParameters);

    // uAssocReqSize
    ntstatus = RtlULongAdd(requiredIndicationSize, m_ResponseParameters.IncomingRequestInfo.AssocRequestFrame.ElementCount, &requiredIndicationSize);
    if (!NT_SUCCESS(ntstatus))
    {
        status = NTSTATUS2NDISSTATUS(ntstatus);
        WFCActErrorStatus(
            "Error: Integer Overflow while adding uAssocReqSize (%u) to requiredIndicationSize (%u)",
            m_ResponseParameters.IncomingRequestInfo.AssocRequestFrame.ElementCount,
            requiredIndicationSize
            );
        goto exit;
    }

    if ( completionParameters.AssocResponseFrame.ElementCount < sizeof(DOT11_ASSOC_RESPONSE_FRAME) )
    {
        WFCActErrorStatus("Invalid size for association response frame 0x%x", completionParameters.AssocResponseFrame.ElementCount);
        status = NDIS_STATUS_INVALID_DATA;
        NT_ASSERT(FALSE);
        goto exit;
    }

    // uAssocRespSize
    ntstatus = RtlULongAdd(requiredIndicationSize, completionParameters.AssocResponseFrame.ElementCount, &requiredIndicationSize);
    if (!NT_SUCCESS(ntstatus))
    {
        status = NTSTATUS2NDISSTATUS(ntstatus);
        WFCActErrorStatus("Error: Integer Overflow while adding uAssocRespSize (0x%x) to requiredIndicationSize (0x%x)",
            completionParameters.AssocResponseFrame.ElementCount,
            requiredIndicationSize
            );
        goto exit;
    }

    pResponseFrame = (DOT11_ASSOC_RESPONSE_FRAME*)completionParameters.AssocResponseFrame.pElements;

    // uActivePhyListSize
    ULONG uActivePhyListSizeMult;
    ntstatus = RtlULongMult(completionParameters.ActivePhyList.ElementCount, (ULONG)sizeof(ULONG), &uActivePhyListSizeMult);
    if (!NT_SUCCESS(ntstatus))
    {
        status = NTSTATUS2NDISSTATUS(ntstatus);
        WFCActErrorStatus(
            "Error: Integer Overflow while calculating size of uActivePhyListSize (0x%x) * sizeof(ULONG) (0x%x)",
            completionParameters.ActivePhyList.ElementCount,
            sizeof(ULONG)
            );
        goto exit;
    }

    ntstatus = RtlULongAdd(requiredIndicationSize, uActivePhyListSizeMult, &requiredIndicationSize);
    if (!NT_SUCCESS(ntstatus))
    {
        status = NTSTATUS2NDISSTATUS(ntstatus);
        WFCActErrorStatus(
            "Error: Integer Overflow while adding uActivePhyListSize (0x%x) to requiredIndicationSize (0x%x)",
            uActivePhyListSizeMult,
            requiredIndicationSize
            );
        goto exit;
    }

    // uBeaconSize
    ntstatus = RtlULongAdd(requiredIndicationSize, completionParameters.BeaconIEs.ElementCount, &requiredIndicationSize);
    if (!NT_SUCCESS(ntstatus))
    {
        status = NTSTATUS2NDISSTATUS(ntstatus);
        WFCActErrorStatus("Error: Integer Overflow while adding uBeaconSize (0x%x) to requiredIndicationSize (0x%x)",
            completionParameters.BeaconIEs.ElementCount,
            requiredIndicationSize
            );
        goto exit;
    }

    allocatedSpace = new BYTE[requiredIndicationSize];

    if ( allocatedSpace  == NULL )
    {
        status = NDIS_STATUS_RESOURCES;
        WFCActErrorStatus("Couldn't allocate completion parameters structure.");
        goto exit;
    }

    pCompletionParameters =
        (DOT11_INCOMING_ASSOC_COMPLETION_PARAMETERS*)allocatedSpace;

    //
    // Populate Object Header
    //
    ASSIGN_NDIS_OBJECT_HEADER(
        pCompletionParameters->Header, 
        NDIS_OBJECT_TYPE_DEFAULT,
        DOT11_INCOMING_ASSOC_COMPLETION_PARAMETERS_REVISION_1,
        sizeof(*pCompletionParameters));

    //
    // Copy Mac Address
    //
    RtlCopyMemory(
        &pCompletionParameters->PeerMacAddr,
        &completionParameters.AssocResponseResult.PeerMACAddress,
        sizeof(pCompletionParameters->PeerMacAddr) );

    //
    // Populate static parameters
    //

    pCompletionParameters->uStatus = pResponseFrame->usStatusCode;

    if ( pResponseFrame->usStatusCode != 0 )
    {
        pCompletionParameters->ucErrorSource = DOT11_ASSOC_ERROR_SOURCE_REMOTE;
    }

    pCompletionParameters->bReAssocReq = completionParameters.AssocResponseResult.IsReassociationRequest;
    pCompletionParameters->bReAssocResp = completionParameters.AssocResponseResult.IsReAssociationResponse;
    pCompletionParameters->AuthAlgo = (DOT11_AUTH_ALGORITHM)completionParameters.AssocResponseResult.AuthAlgorithm;
    pCompletionParameters->UnicastCipher = (DOT11_CIPHER_ALGORITHM)completionParameters.AssocResponseResult.UnicastCipherAlgorithm;
    pCompletionParameters->MulticastCipher = (DOT11_CIPHER_ALGORITHM)completionParameters.AssocResponseResult.MulticastCipherAlgorithm;

    //
    // Association Request Frame
    //
    pCompletionParameters->uAssocReqOffset = currentOffset;
    pCompletionParameters->uAssocReqSize = m_ResponseParameters.IncomingRequestInfo.AssocRequestFrame.ElementCount;

    if ( pCompletionParameters->uAssocReqSize > 0 )
    {
        RtlCopyMemory(
            allocatedSpace + currentOffset,
            m_ResponseParameters.IncomingRequestInfo.AssocRequestFrame.pElements,
            pCompletionParameters->uAssocReqSize );
    }

    currentOffset += pCompletionParameters->uAssocReqSize;

    //
    // Association Response Frame
    //
    pCompletionParameters->uAssocRespOffset = currentOffset;
    pCompletionParameters->uAssocRespSize = completionParameters.AssocResponseFrame.ElementCount;

    if ( pCompletionParameters->uAssocRespSize > 0 )
    {
        RtlCopyMemory(
            allocatedSpace + currentOffset,
            completionParameters.AssocResponseFrame.pElements,
            pCompletionParameters->uAssocRespSize );
    }

    currentOffset += pCompletionParameters->uAssocRespSize;

    //
    // Active PHY List
    //
    pCompletionParameters->uActivePhyListOffset = currentOffset;
    pCompletionParameters->uActivePhyListSize = completionParameters.ActivePhyList.ElementCount * sizeof(ULONG);

    {
        ULONG* pCurrentPhyType = (ULONG*)(allocatedSpace + currentOffset);

        for( ULONG i = 0; i < completionParameters.ActivePhyList.ElementCount; ++i )
        {
            m_pAdapter->GetAdapterPropertyCache()->GetDot11PhyIDFromPhyType(
                (UINT32)completionParameters.ActivePhyList.pElements[i],
                pCurrentPhyType );

            pCurrentPhyType++;
        }
    }

    currentOffset += pCompletionParameters->uActivePhyListSize;

    //
    // Beacon Frame
    //
    pCompletionParameters->uBeaconOffset = currentOffset;
    pCompletionParameters->uBeaconSize = completionParameters.BeaconIEs.ElementCount;

    if ( pCompletionParameters->uBeaconSize > 0 )
    {
        RtlCopyMemory(
            allocatedSpace + currentOffset,
            completionParameters.BeaconIEs.pElements,
            pCompletionParameters->uBeaconSize );
    }

    currentOffset += pCompletionParameters->uBeaconSize;

    m_pSendCompleteIndication = allocatedSpace;
    m_SendCompleteIndicationLength = requiredIndicationSize;
    allocatedSpace = NULL;

    // Add the peer
    m_pAdapter->GetConnectedPeerList(m_PortId)->AddPeer(
        pCompletionParameters->PeerMacAddr,
        dot11_assoc_state_auth_assoc,
        0
        );

    WFCActInfo("Peer associated successfully. Blocking background scan for 30 seconds.");
    m_pAdapter->SetNextAllowedScanTime(30000);

exit:
    if ( allocatedSpace != NULL )
    {
        delete [] allocatedSpace;
    }

    TraceActExit(status);
    return status;
}

void CSendAssociationResponseJob::OnNotifyJobCompletion(
    _In_ WFC_DEVICE_COMMAND_TOKEN,
    _In_ NDIS_STATUS OperationStatus )
{
    WFCActInfo("Send association response job completed with 0x%x", OperationStatus);
}

NDIS_STATUS CAPResetJob::InitializeFromJob(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Some jobs reuse without deleting and cleaning up
    if ( m_pCommandBuffer )
    {
        FreeGenerated( m_pCommandBuffer );
        m_pCommandBuffer = NULL;
    }

    status = COidJobBase::InitializeWithoutOid(
        WiFiJobStopAp,
        pAdapter,
        FALSE,
        FALSE,
        WiFiPriorityScopeAdapter,
        WiFiSerializedJobPriority_Lowest,
        NdisPortNumber );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize COidJobBase.");
        goto exit;
    }

exit:
    TraceActExit(status);

    return status;
}

NDIS_STATUS CAPResetJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST )
{
    NT_ASSERT(FALSE);
    return NDIS_STATUS_FAILURE;
}

void CAPResetJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS )
{
    NT_ASSERT(FALSE);
}

void CAPResetJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    DeviceCommand* pTaskDeviceCommand;
    CPort* pPort;
    BOOLEAN JobComplete = FALSE;
    WDI_TASK_STOP_AP_PARAMETERS stopAPParams = {0};
    ULONG CommandLength = 0;

    TraceActEntry();

    pPort = m_pAdapter->GetPortFromPortId (get_PortId());
    if (!pPort)
    {
        status = NDIS_STATUS_INVALID_PORT;
        WFCActErrorStatus("Port is NULL.");
        goto exit;
    }
     
    if ( pPort->GetDot11State() == WfcPortDot11StateInit )
    {
        WFCActInfo ("the Ap on PortId %u is not started", (ULONG) get_PortId());
        JobComplete = TRUE;
        goto exit;
    }

    status = m_StopApTask.Initialize(
        m_pAdapter->GetNotificationManager(),
        m_pAdapter->GetDeviceCommandScheduler(),
        m_pAdapter->GetEventQueue() );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize StopAp task.");
        goto exit;
    }

    status = m_StopApTask.get_TaskDeviceCommand (&pTaskDeviceCommand);
    if (status != NDIS_STATUS_SUCCESS || !pTaskDeviceCommand)
    {
        WFCActErrorStatus("Couldn't get task device command.");
        goto exit;
    }

    status = GenerateWdiTaskStopAp(&stopAPParams,
        (ULONG)sizeof (WFC_MESSAGE_METADATA),
        m_pAdapter->GetTlvContext( ),
        &CommandLength,
        (UINT8**)&m_pCommandBuffer
        );
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to Generate TLV for WDI_TASK_STOP_AP (0x%x)", status);
        goto exit;
    }

    status = pTaskDeviceCommand->Initialize (get_PortId(),
                                            WDI_TASK_STOP_AP,
                                            CommandLength,
                                            m_pCommandBuffer);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't initialize StopAp command." );
        goto exit;
    }

    status = StartTask (&m_StopApTask);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't start StopAp task.");
        goto exit;
    }

exit:
    TraceActExit(status);

    if ( status != NDIS_STATUS_SUCCESS || JobComplete)
    {
        CompleteJob (status);
    }
}

void CAPResetJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_STATUS commandStatus;
    CPort* pPort = m_pAdapter->GetPortFromPortId(get_PortId());

    UNREFERENCED_PARAMETER(SenderObject);

    TraceActEntry();

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        status = OperationStatus;
        WFCActErrorStatus("StopAp operation failed.");
        goto exit;
    }

    status = CMessageHelper::GetStatusFromTaskOutput (&m_StopApTask,
                                                    &commandStatus);
    if ((status != NDIS_STATUS_SUCCESS) && (commandStatus == NDIS_STATUS_SUCCESS))
    {
        WFCActErrorStatus("Couldn't get status from task output.");
        goto exit;
    }

    if (commandStatus != NDIS_STATUS_SUCCESS)
    {
        status = commandStatus;
        WFCActErrorStatus("StopAp WABI command failed.");
        goto exit;
    }
    
    WFCActInfo("AP Stopped Successfully.");
    pPort->SetDot11State(WfcPortDot11StateInit);
    pPort->SetAPStarted(false);

exit:
    TraceActExit(status);

    CompleteJob(status);
}


NDIS_STATUS CSetWpsEnabledJob::GetDeviceCommandData(
    _In_ PVOID             pInformationBuffer,
    _In_ UINT              InformationBufferLength,
    _In_ PUINT             pBytesRead,
    _Inout_ PUINT          pBytesNeeded,
    _In_ UINT              ReservedHeaderLength,
    _Out_ ULONG*           pCommandId,
    _Out_ ULONG*           pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer)
{
    NDIS_STATUS status;

    UNREFERENCED_PARAMETER(pBytesNeeded);
    UNREFERENCED_PARAMETER(InformationBufferLength);

    TraceActEntry();

    if ( InformationBufferLength < sizeof(BOOLEAN) )
    {
        *pBytesNeeded = sizeof(BOOLEAN);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto Exit;
    }

    m_WpsEnabled.WPSEnabled = *((BOOLEAN*)pInformationBuffer);
    *pCommandId = WDI_SET_P2P_WPS_ENABLED;
    *pBytesRead = sizeof(BOOLEAN);
    *pCommandBufferLength = 0;

    status = GenerateWdiSetP2pWpsEnabled(
        &m_WpsEnabled,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer
        );

Exit:
    TraceActExit(status);
    return status;
}


NDIS_STATUS CSetWpsEnabledJob::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader )
{
    UNREFERENCED_PARAMETER(MessageLength);
    UNREFERENCED_PARAMETER(pMessageHeader);

    TraceActEntry();

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("CSetWpsEnabledJob::OnDeviceCommandCompletion: LE returned Error <%d>\n", OperationStatus);
        OperationStatus = NDIS_STATUS_SUCCESS;
//        goto Exit;
    }

    WFCActInfo("CSetWpsEnabledJob::OnDeviceCommandCompletion: Setting WpsEnabled state to <%x>\n", m_WpsEnabled.WPSEnabled);
    if ( m_pAdapter->GetAdapterPropertyCache()->SetPropertyBoolean(
             WfcPortPropertyWpsEnabled,
             (BOOLEAN) m_WpsEnabled.WPSEnabled ) != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Couldn't store WpsEnabled state in property cache!");
    }

    TraceActExit(OperationStatus);
    return OperationStatus;
}

NDIS_STATUS CSetEndDwellOnChannel::GetDeviceCommandData(
    _In_ PVOID             pInformationBuffer,
    _In_ UINT              InformationBufferLength,
    _In_ PUINT             pBytesRead,
    _Inout_ PUINT          pBytesNeeded,
    _In_ UINT              ReservedHeaderLength,
    _Out_ ULONG*           pCommandId,
    _Out_ ULONG*           pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer)
{
    NDIS_STATUS status;

    UNREFERENCED_PARAMETER(pInformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(pBytesNeeded);

    TraceActEntry();

    *pCommandId = WDI_SET_END_DWELL_TIME;
    *pBytesRead = 0;
    *pCommandBufferLength = 0;

    status = GenerateWdiSetEndDwellTime(
        &m_EndDwellTime,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext(),
        pCommandBufferLength,
        pCommandBuffer
        );

    TraceActExit(status);
    return status;
}


NDIS_STATUS CSetEndDwellOnChannel::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader )
{
    UNREFERENCED_PARAMETER(MessageLength);
    UNREFERENCED_PARAMETER(pMessageHeader);

    TraceActEntry();

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("CSetEndDwellOnChannel::OnDeviceCommandCompletion: LE returned Error <%d>\n", OperationStatus);
        OperationStatus = NDIS_STATUS_SUCCESS;
    }

    TraceActExit(OperationStatus);
    return OperationStatus;
}

