// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"

//-----------------------------------------------------------------------------
// Description:
//
//  Test for the Initialize method of the IAudioRateConvert interfaces
//
// Parameters:
//
// Return values:
//  
//      FNS_PASS            Success
//      FNS_FAIL
//
// Remarks:
//
// See Also:
//
// TODO:
//      Add tests for the param curve 
//
DWORD CSRCAPOAPITest::InitializeInvalid()
{
    AUDIO_RATE_CONVERT_PARAMS	apoParams;                   // initialize params
    AUDIO_FRAME_FORMAT			inputFormat, outputFormat;   // input/output format in the old style
    IAudioMediaTypePtr		inputMediaType;            // input format
    IAudioMediaTypePtr		outputMediaType;           // output format
    IAudioMediaType*			pinputMediaType;
    IAudioMediaType*			poutputMediaType;
	float						pInputBuffer[120];
	float						pOutputBuffer[240];
    APO_REG_PROPERTIES*			pProperties = NULL;

    HRESULT hr;                                     
    IAudioProcessingObjectConfigurationPtr apoConfig;


	hr = m_apoObject->GetRegistrationProperties( &pProperties ); 
	CHECK_FAIL_GOTO(hr, CLEANUP);

	//
	// Common to all APO fields
	apoParams.APOInit.cbSize	  = sizeof( apoParams );
	apoParams.APOInit.clsid       = pProperties->clsid;

    //
    // Acquire the configuration interfaces
    START_ASSERTION(TEXT("APO supports IAudioProcessingObjectConfiguration"));
    METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectConfiguration"),hr , m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (LPVOID*)&apoConfig)); 
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

    //*
    // Lock for process call with values that don't match
    // the values specified in the initialize call 
    //*
    START_ASSERTION(TEXT("Number of channels do not match input/output number of channels"));
    BUILD_FORMAT(&inputFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
    BUILD_FORMAT(&outputFormat,KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 44100);
    BUILD_FORMAT_ITF(&inputMediaType,  &inputFormat);
    BUILD_FORMAT_ITF(&outputMediaType, &outputFormat);

    //
    // fill the SRC initialization structure
    apoParams.fInputFrameRate  = 44100;
    apoParams.fOutputFrameRate = 48000;
    apoParams.fMaxInputFrameRate  = 44100;
    apoParams.fMaxOutputFrameRate = 48000;
    apoParams.fMinInputFrameRate  = 44100;
    apoParams.fMinOutputFrameRate = 48000;
    apoParams.eAlgorithmType = SRC_QUALITY_LINEAR;
    apoParams.eFlowType = AUDIO_FLOW_PULL;
    apoParams.u32SamplesPerFrame = 1;

    pinputMediaType = inputMediaType;
    poutputMediaType = outputMediaType;


    // call APO initialize
    METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams) );
    CHECKFAILED_GOTO(hr, CLEANUP);
    // call LockForProcess
    METHODCHECKRESULT(TEXT("LockForProcess"), hr,  apoConfig->LockForProcess(1,CLockForProcAdapter<1,41,4>(&pinputMediaType,pInputBuffer), 1, CLockForProcAdapter<1,48,4>(&poutputMediaType,pOutputBuffer)));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    // reset the APO because we can't re-initialize the APO
    METHODCHECKRESULT(TEXT("CSRCAPOAPITest::ResetApo"), hr,  ResetApo(apoConfig));
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
    _RELEASE_(inputMediaType);
    _RELEASE_(outputMediaType);
    END_ASSERTION();

    //*
    // Call initialize twice should fail
    //*
    START_ASSERTION(TEXT("Initialize can't be called twice"));
    BUILD_FORMAT(&inputFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 44100);
    BUILD_FORMAT(&outputFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2, 4, 32, 48000);
    BUILD_FORMAT_ITF(&inputMediaType,  &inputFormat);
    BUILD_FORMAT_ITF(&outputMediaType, &outputFormat);

    //
    // fill the SRC parameters
    apoParams.fInputFrameRate     = 44100;
    apoParams.fOutputFrameRate    = 48000;
    apoParams.fMaxInputFrameRate  = 44100;
    apoParams.fMaxOutputFrameRate = 48000;
    apoParams.fMinInputFrameRate  = 44100;
    apoParams.fMinOutputFrameRate = 48000;
    apoParams.eAlgorithmType      = SRC_QUALITY_LINEAR;
    apoParams.eFlowType           = AUDIO_FLOW_PULL;
    apoParams.u32SamplesPerFrame  = 2;

    pinputMediaType = inputMediaType;
    poutputMediaType = outputMediaType;

    // call APO initialize
    METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams) );
    CHECKFAILED_GOTO(hr, CLEANUP);
    // call APO intialize again
    METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams) );
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    // call LockForProcess
    METHODCHECKRESULT(TEXT("LockForProcess"), hr,  apoConfig->LockForProcess(1,CLockForProcAdapter<1,49,4>(&pinputMediaType,pInputBuffer), 1, CLockForProcAdapter<1,48,4>(&poutputMediaType,pOutputBuffer)));
    CHECKFAILED_GOTO(hr, CLEANUP);
    // call UnlockForProcess
    METHODCHECKRESULT(TEXT("UnlockForProcess"), hr,  apoConfig->UnlockForProcess());
    CHECKFAILED_GOTO(hr, CLEANUP);
    METHODCHECKRESULT(TEXT("CSRCAPOAPITest::ResetApo"), hr,  ResetApo(apoConfig));
    CHECKFAILED_GOTO(hr, CLEANUP);
    _RELEASE_(inputMediaType);
    _RELEASE_(outputMediaType);
    END_ASSERTION();

    //*
    // SetFrameRate called before initialize
    //*
    START_ASSERTION(TEXT("SetFrameRate should fail if called before Initialize"));
    // change input frame rate
    METHODCHECKRESULT(TEXT("SetInputFrameRate"), hr, m_apoSRC->SetInputFrameRate(48000));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    // change output frame rate
    METHODCHECKRESULT(TEXT("SetOutputFrameRate"), hr, m_apoSRC->SetOutputFrameRate(48000));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    END_ASSERTION();


    //*
    // SetFrameRate invalid frame rates
    //*
    START_ASSERTION(TEXT("SetFrameRate with sample rate not in the range specified by Initialize"));
    BUILD_FORMAT(&inputFormat,KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 2,4,32,48000);
    BUILD_FORMAT(&outputFormat,KSDATAFORMAT_SUBTYPE_IEEE_FLOAT,2,4,32,44100);
    BUILD_FORMAT_ITF(&inputMediaType,  &inputFormat);
    BUILD_FORMAT_ITF(&outputMediaType, &outputFormat);

    // fill the APO params
    apoParams.fInputFrameRate  = 44100;
    apoParams.fOutputFrameRate = 48000;
    apoParams.fMaxInputFrameRate  = 44100;
    apoParams.fMaxOutputFrameRate = 48000;
    apoParams.fMinInputFrameRate  = 44100;
    apoParams.fMinOutputFrameRate = 48000;
    apoParams.eAlgorithmType = SRC_QUALITY_LINEAR;
    apoParams.eFlowType = AUDIO_FLOW_PULL;
    apoParams.u32SamplesPerFrame = 2;

    // call initialize
    METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
    CHECKFAILED_GOTO(hr, CLEANUP);
    // change input frame rate 
    METHODCHECKRESULT(TEXT("SetInputFrameRate"), hr, m_apoSRC->SetInputFrameRate(16000));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    // change output frame rate
    METHODCHECKRESULT(TEXT("SetInputFrameRate"), hr, m_apoSRC->SetOutputFrameRate(16000));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    METHODCHECKRESULT(TEXT("CSRCAPOAPITest::ResetApo"), hr,  ResetApo(apoConfig));
    CHECKFAILED_GOTO(hr, CLEANUP);
    _RELEASE_(inputMediaType);
    _RELEASE_(outputMediaType);
    END_ASSERTION();

    //*
    // Invalid flags
    //* 
    START_ASSERTION(TEXT("Initialize should fail for invalid algorithm type"));
    apoParams.fInputFrameRate  = 44100;
    apoParams.fOutputFrameRate = 48000;
    apoParams.fMaxInputFrameRate  = 44100;
    apoParams.fMaxOutputFrameRate = 48000;
    apoParams.fMinInputFrameRate  = 44100;
    apoParams.fMinOutputFrameRate = 48000;
    apoParams.eAlgorithmType = (SRC_ALGORITHM_TYPE)0x80;
    apoParams.eFlowType = AUDIO_FLOW_PULL;
    apoParams.u32SamplesPerFrame = 2;
    
	METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams) );
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    END_ASSERTION()

    START_ASSERTION(TEXT("Initialize should fail for invalid flow type"));
    apoParams.fInputFrameRate  = 44100;
    apoParams.fOutputFrameRate = 48000;
    apoParams.eAlgorithmType = SRC_QUALITY_LINEAR;
    apoParams.eFlowType = (AUDIO_FLOW_TYPE)0x80;
    apoParams.u32SamplesPerFrame = 2;
    apoParams.fMaxInputFrameRate  = 44100;
    apoParams.fMaxOutputFrameRate = 48000;
    apoParams.fMinInputFrameRate  = 44100;
    apoParams.fMinOutputFrameRate = 48000;
    
	METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    END_ASSERTION()

    START_ASSERTION(TEXT("Initialize should fail if min frame rate > max frame rate"));
    apoParams.fInputFrameRate  = 44100;
    apoParams.fOutputFrameRate = 48000;
    apoParams.eAlgorithmType = SRC_QUALITY_LINEAR;
    apoParams.eFlowType = (AUDIO_FLOW_TYPE)0x80;
    apoParams.u32SamplesPerFrame = 2;
    apoParams.fMaxInputFrameRate  = 44100;
    apoParams.fMaxOutputFrameRate = 48000;
    apoParams.fMinInputFrameRate  = 48000;
    apoParams.fMinOutputFrameRate = 48000;
    
	METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    END_ASSERTION()

    START_ASSERTION(TEXT("Initialize should fail if the input frame rate is not in the input range"));
    apoParams.fInputFrameRate  = 44100;
    apoParams.fOutputFrameRate = 48000;
    apoParams.eAlgorithmType = SRC_QUALITY_LINEAR;
    apoParams.eFlowType = (AUDIO_FLOW_TYPE)0x80;
    apoParams.u32SamplesPerFrame = 2;
    apoParams.fMaxInputFrameRate  = 48000;
    apoParams.fMaxOutputFrameRate = 48000;
    apoParams.fMinInputFrameRate  = 48000;
    apoParams.fMinOutputFrameRate = 48000;

    METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    END_ASSERTION()

    START_ASSERTION(TEXT("Initialize should fail if the output frame rate not in the output range"));
    apoParams.fInputFrameRate  = 48000;
    apoParams.fOutputFrameRate = 44100;
    apoParams.eAlgorithmType = SRC_QUALITY_LINEAR;
    apoParams.eFlowType = (AUDIO_FLOW_TYPE)0x80;
    apoParams.u32SamplesPerFrame = 2;
    apoParams.fMaxInputFrameRate  = 48000;
    apoParams.fMaxOutputFrameRate = 48000;
    apoParams.fMinInputFrameRate  = 48000;
    apoParams.fMinOutputFrameRate = 48000;

    METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    END_ASSERTION()

    START_ASSERTION(TEXT("Initialize should fail if NaN is used for the sample rate"));
    apoParams.fInputFrameRate  = sqrt(-1.00f);
    apoParams.fOutputFrameRate = 48000;
    apoParams.eAlgorithmType = SRC_QUALITY_LINEAR;
    apoParams.eFlowType = (AUDIO_FLOW_TYPE)0x80;
    apoParams.u32SamplesPerFrame = 2;
    apoParams.fMaxInputFrameRate  = 48000;
    apoParams.fMaxOutputFrameRate = 48000;
    apoParams.fMinInputFrameRate  = 48000;
    apoParams.fMinOutputFrameRate = 48000;

    METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
    END_ASSERTION()


