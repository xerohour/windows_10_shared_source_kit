/*
 *  file        :   OPMControl.h
 *  description :   OPM control types declarations
 *  history     :	6/2/2005 created
 *
 */
#pragma once

#include <windows.h>
#include <wincrypt.h>

#include <d3d9.h>
#include <dxva9typ.h>
#include <ddraw.h>
#include <opmapi.h>
#include "AuthHelper.h"
#include "OPMUtil.h"

const DWORD OPM_MESSAGE_LENGTH = 40;
const DWORD MAX_MON = 8;

typedef struct _tagLevelEntry
{
    TCHAR*       LevelTag;
    ULONG       Level;
}   LEVEL_ENTRY;

typedef struct _tagSignalingEntry
{
    TCHAR*       StandardTag;
    ULONG       Standard;
}   SIGNAL_ENTRY;

static  LEVEL_ENTRY     HDCPLevels[] = 
        {
            {TEXT("HDCP_OFF"), OPM_HDCP_OFF},
            {TEXT("HDCP_ON"), OPM_HDCP_ON},
        };

static  LEVEL_ENTRY     CGMSALevels[] =
        {
            {TEXT("CGMSA_OFF"), OPM_CGMSA_OFF},
            {TEXT("CGMSA_COPY_FREELY"), OPM_CGMSA_COPY_FREELY},
            {TEXT("CGMSA_COPY_NO_MORE"), OPM_CGMSA_COPY_NO_MORE},
            {TEXT("CGMSA_COPY_ONE_GENERATION"), OPM_CGMSA_COPY_ONE_GENERATION},
            {TEXT("CGMSA_COPY_NEVER"), OPM_CGMSA_COPY_NEVER},
            {TEXT("RED_CTL and CGMSA_OFF"), OPM_CGMSA_REDISTRIBUTION_CONTROL_REQUIRED | OPM_CGMSA_OFF},
            {TEXT("RED_CTL and CGMSA_COPY_FREELY"), OPM_CGMSA_REDISTRIBUTION_CONTROL_REQUIRED | OPM_CGMSA_COPY_FREELY},
            {TEXT("RED_CTL and CGMSA_COPY_NO_MORE"), OPM_CGMSA_REDISTRIBUTION_CONTROL_REQUIRED | OPM_CGMSA_COPY_NO_MORE},
            {TEXT("RED_CTL and CGMSA_COPY_ONE_GENERATION"), OPM_CGMSA_REDISTRIBUTION_CONTROL_REQUIRED | OPM_CGMSA_COPY_ONE_GENERATION},
            {TEXT("RED_CTL and CGMSA_COPY_NEVER"), OPM_CGMSA_REDISTRIBUTION_CONTROL_REQUIRED | OPM_CGMSA_COPY_NEVER},
        };

static  LEVEL_ENTRY     ACPLevels[] =
        {
            {TEXT("ACP_OFF"), OPM_ACP_OFF},
            {TEXT("ACP_LEVEL_ONE"), OPM_ACP_LEVEL_ONE},
            {TEXT("ACP_LEVEL_TWO"), OPM_ACP_LEVEL_TWO},
            {TEXT("ACP_LEVEL_THREE"), OPM_ACP_LEVEL_THREE},
        };

static  SIGNAL_ENTRY     SignalStandards[] =
        {
            {TEXT("NONE"), OPM_PROTECTION_STANDARD_NONE},
            {TEXT("IEC61880_525I"), OPM_PROTECTION_STANDARD_IEC61880_525I},
            {TEXT("IEC61880_2_525I"), OPM_PROTECTION_STANDARD_IEC61880_2_525I},
            {TEXT("IEC62375_625P"), OPM_PROTECTION_STANDARD_IEC62375_625P},
            {TEXT("EIA608B_525"), OPM_PROTECTION_STANDARD_EIA608B_525},
            {TEXT("EN300294_625I"), OPM_PROTECTION_STANDARD_EN300294_625I},
            {TEXT("CEA805A_TYPEA_525P"), OPM_PROTECTION_STANDARD_CEA805A_TYPEA_525P},
            {TEXT("CEA805A_TYPEA_750P"), OPM_PROTECTION_STANDARD_CEA805A_TYPEA_750P},
            {TEXT("CEA805A_TYPEA_1125I"), OPM_PROTECTION_STANDARD_CEA805A_TYPEA_1125I},
            {TEXT("CEA805A_TYPEB_525P"), OPM_PROTECTION_STANDARD_CEA805A_TYPEB_525P},
            {TEXT("CEA805A_TYPEB_750P"), OPM_PROTECTION_STANDARD_CEA805A_TYPEB_750P},
            {TEXT("CEA805A_TYPEB_1125I"), OPM_PROTECTION_STANDARD_CEA805A_TYPEB_1125I},
            {TEXT("ARIBTRB15_525I"), OPM_PROTECTION_STANDARD_ARIBTRB15_525I},
            {TEXT("ARIBTRB15_525P"), OPM_PROTECTION_STANDARD_ARIBTRB15_525P},
            {TEXT("ARIBTRB15_750P"), OPM_PROTECTION_STANDARD_ARIBTRB15_750P},
            {TEXT("ARIBTRB15_1125I"), OPM_PROTECTION_STANDARD_ARIBTRB15_1125I},
            {TEXT("OTHER"), OPM_PROTECTION_STANDARD_OTHER},
        };
