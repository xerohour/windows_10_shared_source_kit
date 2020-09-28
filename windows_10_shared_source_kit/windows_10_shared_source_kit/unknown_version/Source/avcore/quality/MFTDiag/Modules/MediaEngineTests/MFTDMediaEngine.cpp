//*****************************************************************************
// Copyright (C) Microsoft Corporation, 2005.
//
// File:    WMVTests.cpp
//
// History:
//  5/18/05      Created as part of the Codec Pack Automation BVT
//               suite.
//
//*****************************************************************************
//

#include "MFTDMediaEngine.h"
#include "resource.h"

EXTERN_GUID(D3D11_DECODER_PROFILE_HEVC_VLD_MAIN, 0x5b11d51b, 0x2f4c, 0x4452, 0xbc, 0xc3, 0x09, 0xf2, 0xa1, 0x16, 0x0c, 0xc0);
EXTERN_GUID(D3D11_DECODER_PROFILE_HEVC_VLD_MAIN10, 0x107af0e0, 0xef1a, 0x4d19, 0xab, 0xa8, 0x67, 0xa1, 0x63, 0x07, 0x3d, 0x13);

EXTERN_GUID(D3D11_DECODER_PROFILE_VP8_VLD, 0x463707f8, 0xa1d0, 0x4585, 0x87, 0x6d, 0x83, 0xaa, 0x6d, 0x60, 0xb8, 0x9e);
EXTERN_GUID(D3D11_DECODER_PROFILE_VP9_VLD_PROFILE0, 0x463707f8, 0xa1d0, 0x4585, 0x87, 0x6d, 0x83, 0xaa, 0x6d, 0x60, 0xb8, 0x9e);
EXTERN_GUID(D3D11_DECODER_PROFILE_VP9_VLD_10BIT_PROFILE2, 0xa4c749ef, 0x6ecf, 0x48aa, 0x84, 0x48, 0x50, 0xa7, 0xa1, 0x16, 0x5f, 0xf7);


using namespace WEX::Logging;
using namespace WEX::Common;
using namespace WEX::TestExecution;

#define SAFECOMFREE(x) \
    { \
		if((x) != NULL) \
        		{ \
			CoTaskMemFree(x); \
			(x) = NULL; \
        		} \
	}

HRESULT InitializePlaybackTestTable(CXmlRootConfig** ppXMLRootConfig, DWORD dwTestConfig);

//Entry point for the test.
//This function will instantiate the desired test object and call the methods for executing the test.


HRESULT VerifyHardwareDecoderPresent(GUID subType)
{
    HRESULT                 hr = S_OK;
    MFT_REGISTER_TYPE_INFO  mfrtiInputType = { GUID_NULL };
    IMFActivate**           ppMFTActivates = NULL;
    UINT32                  un32NumActivates = 0;

    do
    {
        mfrtiInputType.guidMajorType = MFMediaType_Video;
        mfrtiInputType.guidSubtype = subType;

        hr = MFTEnumEx(
            MFT_CATEGORY_VIDEO_DECODER,
            MFT_ENUM_FLAG_HARDWARE,
            &mfrtiInputType,
            NULL,
            &ppMFTActivates,
            &un32NumActivates
            );
        if (FAILED(hr))
        {
            break;
        }

        if (un32NumActivates == 0)
        {
            // No Hardware decoders are present
            hr = S_FALSE;
            break;
        }
    } while (false);

    for (UINT32 un32CurrActivate = 0; un32CurrActivate < un32NumActivates; un32CurrActivate++)
    {
        SAFERELEASE(ppMFTActivates[un32CurrActivate]);
    }

    SAFECOMFREE(ppMFTActivates);

    return hr;
}

GUID GetSubTypeFromString(CString stringSubType)
{
    GUID subtype = GUID_NULL;

    if (stringSubType == "MEDIASUBTYPE_MP4V")
    {
        subtype = MFVideoFormat_MP4V;
    }
    else if (stringSubType == "MFVideoFormat_MJPG" || stringSubType == "MEDIASUBTYPE_MJPG")
    {
        subtype = MFVideoFormat_MJPG;
    }
    else if (stringSubType == "MEDIASUBTYPE_H263")
    {
        subtype = MFVideoFormat_H263;
    }

    return subtype;
}


