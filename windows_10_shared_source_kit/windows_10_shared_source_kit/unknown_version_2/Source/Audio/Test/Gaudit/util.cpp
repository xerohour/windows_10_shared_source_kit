//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       util.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"
#include "channels.h"
#include "eqbands.h"

/*************************************************

    Function:

        CScratchHandle::Open

    Description:

        Open a scratch handle for a given object.

    Arguments:

        None

    Return Value:

        Success / Failure

*************************************************/

DWORD
CScratchHandle::
Open (
    )

{
    #ifdef BYPASS_PORTCLS_HANG
        PTSTR TempDir;
        TCHAR StaticFileName [128];
        PTSTR FileName = StaticFileName;
        BOOL FreeFileName = FALSE;

        DWORD tlen = GetEnvironmentVariable (
            TEXT("TEMP"),
            NULL,
            0
            );

        //
        // If we can't find the temp directory, write the file to c:\
        //
        if (!tlen) {
            _stprintf (FileName, TEXT("c:\\tmp%08p.tmp"), m_Object);
        } else {
            TempDir = new TCHAR [tlen + 1];
            FileName = new TCHAR [tlen + 17];

            if (!TempDir || !FileName) {
                if (TempDir) delete [] TempDir;
                if (FileName) delete [] FileName;
                return ERROR_NOT_ENOUGH_MEMORY;
            }

            DWORD nlen = GetEnvironmentVariable (
                TEXT("TEMP"),
                TempDir,
                tlen + 1
                );

            if (!nlen || nlen > tlen) {
                delete [] TempDir;
                delete [] FileName;
                return ERROR_BAD_ENVIRONMENT;
            }

            _stprintf (FileName, TEXT("%s\\tmp%08p.tmp"), TempDir, m_Object);
            delete [] TempDir;
            FreeFileName = TRUE;
        }

        m_Handle = CreateFile (
            FileName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE,
            NULL
            );

        if (FreeFileName) {
            delete [] FileName;
        }
    
        if (m_Handle == INVALID_HANDLE_VALUE) {
            m_Handle = NULL;
            return ERROR_INVALID_HANDLE;
        }
    #else // BYPASS_PORTCLS_HANG
        m_Handle = m_Object -> GetHandle ();
    #endif // BYPASS_PORTCLS_HANG

    return 0;

}

/*************************************************

    Function:

        CScratchHandle::~CScratchHandle

    Description:
        
        Close an existing scratch handle

*************************************************/

CScratchHandle::
~CScratchHandle (
    )
{

    #ifndef BYPASS_PORTCLS_HANG
        if (m_Handle) {
            CloseHandle (m_Handle);
        }
    #endif // BYPASS_PORTCLS_HANG
}


