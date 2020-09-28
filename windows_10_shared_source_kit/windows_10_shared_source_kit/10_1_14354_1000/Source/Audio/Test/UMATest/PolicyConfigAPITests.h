// Copyright(C) Microsoft. All rights reserved.

//
// PolicyConfigAPITests.h
//  
//
// Description:
//
//  IAudioClient API Tests
//

#pragma once

//
// Class Name: CAudioPolicyConfigApiTest
// Description: API Testing for the IPolicyConfig Interface
//
class CAudioPolicyConfigApiTest
{
    CComPtr<IPolicyConfig>      m_spPolicyConfig;
public:
    CAudioPolicyConfigApiTest(HRESULT& hr); 

    DWORD                       GetDeviceFormat_InvalidParamsTest();
    DWORD                       GetMixFormat_InvalidParamsTest();
    DWORD                       SetDeviceFormat_InvalidParamsTest();
    DWORD                       GetProcessingPeriod_InvalidParamsTest();
    DWORD                       SetProcessingPeriod_InvalidParamsTest();
    DWORD                       GetShareMode_InvalidParamsTest();
    DWORD                       SetShareMode_InvalidParamsTest();
    DWORD                       SetDefaultEndpoint_InvalidParamsTest();
    DWORD                       SetEndpointVisibility_InvalidParamsTest();

};

