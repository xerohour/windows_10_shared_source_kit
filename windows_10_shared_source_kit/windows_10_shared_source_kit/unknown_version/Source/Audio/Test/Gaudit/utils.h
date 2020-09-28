// Copyright(C) Microsoft.All rights reserved.

#ifndef _UTILS_H
#define _UTILS_H


#define IN
#define OUT
#define OPTONAL

// implemented allocators
typedef enum { 	ALLOCATOR_STATIC, 
				              ALLOCATOR_DEFAULT, 
								ALLOCATOR_STATIC_UNALIGNED, 
								ALLOCATOR_GUARD_PAGE } eAllocatorType;

// reported errors
typedef enum { 	FAULT_NO_FAULT, 
				FAULT_MEMORY_OVERWRITTEN, 
				FAULT_DIFFERENT_RESULT, 
				FAULT_DIFFERENT_OUT_BYTES, 
				FAULT_FRAMEWORK } eFaultType;

// Copied from Proptest as we wanted to have data Format Testing for Source pin also. 
// BYPASS_PORTCLS_HANG:
//
// Indicates that bogus handles for source pin creation should not come from
// the same device.  This will bypass a hang in portcls, BUG: 555413; however,
// it requires that a temp file be creatable at c:\
//
#define BYPASS_PORTCLS_HANG
class CScratchHandle {

private:

    HANDLE m_Handle;
    CKsFilter *m_Object;

public:

    //
    // CScratchHandle():
    //
    // Construct a new scratch handle for a given object.
    //
    CScratchHandle (
        IN CKsFilter *Object
        ) :
        m_Object (Object),
        m_Handle (NULL)
    {
    }

    //
    // ~CScratchHandle():
    //
    // Destruct the scratch handle.
    //
    ~CScratchHandle (
        );

    //
    // Open():
    //
    // Open the scratch handle.
    //
    DWORD
    Open (
        );

    //
    // GetHandle():
    //
    // Get the scratch handle.
    //
    HANDLE
    GetHandle (
        )
    {
        return m_Handle;
    }

};

BOOL SyncDeviceIoControl( IN HANDLE hFile,
                          IN DWORD dwIoControlCode,
                          IN LPVOID lpInBuffer,
                          IN DWORD nInBufferSize,
                          OUT LPVOID lpOutBuffer,
                          IN DWORD nOutBufferSize,
                          OUT LPDWORD lpBytesReturned );

// generic testing wrapper
class CGenericIoControl
{
    LPVOID      m_pIn;							// input data buffer for IOCTL
    ULONG       m_cbInSize;
    ULONG       m_cbOutSize;
    ULONG       m_cbInSizeParam;		//size of input data buffer
    ULONG       m_cbOutSizeParam;	//size of output data buffer
    LPVOID      m_pData;						//output data buffer
    HANDLE		m_hFile;						//handle to device - to send device io control
    DWORD      m_cbWritten;				//byte count
    eAllocatorType m_Allocator;			// to indicate allocator type
    eFaultType  m_FaultType; 				// to return faluth type
    ULONG       m_Ioctl;							//operation - IOCTL cdoe
    OVERLAPPED  m_ov;							//overlapped information
public:
	// constructors
    CGenericIoControl(IN HANDLE hFile,
                    	IN LPVOID pIn,
                    	IN ULONG cbInSize,
                    	IN ULONG cbInSizeParam,
                    	IN ULONG cbOutSize, 
                    	IN ULONG cbOutSizeParam,
                   	OPTIONAL IN eAllocatorType = ALLOCATOR_STATIC,
			OPTIONAL IN ULONG Ioctl = IOCTL_KS_PROPERTY);

	// destructors
    ~CGenericIoControl();

	// wrapper for DeviceIoControl
    DWORD  Test(IN BOOL fExpectedToFail , IN ULONG cbExpectedWrittenBytes, IN BOOL fAsync = FALSE );
    
	// accessor
	LPVOID GetData() const ;
	eFaultType GetFault() const;
	DWORD  GetWritten() const;
	void   GetOverlapped(OVERLAPPED** ppov);
};

typedef enum { 	NodeID_ULONGMAX, 						// Node Id = 0xFFFFFFFF
				NodeID_0x80000001, 			// Node Id  = 0x80000001/2
				ChannelID_ULONGMAX_Sub1, 		// Channel ID = FFFFFFFE - since FFFFFFFF is valid node id.
				ChannelID_0x80000001,	// Channel Id  = 0x80000001
				NULL_Buffer,									// sending Null in / output buffer
				Small_Buffer,									// Sending small in/out buffer
				Large_Buffer,									//Sending large Buffer			
				Valid_Test,                                                                  // valid test data
				PinFactory_BAD,                                                         //sending bad id for pin factory
				PinFactory_ULONG_MAX,                                             //sending bad id for pin factory
				Invalid_Data                                                                 // send invalid data
				} eTestCaseType;

//General testing parameters
DWORD WINAPI Generic_Property_KSNODEPROPERTY(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase, IN LPVOID pSetData, ULONG DataSize);
//DWORD Generic_Property_KSNODEPROPERTY(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase); // testcase used to pass invalid ID with other correct parameters for KSNODEPROPERTY
DWORD WINAPI Generic_Property_KSNODEPROPERTY_ULONG(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase, OPTIONAL IN PLONG pResultData = NULL); // testcase used to pass invalid ID with other correct parameters for KSNODEPROPERTY
//DWORD Generic_Property_KSNODEPROPERTY_BOOL(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase); // testcase used to pass invalid ID with other correct parameters for KSNODEPROPERTY
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase, OPTIONAL IN PLONG pResultData = NULL); // testcase used to pass invalid ID with other correct parameters for KSNODEPROPERTY
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_Basic_Support(IN PGAUDITPARAMS pParams );
DWORD WINAPI Generic_Property_KSAUDIOCHANNELNODEPROPERTY(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase, IN LPVOID pSetData, ULONG DataSize);// testcase used to pass invalid ID / channel number with other correct parameters for KSNODEPROPERTY_AUDIO_CHANNEL
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_NodeID_ULONGMAX(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_NodeID_ULONGMAX(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_ChannelID_0xFFFFFFFE(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_NodeID_0x80000001(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_NodeID_0x80000001(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_ChannelID_0x80000001(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_NULL_Buffer(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_NULL_Buffer(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_Small_Buffer(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_Small_Buffer(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_Large_Buffer(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_Large_Buffer(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_Valid_Test(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_Valid_Test(PGAUDITPARAMS pParams);
DWORD WINAPI Generic_Property_KSSYNTHNODEPROPERTY(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase);
DWORD WINAPI Generic_Property_KS3DBUFFERNODEPROPERTY(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase);
DWORD WINAPI Generic_Property_KS3DLISTENERNODEPROPERTY(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase);
VOID DisplayDataRange (PKSDATARANGE pDataRange);
VOID DisplayDataRangeAudio (PKSDATARANGE_AUDIO pDataRangeAudio );    


#endif
