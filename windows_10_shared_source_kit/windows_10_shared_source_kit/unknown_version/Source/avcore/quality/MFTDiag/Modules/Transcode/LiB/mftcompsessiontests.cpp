#include <stdafx.h>
#include "MFTCompsessionTests.h"
#include "TranscodeProfileReader.h"
#include "CTestMediaSessionTranscode.h"
#include "CTestMediaSessionPlayback.h"
#include <Mmsystem.h>
#include "PerfHelper.h"
#include "LogTC.h"
#include "H264ESAnalyzer.h"
#include "M4S2ESAnalyzer.h"
#include "VC1ESAnalyzer.h"
#include "WMV9ESAnalyzer.h"
#include "CConfigRegKey.h"
#include "resource.h"
#include "VerifyTestSupported.h"
#include "MediaTimeConverter.h"
#include "MFTDMIHelper.h"
#include "MFTDPerfHelper.h"
#include "MFTDFileHelpers.h"
#include "MFTDiagCustomAttributes.h"
#include "FindHCKContentFile.h"

#define SMARTSOURCENAME                     L"UseSmartSource"
#define RUNTIMENAME                         L"RunTime"
#define NUMINSTANCESNAME                    L"NumInstances"
#define VERIFYMIOUTPUTNAME                  L"VerifyMIOutput"
#define DELETEFILESONSUCCEESSNAME           L"TranscodeDeleteOnSuccess"
#define DELETEFILESONFAILURENAME            L"TranscodeDeleteOnFailure"
#define PERFTESTNAME                        L"PerfTest"
#define TRANSCODEFILESIZELOWERLIMITNAME     L"TranscodeFileSizeLowerLimit_x100"
#define TRANSCODEFILESIZELOWERLIMITRATIO    100.0 // This should match the scaling factor from above
#define TRANSCODEFILESIZEUPPERLIMITNAME     L"TranscodeFileSizeLowerLimit_x100"
#define TRANSCODEFILESIZEUPPERLIMITRATIO    100.0 // This should match the scaling factor from above
#define OUTPUTINSTANCEFORMATSTRING          L"_Instance_%03u"
#define OUT_DIR_NAME                        L"TranscodeOutput"
#define EXT_ASF                             L".asf"
#define EXT_MP4                             L".mp4"
#define EXT_MP3                             L".mp3"
#define EXT_3GP                             L".3gp"

#define FILECOMPBUFFERLEN                   1048576 // 1MB

#define DEFAULT_FILESIZE_RATIO_LOWER_LIMIT  .8
#define DEFAULT_FILESIZE_RATIO_UPPER_LIMIT  1.2

#define SAFERELEASE(x) \
	{ \
		if( NULL != (x)) \
		{ \
			(x)->Release(); \
			(x) = NULL; \
		} \
	}

#define CHKHR_BRK(x) \
	{ \
		hr = (x); \
		if(FAILED(hr)) \
		{ \
			break; \
		}\
	}

#define SAFEDELETES(x) \
	{ \
		if((x) != NULL) \
		{ \
			delete (x); \
			(x) = NULL; \
		} \
	}

#define SAFEDELETE(x) \
	{ \
		if((x) != NULL) \
		{ \
			delete[] (x); \
			(x) = NULL; \
		} \
	}

#define SAFESPRELEASE(x) \
	{ \
		if((x) != NULL) \
		{ \
			(x).Release(); \
			(x) = NULL; \
		} \
	}

#define SAFEFREE(x) \
	{ \
		if((x) != NULL) \
		{ \
			free(x); \
			(x) = NULL; \
		} \
	}

#define SAFECLOSE(x) \
	{ \
		if((x) != NULL) \
		{ \
			fclose(x); \
			(x) = NULL; \
		} \
	}

#define SAFEUNLOCK(x) \
	{ \
		if((x) != NULL) \
		{ \
			(x)->Unlock(); \
		} \
	}

typedef enum
{
	CONTAINER_TYPE_INVALID, 
	CONTAINER_TYPE_ASF, 
	CONTAINER_TYPE_MPEG4,
	CONTAINER_TYPE_MP3,
	CONTAINER_TYPE_3GP,
} CONTAINER_TYPE;

// Logging Wrapper
static ILogger* g_pLogger = NULL;

class CTranscodeTestContext: public CMIContext
{
public:
	DWORD                   dwTimeoutSec;
	DWORD                   dwOutputType;
	DWORD                   dwNumInstances;
	DWORD                   dwRunTimeInSec;
	CString                 csInFileName;
	CString                 csTestCaseName;
	CString                 csAcceptableFailureReason;
	CPath                   cpOutFileName;
	HRESULT                 hrAcceptable;
	WCHAR*                  pszHardwareURL;
	IMFTranscodeProfile*    pTranscodeProfile;
	BOOL                    bUseSmartSource;
	BOOL                    bPerfTest;
	BOOL                    bAllowHW;
	MFT_REGISTER_TYPE_INFO* pMediaTypes;
	UINT32                  dwNumMediaTypes;

	CTranscodeTestContext(IConfig* pConfig, IMFActivate* pActivate): 
		CMIContext(pConfig, pActivate)
	{
		dwSize              = sizeof(CTranscodeTestContext);
		dwOutputType        = TRANSCODE_OUTPUT_UNKNOWN;
		dwTimeoutSec        = INFINITE;
		dwNumInstances      = 0;
		dwRunTimeInSec      = 0;
		hrAcceptable        = S_OK;
		pszHardwareURL      = NULL;
		pTranscodeProfile   = NULL;
		bUseSmartSource     = FALSE;
		bPerfTest           = FALSE;
		bAllowHW            = TRUE;
		pMediaTypes         = NULL;
		dwNumMediaTypes     = 0;
	}

	~CTranscodeTestContext(void)
	{
		SAFEDELETE(pszHardwareURL);
		SAFERELEASE(pTranscodeProfile);
		SAFEDELETE(pMediaTypes);
	}
};

int MFTDLog(const TCHAR* pszText, ...)
{
	va_list vargs;
	va_start(vargs, pszText);

	if(g_pLogger != NULL)
	{
		g_pLogger->LogArgs(NORMAL, pszText, vargs);
	}

	return 0;
}

