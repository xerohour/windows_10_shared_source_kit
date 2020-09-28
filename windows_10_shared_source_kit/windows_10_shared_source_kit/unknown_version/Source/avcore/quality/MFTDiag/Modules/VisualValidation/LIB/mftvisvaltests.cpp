#include "stdafx.h"
#include "MFTVisValTests.h"
#include "DXCompare.h"
#include "CTestMediaSessionPlayback.h"
#include "PerfHelper.h"
#include "LogTC.h"
#include <uuids.h>
#include <wmcodecdsp.h>
#include <mfds.h>
#include "CTestMediaSessionTranscode.h"
#include "TranscodeProfileReader.h"
#include "CConfigRegKey.h"
#include "Resource.h"
#include "MFTDiagCodecHelper.h"
#include "VerifyTestSupported.h"
#include "MediaTimeConverter.h"
#include "MFTDPerfHelper.h"
#include "MFTDFileHelpers.h"
#include "MFActivateHelper.h"
#include "CILoggerToIBasicLog.h"
#include "EncQualPerfWLK.h"
#include "VerificationEncodeHelper.h"
#include "MFTDiagCustomAttributes.h"
#include "FindHCKContentFile.h"
#include "IH264EncodeVerify.h"
//External encoder (HWMFT) tests
#include "hmftunit.h"
// Add more includes above this
#include "MFTVisValTests_Priv.h"

BOOL g_bUseCabacForQualityTests                 = FALSE;
BOOL g_bDisableMPEG4pt2DecoderPostProcessing    = FALSE;

#define E_SKIP                  _HRESULT_TYPEDEF_(0x800FF001L)

#define COMPARISON_BUFF_SIZE    102400
#define HW_OPAQUE_SUBTYPE       MFVideoFormat_420O
#define SW_OPAQUE_SUBTYPE       MFVideoFormat_NV12

typedef enum
{
    CONTAINER_TYPE_INVALID = 0,
    CONTAINER_TYPE_ASF, 
    CONTAINER_TYPE_MPEG4,
    CONTAINER_TYPE_MP3,
    CONTAINER_TYPE_3GP
} CONTAINER_TYPE;

struct sPSNR
{
    double lfY;
    double lfU;
    double lfV;
};

static const GUID CODECAPI_AVDecDisablePostProcessing = {0xf8749193, 0x667a, 0x4f2c, {0xa9, 0xe8, 0x5d, 0x4a, 0xf9, 0x24, 0xf0, 0x8f}};

#define MANUALADDVALUENAME              L"ManualAddMFT"
#define REFERENCEDECODERVALUENAME       L"RefDecoder"
#define DELETEFILESONSUCCEESSNAME       L"VVDeleteOnSuccess"
#define DELETEFILESONFAILURENAME        L"VVDeleteOnFailure"
#define DUMPPLAYBACKNAME                L"DumpPlayback"
#define VVTHRESHOLDNAME                 L"VVThreshold_x10"
#define VVTHRESHOLDSCALINGFACTOR        10.0 // Should match what's in name above
#define VVFAILPASSRATIONAME             L"VVFaillPassRatio_x10"
#define VVTFAILPASSRATIOSCALINGFACTOR   10.0 // Should match what's in name above
#define VVLOWERTHRESHOLDNAME            L"VVLowerThreshold_x10"
#define VVTLOWERHRESHOLDSCALINGFACTOR   10.0 // Should match what's in name above
#define PSNRYWEIGHTVALUENAME            L"PSNRYWeight"
#define PSNRUWEIGHTVALUENAME            L"PSNRUWeight"
#define PSNRVWEIGHTVALUENAME            L"PSNRVWeight"
#define DUMPDIRECTORY                   L".\\PlaybackVV\\"
#define VVTRANSCODEOUTPUTNAME           L"VisValTranscodeTest_"
#define REFERENCEFILEPOSTFIX            L"_REF"
#define TRANSCODEVV_OUT_DIR_NAME        L"TranscodeVV"
#define ENCODERQUALITY_OUT_DIR_NAME     L"EncoderQuality"
#define ENCODERQUALITYOUTPUTNAME        L"EncoderQualityTest_"
#define ENCODERSETTINGS_OUT_DIR_NAME    L"EncoderSettings"
#define ENCODERSETTINGSOUTPUTNAME       L"EncoderSettingsTest_"
#define ENCODERDYNAMICTYPE_OUT_DIR_NAME L"EncoderDynamicType"
#define ENCODERDYNAMICTYPEOUTPUTNAME    L"EncoderDynamicTypeTest_"
#define ENCODERLTR_OUT_DIR_NAME         L"EncoderLTR"
#define ENCODERLTROUTPUTNAME            L"EncoderLTRTest_"
#define ENCODERINTRA_REF_OUT_DIR_NAME   L"EncoderIntraRefresh"
#define ENCODERINTRA_REF_OUTPUTNAME     L"EncoderIntraRefreshTest_"
#define ALLOWHWENCNAME                  L"VVAllowHWEnc"
#define ALLOWHWDECNAME                  L"VVAllowHWDec"

#define DEFAULT_RSMTHRESHOLD                        0.0
#define DEFAULT_PSNRTHRESHOLD                       40.0
#define DEFAULT_FAILPASSRATIO                       0.1
#define OVERSCAN_HEIGHT                             1088
#define OVERSCAN_VALUE                              8
#define DEFAULT_TRANSCODE_PSNR_DIFF_THRESHOLD       1.5
#define DEFAULT_TRANSCODE_PSNR_DIFF_LOWER_THRESHOLD 1.0

#define DEFAULT_PSNR_Y_WEIGHT                       6
#define DEFAULT_PSNR_U_WEIGHT                       1
#define DEFAULT_PSNR_V_WEIGHT                       1

#define DEFAULT_BUFFER_SIZE 8294400 // (1920 * 1080 * 4), RGB32 @ 1920x1080

#ifdef SAFERELEASE
#undef SAFERELEASE
#endif

#ifdef SAFEDELETES
#undef SAFEDELETES
#endif

#ifdef SAFEDELETE
#undef SAFEDELETE
#endif

#ifdef SAFEFREE
#undef SAFEFREE
#endif

#define SAFERELEASE(x) \
    { \
        if( NULL != (x)) \
        { \
            (x)->Release(); \
            (x) = NULL; \
        } \
    }

#define SAFESPRELEASE(x) \
    { \
        if( NULL != (x)) \
        { \
            (x).Release(); \
            (x) = NULL; \
        } \
    }

#define SAFEDELETES(x) \
    { \
        if( NULL != (x)) \
        { \
            delete (x); \
            (x) = NULL; \
        } \
    }

#define SAFEDELETE(x) \
    { \
        if( NULL != (x)) \
        { \
            delete[] (x); \
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

#define SAFEFREE(x) \
    { \
        if( NULL != (x)) \
        { \
            free(x); \
            (x) = NULL; \
        } \
    }
#define SAFECOMFREE(x) \
    { \
        if((x) != NULL) \
        { \
            CoTaskMemFree(x); \
            (x) = NULL; \
        } \
    }

struct sSetupDecoderContext
{
public:
    ILogger*    pLogger;
};

// Forward declaraions - Most are defined in MFTVisValTests_Priv.h
static HRESULT SetOutputFileExt(__in IMFTranscodeProfile* pProfile, __out CONTAINER_TYPE *pContainerType)
{
    HRESULT hr = S_OK;
    IMFAttributes *pContAttrs = NULL;
    UINT32 cContAttrsCount = 0;
    MF_ATTRIBUTE_TYPE attrType = MF_ATTRIBUTE_IUNKNOWN;
    PROPVARIANT varAttrValue;
    GUID guidAttr = GUID_NULL;
    GUID guidAttrVal = GUID_NULL;
    *pContainerType = CONTAINER_TYPE_INVALID;

    if(!pProfile || !pContainerType)
    {
        return E_INVALIDARG;
    }

    PropVariantInit(&varAttrValue);

    do
    {
        hr = pProfile->GetContainerAttributes(&pContAttrs);
        if(FAILED(hr))
        {
            break;
        }

        hr = pContAttrs->LockStore();
        if(FAILED(hr))
        {
            break;
        }

        hr = pContAttrs->GetCount(&cContAttrsCount);
        if(FAILED(hr))
        {
            break;
        }        

        for (DWORD j = 0; j < cContAttrsCount; j++)
        {
            PropVariantClear(&varAttrValue);
            hr = pContAttrs->GetItemByIndex(j, &guidAttr, &varAttrValue);
            if(FAILED(hr))
            {
                break;
            }

            hr = pContAttrs->GetItemType(guidAttr, &attrType);
            if(FAILED(hr))
            {
                break;
            }

            if(MF_TRANSCODE_CONTAINERTYPE == guidAttr)
            {
                hr = pContAttrs->GetGUID(guidAttr, &guidAttrVal);
                if(FAILED(hr))
                {
                    break;
                }

                if(MFTranscodeContainerType_ASF == guidAttrVal)
                {
                    *pContainerType = CONTAINER_TYPE_ASF;
                }
                else if(MFTranscodeContainerType_MPEG4 == guidAttrVal)
                {
                    *pContainerType = CONTAINER_TYPE_MPEG4;
                }
                else if(MFTranscodeContainerType_MP3 == guidAttrVal)
                {
                    *pContainerType = CONTAINER_TYPE_MP3;
                }
                else if(MFTranscodeContainerType_3GP == guidAttrVal)
                {
                    *pContainerType = CONTAINER_TYPE_3GP;
                }
                else
                {
                    *pContainerType = CONTAINER_TYPE_INVALID;
                    hr = E_FAIL;
                    break;
                }
                break;
            }
        }

    } while(FALSE);

    pContAttrs->UnlockStore();
    SAFERELEASE(pContAttrs);
    PropVariantClear(&varAttrValue);

    return hr;
}

HRESULT GetPSNR(ILogger* pLogger, const WCHAR* pszTestFile, const WCHAR* pszRefFile, const DWORD dwWidth, const DWORD dwHeight, 
                const GUID* pguidSubtype, sPSNR* pspsnrResults)
{
    HRESULT hr = S_OK;
    CDXCompare* pdxComp = NULL;

    do
    {
        if((pLogger == NULL) || (pszTestFile == NULL) || (pszRefFile == NULL) ||(pguidSubtype == NULL))
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

        pdxComp->SetFrameCompleteCallback(&DXCPSNRFrameCompCallback, pLogger);

        hr = pdxComp->ValidateData(pszTestFile, pszRefFile, dwWidth, dwHeight, *pguidSubtype, 0, 0, 1.0);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: Visual Validation Failed during PSNR check");
            break;
        }
        
        pdxComp->GetPSNRValues(&(pspsnrResults->lfY), &(pspsnrResults->lfU), &(pspsnrResults->lfV));
    }while(false);

    SAFEDELETES(pdxComp);

    return hr;
}

double GetWeightedPSNR(ILogger* pLogger, const sPSNR* pspsnrPSNR)
{
    DWORD   dwYWeight   = DEFAULT_PSNR_Y_WEIGHT;
    DWORD   dwUWeight   = DEFAULT_PSNR_U_WEIGHT;
    DWORD   dwVWeight   = DEFAULT_PSNR_V_WEIGHT;
    BOOL    bWow64      = FALSE;
    double  lfPSNR      = 0.0;

    do
    {
        if((pLogger == NULL) || (pspsnrPSNR == NULL))
        {
            break;
        }

        IsWow64Process(GetCurrentProcess(), &bWow64);

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
            PSNRYWEIGHTVALUENAME, &dwYWeight);

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
            PSNRUWEIGHTVALUENAME, &dwUWeight);

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
            PSNRVWEIGHTVALUENAME, &dwVWeight);

        lfPSNR = (((dwYWeight * pspsnrPSNR->lfY) + (dwUWeight * pspsnrPSNR->lfU) + (dwVWeight * pspsnrPSNR->lfV)) / 
                    (dwYWeight + dwUWeight + dwVWeight));
    }while(false);

    return lfPSNR;
}

HRESULT VerifyPSNRvsFileSize(ILogger* pLogger, sPSNR* pspsnrTest, sPSNR* pspsnrRef, const UINT64 un64TestFileSize, const UINT64 un64RefFileSize, 
                             double lfPSNRThreshold)
{
    HRESULT hr                      = S_OK;
    double  lfPSNRDiff              = 0.0;
    double  lfPSNRLowerThreshold    = DEFAULT_TRANSCODE_PSNR_DIFF_LOWER_THRESHOLD;

    do
    {
        if((pspsnrTest == NULL) || (pspsnrRef == NULL))
        {
            hr = E_POINTER;
            break;
        }

        lfPSNRDiff = GetWeightedPSNR(pLogger, pspsnrTest) - GetWeightedPSNR(pLogger, pspsnrRef);

        DWORD dwVVLowerPSNROverride = 0;

        BOOL bWow64 = FALSE;

        IsWow64Process(GetCurrentProcess(), &bWow64);

        if(SUCCEEDED(GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
                        VVLOWERTHRESHOLDNAME, &dwVVLowerPSNROverride)))
        {
            lfPSNRLowerThreshold = (double)dwVVLowerPSNROverride / VVTLOWERHRESHOLDSCALINGFACTOR;
        }

        // First, ensure that the PSNR is not below the minimum threshold
        if(lfPSNRDiff < ((-lfPSNRThreshold) - lfPSNRLowerThreshold))
        {
            hr = E_FAIL;
            break;
        }

        // Now get the PSNR comparison results

        double lfMaxFileSize = (double)un64RefFileSize;

        if(lfPSNRDiff < 0.0)
        {
            // The PSNR of the test file is lower than that of the reference
            // For each .1dB lower PSNR, the file size is allowed to be 2% smaller and still pass (up to the threshold)

            for(double lfThresholdLeft = abs(((-lfPSNRDiff) < lfPSNRThreshold) ? lfPSNRDiff : lfPSNRThreshold); lfThresholdLeft >= 0.1; lfThresholdLeft -= 0.1)
            {
                lfMaxFileSize *= 0.98; // Remove 2% for each .1dB
            }

            // We don't have to worry about the PSNR being "too" low becuase that was checked at the beginning of this function
        }
        else
        {
            // The PSNR of the test file is higher than that of the reference
            // For each .1dB higher PSNR, the file size is allowed to be 2% larger and still pass (up to the threshold)

            for(double lfThresholdLeft = ((lfPSNRDiff > lfPSNRThreshold) ? lfPSNRThreshold : lfPSNRDiff); lfThresholdLeft >= 0.0; lfThresholdLeft -= 0.1)
            {
                lfMaxFileSize *= 1.02; // Add 2% for each .1dB
            }
        }

        pLogger->Log(FILE_ONLY, L"INFO>> PSNR difference is %04.04lf dB, Max passing file size is %I64u Bytes", lfPSNRDiff, (UINT64)lfMaxFileSize);

        if(un64TestFileSize > (UINT64)lfMaxFileSize)
        {
            hr = E_FAIL;
            break;
        }
    }while(false);

    return hr;
}