BOOL SyncDeviceIoControl( IN HANDLE hFile,
                          IN DWORD dwIoControlCode,
                          IN LPVOID lpInBuffer,
                          IN DWORD nInBufferSize,
                          OUT LPVOID lpOutBuffer,
                          IN DWORD nOutBufferSize,
                          OUT LPDWORD lpBytesReturned )
{
    BOOL success = FALSE;
    OVERLAPPED overlapped;

    *lpBytesReturned = 0;

    overlapped.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    if( NULL == overlapped.hEvent )
    {
        DWORD error = GetLastError();
        SLOG( eError, "ERROR: Could not create event object (error:%u)", error );

        return FALSE;
    }

    success = DeviceIoControl( hFile,
                               dwIoControlCode,
                               lpInBuffer,
                               nInBufferSize,
                               lpOutBuffer,
                               nOutBufferSize,
                               lpBytesReturned,
                               &overlapped );
    if( FALSE == success )
    {
        DWORD error = GetLastError();

        if( ERROR_IO_PENDING == error )
        {
            success = GetOverlappedResult( hFile, &overlapped, lpBytesReturned, TRUE );
        }

        if( FALSE == success )
        {
            error = GetLastError();

            if( ( ERROR_INSUFFICIENT_BUFFER == error || ERROR_MORE_DATA == error ) &&
                NULL == lpOutBuffer && 0 == nOutBufferSize )
            {
                // this was a request for the outbuffer size
                SLOG( eInfo2, "DeviceIoControl honored outbuffer size request (error:%u)", error );
            }
            else
            {
                SLOG( eError, "ERROR: Failed to call DeviceIoControl (error:%u)", error );
            }
        }
    }

    CloseHandle( overlapped.hEvent );

    return success;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGenericControl implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------
// Class: CGenericControl 
// Method: Initialization
//
// Description:
//  
//
// Parameters:
//
//      IN HANDLE hFile
//          handle to device - to send device io control
//      IN LPVOID pIn,
//          input data buffer for IOCTL
//      IN ULONG  cbInSize,
//          
//      IN ULONG  cbInSizeParam,
//          size of input data buffer
//      IN ULONG  cbOutSize, 
//          allocate outbuffer long ny number of bytes mentioned by cbOutSize. So input data/output data buffer will be long by cbOutsize    
//      IN ULONG  cbOutSizeParam,
//          size of output data buffer. In normal case it will equal to cboutsize. In case of finding overwrite-specify it more then cboutsize.
//      OPTIONAL IN eAllocatorType Allocator,
//          to indicate allocator type
//      OPTIONAL IN ULONG Ioctl
//          operation - IOCTL cdoe
//
// Return values (HRESULT):
//
//

CGenericIoControl::CGenericIoControl( IN HANDLE hFile,
                                      IN LPVOID pIn,
                                      IN ULONG  cbInSize,
                                      IN ULONG  cbInSizeParam,
                                      IN ULONG  cbOutSize,
                                      IN ULONG  cbOutSizeParam,
                                      OPTIONAL IN eAllocatorType Allocator,
                                      OPTIONAL IN ULONG Ioctl )
{
    m_hFile = hFile;                        // handle
    m_pIn = pIn;                            // input data
    m_cbInSize = cbInSize;                  // ## never used ...... ? ##
    m_cbOutSize = cbOutSize;                // size of output buffer allocated
    m_cbInSizeParam = cbInSizeParam;        // input size passed to DeviceIoControl
    m_cbOutSizeParam = cbOutSizeParam;      // output buffer size passed to DeviceIoControl

    m_Allocator = Allocator;
    m_FaultType = FAULT_NO_FAULT;
    m_cbWritten = 0;
    m_Ioctl = Ioctl;
    m_pData = NULL;

    memset( &m_ov, 0, sizeof(m_ov) );

    // no need to allocate memory
    if( 0 == cbOutSize)
    {
        return;
    }

    switch( Allocator )
    {
        case ALLOCATOR_STATIC:
            StaticMemoryInit();
            m_pData = StaticMemoryAllocAligned(7, cbOutSize);
            break;

        case ALLOCATOR_STATIC_UNALIGNED:
            StaticMemoryInit();
            m_pData = StaticMemoryAllocUnaligned(cbOutSize);
            break;

        case ALLOCATOR_DEFAULT:
            m_pData = new BYTE[cbOutSize];
            break;

        case ALLOCATOR_GUARD_PAGE:
            m_pData = GuardPageAllocate(cbOutSize);
            break;

        default:
            _ASSERT(FALSE);
    }

} // CGenericIoControl::CGenericIoControl

//-------------------------------------------------------------------------
// Class: CGenericControl 
// Method: Test
//
// Description:
//          This method sends device IOCTL to device.
//          It will then verify the result with the expected behavior of the device for given IOCTL.
//
// Parameters:
//
//      IN BOOL fFailureExpected
//          TRUE - expected failure for the test case, FALSE - Test should Pass
//      IN ULONG cbExpectedWritten,
//          Expected data to be wriiten by the driver
//      IN BOOL fAsync,
//          TRUE -  , FALSE - 
//
// Return values (DWORD):
//
//      System Error Code
//

DWORD CGenericIoControl::Test( IN BOOL fFailureExpected, IN ULONG cbExpectedWritten, IN BOOL fAsync )
{
    DWORD dwError = ERROR_SUCCESS;
    BOOL  fResult = TRUE;
    LPVOID lpMsgBuf = NULL; // used for logging the GetLastError string

    if( fFailureExpected != (BOOL)-1 && fFailureExpected != TRUE && fFailureExpected != FALSE )
    {
        SLOG( eError, "ERROR: fFailureExpected is %u.  Expected TRUE (%u) or FALSE (%u).",
                fFailureExpected, TRUE, FALSE );

        return ERROR_BAD_ARGUMENTS;
    }

    if( fAsync != TRUE && fAsync != FALSE )
    {
        SLOG( eError, "ERROR: fAsync is %u.  Expected TRUE (%u) or FALSE (%u).",
                fAsync, TRUE, FALSE );

        return ERROR_BAD_ARGUMENTS;
    }

    if( m_cbOutSize < m_cbOutSizeParam )
    {
        SLOG( eInfo2, "A/V WARNING: Size of IN/OUT Buffer (%u) is Smaller than Size Sent (%u) to DeviceIoControl",
                m_cbOutSize, m_cbOutSizeParam );
    }

    // check that allocation succeeded
    if( NULL == m_pData && 0 < m_cbOutSize )
    {
        SLOG( eError, "ERROR: Internal memory allocation failure" );
        m_FaultType = FAULT_FRAMEWORK;

        return ERROR_NOT_ENOUGH_MEMORY;
    }

    // if overlapped IO allocate overlapped struct
    if( FALSE != fAsync && ( NULL == m_ov.hEvent || INVALID_HANDLE == m_ov.hEvent ) )
    {
        m_ov.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
        if( NULL == m_ov.hEvent || INVALID_HANDLE == m_ov.hEvent )
        {
            dwError = GetLastError();
            SLOG( eError, "ERROR: Could not create event object (error:%u)", dwError );
            m_FaultType = FAULT_FRAMEWORK;

            return dwError;
        }
    }

    if( FALSE != fAsync )
    {
        fResult = ResetEvent( m_ov.hEvent );
        if( FALSE == fResult )
        {
            dwError = GetLastError();
            SLOG( eError, "ERROR: Could not resest event object (error:%u)", dwError );
            m_FaultType = FAULT_FRAMEWORK;

            return dwError;
        }
    }

    fResult = DeviceIoControl( m_hFile,
                               m_Ioctl,
                               m_pIn,
                               m_cbInSizeParam,
                               m_pData,
                               m_cbOutSizeParam,
                               &m_cbWritten,
                               (FALSE != fAsync) ? &m_ov : NULL );

    // get possible error
    dwError = (FALSE == fResult) ? GetLastError() : ERROR_SUCCESS;
    if( dwError == ERROR_IO_PENDING && FALSE != fAsync )
    {
        // wait on the overlapped event
        dwError = WaitForSingleObject( m_ov.hEvent, 30000 );  // wait 30 seconds
        if( WAIT_TIMEOUT == dwError )
        {
            // overlapped event timed out without being signaled
            SLOG( eError, "ERROR: Overlapped Event was NOT signaled within 30 seconds after DeviceIoControl call." );
            m_FaultType = FAULT_FRAMEWORK;

            return dwError;
        }
        else if( WAIT_FAILED == dwError )
        {
            dwError = GetLastError();
            SLOG( eError, "ERROR: Waiting on Overlapped Event returned error %u.", dwError );
            m_FaultType = FAULT_FRAMEWORK;

            return dwError;
        }

        fResult = ResetEvent( m_ov.hEvent );
        if( FALSE == fResult )
        {
            dwError = GetLastError();
            SLOG( eError, "ERROR: Could not resest event object (error:%u)", dwError );
            m_FaultType = FAULT_FRAMEWORK;

            return dwError;
        }

        fResult = TRUE;
    }
    else if( ERROR_SUCCESS != dwError )
    {
		SLOG( eInfo2, "DeviceIoControl returned 0x%X", dwError );

        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                       NULL, dwError,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPTSTR) &lpMsgBuf, 0, NULL );

        SLOG( eInfo2, "Error : %s", lpMsgBuf );

        LocalFree( lpMsgBuf );
        
    }

    // check invalid result
    if( fFailureExpected != (BOOL)-1 && fFailureExpected != FALSE && FALSE != fResult )
    {
        // expecting a failure, but DeviceIoControl passed
        SLOG(eInfo2, "DeviceIoControl Passed when expecting DeviceIoControl to Fail." );

        m_FaultType = FAULT_DIFFERENT_RESULT;
    }
    else if( fFailureExpected != (BOOL)-1 && fFailureExpected == FALSE && FALSE == fResult )
    {
        // not expecting a failure, but DeviceIoControl failed
        SLOG(eInfo2, "DeviceIoControl Failed when expecting DeviceIoControl to Pass." );

        m_FaultType = FAULT_DIFFERENT_RESULT;
    }

    if( ( ERROR_MORE_DATA == dwError || ERROR_INSUFFICIENT_BUFFER == dwError ) &&
             ( NULL == m_pData || 0 == m_cbOutSize ) )
    {
        // Passed a NULL buffer or buffer too small
        // m_cbWritten could be 0
        // m_cbWritten could be amount of data written
        // m_cbWritten could be the size the outbuffer needs to be
        cbExpectedWritten = (ULONG)-1;
    }

    // check expected number of bytes written
    if( m_cbWritten != cbExpectedWritten && cbExpectedWritten != (ULONG)-1 )
    {
        SLOG(eInfo2, "Actual bytes wrote %u.  Expected %u.", m_cbWritten, cbExpectedWritten );

        m_FaultType = FAULT_DIFFERENT_OUT_BYTES;
    }

    // check possible memory overwrite
    if( m_Allocator == ALLOCATOR_STATIC && 0 < m_cbOutSize )
    {
        if( FALSE == StaticMemoryCheckValidity( m_pData ) )
        {
            m_FaultType = FAULT_MEMORY_OVERWRITTEN;
        }
    }

    INCREMENT_INDENT();

    // log information
    if( m_FaultType != FAULT_NO_FAULT)
    {
        SLOG(eError, "ERROR:Failure information:");
        SLOG(eError, "      Context: pin=%ld node=%ld device=%ld",
                (PIN() != NULL) ? PIN()->m_nId : -1, (NODE() != NULL) ? NODE()->m_nId : -1, DEVICE()) ;

        if( m_Ioctl == IOCTL_KS_PROPERTY )
        {
            PKSPROPERTY pProp = (PKSPROPERTY) m_pIn;
            if( pProp->Flags & KSPROPERTY_TYPE_SET )
            {
                SLOG(eError, "      DeviceIoControl: returned 0x%08X during property set request", dwError);
            }
            else if( pProp->Flags & KSPROPERTY_TYPE_GET )
            {
                SLOG(eError, "      DeviceIoControl: returned 0x%08X during property Get request", dwError);
            }
        }
        else
        {
            SLOG(eError, "      DeviceIoControl: returned 0x%08X ", dwError);
        }

	    // Workaround for 305356 to silence TEX runs. This bug was won't fix, and we can't resolve failures agains won't fix bugs.
	    // This is more of a process workaround - it should be OK to have failures of tests because of a bug which was decided not to fix.
	    if ((dwError == 0x1f) && ((KSPROPERTY*)m_pIn)->Id == KSPROPERTY_PIN_NAME) 
	    {
		    OSVERSIONINFOEXW osVersionInfoEx;
		    ZeroMemory(&osVersionInfoEx, sizeof(osVersionInfoEx));
		    osVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
		    osVersionInfoEx.dwMajorVersion = 6;
		    osVersionInfoEx.dwMinorVersion = 3;

		    DWORDLONG dwlConditionMask = 0;
		    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION|VER_MINORVERSION , VER_LESS_EQUAL);
		    if (VerifyVersionInfoW(&osVersionInfoEx, VER_MAJORVERSION, dwlConditionMask))
		    {
			    SLOG( eInfo2, "This is bug 305356 which was Won’t Fixed for Windows 8.1");
			    m_FaultType = FAULT_NO_FAULT;
		    }
		    else
		    {
			    SLOG( eError, "This is bug 305356 which should be fixed for this version of Windows");
			    m_FaultType = FAULT_FRAMEWORK;
		    }
	    }

        switch( m_FaultType )
        {
            case FAULT_DIFFERENT_RESULT: 
                SLOG( eError, "      Type: unexpected result" );
                break;

            case FAULT_DIFFERENT_OUT_BYTES:
                SLOG( eError, "      Type: unexpected number of bytes wrote. Expected %ld  but Wrote %ld",
                        cbExpectedWritten, m_cbWritten );
                break;

            case FAULT_MEMORY_OVERWRITTEN: 
                SLOG( eError, "      Type: detected memory overwrite" );
                break;

            default:
                _ASSERT(FALSE);
        }
    }

    DECREMENT_INDENT()

    return dwError;
} // CGenericIoControl::Test


