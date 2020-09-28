// Copyright(C) Microsoft. All rights reserved.//

// APISrcAPO.h
//
// Description:
//
//  Definitions for SRC APO API test classes 
//

#pragma once

class CSRCAPOAPITest : public CProcessorTestBaseAuto
{
    // General APO interface
    IAudioProcessingObjectPtr m_apoObject;
    // Specific APO interface
    IAudioRateConvertPtr m_apoSRC;
public:
    //
    // Constructor
    CSRCAPOAPITest() : CProcessorTestBaseAuto() 
    {
        HRESULT hr = APORegistration::CreateAPO(APO_SRC, NULL, m_apoObject);
        if ( FAILED(hr) )
		{
            throw TEXT("Can't create APO");
		}
        hr = m_apoObject.QueryInterface(__uuidof(IAudioRateConvert), (LPVOID*)&m_apoSRC);
        if ( FAILED(hr) )
		{
            throw TEXT("SRC APO does not support IAudioRateConvert interface");
		}
    }

    //
    // Real "reset" the APO
    HRESULT ResetApo(IAudioProcessingObjectConfigurationPtr& pApoConfig)
    {
        m_apoSRC	=	NULL;
        m_apoObject =	NULL;
        pApoConfig	=	NULL;
        HRESULT hr = APORegistration::CreateAPO(APO_SRC, NULL, m_apoObject);
        CHECK_FAIL_GOTO(hr, EXIT);
        hr	= m_apoObject.QueryInterface(__uuidof(IAudioRateConvert), (LPVOID*)&m_apoSRC);
        CHECK_FAIL_GOTO(hr, EXIT);
        hr	= m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (LPVOID*)&pApoConfig); 
        CHECK_FAIL_GOTO(hr, EXIT);

EXIT:
        return hr;
    }

    //
    // Initialize test invalid scenarios
    DWORD InitializeInvalid();

    //
    // CalculateMaxInput/Output frames valid/invalid
    DWORD CalculateFrames();

	//
	// Calculate input sample state testing
	DWORD CalcInputFramesStateTesting();

	//
	// Calculate output sample state testing
	DWORD CalcOutputFramesStateTesting();

};