static HRESULT SetOutputFileExt(__in IMFTranscodeProfile* pProfile, __out CONTAINER_TYPE *pContainerType)
{
	HRESULT hr = S_OK;
	IMFAttributes *pContAttrs = NULL;
	UINT32 cContAttrsCount = 0;
	MF_ATTRIBUTE_TYPE attrType = MF_ATTRIBUTE_IUNKNOWN;
	PROPVARIANT varAttrValue;
	GUID guidAttr = GUID_NULL;
	GUID guidAttrVal = GUID_NULL;

	if(!pProfile || !pContainerType)
	{
		return E_INVALIDARG;
	}

    PropVariantInit(&varAttrValue);

	do
	{
		CHKHR_BRK(pProfile->GetContainerAttributes(&pContAttrs));
		CHKHR_BRK(pContAttrs->LockStore());
		CHKHR_BRK(pContAttrs->GetCount(&cContAttrsCount));		

		for (DWORD j = 0; j < cContAttrsCount; j++)
		{
			PropVariantClear(&varAttrValue);
			CHKHR_BRK(pContAttrs->GetItemByIndex(j, 
				&guidAttr, 
				&varAttrValue));
			CHKHR_BRK(pContAttrs->GetItemType(guidAttr, 
				&attrType));
			if (MF_TRANSCODE_CONTAINERTYPE == guidAttr)
			{
				CHKHR_BRK(pContAttrs->GetGUID(guidAttr, 
					&guidAttrVal));
				if (MFTranscodeContainerType_ASF == guidAttrVal)
				{
					*pContainerType = CONTAINER_TYPE_ASF;
				}
				else if (MFTranscodeContainerType_MPEG4 == guidAttrVal)
				{
					*pContainerType = CONTAINER_TYPE_MPEG4;
				}
				else if (MFTranscodeContainerType_MP3 == guidAttrVal)
				{
					*pContainerType = CONTAINER_TYPE_MP3;
				}
				else if (MFTranscodeContainerType_3GP == guidAttrVal)
				{
					*pContainerType = CONTAINER_TYPE_3GP;
				}
				else
				{
					*pContainerType = CONTAINER_TYPE_INVALID;
					CHKHR_BRK(E_FAIL);
				}
				break;
			}
		}

	} while (FALSE);

	pContAttrs->UnlockStore();
	SAFERELEASE(pContAttrs);

	PropVariantClear(&varAttrValue);

	return hr;
}