LPVOID CGenericIoControl::GetData()  const
{ return m_pData; } // CGenericIoControl::GetData
eFaultType CGenericIoControl::GetFault() const
{ return m_FaultType; } // CGenericIoControl::GetFault
DWORD  CGenericIoControl::GetWritten() const
{ return m_cbWritten; } // CGenericIoControl::GetWritten
void   CGenericIoControl::GetOverlapped(OVERLAPPED** ppOverlappedIo)
{ *ppOverlappedIo = &m_ov; } // CGenericIoControl::GetOverlapped


CGenericIoControl::~CGenericIoControl()
{
    if( ALLOCATOR_DEFAULT == m_Allocator && m_pData )
    {
        delete[] m_pData;
        m_pData = NULL;
    }
    else if( ALLOCATOR_GUARD_PAGE == m_Allocator )
    {
        GuardPageFree( m_pData );
        m_pData = NULL;
    }

    if( NULL != m_ov.hEvent && INVALID_HANDLE != m_ov.hEvent )
    {
        // close overlapped event and set to NULL
        SAFE_CLOSE_HANDLE( m_ov.hEvent );
    }

} // CGenericIoControl::~CGenericIoControl

VOID DisplayDataRange (PKSDATARANGE pDataRange )    {
    SLOG(eInfo2, "Datarange->Formatsize = %d", pDataRange->FormatSize);
    SLOG(eInfo2, "Datarange->Flags = 0x%x", pDataRange->Flags);
    SLOG(eInfo2, "Datarange->SampleSize = 0x%x", pDataRange->SampleSize);
    SLOG(eInfo2, "Datarange->Reserved = 0x%x", pDataRange->Reserved);
    SLOG(eInfo2, "Datarange->MajorFormat = %x-%x-%x-%x%x-%x%x%x%x%x%x ", DisplayGUID(pDataRange->MajorFormat));
    SLOG(eInfo2, "Datarange->SubFormat = %x-%x-%x-%x%x-%x%x%x%x%x%x ", DisplayGUID(pDataRange->SubFormat));
    SLOG(eInfo2, "Datarange->Specifier = %x-%x-%x-%x%x-%x%x%x%x%x%x ", DisplayGUID(pDataRange->Specifier));
}

