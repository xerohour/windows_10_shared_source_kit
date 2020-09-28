/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Dot11Utils.cpp

Abstract:

    Implementation for Dot11 IEs

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "dot11utils.tmh"

NDIS_STATUS
Dot11GetIEsSizeFromIEBlob(
    _In_reads_bytes_(BufferLength)  PUCHAR   pIEBlobBuffer,
    _In_  ULONG                   BufferLength,
    _Out_ PULONG                  pInfoElemBlobSize
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_INFO_ELEMENT pInfoElemHdr = NULL;
    ULONG               InfoBlobSize = 0;

    // Walk through to check nothing is malformed
    while (BufferLength >= sizeof(DOT11_INFO_ELEMENT))
    {
        // Atleast one more header is present
        pInfoElemHdr = (PDOT11_INFO_ELEMENT)(pIEBlobBuffer + InfoBlobSize);
        BufferLength -= sizeof(DOT11_INFO_ELEMENT);
        InfoBlobSize += sizeof(DOT11_INFO_ELEMENT);
        
        if (pInfoElemHdr->Length == 0)
        {
            continue;
        }

        if (BufferLength < pInfoElemHdr->Length)
        {
            // Incomplete/bad info element
            ndisStatus = NDIS_STATUS_NOT_ACCEPTED;
            break;
        }

        // Consume info element 
        BufferLength -= pInfoElemHdr->Length;
        InfoBlobSize += pInfoElemHdr->Length;
    }

    *pInfoElemBlobSize = InfoBlobSize;
    return ndisStatus;
}


NDIS_STATUS
Dot11ValidateIEBlob(
    _In_reads_bytes_(uSizeOfBlob) PUCHAR pucInfoBlob,
    _In_ ULONG uSizeOfBlob
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_INFO_ELEMENT pInfoEleHdr = NULL;
    ULONG uRequiredSize = 0;

    while(uSizeOfBlob) {

        pInfoEleHdr = (PDOT11_INFO_ELEMENT)pucInfoBlob;
        if (uSizeOfBlob < sizeof(DOT11_INFO_ELEMENT)) {
            break;
        }

        uRequiredSize = (ULONG)(pInfoEleHdr->Length) + sizeof(DOT11_INFO_ELEMENT);
        if (uSizeOfBlob < uRequiredSize) {
            break;
        }

        uSizeOfBlob -= uRequiredSize;
        pucInfoBlob += uRequiredSize;
    }

    if (uSizeOfBlob) {
        ndisStatus = NDIS_STATUS_FAILURE;
    }
    return ndisStatus;
}