HRESULT VerifyAllOutputFilesMatch(UINT32 un32NumInstances, ILogger* pLogger, CTranscodeTestContext* pTestContext)
{
	HRESULT             hr                      = S_OK;
	HRESULT             hrRefBuff               = S_OK;
	HRESULT             hrCurrBuff              = S_OK;
	IMFSourceReader*    pReaderRef              = NULL;
	IMFSourceReader*    pReaderCurr             = NULL;
	IMFMediaType*       pMT                     = NULL;
	IMFSample*          pRefSample              = NULL;
	IMFSample*          pCurrSample             = NULL;
	IMFMediaBuffer*     pRefBuffer              = NULL;
	IMFMediaBuffer*     pCurrBuffer             = NULL;
	ULONGLONG           ullFramesVerified       = 0;
	LONGLONG            llTimeStamp             = 0;
	CPath               cpFileName;
	CString             csID;
	PROPVARIANT         propvarStartPos         = {0};
	DWORD               dwSRRefFlags            = 0;
	DWORD               dwSRCurrFlags           = 0;
	DWORD               dwSRStreamIndex         = 0;
	DWORD               dwRefBuffLen            = 0;
	DWORD               dwCurrBuffLen           = 0;
	BYTE*               pbRefBuff               = NULL;
	BYTE*               pbCurrBuff              = NULL;

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

		propvarStartPos.vt = VT_I8;
		propvarStartPos.hVal.QuadPart = 0;

		csID.Format(OUTPUTINSTANCEFORMATSTRING, 0);

		cpFileName = pTestContext->cpOutFileName;
		cpFileName.RenameExtension(csID + pTestContext->cpOutFileName.GetExtension());

		hr = MFCreateSourceReaderFromURL((const WCHAR*)cpFileName, NULL, &pReaderRef);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"ERROR>> Failed to create reference source reader instance from file %s (hr=0x%x)", 
				(const WCHAR*)cpFileName, hr);
			break;
		}

		hr = pReaderRef->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &pMT);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"ERROR>> Failed to get native media type from reference source reader (hr=0x%x)", hr);
			break;
		}

		hr = pReaderRef->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pMT);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"ERROR>> Failed to set native media type on reference source reader (hr=0x%x)", hr);
			break;
		}

		SAFERELEASE(pMT);

		for(UINT32 i = 1; i < un32NumInstances; i++)
		{
			hr = pReaderRef->SetCurrentPosition(GUID_NULL, propvarStartPos);
			if(FAILED(hr))
			{
				LogTC(pLogger, HIGH, L"ERROR>> Failed to reference seek source reader to beginning (hr=0x%x)", hr);
				break;
			}

			SAFERELEASE(pReaderCurr);

			csID.Format(OUTPUTINSTANCEFORMATSTRING, i);

			cpFileName = pTestContext->cpOutFileName;
			cpFileName.RenameExtension(csID + pTestContext->cpOutFileName.GetExtension());

			hr = MFCreateSourceReaderFromURL((const WCHAR*)cpFileName, NULL, &pReaderCurr);
			if(FAILED(hr))
			{
				LogTC(pLogger, HIGH, L"ERROR>> Failed to create current source reader instance from file %s (hr=0x%x)", 
					(const WCHAR*)cpFileName, hr);
				break;
			}

			hr = pReaderCurr->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &pMT);
			if(FAILED(hr))
			{
				LogTC(pLogger, HIGH, L"ERROR>> Failed to get native media type from current source reader (hr=0x%x)", hr);
				break;
			}

			hr = pReaderCurr->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, pMT);
			if(FAILED(hr))
			{
				LogTC(pLogger, HIGH, L"ERROR>> Failed to set native media type on current source reader (hr=0x%x)", hr);
				break;
			}

			SAFERELEASE(pMT);

			hr = pReaderCurr->SetCurrentPosition(GUID_NULL, propvarStartPos);
			if(FAILED(hr))
			{
				LogTC(pLogger, HIGH, L"ERROR>> Failed to seek current source reader to beginning (hr=0x%x)", hr);
				break;
			}

			while(true)
			{
				SAFERELEASE(pRefSample);
				SAFERELEASE(pCurrSample);
				SAFERELEASE(pRefBuffer);
				SAFERELEASE(pCurrBuffer);

				hr = pReaderRef->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &dwSRStreamIndex, &dwSRRefFlags, &llTimeStamp,
					&pRefSample);
				if(FAILED(hr))
				{
					LogTC(pLogger, HIGH, L"ERROR>> Failed to get sample from reference source reader (hr=0x%x)", hr);
					break;
				}

				hr = pReaderCurr->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &dwSRStreamIndex, &dwSRCurrFlags, &llTimeStamp,
					&pCurrSample);
				if(FAILED(hr))
				{
					LogTC(pLogger, HIGH, L"ERROR>> Failed to get sample from reference source reader (hr=0x%x)", hr);
					break;
				}

				if(((dwSRRefFlags & MF_SOURCE_READERF_ERROR) != 0) || ((dwSRCurrFlags & MF_SOURCE_READERF_ERROR) != 0))
				{
					hr = E_FAIL;
					LogTC(pLogger, HIGH, L"ERROR>> An error was reported by source reader while reading the files");
					break;
				}

				do
				{
					if(pRefSample != NULL)
					{
						if(pCurrSample == NULL)
						{
							hr = E_FAIL;
							LogTC(pLogger, HIGH, L"ERROR>> Reference source reader returned a sample but current source reader did not");
							break;
						}
					}
					else
					{
						if(pCurrSample != NULL)
						{
							hr = E_FAIL;
							LogTC(pLogger, HIGH, L"ERROR>> Current source reader returned a sample but reference source reader did not");
							break;
						}

						// Both samples are null
						break;
					}

					hrRefBuff = pRefSample->ConvertToContiguousBuffer(&pRefBuffer);
					hrCurrBuff = pCurrSample->ConvertToContiguousBuffer(&pCurrBuffer);

					if(FAILED(hrRefBuff) || FAILED(hrCurrBuff))
					{
						if((hrRefBuff == E_UNEXPECTED) && (hrCurrBuff == E_UNEXPECTED))
						{
							// Both buffers were empty
							hr = S_OK;
							break;
						}

						hr = FAILED(hrRefBuff) ? hrRefBuff : hrCurrBuff;
						LogTC(pLogger, HIGH, L"ERROR>> The failed to convert sample to contiguous buffer (hr=0x%x)", hr);
						break;
					}

					ullFramesVerified += 1;

					hr = pRefBuffer->Lock(&pbRefBuff, NULL, &dwRefBuffLen);
					if(FAILED(hr))
					{
						LogTC(pLogger, HIGH, L"ERROR>> Failed to lock reference buffer (hr=0x%x)", hr);
						break;
					}

					hr = pCurrBuffer->Lock(&pbCurrBuff, NULL, &dwCurrBuffLen);
					if(FAILED(hr))
					{
						LogTC(pLogger, HIGH, L"ERROR>> Failed to lock current buffer (hr=0x%x)", hr);
						break;
					}

					if(dwRefBuffLen != dwCurrBuffLen)
					{
						hr = E_FAIL;
						LogTC(pLogger, HIGH, L"ERROR>> The output file sizes don't match");
						break;
					}

					if(memcmp(pbRefBuff, pbCurrBuff, dwRefBuffLen) != 0)
					{
						hr = E_FAIL;
						LogTC(pLogger, HIGH, L"ERROR>> The output files don't match");
						break;
					}
				}while(false);

				SAFEUNLOCK(pRefBuffer);
				SAFEUNLOCK(pCurrBuffer);

				if(FAILED(hr))
				{
					break;
				}

				if(((dwSRRefFlags & MF_SOURCE_READERF_ENDOFSTREAM) != 0))
				{
					// The reference source reader hit an EOS, check to see if the current did not

					if(((dwSRCurrFlags & MF_SOURCE_READERF_ENDOFSTREAM) == 0))
					{
						hr = E_FAIL;
						LogTC(pLogger, HIGH, L"ERROR>> The reference source reader hit an EOS before the current source reader");
						break;
					}

					// Both source readers hit an EOS, we're done

					break;
				}
				else
				{
					// The reference source reader did not hit an EOS, check to see if the current did

					if(((dwSRCurrFlags & MF_SOURCE_READERF_ENDOFSTREAM) != 0))
					{
						hr = E_FAIL;
						LogTC(pLogger, HIGH, L"ERROR>> The current source reader hit an EOS before the reference source reader");
						break;
					}
				}
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

		if(ullFramesVerified == 0)
		{
			LogTC(pLogger, HIGH, L"ERROR>> Output files were empty");
			hr = E_FAIL;
			break;
		}
	}while(false);

	SAFERELEASE(pReaderRef);
	SAFERELEASE(pReaderCurr);
	SAFERELEASE(pMT);
	SAFERELEASE(pRefSample);
	SAFERELEASE(pCurrSample);
	SAFERELEASE(pRefBuffer);
	SAFERELEASE(pCurrBuffer);

    do
    {
        DWORD dwDeleteFiles = 0;
		BOOL bWow64         = FALSE;

        if(CCMFTDiagGlobalParams::g_pParams->bAlwaysCleanOutput != FALSE)
        {
            dwDeleteFiles   = 1;
        }

        if(pTestContext == NULL)
        {
            break;
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
			csID.Format(OUTPUTINSTANCEFORMATSTRING, i);

			cpFileName = pTestContext->cpOutFileName;
			cpFileName.RenameExtension(csID + pTestContext->cpOutFileName.GetExtension());

			DeleteFile((const WCHAR*)cpFileName);
		}
    }while(false);

	return hr;
}