VOID DisplayDataRangeAudio (PKSDATARANGE_AUDIO pDataRangeAudio )    {
    SLOG(eInfo2, "Displaying KSDATARANGE_AUDIO. DataRange");
    DisplayDataRange ((PKSDATARANGE)(&(pDataRangeAudio->DataRange)));
    SLOG(eInfo2, "KSDATARANGE_AUDIO.MaximumChannels = 0x%x", pDataRangeAudio->MaximumChannels);
    SLOG(eInfo2, "KSDATARANGE_AUDIO.MinimumBitsPerSample  = 0x%x", pDataRangeAudio->MinimumBitsPerSample );
    SLOG(eInfo2, "KSDATARANGE_AUDIO.MaximumBitsPerSample  = 0x%x", pDataRangeAudio->MaximumBitsPerSample );
    SLOG(eInfo2, "KSDATARANGE_AUDIO.MinimumSampleFrequency  = 0x%x", pDataRangeAudio->MinimumSampleFrequency );
    SLOG(eInfo2, "KSDATARANGE_AUDIO.MaximumSampleFrequency  = 0x%x", pDataRangeAudio->MaximumSampleFrequency );
}    
DWORD Generic_Property_KSNODEPROPERTY( IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase, IN LPVOID pSetData, ULONG DataSize )
{
    ULONG OutBufferSize = DataSize;     // Output bufferzise
    ULONG numBytesToCopy = DataSize;    // number of bytes to memcpy
    DWORD dwResult = FNS_FAIL;          // used for test result
    errno_t errNo = 0;                  // errno from memcpy routine
    BOOL bExpectedTestResult = TRUE;    // as normally test case must fail as we are passing invalid parameters
    ULONG cbExpectedWritten = 0;        // as normally test case must fail as we are passing invalid parameters

    switch( TestCase )
    {

        case NodeID_ULONGMAX:
            pParams->NodeProp.NodeId = ULONG_MAX;  //setting node id to invalid node id and passing all other parameters correct
            break;

        case NodeID_0x80000001:
            pParams->NodeProp.NodeId = 0x80000001;  //setting node id to invalid node id and passing all other parameters correct
            break;

        case NULL_Buffer:
            OutBufferSize = 0;  //setting channel as invalid.
            break;

        case Small_Buffer:
            OutBufferSize = OutBufferSize - 1;  //setting channel as invalid.
            break;

        case Large_Buffer:
            OutBufferSize = OutBufferSize + 1;  // lying about buffer size
            // ks.sys might step in, in which case DeviceIoControl should fail
            // if ks.sys doesn't step in, then the overwrite check is necessary
            bExpectedTestResult = -1;
            cbExpectedWritten = -1;
            break;

        case Valid_Test:
            cbExpectedWritten = OutBufferSize;
            bExpectedTestResult = FALSE; // should succeed
            break;

        default:
            _ASSERT(FALSE);
            return FNS_FAIL;
    }

    // since for set request we are not expecting any data to be writtne back. 
    // Ideally we should expect zero bytes as written, however there is a bug in some of drivers that they are
    // returning number during set request. However this was take care by KS so that IO manager will not copy data back
    // to user mode for Property Set request
    if (pParams->NodeProp.Property.Flags & KSPROPERTY_TYPE_SET)
    {
        cbExpectedWritten = (ULONG) -1;
    }

    CGenericIoControl gnt( pParams->hFile,
                           &pParams->NodeProp,
                           pParams->cbSize,
                           pParams->cbSize,
                           OutBufferSize,
                           OutBufferSize );

    if( DataSize > OutBufferSize )
    {
        numBytesToCopy = OutBufferSize;
    }

    errNo = memcpy_s( gnt.GetData(), OutBufferSize, pSetData, numBytesToCopy );
    if( 0 != errNo )
    {
        SLOG( eError, "Failed to copy %u bytes to DeviceIoControl Set Buffer with errno: %u", numBytesToCopy, errNo );

        return FNS_FAIL;
    }

    dwResult = gnt.Test( bExpectedTestResult, cbExpectedWritten );
    if( ERROR_SUCCESS != dwResult )
    {
        SLOG( eInfo2, "Test failed with error code %u (0x%x)", dwResult, dwResult );
    }

    if( gnt.GetFault() != FAULT_NO_FAULT )
    {
        return FNS_FAIL;
    }
    else
    {
       return FNS_PASS;
    }
}

// Testing for in - KSNODEPROPERTY and result LONG/ULONG and BOOL as BOOL is also 32 bit unsigned number.
// if we want the result back then caller has to send pointer to LONG.
DWORD Generic_Property_KSNODEPROPERTY_ULONG( IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase , OPTIONAL IN PLONG pResultData )
{
    ULONG OutBufferSize = 0;            // Output bufferzise
    ULONG numBytesToCopy = 0;           // number of bytes to copy for set
    DWORD dwResult = FNS_FAIL;          // used for test result
    BOOL bExpectedTestResult = TRUE;    // as normally test case must fail as we are passing invalid parameters
    ULONG cbExpectedWritten = 0;        // as normally test case must fail as we are passing invalid parameters
    errno_t errNo = 0;                  // errno returned from memcpy routine

    LONG tLONGData = 1;
    OutBufferSize = numBytesToCopy = sizeof(LONG);

    switch( pParams->NodeProp.Property.Id )
    {

        case KSPROPERTY_AUDIO_CHORUS_LEVEL:
            tLONGData = 0x00010000; // 100% Chorus
            break;

        case KSPROPERTY_AUDIO_DEMUX_DEST: 
            tLONGData = 1;          // TODO : to find out about pin id
            break;

        case KSPROPERTY_AUDIO_MUX_SOURCE: 
            tLONGData = 1;          // TODO : to find out about pin id
            break;

        case KSPROPERTY_AUDIO_QUALITY: 
            tLONGData = KSAUDIO_QUALITY_BASIC;
            break;

        case KSPROPERTY_AUDIO_REVERB_LEVEL:
            tLONGData = 0x00010000;
            break;

        case KSPROPERTY_AUDIO_SAMPLING_RATE:
            tLONGData = 48000;
            break;

        case KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE:
            tLONGData = 0;
            break;

        case KSPROPERTY_AUDIO_SURROUND_ENCODE:
            tLONGData = 0;
            break;

        case KSPROPERTY_AUDIO_CPU_RESOURCES:
            tLONGData = 0;
            break;

        case KSPROPERTY_AUDIO_STEREO_SPEAKER_GEOMETRY:
            tLONGData = 5;
            break;

// Acoustic Echo Cancellation is no longer done this way
// (in Vista, use SysEffects)
#if (NTDDI_VERSION < NTDDI_VISTA)
        case KSPROPERTY_AEC_MODE:
            tLONGData = AEC_MODE_PASS_THROUGH;
            break;

        case KSPROPERTY_AEC_NOISE_FILL_ENABLE:
            tLONGData = 1;
            break;

        case KSPROPERTY_AEC_STATUS:
            tLONGData = AEC_STATUS_FD_HISTORY_UNINITIALIZED;
            break;
#endif
    }
    
    switch( TestCase )
    {

        case NodeID_ULONGMAX:
            pParams->NodeProp.NodeId = ULONG_MAX;  //setting node id to invalid node id and passing all other parameters correct
            break;

        case NodeID_0x80000001:
            pParams->NodeProp.NodeId = 0x80000001;  //setting node id to invalid node id and passing all other parameters correct
            break;

        case NULL_Buffer:
            OutBufferSize = 0;  //setting channel as invalid.
            break;

        case Small_Buffer:
            OutBufferSize = OutBufferSize - 1;  //sending smaller buffer than required one
            break;

        case Large_Buffer:
            OutBufferSize = OutBufferSize + 1;  // lying about buffer size
            // ks.sys might step in, in which case DeviceIoControl should fail
            // if ks.sys doesn't step in, then the overwrite check is necessary
            bExpectedTestResult = -1;
            cbExpectedWritten = -1;
            break;

        case Valid_Test:
            cbExpectedWritten = OutBufferSize;
            bExpectedTestResult = FALSE; // should succeed
            break;

        default:
            _ASSERT(FALSE);
            return FNS_FAIL;
    }

    // since for set request we are not expecting any data to be writtne back. 
    // Ideally we should expect zero bytes as written, however there is a bug in some of drivers that they are returning
    // number during set request. However this was take care by KS so that IO manager will not copy data back to user
    // mode for Property Set request
    if( pParams->NodeProp.Property.Flags & KSPROPERTY_TYPE_SET )
    {
        cbExpectedWritten = (ULONG) -1;
    }

    CGenericIoControl gnt( pParams->hFile,
                           &pParams->NodeProp,
                           pParams->cbSize,
                           pParams->cbSize,
                           OutBufferSize,   // size of GetData()
                           OutBufferSize ); // size sent in DeviceIoControl

    if( OutBufferSize < sizeof(LONG) )
    {
        numBytesToCopy = OutBufferSize;
    }

    errNo = memcpy_s( gnt.GetData(), OutBufferSize, &tLONGData, numBytesToCopy );
    if( 0 != errNo )
    {
        SLOG( eError, "Failed to copy %u Bytes to DeviceIoControl Set Buffer with errno: %u", numBytesToCopy, errNo );

        return FNS_FAIL;
    }

    dwResult = gnt.Test( bExpectedTestResult, cbExpectedWritten, TRUE );
    if( ERROR_SUCCESS != dwResult )
    {
        SLOG( eInfo2, "Test failed with error code %u (0x%x)", dwResult, dwResult );
    }

    if( gnt.GetFault() != FAULT_NO_FAULT )
    {
        return FNS_FAIL;
    }
    else
    {
        dwResult = FNS_PASS;
    }

    // TODO: in case of get and set we have to verify results
    if( TestCase == Valid_Test )
    {
        tLONGData = *((LONG *)gnt.GetData());

        if( NULL != pResultData )
        {
            *pResultData = tLONGData;
        }

        SLOG( eInfo2, "Value received for the property = %d", tLONGData );
        switch( pParams->NodeProp.Property.Id )
        {

            case KSPROPERTY_AUDIO_CHORUS_LEVEL:
                break;

            case KSPROPERTY_AUDIO_DEMUX_DEST:
                break;

            case KSPROPERTY_AUDIO_MUX_SOURCE:

                break;
            case KSPROPERTY_AUDIO_QUALITY:
                break;

            case KSPROPERTY_AUDIO_REVERB_LEVEL:
                break;

            case KSPROPERTY_AUDIO_SAMPLING_RATE:
                break;

            case KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE:
                break;

            case KSPROPERTY_AUDIO_SURROUND_ENCODE:
                break;

            case KSPROPERTY_AUDIO_CPU_RESOURCES:
                if( (tLONGData == KSAUDIO_CPU_RESOURCES_HOST_CPU) || (tLONGData == KSAUDIO_CPU_RESOURCES_NOT_HOST_CPU) )
                {
                    dwResult = FNS_PASS;
                }
                else
                {
                    dwResult = FNS_FAIL;
                }
                break;

            case KSPROPERTY_AUDIO_STEREO_SPEAKER_GEOMETRY:
                break;
// Acoustic Echo Cancellation is no longer done this way
// (in Vista, use SysEffects)
#if (NTDDI_VERSION < NTDDI_VISTA)
            case KSPROPERTY_AEC_MODE:
                break;

            case KSPROPERTY_AEC_NOISE_FILL_ENABLE:
                break;

            case KSPROPERTY_AEC_STATUS:
                break;
#endif
        }
    }

    return dwResult;
}

