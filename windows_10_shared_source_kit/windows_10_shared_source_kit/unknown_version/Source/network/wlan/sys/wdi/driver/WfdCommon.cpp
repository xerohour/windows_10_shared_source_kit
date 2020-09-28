#include "Precomp.hpp"
#include "WfdCommon.hpp"
#include "WfdCommon.tmh"

#ifdef OLDER_OS_COMPAT
    typedef struct _DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1 { 
        NDIS_OBJECT_HEADER Header; 
        ULONG uAdvertisedServicesCount;
        ULONG uAdvertisedServicesLength;
        USHORT uServiceUpdateIndicator;
        DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_ENTRY AdvertisedServices[1];
    } DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1, *PDOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1;

NDIS_STATUS
CheckAdvListForOldVersion(
    _In_ UINT uInformationBufferLength,
    _In_ PVOID pBuffer,
    _Out_opt_ UINT *puLength,
    _Out_ DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST **ppDot11FullAdvListNew
    )
{
    UINT uLength;
    DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST *pDot11FullAdvList = (DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST*) pBuffer;
    DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1 *pDot11FullAdvListV1 = (DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1*) pBuffer;
    DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST *pDot11FullAdvListNew = NULL;
    PDOT11_WFD_SERVICE_FULL_ADVERTISEMENT_ENTRY pEntryOld, pEntryNew;

    if ((uInformationBufferLength >= sizeof(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST)) &&
        (pDot11FullAdvList->Header.Size == DOT11_SIZEOF_WFD_SERVICE_SET_ADVERTISEMENT_LIST_REVISION_1) &&
        (uInformationBufferLength ==
         (sizeof(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST) +
          pDot11FullAdvList->uAdvertisedServicesLength +
          pDot11FullAdvList->uAdvertisedServicePrefixesLength)))
    {
        WFCInfo("CheckAdvListForOldVersion: Version is New");

        if (puLength)
        {
            *puLength = uInformationBufferLength;
        }
        *ppDot11FullAdvListNew = pDot11FullAdvList;
        return (NDIS_STATUS_SUCCESS);
    }

    if ((uInformationBufferLength < FIELD_OFFSET(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1,AdvertisedServices)) ||
        (pDot11FullAdvListV1->Header.Size != FIELD_OFFSET(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1,AdvertisedServices)) ||
        (uInformationBufferLength !=
         (FIELD_OFFSET(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1,AdvertisedServices) +
          pDot11FullAdvListV1->uAdvertisedServicesLength)))
    {
        WFCError("CheckAdvListForOldVersion: Invalid Size for Old version -- <%d != %d>, <%d != %d>",
            pDot11FullAdvListV1->Header.Size,
            FIELD_OFFSET(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1,AdvertisedServices),
            uInformationBufferLength,
            (FIELD_OFFSET(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1,AdvertisedServices) +
             pDot11FullAdvListV1->uAdvertisedServicesLength));

        return (NDIS_STATUS_INVALID_DATA);
    }

    uLength = sizeof(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST) + pDot11FullAdvListV1->uAdvertisedServicesLength;
    pDot11FullAdvListNew = (DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST*) new UCHAR [uLength];
    if (!pDot11FullAdvListNew)
    {
        WFCError("CheckAdvListForOldVersion: Couldn't allocate %d bytess", uLength);
        return (NDIS_STATUS_RESOURCES);
    }

    RtlZeroMemory (pDot11FullAdvListNew, uLength);
    RtlCopyMemory (pDot11FullAdvListNew, pDot11FullAdvListV1, sizeof(NDIS_OBJECT_HEADER));
    pDot11FullAdvListNew->uAdvertisedServicesCount = pDot11FullAdvListV1->uAdvertisedServicesCount;
    pDot11FullAdvListNew->uAdvertisedServicesOffset = sizeof(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST);
    pDot11FullAdvListNew->uAdvertisedServicesLength = pDot11FullAdvListV1->uAdvertisedServicesLength;
    RtlCopyMemory ((((PBYTE) pDot11FullAdvListNew) + pDot11FullAdvListNew->uAdvertisedServicesOffset),
                   &pDot11FullAdvListV1->AdvertisedServices,
                   pDot11FullAdvListNew->uAdvertisedServicesLength);

    pEntryNew = (PDOT11_WFD_SERVICE_FULL_ADVERTISEMENT_ENTRY)
                (((PBYTE)pDot11FullAdvListNew) + pDot11FullAdvListNew->uAdvertisedServicesOffset);
    for (UINT i=0; i<pDot11FullAdvListNew->uAdvertisedServicesCount; i++)
    {
        pEntryOld = &pDot11FullAdvListV1->AdvertisedServices[i];

        pEntryNew->uServiceNameOffset = pEntryOld->uServiceNameOffset +
                                        pDot11FullAdvListNew->uAdvertisedServicesOffset -
                                        FIELD_OFFSET(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1,AdvertisedServices);
        pEntryNew->uServiceInformationOffset = pEntryOld->uServiceInformationOffset +
                                               pDot11FullAdvListNew->uAdvertisedServicesOffset -
                                               FIELD_OFFSET(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST_V1,AdvertisedServices);

        pEntryNew++;
    }

    if (puLength)
    {
        *puLength = uLength;
    }
    *ppDot11FullAdvListNew = pDot11FullAdvListNew;

    return (NDIS_STATUS_SUCCESS);
}
#endif  // OLDER_OS_COMPAT

