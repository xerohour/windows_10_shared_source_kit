//--------------------------------------------------------------------------
//
//  File: Render.cpp
//
//  Copyright (C) Microsoft. All rights reserved.
//
//  Abstract:
//      This is the skeleton for the test application.
//
//  Contents:
//      PinGetIOCTL()
//      GetEndpointCompressedSupport()
//      CUnitRenderEndpoint::CUnitRenderEndpoint()
//      CUnitRenderEndpoint::~CUnitRenderEndpoint()
//      CUnitRenderEndpoint::PlaySine()
//      CUnitRenderEndpoint::Stop()
//      CRenderEndpoints::CRenderEndpoints()
//      CRenderEndpoints::~CRenderEndpoints()
//      CRenderEndpoints::PlaySine()
//      CRenderEndpoints::Stop()
//      TC_RenderToAllEndpoints()
//      TC_VerifyMuteOnCompressedEndpoint()
//
//
//--------------------------------------------------------------------------
#include "Common.h"
#include <initguid.h>
#include <MMDeviceAPIP.h>
#include <DeviceTopology.h>
#include <DeviceTopologyP.h>
#include <AudioStreaming.h>
#include <BasicLogHelper.h>
#include <MMSystem.h>
#include <DevTrans.h>
#include <HRString.h>
#include <cpropvariant.h>

#pragma warning (push, 3)
#include <ks.h>
#pragma warning (pop)
// we are using the old header
// so we need ksmedia.h
#include <ksmedia.h>
#include <devioctl.h>

#define ROUNDUP_8(x) (((x) + 7) & ~7)

using namespace DevTrans;
using namespace BasicLogEx;

DEFINE_GUID(ROOT_CONTAINERID, 0x00000000, 0x0000, 0x0000, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);

namespace DevTrans_Internal 
{


//  Since DeviceTranslation has an internal function that does precisely
//  what we need, no need to re-invent the wheel or even copy code, just
//  expose it so that it's available.
HRESULT GetEndpointPartsToHostConnector
(
    __in IMMDevice     *pIMMEndpoint,
    __out IPartsList  **ppIParts
);
        
}

using namespace DevTrans_Internal;

