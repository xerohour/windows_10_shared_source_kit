#include "stdafx.h"
#include "MFTPlaybackTests.h"
#include "CTestMediaSessionPlayback.h"
#include "PerfHelper.h"
#include "LogTC.h"
#include "CConfigRegKey.h"
#include "Resource.h"
#include "VerifyTestSupported.h"
#include "MediaTimeConverter.h"
#include "MFTDMIHelper.h"
#include "MFTDPerfHelper.h"
#include "MFTDPower.h"
#include "FindHCKContentFile.h"

#define NUMINSTANCESNAME            L"NumInstances"
#define DUMPPLAYBACKNAME            L"DumpPlayback"
#define VERIFYMIOUTPUTNAME          L"VerifyMIOutput"
#define DELETEFILESONSUCCEESSNAME   L"PlaybackDeleteOnSuccess"
#define DELETEFILESONFAILURENAME    L"PlaybackDeleteOnFailure"
#define SMARTSOURCENAME             L"UseSmartSource"
#define RUNTIMENAME                 L"RunTime"
#define PERFTESTNAME                L"PerfTest"
#define DISABLEAUDIONAME            L"DisableAudio"
#define RENDERINGMODENAME           L"RenderingMode"
#define MIOUTOFPROCNAME             L"MIOutOfProc"
#define OUT_DIR_NAME                L".\\PlaybackOutput\\"
#define OUTPUTFILETESTNAME          L"PlaybackTest"
#define OUTPUTFILEFORMATSTRING      L"_%u_Instance_%03u.DUMP"
#define FILECOMPBUFFERLEN           1048576 // 1MB
#define BASE_PLAYBACKCONTEXT_SIZE   sizeof(CPlaybackTestContext)

namespace MFTPlaybackNS
{
    extern const WCHAR* g_pwszCMDLine;  // Anything using this lib must define the process command line
                                        // use in RunMITestsOutofProc as defined in MFTDMIHelper.h
};

using namespace MFTPlaybackNS;

