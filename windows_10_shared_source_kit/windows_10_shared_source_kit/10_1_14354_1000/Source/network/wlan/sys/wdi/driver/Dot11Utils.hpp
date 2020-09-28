/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Dot11Utils.cpp

Abstract:

    Implementation for manipulating IEs

Environment:

    Kernel mode

Revision History:

--*/

#pragma once


#define WPA_IE_TAG      0x01f25000
#define WCN_IE_TAG      0x04f25000

#define RSNA_OUI_PREFIX             0xac0f00
#define WPA_OUI_PREFIX              0xf25000

#define RSNA_AKM_RSNA_NONE          0x00000000
#define RSNA_AKM_RSNA               0x01000000
#define RSNA_AKM_RSNA_PSK           0x02000000
#define RSNA_AKM_RSNA_FT            0x03000000
#define RSNA_AKM_RSNA_FT_PSK        0x04000000

#define RSNA_CAPABILITY_PRE_AUTH    0x01

//
// This structure is used for extracting RSN IE in beacon or probe response frame.
// Both RSN (WPA2) and WPA share the same structure. However, for WPA, PMKIDCount
// is also 0 and PKMID NULL.
// 
typedef struct _RSN_IE_INFO
{
    ULONG   OUIPrefix;
    USHORT  Version;
    USHORT  GroupCipherCount;
    USHORT  PairwiseCipherCount;
    USHORT  AKMSuiteCount;
    USHORT  Capability;
    USHORT  PMKIDCount;
    PUCHAR  GroupCipher;
    PUCHAR  PairwiseCipher;
    PUCHAR  AKMSuite;
    PUCHAR  PMKID;
} RSN_IE_INFO, *PRSN_IE_INFO;


#define RM_IE_INFO_LENGTH 5 // RM_IE_INFO has fixed length of 5 octets

//
// This structure is used of extracting RM Enabled Capabilities IE in beacon or probe response frame.
//
typedef struct _RM_IE_INFO
{
    UCHAR  LinkMeasurement : 1;                            //B0
    UCHAR  NeighborReport : 1;                             //B1
    UCHAR  ParallelMeasurements : 1;                       //B2
    UCHAR  RepeatedMeasurements : 1;                       //B3
    UCHAR  BeaconPassiveMeasurement : 1;                   //B4
    UCHAR  BeaconActiveMeasurement : 1;                    //B5
    UCHAR  BeaconTableMeasurement : 1;                     //B6
    UCHAR  BeaconMeasurementReportingConditions : 1;       //B7
    UCHAR  FrameMeasurement : 1;                           //B8
    UCHAR  ChannelLoadMeasurement : 1;                     //B9
    UCHAR  NoiseHistogramMeasurement : 1;                  //B10
    UCHAR  StatisticsMeasurement : 1;                      //B11
    UCHAR  LCIMeasurement : 1;                             //B12
    UCHAR  LICAzimuth : 1;                                 //B13
    UCHAR  TransmitStreamCategoryMeasurement : 1;          //B14
    UCHAR  TriggeredTransmitStreamCategoryMeasurement : 1; //B15
    UCHAR  APChannelReport : 1;                            //B16
    UCHAR  RMMIB : 1;                                      //B17
    UCHAR  OperatingChannelMaxMeasurementDuration : 3;     //B18-B20
    UCHAR  NonoperatingChannelMaxMeasurementDuration : 3;  //B21-B23
    UCHAR  MeasurementPilot : 3;                           //B24-B26
    UCHAR  MeasurementPilotTransmissionInformation : 1;    //B27
    UCHAR  NeighborReportTSFOffset : 1;                    //B28
    UCHAR  RCPIMeasurement : 1;                            //B29
    UCHAR  RSNIMeasurement : 1;                            //B30
    UCHAR  BSSAverageAccessDelay : 1;                      //B31
    UCHAR  BSSAvailableAdmissionCapacity : 1;              //B32
    UCHAR  Antenna : 1;                                    //B33
    UCHAR  Reserved : 6;                                   //B34-B39
} RM_IE_INFO, *PRM_IE_INFO;

//
// This structure is used for Mobility Domain IEs
//

#define MD_IE_INFO_LENGTH 3 // MD_IE_INFO has fixed length of 3 octets

