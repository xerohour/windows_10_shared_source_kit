/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Request.cpp

Abstract:

    Implementation for generic request handler

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "Request.tmh"

#include "OidTable_.hpp"

//==============================================================================================
COidJobNotificationHandler::COidJobNotificationHandler()
{
    // Do nothing
}

void COidJobNotificationHandler::OnOperationStarted(
    _In_ NDIS_STATUS OperationStatus,
    _In_opt_ WFC_SENDER_OBJECT SenderObject )
{
    CJobBase* pJobBase = (CJobBase*)SenderObject;

    // The job is really a OID base
    COidJobBase* pOidJob = (COidJobBase *)pJobBase;

    // Forward this to it
    pOidJob->OnOidJobStartedEvent(OperationStatus);
}

void COidJobNotificationHandler::OnOperationCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_opt_ WFC_SENDER_OBJECT SenderObject )
{
    CJobBase* pJobBase = (CJobBase*)SenderObject;

    // The job is really a OID base
    COidJobBase* pOidJob = (COidJobBase *)pJobBase;

    // Forward this to it
    pOidJob->OnOidJobCompletedEvent(OperationStatus);

    delete pOidJob;
}


NDIS_STATUS FactoryCreateJob(
    _In_ WFC_ACTIVITY_ID ActivityId,
    _In_ WFC_JOB_TYPE JobType,
    _Out_ CJobBase** ppJobBase
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CJobBase    *pNewJob = NULL;

    TraceEntry();

    switch (JobType)
    {
    case WiFiJobOIDSetNotYetImplemented:
        pNewJob = new CDummySetJob(
            ActivityId );
        break;

    case WiFiJobOIDQueryNotYetImplemented:
        pNewJob = new CDummyQueryReturnTrueJob(
            ActivityId );
        break;

    case WiFiJobCreateMac:
        pNewJob = new CCreateMacOidJob(
            ActivityId );
        break;

    case WiFiJobDeleteMac:
        pNewJob = new CDeleteMacOidJob(
            ActivityId );
        break;

    case WiFiJobQuerySupportedGuids:
        pNewJob = new CQuerySupportedGuidsJob(
            ActivityId );
        break;

    case WiFiJobDot11ResetPort:
        pNewJob = new CDot11ResetJob(
            ActivityId );
        break;

    case WiFiJobQueryOperationMode:
        pNewJob = new CQueryOperationModeJob(
            ActivityId );
        break;

    case WiFiJobSetOperationMode:
        pNewJob = new CSetOperationModeJob(
            ActivityId );
        break;


    case WiFiJobQueryDesiredBssType:
        pNewJob = new CSimplePropertyCacheULongQueryOidJob(
            WfcPortBssType,
            ActivityId );
        break;

    case WiFiJobSetDesiredBssType:
        pNewJob = new CSetDesirerdBssTypeJob(
            ActivityId );
        break;

    case WiFiJobQueryEnabledAuthAlgo:
        pNewJob = new CSimplePropertyCacheListQueryOidJob(
            WfcPortPropertyAuthAlgoList,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_AUTH_ALGORITHM_LIST_REVISION_1,
            sizeof(DOT11_AUTH_ALGORITHM_LIST),
            FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds),
            sizeof(DOT11_AUTH_ALGORITHM),
            ActivityId );
        break;

    case WiFiJobSetEnabledAuthAlgo:
        pNewJob = new CSimplePropertyCacheListSetOidJob(
            WfcPortPropertyAuthAlgoList,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_AUTH_ALGORITHM_LIST_REVISION_1,
            sizeof(DOT11_AUTH_ALGORITHM_LIST),
            FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_AUTH_ALGORITHM_LIST, AlgorithmIds),
            sizeof(DOT11_AUTH_ALGORITHM),
            1,
            1,
            ActivityId );
        break;

    case WiFiJobQueryEnabledMulticastCipherAlgo:
        pNewJob = new CSimplePropertyCacheListQueryOidJob(
            WfcPortPropertyMulticastCipherAlgoList,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_CIPHER_ALGORITHM_LIST_REVISION_1,
            sizeof(DOT11_CIPHER_ALGORITHM_LIST),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds),
            sizeof(DOT11_CIPHER_ALGORITHM),
            ActivityId );
        break;

    case WiFiJobSetEnabledMulticastCipherAlgo:
        pNewJob = new CSimplePropertyCacheListSetOidJob(
            WfcPortPropertyMulticastCipherAlgoList,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_CIPHER_ALGORITHM_LIST_REVISION_1,
            sizeof(DOT11_CIPHER_ALGORITHM_LIST),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds),
            sizeof(DOT11_CIPHER_ALGORITHM),
            0,
            256,
            ActivityId );
        break;

    case WiFiJobQueryEnabledUnicastCipherAlgo:
        pNewJob = new CSimplePropertyCacheListQueryOidJob(
            WfcPortPropertyUnicastCipherAlgoList,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_CIPHER_ALGORITHM_LIST_REVISION_1,
            sizeof(DOT11_CIPHER_ALGORITHM_LIST),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds),
            sizeof(DOT11_CIPHER_ALGORITHM),
            ActivityId );
        break;

    case WiFiJobSetEnabledUnicastCipherAlgo:
        pNewJob = new CSimplePropertyCacheListSetOidJob(
            WfcPortPropertyUnicastCipherAlgoList,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_CIPHER_ALGORITHM_LIST_REVISION_1,
            sizeof(DOT11_CIPHER_ALGORITHM_LIST),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_CIPHER_ALGORITHM_LIST, AlgorithmIds),
            sizeof(DOT11_CIPHER_ALGORITHM),
            1,
            1,
            ActivityId );
        break;

    case WiFiJobSetFTParameters:
        pNewJob = new CSetFTParametersJob(
            ActivityId );
        break;
    case WiFiJobSetFTReassociationParameters:
        pNewJob = new CSetFTReassociationParametersJob(
            ActivityId );
        break;
    case WiFiJobQueryPhyId:
        pNewJob = new CQueryPhyIdJob(
            ActivityId );
        break;

    case WiFiJobSetPhyId:
        pNewJob = new CSetPhyIdJob(
            ActivityId );
        break;

    case WiFiJobQueryOperationalRateSet:
        pNewJob = new CQueryOperationalRateSet(
            ActivityId );
        break;

    case WiFiJobScan:
        pNewJob = new CScanJob(
            ActivityId);
        break;

    case WiFiJobScanOidRequest:
        pNewJob = new CScanOidJob(
            ActivityId);
        break;

    case WiFiJobEnumBSSList:
    case WiFiJobP2PEnumDeviceList:
        pNewJob = new CEnumBSSListJob(
            ActivityId);
        break;

    case WiFiJobQuery8023MulticastList:
        pNewJob = new CQueryMulticastListJob(
            WfcPortProperty8023MulticastList,
            ActivityId );
        break;

    case WiFiJobSet8023MulticastList:
        pNewJob = new CSetMulticastListJob(
            WfcPortProperty8023MulticastList,
            ActivityId );
        break;

    case WiFiJobQueryDot11MulticastList:
        pNewJob = new CQueryMulticastListJob(
            WfcPortPropertyDot11MulticastList,
            ActivityId );
        break;

    case WiFiJobSetDot11MulticastList:
        pNewJob = new CSetMulticastListJob(
            WfcPortPropertyDot11MulticastList,
            ActivityId );
        break;

    case WiFiJobQueryDesiredSSIDList:
        pNewJob = new CSimplePropertyCacheListQueryOidJob(
            WfcPortPropertySSID,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_SSID_LIST_REVISION_1,
            sizeof(DOT11_SSID_LIST),
            FIELD_OFFSET(DOT11_SSID_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_SSID_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_SSID_LIST, SSIDs),
            sizeof(DOT11_SSID),
            ActivityId );
        break;

    case WiFiJobSetDesiredSSIDList:
        pNewJob = new CSimplePropertyCacheListSetOidJob(
            WfcPortPropertySSID,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_SSID_LIST_REVISION_1,
            sizeof(DOT11_SSID_LIST),
            FIELD_OFFSET(DOT11_SSID_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_SSID_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_SSID_LIST, SSIDs),
            sizeof(DOT11_SSID),
            1,
            1,
            ActivityId );
        break;

    case WiFiJobQueryDesiredHESSIDList:
        pNewJob = new CQueryDesiredHESSIDList(
            ActivityId );
        break;

    case WiFiJobSetDesiredHESSIDList:
        pNewJob = new CSetDesiredHESSIDList(
            ActivityId );
        break;

    case WiFiJobQueryHiddenNetworkEnabled:
        pNewJob = new CSimplePropertyCacheBooleanQueryOidJob(
            WfcPortPropertyHiddenNetwork,
            ActivityId );
        break;

    case WiFiJobSetHiddenNetworkEnabled:
        pNewJob = new CSimplePropertyCacheBooleanSetOidJob(
            WfcPortPropertyHiddenNetwork,
            ActivityId );
        break;

    case WiFiJobQueryExcludeUnencrypted:
        pNewJob = new CSimplePropertyCacheBooleanQueryOidJob(
            WfcPortPropertyExcludeUnencrypted,
            ActivityId );    
        break;

    case WiFiJobSetExcludeUnencrypted:
        pNewJob = new CSimplePropertyCacheBooleanSetOidJob(
            WfcPortPropertyExcludeUnencrypted,
            ActivityId );
        break;

    case WiFiJobConnectRequest:
    case WiFiJobP2PConnectRequest:
        pNewJob = new CConnectJob(
            ActivityId );
        break;

    case WiFiJobDisconnectRequest:
        pNewJob = new CDisconnectJob(
            ActivityId );
        break;

    case WiFiJobQueryPrivacyExemptionList:
        pNewJob = new CSimplePropertyCacheListQueryOidJob(
            WfcPortPropertyNamePrivacyExemptionList,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_PRIVACY_EXEMPTION_LIST_REVISION_1,
            sizeof(DOT11_PRIVACY_EXEMPTION_LIST),
            FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_PRIVACY_EXEMPTION_LIST, PrivacyExemptionEntries),
            sizeof(DOT11_PRIVACY_EXEMPTION),
            ActivityId );
        break;
        
    case WiFiJobQueryDesiredBSSIDList:
        pNewJob = new CSimplePropertyCacheListQueryOidJob(
            WfcPortPropertyDesiredBSSIDList,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_BSSID_LIST_REVISION_1,
            sizeof(DOT11_BSSID_LIST),
            FIELD_OFFSET(DOT11_BSSID_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_BSSID_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_BSSID_LIST, BSSIDs),
            sizeof(DOT11_MAC_ADDRESS),
            ActivityId );
        break;

    case WiFiJobSetDesiredBSSIDList:
        pNewJob = new CSetDesiredBSSIDListJob(
            ActivityId );
        break;

    case WiFiJobSetStaAssociationIEs:
        pNewJob = new CSetStaAssociationIEsJob(
            ActivityId );
        break;
    

    case WiFiJobFlushBssList:
    case WiFiJobP2PFlushDeviceList:
        pNewJob = new CFlushBSSListJob(
            ActivityId );
        break;

    case WiFiJobEnumAssociationInfo:
        pNewJob = new CEnumAssociationInfoJob(
            ActivityId );
        break;

    case WiFiJobQueryOperationModeCapability:
        pNewJob = new CQueryOperationModeCapabilityJob(
            ActivityId );
        break;

    case WiFiJobQueryMacAddress:
        pNewJob = new CQueryMacAddressJob(
            ActivityId );
        break;

    case WiFiJobQueryHardwareAddress:
        pNewJob = new CQueryHardwareAddressJob(
            ActivityId );
        break;

    case WiFiJobQueryMediaStreamingEnabled:
        pNewJob = new CQueryMediaStreamingEnabledOidJob(
            ActivityId );
        break;

    case WiFiJobSetMediaStreamingEnabled:
        pNewJob = new CSetMediaStreamingEnabledOidJob(
            ActivityId );
        break;

    case WiFiJobQueryAutoConfigEnabled:
        pNewJob = new CSimplePropertyCacheULongQueryOidJob(
            WfcPortPropertyAutoConfigEnabled,
            ActivityId );    
        break;

    case WiFiJobSetAutoConfigEnabled:
        pNewJob = new CSimplePropertyCacheULongSetOidJob(
            WfcPortPropertyAutoConfigEnabled,
            0,
            (DOT11_MAC_AUTO_CONFIG_ENABLED_FLAG | DOT11_MAC_AUTO_CONFIG_ENABLED_FLAG),
            ActivityId );
        break;

    case WiFiJobQueryUnicastUseGroupEnabled:
        pNewJob = new CSimplePropertyCacheBooleanQueryOidJob(
            WfcPortPropertyUnicastUseGroupEnabled_Dummy,
            ActivityId );    
        break;

    case WiFiJobSetUnicastUseGroupEnable:
        pNewJob = new CSimplePropertyCacheBooleanQueryOidJob(
            WfcPortPropertyUnicastUseGroupEnabled_Dummy,
            ActivityId );    
        break;

    case WiFiJobOIDQueryBeaconPeriod:
        pNewJob = new CSimplePropertyCacheULongQueryOidJob(
            WfcPortCurrentBeaconPeriod,
            ActivityId );    
        break;

    case WiFiJobOIDSetBeaconPeriod:
        pNewJob = new CSimplePropertyCacheULongSetOidJob(
            WfcPortCurrentBeaconPeriod,
            1,
            65535,
            ActivityId );
        break;

    case WiFiJobQueryRTSThreshold:
        pNewJob = new CSimplePropertyCacheULongQueryOidJob(
            WfcPortPropertyRTSThreshold_Dummy,
            ActivityId );
        break;

    case WiFiJobSetRTSThreshold:
        pNewJob = new CSimplePropertyCacheULongSetOidJob(
            WfcPortPropertyRTSThreshold_Dummy,
            0,
            2347,
            ActivityId );
        break;

    case WiFiJobQueryFragmentationThreshold:
        pNewJob = new CSimplePropertyCacheULongQueryOidJob(
            WfcPortPropertyFragmentationThreshold_Dummy,
            ActivityId );
        break;

    case WiFiJobSetFragmentationThreshold:
        pNewJob = new CSimplePropertyCacheULongSetOidJob(
            WfcPortPropertyFragmentationThreshold_Dummy,
            256,
            2346,
            ActivityId );
        break;

    case WiFiJobQuerySafeModeEnabled:
        pNewJob = new CSimplePropertyCacheBooleanQueryOidJob(
            WfcPortPropertySafeModeEnabled,
            ActivityId );    
        break;

    case WiFiJobSetSafeModeEnabled:
        pNewJob = new CSimplePropertyCacheBooleanSetOidJob(
            WfcPortPropertySafeModeEnabled,
            ActivityId );    
        break;

    case WiFiJobGetQoSParams:
        pNewJob = new CSimplePropertyCacheUCharWithHeaderQueryOidJob(
            WfcPortPropertyQoSProtocolFlags,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_QOS_PARAMS_REVISION_1,
            sizeof(DOT11_QOS_PARAMS),
            ActivityId );
        break;

    case WiFiJobSetQoSParams:
        pNewJob = new CSimplePropertyCacheUCharWithHeaderSetOidJob(
            WfcPortPropertyQoSProtocolFlags,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_QOS_PARAMS_REVISION_1,
            sizeof(DOT11_QOS_PARAMS),
            ActivityId );    
        break;

    case WiFiJobQueryCurrentChannel:
        pNewJob = new CSimplePropertyCacheULongQueryOidJob(
            WfcPortPropertyCurrentChannelNumber,
            ActivityId );
        break;


    case WiFiJobQueryCurrentFrequency:
        pNewJob = new CSimplePropertyCacheULongQueryOidJob(
            WfcPortPropertyCurrentChannelNumber,
            ActivityId );
        break;

    // TODO: We dont support Netmon mode, but we save this value
    case WiFiJobSetCurrentChannel:
        pNewJob = new CSimplePropertyCacheULongSetOidJob(
            WfcPortPropertyCurrentChannelNumber,
            0,
            200,
            ActivityId );
        break;

    // TODO: We dont support Netmon mode, but we save this value
    case WiFiJobSetCurrentFrequency:
        pNewJob = new CSimplePropertyCacheULongSetOidJob(
            WfcPortPropertyCurrentChannelNumber,
            0,
            200,
            ActivityId );
        break;

    case WiFiJobQueryUnreachableDetectionThreshold:
        pNewJob = new CSimplePropertyCacheULongQueryOidJob(
            WfcPortPropertyUnreachableThreshold_Dummy,
            ActivityId );
        break;

    case WiFiJobSetUnreachableDetectionThreshold:
        pNewJob = new CSimplePropertyCacheULongSetOidJob(
            WfcPortPropertyUnreachableThreshold_Dummy,
            0,
            60000,
            ActivityId );
        break;

    case WiFiJobQueryExcludedMacAddressList:
        pNewJob = new CSimplePropertyCacheListQueryOidJob(
            WfcPortPropertyExcludedMacAddressList,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_MAC_ADDRESS_LIST_REVISION_1,
            sizeof(DOT11_MAC_ADDRESS_LIST),
            FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_MAC_ADDRESS_LIST, MacAddrs),
            sizeof(DOT11_MAC_ADDRESS),
            ActivityId );
        break;

    case WiFiJobSetExcludedMacAddressList:
        pNewJob = new CSetExcludedMacAddressListJob(
            ActivityId );
        break;

    case WiFiJobQueryDesiredPhyList:
        pNewJob = new CSimplePropertyCacheListQueryOidJob(
            WfcPortPropertyDesiredPhyList_Dummy,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_PHY_ID_LIST_REVISION_1,
            sizeof(DOT11_PHY_ID_LIST),
            FIELD_OFFSET(DOT11_PHY_ID_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_PHY_ID_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId),
            sizeof(ULONG),
            ActivityId );
        break;

    case WiFiJobSetDesiredPhyList:
        pNewJob = new CSimplePropertyCacheListSetOidJob(
            WfcPortPropertyDesiredPhyList_Dummy,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_PHY_ID_LIST_REVISION_1,
            sizeof(DOT11_PHY_ID_LIST),
            FIELD_OFFSET(DOT11_PHY_ID_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_PHY_ID_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId),
            sizeof(ULONG),
            1,
            10, // TODO: Figure this number out
            ActivityId );
        break;



    case WiFiJobQueryIBSSParams:
    case WiFiJobSetIBSSParams:
        // We dont support Adhoc. Fail these OIDs if they come down
        WFCError("OID_DOT11_IBSS_PARAMS Not supported");    
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;

    case WiFiJobGeneralStatistics:
        // We dont support general statistics
        WFCError("OID_GEN_STATISTICS Not supported");    
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;        

    case WiFiJobQueryActivePhyList:
        pNewJob = new CSimplePropertyCacheListQueryOidJob(
            WfcPortPropertyActivePhyList_Dummy,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_PHY_ID_LIST_REVISION_1,
            sizeof(DOT11_PHY_ID_LIST),
            FIELD_OFFSET(DOT11_PHY_ID_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_PHY_ID_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_PHY_ID_LIST, dot11PhyId),
            sizeof(ULONG),
            ActivityId );
        break;

    case WiFiJobQuerySupportedMulticastAlgorithmPair:
        pNewJob = new CQuerySupportedMulticastAlgorithmPairJob(
            ActivityId );
        break;

    case WiFiJobQuerySupportedUnicastAlgorithmPair:
        pNewJob = new CQuerySupportedUnicastAlgorithmPairJob(
            ActivityId );
        break;

    case WiFiJobQueryVendorDescription:
        pNewJob = new CQueryVendorDescriptionJob(
            ActivityId );
        break;

    case WiFiJobQueryVendorDriverVersion:
        pNewJob = new CQueryVendorDriverVersionJob(
            ActivityId );
        break;

    case WiFiJobQueryVendorID:
        pNewJob = new CQueryVendorIDJob(
            ActivityId );
        break;

    case WiFiJobQueryExtSTACapability:
        pNewJob = new CQueryExtSTACapabilityJob(
            ActivityId );
        break;

    case WiFiJobQueryRegDomainsSupportValue:
        pNewJob = new CQueryRegDomainsSupportJob(
            ActivityId );
        break;

    case WiFiJobQueryCurrentRegDomain:
        pNewJob = new CQueryCurrentRegDomainJob(
            ActivityId );
        break;

    case WiFiJobSetPrivacyExemptionList:
        pNewJob = new CSetPrivacyExemptionListJob(
            ActivityId );
        break;

    case WiFiJobSetDefaultKey:
        pNewJob = new CSetDefaultKeyJob(
            ActivityId );
        break;

    case WiFiJobQueryDefaultKeyId:
        pNewJob = new CSimplePropertyCacheULongQueryOidJob(
            WfcPortPropertyNameDefaultKeyId,
            ActivityId );
        break;

    case WiFiJobSetDefaultKeyId:
        pNewJob = new CSetDefaultKeyIdJob(
            ActivityId );
        break;

    case WiFiJobSetKeyMappingKey:
        pNewJob = new CSetKeyMappingKeyJob(
            ActivityId );
        break;

    case WiFiJobQueryPMKIDList:
        pNewJob = new CSimplePropertyCacheListQueryOidJob(
            WfcPortPropertyPMKIDList,
            NDIS_OBJECT_TYPE_DEFAULT,
            DOT11_PMKID_LIST_REVISION_1,
            sizeof(DOT11_PMKID_LIST),
            FIELD_OFFSET(DOT11_PMKID_LIST, uTotalNumOfEntries),
            FIELD_OFFSET(DOT11_PMKID_LIST, uNumOfEntries),
            FIELD_OFFSET(DOT11_PMKID_LIST, PMKIDs),
            sizeof(DOT11_PMKID_ENTRY),
            ActivityId );
        break;

    case WiFiJobSetPMKIDList:
        pNewJob = new CSetPMKIDList(
            ActivityId
            );
         
        break;

    case WiFiJobSetPacketFilter:
        pNewJob = new CSetPacketFilterJob(
            ActivityId );
        break;

    case WiFiJobQueryNicPowerState:
        pNewJob = new CGetRadioStateJob( // was CGetNicPowerStateJob
            ActivityId,
            FALSE
            );
        break;

    case WiFiJobSetNicPowerState:
        pNewJob = new CSetRadioStateJob( // was CSetNicPowerStateJob
            ActivityId );
        break;

    case WiFiJobQueryHardwarePhyState:
        pNewJob = new CGetRadioStateJob( // was CGetNicPowerStateJob
            ActivityId,
            TRUE
            );
        break;

    case WiFiJobQueryStatistics:
        pNewJob = new CQueryStatisticsJob(
            ActivityId);
        break;

    case WiFiJobGetAutoPowerSave:
        pNewJob = new CGetAutoPowerSaveJob(
            ActivityId );
        break;

    case WiFiJobSetAutoPowerSave:
        pNewJob = new CSetAutoPowerSaveJob(
            ActivityId );
        break;

    case WiFiJobSetPowerMgmtRequest:
        pNewJob = new CSetPowerMgmtRequestJob(
            ActivityId );
        break;

    case WiFiJobSetPower:
        pNewJob = new CSetPowerJob(
            ActivityId);
        break;

    case WiFiJobQueryPower:
        pNewJob = new CQueryPowerJob(
            ActivityId);
        break;

    case WiFiJobSetAddWolPattern:
        pNewJob = new CSetAddWolPatternJob(
            ActivityId );
        break;

    case WiFiJobSetRemoveWolPattern:
        pNewJob = new CSetRemoveWolPatternJob(
            ActivityId );
        break;

    case WiFiJobSetNlo:
        pNewJob = new CSetNloJob(
            ActivityId );
        break;

    case WiFiJobGetPowerMgmtRequest:
        pNewJob = new CGetPowerMgmtRequestJob(
            ActivityId );
        break;


    case WiFiJobGetPmParameters:
        pNewJob = new CGetPmParametersJob(
            ActivityId );
        break;

    case WiFiJobSetPmParameters:
        pNewJob = new CSetPmParametersJob(
            ActivityId );
        break;

    case WiFiJobSetAddPmProtocolOffload:
        pNewJob = new CSetAddPmProtocolOffloadJob(
            ActivityId );
        break;

    case WiFiJobSetRemovePmProtocolOffload:
        pNewJob = new CSetRemovePmProtocolOffloadJob(
            ActivityId );
        break;

    case WiFiJobGetPmProtocolOffload:
        pNewJob = new CGetPmProtocolOffloadJob(
            ActivityId );
        break;

    case WiFiJobGetReceiveCoalescingMatchCount:
        pNewJob = new CGetReceiveCoalescingMatchCountJob(
            ActivityId );
        break;

    case WiFiJobSetReceiveCoalescing:
        pNewJob = new CSetReceiveCoalescingJob(
            ActivityId );
        break;

    case WiFiJobSetClearReceiveCoalescing:
        pNewJob = new CSetClearReceiveCoalescingJob(
            ActivityId );
        break;

    case WiFiJobP2PSetDeviceCapability:    
    case WiFiJobP2PSetAdditionalIE:
    case WiFiJobP2PSetDeviceInfo:
    case WiFiJobP2PSetGroupOwnerCapability:
    case WiFiJobP2PSetSecondaryDeviceTypeList:
    case WiFiJobP2PSetAdvertisementList:
        pNewJob = new CSetWFDAdvertisementSettings(
            ActivityId );
        break;

    case WiFiJobP2PGetAdvertisementList:
        pNewJob = new CGetWFDAdvertisementSettings(
            ActivityId );
        break;

    case WiFiJobP2PSetDesiredGroupID:
        pNewJob = new CSetP2PDesiredGroupIDJob(
            ActivityId );
        break;

    case WiFiJobP2PDisconnectPeerRequest:
        pNewJob = new CP2PDisconnectJob(
            ActivityId );
        break;

    case WiFiJobP2PDiscover:
        pNewJob = new CP2PDiscoverJob(
            ActivityId );
        break;

    case WiFiJobP2PSetGroupJoinParameters:
        pNewJob = new CSetP2PGroupJoinParameters(
            ActivityId );
        break;

    case WiFiJobP2PSetListenState:
    case WiFiJobP2PSetListenStatePassiveAvailability:
        pNewJob = new CSetWFDListenState(
            ActivityId );
        break;

    case WiFiJobP2PStartBackgroundDiscovery:
        pNewJob = new CSetWFDStartBackgroundDiscovery(
            ActivityId );
        break;

    case WiFiJobP2PStopBackgroundDiscovery:
        pNewJob = new CSetWFDStopBackgroundDiscovery(
            ActivityId );
        break;

    case WiFiJobP2PSendActionFrameRequest:
        pNewJob = new CP2PSendActionFrameRequestJob(
            ActivityId );
        break;

    case WiFiJobP2PSetGroupStartParameters:
        pNewJob = new CSetAPGroupStartParametersJob(
            ActivityId );
        break;

    case WiFiJobP2PSendActionFrameResponse:
        pNewJob = new CP2PSendActionFrameResponseJob(
            ActivityId );
        break;

    case WiFiJobStartP2PAp:
        pNewJob = new CStartAPJob(
            FALSE,              // P2P AP
            ActivityId );
        break;

    case WiFiJobP2PCancelDiscover:                
        pNewJob = new CP2PDiscoverCancelJob (
            ActivityId );
        break;

    case WiFiJobSendAssociationResponse:
        pNewJob = new CSendAssociationResponseJob(
            ActivityId );
        break;