const INT NUMOFSIGNALPROT = 16;
static  SIGNAL_ENTRY     AspectRatios[] =
        {
            {TEXT("EN300294_FULL_FORMAT_4_BY_3"), OPM_ASPECT_RATIO_EN300294_FULL_FORMAT_4_BY_3},
            {TEXT("EN300294_BOX_14_BY_9_CENTER"), OPM_ASPECT_RATIO_EN300294_BOX_14_BY_9_CENTER},
            {TEXT("EN300294_BOX_14_BY_9_TOP"), OPM_ASPECT_RATIO_EN300294_BOX_14_BY_9_TOP},
            {TEXT("EN300294_BOX_16_BY_9_CENTER"), OPM_ASPECT_RATIO_EN300294_BOX_16_BY_9_CENTER},
            {TEXT("EN300294_BOX_16_BY_9_TOP"), OPM_ASPECT_RATIO_EN300294_BOX_16_BY_9_TOP},
            {TEXT("EN300294_BOX_GT_16_BY_9_CENTER"), OPM_ASPECT_RATIO_EN300294_BOX_GT_16_BY_9_CENTER},
            {TEXT("EN300294_FULL_FORMAT_4_BY_3_PROTECTED_CENTER"), OPM_ASPECT_RATIO_EN300294_FULL_FORMAT_4_BY_3_PROTECTED_CENTER},
            {TEXT("EN300294_FULL_FORMAT_16_BY_9_ANAMORPHIC"), OPM_ASPECT_RATIO_EN300294_FULL_FORMAT_16_BY_9_ANAMORPHIC},
};


typedef struct  tagOPMDisplayData
{
    ULONG DisplayWidth;
    ULONG DisplayHeight;
    DXVA2_SampleFormat dsfSampleInterleaveFormat;
    D3DFORMAT d3dFormat;
    ULONG ulFrequencyNumerator;
    ULONG ulFrequencyDenominator;
}   OPM_DISPLAYDATA;

typedef struct  tagOPMHDCPKeyData
{
    ULONG               HDCPFlags;
    GUID                BKey;
    GUID                reserved1;
    GUID                reserved2;
}   OPM_HDCPKEYDATA;

typedef struct  tagOPMSRMPARAMETERS
{
    BOOL				IsSRMSet;
    ULONG               SRMVersion;
}   OPM_SRMPARAMETERS;

typedef struct  tagOPMStatusSignalData
{
    ULONG               AvailableTVProtectionStandards;
    ULONG               ActiveTVProtectionStandard;
    ULONG               Reserved;
    ULONG               AspectRatioValidMask1;
    ULONG               AspectRatioData1;
    ULONG               AspectRatioValidMask2;
    ULONG               AspectRatioData2;
    ULONG               AspectRatioValidMask3;
    ULONG               AspectRatioData3;
    ULONG               Reserved2[4];
    ULONG               Reserved3[4];
}   OPM_SIGNALDATA;

typedef struct  tagOPMSetSignalData
{
    ULONG NewTVProtectionStandard;
    ULONG AspectRatioChangeMask1;
    ULONG AspectRatioData1;
    ULONG AspectRatioChangeMask2;
    ULONG AspectRatioData2;
    ULONG AspectRatioChangeMask3;
    ULONG AspectRatioData3;
    ULONG Reserved[ 4 ];
    ULONG Reserved2[ 4 ];
    ULONG Reserved3;
}   OPM_SETSIGNALDATA;
    