HRESULT SetupDecoder(
    IMFTransform*   pDecoderMFT,
    void*           pContext)
{
    HRESULT                 hr                      = S_OK;
    sSetupDecoderContext*   pSetupDecoderContext    = NULL;

    do
    {
        if( (pDecoderMFT == NULL)   ||
            (pContext == NULL)      )
        {
            hr = E_POINTER;
            break;
        }

        pSetupDecoderContext    = (sSetupDecoderContext*)pContext;

        if(g_bDisableMPEG4pt2DecoderPostProcessing != FALSE)
        {
            ICodecAPI*  pCodecAPI   = NULL;
            VARIANT     var         = {0};

            do
            {
                VariantInit(&var);

                hr = pDecoderMFT->QueryInterface(
                    IID_ICodecAPI,
                    (void**)&pCodecAPI
                    );
                if(FAILED(hr))
                {
                    pSetupDecoderContext->pLogger->Log(NORMAL, L"ERROR>> Failed to get CodecAPI from decoder (hr=0x%x)", hr);
                    break;
                }

                var.vt      = VT_UI4;
                var.ulVal   = 1;

                hr = pCodecAPI->SetValue(
                    &CODECAPI_AVDecDisablePostProcessing,
                    &var
                    );
                if(FAILED(hr))
                {
                    pSetupDecoderContext->pLogger->Log(NORMAL, L"ERROR>> Failed to set CODECAPI_AVDecDisablePostProcessing on decoder (hr=0x%x)", hr);
                    break;
                }

                pSetupDecoderContext->pLogger->Log(NORMAL, L"INFO>> Post-Processing is disabled on MPEG4pt2 Decoder");
            }while(false);

            SAFERELEASE(pCodecAPI);
            VariantClear(&var);

            if(FAILED(hr))
            {
                break;
            }
        }
    }while(false);

    return hr;
}