CLEANUP:
    return HRESULT2SHELL(hr);
}

//-----------------------------------------------------------------------------
// Description:
//
//  Test for the frame computing API
//
// Parameters:
//
// Return values:
//  
//      FNS_PASS            Success
//      FNS_FAIL
//
// Remarks:
//
// See Also:
//
// TODO:
//      Add tests for the param curve 
//
DWORD CSRCAPOAPITest::CalculateFrames()
{
    AUDIO_RATE_CONVERT_PARAMS				apoParams;            // initialize params
    AUDIO_FRAME_FORMAT						inputFormat, 
											outputFormat;   // input/output format in the old style
    APO_REG_PROPERTIES*						pProperties	=	NULL;

    HRESULT hr;                                     // error code
    IAudioProcessingObjectConfigurationPtr apoConfig;
    IAudioProcessingObjectVBRPtr		   apoObjectVBR;


	hr = m_apoObject->GetRegistrationProperties( &pProperties ); 
	CHECK_FAIL_GOTO(hr, CLEANUP);

	//
	// Common to all APO fields
	apoParams.APOInit.cbSize	  = sizeof( apoParams );
	apoParams.APOInit.clsid       = pProperties->clsid;

    //
    // Acquire the configuration interfaces
    START_ASSERTION(TEXT("APO supports IAudioProcessingObjectConfiguration"));
    METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectConfiguration"),hr , m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (LPVOID*)&apoConfig)); 
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

    START_ASSERTION(TEXT("APO supports IAudioProcessingObjectVBR"));
    METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectVBR"),hr , m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectVBR), (LPVOID*)&apoObjectVBR)); 
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

    //
    // fill the SRC initialization structure
    apoParams.fInputFrameRate		= 44100;
    apoParams.fOutputFrameRate		= 48000;
    apoParams.fMaxInputFrameRate	= 44100;
    apoParams.fMaxOutputFrameRate	= 48000;
    apoParams.fMinInputFrameRate	= 44100;
    apoParams.fMinOutputFrameRate	= 48000;
    apoParams.eAlgorithmType		= SRC_QUALITY_LINEAR;
    apoParams.eFlowType				= AUDIO_FLOW_PULL;
    apoParams.u32SamplesPerFrame	= 1;

	//
    // call APO initialize
	START_ASSERTION(TEXT("Can Initialize SRC APO"));
    METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
    CHECKFAILED_GOTO(hr, CLEANUP);
	END_ASSERTION();

	//
	// calculate max input frames NULL param
	START_ASSERTION(TEXT("CalcMaxInputFrames NULL out param"));	
    METHODCHECKRESULT(TEXT("CalcMaxInputFrames"), hr, apoObjectVBR->CalcMaxInputFrames(20, NULL));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
	END_ASSERTION();

	//
	// calculate max output frames NULL param
	START_ASSERTION(TEXT("CalcMaxOutputFrames NULL out param"));	
    METHODCHECKRESULT(TEXT("CalcMaxOutputFrames"), hr, apoObjectVBR->CalcMaxOutputFrames(20, NULL));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
	END_ASSERTION();

	//
	// calculate max input frames for 0 output frames
	UINT32 u32Frames;
	START_ASSERTION(TEXT("CalcMaxInputFrames for 0 output frames"));	
    METHODCHECKRESULT(TEXT("CalcMaxInputFrames"), hr, apoObjectVBR->CalcMaxInputFrames(0, &u32Frames));
    ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
	END_ASSERTION();

	//
	// calculate max output frames for 0 input frames
	// this makes sense sometimes in flush mode ( filter tail however
	// I would argue against )
	START_ASSERTION(TEXT("CalcMaxOutputFrames for 0 input frames"));	
    METHODCHECKRESULT(TEXT("CalcMaxOutputFrames"), hr, apoObjectVBR->CalcMaxOutputFrames(0, &u32Frames));
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

	//
	// calculate max input frames some output value
	START_ASSERTION(TEXT("CalcMaxInputFrames valid"));	
    METHODCHECKRESULT(TEXT("CalcMaxInputFrames"), hr, apoObjectVBR->CalcMaxInputFrames(20, &u32Frames));
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

	//
	// calculate max output frames for some input value
	START_ASSERTION(TEXT("CalcMaxOutputFrames valid"));	
    METHODCHECKRESULT(TEXT("CalcMaxOutputFrames"), hr, apoObjectVBR->CalcMaxOutputFrames(20, &u32Frames));
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

	//TODO: Add validity check to make sure the returned values are good
CLEANUP:

    return HRESULT2SHELL(hr);
} // CalculateFrames

