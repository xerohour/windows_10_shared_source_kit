// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"
#include <kaiser.h>
//#include <mfrate.h>
//#include <wmcodecdsp.h>

#define MAX_RATES 16

//
// TODO: figure out why the 2 channel doesn't work for KMixer and large frequencies!!
// Weird!
DWORD Test_FixedFormatConvertor_SNR32(SRC_ALGORITHM_TYPE SrcType, 
									  UINT nChannels = 1, 
									  AUDIO_FLOW_TYPE SrcFlow = AUDIO_FLOW_PUSH, 
									  BOOL bUseVariableFrequency = false)
{
    SLOG(eInfo1, "Create a graph Mem-->32-->Mixer-->SRC-->-->32-->Mem");
    SLOG(eInfo1, "The test determines the SNR of the converter for %s quality %ld channels and flow %s (24 bit)", 
					(SrcType==SRC_QUALITY_LINEAR) ? "LINEAR" : "OTHER",
					 nChannels,
					(SrcFlow==AUDIO_FLOW_PUSH) ? "PUSH" : "PULL");
 
    //
    // Input/output formats
    AUDIO_FRAME_FORMAT InputFormat, OutputFormat;


    //
    // Structure used for test pairs
    typedef struct _SRPAIR
    {
        float  finputSR, foutputSR, fFrequency;
        double dSNR;
    } SRPAIR;

    UINT32 u,v;

    //
    // Initialize test matrix
    float aFrequencies[] = { 8000, 15999, 16000, 11025, 22050, 31999, 32000, 44100, 48000, 96000 }; 
    UINT32 u32MatrixDimension = sizeof(aFrequencies)/sizeof(aFrequencies[0]);
    SRPAIR  aTestMatrix[MAX_RATES][MAX_RATES];
	
	//
	// For stereo mode we only test a restricted number of conversion
	if (nChannels > 1)
		u32MatrixDimension = 2;

	//
	// Fill the test matrix
    for (u=0; u < u32MatrixDimension; u++)
        for (v=0; v < u32MatrixDimension; v++)
        {
            aTestMatrix[u][v].finputSR  = aFrequencies[u];
            aTestMatrix[u][v].foutputSR = aFrequencies[v];
			if (bUseVariableFrequency)
			{
				// always use a frequency near input signal Nyquist
				// note that if we downsample we expect the SNR to be zero
				// otherwise as good as possible
	            aTestMatrix[u][v].fFrequency= min(20000, min(aTestMatrix[u][v].finputSR/2,aTestMatrix[u][v].foutputSR/2)) - 1000;
			}
			else
	            aTestMatrix[u][v].fFrequency= 997.00;

            aTestMatrix[u][v].dSNR= 0;
        }
    

    //
    // For each pair of input/output frequencies
    for (u=0; u < u32MatrixDimension; u++)
        for (v=0; v < u32MatrixDimension; v++)
        {
			//
            // create input out/formats
            BUILD_FORMAT(&InputFormat,
                        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 
                        nChannels,
                        4,
                        32,
                        aTestMatrix[u][v].finputSR);

            BUILD_FORMAT(&OutputFormat,
                        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 
                        nChannels,
                        4,
                        32,
                        aTestMatrix[u][v].foutputSR);

            // create graph descriptors
            GRAPH_APO_DESCRIPTOR aApoDescriptors[] = { { APO_MIXER, NULL } , { APO_SRC , NULL } };
            GRAPH_BINDING_DESCRIPTOR aBindingDescriptors[] = { 
                { 0, 0, true  }, 
                { 0, 1, false },
                { 1, 1, true  },
                { 1, 2, false } };
            GRAPH_CONNECTION_DESCRIPTOR aConnectionDescriptors[3]; 
            
			ZeroMemory(aConnectionDescriptors, sizeof(aConnectionDescriptors));
            CopyMemory(&aConnectionDescriptors[0].FrameFormat, &InputFormat, sizeof(InputFormat)); 
            CopyMemory(&aConnectionDescriptors[1].FrameFormat, &InputFormat, sizeof(InputFormat)); 
            CopyMemory(&aConnectionDescriptors[2].FrameFormat, &OutputFormat, sizeof(OutputFormat)); 


			//
			// In the pull case we want to get this much data out 
			// in a pass (10 ms at the output rate) 
			// In the push case we will always push 10 ms at the inpu rate
			// trough the graph

			UINT32 u32Ms10=1;

			//
			// The max sampling ratio is 1:24 (8:192)
			if (SrcFlow == AUDIO_FLOW_PULL)
			{
				//
				// For the given amount of data that we expect as output we might
				// require in the case output = 8 and input = 192, 24 x amount of input frames
				u32Ms10 = (UINT32)(OutputFormat.f32FramesPerSecond/100.00 + 0.5);
	            aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10 * 24 + 4; 
				aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10 * 24 + 4; 
				aConnectionDescriptors[2].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10; 
			}
			else
			{
				//
				// For the given amount of data that we will push trough the graph
				// we might require in the case input = 8 and output = 192, 24 x amount of output frames 
				u32Ms10 = (UINT32)(InputFormat.f32FramesPerSecond/100 + 0.5);
	            aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10; 
				aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10; 
				aConnectionDescriptors[2].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10*24 + 4; 
			}

			//aConnectionDescriptors[1].ConnectionDescriptor.u32ExtraFrameCount =  4; 
            //aConnectionDescriptors[2].ConnectionDescriptor.u32ExtraFrameCount =  0; 

            SLOG(eInfo1, "");
            SLOG(eInfo1, "");
            SLOG(eInfo1, "*********************************************************************************************");
            SLOG(eInfo1, "Testing conversion %.5lf --> %.5lf", aTestMatrix[u][v].finputSR, aTestMatrix[u][v].foutputSR);
            SLOG(eInfo1, "*********************************************************************************************");
            SLOG(eInfo1, "");
            SLOG(eInfo1, "");

            HRESULT hr;

			//
			// Create the graph
            CGraphRepresentation graph(aConnectionDescriptors,
                                    sizeof(aConnectionDescriptors)/sizeof(aConnectionDescriptors[0]), 
                                    aBindingDescriptors,
                                    sizeof(aBindingDescriptors)/sizeof(aBindingDescriptors[0]), 
                                    aApoDescriptors,
                                    sizeof(aApoDescriptors)/sizeof(aApoDescriptors[0]),                                     
                                    (UINT32)InputFormat.f32FramesPerSecond*3,  // process 2 seconds
                                    (UINT32)OutputFormat.f32FramesPerSecond*3, // process 2 seconds
                                    NULL,                                      // use output memory endpoint
                                    aTestMatrix[u][v].fFrequency
                                    );

			//
            // Change SRC type
            graph.SetSRCAlgorithm(SrcType);

			//
			// Change SRC flow 
            graph.SetSRCFlow(SrcFlow);

			//
			// Build graph
            if (FAILED(hr = graph.BuildGraph()))
            {
                SLOG(eError, "ERROR: Can't build graph");
                return FNS_FAIL;
            }

            MFTIME mfTime = 0;
            MFTIME mfPeriod = (MFTIMEFREQUENCY/1000)*10; // 10 ms per clock

			//
            // Start the endpoints
            graph.StartEndpoints();
            graph.StartEndpoints(NULL, true);

			CComPtr<IAudioMemoryEndpointEx> pMemoryInputEndpoint;
			CComPtr<IAudioMemoryEndpointEx> pMemoryOutputEndpoint;


			//
            // Get the endpoints OOB interfaces
            graph.GetEndpoint(true)->QueryInterface(__uuidof(IAudioMemoryEndpointEx), (LPVOID*)&pMemoryInputEndpoint);  
            graph.GetEndpoint(false)->QueryInterface(__uuidof(IAudioMemoryEndpointEx), (LPVOID*)&pMemoryOutputEndpoint);  


			//
			// Set the input endpoint to push mode and to process always 10 ms of input data
//			if (SrcFlow == AUDIO_FLOW_PUSH)
//			{
//				SLOG(eInfo1, "PUSH'ing %d frames trough the graph each time", u32Ms10);
//				pMemoryInputEndpoint->SetPushMode(u32Ms10/2);
//			}

			UINT32 u32InputPosition=0;
			UINT32 u32OutputPosition=0;

	  
            // See a detailed explanation bellow in the format converter case
			if (SrcFlow == AUDIO_FLOW_PUSH) {
				mfPeriod/=10;
			}

			//
            // Call process
            while (1)  
            {
				if (SrcFlow == AUDIO_FLOW_PUSH)
				{
					//
					// check that we have enough input space
					if (u32InputPosition + (UINT32)4*u32Ms10*nChannels*InputFormat.f32FramesPerSecond/1000 >=
						nChannels*InputFormat.f32FramesPerSecond*3*4)
						break;
				}
				else
				{
					//
					// check that we have enough output space
					if (u32OutputPosition + (UINT32)nChannels*4*u32Ms10*OutputFormat.f32FramesPerSecond/1000 >=
						nChannels*OutputFormat.f32FramesPerSecond*3*4)
						break;
				}

				//
				// Call process on the processor
                graph.Step(&mfTime, &mfPeriod); 
				u32InputPosition  = pMemoryInputEndpoint->GetPosition();
				u32OutputPosition = pMemoryOutputEndpoint->GetPosition();
//				SLOG(eInfo1, "Pos input = %ld, Pos output = %ld", u32InputPosition, u32OutputPosition);
            }


            u32InputPosition  = pMemoryInputEndpoint->GetPosition();
            u32OutputPosition = pMemoryOutputEndpoint->GetPosition();

            WAVEFORMATEX wfx;
            FORMAT2WAVEXIEEE(&OutputFormat, &wfx);

			//
            // validate samples in the output buffer
            UINT32 nAvailableFrames = u32OutputPosition/wfx.nBlockAlign;
            UINT32 nOffset = (UINT32)OutputFormat.f32FramesPerSecond/2;
            UINT32 nFrames = (UINT32)OutputFormat.f32FramesPerSecond;
            if (nAvailableFrames < 3*OutputFormat.f32FramesPerSecond/2)
            {
                SLOG(eError, "ERROR: Not enough output frames - expected ~2s");
                return FNS_FAIL;
            }

			//
            // design filter
            float fNormalFrequency = aTestMatrix[u][v].fFrequency*2/OutputFormat.f32FramesPerSecond; 
			// this means that the old frequency should be completely attenuated!
			// in oher words we should have only noise in the signal meaning that
			// we can pick any frequency and get ~0dB Snr
			// in the case were the frequency is aliased back we should get a negative Snr
			if (fNormalFrequency >= 1)
			{
				float f = fNormalFrequency;
				while (f>=1) f-=1;
				fNormalFrequency= f;
			}
            double dSNR=0;
            double* pf64Filter=NULL;
            KAISER_INFO kinfo;
			//
			// Design Kaiser notch filter
            if (!KaiserNotchDesign(fNormalFrequency, wfx.wBitsPerSample, &pf64Filter, kinfo))
            {
                SLOG(eError, "INTERNAL ERROR: Out of memory or filter design bug");
                return FNS_PENDING;
            }

#ifdef _DEBUG
			//
			// Debug dump fo the input/output/filter
            KaiserDumpText(L"C:\\MATLABR11\\WORK\\InputBuffer.m", 
                        (float*)graph.GetInputData(), 
                        (UINT32)1024);
            KaiserDumpText(L"C:\\MATLABR11\\WORK\\OutputBuffer.m", 
                        (float*)(((LPBYTE)graph.GetOutputData()) + nOffset*wfx.nBlockAlign), 
                        (UINT32)2048);
            KaiserDumpText(L"C:\\MATLABR11\\WORK\\KaiserDesign.m", 
                        (double*)pf64Filter, 
                        kinfo.M + 1);
#endif

			//
            // calculate the SNR
            if (fir_snr(((LPBYTE)graph.GetOutputData()) + nOffset*wfx.nBlockAlign, // data to analyze
                        nFrames, // number of frames to analyze
                        &wfx,    // data type
                        pf64Filter,  // filter to use
                        kinfo.M + 1, // number of taps
                        &dSNR))
            {
                SLOG(eInfo1, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
                SLOG(eInfo1, "");
                SLOG(eInfo1, "SNR calculated in time domain: ^^ %4.2lf ^^", dSNR);
                SLOG(eInfo1, "");
                SLOG(eInfo1, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

                aTestMatrix[u][v].dSNR = dSNR;
            }
            else
            {
                SLOG(eError, "INTERNAL ERROR: Out of memory");
                return FNS_PENDING;
            }
        }

    //
    // Print headers 
    TCHAR  szHeader[256];
    memset(szHeader, TEXT(' '), sizeof(TCHAR)*256);
    TCHAR* pszHeader = szHeader + 16;
    
    SLOG(eInfo1, "----------------------------------------------------------------------------------------");
    for (u=0; u < u32MatrixDimension; u++)
    {
        pszHeader +=_stprintf(pszHeader, TEXT("%6ld          "), (UINT32)aFrequencies[u]);
    }
    SLOG(eInfo1, szHeader);
    SLOG(eInfo1, "");

    //
    // Print table
    for (u=0; u < u32MatrixDimension; u++)
    {
        TCHAR  szRow[256]={0};
        TCHAR* pszRow = szRow;
        pszRow +=_stprintf(pszRow, TEXT("%6ld          "), (UINT32)aTestMatrix[u][0].finputSR);
        for (v=0; v < u32MatrixDimension; v++)
        {
            pszRow +=_stprintf(pszRow, TEXT("%5.1lf          "), aTestMatrix[u][v].dSNR);
        }
        SLOG(eInfo1, szRow);
        SLOG(eInfo1, "");
    }
    SLOG(eInfo1, "----------------------------------------------------------------------------------------");

    return FNS_PASS;
}

DWORD Test_FixedFormatConvertor_SNR16(SRC_ALGORITHM_TYPE SrcType)
{
    SLOG(eInfo1, "Create a graph Mem-->32-->Fmt-->16-->SRC-->Fmt-->16-->Mem");
    SLOG(eInfo1, "The test will determine the SNR of the converter for %s quality (16 bit)", (SrcType==SRC_QUALITY_LINEAR) ? "LINEAR" : "OTHER");
	UINT nChannels=1;

	//
    // Input/output formats
    AUDIO_FRAME_FORMAT InputFormat, OutputFormat, InputFormat1, OutputFormat1;

	//
    // Structure used for test pairs
    typedef struct _SRPAIR
    {
        float  finputSR, foutputSR, fFrequency;
        double dSNR;
    } SRPAIR;

    UINT32 u,v;

    //
    // Initialize test matrix
    float aFrequencies[] = { 8000, 15999, 16000, 11025, 22050, 31999, 32000, 44100, 48000 }; 
    const UINT32 u32MatrixDimension = sizeof(aFrequencies)/sizeof(aFrequencies[0]);
    SRPAIR  aTestMatrix[u32MatrixDimension][u32MatrixDimension];
    for (u=0; u < u32MatrixDimension; u++)
        for (v=0; v < u32MatrixDimension; v++)
        {
            aTestMatrix[u][v].finputSR  = aFrequencies[u];
            aTestMatrix[u][v].foutputSR = aFrequencies[v];
            aTestMatrix[u][v].fFrequency= 997.00;
            aTestMatrix[u][v].dSNR= 0;
        }
    


    for (u=0; u < u32MatrixDimension; u++)
        for (v=0; v < u32MatrixDimension; v++)
        {
            // create input out/formats
            BUILD_FORMAT(&InputFormat,
                        KSDATAFORMAT_SUBTYPE_PCM, 
                        1,
                        2,
                        16,
                        aTestMatrix[u][v].finputSR);

            BUILD_FORMAT(&InputFormat1,
                        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 
                        1,
                        4,
                        32,
                        aTestMatrix[u][v].finputSR);

            BUILD_FORMAT(&OutputFormat1,
                        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 
                        1,
                        4,
                        32,
                        aTestMatrix[u][v].foutputSR);

            BUILD_FORMAT(&OutputFormat,
                        KSDATAFORMAT_SUBTYPE_PCM, 
                        1,
                        2,
                        16,
                        aTestMatrix[u][v].foutputSR);

            // create graph descriptors
            GRAPH_APO_DESCRIPTOR aApoDescriptors[] = { { APO_FMT_i16f32, NULL } , { APO_SRC , NULL }, { APO_FMT_f32i16, NULL } };
            GRAPH_BINDING_DESCRIPTOR aBindingDescriptors[] = { 
                { 0, 0, true  }, 
                { 0, 1, false },
                { 1, 1, true  },
                { 1, 2, false },
                { 2, 2, true  },
                { 2, 3, false } };
            GRAPH_CONNECTION_DESCRIPTOR aConnectionDescriptors[4]; 
            
			ZeroMemory(aConnectionDescriptors, sizeof(aConnectionDescriptors));
            CopyMemory(&aConnectionDescriptors[0].FrameFormat, &InputFormat, sizeof(InputFormat)); 
            CopyMemory(&aConnectionDescriptors[1].FrameFormat, &InputFormat1, sizeof(InputFormat)); 
            CopyMemory(&aConnectionDescriptors[2].FrameFormat, &OutputFormat1, sizeof(OutputFormat)); 
            CopyMemory(&aConnectionDescriptors[3].FrameFormat, &OutputFormat, sizeof(OutputFormat)); 

			//
			// Always pushmode
			UINT32 u32Ms10 = (UINT32)(InputFormat.f32FramesPerSecond/100.00 + 0.5);
	        aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10; 
			aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10; 
			aConnectionDescriptors[2].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10*24 + 4;

            aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10;
            aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10; 
            aConnectionDescriptors[2].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10*24 + 4; 
            aConnectionDescriptors[3].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10*24 + 4; 
            
			//aConnectionDescriptors[1].ConnectionDescriptor.u32ExtraFrameCount =  4; 

            SLOG(eInfo1, "");
            SLOG(eInfo1, "");
            SLOG(eInfo1, "*********************************************************************************************");
            SLOG(eInfo1, "Testing conversion %.5lf --> %.5lf", aTestMatrix[u][v].finputSR, aTestMatrix[u][v].foutputSR);
            SLOG(eInfo1, "*********************************************************************************************");
            SLOG(eInfo1, "");
            SLOG(eInfo1, "");

            HRESULT hr;


            // In this case we run always in the push model which means that
            // the output buffer size is constant and the input frames will vary
            CGraphRepresentation graph(aConnectionDescriptors,
                                    sizeof(aConnectionDescriptors)/sizeof(aConnectionDescriptors[0]), 
                                    aBindingDescriptors,
                                    sizeof(aBindingDescriptors)/sizeof(aBindingDescriptors[0]), 
                                    aApoDescriptors,
                                    sizeof(aApoDescriptors)/sizeof(aApoDescriptors[0]),                                     
                                    (UINT32)InputFormat.f32FramesPerSecond*3,  // process 2 seconds
                                    (UINT32)OutputFormat.f32FramesPerSecond*3, // process 3 seconds
                                    NULL,                                      // use output memory endpoint
                                    aTestMatrix[u][v].fFrequency
                                    );

            // Change SRC type
            graph.SetSRCAlgorithm(SrcType);
            graph.SetSRCFlow(AUDIO_FLOW_PUSH);

            if (FAILED(hr = graph.BuildGraph()))
            {
                SLOG(eError, "ERROR: Can't build graph");
                return FNS_FAIL;
            }

            MFTIME mfTime = 0;
            MFTIME mfPeriod = (MFTIMEFREQUENCY/1000)*10; // 10 ms per clock

            // start the endpoints
            graph.StartEndpoints();
            graph.StartEndpoints(NULL, true);

            CComPtr<IAudioMemoryEndpointEx> pMemoryInputEndpoint;
            CComPtr<IAudioMemoryEndpointEx> pMemoryOutputEndpoint;

            // this will always work
            graph.GetEndpoint(true)->QueryInterface(__uuidof(IAudioMemoryEndpointEx), (LPVOID*)&pMemoryInputEndpoint);  
            graph.GetEndpoint(false)->QueryInterface(__uuidof(IAudioMemoryEndpointEx), (LPVOID*)&pMemoryOutputEndpoint);  

			// set the input endpoint in push mode because the processor only supports
			// pull mode
//			SLOG(eInfo1, "PUSH'ing %d frames trough the graph each time", u32Ms10/10);
//			pMemoryInputEndpoint->SetPushMode(u32Ms10/10);

			UINT32 u32InputPosition=0;
			UINT32 u32OutputPosition=0;

	    // we need this to get rid of as assert in the processor 
	    // we get this assert because the processor will still treat the
	    // the graph as a pull graph and we don't have enough space in our connections
	    // for both pull and push
	    mfPeriod/=10;

            // run until no more data
            while (1)  
            {
				//
				// check that we have enough input space
				if (u32InputPosition + (UINT32)2*u32Ms10*nChannels*InputFormat.f32FramesPerSecond/1000 >=
					nChannels*InputFormat.f32FramesPerSecond*3*2)
				{
					break;
				}
			
				u32InputPosition  = pMemoryInputEndpoint->GetPosition();
				u32OutputPosition = pMemoryOutputEndpoint->GetPosition();
//				SLOG(eInfo1, "Pos input = %ld, Pos output = %ld", u32InputPosition, u32OutputPosition);
                graph.Step(&mfTime, &mfPeriod); 
            }

			u32InputPosition  = pMemoryInputEndpoint->GetPosition();
            u32OutputPosition = pMemoryOutputEndpoint->GetPosition();

            WAVEFORMATEX wfx;
            FORMAT2WAVEXIEEE(&OutputFormat, &wfx);

			//
            // validate samples in the output buffer
            UINT32 nAvailableFrames = u32OutputPosition/wfx.nBlockAlign;
            UINT32 nOffset = (UINT32)OutputFormat.f32FramesPerSecond/2;
            UINT32 nFrames = (UINT32)OutputFormat.f32FramesPerSecond;
            if (nAvailableFrames < 3*OutputFormat.f32FramesPerSecond/2)
            {
                SLOG(eError, "ERROR: Not enough output frames - expected ~2s");
                return FNS_FAIL;
            }

            // design filter
            float fNormalFrequency = aTestMatrix[u][v].fFrequency*2/OutputFormat.f32FramesPerSecond; 
			// this means that the old frequency should be completely attenuated!
			// in oher words we should have only noise in the signal meaning that
			// we can pick any frequency and get ~0dB Snr
			// in the case were the frequency is aliased back we should get a negative Snr
			if (fNormalFrequency >= 1)
			{
				float f = fNormalFrequency;
				while (f>=1) f-=1;
				fNormalFrequency= f;
			}
            double dSNR=0;
            double* pf64Filter=NULL;
            KAISER_INFO kinfo;
            if (!KaiserNotchDesign(fNormalFrequency, wfx.wBitsPerSample, &pf64Filter, kinfo))
            {
                SLOG(eError, "INTERNAL ERROR: Out of memory of filter design bug");
                return FNS_PENDING;
            }

#ifdef _DEBUG
            KaiserDumpText(L"C:\\MATLABR11\\WORK\\InputBuffer.m", 
                           (short*)graph.GetInputData(), 
                           (UINT32)1024);
            KaiserDumpText(L"C:\\MATLABR11\\WORK\\OutputBuffer.m", 
                           (short*)(((LPBYTE)graph.GetOutputData()) + nOffset*wfx.nBlockAlign), 
                           (UINT32)2048);
            KaiserDumpText(L"C:\\MATLABR11\\WORK\\KaiserDesign.m", 
                           (double*)pf64Filter, 
                           kinfo.M + 1);
#endif

            // calculate the SNR
            if (fir_snr(((LPBYTE)graph.GetOutputData()) + nOffset*wfx.nBlockAlign, // data to analyze
                        nFrames, // number of frames to analyze
                        &wfx,    // data type
                        pf64Filter,  // filter to use
                        kinfo.M + 1, // number of taps
                        &dSNR))
            {
                SLOG(eInfo1, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
                SLOG(eInfo1, "");
                SLOG(eInfo1, "SNR calculated in time domain: ^^ %4.2lf ^^", dSNR);
                SLOG(eInfo1, "");
                SLOG(eInfo1, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

                aTestMatrix[u][v].dSNR = dSNR;
            }
        }

    //
    // Print headers 
    TCHAR  szHeader[256];
    memset(szHeader, TEXT(' '), sizeof(TCHAR)*256);
    TCHAR* pszHeader = szHeader + 16;
    
    SLOG(eInfo1, "----------------------------------------------------------------------------------------");
    for (u=0; u < u32MatrixDimension; u++)
    {
        pszHeader +=_stprintf(pszHeader, TEXT("%6ld          "), (UINT32)aFrequencies[u]);
    }
    SLOG(eInfo1, szHeader);
    SLOG(eInfo1, "");

    //
    // Print table
    for (u=0; u < u32MatrixDimension; u++)
    {
        TCHAR  szRow[256]={0};
        TCHAR* pszRow = szRow;
        pszRow +=_stprintf(pszRow, TEXT("%6ld          "), (UINT32)aTestMatrix[u][0].finputSR);
        for (v=0; v < u32MatrixDimension; v++)
        {
            pszRow +=_stprintf(pszRow, TEXT("%5.1lf          "), aTestMatrix[u][v].dSNR);
        }
        SLOG(eInfo1, szRow);
        SLOG(eInfo1, "");
    }
    SLOG(eInfo1, "----------------------------------------------------------------------------------------");

    return FNS_PASS;
} // Test_FixedFormatConvertor_SNR16


DWORD Test_FixedFormatConvertor_SNR32_KMIXER(BOOL bUseVariableFrequency=false)
{
    SLOG(eInfo1, "Create a graph Mem-->32-->Mixer-->KSRC-->-->32-->Mem");
 
    //
    // Input/output formats
    AUDIO_FRAME_FORMAT InputFormat, OutputFormat;

    //
    // Structure used for test pairs
    typedef struct _SRPAIR
    {
        float  finputSR, foutputSR, fFrequency;
        double dSNR;
    } SRPAIR;

    UINT32 u,v;

    //
    // Initialize test matrix
    float aFrequencies[] = { 8000, 16000, 11025, 22050, 32000, 44100, 48000, 96000 }; 
    UINT32 u32MatrixDimension = sizeof(aFrequencies)/sizeof(aFrequencies[0]);
    SRPAIR  aTestMatrix[MAX_RATES][MAX_RATES];
    for (u=0; u < u32MatrixDimension; u++)
        for (v=0; v < u32MatrixDimension; v++)
        {
            aTestMatrix[u][v].finputSR  = aFrequencies[u];
            aTestMatrix[u][v].foutputSR = aFrequencies[v];
			if (bUseVariableFrequency)
			{
				// always use a frequency near input signal Nyquist
				// note that if we downsample we expect the SNR to be zero
				// otherwise as good as possible
	            aTestMatrix[u][v].fFrequency= min(20000, min(aTestMatrix[u][v].finputSR/2,aTestMatrix[u][v].foutputSR/2))  - 1000;
			}
			else
	            aTestMatrix[u][v].fFrequency= 997.00;
            aTestMatrix[u][v].dSNR= 0;
        }
    


    //
    // For each pair
    for (u=0; u < u32MatrixDimension; u++)
        for (v=0; v < u32MatrixDimension; v++)
        {
            // create input out/formats
            BUILD_FORMAT(&InputFormat,
                        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 
                        1,
                        4,
                        32,
                        aTestMatrix[u][v].finputSR);

            BUILD_FORMAT(&OutputFormat,
                        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 
                        1,
                        4,
                        32,
                        aTestMatrix[u][v].foutputSR);

            // create graph descriptors
            GRAPH_APO_DESCRIPTOR aApoDescriptors[] = { { APO_MIXER, NULL } , { APO_KMIXER , NULL } };
            GRAPH_BINDING_DESCRIPTOR aBindingDescriptors[] = { 
                { 0, 0, true  }, 
                { 0, 1, false },
                { 1, 1, true  },
                { 1, 2, false } };
            GRAPH_CONNECTION_DESCRIPTOR aConnectionDescriptors[3]; 
            
			ZeroMemory(aConnectionDescriptors, sizeof(aConnectionDescriptors));
            CopyMemory(&aConnectionDescriptors[0].FrameFormat, &InputFormat, sizeof(InputFormat)); 
            CopyMemory(&aConnectionDescriptors[1].FrameFormat, &InputFormat, sizeof(InputFormat)); 
            CopyMemory(&aConnectionDescriptors[2].FrameFormat, &OutputFormat, sizeof(OutputFormat)); 
            // all the connection buffers will have 20 ms of data 
            aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  2*(UINT32)InputFormat.f32FramesPerSecond/100; 
            aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  2*(UINT32)InputFormat.f32FramesPerSecond/100; 
            aConnectionDescriptors[2].ConnectionDescriptor.u32MaxFrameCount =  2*(UINT32)OutputFormat.f32FramesPerSecond/100; 
            //aConnectionDescriptors[1].ConnectionDescriptor.u32ExtraFrameCount =  4; 
            //aConnectionDescriptors[2].ConnectionDescriptor.u32ExtraFrameCount =  0; 

            SLOG(eInfo1, "");
            SLOG(eInfo1, "");
            SLOG(eInfo1, "*********************************************************************************************");
            SLOG(eInfo1, "Testing conversion %.5lf --> %.5lf", aTestMatrix[u][v].finputSR, aTestMatrix[u][v].foutputSR);
            SLOG(eInfo1, "*********************************************************************************************");
            SLOG(eInfo1, "");
            SLOG(eInfo1, "");

            HRESULT hr;


            // In this case we run always in the push model which means that
            // the output buffer size is constant and the input frames will vary
            CGraphRepresentation graph(aConnectionDescriptors,
                                    sizeof(aConnectionDescriptors)/sizeof(aConnectionDescriptors[0]), 
                                    aBindingDescriptors,
                                    sizeof(aBindingDescriptors)/sizeof(aBindingDescriptors[0]), 
                                    aApoDescriptors,
                                    sizeof(aApoDescriptors)/sizeof(aApoDescriptors[0]),                                     
                                    (UINT32)InputFormat.f32FramesPerSecond*2,  // process 2 seconds
                                    (UINT32)OutputFormat.f32FramesPerSecond*2, // process 3 seconds
                                    NULL,                                      // use output memory endpoint
                                    aTestMatrix[u][v].fFrequency
                                    );


            if (FAILED(hr = graph.BuildGraph()))
            {
                SLOG(eError, "ERROR: Can't build graph");
                return FNS_FAIL;
            }

			//
            // Start the endpoints
            graph.StartEndpoints();
            graph.StartEndpoints(NULL, true);

            MFTIME mfTime = 0;
            MFTIME mfPeriod = (MFTIMEFREQUENCY/1000)*10; // 10 ms per clock

            // run until no more data
            while (graph.IsDataAvailable(true) && graph.IsDataAvailable(false))  
            {
                graph.Step(&mfTime, &mfPeriod); 
            }
            CComPtr<IAudioMemoryEndpointEx> pMemoryInputEndpoint;
            CComPtr<IAudioMemoryEndpointEx> pMemoryOutputEndpoint;

            // this will always work
            graph.GetEndpoint(true)->QueryInterface(__uuidof(IAudioMemoryEndpointEx), (LPVOID*)&pMemoryInputEndpoint);  
            graph.GetEndpoint(false)->QueryInterface(__uuidof(IAudioMemoryEndpointEx), (LPVOID*)&pMemoryOutputEndpoint);  

            UINT32 u32InputPosition  = pMemoryInputEndpoint->GetPosition();
            UINT32 u32OutputPosition = pMemoryOutputEndpoint->GetPosition();

            WAVEFORMATEX wfx;
            FORMAT2WAVEXIEEE(&OutputFormat, &wfx);

            // samples in the output buffer
            UINT32 nAvailableFrames = u32OutputPosition/wfx.nBlockAlign;
            UINT32 nOffset = (UINT32)OutputFormat.f32FramesPerSecond/2;
            UINT32 nFrames = (UINT32)OutputFormat.f32FramesPerSecond;
            if (nAvailableFrames < 3*OutputFormat.f32FramesPerSecond/2)
            {
                SLOG(eError, "ERROR: Not enough output frames - expected ~2s");
                return FNS_FAIL;
            }

            // design filter
            float fNormalFrequency = aTestMatrix[u][v].fFrequency*2/OutputFormat.f32FramesPerSecond; 
			// this means that the old frequency should be completely attenuated!
			// in oher words we should have only noise in the signal meaning that
			// we can pick any frequency and get ~0dB Snr
			// in the case were the frequency is aliased back we should get a negative Snr
			if (fNormalFrequency >= 1)
			{
				float f = fNormalFrequency;
				while (f>=1) f-=1;
				fNormalFrequency= f;
			}
            double dSNR=0;
            double* pf64Filter=NULL;
            KAISER_INFO kinfo;
            if (!KaiserNotchDesign(fNormalFrequency, wfx.wBitsPerSample, &pf64Filter, kinfo))
            {
                SLOG(eError, "INTERNAL ERROR: Out of memory or filter design bug");
                return FNS_PENDING;
            }

#ifdef _DEBUG
            KaiserDumpText(L"C:\\Temp\\InputBuffer.m", 
                        (float*)graph.GetInputData(), 
                        (UINT32)1024);
            KaiserDumpText(L"C:\\Temp\\OutputBuffer.m", 
                        (float*)(((LPBYTE)graph.GetOutputData()) + nOffset*wfx.nBlockAlign), 
                        (UINT32)2048);
            KaiserDumpText(L"C:\\Temp\\KaiserDesign.m", 
                        (double*)pf64Filter, 
                        kinfo.M + 1);
#endif

            // calculate the SNR
            if (fir_snr(((LPBYTE)graph.GetOutputData()) + nOffset*wfx.nBlockAlign, // data to analyze
                        nFrames, // number of frames to analyze
                        &wfx,    // data type
                        pf64Filter,  // filter to use
                        kinfo.M + 1, // number of taps
                        &dSNR))
            {
                SLOG(eInfo1, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
                SLOG(eInfo1, "");
                SLOG(eInfo1, "SNR calculated in time domain: ^^ %4.2lf ^^", dSNR);
                SLOG(eInfo1, "");
                SLOG(eInfo1, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

                aTestMatrix[u][v].dSNR = dSNR;
            }
            else
            {
                SLOG(eError, "INTERNAL ERROR: Out of memory");
                return FNS_PENDING;
            }
        }

    //
    // Print headers 
    TCHAR  szHeader[256];
    memset(szHeader, TEXT(' '), sizeof(TCHAR)*256);
    TCHAR* pszHeader = szHeader + 16;
    
    SLOG(eInfo1, "----------------------------------------------------------------------------------------");
    for (u=0; u < u32MatrixDimension; u++)
    {
        pszHeader +=_stprintf(pszHeader, TEXT("%6ld          "), (UINT32)aFrequencies[u]);
    }
    SLOG(eInfo1, szHeader);
    SLOG(eInfo1, "");

    //
    // Print table
    for (u=0; u < u32MatrixDimension; u++)
    {
        TCHAR  szRow[256]={0};
        TCHAR* pszRow = szRow;
        pszRow +=_stprintf(pszRow, TEXT("%6ld          "), (UINT32)aTestMatrix[u][0].finputSR);
        for (v=0; v < u32MatrixDimension; v++)
        {
            pszRow +=_stprintf(pszRow, TEXT("%5.1lf          "), aTestMatrix[u][v].dSNR);
        }
        SLOG(eInfo1, szRow);
        SLOG(eInfo1, "");
    }
    SLOG(eInfo1, "----------------------------------------------------------------------------------------");

    return FNS_PASS;
} // Test_FixedFormatConvertor_SNR32_KMIXER




//********************************************************************************************
// TEST CASES START HERE
//********************************************************************************************

//
// Mono test Linear push
DWORD Test_FixedFormatConvertor_SNR_Linear32()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_LINEAR);
}

//
// Multichannel test Linear  push
DWORD Test_FixedFormatConvertor_SNR_Linear32_Multi()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_LINEAR, 4, AUDIO_FLOW_PUSH);
}

//
// Mono test Linear pull
DWORD Test_FixedFormatConvertor_SNR_Linear32_Pull()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_LINEAR, 1, AUDIO_FLOW_PULL);
}