typedef struct  tagOPMConfig
{
    ULONG               ProtectionMask;
    ULONG               LocalProtectionLevel[PROTECTION_TYPES];
    ULONG               GlobalProtectionLevel[PROTECTION_TYPES];
    ULONG               StatusFlags;
    LONG				ConnectorType;
    ULONG               BusType;
    

    OPM_DISPLAYDATA    DisplayData;
    OPM_HDCPKEYDATA    HDCPKeyData;
    OPM_SIGNALDATA     SignalData;
    OPM_SRMPARAMETERS  SRMData;
    
}   OPM_CONFIG;

/*
 *  class       :   OPMControl
 *  description :   
 *  notes       :   abstract base
 */

HRESULT CreateOPMFromDirect3D(IDirect3DDevice9* pDirect3DDevice9,OPM_VIDEO_OUTPUT_SEMANTICS vos,ULONG *pulNumVideoOutputs,IOPMVideoOutput*** pppOPMVideoOutput);

class OPMControl
{
public:
    OPMControl(IDirect3DDevice9*, OPM_VIDEO_OUTPUT_SEMANTICS vos, HRESULT*, BOOL fClone = FALSE , UINT MonitorIndex = 0);
    OPMControl(HWND hWnd, OPM_VIDEO_OUTPUT_SEMANTICS vos, HRESULT*, BOOL fClone = FALSE , UINT MonitorIndex = 0);
    ~OPMControl();

    HRESULT QueryConnectorType(LONG*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS vos);
    HRESULT QueryProtectionType(ULONG*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS vos);
    HRESULT QueryLocalProtectionLevel(const ULONG, ULONG*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS vos);
    HRESULT QueryGlobalProtectionLevel(const ULONG, ULONG*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS vos);
    HRESULT QueryDisplayData(OPM_DISPLAYDATA*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS vos);
    HRESULT QuerySignalData(OPM_SIGNALDATA*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS vos);
    HRESULT QueryHDCPKeyData(OPM_HDCPKEYDATA*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS vos);
    HRESULT QueryBusData(ULONG*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS vos);
    HRESULT QueryHDCPSRMParameters(ULONG *, ULONG *,OPM_VIDEO_OUTPUT_SEMANTICS vos);
    HRESULT QueryOutputId(UINT64*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS vos);
    HRESULT QueryDVICharacteristics(ULONG*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS vos);

    HRESULT SetProtectionHDCP(OPM_HDCP_PROTECTION_LEVEL);
    HRESULT SetProtectionCOPPCompatibleHDCP(OPM_HDCP_PROTECTION_LEVEL);
    HRESULT SetProtectionCSSDVDHDCP(OPM_HDCP_PROTECTION_LEVEL);
    HRESULT SetProtectionCGMSA(ULONG);
    HRESULT SetProtectionACP(OPM_ACP_PROTECTION_LEVEL);
    HRESULT SetSignalData(OPM_SETSIGNALDATA*);
    HRESULT SetHDCPSRMParameters(OPM_SET_HDCP_SRM_PARAMETERS *, ULONG ulAdditionalParametersSize, const BYTE* pbAdditionalParameters); 
    HRESULT Initialize(BOOL bTryWithRSAEnc = FALSE, BOOL bRevoked = FALSE);

    _OPM_VIDEO_OUTPUT_SEMANTICS GetInitializationSemantics();
    BYTE* GetCertificate(){return m_pbGHCert;};
    ULONG GetCertificateSize(){return m_cbGHCertLen;};

    IOPMVideoOutput*   m_pOPMDevice;

    // Monitor
    HMONITOR        m_hMonitor;
    
protected:

    OPM_VIDEO_OUTPUT_SEMANTICS m_vos;//note that this is only used for creation

    //  context
    IDirect3DDevice9*    m_pParent;
    
    
    //  OPM parameters
    UINT            m_uStatusSeqIdx;
    UINT            m_uCommandSeqIdx;

    //  helper
    CPAuthHelper  m_AuthHelper;

    //certificate
    BYTE*           m_pbGHCert;
    ULONG           m_cbGHCertLen;


    //  methods
    HRESULT QueryStatus(GUID*, BYTE*, const ULONG, BYTE*, ULONG*, OPM_VIDEO_OUTPUT_SEMANTICS);
    HRESULT Command(GUID*, BYTE*, const ULONG, 	ULONG ulAdditionalParametersSize = 0, const BYTE *pbAdditionalParameters = NULL);

};  //  class   OPMControl