/*
  Handles the following jobs:
    WiFiJobP2PSetDeviceCapability
    WiFiJobP2PSetAdditionalIE
    WiFiJobP2PSetDeviceInfo
    WiFiJobP2PSetGroupOwnerCapability
    WiFiJobP2PSetSecondaryDeviceTypeList
    WiFiJobP2PSetAdvertisementList
*/
NDIS_STATUS CSetWFDAdvertisementSettings::GetDeviceCommandData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesRead,
    _In_ PUINT  pBytesNeeded,
    _In_ UINT   ReservedHeaderLength,
    _Out_ ULONG* pMessageID,
    _Out_ ULONG* pCommandBufferLength,
    _Outptr_result_bytebuffer_(*pCommandBufferLength) BYTE** pCommandBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    UNREFERENCED_PARAMETER(pBytesNeeded);

    WFCActInfo("Setting advertisement settings for job type 0x%x", m_JobType);

    switch(m_JobType)
    {
    case WiFiJobP2PSetDeviceCapability:
    {
        DOT11_WFD_DEVICE_CAPABILITY_CONFIG* pDeviceCaps = (DOT11_WFD_DEVICE_CAPABILITY_CONFIG*)pInformationBuffer;
        
        m_AdvertisementParameters.Optional.DeviceCapability_IsPresent = 1;

        m_AdvertisementParameters.DeviceCapability.DeviceCapabilityBitmap =
            CDot11ToWabiConverter::MapWFDDeviceCapabilities(
                pDeviceCaps->bServiceDiscoveryEnabled,              // bServiceDiscoveryEnabled
                pDeviceCaps->bClientDiscoverabilityEnabled,         // bClientDiscoverabilityEnabled
                pDeviceCaps->bConcurrentOperationSupported,         // bConcurrentOperationSupported
                pDeviceCaps->bInfrastructureManagementEnabled,      // bInfrastructureManagementEnabled
                pDeviceCaps->bDeviceLimitReached,                   // bDeviceLimitReached
                pDeviceCaps->bInvitationProcedureEnabled);          // bInvitationProcedureEnabled

        m_AdvertisementParameters.DeviceCapability.OSSetDeviceCapabilities =
            (DOT11_WFD_DEVICE_CAPABILITY_SERVICE_DISCOVERY |
             DOT11_WFD_DEVICE_CAPABILITY_P2P_CLIENT_DISCOVERABILITY |
             DOT11_WFD_DEVICE_CAPABILITY_CONCURRENT_OPERATION |
             DOT11_WFD_DEVICE_CAPABILITY_P2P_INFRASTRUCTURE_MANAGED |
             DOT11_WFD_DEVICE_CAPABILITY_P2P_DEVICE_LIMIT |
             DOT11_WFD_DEVICE_CAPABILITY_P2P_INVITATION_PROCEDURE);

        m_AdvertisementParameters.DeviceCapability.WPSVersionsEnabled = pDeviceCaps->WPSVersionsEnabled;

        WFCActInfo("[SetDeviceCapability]: Device:OSSetCapabilities = <0x%08x : 0x%08x>, WPSVersion=<%x>",
            m_AdvertisementParameters.DeviceCapability.DeviceCapabilityBitmap,
            m_AdvertisementParameters.DeviceCapability.OSSetDeviceCapabilities,
            m_AdvertisementParameters.DeviceCapability.WPSVersionsEnabled);
    }
    break;
    case WiFiJobP2PSetAdditionalIE:
    {
        DOT11_WFD_ADDITIONAL_IE* pAdditionalIEs = (DOT11_WFD_ADDITIONAL_IE*)pInformationBuffer;

        m_AdvertisementParameters.Optional.AdditionalIEs_IsPresent = 1;

        if (pAdditionalIEs->uBeaconIEsLength > 0)
        {
            m_AdvertisementParameters.AdditionalIEs.Optional.AdditionalBeaconIEs_IsPresent = 1;
            m_AdvertisementParameters.AdditionalIEs.AdditionalBeaconIEs.pElements = (BYTE*)pInformationBuffer + pAdditionalIEs->uBeaconIEsOffset;
            m_AdvertisementParameters.AdditionalIEs.AdditionalBeaconIEs.ElementCount = pAdditionalIEs->uBeaconIEsLength;
        }

        if (pAdditionalIEs->uProbeResponseIEsLength > 0)
        {
            m_AdvertisementParameters.AdditionalIEs.Optional.AdditionalProbeResponseIEs_IsPresent = 1;
            m_AdvertisementParameters.AdditionalIEs.AdditionalProbeResponseIEs.pElements = (BYTE*)pInformationBuffer + pAdditionalIEs->uProbeResponseIEsOffset;
            m_AdvertisementParameters.AdditionalIEs.AdditionalProbeResponseIEs.ElementCount = pAdditionalIEs->uProbeResponseIEsLength;
        }

        if (pAdditionalIEs->uDefaultRequestIEsLength > 0)
        {
            m_AdvertisementParameters.AdditionalIEs.Optional.AdditionalProbeRequestDefaultIEs_IsPresent = 1;
            m_AdvertisementParameters.AdditionalIEs.AdditionalProbeRequestDefaultIEs.pElements = (BYTE*)pInformationBuffer + pAdditionalIEs->uDefaultRequestIEsOffset;
            m_AdvertisementParameters.AdditionalIEs.AdditionalProbeRequestDefaultIEs.ElementCount = pAdditionalIEs->uDefaultRequestIEsLength;        
        }

    }
    break;
    case WiFiJobP2PSetDeviceInfo:
    {
        DOT11_WFD_DEVICE_INFO* pDeviceInfo = (DOT11_WFD_DEVICE_INFO*)pInformationBuffer;

        m_AdvertisementParameters.Optional.DeviceInformation_IsPresent = 1;

        RtlCopyMemory (&m_AdvertisementParameters.DeviceInformation.DeviceInfoParameters.DeviceAddress,
                    &pDeviceInfo->DeviceAddress,
                    sizeof(m_AdvertisementParameters.DeviceInformation.DeviceInfoParameters.DeviceAddress) );

        m_AdvertisementParameters.DeviceInformation.DeviceInfoParameters.ConfigurationMethods = pDeviceInfo->ConfigMethods;

        m_AdvertisementParameters.DeviceInformation.DeviceInfoParameters.DeviceType.CategoryID = pDeviceInfo->PrimaryDeviceType.CategoryID;
        m_AdvertisementParameters.DeviceInformation.DeviceInfoParameters.DeviceType.SubcategoryID = pDeviceInfo->PrimaryDeviceType.SubCategoryID;

        RtlCopyMemory (&m_AdvertisementParameters.DeviceInformation.DeviceInfoParameters.DeviceType.OUI,
                    pDeviceInfo->PrimaryDeviceType.OUI,
                    sizeof(m_AdvertisementParameters.DeviceInformation.DeviceInfoParameters.DeviceType.OUI));

        m_AdvertisementParameters.DeviceInformation.DeviceName.pElements = pDeviceInfo->DeviceName.ucDeviceName;
        m_AdvertisementParameters.DeviceInformation.DeviceName.ElementCount = pDeviceInfo->DeviceName.uDeviceNameLength;
    }
    break;
    case WiFiJobP2PSetGroupOwnerCapability:
    {
        BOOLEAN bGroupOwner = FALSE;
        ULONG uOpmodeMask = 0;
        NDIS_OBJECT_HEADER *pNdisHeader = (NDIS_OBJECT_HEADER *) pInformationBuffer;

        //
        // Check if this port is operating as a GO
        //
        status = GetPortPropertyCache()->GetPropertyULong(WfcPortCurrentOperationalMode, &uOpmodeMask);
        if ((status == NDIS_STATUS_SUCCESS) &&
            (uOpmodeMask & WDI_OPERATION_MODE_P2P_GO))
        {
            bGroupOwner = TRUE;
        }

        m_AdvertisementParameters.GroupOwnerCapability.OSSetGroupCapabilities = 0;

        switch (pNdisHeader->Revision)
        {
            case (DOT11_WFD_GROUP_OWNER_CAPABILITY_CONFIG_REVISION_1):
            {
                DOT11_WFD_GROUP_OWNER_CAPABILITY_CONFIG* pGroupCaps = (DOT11_WFD_GROUP_OWNER_CAPABILITY_CONFIG*)
                                                                        pInformationBuffer;

                m_AdvertisementParameters.GroupOwnerCapability.GroupCapabilityBitmap =
                    CDot11ToWabiConverter::MapWFDGroupCapabilities(
                        bGroupOwner,                                        // bGroupOwner
                        pGroupCaps->bPersistentGroupEnabled,                // bPersistentGroupEnabled
                        FALSE,                                              // bGroupLimitReached
                        pGroupCaps->bIntraBSSDistributionSupported,         // bIntraBSSDistributionSupported
                        pGroupCaps->bCrossConnectionSupported,              // bCrossConnectionSupported
                        pGroupCaps->bPersistentReconnectSupported,          // bPersistentReconnectSupported
                        pGroupCaps->bGroupFormationEnabled,                 // bGroupFormationEnabled
                        FALSE);                                             // bEapolKeyIpAddressAllocationSupported

                m_AdvertisementParameters.GroupOwnerCapability.MaximumGroupLimit = pGroupCaps->uMaximumGroupLimit;
            }
            break;

            case (DOT11_WFD_GROUP_OWNER_CAPABILITY_CONFIG_REVISION_2):
            {
                DOT11_WFD_GROUP_OWNER_CAPABILITY_CONFIG_V2* pGroupCaps = (DOT11_WFD_GROUP_OWNER_CAPABILITY_CONFIG_V2*)
                                                                         pInformationBuffer;

                m_AdvertisementParameters.GroupOwnerCapability.GroupCapabilityBitmap =
                    CDot11ToWabiConverter::MapWFDGroupCapabilities(
                        bGroupOwner,                                        // bGroupOwner
                        pGroupCaps->bPersistentGroupEnabled,                // bPersistentGroupEnabled
                        FALSE,                                              // bGroupLimitReached
                        pGroupCaps->bIntraBSSDistributionSupported,         // bIntraBSSDistributionSupported
                        pGroupCaps->bCrossConnectionSupported,              // bCrossConnectionSupported
                        pGroupCaps->bPersistentReconnectSupported,          // bPersistentReconnectSupported
                        pGroupCaps->bGroupFormationEnabled,                 // bGroupFormationEnabled
                        pGroupCaps->bEapolKeyIpAddressAllocationSupported); // bEapolKeyIpAddressAllocationSupported

                m_AdvertisementParameters.GroupOwnerCapability.OSSetGroupCapabilities |=
                    DOT11_WFD_GROUP_CAPABILITY_EAPOL_KEY_IP_ADDRESS_ALLOCATION_SUPPORTED;

                m_AdvertisementParameters.GroupOwnerCapability.MaximumGroupLimit = pGroupCaps->uMaximumGroupLimit;
            }
            break;

            default:
            {
                status = NDIS_STATUS_INVALID_DATA;
                WFCActErrorStatus("Invalid Revision value = %d\n", pNdisHeader->Revision);
                goto exit;
            }
        }

        m_AdvertisementParameters.GroupOwnerCapability.OSSetGroupCapabilities |=
            (DOT11_WFD_GROUP_CAPABILITY_GROUP_OWNER |
             DOT11_WFD_GROUP_CAPABILITY_PERSISTENT_GROUP |
             DOT11_WFD_GROUP_CAPABILITY_INTRABSS_DISTRIBUTION_SUPPORTED |
             DOT11_WFD_GROUP_CAPABILITY_CROSS_CONNECTION_SUPPORTED |
             DOT11_WFD_GROUP_CAPABILITY_PERSISTENT_RECONNECT_SUPPORTED |
             DOT11_WFD_GROUP_CAPABILITY_IN_GROUP_FORMATION);

        m_AdvertisementParameters.Optional.GroupOwnerCapability_IsPresent = 1; 

        WFCActInfo("[SetGroupOwnerCapability]: Group:OSSetCapabilities = <0x%08x : 0x%08x>",
            m_AdvertisementParameters.GroupOwnerCapability.GroupCapabilityBitmap,
            m_AdvertisementParameters.GroupOwnerCapability.OSSetGroupCapabilities);
    }
    break;
    case WiFiJobP2PSetSecondaryDeviceTypeList:
    {
        DOT11_WFD_SECONDARY_DEVICE_TYPE_LIST* pDeviceTypeList = (DOT11_WFD_SECONDARY_DEVICE_TYPE_LIST*)pInformationBuffer;

        m_AdvertisementParameters.Optional.SecondaryDeviceTypeList_IsPresent = 1;

        //
        // The structure definition for WDI_P2P_DEVICE_TYPE and
        // DOT11_WFD_DEVICE_TYPE are identical, so this is safe and
        // avoids extra allocation.
        //
        C_ASSERT(sizeof(WDI_P2P_DEVICE_TYPE) == sizeof(DOT11_WFD_DEVICE_TYPE));
        m_AdvertisementParameters.SecondaryDeviceTypeList.pElements = (WDI_P2P_DEVICE_TYPE*)((PVOID)&pDeviceTypeList->SecondaryDeviceTypes);
        m_AdvertisementParameters.SecondaryDeviceTypeList.ElementCount = pDeviceTypeList->uNumOfEntries;
    }
    break;
    case WiFiJobP2PSetAdvertisementList:
    {
        DWORD   i;
        DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST* pDot11FullAdvList = (DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST*)pInformationBuffer;

#ifdef OLDER_OS_COMPAT
        status = CheckAdvListForOldVersion(m_InformationBufferLength, pInformationBuffer, &m_InformationBufferLength, &pDot11FullAdvList);
        if (NDIS_STATUS_SUCCESS != status)
        {
            goto exit;
        }

        if (pInformationBuffer != (PVOID) pDot11FullAdvList)
        {
            m_pInformationBuffer = pDot11FullAdvList;
        }
#endif  // OLDER_OS_COMPAT

        if ((sizeof(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST) +
             pDot11FullAdvList->uAdvertisedServicesLength +
             pDot11FullAdvList->uAdvertisedServicePrefixesLength) > m_InformationBufferLength)
        {
            status = NDIS_STATUS_INVALID_DATA;
            WFCActErrorStatus("BufferLength=<%d> < ServicesOffset+ServicesLength+PrefixLength=<%d+%d+%d>\n",
                m_InformationBufferLength, sizeof(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST),
                pDot11FullAdvList->uAdvertisedServicesLength, pDot11FullAdvList->uAdvertisedServicePrefixesLength);
            goto exit;
        }

        WFCActInfo("Got %d Services and %d Prefixes",
            pDot11FullAdvList->uAdvertisedServicesCount, pDot11FullAdvList->uAdvertisedServicePrefixesCount);
        m_AdvertisementParameters.Optional.AdvertisedServices_IsPresent = 1;

        if (pDot11FullAdvList->uAdvertisedServicesCount)
        {
            m_AdvertisementParameters.AdvertisedServices.Optional.ServiceEntry_IsPresent = 1;

            status = m_AdvertisementParameters.AdvertisedServices.ServiceEntry.AllocateElements(
                pDot11FullAdvList->uAdvertisedServicesCount,
                0);
            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCActErrorStatus("Couldn't allocate %d AdvertisedService elements",
                    pDot11FullAdvList->uAdvertisedServicesCount);
                goto exit;
            }
        }

        DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_ENTRY *pDot11SvcEntry = (PDOT11_WFD_SERVICE_FULL_ADVERTISEMENT_ENTRY)(((PBYTE)pDot11FullAdvList) + pDot11FullAdvList->uAdvertisedServicesOffset);
        for (i=0; i<pDot11FullAdvList->uAdvertisedServicesCount; i++)
        {
            WDI_P2P_ADVERTISED_SERVICE_ENTRY_CONTAINER *pWdiSvcContainer = &m_AdvertisementParameters.AdvertisedServices.ServiceEntry.pElements[i];
            
            if (((pDot11SvcEntry->uServiceNameOffset+pDot11SvcEntry->uServiceNameLength) > m_InformationBufferLength) ||
                ((pDot11SvcEntry->uServiceInformationOffset+pDot11SvcEntry->uServiceInformationLength) > m_InformationBufferLength))
            {
                WFCActErrorStatus("[%d] BAD Entry NameOffset+Length=<%d+%d>, InfoOffset+Length=<%d+%d>, MaxLength=<%d>",
                    i, pDot11SvcEntry->uServiceNameOffset, pDot11SvcEntry->uServiceNameLength,
                    pDot11SvcEntry->uServiceInformationOffset, pDot11SvcEntry->uServiceInformationLength,
                    m_InformationBufferLength);
                goto exit;
            }

            RtlCopyMemory (&pWdiSvcContainer->ServiceNameHash,
                           &pDot11SvcEntry->ServiceHash,
                           sizeof(pWdiSvcContainer->ServiceNameHash));
            pWdiSvcContainer->ServiceName.ElementCount = pDot11SvcEntry->uServiceNameLength;
            pWdiSvcContainer->ServiceName.pElements = ((PBYTE) pDot11FullAdvList) + pDot11SvcEntry->uServiceNameOffset;

            if (pDot11SvcEntry->uServiceInformationLength)
            {
                pWdiSvcContainer->ServiceInformation.ElementCount = pDot11SvcEntry->uServiceInformationLength;
                pWdiSvcContainer->ServiceInformation.pElements = ((PBYTE) pDot11FullAdvList) + pDot11SvcEntry->uServiceInformationOffset;

                pWdiSvcContainer->Optional.ServiceInformation_IsPresent = TRUE;
            }

            pWdiSvcContainer->ServiceStatus = pDot11SvcEntry->ServiceStatus;
            pWdiSvcContainer->AdvertisementID = pDot11SvcEntry->AdvertisementID;
            pWdiSvcContainer->ConfigurationMethods = (USHORT) pDot11SvcEntry->ConfigMethods;

            pDot11SvcEntry++;
        }

        if (pDot11FullAdvList->uAdvertisedServicePrefixesCount)
        {
            m_AdvertisementParameters.AdvertisedServices.Optional.ServicePrefix_IsPresent = 1;

            status = m_AdvertisementParameters.AdvertisedServices.ServicePrefix.AllocateElements(
                pDot11FullAdvList->uAdvertisedServicePrefixesCount,
                0);
            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCActErrorStatus("Couldn't allocate %d AdvertisedPrefix elements",
                    pDot11FullAdvList->uAdvertisedServicePrefixesCount);
                goto exit;
            }
        }

        DOT11_WFD_SERVICE_PREFIX_ENTRY *pDot11PrefixEntry = (PDOT11_WFD_SERVICE_PREFIX_ENTRY)(((PBYTE)pDot11FullAdvList) + pDot11FullAdvList->uAdvertisedServicePrefixesOffset);
        for (i=0; i<pDot11FullAdvList->uAdvertisedServicePrefixesCount; i++)
        {
            WDI_P2P_ADVERTISED_PREFIX_ENTRY_CONTAINER *pWdiPrefixContainer = &m_AdvertisementParameters.AdvertisedServices.ServicePrefix.pElements[i];

            if ((pDot11PrefixEntry->uServicePrefixOffset+pDot11PrefixEntry->uServicePrefixLength) > m_InformationBufferLength)
            {
                WFCActErrorStatus("[%d] BAD Entry PrefixOffset+Length=<%d+%d>, MaxLength=<%d>",
                    i, pDot11PrefixEntry->uServicePrefixOffset, pDot11PrefixEntry->uServicePrefixLength,
                    m_InformationBufferLength);
                goto exit;
            }

            RtlCopyMemory (&pWdiPrefixContainer->ServicePrefixHash,
                           &pDot11PrefixEntry->ServicePrefixHash,
                           sizeof(pWdiPrefixContainer->ServicePrefixHash));
            pWdiPrefixContainer->PrefixName.ElementCount = pDot11PrefixEntry->uServicePrefixLength;
            pWdiPrefixContainer->PrefixName.pElements = ((PBYTE) pDot11FullAdvList) + pDot11PrefixEntry->uServicePrefixOffset;

            pDot11PrefixEntry++;
        }

        m_AdvertisementParameters.AdvertisedServices.ServiceUpdateIndicator = pDot11FullAdvList->uServiceUpdateIndicator;
    }
    break;
    default:
        status = NDIS_STATUS_FAILURE;
        NT_ASSERT(FALSE);
        goto exit;
        break;
    };

    *pBytesRead = InformationBufferLength;
    *pMessageID = WDI_SET_ADVERTISEMENT_INFORMATION;
    *pCommandBufferLength = 0;

    status = GenerateWdiSetAdvertisementInformation(
        &m_AdvertisementParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer );

exit:
    TraceActExit(status);

    return status;
}

NDIS_STATUS CSetWFDAdvertisementSettings::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader)
{
    NDIS_STATUS status = OperationStatus;
    WFC_PORT_PROPERTY_NAME portProperty;

    UNREFERENCED_PARAMETER(pMessageHeader);
    UNREFERENCED_PARAMETER(MessageLength);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    //
    // Store what was sent down for debugging purposes
    //
    switch(m_JobType)
    {
    case WiFiJobP2PSetDeviceCapability:
        portProperty = WfcPortPropertyP2PDeviceCaps;
        break;
    case WiFiJobP2PSetAdditionalIE:
        portProperty = WfcPortPropertyP2PAdditionalIEs;
        break;
    case WiFiJobP2PSetDeviceInfo:
        portProperty = WfcPortPropertyP2PDeviceInfo;
        break;
    case WiFiJobP2PSetGroupOwnerCapability:
        portProperty = WfcPortPropertyP2PGroupCaps;
        break;
    case WiFiJobP2PSetSecondaryDeviceTypeList:
        portProperty = WfcPortPropertyP2PSecondaryDeviceTypeList;
        break;
    case WiFiJobP2PSetAdvertisementList:
        portProperty = WfcPortPropertyP2PAdvertisementList;
        break;
    default:
        status = NDIS_STATUS_FAILURE;
        NT_ASSERT(FALSE);
        goto exit;
        break;
    };

    if ( GetPortPropertyCache()->SetPropertyBuffer(
             portProperty,
             m_InformationBufferLength,
             (BYTE*)m_pInformationBuffer ) != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Couldn't store buffer for advertisement information in property cache!");
    }

exit:
#ifdef OLDER_OS_COMPAT
    if ((WiFiJobP2PSetAdvertisementList == m_JobType) &&
        (m_pInformationBuffer != SET_OID_BUFFER(m_pOidRequest)))
    {
        delete [] m_pInformationBuffer;
        m_pInformationBuffer = SET_OID_BUFFER(m_pOidRequest);
    }
#endif  // OLDER_OS_COMPAT
    return status;
}