//-----------------------------------------------------------------------------
// Description:
//
//  State testing for CalcInputFrames
//
// Parameters:
//
// Return values:
//  
//      FNS_PASS            Success
//      FNS_FAIL
//
// Remarks:
//
// See Also:
//
DWORD CSRCAPOAPITest::CalcInputFramesStateTesting()
{
    AUDIO_RATE_CONVERT_PARAMS apoParams;            // initialize params
    AUDIO_FRAME_FORMAT inputFormat, outputFormat;
	float* pfOutputBuffer = NULL;
	float* pfInputBuffer  = NULL;

    HRESULT hr;                                     

	IAudioProcessingObjectConfigurationPtr	apoConfig;
	IAudioProcessingObjectRTPtr				apoProcess;
    IAudioProcessingObjectVBRPtr			apoObjectVBR;
	
	UINT32									u32MaxOutputFrames = 48, 
											u32MaxInputFrames = 0;
    
	IAudioMediaType						*OutMediaType = NULL;
    IAudioMediaType						*InMediaType  = NULL;

	APO_CONNECTION_PROPERTY					InputConnection, 
											OutputConnection;
	APO_CONNECTION_PROPERTY					*pInputConnection  = &InputConnection, 
											*pOutputConnection = &OutputConnection;

	SYSTEM_INFO sysinfo;
	
	UINT32									u32InputOffset	= 0, 
											u32OutputOffset = 0;
    
	APO_REG_PROPERTIES*						pProperties = NULL;

    //
    // Acquire the configuration interfaces
    START_ASSERTION(TEXT("APO supports IAudioProcessingObjectConfiguration"));
    METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectConfiguration"),hr , m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (LPVOID*)&apoConfig)); 
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

    START_ASSERTION(TEXT("APO supports IAudioProcessingObjectVBR"));
    METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectVBR"),hr , m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectVBR), (LPVOID*)&apoObjectVBR)); 
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

	hr = m_apoObject->GetRegistrationProperties( &pProperties ); 
	CHECK_FAIL_GOTO(hr, CLEANUP);

	//
	// Common to all APO fields
	apoParams.APOInit.cbSize	  = sizeof( apoParams );
	apoParams.APOInit.clsid       = pProperties->clsid;

    //
    // fill the SRC initialization structure
    apoParams.fInputFrameRate         = 44100;
    apoParams.fOutputFrameRate        = 48000;
    apoParams.fMaxInputFrameRate      = 48000;
    apoParams.fMaxOutputFrameRate     = 48000;
    apoParams.fMinInputFrameRate      = 8000;
    apoParams.fMinOutputFrameRate     = 8000;
    apoParams.eAlgorithmType          = SRC_QUALITY_LINEAR;
    apoParams.eFlowType               = AUDIO_FLOW_PULL; 
    apoParams.u32SamplesPerFrame      = 1; // 1 channel
	apoParams.u32HighQualityTaplength = 0; // not supported

	//
	// create formats
    BUILD_FORMAT(&inputFormat,   KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 44100); 
    BUILD_FORMAT(&outputFormat,  KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000); 

	//
    // call APO initialize
	START_ASSERTION(TEXT("Can Initialize SRC APO"));
    METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
    CHECKFAILED_GOTO(hr, CLEANUP);
	END_ASSERTION();

	//
	// calculate max input frames
	START_ASSERTION(TEXT("CalcMaxInputFrames should succeed"));
    METHODCHECKRESULT(TEXT("CalcMaxInputFrames"), hr, apoObjectVBR->CalcMaxInputFrames(u32MaxOutputFrames,&u32MaxInputFrames));
    CHECKFAILED_GOTO(hr, CLEANUP);
	END_ASSERTION();

	// allocate input/output buffer
	// the buffer is allocated such that we can detect a buffer overrun with an AV
	GetSystemInfo(&sysinfo);

	pfOutputBuffer  =(float*)VirtualAlloc(NULL, 3*sysinfo.dwPageSize, MEM_COMMIT, PAGE_READWRITE);
	pfInputBuffer   =(float*)VirtualAlloc(NULL,  3*sysinfo.dwPageSize, MEM_COMMIT, PAGE_READWRITE);
	u32InputOffset  = sysinfo.dwPageSize;
	u32OutputOffset = sysinfo.dwPageSize;
	pfOutputBuffer  = (float*)((LPBYTE)pfOutputBuffer + u32OutputOffset);
	pfInputBuffer   = (float*)((LPBYTE)pfInputBuffer + u32InputOffset);

	if (!pfOutputBuffer || !pfInputBuffer)
	{
		SLOG(eError, "Out of memory!");
		hr = E_OUTOFMEMORY;
		goto CLEANUP;
	}

	VirtualProtect((LPBYTE)pfOutputBuffer - u32OutputOffset,sysinfo.dwPageSize,  PAGE_READONLY, NULL);
	VirtualProtect((LPBYTE)pfInputBuffer  - u32InputOffset ,sysinfo.dwPageSize,  PAGE_READONLY, NULL);
	VirtualProtect((LPBYTE)pfOutputBuffer + u32OutputOffset,sysinfo.dwPageSize,  PAGE_READONLY, NULL);
	VirtualProtect((LPBYTE)pfInputBuffer  + u32InputOffset ,sysinfo.dwPageSize,  PAGE_READONLY, NULL);


	//
	// prepare media types for LockForProcess
	BUILD_FORMAT_ITF(&InMediaType,  &inputFormat);
	BUILD_FORMAT_ITF(&OutMediaType, &outputFormat);

	//
	// fill the input/output connection property
	InputConnection.pBuffer = (UINT_PTR)pfInputBuffer;
	InputConnection.u32BufferFlags = BUFFER_VALID;
	InputConnection.u32Signature = APO_CONNECTION_PROPERTY_SIGNATURE;
	InputConnection.u32ValidFrameCount = 0;

	OutputConnection.pBuffer = (UINT_PTR)pfOutputBuffer;
	OutputConnection.u32BufferFlags = BUFFER_INVALID;
	OutputConnection.u32Signature = APO_CONNECTION_PROPERTY_SIGNATURE;
	OutputConnection.u32ValidFrameCount = 0;

	START_ASSERTION(TEXT("LockForProcess should succeed"));
	METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,0,4>(&InMediaType, pfInputBuffer,u32MaxInputFrames), 1, CLockForProcAdapter<1,0,0>(&OutMediaType, pfOutputBuffer,u32MaxOutputFrames))); 
	ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

	START_ASSERTION(TEXT("IAudioProcessingObjectRT is supported"));
    METHODCHECKRESULT(TEXT("QI"),hr, m_apoObject.QueryInterface(__uuidof(IAudioProcessingObjectRT), (LPVOID*)&apoProcess));
	ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();
	
	//
	// Test: SetInput/OutputFrameRate->CalcInputFrames->Process
	// Note that: CalcInputFrames->SetInputFrameRate->Process is illegal
	START_ASSERTION(TEXT("SetInput(Output)FrameRate->CalcInputFrames->Process"));
	srand(time(NULL));
	for (UINT32 u32=0; u32 < 20; u32++)
	{
		int usample=rand();
		if (usample%2)
		{
			double lfNewRate = 8000.0 + ((double)usample/(double)RAND_MAX)*(48000.0-8000.0);
			SLOG(eInfo2,"New input sample rate %.2lf", lfNewRate);
			METHODCHECKRESULT(TEXT("SetInputFrameRate"),hr, m_apoSRC->SetInputFrameRate( lfNewRate ));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
		else
		{
			double lfNewRate = 8000.0 + ((double)usample/(double)RAND_MAX)*(48000.0-8000.0);
			SLOG(eInfo2,"New output sample rate %.2lf", lfNewRate);
			METHODCHECKRESULT(TEXT("SetOutputFrameRate"),hr, m_apoSRC->SetOutputFrameRate( lfNewRate ));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
		UINT32 u32InputFrames = apoProcess->CalcInputFrames(u32MaxOutputFrames);
		InputConnection.u32ValidFrameCount = u32InputFrames;
		apoProcess->APOProcess(1, &pInputConnection, 1, &pOutputConnection); 			
		// 
		// While it is possible to get the exact number of frames that we will 
		// output for a given input number of frames we have agreed for now to
		// accept a smaller number of real output frames 
		if (pOutputConnection->u32ValidFrameCount > u32MaxOutputFrames)
		{
			SLOG(eError, "ERROR: Returned frames from APOProcess %ld > CalcOutputFrames result %ld",pOutputConnection->u32ValidFrameCount , u32MaxOutputFrames );
			hr=E_FAIL;
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
		//
		// For now enforce that the difference equal the history which is probably
		// unnacounted for
		if ( (__int64)(pOutputConnection->u32ValidFrameCount) < (__int64)u32MaxOutputFrames - 4 )
		{
			SLOG(eError, "ERROR: Returned frames from APOProcess %ld differ by more than 4 from CalcOutputFrames result %ld",pOutputConnection->u32ValidFrameCount , u32MaxOutputFrames );
			hr=E_FAIL;
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}

	}
	END_ASSERTION();

	//
	// Test: CalcInputFrames->CalcInputFrames->Process
	START_ASSERTION(TEXT("CalcInputFrames->CalcInputFrames->Process"));
	srand(g_IShell->GetRandomSeed());
	for (UINT32 u32=0; u32 < 40; u32++)
	{
		UINT32 u32InputFrames = apoProcess->CalcInputFrames(u32MaxOutputFrames);
		u32InputFrames = apoProcess->CalcInputFrames(u32MaxOutputFrames);
		InputConnection.u32ValidFrameCount = u32InputFrames;
		apoProcess->APOProcess(1, &pInputConnection, 1, &pOutputConnection); 			
		// 
		// While it is possible to get the exact number of frames that we will 
		// output for a given input number of frames we have agreed for now to
		// accept a smaller number of real output frames 
		if (pOutputConnection->u32ValidFrameCount > u32MaxOutputFrames)
		{
			SLOG(eError, "ERROR: Returned frames from APOProcess %ld > CalcOutputFrames result %ld",pOutputConnection->u32ValidFrameCount , u32MaxOutputFrames );
			hr=E_FAIL;
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
		//
		// For now enforce that the difference equal the history which is probably
		// unnacounted for
		if ( (__int64)(pOutputConnection->u32ValidFrameCount) < (__int64)u32MaxOutputFrames - 4 )
		{
			SLOG(eError, "ERROR: Returned frames from APOProcess %ld differ by more than 4 from CalcOutputFrames result %ld",pOutputConnection->u32ValidFrameCount , u32MaxOutputFrames );
			hr=E_FAIL;
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
	}
	END_ASSERTION();
/* This is also illegal
	//
	// Test: CalcInputFrames->Reset->Process
	START_ASSERTION(TEXT("CalcInputFrames->Reset->Process"));
	srand(time(NULL));
	for (UINT32 u32=0; u32 < 20; u32++)
	{
		UINT32 u32InputFrames = apoProcess->CalcInputFrames(u32MaxOutputFrames);
		InputConnection.u32ValidFrameCount = u32InputFrames;
		METHODCHECKRESULT(TEXT("Reset"), hr, m_apoObject->Reset());
		ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		apoProcess->APOProcess(1, &pInputConnection, 1, &pOutputConnection); 			
	}
	END_ASSERTION();
*/

CLEANUP:
	if (pfOutputBuffer) {
		BOOL bFree = VirtualFree((LPBYTE)pfOutputBuffer - u32OutputOffset, 0, MEM_RELEASE);
		_ASSERT( bFree );
	}
	if (pfInputBuffer) {
		BOOL bFree = VirtualFree((LPBYTE)pfInputBuffer - u32InputOffset, 0, MEM_RELEASE);
		_ASSERT( bFree );
	}

	_RELEASE_(InMediaType);
	_RELEASE_(OutMediaType);

    return HRESULT2SHELL(hr);
}

//-----------------------------------------------------------------------------
// Description:
//
//  State testing for CalcOutputFrames
//
// Parameters:
//
// Return values:
//  
//      FNS_PASS            Success
//      FNS_FAIL
//
// Remarks:
//
// See Also:
//
DWORD CSRCAPOAPITest::CalcOutputFramesStateTesting()
{
    AUDIO_RATE_CONVERT_PARAMS apoParams;            // initialize params
    AUDIO_FRAME_FORMAT inputFormat, outputFormat;
	float* pfInputBuffer = NULL;
	float* pfOutputBuffer = NULL;

    HRESULT hr;                                     // error code
    IAudioProcessingObjectConfigurationPtr	apoConfig;
	IAudioProcessingObjectRTPtr				apoProcess;
	IAudioProcessingObjectVBRPtr			apoObjectVBR;
	
	UINT32	u32MaxOutputFrames = 0, 
			u32MaxInputFrames = 48;

	IAudioMediaType   *OutMediaType = NULL;
    IAudioMediaType   *InMediaType = NULL;

	APO_CONNECTION_PROPERTY InputConnection, 
							OutputConnection;
	APO_CONNECTION_PROPERTY *pInputConnection = &InputConnection, 
							*pOutputConnection = &OutputConnection;

	SYSTEM_INFO sysinfo;

	UINT32 u32InputOffset	=	 0, 
		   u32OutputOffset	=	0;

	APO_REG_PROPERTIES* pProperties = NULL;

	hr = m_apoObject->GetRegistrationProperties( &pProperties ); 
	CHECK_FAIL_GOTO(hr, CLEANUP);

	//
	// Common to all APO fields
	apoParams.APOInit.cbSize	  = sizeof( apoParams );
	apoParams.APOInit.clsid       = pProperties->clsid;

    //
    // Acquire the configuration interfaces
    START_ASSERTION(TEXT("APO supports IAudioProcessingObjectConfiguration"));
    METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectConfiguration"),hr , m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (LPVOID*)&apoConfig)); 
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

    START_ASSERTION(TEXT("APO supports IAudioProcessingObjectVBR"));
    METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectVBR"),hr , m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectVBR), (LPVOID*)&apoObjectVBR)); 
    ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

    //
    // fill the SRC initialization structure
    apoParams.fInputFrameRate			= 44100;
    apoParams.fOutputFrameRate			= 48000;
    apoParams.fMaxInputFrameRate		= 48000;
    apoParams.fMaxOutputFrameRate		= 48000;
    apoParams.fMinInputFrameRate		= 8000;
    apoParams.fMinOutputFrameRate		= 8000;
    apoParams.eAlgorithmType			= SRC_QUALITY_LINEAR;
    apoParams.eFlowType					= AUDIO_FLOW_PUSH; 
    apoParams.u32SamplesPerFrame		= 1; // 1 channel
	apoParams.u32HighQualityTaplength	= 0; // not supported

	//
	// create formats
    BUILD_FORMAT(&inputFormat,   KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 44100); 
    BUILD_FORMAT(&outputFormat,  KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 1, 4, 32, 48000); 

	//
    // call APO initialize
	START_ASSERTION(TEXT("Can Initialize SRC APO"));
    METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
    CHECKFAILED_GOTO(hr, CLEANUP);
	END_ASSERTION();

	//
	// calculate max output frames
	START_ASSERTION(TEXT("CalcMaxOutputFrames should succeed"));
    METHODCHECKRESULT(TEXT("CalcMaxOutputFrames"), hr, apoObjectVBR->CalcMaxOutputFrames(u32MaxInputFrames,&u32MaxOutputFrames));
    CHECKFAILED_GOTO(hr, CLEANUP);
	END_ASSERTION();

	SLOG(eInfo1, "CalcMaxOutputFrames returned %ld for %ld", u32MaxOutputFrames, u32MaxInputFrames);

	//
	// allocate input/output buffer
	// the buffer is allocated such that we can detect a buffer overrun/underrun with an AV
	GetSystemInfo(&sysinfo);

	pfOutputBuffer=(float*)VirtualAlloc(NULL, 3*sysinfo.dwPageSize, MEM_COMMIT, PAGE_READWRITE);
	pfInputBuffer=(float*)VirtualAlloc(NULL,  3*sysinfo.dwPageSize, MEM_COMMIT, PAGE_READWRITE);
	u32InputOffset = sysinfo.dwPageSize;
	u32OutputOffset = sysinfo.dwPageSize;
	pfOutputBuffer = (float*)((LPBYTE)pfOutputBuffer + u32InputOffset);
	pfInputBuffer = (float*)((LPBYTE)pfInputBuffer + u32OutputOffset);
	if (!pfOutputBuffer || !pfInputBuffer)
	{
		SLOG(eError, "Out of memory!");
		hr=E_OUTOFMEMORY;
		goto CLEANUP;
	}
	VirtualProtect((LPBYTE)pfOutputBuffer - u32OutputOffset,sysinfo.dwPageSize,  PAGE_READONLY, NULL);
	VirtualProtect((LPBYTE)pfInputBuffer  - u32InputOffset ,sysinfo.dwPageSize,  PAGE_READONLY, NULL);
	VirtualProtect((LPBYTE)pfOutputBuffer + u32OutputOffset,sysinfo.dwPageSize,  PAGE_READONLY, NULL);
	VirtualProtect((LPBYTE)pfInputBuffer  + u32InputOffset ,sysinfo.dwPageSize,  PAGE_READONLY, NULL);

	//
	// prepare media types for LockForProcess
	BUILD_FORMAT_ITF(&InMediaType,  &inputFormat);
	BUILD_FORMAT_ITF(&OutMediaType, &outputFormat);

	//
	// fill the input/output connection property
	InputConnection.pBuffer = (UINT_PTR)pfInputBuffer;
	InputConnection.u32BufferFlags = BUFFER_VALID;
	InputConnection.u32Signature = APO_CONNECTION_PROPERTY_SIGNATURE;
	InputConnection.u32ValidFrameCount = 0;

	OutputConnection.pBuffer = (UINT_PTR)pfOutputBuffer;
	OutputConnection.u32BufferFlags = BUFFER_INVALID;
	OutputConnection.u32Signature = APO_CONNECTION_PROPERTY_SIGNATURE;
	OutputConnection.u32ValidFrameCount = 0;

	START_ASSERTION(TEXT("LockForProcess should succeed"));
	METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,0,4>(&InMediaType, pfInputBuffer,u32MaxInputFrames), 1, CLockForProcAdapter<1,0,0>(&OutMediaType, pfOutputBuffer,u32MaxOutputFrames))); 
	ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();

	START_ASSERTION(TEXT("IAudioProcessingObjectRT is supported"));
    METHODCHECKRESULT(TEXT("QI"),hr, m_apoObject.QueryInterface(__uuidof(IAudioProcessingObjectRT), (LPVOID*)&apoProcess));
	ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
	END_ASSERTION();
	
	//
	// Test: SetInput/OutputFrameRate->CalcOutputFrames->Process
	// Note that: CalcOutputFrames->SetInputFrameRate->Process is illegal
	START_ASSERTION(TEXT("SetInput(Output)FrameRate->CalcInputFrames->Process"));
	srand(g_IShell->GetRandomSeed());
	for (UINT32 u32=0; u32 < 40; u32++)
	{
		int usample=rand();
		u32MaxOutputFrames = apoProcess->CalcOutputFrames(u32MaxInputFrames);

		if ( usample%2 ) {
			double lfNewRate = 8000.0 + ((double)usample/(double)RAND_MAX)*(48000.0-8000.0);
			SLOG(eInfo2,"New input sample rate %.2lf", lfNewRate);
			METHODCHECKRESULT(TEXT("SetInputFrameRate"),hr, m_apoSRC->SetInputFrameRate( lfNewRate ));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
		else {
			double lfNewRate = 8000.0 + ((double)usample/(double)RAND_MAX)*(48000.0-8000.0);
			SLOG(eInfo2,"New output sample rate %.2lf", lfNewRate);
			METHODCHECKRESULT(TEXT("SetOutputFrameRate"),hr, m_apoSRC->SetOutputFrameRate( lfNewRate ));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
		SLOG(eInfo2,"CalcOutputFrames before sample rate change (%ld)=%ld",u32MaxOutputFrames,u32MaxInputFrames);
		u32MaxOutputFrames = apoProcess->CalcOutputFrames(u32MaxInputFrames);
		SLOG(eInfo2,"CalcOutputFrames after sample rate change (%ld)=%ld",u32MaxOutputFrames,u32MaxInputFrames);
		InputConnection.u32ValidFrameCount = u32MaxInputFrames;
		apoProcess->APOProcess(1, &pInputConnection, 1, &pOutputConnection); 
		// 
		// While it is possible to get the exact number of frames that we will 
		// output for a given input number of frames we have agreed for now to
		// accept a smaller number of real output frames 
		if (pOutputConnection->u32ValidFrameCount > u32MaxOutputFrames)
		{
			SLOG(eError, "ERROR: Returned frames from APOProcess %ld > CalcOutputFrames result %ld",pOutputConnection->u32ValidFrameCount , u32MaxOutputFrames );
			hr=E_FAIL;
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
		//
		// For now enforce that the difference equal the history which is probably
		// unnacounted for
		if ( (__int64)pOutputConnection->u32ValidFrameCount < (__int64)u32MaxOutputFrames - 10 )
		{
			SLOG(eError, "ERROR: Returned frames from APOProcess %ld differ by more than 10 from CalcOutputFrames result %ld",pOutputConnection->u32ValidFrameCount , u32MaxOutputFrames );
			hr=E_FAIL;
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
	}
	END_ASSERTION();


    // DennisE 05/30/2006
    // Fix for Bug: 1585755	Audio Engine Test\API Test\Processor\SRC APO\CalcOutputFrames state testing: failing ERROR: Returned frames from APOProcess 26 > CalcOutputFrames result 25
    //     The following loop does not do anything that is not covered in the loop above. It looks like there was an attempt to test 
    //     "CalcOutputFrames->SetInputFrameRate->Process", but it is stated above it would be illegal. 
    //     It does not make sense to call CalcOutputFrames() twice in the loop below: it does not change apoProcess since there are no rate changes. 
    //     The loop would execute 20 times with the same input/output rates set in the last loop execution above. 
    //     Fixing by commenting out.

    /*
	//
	// Test: CalcOutputFrames->CalcOutputFrames->Process
	START_ASSERTION(TEXT("CalcOutputFrames->CalcOutputFrames->Process"));
	for (UINT32 u32=0; u32 < 20; u32++)
	{
		UINT32 u32OutputFrames = apoProcess->CalcOutputFrames(u32MaxInputFrames);
		u32OutputFrames = apoProcess->CalcOutputFrames(u32MaxInputFrames);
		InputConnection.u32ValidFrameCount = u32MaxInputFrames;
		apoProcess->APOProcess(1, &pInputConnection, 1, &pOutputConnection); 	

		// 
		// While it is possible to get the exact number of frames that we will 
		// output for a given input number of frames we have agreed for now to
		// accept a smaller number of real output frames 
		if (pOutputConnection->u32ValidFrameCount > u32OutputFrames)
		{
			SLOG(eError, "ERROR: Returned frames from APOProcess %ld > CalcOutputFrames result %ld",pOutputConnection->u32ValidFrameCount , u32MaxOutputFrames );
			hr=E_FAIL;
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
		//
		// For now enforce that the difference equal the history which is probably
		// unnacounted for
		if ( pOutputConnection->u32ValidFrameCount < u32OutputFrames - 4 )
		{
			SLOG(eError, "ERROR: Returned frames from APOProcess %ld differ by more than 4 from CalcOutputFrames result %ld",pOutputConnection->u32ValidFrameCount , u32MaxOutputFrames );
			hr=E_FAIL;
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
	}
	END_ASSERTION();
    */

/*
	//
	// Test: CalcOutputFrames->Reset->Process
	START_ASSERTION(TEXT("CalcOutputFrames->Reset->Process"));
	for (UINT32 u32=0; u32 < 20; u32++)
	{
		UINT32 u32OutputFrames = apoProcess->CalcOutputFrames(u32MaxInputFrames);
		METHODCHECKRESULT(TEXT("Reset"), hr, m_apoObject->Reset());
		ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

		InputConnection.u32ValidFrameCount = u32MaxInputFrames;
		apoProcess->APOProcess(1, &pInputConnection, 1, &pOutputConnection); 			
	}
	END_ASSERTION();
*/
CLEANUP:
	if (pfOutputBuffer) {
		VirtualFree((LPBYTE)pfOutputBuffer - u32OutputOffset, 0, MEM_RELEASE);
	}
	if (pfInputBuffer) {
		VirtualFree((LPBYTE)pfInputBuffer - u32InputOffset, 0, MEM_RELEASE);
	}

	_RELEASE_(InMediaType);
	_RELEASE_(OutMediaType);

    return HRESULT2SHELL(hr);
}

// ***********************************************************************************************
// BEGIN SHELL98 methods
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

DWORD Test_SRCAPO_InitializeInvalid()
{
    DWORD dwResult;

    BEGIN_TRY()
    CSRCAPOAPITest Test;
    dwResult = Test.InitializeInvalid();
    END_TRY(dwResult)

    return dwResult;
}

DWORD Test_SRCAPO_CalculateFrames()
{
    DWORD dwResult;

    BEGIN_TRY()
    CSRCAPOAPITest Test;
    dwResult = Test.CalculateFrames();
    END_TRY(dwResult)

    return dwResult;
}

DWORD Test_SRCAPO_CalcOutputFramesStateTesting()
{
    DWORD dwResult;

    BEGIN_TRY()
    CSRCAPOAPITest Test;
    dwResult = Test.CalcOutputFramesStateTesting();
    END_TRY(dwResult)

    return dwResult;
}

DWORD Test_SRCAPO_CalcInputFramesStateTesting()
{
    DWORD dwResult;

    BEGIN_TRY()
    CSRCAPOAPITest Test;
    dwResult = Test.CalcInputFramesStateTesting();
    END_TRY(dwResult)

    return dwResult;
}