//--------------------------------------------------------------------------;
//
//  HRESULT PinGetIOCTL
//
//  Description:
//      Sends IOCTL to specific pin for the DeviceInterface.  Pilfered from
//      KSTopTest and modified to unclassify it.
//
//  Arguments:
//      HANDLE hFilter: Open handle for the device interface.
//
//      UINT uPin: Pin in question.
//
//      GUID PropertySetId: Property guid.
//
//      ULONG ulProperty: Property id.
//
//      LPVOID pInData: Input instance data.
//
//      UINT cbInSize: Size of above buffer.
//
//      LPVOID *ppOutData: Output buffer allocated using 'new'.  Delete
//                          locally!!
//
//      UINT &cbOutSize: Size of the above buffer.
//
//  Return (HRESULT):
//      S_OK if no error, error code otherwise.
//
//
//--------------------------------------------------------------------------;
HRESULT PinGetIOCTL
(
    HANDLE      hFilter,
    UINT        uPin,
    GUID        PropertySetId,
    ULONG       ulProperty, 
    LPVOID      pInData, 
    UINT        cbInSize,
    LPVOID     *ppOutData, 
    PUINT       pcbOutSize
)
{
    KSP_PIN    *pKsPinProp = NULL;
    LPVOID      pInBuffer  = NULL;
    LPVOID      pOutBuffer = NULL;
    ULONG       cbReturned = 0;
    UINT        cbAlloc;
    BOOL        bIOCTL;
    UINT        uError = 0;
    HRESULT     hr;
    
    // Initialize output data
    *ppOutData  = NULL;
    *pcbOutSize = 0;
    
    // Create the input data buffer to send to the DeviceIoControl
    // by appending the pInData to a KSP_PIN data structure and 
    // copying the whole thing in pInBuffer
    cbAlloc = sizeof(KSP_PIN) + cbInSize;
    if (NULL == (pInBuffer = new BYTE[cbAlloc]))
    {
        XLOG(
                    XMSG,
                    eError,
                    L"PinGetIOCTL: Insufficient memory.");

        return (E_OUTOFMEMORY);
    }

    pKsPinProp = (KSP_PIN*)pInBuffer;
    pKsPinProp->Property.Set   = PropertySetId;
    pKsPinProp->Property.Id    = ulProperty;
    pKsPinProp->Property.Flags = KSPROPERTY_TYPE_GET;
    pKsPinProp->PinId          = uPin;
    pKsPinProp->Reserved       = 0;

    if((0 != cbInSize) && (NULL != pInData))
    {   
        CopyMemory((LPBYTE)(&pKsPinProp[1]), pInData, cbInSize);
    }

    // Determine the needed size
    bIOCTL = DeviceIoControl(
                hFilter,
                IOCTL_KS_PROPERTY,
                pInBuffer,
                sizeof(KSP_PIN) + cbInSize,
                NULL,
                0,
                &cbReturned,
                NULL);

    if (!bIOCTL)
    {
        uError = GetLastError();
        
        if (ERROR_MORE_DATA != uError)
        {
            hr = HRESULT_FROM_WIN32(uError);

            XLOG(
                        XMSG,
                        eError,
                        L"PinGetIOCTL: DeviceIoControl encountered error (%s).",
                        GetHRString(hr));

            delete [] pInBuffer;    
            return (hr);
        }
    }

    if (0 == cbReturned)
    {
        XLOG(
                    XMSG,
                    eError,
                    L"PinGetIOCTL: Output buffer size is zero.");

        delete [] pInBuffer;    
        return (E_INVALIDARG);
    }
    cbAlloc = cbReturned;

    // Allocate the specified size...
    if (NULL == (pOutBuffer = new BYTE[cbAlloc]))
    {
        XLOG(
                    XMSG,
                    eError,
                    L"PinGetIOCTL: Insufficient memory.");

        delete [] pInBuffer;    
        return (E_OUTOFMEMORY);
    }

    bIOCTL = DeviceIoControl(
                hFilter,
                IOCTL_KS_PROPERTY,
                pInBuffer,
                sizeof(KSP_PIN) + cbInSize,
                pOutBuffer,
                cbAlloc,
                &cbReturned,
                NULL);

    delete [] pInBuffer;

    if (!bIOCTL)
    {
        uError = GetLastError();
        hr     = HRESULT_FROM_WIN32(uError);

        XLOG(
                    XMSG,
                    eError,
                    L"PinGetIOCTL: DeviceIoControl encountered error (%s).",
                    GetHRString(hr));

        delete [] pOutBuffer;
        return (hr);
    }

    *ppOutData  = pOutBuffer;
    *pcbOutSize = cbAlloc;
    return (S_OK);
}   //   PinGetIOCTL()


