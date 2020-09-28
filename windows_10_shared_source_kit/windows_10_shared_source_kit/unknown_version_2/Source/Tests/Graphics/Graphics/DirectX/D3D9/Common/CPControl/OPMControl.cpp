/*
 *  file        :   OPMControl.cpp
 *  description :   OPM control implementOPM_RANDOM_NUMBERation
 *  history     :	6/2/2005 created
 *
 */

#include <windows.h>
#include <strsafe.h>
#include <assert.h>

#ifdef _DEBUGFORKEY
#pragma warning(disable : 4309)
#include "testcertOPM.h"
#endif
#include <OPMControl.h>

/*****************************************************
 *  class implementation:   OPMControl
 *****************************************************/

OPMControl::OPMControl
(
    IDirect3DDevice9*        pParent,
    OPM_VIDEO_OUTPUT_SEMANTICS vos,
    HRESULT*        phr,
    BOOL fClone,
    UINT MonitorIndex     
):  m_pParent(0),
    m_uStatusSeqIdx(0),
    m_uCommandSeqIdx(0),
    m_pbGHCert(NULL),
    m_cbGHCertLen(0),
    m_pOPMDevice(NULL),
    m_vos(vos)
{

    OutputDebugString(TEXT("COPPControl::COPPControl \n"));
    do
    {
        if ((0 == pParent) || (0 == phr))
            break;

        if (FAILED(*phr))
            break;

        *phr = pParent->QueryInterface(IID_IDirect3DDevice9, (void**)&m_pParent);
        if (FAILED(*phr))
            break;
        IOPMVideoOutput **ppOPMDevice=NULL;
        ULONG ulNumVideoOutputs = 0;
        *phr = CreateOPMFromDirect3D(m_pParent, vos, &ulNumVideoOutputs, (IOPMVideoOutput ***)&ppOPMDevice);
        if(FAILED(*phr))
            break;	
        if(NULL == ppOPMDevice){
            *phr = E_OUTOFMEMORY;
            break;
        }
        
        if(fClone)
        { //more than one OPM for d3ddevice only when the monitor is in cloned mode.
            ppOPMDevice[MonitorIndex]->AddRef();
            m_pOPMDevice = (IOPMVideoOutput*)ppOPMDevice[MonitorIndex];
        }
        else
        {   //In Any other scenario(including Extended mode ) two monitors 
            //would correspond to a different d3ddevices
            ppOPMDevice[0]->AddRef();
            m_pOPMDevice = (IOPMVideoOutput*)ppOPMDevice[0];
        }
        
        
        for(DWORD i=0;i<ulNumVideoOutputs;i++)
            ppOPMDevice[i]->Release();

        CoTaskMemFree(ppOPMDevice);
        
        if (0 == m_pOPMDevice)
        {
            *phr = E_OUTOFMEMORY;
        }
    }   while (FALSE);


}


OPMControl::OPMControl
(
    HWND hWnd,
    OPM_VIDEO_OUTPUT_SEMANTICS vos,
    HRESULT*        phr,
    BOOL fClone,
    UINT MonitorIndex     
):  m_pParent(0),
    m_uStatusSeqIdx(0),
    m_uCommandSeqIdx(0),
    m_pbGHCert(NULL),
    m_cbGHCertLen(0),
    m_pOPMDevice(NULL),
    m_vos(vos),
    m_hMonitor(NULL)
{
    OutputDebugString(TEXT("COPPControl::COPPControl \n"));
    do
    {
        if (!hWnd)
            break;

        if (FAILED(*phr))
            break;

        m_hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);

        if(!m_hMonitor)
        {
            assert(0);
            OutputDebugString(TEXT("COPPControl::COPPControl - Failed to get Monitor from Window"));
            *phr = E_FAIL;
            break;
        }
 
        IOPMVideoOutput **ppOPMDevice=NULL;
        ULONG ulNumVideoOutputs = 0;
        *phr = OPMGetVideoOutputsFromHMONITOR(m_hMonitor, vos, &ulNumVideoOutputs, (IOPMVideoOutput ***)&ppOPMDevice);
        if(FAILED(*phr))
            break;	
        if(NULL == ppOPMDevice){
            *phr = E_OUTOFMEMORY;
            break;
        }
        
        if(fClone)
        { //more than one OPM for d3ddevice only when the monitor is in cloned mode.
            ppOPMDevice[MonitorIndex]->AddRef();
            m_pOPMDevice = (IOPMVideoOutput*)ppOPMDevice[MonitorIndex];
        }
        else
        {   //In Any other scenario(including Extended mode ) two monitors 
            //would correspond to a different d3ddevices
            ppOPMDevice[0]->AddRef();
            m_pOPMDevice = (IOPMVideoOutput*)ppOPMDevice[0];
        }
        
        
        for(DWORD i=0;i<ulNumVideoOutputs;i++)
            ppOPMDevice[i]->Release();

        CoTaskMemFree(ppOPMDevice);
        
        if (0 == m_pOPMDevice)
        {
            *phr = E_OUTOFMEMORY;
        }
    }   while (FALSE);


}