#if WDI_DISABLED
    case WiFiJobStartLegacyAp:
        pNewJob = new CStartAPJob(
            TRUE,               // Legacy AP
            ActivityId );
        break;
#endif // WDI_DISABLED            

    case WiFiJobSetWpsEnabled:
        pNewJob = new CSetWpsEnabledJob(
            ActivityId );
        break;

    case WiFiJobP2PGetDialogToken:
        pNewJob = new CGetNextDialogTokenJob(
            ActivityId );
        break;

    case WiFiJobGetPeerInfo:
        NT_ASSERT(FALSE); // still integrating offline
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        break;

    case WiFiJobNicSpecificExtension:
        pNewJob = new CNicSpecificExtensionJob(
            ActivityId );
        break;

    case WiFiJobResetRecovery:
        pNewJob = new CResetRecoveryJob( ActivityId );
        break;

    case WiFiJobANQPQuery:
        pNewJob = new CANQPQueryJob(
            ActivityId);
        break;

    case WiFiJobQueryTcpOffloadCurrentConfig:
        pNewJob = new CTcpOffloadCurrentConfigJob( ActivityId );
        break;

    case WiFiJobQueryTcpOffloadHardwareCaps:
        pNewJob = new CTcpOffloadHardwareCapsJob(ActivityId);
        break;

    case WiFiJobSetTcpOffloadParameters:
        pNewJob = new CTcpOffloadSetParametersJob(ActivityId);
        break;

    case WiFiJobQueryTcpRscStatistics:
        pNewJob = new CTcpOffloadOffloadRscStatisticsJob(ActivityId);
        break;

    case WiFiJobSetEncapsulationOffload:
        pNewJob = new CTcpEncapsulationOffloadParametersJob(ActivityId);
        break;

    case WiFiJobRandomMacForScan:
        pNewJob = new CDot11RandomMacForScan(ActivityId);
        break;

    case WiFiJobGetAdapterEnhancedCapabilities:
    case WiFiJobGetAdapterBandCapabilities:
        pNewJob = new CGetAdapterCapabilitiesJob(ActivityId);
        break;

    case WiFiJobP2PEnableHrdssDevices:
        pNewJob = new CP2PEnableHrdssDevices(ActivityId);
        break;

    case WiFiJobSetEndDwellOnChannel:
        pNewJob = new CSetEndDwellOnChannel(ActivityId);
        break;

    case WiFiJobUnrecognizedOidHandler:
        pNewJob = new CUnrecognizedOidHandlerJob(ActivityId);
        break;

    case WiFiJobIhvTaskRequest:
        pNewJob = new CIhvTaskRequestJob(ActivityId);
        break;

    case WiFiJobQueryXmitOk:
    case WiFiJobQueryRcvOk:
        // We don't support these Oids - they are only listed to enable NDIS to return Tx/Rx statistics for health check
        // Fail these OIDs if they come down
        WFCError("JobType=%d, OID_GEN_XMIT_OK | OID_GEN_RCV_OK Not supported", JobType);
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;

    default:
#if WDI_DISABLED                
        NT_ASSERT(FALSE);   
#endif // WDI_DISABLED            

        WFCError("Unsupported Job Type %d", JobType);    
        ndisStatus = NDIS_STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    if (pNewJob == NULL)
    {
        WFCError("Failed to create Job for type %d", JobType);    
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    *ppJobBase = pNewJob;

Exit:
    TraceExit(ndisStatus);
    return ndisStatus;
}

//==============================================================================================

COidJobBase::COidJobBase(
    _In_ WFC_ACTIVITY_ID ActivityId ) :
    m_pAdapter(NULL),
    m_pHandlerInfo(NULL),
    m_pOidRequest(NULL),
    CJobBase(ActivityId),
    m_pPortPropertyCache(NULL)
{

}

COidJobBase::~COidJobBase()
{

}

NDIS_STATUS COidJobBase::InitializeWithoutOid(
    _In_ WFC_JOB_TYPE JobType,
    _In_ CAdapter*  pAdapter,
    _In_ bool       IsPreemptable,
    _In_ bool       CanPreempt,
    _In_ WFC_SERIALIZED_JOB_PRIORITY_SCOPE PriorityScope,
    _In_ WFC_SERIALIZED_JOB_PRIORITY       JobPriority,
    _In_ NDIS_PORT_NUMBER PortNumber,
    _In_opt_ bool IsDeferrable
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    UNREFERENCED_PARAMETER(PortNumber);
    
    m_pAdapter = pAdapter;
    m_NdisPortNumber = PortNumber;

    // Initialize the base
    ndisStatus = CJobBase::Initialize(
            m_pAdapter->GetEventQueue(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetActiveJobsList(),
            m_pAdapter->GetSerializedJobList(),
            JobType,
            IsPreemptable,
            CanPreempt,
            PriorityScope,
            JobPriority,
            (IPropertyCacheDirectory *)m_pAdapter,
            (INdisConversion *)m_pAdapter,
            PortNumber,
            IsDeferrable
            );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize base job (0x%x)", ndisStatus);    
        goto Exit;
    }

Exit:
    TraceActExit(ndisStatus);
    
    return ndisStatus;
}