//--------------------------------------------------------------------------;
//
//  HRESULT GetEndpointCompressedSupport
//
//  Description:
//      Determines if the given endpoint supports compressed (non-PCM) formats.
//
//  Arguments:
//      IMMDevice *pIEndpoint: Endpoint in question.
//
//      bool *pbCompressed: Return buffer for boolean, true if non-PCM
//                  formats are supported.
//
//  Return (HRESULT):
//      S_OK if no error, error code otherwise.
//
//
//--------------------------------------------------------------------------;
HRESULT GetEndpointCompressedSupport
(
    IMMDevice          *pIEndpoint,
    bool               *pbCompressed
)
{
    CComPtr<IMMDevice>      pIInterface = NULL;
    CComPtr<IPnpInterface>  pIPnpInterface = NULL;
    CComHeapPtr<WCHAR>      pszDeviceInterfacePath;
    HANDLE                  hFilter = INVALID_HANDLE_VALUE;
    KSMULTIPLE_ITEM        *pKsDataRanges = NULL;
    KSDATARANGE            *pKsDataRange = NULL;
    UINT                    iKsDataRanges = 0;
    UINT                    iRange;
    UINT                    uError;
    CComHeapPtr<UINT>       spPinArray;
    UINT                    cPinArray = 0;
    bool                    bHasAudio = false;
    HRESULT                 hr;

    *pbCompressed = false;

    //  Get the communication pin for the endpoint.
    if (S_OK != (hr = EndpointToKSStreamPins(pIEndpoint, &spPinArray, &cPinArray, &pIInterface)))
    {
        XLOG(
                    XMSG,
                    eError,
                    L"GetEndpointCompressedSupport:EndpointToKSStreamPins "
                        L"returned error (%s)",
                    GetHRString(hr));

        return (hr);
    }

    //  Get the interface so we can get the DevicePath for the CreateFile...
    hr = pIInterface->QueryInterface(
                        __uuidof(IPnpInterface),
                        (void**)&pIPnpInterface);

    if (S_OK != hr)
    {
        XLOG(
                    XMSG,
                    eError,
                    L"GetEndpointCompressedSupport: QueryInterface returned "
                        L"error (%s)",
                    GetHRString(hr));

        return (hr);
    }

    if (S_OK != (hr = pIPnpInterface->GetDevicePath(&pszDeviceInterfacePath)))
    {
        XLOG(
                    XMSG,
                    eError,
                    L"GetEndpointCompressedSupport: GetDevicePath returned "
                        L"error (%s)",
                    GetHRString(hr));

        return (hr);
    }

    //  CreateFile for the IOCTL...
    hFilter = CreateFileW(
                pszDeviceInterfacePath,
                GENERIC_READ,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                0L,
                NULL);

    if (INVALID_HANDLE_VALUE == hFilter)
    {
        uError = GetLastError();
        hr     = HRESULT_FROM_WIN32(uError);

        XLOG(
                    XMSG,
                    eError,
                    L"GetEndpointCompressedSupport: CreateFile encountered "
                        L"error (%s).",
                    GetHRString(hr));

        return (hr);
    }

    for(UINT i = 0; i < cPinArray; i++)
    {
        bool thisIsAttributeList = false;
        bool nextIsAttributeList = false;

        //  Getting the KSDataRanges...
        hr = PinGetIOCTL(
                hFilter,
                spPinArray[i],
                KSPROPSETID_Pin,
                KSPROPERTY_PIN_DATARANGES,
                NULL,
                0,
                (LPVOID*)(&pKsDataRanges),
                &iKsDataRanges);


        if (S_OK != hr)
        {
            XLOG(
                        XMSG,
                        eError,
                        L"GetEndpointCompressedSupport: PinGetIOCTL to get "
                            L"DataRanges returned error (%s).",
                        GetHRString(hr));
            CloseHandle(hFilter);
            return (hr);
        }

        if (0 == pKsDataRanges->Count)
        {
            XLOG(
                        XMSG,
                        eError,
                        L"GetEndpointCompressedSupport: DataRanges count is "
                            L"unexpectedly zero.");

            delete [] pKsDataRange;
            CloseHandle(hFilter);
            return (E_INVALIDARG);
        }

        //  Get the first one... Immediately following the count structure.
        pKsDataRange = (KSDATARANGE*)(&(pKsDataRanges[1]));

        for (
            iRange = pKsDataRanges->Count;
            iRange;
            iRange--, pKsDataRange = (
                thisIsAttributeList ?
                    (KSDATARANGE*)((PBYTE)pKsDataRange + ROUNDUP_8(((PKSMULTIPLE_ITEM)pKsDataRange)->Size)) :
                    (KSDATARANGE*)((PBYTE)pKsDataRange + pKsDataRange->FormatSize)
            )
        )
        {
            thisIsAttributeList = nextIsAttributeList;
            nextIsAttributeList = false;

            // we're not interested in attribute lists
            if (thisIsAttributeList) { continue; }

            nextIsAttributeList = ((pKsDataRange->Flags & KSDATARANGE_ATTRIBUTES) == KSDATARANGE_ATTRIBUTES);
            
            if (!IsEqualGUID(pKsDataRange->MajorFormat, KSDATAFORMAT_TYPE_AUDIO))
            {
                //  We're only interested in audio formats...
                continue;
            }

            bHasAudio = true;

            if (!IsEqualGUID(pKsDataRange->SubFormat, KSDATAFORMAT_SUBTYPE_PCM))
            {
                WCHAR       szGuid[40];

                //  Log format subtype...
                StringFromGUID2(
                        pKsDataRange->SubFormat,
                        szGuid,
                        sizeof(szGuid)/sizeof(szGuid[0]));

                XLOG(
                            XMSG,
                            eInfo2,
                            L"  Note: Endpoint supports non-PCM format "
                                L"(SubFormat: %ls).",
                            szGuid);

                *pbCompressed = true;
            }
        } // for each data range

        delete [] pKsDataRanges;
    }
    
    CloseHandle(hFilter);
    return ((bHasAudio)?S_OK:E_INVALIDARG);
}   //   GetEndpointCompressedSupport()

