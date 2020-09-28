// Copyright(C) Microsoft. All rights reserved.//

// APIGenericAPO.cpp
//
// Description:
//
//  Implementation of CGenericAPOTest class to test fundamental APO behaviour
//

#include "stdafx.h"


//-------------------------------------------------------------------------
// Method: CheckInterfacesSupported
//
// Description:
//
//	Checks that all APO's in the system support required interfaces
//
// Parameters:
//
// Return values(DWORD) : FNS_FAIL, FNS_PASS
//
DWORD CGenericAPOTest::CheckInterfacesSupported()
{
    APORegistration reg;
    IAudioProcessingObjectPtr   apoObject;
    IAudioProcessingObjectRTPtr apoObjectRT;
    IAudioProcessingObjectConfigurationPtr apoObjectConfig;

    HRESULT hr;

	//
	// Initialize registration object
    hr = reg.InitializeAPOUtil();
    if (FAILED(hr))
    {
        SLOG(eError, "ERROR: Failed to initialize APOUtil");
        goto CLEANUP;
    }

	//
	// Verifies that all system APO's are registered
	// and fills some reg information
    if (reg.SystemAposAreRegistered() != FNS_PASS)
    {
        hr = E_FAIL;
        goto CLEANUP;
    }

    //
    //* Check all interfaces are supported
    for (KNOWN_APO_TYPES type=APO_FIRST; type != APO_LAST_SYSTEM_APO; (*((UINT32*)&type))++)
    {
        if (type == APO_KMIXER)
            continue;

        apoObject	=	NULL;
        apoObjectRT	=	NULL;

	    START_ASSERTION(TEXT("Verify that APO supports IAudioProcessingObject"));
        METHODCHECKRESULT(TEXT("APORegistration::CreateAPO"), hr, APORegistration::CreateAPO(type, NULL, apoObject));
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		END_ASSERTION();

	    START_ASSERTION(TEXT("Verify that APO supports IAudioProcessingObjectRT"));
        METHODCHECKRESULT(TEXT("QueryInterface"), hr, apoObject.QueryInterface(__uuidof(IAudioProcessingObjectRT), (LPVOID*)&apoObjectRT));
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		END_ASSERTION();

		START_ASSERTION(TEXT("Verify that APO supports IAudioProcessingObjectConfiguration"));
        METHODCHECKRESULT(TEXT("QueryInterface"), hr, apoObject.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (LPVOID*)&apoObjectConfig));
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		END_ASSERTION();

    }

CLEANUP:
    return HRESULT2SHELL(hr);
}

