//#ifndef __STREAMS__
//#define __STREAMS__
//#endif
//#include <strmif.h>
//#include <ksproxy.h>

class CKsStreamRender
{
public:
    CKsStreamRender(bool &fRet, CPCMAudioPin *pPin);
    ~CKsStreamRender() { Stop(); }
    bool Play(WAVEFORMATEX *pWfxIn, HANDLE hNotifyFinished = NULL);
    bool Stop();

private:
    static DWORD WINAPI USBRender (LPVOID pThisObj);
    bool USBWriteData (PBYTE pData, ULONGLONG cbData, ULONG nFrame);

private:
    // Minimum time difference between curr position and 
    // player feed position in ms
    static const UINT   MIN_DIFF_TIME = 50;

    CPCMAudioPin       *m_pPin;
    DWORD               m_dwPlayerThreadId;
    HANDLE              m_EventWaitForStreamStart;
    HANDLE              m_EventWaitForStreamStop;
    bool                m_StreamStop;

    // This is to notify callers that streaming is done
    HANDLE              m_hNotifyClientFinished;
};

CKsStreamRender::CKsStreamRender(bool &fRet, CPCMAudioPin *pPin)
: m_pPin(pPin)
, m_dwPlayerThreadId(0)
, m_EventWaitForStreamStart(NULL)
, m_EventWaitForStreamStop(NULL)
, m_StreamStop(false)
, m_hNotifyClientFinished(NULL)
{
    fRet = (NULL != m_pPin);
}

bool CKsStreamRender::Play(WAVEFORMATEX * /*pWfxIn*/, HANDLE hNotifyFinished)
{
    // Create an event that is unset
    m_EventWaitForStreamStart = CreateEvent(NULL, FALSE, FALSE, NULL);
    if(NULL == m_EventWaitForStreamStart)       { return false; }
    m_EventWaitForStreamStop = CreateEvent(NULL, FALSE, FALSE, NULL);
    if(NULL == m_EventWaitForStreamStop)        { return false; }
    m_hNotifyClientFinished = hNotifyFinished;

    CreateThread(NULL, 0, &USBRender, (void*)this, 0, &m_dwPlayerThreadId);
    WaitForSingleObject(m_EventWaitForStreamStart, INFINITE);

    return true;
}

bool CKsStreamRender::Stop()
{
    if (NULL == m_EventWaitForStreamStop)
    {
        return false;
    }
    
    ResetEvent(m_EventWaitForStreamStop);
    m_StreamStop = true;
    if(WAIT_TIMEOUT == WaitForSingleObject(m_EventWaitForStreamStop, 5500))
    {
        return false;
    }
    return true;

}