// get a channel count, then test channels one at a time
DWORD Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase, OPTIONAL IN PLONG pResultData) {
    DWORD dwResult = FNS_PASS;
    ULONG ulChannels;
    BOOL bBasicSupport;
    BOOL bBasicSupportMultichannel;

    dwResult = GetChannelCount(
        pParams->hFile,
        pParams->NodeProp.Property.Set,
        pParams->NodeProp.Property.Id,
        pParams->NodeProp.NodeId,
        &ulChannels,
        &bBasicSupport,
        &bBasicSupportMultichannel
    );

    if (FNS_PASS != dwResult) {
        SLOG(eError, "ERROR: Could not query number of channels");
        return dwResult;
    }

    if (
        NTDDI_VERSION >= NTDDI_VISTA &&
        bBasicSupport &&
        !bBasicSupportMultichannel
    ) {
        SLOG(eError, "ERROR: Vista drivers are required to set KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL for basic support requests");
        return FNS_FAIL;
    }

    SLOG(eInfo1, "Number of channels: %u", ulChannels);

    // known number of channels... iterate over them
    for (ULONG i = 0; i < ulChannels; i++) {
        SLOG(eInfo1, "Trying channel %u (0-based) of %u...", i, ulChannels);
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG_Channel(i, pParams, TestCase, pResultData);
        if (FNS_PASS != dwResult) {
            SLOG(eError, "ERROR: Channel %u (0-based) of %u failed, failing test case", i, ulChannels);
            return dwResult;
        } else {
            SLOG(eInfo1, "Channel %u (0-based) of %u passed", i, ulChannels);
        }
    }

    return dwResult;
}