class CUnitRenderEndpoint
{
public:
    CUnitRenderEndpoint
    (
        IMMDevice  *pEndpoint,
        IBasicLog  *pBasicLog,
        HRESULT    &hr
    );
    ~CUnitRenderEndpoint();
    operator IMMDevice *() { return ((IMMDevice*)m_pIEndpoint); }
    LPCWSTR GetName() { return (&(m_szName[0])); }
    HRESULT GetContainerId(GUID *pContainerId);
    HRESULT PlaySine();
    HRESULT Stop();

private:
    CComPtr<IMMDevice>                      m_pIEndpoint;
    CComPtr<IBasicLogEx>                    m_pILogEx;
    CComPtr<IAudioStreamingRender>          m_pRender;
    WCHAR                                   m_szName[MAX_PATH];
};

//--------------------------------------------------------------------------;
//
//  CUnitRenderEndpoint::CUnitRenderEndpoint
//
//  Description:
//      Constructor for Unit for audio render
//
//  Arguments:
//      IMMDevice *pIEndpoint: Endpoint from device enumerator.
//
//      IBasicLog *pBasicLog: Logger to report errors.
//
//      HRESULT &hr: HRESULT to bubble out.
//
//  Return (None):
//
//
//--------------------------------------------------------------------------;
CUnitRenderEndpoint::CUnitRenderEndpoint
(
    IMMDevice      *pEndpoint,
    IBasicLog      *pBasicLog,
    HRESULT         &hr
):
    m_pIEndpoint(pEndpoint),
    m_pILogEx(NULL),
    m_pRender(NULL)
{
    CComHeapPtr<WCHAR>      pszDesc;
    CComHeapPtr<WCHAR>      pszName;

    m_szName[0] = 0;

    if (FAILED(hr))
    {
        return;
    }

    if (S_OK != (hr = CreateBasicLogEx(pBasicLog, 0L, &m_pILogEx)))
    {
        pBasicLog->Log(XMSG, eError, L"CreateBasicLogEx returned error.");
        return;
    }

    if (S_OK != (hr = GetEndpointDisplayNameW(&pszName, pEndpoint)))
    {
        m_pILogEx->LogEx(
            XMSG,
            eError,
            L"GetEndpointDisplayName returned error (%s).",
            GetHRString(hr));

        return;
    }

    if (S_OK != (hr = GetEndpointDescriptionW(&pszDesc, pEndpoint)))
    {
        m_pILogEx->LogEx(
            XMSG,
            eError,
            L"GetEndpointDescription returned error (%s).",
            GetHRString(hr));

        return;
    }

    hr = StringCbPrintfW(
            m_szName,
            sizeof(m_szName),
            L"%ls (%ls)",
            (LPCWSTR)pszDesc,
            (LPCWSTR)pszName);

    if (S_OK != hr)
    {
        m_pILogEx->LogEx(
            XMSG,
            eError,
            L"StringCbPrintf returned error (%s).",
            GetHRString(hr));

        return;
    }

    hr = CreateAudioStreamingRender(
                api_WASAPI,
                pEndpoint,
                pBasicLog,
                &m_pRender);

    if (S_OK != hr)
    {
        m_pILogEx->LogEx(
            XMSG,
            eError,
            L"CreateAudioStreamingRender returned error (%s).",
            GetHRString(hr));

        return;
    }

    if (S_OK != (hr = m_pRender->Initialize(NULL)))
    {
        m_pILogEx->LogEx(
            XMSG,
            eError,
            L"IAudioStreamingRender::Initialize returned error (%s).",
            GetHRString(hr));
    }
}   //  CUnitRenderEndpoint::CUnitRenderEndpoint()


//--------------------------------------------------------------------------;
//
//  CUnitRenderEndpoint::~CUnitRenderEndpoint
//
//  Description:
//      Destructor for Unit for audio render
//
//  Arguments:
//      None.
//
//  Return (None):
//
//
//--------------------------------------------------------------------------;
CUnitRenderEndpoint::~CUnitRenderEndpoint()
{
    Stop();
}   //  CUnitRenderEndpoint::~CUnitRenderEndpoint()


HRESULT CUnitRenderEndpoint::GetContainerId(GUID *pContainerId)
{
    HRESULT hr;

    CComPtr<IPropertyStore> pPropertyStore;
    if (S_OK != (hr = m_pIEndpoint->OpenPropertyStore(STGM_READ, &pPropertyStore)))
    {
        m_pILogEx->LogEx(
            XFAIL,
            eError,
            L"IMMDevice::OpenPropertyStore returned error (%s).",
            GetHRString(hr));
        return hr;
    }
    
    CPropVariant containerId;
    if (S_OK != (hr = pPropertyStore->GetValue(PKEY_Device_ContainerId, &containerId)))
    {
        m_pILogEx->LogEx(
            XFAIL,
            eError,
            L"IPropertyStore::GetValue(PKEY_Device_ContainerId) returned error (%s).",
            GetHRString(hr));
        return hr;
    }
    
    if (VT_CLSID != containerId.vt)
    {
        m_pILogEx->LogEx(
            XFAIL,
            eError,
            L"PKEY_Device_ContainerId has unexpected vartype 0x%x instead of VT_CLSID",
            containerId.vt);
        return HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
    }
    
    *pContainerId = *(containerId.puuid);
    return S_OK;
}