HRESULT CreateDXGIManager(
    UINT32*                 pun32ResetToken,
    IMFDXGIDeviceManager**  ppDXGIManager)
{
    HRESULT                 hr                      = S_OK;
    ID3D11Device*           pD3D11Device            = NULL;
    D3D_FEATURE_LEVEL       d3dflFeatureLevelUsed   = D3D_FEATURE_LEVEL_9_1;
    UINT32                  un32ResetToken          = 0;
    IMFDXGIDeviceManager*   pDXGIDeviceManager      = NULL;

    static const D3D_FEATURE_LEVEL levels [] = {
        D3D_FEATURE_LEVEL_11_1,

        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,

        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };



    do
    {
        if( (pun32ResetToken == NULL)   ||
            (ppDXGIManager == NULL)     )
        {
            hr = E_POINTER;
            break;
        }

        (*pun32ResetToken)  = 0;
        (*ppDXGIManager)    = NULL;

        hr = D3D11CreateDevice(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            levels,
            ARRAYSIZE(levels), 
            D3D11_SDK_VERSION,
            &pD3D11Device,
            &d3dflFeatureLevelUsed,
            NULL
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = MFCreateDXGIDeviceManager(
            &un32ResetToken,
            &pDXGIDeviceManager
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pDXGIDeviceManager->ResetDevice(
            pD3D11Device,
            un32ResetToken
            );
        if(FAILED(hr))
        {
            break;
        }

        (*pun32ResetToken)  = un32ResetToken;
        (*ppDXGIManager)    = pDXGIDeviceManager;
        (*ppDXGIManager)->AddRef();
    }while(false);

    SAFERELEASE(pD3D11Device);
    SAFERELEASE(pDXGIDeviceManager);

    return hr;
}


HRESULT VerifyDXVASupported(GUID guid)
{
    HRESULT                     hr = S_OK;
    BOOL                        bShutdownMF = FALSE;
    UINT32                      un32ResetToken = 0;
    UINT32                      un32NumDecoderProfiles = 0;
    IMFDXGIDeviceManager*       pDXGIDeviceManager = NULL;
    ID3D11VideoDevice*          pD3D11VideoDevice = NULL;
    HANDLE                      hDevice = NULL;
    GUID                        guidDecoder = GUID_NULL;
    BOOL                        bSupportsDXVA = FALSE;

    do
    {
        hr = CreateDXGIManager(
            &un32ResetToken,
            &pDXGIDeviceManager
            );
        if (FAILED(hr))
        {
            break;
        }

        hr = pDXGIDeviceManager->OpenDeviceHandle(
            &hDevice
            );
        if (FAILED(hr))
        {
            break;
        }

        hr = pDXGIDeviceManager->GetVideoService(
            hDevice,
            __uuidof(ID3D11VideoDevice),
            (void**)&pD3D11VideoDevice
            );

        if (FAILED(hr))
        {
            if (hr == E_NOINTERFACE)
            {
                /*If a ID3D11VideoDevice is specified and the D3D device created is using the reference rasterizer or WARP.Or it is a hardware device and you
                are using the Microsoft Basic Display Adapter.*/
                hr = S_FALSE;
            }
            break;
        }

        un32NumDecoderProfiles = pD3D11VideoDevice->GetVideoDecoderProfileCount();

        for (UINT32 un32CurrProfile = 0; un32CurrProfile < un32NumDecoderProfiles; un32CurrProfile++)
        {
            hr = pD3D11VideoDevice->GetVideoDecoderProfile(
                un32CurrProfile,
                &guidDecoder
                );
            if (FAILED(hr))
            {
                break;
            }
            // D3D11_DECODER_PROFILE_VP9_VLD_10BIT_PROFILE2
            // 8bit VP9 file can be decoded by 8bit decoder or 10bit hardware decoder
            if (guid == D3D11_DECODER_PROFILE_VP9_VLD_PROFILE0)
            {
                if ((guidDecoder == guid) || (guidDecoder == D3D11_DECODER_PROFILE_VP9_VLD_10BIT_PROFILE2))
                {
                    bSupportsDXVA = TRUE;
                    break;
                }
            }
            else if(guid == D3D11_DECODER_PROFILE_VP9_VLD_10BIT_PROFILE2 || guid == D3D11_DECODER_PROFILE_VP8_VLD)
            {
                 if (guid == guidDecoder)
                {
                    bSupportsDXVA = TRUE;
                    break;
                }
            } else if (guid == D3D11_DECODER_PROFILE_HEVC_VLD_MAIN) 
            {
                // D3D11_DECODER_PROFILE_HEVC_VLD_MAIN
                // 8bit HEVC file can be decoded by 8bit decoder or 10bit hardware decoder
                if ((guidDecoder == guid) || (guidDecoder == D3D11_DECODER_PROFILE_HEVC_VLD_MAIN10))
                {
                    bSupportsDXVA = TRUE;
                    break;
                }
            }
            else
            {
                // D3D11_DECODER_PROFILE_HEVC_VLD_MAIN10
                // 10bit hevc file has to be decoded by 10bit hevc hardware decoder
                if (guid == guidDecoder)
                {
                    bSupportsDXVA = TRUE;
                    break;
                }
            }
        }

        if (FAILED(hr))
        {
            break;
        }

        if (bSupportsDXVA == FALSE)
        {
            hr = S_FALSE;
            break;
        }
    } while (false);

    if (hDevice != NULL)
    {
        pDXGIDeviceManager->CloseDeviceHandle(hDevice);
        hDevice = NULL;
    }

    SAFERELEASE(pD3D11VideoDevice);
    SAFERELEASE(pDXGIDeviceManager);


    return hr;
}



#ifdef BUILD_MINWIN

void CMediaEngineTests::PlaybackTest110() {
    VERIFY_SUCCEEDED(ExecuteTest(110));
}
void CMediaEngineTests::PlaybackTest111() {
    VERIFY_SUCCEEDED(ExecuteTest(111));
}
void CMediaEngineTests::PlaybackTest118() {
    VERIFY_SUCCEEDED(ExecuteTest(118));
}
void CMediaEngineTests::PlaybackTest125() {
    VERIFY_SUCCEEDED(ExecuteTest(125));
}
void CMediaEngineTests::PlaybackTest129() {
    VERIFY_SUCCEEDED(ExecuteTest(129));
}
void CMediaEngineTests::PlaybackTest131() {
    VERIFY_SUCCEEDED(ExecuteTest(131));
}
void CMediaEngineTests::PlaybackTest150() {
    VERIFY_SUCCEEDED(ExecuteTest(150));
}
void CMediaEngineTests::PlaybackTest151() {
    VERIFY_SUCCEEDED(ExecuteTest(151));
}
void CMediaEngineTests::PlaybackTest152() {
    VERIFY_SUCCEEDED(ExecuteTest(152));
}
void CMediaEngineTests::PlaybackTest153() {
    VERIFY_SUCCEEDED(ExecuteTest(153));
}
void CMediaEngineTests::PlaybackTest154() {
    VERIFY_SUCCEEDED(ExecuteTest(154));
}
void CMediaEngineTests::PlaybackTest155() {
    VERIFY_SUCCEEDED(ExecuteTest(155));
}
void CMediaEngineTests::PlaybackTest156() {
    VERIFY_SUCCEEDED(ExecuteTest(156));
}
void CMediaEngineTests::PlaybackTest157() {
    VERIFY_SUCCEEDED(ExecuteTest(157));
}



//H263 tests
void CMediaEngineTests::PlaybackTest160() {
    VERIFY_SUCCEEDED(ExecuteTest(160));
}
void CMediaEngineTests::PlaybackTest161() {
    VERIFY_SUCCEEDED(ExecuteTest(161));
}
void CMediaEngineTests::PlaybackTest162() {
    VERIFY_SUCCEEDED(ExecuteTest(162));
}
void CMediaEngineTests::PlaybackTest163() {
    VERIFY_SUCCEEDED(ExecuteTest(163));
}


void CMediaEngineTests::PlaybackTest444() {
    VERIFY_SUCCEEDED(ExecuteTest(444));
}
void CMediaEngineTests::PlaybackTest450() {
    VERIFY_SUCCEEDED(ExecuteTest(450));
}

//Decoder  Tests
void CMediaEngineTests::H264PlaybackTest1()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(1));
}