HRESULT VerifyVisValE2E(ILogger* pLogger, IConfig* pConfig, IMFActivate* pMFTActivate)
{
    HRESULT hr = S_OK;
    CString sVisValType;

    do
    {
        if((NULL == pLogger) || (NULL == pConfig) || (NULL == pMFTActivate))
        {
            hr = E_POINTER;
            break;
        }

        hr = pConfig->GetString(L"VisValType", sVisValType);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find VisVal Type :0x%x", hr);
            break;
        }

        if(0 == _tcsicmp(sVisValType, L"Playback"))
        {
            hr = VerifyVisValPlayback(pLogger, pConfig, pMFTActivate);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderQuality"))
        {
            hr = VerifyEncoderQuality(pLogger, pConfig, pMFTActivate);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderFastQuality"))
        {
            hr = VerifyEncoderWinBlue(pLogger, pConfig, pMFTActivate, BLUE_TEST_RDCURVE);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderSettings"))
        {
            hr = VerifyEncoderSettings(pLogger, pConfig, pMFTActivate);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderSettingsPerSample"))
        {
            hr = VerifyEncoderSettingsPerSample(pLogger, pConfig, pMFTActivate);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderDynamicType"))
        {
            hr = VerifyEncoderDynamicType(pLogger, pConfig, pMFTActivate);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderDynSinkWriter"))
        {
            hr = VerifyEncoderWinBlue(pLogger, pConfig, pMFTActivate, BLUE_TEST_DYNSINKWRITER);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderLTR"))
        {
            hr = VerifyEncoderWinBlue(pLogger, pConfig, pMFTActivate);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderLTRRDCurve"))
        {
            hr = VerifyEncoderLTRRDCurve(pLogger, pConfig, pMFTActivate);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderTemporalLayers"))
        {
            hr = VerifyEncoderWinBlue(pLogger, pConfig, pMFTActivate);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderWinBlue"))
        {
            hr = VerifyEncoderWinBlue(pLogger, pConfig, pMFTActivate);
        }
        else if(0 == _tcsicmp(sVisValType, L"EncoderROI"))
        {
            hr = VerifyEncoderWinBlue(pLogger, pConfig, pMFTActivate, BLUE_TEST_ROI);
        }
        else if (0 == _tcsicmp(sVisValType, L"EncoderGradualIntraRefresh"))
        {
            hr = VerifyEncoderGradualIntraRefresh(pLogger, pConfig, pMFTActivate);
        }
        else
        {
            hr = E_FAIL;
            LogTC(pLogger, HIGH, L"Test Error: %s is not a valid VisValType", (const WCHAR*)sVisValType);
        }
    }while(false);

    return hr;
}

#pragma prefast(push)
#pragma prefast(disable:26006, "disabling warning for test code")

HRESULT VerifyVisValPlayback(ILogger* pLogger, IConfig* pConfig, IMFActivate* pMFTActivate)
{
    HRESULT hr = S_OK;

#ifndef MINWIN

    DWORD dwTimeoutSec = INFINITE;
    CString sInFilename;
    CString sTestCaseName;
    CString sDumpFileName;
    CString sSWDumpFileName;
    HRESULT hrExpectedResult;    // expected HR for TranscodeAPI

    CTestMediaSessionPlayback*  pPlaybackSession        = NULL;
    BOOL                        bCancelTest             = FALSE;
    BOOL                        bCloseSession           = FALSE;
    BOOL                        bTargetMFTFound         = FALSE;
    BOOL                        bNoRender               = FALSE;
    BOOL                        bMFTD3DAware            = FALSE;
    BOOL                        bHWAllowed              = TRUE;
    HRESULT                     hrSW                    = S_OK;
    GUID                        guidSubType             = GUID_NULL;
    GUID                        guidRefDecoder          = GUID_NULL;
    DWORD                       dwManualAdd             = 1;
    DWORD                       dwWidth                 = 0;
    DWORD                       dwHeight                = 0;
    DWORD                       dwRenderingMode         = MFTDRENDERING_CUSTOMVIDRENDERER;
    double                      flRate                  = 0;
    double                      lfVVThreshold           = DEFAULT_PSNRTHRESHOLD;
    double                      lfFailPassRatio         = DEFAULT_FAILPASSRATIO;
    DWORD                       dwSeek                  = 0;
    GUID                        guidInSubType           = GUID_NULL;
    IMFAttributes*              pInputAttributes        = NULL;
    IMFAttributes*              pAttr                   = NULL;
    MFT_REGISTER_TYPE_INFO*     pMediaTypes             = NULL;
    WCHAR*                      pszHardwareURL          = NULL;
    WCHAR*                      pwszContentFileName     = NULL;
    sSetupDecoderContext        ssdcContext             = {0};

    PERF_LOAD(PERF_LOAD_DEFAULT);

    do
    {
        BOOL bWow64 = FALSE;

        IsWow64Process(GetCurrentProcess(), &bWow64);

        hr = pConfig->GetString(L"Description", sTestCaseName);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find Test Case Description :0x%x", hr);
        }

        LogTC(pLogger, HIGH, L"\nStarting Test: %s", (const WCHAR*)sTestCaseName);

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"System Error: failed in MFStartup(): 0x%08X", hr);
            break;
        }

        hr = pMFTActivate->QueryInterface(IID_IMFAttributes, reinterpret_cast<VOID**>(&pAttr));
        if(FAILED(hr))
        {
            LogTC(pLogger,HIGH, L"MF ERROR>> Query Interface(IID_IMFAttributes) failed :0x%x", hr);
            break;
        }
        if(!pAttr)
        {
            hr = E_POINTER;
            LogTC(pLogger,HIGH, L"MF ERROR>> IMFAttribute is null :0x%x", hr);
            break;
        }

        GUID guidCategory = GUID_NULL;
        hr = pAttr->GetGUID(MF_TRANSFORM_CATEGORY_Attribute, &guidCategory);
        if(FAILED(hr))
        {
            LogTC(pLogger,HIGH, L"MF ERROR>> MF_TRANSFORM_CATEGORY_Attribute not found :0x%x", hr);
            break;
        }

        UINT32 un32TransformAttrib = 0;
        hr = pAttr->GetUINT32(MF_TRANSFORM_FLAGS_Attribute, &un32TransformAttrib);
        if(FAILED(hr))
        {
            LogTC(pLogger,HIGH, L"MF ERROR>> MF_TRANSFORM_FLAGS_Attribute not found :0x%x", hr);
            break;
        }
        
        if(guidCategory != MFT_CATEGORY_VIDEO_DECODER)
        {
            hr = S_SKIPPED;
            LogTC(pLogger, NORMAL, L"INFO>> This test is not applicable to this component (it is not a video decoder)");
            break;
        }

        if((un32TransformAttrib & MFT_ENUM_FLAG_TRANSCODE_ONLY) != 0)
        {
            bNoRender = TRUE;
        }

        DWORD dwDumpPlayback = 0;

        if(SUCCEEDED(GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
            DUMPPLAYBACKNAME, &dwDumpPlayback)))
        {
            if(dwDumpPlayback == 0)
            {
                bNoRender = FALSE;
            }
            else
            {
                bNoRender = TRUE;
            }
        }

        BOOL bContinueTest = FALSE;

        do
        {
            UINT32 un32MediaTypeSize = 0;

            hr = pAttr->GetBlobSize(MFT_INPUT_TYPES_Attributes, &un32MediaTypeSize);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> MFT_INPUT_TYPES_Attributes not found: 0x%x", hr);
                break;
            }

            pMediaTypes = (MFT_REGISTER_TYPE_INFO*)malloc(sizeof(BYTE) * un32MediaTypeSize);
            if(pMediaTypes == NULL)
            {
                hr = E_OUTOFMEMORY;
                LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate MediaType info blob");
                break;
            }

            hr = MFCreateAttributes(&pInputAttributes, 1);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to allocate Test Attribute Store: 0x%x", hr);
                break;
            }

            hr = pAttr->GetBlob(MFT_INPUT_TYPES_Attributes, (BYTE*)pMediaTypes, un32MediaTypeSize, NULL);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_INPUT_TYPES_Attributes: 0x%x", hr);
                break;
            }

            if(FAILED(pConfig->GetAttributesFromNode(L"InputType", pInputAttributes)))
            {
                LogTC(pLogger, HIGH, L"TEST ERROR>> Failed to get InputType attribute from XML");
                hr = E_FAIL;
                break;
            }

            hr = VerifyTestSupported(pMediaTypes, un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO), pInputAttributes);
            if(hr != S_OK)
            {
                if(hr == S_FALSE)
                {
                    hr = S_SKIPPED;
                    LogTC(pLogger, NORMAL, L"INFO>> This test is not applicable as the component doesn't support the media type");
                    break;
                }
                else
                {
                    LogTC(pLogger, HIGH, L"WARNING>> Media Type support verification failed: 0x%x", hr);
                    LogTC(pLogger, HIGH, L"WARNING>> Test will be run");
                    hr = S_OK;
                }
            }

            bContinueTest = TRUE;
        }while(false);

        SAFEFREE(pMediaTypes);

        if(FAILED(hr) || (bContinueTest == FALSE))
        {
            break;
        }

        IMFTransform* pTransform = NULL;
        IMFAttributes* pTransformAttrib = NULL;

        do
        {
            hr = pMFTActivate->ActivateObject(IID_IMFTransform, (void**)&pTransform);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"ERROR>> Failed to activate Target MFT: 0x%x", hr);
                break;
            }

            hr = pTransform->GetAttributes(&pTransformAttrib);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"ERROR>> Failed to get Attributes from Target MFT: 0x%x", hr);
                break;
            }

            bMFTD3DAware = MFGetAttributeUINT32(pTransformAttrib, MF_SA_D3D_AWARE, (UINT32)FALSE);

            UINT32 un32Merit = MFGetAttributeUINT32(pAttr, MFT_CODEC_MERIT_Attribute, 0);

            UINT32 un32HardwareURLLen = 0;
            if((un32Merit != 0) && SUCCEEDED(pTransformAttrib->GetStringLength(MFT_ENUM_HARDWARE_URL_Attribute, &un32HardwareURLLen)))
            {
                pszHardwareURL = new WCHAR[++un32HardwareURLLen];
                if(pszHardwareURL == NULL)
                {
                    LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate HardwareURL string");
                    hr = E_OUTOFMEMORY;
                    break;
                }

                memset(pszHardwareURL, 0, sizeof(WCHAR) * un32HardwareURLLen);

                hr = pTransformAttrib->GetString(MFT_ENUM_HARDWARE_URL_Attribute, pszHardwareURL, un32HardwareURLLen, NULL);
                if(FAILED(hr))
                {
                    LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_ENUM_HARDWARE_URL_Attribute: 0x%x", hr);
                    break;
                }

                bHWAllowed = TRUE;

                LogTC(pLogger, HIGH, L"Starting Test for Hardware Component");
            }
            else
            {
                LogTC(pLogger, NORMAL, L"INFO>> Software component under test (Merrit 0 or no MFT_ENUM_HARDWARE_URL_Attribute found), turning off HW MFTs");

                bHWAllowed = FALSE;
            }
        }while(false);

        if(pTransform != NULL)
        {
            pTransform->Release();
            pMFTActivate->ShutdownObject();
        }

        SAFERELEASE(pTransformAttrib);

        if(FAILED(hr))
        {
            break;
        }

        RegHWSWOverload(pLogger);

        hr = pConfig->GetString(L"InputFile", sInFilename);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find InputFile :0x%x", hr);
            break;
        }

        pConfig->GetUINT32(L"TimeoutSec", (UINT*)&dwTimeoutSec);
        if(dwTimeoutSec == 0)
        {
            dwTimeoutSec = INFINITE;
        }

        pConfig->GetUINT32(L"ManualAdd", (UINT*)&dwManualAdd); // Don't care if this passes or not, default is true
        pConfig->GetGUID(L"RefDecoder", &guidRefDecoder); // Don't care if this passes or not, default is no reference
        pConfig->GetDouble(L"Rate", &flRate); // if fails, rate is not set
        pConfig->GetUINT32(L"Seek", (UINT*)&dwSeek); // if fails, seek is not set

        if((bNoRender == TRUE) && ((flRate != 0.0) || (dwSeek != 0)))
        {
        
            LogTC(pLogger, NORMAL, L"INFO>> This test does not apply to this component (it is a transcode only decoder)");
            hr = S_SKIPPED;
            break;
        }

        if(pConfig->GetDouble(L"VVThreshold", &lfVVThreshold) != S_OK)
        {
            lfVVThreshold = DEFAULT_PSNRTHRESHOLD;
        }
        if(pConfig->GetDouble(L"VVFailPassRatio", &lfFailPassRatio) != S_OK)
        {
            lfFailPassRatio = DEFAULT_FAILPASSRATIO;
        }

        DWORD dwVVOverride = 0;

        if(SUCCEEDED(GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
                        VVTHRESHOLDNAME, &dwVVOverride)))
        {
            lfVVThreshold = (double)dwVVOverride / VVTHRESHOLDSCALINGFACTOR;
        }

        dwVVOverride = 0;

        if(SUCCEEDED(GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
                        VVFAILPASSRATIONAME, &dwVVOverride)))
        {
            lfFailPassRatio = (double)dwVVOverride / VVTFAILPASSRATIOSCALINGFACTOR;
        }
        
        hr = pInputAttributes->GetGUID(MF_MT_SUBTYPE, &guidInSubType);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Failed in getting subtype from the input type :0x%x", hr);
            break;
        }

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
            MANUALADDVALUENAME, &dwManualAdd);
        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
            REFERENCEDECODERVALUENAME, &guidRefDecoder);

        if(bNoRender == TRUE)
        {
            dwRenderingMode = MFTDRENDERING_FILE;
        }

        hr = FindHCKContentFile(
            (const WCHAR*)sInFilename,
            &pwszContentFileName
            );
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", (const WCHAR*)sInFilename, hr);
            break;
        }

        hr = CTestMediaSessionPlayback::CreateInstance(pLogger, dwRenderingMode, &pPlaybackSession);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Failed to create instance of a playback session: 0x%08X", hr);
            break;
        }

        if(bMFTD3DAware == FALSE)
        {
            hr = pPlaybackSession->SetDXVAMode(MFTOPOLOGY_DXVA_NONE);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"ERROR>> Failed to set DXVA Mode hr=0x%x)", hr);
                break;
            }
        }
        else
        {
            hr = pPlaybackSession->SetDXVAMode(MFTOPOLOGY_DXVA_FULL);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"ERROR>> Failed to set DXVA Mode hr=0x%x)", hr);
                break;
            }
        }

        LogTC(pLogger, NORMAL, L"Successfully initialized playback session");
        bCloseSession = TRUE;

        hr = pPlaybackSession->SetBuildMode((dwManualAdd == 0) ? FALSE : TRUE, pMFTActivate);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Failed to set build mode on playback session");
            break;
        }

        CString strID;

        if(FAILED(pConfig->GetAttribute(L"testId", strID)))
        {
            strID = "{ID_Unknown}";
        }

        sDumpFileName = DUMPDIRECTORY;
        sDumpFileName += L"VisValPlaybackTest_";
        sDumpFileName += strID;
        sDumpFileName += L".DUMP";

        DeleteFile(sDumpFileName);

        if(bNoRender == FALSE)
        {
            hr = pPlaybackSession->SetDumpMode(TRUE, sDumpFileName);
            if(FAILED(hr))
            {
                break;
            }
        }
        else
        {
            hr = pPlaybackSession->SetDumpFileName((const WCHAR*)sDumpFileName);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Test ERROR>> Unable to set dump file name");
                break;
            }
        }

        if(guidRefDecoder == GUID_NULL)
        {
            hr = E_FAIL;
            LogTC(pLogger, HIGH, L"Visual Validation Mode is turned on, but no reference decoder is specified");
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

        if(flRate != 0)
        {
            hr = pPlaybackSession->SetRateMode(flRate);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed to set rate on the playback session: 0x%08X", hr);
                break;
            }

            LogTC(pLogger, NORMAL, L"Set Playback Rate to %f", flRate);
        }

        if(dwSeek > 0)
        {
            hr = pPlaybackSession->SetSeekMode(dwSeek);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed to set seek mode on the playback session: 0x%08X", hr);
                break;
            }
        }

        hr = pPlaybackSession->SetAudioAllowed(FALSE);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set audio allowed: 0x%08X", hr);
            break;
        }

        hr = pPlaybackSession->SetHWAllowed(bHWAllowed);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set HW Allowed: 0x%08X", hr);
            break;
        }


        hr = pPlaybackSession->BuildTopology(CString(pwszContentFileName), TRUE);
        if(hr == S_SKIPPED)
        {
            LogTC(pLogger, HIGH, L"INFO>> Decoder does not support inputype, skipping test");
            break;
        }
        else if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to build playback session: 0x%08X", hr);
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully built a playback topology");

        hr = pPlaybackSession->WaitForState(STATE_TOPOLOGY_READY, dwTimeoutSec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: cannot build a topology over %lu sec, assuming failure: 0x%08X", dwTimeoutSec, hr);
            break;
        }

        hr = pConfig->GetUINT32(L"ExpectedHR", (UINT*) &hrExpectedResult);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to find ExpectedHR, 0x%08X", hr);
            break;
        }

        if(dwTimeoutSec == INFINITE)
        {
            UINT64 ullDuration = 0;

            if(SUCCEEDED(pPlaybackSession->GetDuration(&ullDuration)))
            {
                dwTimeoutSec = (DWORD)(MediaTimeToMiliSeconds(ullDuration) * GetTimeoutMultiplier(pLogger));
                if(dwTimeoutSec == 0)
                {
                    dwTimeoutSec = INFINITE;
                }
            }
        }

        LogTC(pLogger, NORMAL, L"Topology in use:");
        if(pPlaybackSession->WalkFullTopology(2, pMFTActivate, pszHardwareURL) == S_OK)
        {
            bTargetMFTFound = TRUE;
        }
        else
        {
            LogTC(pLogger, NORMAL, L"INFO>> Target MFT was not used in this test");
            LogTC(pLogger, NORMAL, L"INFO>> Skipping test as it is not applicable");
            hr = S_SKIPPED;
            break;
        }

        if(flRate != 0)
        {
            hr = pPlaybackSession->SetRate();
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed in setting the rate on the toplogy, 0x%08X", hr);
                break;
            }
        }

        /*
        if(dwSeek > 0)
        {   
            IMFTopologyNode* pNode = NULL;

            do
            {
                hr = pPlaybackSession->GetNodeInterfaceBySubtype(&guidInSubType, NULL, &pNode);
                if(FAILED(hr))
                {
                    LogTC(pLogger, HIGH, L"Error: failed in getting the node interface, 0x%08X", hr);
                    break;
                }

                hr = pNode->SetUINT32(MF_TOPONODE_DECODER, (UINT32)TRUE);
                if(FAILED(hr))
                {
                    LogTC(pLogger, HIGH, L"Error: failed in setting MF_TOPONODE_DECODER, 0x%08X", hr);
                    break;
                }

                hr = pNode->SetUINT32(MF_TOPONODE_MARKIN_HERE, (UINT32)TRUE);
                if(FAILED(hr))
                {
                    LogTC(pLogger, HIGH, L"Error: failed in setting MF_TOPONODE_MARKIN_HERE, 0x%08X", hr);
                    break;
                }

                hr = pNode->SetUINT32(MF_TOPONODE_MARKOUT_HERE, (UINT32)TRUE);
                if(FAILED(hr))
                {
                    LogTC(pLogger, HIGH, L"Error: failed in setting MF_TOPONODE_MARKOUT_HERE, 0x%08X", hr);
                    break;
                }

            }while(FALSE);

            SAFERELEASE(pNode);

            if(FAILED(hr))
            {
                break;
            }
        }
        */

        PERF_START(PERF_LOAD_DEFAULT,L"Hardware :"+sTestCaseName);
        
        LogTC(pLogger, NORMAL, L"Starting playback...");

        hr = pPlaybackSession->Start();
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to start playback topology, 0x%08X", hr);
            break;
        }

        hr = pPlaybackSession->WaitForState(STATE_PLAYING, dwTimeoutSec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: call to start a media session succeeded, but session did not get to a running state over %ld sec, 0x%08X", dwTimeoutSec, hr);
            break;
        }

        LogTC(pLogger, NORMAL, L"Media session is in playing state, waiting for completion");

        if(dwSeek > 0)
        {
            hr = pPlaybackSession->Seek();
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Error: failed in seek during playback: 0x%08X", hr);
                break;
            }
        }

        hr = pPlaybackSession->WaitForState(STATE_STOPPED, dwTimeoutSec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: failed to wait for ending of the media session (stopping), 0x%08X", hr);
            break;
        }

        pPlaybackSession->GetDumpFileInfo(&dwWidth, &dwHeight, &guidSubType);
        
        hr = pPlaybackSession->Close();
        bCloseSession = FALSE;

        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Failed to close playback session, 0x%08X", hr);
            break;
        }

        PERF_STOP(PERF_STOP_DEFAULT);

        hr = pPlaybackSession->GetCallbackHRESULT();
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Error: Media Session recieved a failed event, 0x%x", hr);
            break;
        }

        if(pPlaybackSession->ShouldFailTest() != FALSE)
        {
            hr = E_FAIL;
            break;
        }

        SAFERELEASE(pPlaybackSession);

        WriteInfoFile(sDumpFileName, dwWidth, dwHeight, &guidSubType);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               Software Comparison Playback
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        LogTC(pLogger, HIGH, L"Starting Test for Software Comparison Component");

        PERF_START(PERF_LOAD_DEFAULT,L"Software :"+sTestCaseName);
        
        hrSW = CTestMediaSessionPlayback::CreateInstance(pLogger, dwRenderingMode, &pPlaybackSession);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Failed to create instance of a playback session: 0x%08X", hrSW);
            break;
        }

        hrSW = pPlaybackSession->SetDXVAMode(MFTOPOLOGY_DXVA_NONE);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set DXVA Mode hr=0x%x)", hrSW);
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully initialized playback session");
        bCloseSession = TRUE;

        sSWDumpFileName = DUMPDIRECTORY;
        sSWDumpFileName += L"VisValPlaybackTest_";
        sSWDumpFileName += strID;
        sSWDumpFileName += L"_SW.DUMP";

        DeleteFile(sSWDumpFileName);

        if(bNoRender == FALSE)
        {
            hrSW = pPlaybackSession->SetDumpMode(TRUE, sSWDumpFileName);
            if(FAILED(hrSW))
            {
                break;
            }
        }
        else
        {
            hrSW = pPlaybackSession->SetDumpFileName((const WCHAR*)sSWDumpFileName);
            if(FAILED(hrSW))
            {
                LogTC(pLogger, HIGH, L"Test ERROR>> Unable to set dump file name");
                break;
            }
        }

        IMFActivate* pRefDecMFTActivate = NULL;

        do
        {
            hrSW = GetActivateForGUID(&guidRefDecoder, NULL, &pRefDecMFTActivate);
            if(FAILED(hrSW))
            {
                LogTC(pLogger, HIGH, L"Unable to find reference decoder (hr=0x%x)", hrSW);
                break;
            }

            hrSW = pPlaybackSession->SetBuildMode(TRUE, pRefDecMFTActivate);
            if(FAILED(hrSW))
            {
                LogTC(pLogger, NORMAL, L"Failed to set build mode on playback session");
                break;
            }
        }while(false);

        SAFERELEASE(pRefDecMFTActivate);

        if(FAILED(hrSW))
        {
            break;
        }

        if(flRate != 0)
        {
            hrSW = pPlaybackSession->SetRateMode(flRate);
            if(FAILED(hrSW))
            {
                LogTC(pLogger, HIGH, L"Error: failed to set rate on the playback session: 0x%08X", hrSW);
                break;
            }

            LogTC(pLogger, NORMAL, L"Set Playback Rate to %f", flRate);
        }

        if(dwSeek > 0)
        {
            hrSW = pPlaybackSession->SetSeekMode(dwSeek);
            if(FAILED(hrSW))
            {
                LogTC(pLogger, HIGH, L"Error: failed to set seek mode on the playback session: 0x%08X", hrSW);
                break;
            }
        }

        hrSW = pPlaybackSession->SetForceDecoderMT(TRUE, &guidSubType);
        if(FAILED(hrSW))
        {
            break;
        }

        hrSW = pPlaybackSession->SetAudioAllowed(FALSE);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set audio allowed: 0x%08X", hrSW);
            break;
        }

        hr = pPlaybackSession->SetHWAllowed(FALSE);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set HW Allowed: 0x%08X", hr);
            break;
        }

        if(g_bDisableMPEG4pt2DecoderPostProcessing != FALSE)
        {
            ssdcContext.pLogger = pLogger;

            hr = pPlaybackSession->SetPostManualInsertDecoderCallback(
                &SetupDecoder,
                &ssdcContext
                );
            if(FAILED(hr))
            {
                break;
            }
        }

        hrSW = pPlaybackSession->BuildTopology(CString(pwszContentFileName), TRUE);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: failed to build playback session: 0x%08X", hrSW);
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully built a playback topology");

        hrSW = pPlaybackSession->WaitForState(STATE_TOPOLOGY_READY, dwTimeoutSec);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: cannot build a topology over %ld sec, assuming failure: 0x%08X", dwTimeoutSec, hrSW);
            break;
        }

        hrSW = pConfig->GetUINT32(L"ExpectedHR", (UINT*) &hrExpectedResult);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: failed to find ExpectedHR, 0x%08X", hrSW);
            break;
        }

        LogTC(pLogger, NORMAL, L"Topology in use:");
        if(FAILED(pPlaybackSession->WalkFullTopology(2)))
        {
            LogTC(pLogger, HIGH, L"Failed to walk topology");	
        }

        if(flRate != 0)
        {
            hrSW = pPlaybackSession->SetRate();
            if(FAILED(hrSW))
            {
                LogTC(pLogger, HIGH, L"Error: failed in setting the rate on the toplogy, 0x%08X", hrSW);
                break;
            }
        }

        /*
        if(dwSeek > 0)
        {   
            IMFTopologyNode* pNode = NULL;

            do
            {
                hrSW = pPlaybackSession->GetNodeInterfaceBySubtype(&guidInSubType, NULL, &pNode);
                if(FAILED(hrSW))
                {
                    LogTC(pLogger, HIGH, L"Error: failed in getting the node interface, 0x%08X", hrSW);
                    break;
                }

                hrSW = pNode->SetUINT32(MF_TOPONODE_DECODER, (UINT32)TRUE);
                if(FAILED(hrSW))
                {
                    LogTC(pLogger, HIGH, L"Error: failed in setting MF_TOPONODE_DECODER, 0x%08X", hrSW);
                    break;
                }

                hrSW = pNode->SetUINT32(MF_TOPONODE_MARKIN_HERE, (UINT32)TRUE);
                if(FAILED(hrSW))
                {
                    LogTC(pLogger, HIGH, L"Error: failed in setting MF_TOPONODE_MARKIN_HERE, 0x%08X", hrSW);
                    break;
                }

                hrSW = pNode->SetUINT32(MF_TOPONODE_MARKOUT_HERE, (UINT32)TRUE);
                if(FAILED(hrSW))
                {
                    LogTC(pLogger, HIGH, L"Error: failed in setting MF_TOPONODE_MARKOUT_HERE, 0x%08X", hrSW);
                    break;
                }

            }while(FALSE);

            SAFERELEASE(pNode);

            if(FAILED(hrSW))
            {
                break;
            }
        }
        */

        LogTC(pLogger, NORMAL, L"Starting playback...");

        hrSW = pPlaybackSession->Start();
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: failed to start playback topology, 0x%08X", hrSW);
            break;
        }

        hrSW = pPlaybackSession->WaitForState(STATE_PLAYING, dwTimeoutSec);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: call to start a media session succeeded, but session did not get to a running state over %ld sec, 0x%08X", dwTimeoutSec, hrSW);
            break;
        }

        LogTC(pLogger, NORMAL, L"Media session is in playing state, waiting for completion");

        if(dwSeek > 0)
        {
            hrSW = pPlaybackSession->Seek();
            if(FAILED(hrSW))
            {
                LogTC(pLogger, HIGH, L"Error: failed in seek during playback: 0x%08X", hrSW);
                break;
            }
        }

        hrSW = pPlaybackSession->WaitForState(STATE_STOPPED, dwTimeoutSec);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: failed to wait for ending of the media session (stopping), 0x%08X", hrSW);
            break;
        }

        DWORD dwSWWidth = 0;
        DWORD dwSWHeight = 0;
        GUID guidSWSubType = GUID_NULL;
        BOOL bParamMismatch = FALSE;

        pPlaybackSession->GetDumpFileInfo(&dwSWWidth, &dwSWHeight, &guidSWSubType);

        hrSW = pPlaybackSession->Close();
        bCloseSession = FALSE;

        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Failed to close playback session, 0x%08X", hrSW);
            break;
        }

        PERF_STOP(PERF_STOP_DEFAULT);

        hrSW = pPlaybackSession->GetCallbackHRESULT();
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: Media Session recieved a failed event, 0x%x", hrSW);
            break;
        }

        WriteInfoFile(sSWDumpFileName, dwSWWidth, dwSWHeight, &guidSWSubType);

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
            LogTC(pLogger, HIGH, L"          SubType: %s (HW) vs %s (SW)", SubTypeToString(&guidSubType), 
                SubTypeToString(&guidSWSubType));
            break;
        }

        LogTC(pLogger, NORMAL, L"Info: Decodes completed, running Visual Validation comparison on output files");

        hr = PerformVisualValidation(pLogger, sDumpFileName, sSWDumpFileName, dwSWWidth, dwSWHeight, &guidSWSubType, 
            lfVVThreshold, lfFailPassRatio);
        if(FAILED(hr))
        {
            break;
        }
    }while(false);

    do
    {
        DWORD   dwDeleteFiles   = 0;
        BOOL    bWow64          = FALSE;

        if(CCMFTDiagGlobalParams::g_pParams->bAlwaysCleanOutput != FALSE)
        {
            dwDeleteFiles   = 1;
        }

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

        // Release the sessions so the files can be deleted
        SAFERELEASE(pPlaybackSession);

        DeleteFileW((const WCHAR*)sDumpFileName);
        DeleteFileW((const WCHAR*)sSWDumpFileName);
    }while(false);

    if(FAILED(hrSW))
    {
        hr = hrSW;
    }

    if(TRUE == bCloseSession && NULL != pPlaybackSession)
    {
        HRESULT hrClose = pPlaybackSession->Close();
        if(FAILED(hrClose))
        {
            LogTC(pLogger, HIGH, L"Error: failed to close playback session, 0x%08X", hrClose);
        }
    }

    LogTC(pLogger, NORMAL, L"Media session completed, calling MFShutdown()");

    if(SUCCEEDED(hr) && (bTargetMFTFound == TRUE))
    {
        hr = PERF_POSTPROCESS(PERF_POSTPROCESS_DEFAULT);

        HandlePerfLogs(pLogger, hr, 2);

        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"This playback session did not meet the performance requirements");
            if(SUCCEEDED(hr))
            {
                hr = E_FAIL;
            }
        }
    }
    else
    {
        PERF_STOP(PERF_STOP_DEFAULT);
    }
    
    SAFEFREE(pMediaTypes);
    SAFERELEASE(pAttr);
    SAFEDELETE(pszHardwareURL);
    SAFERELEASE(pInputAttributes);
    SAFERELEASE(pPlaybackSession);
    SAFEFREE(pwszContentFileName);

    MFShutdown();

    if((hr != S_SKIPPED) && SUCCEEDED(hr) && (bTargetMFTFound == FALSE))
    {
        hr = S_FALSE;
    }