HRESULT AnalyzeOutputFile(ILogger* pLogger, const WCHAR* pszFileName, DWORD dwOutputType)
{
	HRESULT             hr          = S_OK;
	CBaseESAnalyzer*    pESAnalyzer = NULL;

	do
	{
		if((pLogger == NULL) || (pszFileName == NULL))
		{
			hr = E_POINTER;
			break;
		}

		switch(dwOutputType)
		{
		case TRANSCODE_OUTPUT_VC1:
			LogTC(pLogger, NORMAL, L"Test INFO>> Analyzing VC-1 output (%s)", pszFileName);
			pESAnalyzer = new CVC1ESAnalyzer();
			break;
		case TRANSCODE_OUTPUT_WMV3:
			LogTC(pLogger, NORMAL, L"Test INFO>> Analyzing WMV9 output (%s)", pszFileName);
			pESAnalyzer = new CWMV9ESAnalyzer();
			break;
		case TRANSCODE_OUTPUT_H264:
			LogTC(pLogger, NORMAL, L"Test INFO>> Analyzing H264 output (%s)", pszFileName);
			pESAnalyzer = new CH264ESAnalyzer();
			break;
		case TRANSCODE_OUTPUT_MPEG4PT2:
			LogTC(pLogger, NORMAL, L"Test INFO>> Analyzing MPEG4pt2 output (%s)", pszFileName);
			pESAnalyzer = new CM4S2ESAnalyzer();
			break;
		case TRANSCODE_OUTPUT_MPEG2:
			LogTC(pLogger, NORMAL, L"Test INFO>> Analyzing MPEG2 output (%s)", pszFileName);
			pESAnalyzer = new CVC1ESAnalyzer();
			break;
		case TRANSCODE_OUTPUT_UNKNOWN:
		default:
			LogTC(pLogger, HIGH, L"Test INFO>> Output type is unknown, cannot verify stream");
			break;
		}

		if((dwOutputType != TRANSCODE_OUTPUT_UNKNOWN) && (pESAnalyzer == NULL))
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> Failed to create Stream Analyzer");
			hr = E_OUTOFMEMORY;
			break;
		}

		if(pESAnalyzer == NULL)
		{
			hr = E_OUTOFMEMORY;
			break;
		}

		hr = pESAnalyzer->SetInputFile(pszFileName);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> Failed to set input file on Stream Analyzer, 0x%x", hr);
			break;
		}

		pESAnalyzer->SetAnalysisMode(IGNORE_ALL);

		g_pLogger = pLogger;

		pESAnalyzer->SetLoggingFunction(&MFTDLog);
		
		hr = pESAnalyzer->ValidateInputParams();
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> Stream Analyzer failed to validate input parameters, 0x%x", hr);
			break;
		}

		hr = pESAnalyzer->OpenIOFiles();
		if(FAILED(hr))
		{			    
			LogTC(pLogger, HIGH, L"Test ERROR>> Stream Analyzer failed to open files, 0x%x", hr);
			break;
		}      

		hr = pESAnalyzer->ParseBitstream();
		if(FAILED(hr) || (pESAnalyzer->ErrorsFound() == TRUE))
		{
			hr = E_FAIL;
			LogTC(pLogger, HIGH, L"Test ERROR>> Stream Analyzer found issues with bitstream");
			break;
		}

		LogTC(pLogger, NORMAL, L"Test INFO>> Bitstream validated");
	}while(false);

	SAFEDELETES(pESAnalyzer);

	return hr;
}