NDIS_STATUS COidJobBase::InitializeForOid(
    _In_ WFC_JOB_TYPE JobType,
    _In_ CAdapter*  pAdapter,
    _In_ POID_HANDLER_ENTRY pHandlerInfo,
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();
    
    // Use the other initialize function
    ndisStatus = COidJobBase::InitializeWithoutOid(
            JobType,
            pAdapter,
            pHandlerInfo->IsPreemptable,
            pHandlerInfo->CanPreempt,
            pHandlerInfo->PriorityScope,
            pHandlerInfo->Priority,
            pOidRequest->PortNumber
            );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize base job (0x%x)", ndisStatus);    
        goto Exit;
    }

    m_pHandlerInfo = pHandlerInfo;
    m_pOidRequest = pOidRequest;

    // Initialize the derived class from the OID request
    ndisStatus = this->InitializeFromOidRequest(pOidRequest);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to Initialize the derived OID job (0x%x)", ndisStatus);    
        goto Exit;
    }
    
Exit:
    TraceActExit(ndisStatus);
    
    return ndisStatus;
}

VOID COidJobBase::OnOidJobStartedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // If derived class did not implement this, then its not
    // interested in doing any work on this event
    UNREFERENCED_PARAMETER(NdisStatus);
}

CPortPropertyCache* COidJobBase::GetPortPropertyCache()
{
    // If we dont have the value cached, get it from the parent
    if (m_pPortPropertyCache == NULL)
    {
        NDIS_STATUS ndisStatus = CJobBase::get_PortPropertyCache(&m_pPortPropertyCache);

        // Port property cache must always be present for a port
        NT_ASSERT(ndisStatus == NDIS_STATUS_SUCCESS);
        UNREFERENCED_PARAMETER(ndisStatus);
        NT_ASSERT(m_pPortPropertyCache != NULL);
    }
    
    return m_pPortPropertyCache;
}

_Success_(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS COidJobBase::s_CreateOidJob(   
    _In_ WFC_JOB_TYPE JobType,
    _In_ CAdapter* pAdapter,
    _In_ POID_HANDLER_ENTRY pHandlerInfo,
    _In_ PNDIS_OID_REQUEST pOidRequest,
    _Out_ COidJobBase** ppOidJobBase
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CJobBase *pNewJob = NULL;
    COidJobBase *pOidJob = NULL;
    
    TraceEntry();

    // Call the factory to create the job
    ndisStatus = FactoryCreateJob(
                    IActivityId::get_NextActivityId(),
                    JobType, 
                    &pNewJob);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to create Job (0x%x)", ndisStatus);    
        goto Exit;
    }

    // We would only be called for OID jobs
    pOidJob = (COidJobBase *)pNewJob;
    
    // Initialize the job
    ndisStatus = pOidJob->InitializeForOid(
        JobType,
        pAdapter,
        pHandlerInfo,
        pOidRequest
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to intialize Job (0x%x)", ndisStatus);    
        goto Exit;
    }

    // This is the job that goes back to the caller
    *ppOidJobBase = pOidJob;
    pOidJob = NULL;
    
Exit:
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        if (pOidJob)
        {
            delete pOidJob;
        }
    }
    
    TraceExit(ndisStatus);
    return ndisStatus;
}

POID_HANDLER_ENTRY COidJobBase::s_FindOidHandlerEntry(
    _In_ NDIS_OID NdisOid
    )
{
    ULONG           i;
    POID_HANDLER_ENTRY   oidTableEntry = NULL;

    // Find the OID in the oid table
    for (i = 0; i < ((ULONG)sizeof(OidHandlerTable)/(ULONG)sizeof(OID_HANDLER_ENTRY)); i++)
    {
        if (OidHandlerTable[i].Oid == NdisOid)
        {
            oidTableEntry = &OidHandlerTable[i];
            break;
        }
    }

    return oidTableEntry;
}

