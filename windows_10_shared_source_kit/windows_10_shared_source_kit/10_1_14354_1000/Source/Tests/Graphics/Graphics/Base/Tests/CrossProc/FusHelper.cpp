#include "stdafx.h"

#include "FusHelper.h"

CFusHelper::CFusHelper(CCTLLogger* logger) :
    m_log(logger)
{}

CFusHelper::~CFusHelper()
{
    //Make sure we uninitialize the COM objects used in FUS framework.
    CoUninitialize();
    m_pFus = NULL;
}

HRESULT CFusHelper::InitializeFusFramework()
{
    HRESULT hr = S_OK;
    VARIANT_BOOL bResult = VARIANT_TRUE;
    BOOL bRes = TRUE;

    CoInitialize(NULL);
    
    // Create COM object
    if(FAILED(hr = CoCreateInstance(CLSID_FUSFrame, NULL, CLSCTX_INPROC_SERVER, IID_IFUSFrame, (LPVOID*)&m_pFus)))
    {
        return hr;
    }
    
    // Initialize
    if(FAILED(hr = (m_pFus)->Initialize(&bResult)) || !bResult)
    {
        PATH_TRAP();
        DBG_MSG(L"FUS framework call to Initialize failed.\n");
        if(SUCCEEDED(hr))
        {
            PATH_TRAP();
            hr = E_FAIL;
        }
    }
    // make sure FUS is enabled
    if(FAILED(hr = (m_pFus)->get_IsFUSEnabled(&bRes)) || !bRes)
    {
        PATH_TRAP();
        LOG_AND_DBG_MSG(m_log,L"FUS framework call to get_IsFUSEnabled failed.\n");
        if(SUCCEEDED(hr))
        {
            PATH_TRAP();
            hr = E_FAIL;
        }
    }
    return hr;
}

HRESULT CFusHelper::AddUser(LPCWSTR pwszUserName, LPCWSTR pwszPassword)
{
    HRESULT hr = S_OK;
    ULONG nRet = 0;

    //if(FAILED(hr = m_pFus->AddAdminUser(_bstr_t(pwszUserName), _bstr_t(pwszPassword), &nRet)))
    if(FAILED(hr = m_pFus->AddUser(_bstr_t(pwszUserName), _bstr_t(pwszPassword), &nRet)))
    {
        PATH_TRAP();
        LOG_AND_DBG_MSG(m_log,L"Add user failed.\n");
    }
    else
    {
        //Wait for a short time to allow changes to be completed.
        Sleep(5000);
    }
 
    return hr;
}

HRESULT CFusHelper::SwitchToUser(LPCWSTR pwszUserName, LPCWSTR pwszPassword)
{
    HRESULT hr = S_OK;
    VARIANT_BOOL bRet = TRUE;

    if(FAILED(hr = m_pFus->SwitchToUser(_bstr_t(pwszUserName), _bstr_t(pwszPassword), &bRet)) || !bRet)
    {
        PATH_TRAP();
        LOG_AND_DBG_MSG(m_log,L"Call to SwitchToUser failed.\n");
        if(SUCCEEDED(hr))
        {
            hr = E_FAIL;
        }
    }

    //Wait for everything to get initialized before continuing.
    Sleep(60000);

    return hr;
}

HRESULT CFusHelper::RemoveUser(LPCWSTR pwszUserName)
{
    HRESULT hr = S_OK;
    VARIANT_BOOL bRet = VARIANT_TRUE;

    if(FAILED(hr = m_pFus->RemoveUser(_bstr_t(pwszUserName), &bRet)) || !bRet)
    {
        PATH_TRAP();
        LOG_AND_DBG_MSG(m_log,L"Call to RemoveUser failed.\n");
        if(SUCCEEDED(hr))
        {
            PATH_TRAP();
            hr = E_FAIL;
        }
    }

    Sleep(5000);
    return hr;
}

HRESULT CFusHelper::RestoreUser()
{
    HRESULT hr = S_OK;
    VARIANT_BOOL bRet = VARIANT_TRUE;

    if(FAILED(hr = m_pFus->RestoreUser(&bRet)))
    {
        PATH_TRAP();
        LOG_AND_DBG_MSG(m_log,L"Call to RestoreUser failed.\n");
    }

    Sleep(30000);
    return hr;
}

HRESULT CFusHelper::CreateProcessInSession(LPCWSTR pwszUserName, LPCWSTR pwszFullExecutablePath)
{
    HRESULT hr = S_OK;
    DWORD procId = 0;

    TCHAR pszDest[200];
    hr = StringCchPrintf(pszDest, 200, L"CreateProcessInSession :: UserName=%s, Executable=%s.\n", pwszUserName, pwszFullExecutablePath);
    if(SUCCEEDED(hr))
    {
        LOG_AND_DBG_MSG(m_log,pszDest);
    }

    if(FAILED(hr = m_pFus->Execute(_bstr_t(pwszUserName), 
                                   _bstr_t(pwszFullExecutablePath), 
                                   _bstr_t(" user"),
                                   NULL,
                                   &procId)) || procId == 0)
    {
        LOG_AND_DBG_MSG(m_log,L"CreateProcessInSession failed, check to see if exe failed or fus failed.\n");
        
        //Failed to run crossproc.exe,  the framework might have worked okay,
        //but the execution of the process may have failed.
        if(SUCCEEDED(hr))
        {
            LOG_AND_DBG_MSG(m_log,L"crossproc.exe from different session failed.\n");
            hr = E_FAIL;
        }
    }

    return hr;
}