void CMediaEngineTests::H264PlaybackTest2()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(2));
}

void CMediaEngineTests::H264PlaybackTest3()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(3));
}

void CMediaEngineTests::H264PlaybackTest4()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(4));
}

void CMediaEngineTests::H264PlaybackTest5()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(5));
}

void CMediaEngineTests::H264PlaybackTest6()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(6));
}

void CMediaEngineTests::H264PlaybackTest7()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(7));
}

void CMediaEngineTests::H264PlaybackTest8()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(8));
}

void CMediaEngineTests::H264PlaybackTest9()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(9));
}

void CMediaEngineTests::H264PlaybackTest11()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(11));
}

void CMediaEngineTests::H264PlaybackTest12()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(12));
}

void CMediaEngineTests::H264PlaybackTest13()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(13));
}

void CMediaEngineTests::H264PlaybackTest14()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(14));
}

void CMediaEngineTests::H264PlaybackTest15()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(15));
}

void CMediaEngineTests::H264PlaybackTest16()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(16));
}

void CMediaEngineTests::H264PlaybackTest17()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(17));
}

void CMediaEngineTests::H264PlaybackTest18()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(18));
}

void CMediaEngineTests::H264PlaybackTest19()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(19));
}


void CMediaEngineTests::WMVPlaybackTest32()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(32));
}
void CMediaEngineTests::WMVPlaybackTest33()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(33));
}
void CMediaEngineTests::WMVPlaybackTest34()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(34));
}
void CMediaEngineTests::WMVPlaybackTest35()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(35));
}
void CMediaEngineTests::WMVPlaybackTest36()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(36));
}
void CMediaEngineTests::WMVPlaybackTest37()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(37));
}
void CMediaEngineTests::WMVPlaybackTest38()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(38));
}
void CMediaEngineTests::WMVPlaybackTest39()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(39));
}
void CMediaEngineTests::WMVPlaybackTest40()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(40));
}
void CMediaEngineTests::WMVPlaybackTest41()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(41));
}
void CMediaEngineTests::WMVPlaybackTest42()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(42));
}
void CMediaEngineTests::WMVPlaybackTest43()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(43));
}

void CMediaEngineTests::H264FuzzTest100()
{
    ExecuteDXVATest(100);
}
void CMediaEngineTests::H264FuzzTest101()
{
    ExecuteDXVATest(101);
}
void CMediaEngineTests::H264FuzzTest102()
{
    ExecuteDXVATest(102);
}


void CMediaEngineTests::WMVFuzzTest114() {
    ExecuteDXVATest(114);
}
void CMediaEngineTests::WMVFuzzTest115() {
    ExecuteDXVATest(115);
}
void CMediaEngineTests::WMVFuzzTest116() {
    ExecuteDXVATest(116);
}


void CMediaEngineTests::H264QualityTest200()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(200));
}
void CMediaEngineTests::H264QualityTest201()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(201));
}
void CMediaEngineTests::H264QualityTest202()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(202));
}
void CMediaEngineTests::H264QualityTest203()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(203));
}
void CMediaEngineTests::H264QualityTest204()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(204));
}
void CMediaEngineTests::H264QualityTest205()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(205));
}

void CMediaEngineTests::WMVQualityTest212() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(212));
}
void CMediaEngineTests::WMVQualityTest213() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(213));
}
void CMediaEngineTests::WMVQualityTest214() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(214));
}
void CMediaEngineTests::WMVQualityTest215() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(215));
}
void CMediaEngineTests::WMVQualityTest216() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(216));
}
void CMediaEngineTests::WMVQualityTest217() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(217));
}
void CMediaEngineTests::WMVQualityTest218() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(218));
}
void CMediaEngineTests::WMVQualityTest219() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(219));
}
void CMediaEngineTests::WMVQualityTest220(void)
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(220));
}

void CMediaEngineTests::WMVQualityTest221(void)
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(221));
}

void CMediaEngineTests::H264QualityTest299(void)
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(299));
}

void CMediaEngineTests::VisValPlaybackTest90() {
    VERIFY_SUCCEEDED(ExecuteVisValTest(90));
}
void CMediaEngineTests::VisValPlaybackTest91() {
    VERIFY_SUCCEEDED(ExecuteVisValTest(91));
}
void CMediaEngineTests::VisValPlaybackTest92() {
    VERIFY_SUCCEEDED(ExecuteVisValTest(92));
}