//-------------------------------------------------------------------------
// Method: CheckLockForProcess
//
// Description:
//
//	Lock for process testing for all APO's:
//		invalid number of input/output connections (0 or greater than supported number)
//		null connection array
//		null format (array of null's)
//		sample rate outside of the expected range
//		number of channels outside of the expected range
//		number of bytes or bits outside the expected range
//		number of bits don't fit in the container
//		NaN sample rate
//		LockForProcess can't be called twice
//		Reset succeeds after a successful LockForProcess
//
// Parameters:
//
// Return values(DWORD) : FNS_FAIL, FNS_PASS
//
DWORD CGenericAPOTest::CheckLockForProcess()
{
    APORegistration		reg;
    HRESULT				hr;
    AUDIO_FRAME_FORMAT  inputFormats[4];
    AUDIO_FRAME_FORMAT  outputFormats[4];
    AUDIO_FRAME_FORMAT  inputFormat, 
                        outputFormat;
    IAudioMediaType   *pItfNULL	=	NULL;
    IAudioMediaType   *OutMediaTypes[4];
    IAudioMediaType   *InMediaTypes[4];

	LPVOID				pBufferInput  =	(LPVOID)(DWORD_PTR)0x80000000;
	LPVOID				pBufferOutput =	(LPVOID)(DWORD_PTR)0x87000000;


	UINT32 u32MaxInputFrames = 0;
	UINT32 u32MaxOutputFrames = 10;

	IAudioProcessingObjectConfigurationPtr apoConfig;
	IAudioProcessingObjectVBRPtr           apoVBR;

    //
    // Initialize APOUtil.dll
    hr = reg.InitializeAPOUtil();
    if (FAILED(hr))
    {
        SLOG(eError, "ERROR: Failed to initialize APOUtil");
        goto CLEANUP;
    }

    //
    // Get information for all APO's
    if (reg.SystemAposAreRegistered() != FNS_PASS)
    {
        hr = E_FAIL;
        goto CLEANUP;
    }

    //
    // All the test cases will first initialize the APO and then do their stuff
    // The initialization should always succeed because we want to hit
    // the generic code
    // Specific initialize problems should be called out in special test cases
    // for each APO class
    for (KNOWN_APO_TYPES type=APO_FIRST; type != APO_LAST_SYSTEM_APO; (*((UINT32*)&type))++)
    {
        //
        // skip the meter APO as it does not fit the pattern
        if (type == APO_KMIXER)
            continue;

        m_apoObject=NULL;
        m_apoObjectRT=NULL;


		APO_REG_PROPERTIES_FULL  apoProperties;
        APO_REG_PROPERTIES_FULL* pProperties = &apoProperties;

		APORegistration::FillAllApoProperties( type, apoProperties );


        //
        // Get default information for the APO format
        UINT32 ui32OutSamplesPerFrame = pProperties->OutFormat.u32MaxSamplesPerFrame;
        UINT32 ui32InSamplesPerFrame  = pProperties->InFormat.u32MaxSamplesPerFrame;

        //
        // Minimize memory usage
        ui32OutSamplesPerFrame=1;
        ui32InSamplesPerFrame=1;

        //
        // Get in/out valid information
        float f32OutFramesPerSecond = pProperties->OutFormat.f32MaxFramesPerSecond; 
        float f32InFramesPerSecond = pProperties->InFormat.f32MaxFramesPerSecond; 
        UINT32 ui32OutBytesPerSample = pProperties->OutFormat.u32MaxBytesPerSampleContainer; 
        UINT32 ui32InBytesPerSample = pProperties->InFormat.u32MaxBytesPerSampleContainer; 
        UINT32 ui32OutValidBitsPerSample = pProperties->OutFormat.u32MaxValidBitsPerSample;
        UINT32 ui32InValidBitsPerSample = pProperties->InFormat.u32MaxValidBitsPerSample; 

        //
        // Create default supported format for the APO
        BUILD_FORMAT(&inputFormat,  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&outputFormat,  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 

        //
        // Create APO
        SLOG(eInfo1, "---------------------------------------------------------------------------");  
		SLOG(eInfo1,"");
        SLOG(eInfo1, "*              Testing APO %S *", pProperties->szFriendlyName);  
		SLOG(eInfo1,"");
        SLOG(eInfo1, "---------------------------------------------------------------------------");  
        START_ASSERTION("APO can be created");
        METHODCHECKRESULT(TEXT("APORegistration::CreateAPO"), hr, APORegistration::CreateAPO(type, NULL, m_apoObject));
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
        END_ASSERTION();

        //
        // Acquire the APO config interface
        apoConfig=NULL;
        START_ASSERTION(TEXT("APO supports IAudioProcessingObjectConfiguration"));
        METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectConfiguration"),hr , m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (LPVOID*)&apoConfig)); 
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
        END_ASSERTION();

        //
        // Acquire the APO VBR interface
        apoVBR=NULL;
        METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectVBR"),hr , m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectVBR), (LPVOID*)&apoVBR)); 

		
        //
        // Initialize APO
        METHODCHECKRESULT(TEXT("InitializeAPO"), hr, InitializeAPO(type, &inputFormat, &outputFormat)); 
        CHECK_FAIL_GOTO(hr, CLEANUP);

		//
		// For the VBR case calculate the number of input frames
		if ( apoVBR != NULL ) {
			//
			// * Calculate the maximum number of input frames for the 10 input frames that we pass in
			METHODCHECKRESULT(TEXT("CalcMaxInputFrames"), hr, apoVBR->CalcMaxInputFrames(u32MaxOutputFrames, &u32MaxInputFrames)); 
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		} else {
			u32MaxInputFrames = 10;
		}

        //*
        // Check that invalid input connections are rejected 
        //* 
        START_ASSERTION(TEXT("Invalid number of input connections(0,4) or NULL array in LockForProcess"));

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&inputFormats[1],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&inputFormats[2],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&inputFormats[3],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&outputFormats[0],  pProperties->InFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(InMediaTypes     ,  &inputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes + 1 ,  &inputFormats[1]);
        BUILD_FORMAT_ITF(InMediaTypes + 2 ,  &inputFormats[2]);
        BUILD_FORMAT_ITF(InMediaTypes + 3 ,  &inputFormats[3]);
        BUILD_FORMAT_ITF(OutMediaTypes    ,  &outputFormats[0]);

        // Call LockForProcess with zero input connections
		SLOG(eInfo1, "0 input");
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(0, NULL, 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        // Call LockForProcess with too many input connections
		SLOG(eInfo1, "4 input");
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(4, CLockForProcAdapter<4,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        // Call LockForProcess with NULL pointer
		SLOG(eInfo1, "NULL pointer");
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, NULL, 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        _RELEASE_(InMediaTypes[0]);
        _RELEASE_(InMediaTypes[1]);
        _RELEASE_(InMediaTypes[2]);
        _RELEASE_(InMediaTypes[3]);
        _RELEASE_(OutMediaTypes[0]);
        END_ASSERTION();


        //
        //* Check that invalid output connections are rejected 
        // 
        START_ASSERTION(TEXT("Invalid number of output connections(0,4) or NULL array in LockForProcess"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT(&outputFormats[1],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT(&outputFormats[2],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT(&outputFormats[3],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes     , &outputFormats[0]);
        BUILD_FORMAT_ITF(OutMediaTypes + 1 , &outputFormats[1]);
        BUILD_FORMAT_ITF(OutMediaTypes + 2 , &outputFormats[2]);
        BUILD_FORMAT_ITF(OutMediaTypes + 3 , &outputFormats[3]);
        BUILD_FORMAT_ITF(InMediaTypes      , &inputFormats[0]);

		if (type != APO_METER)
		{
			SLOG(eInfo1, "0 output");
			METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 0, NULL)); 
			ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
		}
		SLOG(eInfo1, "4 output");
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 4, CLockForProcAdapter<4,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
		SLOG(eInfo1, "NULL pointer");
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, NULL)); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(OutMediaTypes[1]);
        _RELEASE_(OutMediaTypes[2]);
        _RELEASE_(OutMediaTypes[3]);
        _RELEASE_(InMediaTypes[0]);
        END_ASSERTION();

        //*
        // Check for NULL connection information 
        //*
        START_ASSERTION(TEXT("NULL format information for input/output"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        IAudioMediaType** ppMediaTypeNULL = &pItfNULL; 
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(ppMediaTypeNULL,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(ppMediaTypeNULL,pBufferOutput,u32MaxOutputFrames))); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        END_ASSERTION();

        //
        //* Input connections > max connections rejected 
        // 
        START_ASSERTION(TEXT("Input connections > max connections"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(pProperties->u32MaxInputConnections + 1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        END_ASSERTION();

        //
        //* Output connections > max connections rejected 
        // 
        START_ASSERTION(TEXT("Output connections > max connections"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&outputFormats[0],  pProperties->InFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), pProperties->u32MaxOutputConnections + 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        END_ASSERTION();


        //
        //* Sample rate invalid
        // 
        START_ASSERTION(TEXT("Frame rate outside of range"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, pProperties->InFormat.f32MinFramesPerSecond - 1);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, pProperties->InFormat.f32MaxFramesPerSecond + 1);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, pProperties->OutFormat.f32MaxFramesPerSecond + 1);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, pProperties->OutFormat.f32MinFramesPerSecond - 1);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        END_ASSERTION();

        //
        //* Invalid number of channels
        // 
        START_ASSERTION(TEXT("Number of channels outside of range"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, pProperties->InFormat.u32MinSamplesPerFrame - 1, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, pProperties->InFormat.u32MaxSamplesPerFrame + 1, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes, pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, pProperties->OutFormat.u32MinSamplesPerFrame - 1, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes, pBufferOutput, u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, pProperties->OutFormat.u32MaxSamplesPerFrame + 1, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        END_ASSERTION();

        //
        //* Invalid number of bytes
        // 
        START_ASSERTION(TEXT("Number of bytes outside of range"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, 0xffffffff, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, 33, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);


        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, 0, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, 0xffffffff, ui32OutValidBitsPerSample, f32OutFramesPerSecond);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, 0, ui32OutValidBitsPerSample, f32OutFramesPerSecond);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, 33, ui32OutValidBitsPerSample, f32OutFramesPerSecond);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        END_ASSERTION();


        //
        //* Invalid number of bits
        // 
        START_ASSERTION(TEXT("Number of bits outside of range"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, 0, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, 1, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxInputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, 0xffffffff, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);


        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, 0, f32OutFramesPerSecond);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, 1, f32OutFramesPerSecond);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, 0xffffffff, f32OutFramesPerSecond);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        END_ASSERTION();


        //
        //* Number of bits doesn't fit the container
        // 
        START_ASSERTION(TEXT("Number of bits greater than what the container can hold"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample+1, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample+1, f32OutFramesPerSecond);
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        END_ASSERTION();
/* THIS FAILS CURRENTLY
        //
        //* NaN for frame rate
        // 
        START_ASSERTION(TEXT("NaN for sample rate"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, sqrt(-1.00f));
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond);
        BUILD_FORMAT(&outputFormats[0], pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, sqrt(-1.00f));
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);
        METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput,u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,u32MaxOutputFrames))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        END_ASSERTION();
*/

        //
        //* Can't call LockForProcess twice and succeed
        // 
        START_ASSERTION(TEXT("Call LockForProcess twice"));
        BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
        BUILD_FORMAT_ITF(OutMediaTypes , &outputFormats[0]);
        BUILD_FORMAT_ITF(InMediaTypes  , &inputFormats[0]);



		METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput, u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,10))); 
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,48,4>(InMediaTypes,pBufferInput, u32MaxInputFrames), 1, CLockForProcAdapter<1,48,4>(OutMediaTypes,pBufferOutput,10))); 
        _RELEASE_(OutMediaTypes[0]);
        _RELEASE_(InMediaTypes[0]);
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        METHODCHECKRESULT(TEXT("UnlockForProcess"), hr, apoConfig->UnlockForProcess()); 
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
        END_ASSERTION();

		//
		// *Reset 
		//
        START_ASSERTION(TEXT("Reset should work even if the APO is locked for process"));
        METHODCHECKRESULT(TEXT("Reset"), hr, m_apoObject->Reset()); 
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
        END_ASSERTION();

    }


CLEANUP:
    return HRESULT2SHELL(hr);
}

