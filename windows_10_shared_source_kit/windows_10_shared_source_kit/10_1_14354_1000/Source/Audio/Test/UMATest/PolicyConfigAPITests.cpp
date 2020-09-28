// Copyright(C) Microsoft. All rights reserved.

//
// PolicyConfigAPITests.cpp
//  
//
// Description:
//
//  IPolicyConfig API Tests

#include <stdafx.h>


#include <TestParams.h>
#include "PolicyConfigAPITests.h"


DWORD TC_PolicyConfig_GetDeviceFormat_Invalid()
{   
    HRESULT hr = S_OK;

    CAudioPolicyConfigApiTest objTest(hr);
    if(FAILED(hr))
    {
        goto END;
    }
        
    objTest.GetDeviceFormat_InvalidParamsTest();
    
END:
    return FNS_PASS;
}

DWORD TC_PolicyConfig_GetMixFormat_Invalid()
{   
    HRESULT hr = S_OK;

    CAudioPolicyConfigApiTest objTest(hr);
    if(FAILED(hr))
    {
        goto END;
    }
        
    objTest.GetMixFormat_InvalidParamsTest();
    
END:
    return FNS_PASS;
}

DWORD TC_PolicyConfig_SetDeviceFormat_Invalid()
{
    HRESULT hr = S_OK;

    CAudioPolicyConfigApiTest objTest(hr);
    if(FAILED(hr))
    {
        goto END;
    }
        
    objTest.SetDeviceFormat_InvalidParamsTest();

END:
    return FNS_PASS;
}

DWORD TC_PolicyConfig_GetProcessingPeriod_Invalid()
{
    HRESULT hr = S_OK;

    CAudioPolicyConfigApiTest objTest(hr);
    if(FAILED(hr))
    {
        goto END;
    }
        
    objTest.GetProcessingPeriod_InvalidParamsTest();

END:
    return FNS_PASS;
}

DWORD TC_PolicyConfig_SetProcessingPeriod_Invalid()
{
    HRESULT hr = S_OK;

    CAudioPolicyConfigApiTest objTest(hr);
    if(FAILED(hr))
    {
        goto END;
    }
        
    objTest.SetProcessingPeriod_InvalidParamsTest();

END:
    return FNS_PASS;
}

DWORD TC_PolicyConfig_GetShareMode_Invalid()
{
    HRESULT hr = S_OK;

    CAudioPolicyConfigApiTest objTest(hr);
    if(FAILED(hr))
    {
        goto END;
    }
        
    objTest.GetShareMode_InvalidParamsTest();

END:
    return FNS_PASS;
}

DWORD TC_PolicyConfig_SetShareMode_Invalid()
{
    HRESULT hr = S_OK;

    CAudioPolicyConfigApiTest objTest(hr);
    if(FAILED(hr))
    {
        goto END;
    }
        
    objTest.SetShareMode_InvalidParamsTest();

END:
    return FNS_PASS;
}

DWORD TC_PolicyConfig_SetDefaultEndpoint_Invalid()
{
    HRESULT hr = S_OK;

    CAudioPolicyConfigApiTest objTest(hr);
    if(FAILED(hr))
    {
        goto END;
    }
        
    objTest.SetDefaultEndpoint_InvalidParamsTest();

END:
    return FNS_PASS;
}

DWORD TC_PolicyConfig_SetEndpointVisibility_Invalid()
{
    HRESULT hr = S_OK;

    CAudioPolicyConfigApiTest objTest(hr);
    if(FAILED(hr))
    {
        goto END;
    }
        
    objTest.SetEndpointVisibility_InvalidParamsTest();

END:
    return FNS_PASS;
}