NDIS_STATUS CSetWFDListenState::GetDeviceCommandData(
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

    switch(m_JobType)
    {
        case WiFiJobP2PSetListenState:
        {
            m_ListenStateParameters.ListenState = *((WDI_P2P_LISTEN_STATE*)pInformationBuffer);
            *pBytesRead = sizeof(UINT32);

            break;
        }

        case WiFiJobP2PSetListenStatePassiveAvailability:
        {
            DOT11_WFD_SET_LISTEN_STATE_PASSIVE_AVAILABILITY *pListenPassiveAvailability = 
                (DOT11_WFD_SET_LISTEN_STATE_PASSIVE_AVAILABILITY *) pInformationBuffer;

            if (InformationBufferLength < sizeof(DOT11_WFD_SET_LISTEN_STATE_PASSIVE_AVAILABILITY))
            {
                status = NDIS_STATUS_INVALID_DATA;
                WFCActErrorStatus ("BufferLength=%d < %d\n",
                    InformationBufferLength, sizeof(DOT11_WFD_SET_LISTEN_STATE_PASSIVE_AVAILABILITY));
                goto Exit;
            }

            m_ListenStateParameters.ListenState = WDI_P2P_LISTEN_STATE_PASSIVE_AVAILABILITY;
            m_ListenStateParameters.Optional.ListenChannel_IsPresent = 1;

            status = CDot11ToWabiConverter::MapP2PChannel( &pListenPassiveAvailability->Channel,
                &m_ListenStateParameters.ListenChannel);
            if (NDIS_STATUS_SUCCESS != status)
            {
                WFCActErrorStatus("MapP2PChannel FAIled");
                goto Exit;
            }

            m_ListenStateParameters.ListenDuration.CycleDuration = pListenPassiveAvailability->ListenDuration.CycleDuration;
            m_ListenStateParameters.ListenDuration.ListenActiveDuration = pListenPassiveAvailability->ListenDuration.ListenActiveDuration;

            *pBytesRead = sizeof(DOT11_WFD_SET_LISTEN_STATE_PASSIVE_AVAILABILITY);

            break;
        }

        default:
        {
            status = NDIS_STATUS_FAILURE;
            NT_ASSERT(FALSE);
            goto Exit;
        }
    }

    *pCommandId = WDI_SET_P2P_LISTEN_STATE;
    *pCommandBufferLength = 0;

    status = GenerateWdiSetP2pListenState(
        &m_ListenStateParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer
        );

Exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CSetWFDListenState::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader )
{
    UNREFERENCED_PARAMETER(MessageLength);
    UNREFERENCED_PARAMETER(pMessageHeader);

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    WFCActInfo("CSetWFDListenState::OnDeviceCommandCompletion: Setting Listen state to <%d>\n",
        m_ListenStateParameters.ListenState);
    if ( m_pAdapter->GetAdapterPropertyCache()->SetPropertyT(
             WfcAdapterPropertyCurrentListenState,
             m_ListenStateParameters.ListenState ) != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Couldn't store listen state in property cache!");
    }

exit:
    return OperationStatus;
}