void CMediaEngineTests::HEVCPlaybackTest50(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(50));
}

void CMediaEngineTests::HEVCPlaybackTest51(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(51));
}

void CMediaEngineTests::HEVCPlaybackTest52(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(52));
}

void CMediaEngineTests::HEVCPlaybackTest53(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(53));
}

void CMediaEngineTests::HEVCPlaybackTest54(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(54));
}

void CMediaEngineTests::HEVCPlaybackTest55(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(55));
}

void CMediaEngineTests::HEVCPlaybackTest56(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(56));
}

void CMediaEngineTests::HEVCPlaybackTest57(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(57));
}

void CMediaEngineTests::HEVCPlaybackTest58(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(58));
}

void CMediaEngineTests::HEVCPlaybackTest59(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(59));
}

void CMediaEngineTests::HEVCPlaybackTest60(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(60));
}

void CMediaEngineTests::HEVCPlaybackTest61(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(61));
}

void CMediaEngineTests::HEVCPlaybackTest62(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(62));
}

void CMediaEngineTests::HEVCPlaybackTest63(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(63));
}

void CMediaEngineTests::HEVCPlaybackTest64(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(64));
}

void CMediaEngineTests::HEVCPlaybackTest65(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(65));
}

void CMediaEngineTests::HEVCFuzzTest120(void)
{
    ExecuteHEVCDXVATest(120);
}

void CMediaEngineTests::HEVCFuzzTest121(void)
{
    ExecuteHEVCDXVATest(121);
}

void CMediaEngineTests::HEVCFuzzTest122(void)
{
    ExecuteHEVCDXVATest(122);
}

void CMediaEngineTests::HEVCFuzzTest123(void)
{
    ExecuteHEVCDXVATest(123);
}

void CMediaEngineTests::HEVCFuzzTest124(void)
{
    ExecuteHEVCDXVATest(124);
}

void CMediaEngineTests::HEVCFuzzTest125(void)
{
    ExecuteHEVCDXVATest(125);
}

void CMediaEngineTests::HEVCQualityTest230(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(230));
}

void CMediaEngineTests::HEVCQualityTest231(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(231));
}

void CMediaEngineTests::HEVCQualityTest232(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(232));
}

void CMediaEngineTests::HEVCQualityTest233(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(233));
}

void CMediaEngineTests::HEVCQualityTest234(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(234));
}

void CMediaEngineTests::HEVCQualityTest235(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(235));
}

void CMediaEngineTests::HEVCQualityTest236(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(236));
}

void CMediaEngineTests::HEVCQualityTest237(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(237));
}

void CMediaEngineTests::HEVCQualityTest238(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(238));
}

void CMediaEngineTests::HEVCQualityTest239(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(239));
}

void CMediaEngineTests::HEVCQualityTest240(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(240));
}

void CMediaEngineTests::HEVCQualityTest241(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(241));
}

void CMediaEngineTests::HEVCQualityTest242(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(242));
}

void CMediaEngineTests::HEVCQualityTest243(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(243));
}

void CMediaEngineTests::HEVCQualityTest244(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(244));
}

void CMediaEngineTests::HEVCQualityTest245(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(245));
}

void CMediaEngineTests::HEVCQualityTest246(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(246));
}

void CMediaEngineTests::HEVCQualityTest247(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(247));
}

void CMediaEngineTests::HEVCQualityTest248(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(248));
}

void CMediaEngineTests::HEVCQualityTest249(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(249));
}

void CMediaEngineTests::HEVCQualityTest250(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(250));
}

void CMediaEngineTests::HEVCQualityTest251(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(251));
}

void CMediaEngineTests::HEVCQualityTest252(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(252));
}

void CMediaEngineTests::HEVCQualityTest253(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(253));
}

void CMediaEngineTests::HEVCQualityTest254(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(254));
}

void CMediaEngineTests::HEVCQualityTest255(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(255));
}

void CMediaEngineTests::HEVCQualityTest256(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(256));
}

void CMediaEngineTests::HEVCQualityTest257(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(257));
}

void CMediaEngineTests::HEVCQualityTest258(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(258));
}

//OneCore tests
void CMediaEngineTests::MJPEGQualityTest1()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(1));
}
void CMediaEngineTests::MSS1QualityTest2()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(2));
}
void CMediaEngineTests::DVQualityTest3()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(3));
}
void CMediaEngineTests::MP43QualityTest4()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(4));
}
void CMediaEngineTests::Mpeg4DecQualityTest5()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(5));
}
void CMediaEngineTests::MP4SQualityTest6()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(6));
}
void CMediaEngineTests::MP4ResChange()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(7));
}
#endif

void CMediaEngineTests::HEVCPlaybackTest66(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(66));
}
void CMediaEngineTests::HEVCQualityTest259(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(259));
}

static HRESULT ReadRegDWORD(
    __in LPCWSTR szKeyName,
    __in LPCWSTR szValueName,
    __out DWORD* pValue
    )
{
    DWORD cbValue = sizeof(*pValue);
    HRESULT hr = S_OK;

    const LONG ret = RegGetValue(HKEY_LOCAL_MACHINE,
        szKeyName,
        szValueName,
        RRF_RT_REG_DWORD,
        nullptr,
        pValue,
        &cbValue);
    if( ret != ERROR_SUCCESS )
    {
        hr = HRESULT_FROM_WIN32(ret);
    }
    else if (cbValue != sizeof(*pValue)) {
        hr = E_INVALIDARG;
    }
    return hr;
}