NDIS_STATUS COidJobBase::s_GetSupportedOidsList(
    _In_opt_count_(NonWdiOidCount) UINT32 * pNonWdiOidList,
    _In_ ULONG NonWdiOidCount,
    _Outptr_result_buffer_(*pSupportedOidsCount) PNDIS_OID *ppSupportedOidsList, 
    _Out_ ULONG *pSupportedOidsCount
    )
{
    ULONG           i;
    PNDIS_OID       outputList = NULL;

    *ppSupportedOidsList = NULL;
    *pSupportedOidsCount = ((ULONG)sizeof(OidHandlerTable)/(ULONG)sizeof(OID_HANDLER_ENTRY));
    
    outputList = new NDIS_OID[*pSupportedOidsCount + NonWdiOidCount];
    if (outputList == NULL)
    {
        WFCError("Failed to allocate memory for Supported Oids List");    
        return NDIS_STATUS_RESOURCES;
    }

    // Copy the WDI OIDs in
    for (i = 0; i < ((ULONG)sizeof(OidHandlerTable)/(ULONG)sizeof(OID_HANDLER_ENTRY)); i++)
    {
        outputList[i] = OidHandlerTable[i].Oid;
    }

    if (NonWdiOidCount)
    {
        // Copy the non-WDI OID list
        RtlCopyMemory(&outputList[i], pNonWdiOidList, NonWdiOidCount * sizeof(ULONG));
        *pSupportedOidsCount += NonWdiOidCount;
    }

    *ppSupportedOidsList = outputList;

    return NDIS_STATUS_SUCCESS;

}


//==============================================================================================

CStubOidJob::CStubOidJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) 
        : COidJobBase(ActivityId)
{
}

CStubOidJob::~CStubOidJob ()
{
}

// COidJobBase
NDIS_STATUS CStubOidJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    UNREFERENCED_PARAMETER(pOidRequest);

    return NDIS_STATUS_SUCCESS;
}

VOID CStubOidJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);        
}
    
// CJobBase
void CStubOidJob::OnJobStarted()
{
    // Just complete the job
    CompleteJob(NDIS_STATUS_SUCCESS);
}

void CStubOidJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
}

void CStubOidJob::OnJobCancelled()
{
    // We do nothing yet
}


//==============================================================================================

CSimpleQueryOidJob::CSimpleQueryOidJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) 
        : COidJobBase(ActivityId)
{
    m_pInformationBuffer = NULL;
    m_InformationBufferLength = 0;
    m_pBytesWritten = NULL;
    m_pBytesNeeded = NULL;
}

CSimpleQueryOidJob::~CSimpleQueryOidJob ()
{
}

// COidJobBase
NDIS_STATUS CSimpleQueryOidJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    m_Oid = QUERY_OID_VALUE(pOidRequest);
    m_pInformationBuffer = QUERY_OID_BUFFER(pOidRequest);
    m_InformationBufferLength = QUERY_OID_BUFFER_LENGTH(pOidRequest);
    m_pBytesWritten = &QUERY_OID_BYTES_WRITTEN(pOidRequest);
    m_pBytesNeeded = &QUERY_OID_BYTES_NEEDED(pOidRequest);
    
    return NDIS_STATUS_SUCCESS;
}

VOID CSimpleQueryOidJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);        
}
    
// CJobBase
void CSimpleQueryOidJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Call the child
    ndisStatus = QueryData(
        m_pInformationBuffer,
        m_InformationBufferLength,
        m_pBytesWritten,
        m_pBytesNeeded
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to query OID (0x%x)", ndisStatus);
        goto Exit;
    }

Exit:
    // Complete the job
    CompleteJob(ndisStatus);

    TraceActExit(ndisStatus);
}

void CSimpleQueryOidJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
}

void CSimpleQueryOidJob::OnJobCancelled()
{
    // We do nothing yet
}


//==============================================================================================

CSimpleSetOidJob::CSimpleSetOidJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) 
        : COidJobBase(ActivityId)
{
    m_pInformationBuffer = NULL;
    m_InformationBufferLength = 0;
    m_pBytesRead = NULL;
    m_pBytesNeeded = NULL;
}

CSimpleSetOidJob::~CSimpleSetOidJob ()
{
}

// COidJobBase
NDIS_STATUS CSimpleSetOidJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    m_Oid = SET_OID_VALUE(pOidRequest);
    m_pInformationBuffer = SET_OID_BUFFER(pOidRequest);
    m_InformationBufferLength = SET_OID_BUFFER_LENGTH(pOidRequest);
    m_pBytesRead = &SET_OID_BYTES_READ(pOidRequest);
    m_pBytesNeeded = &SET_OID_BYTES_NEEDED(pOidRequest);
    
    return NDIS_STATUS_SUCCESS;
}

VOID CSimpleSetOidJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);        
}
    
// CJobBase
void CSimpleSetOidJob::OnJobStarted()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    // Call the child
    ndisStatus = SetData(
        m_pInformationBuffer,
        m_InformationBufferLength,
        m_pBytesRead,
        m_pBytesNeeded
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to set OID (0x%x)", ndisStatus);
        goto Exit;
    }
        
Exit:
    // Just complete the job
    CompleteJob(ndisStatus);

    TraceActExit(ndisStatus);
}

void CSimpleSetOidJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(SenderObject);
}

void CSimpleSetOidJob::OnJobCancelled()
{
    // We do nothing yet
}


//==============================================================================================
CSimplePropertyCacheListQueryOidJob::CSimplePropertyCacheListQueryOidJob(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ UCHAR ObjectType,
    _In_ UCHAR ObjectRevision,
    _In_ size_t ObjectSize,
    _In_ ULONG OffsetToTotalNumberOfEntries,
    _In_ ULONG OffsetToNumberOfEntries,
    _In_ ULONG OffsetToEntriesList,
    _In_ ULONG SizeOfEachEntry,
    _In_ WFC_ACTIVITY_ID ActivityId ) 
        : CSimpleQueryOidJob(ActivityId) 
{
    m_PropertyName = PropertyName;
    m_ObjectType = ObjectType;
    m_ObjectRevision = ObjectRevision;
    m_ObjectSize = (USHORT)ObjectSize;
    m_OffsetToTotalNumberOfEntries = OffsetToTotalNumberOfEntries;
    m_OffsetToNumberOfEntries = OffsetToNumberOfEntries;
    m_OffsetToEntriesList = OffsetToEntriesList;
    m_SizeOfEachEntry = SizeOfEachEntry;        
}


NDIS_STATUS CSimplePropertyCacheListQueryOidJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_OBJECT_HEADER* pOutHeader = (NDIS_OBJECT_HEADER*)pInformationBuffer;
    ULONG cacheListSize;
    UINT16 cacheElementCount;
    BYTE *pCacheList;
    PULONG pNumOfEntriesLocation;
    PULONG pTotalNumOfEntriesLocation;
    PUCHAR pEntriesLocation;

    TraceActEntry();

    NT_ASSERT(pInformationBuffer);
    
    RtlZeroMemory(pInformationBuffer, InformationBufferLength);

    if ((InformationBufferLength < m_OffsetToNumberOfEntries) ||
        (InformationBufferLength < m_OffsetToTotalNumberOfEntries) ||
        (InformationBufferLength < m_OffsetToEntriesList))
    {
        // Should already be validated in CAdapter::PreprocessOid
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    // Find offsets that we would fill
    pOutHeader = (NDIS_OBJECT_HEADER*)pInformationBuffer;
    pNumOfEntriesLocation = (PULONG)((UCHAR *)pInformationBuffer + m_OffsetToNumberOfEntries);
    pTotalNumOfEntriesLocation = (PULONG)((UCHAR *)pInformationBuffer + m_OffsetToTotalNumberOfEntries);
    pEntriesLocation = ((UCHAR *)pInformationBuffer + m_OffsetToEntriesList);

    ASSIGN_NDIS_OBJECT_HEADER_PTR(pOutHeader, 
        m_ObjectType,
        m_ObjectRevision,
        m_ObjectSize);

    status = GetPortPropertyCache()->GetPropertyList(
        m_PropertyName,
        &cacheListSize,
        &cacheElementCount,
        &pCacheList
        );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        *pTotalNumOfEntriesLocation = 0;
        WFCActError("Unable to find list property %d in Property Cache. Error = 0x%08x", 
            m_PropertyName, status);
        goto exit;
    }

    *pTotalNumOfEntriesLocation = cacheElementCount;

    // Integer overflow
    if (m_OffsetToEntriesList > 
            (m_OffsetToEntriesList + cacheListSize))
    {
        status = NDIS_STATUS_FAILURE;
        goto exit;
    }
    
    *pBytesNeeded = m_OffsetToEntriesList + cacheListSize;
    if (InformationBufferLength < *pBytesNeeded)
    {
        *pNumOfEntriesLocation = 0;
        WFCActError("Insufficient space. Needed %d Received %d", 
            *pBytesNeeded, InformationBufferLength);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    *pNumOfEntriesLocation = *pTotalNumOfEntriesLocation;
 
    RtlCopyMemory(
        pEntriesLocation,
        pCacheList,
        cacheListSize
        );

    *pBytesWritten = *pBytesNeeded;
    
exit:
    TraceActExit(status);
    return status;
}