#endif //#ifndef MINWIN

    return hr;
}

static HRESULT PerformVisualValidation(ILogger* pLogger, const WCHAR* pszTestFileName, const WCHAR* pszRefFileName,
                                const DWORD dwWidth, const DWORD dwHeight, const GUID* pGUIDSubType,
                                double lfPSNRThreshold, double lfFailPassRatio)
{
    HRESULT hr = S_OK;
    CDXCompare* pdxComp = NULL;

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

        hr = pdxComp->ValidateData(pszTestFileName, pszRefFileName, dwWidth, dwHeight, *pGUIDSubType, 
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

            LogTC(pLogger, HIGH, L"Error: Visual Validation Failed:");
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

HRESULT GetTestOutputFileName(ILogger* pLogger, const WCHAR* pwszOutputSubdirectoryName, const WCHAR* pwszOutputNamePrefix,
                              const WCHAR* pszTestID, const WCHAR* pszNameExtension, CPath& cpFileName)
{
    HRESULT     hr              = S_OK;
    WCHAR*      pszCurrDir      = NULL;
    DWORD       dwCurrDirLength = 0;

    do
    {
        if(pLogger == NULL)
        {
            hr = E_POINTER;
            break;
        }

        dwCurrDirLength = GetCurrentDirectory(0, NULL);
        
        pszCurrDir = new WCHAR[dwCurrDirLength];
        if(pszCurrDir == NULL)
        {
            hr = E_OUTOFMEMORY;
            LogTC(pLogger, HIGH, L"Test ERROR>> Out of memory, can't allocate buffer for current directory");
            break;
        }

        GetCurrentDirectory(dwCurrDirLength, pszCurrDir);

        if(pwszOutputSubdirectoryName != NULL)
        {
#pragma prefast(push)
#pragma prefast(disable:26035, "disabling warning for test code")

            cpFileName.Combine(pszCurrDir, pwszOutputSubdirectoryName);

#pragma prefast(pop)
        }

        DWORD dwAttr = GetFileAttributes(cpFileName);
        if (INVALID_FILE_ATTRIBUTES == dwAttr || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY)) 
        {
            if (NULL == CreateDirectory(cpFileName , NULL)) 
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                LogTC(pLogger, HIGH, L"Test ERROR>> Unable to create directory for output file: 0x%x", hr);
                break;
            }
        }

        cpFileName.m_strPath.AppendFormat(L"\\%s%s%s", (pwszOutputNamePrefix != NULL) ? pwszOutputNamePrefix : L"",
            pszTestID, pszNameExtension);
    }while(false);

    SAFEDELETE(pszCurrDir);

    return hr;
}

const WCHAR* SubTypeToString(GUID* pSubType)
{
    if(pSubType == NULL)
    {
        return L"None";
    }
    else if(*pSubType == MEDIASUBTYPE_NV12)
    {
        return L"MEDIASUBTYPE_NV12";
    }
    else if(*pSubType == MEDIASUBTYPE_YV12)
    {
        return L"MEDIASUBTYPE_YV12";
    }
    else if(*pSubType == MEDIASUBTYPE_I420)
    {
        return L"MEDIASUBTYPE_I420";
    }
    else if(*pSubType == MEDIASUBTYPE_IYUV)
    {
        return L"MEDIASUBTYPE_IYUV";
    }
    else if(*pSubType == MEDIASUBTYPE_YUY2)
    {
        return L"MEDIASUBTYPE_YUY2";
    }
    else if(*pSubType == MEDIASUBTYPE_UYVY)
    {
        return L"MEDIASUBTYPE_UYVY";
    }
    else if(*pSubType == MEDIASUBTYPE_RGB24)
    {
        return L"MEDIASUBTYPE_RGB24";
    }
    else if(*pSubType == MEDIASUBTYPE_RGB32)
    {
        return L"MEDIASUBTYPE_RGB32";
    }
    else if(*pSubType == MEDIASUBTYPE_RGB555)
    {
        return L"MEDIASUBTYPE_RGB555";
    }
    else if(*pSubType == MEDIASUBTYPE_RGB565)
    {
        return L"MEDIASUBTYPE_RGB565";
    }
    else if(*pSubType == MEDIASUBTYPE_RGB8)
    {
        return L"MEDIASUBTYPE_RGB8";
    }
    else if(*pSubType == MEDIASUBTYPE_RGB4)
    {
        return L"MEDIASUBTYPE_RGB4";
    }
    else
    {
        return L"Unknown";
    }
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

static BOOL DXCPSNRFrameCompCallback(void* pContext, DWORD dwCurrFrame, DWORD dwNumFailed, UINT64 un64CurrBytesProcessed, 
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

        wprintf_s(L"%s\r", pszProgressBar);
    }

    return TRUE;
}


HRESULT RunVisValTest(const DWORD dwTestID, ILogger* pLogger, CXmlRootConfig* pXMLConfig, IMFActivate* pActivate)
{
    HRESULT             hr          = S_OK;
    CXmlConfig*         pTestConfig = NULL;
    //CScreenIdleControl  ScreenIdle;

    do
    {
        if((pLogger == NULL) || (pXMLConfig == NULL) || (pActivate == NULL))
        {
            hr = E_POINTER;
            break;
        }

        /*hr = ScreenIdle.PreventScreenFromPoweringOff();
        if(FAILED(hr))
        {
            pLogger->Log(NORMAL, L"ERROR>> Failed to set prevent screen from idling off (hr=0x%x)", hr);
            break;
        }
*/
        pLogger->BeginTestCase(0, dwTestID);

        CString cstrTestID;
        cstrTestID.Format(_T("%u"), dwTestID);

        pTestConfig = pXMLConfig->get_TestConfig(cstrTestID);
        if(pTestConfig == NULL)
        {
            pLogger->Log(HIGH, _T("ERROR>> Test %u not found"), dwTestID);
            hr = E_FAIL;
            break;
        }

        COMPLETION_TYPE eCompletionType = TEST_PASSES;

        do
        {
            ULONGLONG ullMinFreeSpace       = 0;
            ULONGLONG ullFreeSpaceAvailable = 0;

            hr = CheckForFreeDiskSpace(&ullMinFreeSpace, &ullFreeSpaceAvailable);
            if(FAILED(hr))
            {
                pLogger->Log(HIGH, L"ERROR>> There is not enough free space on the disk to run this test (available: %I64u bytes, minimum required: %I64u bytes)",
                    ullFreeSpaceAvailable, ullMinFreeSpace);
                break;
            }

            hr = VerifyVisValE2E(pLogger, pTestConfig, pActivate);
        }while(false);


        if (E_SKIP == hr)
        {
            hr = S_OK;
            eCompletionType = TEST_SKIPPED;
        }
        else if(FAILED(hr))
        {
            pLogger->Log(NORMAL, _T("ERROR>> Test %u Failed"), dwTestID);
            eCompletionType = TEST_FAILED;
        }
        else if(hr == S_SKIPPED)
        {
            eCompletionType = TEST_SKIPPED;
        }
        else if(hr == S_FALSE)
        {
            eCompletionType = TEST_MFTNOTUSED;

            pLogger->Log(NORMAL, _T("SUCCESS>> Test %u Passed, but Target MFT was not found"), dwTestID);
        }
        else
        {
            pLogger->Log(NORMAL, _T("SUCCESS>> Test %u Passed"), dwTestID);
        }

        pLogger->EndTestCase(eCompletionType);
    }while(false);

    SAFERELEASE(pTestConfig);

    return hr;
}

HRESULT InitializeVisValTestTable(CXmlRootConfig** ppXMLRootConfig)
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
            (TCHAR*)&InitializeVisValTestTable, &hMod) == FALSE)
        {
            hr = E_FAIL;
            break;
        }

        HRSRC hRsrc = FindResource(hMod, MAKEINTRESOURCE(IDR_VISVALCONFIG), L"Config");
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