void CMediaEngineTests::MediaPowerProfileTest() {
    wchar_t szPowerKeys[] = L"SYSTEM\\ControlSet001\\Control\\Power\\PowerSettings\\54533251-82be-4824-96c1-47b60b740d00\\";
    wchar_t szPostfix[] = L"\\4569E601-272E-4869-BCAB-1C6C03D7966F\\381b4222-f694-41f0-9685-ff5bb260df2e";
    wchar_t szCPIncrease[] = L"2ddd5a84-5a71-437e-912a-db0b8c788732";

    wchar_t szFull[500];
    VERIFY_SUCCEEDED(StringCchPrintf(szFull, ARRAYSIZE(szFull), L"%s%s%s", szPowerKeys, szCPIncrease, szPostfix));

    DWORD dwValue = 0;
    VERIFY_SUCCEEDED(ReadRegDWORD(szFull, L"AcSettingIndex", &dwValue));

    if (dwValue != 2) {
        VERIFY_SUCCEEDED(E_INVALIDARG);
    }

    VERIFY_SUCCEEDED(ReadRegDWORD(szFull, L"DcSettingIndex", &dwValue));

    if (dwValue != 2) {
        VERIFY_SUCCEEDED(E_INVALIDARG);
    }
}

void CMediaEngineTests::H264DRMTest1() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(1));
}

void CMediaEngineTests::H264DRMTest2() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(2));
}

void CMediaEngineTests::H264DRMTest3() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(3));
}

void CMediaEngineTests::H264DRMTest4() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(4));
}

void CMediaEngineTests::H264DRMTest5() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(5));
}

void CMediaEngineTests::HEVCDRMTest6() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(6));
}

void CMediaEngineTests::HEVCDRMTest7() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(7));
}

void CMediaEngineTests::HEVCDRMTest8() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(8));
}

void CMediaEngineTests::HEVCDRMTest9() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(9));
}

void CMediaEngineTests::HEVCDRMTest10() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(10));
}

void CMediaEngineTests::H264MPOTest11() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(11));
}


void CMediaEngineTests::HEVCMPOTest12() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(12));
}

void CMediaEngineTests::VP9PlaybackTest1(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(1));
}

void CMediaEngineTests::VP9PlaybackTest2(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(2));
}

void CMediaEngineTests::VP9PlaybackTest3(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(3));
}

void CMediaEngineTests::VP9PlaybackTest4(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(4));
}

void CMediaEngineTests::VP9PlaybackTest5(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(5));
}

void CMediaEngineTests::VP9PlaybackTest6(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(6));
}

void CMediaEngineTests::VP9PlaybackTest7(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(7));
}

void CMediaEngineTests::VP9PlaybackTest8(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(8));
}

void CMediaEngineTests::VP9PlaybackTest9(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(9));
}



void CMediaEngineTests::VP9QualityTest50(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(50));
}

void CMediaEngineTests::VP9QualityTest51(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(51));
}

void CMediaEngineTests::VP9QualityTest52(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(52));
}

void CMediaEngineTests::VP9QualityTest53(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(53));
}

void CMediaEngineTests::VP9QualityTest54(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(54));
}

void CMediaEngineTests::VP9QualityTest55(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(55));
}

void CMediaEngineTests::VP9QualityTest56(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(56));
}

void CMediaEngineTests::VP9QualityTest57(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(57));
}

void CMediaEngineTests::VP9QualityTest58(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(58));
}


void CMediaEngineTests::VP8PlaybackTest20(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(20));
}

void CMediaEngineTests::VP8PlaybackTest21(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(21));
}

void CMediaEngineTests::VP8PlaybackTest22(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(22));
}

void CMediaEngineTests::VP8PlaybackTest23(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(23));
}

void CMediaEngineTests::VP8PlaybackTest24(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(24));
}


void CMediaEngineTests::VP8QualityTest70(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(70));
}

void CMediaEngineTests::VP8QualityTest71(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(71));
}

void CMediaEngineTests::VP8QualityTest72(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(72));
}

void CMediaEngineTests::VP8QualityTest73(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(73));
}

void CMediaEngineTests::VP8QualityTest74(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(74));
}

void CMediaEngineTests::VP9_10bitPlaybackTest90(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(90));
}


void CMediaEngineTests::VP9_10bitQualityTest91(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(91));
}

void CMediaEngineTests::VP9_10bitPlaybackTest92(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(92));
}


void CMediaEngineTests::VP9_10bitQualityTest93(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(93));
}

void CMediaEngineTests::VP8FuzzTest100(void)
{
    ExecuteVPxDXVATest(100);
}

void CMediaEngineTests::VP9FuzzTest101(void)
{
    ExecuteVPxDXVATest(101);
}

void CMediaEngineTests::VP9FuzzTest102(void)
{
    ExecuteVPxDXVATest(102);
}

void CMediaEngineTests::VP9FuzzTest103(void)
{
    ExecuteVPxDXVATest(103);
}

void CMediaEngineTests::VP9FuzzTest104(void)
{
    ExecuteVPxDXVATest(104);
}

void CMediaEngineTests::VP9FuzzTest105(void)
{
    ExecuteVPxDXVATest(105);
}

void CMediaEngineTests::CustomXML(void)
{
    VERIFY_SUCCEEDED(RunCustomXML());
}



static  const TCHAR StoreMFTRegKey[] = TEXT("SOFTWARE\\Microsoft\\Windows Media Foundation\\StoreMFT");