//==============================================================================================
CSimplePropertyCacheListSetOidJob::CSimplePropertyCacheListSetOidJob(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ UCHAR ObjectType,
    _In_ UCHAR ObjectRevision,
    _In_ size_t ObjectSize,
    _In_ ULONG OffsetToTotalNumberOfEntries,
    _In_ ULONG OffsetToNumberOfEntries,
    _In_ ULONG OffsetToEntriesList,
    _In_ ULONG SizeOfEachEntry,
    _In_ ULONG MinimumRequiredEntries,
    _In_ ULONG MaximumAllowedEntries,
    _In_ WFC_ACTIVITY_ID ActivityId ) 
        : CSimpleSetOidJob(ActivityId) 
{
    m_PropertyName = PropertyName;
    m_ObjectType = ObjectType;
    m_ObjectRevision = ObjectRevision;
    m_ObjectSize = (USHORT)ObjectSize;
    m_OffsetToTotalNumberOfEntries = OffsetToTotalNumberOfEntries;
    m_OffsetToNumberOfEntries = OffsetToNumberOfEntries;
    m_OffsetToEntriesList = OffsetToEntriesList;
    m_SizeOfEachEntry = SizeOfEachEntry;        
    m_MinimumRequiredEntries = MinimumRequiredEntries;
    m_MaximumAllowedEntries = MaximumAllowedEntries;
}

NDIS_STATUS CSimplePropertyCacheListSetOidJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_OBJECT_HEADER* pInHeader = (NDIS_OBJECT_HEADER*)pInformationBuffer;
    PULONG pNumOfEntriesLocation;
    PUCHAR pEntriesLocation;

    TraceActEntry();

    NT_ASSERT(pInformationBuffer);
    
    if ((InformationBufferLength < m_OffsetToNumberOfEntries) ||
        (InformationBufferLength < m_OffsetToTotalNumberOfEntries) ||
        (InformationBufferLength < m_OffsetToEntriesList))
    {
        // Should already be validated in CAdapter::PreprocessOid
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    // Find offsets that we would use
    pInHeader = (NDIS_OBJECT_HEADER*)pInformationBuffer;
    pNumOfEntriesLocation = (PULONG)((UCHAR *)pInformationBuffer + m_OffsetToNumberOfEntries);
    pEntriesLocation = ((UCHAR *)pInformationBuffer + m_OffsetToEntriesList);

    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT_PTR(pInHeader, 
            m_ObjectType,
            m_ObjectRevision,
            m_ObjectSize))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid NDIS object header. Expected (T:%d R:%d S:%d) Received (T:%d R:%d S:%d)",
            m_ObjectType, m_ObjectRevision, m_ObjectSize,
            pInHeader->Type, pInHeader->Revision, pInHeader->Size
            );
        goto exit;
    }

    // Verify length/number of entries match up
    *pBytesNeeded = (*pNumOfEntriesLocation) * m_SizeOfEachEntry +
        m_OffsetToEntriesList;       

    // Integer overflow        
    if ( (m_SizeOfEachEntry > ((*pNumOfEntriesLocation) * m_SizeOfEachEntry)) ||
          (m_OffsetToEntriesList > (*pBytesNeeded))
          )
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActError("Integer overflow. %d * %d + %d",
            (*pNumOfEntriesLocation), m_SizeOfEachEntry, m_OffsetToEntriesList
            );
        goto exit;
    }        

    if (InformationBufferLength < *pBytesNeeded)
    {
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        WFCActError("Insufficient data. Expected %d Received %d", 
            *pBytesNeeded, InformationBufferLength);
        goto exit;
    }

    // Validate data sizes
    if (((*pNumOfEntriesLocation) < m_MinimumRequiredEntries) ||
        (((*pNumOfEntriesLocation) > m_MaximumAllowedEntries)))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid number of entries. Min %d Max %d Received %d",
            m_MinimumRequiredEntries, m_MaximumAllowedEntries, (*pNumOfEntriesLocation)            
            );
        goto exit;
    }
    
    status = GetPortPropertyCache()->SetPropertyList(
        m_PropertyName,
        (*pNumOfEntriesLocation) * m_SizeOfEachEntry,
        (UINT16)(*pNumOfEntriesLocation),
        (BYTE *)pEntriesLocation
        );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Unable to save list property %d in Property Cache. Error = 0x%08x", 
            m_PropertyName, status);
        goto exit;
    }

    *pBytesRead = *pBytesNeeded;

exit:
    TraceActExit(status);
    return status;
}

//==============================================================================================
NDIS_STATUS CSimplePropertyCacheULongQueryOidJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( InformationBufferLength < sizeof(ULONG) )
    {
        *pBytesNeeded = sizeof(ULONG);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    status = GetPortPropertyCache()->GetPropertyULong(
        m_PropertyName,
        (ULONG*)pInformationBuffer
        );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Unable to find ULONG property %d in Property Cache. Error = 0x%08x", 
            m_PropertyName, status);
        goto exit;
    }

    *pBytesWritten = sizeof(ULONG);
exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CSimplePropertyCacheULongSetOidJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG newValue;
    TraceActEntry();

    if ( InformationBufferLength < sizeof(ULONG) )
    {
        *pBytesNeeded = sizeof(ULONG);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    newValue = *((ULONG*)pInformationBuffer);
    if ((newValue < m_MinimumRequiredValue) ||
        (newValue > m_MaximumAllowedValue))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid ULONG value. Min %d Max %d Received %d",
            m_MinimumRequiredValue, m_MaximumAllowedValue, newValue           
            );
        goto exit;
    }   

    status = GetPortPropertyCache()->SetPropertyULong(
        m_PropertyName,
        newValue
        );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Unable to save ULONG property %d in Property Cache. Error = 0x%08x", 
            m_PropertyName, status);
        goto exit;
    }
    *pBytesRead = sizeof(ULONG);

exit:
    TraceActExit(status);
    return status;
}

//==============================================================================================
CSimplePropertyCacheUCharWithHeaderSetOidJob::CSimplePropertyCacheUCharWithHeaderSetOidJob(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ UCHAR ObjectType,
    _In_ UCHAR ObjectRevision,
    _In_ size_t ObjectSize,
    _In_ WFC_ACTIVITY_ID ActivityId ) 
        : CSimpleSetOidJob(ActivityId) 
{
    m_PropertyName = PropertyName;
    m_ObjectType = ObjectType;
    m_ObjectRevision = ObjectRevision;
    m_ObjectSize = (USHORT)ObjectSize;
}

NDIS_STATUS CSimplePropertyCacheUCharWithHeaderSetOidJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_OBJECT_HEADER* pInHeader = (NDIS_OBJECT_HEADER*)pInformationBuffer;
    
    TraceActEntry();

    NT_ASSERT(pInformationBuffer);
    *pBytesRead  = 0;
    *pBytesNeeded = m_ObjectSize;

    if (InformationBufferLength < m_ObjectSize)
    {
        // Should already be validated in CAdapter::PreprocessOid
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    // Find offsets that we would use
    pInHeader = (NDIS_OBJECT_HEADER*)pInformationBuffer;

    if (!VERIFY_NDIS_OBJECT_HEADER_DEFAULT_PTR(pInHeader, 
            m_ObjectType,
            m_ObjectRevision,
            m_ObjectSize))
    {
        status = NDIS_STATUS_INVALID_DATA;
        WFCActError("Invalid NDIS object header. Expected (T:%d R:%d S:%d) Received (T:%d R:%d S:%d)",
            m_ObjectType, m_ObjectRevision, m_ObjectSize,
            pInHeader->Type, pInHeader->Revision, pInHeader->Size
            );
        goto exit;
    }

    status = GetPortPropertyCache()->SetPropertyUchar(
        m_PropertyName,
        *((BYTE*)pInformationBuffer + sizeof(NDIS_OBJECT_HEADER)));

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Unable to save  property %d in Property Cache. Error = 0x%08x", 
            m_PropertyName, status);
        goto exit;
    }

    *pBytesRead = *pBytesNeeded;

exit:
    TraceActExit(status);
    return status;
}

//==============================================================================================

CSimplePropertyCacheUCharWithHeaderQueryOidJob::CSimplePropertyCacheUCharWithHeaderQueryOidJob(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ UCHAR ObjectType,
    _In_ UCHAR ObjectRevision,
    _In_ size_t ObjectSize,
    _In_ WFC_ACTIVITY_ID ActivityId ) 
        : CSimpleQueryOidJob(ActivityId) 
{
    m_PropertyName = PropertyName;
    m_ObjectType = ObjectType;
    m_ObjectRevision = ObjectRevision;
    m_ObjectSize = (USHORT)ObjectSize;
}


NDIS_STATUS CSimplePropertyCacheUCharWithHeaderQueryOidJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    NDIS_OBJECT_HEADER* pOutHeader = (NDIS_OBJECT_HEADER*)pInformationBuffer;
    BYTE* pData;
    BYTE value;
    TraceActEntry();

    NT_ASSERT(pInformationBuffer);
    
    RtlZeroMemory(pInformationBuffer, InformationBufferLength);

    *pBytesNeeded =  m_ObjectSize;
    if (InformationBufferLength < m_ObjectSize)
    {
        // Should already be validated in CAdapter::PreprocessOid
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    // Find offsets that we would fill
    pOutHeader = (NDIS_OBJECT_HEADER*)pInformationBuffer;
    pData= (UCHAR *)pInformationBuffer + sizeof(NDIS_OBJECT_HEADER);

    ASSIGN_NDIS_OBJECT_HEADER_PTR(pOutHeader, 
        m_ObjectType,
        m_ObjectRevision,
        m_ObjectSize);

    status = GetPortPropertyCache()->GetPropertyUchar(
        m_PropertyName,
        &value);
    
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Unable to find UChar property %d in Property Cache. Error = 0x%08x", 
            m_PropertyName, status);
        goto exit;
    }
    
    *pData = value;
    
    *pBytesWritten = *pBytesNeeded;
    