DWORD WINAPI CKsStreamRender::USBRender(LPVOID pThisObj)
{
    CKsStreamRender    *pThis = (CKsStreamRender*)pThisObj;
    BOOL                fRes = TRUE;
    PBYTE               pData = pThis->m_pPin->m_pbStreamData;
    ULONGLONG           cbData = pThis->m_pPin->m_cbStreamData;
    WAVEFORMATEX       *pwfx = &(((KSDATAFORMAT_WAVEFORMATEX*)(pThis->m_pPin->m_pksDataFormat))->WaveFormatEx);
    UINT                MaxDataSent = 250 * pwfx->nAvgBytesPerSec / 1000;
    UINT                FrameDataSize = 50 * pwfx->nAvgBytesPerSec / 1000;

    ULONGLONG           CurrByteStreamPos = 0;
    ULONGLONG           AmountToStream = 0;
    KSAUDIO_POSITION    pos = {0};

    // Frame management
    const UINT          NUM_FRAMES_USED = 5;
    UINT                FrameStates[NUM_FRAMES_USED] = {0};
    HANDLE              FrameEvents[NUM_FRAMES_USED] = {0};
    UINT                iFrame = 0;
    DWORD               WaitRes = 0;

    CThreadPriorityChange TPC(THREAD_PRIORITY_TIME_CRITICAL, &fRes); 

    // Initialize frame events array
    for(iFrame = 0; iFrame < NUM_FRAMES_USED; ++iFrame)
    {
        FrameEvents[iFrame] = pThis->m_pPin->m_ov[iFrame].hEvent;
        SetEvent(FrameEvents[iFrame]);
    }

    while(CurrByteStreamPos < cbData)
    {
        // Find an empty frame. If none empty continue and try again
        for(iFrame = 0; iFrame < NUM_FRAMES_USED; ++iFrame)
        {
            if(0 == FrameStates[iFrame]) { break; }
        }

        // Get position and see if more data is needed to be sent
        pThis->m_pPin->GetPosition(&pos);
        if( (NUM_FRAMES_USED != iFrame) && // if an empty frame is available
            (MaxDataSent - (CurrByteStreamPos - pos.PlayOffset)) >  FrameDataSize)
        {
            AmountToStream = ((cbData - CurrByteStreamPos) < FrameDataSize)?
                                (cbData - CurrByteStreamPos) : FrameDataSize;
            pThis->USBWriteData(pData, AmountToStream, iFrame);
            pData += AmountToStream;
            CurrByteStreamPos += AmountToStream;
            FrameStates[iFrame] = 1;            
        }

        // Wait on frame events and set them to free when they are done
        WaitRes = WaitForMultipleObjectsEx(NUM_FRAMES_USED, FrameEvents, FALSE, pThis->MIN_DIFF_TIME, FALSE);
        if( (WaitRes - WAIT_OBJECT_0) < NUM_FRAMES_USED)
        {
            iFrame = WaitRes - WAIT_OBJECT_0;
            FrameStates[iFrame] = 0;
            ResetEvent(FrameEvents[iFrame]);
        }
        else if(WAIT_TIMEOUT == WaitRes)
        {
            // USB driver primed with five buffers.
            // Release parent thread
            SetEvent(pThis->m_EventWaitForStreamStart);
        }

        if(pThis->m_StreamStop)
        {
            break;
        }
    }

    // Wait for remaining events to finish
    UINT    cFrameEventsRemaining = 0;
    HANDLE  hFrameEventsRemaining[NUM_FRAMES_USED] = {0};
    for(int i=0; i < NUM_FRAMES_USED; ++i)
    {
        if(1 == FrameStates[i])
        {
            hFrameEventsRemaining[cFrameEventsRemaining] = FrameEvents[i];
            ++cFrameEventsRemaining;
        }
    }
    if(0 != cFrameEventsRemaining)
    {
        (void) WaitForMultipleObjectsEx(cFrameEventsRemaining, hFrameEventsRemaining, TRUE, 5000, FALSE);
    }

    // Set events when done
    SetEvent(pThis->m_EventWaitForStreamStart);
    SetEvent(pThis->m_EventWaitForStreamStop);
    if(NULL != pThis->m_hNotifyClientFinished)
    {
        SetEvent(pThis->m_hNotifyClientFinished);
    }
    return 1;
}


bool CKsStreamRender::USBWriteData
(
    PBYTE pData,
    ULONGLONG cbData,
    ULONG nFrame
)
{
    DWORD   cbReturned;
    BOOL    fRes = TRUE;
    bool    fRet = true;

    if (nFrame >= 10)
        return false;

    ZeroMemory(&(m_pPin->m_ksStreamHeader[nFrame]), sizeof(KSSTREAM_HEADER));
    m_pPin->m_ksStreamHeader[nFrame].Size = sizeof(KSSTREAM_HEADER);
    m_pPin->m_ksStreamHeader[nFrame].Data = pData;
    m_pPin->m_ksStreamHeader[nFrame].FrameExtent = (ULONG)cbData;
    m_pPin->m_ksStreamHeader[nFrame].DataUsed = (ULONG)cbData;
    m_pPin->m_ksStreamHeader[nFrame].PresentationTime.Time          = 0;
    m_pPin->m_ksStreamHeader[nFrame].PresentationTime.Numerator     = 1;
    m_pPin->m_ksStreamHeader[nFrame].PresentationTime.Denominator   = 1;
    m_pPin->m_ksStreamHeader[nFrame].OptionsFlags = 0;
    
    ResetEvent(m_pPin->m_ov[nFrame].hEvent);
    fRes = 
        DeviceIoControl
        ( 
            m_pPin->m_handle,
            IOCTL_KS_WRITE_STREAM,
            NULL,
            0,
            &(m_pPin->m_ksStreamHeader[nFrame]),
            sizeof(KSSTREAM_HEADER),
            &cbReturned,
            &(m_pPin->m_ov[nFrame])
        );
    if(!fRes)    { fRet = false; }

    return fRet;
}


////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class CKsStreamCapture
{
public:
    CKsStreamCapture(bool &fRet, CPCMAudioPin *pPin);
    ~CKsStreamCapture() {Stop();}
    bool Capture(WAVEFORMATEX *pWfxIn, HANDLE hNotifyFinished = NULL);
    bool Stop();

private:
    static DWORD WINAPI USBCapture (LPVOID pThisObj);
    bool USBReadData (ULONG nFrame, ULONG cbReadSize);

private:
    // Minimum time difference between curr position and 
    // player feed position in ms
    static const UINT   MIN_DIFF_TIME = 50;

    CPCMAudioPin       *m_pPin;
    DWORD               m_dwPlayerThreadId;
    HANDLE              m_EventWaitForStreamStart;
    HANDLE              m_EventWaitForStreamStop;
    bool                m_StreamStop;

    HANDLE              m_hNotifyClientFinished;
};