#define SAFERELEASE(x) \
	{ \
		if( NULL != (x)) \
		{ \
			(x)->Release(); \
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

enum eDXVAMode
{
    DXVA_None   = 0x00000000,
    DXVA_9Only  = 0x00000001,
    DXVA_11Only = 0x00000002,
    DXVA_Both   = DXVA_9Only | DXVA_11Only,
};

CPlaybackTestContext::CPlaybackTestContext(IConfig* pConfig, IMFActivate* pActivate): 
CMIContext(pConfig, pActivate)
{
    dwSize              = sizeof(CPlaybackTestContext);
    hrAcceptable        = S_OK;
    pszHardwareURL      = NULL;
    dwTimeoutSec        = INFINITE;
    dwTestID            = 0;
    dwNumInstances      = 0;
    dwRunTimeInSec      = 0;
    dwRenderingMode     = MFTDRENDERING_EVR;
    bUseSmartSource     = FALSE;
    bPerfTest           = FALSE;
    bAllowAudio         = TRUE;
    bAllowHW            = TRUE;
    bRateless           = FALSE;
    bAllowTranscodeDec  = FALSE;
    bLowPowerTest       = FALSE;
    bRunIfMFTNotFound   = TRUE;
    pMediaTypes         = NULL;
    dwNumMediaTypes     = 0;
    un64WallTime        = 0;
    eDXVAMode           = MFTOPOLOGY_DXVA_DEFAULT;
}

CPlaybackTestContext::~CPlaybackTestContext(void)
{
    SAFEDELETE(pszHardwareURL);
    SAFEDELETE(pMediaTypes);
}

HRESULT CPlaybackTestContext::SerializeInherited(
    CSerialBuffer*  pBuffer,
    DWORD*          pdwBufferSize)
{
    HRESULT hr              = S_OK;
    DWORD   dwValue         = 0;
    DWORD   dwOutBufferSize = 0;

    do
    {
        if((pdwBufferSize == NULL))
        {
            hr = E_POINTER;
            break;
        }

        dwOutBufferSize = BASE_PLAYBACKCONTEXT_SIZE;

        dwOutBufferSize += (DWORD)(wcslen(csInFileName)                 * sizeof(WCHAR))        + sizeof(DWORD);
        dwOutBufferSize += (DWORD)(wcslen(csTestCaseName)               * sizeof(WCHAR))        + sizeof(DWORD);
        dwOutBufferSize += (DWORD)(wcslen(csAcceptableFailureReason)    * sizeof(WCHAR))        + sizeof(DWORD);
        dwOutBufferSize += (DWORD)((pszHardwareURL != NULL) ? 
            (wcslen(pszHardwareURL)                                     * sizeof(WCHAR)) : 0)   + sizeof(DWORD);
        dwOutBufferSize += (DWORD)sizeof(MFT_REGISTER_TYPE_INFO) * dwNumMediaTypes;

        if(((*pdwBufferSize) < dwOutBufferSize) ||
            (pBuffer == NULL))
        {
            hr = S_FALSE;
        }
#pragma warning( suppress:__WARNING_REDUNDANT_POINTER_TEST ) // this check is not redundant, the check above verifies against two cases so the NULL case must be verified below
        else if(pBuffer != NULL)
        {
            if(pBuffer->GetLength() < dwOutBufferSize)
            {
                hr = S_FALSE;
            }
        }

        (*pdwBufferSize)    = dwOutBufferSize;

        if(hr != S_OK)
        {
            break;
        }

        hr = pBuffer->AddData(
            &hrAcceptable,
            sizeof(hrAcceptable)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddString(csInFileName);
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddString(csTestCaseName);
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddString(csAcceptableFailureReason);
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddString(pszHardwareURL);
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &dwTimeoutSec,
            sizeof(dwTimeoutSec)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &dwTestID,
            sizeof(dwTestID)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &dwNumInstances,
            sizeof(dwNumInstances)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &dwRunTimeInSec,
            sizeof(dwRunTimeInSec)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &dwRenderingMode,
            sizeof(dwRenderingMode)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &bUseSmartSource,
            sizeof(bUseSmartSource)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &bPerfTest,
            sizeof(bPerfTest)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &bAllowAudio,
            sizeof(bAllowAudio)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &bAllowHW,
            sizeof(bAllowHW)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &bRateless,
            sizeof(bRateless)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &bAllowTranscodeDec,
            sizeof(bAllowTranscodeDec)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &bLowPowerTest,
            sizeof(bLowPowerTest)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &bRunIfMFTNotFound,
            sizeof(bRunIfMFTNotFound)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &dwNumMediaTypes,
            sizeof(dwNumMediaTypes)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            pMediaTypes,
            (sizeof(MFT_REGISTER_TYPE_INFO) * dwNumMediaTypes)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &un64WallTime,
            sizeof(un64WallTime)
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->AddData(
            &eDXVAMode,
            sizeof(eDXVAMode)
            );
        if(FAILED(hr))
        {
            break;
        }
    }while(false);

    return hr;
}

HRESULT CPlaybackTestContext::DeSerializeInherited(
    CSerialBuffer*  pBuffer)
{
    HRESULT hr          = S_OK;
    WCHAR*  pwszString  = NULL;
    DWORD   dwStringLen = 0;

    do
    {
        if(pBuffer == NULL)
        {
            hr = E_POINTER;
            break;
        }

        hr = pBuffer->GetData(
            sizeof(hrAcceptable),
            &hrAcceptable
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetString(&pwszString);
        if(FAILED(hr))
        {
            break;
        }

        csInFileName    = pwszString;
        SAFEFREE(pwszString);

        hr = pBuffer->GetString(&pwszString);
        if(FAILED(hr))
        {
            break;
        }

        csTestCaseName  = pwszString;
        SAFEFREE(pwszString);

        hr = pBuffer->GetString(&pwszString);
        if(FAILED(hr))
        {
            break;
        }

        csAcceptableFailureReason   = pwszString;
        SAFEFREE(pwszString);

        hr = pBuffer->GetString(&pwszString);
        if(FAILED(hr))
        {
            break;
        }

        SAFEDELETE(pszHardwareURL);

        if(pwszString != NULL)
        {
            dwStringLen = (DWORD)wcslen(pwszString) + 1;

            pszHardwareURL  = new WCHAR[dwStringLen];
            if(pszHardwareURL == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            wcscpy_s(
                pszHardwareURL,
                dwStringLen,
                pwszString
                );
        }

        hr = pBuffer->GetData(
            sizeof(dwTimeoutSec),
            &dwTimeoutSec
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(dwTestID),
            &dwTestID
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(dwNumInstances),
            &dwNumInstances
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(dwRunTimeInSec),
            &dwRunTimeInSec
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(dwRenderingMode),
            &dwRenderingMode
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(bUseSmartSource),
            &bUseSmartSource
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(bPerfTest),
            &bPerfTest
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(bAllowAudio),
            &bAllowAudio
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(bAllowHW),
            &bAllowHW
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(bRateless),
            &bRateless
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(bAllowTranscodeDec),
            &bAllowTranscodeDec
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(bLowPowerTest),
            &bLowPowerTest
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(bRunIfMFTNotFound),
            &bRunIfMFTNotFound
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(dwNumMediaTypes),
            &dwNumMediaTypes
            );
        if(FAILED(hr))
        {
            break;
        }

        SAFEDELETE(pMediaTypes);
        pMediaTypes = new MFT_REGISTER_TYPE_INFO[dwNumMediaTypes];
        if(pMediaTypes == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        hr = pBuffer->GetData(
            (sizeof(MFT_REGISTER_TYPE_INFO) * dwNumMediaTypes),
            pMediaTypes
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(un64WallTime),
            &un64WallTime
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pBuffer->GetData(
            sizeof(eDXVAMode),
            &eDXVAMode
            );
        if(FAILED(hr))
        {
            break;
        }
    }while(false);

    SAFEFREE(pwszString);

    return hr;
}

HRESULT VerifyAllOutputFilesMatch(UINT32 un32NumInstances, ILogger* pLogger, CPlaybackTestContext* pTestContext)
{
    HRESULT     hr                      = S_OK;
    FILE*       fpRef                   = NULL;
    FILE*       fpUnderTest             = NULL;
    BYTE*       pbInputBuff             = NULL;
    BYTE*       pbRefBuff               = NULL;
    WCHAR*      pszCurrentDirectory     = NULL;
    DWORD       dwInputBuffValidBytes   = 0;
    DWORD       dwRefBuffValidBytes     = 0;
    ULONGLONG   ullBytesVerified        = 0;
    CPath       cpFileName;
    CString     csID;

    do
    {
        if(un32NumInstances == 1)
        {
            break;
        }

        if((pLogger == NULL) || (pTestContext == NULL))
        {
            hr = E_POINTER;
            break;
        }

        DWORD dwCurrentDirectoryLen = GetCurrentDirectory(0, NULL);

        pszCurrentDirectory = new WCHAR[dwCurrentDirectoryLen];
        if(pszCurrentDirectory == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        GetCurrentDirectory(dwCurrentDirectoryLen, pszCurrentDirectory);

        cpFileName.Combine(pszCurrentDirectory, OUT_DIR_NAME);

        cpFileName.Combine(cpFileName, OUTPUTFILETESTNAME);

        cpFileName.m_strPath.AppendFormat(OUTPUTFILEFORMATSTRING, pTestContext->dwTestID, 0);

        pbInputBuff = new BYTE[FILECOMPBUFFERLEN];
        if(pbInputBuff == NULL)
        {
            LogTC(pLogger, HIGH, L"Test ERROR>> Out of memory, can't allocate file comparison buffer");
            hr = E_OUTOFMEMORY;
            break;
        }

        pbRefBuff = new BYTE[FILECOMPBUFFERLEN];
        if(pbRefBuff == NULL)
        {
            LogTC(pLogger, HIGH, L"Test ERROR>> Out of memory, can't allocate file comparison buffer");
            hr = E_OUTOFMEMORY;
            break;
        }

        if(_wfopen_s(&fpRef, (const WCHAR*)cpFileName, L"rb") != 0)
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to open output file %s", (const WCHAR*)cpFileName);
            hr = E_FAIL;
            break;            
        }

        for(UINT32 i = 1; i < un32NumInstances; i++)
        {
            fseek(fpRef, 0, SEEK_SET);

            SAFECLOSE(fpUnderTest);

            cpFileName.Combine(pszCurrentDirectory, OUT_DIR_NAME);

            cpFileName.Combine(cpFileName, OUTPUTFILETESTNAME);

            cpFileName.m_strPath.AppendFormat(OUTPUTFILEFORMATSTRING, pTestContext->dwTestID, i);

            if(_wfopen_s(&fpUnderTest, (const WCHAR*)cpFileName, L"rb") != 0)
            {
                LogTC(pLogger, HIGH, L"ERROR>> Failed to open output file %s", (const WCHAR*)cpFileName);
                hr = E_FAIL;
                break; 
            }

            while(true)
            {
                dwInputBuffValidBytes = (DWORD)fread(pbInputBuff, sizeof(BYTE), FILECOMPBUFFERLEN, fpRef);
                
                dwRefBuffValidBytes = (DWORD)fread(pbRefBuff, sizeof(BYTE), FILECOMPBUFFERLEN, fpUnderTest);

                if((ferror(fpRef) != 0) || (ferror(fpUnderTest) != 0))
                {
                    LogTC(pLogger, HIGH, L"ERROR>> An error occured while reading the output files");
                    hr = E_FAIL;
                    break;
                }

                if(dwInputBuffValidBytes != dwRefBuffValidBytes)
                {
                    LogTC(pLogger, HIGH, L"ERROR>> Output file sizes do not match");
                    hr = E_FAIL;
                    break;
                }
                
                if(dwInputBuffValidBytes == 0)
                {
                    // The files match
                    break;
                }

                if(memcmp(pbInputBuff, pbRefBuff, sizeof(BYTE) * dwInputBuffValidBytes) != 0)
                {
                    LogTC(pLogger, HIGH, L"ERROR>> Output files do not match");
                    hr = E_FAIL;
                    break;
                }

                ullBytesVerified += dwInputBuffValidBytes;
            }

            if(FAILED(hr))
            {
                break;
            }
        }

        if(FAILED(hr))
        {
            break;
        }

        if(ullBytesVerified == 0)
        {
            LogTC(pLogger, HIGH, L"ERROR>> Output files were empty");
            hr = E_FAIL;
            break;
        }
    }while(false);

    SAFECLOSE(fpRef);
    SAFECLOSE(fpUnderTest);

    do
    {
        DWORD dwDeleteFiles = 0;
        BOOL bWow64         = FALSE;

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

        for(DWORD i = 0; i < un32NumInstances; i++)
        {
            cpFileName.Combine(pszCurrentDirectory, OUT_DIR_NAME);

            cpFileName.Combine(cpFileName, OUTPUTFILETESTNAME);

            cpFileName.m_strPath.AppendFormat(OUTPUTFILEFORMATSTRING, pTestContext->dwTestID, i);

            DeleteFile((const WCHAR*)cpFileName);
        }
    }while(false);

    SAFEDELETE(pszCurrentDirectory)
    SAFEDELETE(pbInputBuff);
    SAFEDELETE(pbRefBuff);

    return hr;
}

HRESULT RunPlayback(DWORD dwInstanceID, ILogger* pLogger, CMIContext* pContext)
{
    HRESULT                     hr                          = S_OK;
    HRESULT                     hrSW                        = S_OK;
    BOOL                        bCloseSession               = FALSE;
    BOOL                        bTargetMFTFound             = FALSE;
    CTestMediaSessionPlayback*  pPlaybackSession            = NULL;
    CPlaybackTestContext*       pTestContext                = NULL;
    CPath                       cpDumpFileName;
    LARGE_INTEGER               liStart                     = {0};
    LARGE_INTEGER               liEnd                       = {0};
    CMFTDPower                  mftdpPowerSettings(TRUE);

    PERF_LOAD(PERF_LOAD_DEFAULT);

    do
    {
        pTestContext = (CPlaybackTestContext*)pContext;
        if((pLogger == NULL) || (pTestContext == NULL))
        {
            hr = E_POINTER;
            break;
        }

        if(pTestContext->dwSize != sizeof(CPlaybackTestContext))
        {
            hr = E_INVALIDARG;
            break;
        }

        hr = pTestContext->VerifyPointers();
        if(FAILED(hr))
        {
            break;
        }

        if(pTestContext->bLowPowerTest == TRUE)
        {
            hr = mftdpPowerSettings.SetPowerSaver();
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Failed to set low power mode: 0x%08X", hr);
                break;
            }
        }

        PERF_START(PERF_LOAD_DEFAULT,L"Hardware :" + pTestContext->csTestCaseName);

        BOOL bWow64 = FALSE;

        IsWow64Process(GetCurrentProcess(), &bWow64);

        if((pTestContext->dwRenderingMode == MFTDRENDERING_FILE) && (pTestContext->bPerfTest == TRUE))
        {
            pTestContext->dwRenderingMode = MFTDRENDERING_NULL;
        }

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
	        RENDERINGMODENAME, &pTestContext->dwRenderingMode);

        hr = CTestMediaSessionPlayback::CreateInstance(pLogger, pTestContext->dwRenderingMode, &pPlaybackSession);
        if(FAILED(hr))
		{
            LogTC(pLogger, HIGH, L"Failed to create instance of a playback session: 0x%08X", hr);
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully initialized playback session");
        bCloseSession = TRUE;

        if(pTestContext->dwRenderingMode == MFTDRENDERING_FILE)
        {
            cpDumpFileName = (const WCHAR*)(pTestContext->csInFileName);

            DWORD dwCurrentDirectoryLen = GetCurrentDirectory(0, NULL);

            WCHAR* pszCurrentDirectory = new WCHAR[dwCurrentDirectoryLen];
            if(pszCurrentDirectory == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            GetCurrentDirectory(dwCurrentDirectoryLen, pszCurrentDirectory);

            cpDumpFileName.Combine(pszCurrentDirectory, OUT_DIR_NAME);

            SAFEDELETE(pszCurrentDirectory)

            DWORD dwAttr = GetFileAttributes(cpDumpFileName);
            if (INVALID_FILE_ATTRIBUTES == dwAttr || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY)) 
		    {
                if (NULL == CreateDirectory(cpDumpFileName, NULL)) 
			    {
                    hr = HRESULT_FROM_WIN32(GetLastError());
                    LogTC(pLogger, HIGH, L"Test ERROR>> Unable to create directory for output file: 0x%x", hr);
                    break;
                }
            }

            cpDumpFileName.Combine(cpDumpFileName, OUTPUTFILETESTNAME);

            cpDumpFileName.m_strPath.AppendFormat(OUTPUTFILEFORMATSTRING, pTestContext->dwTestID, dwInstanceID);

            hr = pPlaybackSession->SetDumpFileName((const WCHAR*)cpDumpFileName);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Test ERROR>> Unable to set dump file name");
                break;
            }
        }

        hr = pPlaybackSession->SetRegisterWithMMCSS(TRUE);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set playback session to register with MMCSS: 0x%08X", hr);
            break;
        }

        if(pTestContext->bUseSmartSource == TRUE)
        {
            const GUID** ppGUIDs = NULL;

            do
            {
                ppGUIDs = new const GUID*[pTestContext->dwNumMediaTypes];
                if(ppGUIDs == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate supported GUID list");
                    break;
                }

                for(UINT32 i = 0; i < pTestContext->dwNumMediaTypes; i++)
                {
                    ppGUIDs[i] = &(pTestContext->pMediaTypes[i].guidSubtype);
                }

                hr = pPlaybackSession->SetUseSmartSource(pTestContext->bUseSmartSource, ppGUIDs, pTestContext->dwNumMediaTypes);
                if(FAILED(hr))
                {
                    LogTC(pLogger, HIGH, L"ERROR>> Failed to set playback session to use smart source: 0x%08X", hr);
                    break;
                }
            }while(false);

            SAFEDELETE(ppGUIDs);

            if(FAILED(hr))
            {
                break;
            }
        }

        if(pTestContext->dwRunTimeInSec > 0)
        {
            hr = pPlaybackSession->SetRunTimeInSeconds(pTestContext->dwRunTimeInSec);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"ERROR>> Failed to set playback run times: 0x%08X", hr);
                break;
            }
        }

        hr = pPlaybackSession->SetAudioAllowed(pTestContext->bAllowAudio);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set audio allowed: 0x%08X", hr);
            break;
        }

        hr = pPlaybackSession->SetHWAllowed(pTestContext->bAllowHW);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set HW Allowed: 0x%08X", hr);
            break;
        }

        hr = pPlaybackSession->SetRatelessSink(pTestContext->bRateless);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set Rateless: 0x%08X", hr);
            break;
        }

        hr = pPlaybackSession->SetAllowTranscodeDec(pTestContext->bAllowTranscodeDec);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set Transcode Dec Allowed: 0x%08X", hr);
            break;
        }

        hr = pPlaybackSession->SetDXVAMode(pTestContext->eDXVAMode);
        if(FAILED(hr))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set DXVA Mode 0x%x08X", hr);
            break;
        }

        switch(pTestContext->dwRenderingMode)
        {
        case MFTDRENDERING_EVR:
            LogTC(pLogger, NORMAL, L"INFO>> Building topology using DX9 renderer");
            break;
        case MFTDRENDERING_CUSTOMVIDRENDERER:
            LogTC(pLogger, NORMAL, L"INFO>> Building topology using DX11 renderer");
            break;
        default:
            // Nothing to do
            break;
        };

        hr = pPlaybackSession->BuildTopology(pTestContext->csInFileName, FALSE);
        if(FAILED(hr))
		{
            pPlaybackSession->SilenceOutput(); // We're already failing, no need to see more error messages

            LogTC(pLogger, HIGH, L"ERROR>> Cannot build a topology: 0x%08X", hr);

            if((pTestContext->hrAcceptable != S_OK) && (hr == pTestContext->hrAcceptable))
            {
                LogTC(pLogger, HIGH, L"INFO>> This error is acceptable for this test");
                LogTC(pLogger, HIGH, L"INFO>> Reason: %s", (const WCHAR*)(pTestContext->csAcceptableFailureReason));
                hr = S_FALSE;
                break;
            }

            HRESULT hrCallback = pPlaybackSession->GetCallbackHRESULT();
            if((pTestContext->hrAcceptable != S_OK) && (hrCallback == pTestContext->hrAcceptable))
            {
                LogTC(pLogger, HIGH, L"INFO>> This error is acceptable for this test");
                LogTC(pLogger, HIGH, L"INFO>> Reason: %s", (const WCHAR*)(pTestContext->csAcceptableFailureReason));
                hr = S_FALSE;
            }

            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully built a playback topology");

        hr = pPlaybackSession->WaitForState(STATE_TOPOLOGY_READY, pTestContext->dwTimeoutSec);
        if(FAILED(hr))
		{
            pPlaybackSession->SilenceOutput(); // We're already failing, no need to see more error messages

            LogTC(pLogger, HIGH, L"ERROR>> Cannot build a topology: 0x%08X", hr);

            if((pTestContext->hrAcceptable != S_OK) && (hr == pTestContext->hrAcceptable))
            {
                LogTC(pLogger, HIGH, L"INFO>> This error is acceptable for this test");
                LogTC(pLogger, HIGH, L"INFO>> Reason: %s", (const WCHAR*)(pTestContext->csAcceptableFailureReason));
                hr = S_FALSE;
                break;
            }

            HRESULT hrCallback = pPlaybackSession->GetCallbackHRESULT();
            if((pTestContext->hrAcceptable != S_OK) && (hrCallback == pTestContext->hrAcceptable))
            {
                LogTC(pLogger, HIGH, L"INFO>> This error is acceptable for this test");
                LogTC(pLogger, HIGH, L"INFO>> Reason: %s", (const WCHAR*)(pTestContext->csAcceptableFailureReason));
                hr = S_FALSE;
            }

            break;
        }

        if(pTestContext->dwTimeoutSec == INFINITE)
        {
            UINT64 ullDuration = 0;

            if(SUCCEEDED(pPlaybackSession->GetDuration(&ullDuration)))
            {
                pTestContext->dwTimeoutSec = (DWORD)(MediaTimeToMiliSeconds(ullDuration) * GetTimeoutMultiplier(pLogger));
                if(pTestContext->dwTimeoutSec == 0)
                {
                    pTestContext->dwTimeoutSec = INFINITE;
                }
            }
        }

        LogTC(pLogger, NORMAL, L"Topology in use:");

        if(dwInstanceID == 0)
        {
		    if(pPlaybackSession->WalkFullTopology(2, pTestContext->pActivate, pTestContext->pszHardwareURL) == S_OK)
		    {
                bTargetMFTFound = TRUE;
		    }
            else
            {
                LogTC(pLogger, NORMAL, L"INFO>> Target MFT was not used in this test");
            }
        }
        else
        {
            pPlaybackSession->WalkFullTopology(2);

            bTargetMFTFound = TRUE; // For all other instances, we don't need to verify the MFT is used
        }

        if((bTargetMFTFound == FALSE) && (pTestContext->bRunIfMFTNotFound == FALSE))
        {
            break;
        }

        LogTC(pLogger, NORMAL, L"Starting playback...");

        QueryPerformanceCounter(&liStart);

        hr = pPlaybackSession->Start();
        if(FAILED(hr))
		{
            LogTC(pLogger, HIGH, L"Error: failed to start playback topology, 0x%08X", hr);
            break;
        }

        hr = pPlaybackSession->WaitForState(STATE_PLAYING, pTestContext->dwTimeoutSec);
        if(FAILED(hr))
		{
            LogTC(pLogger, HIGH, L"Error: call to start a media session succeeded, but session did not get to a running state over %ld sec, 0x%08X", pTestContext->dwTimeoutSec, hr);
            break;
        }

        LogTC(pLogger, NORMAL, L"Media session is in playing state, waiting for completion");

        hr = pPlaybackSession->WaitForState(STATE_STOPPED, pTestContext->dwTimeoutSec);
        if(FAILED(hr))
		{
            LogTC(pLogger, HIGH, L"Error: failed to wait for ending of the media session (stopping), 0x%08X", hr);
            break;
        }

        QueryPerformanceCounter(&liEnd);

        pTestContext->un64WallTime = liEnd.QuadPart - liStart.QuadPart;

        if(pTestContext->dwRenderingMode == MFTDRENDERING_FILE)
        {
            DWORD dwWidth = 0;
            DWORD dwHeight = 0;
            GUID guidSubType = GUID_NULL;

            pPlaybackSession->GetDumpFileInfo(&dwWidth, &dwHeight, &guidSubType);

            WriteInfoFile(cpDumpFileName, dwWidth, dwHeight, &guidSubType);
        }

        hr = pPlaybackSession->Close();

        bCloseSession = FALSE;

        if(FAILED(hr))
		{
            LogTC(pLogger, HIGH, L"Failed to close playback session, 0x%08X", hr);
            break;
        }

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

        PERF_STOP(PERF_STOP_DEFAULT);

        if(bTargetMFTFound == FALSE)
        {
            LogTC(pLogger, NORMAL, L"Info: Since Target MFT was not found, no software comparison will occur");
            break;
        }

        if(pTestContext->bPerfTest == FALSE)
        {
            break;
        }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                               Software Comparison Playback
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        LogTC(pLogger, HIGH, L"Starting Test for Software Comparison Component");

        PERF_START(PERF_LOAD_DEFAULT,L"Software :" + pTestContext->csTestCaseName);
        
        hrSW = CTestMediaSessionPlayback::CreateInstance(pLogger, pTestContext->dwRenderingMode, &pPlaybackSession);
        if(FAILED(hrSW))
		{
            LogTC(pLogger, HIGH, L"Failed to create instance of a playback session: 0x%08X", hrSW);
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully initialized playback session");
        
        bCloseSession = TRUE;

        if(pTestContext->dwRenderingMode == MFTDRENDERING_FILE)
        {
            cpDumpFileName.RenameExtension(L"_SW.DUMP");

            hr = pPlaybackSession->SetDumpFileName((const WCHAR*)cpDumpFileName);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"Test ERROR>> Unable to set dump file name");
                break;
            }
        }

        hrSW = pPlaybackSession->SetRegisterWithMMCSS(TRUE);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set playback session to register with MMCSS: 0x%08X", hrSW);
            break;
        }

        hrSW = pPlaybackSession->SetUseSmartSource(pTestContext->bUseSmartSource);
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"ERROR>> Failed to set playback session to use smart source: 0x%08X", hrSW);
            break;
        }

        hrSW = pPlaybackSession->SetAudioAllowed(pTestContext->bAllowAudio);
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

        hrSW = pPlaybackSession->BuildTopology(pTestContext->csInFileName, FALSE);
        if(FAILED(hrSW))
		{
            LogTC(pLogger, HIGH, L"Error: failed to build playback session: 0x%08X", hrSW);
            break;
        }

        LogTC(pLogger, NORMAL, L"Successfully built a playback topology");

        hrSW = pPlaybackSession->WaitForState(STATE_TOPOLOGY_READY, pTestContext->dwTimeoutSec);
        if(FAILED(hrSW))
		{
            LogTC(pLogger, HIGH, L"Error: cannot build a topology: 0x%08X", hrSW);
            break;
        }

		LogTC(pLogger, NORMAL, L"Topology in use:");
		if(FAILED(pPlaybackSession->WalkFullTopology(2)))
		{
			LogTC(pLogger, HIGH, L"Failed to walk topology");	
		}

        LogTC(pLogger, NORMAL, L"Starting playback...");

        hrSW = pPlaybackSession->Start();
        if(FAILED(hrSW))
		{
            LogTC(pLogger, HIGH, L"Error: failed to start playback topology, 0x%08X", hrSW);
            break;
        }

        hrSW = pPlaybackSession->WaitForState(STATE_PLAYING, pTestContext->dwTimeoutSec);
        if(FAILED(hrSW))
		{
            LogTC(pLogger, HIGH, L"Error: call to start a media session succeeded, but session did not get to a running state over %ld sec, 0x%08X", 
                pTestContext->dwTimeoutSec, hrSW);
            break;
        }

        LogTC(pLogger, NORMAL, L"Media session is in playing state, waiting for completion");

        hrSW = pPlaybackSession->WaitForState(STATE_STOPPED, pTestContext->dwTimeoutSec);
        if(FAILED(hrSW))
		{
            LogTC(pLogger, HIGH, L"Error: failed to wait for ending of the media session (stopping), 0x%08X", hrSW);
            break;
        }

        if(pTestContext->dwRenderingMode == MFTDRENDERING_FILE)
        {
            DWORD dwWidth = 0;
            DWORD dwHeight = 0;
            GUID guidSubType = GUID_NULL;

            pPlaybackSession->GetDumpFileInfo(&dwWidth, &dwHeight, &guidSubType);

            WriteInfoFile(cpDumpFileName, dwWidth, dwHeight, &guidSubType);
        }

        hrSW = pPlaybackSession->Close();

        bCloseSession = FALSE;

        if(FAILED(hrSW))
		{
            LogTC(pLogger, HIGH, L"Failed to close playback session, 0x%08X", hrSW);
            break;
        }

        hrSW = pPlaybackSession->GetCallbackHRESULT();
        if(FAILED(hrSW))
        {
            LogTC(pLogger, HIGH, L"Error: Media Session recieved a failed event, 0x%x", hrSW);
            break;
        }

        PERF_STOP(PERF_STOP_DEFAULT);
    }while(false);

    if(TRUE == bCloseSession && NULL != pPlaybackSession)
	{
        HRESULT hrTemp = pPlaybackSession->Close();
        if(FAILED(hrTemp))
		{
            LogTC(pLogger, HIGH, L"Error: failed to close playback session, 0x%08X", hrTemp);
        }
    }

    HRESULT hrPerf = S_OK;
    if(SUCCEEDED(hr) && (bTargetMFTFound == TRUE))
	{
        hrPerf = PERF_POSTPROCESS(PERF_POSTPROCESS_DEFAULT);
        
        HandlePerfLogs(pLogger, hrPerf, 2);
    }
    if(FAILED(hr) || FAILED(hrSW))
	{
        PERF_STOP(PERF_STOP_DEFAULT);
    }
    if(FAILED(hrPerf))
	{
        if((pTestContext != NULL) && (pLogger != NULL))
        {
            LogTC(pLogger, HIGH, L"This playback session did not meet the performance requirements");
        }
        if(SUCCEEDED(hr))
        {
            hr = E_FAIL;
        }
    }
    
    SAFERELEASE(pPlaybackSession);

    do
    {
        if(pTestContext == NULL)
        {
            break;
        }

        if(pTestContext->dwRenderingMode == MFTDRENDERING_EVR)
        {
            break;
        }

        if(pTestContext->dwNumInstances > 1)
        {
            break;
        }

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

        DeleteFile((const WCHAR*)cpDumpFileName);
            
        cpDumpFileName.m_strPath.Replace(L"_SW.DUMP", L".DUMP");

        DeleteFile((const WCHAR*)cpDumpFileName);
    }while(false);

    if((hr != S_SKIPPED) && SUCCEEDED(hr) && (bTargetMFTFound == FALSE))
    {
        hr = S_FALSE;
    }

    return hr;
}