exit:
    TraceActExit(status);
    return status;
}

//==============================================================================================
NDIS_STATUS CSimplePropertyCacheBooleanQueryOidJob::QueryData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _Inout_ PUINT  pBytesWritten,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( InformationBufferLength < sizeof(BOOLEAN) )
    {
        *pBytesNeeded = sizeof(BOOLEAN);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    status = GetPortPropertyCache()->GetPropertyBoolean(
        m_PropertyName,
        (BOOLEAN*)pInformationBuffer
        );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Unable to find BOOLEAN property %d in Property Cache. Error = 0x%08x", 
            m_PropertyName, status);
        goto exit;
    }

    *pBytesWritten = sizeof(BOOLEAN);
    
exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CSimplePropertyCacheBooleanSetOidJob::SetData(
    _In_ PVOID  pInformationBuffer,
    _In_ UINT   InformationBufferLength,
    _In_ PUINT  pBytesRead,
    _Inout_ PUINT  pBytesNeeded
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    TraceActEntry();

    if ( InformationBufferLength < sizeof(BOOLEAN) )
    {
        *pBytesNeeded = sizeof(BOOLEAN);
        status = NDIS_STATUS_BUFFER_OVERFLOW;
        goto exit;
    }

    status = GetPortPropertyCache()->SetPropertyBoolean(
        m_PropertyName,
        *((BOOLEAN*)pInformationBuffer)
        );
    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Unable to save BOOLEAN property %d in Property Cache. Error = 0x%08x", 
            m_PropertyName, status);
        goto exit;
    }
    *pBytesRead = sizeof(BOOLEAN);

exit:
    TraceActExit(status);
    return status;
}
//==============================================================================================

CSimpleSetDeviceCommandOidJob::CSimpleSetDeviceCommandOidJob(
    _In_ WFC_ACTIVITY_ID ActivityId,
    _In_ bool UseTask ) 
    : COidJobBase(ActivityId),
      m_TaskCommand(ActivityId),
      m_UseTask(UseTask)
{
    m_pInformationBuffer = NULL;
    m_InformationBufferLength = 0;
    m_pBytesRead = NULL;
    m_pBytesNeeded = NULL;
    m_pCommandDataBuffer = NULL;
    m_CommandBufferLength = 0;
}

CSimpleSetDeviceCommandOidJob::~CSimpleSetDeviceCommandOidJob ()
{
    if (m_pCommandDataBuffer)
    {
        delete[] m_pCommandDataBuffer;
    }
}

// COidJobBase
NDIS_STATUS CSimpleSetDeviceCommandOidJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    m_Oid = QUERY_OID_VALUE(pOidRequest);
    m_pInformationBuffer = SET_OID_BUFFER(pOidRequest);
    m_InformationBufferLength = SET_OID_BUFFER_LENGTH(pOidRequest);
    m_pBytesRead = &SET_OID_BYTES_READ(pOidRequest);
    m_pBytesNeeded = &SET_OID_BYTES_NEEDED(pOidRequest);
    
    return NDIS_STATUS_SUCCESS;
}

VOID CSimpleSetDeviceCommandOidJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);        
}
    
// CJobBase
void CSimpleSetDeviceCommandOidJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG CommandId;
    ULONG CommandBufferLength = 0;
    BYTE* CommandBuffer = NULL;

    TraceActEntry();

    status = m_TaskCommand.get_TaskDeviceCommand(
        &m_pDeviceCommand );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't get device command from task object.");
        NT_ASSERT(FALSE);
        goto exit;
    }

    status = GetDeviceCommandData(
        m_pInformationBuffer,
        m_InformationBufferLength,
        m_pBytesRead,
        m_pBytesNeeded,
        sizeof(WFC_MESSAGE_METADATA),
        &CommandId,
        &CommandBufferLength,
        &CommandBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't get device command data");
        goto exit;
    }

    m_pCommandDataBuffer = CommandBuffer;
    m_CommandBufferLength = CommandBufferLength;

    status = m_pDeviceCommand->Initialize(
        get_PortId(),
        CommandId,
        CommandBufferLength,
        CommandBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize device command with given data.");
        goto exit;
    }

    if ( m_UseTask )
    {
        status = m_TaskCommand.Initialize(
            m_pAdapter->GetNotificationManager(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetEventQueue() );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Couldn't initialize task.");
            goto exit;
        }

        status = StartTask(
            &m_TaskCommand );
    }
    else
    {
        status = QueueDeviceCommand(
            m_pDeviceCommand );

        if ( status != NDIS_STATUS_SUCCESS )
        {
            WFCActErrorStatus("Queue device command failed.");
            goto exit;
        }
    }

    WFCActInfo("Device Command Queued. Waiting for completion.");
        
exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        // Just complete the job
        CompleteJob(status);
    }

    TraceActExit(status);
}


void CSimpleSetDeviceCommandOidJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS commandStatus;
    ULONG messageLength = 0;
    WFC_MESSAGE_METADATA* pMessageHeader = NULL;
    
    UNREFERENCED_PARAMETER(SenderObject);
    NT_ASSERT(
        SenderObject == m_pDeviceCommand ||
        SenderObject == &m_TaskCommand );
    
    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Device command failed with status %x.", OperationStatus);
        goto exit;
    }

    OperationStatus = m_pDeviceCommand->get_CommandResult(
        &messageLength,
        (BYTE**)(&pMessageHeader));

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Not results available for the command");
        goto exit;
    }

    if (messageLength < sizeof(WFC_MESSAGE_METADATA) )
    {
        NT_ASSERT(FALSE);
        WFCActError("Command failed by lower edge");
        OperationStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    if ( m_UseTask )
    {
        OperationStatus = CMessageHelper::GetStatusFromTaskOutput(
            &m_TaskCommand,
            &commandStatus );

        if ( OperationStatus != NDIS_STATUS_SUCCESS )
        {
            WFCActError("Couldn't get WABI status from task result 0x%08x", OperationStatus);
            goto exit;
        }        

    }
    else
    {
        OperationStatus = CMessageHelper::GetStatusFromCommandResult(
            m_pDeviceCommand,
            &commandStatus );

        if ( OperationStatus != NDIS_STATUS_SUCCESS )
        {
            WFCActError("Couldn't get WABI status from command result 0x%08x", OperationStatus);
            goto exit;
        }        
    }


    if ( commandStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Device command completed with status 0x%08x.", commandStatus);
        OperationStatus = commandStatus;
        goto exit;
    }

    WFCActInfo("Device Command succeeded.");
    
exit:
    // Notify the child
    OperationStatus = OnDeviceCommandCompletion(
        OperationStatus,
        messageLength,
        (WFC_MESSAGE_METADATA *)pMessageHeader );

    CompleteJob(
        OperationStatus );
}

NDIS_STATUS CSimpleSetDeviceCommandOidJob::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG MessageLength,
    _In_reads_bytes_(MessageLength) WFC_MESSAGE_METADATA* pMessageHeader )
{
    UNREFERENCED_PARAMETER(MessageLength);
    UNREFERENCED_PARAMETER(pMessageHeader);

    return OperationStatus;
}

void CSimpleSetDeviceCommandOidJob::OnJobCancelled()
{
    // We do nothing yet
}

//==============================================================================================
CSimpleQueryDeviceCommandOidJob::CSimpleQueryDeviceCommandOidJob(
    _In_ WFC_ACTIVITY_ID ActivityId ) 
    : COidJobBase(ActivityId),
      m_DeviceCommand(ActivityId)      
{
    m_pInformationBuffer = NULL;
    m_InformationBufferLength = 0;
    m_pBytesWritten = NULL;
    m_pBytesNeeded = NULL;
    m_pCommandDataBuffer = NULL;
    m_CommandBufferLength = 0;
}

CSimpleQueryDeviceCommandOidJob::~CSimpleQueryDeviceCommandOidJob()
{
    if (m_pCommandDataBuffer)
    {
        delete[] m_pCommandDataBuffer;
    }
}

// COidJobBase
NDIS_STATUS CSimpleQueryDeviceCommandOidJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    m_Oid = QUERY_OID_VALUE(pOidRequest);
    m_pInformationBuffer = QUERY_OID_BUFFER(pOidRequest);
    m_InformationBufferLength = QUERY_OID_BUFFER_LENGTH(pOidRequest);
    m_pBytesWritten = &QUERY_OID_BYTES_WRITTEN(pOidRequest);
    m_pBytesNeeded = &QUERY_OID_BYTES_NEEDED(pOidRequest);
    

    return NDIS_STATUS_SUCCESS;
}

VOID CSimpleQueryDeviceCommandOidJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);        
}