HRESULT RunTranscode(DWORD dwInstanceID, ILogger* pLogger, CMIContext* pContext)
{
	HRESULT                     hr                      = S_OK;
	CTestMediaSessionTranscode* pTranscodeSession       = NULL;
	DWORD                       dwTranscodeStart        = 0;
	DWORD                       dwTranscodeTime         = 0;
	UINT64                      ullDuration             = 0;
	UINT64                      un64HWFileLen           = 0;
	UINT32                      uiBitrate               = 0;
	BOOL                        bCloseTranscodeSession  = FALSE;
	BOOL                        bTargetMFTFound         = FALSE;
	CTranscodeTestContext*      pTestContext            = NULL;
	CPath                       cpOutFileName;
	DWORD                       dwRenderingMode         = MFTDRENDERING_FILE;
	IMFAttributes*              pVideoAttrib            = NULL;

	PERF_LOAD(PERF_LOAD_DEFAULT);

	do
	{
		pTestContext = (CTranscodeTestContext*)pContext;
		if((pLogger == NULL) || (pTestContext == NULL))
		{
			hr = E_POINTER;
			break;
		}

		if(pTestContext->dwSize != sizeof(CTranscodeTestContext))
		{
			hr = E_INVALIDARG;
			break;
		}

		hr = pTestContext->VerifyPointers();
		if(FAILED(hr))
		{
			break;
		}

		CString csID;

		csID.Format(OUTPUTINSTANCEFORMATSTRING, dwInstanceID);

		cpOutFileName = pTestContext->cpOutFileName;
		cpOutFileName.RenameExtension(csID + pTestContext->cpOutFileName.GetExtension());

		if(pTestContext->bPerfTest == TRUE)
		{
			dwRenderingMode = MFTDRENDERING_NULL;
		}

		PERF_START(PERF_LOAD_DEFAULT, L"Hardware :" + pTestContext->csTestCaseName);
		if(pTestContext->pTranscodeProfile == NULL)
		{
			hr = E_POINTER;
			break;
		}
		hr = pTestContext->pTranscodeProfile->GetVideoAttributes(&pVideoAttrib);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> Failed to GetVideoAttributes for the Transcode Profile: 0x%08X", hr);
			break;
		}
		hr = pVideoAttrib->GetUINT32(MF_MT_AVG_BITRATE, &(uiBitrate));
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> Failed to get MF_MT_AVG_BITRATE from the Transcode Profile video attributes: 0x%08X", hr);
			break;
		}
		hr = CTestMediaSessionTranscode::CreateInstance(pLogger, pTestContext->pTranscodeProfile, cpOutFileName, dwRenderingMode, &pTranscodeSession);
		if(FAILED(hr) || NULL == pTranscodeSession)
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> Failed to create CTestMediaSessionTranscode: 0x%08X", hr);
			break;
		}

		LogTC(pLogger, NORMAL, L"Successfully initialized transcoding session");

		bCloseTranscodeSession = TRUE;

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

				hr = pTranscodeSession->SetUseSmartSource(pTestContext->bUseSmartSource, ppGUIDs, pTestContext->dwNumMediaTypes);
				if(FAILED(hr))
				{
					LogTC(pLogger, HIGH, L"ERROR>> Failed to set transcode session to use smart source: 0x%08X", hr);
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
			hr = pTranscodeSession->SetRunTimeInSeconds(pTestContext->dwRunTimeInSec);
			if(FAILED(hr))
			{
				LogTC(pLogger, HIGH, L"ERROR>> Failed to set transcode run times: 0x%08X", hr);
				break;
			}
		}

		hr = pTranscodeSession->SetBuildMode(pTestContext->bAllowHW, pTestContext->bAllowHW);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"ERROR>> Failed to set build mode on transcode topology: 0x%X", hr);
			break;
		}

		hr = pTranscodeSession->BuildTopology(pTestContext->csInFileName);
		if(FAILED(hr))
		{
			pTranscodeSession->SilenceOutput(); // We're already failing, no need to see more error messages

			LogTC(pLogger, HIGH, L"ERROR>> Cannot build a topology: 0x%08X", hr);

			if((pTestContext->hrAcceptable != S_OK) && (hr == pTestContext->hrAcceptable))
			{
				LogTC(pLogger, HIGH, L"INFO>> This error is acceptable for this test");
				LogTC(pLogger, HIGH, L"INFO>> Reason: %s", (const WCHAR*)(pTestContext->csAcceptableFailureReason));
				hr = S_FALSE;
				break;
			}

			HRESULT hrCallback = pTranscodeSession->GetCallbackHRESULT();
			if((pTestContext->hrAcceptable != S_OK) && (hrCallback == pTestContext->hrAcceptable))
			{
				LogTC(pLogger, HIGH, L"INFO>> This error is acceptable for this test");
				LogTC(pLogger, HIGH, L"INFO>> Reason: %s", (const WCHAR*)(pTestContext->csAcceptableFailureReason));
				hr = S_FALSE;
			}

			break;
		}

		hr = pTranscodeSession->WaitForState(STATE_TOPOLOGY_READY, pTestContext->dwTimeoutSec);
		if(FAILED(hr))
		{
			pTranscodeSession->SilenceOutput(); // We're already failing, no need to see more error messages

			LogTC(pLogger, HIGH, L"ERROR>> Cannot build a topology: 0x%08X", hr);

			if((pTestContext->hrAcceptable != S_OK) && (hr == pTestContext->hrAcceptable))
			{
				LogTC(pLogger, HIGH, L"INFO>> This error is acceptable for this test");
				LogTC(pLogger, HIGH, L"INFO>> Reason: %s", (const WCHAR*)(pTestContext->csAcceptableFailureReason));
				hr = S_FALSE;
				break;
			}

			HRESULT hrCallback = pTranscodeSession->GetCallbackHRESULT();
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
			if(SUCCEEDED(pTranscodeSession->GetDuration(&ullDuration)))
			{
				pTestContext->dwTimeoutSec = (DWORD)(MediaTimeToMiliSeconds(ullDuration) * GetTimeoutMultiplier(pLogger));
				if(pTestContext->dwTimeoutSec == 0)
				{
					pTestContext->dwTimeoutSec = INFINITE;
				}
			}
		}

		LogTC(pLogger, NORMAL, L"INFO>> Topology in use:");

		if(dwInstanceID == 0)
		{
			if(pTranscodeSession->WalkFullTopology(2, pTestContext->pActivate, pTestContext->pszHardwareURL) == S_OK)
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
			pTranscodeSession->WalkFullTopology(2);
			
			bTargetMFTFound = TRUE; // For all other instances, we don't need to verify the MFT is used
		}

		hr = pTranscodeSession->GetDuration(&ullDuration);
		if(FAILED(hr)){
			LogTC(pLogger, HIGH, L"ERROR>> failed to obtain source duration, 0x%08X", hr);
			break;
		}

		LogTC(pLogger, NORMAL, L"Starting transcoding...");

		hr = pTranscodeSession->Start();
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"ERROR>> failed to start transcoding topology, 0x%08X", hr);
			break;
		}

		dwTranscodeStart = timeGetTime();

		hr = pTranscodeSession->WaitForState(STATE_PLAYING, pTestContext->dwTimeoutSec);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"ERROR>> Topology Failed to transition to playing state: 0x%08X", hr);
			break;
		}

		LogTC(pLogger, NORMAL, L"Media session is in playing state, waiting for completion");

		hr = pTranscodeSession->WaitForState(STATE_STOPPED, pTestContext->dwTimeoutSec);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"ERROR>> failed to wait for ending of the media session (stopping), 0x%08X", hr);
			break;
		}

		hr = pTranscodeSession->Close();

		bCloseTranscodeSession = FALSE;

		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"ERROR>> failed to close transcoding session, 0x%08X", hr);
			break;
		}

		hr = pTranscodeSession->GetCallbackHRESULT();

		SAFERELEASE(pTranscodeSession);

		PERF_STOP(PERF_STOP_DEFAULT);

		dwTranscodeTime = timeGetTime() - dwTranscodeStart;
		LogTC(pLogger, HIGH, L"------------------------------------");
		LogTC(pLogger, HIGH, L"Transcoding took %ld ms", dwTranscodeTime);

		if(ullDuration > 0)
		{
			DWORD dwDurationMs = (DWORD)(ullDuration/10000);
			double fRealTimePerCent = 100.0 * ((double)dwTranscodeTime) / (double)(dwDurationMs);
			LogTC(pLogger, HIGH, L"This is %8.2f per cent of real time (%ld ms)", fRealTimePerCent, dwDurationMs);
			if(fRealTimePerCent > 0.0)
			{
				double fAcceleration = 1.0 / (fRealTimePerCent/100.0);
				LogTC(pLogger, HIGH, L"Acceleration is %8.2f", fAcceleration);
			}
		}
		LogTC(pLogger, HIGH, L"------------------------------------");

		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"ERROR>> Media Session recieved a failed event, 0x%x", hr);
			break;
		}

		if(pTestContext->bPerfTest == FALSE)
		{
			hr = AnalyzeOutputFile(pLogger, (const WCHAR*)cpOutFileName, pTestContext->dwOutputType);
			if(FAILED(hr))
			{
				break;
			}
		}
		
		if(bTargetMFTFound == FALSE)
		{
			LogTC(pLogger, NORMAL, L"Info: Since Target MFT was not found, no software comparison will occur");
			break;
		}		
		
		if((pTestContext->dwRunTimeInSec > 0) || (pTestContext->dwNumInstances > 1))
		{
			// Since this was a stress test, no software comparison needs to be performed
			break;
		}
		if(pVideoAttrib)
		{			
			UINT32 uiRateControlMode = 0;
			hr = pVideoAttrib->GetUINT32(MFTDIAG_ATTRIBUTE_CODECAPI_BITRATECONTROLMODE, &uiRateControlMode);
			//Don't validate the file sizes when CBR is not used or if rate control mode is not used
			if(FAILED(hr) || uiRateControlMode != 0)
			{
				if(hr == MF_E_ATTRIBUTENOTFOUND)
				{
					hr = S_OK;
				}
				break;
			}
		}
		hr = GetFileSize((const WCHAR*)cpOutFileName, &un64HWFileLen);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"ERROR>> Failed to get file length of output file %s", (const WCHAR*)cpOutFileName);
			break;
		}
		else
		{
			hr = S_OK; // GetFileSize can return S_FALSE
		}

		double lfExpectedFileLen = 0.0;
		if(uiBitrate != 0 && ullDuration != 0)
		{
			lfExpectedFileLen = ((double)MediaTimeToMiliSeconds(ullDuration) / 1000) * ((double)uiBitrate/8);
		}
		else
		{
			LogTC(pLogger, HIGH, L"ERROR>> Cannot calculate expected file length with bitrate=%u and duration=%lu", uiBitrate, ullDuration);
			hr = E_FAIL;
			break;
		}
		// Now verify that the file size is within specified limits
		LogTC(pLogger, LOW, L"INFO>> File from MFT under test pipeline transcode file size (%I64u vs %lf bytes)",
				un64HWFileLen, lfExpectedFileLen);
		double lfHWFileSizeRatio = (double)un64HWFileLen / lfExpectedFileLen;
		double lfFileSizeRatioLowerLimit = DEFAULT_FILESIZE_RATIO_LOWER_LIMIT;
		double lfFileSizeRatioUpperLimit = DEFAULT_FILESIZE_RATIO_UPPER_LIMIT;

		DWORD dwOverrideValue = 0;

		BOOL bWow64 = FALSE;
		IsWow64Process(GetCurrentProcess(), &bWow64);

		if(SUCCEEDED(GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
						TRANSCODEFILESIZELOWERLIMITNAME, &dwOverrideValue)))
		{
			lfFileSizeRatioLowerLimit = ((double)dwOverrideValue / TRANSCODEFILESIZELOWERLIMITRATIO);
		}

		dwOverrideValue = 0;

		if(SUCCEEDED(GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
						TRANSCODEFILESIZEUPPERLIMITNAME, &dwOverrideValue)))
		{
			lfFileSizeRatioUpperLimit = ((double)dwOverrideValue / TRANSCODEFILESIZEUPPERLIMITRATIO);
		}

		if((lfHWFileSizeRatio < lfFileSizeRatioLowerLimit) || (lfHWFileSizeRatio > lfFileSizeRatioUpperLimit))
		{
			hr = E_FAIL;
			LogTC(pLogger, HIGH, L"ERROR>> File from MFT under test pipeline is beyond the threshold for transcode file size (%I64u vs %lf bytes)",
				un64HWFileLen, lfExpectedFileLen);
			LogTC(pLogger, HIGH, L"        The lower limit is %.4lf%% and the upper limit is %.4lf%%", lfFileSizeRatioLowerLimit, lfFileSizeRatioUpperLimit);
			break;
		}