//
// Multichannel test Linear pull
DWORD Test_FixedFormatConvertor_SNR_Linear32_Multi_Pull()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_LINEAR, 4, AUDIO_FLOW_PULL);
}

//
// Nyquist test linear mono, push
DWORD Test_FixedFormatConvertor_SNR_Linear32_Nyq()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_LINEAR, 1, AUDIO_FLOW_PUSH, true);
}

//
// 16 bit linear mono, push
DWORD Test_FixedFormatConvertor_SNR_Linear16()
{
    return Test_FixedFormatConvertor_SNR16(SRC_QUALITY_LINEAR);
}

//
// Kmixer mono tests 32 bit only
DWORD Test_FixedFormatConvertor_SNR32_KMIXER_1()
{
	return Test_FixedFormatConvertor_SNR32_KMIXER();
}

DWORD Test_FixedFormatConvertor_SNR32_KMIXER_2()
{
	return Test_FixedFormatConvertor_SNR32_KMIXER(true);
}

//
// Mono test Sinc push
DWORD Test_FixedFormatConvertor_SNR_Sinc1_32()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC1);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc2_32()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC2);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc3_32()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC3);
}

//
// 16 bit sinc mono, push
DWORD Test_FixedFormatConvertor_SNR_Sinc1_16()
{
    return Test_FixedFormatConvertor_SNR16(SRC_QUALITY_SINC1);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc2_16()
{
    return Test_FixedFormatConvertor_SNR16(SRC_QUALITY_SINC2);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc3_16()
{
    return Test_FixedFormatConvertor_SNR16(SRC_QUALITY_SINC3);
}

//
// Nyquist test sinc mono, push
DWORD Test_FixedFormatConvertor_SNR_Sinc1_32_Nyq()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC1, 1, AUDIO_FLOW_PUSH, true);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc2_32_Nyq()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC2, 1, AUDIO_FLOW_PUSH, true);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc3_32_Nyq()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC3, 1, AUDIO_FLOW_PUSH, true);
}