NDIS_STATUS
Dot11GetDeviceAddressFromBSSEntry(
    _In_    WDI_BSS_ENTRY_CONTAINER *pParsedEntry,
    _Inout_ UINT16 *pBeaconBufferLength,
    _Inout_ UINT16 *pProbeBufferLength,
    _Inout_ DOT11_MAC_ADDRESS *pWfdDeviceAddress,
    _Inout_ bool *pbIsWFDDevice
    )
{
    NDIS_STATUS         ndisStatus = NDIS_STATUS_SUCCESS;
    NTSTATUS            ntStatus = STATUS_SUCCESS;
    BYTE                *pBuffer;
    UINT16              bufferLength;
    ULONG               ieBlobSize = 0;

    if (pParsedEntry->Optional.BeaconFrame_IsPresent)
    {
        pBuffer = (PBYTE) pParsedEntry->BeaconFrame.pElements;
        bufferLength = (UINT16) pParsedEntry->BeaconFrame.ElementCount;
        ieBlobSize = 0;

        if (bufferLength < FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))
        {
            WFCError("Dot11GetDeviceAddressFromBSSEntry[BEACON]: Invalid buffer length %u < %d", bufferLength, FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements));
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            goto Exit;
        }

        //
        // Validate that IE blob is good. We dont save it unless its good.
        // As part of this process, we also get the real length of IE blob
        //
        ndisStatus = Dot11GetIEsSizeFromIEBlob(
            (pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements)),
            (bufferLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements)),
            &ieBlobSize);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("Dot11GetDeviceAddressFromBSSEntry[BEACON]: Dot11GetIEsSizeFromIEBlob FAILed - bufferLength=<%d>", bufferLength);
            goto Exit;
        }

        //
        // P2P Beacon frames return the DeviceAddress in the WFD_OTA_ELEMENT_DEVICE_ID attribute in the P2P IE
        //
        ntStatus = WFDGetDeviceAddress(
                (pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements)),
                ieBlobSize,
                pWfdDeviceAddress);
        if (NT_SUCCESS(ntStatus))
        {
            *pbIsWFDDevice = true;
        }

        //
        // Adjust the frame length based on the verification above
        //
        *pBeaconBufferLength = (UINT16) (FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements) + ieBlobSize);
    }

        
    if (pParsedEntry->Optional.ProbeResponseFrame_IsPresent)
    {
        WFD_OTA_DEVICE_INFO deviceInfo = { 0 };
        ULONG deviceInfoLength = sizeof(deviceInfo);

        pBuffer = (PBYTE) pParsedEntry->ProbeResponseFrame.pElements;
        bufferLength = (UINT16) pParsedEntry->ProbeResponseFrame.ElementCount;
        ieBlobSize = 0;

        if (bufferLength < FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements))
        {
            WFCError("Dot11GetDeviceAddressFromBSSEntry[PROBE]: Invalid buffer length %u < %d", bufferLength, FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements));
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            goto Exit;
        }

        //
        // Validate that IE blob is good. We dont save it unless its good.
        // As part of this process, we also get the real length of IE blob
        //
        ndisStatus = Dot11GetIEsSizeFromIEBlob(
            (pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements)),
            (bufferLength - FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements)),
            &ieBlobSize);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("Dot11GetDeviceAddressFromBSSEntry[PROBE]: Dot11GetIEsSizeFromIEBlob FAILed - bufferLength=<%d>", bufferLength);
            goto Exit;
        }

        //
        // P2P ProbeResponse frames return DeviceAddress in the WFD_OTA_ELEMENT_DEVICE_INFO attribute in the P2P IE
        //
        ntStatus = WFDGetDeviceInfo(
                (pBuffer + FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements)),
                ieBlobSize,
                &deviceInfo,
                &deviceInfoLength,
                NULL,
                NULL);
        if ((NT_SUCCESS(ntStatus)) ||
            (ntStatus == STATUS_NDIS_BUFFER_TOO_SHORT))
        {
            RtlCopyMemory( pWfdDeviceAddress, &deviceInfo.DeviceAddress, sizeof(DOT11_MAC_ADDRESS) );
            *pbIsWFDDevice = true;
        }

        //
        // Adjust the frame length based on the verification above
        //
        *pProbeBufferLength = (UINT16) (FIELD_OFFSET(DOT11_BEACON_FRAME, InfoElements) + ieBlobSize);
    }

Exit:
    return ndisStatus;
}