typedef struct _MD_IE_INFO
{
    USHORT MDID;
    union {
        struct {
            UCHAR FastBSSTransitionOverDS : 1;              //B0
            UCHAR ResourceRequestProtocolCapability : 1;    //B1
            UCHAR Reserved : 6;                             //B2-7
        } FTCapabilityAndPolicy;
    };
} MD_IE_INFO, *PMD_IE_INFO;

NDIS_STATUS
Dot11GetIEsSizeFromIEBlob(
    _In_reads_bytes_(BufferLength)  PUCHAR   pIEBlobBuffer,
    _In_  ULONG                   BufferLength,
    _Out_ PULONG                  pInfoElemBlobSize
    );


NDIS_STATUS
Dot11ValidateIEBlob(
    _In_reads_bytes_(uSizeOfBlob) PUCHAR pucInfoBlob,
    _In_ ULONG uSizeOfBlob
    );


_Success_(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS
Dot11GetInfoElementFromIEBlob(
    _In_reads_bytes_(uSizeOfBlob) PUCHAR pucInfoBlob,
    _In_ ULONG uSizeOfBlob,
    _In_ UCHAR ucInfoId,
    _Out_ PUCHAR pucLength,
    _Outptr_result_bytebuffer_(*pucLength) PVOID * ppvInfoEle
    );


_Success_(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS
Dot11CopySSIDFromIEBlob(
    _In_reads_bytes_(uSizeOfBlob) PUCHAR pucInfoBlob,
    _In_ ULONG uSizeOfBlob,
    _Out_ PDOT11_SSID pDot11SSID
    );
    

_Success_(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS
Dot11CopyInfoElementFromIEBlob(
    _In_reads_bytes_(uSizeOfBlob) PUCHAR pucInfoBlob,
    _In_ ULONG uSizeOfBlob,
    _In_ UCHAR ucInfoId,
    _Out_ PUCHAR pucLength,
    _In_ ULONG uBufSize,
    _Out_ PVOID pvInfoEle
    );


BOOLEAN
Dot11IsHiddenSSID(
    _In_reads_bytes_(SSIDLength) PUCHAR               SSID,
    _In_ ULONG                SSIDLength
    );

UINT32
ConvertOperatingClassToBand(
    _In_ UCHAR OperatingClass);

NDIS_STATUS
ConvertP2PChannelToBandChannel(
    _In_ CAdapterPropertyCache* pAdapterPropertyCache,
    _In_ PWFD_OTA_CHANNEL pChannelInfo,
    _Out_ UINT32* pBandID,
    _Out_ UINT32* pChannelNumber );


NDIS_STATUS
Dot11ParseRSNIE(
    _In_reads_bytes_(RSNIELength) PUCHAR RSNIEData,
    _In_ ULONG OUIPrefix,
    _In_ UCHAR RSNIELength,
    _Out_ PRSN_IE_INFO RSNIEInfo );

NDIS_STATUS
Dot11ParseRMIE(
    _In_reads_bytes_(RMIELength) PUCHAR RMIEData,
    _In_ UCHAR RMIELength,
    _Out_ PRM_IE_INFO RMIEInfo);

NDIS_STATUS
Dot11ParseMDIE(
    _In_reads_bytes_(MDIELength) PUCHAR MDIEData,
    _In_ UCHAR MDIELength,
    _Out_ PMD_IE_INFO MDIEInfo);

DOT11_AUTH_ALGORITHM
Dot11GetAKMSuiteFromRSNIEInfo(
    _In_ PRSN_IE_INFO RSNIEInfo,
    _In_ USHORT index );

ULONG
Dot11GetFTAKMSuite(
    _In_ PRSN_IE_INFO RSNIEInfo,
    _In_ USHORT index );

NDIS_STATUS
Dot11GetDeviceAddressFromBSSEntry(
    _In_    WDI_BSS_ENTRY_CONTAINER *pParsedEntry,
    _Inout_ UINT16 *pBeaconBufferLength,
    _Inout_ UINT16 *pProbeBufferLength,
    _Inout_ DOT11_MAC_ADDRESS *pWfdDeviceAddress,
    _Inout_ bool *pbIsWFDDevice
    );

UINT32 MapWMMInfoToAcessClass(UCHAR wmmINfo);