void AllowVPxDecoderForClassicApps()
{
    HKEY    hkey;
    DWORD dwKeyValue = 1;

    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, StoreMFTRegKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL) == ERROR_SUCCESS)
    {
        Log::Comment(String().Format(L"EnableForwin32App reg key = 1", dwKeyValue));

        RegSetValueEx(hkey, TEXT("EnableForwin32App"), 0, REG_DWORD, (BYTE *)&dwKeyValue, sizeof(dwKeyValue));
        RegCloseKey(hkey);
    }
    else
    {
        Log::Comment(String().Format(L"failed to create reg key %s for VP9 decoder creation", StoreMFTRegKey));
    }
}

bool CMediaEngineTests::ClassSetup()
{
    VERIFY_SUCCEEDED(MFStartup(MF_VERSION));
    AllowVPxDecoderForClassicApps();
    return true;
}


bool CMediaEngineTests::ClassCleanup()
{
    VERIFY_SUCCEEDED(MFShutdown());
    return true;
}

#ifdef BUILD_MINWIN

HRESULT CMediaEngineTests::ExecuteTest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;
    CString stringSubType;
    GUID subType = GUID_NULL;



    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_PLAYBACKCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputType/MF_MT_SUBTYPE", stringSubType);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if((stringSubType == "MFVideoFormat_MJPG") || (stringSubType == "MEDIASUBTYPE_MP4V") || (stringSubType == "MEDIASUBTYPE_H263"))
        {
            subType = GetSubTypeFromString(stringSubType);
            hr = VerifyHardwareDecoderPresent(subType);
            if (FAILED(hr))
            {
                break;
            }
            else if (hr == S_FALSE)
            {
                // No HW decoder present, skip test
                break;
            }
        }

        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Config", stringConfig);
        if (FAILED(hr) || stringConfig.IsEmpty())
        {
            // Use default config
            stringConfig = L"-DCOMP -DXVA";
        }
        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty() || !stringTestType.CompareNoCase(L"Playback"))
        {
            // Use default config
            stringTestType = L"Render";
        }

        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped, WEX::Common::String().Format(L"No Hardware decoder for %s found, skipping test", stringSubType));
    }

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}

HRESULT CMediaEngineTests::ExecuteTest()
{
    HRESULT hr = S_OK;

    //SetWexLogger();

    //Call the methods of to initialize and execute a particular test case until done.
    WEX::Common::String stringConfig;
    TestData::TryGetValue(L"Config", stringConfig);

    WEX::Common::String stringInput;
    TestData::TryGetValue(L"Input", stringInput);

    WEX::Common::String stringOutput;
    TestData::TryGetValue(L"Output", stringOutput);

    WEX::Common::String stringTestType;
    TestData::TryGetValue(L"TestType", stringTestType);

    WEX::Common::String stringTestID;
    TestData::TryGetValue(L"ID", stringTestID);

    WEX::Common::String stringTestTitle;
    TestData::TryGetValue(L"Title", stringTestTitle);

    WEX::Common::String stringDLLName;
    TestData::TryGetValue(L"DLL", stringDLLName);

    hr = MediaEngineTestEntryPointMobile(stringInput, stringOutput, stringConfig, stringTestType,
        _ttoi(stringTestID), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped);
    }

    return hr;
}

HRESULT CMediaEngineTests::ExecuteVisValTest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";
    CString stringSubType;

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;
    GUID subType = GUID_NULL;


    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_VISVALCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputType/MF_MT_SUBTYPE", stringSubType);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if((stringSubType == "MFVideoFormat_MJPG") || (stringSubType == "MEDIASUBTYPE_MP4V") || (stringSubType == "MEDIASUBTYPE_H263"))
        {
            subType = GetSubTypeFromString(stringSubType);
            hr = VerifyHardwareDecoderPresent(subType);
            if (FAILED(hr))
            {
                break;
            }
            else if (hr == S_FALSE)
            {
                // No HW decoder present, skip test
                break;
            }
        }

        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Config", stringConfig);
        if (FAILED(hr) || stringConfig.IsEmpty())
        {
            // Use default config
            stringConfig = L"-frameserver -DXVA -frames 200 -frameinterval 15 -visval";
        }
        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty() || !stringTestType.CompareNoCase(L"Playback"))
        {
            // Use default config
            stringTestType = L"Render";
        }

        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped, WEX::Common::String().Format(L"No Hardware decoder for %s found, skipping test", stringSubType));
    }

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}

HRESULT CMediaEngineTests::ExecuteOneCoreTest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;

    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_ONECORETESTSCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config. Assume frame server test if nothing is given
            stringConfig = L"-frameserver -frames 20 ";
        }
        else if (!stringTestType.CompareNoCase(L"Quality"))
        {
            stringConfig = L"-frameserver -frames 20 ";
        }
        else if (!stringTestType.CompareNoCase(L"Full_Quality"))
        {
            CString stringNumFrames;
            hr = pTestConfig->GetString(L"NumFrames", stringNumFrames);
            if (FAILED(hr) || stringNumFrames.IsEmpty())
            {
                stringConfig = L"-frameserver -NoDumpFrame  ";
            }
            else
            {
                stringConfig = L"-frameserver -NoDumpFrame -frames "+ stringNumFrames;
            }           
        }


        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

        if (hr == S_FALSE)
        {
            WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped);
        }

    } while (FALSE);

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}


HRESULT CMediaEngineTests::ExecuteDXVATest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;

    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_DXVATESTSCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config. Assume plabyack test if nothing is given
            stringTestType = L"Render";

            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Playback"))
        {
            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Quality"))
        {
            stringConfig = L"-frameserver -DXVA -frames 200  -frameinterval 15 -visval";
        }


        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

        if (hr == S_FALSE)
        {
            WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped);
        }

    } while (FALSE);

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}

