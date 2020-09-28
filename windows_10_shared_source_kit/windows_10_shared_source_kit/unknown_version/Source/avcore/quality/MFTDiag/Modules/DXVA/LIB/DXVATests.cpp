#include "stdafx.h"
#include "DXVATests.h"
#include "CTestMediaSessionPlayback.h"
#include "MFTVisValTests.h"
#include "LogTC.h"
#include "Resource.h"
#include "MFTDMIHelper.h"
#include "wmcodecdsp.h"
#include "ks.h"
#include "codecapi.h"
#include "codecapi_ext.h"
#include "securecommand.h"
#include "DXCompare.h"
#include "MFTDTDR.h"
#include "CConfigRegKey.h"
#include <uuids.h>
#include <Slpublic.h>
#include <Setupapi.h>
#include <D3dkmthk.h>
#include <shellscalingapi.h>
#include "MFTDPower.h"
#include "FindHCKContentFile.h"
#include "GetWinVer.h"
#include <comppkgsup.h>

// include others above here
#include <initguid.h>
#include <ntddvdeo.h>

#define MAX_DWORD                   0xFFFFFFFF

#define OUT_DIR_NAME                L".\\PlaybackOutput\\"
#define OUTPUTFILETESTNAME          L"PlaybackTest"
#define OUTPUTFILEFORMATSTRING      L"_%u_Instance_%03u.DUMP"
#define FILECOMPBUFFERLEN           1048576 // 1MB
#define DUMPDIRECTORY               L".\\PlaybackVV\\"
#define DELETEFILESONSUCCEESSNAME   L"VVDeleteOnSuccess"
#define DELETEFILESONFAILURENAME    L"VVDeleteOnFailure"

#define DEFAULT_RSMTHRESHOLD                        0.0
#define DEFAULT_PSNRTHRESHOLD                       50.0
#define DEFAULT_FAILPASSRATIO                       0.1
#define OVERSCAN_HEIGHT                             1088
#define OVERSCAN_VALUE                              8
#define DEFAULT_TRANSCODE_PSNR_DIFF_THRESHOLD       1.5
#define DEFAULT_TRANSCODE_PSNR_DIFF_LOWER_THRESHOLD 1.0
#define MAX_ADAPTERS                                256
#define HW_OPAQUE_SUBTYPE                           MFVideoFormat_420O
#define SW_OPAQUE_SUBTYPE                           MFVideoFormat_NV12

#define MPEG2_DECODE_INSKU  L"msmpeg2vdec-MPEG2VideoDecoderV3InSKU"

DEFINE_GUID(D3D11_DECODER_PROFILE_HEVC_VLD_MAIN, 0x5b11d51b, 0x2f4c, 0x4452, 0xbc, 0xc3, 0x09, 0xf2, 0xa1, 0x16, 0x0c, 0xc0);
DEFINE_GUID(D3D11_DECODER_PROFILE_HEVC_VLD_MAIN10, 0x107af0e0, 0xef1a, 0x4d19, 0xab, 0xa8, 0x67, 0xa1, 0x63, 0x07, 0x3d, 0x13);  