// Testing for KSPROPERTY_TYPE_BASICSUPPORT (Basic Support Queries for Audio Properties)
DWORD Generic_Property_KSNODEPROPERTY_AUDIO_Basic_Support(IN PGAUDITPARAMS pParams)
{
    // we have to run this test for only once. 
    if (pParams->NodeProp.Property.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;

    ULONG OutBufferSize = 0;    //Output buffersize
    DWORD dwResult = FNS_PASS;     //used for test result
    ULONG AccessFlag = 0;
    KSPROPERTY_DESCRIPTION PropertyDes = {0};
    BOOL bExpectedTestResult = FALSE;    
    ULONG cbExpectedWritten = 0;    //as normally test case must fail as we are passing invalid parameters 
    KSNODEPROPERTY_AUDIO_CHANNEL NodeAudioChannelProperty = {0};
    NodeAudioChannelProperty.NodeProperty = pParams->NodeProp;
    NodeAudioChannelProperty.Channel = 0; // basic support request ignores the channel, but let's be safe and use 0
    NodeAudioChannelProperty.Reserved = 0;
    //We are only interested in BASIC_SUPPORT so remove TYPE_GET and TYPE_SET flags
    NodeAudioChannelProperty.NodeProperty.Property.Flags &= (~(KSPROPERTY_TYPE_SET) & ~(KSPROPERTY_TYPE_GET));
    //Add BASIC_SUPPORT flags because that is what is being tested
    NodeAudioChannelProperty.NodeProperty.Property.Flags |= KSPROPERTY_TYPE_BASICSUPPORT;
    
    // first send ULONG size buffer 
    OutBufferSize = sizeof(ULONG);                
    CGenericIoControl gnt_ulong(  pParams->hFile, &NodeAudioChannelProperty, 
                            sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                            OutBufferSize, OutBufferSize);
    cbExpectedWritten = sizeof(ULONG);
    dwResult = gnt_ulong.Test(bExpectedTestResult, cbExpectedWritten,TRUE);
    if( ERROR_SUCCESS != dwResult )
    {
        SLOG( eInfo2, "Test failed with error code %u (0x%x)", dwResult, dwResult );
    }

    if (gnt_ulong.GetFault() != FAULT_NO_FAULT) {
        SLOG (eError, "Passing buffer of ULONG size for the Basic support request failed");
        return FNS_FAIL;
    }

    {
        // send one byte buffer
        OutBufferSize = 1;                
        CGenericIoControl gnt_byte(  pParams->hFile, &NodeAudioChannelProperty, 
                                sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                                OutBufferSize, OutBufferSize);
        cbExpectedWritten = 0;
        dwResult = gnt_byte.Test(TRUE, cbExpectedWritten,TRUE);
        if (gnt_byte.GetFault() != FAULT_NO_FAULT || dwResult != ERROR_INSUFFICIENT_BUFFER) {
            SLOG (eError, "Passing buffer of 1 byte size for the Basic support request failed");
            return FNS_FAIL;
        }
    }

    // get the result fo access flag
    AccessFlag = *(ULONG *)(gnt_ulong.GetData());
    if ( AccessFlag & KSPROPERTY_TYPE_BASICSUPPORT )    {   // that means that driver provides further support for basic-support property requests
        OutBufferSize = sizeof(KSPROPERTY_DESCRIPTION);
        CGenericIoControl gnt_desc(  pParams->hFile, &NodeAudioChannelProperty, 
                                 sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                                 OutBufferSize, OutBufferSize);    
        cbExpectedWritten = sizeof(KSPROPERTY_DESCRIPTION);    
        dwResult = gnt_desc.Test(bExpectedTestResult, cbExpectedWritten,TRUE);
        if( ERROR_SUCCESS != dwResult )
        {
            SLOG( eInfo2, "Test failed with error code %u (0x%x)", dwResult, dwResult );
        }

        if (gnt_desc.GetFault() != FAULT_NO_FAULT) {
            SLOG (eError, "Passing buffer of KSPROPERTY_DESCRIPTION size for the Basic support request failed");
            return FNS_FAIL;
        }
        PropertyDes = *(KSPROPERTY_DESCRIPTION*)(gnt_desc.GetData());
        OutBufferSize = PropertyDes.DescriptionSize;

        //Check the DescriptionSize
        SLOG(eError, "Validate DescriptionSize for Node Properties");
        //Validate that the description size is not less than sizeof(KSPROPERTY_DESCRIPTION) and
        //that it is not more than 262144 bytes (1/4 MB), this upper limit comes from wdmaud.sys
        //see definition for MEMORY_LIMIT_CHECK in robust.c in wdmaud.sys
        //This validation check stems from the check that wdmaud.sys does before allocating
        //memory for the driver (kmxlQueryPropertyRange in kmxltop.c in wdmaud.sys).
        if(OutBufferSize < sizeof(KSPROPERTY_DESCRIPTION) ||
            OutBufferSize > 262144 ) 
        {       
            SLOG(eError, "ERROR: Property Call returned DescriptionSize = %d", OutBufferSize);
            return FNS_FAIL;
        }

        CGenericIoControl gnt_full(  pParams->hFile, &NodeAudioChannelProperty, 
                                 sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                                 OutBufferSize, OutBufferSize);    
        cbExpectedWritten = OutBufferSize;    
        dwResult = gnt_full.Test(bExpectedTestResult, cbExpectedWritten,TRUE);
        if (gnt_full.GetFault() != FAULT_NO_FAULT) {
            SLOG (eError, "Passing buffer of size %u for the Basic support request failed", OutBufferSize);
            return FNS_FAIL;
        }

    }
    else    {
        SLOG (eInfo2, "Driver does not support basic support fruther!!!!!");
    }
        
    return dwResult;
}

DWORD Generic_Property_KSAUDIOCHANNELNODEPROPERTY(IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase, IN LPVOID pSetData, ULONG DataSize) {
    DWORD dwResult = FNS_PASS;
    ULONG ulChannels;
    BOOL bBasicSupport;
    BOOL bBasicSupportMultichannel;

    dwResult = GetChannelCount(
        pParams->hFile,
        pParams->NodeProp.Property.Set,
        pParams->NodeProp.Property.Id,
        pParams->NodeProp.NodeId,
        &ulChannels,
        &bBasicSupport,
        &bBasicSupportMultichannel
    );

    if (FNS_PASS != dwResult) {
        SLOG(eError, "ERROR: Could not query number of channels");
        return dwResult;
    }

    if (
        NTDDI_VERSION >= NTDDI_VISTA &&
        bBasicSupport &&
        !bBasicSupportMultichannel
    ) {
        SLOG(eError, "ERROR: Vista drivers are required to set KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL for basic support requests");
        return FNS_FAIL;
    }

    SLOG(eInfo1, "Number of channels: %u", ulChannels);

    // known number of channels... iterate over them
    for (ULONG i = 0; i < ulChannels; i++) {
        dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY_Channel(i, pParams, TestCase, pSetData, DataSize);
        if (FNS_PASS != dwResult) {
            SLOG(eError, "ERROR: Channel #%u of %u failed", i + 1, ulChannels);
            return dwResult;
        }
    }

    return dwResult;
}

// for testing of KSPROPSETID_Synth properties for Invalid Node ID
DWORD Generic_Property_KSSYNTHNODEPROPERTY( IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase )
{
    ULONG OutBufferSize = 0;            // Output buffersize
    DWORD dwResult = FNS_FAIL;          // used for test result
    BOOL bExpectedTestResult = TRUE;    // as normally test case must fail as we are passing invalid parameters
    ULONG cbExpectedWritten = 0;        // as normally test case must fail as we are passing invalid parameters

    switch(pParams->NodeProp.Property.Id)   // nodeprop / prop / pinprop all has same KSPROPERTY
    {

        case KSPROPERTY_SYNTH_CAPS:
            OutBufferSize = sizeof(SYNTHCAPS);
            break;

        case KSPROPERTY_SYNTH_CHANNELGROUPS:
            OutBufferSize = sizeof(ULONG);
            break;

        case KSPROPERTY_SYNTH_LATENCYCLOCK:
            OutBufferSize = sizeof(ULONGLONG);
            break;

        case KSPROPERTY_SYNTH_RUNNINGSTATS:
            OutBufferSize = sizeof(SYNTH_STATS);
            break;

        default:
            _ASSERT(FALSE);

    }

    switch( TestCase )
    {
        case NodeID_ULONGMAX:
            pParams->NodeProp.NodeId = ULONG_MAX;  //setting node id to invalid node id and passing all other parameters correct
            break;

        case NodeID_0x80000001:
            pParams->NodeProp.NodeId = 0x80000001;  //setting node id to invalid node id and passing all other parameters correct
            break;

        case NULL_Buffer:
            OutBufferSize = 0;  //sending a NULL buffer
            break;

        case Small_Buffer:
            OutBufferSize = OutBufferSize - 1;  //sending small buffer than required one
            break;

        case Large_Buffer:
            OutBufferSize = OutBufferSize + 1;  // lying about buffer size
            // ks.sys might step in, in which case DeviceIoControl should fail
            // if ks.sys doesn't step in, then the overwrite check is necessary
            bExpectedTestResult = -1;
            cbExpectedWritten = -1;
            break;

        case Valid_Test:
            cbExpectedWritten = OutBufferSize;
            bExpectedTestResult = FALSE; // should succeed
            break;

        default:
            _ASSERT(FALSE);
            return FNS_FAIL;
    }

    // since for set request we are not expecting any data to be writtne back.
    // Ideally we should expect zero bytes as written, however there is a bug in some of drivers that they are returning number during set request. However this was take care by KS so that IO manager will not copy data back to user mode for Property Set request
    if( pParams->NodeProp.Property.Flags & KSPROPERTY_TYPE_SET )
    {
        cbExpectedWritten = (ULONG) -1;
    }

    CGenericIoControl gnt( pParams->hFile,
                           &pParams->NodeProp,
                           pParams->cbSize,
                           pParams->cbSize,
                           OutBufferSize,   // size of GetData()
                           OutBufferSize ); // size sent to DeviceIoControl

    dwResult = gnt.Test( bExpectedTestResult, cbExpectedWritten );
    if( ERROR_SUCCESS != dwResult )
    {
        SLOG( eInfo2, "Test failed with error code %u (0x%x)", dwResult, dwResult );
    }

    if( gnt.GetFault() != FAULT_NO_FAULT )
    {
        return FNS_FAIL;
    }
    else
    {
       return FNS_PASS;
    }
}


// for testing of KSPROPSETID_DirectSound3DBuffer properties for Invalid Node ID
DWORD Generic_Property_KS3DBUFFERNODEPROPERTY( IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase )
{
    ULONG OutBufferSize;                // Output bufferzise
    ULONG sizeOfSetDataInBytes = 0;     // the size of the data pointed to by pSetData
    ULONG numBytesToCopy = 0;           // number of bytes to copy
    LPVOID pSetData = NULL;             // pointer to hold data required to send for SET request
    DWORD dwResult = FNS_FAIL;          // used for test result
    errno_t errNo = 0;                  // errno from memcpy routine
    BOOL bExpectedTestResult = TRUE;    // as normally test case must fail as we are passing invalid parameters
    ULONG cbExpectedWritten = 0;        // as normally test case must fail as we are passing invalid parameters

    KSDS3D_BUFFER_ALL b_allValue;
    KSDS3D_BUFFER_CONE_ANGLES b_canglesValue;
    DS3DVECTOR vectorValue;
    FLOAT fValue;

    switch( pParams->NodeProp.Property.Id )   // nodeprop / prop / pinprop all has same KSPROPERTY parameter in pParams
    {

        case KSPROPERTY_DIRECTSOUND3DBUFFER_ALL:
            OutBufferSize = numBytesToCopy = sizeOfSetDataInBytes = sizeof(KSDS3D_BUFFER_ALL);
            b_allValue.Mode = DS3DMODE_NORMAL;
            pSetData = &b_allValue;
            break;

        case KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES:
            OutBufferSize = numBytesToCopy = sizeOfSetDataInBytes = sizeof(KSDS3D_BUFFER_CONE_ANGLES);
            pSetData = &b_canglesValue;
            break;

        case KSPROPERTY_DIRECTSOUND3DBUFFER_CONEORIENTATION:
            OutBufferSize = numBytesToCopy = sizeOfSetDataInBytes = sizeof(DS3DVECTOR);
            pSetData = &vectorValue;
            break;

        case KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE:
            OutBufferSize = numBytesToCopy = sizeOfSetDataInBytes = sizeof(FLOAT);
            pSetData = &fValue;
            break;

        case KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE:
            OutBufferSize = numBytesToCopy = sizeOfSetDataInBytes = sizeof(FLOAT);
            pSetData = &fValue;
            break;

        case KSPROPERTY_DIRECTSOUND3DBUFFER_POSITION:
            OutBufferSize = sizeOfSetDataInBytes = sizeof(DS3DVECTOR);
            pSetData = &vectorValue;
            break;

        case KSPROPERTY_DIRECTSOUND3DBUFFER_VELOCITY:
            OutBufferSize = numBytesToCopy = sizeOfSetDataInBytes = sizeof(DS3DVECTOR);
            pSetData = &vectorValue;
            break;

        default:
            _ASSERT(FALSE);

    }

    switch (TestCase)
    {

        case NodeID_ULONGMAX:
            pParams->NodeProp.NodeId = ULONG_MAX;  //setting node id to invalid node id and passing all other parameters correct
            break;

        case NodeID_0x80000001:
            pParams->NodeProp.NodeId = 0x80000001;  //setting node id to invalid node id and passing all other parameters correct
            break;

        case NULL_Buffer:
            OutBufferSize = 0;  //setting channel as invalid.
            cbExpectedWritten = 0;
            break;

        case Small_Buffer:
            OutBufferSize = OutBufferSize -1;  //setting channel as invalid.
            break;

        case Large_Buffer:
            OutBufferSize = OutBufferSize + 1;  // lying about buffer size
            // ks.sys might step in, in which case DeviceIoControl should fail
            // if ks.sys doesn't step in, then the overwrite check is necessary
            bExpectedTestResult = -1;
            cbExpectedWritten = -1;
            break;

        case Valid_Test:
            cbExpectedWritten = OutBufferSize;
            bExpectedTestResult = FALSE; // should succeed
            break;

        default:
            _ASSERT(FALSE);
            return FNS_FAIL;
    }

    // since for set request we are not expecting any data to be writtne back. 
    // Ideally we should expect zero bytes as written, however there is a bug in some of drivers that they are
    // returning number during set request. However this was take care by KS so that IO manager will not copy data
    // back to user mode for Property Set request
    if( pParams->NodeProp.Property.Flags & KSPROPERTY_TYPE_SET )
    {
        cbExpectedWritten = (ULONG) -1;
    }

    CGenericIoControl gnt( pParams->hFile,
                           &pParams->NodeProp,
                           pParams->cbSize,
                           pParams->cbSize,
                           OutBufferSize,   // size of GetData()
                           OutBufferSize ); // size sent to DeviceIoControl

    if( OutBufferSize < sizeOfSetDataInBytes )
    {
        numBytesToCopy = OutBufferSize;
    }

    errNo = memcpy_s( gnt.GetData(), OutBufferSize, pSetData, numBytesToCopy );
    if( 0 != errNo )
    {
        SLOG( eError, "Failed to copy %u bytes to DeviceIoControl Set Buffer with errno: %u", numBytesToCopy, errNo );

        return FNS_FAIL;
    }

    dwResult = gnt.Test( bExpectedTestResult, cbExpectedWritten );
    if( ERROR_SUCCESS != dwResult )
    {
        SLOG( eInfo2, "Test failed with error code %u (0x%x)", dwResult, dwResult );
    }

    if( gnt.GetFault() != FAULT_NO_FAULT )
    {
        return FNS_FAIL;
    }
    else
    {
       return FNS_PASS;
    }
}


// for testing of KSPROPSETID_DirectSound3DListener properties for Invalid Node ID
DWORD Generic_Property_KS3DLISTENERNODEPROPERTY( IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase )
{
    ULONG OutBufferSize = 0;            // Output buffersize
    DWORD dwResult = FNS_FAIL;          // used for test result
    BOOL bExpectedTestResult = TRUE;    // as normally test case must fail as we are passing invalid parameters
    ULONG cbExpectedWritten = 0;        // as normally test case must fail as we are passing invalid parameters

    switch(pParams->NodeProp.Property.Id)   // nodeprop / prop / pinprop all has same KSPROPERTY parameter in pParams
    {

        case KSPROPERTY_DIRECTSOUND3DLISTENER_ALL:
            OutBufferSize = sizeof(KSDS3D_LISTENER_ALL);
            break;

        case KSPROPERTY_DIRECTSOUND3DLISTENER_DISTANCEFACTOR:
            OutBufferSize = sizeof(FLOAT);
            break;

        case KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR:
            OutBufferSize = sizeof(FLOAT);
            break;

        case KSPROPERTY_DIRECTSOUND3DLISTENER_ORIENTATION:
            OutBufferSize = sizeof(KSDS3D_LISTENER_ORIENTATION);
            break;

        case KSPROPERTY_DIRECTSOUND3DLISTENER_POSITION:
            OutBufferSize = sizeof(DS3DVECTOR);
            break;

        case KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR:
            OutBufferSize = sizeof(FLOAT);
            break;

        case KSPROPERTY_DIRECTSOUND3DLISTENER_VELOCITY:
            OutBufferSize = sizeof(DS3DVECTOR);
            break;

        default:
            _ASSERT(FALSE);

    }

    switch( TestCase )
    {
        case NodeID_ULONGMAX:
            pParams->NodeProp.NodeId = ULONG_MAX;  //setting node id to invalid node id and passing all other parameters correct
            break;

        case NodeID_0x80000001:
            pParams->NodeProp.NodeId = 0x80000001;  //setting node id to invalid node id and passing all other parameters correct
            break;

        case NULL_Buffer:
            OutBufferSize = 0;  //setting channel as invalid.
            cbExpectedWritten = 0;
            break;

        case Small_Buffer:
            OutBufferSize = OutBufferSize - 1;  // setting a smaller buffer
            break;

        case Large_Buffer:
            OutBufferSize = OutBufferSize + 1;  // lying about buffer size
            // ks.sys might step in, in which case DeviceIoControl should fail
            // if ks.sys doesn't step in, then the overwrite check is necessary
            bExpectedTestResult = -1;
            cbExpectedWritten = -1;
            break;

        case Valid_Test:
            cbExpectedWritten = OutBufferSize;
            bExpectedTestResult = FALSE; // should succeed
            break;

        default:
            _ASSERT(FALSE);
            return FNS_FAIL;
    }

    // since for set request we are not expecting any data to be writtne back. 
    // Ideally we should expect zero bytes as written, however there is a bug in some of drivers that they are
    // returning number during set request. However this was take care by KS so that IO manager will not copy data back
    // to user mode for Property Set request
    if( pParams->NodeProp.Property.Flags & KSPROPERTY_TYPE_SET )
    {
        cbExpectedWritten = (ULONG) -1;
    }

    CGenericIoControl gnt( pParams->hFile,
                           &pParams->NodeProp,
                           pParams->cbSize,
                           pParams->cbSize,
                           OutBufferSize,
                           OutBufferSize );

    dwResult = gnt.Test( bExpectedTestResult, cbExpectedWritten );
    if( ERROR_SUCCESS != dwResult )
    {
        SLOG( eInfo2, "Test failed with error code %u (0x%x)", dwResult, dwResult );
    }

    if( gnt.GetFault() != FAULT_NO_FAULT )
    {
        return FNS_FAIL;
    }
    else
    {
       return FNS_PASS;
    }
}

//==================================================================================
/* Testing for 
KSPROPERTY_AUDIO_CHORUS_LEVEL
KSPROPERTY_AUDIO_DEMUX_DEST: 
KSPROPERTY_AUDIO_MUX_SOURCE: 
KSPROPERTY_AUDIO_QUALITY: 
KSPROPERTY_AUDIO_REVERB_LEVEL: 
KSPROPERTY_AUDIO_SAMPLING_RATE:
KSPROPERTY_AUDIO_CPU_RESOURCES
KSPROPERTY_AUDIO_STEREO_SPEAKER_GEOMETRY
KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE
KSPROPERTY_AUDIO_SURROUND_ENCODE: 
*/
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_NodeID_ULONGMAX(PGAUDITPARAMS pParams)
//sending invalid node id = 0xFFFFFFFF in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, NodeID_ULONGMAX);
}
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_NodeID_0x80000001(PGAUDITPARAMS pParams)
//sending invalid node id = 0x80000001 in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, NodeID_0x80000001);
}
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_NULL_Buffer(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, NULL_Buffer);
}
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_Small_Buffer(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, Small_Buffer);
}
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_Large_Buffer(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, Large_Buffer);
}
DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_Valid_Test(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, Valid_Test);
}