#endif


HRESULT CMediaEngineTests::ExecuteVPxDXVATest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";
    CString stringInputType;

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;
    GUID guid = D3D11_DECODER_PROFILE_VP8_VLD;


    do
    {

        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_VPxDXVATESTSCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"InputType", stringInputType);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if (!stringInputType.CompareNoCase(L"VP8"))
        {
            guid = D3D11_DECODER_PROFILE_VP8_VLD;
        }
        else if (!stringInputType.CompareNoCase(L"VP9_PROFILE0"))
        {
            guid = D3D11_DECODER_PROFILE_VP9_VLD_PROFILE0;
        }
        else if (!stringInputType.CompareNoCase(L"VP9_PROFILE2"))
        {
            guid = D3D11_DECODER_PROFILE_VP9_VLD_10BIT_PROFILE2;
        }

        hr = VerifyDXVASupported(guid);
        if (FAILED(hr))
        {
            break;
        }
        else if (hr == S_FALSE)
        {
            // No HW HEVC decoder present, skip test
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config. Assume plabyack test if nothing is given
            stringTestType = L"Render";

            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Playback"))
        {
            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Quality"))
        {
            stringConfig = L"-frameserver -DXVA -frames 200  -frameinterval 15 -visval";
        }


        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);


    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped, WEX::Common::String().Format(L"No Hardware VPx decoder found, skipping test"));
    }

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}



HRESULT CMediaEngineTests::ExecutePipelineTest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringKID = L"";
    CString stringContentKey = L"";
    CString stringDLLName = L"";

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;

    GUID clsid = GUID_NULL;

    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_MEDIAPIPELINECONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetGUID(L"Component_CLSID", &clsid);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if (clsid == CLSID_MSH265DecoderMFT)
        {
            hr = VerifyDXVASupported(D3D11_DECODER_PROFILE_HEVC_VLD_MAIN);
            if (FAILED(hr))
            {
                WEX::Logging::Log::Comment(WEX::Common::String().Format(L"VerifyDXVASupported for D3D11_DECODER_PROFILE_HEVC_VLD_MAIN Error: 0x%x", hr));
                break;
            }
            else if (hr == S_FALSE)
            {
                // No HW decoder present, skip test
                break;
            }
        }

        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Config", stringConfig);
        if (FAILED(hr) || stringConfig.IsEmpty())
        {
            // Use default config
            stringConfig = L"-DCOMP -DXVA";
        }
        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config
            stringTestType = L"Render";
        }

        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if (!stringTestType.CompareNoCase(L"DRM"))
        {
            hr = pTestConfig->GetString(L"KID", stringKID);
            if (FAILED(hr))
            {
                hr = E_FAIL;
                break;
            }

            hr = pTestConfig->GetString(L"ContentKey", stringContentKey);
            if (FAILED(hr))
            {
                hr = E_FAIL;
                break;
            }
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

#ifndef BUILD_MINWIN

        hr = MediaEngineTestEntryPoint(stringInput, stringOutput, stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, stringKID, stringContentKey, WEX_LOG);
#else
        hr = MediaEngineTestEntryPointMobile(stringInput, stringOutput, stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, stringKID, stringContentKey, WEX_LOG);
#endif

    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped, WEX::Common::String().Format(L"No H265 DXVA decoder found, skipping test"));
    }

    // Mark the test as blocked if it is a DRM test and it failed due to issues with internet access.
    if (hr == WININET_E_NAME_NOT_RESOLVED && !stringTestType.CompareNoCase(L"DRM"))
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Blocked, WEX::Common::String().Format(L"DRM tests require internet access."));
        hr = S_FALSE;
    }

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}

HRESULT InitializePlaybackTestTable(CXmlRootConfig** ppXMLRootConfig, DWORD dwTestConfig)
{
    HRESULT hr = S_OK;
    HGLOBAL hResource = NULL;
    CXmlRootConfig* pConfig = NULL;

    do
    {
        if (ppXMLRootConfig == NULL)
        {
            hr = E_POINTER;
            break;
        }

        pConfig = new CXmlRootConfig();
        if (pConfig == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        HMODULE hMod = NULL;

        if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (TCHAR*)&InitializePlaybackTestTable, &hMod) == FALSE)
        {
            hr = E_FAIL;
            break;
        }

        HRSRC hRsrc = FindResource(hMod, MAKEINTRESOURCE(dwTestConfig), L"Config");
        if (hRsrc == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hResource = LoadResource(hMod, hRsrc);
        if (hResource == NULL)
        {
            hr = E_FAIL;
            break;
        }

        char* pszXML = (char*)LockResource(hResource);
        if (pszXML == NULL)
        {
            hr = E_FAIL;
            break;
        }

        DWORD dwXML = SizeofResource(hMod, hRsrc);
        if (dwXML == 0)
        {
            hr = E_FAIL;
            break;
        }

        int nConvertedLength = MultiByteToWideChar(CP_ACP, 0, pszXML, dwXML, NULL, NULL);

        CComBSTR bstrXML(nConvertedLength);

        if (MultiByteToWideChar(CP_ACP, 0, pszXML, dwXML, bstrXML, nConvertedLength) == 0)
        {
            hr = E_FAIL;
            break;
        }

        hr = pConfig->Init(bstrXML);
        if (FAILED(hr))
        {
            break;
        }

        *ppXMLRootConfig = pConfig;
    } while (false);

    if (hResource != NULL)
    {
        FreeResource(hResource);
    }

    if (FAILED(hr))
    {
        SAFEDELETES(pConfig);
    }

    return hr;
}