CKsStreamCapture::CKsStreamCapture(bool &fRet, CPCMAudioPin *pPin)
{
    fRet = false;
    if(NULL == pPin)    { return; }
    m_pPin = pPin; 
    m_StreamStop = false;
    fRet = true;
}

bool CKsStreamCapture::Capture(WAVEFORMATEX * /*pWfxIn*/, HANDLE hNotifyFinished)
{
    // Create an event that is unset
    m_EventWaitForStreamStart = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_EventWaitForStreamStop = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hNotifyClientFinished = hNotifyFinished;

    CreateThread(NULL, 0, &USBCapture, (void*)this, 0, &m_dwPlayerThreadId);
    WaitForSingleObject(m_EventWaitForStreamStart, INFINITE);

    return true;
}

bool CKsStreamCapture::Stop()
{
    ResetEvent(m_EventWaitForStreamStop);
    m_StreamStop = true;
    if(WAIT_TIMEOUT == WaitForSingleObject(m_EventWaitForStreamStop, 5500))
    {
        return false;
    }
    return true;
}

DWORD WINAPI CKsStreamCapture::USBCapture(LPVOID pThisObj)
{
    CKsStreamCapture   *pThis = (CKsStreamCapture*)pThisObj;
    BOOL                fRes = TRUE;
    PBYTE               pData = pThis->m_pPin->m_pbStreamData;
    ULONGLONG           cbData = pThis->m_pPin->m_cbStreamData;
    WAVEFORMATEX       *pwfx = &(((KSDATAFORMAT_WAVEFORMATEX*)(pThis->m_pPin->m_pksDataFormat))->WaveFormatEx);

    ULONGLONG           CurrByteCapturePos = 0;
    KSAUDIO_POSITION    pos = {0};

    // Frame management
    const UINT          MAX_USB_STREAM_TIME = 250; // USB can handle only 250 ms of data
    const UINT          NUM_FRAMES_USED = 10; // Limit is 10 frames due to only 10 KSSTREAM_HEADER in CKsPin
    UINT                FrameStates[NUM_FRAMES_USED] = {0};
    HANDLE              FrameEvents[NUM_FRAMES_USED] = {0};
    //UINT                MaxDataSent = (MAX_USB_STREAM_TIME * pwfx->nSamplesPerSec / 1000) * pwfx->nBlockAlign;
    UINT                FrameDataSize = (MAX_USB_STREAM_TIME/NUM_FRAMES_USED * pwfx->nSamplesPerSec / 1000) * pwfx->nBlockAlign;
    UINT                iFrame = 0;
    DWORD               WaitRes = 0;
    ULONGLONG           lastInterestingThing = GetTickCount64();

    CThreadPriorityChange TPC(THREAD_PRIORITY_TIME_CRITICAL, &fRes); 

    // Initialize frame events array and allocate frame buffers
    for(iFrame = 0; iFrame < NUM_FRAMES_USED; ++iFrame)
    {
        FrameEvents[iFrame] = pThis->m_pPin->m_ov[iFrame].hEvent;
        SetEvent(FrameEvents[iFrame]);
        pThis->m_pPin->m_ksStreamHeader[iFrame].Data = new BYTE[FrameDataSize];
        if(NULL == pThis->m_pPin->m_ksStreamHeader[iFrame].Data)
        {
            return 0;
        }
        pThis->m_pPin->m_ksStreamHeader[iFrame].FrameExtent = FrameDataSize;
    }

    while(CurrByteCapturePos < cbData)
    {
        // Find an empty frame. If none empty continue and try again
        for(iFrame = 0; iFrame < NUM_FRAMES_USED; ++iFrame)
        {
            if(0 == FrameStates[iFrame]) { break; }
        }

        pThis->m_pPin->GetPosition(&pos);
        // if an empty frame is available
        if( (NUM_FRAMES_USED != iFrame) && (pos.WriteOffset < cbData))
        {
            ULONG cbRemainBuffer = (ULONG)(cbData - pos.WriteOffset);
            pThis->USBReadData(iFrame, ((cbRemainBuffer < FrameDataSize)? cbRemainBuffer : FrameDataSize));
            FrameStates[iFrame] = 1;
        }

        // Wait on frame events and set them to free when they are done
        WaitRes = WaitForMultipleObjectsEx(NUM_FRAMES_USED, FrameEvents, FALSE, pThis->MIN_DIFF_TIME, FALSE);
        if( (WaitRes - WAIT_OBJECT_0) < NUM_FRAMES_USED)
        {
            auto streamHeader = &pThis->m_pPin->m_ksStreamHeader[WaitRes - WAIT_OBJECT_0];
            size_t bytesToCopy = streamHeader->DataUsed;
            
            if(streamHeader->DataUsed > streamHeader->FrameExtent)
            {
                XLOG(XFAIL, 1, "FAIL: Driver claims to have written %u bytes but the buffer size is only %u bytes", streamHeader->DataUsed, streamHeader->FrameExtent);
                bytesToCopy = streamHeader->FrameExtent;
            }
            else if((cbData - CurrByteCapturePos) < streamHeader->DataUsed)
            {
                bytesToCopy = (size_t)(cbData - CurrByteCapturePos);
            }
            
            memcpy(pData + CurrByteCapturePos, streamHeader->Data, bytesToCopy);
            CurrByteCapturePos += bytesToCopy;
            
            FrameStates[WaitRes - WAIT_OBJECT_0] = 0;
            ResetEvent(FrameEvents[WaitRes - WAIT_OBJECT_0]);
            
            lastInterestingThing = GetTickCount64();
        }

        // Release parent thread
        SetEvent(pThis->m_EventWaitForStreamStart);

        if(pThis->m_StreamStop)
        {
            // clean break
            break;
        }
        
        // if we've been spinning for 20 seconds or more
        if(GetTickCount64() - lastInterestingThing > 20 * 1000)
        {
            XLOG(XFAIL, 1, "FAIL: Audio capture has stalled for at least 20 seconds");
            break;
        }
    }

    // Wait for remaining events to finish
    UINT    cFrameEventsRemaining = 0;
    HANDLE  hFrameEventsRemaining[NUM_FRAMES_USED] = {0};
    for(int i=0; i < NUM_FRAMES_USED; ++i)
    {
        if(1 == FrameStates[i])
        {
            hFrameEventsRemaining[cFrameEventsRemaining] = FrameEvents[i];
            ++cFrameEventsRemaining;
        }
    }
    if(0 != cFrameEventsRemaining)
    {
        (void) WaitForMultipleObjectsEx(cFrameEventsRemaining, hFrameEventsRemaining, TRUE, 5000, FALSE);
    }

    // Cleanup
    for(iFrame = 0; iFrame < NUM_FRAMES_USED; ++iFrame)
    {
        delete pThis->m_pPin->m_ksStreamHeader[iFrame].Data;
    }
    SetEvent(pThis->m_EventWaitForStreamStart);
    SetEvent(pThis->m_EventWaitForStreamStop);
    if(NULL != pThis->m_hNotifyClientFinished)
    {
        SetEvent(pThis->m_hNotifyClientFinished);
    }

    return 1;
}