HRESULT VerifyEncoderQuality(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate)
{
    HRESULT                 hr                  = S_OK;
    BOOL                    bMFStarted          = FALSE;
    GUID                    guidMFT             = GUID_NULL;
    GUID                    guidSubType         = GUID_NULL;
    WCHAR*                  pwszHWURL           = NULL;
    UINT32                  un32HWURLLen        = 0;
    CMFActivateHelper*      pActivateHelper     = NULL;
    IEncoderQualityWLK*     pEncoderQualityWLK  = NULL;
    CString                 sInFileName;
    CString                 sTestID;
    CString                 sConfig;
    CPath                   cpOutFileName;
    IBasicLog*              pBasicLog           = NULL;
    WCHAR*                  pwszContentFileName = NULL;

    do
    {
        if((pLogger == NULL) || (pConfig == NULL) || (pActivate == NULL))
        {
            hr = E_POINTER;
            break;
        }

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to startup MF (hr0x%x)", hr);
            break;
        }

        BOOL                    bContinueTest       = FALSE;
        IMFAttributes*          pOutputAttributes   = NULL;
        MFT_REGISTER_TYPE_INFO* pMediaTypes         = NULL;

        do
        {
            hr = pConfig->GetString(L"Config", sConfig);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Test Error: Failed to find Config node in XML :0x%x", hr);
                break;
            }

            UINT32 un32MediaTypeSize = 0;

            hr = pActivate->GetBlobSize(MFT_OUTPUT_TYPES_Attributes, &un32MediaTypeSize);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> MFT_OUTPUT_TYPES_Attributes not found: 0x%x", hr);
                break;
            }

            pMediaTypes = (MFT_REGISTER_TYPE_INFO*)malloc(sizeof(BYTE) * un32MediaTypeSize);
            if(pMediaTypes == NULL)
            {
                hr = E_OUTOFMEMORY;
                LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate MediaType info blob");
                break;
            }

            hr = MFCreateAttributes(&pOutputAttributes, 1);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to allocate Test Attribute Store: 0x%x", hr);
                break;
            }

            hr = pActivate->GetBlob(MFT_OUTPUT_TYPES_Attributes, (BYTE*)pMediaTypes, un32MediaTypeSize, NULL);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_OUTPUT_TYPES_Attributes: 0x%x", hr);
                break;
            }

            if(FAILED(pConfig->GetAttributesFromNode(L"OutputType", pOutputAttributes)))
            {
                LogTC(pLogger, HIGH, L"TEST ERROR>> Failed to get InputType attribute from XML");
                hr = E_FAIL;
                break;
            }

            hr = VerifyTestSupported(pMediaTypes, un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO), pOutputAttributes);
            if(hr != S_OK)
            {
                if(hr == S_FALSE)
                {
                    hr = S_SKIPPED;
                    LogTC(pLogger, NORMAL, L"INFO>> This test is not applicable as the component doesn't support the media type");
                    break;
                }
                else
                {
                    LogTC(pLogger, HIGH, L"WARNING>> Media Type support verification failed: 0x%x", hr);
                    LogTC(pLogger, HIGH, L"WARNING>> Test will be run");
                    hr = S_OK;
                }
            }

            hr = pOutputAttributes->GetGUID(MF_MT_SUBTYPE, &guidSubType);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"TEST ERROR>> Failed to get Subtype from XML");
                break;
            }

            bContinueTest = TRUE;
        }while(false);

        SAFEFREE(pMediaTypes);
        SAFERELEASE(pOutputAttributes);

        if(FAILED(hr) || (bContinueTest == FALSE))
        {
            break;
        }

        bMFStarted = TRUE;

        hr = pConfig->GetString(L"InputFile", sInFileName);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find InputFile (hr=0x%x)", hr);
            break;
        }

        if(FAILED(pConfig->GetAttribute(L"testId", sTestID)))
        {
            sTestID = "{ID_Unknown}";
        }

        hr = GetTestOutputFileName(pLogger, ENCODERQUALITY_OUT_DIR_NAME, ENCODERQUALITYOUTPUTNAME,
            sTestID, L"", cpOutFileName);
        if(FAILED(hr))
        {
            break;
        }

        hr = pActivate->GetGUID(MFT_TRANSFORM_CLSID_Attribute, &guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Activate object does not have a transform CLSID (hr=0x%x)", hr);
            break;
        }

        if(pActivate->GetAllocatedString(MFT_ENUM_HARDWARE_URL_Attribute, &pwszHWURL, &un32HWURLLen) == E_OUTOFMEMORY)
        {
            hr = E_OUTOFMEMORY;
            LogTC(pLogger, NORMAL, L"Error: Out of memory, can't allocate HW URL String (hr=0x%x)", hr);
            break;
        }

        hr = PrintMFTUnderTestInfo(pLogger,pActivate,guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: PrintMFTUnderTestInfo Failed (hr=0x%x)", hr);
            break;
        }

        pActivateHelper = new CMFActivateHelper();
        if(pActivateHelper == NULL)
        {
            LogTC(pLogger, NORMAL, L"Error: Out of memory, can't allocate activation helper class");
            hr = E_OUTOFMEMORY;
            break;
        }

        hr = CILoggerToIBasicLog::CreateInstance(pLogger, &pBasicLog);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to create basic log wrapper object (hr=0x%x)", hr);
            break;
        }

        hr = pActivateHelper->InitializeActivateByGUID(&guidMFT, pwszHWURL);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to initialize activation helper class (hr=0x%x)", hr);
            break;
        }

        hr = FindHCKContentFile(
            (const WCHAR*)sInFileName,
            &pwszContentFileName
            );
        if(FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", (const WCHAR*)sInFileName, hr);
            break;
        }

        hr = CreateEncoderQualityModule(pBasicLog, pBasicLog, &pEncoderQualityWLK);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to create encoder quality module (hr=0x%x)", hr);
            break;
        }        

        hr = pEncoderQualityWLK->Initialize(pActivateHelper, cpOutFileName, (const WCHAR*)pwszContentFileName, (const WCHAR*)sConfig);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to initialize encoder quality module (hr=0x%x)", hr);
            break;
        }

        hr = pEncoderQualityWLK->SetEncoderFormat(&guidSubType);
        if(FAILED(hr))
        {
            break;
        }

        hr = pEncoderQualityWLK->EvaluateHWEncoder(&guidMFT, pwszHWURL);
        if(FAILED(hr))
        {
            break;
        }
    }while(false);

    do
    {
        DWORD dwDeleteFiles = 0;
        BOOL bWow64 = FALSE;

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

        DeleteDirectoryWithFiles((const WCHAR*)cpOutFileName);
    }while(false);

    if(pwszHWURL != NULL)
    {
        CoTaskMemFree(pwszHWURL);
    }
    SAFEDELETES(pActivateHelper);
    SAFERELEASE(pEncoderQualityWLK);
    SAFERELEASE(pBasicLog);
    SAFEFREE(pwszContentFileName);

    if(bMFStarted != FALSE)
    {
        MFShutdown();
    }

    return hr;
}

HRESULT VerifyEncoderSettingsPerSample(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate)
{
    HRESULT                     hr                  = S_OK;
    BOOL                        bMFStarted          = FALSE;
    GUID                        guidMFT             = GUID_NULL;
    WCHAR*                      pwszHWURL           = NULL;
    UINT32                      un32HWURLLen        = 0;
    IVerificationEncodeHelper*  pEncVerifier        = NULL;
    ITAEFConfigHelper*          pConfigHelper       = NULL;
    CString                     csTestID;
    CString                     sInFileName;
    CString                     sConfig;
    CPath                       cpOutFileName;
    IBasicLog*                  pBasicLog           = NULL;
    EncodingParameters*         pepEncodingParam    = NULL;
    WCHAR*                      pwszContentFileName = NULL;

    do
    {
        if((pLogger == NULL) || (pConfig == NULL) || (pActivate == NULL))
        {
            hr = E_POINTER;
            break;
        }

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to startup MF (hr0x%x)", hr);
            break;
        }
        hr = pConfig->GetString(L"Config", sConfig);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find Config node in XML :0x%x", hr);
            break;
        }
        BOOL                    bContinueTest       = FALSE;
        IMFAttributes*          pOutputAttributes   = NULL;
        MFT_REGISTER_TYPE_INFO* pMediaTypes         = NULL;
        do
        {            

            UINT32 un32MediaTypeSize = 0;

            hr = pActivate->GetBlobSize(MFT_OUTPUT_TYPES_Attributes, &un32MediaTypeSize);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> MFT_OUTPUT_TYPES_Attributes not found: 0x%x", hr);
                break;
            }

            pMediaTypes = (MFT_REGISTER_TYPE_INFO*)malloc(sizeof(BYTE) * un32MediaTypeSize);
            if(pMediaTypes == NULL)
            {
                hr = E_OUTOFMEMORY;
                LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate MediaType info blob");
                break;
            }

            hr = MFCreateAttributes(&pOutputAttributes, 1);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to allocate Test Attribute Store: 0x%x", hr);
                break;
            }

            hr = pActivate->GetBlob(MFT_OUTPUT_TYPES_Attributes, (BYTE*)pMediaTypes, un32MediaTypeSize, NULL);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_OUTPUT_TYPES_Attributes: 0x%x", hr);
                break;
            }

            if(FAILED(pConfig->GetAttributesFromNode(L"OutputType", pOutputAttributes)))
            {
                LogTC(pLogger, HIGH, L"TEST ERROR>> Failed to get InputType attribute from XML");
                hr = E_FAIL;
                break;
            }

            hr = VerifyTestSupported(pMediaTypes, un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO), pOutputAttributes);
            if(hr != S_OK)
            {
                if(hr == S_FALSE)
                {
                    hr = S_SKIPPED;
                    LogTC(pLogger, NORMAL, L"INFO>> This test is not applicable as the component doesn't support the media type");
                    break;
                }
                else
                {
                    LogTC(pLogger, HIGH, L"WARNING>> Media Type support verification failed: 0x%x", hr);
                    LogTC(pLogger, HIGH, L"WARNING>> Test will be run");
                    hr = S_OK;
                }
            }

            bContinueTest = TRUE;
        }while(false);

        SAFEFREE(pMediaTypes);
        SAFERELEASE(pOutputAttributes);

        if(FAILED(hr) || (bContinueTest == FALSE))
        {
            break;
        }

        bMFStarted = TRUE;

        hr = pConfig->GetString(L"InputFile", sInFileName);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find InputFile (hr=0x%x)", hr);
            break;
        }

        hr = pActivate->GetGUID(MFT_TRANSFORM_CLSID_Attribute, &guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Activate object does not have a transform CLSID (hr=0x%x)", hr);
            break;
        }

        if(pActivate->GetAllocatedString(MFT_ENUM_HARDWARE_URL_Attribute, &pwszHWURL, &un32HWURLLen) == E_OUTOFMEMORY)
        {
            hr = E_OUTOFMEMORY;
            LogTC(pLogger, NORMAL, L"Error: Out of memory, can't allocate HW URL String (hr=0x%x)", hr);
            break;
        }

        hr = PrintMFTUnderTestInfo(pLogger,pActivate,guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: PrintMFTUnderTestInfo Failed (hr=0x%x)", hr);
            break;
        }

        hr = CILoggerToIBasicLog::CreateInstance(pLogger, &pBasicLog);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to create basic log wrapper object (hr=0x%x)", hr);
            break;
        }
                      
        hr = CreateTAEFConfigHelperFromString(pBasicLog, NULL, &pConfigHelper, L"Out", L"In", sConfig);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Failed to retrieve test settings from string, hr = 0x%08X", hr);
            break;
        }

        hr = FindHCKContentFile(
            (const WCHAR*)sInFileName,
            &pwszContentFileName
            );
        if(FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", (const WCHAR*)sInFileName, hr);
            break;
        }

        hr = CreateVerificationEncodeHelper(pConfigHelper, pBasicLog, &pEncVerifier);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to create encoder verifier module (hr=0x%x)", hr);
            break;
        }

        pepEncodingParam = new EncodingParameters();
        if(pepEncodingParam == NULL)
        {
            hr = E_OUTOFMEMORY;
            LogTC(pLogger, NORMAL, L"Error: Out of memory, failed to create encoder parameters (hr=0x%x)", hr);
            break;
        }

        memset(pepEncodingParam, 0, sizeof(EncodingParameters));

        hr = ConvertTranscodeProfileToEncodingParameters(pLogger, pConfig, pepEncodingParam);
        if(FAILED(hr))
        {
            break;
        }

        if(FAILED(pConfig->GetAttribute(L"testId", csTestID)))
        {
            csTestID = L"{TestIDUnknown}";
        }

        hr = GetTestOutputFileName(pLogger, ENCODERSETTINGS_OUT_DIR_NAME, ENCODERSETTINGSOUTPUTNAME, 
            csTestID, L"", cpOutFileName);
        if(FAILED(hr))
        {
            break;
        }
        CreateDirectory(cpOutFileName, NULL);
        if((pepEncodingParam->gSubtype == MFVideoFormat_WVC1) || (pepEncodingParam->gSubtype == MFVideoFormat_WMV3)){
            cpOutFileName.m_strPath.Append(L"test.asf");
        }
        else{
            cpOutFileName.m_strPath.Append(L"test.mp4");
        }

       

        hr = pEncVerifier->SetSources((const WCHAR*)pwszContentFileName, (DWORD)wcslen(pwszContentFileName),
            (const WCHAR*)cpOutFileName, (DWORD)cpOutFileName.m_strPath.GetLength());
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to set sources on encoder verifier module (hr=0x%x)", hr);
            break;
        }
        LogTC(pLogger, NORMAL, L"Info: Input File %s", sInFileName);
        LogTC(pLogger, NORMAL, L"Info: Output File %s", cpOutFileName);

        hr = pEncVerifier->SetEncoderProperties(guidMFT, pepEncodingParam);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to set encoder properties (hr=0x%x)", hr);
            break;
        }

        hr = pEncVerifier->ProcessSamples(guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed in encode (hr=0x%x)", hr);
            break;
        }

        hr = pEncVerifier->StartVerification();
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed in verification (hr=0x%x)", hr);
            break;
        }

    }while(false);

    do
    {
        DWORD dwDeleteFiles = 0;
        BOOL bWow64 = FALSE;

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

        if(FAILED(GetTestOutputFileName(pLogger, ENCODERSETTINGS_OUT_DIR_NAME, ENCODERSETTINGSOUTPUTNAME, 
            csTestID, L"", cpOutFileName)))
        {
            break;
        }

        DeleteDirectoryWithFiles((const WCHAR*)cpOutFileName);
    }while(false);

    if(pwszHWURL != NULL)
    {
        CoTaskMemFree(pwszHWURL);
    }
    SAFERELEASE(pConfigHelper);
    SAFERELEASE(pEncVerifier);
    SAFERELEASE(pBasicLog);
    SAFEDELETES(pepEncodingParam);
    SAFEFREE(pwszContentFileName);

    if(bMFStarted != FALSE)
    {
        MFShutdown();
    }

    return hr;
}