NDIS_STATUS CP2PDisconnectJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST )
{
    return NDIS_STATUS_SUCCESS;
}

void CP2PDisconnectJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus )
{
    TraceActEntry();

    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus );

    TraceActExit(NdisStatus);
}


NDIS_STATUS CP2PDisconnectJob::QueryNextCommandInfo(
    _In_ ULONG ReservedHeaderLength,
    _Inout_ WFC_DEVICE_COMMAND_TOKEN* pCommandToken,
    _Out_ WFC_DEVICE_COMMAND_TYPE* pCommandType,
    _Out_ UINT32* pMessageID,
    _Out_ ULONG* pCommandBufferLength,
    _Outptr_result_buffer_(*pCommandBufferLength) BYTE** ppCommandBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    DOT11_DISASSOCIATE_PEER_REQUEST* pDisassocPeerInfo =
        (DOT11_DISASSOCIATE_PEER_REQUEST*) SET_OID_BUFFER(m_pOidRequest);

    TraceActEntry();

    RtlCopyMemory(
        &m_DisconnectParams.DisconnectParameters.MacAddress,
        pDisassocPeerInfo->PeerMacAddr,
        sizeof(m_DisconnectParams.DisconnectParameters.MacAddress) );

    m_DisconnectParams.DisconnectParameters.Disassociation80211Reason =
        pDisassocPeerInfo->usReason;

    status = GenerateWdiTaskDisconnect(
        &m_DisconnectParams,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        ppCommandBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't generate disconnect task TLV blob.");
        *pCommandType = WfcDeviceCommandTypeInvalid;
        *pMessageID = 0;
        goto exit;
    }

    *pCommandToken = NULL;
    *pCommandType = WfcDeviceCommandTypeTask;
    *pMessageID = WDI_TASK_DISCONNECT;

exit:
    TraceActExit(status);
    return status;
}