//
// Multichannel test sinc  push
DWORD Test_FixedFormatConvertor_SNR_Sinc1_32_Multi()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC1, 4, AUDIO_FLOW_PUSH);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc2_32_Multi()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC2, 4, AUDIO_FLOW_PUSH);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc3_32_Multi()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC3, 4, AUDIO_FLOW_PUSH);
}

//
// Mono test sinc pull
DWORD Test_FixedFormatConvertor_SNR_Sinc1_32_Pull()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC1, 1, AUDIO_FLOW_PULL);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc2_32_Pull()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC2, 1, AUDIO_FLOW_PULL);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc3_32_Pull()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC3, 1, AUDIO_FLOW_PULL);
}

//
// Multichannel test sinc pull
DWORD Test_FixedFormatConvertor_SNR_Sinc1_32_Multi_Pull()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC1, 4, AUDIO_FLOW_PULL);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc2_32_Multi_Pull()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC2, 4, AUDIO_FLOW_PULL);
}
DWORD Test_FixedFormatConvertor_SNR_Sinc3_32_Multi_Pull()
{
    return Test_FixedFormatConvertor_SNR32(SRC_QUALITY_SINC3, 4, AUDIO_FLOW_PULL);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//  SRC DMO quality testing
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
class CDMOResamplerAttribs
{
	WAVEFORMATEXTENSIBLE	m_wfxInput;
	WAVEFORMATEXTENSIBLE	m_wfxOutput;
	CComPtr<IMediaObject>   m_pDMO;
public:
	CDMOResamplerAttribs( WAVEFORMATEX* pwfxInput, WAVEFORMATEX* pwfxOutput);
	
	//@@ Creates the CMPT SRC DMO and sets it up for processing at the 
	//@@ rates specified in the input and output formats
	HRESULT Initialize();
};

CDMOResamplerAttribs::CDMOResamplerAttribs( WAVEFORMATEX* pwfxInput, WAVEFORMATEX* pwfxOutput ) {
	// copy the input parameters
	_ASSERT( pwfxInput );
	_ASSERT( pwfxOutput );
	_ASSERT( pwfxInput->cbSize + sizeof(WAVEFORMATEX)  <= sizeof(m_wfxInput) );
	CopyMemory( &m_wfxInput, pwfxInput, pwfxInput->cbSize + sizeof(WAVEFORMATEX) );
	_ASSERT( pwfxOutput->cbSize + sizeof(WAVEFORMATEX)  <= sizeof(m_wfxOutput) );
	CopyMemory( &m_wfxOutput, pwfxOutput, pwfxOutput->cbSize + sizeof(WAVEFORMATEX) );
}

HRESULT CDMOResamplerAttribs::Initialize() {
	HRESULT hr = S_OK;
	CComPtr<IWMResamplerProps> pResamplerConfig;
	AM_MEDIA_TYPE              inputMediaType;
	AM_MEDIA_TYPE			   outputMediaType;
	DWORD					   dwInputBufferSize;
	DWORD					   dwInputBufferAlignment;
	DWORD					   dwInputBufferLookahead;
	DWORD					   dwOutputBufferSize;
	DWORD					   dwOutputBufferAlignment;

	// instantiate DMO
	hr = m_pDMO.CoCreateInstance( CLSID_CResamplerMediaObject );

	if ( FAILED(hr) ) {
		SLOG(eError,"ERROR: Can't instantiate the resampler DMO 0x%X", hr);
		goto DONE;
	}

	// configure DMO
	hr = CreateAudioMediaType( &m_wfxInput,  &inputMediaType, FALSE);
	if ( FAILED(hr) ) {
		SLOG(eError,"ERROR: Can't fill media type 0x%X", hr);
		goto DONE;
	}
	hr = CreateAudioMediaType( &m_wfxOutput, &outputMediaType, FALSE);
	if ( FAILED(hr) ) {
		SLOG(eError,"ERROR: Can't fill media type 0x%X", hr);
		goto DONE;
	}

	hr = m_pDMO->SetInputType( 0, &inputMediaType, 0 );
	if ( FAILED(hr) ) {
		SLOG(eError,"ERROR: Failed setting DMO input type 0x%X", hr);
		goto DONE;
	}

	hr = m_pDMO->SetOutputType( 0, &outputMediaType, 0 );
	if ( FAILED(hr) ) {
		SLOG(eError,"ERROR: Failed setting DMO output type 0x%X", hr);
		goto DONE;
	}

	// optimize DMO processing pass
	hr = m_pDMO->AllocateStreamingResources();
	if ( FAILED(hr) ) {
		SLOG(eError,"ERROR: AllocateStreamingResources failed 0x%X", hr);
		goto DONE;
	}

	// get information about DMO's buffer alignment, size and lookahead
	hr = m_pDMO->GetInputSizeInfo( 0, &dwInputBufferSize, &dwInputBufferAlignment, &dwInputBufferLookahead );
	if ( FAILED(hr) ) {
		SLOG(eError,"ERROR: GetInputSizeInfo failed 0x%X", hr);
		goto DONE;
	}

	hr = m_pDMO->GetOutputSizeInfo( 0, &dwOutputBufferSize, &dwInputBufferAlignment );
	if ( FAILED(hr) ) {
		SLOG(eError,"ERROR: GetOutputSizeInfo failed 0x%X", hr);
		goto DONE;
	}

DONE:
	return hr;
}
*/