// CJobBase
void CSimpleQueryDeviceCommandOidJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG CommandId;
    ULONG CommandBufferLength = 0;
    WFC_MESSAGE_METADATA* CommandBuffer = NULL;

    TraceActEntry();

    status = GetDeviceCommandId(&CommandId);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't get device command ioctl");
        goto exit;
    }

    CommandBufferLength = sizeof(WFC_MESSAGE_METADATA);
    CommandBuffer = new WFC_MESSAGE_METADATA;

    if (CommandBuffer == NULL) 
    {
        WFCActErrorStatus("Failed to allocate memory for the command buffer");
        status = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    m_pCommandDataBuffer = CommandBuffer;
    m_CommandBufferLength = CommandBufferLength;

    status = m_DeviceCommand.Initialize(
        get_PortId(),
        CommandId,
        CommandBufferLength,
        (BYTE*)CommandBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize device command with given data.");
        goto exit;
    }

    status = QueueDeviceCommand(
        &m_DeviceCommand );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Queue device command failed.");
        goto exit;
    }

    WFCActInfo("Device Command Queued. Waiting for completion.");
        
exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        // Just complete the job
        CompleteJob(status);
    }

    TraceActExit(status);
}


void CSimpleQueryDeviceCommandOidJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS commandStatus;
    ULONG commandBufferLength = 0;
    WFC_MESSAGE_METADATA* commandBuffer = NULL;
    
    UNREFERENCED_PARAMETER(SenderObject);
    NT_ASSERT(SenderObject == &m_DeviceCommand);
    
    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Device command failed with status %x.", OperationStatus);
        goto exit;
    }

    OperationStatus = m_DeviceCommand.get_CommandResult(
        &commandBufferLength,
        (BYTE**)(&commandBuffer));

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Not results available for the command");
        goto exit;
    }

    if (commandBufferLength < sizeof(WFC_MESSAGE_METADATA) )
    {
        NT_ASSERT(FALSE);
        WFCActError("Command failed by lower edge");
        OperationStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    OperationStatus = CMessageHelper::GetStatusFromCommandResult(
        &m_DeviceCommand,
        &commandStatus );

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't get WABI status from command result 0x%08x", OperationStatus);
        goto exit;
    }        


    if ( commandStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Device command completed with status 0x%08x", commandStatus);
        OperationStatus = commandStatus;
        goto exit;
    }

    WFCActInfo("Device Command succeeded.");
    
exit:
    // Notify the child
    OperationStatus = OnDeviceCommandCompletion(
        OperationStatus,
        commandBufferLength,
        (BYTE*)commandBuffer,
        m_pInformationBuffer,
        m_InformationBufferLength,
        m_pBytesWritten,
        m_pBytesNeeded);

    CompleteJob(
        OperationStatus );
}

NDIS_STATUS CSimpleQueryDeviceCommandOidJob::OnDeviceCommandCompletion(
    _In_ NDIS_STATUS OperationStatus,
    _In_ ULONG CommandBufferLength,
    _In_reads_bytes_(CommandBufferLength) BYTE* pCommandBuffer,
    _In_ PVOID pInformationBuffer,
    _In_ UINT InformationBufferLength,
    _Inout_ PUINT pBytesWritten,
    _Inout_ PUINT pBytesNeeded)
{
    UNREFERENCED_PARAMETER(OperationStatus);
    UNREFERENCED_PARAMETER(CommandBufferLength);
    UNREFERENCED_PARAMETER(pCommandBuffer);
    UNREFERENCED_PARAMETER(pInformationBuffer);
    UNREFERENCED_PARAMETER(InformationBufferLength);
    UNREFERENCED_PARAMETER(pBytesWritten);
    UNREFERENCED_PARAMETER(pBytesNeeded);

    return OperationStatus;
}

void CSimpleQueryDeviceCommandOidJob::OnJobCancelled()
{
    // We do nothing yet
}

//==============================================================================================
CSimpleMethodDeviceCommandOidJob::CSimpleMethodDeviceCommandOidJob(
    _In_ WFC_ACTIVITY_ID ActivityId) 
    : COidJobBase(ActivityId),
      m_DeviceCommand(ActivityId)      
{
    m_pInformationBuffer = NULL;
    m_InputBufferLength = 0;
    m_OutputBufferLength = 0;
    m_pBytesRead = NULL;
    m_pBytesNeeded = NULL;
    m_pBytesNeeded = NULL;
    m_pBytesWritten = NULL;
    m_pCommandDataBuffer = NULL;
    m_CommandBufferLength = 0;
}

CSimpleMethodDeviceCommandOidJob::~CSimpleMethodDeviceCommandOidJob ()
{
    if (m_pCommandDataBuffer)
    {
        delete[] m_pCommandDataBuffer;
    }
}

// COidJobBase
NDIS_STATUS CSimpleMethodDeviceCommandOidJob::InitializeFromOidRequest(
    _In_ PNDIS_OID_REQUEST pOidRequest
    )
{
    m_Oid = METHOD_OID_VALUE(pOidRequest);
    m_pInformationBuffer = METHOD_OID_BUFFER(pOidRequest);
    m_InputBufferLength = METHOD_OID_INPUT_LENGTH(pOidRequest);
    m_OutputBufferLength = METHOD_OID_OUTPUT_LENGTH(pOidRequest);
    m_pBytesRead = &METHOD_OID_BYTES_READ(pOidRequest);
    m_pBytesNeeded = &METHOD_OID_BYTES_NEEDED(pOidRequest);
    m_pBytesWritten = &METHOD_OID_BYTES_WRITTEN(pOidRequest);
    
    return NDIS_STATUS_SUCCESS;
}

VOID CSimpleMethodDeviceCommandOidJob::OnOidJobCompletedEvent(
    _In_ NDIS_STATUS NdisStatus
    )
{
    // Complete the OID back to NDIS
    m_pAdapter->CompleteOidRequest(
        COidJobBase::GetOidRequest(),
        NdisStatus);        
}
    
// CJobBase
void CSimpleMethodDeviceCommandOidJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG CommandId;
    ULONG CommandBufferLength = 0;
    BYTE* CommandBuffer = NULL;
    WFC_MESSAGE_METADATA* pCommandHeader;

    TraceActEntry();

    status = GetDeviceCommandData(
        m_pInformationBuffer,
        m_InputBufferLength,
        m_pBytesRead,
        m_pBytesNeeded,
        sizeof(WFC_MESSAGE_METADATA),
        &CommandId,
        &CommandBufferLength,
        &CommandBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't get device command data");
        goto exit;
    }

    m_pCommandDataBuffer = CommandBuffer;
    m_CommandBufferLength = CommandBufferLength;

    status = m_DeviceCommand.Initialize(
        get_PortId(),
        CommandId,
        CommandBufferLength,
        CommandBuffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Couldn't initialize device command with given data.");
        goto exit;
    }

    // For method requests, we may have a larger output size compared to input
    // Let CtlPlane know about this
    pCommandHeader = (WFC_MESSAGE_METADATA*)CommandBuffer;    
    pCommandHeader->Command()->MinimumOutputLength = m_OutputBufferLength;

    status = QueueDeviceCommand(
        &m_DeviceCommand );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCActErrorStatus("Queue device command failed.");
        goto exit;
    }

    WFCActInfo("Device Command Queued. Waiting for completion.");
        
exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        // Just complete the job
        CompleteJob(status);
    }

    TraceActExit(status);
}


void CSimpleMethodDeviceCommandOidJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject )
{
    NDIS_STATUS commandStatus;
    ULONG messageLength = 0;
    WFC_MESSAGE_METADATA* pMessageHeader = NULL;
    
    UNREFERENCED_PARAMETER(SenderObject);
    NT_ASSERT(
        SenderObject == &m_DeviceCommand);
    
    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Device command failed with status %x.", OperationStatus);
        goto exit;
    }

    OperationStatus = m_DeviceCommand.get_CommandResult(
        &messageLength,
        (BYTE**)(&pMessageHeader));

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Not results available for the command");
        goto exit;
    }

    if (messageLength < sizeof(WFC_MESSAGE_METADATA) )
    {
        NT_ASSERT(FALSE);
        WFCActError("Command failed by lower edge");
        OperationStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    OperationStatus = CMessageHelper::GetStatusFromCommandResult(
        &m_DeviceCommand,
        &commandStatus );

    if ( OperationStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActError("Couldn't get WABI status from command result 0x%08x", OperationStatus);
        goto exit;
    }        


    if ( commandStatus != NDIS_STATUS_SUCCESS )
    {
        WFCActInfo("Device command completed with status 0x%08x.", commandStatus);
        OperationStatus = commandStatus;
        goto exit;
    }

    WFCActInfo("Device Command succeeded.");
    
exit:
    // Notify the child
    OperationStatus = OnDeviceCommandCompletion(
        m_pInformationBuffer,
        m_OutputBufferLength,
        m_pBytesWritten,
        m_pBytesNeeded,
        OperationStatus,
        messageLength,
        (WFC_MESSAGE_METADATA *)pMessageHeader );

    CompleteJob(
        OperationStatus );
}

void CSimpleMethodDeviceCommandOidJob::OnJobCancelled()
{
    // We do nothing yet
}