void CP2PDisconnectJob::OnNotifyJobCompletion(
    _In_ WFC_DEVICE_COMMAND_TOKEN,
    _In_ NDIS_STATUS OperationStatus )
{
    NDIS_STATUS status = OperationStatus;

    TraceActEntry();

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Could not disconnect peer!.");
    }
    else
    {
        WFCActInfo("Peer successfully.");
    }
    
    TraceActExit(status);
}

//==============================================================================================

/*
  Handles the following jobs:
    WiFiJobP2PGetAdvertisementList
*/

NDIS_STATUS CGetWFDAdvertisementSettings::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded)
{
    UNREFERENCED_PARAMETER(pBytesNeeded);
    UNREFERENCED_PARAMETER(InformationBufferLength);

    NDIS_STATUS status;
    ULONG i, uBytes = 0, uServiceNameOffset;
    DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST *pDot11FullAdvList = NULL;
    DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_ENTRY *pDot11FullAdvEntry = NULL;
    DOT11_WFD_SERVICE_BASIC_ADVERTISEMENT_LIST *pBasicAdvList = NULL;

    TraceActEntry();

    switch (m_JobType)
    {
        case (WiFiJobP2PGetAdvertisementList):
        {
            status = GetPortPropertyCache()->GetPropertyBuffer(
                WfcPortPropertyP2PAdvertisementList,
                &uBytes,
                (PBYTE *) &pDot11FullAdvList);
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCActErrorStatus("FAIled to find WfcPortPropertyP2PAdvertisementList in port Property Cache.");
                goto Exit;
            }

            if ((!uBytes) ||
                (uBytes < sizeof(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST)) ||
                (uBytes < (sizeof(DOT11_WFD_SERVICE_FULL_ADVERTISEMENT_LIST) +
                                  pDot11FullAdvList->uAdvertisedServicesLength +
                                  pDot11FullAdvList->uAdvertisedServicePrefixesLength)))
            {
                status = NDIS_STATUS_INVALID_DATA;
                WFCActErrorStatus("Invalid AdvertisementList in property cache.");
                goto Exit;
            }

            pDot11FullAdvEntry = (PDOT11_WFD_SERVICE_FULL_ADVERTISEMENT_ENTRY)(((PBYTE)pDot11FullAdvList) + pDot11FullAdvList->uAdvertisedServicesOffset);

            //
            // Compute the required services length
            //
            uBytes = pDot11FullAdvList->uAdvertisedServicesCount * sizeof(DOT11_WFD_SERVICE_BASIC_ADVERTISEMENT_ENTRY);
            for (i=0; i<pDot11FullAdvList->uAdvertisedServicesCount; i++)
            {
                uBytes += pDot11FullAdvEntry[i].uServiceNameLength;
            }

            pBasicAdvList = (DOT11_WFD_SERVICE_BASIC_ADVERTISEMENT_LIST *) pInformationBuffer;
            *pBytesNeeded = FIELD_OFFSET(DOT11_WFD_SERVICE_BASIC_ADVERTISEMENT_LIST,AdvertisedServices) + uBytes;
            if(InformationBufferLength < *pBytesNeeded)
            {
                if (InformationBufferLength >=
                    FIELD_OFFSET(DOT11_WFD_SERVICE_BASIC_ADVERTISEMENT_LIST,AdvertisedServices))
                {
                    pBasicAdvList->uAdvertisedServicesLength = uBytes;
                }

                status = NDIS_STATUS_BUFFER_OVERFLOW;
                WFCActError("CGetWFDAdvertisementSettings: QueryData. Buffer too short <%d < %d>, Error = 0x%08x",
                    InformationBufferLength, *pBytesNeeded, status);
                goto Exit;
            }

            pBasicAdvList->Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
            pBasicAdvList->Header.Revision = DOT11_WFD_SERVICE_ADVERTISEMENT_LIST_REVISION_1;
            pBasicAdvList->Header.Size = DOT11_SIZEOF_WFD_SERVICE_GET_ADVERTISEMENT_LIST_REVISION_1;
            pBasicAdvList->uAdvertisedServicesCount = pDot11FullAdvList->uAdvertisedServicesCount;
            pBasicAdvList->uAdvertisedServicesLength = uBytes;

            WFCActInfo("AdvertisedServices Count:Length = %d:%d, BytesNeeded = %d\n",
                pBasicAdvList->uAdvertisedServicesCount, pBasicAdvList->uAdvertisedServicesLength, *pBytesNeeded);

            uServiceNameOffset = sizeof(DOT11_WFD_SERVICE_BASIC_ADVERTISEMENT_LIST) +
                                 (pDot11FullAdvList->uAdvertisedServicesCount * sizeof(DOT11_WFD_SERVICE_BASIC_ADVERTISEMENT_ENTRY));
            for (i=0; i<pDot11FullAdvList->uAdvertisedServicesCount; i++)
            {
                pBasicAdvList->AdvertisedServices[i].ServiceStatus = pDot11FullAdvEntry[i].ServiceStatus;
                pBasicAdvList->AdvertisedServices[i].AdvertisementID = pDot11FullAdvEntry[i].AdvertisementID;
                pBasicAdvList->AdvertisedServices[i].uServiceNameLength = pDot11FullAdvEntry[i].uServiceNameLength;
                pBasicAdvList->AdvertisedServices[i].uServiceNameOffset = uServiceNameOffset;
                NdisMoveMemory (((PBYTE) pBasicAdvList) + uServiceNameOffset,
                                ((PBYTE)pDot11FullAdvList) + pDot11FullAdvEntry[i].uServiceNameOffset,
                                pDot11FullAdvEntry[i].uServiceNameLength);

                uServiceNameOffset += pDot11FullAdvEntry[i].uServiceNameLength;
            }

            *pBytesWritten = *pBytesNeeded;
            break;
        }

        default:
        {
            status = NDIS_STATUS_FAILURE;
            NT_ASSERT(FALSE);
            break;
        }
    }