CAudioPolicyConfigApiTest::CAudioPolicyConfigApiTest(HRESULT &hr)
{
    hr = S_OK;
    
    METHODCHECKRESULT("CoCreateInstance(IPolicyConfig)",
                      hr,
                      m_spPolicyConfig.CoCreateInstance(__uuidof(PolicyConfig)));
    if(FAILED(hr))
    {
        XLOG(XFAIL,
             eError,
             "FAIL: Unable to CoCreate IPolicyConfig for testing");
    }

    return;
}

DWORD CAudioPolicyConfigApiTest::GetDeviceFormat_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    WAVEFORMATEX* pWfx = NULL;
    LPCWSTR pwszCurrentEndpoint = NULL;

    pwszCurrentEndpoint = GetEndpointIDUnderTest();
    if(NULL == pwszCurrentEndpoint)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint ID under test");
        goto END;
    }

    // NULL string
    METHODCHECKRESULT("IPolicyConfig::GetDeviceFormat", 
                      hr, 
                      m_spPolicyConfig->GetDeviceFormat( NULL, FALSE, &pWfx ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetDeviceFormat succeeded for NULL device name");    
    }

    // NULL wave param
    METHODCHECKRESULT("IPolicyConfig::GetDeviceFormat", 
                      hr, 
                      m_spPolicyConfig->GetDeviceFormat( pwszCurrentEndpoint, FALSE, NULL ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetDeviceFormat succeeded for NULL wave name");
    }

END:
    return FNS_PASS;
}

DWORD CAudioPolicyConfigApiTest::GetMixFormat_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    WAVEFORMATEX* pWfx = NULL;
    LPCWSTR pwszCurrentEndpoint = NULL;

    pwszCurrentEndpoint = GetEndpointIDUnderTest();
    if(NULL == pwszCurrentEndpoint)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint ID under test");
        goto END;
    }

    // NULL string
    METHODCHECKRESULT("IPolicyConfig::GetMixFormat", 
                      hr, 
                      m_spPolicyConfig->GetMixFormat( NULL, &pWfx ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetMixFormat succeeded for NULL device name");    
    }

    // NULL wave param
    METHODCHECKRESULT("IPolicyConfig::GetDeviceFormat", 
                      hr, 
                      m_spPolicyConfig->GetMixFormat( pwszCurrentEndpoint, NULL ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetMixFormat succeeded for NULL wave name");
    }

END:
    if(NULL != pWfx) 
    { 
        CoTaskMemFree(pWfx); 
    }

    return FNS_PASS;
}

DWORD CAudioPolicyConfigApiTest::SetDeviceFormat_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    LPCWSTR pwszCurrentEndpoint = GetEndpointIDUnderTest();

    if(NULL == pwszCurrentEndpoint)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint ID under test");
        goto END;
    }

    // NULL string
    METHODCHECKRESULT("IPolicyConfig::SetDeviceFormat", 
                      hr, 
                      m_spPolicyConfig->SetDeviceFormat( NULL, NULL, NULL ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: SetDeviceFormat succeeded for NULL device name");
    }

    // NULL wave param
    METHODCHECKRESULT("IPolicyConfig::SetDeviceFormat", 
                      hr, 
                      m_spPolicyConfig->SetDeviceFormat( pwszCurrentEndpoint, NULL, NULL) );
    if ( SUCCEEDED(hr) ) {
        XLOG(XFAIL, eError, "FAIL: SetDeviceFormat succeeded for NULL wave name");
    }

    //
    // Note: We no longer test invalid formats here, as the validity of the format is not rigorously verified
    // by the API (by design).
    //
    
END:

    return FNS_PASS;
}