HRESULT VerifyEncoderDynamicType(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate)
{
    HRESULT                     hr                  = S_OK;
    BOOL                        bMFStarted          = FALSE;
    GUID                        guidMFT             = GUID_NULL;
    WCHAR*                      pwszHWURL           = NULL;
    WCHAR*                      pwszContentFileName = NULL;
    UINT32                      un32HWURLLen        = 0;
    CString                     csTestID;
    CString                     sInFileName;
    CString                     sConfig;
    CPath                       cpOutFileName;
    IBasicLog*                  pBasicLog           = NULL;
    IH264EncodeVerify*          p264EncVerify       = NULL;

    do
    {
        if((pLogger == NULL) || (pConfig == NULL) || (pActivate == NULL))
        {
            hr = E_POINTER;
            break;
        }

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to startup MF (hr0x%x)", hr);
            break;
        }

        hr = pConfig->GetString(L"Config", sConfig);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find Config node in XML :0x%x", hr);
            break;
        }

        BOOL                    bContinueTest       = FALSE;
        IMFAttributes*          pOutputAttributes   = NULL;
        MFT_REGISTER_TYPE_INFO* pMediaTypes         = NULL;
        do
        {            
            UINT32 un32MediaTypeSize = 0;

            hr = pActivate->GetBlobSize(MFT_OUTPUT_TYPES_Attributes, &un32MediaTypeSize);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> MFT_OUTPUT_TYPES_Attributes not found: 0x%x", hr);
                break;
            }

            pMediaTypes = (MFT_REGISTER_TYPE_INFO*)malloc(sizeof(BYTE) * un32MediaTypeSize);
            if(pMediaTypes == NULL)
            {
                hr = E_OUTOFMEMORY;
                LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate MediaType info blob");
                break;
            }

            hr = MFCreateAttributes(&pOutputAttributes, 1);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to allocate Test Attribute Store: 0x%x", hr);
                break;
            }

            hr = pActivate->GetBlob(MFT_OUTPUT_TYPES_Attributes, (BYTE*)pMediaTypes, un32MediaTypeSize, NULL);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_OUTPUT_TYPES_Attributes: 0x%x", hr);
                break;
            }

            if(FAILED(pConfig->GetAttributesFromNode(L"OutputType", pOutputAttributes)))
            {
                LogTC(pLogger, HIGH, L"TEST ERROR>> Failed to get InputType attribute from XML");
                hr = E_FAIL;
                break;
            }

            hr = VerifyTestSupported(pMediaTypes, un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO), pOutputAttributes);
            if(hr != S_OK)
            {
                if(hr == S_FALSE)
                {
                    hr = S_SKIPPED;
                    LogTC(pLogger, NORMAL, L"INFO>> This test is not applicable as the component doesn't support the media type");
                    break;
                }
                else
                {
                    LogTC(pLogger, HIGH, L"WARNING>> Media Type support verification failed: 0x%x", hr);
                    LogTC(pLogger, HIGH, L"WARNING>> Test will be run");
                    hr = S_OK;
                }
            }

            bContinueTest = TRUE;
        }while(false);

        SAFEFREE(pMediaTypes);
        SAFERELEASE(pOutputAttributes);

        if(FAILED(hr) || (bContinueTest == FALSE))
        {
            break;
        }

        bMFStarted = TRUE;

        hr = pConfig->GetString(L"InputFile", sInFileName);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find InputFile (hr=0x%x)", hr);
            break;
        }

        hr = pActivate->GetGUID(MFT_TRANSFORM_CLSID_Attribute, &guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Activate object does not have a transform CLSID (hr=0x%x)", hr);
            break;
        }

        if(pActivate->GetAllocatedString(MFT_ENUM_HARDWARE_URL_Attribute, &pwszHWURL, &un32HWURLLen) == E_OUTOFMEMORY)
        {
            hr = E_OUTOFMEMORY;
            LogTC(pLogger, NORMAL, L"Error: Out of memory, can't allocate HW URL String (hr=0x%x)", hr);
            break;
        }

        hr = PrintMFTUnderTestInfo(pLogger,pActivate,guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: PrintMFTUnderTestInfo Failed (hr=0x%x)", hr);
            break;
        }

        hr = CILoggerToIBasicLog::CreateInstance(pLogger, &pBasicLog);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to create basic log wrapper object (hr=0x%x)", hr);
            break;
        }
                      
        hr = FindHCKContentFile(
            (const WCHAR*)sInFileName,
            &pwszContentFileName
            );
        if(FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", (const WCHAR*)sInFileName, hr);
            break;
        }

        if(FAILED(pConfig->GetAttribute(L"testId", csTestID)))
        {
            csTestID = "{ID_Unknown}";
        }

        hr = GetTestOutputFileName(pLogger, ENCODERDYNAMICTYPE_OUT_DIR_NAME, ENCODERDYNAMICTYPEOUTPUTNAME, 
            csTestID, L"", cpOutFileName);
        if(FAILED(hr))
        {
            break;
        }

        CreateDirectory(cpOutFileName, NULL);

        WEX::Common::String strTestOutputPath = WEX::Common::String(cpOutFileName.m_strPath);

        cpOutFileName.m_strPath.Append(L"test.mp4");

        hr = CreateH264EncodeVerify(
            pwszContentFileName,
            (const WCHAR*)cpOutFileName.m_strPath,
            (const WCHAR*)sConfig,
            pBasicLog,
            &p264EncVerify
            );
        if(FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to initialize test object (hr=0x%x)", hr);
            break;
        }

        hr = p264EncVerify->RunDynamicTypeScenario(strTestOutputPath.GetBuffer());
        if(FAILED(hr))
        {
            break;
        }
    }while(false);

    do
    {
        DWORD dwDeleteFiles = 0;
        BOOL bWow64 = FALSE;

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

        if(FAILED(GetTestOutputFileName(pLogger, ENCODERSETTINGS_OUT_DIR_NAME, ENCODERSETTINGSOUTPUTNAME, 
            csTestID, L"", cpOutFileName)))
        {
            break;
        }

        DeleteDirectoryWithFiles((const WCHAR*)cpOutFileName);
    }while(false);

    SAFEFREE(pwszContentFileName);
    if(pwszHWURL != NULL)
    {
        CoTaskMemFree(pwszHWURL);
    }
    SAFERELEASE(pBasicLog);
    SAFERELEASE(p264EncVerify);

    if(bMFStarted != FALSE)
    {
        MFShutdown();
    }

    return hr;
}

HRESULT VerifyEncoderWinBlue(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate, BLUE_TEST_TYPE type)
{
    HRESULT                     hr                  = S_OK;
    BOOL                        bMFStarted          = FALSE;
    GUID                        guidMFT             = GUID_NULL;
    WCHAR*                      pwszHWURL           = NULL;
    WCHAR*                      pwszContentFileName = NULL;
    UINT32                      un32HWURLLen        = 0;
    CString                     csTestID;
    CString                     sInFileName;
    CString                     sConfig;
    CPath                       cpOutFileName;
    IBasicLog*                  pBasicLog           = NULL;
    IH264EncodeVerify*          p264EncVerify       = NULL;

    do
    {
        if((pLogger == NULL) || (pConfig == NULL) || (pActivate == NULL))
        {
            hr = E_POINTER;
            break;
        }

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to startup MF (hr0x%x)", hr);
            break;
        }

        hr = pConfig->GetString(L"Config", sConfig);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find Config node in XML :0x%x", hr);
            break;
        }

        BOOL                    bContinueTest       = FALSE;
        IMFAttributes*          pOutputAttributes   = NULL;
        MFT_REGISTER_TYPE_INFO* pMediaTypes         = NULL;
        do
        {
            UINT32 un32MediaTypeSize = 0;

            hr = pActivate->GetBlobSize(MFT_OUTPUT_TYPES_Attributes, &un32MediaTypeSize);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> MFT_OUTPUT_TYPES_Attributes not found: 0x%x", hr);
                break;
            }

            pMediaTypes = (MFT_REGISTER_TYPE_INFO*)malloc(sizeof(BYTE) * un32MediaTypeSize);
            if(pMediaTypes == NULL)
            {
                hr = E_OUTOFMEMORY;
                LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate MediaType info blob");
                break;
            }

            hr = MFCreateAttributes(&pOutputAttributes, 1);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to allocate Test Attribute Store: 0x%x", hr);
                break;
            }

            hr = pActivate->GetBlob(MFT_OUTPUT_TYPES_Attributes, (BYTE*)pMediaTypes, un32MediaTypeSize, NULL);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_OUTPUT_TYPES_Attributes: 0x%x", hr);
                break;
            }

            if(FAILED(pConfig->GetAttributesFromNode(L"OutputType", pOutputAttributes)))
            {
                LogTC(pLogger, HIGH, L"TEST ERROR>> Failed to get InputType attribute from XML");
                hr = E_FAIL;
                break;
            }

            hr = VerifyTestSupported(pMediaTypes, un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO), pOutputAttributes);
            if(hr != S_OK)
            {
                if(hr == S_FALSE)
                {
                    hr = S_SKIPPED;
                    LogTC(pLogger, NORMAL, L"INFO>> This test is not applicable as the component doesn't support the media type");
                    break;
                }
                else
                {
                    LogTC(pLogger, HIGH, L"WARNING>> Media Type support verification failed: 0x%x", hr);
                    LogTC(pLogger, HIGH, L"WARNING>> Test will be run");
                    hr = S_OK;
                }
            }

            bContinueTest = TRUE;
        }while(false);

        SAFEFREE(pMediaTypes);
        SAFERELEASE(pOutputAttributes);

        if(FAILED(hr) || (bContinueTest == FALSE))
        {
            break;
        }

        bMFStarted = TRUE;

        hr = pConfig->GetString(L"InputFile", sInFileName);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find InputFile (hr=0x%x)", hr);
            break;
        }

        hr = pActivate->GetGUID(MFT_TRANSFORM_CLSID_Attribute, &guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Activate object does not have a transform CLSID (hr=0x%x)", hr);
            break;
        }

        if(pActivate->GetAllocatedString(MFT_ENUM_HARDWARE_URL_Attribute, &pwszHWURL, &un32HWURLLen) == E_OUTOFMEMORY)
        {
            hr = E_OUTOFMEMORY;
            LogTC(pLogger, NORMAL, L"Error: Out of memory, can't allocate HW URL String (hr=0x%x)", hr);
            break;
        }

        hr = PrintMFTUnderTestInfo(pLogger,pActivate,guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: PrintMFTUnderTestInfo Failed (hr=0x%x)", hr);
            break;
        }

        hr = CILoggerToIBasicLog::CreateInstance(pLogger, &pBasicLog);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to create basic log wrapper object (hr=0x%x)", hr);
            break;
        }
                      
        hr = FindHCKContentFile(
            (const WCHAR*)sInFileName,
            &pwszContentFileName
            );
        if(FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", (const WCHAR*)sInFileName, hr);
            break;
        }

        if(FAILED(pConfig->GetAttribute(L"testId", csTestID)))
        {
            csTestID = "{ID_Unknown}";
        }

        hr = GetTestOutputFileName(pLogger, ENCODERLTR_OUT_DIR_NAME, ENCODERLTROUTPUTNAME, 
            csTestID, L"", cpOutFileName);
        if(FAILED(hr))
        {
            break;
        }

        CreateDirectory(cpOutFileName, NULL);

        cpOutFileName.m_strPath.Append(L"test.mp4");

        if(g_bUseCabacForQualityTests != FALSE)
        {
            // replace -cabacenable false with -cabacenable true, or append -cabacenable true if it's not in the string
            if(sConfig.Replace(
                L"-cabacenable false",
                L"-cabacenable true"
                ) == 0)
            {
                sConfig.Append(
                    L" -cabacenable true"
                    );
            }
        }

        hr = CreateH264EncodeVerify(
            pwszContentFileName,
            (const WCHAR*)cpOutFileName.m_strPath,
            (const WCHAR*)sConfig,
            pBasicLog,
            &p264EncVerify
            );
        if(FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to initialize test object (hr=0x%x)", hr);
            break;
        }
        
        switch(type){
        case BLUE_TEST_STREAMING:
            hr = p264EncVerify->RunStreamingScenario();
            break;
        case BLUE_TEST_DYNSINKWRITER:
            hr = p264EncVerify->RunSinkWriterDynamicScenario(FALSE);
            break;
        case BLUE_TEST_ROI:
            hr = p264EncVerify->RunROIScenario();
            break;
        case BLUE_TEST_RDCURVE:
            hr = p264EncVerify->RunRDCurveScenario();
            break;
        default:
            hr = p264EncVerify->RunStreamingScenario();
            break;
        }
        if(FAILED(hr))
        {
            break;
        }
    }while(false);

    do
    {
        DWORD dwDeleteFiles = 0;
        BOOL bWow64 = FALSE;

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

        if(FAILED(GetTestOutputFileName(pLogger, ENCODERSETTINGS_OUT_DIR_NAME, ENCODERSETTINGSOUTPUTNAME, 
            csTestID, L"", cpOutFileName)))
        {
            break;
        }

        DeleteDirectoryWithFiles((const WCHAR*)cpOutFileName);
    }while(false);

    SAFEFREE(pwszContentFileName);
    if(pwszHWURL != NULL)
    {
        CoTaskMemFree(pwszHWURL);
    }
    SAFERELEASE(pBasicLog);
    SAFERELEASE(p264EncVerify);

    if(bMFStarted != FALSE)
    {
        MFShutdown();
    }

    return hr;
}