//--------------------------------------------------------------------------;
//
//  HRESULT CUnitRenderEndpoint::PlaySine
//
//  Description:
//      Plays a sine tone through endpoint.
//
//  Arguments:
//      None.
//
//  Return (HRESULT):
//      S_OK if no error, error code otherwise.
//
//
//--------------------------------------------------------------------------;
HRESULT CUnitRenderEndpoint::PlaySine()
{
    if (NULL == m_pRender)
    {
        return (E_INVALIDARG);
    }

    m_pILogEx->LogEx(
        XMSG,
        eInfo1,
        L"--++** Playing sine tone through %ls...  **++--",
        GetName());

    return (m_pRender->PlaySineWaveAsync(TEST_FREQUENCY, TEST_AMPLITUDE, Method_NoDithering));
}   //   CUnitRenderEndpoint::PlaySine()


//--------------------------------------------------------------------------;
//
//  HRESULT CUnitRenderEndpoint::Stop
//
//  Description:
//      Stops any playback through endpoint.
//
//  Arguments:
//      None.
//
//  Return (HRESULT):
//      S_OK if no error, error code otherwise.
//
//
//--------------------------------------------------------------------------;
HRESULT CUnitRenderEndpoint::Stop()
{
    if (NULL == m_pRender)
    {
        return (E_INVALIDARG);
    }

    m_pILogEx->LogEx(
        XMSG,
        eInfo1,
        L"--++** Stopping streaming on %ls...  **++--",
        GetName());

    return (m_pRender->Stop());
}   //   CUnitRenderEndpoint::Stop()


class CRenderEndpoints
{
public:
    CRenderEndpoints(IBasicLog *pBasicLog, LPCWSTR szDeviceInstanceId, HRESULT &hr);
    ~CRenderEndpoints();
    HRESULT PlaySine();
    HRESULT Stop();
    size_t Count() { return m_Endpoints.GetCount(); }

private:
    CComPtr<IBasicLogEx>                    m_pILogEx;
    CAtlArray<CUnitRenderEndpoint *>        m_Endpoints;
};