////////////////////////////////////////////////////////////////////////////////////////////////////
	}while(false);

	if((TRUE == bCloseTranscodeSession) && (NULL != pTranscodeSession))
	{
		HRESULT hrTemp = pTranscodeSession->Close();
		if(FAILED(hrTemp))
		{
			LogTC(pLogger, HIGH, L"ERROR>> failed to close transcoding session, 0x%08X", hrTemp);
		}
	}

	HRESULT hrPerf = S_OK;
	if(SUCCEEDED(hr) && (bTargetMFTFound == TRUE))
	{
			hrPerf = PERF_POSTPROCESS(PERF_POSTPROCESS_DEFAULT);

			HandlePerfLogs(pLogger, hrPerf, 2);
	}
	if(FAILED(hr))
	{
		PERF_STOP(PERF_STOP_DEFAULT);
	}
	if(FAILED(hrPerf))
	{
		if((pTestContext != NULL) && (pLogger != NULL))
		{
			LogTC(pLogger, HIGH, L"This transcode session did not meet the performance requirements");
		}
		if(SUCCEEDED(hr))
		{
			hr = E_FAIL;
		}
	}

	SAFERELEASE(pTranscodeSession);
	SAFERELEASE(pVideoAttrib);

	do
	{
		if(pTestContext == NULL)
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

		DeleteFile((const WCHAR*)cpOutFileName);
	}while(false);

	if((hr != S_SKIPPED) && SUCCEEDED(hr) && (bTargetMFTFound == FALSE))
	{
		hr = S_FALSE;
	}

	return hr;
}