Exit:
    TraceActExit(status);
    return status;
}


//==============================================================================================

/*
  Handles the following jobs:
    WiFiJobP2PStartBackgroundDiscovery
*/

NDIS_STATUS CSetWFDStartBackgroundDiscovery::BuildBackgroundDiscoveryWdiBuffer(
    _In_  UINT uDot11BufferLength,
    _In_reads_bytes_(uDot11BufferLength) DOT11_WFD_BACKGROUND_DISCOVERY_REQUEST *pDot11WFDDiscovery,
    _Out_ WDI_SET_P2P_START_BACKGROUND_DISCOVERY_PARAMETERS *pWdiDiscoveryParameters
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    DWORD       i, dwOffset;

    //
    // Verify that the basic struct is present
    //
    if (uDot11BufferLength < sizeof(DOT11_WFD_BACKGROUND_DISCOVERY_REQUEST))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCError("Invalid BackgroundDiscovery struct, uDot11BufferLength=%d/%d",
            uDot11BufferLength, sizeof(DOT11_WFD_BACKGROUND_DISCOVERY_REQUEST));
            
        goto Exit;
    }

    //
    // Now, verify the optional fields
    //
    if ((uDot11BufferLength < pDot11WFDDiscovery->uDeviceFilterListOffset +
                              (pDot11WFDDiscovery->uNumDeviceFilters * sizeof(WDI_MAC_ADDRESS))) ||
        (uDot11BufferLength < pDot11WFDDiscovery->uChannelInfoListOffset +
                              (pDot11WFDDiscovery->uNumChannelInfoElements *
                               sizeof(DOT11_WFD_REMOTE_CHANNEL_AVAILABILITY_INFO))) ||
        (uDot11BufferLength < pDot11WFDDiscovery->uServiceHashListOffset +
                              (pDot11WFDDiscovery->uNumServiceHashes*sizeof(DOT11_WFD_SERVICE_HASH))) ||
        (uDot11BufferLength < (pDot11WFDDiscovery->uIEsOffset + pDot11WFDDiscovery->uIEsLength)))
    {
        WFCError("BGDiscovery BuffLen=<%d>: DevFilters=<%d+%d*%d>, Channels=<%d+%d*%d>, Hashes=<%d+%d*%d>, IEs=<%d+%d>",
            uDot11BufferLength,
            pDot11WFDDiscovery->uDeviceFilterListOffset, pDot11WFDDiscovery->uNumDeviceFilters,
            sizeof(WDI_MAC_ADDRESS),
            pDot11WFDDiscovery->uChannelInfoListOffset, pDot11WFDDiscovery->uNumChannelInfoElements,
            sizeof(DOT11_WFD_REMOTE_CHANNEL_AVAILABILITY_INFO),
            pDot11WFDDiscovery->uServiceHashListOffset, pDot11WFDDiscovery->uNumServiceHashes,
            sizeof(DOT11_WFD_SERVICE_HASH),
            pDot11WFDDiscovery->uIEsOffset, pDot11WFDDiscovery->uIEsLength);

        status = NDIS_STATUS_INVALID_DATA;
        goto Exit;
    }

    WFCInfo ("BuildBackgroundDiscoveryWdiBuffer: Type=<%d>, SvcType=<%d>, Timeout=<%d>, Filters=<%d>, Channels=<%d>\n",
        pDot11WFDDiscovery->DiscoveryType, pDot11WFDDiscovery->ServiceDiscoveryType,
        pDot11WFDDiscovery->uDeviceEntryTimeout, pDot11WFDDiscovery->uNumDeviceFilters,
        pDot11WFDDiscovery->uNumChannelInfoElements);

    pWdiDiscoveryParameters->DiscoverMode.DiscoveryType =
        CDot11ToWabiConverter::MapDiscoverType( pDot11WFDDiscovery->DiscoveryType );
    pWdiDiscoveryParameters->DiscoverMode.ServiceDiscoveryType =
        CDot11ToWabiConverter::MapServiceDiscoveryType( pDot11WFDDiscovery->ServiceDiscoveryType );
    pWdiDiscoveryParameters->DiscoverMode.DeviceVisiblilityTimeout = pDot11WFDDiscovery->uDeviceEntryTimeout;

    if (pDot11WFDDiscovery->uNumDeviceFilters)
    {
        pWdiDiscoveryParameters->Optional.DeviceFilterList_IsPresent = 1;
        pWdiDiscoveryParameters->DeviceFilterList.SimpleAssign(
            (WDI_MAC_ADDRESS*) (((PBYTE) pDot11WFDDiscovery) + pDot11WFDDiscovery->uDeviceFilterListOffset),
            pDot11WFDDiscovery->uNumDeviceFilters);
    }

    if (pDot11WFDDiscovery->uNumChannelInfoElements)
    {
        status = pWdiDiscoveryParameters->DiscoveryChannelSettings.AllocateElements(
            pDot11WFDDiscovery->uNumChannelInfoElements, 0);
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to allocate memory for %d DiscoveryChannelSettings. Error = 0x%08x",
                pDot11WFDDiscovery->uNumChannelInfoElements, status);
            goto Exit;
        }

        pWdiDiscoveryParameters->Optional.DiscoveryChannelSettings_IsPresent = 1;
        dwOffset = pDot11WFDDiscovery->uChannelInfoListOffset;
        for (i=0; i<pDot11WFDDiscovery->uNumChannelInfoElements; i++)
        {
            status = CDot11ToWabiConverter::MapWFDChannelInfo(
                (PBYTE) pDot11WFDDiscovery,
                uDot11BufferLength,
                &dwOffset,
                (PDOT11_WFD_REMOTE_CHANNEL_AVAILABILITY_INFO) (((PBYTE) pDot11WFDDiscovery) + dwOffset),
                &pWdiDiscoveryParameters->DiscoveryChannelSettings.pElements[i]);
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCError("MapWFDChannelInfo FAILed. Error = 0x%08x", status);
                goto Exit;
            }
        }
    }

    if (pDot11WFDDiscovery->uNumServiceHashes)
    {
        pWdiDiscoveryParameters->Optional.ServiceNameHash_IsPresent = 1;

        C_ASSERT(sizeof(WDI_P2P_SERVICE_NAME_HASH_CONTAINER) == sizeof(DOT11_WFD_SERVICE_HASH));
        pWdiDiscoveryParameters->ServiceNameHash.SimpleAssign(
            (WDI_P2P_SERVICE_NAME_HASH_CONTAINER*) (((PBYTE) pDot11WFDDiscovery) +
                                                    pDot11WFDDiscovery->uServiceHashListOffset),
            pDot11WFDDiscovery->uNumServiceHashes);
    }

    if (pDot11WFDDiscovery->uIEsLength)
    {
        pWdiDiscoveryParameters->Optional.VendorIEs_IsPresent = 1;
        pWdiDiscoveryParameters->VendorIEs.pElements = ((PBYTE)pDot11WFDDiscovery)+pDot11WFDDiscovery->uIEsOffset;
        pWdiDiscoveryParameters->VendorIEs.ElementCount = pDot11WFDDiscovery->uIEsLength;
    }

