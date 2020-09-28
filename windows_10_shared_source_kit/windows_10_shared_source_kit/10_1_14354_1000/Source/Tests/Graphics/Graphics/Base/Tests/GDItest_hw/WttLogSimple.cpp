#include "WttLogSimple.h"


//For the sake of easing the use of TJRE resolver,
//the format is aligned to be consistent with basevid wtt logger
//CVidWttLog has been moved from Global, to member object within CSimpleWTTLOG.

CSimpleWTTLOG::CSimpleWTTLOG(const TCHAR* szLogFileName)
{
    m_szwString = NULL;
    m_bStatus = TRUE;
    
    //when the device is created, a string is used to set its options
    TCHAR szDeviceCreationString[256];
    memset(szDeviceCreationString, 0, sizeof(szDeviceCreationString));
    
    StringCchPrintf(szDeviceCreationString,
              255,
              TEXT("$LogFile:file=%s,writemode=overwrite"),
              szLogFileName);

    WCHAR* szDeviceString = MakeWideCharString(szDeviceCreationString);
    
    HRESULT hr = m_Log.Initialize();
    if(FAILED(hr))
    {
        OutputDebugString(TEXT("Failed to initialize the VidWttlog." ));
        m_bStatus = FALSE;
    }
    DeleteWideCharString();

    memset(m_sCurrentTestName, 0, sizeof(m_sCurrentTestName));
}

CSimpleWTTLOG::~CSimpleWTTLOG()
{
    m_Log.Close();    
}

void CSimpleWTTLOG::StartTest(const TCHAR* szTestName)
{
    m_bStatus = TRUE;
    //wtt logger is not const-correct
    WCHAR* szTemp = MakeWideCharString(szTestName);

    HRESULT  hr = m_Log.StartTest(szTemp);
    if(FAILED(hr))
    {
        OutputDebugString("Failed to log StartTest\n");
        m_bStatus = FALSE;
    }
    DeleteWideCharString();

    //remember the current test name
    StringCchPrintf(m_sCurrentTestName,
              255,
              szTestName);
}


void CSimpleWTTLOG::EndTest()
{
    m_bStatus = TRUE;
    //wttlogger is not const-correct
    WCHAR* szTemp = MakeWideCharString(m_sCurrentTestName);

    HRESULT  hr = m_Log.EndTest(szTemp);
    if(FAILED(hr))
    {
        OutputDebugString("Failed to log EndTest\n");
        m_bStatus = FALSE;
    }
    DeleteWideCharString();
}

void CSimpleWTTLOG::Skip()
{
    m_bStatus = TRUE;
    //wttlogger is not const-correct
    WCHAR* szTemp = MakeWideCharString(m_sCurrentTestName);
 
    HRESULT  hr = m_Log.SkipTest(szTemp);
    if(FAILED(hr))
    {
        OutputDebugString("Failed to log SkipTest\n");
        m_bStatus = FALSE;
    }
    DeleteWideCharString();
}
void CSimpleWTTLOG::Warn(const TCHAR* szDescription)
{
    m_bStatus = TRUE;
    WCHAR* szTemp = MakeWideCharString(szDescription);

    HRESULT  hr = m_Log.LogWarning(szTemp);
    if(FAILED(hr))
    {
        OutputDebugString("Failed to log Warning\n");
        m_bStatus = FALSE;
    }
    DeleteWideCharString();
}
void CSimpleWTTLOG::Message(const TCHAR* szDescription)
{
    m_bStatus = TRUE;
    WCHAR* szTemp = MakeWideCharString(szDescription);

    HRESULT  hr = m_Log.LogMsg(szTemp);
    if(FAILED(hr))
    {
        OutputDebugString("Failed to log Message\n");
        m_bStatus = FALSE;
    }
    DeleteWideCharString();
}
void CSimpleWTTLOG::Fail(const TCHAR* szDescription)
{
    m_bStatus = TRUE;
    WCHAR* szTemp = MakeWideCharString(szDescription);

    HRESULT  hr = m_Log.LogError(szTemp);
    if(FAILED(hr))
    {
        OutputDebugString("Failed to log failure errors\n");
        m_bStatus = FALSE;
    }
    DeleteWideCharString();
}

WCHAR*	CSimpleWTTLOG::MakeWideCharString(const TCHAR* szString)
{
#ifndef _UNICODE
    UINT uBufferSize = MultiByteToWideChar(CP_ACP, 0, szString, _tcslen(szString), NULL, 0);
    uBufferSize ++;
    
    DeleteWideCharString();

    if(0 == uBufferSize)
        uBufferSize = 1;

    m_szwString = new WCHAR[uBufferSize];
    if(!m_szwString)
        return NULL;

    if(0 == MultiByteToWideChar(CP_ACP, 0, szString, _tcslen(szString), m_szwString, uBufferSize))
        return NULL;
    m_szwString[uBufferSize-1] = L'\0';

    return m_szwString;
#else
    return (WCHAR*) szString;
#endif
}

void CSimpleWTTLOG::DeleteWideCharString()
{
#ifndef _UNICODE
    if(NULL != m_szwString)
    {
        delete m_szwString;
        m_szwString = NULL;
    }
#endif
}