bool CKsStreamCapture::USBReadData
(
    ULONG nFrame,
    ULONG cbReadSize
)
{
    DWORD   cbReturned;
    BOOL    fRes = TRUE;
    bool    fRet = true;
    PVOID   tBuf = NULL;

    if (nFrame >= 10)
        return false;

    tBuf = m_pPin->m_ksStreamHeader[nFrame].Data;
    ZeroMemory(&(m_pPin->m_ksStreamHeader[nFrame]), sizeof(KSSTREAM_HEADER));

    m_pPin->m_ksStreamHeader[nFrame].Size = sizeof(KSSTREAM_HEADER);
    m_pPin->m_ksStreamHeader[nFrame].Data = tBuf;
    m_pPin->m_ksStreamHeader[nFrame].FrameExtent = cbReadSize;
    m_pPin->m_ksStreamHeader[nFrame].DataUsed = 0;
    m_pPin->m_ksStreamHeader[nFrame].PresentationTime.Time          = 0;
    m_pPin->m_ksStreamHeader[nFrame].PresentationTime.Numerator     = 1;
    m_pPin->m_ksStreamHeader[nFrame].PresentationTime.Denominator   = 1;
    m_pPin->m_ksStreamHeader[nFrame].OptionsFlags = 0;
    
    ResetEvent(m_pPin->m_ov[nFrame].hEvent);
    fRes = 
        DeviceIoControl
        ( 
            m_pPin->m_handle,
            IOCTL_KS_READ_STREAM,
            NULL,
            0,
            &(m_pPin->m_ksStreamHeader[nFrame]),
            sizeof(KSSTREAM_HEADER),
            &cbReturned,
            &(m_pPin->m_ov[nFrame])
        );
    if(fRes)    { fRet = false; }
    
    return fRet;
}

