//Modified from WTTLogSample

#include "wttlogger.h"
#include <string>
#include "strsafe.h"
#include <TCHAR.h>
#include <CVidWttLog.h>

class CSimpleWTTLOG
{
public:
    CSimpleWTTLOG(const TCHAR* szLogFileName);

    ~CSimpleWTTLOG();

    void StartTest(const TCHAR* szTestName);
    void EndTest();
    void Warn(const TCHAR* szDescription);
    void Message(const TCHAR* szDescription);
    void Skip();
    void Fail(const TCHAR* szDescription);
    BOOL IsValid(){return m_bStatus;}
    
private:

    CVidWttLog m_Log;
    BOOL	   m_bStatus;
    TCHAR m_sCurrentTestName[256];
    WCHAR* MakeWideCharString(const TCHAR* szString);
    void DeleteWideCharString();
    WCHAR*			m_szwString;
};