HRESULT VerifyTranscodeE2E(ILogger* pLogger, IConfig* pConfig, IMFActivate* pMFTActivate) 
{
	HRESULT                     hr                  = S_OK;
	IMFAttributes*              pMFTAttributes      = NULL;
	CTranscodeProfileReader*    pProfileReader      = NULL;
	CTranscodeTestContext       ttcTestContext(pConfig, pMFTActivate);
    //CScreenIdleControl          ScreenIdle;

	do
	{
		if((pLogger == NULL) || (pConfig == NULL) || (pMFTActivate == NULL))
		{
			hr = E_POINTER;
			break;
		}

        /*hr = ScreenIdle.PreventScreenFromPoweringOff();
        if(FAILED(hr))
        {
            pLogger->Log(NORMAL, L"ERROR>> Failed to set prevent screen from idling off (hr=0x%x)", hr);
            break;
        }*/

		hr = pConfig->GetString(L"Description", ttcTestContext.csTestCaseName);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> Failed to find Test Case Description :0x%x", hr);
		}

		LogTC(pLogger, HIGH, L"Starting Test: %s", ttcTestContext.csTestCaseName);

		hr = MFStartup(MF_VERSION);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"System ERROR>> failed in MFStartup(): 0x%08X", hr);
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

		BOOL bContinueTest = FALSE;
		IMFAttributes* pTestAttributes = NULL;

		do
		{
			UINT32 un32MediaTypeSize = 0;

			if(IsEqualGUID(guidCategory, MFT_CATEGORY_VIDEO_DECODER))
			{
				hr = pMFTAttributes->GetBlobSize(MFT_INPUT_TYPES_Attributes, &un32MediaTypeSize);
				if(FAILED(hr))
				{
					LogTC(pLogger, HIGH, L"MF ERROR>> MFT_INPUT_TYPES_Attributes not found: 0x%x", hr);
					break;
				}
			}
			else if(IsEqualGUID(guidCategory, MFT_CATEGORY_VIDEO_ENCODER))
			{
				hr = pMFTAttributes->GetBlobSize(MFT_OUTPUT_TYPES_Attributes, &un32MediaTypeSize);
				if(FAILED(hr))
				{
					LogTC(pLogger, HIGH, L"MF ERROR>> MFT_OUTPUT_TYPES_Attributes not found: 0x%x", hr);
					break;
				}
			}
			else
			{
				// Video Processors should be used for all formats
				break;
			}

			ttcTestContext.dwNumMediaTypes = un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO);

			ttcTestContext.pMediaTypes = new MFT_REGISTER_TYPE_INFO[ttcTestContext.dwNumMediaTypes];
			if(ttcTestContext.pMediaTypes == NULL)
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

			if(IsEqualGUID(guidCategory, MFT_CATEGORY_VIDEO_DECODER))
			{
				hr = pMFTAttributes->GetBlob(MFT_INPUT_TYPES_Attributes, (BYTE*)ttcTestContext.pMediaTypes, un32MediaTypeSize, NULL);
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
			}
			else if(IsEqualGUID(guidCategory, MFT_CATEGORY_VIDEO_ENCODER))
			{
				hr = pMFTAttributes->GetBlob(MFT_OUTPUT_TYPES_Attributes, (BYTE*)ttcTestContext.pMediaTypes, un32MediaTypeSize, NULL);
				if(FAILED(hr))
				{
					LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_OUTPUT_TYPES_Attributes: 0x%x", hr);
					break;
				}

				if(FAILED(pConfig->GetAttributesFromNode(L"OutputType", pTestAttributes)))
				{
					break;
				}
			}

			hr = VerifyTestSupported(ttcTestContext.pMediaTypes, un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO), pTestAttributes);
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
					ttcTestContext.pszHardwareURL = new WCHAR[++un32HardwareURLLen];
					if(ttcTestContext.pszHardwareURL == NULL)
					{
						LogTC(pLogger, HIGH, L"ERROR>> Out of memory, can't allocate HardwareURL string");
						hr = E_OUTOFMEMORY;
						break;
					}

					memset(ttcTestContext.pszHardwareURL, 0, sizeof(WCHAR) * un32HardwareURLLen);

					hr = pTransformAttrib->GetString(MFT_ENUM_HARDWARE_URL_Attribute, ttcTestContext.pszHardwareURL, un32HardwareURLLen, NULL);
					if(FAILED(hr))
					{
						LogTC(pLogger, HIGH, L"MF ERROR>> Failed to get MFT_ENUM_HARDWARE_URL_Attribute: 0x%x", hr);
						break;
					}

					ttcTestContext.bAllowHW = TRUE;

					LogTC(pLogger, HIGH, L"Starting Test for Hardware Component");
				}
				else
				{
					LogTC(pLogger, NORMAL, L"INFO>> Software component under test (no MFT_ENUM_HARDWARE_URL_Attribute found), turning off HW MFTs");

					ttcTestContext.bAllowHW = FALSE;
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

			ttcTestContext.bAllowHW = FALSE;
		}

		RegHWSWOverload(pLogger);

		hr = CTranscodeProfileReader::CreateTranscodeProfileReader(pConfig, &pProfileReader);
		if(FAILED(hr) || !pProfileReader)
		{            
			hr = (FAILED(hr) ? hr : E_FAIL);
			LogTC(pLogger, HIGH, L"Test ERROR>> Failed to create CTranscodeProfileReader :0x%x", hr);
			break;
		}

		hr = pProfileReader->GetTranscodeProfile(&(ttcTestContext.pTranscodeProfile));
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> Failed to create CTranscodeProfileReader :0x%x", hr);
			break;
		}

		hr = DetermineOutputType(ttcTestContext.pTranscodeProfile, &(ttcTestContext.dwOutputType));
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> Unable to determine transcode output type: 0x%x", hr);
			break;
		}

		hr = pConfig->GetString(L"InputFile", ttcTestContext.csInFileName);
		if(FAILED(hr))
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> Failed to find InputFile :0x%x", hr);
			break;
		}

		pConfig->GetUINT32(L"AcceptableHR", (UINT32*)&(ttcTestContext.hrAcceptable));
		pConfig->GetString(L"AcceptableReason", ttcTestContext.csAcceptableFailureReason);
		
#ifdef _ARM_
		if(ttcTestContext.hrAcceptable == S_OK)
		{
			pConfig->GetUINT32(L"AcceptableHRWoA", (UINT32*)&(ttcTestContext.hrAcceptable));
			pConfig->GetString(L"AcceptableReasonWoA", ttcTestContext.csAcceptableFailureReason);
		}