DWORD CAudioPolicyConfigApiTest::GetProcessingPeriod_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    LPCWSTR pwszCurrentEndpoint = NULL;
    HNSTIME hnsPeriod = 0;

    pwszCurrentEndpoint = GetEndpointIDUnderTest();
    if(NULL == pwszCurrentEndpoint)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint ID under test");
        goto END;
    }

    METHODCHECKRESULT("IPolicyConfig::GetProcessingPeriod", 
                      hr, 
                      m_spPolicyConfig->GetProcessingPeriod(NULL, 
                                                            TRUE, 
                                                            &hnsPeriod, 
                                                            &hnsPeriod ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetProcessingPeriod succeeded for NULL device name");
    }

    METHODCHECKRESULT("IPolicyConfig::GetProcessingPeriod", 
                      hr, 
                      m_spPolicyConfig->GetProcessingPeriod(pwszCurrentEndpoint, 
                                                            TRUE, 
                                                            NULL, 
                                                            &hnsPeriod ) );
    if ( FAILED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetProcessingPeriod should succeed with one NULL period parameter.");
    }

    METHODCHECKRESULT("IPolicyConfig::GetProcessingPeriod", 
                      hr, 
                      m_spPolicyConfig->GetProcessingPeriod(pwszCurrentEndpoint, 
                                                            TRUE, 
                                                            &hnsPeriod, 
                                                            NULL ) );
    if ( FAILED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetProcessingPeriod should succeed with one NULL period parameter.");
    }

    METHODCHECKRESULT("IPolicyConfig::GetProcessingPeriod", 
                      hr,
                      m_spPolicyConfig->GetProcessingPeriod(pwszCurrentEndpoint, 
                                                            TRUE, 
                                                            NULL, 
                                                            NULL ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL,
            eError,
            "FAIL: GetProcessingPeriod should FAIL with two NULL period parameters.");
    }

END:
    return FNS_PASS;
}