HRESULT VerifyPlaybackE2E(ILogger* pLogger, IConfig* pConfig, IMFActivate* pMFTActivate)
{
    HRESULT                     hr                  = S_OK;
    IMFAttributes*              pMFTAttributes      = NULL;
    CPlaybackTestContext        ptcContext(pConfig, pMFTActivate);
    DWORD                       dwVerifyMIOutput    = 0;
    DWORD                       dwDXVA9or11         = DXVA_11Only;
    BOOL                        bMIInProc           = TRUE;
    //CScreenIdleControl          ScreenIdle;

    do
	{
        if((pLogger == NULL) || (pConfig == NULL) || (pMFTActivate == NULL))
        {
            hr = E_POINTER;
            break;
        }

       /* hr = ScreenIdle.PreventScreenFromPoweringOff();
        if(FAILED(hr))
        {
            pLogger->Log(NORMAL, L"ERROR>> Failed to set prevent screen from idling off (hr=0x%x)", hr);
            break;
        }*/

        BOOL bWow64 = FALSE;

        IsWow64Process(GetCurrentProcess(), &bWow64);

        hr = pConfig->GetString(L"Description", ptcContext.csTestCaseName);
        if(FAILED(hr))
		{
            LogTC(pLogger, HIGH, L"Test Error: Failed to find Test Case Description :0x%x", hr);
        }

        LogTC(pLogger, HIGH, L"Start Test: %s", (const WCHAR*)(ptcContext.csTestCaseName));

        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
		{
            LogTC(pLogger, HIGH, L"System Error: failed in MFStartup(): 0x%08X", hr);
            break;
        }

        hr = pMFTActivate->QueryInterface(IID_IMFAttributes, (void**)&pMFTAttributes);
        if(FAILED(hr))
		{
            LogTC(pLogger,HIGH, L"MF ERROR>> Query Interface(IID_IMFAttributes) failed :0x%x", hr);
            break;
        }
        if(pMFTAttributes == NULL)
		{
            hr = E_POINTER;
            LogTC(pLogger,HIGH, L"MF ERROR>> IMFAttribute is null :0x%x", hr);
            break;
        }

        GUID guidMFT = GUID_NULL;
        if(SUCCEEDED(pMFTAttributes->GetGUID(MFT_TRANSFORM_CLSID_Attribute, &guidMFT)))
        {
            WCHAR* pszGUID = NULL;
            WCHAR* pszFriendlyName = NULL;
            UINT32 un32FriendlyNameLen = 0;

            StringFromCLSID(guidMFT, &pszGUID);

            LogTC(pLogger, NORMAL, L"INFO>> MFT Under Test: %s", (pszGUID != NULL) ? pszGUID : L"Unknown");

            do
            {
                
                if(FAILED(pMFTAttributes->GetStringLength(MFT_FRIENDLY_NAME_Attribute, &un32FriendlyNameLen)))
                {
                    break;
                }

                un32FriendlyNameLen++;

                pszFriendlyName = new WCHAR[un32FriendlyNameLen];
                if(pszFriendlyName == NULL)
                {
                    break;
                }

                if(FAILED(pMFTAttributes->GetString(MFT_FRIENDLY_NAME_Attribute, pszFriendlyName, un32FriendlyNameLen, NULL)))
                {
                    break;
                }

                LogTC(pLogger, NORMAL, L"INFO>>     Friendly Name: %s", pszFriendlyName);
            }while(false);

            if(pszGUID != NULL)
            {
                CoTaskMemFree(pszGUID);
            }
            SAFEDELETE(pszFriendlyName);
        }
        else
        {
            LogTC(pLogger, HIGH, L"WARNING>> Unable to get CLSID from activate object");
        }

        GUID guidCategory = GUID_NULL;
        hr = pMFTAttributes->GetGUID(MF_TRANSFORM_CATEGORY_Attribute, &guidCategory);
        if(FAILED(hr))
		{
            LogTC(pLogger,HIGH, L"MF ERROR>> MF_TRANSFORM_CATEGORY_Attribute not found :0x%x", hr);
            break;
        }

        UINT32 un32TransformAttrib = 0;
        hr = pMFTAttributes->GetUINT32(MF_TRANSFORM_FLAGS_Attribute, &un32TransformAttrib);
        if(FAILED(hr))
		{
            LogTC(pLogger,HIGH, L"MF ERROR>> MF_TRANSFORM_FLAGS_Attribute not found :0x%x", hr);
            break;
        }
        
        if(guidCategory != MFT_CATEGORY_VIDEO_DECODER)
        {
            hr = S_OK;
            LogTC(pLogger, NORMAL, L"INFO>> This test is not applicable to this component (it is not a video decoder)");
            break;
        }

        if(un32TransformAttrib & MFT_ENUM_FLAG_TRANSCODE_ONLY)
        {
            ptcContext.dwRenderingMode      = MFTDRENDERING_FILE;
            ptcContext.bAllowTranscodeDec   = TRUE;
        }

        DWORD dwDumpPlayback = 0;

        if(SUCCEEDED(GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
	        DUMPPLAYBACKNAME, &dwDumpPlayback)))
        {
            if(dwDumpPlayback == 0)
            {
                ptcContext.dwRenderingMode = MFTDRENDERING_EVR;
            }
            else
            {
                ptcContext.dwRenderingMode = MFTDRENDERING_FILE;
            }
        }

        BOOL bContinueTest = FALSE;
        IMFAttributes* pTestAttributes = NULL;

        do
        {
            UINT32 un32MediaTypeSize = 0;

            hr = pMFTAttributes->GetBlobSize(MFT_INPUT_TYPES_Attributes, &un32MediaTypeSize);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> MFT_INPUT_TYPES_Attributes not found: 0x%x", hr);
                break;
            }

            ptcContext.dwNumMediaTypes = un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO);

            ptcContext.pMediaTypes = new MFT_REGISTER_TYPE_INFO[ptcContext.dwNumMediaTypes];
            if(ptcContext.pMediaTypes == NULL)
            {
                hr = E_OUTOFMEMORY;
                LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate MediaType info blob");
                break;
            }

            hr = MFCreateAttributes(&pTestAttributes, 1);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to allocate Test Attribute Store: 0x%x", hr);
                break;
            }

            hr = pMFTAttributes->GetBlob(MFT_INPUT_TYPES_Attributes, (BYTE*)(ptcContext.pMediaTypes), un32MediaTypeSize, NULL);
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_INPUT_TYPES_Attributes: 0x%x", hr);
                break;
            }

            if(FAILED(pConfig->GetAttributesFromNode(L"InputType", pTestAttributes)))
            {
                LogTC(pLogger, HIGH, L"TEST ERROR>> Failed to get InputType attribute from XML");
                hr = E_FAIL;
                break;
            }

            hr = VerifyTestSupported(ptcContext.pMediaTypes, ptcContext.dwNumMediaTypes, 
                pTestAttributes);
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

        SAFERELEASE(pTestAttributes);

        if(FAILED(hr) || (bContinueTest == FALSE))
        {
            break;
        }

        UINT32 un32Merit = MFGetAttributeUINT32(pMFTAttributes, MFT_CODEC_MERIT_Attribute, 0);

        if(un32Merit > 0)
        {
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

                UINT32 un32HardwareURLLen = 0;
                if(SUCCEEDED(pTransformAttrib->GetStringLength(MFT_ENUM_HARDWARE_URL_Attribute, &un32HardwareURLLen)))
                {
                    ptcContext.pszHardwareURL = new WCHAR[++un32HardwareURLLen];
                    if(ptcContext.pszHardwareURL == NULL)
                    {
                        LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate HardwareURL string");
                        hr = E_OUTOFMEMORY;
                        break;
                    }

                    memset(ptcContext.pszHardwareURL, 0, sizeof(WCHAR) * un32HardwareURLLen);

                    hr = pTransformAttrib->GetString(MFT_ENUM_HARDWARE_URL_Attribute, ptcContext.pszHardwareURL, un32HardwareURLLen, NULL);
                    if(FAILED(hr))
                    {
                        LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_ENUM_HARDWARE_URL_Attribute: 0x%x", hr);
                        break;
                    }

                    ptcContext.bAllowHW = TRUE;

                    LogTC(pLogger, HIGH, L"Starting Test for Hardware Component");
                }
                else
                {
                    LogTC(pLogger, NORMAL, L"INFO>> Software component under test (no MFT_ENUM_HARDWARE_URL_Attribute found), turning off HW MFTs");

                    ptcContext.bAllowHW = FALSE;
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
        }
        else
        {
            LogTC(pLogger, NORMAL, L"INFO>> Software component under test (Merit was %u), turning off HW MFTs", un32Merit);

            ptcContext.bAllowHW = FALSE;
        }

        RegHWSWOverload(pLogger);

        hr = pConfig->GetString(L"InputFile", ptcContext.csInFileName);
        if(FAILED(hr))
		{
            LogTC(pLogger, HIGH, L"Test Error: Failed to find InputFile :0x%x", hr);
            break;
        }

        pConfig->GetUINT32(L"TimeoutSec", (UINT*)&(ptcContext.dwTimeoutSec));
        if(ptcContext.dwTimeoutSec == 0)
        {
            ptcContext.dwTimeoutSec = INFINITE;
        }

        CString csTestID;

        if(SUCCEEDED(pConfig->GetAttribute(L"testId", csTestID)))
        {
            swscanf_s((const WCHAR*)csTestID, L"%u", &(ptcContext.dwTestID));
        }

        pConfig->GetUINT32(L"AcceptableHR", (UINT32*)&(ptcContext.hrAcceptable));
        pConfig->GetString(L"AcceptableReason", ptcContext.csAcceptableFailureReason);

        UINT32 un32Val = 1;

        pConfig->GetUINT32(L"NumInstances", &un32Val);

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
	        NUMINSTANCESNAME, (DWORD*)&un32Val);

        ptcContext.dwNumInstances = (DWORD)un32Val;

        un32Val = 0;

        pConfig->GetUINT32(L"FormatChange", &un32Val);

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
	        SMARTSOURCENAME, (DWORD*)&un32Val);

        ptcContext.bUseSmartSource = (un32Val == 0) ? FALSE : TRUE;

        un32Val = 0;

        pConfig->GetUINT32(L"RunTimeInSeconds", &un32Val);

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
	        RUNTIMENAME, (DWORD*)&un32Val);

        ptcContext.dwRunTimeInSec = (DWORD)un32Val;

        if(ptcContext.dwRunTimeInSec > 0)
        {
            ptcContext.bUseSmartSource = TRUE;
        }

        un32Val = 0;

        pConfig->GetUINT32(L"PerfTest", &un32Val);

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
	        PERFTESTNAME, (DWORD*)&un32Val);

        ptcContext.bPerfTest = (un32Val == 0) ? FALSE : TRUE;
        
        pConfig->GetUINT32(L"VerifyMIOutput", (UINT32*)&dwVerifyMIOutput);

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
	        VERIFYMIOUTPUTNAME, (DWORD*)&dwVerifyMIOutput);

        if((dwVerifyMIOutput != 0) && (ptcContext.dwNumInstances > 1))
        {
            // In order to verify the output, we have to save it
            ptcContext.dwRenderingMode = MFTDRENDERING_FILE;
        }

        un32Val = 0;

        pConfig->GetUINT32(L"DisableAudio", &un32Val);

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
	        DISABLEAUDIONAME, (DWORD*)&un32Val);

        ptcContext.bAllowAudio = (un32Val == 0) ? TRUE : FALSE;

        if((ptcContext.bPerfTest == TRUE) && ptcContext.pszHardwareURL == NULL)
        {
            LogTC(pLogger, NORMAL, L"INFO>> Performance tests do not apply to non-hardware components");
            hr = S_SKIPPED;
            break;
        }

        un32Val = 0;

        pConfig->GetUINT32(L"MIOutOfProc", &un32Val);

        GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
	        MIOUTOFPROCNAME, (DWORD*)&un32Val);

        ptcContext.bInproc = (un32Val == 0) ? TRUE : FALSE;

        if(ptcContext.dwRenderingMode != MFTDRENDERING_FILE)
        {
            CString csDXVAType;

            pConfig->GetString(L"DXVAType", csDXVAType);

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
        }
        else
        {
            dwDXVA9or11     = DXVA_None;
        }

        for(DWORD dwTest = 0; dwTest < 2; dwTest++)
        {
            BOOL    bRunTest    = TRUE;
            HRESULT hrTest      = S_OK;

            if(dwDXVA9or11 != DXVA_None)
            {
                switch(dwTest)
                {
                case 0:
                    {
                        if((dwDXVA9or11 & DXVA_11Only) == 0)
                        {
                            bRunTest    = FALSE;
                            break;
                        }

                        ptcContext.dwRenderingMode  = MFTDRENDERING_CUSTOMVIDRENDERER;
                    }
                    break;
                case 1:
                    {
                        if((dwDXVA9or11 & DXVA_9Only) == 0)
                        {
                            bRunTest    = FALSE;
                            break;
                        }

                        ptcContext.dwRenderingMode  = MFTDRENDERING_EVR;
                    }
                    break;
                default:
                    // Nothing to do
                    break;
                };
            }

            if(bRunTest == FALSE)
            {
                continue;
            }

            if(ptcContext.dwNumInstances > 1)
            {
                // For mulitple instance tests, pull down the input source file before the mulitple instances run
                // so they don't step on eachother's toes trying to copy the file
                WCHAR* pwszActualContentFilePath    = NULL;

                hrTest = FindHCKContentFile(
                    ptcContext.csInFileName,
                    &pwszActualContentFilePath
                    );
                if(FAILED(hrTest))
                {
                    LogTC(pLogger, HIGH, L"ERROR>> Failed to find input file %s", (const WCHAR*)ptcContext.csInFileName);

                    if(SUCCEEDED(hr))
                    {
                        hr = hrTest;
                    }

                    continue;
                }

#pragma prefast(push)
#pragma prefast(disable:26006, "disabling warning for test code")
                ptcContext.csInFileName = pwszActualContentFilePath;
#pragma prefast(pop)
                SAFEFREE(pwszActualContentFilePath);
            }
            
            if(ptcContext.bInproc == TRUE)
            {
                hrTest  = RunMITests(ptcContext.dwNumInstances, pLogger, &ptcContext, &RunPlayback);
                if(hrTest != S_OK)
                {
                    if(SUCCEEDED(hr))
                    {
                        hr = hrTest;
                    }
                }
            }
            else
            {
                hrTest  = RunMITestsOutofProc(
                    ptcContext.dwNumInstances,
                    pLogger,
                    &ptcContext, 
                    g_pwszCMDLine
                    );
                if(hrTest != S_OK)
                {
                    if(SUCCEEDED(hr))
                    {
                        hr = hrTest;
                    }
                }
            }
        }

        if(hr != S_OK)
        {
            break;
        }

        if(ptcContext.dwNumInstances == 1)
        {
            break;
        }

       if(dwVerifyMIOutput == 0)
        {
            break;
        }

        LogTC(pLogger, NORMAL, L"INFO>> Multiple instace test, verifying output files match");

        hr = VerifyAllOutputFilesMatch(ptcContext.dwNumInstances, pLogger, &ptcContext);
        if(FAILED(hr))
        {
            break;
        }

        LogTC(pLogger, NORMAL, L"SUCCESS>> All output files match, multiple instance verified");
    }while(false);

    SAFERELEASE(pMFTAttributes);

    MFShutdown();

    return hr;
}