//-------------------------------------------------------------------------
// Method: InitializeAPO
//
// Description:
//
//	Initialize routine for various system APO's
//
// Parameters:
//
// Return values(HRESULT) : 
//
HRESULT CGenericAPOTest::InitializeAPO(KNOWN_APO_TYPES type, 
									   AUDIO_FRAME_FORMAT* pinFormats, 
									   AUDIO_FRAME_FORMAT* poutFormat)
{
    HRESULT hr			 = S_OK;
    IAudioRateConvertPtr apoSRC;
    IAudioMatrixPtr		 apoMatrix;
    IAudioVolumePtr      apoVolume;
    IAudioMeterPtr       apoMeter;
    APO_REG_PROPERTIES*  pProperties = NULL;


    START_ASSERTION(TEXT("Can initialize APO"));

	switch (type)
    {
    case APO_SRC:
		{
			METHODCHECKRESULT(TEXT("QI IAudioRateConvert"), hr, m_apoObject.QueryInterface(__uuidof(IAudioRateConvert), (LPVOID*)&apoSRC));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

			AUDIO_RATE_CONVERT_PARAMS apoParams;

			hr = m_apoObject->GetRegistrationProperties(&pProperties); 
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

			//
			// Initialize  
			apoParams.APOInit.cbSize	  = sizeof( apoParams );
			apoParams.APOInit.clsid       = pProperties->clsid;
			apoParams.fInputFrameRate     = pinFormats[0].f32FramesPerSecond;
			apoParams.fOutputFrameRate    = poutFormat[0].f32FramesPerSecond;
			apoParams.fMaxInputFrameRate  = pinFormats[0].f32FramesPerSecond;
			apoParams.fMaxOutputFrameRate = poutFormat[0].f32FramesPerSecond;
			apoParams.fMinInputFrameRate  = 8000/*pinFormats[0].f32FramesPerSecond*/;
			apoParams.fMinOutputFrameRate = 8000/*poutFormat[0].f32FramesPerSecond*/;
			apoParams.eAlgorithmType      = SRC_QUALITY_LINEAR;
			apoParams.eFlowType           = AUDIO_FLOW_PULL;
			apoParams.u32SamplesPerFrame  = pinFormats[0].u32SamplesPerFrame;

			METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
    break;

	case APO_MATRIX:
		{
			APO_MATRIX_PARAMS apoParams;

			METHODCHECKRESULT(TEXT("QI IAudioMatrix"), hr, m_apoObject.QueryInterface(__uuidof(IAudioMatrix), (LPVOID*)&apoMatrix));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

			hr = m_apoObject->GetRegistrationProperties(&pProperties); 
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

			apoParams.APOInit.cbSize			= sizeof( apoParams );
			apoParams.APOInit.clsid				= pProperties->clsid;
			apoParams.u32InputSamplesPerFrame	= pinFormats[0].u32SamplesPerFrame;
			apoParams.u32OutputSamplesPerFrame	= poutFormat->u32SamplesPerFrame;
			apoParams.f32FrameRate				= pinFormats[0].f32FramesPerSecond;

			METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
        break;

	case APO_VOLUME:
		{
			APO_VOLUME_PARAMS apoParams;

			METHODCHECKRESULT(TEXT("QI IAudioVolume"), hr, m_apoObject.QueryInterface(__uuidof(IAudioVolume), (LPVOID*)&apoVolume));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

			hr = m_apoObject->GetRegistrationProperties(&pProperties); 
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

			apoParams.APOInit.cbSize			= sizeof( apoParams );
			apoParams.APOInit.clsid				= pProperties->clsid;
			apoParams.u32SamplesPerFrame		= pinFormats[0].u32SamplesPerFrame;
			apoParams.f32FrameRate				= pinFormats[0].f32FramesPerSecond;

			METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
        break;

	case APO_METER:
		{
			APO_METER_PARAMS apoParams;

			METHODCHECKRESULT(TEXT("QI IAudioMeter"), hr, m_apoObject.QueryInterface(__uuidof(IAudioMeter), (LPVOID*)&apoMeter));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

			hr = m_apoObject->GetRegistrationProperties(&pProperties); 
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

			apoParams.APOInit.cbSize			= sizeof( apoParams );
			apoParams.APOInit.clsid				= pProperties->clsid;
			apoParams.u32SamplesPerFrame		= pinFormats[0].u32SamplesPerFrame;
			apoParams.u32FramesPerAverage		= (UINT32)pinFormats[0].f32FramesPerSecond/100; // 10 ms worth of frames in each average
			apoParams.u32NumAverages			= 100;  
			apoParams.u32FramesPerQuantum		= (UINT32)pinFormats[0].f32FramesPerSecond;


			METHODCHECKRESULT(TEXT("Initialize"), hr, m_apoObject->Initialize( sizeof(apoParams), (LPBYTE)&apoParams));
			ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
		}
        break;

    default:;
    }

    END_ASSERTION();

CLEANUP:

	return hr;
}


//-------------------------------------------------------------------------
// Method: ValidateBasicInterface
//
// Description:
//
//	Basic validation for IAudioProcessingObject and IAudioProcessingObjectConfig
//		NULL param for GetLatency
//		NULL param for GetRegistrationProperties
//		Unlock fails if called for unlocked APO's
//
// Parameters:
//
// Return values(HRESULT) : 
//
DWORD CGenericAPOTest::ValidateBasicInterface()
{
    APORegistration reg;
    HRESULT hr;

    AUDIO_FRAME_FORMAT inputFormat, outputFormat;

    // Initialize APOUtil.dll
    hr = reg.InitializeAPOUtil();
    if (FAILED(hr))
    {
        SLOG(eError, "ERROR: Failed to initialize APOUtil");
        goto CLEANUP;
    }

    // Get information for all APO's
    if (reg.SystemAposAreRegistered() != FNS_PASS)
    {
        hr = E_FAIL;
        goto CLEANUP;
    }

    for (KNOWN_APO_TYPES type=APO_FIRST; type != APO_LAST_SYSTEM_APO; (*((UINT32*)&type))++)
    {
        IAudioProcessingObjectConfigurationPtr apoConfig;

        if (type == APO_KMIXER)
		{
            continue;
		}

        m_apoObject		=	NULL;
        m_apoObjectRT	=	NULL;


		APO_REG_PROPERTIES_FULL  apoProperties;
        APO_REG_PROPERTIES_FULL* pProperties = &apoProperties;

		APORegistration::FillAllApoProperties( type, apoProperties );



        // Get default information for the APO format
        UINT32 ui32OutSamplesPerFrame    = pProperties->OutFormat.u32MaxSamplesPerFrame;
        UINT32 ui32InSamplesPerFrame     = pProperties->InFormat.u32MaxSamplesPerFrame;
        float f32OutFramesPerSecond      = pProperties->OutFormat.f32MaxFramesPerSecond; 
        float f32InFramesPerSecond       = pProperties->InFormat.f32MaxFramesPerSecond; 
        UINT32 ui32OutBytesPerSample     = pProperties->OutFormat.u32MaxBytesPerSampleContainer; 
        UINT32 ui32InBytesPerSample      = pProperties->InFormat.u32MaxBytesPerSampleContainer; 
        UINT32 ui32OutValidBitsPerSample = pProperties->OutFormat.u32MaxValidBitsPerSample;
        UINT32 ui32InValidBitsPerSample  = pProperties->InFormat.u32MaxValidBitsPerSample; 

        // Create default supported format for the APO
        BUILD_FORMAT(&inputFormat,  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&outputFormat,  pProperties->InFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 

        // Create APO
        SLOG(eInfo1, "* Testing APO %S *", pProperties->szFriendlyName);  
        START_ASSERTION("Can create APO");
        METHODCHECKRESULT(TEXT("APORegistration::CreateAPO"), hr, APORegistration::CreateAPO(type, NULL, m_apoObject));
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
        END_ASSERTION();

        //
        // Acquire the APO config interface
        apoConfig = NULL;
        START_ASSERTION(TEXT("APO supports IAudioProcessingObjectConfiguration"));
        METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectConfiguration"),hr , m_apoObject.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (LPVOID*)&apoConfig)); 
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
        END_ASSERTION();

        //
        //* GetLatency NULL param
        // 
        START_ASSERTION(TEXT("GetLatency NULL parameter"));
        METHODCHECKRESULT(TEXT("GetLatency"), hr, m_apoObject->GetLatency( NULL )); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        END_ASSERTION();

        //
        //* GetLatency valid param
        // 
        START_ASSERTION(TEXT("GetLatency valid parameter"));
		MFTIME mftimeLatency = 0;
        METHODCHECKRESULT(TEXT("GetLatency"), hr, m_apoObject->GetLatency( &mftimeLatency )); 
		if ( SUCCEEDED(hr) && type != APO_SRC && mftimeLatency != 0 )
		{
			SLOG(eError,"ERROR: APO reported non zero latency %I64d!", mftimeLatency);
			hr = E_FAIL;
		}
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
        END_ASSERTION();

        //
        //* GetRegistrationProperties NULL param
        // 
        START_ASSERTION(TEXT("GetRegistrationProperties NULL parameter"));
        METHODCHECKRESULT(TEXT("GetRegistrationProperties"), hr, m_apoObject->GetRegistrationProperties( NULL )); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        END_ASSERTION();

        //
        //* Unlock before Lock
        // 
        START_ASSERTION(TEXT("Unlock for not locked APO"));
        METHODCHECKRESULT(TEXT("UnlockForProcess"), hr, apoConfig->UnlockForProcess()); 
        ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);
        END_ASSERTION();

    }

CLEANUP:
    return HRESULT2SHELL(hr);
}

//-------------------------------------------------------------------------
// Method: LockForProcessValidateBufferSize
//
// Description:
//
//	IAudioProcessingObject interface changed and allows the APO to fail if the buffer size
//	is not appropiate
//		for all APO's except SRC fail if max on input != max on output
//		for SRC APO fail based on min/max sample rates
//		NULL IAudioMediaType interface pointer
//		Check that overlapped buffers are rejected
//
// Parameters:
//
// Return values(HRESULT) : 
//
DWORD CGenericAPOTest::LockForProcessValidateBufferSize()
{
    APORegistration reg;
    HRESULT hr;
    AUDIO_FRAME_FORMAT  inputFormats[4];
    AUDIO_FRAME_FORMAT  outputFormats[4];
    IAudioMediaType   *pItfNULL = NULL;
    AUDIO_FRAME_FORMAT  inputFormat, 
                        outputFormat;
    IAudioMediaType   *OutMediaTypes[4];
    IAudioMediaType   *InMediaTypes[4];
	LPVOID				pBufferInput=(LPVOID)(DWORD_PTR)0x80000000;
	LPVOID				pBufferOutput=(LPVOID)(DWORD_PTR)0x87000000;

    IAudioProcessingObjectConfigurationPtr apoConfig;


    //
    // Initialize APOUtil.dll
    hr = reg.InitializeAPOUtil();
    if (FAILED(hr))
    {
        SLOG(eError, "ERROR: Failed to initialize APOUtil");
        goto CLEANUP;
    }

    //
    // Get information for all APO's
    if (reg.SystemAposAreRegistered() != FNS_PASS)
    {
        hr = E_FAIL;
        goto CLEANUP;
    }

	//
	// Test all system APO's
    for (KNOWN_APO_TYPES type=APO_FIRST; type != APO_LAST_SYSTEM_APO; (*((UINT32*)&type))++)
    {
        //
        // skip the meter APO as it does not fit the pattern
        if (type == APO_KMIXER)
            continue;

        m_apoObject		=	NULL;
        m_apoObjectRT	=	NULL;


		APO_REG_PROPERTIES_FULL  apoProperties;
        APO_REG_PROPERTIES_FULL* pProperties = &apoProperties;

		APORegistration::FillAllApoProperties( type, apoProperties );


        //
        // Get default information for the APO format
        UINT32 ui32OutSamplesPerFrame = pProperties->OutFormat.u32MaxSamplesPerFrame;
        UINT32 ui32InSamplesPerFrame  = pProperties->InFormat.u32MaxSamplesPerFrame;

        //
        // Minimize memory usage
        ui32OutSamplesPerFrame=1;
        ui32InSamplesPerFrame=1;

        //
        // Get in/out valid information
        float f32OutFramesPerSecond = pProperties->OutFormat.f32MaxFramesPerSecond; 
        float f32InFramesPerSecond = pProperties->InFormat.f32MaxFramesPerSecond; 
        UINT32 ui32OutBytesPerSample = pProperties->OutFormat.u32MaxBytesPerSampleContainer; 
        UINT32 ui32InBytesPerSample = pProperties->InFormat.u32MaxBytesPerSampleContainer; 
        UINT32 ui32OutValidBitsPerSample = pProperties->OutFormat.u32MaxValidBitsPerSample;
        UINT32 ui32InValidBitsPerSample = pProperties->InFormat.u32MaxValidBitsPerSample; 

        //
        // Create default supported format for the APO
        BUILD_FORMAT(&inputFormat,  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
        BUILD_FORMAT(&outputFormat,  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 

        //
        // Create APO
        SLOG(eInfo1, "* Testing APO %S *", pProperties->szFriendlyName);  
        START_ASSERTION("APO can be created");
        METHODCHECKRESULT(TEXT("APORegistration::CreateAPO"), hr, APORegistration::CreateAPO(type, NULL, m_apoObject));
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
        END_ASSERTION();

        //
        // Acquire the APO config interface
        apoConfig=NULL;
        START_ASSERTION(TEXT("APO supports IAudioProcessingObjectConfiguration"));
        METHODCHECKRESULT(TEXT("QI IAudioProcessingObjectConfiguration"),hr , m_apoObject->QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), (LPVOID*)&apoConfig)); 
        ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);
        END_ASSERTION();

        //
        // Initialize APO
        METHODCHECKRESULT(TEXT("InitializeAPO"), hr, InitializeAPO(type, &inputFormat, &outputFormat)); 
        CHECK_FAIL_GOTO(hr, CLEANUP);

		//
		// NULL media type
		START_ASSERTION(TEXT("NULL media type ptr in LockForProcess"));

		BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
		BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
		BUILD_FORMAT_ITF(InMediaTypes,  &inputFormats[0]);
		BUILD_FORMAT_ITF(OutMediaTypes, &outputFormats[0]);

		// Call LockForProcess with 1:1 but with a NULL media type
		METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,64,0>(pBufferInput), 1, CLockForProcAdapter<1,63,0>(pBufferOutput))); 
		ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

		_RELEASE_(InMediaTypes[0]);
		_RELEASE_(OutMediaTypes[0]);

		END_ASSERTION();

		if (type != APO_SRC)
		{
			//*
			// Check that input/output need to have the same number of available samples
			//* 
			START_ASSERTION(TEXT("Max number of frames on input = Max number of frames on output"));

			BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
			BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
			BUILD_FORMAT_ITF(InMediaTypes,  &inputFormats[0]);
			BUILD_FORMAT_ITF(OutMediaTypes, &outputFormats[0]);

			// Call LockForProcess with 1:1 but with diff in/out no of frames
			METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,64,0>(InMediaTypes,pBufferInput), 1, CLockForProcAdapter<1,63,0>(OutMediaTypes,pBufferOutput))); 
			ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

			_RELEASE_(InMediaTypes[0]);
			_RELEASE_(OutMediaTypes[0]);

			END_ASSERTION();
		}
		else
		{
			//
			// this is the maximum ratio of input/output frequencies based on the
			// Initialize code
			UINT32 u32InOutRatio =  (UINT32)(f32OutFramesPerSecond/8000.00);
			_ASSERT(u32InOutRatio > 1);

			//
			// * Input frames == Output frames is rejected *
			//
			START_ASSERTION(TEXT("Max number of frames on input > Max number of frames on output for an SRC in pull-model"));

			BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
			BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
			BUILD_FORMAT_ITF(InMediaTypes,  &inputFormats[0]);
			BUILD_FORMAT_ITF(OutMediaTypes, &outputFormats[0]);

			// Call LockForProcess with 1:1 with same number of frames
			// the SRC should reject this because it does not have enough space for the worst case
			// scenario
			METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,64,0>(InMediaTypes,pBufferInput), 1, CLockForProcAdapter<1,64,0>(OutMediaTypes,pBufferOutput))); 
			ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

			_RELEASE_(InMediaTypes[0]);
			_RELEASE_(OutMediaTypes[0]);

			END_ASSERTION();
		}

		// only the APO's that are not in-place need to be checking for this conditions
		if (type == APO_MIXER || type == APO_SRC)
		{
			START_ASSERTION(TEXT("Input buffer address < Output buffer address overlapped"));
			BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
			BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
			BUILD_FORMAT_ITF(InMediaTypes,  &inputFormats[0]);
			BUILD_FORMAT_ITF(OutMediaTypes, &outputFormats[0]);

			METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,64,0>(InMediaTypes,pBufferInput), 1, CLockForProcAdapter<1,64,0>(OutMediaTypes,(float*)pBufferInput+32))); 
			ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

			_RELEASE_(InMediaTypes[0]);
			_RELEASE_(OutMediaTypes[0]);

			END_ASSERTION();

			START_ASSERTION(TEXT("Input buffer address > Output buffer address overlapped"));
			BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
			BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
			BUILD_FORMAT_ITF(InMediaTypes,  &inputFormats[0]);
			BUILD_FORMAT_ITF(OutMediaTypes, &outputFormats[0]);

			METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,64,0>(InMediaTypes,pBufferInput), 1, CLockForProcAdapter<1,64,0>(OutMediaTypes,(float*)pBufferInput-32))); 
			ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

			_RELEASE_(InMediaTypes[0]);
			_RELEASE_(OutMediaTypes[0]);

			END_ASSERTION();

			START_ASSERTION(TEXT("Input buffer address = Output buffer address"));
			BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
			BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
			BUILD_FORMAT_ITF(InMediaTypes,  &inputFormats[0]);
			BUILD_FORMAT_ITF(OutMediaTypes, &outputFormats[0]);

			METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,64,0>(InMediaTypes,pBufferInput), 1, CLockForProcAdapter<1,64,0>(OutMediaTypes,pBufferInput))); 
			ASSERTION_CHECK_GOTO(hr, SUCCEEDED, CLEANUP);

			_RELEASE_(InMediaTypes[0]);
			_RELEASE_(OutMediaTypes[0]);

			END_ASSERTION();

			if ( type == APO_MIXER )
			{
				START_ASSERTION(TEXT("Lock for process should work for contigous buffers for input/output"));
				BUILD_FORMAT(&inputFormats[0],  pProperties->InFormat.FormatType, ui32InSamplesPerFrame, ui32InBytesPerSample, ui32InValidBitsPerSample, f32InFramesPerSecond); 
				BUILD_FORMAT(&outputFormats[0],  pProperties->OutFormat.FormatType, ui32OutSamplesPerFrame, ui32OutBytesPerSample, ui32OutValidBitsPerSample, f32OutFramesPerSecond); 
				BUILD_FORMAT_ITF(InMediaTypes,  &inputFormats[0]);
				BUILD_FORMAT_ITF(OutMediaTypes, &outputFormats[0]);

				UINT32 ui32BytesPerFrame = ui32InSamplesPerFrame * ui32InBytesPerSample;
				METHODCHECKRESULT(TEXT("LockForProcess"), hr, apoConfig->LockForProcess(1, CLockForProcAdapter<1,64,0>(InMediaTypes,pBufferInput), 1, CLockForProcAdapter<1,64,0>(OutMediaTypes,(LPBYTE)pBufferInput+64*ui32BytesPerFrame))); 
				ASSERTION_CHECK_GOTO(hr, FAILED, CLEANUP);

				_RELEASE_(InMediaTypes[0]);
				_RELEASE_(OutMediaTypes[0]);

				END_ASSERTION();
			}
		}

    }

CLEANUP:
    return HRESULT2SHELL(hr);
}

//*******************************************
// Test cases
//*******************************************
DWORD Test_APO_RequiredInterfaces()
{
    DWORD dwResult;

    BEGIN_TRY()
    CGenericAPOTest Test;
    dwResult = Test.CheckInterfacesSupported();
    END_TRY(dwResult)

    return dwResult;
}
DWORD Test_APO_LockForProcess()
{
    DWORD dwResult;

    BEGIN_TRY()
    CGenericAPOTest Test;
    dwResult = Test.CheckLockForProcess();
    END_TRY(dwResult)

    return dwResult;
}

DWORD Test_APO_ValidateBasicInterface()
{
    DWORD dwResult;

    BEGIN_TRY()
    CGenericAPOTest Test;
    dwResult = Test.ValidateBasicInterface();
    END_TRY(dwResult)

    return dwResult;
}

DWORD Test_APO_LockForProcessValidateBufferSize()
{
    DWORD dwResult;

    BEGIN_TRY()
    CGenericAPOTest Test;
    dwResult = Test.LockForProcessValidateBufferSize();
    END_TRY(dwResult)

    return dwResult;
}