OPMControl::~OPMControl
()
{
    OutputDebugString(TEXT("OPMControl::~OPMControl \n"));
    
    //release certificate memory
    if (0 != m_pbGHCert)
        CoTaskMemFree(m_pbGHCert);

    //Release IDirect3DDevice9
    if (NULL != m_pParent)
    {
        m_pParent->Release();
        m_pParent = NULL;
    }
    
    //Release OPM device
    if (NULL != m_pOPMDevice)
    {
        m_pOPMDevice->Release();
        m_pOPMDevice = NULL;
    }
}   //  OPMControl dtor


HRESULT
OPMControl::Initialize
(BOOL bTryWithRSAEnc,BOOL bRevoked)
{
    HRESULT         hr = S_OK;
    OPM_RANDOM_NUMBER	Random;
    OPM_ENCRYPTED_INITIALIZATION_PARAMETERS sig = {0};

    assert(0 != m_pOPMDevice);

    if (0 != m_pbGHCert)
        CoTaskMemFree(m_pbGHCert);

    do
    {
        if (0 == m_pOPMDevice)
        {
            hr = E_UNEXPECTED;
            break;
        }

        //  key exchange
        if (FAILED(hr = m_pOPMDevice->StartInitialization(&Random, &m_pbGHCert, &m_cbGHCertLen )))
            break;
        
#ifdef _DEBUGFORKEY
        //  initialize
        if (FAILED(hr = m_AuthHelper.Initialize(TestCertOPM, sizeof TestCertOPM, (m_vos == OPM_VOS_OPM_SEMANTICS) ? TRUE : FALSE)))
            break;
#else
        //  initialize
        if (FAILED(hr = m_AuthHelper.Initialize(m_pbGHCert, m_cbGHCertLen, (m_vos == OPM_VOS_OPM_SEMANTICS) ? TRUE : FALSE)))
            break;
#endif

        //fail if bRevoked is TRUE. This is to simulate the code path when certificate is revoked.
        //In reality check for revocation should be done here.
        if(bRevoked)
            return E_FAIL;

        if (FAILED(hr = m_AuthHelper.GenerateRandomNumber((BYTE*)&m_uStatusSeqIdx, sizeof UINT)))
            break;

        if (FAILED(hr = m_AuthHelper.GenerateRandomNumber((BYTE*)&m_uCommandSeqIdx, sizeof UINT)))
            break;

        //  start session
        if (FAILED(hr = m_AuthHelper.PrepareSignature((BYTE*)&sig, OPM_ENCRYPTED_INITIALIZATION_PARAMETERS_SIZE * sizeof BYTE,
                                            (BYTE *)&Random, m_uStatusSeqIdx, m_uCommandSeqIdx, (!bTryWithRSAEnc)&&(m_vos == OPM_VOS_OPM_SEMANTICS) ? TRUE : FALSE)))
            break;

        hr = m_pOPMDevice->FinishInitialization(&sig);
    
    }   while (FALSE);

    return hr;
}   //  OPMControl::Initialize