_Success_(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS
Dot11GetInfoElementFromIEBlob(
    _In_reads_bytes_(uSizeOfBlob) PUCHAR pucInfoBlob,
    _In_ ULONG uSizeOfBlob,
    _In_ UCHAR ucInfoId,
    _Out_ PUCHAR pucLength,
    _Outptr_result_bytebuffer_(*pucLength) PVOID * ppvInfoEle
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    PDOT11_INFO_ELEMENT pInfoEleHdr = NULL;
    ULONG uRequiredSize = 0;
    BOOLEAN bFound = FALSE;

    *pucLength = 0;
    *ppvInfoEle = NULL;
    while(uSizeOfBlob) {

        pInfoEleHdr = (PDOT11_INFO_ELEMENT)pucInfoBlob;
        if (uSizeOfBlob < sizeof(DOT11_INFO_ELEMENT)) {
            break;
        }

        uRequiredSize = (ULONG)(pInfoEleHdr->Length) + sizeof(DOT11_INFO_ELEMENT);
        if (uSizeOfBlob < uRequiredSize) {
            break;
        }

        if (pInfoEleHdr->ElementID == ucInfoId) {
            *pucLength = pInfoEleHdr->Length;
            *ppvInfoEle = pucInfoBlob + sizeof(DOT11_INFO_ELEMENT);
            bFound = TRUE;
            break;
        }

        uSizeOfBlob -= uRequiredSize;
        pucInfoBlob += uRequiredSize;
    }

    if (!bFound) {
        ndisStatus = NDIS_STATUS_FAILURE;
    }
    return ndisStatus;
}


_Success_(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS
Dot11CopySSIDFromIEBlob(
    _In_reads_bytes_(uSizeOfBlob) PUCHAR pucInfoBlob,
    _In_ ULONG uSizeOfBlob,
    _Out_ PDOT11_SSID pDot11SSID
    )
{
    UCHAR ucLength = 0;
    PVOID pParam = NULL;
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ndisStatus = Dot11GetInfoElementFromIEBlob(
                    pucInfoBlob,
                    uSizeOfBlob,
                    DOT11_INFO_ELEMENT_ID_SSID,
                    &ucLength,
                    &pParam
                    );
    if ((ndisStatus != NDIS_STATUS_SUCCESS) || (NULL == pParam))
    {
        ndisStatus = (ndisStatus != NDIS_STATUS_SUCCESS) ? ndisStatus : NDIS_STATUS_FAILURE;
        goto error;
    }

    if (ucLength > DOT11_SSID_MAX_LENGTH) {
        ndisStatus = NDIS_STATUS_INVALID_LENGTH;
        goto error;
    }

    pDot11SSID->uSSIDLength = (ULONG)ucLength;
    RtlCopyMemory(pDot11SSID->ucSSID, pParam, pDot11SSID->uSSIDLength);

error:
    return ndisStatus;
}

_Success_(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS
Dot11CopyInfoElementFromIEBlob(
    _In_reads_bytes_(uSizeOfBlob) PUCHAR pucInfoBlob,
    _In_ ULONG uSizeOfBlob,
    _In_ UCHAR ucInfoId,
    _Out_ PUCHAR pucLength,
    _In_ ULONG uBufSize,
    _Out_ PVOID pvInfoEle
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    UCHAR ucLength = 0;
    PVOID pvTemp = NULL;

    ndisStatus = Dot11GetInfoElementFromIEBlob(
            pucInfoBlob,
            uSizeOfBlob,
            ucInfoId,
            &ucLength,
            &pvTemp
            );
    if ((ndisStatus != NDIS_STATUS_SUCCESS) || (NULL == pvTemp))
    {
        ndisStatus = (ndisStatus != NDIS_STATUS_SUCCESS) ? ndisStatus : NDIS_STATUS_FAILURE;
        goto error;
    }

    *pucLength = ucLength;
    if (uBufSize < ucLength) {
        ndisStatus = STATUS_BUFFER_TOO_SMALL;
        goto error;
    }

    RtlCopyMemory(
            pvInfoEle,
            pvTemp,
            ucLength
            );

error:
    return ndisStatus;
}


BOOLEAN
Dot11IsHiddenSSID(
    _In_reads_bytes_(SSIDLength) PUCHAR               SSID,
    _In_ ULONG                SSIDLength
    )
{
    ULONG i;
    //
    // We flag this as Hidden SSID if the Length is 0
    // of the SSID only contains 0's
    //
    if (SSIDLength == 0)
    {
        // Zero length is hidden SSID
        return TRUE;
    }

    for (i = 0; i < SSIDLength; i++)
    {
        if (SSID[i] != 0)
        {
            // Non zero SSID value
            return FALSE;
        }
    }

    // All 0's
    return TRUE;
}

UINT32
ConvertOperatingClassToBand(
    _In_ UCHAR OperatingClass)
{
    UINT32 bandId = WDI_BAND_ID_ANY;

    //
    // Maps Operating Class to Band ID according to table E-1 in 802.11 spec
    //
    switch (OperatingClass)
    {
    case 12:
        __fallthrough;
    case 32:
        __fallthrough;
    case 33:
        bandId = WDI_BAND_ID_2400;
        break;
    case 1:
        __fallthrough;
    case 5:
        bandId = WDI_BAND_ID_5000;
        break;
    }

    return bandId;
}

NDIS_STATUS
ConvertP2PChannelToBandChannel(
    _In_ CAdapterPropertyCache* pAdapterPropertyCache,
    _In_ PWFD_OTA_CHANNEL pChannelInfo,
    _Out_ UINT32* pBandID,
    _Out_ UINT32* pChannelNumber )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    UINT32 startFrequency;
    
    if ( pChannelInfo->CountryString[2] != 0x04 )
    {
        status = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    }

    //
    // StartFrequency map for table J-4 - Global Operating Classes in
    // 802.11 specification.
    //
    switch(pChannelInfo->OperatingClass)
    {
    case 81:
    case 83:
    case 84:
        startFrequency = 2407000;
        break;

    case 82:
        startFrequency = 2414000;
        break;

    case 112:
    case 113:
    case 115:
    case 116:
    case 117:
    case 118:
    case 119:
    case 120:
    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
    case 127:
        startFrequency = 5000000;
        break;

#if _SUPPORT_NON_STANDARD_CHANNELS
    case 84:
    case 95:
        startFrequency = 3000000;
        break;

    case 96:
        startFrequency = 3002500;
        break;

    case 101:
        startFrequency = 4850000;
        break;

    case 102:
        startFrequency = 4890000;
        break;

    case 103:
        startFrequency = 4937500;
        break;

    case 104:
    case 105:
    case 106:
    case 107:
    case 109:
    case 110:
        startFrequency = 4000000;
        break;

    case 108:
    case 111:
        startFrequency = 4002500;
        break;

    case 114:
        startFrequency = 5002500;
        break;
#endif // _SUPPORT_NON_STANDARD_CHANNELS

    default:
        status = NDIS_STATUS_NOT_SUPPORTED;
        goto exit;
    }

    *pChannelNumber = pChannelInfo->ChannelNumber;

    status = pAdapterPropertyCache->GetBandIDForStartingFrequency(
        startFrequency,
        pBandID );

exit:
    return status;
}

NDIS_STATUS
Dot11ParseRSNIE(
    _In_reads_bytes_(RSNIELength) PUCHAR RSNIEData,
    _In_ ULONG OUIPrefix,
    _In_ UCHAR RSNIELength,
    _Out_ PRSN_IE_INFO RSNIEInfo )
{
    NdisZeroMemory(RSNIEInfo, sizeof(RSN_IE_INFO));
    RSNIEInfo->OUIPrefix = OUIPrefix;

    //
    // Get version (only mandatory field)
    //
    if (RSNIELength < 2)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->Version = *((USHORT UNALIGNED *)RSNIEData);
    RSNIEData += 2;
    RSNIELength -= 2;

    // 
    // Get group cipher 
    //
    if (RSNIELength < 4)
    {
        return (RSNIELength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->GroupCipherCount = 1;
    RSNIEInfo->GroupCipher = RSNIEData;
    RSNIEData += 4;
    RSNIELength -= 4;

    // 
    // Get pairwise cipher count
    //
    if (RSNIELength < 2)
    {
        return (RSNIELength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->PairwiseCipherCount = *((USHORT UNALIGNED *)RSNIEData);
    RSNIEData += 2;
    RSNIELength -= 2;

    //
    // Get pairwise cipher
    //
    if (RSNIELength < RSNIEInfo->PairwiseCipherCount * 4)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->PairwiseCipher = RSNIEData;
    RSNIEData += RSNIEInfo->PairwiseCipherCount * 4;
    RSNIELength -= (UCHAR)RSNIEInfo->PairwiseCipherCount * 4;

    //
    // Get AKM suite count
    //
    if (RSNIELength < 2)
    {
        return (RSNIELength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->AKMSuiteCount = *((USHORT UNALIGNED *)RSNIEData);
    RSNIEData += 2;
    RSNIELength -= 2;

    //
    // Get AKM suite
    //
    if (RSNIELength < RSNIEInfo->AKMSuiteCount * 4)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->AKMSuite = RSNIEData;
    RSNIEData += RSNIEInfo->AKMSuiteCount * 4;
    RSNIELength -= (UCHAR)RSNIEInfo->AKMSuiteCount * 4;

    //
    // Get RSN capability
    //

    if (RSNIELength < 2)
    {
        return (RSNIELength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->Capability = *((USHORT UNALIGNED *)RSNIEData);
    RSNIEData += 2;
    RSNIELength -= 2;

    //
    // Get PMKID count
    //

    if (RSNIELength < 2)
    {
        return (RSNIELength == 0) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->PMKIDCount = *((USHORT UNALIGNED *)RSNIEData);
    RSNIEData += 2;
    RSNIELength -= 2;

    //
    // Get PMKID
    //
    if (RSNIELength != RSNIEInfo->PMKIDCount * 16)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    RSNIEInfo->PMKID = RSNIEData;
    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Dot11ParseRMIE(
    _In_reads_bytes_(RMIELength) PUCHAR RMIEData,
    _In_ UCHAR RMIELength,
    _Out_ PRM_IE_INFO pRMIEInfo)
{
    NdisZeroMemory(pRMIEInfo, sizeof(RM_IE_INFO));

    if (RMIELength < RM_IE_INFO_LENGTH)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    // RM_IE_INFO is a bit field so just copy the memory over
	RtlCopyMemory(pRMIEInfo, RMIEData, RM_IE_INFO_LENGTH);

    return NDIS_STATUS_SUCCESS;
}

NDIS_STATUS
Dot11ParseMDIE(
    _In_reads_bytes_(MDIELength) PUCHAR MDIEData,
    _In_ UCHAR MDIELength,
    _Out_ PMD_IE_INFO pMDIEInfo)
{
    NdisZeroMemory(pMDIEInfo, sizeof(*pMDIEInfo));

    if (MDIELength < MD_IE_INFO_LENGTH)
    {
        return NDIS_STATUS_INVALID_DATA;
    }

    // All fields in MD_IE_INFO are fixed size so just copy the memory over
    RtlCopyMemory(pMDIEInfo, MDIEData, MD_IE_INFO_LENGTH);

    return NDIS_STATUS_SUCCESS;
}

DOT11_AUTH_ALGORITHM
Dot11GetAuthAlgoFromRSNOUI(
    _In_ ULONG oui
    )
{
    switch (oui)
    {
        case RSNA_OUI_PREFIX + RSNA_AKM_RSNA:
            return DOT11_AUTH_ALGO_RSNA;

        case RSNA_OUI_PREFIX + RSNA_AKM_RSNA_PSK:
            return DOT11_AUTH_ALGO_RSNA_PSK;

        case WPA_OUI_PREFIX + RSNA_AKM_RSNA:
            return DOT11_AUTH_ALGO_WPA;

        case WPA_OUI_PREFIX + RSNA_AKM_RSNA_PSK:
            return DOT11_AUTH_ALGO_WPA_PSK;

        default:
            return DOT11_AUTH_ALGO_80211_OPEN;
    }
}

DOT11_AUTH_ALGORITHM
Dot11GetAKMSuiteFromRSNIEInfo(
    _In_ PRSN_IE_INFO RSNIEInfo,
    _In_ USHORT index )
{
    if (RSNIEInfo->AKMSuiteCount > index)
    {
        BYTE* pTargetValue = (BYTE*)RSNIEInfo->AKMSuite + (index * 4);
        return Dot11GetAuthAlgoFromRSNOUI(*((ULONG UNALIGNED *)pTargetValue));
    }
    else
    {
        return DOT11_AUTH_ALGO_80211_OPEN;
    }
}

ULONG
Dot11GetFTAKMSuite(
_In_ PRSN_IE_INFO RSNIEInfo,
_In_ USHORT index)
{
    if (RSNIEInfo->AKMSuiteCount > index)
    {
        BYTE* pTargetValue = (BYTE*)RSNIEInfo->AKMSuite + (index * 4);
        switch(*((ULONG UNALIGNED *)pTargetValue))
        {
            case RSNA_OUI_PREFIX + RSNA_AKM_RSNA_FT:
                return RSNA_AKM_RSNA_FT;

            case RSNA_OUI_PREFIX + RSNA_AKM_RSNA_FT_PSK:
                return RSNA_AKM_RSNA_FT_PSK;

            default:
                return RSNA_AKM_RSNA_NONE;
        }
    }
    return RSNA_AKM_RSNA_NONE;
}

UINT32 MapWMMInfoToAcessClass(UCHAR wmmINfo)
{ 
    switch(wmmINfo)
    {
        case 1: 
        case 2: 
            return AC_BK;

        case 0: 
        case 3: 
            return AC_BE;

        case 5: 
        case 4: 
            return AC_VI;

        case 7: 
        case 6: 
            return AC_VO;
#if WDI_DISABLED      
        default:
            return AC_ANY;
#else
        default:
            return AC_PR0;
#endif            
    }
}