HRESULT VerifyEncoderLTRRDCurve(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate)
{
    HRESULT                     hr                  = S_OK;
    BOOL                        bMFStarted          = FALSE;
    GUID                        guidMFT             = GUID_NULL;
    WCHAR*                      pwszHWURL           = NULL;
    WCHAR*                      pwszContentFileName = NULL;
    UINT32                      un32HWURLLen        = 0;
    CString                     csTestID;
    CString                     sInFileName;
    CString                     sConfig;
    CPath                       cpOutFileName;
    IBasicLog*                  pBasicLog           = NULL;
    IH264EncodeVerify*          p264EncVerify       = NULL;

    do
    {
        if((pLogger == NULL) || (pConfig == NULL) || (pActivate == NULL))
        {
            hr = E_POINTER;
            break;
        }

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to startup MF (hr0x%x)", hr);
            break;
        }

        hr = pConfig->GetString(L"Config", sConfig);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find Config node in XML :0x%x", hr);
            break;
        }

        BOOL                    bContinueTest       = FALSE;
        IMFAttributes*          pOutputAttributes   = NULL;
        MFT_REGISTER_TYPE_INFO* pMediaTypes         = NULL;
        do
        {            
            UINT32 un32MediaTypeSize = 0;

            hr = pActivate->GetBlobSize(MFT_OUTPUT_TYPES_Attributes, &un32MediaTypeSize);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> MFT_OUTPUT_TYPES_Attributes not found: 0x%x", hr);
                break;
            }

            pMediaTypes = (MFT_REGISTER_TYPE_INFO*)malloc(sizeof(BYTE) * un32MediaTypeSize);
            if(pMediaTypes == NULL)
            {
                hr = E_OUTOFMEMORY;
                LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate MediaType info blob");
                break;
            }

            hr = MFCreateAttributes(&pOutputAttributes, 1);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to allocate Test Attribute Store: 0x%x", hr);
                break;
            }

            hr = pActivate->GetBlob(MFT_OUTPUT_TYPES_Attributes, (BYTE*)pMediaTypes, un32MediaTypeSize, NULL);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_OUTPUT_TYPES_Attributes: 0x%x", hr);
                break;
            }

            if(FAILED(pConfig->GetAttributesFromNode(L"OutputType", pOutputAttributes)))
            {
                LogTC(pLogger, HIGH, L"TEST ERROR>> Failed to get InputType attribute from XML");
                hr = E_FAIL;
                break;
            }

            hr = VerifyTestSupported(pMediaTypes, un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO), pOutputAttributes);
            if(hr != S_OK)
            {
                if(hr == S_FALSE)
                {
                    hr = S_SKIPPED;
                    LogTC(pLogger, NORMAL, L"INFO>> This test is not applicable as the component doesn't support the media type");
                    break;
                }
                else
                {
                    LogTC(pLogger, HIGH, L"WARNING>> Media Type support verification failed: 0x%x", hr);
                    LogTC(pLogger, HIGH, L"WARNING>> Test will be run");
                    hr = S_OK;
                }
            }

            bContinueTest = TRUE;
        }while(false);

        SAFEFREE(pMediaTypes);
        SAFERELEASE(pOutputAttributes);

        if(FAILED(hr) || (bContinueTest == FALSE))
        {
            break;
        }

        bMFStarted = TRUE;

        hr = pConfig->GetString(L"InputFile", sInFileName);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find InputFile (hr=0x%x)", hr);
            break;
        }

        hr = pActivate->GetGUID(MFT_TRANSFORM_CLSID_Attribute, &guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Activate object does not have a transform CLSID (hr=0x%x)", hr);
            break;
        }

        if(pActivate->GetAllocatedString(MFT_ENUM_HARDWARE_URL_Attribute, &pwszHWURL, &un32HWURLLen) == E_OUTOFMEMORY)
        {
            hr = E_OUTOFMEMORY;
            LogTC(pLogger, NORMAL, L"Error: Out of memory, can't allocate HW URL String (hr=0x%x)", hr);
            break;
        }

        hr = PrintMFTUnderTestInfo(pLogger,pActivate,guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: PrintMFTUnderTestInfo Failed (hr=0x%x)", hr);
            break;
        }

        hr = CILoggerToIBasicLog::CreateInstance(pLogger, &pBasicLog);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to create basic log wrapper object (hr=0x%x)", hr);
            break;
        }
                      
        hr = FindHCKContentFile(
            (const WCHAR*)sInFileName,
            &pwszContentFileName
            );
        if(FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", (const WCHAR*)sInFileName, hr);
            break;
        }

        if(FAILED(pConfig->GetAttribute(L"testId", csTestID)))
        {
            csTestID = "{ID_Unknown}";
        }

        hr = GetTestOutputFileName(pLogger, ENCODERLTR_OUT_DIR_NAME, ENCODERLTROUTPUTNAME, 
            csTestID, L"", cpOutFileName);
        if(FAILED(hr))
        {
            break;
        }

        CreateDirectory(cpOutFileName, NULL);

        cpOutFileName.m_strPath.Append(L"test.mp4");

        hr = CreateH264EncodeVerify(
            pwszContentFileName,
            (const WCHAR*)cpOutFileName.m_strPath,
            (const WCHAR*)sConfig,
            pBasicLog,
            &p264EncVerify
            );
        if(FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to initialize test object (hr=0x%x)", hr);
            break;
        }

        hr = p264EncVerify->RunLTRRDCurveScenario();
        if(FAILED(hr))
        {
            break;
        }
    }while(false);

    do
    {
        DWORD dwDeleteFiles = 0;
        BOOL bWow64 = FALSE;

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

        if(FAILED(GetTestOutputFileName(pLogger, ENCODERSETTINGS_OUT_DIR_NAME, ENCODERSETTINGSOUTPUTNAME, 
            csTestID, L"", cpOutFileName)))
        {
            break;
        }

        DeleteDirectoryWithFiles((const WCHAR*)cpOutFileName);
    }while(false);

    SAFEFREE(pwszContentFileName);
    if(pwszHWURL != NULL)
    {
        CoTaskMemFree(pwszHWURL);
    }
    SAFERELEASE(pBasicLog);
    SAFERELEASE(p264EncVerify);

    if(bMFStarted != FALSE)
    {
        MFShutdown();
    }

    return hr;
}

HRESULT VerifyEncoderSettings(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate)
{
    HRESULT                     hr                  = S_OK;
    BOOL                        bMFStarted          = FALSE;
    GUID                        guidMFT             = GUID_NULL;
    WCHAR*                      pwszHWURL           = NULL;
    UINT32                      un32HWURLLen        = 0;
    IVerificationEncodeHelper*  pEncVerifier        = NULL;
    ITAEFConfigHelper*          pConfigHelper       = NULL;
    CString                     csTestID;
    CString                     sInFileName;
    CPath                       cpOutFileName;
    IBasicLog*                  pBasicLog           = NULL;
    EncodingParameters*         pepEncodingParam    = NULL;
    WCHAR*                      pwszContentFileName = NULL;

    do
    {
        if((pLogger == NULL) || (pConfig == NULL) || (pActivate == NULL))
        {
            hr = E_POINTER;
            break;
        }

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to startup MF (hr0x%x)", hr);
            break;
        }

        BOOL                    bContinueTest       = FALSE;
        IMFAttributes*          pOutputAttributes   = NULL;
        MFT_REGISTER_TYPE_INFO* pMediaTypes         = NULL;
        CString                 sConfig;
        do
        {
            hr = pConfig->GetString(L"Config", sConfig);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Test Error: Failed to find Config node in XML :0x%x", hr);
                break;
            }

            UINT32 un32MediaTypeSize = 0;

            hr = pActivate->GetBlobSize(MFT_OUTPUT_TYPES_Attributes, &un32MediaTypeSize);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> MFT_OUTPUT_TYPES_Attributes not found: 0x%x", hr);
                break;
            }

            pMediaTypes = (MFT_REGISTER_TYPE_INFO*)malloc(sizeof(BYTE) * un32MediaTypeSize);
            if(pMediaTypes == NULL)
            {
                hr = E_OUTOFMEMORY;
                LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate MediaType info blob");
                break;
            }

            hr = MFCreateAttributes(&pOutputAttributes, 1);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to allocate Test Attribute Store: 0x%x", hr);
                break;
            }

            hr = pActivate->GetBlob(MFT_OUTPUT_TYPES_Attributes, (BYTE*)pMediaTypes, un32MediaTypeSize, NULL);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_OUTPUT_TYPES_Attributes: 0x%x", hr);
                break;
            }

            if(FAILED(pConfig->GetAttributesFromNode(L"OutputType", pOutputAttributes)))
            {
                LogTC(pLogger, HIGH, L"TEST ERROR>> Failed to get InputType attribute from XML");
                hr = E_FAIL;
                break;
            }

            hr = VerifyTestSupported(pMediaTypes, un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO), pOutputAttributes);
            if(hr != S_OK)
            {
                if(hr == S_FALSE)
                {
                    hr = S_SKIPPED;
                    LogTC(pLogger, NORMAL, L"INFO>> This test is not applicable as the component doesn't support the media type");
                    break;
                }
                else
                {
                    LogTC(pLogger, HIGH, L"WARNING>> Media Type support verification failed: 0x%x", hr);
                    LogTC(pLogger, HIGH, L"WARNING>> Test will be run");
                    hr = S_OK;
                }
            }

            bContinueTest = TRUE;
        }while(false);

        SAFEFREE(pMediaTypes);
        SAFERELEASE(pOutputAttributes);

        if(FAILED(hr) || (bContinueTest == FALSE))
        {
            break;
        }

        bMFStarted = TRUE;

        hr = pConfig->GetString(L"InputFile", sInFileName);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find InputFile (hr=0x%x)", hr);
            break;
        }

        hr = pActivate->GetGUID(MFT_TRANSFORM_CLSID_Attribute, &guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Activate object does not have a transform CLSID (hr=0x%x)", hr);
            break;
        }

        if(pActivate->GetAllocatedString(MFT_ENUM_HARDWARE_URL_Attribute, &pwszHWURL, &un32HWURLLen) == E_OUTOFMEMORY)
        {
            hr = E_OUTOFMEMORY;
            LogTC(pLogger, NORMAL, L"Error: Out of memory, can't allocate HW URL String (hr=0x%x)", hr);
            break;
        }
        
        hr = PrintMFTUnderTestInfo(pLogger,pActivate,guidMFT);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: PrintMFTUnderTestInfo Failed (hr=0x%x)", hr);
            break;
        }
        
        hr = CILoggerToIBasicLog::CreateInstance(pLogger, &pBasicLog);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to create basic log wrapper object (hr=0x%x)", hr);
            break;
        }

        hr = CreateTAEFConfigHelperFromString(pBasicLog, NULL, &pConfigHelper, L"Out", L"In", sConfig);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Failed to retrieve test settings from string, hr = 0x%08X", hr);
            break;
        }

        hr = CreateVerificationEncodeHelper(pConfigHelper, pBasicLog, &pEncVerifier);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to create encoder verifier module (hr=0x%x)", hr);
            break;
        }

        hr = FindHCKContentFile(
            (const WCHAR*)sInFileName,
            &pwszContentFileName
            );
        if(FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", (const WCHAR*)sInFileName, hr);
            break;
        }

        pepEncodingParam = new EncodingParameters();
        if(pepEncodingParam == NULL)
        {
            hr = E_OUTOFMEMORY;
            LogTC(pLogger, NORMAL, L"Error: Out of memory, failed to create encoder parameters (hr=0x%x)", hr);
            break;
        }

        memset(pepEncodingParam, 0, sizeof(EncodingParameters));

        hr = ConvertTranscodeProfileToEncodingParameters(pLogger, pConfig, pepEncodingParam);
        if(FAILED(hr))
        {
            break;
        }

        if(FAILED(pConfig->GetAttribute(L"testId", csTestID)))
        {
            csTestID = L"{TestIDUnknown}";
        }

        hr = GetTestOutputFileName(pLogger, ENCODERSETTINGS_OUT_DIR_NAME, ENCODERSETTINGSOUTPUTNAME, 
            csTestID, L"", cpOutFileName);
        if(FAILED(hr))
        {
            break;
        }
        CreateDirectory(cpOutFileName, NULL);
        if((pepEncodingParam->gSubtype == MFVideoFormat_WVC1) || (pepEncodingParam->gSubtype == MFVideoFormat_WMV3)){
            cpOutFileName.m_strPath.Append(L"test.asf");
        }
        else{
            cpOutFileName.m_strPath.Append(L"test.mp4");
        }
        

        hr = pEncVerifier->SetSources((const WCHAR*)pwszContentFileName, (DWORD)wcslen(pwszContentFileName),
            (const WCHAR*)cpOutFileName, (DWORD)cpOutFileName.m_strPath.GetLength());
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to set sources on encoder verifier module (hr=0x%x)", hr);
            break;
        }
        LogTC(pLogger, NORMAL, L"Info: Input File %s", pwszContentFileName);
        LogTC(pLogger, NORMAL, L"Info: Output File %s", cpOutFileName);

        hr = pEncVerifier->SetEncoderProperties(guidMFT, pepEncodingParam);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to set encoder properties (hr=0x%x)", hr);
            break;
        }

        hr = pEncVerifier->StartEncode();
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed in encode (hr=0x%x)", hr);
            break;
        }

        hr = pEncVerifier->StartVerification();
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed in verification (hr=0x%x)", hr);
            break;
        }    
    }while(false);

    do
    {
        DWORD dwDeleteFiles = 0;
        BOOL bWow64 = FALSE;

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

        if(FAILED(GetTestOutputFileName(pLogger, ENCODERSETTINGS_OUT_DIR_NAME, ENCODERSETTINGSOUTPUTNAME, 
            csTestID, L"", cpOutFileName)))
        {
            break;
        }
        
        DeleteDirectoryWithFiles((const WCHAR*)cpOutFileName);
    }while(false);

    if(pwszHWURL != NULL)
    {
        CoTaskMemFree(pwszHWURL);
    }
    
    SAFERELEASE(pConfigHelper);
    SAFERELEASE(pEncVerifier);
    SAFERELEASE(pBasicLog);
    SAFEDELETES(pepEncodingParam);
    SAFEFREE(pwszContentFileName);
    if(bMFStarted != FALSE)
    {
        MFShutdown();
    }

    return hr;
}

#pragma prefast(pop)