HRESULT
OPMControl::QueryConnectorType
(
    LONG* pType, 
    ULONG*              pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT						hr = S_OK;
    GUID						reqID = OPM_GET_CONNECTOR_TYPE;
    OPM_STANDARD_INFORMATION	Tmp = {0};
    ULONG						cbLen = sizeof OPM_STANDARD_INFORMATION;

    hr =  QueryStatus(&reqID, NULL, 0, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        *pType = (LONG)Tmp.ulInformation;
        *pulStatus = Tmp.ulStatusFlags;
    }

    return hr;
}   //  OPMControl::QueryConnectorType

HRESULT
OPMControl::QueryProtectionType
(
    ULONG*              pType, 
    ULONG*              pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT						hr = S_OK;
    GUID						reqID = OPM_GET_SUPPORTED_PROTECTION_TYPES;
    OPM_STANDARD_INFORMATION	Tmp = {0};
    ULONG						cbLen = sizeof OPM_STANDARD_INFORMATION;

    hr = QueryStatus(&reqID, NULL, 0, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        *pType = Tmp.ulInformation;
        *pulStatus = Tmp.ulStatusFlags;
    }

    return hr;
}   //  OPMControl::QueryProtectionType

HRESULT
OPMControl::QueryLocalProtectionLevel
(   
    const ULONG         ProtType, 
    ULONG*              pulLevel, 
    ULONG*              pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT             hr = S_OK;
    GUID                reqID = OPM_GET_VIRTUAL_PROTECTION_LEVEL;
    OPM_STANDARD_INFORMATION Tmp = {0};
    ULONG               cbLen = sizeof OPM_STANDARD_INFORMATION;

    hr = QueryStatus(&reqID, (BYTE*)&ProtType, sizeof ULONG, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        *pulLevel = Tmp.ulInformation;
        *pulStatus = Tmp.ulStatusFlags;
    }
    return hr;
}   //  OPMControl::QueryLocalProtectionLevel

HRESULT
OPMControl::QueryGlobalProtectionLevel
(
    const ULONG         ProtType, 
    ULONG*              pulLevel, 
    ULONG*              pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT             hr = S_OK;
    GUID                reqID = OPM_GET_ACTUAL_PROTECTION_LEVEL;
    OPM_STANDARD_INFORMATION Tmp = {0};
    ULONG               cbLen = sizeof OPM_STANDARD_INFORMATION;

    hr = QueryStatus(&reqID, (BYTE*)&ProtType, sizeof ULONG, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        *pulLevel = Tmp.ulInformation;
        *pulStatus = Tmp.ulStatusFlags;
    }
    return hr;
}   //  OPMControl::QueryGlobalProtectionLevel


HRESULT
OPMControl::QueryDisplayData
(
    OPM_DISPLAYDATA*   pDisplayData,
    ULONG*              pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT             hr = S_OK;
    GUID                reqID = OPM_GET_ACTUAL_OUTPUT_FORMAT;
    OPM_ACTUAL_OUTPUT_FORMAT Tmp = {0};
    ULONG               cbLen = sizeof OPM_ACTUAL_OUTPUT_FORMAT;
    ULONG               ulOffset = cbLen - sizeof OPM_DISPLAYDATA;

    hr = QueryStatus(&reqID, NULL, 0, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        assert(NULL != pDisplayData);
        assert(NULL != pulStatus);

        CopyMemory(pDisplayData, ((BYTE*)&Tmp) + ulOffset, sizeof OPM_DISPLAYDATA);
        *pulStatus = Tmp.ulStatusFlags;
    }

    return hr;
}   //  OPMControl::Query

HRESULT
OPMControl::QuerySignalData
(
    OPM_SIGNALDATA*                pSignalData,
    ULONG*                          pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT                         hr = S_OK;
    GUID                            reqID = OPM_GET_ACP_AND_CGMSA_SIGNALING;
    OPM_ACP_AND_CGMSA_SIGNALING		Tmp = {0};
    ULONG                           cbLen = sizeof OPM_ACP_AND_CGMSA_SIGNALING ;
    ULONG                           ulOffset = cbLen - sizeof OPM_SIGNALDATA;

    hr = QueryStatus(&reqID, NULL, 0, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        assert(NULL != pSignalData);
        assert(NULL != pulStatus);

        CopyMemory(pSignalData, ((BYTE*)&Tmp)+ulOffset, sizeof OPM_SIGNALDATA);
        *pulStatus = Tmp.ulStatusFlags;
    }

    return hr;
}   //  OPMControl::QuerySignalData