NTSTATUS D3DKMTQueryAdapterInfoPrivate(
    _Inout_ const D3DKMT_QUERYADAPTERINFO *pData
    )
{
    typedef NTSTATUS(WINAPI* LPFUNCTION)(_Inout_ const D3DKMT_QUERYADAPTERINFO *pData);

    HINSTANCE hlibGdi32 = LoadLibraryA("Gdi32.DLL");
    if (hlibGdi32 == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    LPFUNCTION lpfnD3DKMTQueryAdapterInfo = (LPFUNCTION)GetProcAddress(
        hlibGdi32, "D3DKMTQueryAdapterInfo");
    if (lpfnD3DKMTQueryAdapterInfo == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    return(*lpfnD3DKMTQueryAdapterInfo)(pData);
}


NTSTATUS  D3DKMTOpenAdapterFromDeviceNamePrivate(
    _Inout_ D3DKMT_OPENADAPTERFROMDEVICENAME *pData
    )
{
    typedef NTSTATUS(WINAPI* LPFUNCTION)(_Inout_ D3DKMT_OPENADAPTERFROMDEVICENAME *pData);

    HINSTANCE hlibGdi32 = LoadLibraryA("Gdi32.DLL");
    if (hlibGdi32 == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    LPFUNCTION lpfnD3DKMTOpenAdapterFromDeviceName = (LPFUNCTION)GetProcAddress(
        hlibGdi32, "D3DKMTOpenAdapterFromDeviceName");
    if (lpfnD3DKMTOpenAdapterFromDeviceName == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    return(*lpfnD3DKMTOpenAdapterFromDeviceName)(pData);
}


NTSTATUS D3DKMTCloseAdapterPrivate(
    _In_ const D3DKMT_CLOSEADAPTER *pData
    )
{
    typedef NTSTATUS(WINAPI* LPFUNCTION)(_Inout_ const D3DKMT_CLOSEADAPTER *pData);

    HINSTANCE hlibGdi32 = LoadLibraryA("Gdi32.DLL");
    if (hlibGdi32 == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    LPFUNCTION lpfnD3DKMTCloseAdapter = (LPFUNCTION)GetProcAddress(
        hlibGdi32, "D3DKMTCloseAdapter");
    if (lpfnD3DKMTCloseAdapter == NULL)
    {
        return STATUS_INVALID_PARAMETER;
    }

    return(*lpfnD3DKMTCloseAdapter)(pData);
}



#ifdef  SAFERELEASE
#undef  SAFERELEASE
#endif
#define SAFERELEASE(x) \
    { \
        if( NULL != (x)) \
        { \
            (x)->Release(); \
            (x) = NULL; \
        } \
    }

#ifdef SAFEDELETES
#undef SAFEDELETES
#endif
#define SAFEDELETES(x) \
    { \
        if( NULL != (x)) \
        { \
            delete (x); \
            (x) = NULL; \
        } \
    }

#ifdef SAFEDELETE
#undef SAFEDELETE
#endif SAFEDELETE
#define SAFEDELETE(x) \
    { \
        if( NULL != (x)) \
        { \
            delete[] (x); \
            (x) = NULL; \
        } \
    }

#ifdef  SAFEFREE
#undef  SAFEFREE
#endif
#define SAFEFREE(x) \
    { \
        if( NULL != (x)) \
        { \
            free (x); \
            (x) = NULL; \
        } \
    }

#define SAFECLOSE(x) \
    { \
        if( NULL != (x)) \
        { \
            fclose(x); \
            (x) = NULL; \
        } \
    }

#define SAFECOTASKMEMFREE(x) \
    { \
        if( NULL != (x)) \
        { \
            CoTaskMemFree(x); \
            (x) = NULL; \
        } \
    } \

enum eDXVA9or11
{
    DXVA_9Only  = 0x00000001,
    DXVA_11Only = 0x00000002,
    DXVA_Both   = DXVA_9Only | DXVA_11Only,
};

enum eDXVAOffloadMode
{
    OFFLOAD_UNSPECIFIED = -1,
    IDCT_SUPPORTED      = 0,
    MOCOMP_SUPPORTED,
    VLD_SUPPORTED,
    // insert more here
    NUM_DXVA_MODES
};

enum eSupportedVideoFormats
{
    H264 = 0,
    MPEG2,
    WMV1,
    WMV2,
    WMV3,
    WVC1,
    HEVC,
    HEVC10
};

class CDecodeCallbackContext
{
public:
    ILogger*    pLogger;
    DWORD       dwVideoFormat;
    DWORD       dwDXVAOffloadMode;
    BOOL        bFailureAcceptable;
    BOOL        bAESSupported;
    WCHAR*      pwszFailureReason;

    CDecodeCallbackContext(void)
    {
        pLogger             = NULL;
        dwDXVAOffloadMode   = OFFLOAD_UNSPECIFIED;
        bFailureAcceptable  = FALSE;
        bAESSupported       = FALSE;
        pwszFailureReason   = NULL;
    }

    ~CDecodeCallbackContext(void)
    {
        SAFEFREE(pwszFailureReason);
    }
};



HRESULT SLGetWindowsInformationDWORDPrivate(_In_  PCWSTR pwszValueName,
    _Out_ DWORD  *pdwValue)
{

    typedef HRESULT(WINAPI* LPFUNCTION)(PCWSTR, DWORD*);

    HINSTANCE hlibSlc = LoadLibraryA("Slc.dll");
    if (hlibSlc == NULL)
    {
        return E_NOINTERFACE;
    }

    LPFUNCTION lpfnSLGetWindowsInformationDWORD = (LPFUNCTION)GetProcAddress(
        hlibSlc, "SLGetWindowsInformationDWORD");
    if (lpfnSLGetWindowsInformationDWORD == NULL)
    {
        return E_NOINTERFACE;
    }

    return(*lpfnSLGetWindowsInformationDWORD)(pwszValueName, pdwValue);

}


BOOL                    g_bVerifyEncryption     = TRUE;
BOOL                    g_bDisableEncryption    = FALSE;
MFTOPOLOGY_DXVA_MODE    g_dxvamodeDefault       = MFTOPOLOGY_DXVA_FULL;





HRESULT VerifyMPEG2Supported(
    ILogger*    pLogger)
{
    HRESULT hr          = S_OK;
    DWORD   dwEnabled   = 0;

    do
    {
        if(pLogger == NULL)
        {
            hr = E_POINTER;
            break;
        }

        hr = AreDvdCodecsEnabled();

        if (FAILED(hr))
        {
            hr = SLGetWindowsInformationDWORDPrivate(
                MPEG2_DECODE_INSKU,  // name of policy defined in *_licensing.h
                &dwEnabled
                );

            if (dwEnabled != 0)
            {
                // MPEG2 is enabled in this SKU
                break;
            }

            LogTC(pLogger, HIGH, L"INFO>> MPEG2 is not enabled in this SKU of Windows. These tests require a SKU of windows with MPEG2 enabled to run.");

            hr = S_SKIPPED;
        }
        
    }while(false);

    return hr;
}

HRESULT CreateDXGIManager(
    ILogger*                pLogger,
    UINT32*                 pun32ResetToken,
    IMFDXGIDeviceManager**  ppDXGIManager)
{
    HRESULT                 hr                      = S_OK;
    ID3D11Device*           pD3D11Device            = NULL;
    D3D_FEATURE_LEVEL       d3dflFeatureLevelUsed   = D3D_FEATURE_LEVEL_9_1;
    UINT32                  un32ResetToken          = 0;
    IMFDXGIDeviceManager*   pDXGIDeviceManager      = NULL;

    do
    {
        if( (pLogger == NULL)           ||
            (pun32ResetToken == NULL)   ||
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
            0,
            NULL,
            0,
            D3D11_SDK_VERSION,
            &pD3D11Device,
            &d3dflFeatureLevelUsed,
            NULL
            );
        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"ERROR>> Failed to create DX11 Device (hr=0x%x : %s) ", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = MFCreateDXGIDeviceManager(
            &un32ResetToken,
            &pDXGIDeviceManager
            );
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to create MFDXGIDeviceManager (hr=0x%x : %s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pDXGIDeviceManager->ResetDevice(
            pD3D11Device,
            un32ResetToken
            );
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to reset DXGI Device Manager (hr=0x%x : %s)", hr, StringFromMFHRESULT(hr));
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

HRESULT VerifyHEVCSupported(
    ILogger*    pLogger,
    DWORD eVideoFormat)
{
    HRESULT                     hr                          = S_OK;
    BOOL                        bShutdownMF                 = FALSE;
    UINT32                      un32ResetToken              = 0;
    UINT32                      un32NumDecoderProfiles      = 0;
    IMFDXGIDeviceManager*       pDXGIDeviceManager          = NULL;
    ID3D11VideoDevice*          pD3D11VideoDevice           = NULL;
    HANDLE                      hDevice                     = NULL;
    GUID                        guidDecoder                 = GUID_NULL;
    BOOL                        bSupportsHEVC               = FALSE;

    do
    {
        if(pLogger == NULL)
        {
            hr = E_POINTER;
            break;
        }

        hr = MFStartup(
            MF_VERSION
            );
        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"ERROR>> Failed to startup MF (hr=0x%x : %s )", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = CreateDXGIManager(
            pLogger,
            &un32ResetToken,
            &pDXGIDeviceManager
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pDXGIDeviceManager->OpenDeviceHandle(
            &hDevice
            );
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to get device handle from DXGIManager (hr=0x%x: %s )", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pDXGIDeviceManager->GetVideoService(
            hDevice,
            __uuidof(ID3D11VideoDevice),
            (void**)&pD3D11VideoDevice
            );
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to get ID3D11VideoDevice DXGIManager (hr=0x%x: %s )", hr, StringFromMFHRESULT(hr));
            break;
        }

        un32NumDecoderProfiles = pD3D11VideoDevice->GetVideoDecoderProfileCount();
        
        for(UINT32 un32CurrProfile = 0; un32CurrProfile < un32NumDecoderProfiles; un32CurrProfile++)
        {
            hr = pD3D11VideoDevice->GetVideoDecoderProfile(
                un32CurrProfile,
                &guidDecoder
                );
            if(FAILED(hr))
            {
                break;
            }

            if (
                ((guidDecoder == D3D11_DECODER_PROFILE_HEVC_VLD_MAIN) && (eVideoFormat == HEVC)) ||
                ((guidDecoder == D3D11_DECODER_PROFILE_HEVC_VLD_MAIN10) && (eVideoFormat == HEVC10))
                )
            {
                bSupportsHEVC   = TRUE;
            }
        }

        if(FAILED(hr))
        {
            break;
        }

        if(bSupportsHEVC == FALSE)
        {
            hr = S_FALSE;
            break;
        }
    }while(false);

    if(hDevice != NULL)
    {
        pDXGIDeviceManager->CloseDeviceHandle(
            hDevice
            );
        hDevice = NULL;
    }

    SAFERELEASE(pD3D11VideoDevice);
    SAFERELEASE(pDXGIDeviceManager);

    if(bShutdownMF != FALSE)
    {
        MFShutdown();
    }

    return hr;
}

HRESULT VerifyEncryption(
    ILogger*                    pLogger,
    DTV_DECODER_PLAYBACK_MODES* pDecSupportedModes)
{
    HRESULT hr          = S_OK;

    do
    {
        if((pLogger == NULL) || (pDecSupportedModes == NULL))
        {
            hr = E_POINTER;
            break;
        }

#if defined(_ARM_) || defined(_ARM64_)
        // Encryption is not required on this platform
        break;
#endif        

        if(pDecSupportedModes->bDXVAVLDMpeg2     != FALSE)
        {
            if(pDecSupportedModes->eDXVAVLDMpeg2EncryptLevel    != ENCRYPTION_LEVEL_STANDARD_AES)
            {
                pLogger->Log(HIGH, L"ERROR>> MPEG2 VLD does not support standard AES (supports %u)", (DWORD)pDecSupportedModes->eDXVAVLDMpeg2EncryptLevel);
                hr = E_FAIL;
                break;
            }
        }
        else
        {
            if(pDecSupportedModes->bDXVAIDCTMpeg2   != FALSE)
            {
                if(pDecSupportedModes->eDXVAIDCTMpeg2EncryptLevel   != ENCRYPTION_LEVEL_STANDARD_AES)
                {
                    pLogger->Log(HIGH, L"ERROR>> MPEG2 IDCT does not support standard AES (supports %u)", (DWORD)pDecSupportedModes->eDXVAIDCTMpeg2EncryptLevel);
                    hr = E_FAIL;
                    break;
                }
            }
        }       

        if(pDecSupportedModes->bDXVAVLDH264     != FALSE)
        {
            if(pDecSupportedModes->eDXVAVLDH264EncryptLevel     != ENCRYPTION_LEVEL_STANDARD_AES)
            {
                pLogger->Log(HIGH, L"ERROR>> H264 VLD does not support standard AES (supports %u)", (DWORD)pDecSupportedModes->eDXVAVLDH264EncryptLevel);
                hr = E_FAIL;
                break;
            }
        }
        else
        {
            if(pDecSupportedModes->bDXVAIDCTH264    != FALSE)
            {
                if(pDecSupportedModes->eDXVAIDCTH264EncryptLevel    != ENCRYPTION_LEVEL_STANDARD_AES)
                {
                    pLogger->Log(HIGH, L"ERROR>> H264 IDCT does not support standard AES (supports %u)", (DWORD)pDecSupportedModes->eDXVAIDCTH264EncryptLevel);
                    hr = E_FAIL;
                    break;
                }
            }
        }
    }while(false);

    return hr;
}

HRESULT VerifyDecodingModeAndEncryption(
    IMFTransform*           pDecoderMFT,
    CDecodeCallbackContext* pContext)
{
    HRESULT                     hr                              = S_OK;
    ICodecAPI*                  pCodecAPI                       = NULL;
    VARIANT                     varSupportedDecoderDXVAModes    = {0};
    DTV_DECODER_PLAYBACK_MODES* pSupportedDXVAModes             = NULL;

    do
    {
        if( (pDecoderMFT == NULL)   ||
            (pContext == NULL)      )
        {
            hr = E_POINTER;
            break;
        }

        hr = pDecoderMFT->QueryInterface(
            IID_ICodecAPI,
            (void**)&pCodecAPI
            );
        if(FAILED(hr))
        {
            if(pContext->dwVideoFormat == HEVC)
            {
                // HEVC does not support this API
                pContext->bAESSupported = TRUE;
                hr = S_OK;
            }
            else
            {
                pContext->pLogger->Log(HIGH, L"Error: VerifyDecodingModeAndEncryption() QI to IID_ICodecAPI failed 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            }
            break;
        }

        hr = pCodecAPI->GetValue(
            &CODECAPI_DecodingModesSupported,
            &varSupportedDecoderDXVAModes
            );
        if(FAILED(hr))
        {
			pContext->pLogger->Log( HIGH, L"Error:  VerifyDecodingModeAndEncryption() Failed to obtain the supported Decoding modes 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        pSupportedDXVAModes = (DTV_DECODER_PLAYBACK_MODES*)(varSupportedDecoderDXVAModes.pcVal);

        switch(pContext->dwVideoFormat)
        {
        case H264:
            {
                switch(pContext->dwDXVAOffloadMode)
                {
                case IDCT_SUPPORTED:
                    {
                        if(pSupportedDXVAModes->bDXVAIDCTH264 == FALSE)
                        {
                            // This is not required as per the HCK 2.0 spec
							pContext->pLogger->Log( NORMAL, L"INFO>> H264 IDCT Mode is not supported. This is not a requirement as per the HCK 2.0 Spec ");
                            hr = S_FALSE;
                            break;
                        }
                    }
                    break;
                case MOCOMP_SUPPORTED:
                    {
#if !defined(_ARM_) && !defined(_ARM64_)
                        if(pSupportedDXVAModes->bDXVAVLDH264 != FALSE)
                        {
                            // If H264 VLD is supported, H264 MoComp is not required as per the HCK 2.0 spec
                            if(pSupportedDXVAModes->bDXVAMoCompH264 == FALSE)
                            {
                                pContext->pLogger->Log( NORMAL, L"INFO>> If H264 VLD is supported, H264 MoComp is not required as per the HCK 2.0 spec ");
								hr = S_FALSE;
                                break;
                            }
                        }
                        else if(pSupportedDXVAModes->bDXVAMoCompH264 == FALSE)
                        {
                            // If H264 VLD is not supported, H264 MoComp is required as per the HCK 2.0 spec
                            SAFEFREE(pContext->pwszFailureReason);
                            pContext->pwszFailureReason = _wcsdup(L"If H264 VLD is not supported, H264 MoComp must be supported");
                            hr = E_FAIL;
                            break;
                        }
#else
                        // For WoA Systems, MoComp is not required
                        if(pSupportedDXVAModes->bDXVAMoCompH264 == FALSE)
                        {
							pContext->pLogger->Log( NORMAL, L"INFO>> H264 MoComp is not required for WoA systems");
                            hr = S_FALSE;
                            break;
                        }
#endif
                    }
                    break;
                case VLD_SUPPORTED:
                    {
#if !defined(_ARM_) && !defined(_ARM64_)
                        if(pSupportedDXVAModes->bDXVAMoCompH264 != FALSE)
                        {
                            // If H264 MoComp is supported, H264 VLD is not required as per the HCK 2.0 spec
                            if(pSupportedDXVAModes->bDXVAVLDH264 == FALSE)
                            {
                                pContext->pLogger->Log( NORMAL, L"INFO>> If H264 MoComp is supported, H264 VLD is not required as per the HCK 2.0 spec ");
								hr = S_FALSE;
                                break;
                            }
                        }
                        else if(pSupportedDXVAModes->bDXVAVLDH264 == FALSE)
                        {
                            // If H264 MoComp is not supported, H264 VLD is required as per the HCK 2.0 spec
                            SAFEFREE(pContext->pwszFailureReason);
                            pContext->pwszFailureReason = _wcsdup(L"If H264 MoComp is not supported, H264 VLD must be supported");
                            hr = E_FAIL;
                            break;
                        }
#else
                        // For WoA Systems, VLD is required
                        if(pSupportedDXVAModes->bDXVAVLDH264 == FALSE)
                        {
                             SAFEFREE(pContext->pwszFailureReason);
                            pContext->pwszFailureReason = _wcsdup(L"H264 VLD must be supported");
                            hr = E_FAIL;
                            break;
                        }
#endif
                    }
                    break;
                }
            }
            break;
        case MPEG2:
            {
                switch(pContext->dwDXVAOffloadMode)
                {
                case IDCT_SUPPORTED:
                    {
#if !defined(_ARM_) && !defined(_ARM64_)
                        if(pSupportedDXVAModes->bDXVAIDCTMpeg2 == FALSE)
                        {
                            // If IDCT is not supported, MoComp or VLD must be supported as per the HCK 2.0 spec
                            if(pSupportedDXVAModes->bDXVAMoCompMpeg2 != FALSE)
                            {
                                hr = S_OK;
								pContext->pLogger->Log( NORMAL, L"INFO>> MPEG2 IDCT is not supported, but MoComp is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else if(pSupportedDXVAModes->bDXVAVLDMpeg2 != FALSE)
                            {
                                hr = S_OK;
								pContext->pLogger->Log( NORMAL, L"INFO>> MPEG2 IDCT is not supported, but VLD is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else
                            {
                                SAFEFREE(pContext->pwszFailureReason);
                                pContext->pwszFailureReason = _wcsdup(L"If MPEG2 IDCT is not supported, MPEG2 VLD or MoComp must be supported");
                                hr = E_FAIL;
                                break;
                            }
                        }
#else
                        // MPEG2 support is not required on WoA
                        if(pSupportedDXVAModes->bDXVAIDCTMpeg2 == FALSE)
                        {
							pContext->pLogger->Log( NORMAL, L"INFO>> MPEG2 support is not required on WoA ");
                            hr = S_FALSE;
                            break;
                        }
#endif
                    }
                    break;
                case MOCOMP_SUPPORTED:
                    {
#if !defined(_ARM_) && !defined(_ARM64_)
                        if(pSupportedDXVAModes->bDXVAMoCompMpeg2 == FALSE)
                        {
                            // If MoComp is not supported, IDCT or VLD must be supported as per the HCK 2.0 spec
                            if(pSupportedDXVAModes->bDXVAIDCTMpeg2 != FALSE)
                            {
                                hr = S_OK;
								pContext->pLogger->Log( NORMAL, L"INFO>> MPEG2 MoComp is not supported, but IDCT is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else if(pSupportedDXVAModes->bDXVAVLDMpeg2 != FALSE)
                            {
                                hr = S_OK;
								pContext->pLogger->Log( NORMAL, L"INFO>> MPEG2 MoComp is not supported, but VLD is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else
                            {
                                SAFEFREE(pContext->pwszFailureReason);
                                pContext->pwszFailureReason = _wcsdup(L"If MPEG2 MoComp is not supported, MPEG2 VLD or IDCT must be supported");
                                hr = E_FAIL;
                                break;
                            }
                        }
#else
                        // MPEG2 support is not required on WoA
                        if(pSupportedDXVAModes->bDXVAMoCompMpeg2 == FALSE)
                        {
                            pContext->pLogger->Log( NORMAL, L"INFO>> MPEG2 support is not required on WoA ");
							hr = S_FALSE;
                            break;
                        }
#endif
                    }
                    break;
                case VLD_SUPPORTED:
                    {
#if !defined(_ARM_) && !defined(_ARM64_)
                        if(pSupportedDXVAModes->bDXVAVLDMpeg2 == FALSE)
                        {
                            // If VLD is not supported, IDCT or MoComp must be supported as per the HCK 2.0 spec
                            if(pSupportedDXVAModes->bDXVAIDCTMpeg2 != FALSE)
                            {
                                pContext->pLogger->Log(NORMAL, L"INFO>> MPEG2 VLD is not supported, but IDCT is supported as per the HCK 2.0 Spec ");
                                hr = S_OK;
                                break;
                            }
                            else if(pSupportedDXVAModes->bDXVAMoCompMpeg2 != FALSE)
                            {
                                hr = S_OK;
								pContext->pLogger->Log( NORMAL, L"INFO>> MPEG2 VLD is not supported, but MoComp is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else
                            {
                                SAFEFREE(pContext->pwszFailureReason);
                                pContext->pwszFailureReason = _wcsdup(L"If MPEG2 VLD is not supported, MPEG2 IDCT or MoComp must be supported");
                                hr = E_FAIL;
                                break;
                            }
                        }
#else
                        // MPEG2 support is not required on WoA
                        if(pSupportedDXVAModes->bDXVAVLDMpeg2 == FALSE)
                        {
                            pContext->pLogger->Log( NORMAL, L"INFO>> MPEG2 support is not required on WoA ");
							hr = S_FALSE;
                            break;
                        }
#endif
                    }
                    break;
                }
            }
            break;
        case WMV1:
        case WMV2:
        case WMV3:
        case WVC1:
            {
                switch(pContext->dwDXVAOffloadMode)
                {
                case IDCT_SUPPORTED:
                    {
#if !defined(_ARM_) && !defined(_ARM64_)
                        if(pSupportedDXVAModes->bDXVAIDCTVC1 == FALSE)
                        {
                            // If VC1 IDCT is not supported, MoComp or VLD must be supported as per the HCK 2.0 spec
                            if(pSupportedDXVAModes->bDXVAMoCompVC1 != FALSE)
                            {
								hr = S_FALSE;
								pContext->pLogger->Log( NORMAL, L"INFO>> VC1 IDCT is not supported, but MoComp is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else if(pSupportedDXVAModes->bDXVAVLDVC1 != FALSE)
                            {
                                hr = S_FALSE;
								pContext->pLogger->Log( NORMAL, L"INFO>> VC1 IDCT is not supported, but VLD is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else
                            {
                                SAFEFREE(pContext->pwszFailureReason);
                                pContext->pwszFailureReason = _wcsdup(L"If VC1 iDCT is not supported, VC1 VLD or MoComp must be supported");
                                hr = E_FAIL;
                                break;
                            }
                        }
#else
                        // For WoA Systems, IDCT is not required
                        if(pSupportedDXVAModes->bDXVAIDCTVC1 == FALSE)
                        {
                            pContext->pLogger->Log( NORMAL, L"INFO>> VC1 IDCT support is not required on WoA ");
							hr = S_FALSE;
                            break;
                        }
#endif
                    }
                    break;
                case MOCOMP_SUPPORTED:
                    {
#if !defined(_ARM_) && !defined(_ARM64_)
                        if(pSupportedDXVAModes->bDXVAMoCompVC1 == FALSE)
                        {
                            // If VC1 MoComp is not supported, IDCT or VLD must be supported as per the HCK 2.0 spec
                            if(pSupportedDXVAModes->bDXVAIDCTVC1 != FALSE)
                            {
                                hr = S_FALSE;
								pContext->pLogger->Log( NORMAL, L"INFO>> VC1 MoComp is not supported, but IDCT is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else if(pSupportedDXVAModes->bDXVAVLDVC1 != FALSE)
                            {
                                hr = S_FALSE;
								pContext->pLogger->Log( NORMAL, L"INFO>> VC1 MoComp is not supported, but VLD is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else
                            {
                                SAFEFREE(pContext->pwszFailureReason);
                                pContext->pwszFailureReason = _wcsdup(L"If VC1 MoComp is not supported, VC1 VLD or iDCT must be supported");
                                hr = E_FAIL;
                                break;
                            }
                        }
#else
                        // For WoA Systems, MoComp is not required
                        if(pSupportedDXVAModes->bDXVAMoCompVC1 == FALSE)
                        {
                            pContext->pLogger->Log( NORMAL, L"INFO>> VC1 MoComp support is not required on WoA ");
							hr = S_FALSE;
                            break;
                        }
#endif
                    }
                    break;
                case VLD_SUPPORTED:
                    {
#if !defined(_ARM_) && !defined(_ARM64_)
                        if(pSupportedDXVAModes->bDXVAVLDVC1 == FALSE)
                        {
                            // If VC1 VLD is not supported, IDCT or MoComp must be supported as per the HCK 2.0 spec
                            if(pSupportedDXVAModes->bDXVAIDCTVC1 != FALSE)
                            {
                                hr = S_FALSE;
								pContext->pLogger->Log( NORMAL, L"INFO>> VC1 VLD is not supported, but IDCT is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else if(pSupportedDXVAModes->bDXVAMoCompVC1 != FALSE)
                            {
                                hr = S_FALSE;
								pContext->pLogger->Log( NORMAL, L"INFO>> VC1 VLD is not supported, but MoComp is supported as per the HCK 2.0 Spec ");
                                break;
                            }
                            else
                            {
                                SAFEFREE(pContext->pwszFailureReason);
                                pContext->pwszFailureReason = _wcsdup(L"If VC1 VLD is not supported, VC1 iDCT or MoComp must be supported");
                                hr = E_FAIL;
                                break;
                            }
                        }
#else
                        // For WoA Systems, VLD is required
                        if(pSupportedDXVAModes->bDXVAVLDVC1 == FALSE)
                        {
                            SAFEFREE(pContext->pwszFailureReason);
                            pContext->pwszFailureReason = _wcsdup(L"WMV VLD must be supported");
                            hr = E_FAIL;
                            break;
                        }
#endif
                    }
                    break;
                }
            }
            break;
        };

        if(hr != S_OK)
        {
            break;
        }

        if(g_bVerifyEncryption == FALSE)
        {
            pContext->pLogger->Log(NORMAL, L"INFO>> Not Verifying Encryption");
            break;
        }

        if(SUCCEEDED(VerifyEncryption(
            pContext->pLogger,
            pSupportedDXVAModes
            )))
        {
            pContext->bAESSupported = TRUE;
        }
    }while(false);

    SAFERELEASE(pCodecAPI);
    VariantClear(&varSupportedDecoderDXVAModes);

    return hr;
}

HRESULT SetupDecodingModeAndEncryption(
    IMFTransform*           pDecoderMFT,
    CDecodeCallbackContext* pContext)
{
    HRESULT                     hr                              = S_OK;
    ICodecAPI*                  pCodecAPI                       = NULL;
    VARIANT                     varSupportedDecoderDXVAModes    = {0};
    VARIANT                     varDXVAOffloadMode              = {0};
    DTV_DECODER_PLAYBACK_MODES* pDecoderReportedDXVAModes       = NULL;
    DTV_DECODER_PLAYBACK_MODES* pDXVAModesToUse                 = NULL;
    ENCRYPTION_LEVEL            elEncryptionUsed                = ENCRYPTION_LEVEL_NO_ENCRYPT;

    do
    {
        hr = pDecoderMFT->QueryInterface(
            IID_ICodecAPI,
            (void**)&pCodecAPI
            );
        if(FAILED(hr))
        {
            if(pContext->dwVideoFormat == HEVC)
            {
                // HEVC does not support this API
                hr = S_OK;
            }
            else
            {
                pContext->pLogger->Log(HIGH, L"Error: SetupDecodingModeAndEncryption() QI to IID_ICodecAPI failed 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            }			
            break;
        }

        hr = pCodecAPI->GetValue(
            &CODECAPI_DecodingModesSupported,
            &varSupportedDecoderDXVAModes
            );
        if(FAILED(hr))
        {
			pContext->pLogger->Log( HIGH, L"Error: SetupDecodingModeAndEncryption() Failed to obtain the supported Decoding modes 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        varDXVAOffloadMode.vt       = VT_BYREF|VT_I1;
        varDXVAOffloadMode.pcVal    = (CHAR*)CoTaskMemAlloc(sizeof(DTV_DECODER_PLAYBACK_MODES));
        if(varDXVAOffloadMode.pcVal == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        memset(
            (void*)varDXVAOffloadMode.pcVal,
            0,
            sizeof(DTV_DECODER_PLAYBACK_MODES)
            );

        pDecoderReportedDXVAModes   = (DTV_DECODER_PLAYBACK_MODES*)(varSupportedDecoderDXVAModes.pcVal);
        pDXVAModesToUse             = (DTV_DECODER_PLAYBACK_MODES*)(varDXVAOffloadMode.pcVal);

        switch(pContext->dwDXVAOffloadMode)
        {
        case IDCT_SUPPORTED:
            {
                switch(pContext->dwVideoFormat)
                {
                case H264:
                    {
                        pDXVAModesToUse->bDXVAIDCTH264              = TRUE;
                        pDXVAModesToUse->eDXVAIDCTH264EncryptLevel  = pDecoderReportedDXVAModes->eDXVAIDCTH264EncryptLevel;
                        elEncryptionUsed                            = pDXVAModesToUse->eDXVAIDCTH264EncryptLevel;
                    }
                    break;
                case MPEG2:
                    {
                        pDXVAModesToUse->bDXVAIDCTH264              = TRUE;
                        pDXVAModesToUse->eDXVAIDCTMpeg2EncryptLevel = pDecoderReportedDXVAModes->eDXVAIDCTMpeg2EncryptLevel;
                        elEncryptionUsed                            = pDXVAModesToUse->eDXVAIDCTMpeg2EncryptLevel;
                    }
                    break;
                case WMV1:
                case WMV2:
                case WMV3:
                case WVC1:
                    {
                        pDXVAModesToUse->bDXVAIDCTVC1               = TRUE;
                        pDXVAModesToUse->eDXVAIDCTVC1EncryptLevel   = pDecoderReportedDXVAModes->eDXVAIDCTVC1EncryptLevel;
                        elEncryptionUsed                            = pDXVAModesToUse->eDXVAIDCTVC1EncryptLevel;
                    }
                    break;
                default:
                    {
                        hr = E_UNEXPECTED;
                        break;
                    }
                    break;
                };

                if(FAILED(hr))
                {
					pContext->pLogger->Log( HIGH, L"Error: Failed to set IDCT DXVA offload mode and encryption level on the decoders 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                    break;
                }
            }
            break;
        case MOCOMP_SUPPORTED:
            {
                switch(pContext->dwVideoFormat)
                {
                case H264:
                    {
                        pDXVAModesToUse->bDXVAMoCompH264                = TRUE;
                        pDXVAModesToUse->eDXVAMoCompH264EncryptLevel    = pDecoderReportedDXVAModes->eDXVAMoCompH264EncryptLevel;
                        elEncryptionUsed                                = pDXVAModesToUse->eDXVAMoCompH264EncryptLevel;
                    }
                    break;
                case MPEG2:
                    {
                        pDXVAModesToUse->bDXVAMoCompH264                = TRUE;
                        pDXVAModesToUse->eDXVAMoCompMpeg2EncryptLevel   = pDecoderReportedDXVAModes->eDXVAMoCompMpeg2EncryptLevel;
                        elEncryptionUsed                                = pDXVAModesToUse->eDXVAMoCompMpeg2EncryptLevel;
                    }
                    break;
                case WMV1:
                case WMV2:
                case WMV3:
                case WVC1:
                    {
                        pDXVAModesToUse->bDXVAMoCompVC1                 = TRUE;
                        pDXVAModesToUse->eDXVAMoCompVC1EncryptLevel     = pDecoderReportedDXVAModes->eDXVAMoCompVC1EncryptLevel;
                        elEncryptionUsed                                = pDXVAModesToUse->eDXVAMoCompVC1EncryptLevel;
                    }
                    break;
                default:
                    {
                        hr = E_UNEXPECTED;
                        break;
                    }
                    break;
                };

                if(FAILED(hr))
                {
					pContext->pLogger->Log( HIGH, L"Error: Failed to set MoComp DXVA offload mode and encryption level on the decoders 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                    break;
                }
            }
            break;
        case VLD_SUPPORTED:
            {
                switch(pContext->dwVideoFormat)
                {
                case H264:
                    {
                        pDXVAModesToUse->bDXVAVLDH264               = TRUE;
                        pDXVAModesToUse->eDXVAVLDH264EncryptLevel   = pDecoderReportedDXVAModes->eDXVAVLDH264EncryptLevel;
                        elEncryptionUsed                            = pDXVAModesToUse->eDXVAVLDH264EncryptLevel;
                    }
                    break;
                case MPEG2:
                    {
                        pDXVAModesToUse->bDXVAVLDH264               = TRUE;
                        pDXVAModesToUse->eDXVAVLDMpeg2EncryptLevel  = pDecoderReportedDXVAModes->eDXVAVLDMpeg2EncryptLevel;
                        elEncryptionUsed                            = pDXVAModesToUse->eDXVAVLDMpeg2EncryptLevel;
                    }
                    break;
                case WMV1:
                case WMV2:
                case WMV3:
                case WVC1:
                    {
                        pDXVAModesToUse->bDXVAVLDVC1                = TRUE;
                        pDXVAModesToUse->eDXVAVLDVC1EncryptLevel    = pDecoderReportedDXVAModes->eDXVAVLDVC1EncryptLevel;
                        elEncryptionUsed                            = pDXVAModesToUse->eDXVAVLDVC1EncryptLevel;
                    }
                    break;
                default:
                    {
                        hr = E_UNEXPECTED;
                        break;
                    }
                    break;
                };

                if(FAILED(hr))
                {
					pContext->pLogger->Log( HIGH, L"Error: Failed to set VLD DXVA offload mode and encryption level on the decoders 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                    break;
                }
            }
            break;
        default:
            {
                hr = E_UNEXPECTED;
                break;
            }
            break;
        };

        if(FAILED(hr))
        {
			pContext->pLogger->Log( HIGH, L"Error: Failed to set any of the available DXVA offload modes and encryption level on any of the available decoders 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        switch(elEncryptionUsed)
        {
        case ENCRYPTION_LEVEL_NOT_ALLOWED:
            {
                pContext->pLogger->Log(NORMAL, L"Info: Encryption Mode - Not Allowed");
            }
            break;
        case ENCRYPTION_LEVEL_NO_ENCRYPT:
            {
                pContext->pLogger->Log(NORMAL, L"Info: Encryption Mode - None");
            }
            break;
        case ENCRYPTION_LEVEL_PRIVATE_ENCRYPT:
            {
                pContext->pLogger->Log(NORMAL, L"Info: Encryption Mode - Private");
            }
            break;
        case ENCRYPTION_LEVEL_AES:
            {
                pContext->pLogger->Log(NORMAL, L"Info: Encryption Mode - AES");
            }
            break;
        case ENCRYPTION_LEVEL_STANDARD_AES:
            {
                pContext->pLogger->Log(NORMAL, L"Info: Encryption Mode - Standard AES");
            }
            break;
        default:
            {
                pContext->pLogger->Log(NORMAL, L"Info: Encryption Mode - Unknown Value (%u)", (DWORD)elEncryptionUsed);
            }
            break;
        };

        if(g_bDisableEncryption != FALSE)
        {
            pContext->pLogger->Log(NORMAL, L"INFO>> Disabling encryption, encryption will not be used");
            pDXVAModesToUse->eDXVAIDCTH264EncryptLevel      = ENCRYPTION_LEVEL_NO_ENCRYPT;
            pDXVAModesToUse->eDXVAIDCTMpeg2EncryptLevel     = ENCRYPTION_LEVEL_NO_ENCRYPT;
            pDXVAModesToUse->eDXVAIDCTVC1EncryptLevel       = ENCRYPTION_LEVEL_NO_ENCRYPT;
            pDXVAModesToUse->eDXVAMoCompH264EncryptLevel    = ENCRYPTION_LEVEL_NO_ENCRYPT;
            pDXVAModesToUse->eDXVAMoCompMpeg2EncryptLevel   = ENCRYPTION_LEVEL_NO_ENCRYPT;
            pDXVAModesToUse->eDXVAMoCompVC1EncryptLevel     = ENCRYPTION_LEVEL_NO_ENCRYPT;
            pDXVAModesToUse->eDXVAVLDH264EncryptLevel       = ENCRYPTION_LEVEL_NO_ENCRYPT;
            pDXVAModesToUse->eDXVAVLDMpeg2EncryptLevel      = ENCRYPTION_LEVEL_NO_ENCRYPT;
            pDXVAModesToUse->eDXVAVLDVC1EncryptLevel        = ENCRYPTION_LEVEL_NO_ENCRYPT;
        }

        hr = pCodecAPI->SetValue(
            &CODECAPI_DecodingModesToBeUsed,
            &varDXVAOffloadMode
            );
        if(FAILED(hr))
        {
			pContext->pLogger->Log( HIGH, L"Error: Failed to set the decoding modes to be used (DXVA offload mode & encryption level) 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }
    }while(false);

    SAFERELEASE(pCodecAPI);
    VariantClear(&varSupportedDecoderDXVAModes);
    VariantClear(&varDXVAOffloadMode);

    return hr;
}

HRESULT SetupDecoderForDXVAMode(
    IMFTransform*   pDecoderMFT,
    void*           pContext)
{
    HRESULT                 hr              = S_OK;
    CDecodeCallbackContext* pDecoderContext = (CDecodeCallbackContext*)pContext;

    do
    {
        if( (pDecoderMFT == NULL)       ||
            (pDecoderContext == NULL)   )
        {
            hr = E_POINTER;
            break;
        }

        hr = VerifyDecodingModeAndEncryption(
            pDecoderMFT,
            pDecoderContext
            );
        if(hr == S_FALSE)
        {
            // Not supported
            pDecoderContext->bFailureAcceptable = TRUE;
            hr = E_FAIL;
            break;
        }
        if(FAILED(hr))
        {
			pDecoderContext->pLogger->Log( HIGH, L"Error: SetupDecoderForDXVAMode() Failed to verify the available decoding modes and encryption levels 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = SetupDecodingModeAndEncryption(
            pDecoderMFT,
            pDecoderContext
            );
        if(FAILED(hr))
        {
			pDecoderContext->pLogger->Log( HIGH, L"Error: SetupDecoderForDXVAMode() Failed to set decoding mode and encryption levels 0%08X (%s)",  hr, StringFromMFHRESULT(hr));
            break;
        }
    }while(false);

    return hr;
}

#pragma prefast(push)
#pragma prefast(disable:28718, "disabling warning for test code")


BOOL SetupDiDestroyDeviceInfoListPrivate(
    _In_ HDEVINFO DeviceInfoSet
    )
{
    typedef BOOL(WINAPI* LPFUNCTION)(_In_ HDEVINFO DeviceInfoSet);

    HINSTANCE hlibSetupapi = LoadLibraryA("Setupapi.DLL");
    if (hlibSetupapi == NULL)
    {
        return FALSE;
    }

    LPFUNCTION lpfnSetupDiDestroyDeviceInfoList = (LPFUNCTION)GetProcAddress(
        hlibSetupapi, "SetupDiDestroyDeviceInfoList");
    if (lpfnSetupDiDestroyDeviceInfoList == NULL)
    {
        return FALSE;
    }

    return(*lpfnSetupDiDestroyDeviceInfoList)(DeviceInfoSet);
}



BOOL SetupDiEnumDeviceInterfacesPrivate(
    _In_           HDEVINFO                  DeviceInfoSet,
    _In_opt_       PSP_DEVINFO_DATA          DeviceInfoData,
    _In_     const GUID                      *InterfaceClassGuid,
    _In_           DWORD                     MemberIndex,
    _Out_          PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData
)
{
    typedef BOOL(WINAPI* LPFUNCTION)(_In_           HDEVINFO                  DeviceInfoSet,
        _In_opt_       PSP_DEVINFO_DATA          DeviceInfoData,
        _In_     const GUID                      *InterfaceClassGuid,
        _In_           DWORD                     MemberIndex,
        _Out_          PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);

    HINSTANCE hlibSetupapi = LoadLibraryA("Setupapi.DLL");
    if (hlibSetupapi == NULL)
    {
        return FALSE;
    }

    LPFUNCTION lpfnSetupDiEnumDeviceInterfaces = (LPFUNCTION)GetProcAddress(
        hlibSetupapi, "SetupDiEnumDeviceInterfaces");
    if (lpfnSetupDiEnumDeviceInterfaces == NULL)
    {
        return FALSE;
    }

    return(*lpfnSetupDiEnumDeviceInterfaces)(DeviceInfoSet, DeviceInfoData, InterfaceClassGuid, MemberIndex, DeviceInterfaceData);
}



BOOL SetupDiGetDeviceInterfaceDetailPrivate(
    _In_      HDEVINFO                         DeviceInfoSet,
    _In_      PSP_DEVICE_INTERFACE_DATA        DeviceInterfaceData,
    _Out_opt_ PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData,
    _In_      DWORD                            DeviceInterfaceDetailDataSize,
    _Out_opt_ PDWORD                           RequiredSize,
    _Out_opt_ PSP_DEVINFO_DATA                 DeviceInfoData
    )
{
    typedef BOOL(WINAPI* LPFUNCTION)(_In_      HDEVINFO                         DeviceInfoSet,
        _In_      PSP_DEVICE_INTERFACE_DATA        DeviceInterfaceData,
        _Out_opt_ PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData,
        _In_      DWORD                            DeviceInterfaceDetailDataSize,
        _Out_opt_ PDWORD                           RequiredSize,
        _Out_opt_ PSP_DEVINFO_DATA                 DeviceInfoData);

    HINSTANCE hlibSetupapi = LoadLibraryA("Setupapi.DLL");
    if (hlibSetupapi == NULL)
    {
        return FALSE;
    }

    LPFUNCTION lpfnSetupDiGetDeviceInterfaceDetail  = (LPFUNCTION)GetProcAddress(
        hlibSetupapi, "SetupDiGetDeviceInterfaceDetail");
    if (lpfnSetupDiGetDeviceInterfaceDetail  == NULL)
    {
        return FALSE;
    }

    return(*lpfnSetupDiGetDeviceInterfaceDetail)(DeviceInfoSet, DeviceInterfaceData, DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, RequiredSize, DeviceInfoData);
}



HDEVINFO  SetupDiGetClassDevsPrivate(
    _In_opt_ const GUID   *ClassGuid,
    _In_opt_       PCTSTR Enumerator,
    _In_opt_       HWND   hwndParent,
    _In_           DWORD  Flags

    )
{
    typedef HDEVINFO(WINAPI* LPFUNCTION)(_In_opt_ const GUID   *ClassGuid,
        _In_opt_       PCTSTR Enumerator,
        _In_opt_       HWND   hwndParent,
        _In_           DWORD  Flags
        );

    HINSTANCE hlibSetupapi = LoadLibraryA("Setupapi.DLL");
    if (hlibSetupapi == NULL)
    {
        return NULL;
    }

    LPFUNCTION lpfnSetupDiGetClassDevs = (LPFUNCTION)GetProcAddress(
        hlibSetupapi, "SetupDiGetClassDevs");
    if (lpfnSetupDiGetClassDevs == NULL)
    {
        return NULL;
    }

    return(*lpfnSetupDiGetClassDevs)(ClassGuid, Enumerator, hwndParent, Flags);
}

HRESULT GetDevicePath(
    WCHAR***    pppwszDevPath,
    DWORD*      pdwNumDevices)
{
    HRESULT                             hr                                  = S_OK;
    HDEVINFO                            hDevInfo                            = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DATA            spDevInterfaceData                  = {0};
    DWORD                               dwDetailSize                        = 0;
    SP_DEVICE_INTERFACE_DETAIL_DATA_W*  pInterfaceDetail                    = NULL;
    SP_DEVINFO_DATA                     deviceInfoData                      = {0};
    WCHAR*                              ppwszAdapterDevices[MAX_ADAPTERS]   = {NULL};
    DWORD                               dwNumAdapterDevices                 = 0;
    
    do
    {
        if( (pppwszDevPath == NULL) ||
            (pdwNumDevices == NULL) )
        {
            hr = E_POINTER;
            break;
        }

        (*pppwszDevPath)            = NULL;

        spDevInterfaceData.cbSize   = sizeof(spDevInterfaceData);
        deviceInfoData.cbSize       = sizeof(deviceInfoData);

        hDevInfo = SetupDiGetClassDevsPrivate(
            &GUID_DISPLAY_DEVICE_ARRIVAL,
            NULL,
            NULL,
            DIGCF_DEVICEINTERFACE | DIGCF_PRESENT
            );
        if(hDevInfo == INVALID_HANDLE_VALUE)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            break;
        }

        while(true)
        {
            if(SetupDiEnumDeviceInterfacesPrivate(
                hDevInfo,
                NULL,
                &GUID_DISPLAY_DEVICE_ARRIVAL,
                dwNumAdapterDevices,
                &spDevInterfaceData
                ) == FALSE)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                if(hr == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS))
                {
                    (*pppwszDevPath)    = (WCHAR**)malloc(
                        sizeof(WCHAR*) * dwNumAdapterDevices
                        );
                    if((*pppwszDevPath) == NULL)
                    {
                        hr = E_OUTOFMEMORY;
                        break;
                    }

                    for(DWORD dwCurrAdapter = 0; dwCurrAdapter < dwNumAdapterDevices; dwCurrAdapter++)
                    {
                        (*pppwszDevPath)[dwCurrAdapter] = ppwszAdapterDevices[dwCurrAdapter];
                    }

                    (*pdwNumDevices)    = dwNumAdapterDevices;

                    hr = S_OK;
                }

                break;
            }

            SetupDiGetDeviceInterfaceDetailPrivate(
                hDevInfo,
                &spDevInterfaceData,
                NULL,
                0,
                &dwDetailSize,
                NULL);
            if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                break;
            }

#pragma prefast(push)
#pragma prefast(disable:6011, "disabling warning for test code")

            SAFEFREE(pInterfaceDetail);
            pInterfaceDetail            = (SP_DEVICE_INTERFACE_DETAIL_DATA_W*)malloc(dwDetailSize);
            ZeroMemory(
                pInterfaceDetail,
                dwDetailSize
                );

            pInterfaceDetail->cbSize    = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

#pragma prefast(pop)
            if (SetupDiGetDeviceInterfaceDetailPrivate(
                hDevInfo,
                &spDevInterfaceData,
                pInterfaceDetail,
                dwDetailSize,
                NULL,
                &deviceInfoData
                ) == FALSE)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                break;
            }

            ppwszAdapterDevices[dwNumAdapterDevices]    = _wcsdup(pInterfaceDetail->DevicePath);
            if(ppwszAdapterDevices[dwNumAdapterDevices] == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            dwNumAdapterDevices++;

            if(dwNumAdapterDevices >= MAX_ADAPTERS)
            {
                // There are more adapters on the system then expected
                hr = E_UNEXPECTED;
                break;
            }
        }
    }while(false);

    if(FAILED(hr))
    {
        for(DWORD dwCurrAdapter = 0; dwCurrAdapter < dwNumAdapterDevices; dwCurrAdapter)
        {
            SAFEFREE(ppwszAdapterDevices[dwCurrAdapter]);
        }
    }

    SAFEFREE(pInterfaceDetail);

    if(hDevInfo != NULL)
    {
        SetupDiDestroyDeviceInfoListPrivate(
            hDevInfo
            );
    }

    return hr;
}

#pragma prefast(pop)

#pragma prefast(push)
#pragma prefast(disable:6031, "disabling warning for test code")



BOOL ShouldVerifyDFlip(
    ILogger* pLogger)
{
    HRESULT                             hr                  = S_OK;
    BOOL                                bVerifyDFlip        = TRUE;
    WCHAR**                             ppwszDevicePath     = NULL;
    DWORD                               dwNumDevices        = 0;
    D3DKMT_OPENADAPTERFROMDEVICENAME    d3dDeviceInfo       = {0};
    D3DKMT_QUERYADAPTERINFO             d3dAdapterInfo      = {0};
    D3DKMT_DRIVERVERSION                d3dDriverVersion    = KMT_DRIVERVERSION_WDDM_1_0;

    do
    {
        hr = GetDevicePath(
            &ppwszDevicePath,
            &dwNumDevices
            );
        if(FAILED(hr))
        {
            if(pLogger != NULL)
            {
				LogTC(pLogger, NORMAL, L"WARNING>> Failed to find adapter device, DFlip will be verified which may cause failures on drivers <= WDDM 1.1 (hr=0x%x : %s)", hr, StringFromMFHRESULT(hr));
            }

            break;
        }

        for(DWORD dwCurrDevice = 0; dwCurrDevice < dwNumDevices; dwCurrDevice++)
        {
            if(d3dDeviceInfo.hAdapter != NULL)
            {
                D3DKMT_CLOSEADAPTER d3dCloseAdapter = {0};

                d3dCloseAdapter.hAdapter    = d3dDeviceInfo.hAdapter;

                D3DKMTCloseAdapterPrivate(
                    &d3dCloseAdapter
                    );
            }

            memset(
                &d3dDeviceInfo,
                0,
                sizeof(d3dDeviceInfo)
                );

            memset(
                &d3dAdapterInfo,
                0,
                sizeof(d3dAdapterInfo)
                );

            d3dDeviceInfo.pDeviceName   = ppwszDevicePath[dwCurrDevice];

            hr = HRESULT_FROM_NT(D3DKMTOpenAdapterFromDeviceNamePrivate(
                &d3dDeviceInfo
                ));
            if(FAILED(hr))
            {
                if(pLogger != NULL)
                {
					LogTC(pLogger, NORMAL, L"WARNING>> Failed to open display adapter, DFlip will be verified which may cause failures on drivers <= WDDM 1.1 (hr=0x%x : %s )", hr, StringFromMFHRESULT(hr));
                }

                break;
            }

            d3dAdapterInfo.hAdapter                 = d3dDeviceInfo.hAdapter;
            d3dAdapterInfo.Type                     = KMTQAITYPE_DRIVERVERSION;
            d3dAdapterInfo.pPrivateDriverData       = (void*)&d3dDriverVersion;
            d3dAdapterInfo.PrivateDriverDataSize    = sizeof(d3dDriverVersion);

            hr = HRESULT_FROM_NT(D3DKMTQueryAdapterInfoPrivate(
                &d3dAdapterInfo
                ));
            if(FAILED(hr))
            {
                if(pLogger != NULL)
                {
                    LogTC(pLogger, NORMAL, L"WARNING>> Failed to query display adapter info, DFlip will be verified which may cause failures on drivers <= WDDM 1.1 (hr=0x%x: %s )", hr, StringFromMFHRESULT(hr));
                }

                break;
            }

            if(d3dDriverVersion <= KMT_DRIVERVERSION_WDDM_1_1)
            {
                if(pLogger != NULL)
                {
                    LogTC(pLogger, NORMAL, L"INFO>> Driver is <= WDDM 1.1, DFlip is not required and will not be verified");
                }

                bVerifyDFlip    = FALSE;

                break;
            }
        }
    }while(false);

    if(d3dDeviceInfo.hAdapter != NULL)
    {
        D3DKMT_CLOSEADAPTER d3dCloseAdapter = {0};

        d3dCloseAdapter.hAdapter    = d3dDeviceInfo.hAdapter;

        D3DKMTCloseAdapterPrivate(
            &d3dCloseAdapter
            );
    }

    for(DWORD dwCurrDevice = 0; dwCurrDevice < dwNumDevices; dwCurrDevice++)
    {
        SAFEFREE(ppwszDevicePath[dwCurrDevice]);
    }
    SAFEFREE(ppwszDevicePath);

    return bVerifyDFlip;
}

#pragma prefast(pop)

CDXVATestContext::CDXVATestContext(IConfig* pConfig, IMFActivate* pActivate): 
CMIContext(pConfig, pActivate)
{
    dwSize = sizeof(CDXVATestContext);
    dwTimeoutSec        = INFINITE;
    dwRenderingMode     = MFTDRENDERING_CUSTOMVIDRENDERER;
    bUseSmartSource     = FALSE;
    bAllowAudio         = FALSE;
    bRunAllModes        = TRUE;
    bUseDComp           = FALSE;
    bFullScreen         = FALSE;
    bH264SendSampleDesc = TRUE;
    bH264SendFakeMT     = FALSE;
    eDXVAMode           = g_dxvamodeDefault;
    eVideoFormat        = H264;
    dwCSTestStartTime   = 0;
    dwRunTimeInSec      = 0;
    lfFailPassRatio     = 0;
    lfVVThreshold       = 0;
}

CDXVATestContext::~CDXVATestContext(void)
{
}

HRESULT InitializeDXVATestTable(CXmlRootConfig** ppXMLRootConfig)
{
    HRESULT hr = S_OK;
    HGLOBAL hResource = NULL;
    CXmlRootConfig* pConfig = NULL;

    do
    {
        if(ppXMLRootConfig == NULL)
        {
            hr = E_POINTER;
            break;
        }

        pConfig = new CXmlRootConfig();
        if(pConfig == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        HMODULE hMod = NULL;

        if(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (TCHAR*)&InitializeDXVATestTable, &hMod) == FALSE)
        {
            hr = E_FAIL;
            break;
        }

        HRSRC hRsrc = FindResource(hMod, MAKEINTRESOURCE(IDR_DXVATESTSCONFIG), L"Config");
        if(hRsrc == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hResource = LoadResource(hMod, hRsrc);
        if(hResource == NULL)
        {
            hr = E_FAIL;
            break;
        }

        char* pszXML = (char*) LockResource(hResource);
        if(pszXML == NULL)
        {
            hr = E_FAIL;
            break;
        }

        DWORD dwXML = SizeofResource(hMod, hRsrc);
        if(dwXML == 0)
        {
            hr = E_FAIL;
            break;
        }

        int nConvertedLength = MultiByteToWideChar(CP_ACP, 0, pszXML, dwXML, NULL, NULL);

        CComBSTR bstrXML(nConvertedLength);

        if(MultiByteToWideChar(CP_ACP, 0, pszXML, dwXML, bstrXML, nConvertedLength) == 0)
        {
            hr = E_FAIL;
            break;
        }

        hr = pConfig->Init(bstrXML);
        if(FAILED(hr))
        {
            break;
        }

        *ppXMLRootConfig = pConfig;
    }while(false);

    if(hResource != NULL)
    {
        FreeResource(hResource);
    }

    if(FAILED(hr))
    {
        SAFEDELETES(pConfig);
    }

    return hr;
}

static BOOL DXCFrameCompCallback(void* pContext, DWORD dwCurrFrame, DWORD dwNumFailed, UINT64 un64CurrBytesProcessed, 
        UINT64 un64TotalBytes, double lfYPSNR, double lfUPSNR, double lfVPSNR, double lfYDSIM, double lfUDSIM, double lfVDSIM)
{
    static WCHAR pszProgressBar[] = L"[--------------------]";
    const DWORD dwProgressBarLen = sizeof(pszProgressBar) / sizeof(pszProgressBar[0]);

    ILogger* pLogger = (ILogger*)pContext;

    if(pLogger != NULL)
    {
        swprintf_s(pszProgressBar, dwProgressBarLen, L"[--------------------]");

        double lfPercentDone = ((double)un64CurrBytesProcessed / (double)un64TotalBytes) * 100.0;

        for(DWORD i = 0; i < (DWORD)lfPercentDone; i+= 5)
        {
            pszProgressBar[(i/5) + 1] = L'*';
        }

        wprintf_s(L"%s - Frame %u (%u failed) %03.02lf%%\r", pszProgressBar, dwCurrFrame, dwNumFailed, lfPercentDone);

        pLogger->Log(FILE_ONLY, L"DXCompare>> Stats for frame %u", dwCurrFrame);
        pLogger->Log(FILE_ONLY, L"                YPSNR - %4.04lf", lfYPSNR);
        pLogger->Log(FILE_ONLY, L"                UPSNR - %4.04lf", lfUPSNR);
        pLogger->Log(FILE_ONLY, L"                VPSNR - %4.04lf", lfVPSNR);
    }

    return TRUE;
}

static HRESULT PerformVisualValidation(ILogger* pLogger, const WCHAR* pszTestFileName, const WCHAR* pszRefFileName,
                                const DWORD dwWidth, const DWORD dwHeight, const GUID* pGUIDSubType,
                                double lfPSNRThreshold, double lfFailPassRatio)
{
    HRESULT     hr                  = S_OK;
    CDXCompare* pdxComp             = NULL;
    GUID        guidCompareSubtype  = GUID_NULL;

    do
    {
        if((pszTestFileName == NULL) || (pszRefFileName == NULL) ||( pGUIDSubType == NULL))
        {
            hr = E_POINTER;
            break;
        }

        pdxComp = new CDXCompare();
        if(pdxComp == NULL)
        {
            LogTC(pLogger, NORMAL, L"Error: Out of memory, can't allocate comparison class");
            hr = E_OUTOFMEMORY;
            break;
        }

        pdxComp->SetNullLogger();

        pdxComp->SetFrameCompleteCallback(&DXCFrameCompCallback, pLogger);

        guidCompareSubtype  = (*pGUIDSubType);

        if( (guidCompareSubtype == MFVideoFormat_RGB32)     ||
            (guidCompareSubtype == MFVideoFormat_ARGB32)    )
        {
            // DXCompare does not understand MF Media Types
            guidCompareSubtype  = MEDIASUBTYPE_ARGB32;
        }

        hr = pdxComp->ValidateData(pszTestFileName, pszRefFileName, dwWidth, dwHeight, guidCompareSubtype, 
            DEFAULT_RSMTHRESHOLD, lfPSNRThreshold, lfFailPassRatio);
        if(FAILED(hr))
        {
            DWORD dwTotalFrames = 0;
            DWORD dwFailedFrames = 0;
            double lfYPSNR = 0.0;
            double lfUPSNR = 0.0;
            double lfVPSNR = 0.0;

            pdxComp->GetFrameFailureStats(&dwTotalFrames, &dwFailedFrames);
            pdxComp->GetPSNRValues(&lfYPSNR, &lfUPSNR, &lfVPSNR);
			LogTC(pLogger, HIGH, L"Error: Visual Validation Failed: ( hr =  0x%08X : %s) ", hr, StringFromMFHRESULT(hr));
            LogTC(pLogger, HIGH, L"       Frame Failure Stats: %u/%u", dwFailedFrames, dwTotalFrames);
            LogTC(pLogger, HIGH, L"       PSNR Stats - observed (threshold):");
            LogTC(pLogger, HIGH, L"             Y - %4.04lf (%4.04lf)", lfYPSNR, lfPSNRThreshold);
            LogTC(pLogger, HIGH, L"             U - %4.04lf (%4.04lf)", lfUPSNR, lfPSNRThreshold);
            LogTC(pLogger, HIGH, L"             V - %4.04lf (%4.04lf)", lfVPSNR, lfPSNRThreshold);
            
            break;
        }
            
        LogTC(pLogger, HIGH, L"Success: Visual Validation Passed");
    }while(false);

    SAFEDELETES(pdxComp);

    return hr;
}

HRESULT GetMFTToTest(ILogger* pLogger, CDXVATestContext* pDXVATestContext, IMFActivate** ppActivate)
{
    if(pDXVATestContext == NULL || ppActivate == NULL || pLogger == NULL)
    {
        return E_INVALIDARG;
    }
    
    HRESULT hr = S_OK;
    UINT32 mftEnumFlag = ~MFT_ENUM_FLAG_HARDWARE & MFT_ENUM_FLAG_ALL;
    IMFActivate** ppResultActivates = NULL;
    UINT32 cResultActivates = 0;
    GUID guidMFTCLSID = GUID_NULL;
    WCHAR* pszMFTCLSID = NULL;
    BOOL bTargetMFTFound = FALSE;
    MFT_REGISTER_TYPE_INFO stInputTypes = {0};
    stInputTypes.guidMajorType = MFMediaType_Video;
	
    do
    {
        switch(pDXVATestContext->eVideoFormat)
        {
            case H264:

                stInputTypes.guidSubtype = MFVideoFormat_H264;
                break;

            case MPEG2: 

                stInputTypes.guidSubtype = MFVideoFormat_MPEG2;
                break;

            case WMV1:
                
                stInputTypes.guidSubtype = MFVideoFormat_WMV1;
                break;

            case WMV2:
                
                stInputTypes.guidSubtype = MFVideoFormat_WMV2;
                break;

            case WMV3:
                
                stInputTypes.guidSubtype = MFVideoFormat_WMV3;
                break;

            case WVC1:
                
                stInputTypes.guidSubtype = MFVideoFormat_WVC1;
                break;

            case HEVC:
            case HEVC10:
                stInputTypes.guidSubtype = MFVideoFormat_HEVC;
                break;
          
            default:

                hr = E_UNEXPECTED;
                break;
        }

        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"Error: Invalid Video Format 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
			break;
        }
    
        hr = MFTEnumEx(MFT_CATEGORY_VIDEO_DECODER, mftEnumFlag, &stInputTypes, NULL, &ppResultActivates, &cResultActivates);
        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"Error: Failed to enumerate the available Video Decoder MFTs 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        // find the activate object we want to test
        for(DWORD i = 0; i < cResultActivates; i++)
        {
            hr = ppResultActivates[i]->GetGUID(MFT_TRANSFORM_CLSID_Attribute, &guidMFTCLSID);
            if(FAILED(hr))
            {
				LogTC(pLogger, HIGH, L"Error: Failed to obtain CLSID/GUID on the activation object 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }
         
            hr = StringFromCLSID(guidMFTCLSID, &pszMFTCLSID);
            if(FAILED(hr))
            {
				LogTC(pLogger, HIGH, L"Error: Failed to convert CLSID/GUID to a string of printable characters 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }

            if(0 == _wcsicmp(pszMFTCLSID, pDXVATestContext->csComponentCLSID))
            {
                *ppActivate = ppResultActivates[0];
                (*ppActivate)->AddRef();
                bTargetMFTFound = TRUE;
                break;
            }

            SAFECOTASKMEMFREE(pszMFTCLSID);
        }
        
        if(bTargetMFTFound == FALSE)
        {
            hr = E_FAIL;
        }

    }while(FALSE);

    for(DWORD i = 0; i < cResultActivates; i++)
    {
        SAFERELEASE(ppResultActivates[i]);
    }

    SAFECOTASKMEMFREE(pszMFTCLSID);
    SAFECOTASKMEMFREE(ppResultActivates);

    return hr;
}

HRESULT VideoFormatStringToEnum(CDXVATestContext* pDXVATestContext)
{
    if(pDXVATestContext == NULL)
    {
        return E_INVALIDARG;
    }
    
    if(0 == _wcsicmp(pDXVATestContext->csInputMediaType, L"H264"))
    {
        pDXVATestContext->eVideoFormat = H264;
    }
    else if(0 == _wcsicmp(pDXVATestContext->csInputMediaType, L"MPEG2"))
    {
        pDXVATestContext->eVideoFormat = MPEG2;
    }
    else if(0 == _wcsicmp(pDXVATestContext->csInputMediaType, L"WMV1"))
    {
        pDXVATestContext->eVideoFormat = WMV1;
    }
    else if(0 == _wcsicmp(pDXVATestContext->csInputMediaType, L"WMV2"))
    {
        pDXVATestContext->eVideoFormat = WMV2;
    }
    else if(0 == _wcsicmp(pDXVATestContext->csInputMediaType, L"WMV3"))
    {
        pDXVATestContext->eVideoFormat = WMV3;
    } 
    else if(0 == _wcsicmp(pDXVATestContext->csInputMediaType, L"WVC1"))
    {
        pDXVATestContext->eVideoFormat = WVC1;
    }
    else if(0 == _wcsicmp(pDXVATestContext->csInputMediaType, L"HEVC"))
    {
        pDXVATestContext->eVideoFormat = HEVC;
    }
    else if (0 == _wcsicmp(pDXVATestContext->csInputMediaType, L"HEVC10"))
    {
        pDXVATestContext->eVideoFormat = HEVC10;
    }
    else
    {
        return E_UNEXPECTED;
    }

    return S_OK;
}

HRESULT RunPlaybackTest(ILogger* pLogger, CMIContext* pDXVAContext)
{
    HRESULT                     hr                          = S_OK;
    CTestMediaSessionPlayback*  pPlaybackSession            = NULL;
    IMFActivate*                pActivateToTest             = NULL;
    BOOL                        bCloseSession               = FALSE;
    BOOL                        bShutdownMF                 = FALSE;
    BOOL                        bVerifyDFlip                = TRUE;
    BOOL                        bAESUsed                    = FALSE;
    BOOL                        bTestFailed                 = FALSE;
    
    do
    {
        if(pDXVAContext == NULL || pLogger == NULL)
        {
            hr = E_INVALIDARG;
            break;
        }

        CDXVATestContext* pDXVATestContext = (CDXVATestContext*)pDXVAContext;

        // run playback for each supported DXVA mode
        for(int nCurrentDXVAOffloadMode = VLD_SUPPORTED; nCurrentDXVAOffloadMode >= IDCT_SUPPORTED; nCurrentDXVAOffloadMode--)
        {
            CDecodeCallbackContext  SetupDecoderForDXVAModeContext;
            SetupDecoderForDXVAModeContext.pLogger              = pLogger;
            SetupDecoderForDXVAModeContext.dwVideoFormat        = pDXVATestContext->eVideoFormat;
            SetupDecoderForDXVAModeContext.dwDXVAOffloadMode    = nCurrentDXVAOffloadMode;

            if(FAILED(hr))
            {
                bTestFailed = TRUE;
				LogTC(pLogger, HIGH, L"Error: hr = 0x%08X : %s", hr, StringFromMFHRESULT(hr));
                // Can only get to this point if one test has already run
                if(pDXVATestContext->bRunAllModes == FALSE)
                {
                    // If we're not doing more than one mode, we're done
                    break;
                }
            }

            if(bCloseSession)
            {
                if(FAILED(pPlaybackSession->Close()))
                {
                    LogTC(pLogger, HIGH, L"Error: failed to close playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                }

                bCloseSession = FALSE;
            }
            SAFERELEASE(pPlaybackSession);

            if(bShutdownMF != FALSE)
            {
                MFShutdown();
                bShutdownMF = FALSE;
            }

            hr = MFStartup(MF_VERSION);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed to startup Media foundation, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }
        
            bShutdownMF = TRUE;

            SAFERELEASE(pActivateToTest);
            hr = GetMFTToTest(
				pLogger,
                pDXVATestContext,
                &pActivateToTest
                );
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed to find the MFT to test, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }

            LogTC(pLogger, HIGH, L"Info: Running test for DXVA mode %s (DX%s%s%s)",
                (nCurrentDXVAOffloadMode == VLD_SUPPORTED) ?                                L"VLD" :
                (nCurrentDXVAOffloadMode == MOCOMP_SUPPORTED) ?                             L"MoComp" :
                (nCurrentDXVAOffloadMode == IDCT_SUPPORTED) ?                               L"IDCT" : L"Unknown",
                (pDXVATestContext->dwRenderingMode == MFTDRENDERING_CUSTOMVIDRENDERER) ?    L"11" : L"9",
                (pDXVATestContext->bUseDComp != FALSE) ?                                    L" DComp" : L"",
                (pDXVATestContext->bFullScreen != FALSE) ?                                  L" DFlip" : L""
                );

            hr = CTestMediaSessionPlayback::CreateInstance(pLogger, pDXVATestContext->dwRenderingMode, &pPlaybackSession);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed to create instance of a playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }

            bCloseSession = TRUE;

            LogTC(pLogger, NORMAL, L"Successfully initialized playback session");

            hr = pPlaybackSession->SetBuildMode(TRUE, pActivateToTest);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed to set the IMFActivate on the playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }

            hr = pPlaybackSession->SetRegisterWithMMCSS(TRUE);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed to set playback session to register with MMCSS, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }

            hr = pPlaybackSession->SetAudioAllowed(pDXVATestContext->bAllowAudio);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: Failed to set audio allowed, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }

            hr = pPlaybackSession->SetDXVAMode(pDXVATestContext->eDXVAMode);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: Failed to set DXVA Mode, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }

            if(pDXVATestContext->dwRenderingMode == MFTDRENDERING_CUSTOMVIDRENDERER)
            {
                hr = pPlaybackSession->SetDComp(
                    pDXVATestContext->bUseDComp,
                    pDXVATestContext->bFullScreen
                    );
                if(FAILED(hr))
                {
                    LogTC(pLogger, HIGH, L"Error: Failed to set DComp Mode, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                    break;
                }

                bVerifyDFlip    = ShouldVerifyDFlip(pLogger);
            }

            if(pDXVATestContext->eVideoFormat == MPEG2)
            {
                hr = pPlaybackSession->SetAllowMFCPKSourceForMPEG2(
                    TRUE
                    );
                if(FAILED(hr))
                {
                    LogTC(pLogger, HIGH, L"Error: Failed to set MFCPKSource on playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                    break;
                }
            }

            hr = pPlaybackSession->SetSendH264SampleDescription(
                pDXVATestContext->bH264SendSampleDesc
                );
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: Failed to set SendH264SampleDescription on playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }

            hr = pPlaybackSession->SetSendH264FakeMediatype(
                pDXVATestContext->bH264SendFakeMT
                );
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: Failed to set SendH264FakeMediatype on playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }
            
            hr = pPlaybackSession->SetPostManualInsertDecoderCallback(
                &SetupDecoderForDXVAMode,
                &SetupDecoderForDXVAModeContext
                );
            if(FAILED(hr))
            {
                break;
            }

            hr = pPlaybackSession->BuildTopology(pDXVATestContext->csInFileName, FALSE);
            if(FAILED(hr))
            {
                if(SetupDecoderForDXVAModeContext.bFailureAcceptable != FALSE)
                {
                    // Failure was acceptable, mode not supported, try next one
                    LogTC(pLogger, HIGH, L"Info: Mode %s is not supported",
                        (nCurrentDXVAOffloadMode == VLD_SUPPORTED) ?       L"VLD" :
                        (nCurrentDXVAOffloadMode == MOCOMP_SUPPORTED) ?    L"MoComp" :
                        (nCurrentDXVAOffloadMode == IDCT_SUPPORTED) ?      L"IDCT" : L"Unknown"
                        );

                    hr = S_OK;
                    continue;
                }

                LogTC(pLogger, HIGH, L"Error: failed to build playback session: 0x%08X", hr);
                if(SetupDecoderForDXVAModeContext.pwszFailureReason != NULL)
                {
                    LogTC(pLogger, HIGH, L"Error: Failure reason - %s", SetupDecoderForDXVAModeContext.pwszFailureReason);
                }
                
                continue;
            }

            LogTC(pLogger, NORMAL, L"Successfully built a playback topology");

            hr = pPlaybackSession->WaitForState(STATE_TOPOLOGY_READY, pDXVATestContext->dwTimeoutSec);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: cannot build a topology: 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                continue;
            }

            LogTC(pLogger, NORMAL, L"Topology in use:");
            if(FAILED(pPlaybackSession->WalkFullTopology(2)))
            {
                LogTC(pLogger, HIGH, L"Error: Failed to walk topology");	
            }

            hr = pPlaybackSession->Start();
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed to start playback topology, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                continue;
            }

            hr = pPlaybackSession->WaitForState(STATE_PLAYING, pDXVATestContext->dwTimeoutSec);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: call to start a media session succeeded, but session did not get to a running state over %ld sec, 0x%08X (%s)", 
					pDXVATestContext->dwTimeoutSec, hr, StringFromMFHRESULT(hr));
                continue;
            }

            if(pDXVATestContext->dwCSTestStartTime > 0)
            {
                Sleep(pDXVATestContext->dwCSTestStartTime);

                LogTC(pLogger, HIGH, L"Info: Entering connected stand-by...");

                hr = pPlaybackSession->EnterExitConnectedStandby();
                if(FAILED(hr))
                {
					LogTC(pLogger, HIGH, L"Error: Failed connected standby test 0x%x (%s)", hr, StringFromMFHRESULT(hr));
                    continue;
                }
            }

            LogTC(pLogger, NORMAL, L"Media session is in playing state, waiting for completion");

            hr = pPlaybackSession->WaitForState(STATE_STOPPED, pDXVATestContext->dwTimeoutSec);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed to wait for ending of the media session (stopping), 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                continue;
            }

            if(bVerifyDFlip != FALSE)
            {
                hr = pPlaybackSession->CheckForRequiredETWEvents();
                if(FAILED(hr))
                {
                    continue;
                }
            }

            hr = pPlaybackSession->GetCallbackHRESULT();
            if(FAILED(hr))
            {
				LogTC(pLogger, HIGH, L"Error: Media Session recieved a failed event, 0x%x (%s)", hr, StringFromMFHRESULT(hr));
                continue;
            }

            if(pPlaybackSession->ShouldFailTest() != FALSE)
            {
                hr = E_FAIL;
                continue;
            }

            if(SetupDecoderForDXVAModeContext.bAESSupported == FALSE)
            {
                LogTC(pLogger, HIGH, L"Error: Playback ran successfully but AES was not used");
            }
            else
            {
                bAESUsed    = TRUE;
            }

            if(pDXVATestContext->bRunAllModes == FALSE)
            {
                // If we're not doing more than one mode, we're done
                break;
            }
        }
    }while(FALSE);

    if(bCloseSession)
    {
        if(FAILED(pPlaybackSession->Close()))
        {
            LogTC(pLogger, HIGH, L"Error: failed to close playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
        }
    }

    SAFERELEASE(pActivateToTest);
    SAFERELEASE(pPlaybackSession);

    if(bShutdownMF)
    {
        if(FAILED(MFShutdown()))
        {
            LogTC(pLogger, HIGH, L"Error: failed to shutdown Media Foundation");
        }
    }

    if(hr == S_OK)
    {
        if(bAESUsed == FALSE)
        {
            // If test succeeded (and not skipped), and AES was not used, mark the test as failed
            hr = E_FAIL;
        }
    }

    if(SUCCEEDED(hr))
    {
        if(bTestFailed != FALSE)
        {
            hr = E_FAIL;
        }
    }

    return hr;
}

HRESULT RunStressTest(ILogger* pLogger, CMIContext* pDXVAContext)
{
    HRESULT hr = S_OK;
    CTestMediaSessionPlayback*  pPlaybackSession = NULL;
    IMFActivate* pActivateToTest = NULL;
    BOOL bCloseSession = FALSE;
    BOOL bShutdownMF = FALSE;
    
    do
    {
        if(pDXVAContext == NULL || pLogger == NULL)
        {
            hr = E_INVALIDARG;
            break;
        }

        CDXVATestContext* pDXVATestContext = (CDXVATestContext*)pDXVAContext;

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to startup Media foundation,  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        bShutdownMF = TRUE;

        hr = GetMFTToTest(pLogger, pDXVATestContext, &pActivateToTest);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to find the MFT to test,  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = CTestMediaSessionPlayback::CreateInstance(pLogger, pDXVATestContext->dwRenderingMode, &pPlaybackSession);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to create instance of a playback session,  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        bCloseSession = TRUE;

        LogTC(pLogger, NORMAL, L"Successfully initialized playback session");

        hr = pPlaybackSession->SetBuildMode(TRUE, pActivateToTest);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to set the IMFActivate on the playback session,  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pPlaybackSession->SetRegisterWithMMCSS(TRUE);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to set playback session to register with MMCSS,  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        if(pDXVATestContext->bUseSmartSource == TRUE) // stress test
        {
            hr = pPlaybackSession->SetUseSmartSource(pDXVATestContext->bUseSmartSource);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: Failed to set playback session to use smart source:  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }

            hr = pPlaybackSession->SetRunTimeInSeconds(pDXVATestContext->dwRunTimeInSec);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: Failed to set playback run times:  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }
        }
        else
        {
            hr = E_UNEXPECTED;
            break;
        }

        hr = pPlaybackSession->SetAudioAllowed(pDXVATestContext->bAllowAudio);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: Failed to set audio allowed,  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pPlaybackSession->SetDXVAMode(pDXVATestContext->eDXVAMode);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: Failed to set DXVA Mode,  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        if(pDXVATestContext->eVideoFormat == MPEG2)
        {
            hr = pPlaybackSession->SetAllowMFCPKSourceForMPEG2(
                TRUE
                );
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: Failed to set MFCPKSource on playback session,  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }
        }

        hr = pPlaybackSession->BuildTopology(pDXVATestContext->csInFileName, FALSE);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to build playback session: 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully built a playback topology");

        hr = pPlaybackSession->WaitForState(STATE_TOPOLOGY_READY, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: cannot build a topology: 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        LogTC(pLogger, NORMAL, L"Topology in use:");
        if(FAILED(pPlaybackSession->WalkFullTopology(2)))
        {
            LogTC(pLogger, HIGH, L"Error: Failed to walk topology");	
        }

        LogTC(pLogger, NORMAL, L"Starting playback...");

        hr = pPlaybackSession->Start();
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to start playback topology, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pPlaybackSession->WaitForState(STATE_PLAYING, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: call to start a media session succeeded, but session did not get to a running state over %ld sec, 0x%08X (%s)", 
				pDXVATestContext->dwTimeoutSec, hr, StringFromMFHRESULT(hr));
            break;
        }

        LogTC(pLogger, NORMAL, L"Media session is in playing state, waiting for completion");

        hr = pPlaybackSession->WaitForState(STATE_STOPPED, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to wait for ending of the media session (stopping), 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        pPlaybackSession->CheckForComponentReports();

        hr = pPlaybackSession->GetCallbackHRESULT();
        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"Error: Media Session recieved a failed event, 0x%x (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        if(pPlaybackSession->ShouldFailTest() != FALSE)
        {
            hr = E_FAIL;
            break;
        }
    }while(FALSE);

    if(bCloseSession)
    {
        if(FAILED(pPlaybackSession->Close()))
        {
            LogTC(pLogger, HIGH, L"Error: failed to close playback session,  0x%08X (%s)", hr, StringFromMFHRESULT(hr));
        }
    }

    if(bShutdownMF)
    {
        if(FAILED(MFShutdown()))
        {
            LogTC(pLogger, HIGH, L"Error: failed to shutdown Media Foundation");
        }
    }

    SAFERELEASE(pActivateToTest);
    SAFERELEASE(pPlaybackSession);

    return hr;
}

HRESULT RunMITest(DWORD dwNumInstances, ILogger* pLogger, CMIContext* pDXVAContext)
{
    HRESULT hr = S_OK;
    CTestMediaSessionPlayback*  pPlaybackSession = NULL;
    IMFActivate* pActivateToTest = NULL;
    BOOL bCloseSession = FALSE;
    BOOL bShutdownMF = FALSE;
    
    do
    {
        if(pDXVAContext == NULL || pLogger == NULL)
        {
            hr = E_INVALIDARG;
            break;
        }

        CDXVATestContext* pDXVATestContext = (CDXVATestContext*)pDXVAContext;

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"Error: failed to startup Media foundation,  0x%08X (%s)", hr , StringFromMFHRESULT(hr));
            break;
        }

        bShutdownMF = TRUE;

        hr = GetMFTToTest(pLogger, pDXVATestContext, &pActivateToTest);
        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"Error: failed to find the MFT to test, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = CTestMediaSessionPlayback::CreateInstance(pLogger, pDXVATestContext->dwRenderingMode, &pPlaybackSession);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to create instance of a playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        bCloseSession = TRUE;

        LogTC(pLogger, NORMAL, L"Successfully initialized playback session");

        hr = pPlaybackSession->SetBuildMode(TRUE, pActivateToTest);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to set the IMFActivate on the playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pPlaybackSession->SetRegisterWithMMCSS(TRUE);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to set playback session to register with MMCSS, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pPlaybackSession->SetAudioAllowed(pDXVATestContext->bAllowAudio);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: Failed to set audio allowed, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pPlaybackSession->SetDXVAMode(pDXVATestContext->eDXVAMode);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: Failed to set DXVA Mode, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        if(pDXVATestContext->eVideoFormat == MPEG2)
        {
            hr = pPlaybackSession->SetAllowMFCPKSourceForMPEG2(
                TRUE
                );
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: Failed to set MFCPKSource on playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }
        }

        hr = pPlaybackSession->BuildTopology(pDXVATestContext->csInFileName, FALSE);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to build playback session: 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully built a playback topology");

        hr = pPlaybackSession->WaitForState(STATE_TOPOLOGY_READY, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: cannot build a topology: 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        LogTC(pLogger, NORMAL, L"Topology in use:");
        if(FAILED(pPlaybackSession->WalkFullTopology(2)))
        {
            LogTC(pLogger, HIGH, L"Error: Failed to walk topology");	
        }

        LogTC(pLogger, NORMAL, L"Starting playback...");

        hr = pPlaybackSession->Start();
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to start playback topology, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pPlaybackSession->WaitForState(STATE_PLAYING, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: call to start a media session succeeded, but session did not get to a running state over %ld sec, 0x%08X (%s)", 
            pDXVATestContext->dwTimeoutSec, hr, StringFromMFHRESULT(hr));
            break;
        }

        LogTC(pLogger, NORMAL, L"Media session is in playing state, waiting for completion");

        hr = pPlaybackSession->WaitForState(STATE_STOPPED, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to wait for ending of the media session (stopping), 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        pPlaybackSession->CheckForComponentReports();

        hr = pPlaybackSession->GetCallbackHRESULT();
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: Media Session recieved a failed event, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        if(pPlaybackSession->ShouldFailTest() != FALSE)
        {
            hr = E_FAIL;
            break;
        }
    }while(FALSE);

    if(bCloseSession)
    {
        if(FAILED(pPlaybackSession->Close()))
        {
            LogTC(pLogger, HIGH, L"Error: failed to close playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
        }
    }

    if(bShutdownMF)
    {
        if(FAILED(MFShutdown()))
        {
            LogTC(pLogger, HIGH, L"Error: failed to shutdown Media Foundation");
        }
    }

    SAFERELEASE(pActivateToTest);
    SAFERELEASE(pPlaybackSession);

    return hr;
}

HRESULT RunQualityTest(ILogger* pLogger, CMIContext* pDXVAContext)
{
    HRESULT hr = S_OK;
    HRESULT hrSW = S_OK;
    CTestMediaSessionPlayback*  pPlaybackSession = NULL;
    IMFActivate* pActivateToTest = NULL;
    DWORD dwWidth = 0;
    DWORD dwHeight = 0;
    GUID  guidSubType = GUID_NULL;
    CString sDumpFileName;
    CString sSWDumpFileName;
    double  lfVVThreshold = 0;
    double  lfFailPassRatio = 0;
    BOOL bCloseSession = FALSE;
    BOOL bShutdownMF = FALSE;
    const WCHAR* pwszFileNameNoPath   = NULL;

    do
    {
        if(pDXVAContext == NULL || pLogger == NULL)
        {
            hr = E_INVALIDARG;
            break;
        }

        CDXVATestContext* pDXVATestContext = (CDXVATestContext*)pDXVAContext;

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to startup Media foundation : 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        bShutdownMF = TRUE;

        pwszFileNameNoPath  = wcsrchr((const WCHAR*)pDXVATestContext->csInFileName, L'\\');
        if(pwszFileNameNoPath == NULL)
        {
            pwszFileNameNoPath = (const WCHAR*)pDXVATestContext->csInFileName;
        }

        hr = GetMFTToTest(pLogger, pDXVATestContext, &pActivateToTest);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to find the MFT to test : 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        if(pDXVATestContext->lfVVThreshold == 0)
        {
            lfVVThreshold = DEFAULT_PSNRTHRESHOLD;
        }
        else
        {
            lfVVThreshold = pDXVATestContext->lfVVThreshold;
        }

        if(pDXVATestContext->lfFailPassRatio == 0)
        {
            lfFailPassRatio = DEFAULT_FAILPASSRATIO;
        }
        else
        {
            lfFailPassRatio = pDXVATestContext->lfFailPassRatio;
        }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               DXVA Playback
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        hr = CTestMediaSessionPlayback::CreateInstance(pLogger, pDXVATestContext->dwRenderingMode, &pPlaybackSession);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to create instance of a playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        bCloseSession = TRUE;

        LogTC(pLogger, NORMAL, L"Successfully initialized playback session");

        hr = pPlaybackSession->SetDXVAMode(pDXVATestContext->eDXVAMode);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set DXVA Mode hr=0x%x)", hr);
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully initialized playback session");

        hr = pPlaybackSession->SetBuildMode(TRUE, pActivateToTest);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to set the IMFActivate on the playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        sDumpFileName = DUMPDIRECTORY;
        sDumpFileName += pwszFileNameNoPath;
        sDumpFileName += L"_VisValPlaybackTest_HW.dump";

        DeleteFile(sDumpFileName);

        hr = pPlaybackSession->SetDumpMode(TRUE, sDumpFileName);
        if(FAILED(hr))
        {
            break;
        }
        
        if(CreateDirectory(DUMPDIRECTORY, NULL) == FALSE)
        {
            DWORD dwError = GetLastError();
            if(dwError != ERROR_ALREADY_EXISTS)
            {
                LogTC(pLogger, HIGH, L"Failed to create Playback Dump Directory, can't proceed with VV Dump (Error: %u)", dwError);
                break;
            }
        }

        hr = pPlaybackSession->SetAudioAllowed(FALSE);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set audio allowed: 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pPlaybackSession->SetHWAllowed(FALSE);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set HW Allowed: 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        switch(pDXVATestContext->eVideoFormat)
        {
        case WMV1:
        case WMV2:
        case WMV3:
        case WVC1:
            {
                VARIANT var = {0};

                var.vt      = VT_I4;
                var.lVal    = 0;

                hr = pPlaybackSession->SetPropertyBagPropertyOnManualBuildMFT(
                    g_wszWMVCForcePostProcessMode,
                    &var
                    );
                if(FAILED(hr))
                {
                    LogTC(pLogger, HIGH, L"ERROR>> Failed to set WMV Post Processing Mode to %d", var.lVal);
                    break;
                }
            }
            break;
        default:
            // Nothing to do
            break;
        };
        if(FAILED(hr))
        {
            break;
        }

        if(pDXVATestContext->eVideoFormat == MPEG2)
        {
            hr = pPlaybackSession->SetAllowMFCPKSourceForMPEG2(
                TRUE
                );
            if(FAILED(hr))
            {
				LogTC(pLogger, HIGH, L"Error: Failed to set MFCPKSource on playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
                break;
            }
        }

        hr = pPlaybackSession->BuildTopology(pDXVATestContext->csInFileName, TRUE);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to build playback session: 0x%08X", hr);
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully built a playback topology");

        hr = pPlaybackSession->WaitForState(STATE_TOPOLOGY_READY, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"Error: cannot build a topology over %lu sec, assuming failure: 0x%08X (%s)", pDXVATestContext->dwTimeoutSec, hr, StringFromMFHRESULT(hr));
            break;
        }

        LogTC(pLogger, NORMAL, L"Topology in use:");
        if(FAILED(pPlaybackSession->WalkFullTopology(2)))
        {
            LogTC(pLogger, HIGH, L"Error: Failed to walk topology");	
        }

        LogTC(pLogger, NORMAL, L"Starting playback...");

        hr = pPlaybackSession->Start();
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to start playback topology, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pPlaybackSession->WaitForState(STATE_PLAYING, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"Error: call to start a media session succeeded, but session did not get to a running state over %ld sec, 0x%08X (%s)", pDXVATestContext->dwTimeoutSec, hr, StringFromMFHRESULT(hr));
            break;
        }

        LogTC(pLogger, NORMAL, L"Media session is in playing state, waiting for completion");

        hr = pPlaybackSession->WaitForState(STATE_STOPPED, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to wait for ending of the media session (stopping), 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        pPlaybackSession->GetDumpFileInfo(&dwWidth, &dwHeight, &guidSubType);

        pPlaybackSession->CheckForComponentReports();

        hr = pPlaybackSession->GetCallbackHRESULT();
        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"Error: Media Session recieved a failed event, 0x%x (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        hr = pPlaybackSession->Close();
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to close playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }

        bCloseSession = FALSE;

        if(pPlaybackSession->ShouldFailTest() != FALSE)
        {
            hr = E_FAIL;
            break;
        }

        SAFERELEASE(pPlaybackSession);
        SAFERELEASE(pActivateToTest);

        WriteInfoFile(sDumpFileName, dwWidth, dwHeight, &guidSubType);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               Software Playback
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        LogTC(pLogger, HIGH, L"Starting Test for Software Comparison Component");

        hrSW = GetMFTToTest(pLogger, pDXVATestContext, &pActivateToTest);
        if(FAILED(hr))
        {
			LogTC(pLogger, HIGH, L"Error: failed to find the MFT to test, 0x%08X (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }
        
        hrSW = CTestMediaSessionPlayback::CreateInstance(pLogger, pDXVATestContext->dwRenderingMode, &pPlaybackSession);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Failed to create instance of a playback session: 0x%08X (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        hrSW = pPlaybackSession->SetDXVAMode(MFTOPOLOGY_DXVA_NONE);
        if(FAILED(hrSW))
        {
			LogTC(pLogger, HIGH, L"ERROR>> Failed to set DXVA Mode hr=0x%x (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        bCloseSession = TRUE;

        LogTC(pLogger, NORMAL, L"Successfully initialized playback session");

        sSWDumpFileName = DUMPDIRECTORY;
        sSWDumpFileName += pwszFileNameNoPath;
        sSWDumpFileName += L"_VisValPlaybackTest_SW.DUMP";

        DeleteFile(sSWDumpFileName);

        hrSW = pPlaybackSession->SetDumpMode(TRUE, sSWDumpFileName);
        if(FAILED(hrSW))
        {
            break;
        }

        hrSW = pPlaybackSession->SetBuildMode(TRUE, pActivateToTest);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: failed to set the IMFActivate on the playback session, 0x%08X (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        hrSW = pPlaybackSession->SetForceDecoderMT(TRUE, &guidSubType);
		if(FAILED(hrSW))
		{
			break;
		}

        hrSW = pPlaybackSession->SetAudioAllowed(FALSE);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set audio allowed: 0x%08X (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        hrSW = pPlaybackSession->SetHWAllowed(FALSE);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set HW Allowed: 0x%08X (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        if(pDXVATestContext->eVideoFormat == MPEG2)
        {
            hrSW = pPlaybackSession->SetAllowMFCPKSourceForMPEG2(
                TRUE
                );
            if(FAILED(hrSW))
            {
                LogTC(pLogger, HIGH, L"Error: Failed to set MFCPKSource on playback session, 0x%08X (%s)", hrSW, StringFromMFHRESULT(hrSW));
                break;
            }
        }

        hrSW = pPlaybackSession->BuildTopology(pDXVATestContext->csInFileName, TRUE);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: failed to build playback session: 0x%08X (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully built a playback topology");

        hrSW = pPlaybackSession->WaitForState(STATE_TOPOLOGY_READY, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hrSW))
        {
			LogTC(pLogger, HIGH, L"Error: cannot build a topology over %ld sec, assuming failure: 0x%08X (%s)", pDXVATestContext->dwTimeoutSec, hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        LogTC(pLogger, NORMAL, L"Topology in use:");
        if(FAILED(pPlaybackSession->WalkFullTopology(2)))
        {
            LogTC(pLogger, HIGH, L"Error: Failed to walk topology");	
        }

        LogTC(pLogger, NORMAL, L"Starting playback...");

        hrSW = pPlaybackSession->Start();
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: failed to start playback topology, 0x%08X (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        hrSW = pPlaybackSession->WaitForState(STATE_PLAYING, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hrSW))
        {
			LogTC(pLogger, HIGH, L"Error: call to start a media session succeeded, but session did not get to a running state over %ld sec, 0x%08X (%s)", pDXVATestContext->dwTimeoutSec, hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        LogTC(pLogger, NORMAL, L"Media session is in playing state, waiting for completion");

        hrSW = pPlaybackSession->WaitForState(STATE_STOPPED, pDXVATestContext->dwTimeoutSec);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: failed to wait for ending of the media session (stopping), 0x%08X (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        DWORD dwSWWidth = 0;
        DWORD dwSWHeight = 0;
        GUID guidSWSubType = GUID_NULL;
        BOOL bParamMismatch = FALSE;

        pPlaybackSession->GetDumpFileInfo(&dwSWWidth, &dwSWHeight, &guidSWSubType);

        pPlaybackSession->CheckForComponentReports();

        hrSW = pPlaybackSession->GetCallbackHRESULT();
        if(FAILED(hrSW))
        {
			LogTC(pLogger, HIGH, L"Error: Media Session recieved a failed event, 0x%x (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        hrSW = pPlaybackSession->Close();
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Failed to close playback session, 0x%08X (%s)", hrSW, StringFromMFHRESULT(hrSW));
            break;
        }

        bCloseSession = FALSE;

        WriteInfoFile(sSWDumpFileName, dwSWWidth, dwSWHeight, &guidSWSubType);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               Visual Validation
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if(((dwSWHeight == OVERSCAN_HEIGHT) || (dwHeight == OVERSCAN_HEIGHT)) && (dwSWHeight != dwHeight))
        {
            if((dwHeight != (dwSWHeight - OVERSCAN_VALUE)) && (dwSWHeight != (dwHeight - OVERSCAN_VALUE)))
            {
                bParamMismatch = TRUE;
            }
        }
        else if(dwSWHeight != dwHeight)
        {
            bParamMismatch = TRUE;					
        }

        if(dwSWWidth != dwWidth)
        {
            bParamMismatch = TRUE;
        }

        if(guidSWSubType != guidSubType)
        {
            if( (guidSubType != HW_OPAQUE_SUBTYPE)  ||
                (guidSWSubType != SW_OPAQUE_SUBTYPE))
            {
                bParamMismatch = TRUE;
            }
        }

        if(bParamMismatch == TRUE)
        {
            hr = E_FAIL;
            LogTC(pLogger, HIGH, L"Error: Parameter mismatch. Hardware and Software decodes don't match");
            LogTC(pLogger, HIGH, L"          Resolution: %ux%u (HW) vs %ux%u (SW)", dwWidth, dwHeight, dwSWWidth, dwSWHeight);
            break;
        }

        LogTC(pLogger, NORMAL, L"Info: Decodes completed, running Visual Validation comparison on output files");

        hr = PerformVisualValidation(pLogger, (const WCHAR*)sDumpFileName, (const WCHAR*)sSWDumpFileName, dwSWWidth, dwSWHeight, &guidSWSubType, 
            lfVVThreshold, lfFailPassRatio);
        if(FAILED(hr))
        {
            break;
        }

        // Release the sessions so the files can be deleted
        SAFERELEASE(pPlaybackSession);

        if(FAILED(hrSW))
        {
            hr = hrSW;
        }

    }while(FALSE);

    if(bCloseSession)
    {
        if(FAILED(pPlaybackSession->Close()))
        {
            LogTC(pLogger, HIGH, L"Error: failed to close playback session, 0x%08X (%s)", hr, StringFromMFHRESULT(hr));
        }
    }

    if(bShutdownMF)
    {
        if(FAILED(MFShutdown()))
        {
            LogTC(pLogger, HIGH, L"Error: failed to shutdown Media Foundation");
        }
    }

    do
    {
        BOOL    bWow64          = FALSE;
        DWORD   dwDeleteFiles   = 0;

        if(CCMFTDiagGlobalParams::g_pParams->bAlwaysCleanOutput != FALSE)
        {
            dwDeleteFiles   = 1;
        }

        IsWow64Process(GetCurrentProcess(), &bWow64);

        if(SUCCEEDED(hr))
        {
            if(FAILED(GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
		        DELETEFILESONSUCCEESSNAME, &dwDeleteFiles)))
            {
                dwDeleteFiles = 1;
            }
        }
        else
        {
            if(FAILED(GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
		        DELETEFILESONFAILURENAME, &dwDeleteFiles)))
            {
                dwDeleteFiles = 0;
            }
        }

        if(dwDeleteFiles == 0)
        {
            break;
        }

        DeleteFileW((const WCHAR*)sDumpFileName);
        DeleteFileW((const WCHAR*)sSWDumpFileName);
    }
    while(false);

    SAFERELEASE(pActivateToTest);
    SAFERELEASE(pPlaybackSession);

    return hr;
}

HRESULT RunDXVATest(DWORD dwTestID, ILogger* pLogger,  CXmlRootConfig* pXMLConfig)
{
    if(pLogger == NULL || pXMLConfig == NULL)
    {
        return E_POINTER;
    }
 
    HRESULT             hr                          = S_OK;
    CXmlConfig*         pTestConfig                 = NULL;
    CString             cstrTestID;
    CString             cstrTestName;
    CString             csDXVAType;
    CString             csArchOptional              = L"";
    DWORD               dwStartTDRs                 = MAX_DWORD;
    DWORD               dwStopTDRs                  = MAX_DWORD;
    DWORD               dwDXVA9or11                 = DXVA_11Only;
    UINT32              un32Val                     = 0;
    CDXVATestContext    DXVATestContext(NULL, NULL);
    //CScreenIdleControl  ScreenIdle;
    BOOL                bIsInArchOptionalList       = FALSE;

    do
    {
        if(pLogger == NULL)
        {
            hr = E_POINTER;
            break;
        }

        {
            RTL_OSVERSIONINFOW  windowsVersionInfo;
            windowsVersionInfo.dwOSVersionInfoSize  = sizeof(RTL_OSVERSIONINFOW);
            RtlGetVersion(&windowsVersionInfo);

            if( (windowsVersionInfo.dwMajorVersion > 6 )                                                ||
                ((windowsVersionInfo.dwMajorVersion == 6) && (windowsVersionInfo.dwMinorVersion >= 3))  )
            {
                SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
            }
        }

        /*hr = ScreenIdle.PreventScreenFromPoweringOff();
        if(FAILED(hr))
        {
			pLogger->Log(NORMAL, L"ERROR>> Failed to set prevent screen from idling off 0x%x (%s)", hr, StringFromMFHRESULT(hr));
            break;
        }*/

        pLogger->BeginTestCase(0, dwTestID);
        cstrTestID.Format(_T("%u"), dwTestID);

        pTestConfig = pXMLConfig->get_TestConfig(cstrTestID);
        if(pTestConfig == NULL)
        {
            pLogger->Log(HIGH, _T("ERROR>> Test %u not found"), dwTestID);
            hr = E_FAIL;
            break;
        }

        pTestConfig->GetString(L"Name",                     cstrTestName);
        pLogger->Log(FILE_ONLY, L"Test Name: %s",           cstrTestName);

        pTestConfig->GetString(L"InputType",                DXVATestContext.csInputMediaType);
        pTestConfig->GetString(L"InputFile",                DXVATestContext.csInFileName);
        pTestConfig->GetString(L"Category",                 DXVATestContext.csTestCategory);
        pTestConfig->GetString(L"Component_CLSID",          DXVATestContext.csComponentCLSID);
        pTestConfig->GetString(L"DXVAType",                 csDXVAType);
        un32Val                                         = 0;
        pTestConfig->GetUINT32(L"CSTestTimeout",            &un32Val);
        DXVATestContext.dwCSTestStartTime               = un32Val;
        pTestConfig->GetUINT32(L"DComp",                    &un32Val);
                DXVATestContext.bUseDComp               = (un32Val == 0) ? FALSE : TRUE;
        if(DXVATestContext.bUseDComp)
        {
            pTestConfig->GetUINT32(L"FullScreen",           &un32Val);
            DXVATestContext.bFullScreen         = (un32Val == 0) ? FALSE : TRUE;
        }
        pTestConfig->GetString(L"ArchOptional",             csArchOptional);
        if(pTestConfig->GetUINT32(L"H264SourceSendFakeMT", &un32Val) == S_OK)
        {
            DXVATestContext.bH264SendSampleDesc = (un32Val == 0) ? TRUE : FALSE;
            DXVATestContext.bH264SendFakeMT     = (un32Val == 0) ? FALSE : TRUE;
        }
        
        WCHAR*  pwszArchOptional    = NULL;

        do
        {
            if(_wcsicmp(csArchOptional, L"") == 0)
            {
                break;
            }

            pwszArchOptional    = _wcsdup((const WCHAR*)csArchOptional);
            if(pwszArchOptional == NULL)
            {
                hr = E_OUTOFMEMORY;
                pLogger->Log(HIGH, L"ERROR>> Out of memory, can't allocate optional OS string");
                break;
            }

            WCHAR*  pwszContext     = NULL;
            WCHAR*  pwszCurrArch    = wcstok_s(
                pwszArchOptional,
                L";",
                &pwszContext
                );
            while(pwszCurrArch != NULL)
            {
                if(_wcsicmp(
                    pwszCurrArch,
#if defined(_ARM_)
                    L"arm"
#elif defined(_ARM64_)
                    L"arm64"
#elif defined(_M_IX86)
                    L"x86"
#elif defined(_M_X64)
                    L"amd64"
#endif

                    ) == 0)
                {
                    bIsInArchOptionalList   = TRUE;
                    break;
                }

                pwszCurrArch    = wcstok_s(
                    NULL,
                    L";",
                    &pwszContext
                );
            }
        }while(false);

        SAFEFREE(pwszArchOptional);

        if(FAILED(hr))
        {
            break;
        }

        if(_wcsicmp((const WCHAR*)csDXVAType, L"9and11") == 0)
        {
            dwDXVA9or11 = DXVA_Both;
        }
        else if(_wcsicmp((const WCHAR*)csDXVAType, L"9") == 0)
        {
            dwDXVA9or11 = DXVA_9Only;
        }
        else if(_wcsicmp((const WCHAR*)csDXVAType, L"11") == 0)
        {
            dwDXVA9or11 = DXVA_11Only;
        }
        else if(_wcsicmp((const WCHAR*)csDXVAType, L"") != 0)
        {
            hr = E_FAIL;
            pLogger->Log(HIGH, L"ERROR>> %s is not a valid DXVAType (9, 11, or 9and11)");
            break;
        }

        hr = VideoFormatStringToEnum(&DXVATestContext);
        if(FAILED(hr))
        {
            pLogger->Log(NORMAL, L"ERROR>> %s is not a valid input type", (const WCHAR*)(DXVATestContext.csInputMediaType));
            break;
        }

        if(DXVATestContext.eVideoFormat == MPEG2)
        {
            hr = VerifyMPEG2Supported(pLogger);
            if(hr != S_OK)
            {
                break;
            }
        }
        else if((DXVATestContext.eVideoFormat == HEVC) || (DXVATestContext.eVideoFormat == HEVC10))
        {
            // HEVC is not required
            hr = VerifyHEVCSupported(pLogger, DXVATestContext.eVideoFormat);
            if(hr != S_OK)
            {
                if(SUCCEEDED(hr))
                {
                    pLogger->Log(NORMAL, L"INFO>> Video device does not support HEVC");
                }

                break;
            }

            // HEVC only supports VLD
            DXVATestContext.bRunAllModes    = FALSE;
        }
        
        if(FAILED(CountNumSystemTDRs(&dwStartTDRs)))
        {
            pLogger->Log(NORMAL, L"ERROR>> Failed to count number of system TDRs");
        }
        
        for(DWORD i = 0; i < 2; i++)
        {
            BOOL    bRunTest    = FALSE;
            HRESULT hrTest      = S_OK;

            switch(i)
            {
            case 0:
                {
                    if((dwDXVA9or11 & DXVA_11Only) != 0)
                    {
                        bRunTest    = TRUE;
                    }

                    DXVATestContext.dwRenderingMode = MFTDRENDERING_CUSTOMVIDRENDERER;
                }
                break;
            case 1:
                {
                    if((dwDXVA9or11 & DXVA_9Only) != 0)
                    {
                        bRunTest    = TRUE;
                    }

                    DXVATestContext.dwRenderingMode = MFTDRENDERING_EVR;
                }
                break;
            default:
                // Nothing to do
                break;
            };

            if(bRunTest == FALSE)
            {
                continue;
            }

            if(0 == _wcsicmp(DXVATestContext.csTestCategory, L"Playback"))
            {
                hrTest  = RunPlaybackTest(pLogger, (CMIContext*)&DXVATestContext);
                if(FAILED(hrTest))
                {
                    if(bIsInArchOptionalList == FALSE)
                    {
                        if(SUCCEEDED(hr))
                        {
                            hr  = hrTest;
                        }
                    }
                    else
                    {
                        LogTC(pLogger, NORMAL, L"INFO>> This test is not required to pass on this architecture");
                    }
                }
            }
            else if(0 == _wcsicmp(DXVATestContext.csTestCategory, L"MultiInstance"))
            {     
                UINT32 unNumInstances = 0;
                pTestConfig->GetUINT32(L"NumInstances", &unNumInstances);
                if(unNumInstances < 2)
                {
                    hr = E_UNEXPECTED;
                    break;
                }

                // In this case we have to copy down the input file first, so the multiple instances don't step on eachother's toes
                WCHAR* pwszExactInputFileLocation   = NULL;

                hrTest  = FindHCKContentFile(
                    (const WCHAR*)DXVATestContext.csInFileName,
                    &pwszExactInputFileLocation
                    );
                if(FAILED(hrTest))
                {
                    LogTC(pLogger, HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", (const WCHAR*)DXVATestContext.csInFileName, hrTest);

                    if(SUCCEEDED(hr))
                    {
                        hr  = hrTest;
                    }
                }
                else
                {
#pragma prefast(push)
#pragma prefast(disable:26006, "disabling warning for test code")
                    DXVATestContext.csInFileName    = pwszExactInputFileLocation;
#pragma prefast(pop)
                    
                    hrTest  = RunMITests((DWORD)unNumInstances, pLogger, (CMIContext*)&DXVATestContext, &RunMITest);
                    if(FAILED(hrTest))
                    {
                        if(bIsInArchOptionalList == FALSE)
                        {
                            if(SUCCEEDED(hr))
                            {
                                hr  = hrTest;
                            }
                        }
                    }
                    else
                    {
                        LogTC(pLogger, NORMAL, L"INFO>> This test is not required to pass on this architecture");
                    }
                }

                SAFEFREE(pwszExactInputFileLocation);
            }
            else if(0 == _wcsicmp(DXVATestContext.csTestCategory, L"Stress"))
            {
                UINT32 unRunTimeInSeconds = 0;
                pTestConfig->GetUINT32(L"RunTimeInSeconds", &unRunTimeInSeconds);
                if(unRunTimeInSeconds == 0)
                {
                    hr = E_UNEXPECTED;
                    break;
                }

                LogTC(pLogger, HIGH, L"INFO>> STRESS Test: %d second run time", unRunTimeInSeconds);

                DXVATestContext.dwRunTimeInSec = (DWORD)unRunTimeInSeconds;            
                DXVATestContext.bUseSmartSource = TRUE;

                hrTest  = RunStressTest(pLogger, (CMIContext*)&DXVATestContext);
                if(FAILED(hrTest))
                {
                    if(bIsInArchOptionalList == FALSE)
                    {
                        if(SUCCEEDED(hr))
                        {
                            hr  = hrTest;
                        }
                    }
                    else
                    {
                        LogTC(pLogger, NORMAL, L"INFO>> This test is not required to pass on this architecture");
                    }
                }
            }
            else if(0 == _wcsicmp(DXVATestContext.csTestCategory, L"Quality"))
            {
                pTestConfig->GetDouble(L"VVThreshold", &(DXVATestContext.lfVVThreshold));
                pTestConfig->GetDouble(L"VVFailPassRatio", &(DXVATestContext.lfFailPassRatio));

#if !defined(_ARM_) && !defined(_ARM64_)
                if(DXVATestContext.dwRenderingMode == MFTDRENDERING_EVR)
                {
                    if(dwDXVA9or11 == DXVA_9Only)
                    {
                        // WoA Quality Test, only DX9 was requested
                        LogTC(pLogger, HIGH, L"INFO>> On this platform, only DX11 will be used for quality tests. Changing Type to DX11");
                     
                        DXVATestContext.dwRenderingMode = MFTDRENDERING_CUSTOMVIDRENDERER;
                    }
                    else if(dwDXVA9or11 == DXVA_Both)
                    {
                        // WoA Quality Test, both DX11 and DX9 was requested
                        // Skip DX9 quality test on WoA
                        LogTC(pLogger, HIGH, L"INFO>> On this platform, only DX11 will be used for quality tests. Skipping DX9");
                        continue;
                    }
                }
#endif
            
                hrTest  = RunQualityTest(pLogger, &DXVATestContext);
                if(FAILED(hrTest))
                {
                    if(bIsInArchOptionalList == FALSE)
                    {
                        if(SUCCEEDED(hr))
                        {
                            hr  = hrTest;
                        }
                    }
                    else
                    {
                        LogTC(pLogger, NORMAL, L"INFO>> This test is not required to pass on this architecture");
                    }
                }
            }
            else
            {
                hr = E_FAIL;
            }

            // After running all the modes, the next DXVA mode should only run the "highest"
            DXVATestContext.bRunAllModes    = FALSE;
        }

        do
        {
            if(dwStartTDRs == MAX_DWORD)
            {
                // Failed to initially count TDRs
                break;
            }

            if(FAILED(CountNumSystemTDRs(&dwStopTDRs)))
            {
                pLogger->Log(NORMAL, L"ERROR>> Failed to count number of system TDRs");
                break;
            }

            const DWORD dwNumTDRs   = dwStopTDRs - dwStartTDRs;

            if(dwNumTDRs == 0)
            {
                // No TDRs detected
                break;
            }

            pLogger->Log(NORMAL, L"ERROR>> %u TDRs were detected during test", dwNumTDRs);

            if(SUCCEEDED(hr)) // We only want to change the return code if the test passed
            {
                hr = E_FAIL;
            }
        }while(false);

        if(FAILED(hr))
        {
            pLogger->Log(NORMAL, _T("ERROR>> Test %u Failed: 0x%08X (%s)"), dwTestID, hr, StringFromMFHRESULT(hr));
            pLogger->EndTestCase(TEST_FAILED);
        }
        else
        {
            pLogger->Log(NORMAL, _T("SUCCESS>> Test %u Passed"), dwTestID);
            pLogger->EndTestCase(TEST_PASSES);
        }

    }while(FALSE);

    SAFEDELETES(pTestConfig);

    return hr;
}