Exit:
    TraceExit(status);
    return status;
}

NDIS_STATUS CSetWFDStartBackgroundDiscovery::GetDeviceCommandData(
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
    DOT11_WFD_BACKGROUND_DISCOVERY_REQUEST *pDot11WFDDiscovery = 
        (DOT11_WFD_BACKGROUND_DISCOVERY_REQUEST *) pInformationBuffer;

    UNREFERENCED_PARAMETER(pBytesNeeded);
    UNREFERENCED_PARAMETER(InformationBufferLength);

    TraceActEntry();

    status = CSetWFDStartBackgroundDiscovery::BuildBackgroundDiscoveryWdiBuffer(
        InformationBufferLength,
        pDot11WFDDiscovery,
        &m_BackgroundDiscoveryParameters);
    if (status != NDIS_STATUS_SUCCESS)
    {
        goto Exit;
    }

    WFCActInfo ("DiscType=<%d>, SvcDiscType=<%d> Timeout=<%d>\n",
            pDot11WFDDiscovery->DiscoveryType, pDot11WFDDiscovery->ServiceDiscoveryType,
            pDot11WFDDiscovery->uDeviceEntryTimeout);

    *pBytesRead = InformationBufferLength;
    *pCommandId = WDI_SET_P2P_START_BACKGROUND_DISCOVERY;
    *pCommandBufferLength = 0;

    status = GenerateWdiSetP2pStartBackgroundDiscovery(
        &m_BackgroundDiscoveryParameters,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer
        );

Exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CSetWFDStartBackgroundDiscovery::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader )
{
    UNREFERENCED_PARAMETER(MessageLength);
    UNREFERENCED_PARAMETER(pMessageHeader);

    TraceActEntry();

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        goto Exit;
    }

    if ( GetPortPropertyCache()->SetPropertyBuffer(
             WfcPortPropertyP2PBackgroundDiscoverySettings,
             m_InformationBufferLength,
             (BYTE*)m_pInformationBuffer ) != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Couldn't store buffer for Background discovery information in property cache!");
        goto Exit;
    }

    //
    // Have the Services manager start tracking the services
    //
    m_pAdapter->GetServicesManager()->SetNewBackgroundDiscoveryContexts(
        TRUE,                                     // Indicate for every device / service change
        m_InformationBufferLength,
        (PBYTE) m_pInformationBuffer);

Exit:
    TraceActExit(OperationStatus);
    return OperationStatus;
}
//==============================================================================================

/*
  Handles the following jobs:
    WiFiJobP2PStopBackgroundDiscovery
*/

NDIS_STATUS CSetWFDStopBackgroundDiscovery::InitializeFromJob(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    status = COidJobBase::InitializeWithoutOid(
        WiFiJobP2PStopBackgroundDiscovery,
        pAdapter,
        FALSE,                                  // IsPreemptable
        TRUE,                                   // CanPreempt
        WiFiPriorityScopeAdapter,               // PriorityScope
        WiFiSerializedJobPriority_5,            // Priority
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

NDIS_STATUS CSetWFDStopBackgroundDiscovery::GetDeviceCommandData(
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
    WDI_SET_P2P_STOP_BACKGROUND_DISCOVERY_PARAMETERS WdiStopDiscovery = {0};

    UNREFERENCED_PARAMETER(pInformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(pBytesNeeded);

    TraceActEntry();

    *pBytesRead = 0;
    *pCommandId = WDI_SET_P2P_STOP_BACKGROUND_DISCOVERY;
    *pCommandBufferLength = 0;

    status = GenerateWdiSetP2pStopBackgroundDiscovery(
        &WdiStopDiscovery,
        ReservedHeaderLength,
        m_pAdapter->GetTlvContext( ),
        pCommandBufferLength,
        pCommandBuffer
        );

    TraceActExit(status);
    return status;
}

NDIS_STATUS CSetWFDStopBackgroundDiscovery::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader )
{
    DOT11_WFD_BACKGROUND_DISCOVERY_REQUEST Dot11WFDDiscovery = {0};

    UNREFERENCED_PARAMETER(MessageLength);
    UNREFERENCED_PARAMETER(pMessageHeader);

    TraceActEntry();

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        goto Exit;
    }

    //
    // Leave all the fields 0 except for the Ndis header
    //
    Dot11WFDDiscovery.Header.Type = NDIS_OBJECT_TYPE_DEFAULT;
    Dot11WFDDiscovery.Header.Revision = DOT11_WFD_START_BACKGROUND_DISCOVERY_REVISION_1;
    Dot11WFDDiscovery.Header.Size = DOT11_SIZEOF_WFD_START_BACKGROUND_DISCOVERY_REVISION_1;

    if ( GetPortPropertyCache()->SetPropertyBuffer(
             WfcPortPropertyP2PBackgroundDiscoverySettings,
             sizeof(Dot11WFDDiscovery),
             (BYTE*)&Dot11WFDDiscovery ) != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Couldn't store buffer for Background discovery information in property cache!");
    }

    //
    // Let the Services manager know to stop tracking the services
    //
    m_pAdapter->GetServicesManager()->SetNewBackgroundDiscoveryContexts(
        FALSE,                                    // Stop indicating device / service changes
        sizeof(Dot11WFDDiscovery),
        (PBYTE) &Dot11WFDDiscovery);

Exit:
    TraceActExit(OperationStatus);
    return OperationStatus;
}