HRESULT
OPMControl::QueryHDCPKeyData
(
    OPM_HDCPKEYDATA*           pHDCPKeyData,
    ULONG*                      pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT                     hr = S_OK;
    GUID						reqID = OPM_GET_CONNECTED_HDCP_DEVICE_INFORMATION;
    OPM_CONNECTED_HDCP_DEVICE_INFORMATION  Tmp = {0};
    ULONG                       cbLen = sizeof OPM_CONNECTED_HDCP_DEVICE_INFORMATION;
    ULONG                       cbOffset = cbLen - sizeof OPM_HDCPKEYDATA;

    hr = QueryStatus(&reqID, NULL, 0, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        assert(NULL != pHDCPKeyData);
        assert(NULL != pulStatus);

        CopyMemory(pHDCPKeyData, ((BYTE*)&Tmp) + cbOffset, sizeof OPM_HDCPKEYDATA);
        *pulStatus = Tmp.ulStatusFlags;
    }

    return hr;
}   //  OPMControl::QueryHDCPKeyData

HRESULT
OPMControl::QueryBusData
(
    ULONG*              pulBusType,
    ULONG*              pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT             hr = S_OK;
    GUID                reqID = OPM_GET_ADAPTER_BUS_TYPE;
    OPM_STANDARD_INFORMATION Tmp = {0};
    ULONG               cbLen = sizeof OPM_STANDARD_INFORMATION;

    hr = QueryStatus(&reqID, NULL, 0, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        assert(NULL != pulBusType);
        assert(NULL != pulStatus);

        *pulBusType = Tmp.ulInformation;
        *pulStatus = Tmp.ulStatusFlags;
    }

    return hr;
}   //  OPMControl::QueryBusData

HRESULT
OPMControl::QueryOutputId
(
    UINT64*              pulOutputID,
    ULONG*               pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT             hr = S_OK;
    GUID                reqID = OPM_GET_OUTPUT_ID;
    OPM_OUTPUT_ID_DATA  Tmp = {0};
    ULONG               cbLen = sizeof OPM_OUTPUT_ID_DATA;

    hr = QueryStatus(&reqID, NULL, 0, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        assert(NULL != pulOutputID);
        assert(NULL != pulStatus);

        *pulOutputID = Tmp.OutputId;
        *pulStatus = Tmp.ulStatusFlags;
    }

    return hr;
}   //  OPMControl::QueryOutputId

HRESULT
OPMControl::QueryDVICharacteristics
(
    ULONG*              pulDVIChar,
    ULONG*              pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT             hr = S_OK;
    GUID                reqID = OPM_GET_DVI_CHARACTERISTICS;
    OPM_STANDARD_INFORMATION Tmp = {0};
    ULONG               cbLen = sizeof OPM_STANDARD_INFORMATION;

    hr = QueryStatus(&reqID, NULL, 0, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        assert(NULL != pulDVIChar);
        assert(NULL != pulStatus);

        *pulDVIChar = Tmp.ulInformation;
        *pulStatus = Tmp.ulStatusFlags;
    }

    return hr;
}   //  OPMControl::QueryDVICharacteristics

HRESULT
OPMControl::SetProtectionHDCP
(
    OPM_HDCP_PROTECTION_LEVEL Level
)
{
    GUID                                cmdID = OPM_SET_PROTECTION_LEVEL;
    OPM_SET_PROTECTION_LEVEL_PARAMETERS  cmd = {OPM_PROTECTION_TYPE_HDCP, Level};
    
    return Command(&cmdID, (BYTE*)&cmd, sizeof OPM_SET_PROTECTION_LEVEL_PARAMETERS);
}   //  OPMControl::SetProtectionHDCP