//--------------------------------------------------------------------------;
//
//  CRenderEndpoints::CRenderEndpoints
//
//  Description:
//      Constructor for collection for audio render units.
//
//  Arguments:
//      IBasicLog *pBasicLog: Logger to report errors.
//
//      HRESULT &hr: HRESULT to bubble out.
//
//  Return (None):
//
//
//--------------------------------------------------------------------------;
CRenderEndpoints::CRenderEndpoints
(
    IBasicLog  *pBasicLog,
    LPCWSTR     pszSelectedDeviceInstanceId,
    HRESULT    &hr
):
    m_pILogEx(NULL)
{
    CComPtr<IMMDeviceEnumerator>        pDevEnum = NULL;
    CComPtr<IMMDeviceCollection>        pEndpoints = NULL;
    UINT                                iEndpoint;

    if (FAILED(hr))
    {
        return;
    }

    if (S_OK != (hr = CreateBasicLogEx(pBasicLog, 0L, &m_pILogEx)))
    {
        pBasicLog->Log(XMSG, eError, L"CreateBasicLogEx returned error.");
        return;
    }

    if (S_OK != (hr = pDevEnum.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
    {
        m_pILogEx->LogEx(
            XMSG,
            eError,
            L"CoCreateInstance for IMMDeviceEnumerator returned error (%s)",
            GetHRString(hr));
        return;
    }

    hr = pDevEnum->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pEndpoints);

    if (S_OK != hr)
    {
        m_pILogEx->LogEx(
            XMSG,
            eError,
            L"IMMDeviceEnumerator::EnumAudioEndpoints returned error (%s).",
            GetHRString(hr));

        return;
    }

    if (S_OK != (hr = pEndpoints->GetCount(&iEndpoint)))
    {
        m_pILogEx->LogEx(
            XMSG,
            eError,
            L"IMMDeviceCollection::GetCount returned error (%s).",
            GetHRString(hr));

        return;
    }

    if (NULL == pszSelectedDeviceInstanceId)
    {
        m_pILogEx->LogEx(XMSG, eInfo2, L"The test will include all endpoints on the system as no specific device is selected.");
    }
    else
    {
        m_pILogEx->LogEx(XMSG, eInfo2, L"The test will include all endpoints on selected device (%s).", pszSelectedDeviceInstanceId);
    }

    for (; iEndpoint; iEndpoint--)
    {
        CComPtr<IMMDevice>      pIEndpoint = NULL;
        CComPtr<IMMDevice>      pDevnode = NULL;
        CComHeapPtr<WCHAR>      pszInstanceId;
        CUnitRenderEndpoint    *pUnitRenderEndpoint = NULL;

        if (S_OK != (hr = pEndpoints->Item(iEndpoint - 1, &pIEndpoint)))
        {
            m_pILogEx->LogEx(
                XMSG,
                eError,
                L"IMMDeviceCollection::Item returned error (%s).",
                GetHRString(hr));

            return;
        }

        // get the pnp ID of the endpoint
        if (S_OK != (hr = GetEndpointDevnode(pIEndpoint, &pDevnode)))
        {
            m_pILogEx->LogEx(XMSG, eError, L"Failed GetEndpointDevnode (%s)", GetHRString(hr));
            return;
        }

        if (S_OK != (hr = GetDevnodeInstanceId(&pszInstanceId, pDevnode)))
        {
            m_pILogEx->LogEx(XMSG, eError, L"Failed to get device instance ID of the devnode of the current endpoint (%s)", GetHRString(hr));
            return;
        }
        
        pUnitRenderEndpoint = new CUnitRenderEndpoint(pIEndpoint, pBasicLog, hr);
        if (NULL == pUnitRenderEndpoint)
        {
            hr = E_OUTOFMEMORY;
            return;
        }

        if (S_OK != hr)
        {
            return;
        }

        // for device kit, skip endpoints that are not on the selected device
        if ((NULL != pszSelectedDeviceInstanceId) &&
            (CSTR_EQUAL != CompareStringEx(
                LOCALE_NAME_INVARIANT, // ignore system/user locale
                NORM_IGNORECASE, // consider a and A equivalent
                pszSelectedDeviceInstanceId, -1,
                pszInstanceId, -1,
                nullptr, nullptr, 0
            )))
        {
            m_pILogEx->LogEx(XMSG, eInfo2, L"...Endpoint (%s) does not match selected device; skipping", pUnitRenderEndpoint->GetName());
            delete pUnitRenderEndpoint;
            continue;
        }

        // if we stream to A2DP and HF on the same Bluetooth device at the same time,
        // weird device-dependent behavior kicks in
        // e.g., the A2DP profile disconnects on a Sony Ericsson MW600
        // 
        // to avoid this, we keep track of the container ID for every endpoint
        // if the container is other than "local machine",
        // and we're already streaming to another (earlier) endpoint on the same container,
        // then we suppress streaming to this endpoint
        GUID gThisEndpointContainer;
        if (S_OK != (hr = pUnitRenderEndpoint->GetContainerId(&gThisEndpointContainer)))
        {
            // GetContainerId handles its own logging
            continue;
        }
        
        if (ROOT_CONTAINERID != gThisEndpointContainer)
        {
            size_t iEndpoint2 = m_Endpoints.GetCount();
            bool keep = true;
            for (; iEndpoint2 > 0; iEndpoint2--)
            {
                CUnitRenderEndpoint *pThatEndpoint = m_Endpoints.GetAt(iEndpoint2 - 1);
                GUID gThatEndpointContainer;
                if (S_OK != (hr = pThatEndpoint->GetContainerId(&gThatEndpointContainer)))
                {
                    // GetContainerId handles its own logging
                    continue;
                }
                
                if (gThisEndpointContainer == gThatEndpointContainer)
                {
                    m_pILogEx->LogEx(
                        XMSG,
                        eInfo2,
                        L"...Endpoint (%s) has the same non-local container ID as previous endpoint (%s); skipping",
                        pUnitRenderEndpoint->GetName(),
                        pThatEndpoint->GetName());
                    keep = false;
                    break; // out of inner loop
                }
            }
            
            if (!keep)
            {
                delete pUnitRenderEndpoint;
                continue;
            }
        }
        
        m_pILogEx->LogEx(XMSG, eInfo2, L"...Endpoint (%s) selected for test", pUnitRenderEndpoint->GetName());
        m_Endpoints.Add(pUnitRenderEndpoint);
    }
}   //  CRenderEndpoints::CRenderEndpoints()


//--------------------------------------------------------------------------;
//
//  CRenderEndpoints::~CRenderEndpoints
//
//  Description:
//      Destructor for collection for audio render units.  Deletes each unit
//      individually.
//
//  Arguments:
//      None.
//
//  Return (None):
//
//
//--------------------------------------------------------------------------;
CRenderEndpoints::~CRenderEndpoints()
{
    CUnitRenderEndpoint    *pEndpoint;

    while (!m_Endpoints.IsEmpty())
    {
        pEndpoint = m_Endpoints.GetAt(0);
        m_Endpoints.RemoveAt(0);
        delete pEndpoint;
    }
}   //  CRenderEndpoints::~CRenderEndpoints()


//--------------------------------------------------------------------------;
//
//  HRESULT CRenderEndpoints::PlaySine
//
//  Description:
//      Plays a sine tone through all endpoints.
//
//  Arguments:
//      None.
//
//  Return (HRESULT):
//      S_OK if no error, error code otherwise.
//
//
//--------------------------------------------------------------------------;
HRESULT CRenderEndpoints::PlaySine()
{
    CUnitRenderEndpoint    *pEndpoint;
    size_t                  iEndpoint;
    HRESULT                 hr;

    size_t cEndpoints = m_Endpoints.GetCount();
    m_pILogEx->LogEx(XMSG, eInfo2, L"There are a total of %Id endpoints to be streamed to.", cEndpoints);
    
    for (iEndpoint = cEndpoints; iEndpoint != 0; iEndpoint--)
    {
        pEndpoint = m_Endpoints.GetAt(iEndpoint - 1);

        m_pILogEx->LogEx(XMSG, eInfo2, L"Streaming to endpoint (%s)", pEndpoint->GetName());
                    
        if (S_OK != (hr = pEndpoint->PlaySine()))
        {
            m_pILogEx->LogEx(
                    XMSG,
                    eError,
                    L"PlaySine() through %ls returned error (%s).",
                    pEndpoint->GetName(),
                    GetHRString(hr));

            return (hr);
        }
    }

    return (S_OK);
}   //   CRenderEndpoints::PlaySine()


//--------------------------------------------------------------------------;
//
//  HRESULT CRenderEndpoints::Stop
//
//  Description:
//      Stops streaming through all endpoints.
//
//  Arguments:
//      None.
//
//  Return (HRESULT):
//      S_OK if no error, error code otherwise.
//
//
//--------------------------------------------------------------------------;
HRESULT CRenderEndpoints::Stop()
{
    CUnitRenderEndpoint    *pEndpoint;
    size_t                  iEndpoint;
    HRESULT                 hr;

    for (iEndpoint = m_Endpoints.GetCount(); iEndpoint != 0; iEndpoint--)
    {
        pEndpoint = m_Endpoints.GetAt(iEndpoint - 1);

        if (S_OK != (hr = pEndpoint->Stop()))
        {
            m_pILogEx->LogEx(
                    XMSG,
                    eError,
                    L"Stop() through %ls returned error (0x%s).",
                    pEndpoint->GetName(),
                    GetHRString(hr));

            return (hr);
        }
    }

    return (S_OK);
}   //   CRenderEndpoints::Stop()


//--------------------------------------------------------------------------;
//
//  DWORD TC_RenderToAllEndpoints
//
//  Description:
//      TestCase: Renders to all render endpoints simultaneously.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_RenderToAllEndpoints()
{
    CComPtr<IBasicLog>      pBasicLog = NULL;
    HRESULT                 hr;

    if (S_OK != (hr = g_IShell->GetBasicLog(&pBasicLog)))
    {
        XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: GetBasicLog returned error (%s).",
                    GetHRString(hr));

        return (FNS_PASS);
    }

    {
#ifndef BUILD_TAEF
        CRenderEndpoints Endpoints(pBasicLog, g_pShell->m_pszHwIdTestDeviceSelect, hr);
#else
        CRenderEndpoints Endpoints(pBasicLog, g_IShell->m_pszHwIdTestDeviceSelect, hr);
#endif

        if (FAILED(hr))
        {
            XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: Creating collection of render endpoints returned error (%s).",
                    GetHRString(hr));
                    
            return (FNS_PASS);
        }
        
        if (0 == Endpoints.Count())
        {
            XLOG(
                    XSKIP,
                    1,
                    L"SKIP: No render endpoints.");

            return (FNS_PASS);
        }

        if (S_OK != (hr = Endpoints.PlaySine()))
        {
            XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: Encountered error playing sine tone (%s).",
                    GetHRString(hr));

            return (FNS_PASS);
        }

        Sleep(5000);

        if (S_OK != (hr = Endpoints.Stop()))
        {
            XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: Encountered error stopping stream (%s).",
                    GetHRString(hr));

            return (FNS_PASS);
        }
    }

    return (FNS_PASS);
}   //   TC_RenderToAllEndpoints()