#endif

		DWORD dwCurrentDirectoryLen = GetCurrentDirectory(0, NULL);

		WCHAR* pszCurrentDirectory = new WCHAR[dwCurrentDirectoryLen];
		if(pszCurrentDirectory == NULL)
		{
			hr = E_OUTOFMEMORY;
			break;
		}

		GetCurrentDirectory(dwCurrentDirectoryLen, pszCurrentDirectory);

		ttcTestContext.cpOutFileName.Combine(pszCurrentDirectory, OUT_DIR_NAME);

		SAFEDELETE(pszCurrentDirectory)

		DWORD dwAttr = GetFileAttributes(ttcTestContext.cpOutFileName);
		if (INVALID_FILE_ATTRIBUTES == dwAttr || !(dwAttr & FILE_ATTRIBUTE_DIRECTORY)) 
		{
			if (NULL == CreateDirectory(ttcTestContext.cpOutFileName , NULL)) 
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				LogTC(pLogger, HIGH, L"Test ERROR>> Unable to create directory for output file: 0x%x", hr);
				break;
			}
		}

		CString strTemp;

		if(FAILED(pConfig->GetAttribute(L"testId", strTemp)))
		{
			strTemp = "{ID_Unknown}";
		}

		strTemp = L"TranscodeTest_" + strTemp;
					
		ttcTestContext.cpOutFileName.Combine(ttcTestContext.cpOutFileName, strTemp);

		CONTAINER_TYPE containerType = CONTAINER_TYPE_INVALID;

		hr = SetOutputFileExt(ttcTestContext.pTranscodeProfile, &containerType);
		if(S_OK != hr)
		{
			LogTC(pLogger, HIGH, L"Test ERROR>> File Extenstion failed: 0x%x", hr);
			break;
		}

		if (CONTAINER_TYPE_ASF == containerType) 
		{
			ttcTestContext.cpOutFileName.RenameExtension(EXT_ASF);
		}
		else if (CONTAINER_TYPE_MPEG4 == containerType) 
		{
			ttcTestContext.cpOutFileName.RenameExtension(EXT_MP4);
		}
		else if (CONTAINER_TYPE_MP3 == containerType) 
		{
			ttcTestContext.cpOutFileName.RenameExtension(EXT_MP3);
		}
		else if (CONTAINER_TYPE_3GP == containerType) 
		{
			ttcTestContext.cpOutFileName.RenameExtension(EXT_3GP);
		}
		else 
		{
			hr = E_FAIL;
			LogTC(pLogger, HIGH, L"Test ERROR>> Invalid container type: 0x%x", hr);
			break;
		}

		PrintTranscodeSettings(pLogger, ttcTestContext.pTranscodeProfile);

		pConfig->GetUINT32(L"TimeoutSec", (UINT*)&(ttcTestContext.dwTimeoutSec));
		if(ttcTestContext.dwTimeoutSec == 0)
		{
			ttcTestContext.dwTimeoutSec = INFINITE;
		}

		UINT32 un32Val = 1;

		pConfig->GetUINT32(L"NumInstances", &un32Val);

		BOOL bWow64 = FALSE;

		IsWow64Process(GetCurrentProcess(), &bWow64);

		GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
			NUMINSTANCESNAME, (DWORD*)&un32Val);

		ttcTestContext.dwNumInstances = (DWORD)un32Val;

		un32Val = 0;

		pConfig->GetUINT32(L"FormatChange", &un32Val);

		GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
			SMARTSOURCENAME, (DWORD*)&un32Val);

		ttcTestContext.bUseSmartSource = (un32Val == 0) ? FALSE : TRUE;

		un32Val = 0;

		pConfig->GetUINT32(L"RunTimeInSeconds", &un32Val);

		GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
			RUNTIMENAME, (DWORD*)&un32Val);

		ttcTestContext.dwRunTimeInSec = (DWORD)un32Val;

		if(ttcTestContext.dwRunTimeInSec > 0)
		{
			ttcTestContext.bUseSmartSource = TRUE;
		}

		un32Val = 0;

		pConfig->GetUINT32(L"PerfTest", &un32Val);

		GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
			PERFTESTNAME, (DWORD*)&un32Val);

		ttcTestContext.bPerfTest = (un32Val == 0) ? FALSE : TRUE;

		if((ttcTestContext.bPerfTest == TRUE) && ttcTestContext.pszHardwareURL == NULL)
		{
			LogTC(pLogger, NORMAL, L"INFO>> Performance tests do not apply to non-hardware components");
			hr = S_SKIPPED;
			break;
		}

        if(ttcTestContext.dwNumInstances > 1)
        {
            // For mulitple instance tests, pull down the input source file before the mulitple instances run
            // so they don't step on eachother's toes trying to copy the file
            WCHAR* pwszActualContentFilePath    = NULL;

            hr = FindHCKContentFile(
                ttcTestContext.csInFileName,
                &pwszActualContentFilePath
                );
            if(FAILED(hr))
            {
                LogTC(pLogger, HIGH, L"ERROR>> Failed to find input file %s", (const WCHAR*)ttcTestContext.csInFileName);

                break;
            }

            ttcTestContext.csInFileName = pwszActualContentFilePath;

            SAFEFREE(pwszActualContentFilePath);
        }

		hr = RunMITests(ttcTestContext.dwNumInstances, pLogger, &ttcTestContext, &RunTranscode);
		if(hr != S_OK)
		{
			break;
		}

		if(ttcTestContext.dwNumInstances == 1)
		{
			break;
		}

		LogTC(pLogger, NORMAL, L"INFO>> Multiple instace test, verifying output files match");

		DWORD dwVerifyMIOutput = 1;

		GetRegistryValue(pLogger, HKEY_LOCAL_MACHINE, (bWow64 == FALSE) ? MFTDOPTIONSKEY : MFTDOPTIONSKEYWOW64, 
			VERIFYMIOUTPUTNAME, (DWORD*)&dwVerifyMIOutput);

		if(dwVerifyMIOutput == 0)
		{
			break;
		}

		hr = VerifyAllOutputFilesMatch(ttcTestContext.dwNumInstances, pLogger, &ttcTestContext);
		if(FAILED(hr))
		{
			break;
		}

		LogTC(pLogger, NORMAL, L"SUCCESS>> All output files match, multiple instance verified");
	}while(false);

	SAFERELEASE(pMFTAttributes);
	SAFEDELETES(pProfileReader);

	MFShutdown();

	return hr;
}

HRESULT InitializeTranscodeTestTable(CXmlRootConfig** ppXMLRootConfig)
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
			(TCHAR*)&InitializeTranscodeTestTable, &hMod) == FALSE)
		{
			hr = E_FAIL;
			break;
		}

		HRSRC hRsrc = FindResource(hMod, MAKEINTRESOURCE(IDR_TRANSCODECONFIG), L"Config");
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
		SAFERELEASE(pConfig);
	}

	return hr;
}

HRESULT RunTranscodeTest(const DWORD dwTestID, ILogger* pLogger, CXmlRootConfig* pXMLConfig, IMFActivate* pActivate)
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

		hr = VerifyTranscodeE2E(pLogger, pTestConfig, pActivate);
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

	SAFERELEASE(pTestConfig);

	return hr;
}