HRESULT InitializePlaybackTestTable(CXmlRootConfig** ppXMLRootConfig)
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
			(TCHAR*)&InitializePlaybackTestTable, &hMod) == FALSE)
		{
			hr = E_FAIL;
			break;
		}

		HRSRC hRsrc = FindResource(hMod, MAKEINTRESOURCE(IDR_PLAYBACKCONFIG), L"Config");
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

HRESULT RunPlaybackTest(const DWORD dwTestID, ILogger* pLogger, CXmlRootConfig* pXMLConfig, IMFActivate* pActivate)
{
    HRESULT hr = S_OK;
    CXmlConfig* pTestConfig = NULL;

    do
    {
        if((pLogger == NULL) || (pXMLConfig == NULL) || (pActivate == NULL))
        {
            hr = E_POINTER;
            break;
        }
 
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

        hr = VerifyPlaybackE2E(pLogger, pTestConfig, pActivate);
        if(FAILED(hr))
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

    SAFEDELETES(pTestConfig);

    return hr;
}

HRESULT RunChildPlaybackTest(
    ILogger*        pLogger,
    const DWORD     dwInstance,
    const WCHAR*    pwszContextName,
    const DWORD     dwContextSize)
{
    HRESULT                 hr          = S_OK;
    CPlaybackTestContext    cptcContext(NULL, NULL);
    BOOL                    bMFStarted  = FALSE;

    do
    {
        hr = MFStartup(MF_VERSION);
        if(FAILED(hr))
		{
            LogTC(pLogger, HIGH, L"System Error: failed in MFStartup(): 0x%08X", hr);
            break;
        }

        bMFStarted  = TRUE;

        hr = OutOfProcEntrypoint(
            pLogger,
            &cptcContext,
            dwInstance,
            pwszContextName,
            dwContextSize,
            &RunPlayback
            );
    }while(false);

    if(bMFStarted != FALSE)
    {
        MFShutdown();
    }

    return hr;
}