HRESULT
OPMControl::SetProtectionCOPPCompatibleHDCP
(
    OPM_HDCP_PROTECTION_LEVEL Level
)
{
    GUID                                cmdID = OPM_SET_PROTECTION_LEVEL;
    OPM_SET_PROTECTION_LEVEL_PARAMETERS  cmd = {OPM_PROTECTION_TYPE_COPP_COMPATIBLE_HDCP, Level};
    
    return Command(&cmdID, (BYTE*)&cmd, sizeof OPM_SET_PROTECTION_LEVEL_PARAMETERS);
}   //  OPMControl::SetProtectionHDCP


HRESULT
OPMControl::SetProtectionCSSDVDHDCP
(
    OPM_HDCP_PROTECTION_LEVEL Level
)
{
    GUID                                cmdID = OPM_SET_PROTECTION_LEVEL_ACCORDING_TO_CSS_DVD;
    OPM_SET_PROTECTION_LEVEL_PARAMETERS  cmd = {OPM_PROTECTION_TYPE_HDCP, Level};
    
    return Command(&cmdID, (BYTE*)&cmd, sizeof OPM_SET_PROTECTION_LEVEL_PARAMETERS);
}   //  OPMControl::SetProtectionHDCP

HRESULT
OPMControl::SetProtectionCGMSA
(
    ULONG Level
)
{
    GUID                                cmdID = OPM_SET_PROTECTION_LEVEL;
    OPM_SET_PROTECTION_LEVEL_PARAMETERS  cmd = {OPM_PROTECTION_TYPE_CGMSA, Level};

    return Command(&cmdID, (BYTE*)&cmd, sizeof OPM_SET_PROTECTION_LEVEL_PARAMETERS);
}   //  OPMControl::SetProtectionCGMSA

HRESULT
OPMControl::SetProtectionACP
(
    OPM_ACP_PROTECTION_LEVEL Level
)
{
    GUID                                cmdID = OPM_SET_PROTECTION_LEVEL;
    OPM_SET_PROTECTION_LEVEL_PARAMETERS  cmd = {OPM_PROTECTION_TYPE_ACP, Level};

    return Command(&cmdID, (BYTE*)&cmd, sizeof OPM_SET_PROTECTION_LEVEL_PARAMETERS);
}   //  OPMControl::SetProtectionACP

HRESULT
OPMControl::SetSignalData
(
    OPM_SETSIGNALDATA*                pSignalData
)
{
    GUID                            cmdID = OPM_SET_ACP_AND_CGMSA_SIGNALING;
    OPM_SET_ACP_AND_CGMSA_SIGNALING_PARAMETERS    SignalData = {0};

    assert(NULL != pSignalData);
    CopyMemory(((BYTE*)&SignalData), pSignalData, sizeof OPM_SETSIGNALDATA);
    
    return Command(&cmdID, (BYTE*)&SignalData, sizeof OPM_SET_ACP_AND_CGMSA_SIGNALING_PARAMETERS);
}   //  OPMControl::SetSignaling




HRESULT
OPMControl::SetHDCPSRMParameters
(
    OPM_SET_HDCP_SRM_PARAMETERS*  pSRMParameters,
    ULONG ulAdditionalParametersSize, 
    const BYTE* pbAdditionalParameters
)
{
    GUID                            cmdID = OPM_SET_HDCP_SRM;
    OPM_SET_HDCP_SRM_PARAMETERS    SRMData = {0};

    CopyMemory(((BYTE*)&SRMData), pSRMParameters, sizeof OPM_SET_HDCP_SRM_PARAMETERS);
    
    return Command(&cmdID, (BYTE*)&SRMData, sizeof OPM_SET_HDCP_SRM_PARAMETERS, ulAdditionalParametersSize, pbAdditionalParameters);
}   //  OPMControl::SetSignaling


HRESULT
OPMControl::QueryHDCPSRMParameters
(
    ULONG*	pSRMParameters,
    ULONG*	pulStatus,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT             hr = S_OK;
    GUID                reqID = OPM_GET_CURRENT_HDCP_SRM_VERSION;
    OPM_STANDARD_INFORMATION Tmp = {0};
    ULONG               cbLen = sizeof OPM_STANDARD_INFORMATION;

    hr = QueryStatus(&reqID, NULL, 0, (BYTE*)&Tmp, &cbLen, vos);
    if (hr == S_OK)
    {
        assert(NULL != pSRMParameters);
        assert(NULL != pulStatus);

        *pSRMParameters = Tmp.ulInformation;
        *pulStatus = Tmp.ulStatusFlags;
    }
    return hr;

}   //  OPMControl::SetSignaling