HRESULT CopyMultipleInputSourceFiles(
    ILogger*    pLogger,
    IConfig*    pConfig)
{
    HRESULT hr                      = S_OK;
    CString csInputFileNames        = L"";
    WCHAR*  pwszInputFileNames      = NULL;
    WCHAR*  pwszCurrInputFileName   = NULL;
    WCHAR*  pwszTokContext          = NULL;
    WCHAR*  pwszContentFileName     = NULL;

    do
    {
        if( (pLogger == NULL)   ||
            (pConfig == NULL)   )
        {
            hr = E_POINTER;
            break;
        }

        if(FAILED(pConfig->GetString(L"InputFiles", csInputFileNames)))
        {
            // No input files to copy
            hr = E_UNEXPECTED;
            break;
        }

        pwszInputFileNames  = _wcsdup(
            (const WCHAR*)csInputFileNames
            );
        if(pwszInputFileNames == NULL)
        {
            pLogger->Log(NORMAL, L"ERROR>> Out of memory, can't allocate input file names string");
            hr = E_OUTOFMEMORY;
            break;
        }

        pwszCurrInputFileName   =   wcstok_s(
            pwszInputFileNames,
            L"';",
            &pwszTokContext
            );
        while(pwszCurrInputFileName != NULL)
        {
            SAFEFREE(pwszContentFileName);

            hr = FindHCKContentFile(
                pwszCurrInputFileName,
                &pwszContentFileName
                );
            if(FAILED(hr))
            {
                pLogger->Log(NORMAL, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", pwszCurrInputFileName, hr);
                break;
            }

            pwszCurrInputFileName   = wcstok_s(
                NULL,
                L"';",
                &pwszTokContext
                );
        }
    }while(false);

    SAFEFREE(pwszInputFileNames);
    SAFEFREE(pwszContentFileName);

    return hr;
}

HRESULT ConvertTranscodeProfileToEncodingParameters(ILogger* pLogger, 
                                                    IConfig* pConfig,
                                                    EncodingParameters* pepEncodingParameters)
{
    HRESULT         hr              = S_OK;
    IMFAttributes*  pVideoAttrib    = NULL;
    ITAEFConfigHelper* pConfigHelper = NULL;
    IMFTranscodeProfile*        pTranscodeProfile   = NULL;
    CTranscodeProfileReader*    pProfileReader      = NULL;
    CString sConfig;
    //GUID            guidSubType     = GUID_NULL;
    IBasicLog* pBasicLog = NULL;

    do
    {
        if((pLogger == NULL) || (pConfig == NULL) || (pepEncodingParameters == NULL))
        {
            hr = E_POINTER;
            break;
        }

        hr = CTranscodeProfileReader::CreateTranscodeProfileReader(pConfig, &pProfileReader);
        if(FAILED(hr) || (pProfileReader == NULL))
        {            
            hr = (FAILED(hr) ? hr : E_FAIL);
            LogTC(pLogger, HIGH, L"Error: Failed to create CTranscodeProfileReader (hr=0x%x)", hr);
            break;
        }

        hr = pProfileReader->GetTranscodeProfile(&pTranscodeProfile);
        if (FAILED(hr) || pTranscodeProfile == NULL)
        {
            LogTC(pLogger, HIGH, L"Error: Failed to create CTranscodeProfileReader (hr=0x%x)", hr);
            break;
        }

        hr = pTranscodeProfile->GetVideoAttributes(&pVideoAttrib);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to get video attributes from transcode profile (hr=0x%x)", hr);
            break;
        }

        pVideoAttrib->GetUINT32(MFTDIAG_ATTRIBUTE_CODECAPI_BITRATECONTROLMODE, (UINT32*)&(pepEncodingParameters->ControlMode));

        pVideoAttrib->GetUINT32(MF_MT_MPEG2_PROFILE, &(pepEncodingParameters->uiEncodingProfile));

        if(FAILED(pVideoAttrib->GetUINT32(MF_MT_MPEG2_LEVEL, (UINT32*)&(pepEncodingParameters->iEncodingLevel))))
        {
            LogTC(pLogger, NORMAL, L"Info: Encoding level was not specified, using auto");
            pepEncodingParameters->iEncodingLevel = -1;
        }

        pVideoAttrib->GetUINT32(MF_MT_AVG_BITRATE, &(pepEncodingParameters->uiBitrate));

        pVideoAttrib->GetUINT32(MFTDIAG_ATTRIBUTE_CODECAPI_ENCQUALITYLEVEL, &(pepEncodingParameters->uiQualityLevel));
        
        pVideoAttrib->GetUINT32(MFTDIAG_ATTRIBUTE_CODECAPI_COMMONBUFFERSIZE, &(pepEncodingParameters->uiBufferSize));

        pVideoAttrib->GetUINT32(MFTDIAG_ATTRIBUTE_CODECAPI_GOPSIZE, &(pepEncodingParameters->uiGOPSize));

        pVideoAttrib->GetUINT32(MFTDIAG_ATTRIBUTE_CODECAPI_BFRAMES, &(pepEncodingParameters->uiBframes));

        pVideoAttrib->GetGUID(MF_MT_SUBTYPE, &(pepEncodingParameters->gSubtype));

    
        pepEncodingParameters->PropType = pt_CodecAPI;

        hr = pConfig->GetString(L"Config", sConfig);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find Config node in XML :0x%x", hr);
            break;
        }
        if(sConfig != L""){

            hr = CILoggerToIBasicLog::CreateInstance(pLogger, &pBasicLog);
            if(FAILED(hr))
            {
                LogTC(pLogger, NORMAL, L"Error: Failed to create basic log wrapper object (hr=0x%x)", hr);
                break;
            }
        
            hr = CreateTAEFConfigHelperFromString(pBasicLog, NULL, &pConfigHelper, L"In", L"Out", sConfig);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Failed to retrieve test settings from string, hr = 0x%08X", hr);
                break;
            }
        
            pConfigHelper->GetQualityVsSpeed(&(pepEncodingParameters->uiQualityVsSpeed));

            WEX::Common::String encodingProfile = pConfigHelper->GetEncodingProfile();
            if(encodingProfile.CompareNoCase(L"main") == 0)
            {
                pepEncodingParameters->ProfileName = epfn_MAIN;
            }
            else if(encodingProfile.CompareNoCase(L"simple") == 0)
            {
                pepEncodingParameters->ProfileName = epfn_SIMPLE;
            }
            else if(encodingProfile.CompareNoCase(L"base") == 0)
            {
                pepEncodingParameters->ProfileName = epfn_BASE;
            }
            else if(encodingProfile.CompareNoCase(L"advanced") == 0)
            {
                pepEncodingParameters->ProfileName = epfn_ADVANCED;
            }
            else if(encodingProfile.CompareNoCase(L"") == 0)
            {
                pepEncodingParameters->ProfileName = epfn_DEFAULT;
            }
            else
            {
                pepEncodingParameters->ProfileName = epfn_INVALID;
            }
            
        }
    
    }while(false);

    SAFERELEASE(pBasicLog);
    SAFERELEASE(pConfigHelper);
    SAFERELEASE(pTranscodeProfile);
    SAFEDELETES(pProfileReader);
    SAFERELEASE(pVideoAttrib);

    return hr;
}
HRESULT PrintMFTUnderTestInfo(ILogger* pLogger,IMFActivate* pActivate,const GUID guidMFT)
{
    WCHAR*  pwszGUID = NULL;
    WCHAR*  pwszHWFriendlyName  = NULL;
    UINT32  un32HWFriendlyNameLen = 0;   
    HRESULT hr = S_OK;  
    do
    {
        if(pLogger == NULL || pActivate == NULL)
        {
            hr = E_POINTER;
            break;
        }
        hr = StringFromCLSID(guidMFT, &pwszGUID);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to get String from CLSID (hr=0x%x)", hr);
            break;
        }
        LogTC(pLogger, NORMAL, L"INFO>> MFT Under Test: %s", (pwszGUID != NULL) ? pwszGUID : L"Unknown");

        hr = pActivate->GetAllocatedString(MFT_FRIENDLY_NAME_Attribute,&pwszHWFriendlyName,&un32HWFriendlyNameLen);
        if(FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"INFO>> Failed to get Friendly Name for the Transform (hr=0x%x)", hr);
            hr = S_OK;
            break;
        }

        LogTC(pLogger, NORMAL, L"INFO>> Name of MFT Under Test: %s", (pwszHWFriendlyName != NULL) ? pwszHWFriendlyName : L"Unknown");
    }while(false);
    SAFECOMFREE(pwszGUID);
    SAFECOMFREE(pwszHWFriendlyName);
    return hr;

}

HRESULT DeleteDirectoryWithFiles(const WCHAR* pwcDirectory)
{
    HRESULT hr = S_OK;
    WIN32_FIND_DATA findFileData;
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    WCHAR *pwcFileSearchPath = NULL;
    WCHAR *pwcFullFilePath = NULL;

    do
    {
        pwcFileSearchPath = (WCHAR*)malloc(MAX_PATH * sizeof(WCHAR));
        pwcFullFilePath = (WCHAR*)malloc(MAX_PATH * sizeof(WCHAR));
        if(NULL == pwcFileSearchPath || NULL == pwcFullFilePath)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        wcscpy(pwcFileSearchPath, pwcDirectory);
        wcscat(pwcFileSearchPath, L"\\*");

        hFindFile = FindFirstFile(pwcFileSearchPath, &findFileData);
        if(INVALID_HANDLE_VALUE == hFindFile)
        {
            hr = E_FAIL;
            break;
        }

        do
        {
            wcscpy(pwcFullFilePath, pwcDirectory);
            wcscat(pwcFullFilePath, L"\\");
            wcscat(pwcFullFilePath, findFileData.cFileName);
            DeleteFile(pwcFullFilePath);
        }
        while(FindNextFile(hFindFile, &findFileData) != FALSE);

        if(!RemoveDirectory(pwcDirectory))
        {
            hr = E_FAIL;
            break;
        }
    }
    while(false);

    FindClose(hFindFile);
    SAFEFREE(pwcFileSearchPath);
    SAFEFREE(pwcFullFilePath);

    return hr;
}


HRESULT VerifyEncoderGradualIntraRefresh(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate)
{
    HRESULT                     hr = S_OK;
    BOOL                        bMFStarted = FALSE;
    GUID                        guidMFT = GUID_NULL;
    WCHAR*                      pwszHWURL = NULL;
    WCHAR*                      pwszContentFileName = NULL;
    UINT32                      un32HWURLLen = 0;
    CString                     csTestID;
    CString                     sInFileName;
    CString                     sConfig;
    CPath                       cpOutFileName;
    IBasicLog*                  pBasicLog = NULL;
    IH264EncodeVerify*          p264EncVerify = NULL;

    do
    {
        if ((pLogger == NULL) || (pConfig == NULL) || (pActivate == NULL))
        {
            hr = E_POINTER;
            break;
        }

        hr = MFStartup(MF_VERSION);
        if (FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to startup MF (hr0x%x)", hr);
            break;
        }

        hr = pConfig->GetString(L"Config", sConfig);
        if (FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find Config node in XML :0x%x", hr);
            break;
        }

        BOOL                    bContinueTest = FALSE;
        IMFAttributes*          pOutputAttributes = NULL;
        MFT_REGISTER_TYPE_INFO* pMediaTypes = NULL;
        do
        {
            UINT32 un32MediaTypeSize = 0;

            hr = pActivate->GetBlobSize(MFT_OUTPUT_TYPES_Attributes, &un32MediaTypeSize);
            if (FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> MFT_OUTPUT_TYPES_Attributes not found: 0x%x", hr);
                break;
            }

            pMediaTypes = (MFT_REGISTER_TYPE_INFO*) malloc(sizeof(BYTE) * un32MediaTypeSize);
            if (pMediaTypes == NULL)
            {
                hr = E_OUTOFMEMORY;
                LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate MediaType info blob");
                break;
            }

            hr = MFCreateAttributes(&pOutputAttributes, 1);
            if (FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to allocate Test Attribute Store: 0x%x", hr);
                break;
            }

            hr = pActivate->GetBlob(MFT_OUTPUT_TYPES_Attributes, (BYTE*) pMediaTypes, un32MediaTypeSize, NULL);
            if (FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_OUTPUT_TYPES_Attributes: 0x%x", hr);
                break;
            }

            if (FAILED(pConfig->GetAttributesFromNode(L"OutputType", pOutputAttributes)))
            {
                LogTC(pLogger, HIGH, L"TEST ERROR>> Failed to get InputType attribute from XML");
                hr = E_FAIL;
                break;
            }

            hr = VerifyTestSupported(pMediaTypes, un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO), pOutputAttributes);
            if (hr != S_OK)
            {
                if (hr == S_FALSE)
                {
                    hr = S_SKIPPED;
                    LogTC(pLogger, NORMAL, L"INFO>> This test is not applicable as the component doesn't support the media type");
                    break;
                }
                else
                {
                    LogTC(pLogger, HIGH, L"WARNING>> Media Type support verification failed: 0x%x", hr);
                    LogTC(pLogger, HIGH, L"WARNING>> Test will be run");
                    hr = S_OK;
                }
            }

            bContinueTest = TRUE;
        } while (false);

        SAFEFREE(pMediaTypes);
        SAFERELEASE(pOutputAttributes);

        if (FAILED(hr) || (bContinueTest == FALSE))
        {
            break;
        }

        bMFStarted = TRUE;

        hr = pConfig->GetString(L"InputFile", sInFileName);
        if (FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"Test Error: Failed to find InputFile (hr=0x%x)", hr);
            break;
        }

        hr = pActivate->GetGUID(MFT_TRANSFORM_CLSID_Attribute, &guidMFT);
        if (FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Activate object does not have a transform CLSID (hr=0x%x)", hr);
            break;
        }

        if (pActivate->GetAllocatedString(MFT_ENUM_HARDWARE_URL_Attribute, &pwszHWURL, &un32HWURLLen) == E_OUTOFMEMORY)
        {
            hr = E_OUTOFMEMORY;
            LogTC(pLogger, NORMAL, L"Error: Out of memory, can't allocate HW URL String (hr=0x%x)", hr);
            break;
        }

        hr = PrintMFTUnderTestInfo(pLogger, pActivate, guidMFT);
        if (FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: PrintMFTUnderTestInfo Failed (hr=0x%x)", hr);
            break;
        }

        hr = CILoggerToIBasicLog::CreateInstance(pLogger, &pBasicLog);
        if (FAILED(hr))
        {
            LogTC(pLogger, NORMAL, L"Error: Failed to create basic log wrapper object (hr=0x%x)", hr);
            break;
        }

        hr = FindHCKContentFile(
            (const WCHAR*) sInFileName,
            &pwszContentFileName
            );
        if (FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", (const WCHAR*) sInFileName, hr);
            break;
        }

        if (FAILED(pConfig->GetAttribute(L"testId", csTestID)))
        {
            csTestID = "{ID_Unknown}";
        }

        hr = GetTestOutputFileName(pLogger, ENCODERINTRA_REF_OUT_DIR_NAME, ENCODERINTRA_REF_OUTPUTNAME,
            csTestID, L"", cpOutFileName);
        if (FAILED(hr))
        {
            break;
        }

        CreateDirectory(cpOutFileName, NULL);

        cpOutFileName.m_strPath.Append(L"test.mp4");

        if (g_bUseCabacForQualityTests != FALSE)
        {
            // replace -cabacenable false with -cabacenable true, or append -cabacenable true if it's not in the string
            if (sConfig.Replace(
                L"-cabacenable false",
                L"-cabacenable true"
                ) == 0)
            {
                sConfig.Append(
                    L" -cabacenable true"
                    );
            }
        }

        hr = CreateH264EncodeVerify(
            pwszContentFileName,
            (const WCHAR*) cpOutFileName.m_strPath,
            (const WCHAR*) sConfig,
            pBasicLog,
            &p264EncVerify
            );
        if (FAILED(hr))
        {
            pLogger->Log(HIGH, L"ERROR>> Failed to initialize test object (hr=0x%x)", hr);
            break;
        }


        hr = p264EncVerify->RunGradualIntraRefreshScenario();
        if (FAILED(hr))
        {
            break;
        }

    } while (false);

    do
    {
        DWORD dwDeleteFiles = 0;
        

        if (CCMFTDiagGlobalParams::g_pParams->bAlwaysCleanOutput != FALSE)
        {
            dwDeleteFiles = 1;
        }

        if (dwDeleteFiles == 0)
        {
            break;
        }

        if (FAILED(GetTestOutputFileName(pLogger, ENCODERSETTINGS_OUT_DIR_NAME, ENCODERSETTINGSOUTPUTNAME,
            csTestID, L"", cpOutFileName)))
        {
            break;
        }

        DeleteDirectoryWithFiles((const WCHAR*) cpOutFileName);
    } while (false);

    SAFEFREE(pwszContentFileName);
    if (pwszHWURL != NULL)
    {
        CoTaskMemFree(pwszHWURL);
    }
    SAFERELEASE(pBasicLog);
    SAFERELEASE(p264EncVerify);

    if (bMFStarted != FALSE)
    {
        MFShutdown();
    }

    return hr;
}
