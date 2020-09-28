#ifndef __MFTPlaybackTests_H__
#define __MFTPlaybackTests_H__

#include "MFTDiagTestDLLCommon.h"
#include "XMLConfig.h"
#include "CMIContext.h"

HRESULT RunPlaybackTest(const DWORD dwTestID, ILogger* pLogger, CXmlRootConfig* pXMLConfig, IMFActivate* pActivate);
HRESULT RunChildPlaybackTest(
    ILogger*        pLogger,
    const DWORD     dwInstance,
    const WCHAR*    pwszContextName,
    const DWORD     dwContextSize
    );
HRESULT InitializePlaybackTestTable(CXmlRootConfig** ppXMLRootConfig);

// This is the function that runs the actual test for using this as a library
HRESULT RunPlayback(DWORD dwInstanceID, ILogger* pLogger, CMIContext* pContext);

class CPlaybackTestContext: public CMIContext
{
public:
    HRESULT                 hrAcceptable;
    CString                 csInFileName;
    CString                 csTestCaseName;
    CString                 csAcceptableFailureReason;
    WCHAR*                  pszHardwareURL;
    DWORD                   dwTimeoutSec;
    DWORD                   dwTestID;
    DWORD                   dwNumInstances;
    DWORD                   dwRunTimeInSec;
    DWORD                   dwRenderingMode;
    BOOL                    bUseSmartSource;
    BOOL                    bPerfTest;
    BOOL                    bAllowAudio;
    BOOL                    bAllowHW;
    BOOL                    bRateless;
    BOOL                    bAllowTranscodeDec;
    BOOL                    bLowPowerTest;
    BOOL                    bRunIfMFTNotFound;
    UINT32                  dwNumMediaTypes;
    MFT_REGISTER_TYPE_INFO* pMediaTypes;
    UINT64                  un64WallTime;
    MFTOPOLOGY_DXVA_MODE    eDXVAMode;
    // If you modify this structure, update Serialize/DeSerialize to match!

    CPlaybackTestContext(IConfig* pConfig, IMFActivate* pActivate);
    virtual ~CPlaybackTestContext(void);

protected:
    virtual HRESULT SerializeInherited(
        CSerialBuffer*  pBuffer,
        DWORD*          pdwBufferSize
        );
    virtual HRESULT DeSerializeInherited(
        CSerialBuffer*  pBuffer
        );
};

#endif