//==========================================
// for testing of KSNODEPROPERTY_AUDIO_CHANNEL adn ULONG/LONG and BOOL
    /*KSPROPERTY_AUDIO_BASS: 
    KSPROPERTY_AUDIO_MID:             
    KSPROPERTY_AUDIO_NUM_EQ_BANDS:             
    KSPROPERTY_AUDIO_PEAKMETER:
    KSPROPERTY_AUDIO_TREBLE: 
    KSPROPERTY_AUDIO_VOLUMELEVEL: 
    KSPROPERTY_AUDIO_AGC: 
    KSPROPERTY_AUDIO_BASS_BOOST: 
    KSPROPERTY_AUDIO_LOUDNESS:
    KSPROPERTY_AUDIO_MUTE:*/

DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_NodeID_ULONGMAX(PGAUDITPARAMS pParams)
//sending invalid node id = 0xFFFFFFFF in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, NodeID_ULONGMAX);
}
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_NodeID_0x80000001(PGAUDITPARAMS pParams)
//sending invalid node id = 0x80000001 in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, NodeID_0x80000001);
}
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_ChannelID_0xFFFFFFFE(PGAUDITPARAMS pParams)
//sending invalid node id = 0xFFFFFFFF in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, NodeID_ULONGMAX);
}
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_ChannelID_0x80000001(PGAUDITPARAMS pParams)
//sending invalid node id = 0x80000001 in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_NULL_Buffer(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, NULL_Buffer);
}
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_Small_Buffer(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Small_Buffer);
}
DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_Large_Buffer(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Large_Buffer);
}

DWORD WINAPI Generic_Property_KSNODEPROPERTY_AUDIO_CHANNELULONG_Valid_Test(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test);
}