HRESULT CMediaEngineTests::GetGUIDsToTest(
    const DWORD dwTestID,
    GUID pGUIDsToTest[],
    WCHAR* pppszHWURLs[],
    const DWORD dwMaxGUIDsToTest,
    DWORD *pdwNumGUIDs)
{
    HRESULT hr = S_OK;

    do
    {
        if (pdwNumGUIDs == NULL)
        {
            hr = E_POINTER;
            break;
        }

        (*pdwNumGUIDs) = 1;
    } while (false);

    return hr;
}


HRESULT CMediaEngineTests::RunActualTest(const DWORD dwTestID, const GUID* pGUID, const WCHAR* pszHWURL)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";
    CString stringKID = L"";
    CString stringContentKey = L"";

    CXmlConfig* pTestConfig = NULL;

    do
    {
        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestID);

        pTestConfig = m_pXMLConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Category", stringTestType);
        
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config. Assume plabyack test if nothing is given
            stringTestType = L"Render";

            stringConfig = L"-DCOMP -DXVA";
        }
        else
        {
            hr = pTestConfig->GetString(L"Config", stringConfig);

            if (!stringTestType.CompareNoCase(L"Playback"))
            {
                if (FAILED(hr) || stringConfig.IsEmpty())
                {
                    stringConfig = L"-DCOMP -DXVA";
                }
                
            }
            else if (!stringTestType.CompareNoCase(L"Quality"))
            {
                if (FAILED(hr) || stringConfig.IsEmpty())
                {
                    stringConfig = L"-frameserver -DXVA -frames 200  -frameinterval 15 -visval";
                }
            }

#ifndef BUILD_MINWIN
            else if (!stringTestType.CompareNoCase(L"DRM"))
            {
                hr = pTestConfig->GetString(L"KID", stringKID);
                if (FAILED(hr))
                {
                    hr = E_FAIL;
                    break;
                }

                hr = pTestConfig->GetString(L"ContentKey", stringContentKey);
                if (FAILED(hr))
                {
                    hr = E_FAIL;
                    break;
                }
            }
#endif
        }

        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

#ifndef BUILD_MINWIN
        hr = MediaEngineTestEntryPoint(stringInput, stringOutput, stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, stringKID, stringContentKey, WEX_LOG);        
#else
        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, stringKID, stringContentKey, WEX_LOG);
#endif

    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped);
    }

    SAFE_DELETE(pTestConfig);

    return hr;
}

HRESULT CMediaEngineTests::RunActualChildTest(
    const DWORD     dwInstance,
    const WCHAR*    pwszContextName,
    const DWORD     dwContextSize)
{
    HRESULT hr = S_OK;
    return hr;
}


HRESULT CMediaEngineTests::ExecuteHEVCDXVATest(DWORD dwTestId)
{
    HRESULT hr = S_OK;

    CString stringInput;
    CString stringOutput;
    CString stringConfig;
    CString stringTestType;
    CString stringTestTitle;
    CString stringTestId;
    CString stringDLLName = L"";
    CString stringInputType;

    CXmlRootConfig* pRootConfig = NULL;
    CXmlConfig* pTestConfig = NULL;
    GUID guid = D3D11_DECODER_PROFILE_HEVC_VLD_MAIN;


    do
    {

        // Use the test ID to read the node from the XML
        stringTestId.Format(_T("%u"), dwTestId);
        InitializePlaybackTestTable(&pRootConfig, IDR_HEVCDXVATESTSCONFIG);

        pTestConfig = pRootConfig->get_TestConfig(stringTestId);
        if (pTestConfig == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"InputType", stringInputType);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        if (!stringInputType.CompareNoCase(L"HEVC10"))
        {
            guid = D3D11_DECODER_PROFILE_HEVC_VLD_MAIN10;
        }

        hr = VerifyDXVASupported(guid);
        if (FAILED(hr))
        {
            break;
        }
        else if (hr == S_FALSE)
        {
            // No HW HEVC decoder present, skip test
            break;
        }

        // Read the test parameters from the XML file
        hr = pTestConfig->GetString(L"InputFile", stringInput);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"Category", stringTestType);
        if (FAILED(hr) || stringTestType.IsEmpty())
        {
            // Use default config. Assume plabyack test if nothing is given
            stringTestType = L"Render";

            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Playback"))
        {
            stringConfig = L"-DCOMP -DXVA";
        }
        else if (!stringTestType.CompareNoCase(L"Quality"))
        {
            stringConfig = L"-frameserver -DXVA -frames 200  -frameinterval 15 -visval";
        }


        hr = pTestConfig->GetString(L"Name", stringTestTitle);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        hr = pTestConfig->GetString(L"testId", stringTestId);
        if (FAILED(hr))
        {
            hr = E_FAIL;
            break;
        }

        int nToken = 0;
        stringOutput = stringInput.Tokenize(L".", nToken);

        hr = MediaEngineTestEntryPointMobile(stringInput.GetString(), stringOutput.GetString(), stringConfig, stringTestType,
            _ttoi(stringTestId), stringTestTitle, stringDLLName, nullptr, nullptr, WEX_LOG);

    } while (FALSE);

    if (hr == S_FALSE)
    {
        WEX::Logging::Log::Result(WEX::Logging::TestResults::Skipped, WEX::Common::String().Format(L"No Hardware HEVC decoder found, skipping test"));
    }

    SAFE_DELETE(pTestConfig);
    SAFE_DELETE(pRootConfig);

    return hr;
}
