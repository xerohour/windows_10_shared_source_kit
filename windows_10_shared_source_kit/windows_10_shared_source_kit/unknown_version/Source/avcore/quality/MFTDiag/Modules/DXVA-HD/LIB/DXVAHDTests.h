#pragma once

#include "MFTDiagTestDLLCommon.h"
#include "XMLConfig.h"
#include "CMIContext.h"
#include "mfstringify.h"

HRESULT RunDXVAHDTest(DWORD dwTestID, ILogger* pLogger,  CXmlRootConfig* pXMLConfig);
HRESULT InitializeDXVAHDTestTable(CXmlRootConfig** ppXMLRootConfig);

class CDXVAHDTestContext: public CMIContext
{
public:
    CString                                 csInFileName;
    CString                                 csTestCategory;
    CString                                 csConfig;
    DWORD                                   dwRenderingMode;
    DWORD                                   dwTimeoutSec;
    double                                  lfVVThreshold;
    double                                  lfFailPassRatio;

    CDXVAHDTestContext(IConfig* pConfig, IMFActivate* pActivate);
    virtual ~CDXVAHDTestContext(void);
};