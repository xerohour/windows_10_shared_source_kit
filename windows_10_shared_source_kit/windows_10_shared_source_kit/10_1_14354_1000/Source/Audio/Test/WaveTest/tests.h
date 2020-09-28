#pragma once
#include <mmdeviceapi.h>

// ----------------------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------------------
typedef enum { 	SetPOStoOutsideofBuffer,          // Set position outside the buffer
				SameWritePlayPOSHalfBuffer,   // Set write and play cursor at the same position at the half of the buffer
				SameWritePlayPOSzero,           // Set write and play cursor at the same position at starting of the buffer
				SameWritePlayPOSEndOfBuffer,   // Set write and play cursor at the same position at end of the buffer
				PlayPOSGreater 		                // Set Play cursor > write cursor.
                        } ePOSTestCaseType;
#define MAX_NUM_SEMAPHORES 4
#define CancelIO 0x01		// test case for Cancel IO
#define StreamingMultiThread 0x02	// test case for streaming in multithread
#define StreamingGetPosition 0x04			// test case for calling get position from diffenrent threads
struct ThreadProcData
{
    HANDLE  ahSemaphore[MAX_NUM_SEMAPHORES];  // array of semaphore handles
    BYTE  TestCase; 		// used to communicate test case intention
};

HRESULT GetCurrentEndpointsUnderTest(IMMDeviceCollection **ppEndpointCollection);

// STANDARD FORMATS
DWORD Test_Starve 		(void);
DWORD Test_SetInvalidPosition(ePOSTestCaseType TestCase);
DWORD Test_SetInvalidPosition1(void);
DWORD Test_SetInvalidPosition2(void);
DWORD Test_SetInvalidPosition3(void);
DWORD Test_SetValidPosition(void);
DWORD Test_ResetPin(void);
DWORD Test_SetGetPosition   (void);
DWORD Test_PinStateChanges (void);
DWORD Test_ChangeAudioFormat(void);
DWORD Test_CancelIO(void);
DWORD Test_StreamingMultiThread(void);
DWORD Test_StreamingSmallBuffer(void);
DWORD Test_GetPositionMultiThread(void);
DWORD Test_StreamingMultiPin(void);
DWORD Test_StreamingMultiPinDifferentFormat(void);
DWORD Test_PinInstantiateCheck(void);

//DataIntersection Tests
DWORD Test_RenderDataIntersection();
DWORD Test_CaptureDataIntersection();

//Compliance Tests
DWORD Test_VerifyPinIsWaveRT(void);
DWORD Test_VerifyPinSupportsPullMode(void);
DWORD Test_EnumerateAudioEndpoint(void);
DWORD Test_EnumerateAllAudioDevice(void); 
DWORD Test_NoDevice(void);
DWORD Test_VerifyRenderRawOrDefaultProcessingSupported(void);
DWORD Test_VerifyCaptureRawOrDefaultProcessingSupported(void);