HRESULT 
OPMControl::QueryStatus
(
    GUID*       pReqID, 
    BYTE*       pbDataIn, 
    const ULONG cbDataIn,
    BYTE*       pbDataOut, 
    ULONG*      pcbDataOut,
    OPM_VIDEO_OUTPUT_SEMANTICS vos
)
{
    HRESULT             hr = S_OK;

    //  params
    if (0 == pReqID) 
        return E_INVALIDARG;

    assert(0 != m_pOPMDevice);

    do
    {
        if(OPM_VOS_OPM_SEMANTICS == vos)
        {

            OPM_GET_INFO_PARAMETERS   In = {0};
            OPM_REQUESTED_INFORMATION  Out = {0};


            if (FAILED(hr = m_AuthHelper.GenerateRandomNumber((BYTE*)&(In.rnRandomNumber), OPM_128_BIT_RANDOM_NUMBER_SIZE)))
                break;

            In.guidInformation = *pReqID;
            In.ulSequenceNumber = m_uStatusSeqIdx;
            In.cbParametersSize = cbDataIn;
            if (cbDataIn > 0)
                CopyMemory(In.abParameters, pbDataIn, cbDataIn);

            //  sign status struct
            if (FAILED(hr = m_AuthHelper.Sign((BYTE*)&In + OPM_OMAC_SIZE * sizeof BYTE, sizeof OPM_GET_INFO_PARAMETERS - OPM_OMAC_SIZE * sizeof BYTE,
                (BYTE*)&In.omac, OPM_OMAC_SIZE * sizeof BYTE))) break;

            //  status req call
            if (FAILED(hr = m_pOPMDevice->GetInformation(&In, &Out)))
                break;

            //  verify signature
            if (FAILED(hr = m_AuthHelper.VerifySignature((BYTE*)&Out + OPM_OMAC_SIZE * sizeof BYTE, 
                sizeof OPM_REQUESTED_INFORMATION - OPM_OMAC_SIZE * sizeof BYTE,
                (BYTE*)&(Out.omac), OPM_OMAC_SIZE * sizeof BYTE)))
                break;

            //  update seq number
            m_uStatusSeqIdx++;

            //  pass on output
            *pcbDataOut = min(*pcbDataOut, Out.cbRequestedInformationSize);
            if (0 != *pcbDataOut)
                CopyMemory(pbDataOut, Out.abRequestedInformation, *pcbDataOut);

        }
        else{
            OPM_COPP_COMPATIBLE_GET_INFO_PARAMETERS   In = {0};
            OPM_REQUESTED_INFORMATION  Out = {0};

            if (FAILED(hr = m_AuthHelper.GenerateRandomNumber((BYTE*)&(In.rnRandomNumber), OPM_128_BIT_RANDOM_NUMBER_SIZE * sizeof BYTE)))
                break;

            In.guidInformation = *pReqID;
            In.ulSequenceNumber = m_uStatusSeqIdx;
            In.cbParametersSize = cbDataIn;
            if (cbDataIn > 0)
                CopyMemory(In.abParameters, pbDataIn, cbDataIn);


            //  status req call
            if (FAILED(hr = m_pOPMDevice->COPPCompatibleGetInformation(&In, &Out)))
                break;

            //  verify signature
            if (FAILED(hr = m_AuthHelper.VerifySignature((BYTE*)&Out + OPM_OMAC_SIZE * sizeof BYTE, 
                sizeof OPM_REQUESTED_INFORMATION - OPM_OMAC_SIZE * sizeof BYTE,
                (BYTE*)&(Out.omac), OPM_OMAC_SIZE * sizeof BYTE)))
                break;

            //  update seq number
            m_uStatusSeqIdx++;

            //  pass on output
            *pcbDataOut = min(*pcbDataOut, Out.cbRequestedInformationSize);
            if (0 != *pcbDataOut)
                CopyMemory(pbDataOut, Out.abRequestedInformation, *pcbDataOut);

        }
    }   while (FALSE);

    return hr;
}   //  OPMControl::QueryStatus