DWORD CAudioPolicyConfigApiTest::SetProcessingPeriod_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    LPCWSTR pwszCurrentEndpoint = NULL;
    HNSTIME hnsPeriod = 10;
    HNSTIME hnsOldPeriod = 0;
    HNSTIME hnsMinPeriodicity = 0;

    pwszCurrentEndpoint = GetEndpointIDUnderTest();
    if(NULL == pwszCurrentEndpoint)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint ID under test");
        goto END;
    }

    METHODCHECKRESULT("IPolicyConfig::SetProcessingPeriod", 
                      hr, 
                      m_spPolicyConfig->SetProcessingPeriod( NULL, &hnsPeriod ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetProcessingPeriod succeeded for NULL device name");
    }

    METHODCHECKRESULT("IPolicyConfig::SetProcessingPeriod", 
                      hr, 
                      m_spPolicyConfig->SetProcessingPeriod( pwszCurrentEndpoint, NULL ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: SetProcessingPeriod succeeded for NULL processing time param");
    }

    // save old processing period
    METHODCHECKRESULT("IPolicyConfig::GetProcessingPeriod", 
                      hr, 
                      m_spPolicyConfig->GetProcessingPeriod(pwszCurrentEndpoint, 
                                                            FALSE, 
                                                            &hnsOldPeriod, 
                                                            &hnsMinPeriodicity ) );
    if ( FAILED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetProcessingPeriod failed");
        goto END;
    }

    hnsPeriod = 0;
    METHODCHECKRESULT("IPolicyConfig::SetProcessingPeriod", 
                      hr, 
                      m_spPolicyConfig->SetProcessingPeriod( pwszCurrentEndpoint, &hnsPeriod ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError,"FAIL: SetProcessingPeriod succeeded for processing time == 0");
    }

    hnsPeriod = -1;
    METHODCHECKRESULT("IPolicyConfig::SetProcessingPeriod", 
                      hr, 
                      m_spPolicyConfig->SetProcessingPeriod( pwszCurrentEndpoint, &hnsPeriod ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: SetProcessingPeriod succeeded for processing time == -1");
    }

    // restore processing period in case something succeeded
    METHODCHECKRESULT("IPolicyConfig::SetProcessingPeriod", 
                      hr, 
                      m_spPolicyConfig->SetProcessingPeriod( pwszCurrentEndpoint, &hnsOldPeriod ) );
    if ( FAILED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: SetProcessingPeriod should have succeeded for old period value");
    }

END:
    return FNS_PASS;
}

DWORD CAudioPolicyConfigApiTest::GetShareMode_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    LPCWSTR pwszCurrentEndpoint = NULL;
    DeviceShareMode shMode;

    pwszCurrentEndpoint = GetEndpointIDUnderTest();
    if(NULL == pwszCurrentEndpoint)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint ID under test");
        goto END;
    }

    METHODCHECKRESULT("IPolicyConfig::GetShareMode", 
                      hr, 
                      m_spPolicyConfig->GetShareMode( NULL, &shMode ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetShareMode succeeded for NULL device name");
    }

    METHODCHECKRESULT("IPolicyConfig::GetShareMode", 
                      hr, 
                      m_spPolicyConfig->GetShareMode( pwszCurrentEndpoint, NULL ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetShareMode succeeded for NULL share mode");
    } 

END:
    return FNS_PASS;
}

DWORD CAudioPolicyConfigApiTest::SetShareMode_InvalidParamsTest()
{
    HRESULT hr = S_OK;
    LPCWSTR pwszCurrentEndpoint = NULL;
    DeviceShareMode shMode = DeviceShared;
    DeviceShareMode shOldMode;

    pwszCurrentEndpoint = GetEndpointIDUnderTest();
    if(NULL == pwszCurrentEndpoint)
    {
        XLOG(XFAIL, eError, "FAIL: Unable to obtain endpoint ID under test");
        goto END;
    }

    // save current mode 
    METHODCHECKRESULT("IPolicyConfig::GetShareMode", 
                      hr, 
                      m_spPolicyConfig->GetShareMode( pwszCurrentEndpoint, &shOldMode ) );
    if ( FAILED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: GetShareMode failed");
        goto END;
    }

    shMode = (DeviceShareMode)(-1);
    METHODCHECKRESULT("IPolicyConfig::SetShareMode", 
                      hr, 
                      m_spPolicyConfig->SetShareMode( pwszCurrentEndpoint, shMode ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError,"FAIL: SetShareMode succeeded for share mode == -1");
    }

    // restore share mode 
    METHODCHECKRESULT("IPolicyConfig::SetShareMode", 
                      hr, 
                      m_spPolicyConfig->SetShareMode( pwszCurrentEndpoint, shOldMode ) );
    if ( FAILED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: SetShareMode should have succeeded for old share mode value");
    }
    
END:
    return FNS_PASS;
}

DWORD CAudioPolicyConfigApiTest::SetDefaultEndpoint_InvalidParamsTest()
{
    HRESULT hr = S_OK;

    // NULL device name
    METHODCHECKRESULT("IPolicyConfig::SetDefaultEndpoint", 
                      hr, 
                      m_spPolicyConfig->SetDefaultEndpoint( NULL, eConsole ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: SetDefaultEndpoint succeeded for NULL device name");
    }

    return FNS_PASS;
}

DWORD CAudioPolicyConfigApiTest::SetEndpointVisibility_InvalidParamsTest()
{
    HRESULT hr = S_OK;

    // NULL device name
    METHODCHECKRESULT("IPolicyConfig::SetEndpointVisibility", 
                      hr, 
                      m_spPolicyConfig->SetEndpointVisibility( NULL, FALSE ) );
    if ( SUCCEEDED(hr) ) 
    {
        XLOG(XFAIL, eError, "FAIL: SetEndpointVisibility succeeded for NULL device name, NOT VISIBLE");
    }

    METHODCHECKRESULT("IPolicyConfig::SetEndpointVisibility", 
                      hr, 
                      m_spPolicyConfig->SetEndpointVisibility( NULL, TRUE ) );
    if ( SUCCEEDED(hr) ) 
    {
        SLOG(eError,"FAIL: SetEndpointVisibility succeeded for NULL device name, VISIBLE");
    }

    return FNS_PASS;
}
