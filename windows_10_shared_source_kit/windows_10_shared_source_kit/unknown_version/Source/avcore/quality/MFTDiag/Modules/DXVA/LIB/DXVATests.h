#pragma once

#include "MFTDiagTestDLLCommon.h"
#include "XMLConfig.h"
#include "CMIContext.h"
#include "mfstringify.h"

extern  BOOL                    g_bVerifyEncryption;
extern  BOOL                    g_bDisableEncryption;
extern  MFTOPOLOGY_DXVA_MODE    g_dxvamodeDefault;

HRESULT RunDXVATest(DWORD dwTestID, ILogger* pLogger,  CXmlRootConfig* pXMLConfig);
HRESULT InitializeDXVATestTable(CXmlRootConfig** ppXMLRootConfig);

class CDXVATestContext: public CMIContext
{
public:
    CString                                 csInFileName;
    CString                                 csTestCategory;
    CString                                 csComponentCLSID;
    DWORD                                   dwRunTimeInSec;
    CString                                 csInputMediaType;
    DWORD                                   dwRenderingMode;
    DWORD                                   dwTimeoutSec;
    BOOL                                    bUseSmartSource;
    BOOL                                    bAllowAudio;
    BOOL                                    bRunAllModes;
    BOOL                                    bVerifyEncryption;
    BOOL                                    bUseDComp;
    BOOL                                    bFullScreen;
    BOOL                                    bH264SendSampleDesc;
    BOOL                                    bH264SendFakeMT;
    MFTOPOLOGY_DXVA_MODE                    eDXVAMode;
    DWORD                                   eVideoFormat;
    DWORD                                   dwCSTestStartTime;
    double                                  lfVVThreshold;
    double                                  lfFailPassRatio;

    CDXVATestContext(IConfig* pConfig, IMFActivate* pActivate);
    virtual ~CDXVATestContext(void);
};