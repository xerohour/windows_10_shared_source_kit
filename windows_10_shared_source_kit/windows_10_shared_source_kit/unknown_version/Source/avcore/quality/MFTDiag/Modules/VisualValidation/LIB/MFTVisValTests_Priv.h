#ifndef __MFTVisValTests_Priv_H__
#define __MFTVisValTests_Priv_H__

/*****************************************************************
** This file is meant to be only included by MFTVisValTests.cpp
** This is to alleviate the growing "Forward Declarations"
** of all the functions only used in that file
** It does not contain the necessary inlcudes to be used by
** any other file, and these are just helper functions for that
** file
*****************************************************************/

enum BLUE_TEST_TYPE {
    BLUE_TEST_STREAMING,
    BLUE_TEST_RDCURVE,
    BLUE_TEST_ROI,
    BLUE_TEST_DYNSINKWRITER,
    BLUE_TEST_DYNAMICTYPE,
    BLUE_TEST_LTRRDCURVE
};


HRESULT VerifyVisValPlayback(ILogger* pLogger, IConfig* pConfig, IMFActivate* pMFTActivate);
HRESULT VerifyVisValTranscode(ILogger* pLogger, IConfig* pConfig, IMFActivate* pMFTActivate);
static HRESULT PerformVisualValidation(ILogger* pLogger, const WCHAR* pszTestFileName, const WCHAR* pszRefFileName,
                                const DWORD dwWidth, const DWORD dwHeight, const GUID* pGUIDSubType, 
                                double lfPSNRThreshold, double lfFailPassRatio);
HRESULT GetTestOutputFileName(ILogger* pLogger, const WCHAR* pwszOutputSubdirectoryName, const WCHAR* pwszOutputNamePrefix,
                              const WCHAR* pszTestID, const WCHAR* pszNameExtension, CPath& cpFileName);
HRESULT VerifyEncoderQuality(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate);
HRESULT VerifyEncoderSettings(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate);
HRESULT VerifyEncoderSettingsPerSample(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate);
HRESULT VerifyEncoderDynamicType(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate);
HRESULT VerifyEncoderLTRRDCurve(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate);
HRESULT VerifyEncoderWinBlue(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate, BLUE_TEST_TYPE type = BLUE_TEST_STREAMING);
HRESULT VerifyEncoderGradualIntraRefresh(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate);
HRESULT VerifyEncoderHighGPUUsage(ILogger* pLogger, IConfig* pConfig, IMFActivate* pActivate);
HRESULT ConvertTranscodeProfileToEncodingParameters(ILogger* pLogger,
                                                    IConfig* pConfig, EncodingParameters* pepEncodingParameters);
HRESULT PrintMFTUnderTestInfo(ILogger* pLogger,IMFActivate* pActivate,const GUID guidMFT);
const WCHAR* SubTypeToString(GUID* pSubType);
HRESULT DeleteDirectoryWithFiles(const WCHAR* pwcDirectory);
static BOOL DXCFrameCompCallback(void* pContext, DWORD dwCurrFrame, DWORD dwNumFailed, UINT64 un64CurrBytesProcessed, 
		UINT64 un64TotalBytes, double lfYPSNR, double lfUPSNR, double lfVPSNR, double lfYDSIM, double lfUDSIM, double lfVDSIM);
static BOOL DXCPSNRFrameCompCallback(void* pContext, DWORD dwCurrFrame, DWORD dwNumFailed, UINT64 un64CurrBytesProcessed, 
		UINT64 un64TotalBytes, double lfYPSNR, double lfUPSNR, double lfVPSNR, double lfYDSIM, double lfUDSIM, double lfVDSIM);

#endif