HRESULT 
OPMControl::Command
(
    GUID*           pCommandID, 
    BYTE*           pbData, 
    const ULONG     cbData,
    ULONG ulAdditionalParametersSize,
    const BYTE* pbAdditionalParameters
)
{
    OPM_CONFIGURE_PARAMETERS   Cmd = {0};
    HRESULT         hr = S_OK;
    assert(0 != m_pOPMDevice);

    do
    {
        if ((0 == pbData) || (cbData == 0)|| (cbData > sizeof Cmd.abParameters))
        {
            hr = E_INVALIDARG;
            break;
        }

        //  fill in data
        Cmd.guidSetting = *pCommandID;
        Cmd.ulSequenceNumber = m_uCommandSeqIdx;
        Cmd.cbParametersSize = cbData;
        CopyMemory(&Cmd.abParameters[0], pbData, cbData);

        //  sign command struct
        if (FAILED(hr = m_AuthHelper.Sign((BYTE*)&Cmd + OPM_OMAC_SIZE * sizeof BYTE, sizeof OPM_CONFIGURE_PARAMETERS - OPM_OMAC_SIZE * sizeof BYTE,
                                        (BYTE*)&Cmd.omac, OPM_OMAC_SIZE * sizeof BYTE)))
            break;

        //  proper call
        if (FAILED(hr = m_pOPMDevice->Configure(&Cmd, ulAdditionalParametersSize, pbAdditionalParameters)))
            break;

        //  update seq number
        m_uCommandSeqIdx++;
        
    }   while (FALSE);

    return hr;
}   //  OPMControl::Command

_OPM_VIDEO_OUTPUT_SEMANTICS 
OPMControl::GetInitializationSemantics()
{
    return m_vos;
}

HRESULT 
CreateOPMFromDirect3D
(
    IDirect3DDevice9* pDirect3DDevice9,
    OPM_VIDEO_OUTPUT_SEMANTICS vos,
    ULONG *pulNumVideoOutputs,
    IOPMVideoOutput*** pppOPMVideoOutput
)
{
    const WCHAR szDXVA2[] = L"DXVA2.DLL";
    const CHAR szCreateOPMFromDirect3D[] = "OPMGetVideoOutputsFromIDirect3DDevice9Object";

    typedef HRESULT (APIENTRY *LPOPMDIRECT3D)
    (
        IDirect3DDevice9* pDirect3DDevice9,                                                
        OPM_VIDEO_OUTPUT_SEMANTICS vos,                                                    
        ULONG* pulNumVideoOutputs,                                                        
        IOPMVideoOutput*** pppOPMVideoOutputArray       
    );

    static HINSTANCE hlibDXVA2 = NULL;
    static LPOPMDIRECT3D lpfnCreateOPMFromDirect3D = NULL;

    if (lpfnCreateOPMFromDirect3D == NULL) 
    {
        if (hlibDXVA2 == NULL)
        {
            // BUGBUG [SEdmison]:  This library reference is leaked, but I'm
            // not sure what the ramifications would be if I were to add a
            // FreeLibrary call within this function, so I'm leaving it alone
            // for the time being.
            hlibDXVA2 = LoadLibraryExW(szDXVA2, NULL, 0);
            if (hlibDXVA2 == NULL)
            {
                DWORD LoadLibraryError = GetLastError();
                return HRESULT_FROM_WIN32(LoadLibraryError);
            }
        }

        lpfnCreateOPMFromDirect3D = (LPOPMDIRECT3D)GetProcAddress(hlibDXVA2, szCreateOPMFromDirect3D);
        if (lpfnCreateOPMFromDirect3D == NULL) 
        {
            DWORD GetProcAddressError = GetLastError();
            return HRESULT_FROM_WIN32(GetProcAddressError);
        }
    }


    return (lpfnCreateOPMFromDirect3D)(pDirect3DDevice9,vos, pulNumVideoOutputs, pppOPMVideoOutput);
}
