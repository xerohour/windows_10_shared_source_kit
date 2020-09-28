#ifndef _FUSHELPER_H_
#define _FUSHELPER_H_

//Required for fast user switching functionality
#include "Includefusframework_i.c"
#include "Includefusframework.h"
#define _WIN32_DCOM

class CFusHelper
{
    public:
        CFusHelper(CCTLLogger* logger);
        ~CFusHelper();
        HRESULT InitializeFusFramework();
        HRESULT AddUser(LPCWSTR pwszUserName, LPCWSTR pwszPassword);
        HRESULT SwitchToUser(LPCWSTR pwszUserName, LPCWSTR pwszPassword);
        HRESULT RemoveUser(LPCWSTR pwszUserName);
        HRESULT RestoreUser();
        HRESULT CreateProcessInSession(LPCWSTR pwszUserName, LPCWSTR pwszFullExecutablePath);
        
    private:
        //Pointer to main FUS framework object
        IFUSFrame* m_pFus;
        CCTLLogger* m_log;
};

#endif