//--------------------------------------------------------------------------;
//
//  DWORD TC_VerifyMuteOnCompressedEndpoint
//
//  Description:
//      TestCase: On compressed endpoints, verifies that there exists a
//      nearby associated hardware mute control.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      Unused.  Failures propagated through XLOG.
//
//
//--------------------------------------------------------------------------;
DWORD WINAPI TC_VerifyMuteOnCompressedEndpoint()
{
    CAudioLogoDevice               *pCurrentDevice = NULL;
    IMMDevice                      *pEndpoint = NULL;
    CComPtr<IPropertyStore>         pIStore = NULL;
    CComPtr<IEndpointUtility>       pIEPUtil = NULL;
    CComPtr<IPartsList>             pIParts = NULL;
    CComPtr<IPart>                  pIPart = NULL;
    bool                            bCompressed;
    HRESULT                         hr;

    if (NULL == (pCurrentDevice = g_pAudioLogoDeviceTestModule->GetDevice()))
    {
        XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: GetDevice encountered error.");

        return (FNS_PASS);
    }

    if (NULL == (pEndpoint = pCurrentDevice->GetEndpoint()))
    {
        XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: GetEndpoint encountered error.");

        return (FNS_PASS);
    }

    if (S_OK != (hr = GetEndpointCompressedSupport(pEndpoint, &bCompressed)))
    {
        XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: GetEndpointCompressedSupport returned error "
                        L"(%s).",
                    GetHRString(hr));

        return (FNS_PASS);
    }

    if (!bCompressed)
    {
        XLOG(
                    XMSG,
                    eInfo2,
                    L"Endpoint (%s) does not support compressed formats.  "
                        L"Skipping check for mute control.",
                    pCurrentDevice->GetName());

        return (FNS_PASS);
    }
    else
    {
        XLOG(
                    XMSG,
                    eInfo2,
                    L"Endpoint (%s) supports compressed formats.  "
                        L"Looking for mute control...",
                    pCurrentDevice->GetName());
    }

    //  It supports non-PCM formats!
    hr = pEndpoint->Activate(
                        __uuidof(IEndpointUtility),
                        CLSCTX_ALL,
                        NULL,
                        (void**)&pIEPUtil);

    if (S_OK != hr)
    {
        XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: QueryInterface for utility returned error (%s).",
                    GetHRString(hr));

        return (FNS_PASS);
    }

    //  Getting the parts 
    if (S_OK != (hr = GetEndpointPartsToHostConnector(pEndpoint, &pIParts)))
    {
        XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: GetEndpointPartsToHostConnector returned "
                        L"error (%s).",
                    GetHRString(hr));

        return (FNS_PASS);
    }

    hr = pIEPUtil->FindNearestPartWithInterface(
                    pIParts,
                    __uuidof(IAudioMute),
                    &pIPart);

    if (S_OK != hr)
    {
        XLOG(
                    XFAIL,
                    eError,
                    L"FAIL: Looking for IPart with IAudioMute returned "
                        L"error (%s).",
                    GetHRString(hr));
    }
    else
    {
        CComHeapPtr<WCHAR>          pszPartId;
        
        XLOG(XMSG, eInfo2, L"Found IPart with IAudioMute interface:");

        if (S_OK != (hr = pIPart->GetGlobalId(&pszPartId)))
        {
            XLOG(
                        XFAIL,
                        eError,
                        L"FAIL: Getting id for IPart with IAudioMute "
                            L"interface returned error (%s).",
                        GetHRString(hr));
        }
        else
        {
            XLOG(
                        XMSG,
                        eInfo2,
                        L"  IPart(GlobalId): %ls",
                        (LPCWSTR)pszPartId);
        }
    }

    return (FNS_PASS);
}   //   TC_VerifyMuteOnCompressedEndpoint()

