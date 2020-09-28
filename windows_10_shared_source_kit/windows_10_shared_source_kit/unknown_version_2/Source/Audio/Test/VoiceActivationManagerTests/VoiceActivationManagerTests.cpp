/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include "Precomp.h"
#include "VoiceActivationManagerTests.h"

#define DEFAULT_WAIT_TIME 5000
#define DETECTION_WAIT_TIME 30000

// Create the COM objects
bool VoiceActivationManagerTests::InitTests()
{
    m_pWnfSubscription = nullptr;
    m_hStateChanged = nullptr;
    m_changeStamp = 0;
    m_pMatchData = nullptr;
    VERIFY_SUCCEEDED(m_csLock.Init());
    VERIFY_SUCCEEDED(m_csLock.Lock());
    
    VERIFY_SUCCEEDED(Initialize());
    
    return true;
}

// Turn the power off
bool VoiceActivationManagerTests::DeinitTests()
{
    if (m_hStateChanged)
    {
        CloseHandle(m_hStateChanged);
        m_hStateChanged = NULL;
    }
    
    if(!SUCCEEDED(m_csLock.Unlock()))
    {
        Log::Error(String().Format(L"Unlock failed"));
    }
    
    if (NULL != m_pWnfSubscription)
    {
        RtlUnsubscribeWnfNotificationWaitForCompletion(m_pWnfSubscription);
        m_pWnfSubscription = NULL;
    }
    
    VERIFY_SUCCEEDED(m_csLock.Term());

    return true;
}

// Set up before every test
bool VoiceActivationManagerTests::TestSetup()
{   
    m_bExtraMatchAllowed = FALSE;
    return true;
}

// Clean up after every test
bool VoiceActivationManagerTests::TestCleanUp()
{
    BOOL fArmed = FALSE;
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->IsArmed(&fArmed));
    if (fArmed)
    {
        VERIFY_SUCCEEDED(m_spVoiceActivationManager->Arm(FALSE),
            L"CleanupDisarm");
        VERIFY_SUCCEEDED(m_spVoiceActivationManager->IsArmed(&fArmed));
        VERIFY_IS_TRUE(fArmed == FALSE);
    }
    
    ResetEvent(m_hStateChanged);
    CoTaskMemFree(m_pMatchData);
    m_pMatchData = nullptr;

    return true;
}

// GetSupportedWakePatternTypes test
VOID VoiceActivationManagerTests::GetSupportedWakePatternTypesTest()
{
    GUID* pguidSupportedWakePatternTypes = nullptr;
    ULONG ulSupportedWakePatternTypeCount = 0;
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->GetSupportedWakePatternTypes(&pguidSupportedWakePatternTypes, &ulSupportedWakePatternTypeCount),
        L"Getting supported wake patterns");
    VERIFY_IS_TRUE(ulSupportedWakePatternTypeCount > 0);
    
    for(ULONG i = 0; i < ulSupportedWakePatternTypeCount; i++)
    {
        PWSTR pszClsid = nullptr;

        VERIFY_SUCCEEDED(StringFromCLSID(pguidSupportedWakePatternTypes[i], &pszClsid));
        Log::Comment(String().Format(L"Supported GUID : %s", pszClsid));
        
        CoTaskMemFree(pszClsid);
    }
    
    CoTaskMemFree(pguidSupportedWakePatternTypes);
}

// IsVoiceActivationEnabled test
VOID VoiceActivationManagerTests::IsArmedTest()
{
    BOOL fArmed = FALSE;
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->IsArmed(&fArmed),
        L"Getting current arm state");
    VERIFY_IS_TRUE(fArmed == FALSE);
}

// Set valid format wake patterns
VOID VoiceActivationManagerTests::ProgramValidWakePatternsTest()
{
    GUID* pguidSupportedWakePatternTypes = nullptr;
    ULONG ulSupportedWakePatternTypeCount = 0;
    
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->GetSupportedWakePatternTypes(&pguidSupportedWakePatternTypes, &ulSupportedWakePatternTypeCount),
        L"Getting supported wake patterns");
    VERIFY_IS_TRUE(ulSupportedWakePatternTypeCount > 0);
    
    for(ULONG i = 0; i < ulSupportedWakePatternTypeCount; i++)
    {
        SOUNDDETECTOR_PATTERNHEADER* pPatternData = nullptr;
        
        GetPatternDataforPatternType(pguidSupportedWakePatternTypes[i], &pPatternData);

        VERIFY_SUCCEEDED(m_spVoiceActivationManager->SetWakePattern(pPatternData),
            L"Setting valid wake pattern");
    
        CoTaskMemFree(pPatternData);
    }
        
    CoTaskMemFree(pguidSupportedWakePatternTypes);
}

// Set Invalid format wake pattern
VOID VoiceActivationManagerTests::ProgramInvalidWakePatternsTest()
{
    GUID* pguidSupportedWakePatternTypes = nullptr;
    ULONG ulSupportedWakePatternTypeCount = 0;
    
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->GetSupportedWakePatternTypes(&pguidSupportedWakePatternTypes, &ulSupportedWakePatternTypeCount),
        L"Getting supported wake patterns");
    VERIFY_IS_TRUE(ulSupportedWakePatternTypeCount > 0);
    
    for(ULONG i = 0; i < ulSupportedWakePatternTypeCount; i++)
    {
        SOUNDDETECTOR_PATTERNHEADER* pPatternData = nullptr;
        
        GetPatternDataforPatternType(pguidSupportedWakePatternTypes[i], &pPatternData);

        // Save valid values
        ULONG ulValidPatternSize = pPatternData->Size;
        GUID guidVaildPatternType = pPatternData->PatternType; 
        
        // Wrong size in header
        pPatternData->Size -= 1;
        VERIFY_FAILED(m_spVoiceActivationManager->SetWakePattern(pPatternData),
            L"Setting invalid wake patterns - wrong smaller header size");
        pPatternData->Size = ulValidPatternSize;
        
        // Wrong size in header
        pPatternData->Size += 1;
        VERIFY_FAILED(m_spVoiceActivationManager->SetWakePattern(pPatternData),
            L"Setting invalid wake patterns - wrong larger header size");
        pPatternData->Size = ulValidPatternSize;
        
        // Wrong patterntype in header
        pPatternData->PatternType = GUID_NULL;
        VERIFY_FAILED(m_spVoiceActivationManager->SetWakePattern(pPatternData),
            L"Setting invalid wake patterns - wrong patterntype");
        pPatternData->PatternType = guidVaildPatternType;
    
        CoTaskMemFree(pPatternData);
    }
    
    CoTaskMemFree(pguidSupportedWakePatternTypes);
}

// Arm test
VOID VoiceActivationManagerTests::ArmTest()
{
    BOOL fArmed = FALSE;
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->Arm(TRUE),
        L"Arm");
    DWORD dwWaitRet = WaitForSingleObjectWithoutLock(DEFAULT_WAIT_TIME);
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->IsArmed(&fArmed));
    VERIFY_IS_TRUE(fArmed == TRUE || dwWaitRet == WAIT_OBJECT_0); 
}

// Double arm test
VOID VoiceActivationManagerTests::DoubleArmTest() 
{
    BOOL fArmed = FALSE;
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->Arm(TRUE),
        L"First of two consecutive arms");

    // The sample driver will fire a pattern match for each arm for testing
    // Allow two pattern matches since we're arming a second time.
    m_bExtraMatchAllowed = TRUE;

    VERIFY_SUCCEEDED(m_spVoiceActivationManager->Arm(TRUE),
        L"Second of two consecutive arms");
    DWORD dwWaitRet = WaitForSingleObjectWithoutLock(DEFAULT_WAIT_TIME);
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->IsArmed(&fArmed));
    VERIFY_IS_TRUE(fArmed == TRUE || dwWaitRet == WAIT_OBJECT_0);

    // Wait a bit in case this is the sample driver firing off a second match.
    WaitForSingleObjectWithoutLock(DEFAULT_WAIT_TIME / 10);
}

// Double disarm test
VOID VoiceActivationManagerTests::DoubleDisarmTest()
{
    BOOL fArmed = FALSE;
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->Arm(FALSE),
        L"First of double disarm");
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->Arm(FALSE),
        L"Second of double disarm");
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->IsArmed(&fArmed));
    VERIFY_IS_TRUE(fArmed == FALSE); 
}

// Test automatic disarm on SetPatterns while armed
VOID VoiceActivationManagerTests::ProgramDuringArmTest()
{
    BOOL fArmed = FALSE;
    GUID* pguidSupportedWakePatternTypes = nullptr;
    ULONG ulSupportedWakePatternTypeCount = 0;
    
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->GetSupportedWakePatternTypes(&pguidSupportedWakePatternTypes, &ulSupportedWakePatternTypeCount),
        L"Getting supported wake patterns");
    VERIFY_IS_TRUE(ulSupportedWakePatternTypeCount > 0);
    
    SOUNDDETECTOR_PATTERNHEADER* pPatternData = nullptr;
        
    GetPatternDataforPatternType(pguidSupportedWakePatternTypes[0], &pPatternData);

    VERIFY_SUCCEEDED(m_spVoiceActivationManager->Arm(TRUE),
        L"Arm");
        
    DWORD dwWaitRet = WaitForSingleObjectWithoutLock(DEFAULT_WAIT_TIME);

    VERIFY_SUCCEEDED(m_spVoiceActivationManager->SetWakePattern(pPatternData),
        L"Setting valid wake pattern");
    
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->IsArmed(&fArmed));
    VERIFY_IS_TRUE(fArmed == FALSE); 
    
    if(dwWaitRet == WAIT_OBJECT_0)
    {
        Log::Comment(String().Format(L"This test was not properly tested because a match was detected before setting pattern on an armed state"));
    }
    
    CoTaskMemFree(pPatternData);
    CoTaskMemFree(pguidSupportedWakePatternTypes);
}

// Match Detection Test
VOID VoiceActivationManagerTests::MatchDetectionTest()
{
    GUID* pguidSupportedWakePatternTypes = nullptr;
    ULONG ulSupportedWakePatternTypeCount = 0;
    
    VERIFY_SUCCEEDED(m_spVoiceActivationManager->GetSupportedWakePatternTypes(&pguidSupportedWakePatternTypes, &ulSupportedWakePatternTypeCount),
        L"Getting supported wake patterns");
    VERIFY_IS_TRUE(ulSupportedWakePatternTypeCount > 0);
    
    for(ULONG i = 0; i < ulSupportedWakePatternTypeCount; i++)
    {
        SOUNDDETECTOR_PATTERNHEADER* pPatternData = nullptr;
        KEYWORDID retKeywordId = KwInvalid;
        LANGID retLangId = 0;
        BOOL isUserMatch = FALSE;
        ULONG64 retKeywordStartPerformanceCounter = 0;
        ULONG64 retKeywordEndPerformanceCounter = 0;
        CComPtr<IKeywordDetectorOemAdapter> spKeywordDetectorOemAdapter;
        IStream* spStream = nullptr;
        BOOL supportsUserModels = FALSE;
        KEYWORDID* pKeywordIds = nullptr;
        ULONG ulNumKeywordIds = 0;
        LANGID* pLangIds = nullptr;
        ULONG ulNumLangIds = 0;
        ComPtr<IMFMediaType> spMFType;
        KEYWORDSELECTOR pKeywordSelector;
    
        VERIFY_SUCCEEDED(CoCreateInstance(pguidSupportedWakePatternTypes[i], NULL, CLSCTX_INPROC_SERVER, IID_IKeywordDetectorOemAdapter, (LPVOID *) &spKeywordDetectorOemAdapter),
            L"Creating OEM DLL keyword detector adapter");
        
        VERIFY_SUCCEEDED(CreateStreamOnHGlobal(NULL, TRUE, &spStream));
        
        VERIFY_SUCCEEDED(spKeywordDetectorOemAdapter->GetCapabilities(&supportsUserModels, &pKeywordIds, &ulNumKeywordIds, &pLangIds, &ulNumLangIds, &spMFType));
        VERIFY_IS_TRUE(ulNumKeywordIds > 0 && ulNumLangIds > 0);

        pKeywordSelector = {pKeywordIds[0], pLangIds[0]};

        VERIFY_SUCCEEDED(spKeywordDetectorOemAdapter->BuildArmingPatternData(spStream, &pKeywordSelector, 1, &pPatternData));
        VERIFY_IS_TRUE(pPatternData->PatternType == pguidSupportedWakePatternTypes[i]);

        VERIFY_SUCCEEDED(m_spVoiceActivationManager->SetWakePattern(pPatternData),
            L"Setting valid wake pattern");
    
        VERIFY_SUCCEEDED(m_spVoiceActivationManager->Arm(TRUE),
            L"Arm");
            
        Log::Comment(String().Format(L"Signal a detection now"));
        DWORD dwWaitRet = WaitForSingleObjectWithoutLock(DETECTION_WAIT_TIME);
        VERIFY_IS_TRUE(dwWaitRet == WAIT_OBJECT_0);
        
        if(m_pMatchData != nullptr)
        {
            VERIFY_IS_TRUE(((SOUNDDETECTOR_PATTERNHEADER *)m_pMatchData)->PatternType == pPatternData->PatternType);
            
            VERIFY_SUCCEEDED(spKeywordDetectorOemAdapter->ParseDetectionResultData(spStream, (SOUNDDETECTOR_PATTERNHEADER *)m_pMatchData, &retKeywordId, &retLangId, &isUserMatch, &retKeywordStartPerformanceCounter, &retKeywordEndPerformanceCounter));
            VERIFY_IS_TRUE(retKeywordId == pKeywordIds[0] && retLangId == pLangIds[0] && isUserMatch == FALSE);
        }
        
        CoTaskMemFree(pPatternData);
        CoTaskMemFree(pKeywordIds);
        CoTaskMemFree(pLangIds);
        SAFE_RELEASE(spStream);
    }
    
    CoTaskMemFree(pguidSupportedWakePatternTypes);
}

class CCompletionDelegate : public RuntimeClass<RuntimeClassFlags<ClassicCom>, FtmBase,
                                   IActivateAudioInterfaceCompletionHandler>
{
private:
    HANDLE                                          m_hCompletionEvent;
    CComPtr<IActivateAudioInterfaceAsyncOperation>  m_spOperation;

public:
    CCompletionDelegate(HANDLE hCompletedEvent):m_hCompletionEvent(hCompletedEvent) {}
    ~CCompletionDelegate() {}
    
    //  IActivateAudioInterfaceCompletionHandler
    STDMETHODIMP            ActivateCompleted(IActivateAudioInterfaceAsyncOperation *operation);
    
    // Other
    STDMETHODIMP            GetOperationInterface(IActivateAudioInterfaceAsyncOperation** ppOperation);
};

HRESULT CCompletionDelegate::ActivateCompleted(IActivateAudioInterfaceAsyncOperation* operation)
{
    if (nullptr == operation)
    {
        return E_POINTER;
    }
    
    // cache the returned interface.  
    m_spOperation.Attach(operation);
    
    SetEvent( m_hCompletionEvent );
    return S_OK;
}

HRESULT CCompletionDelegate::GetOperationInterface(IActivateAudioInterfaceAsyncOperation** ppOperation)
{
    if (nullptr == ppOperation)
    {
        return E_POINTER;
    }

    if (m_spOperation == nullptr)
    {
        return E_FAIL;
    }
    else
    {
        *ppOperation = m_spOperation;
        m_spOperation.Detach();
    }

    return S_OK;
}

VOID VoiceActivationManagerTests::StreamTest()
{         
    WEX::TestExecution::SetVerifyOutput verifySettings(WEX::TestExecution::VerifyOutputSettings::LogOnlyFailures);
    ComPtr<IAudioClient> spAudioClient;
    IAudioCaptureClient *pCaptureClient = NULL;
    PWSTR wstrDetectorDeviceInterfaceId = NULL;
    HANDLE hCompletionEvent = NULL;
    ComPtr<IActivateAudioInterfaceAsyncOperation> spOperation;
    ComPtr<CCompletionDelegate> spCompletionObject;
    ComPtr<IActivateAudioInterfaceCompletionHandler> spCompletionHandler;
    ComPtr<IUnknown> spUnknown;
    HRESULT hrActivateResult = S_OK;
    PWAVEFORMATEX pwfx = NULL;
    UINT32 bufferFrameCount;
    REFERENCE_TIME hnsRequestedDuration = 5000000;
    DOUBLE hnsActualDuration;
    BYTE *pData;
    UINT32 numFramesAvailable, totalFrames = 0;
    DWORD flags;
    UINT32 packetLength = 0;

    VERIFY_SUCCEEDED(m_spVoiceActivationManager->GetKeywordDetectorDeviceInterfaceId(&wstrDetectorDeviceInterfaceId));
 
    hCompletionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    VERIFY_IS_NOT_NULL(hCompletionEvent);

    spCompletionObject = Make<CCompletionDelegate>(hCompletionEvent);
    VERIFY_SUCCEEDED(spCompletionObject.As(&spCompletionHandler));
    
    VERIFY_SUCCEEDED(ActivateAudioInterfaceAsync(wstrDetectorDeviceInterfaceId,
                                IID_IAudioClient,
                                NULL,
                                spCompletionHandler.Get(),
                                spOperation.GetAddressOf()));
                                
    VERIFY_SUCCEEDED(WaitForSingleObject( hCompletionEvent, INFINITE ));
    
    VERIFY_SUCCEEDED(spCompletionObject->GetOperationInterface(spOperation.GetAddressOf()));
    VERIFY_SUCCEEDED(spOperation->GetActivateResult(&hrActivateResult, spUnknown.GetAddressOf()));
    VERIFY_SUCCEEDED(hrActivateResult);
    
    // Get the pointer for the Audio Client
    spUnknown.As( &spAudioClient );
    VERIFY_IS_NOT_NULL(spAudioClient);
    
    VERIFY_SUCCEEDED(spAudioClient->GetMixFormat(&pwfx));
    
    Log::Comment(String().Format(L"%d | %d | %d | %d | %d | %d | %d", pwfx->wFormatTag, pwfx->nChannels, pwfx->nSamplesPerSec, pwfx->nAvgBytesPerSec, pwfx->nBlockAlign, pwfx->wBitsPerSample, pwfx->cbSize));
    
    VERIFY_SUCCEEDED(spAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL));
                         
    VERIFY_SUCCEEDED(spAudioClient->GetBufferSize(&bufferFrameCount));
    VERIFY_SUCCEEDED(spAudioClient->GetService(IID_IAudioCaptureClient, (void**)&pCaptureClient));
    
    hnsActualDuration = (double)hnsRequestedDuration * bufferFrameCount / pwfx->nSamplesPerSec;
    
    VERIFY_SUCCEEDED(spAudioClient->Start());
    
    // Sleep for half the buffer duration.
    Sleep((DWORD)hnsActualDuration/5000/2);
    
    VERIFY_SUCCEEDED(pCaptureClient->GetNextPacketSize(&packetLength));
    
    while (packetLength != 0)
    {
    
        // Get the available data in the shared buffer.
        VERIFY_SUCCEEDED(pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags, NULL, NULL));

        if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
        {
            pData = NULL;  // Tell CopyData to write silence.
        }

        // Log::Comment(String().Format(L"frames captured: %d | current packet size: %d", numFramesAvailable, packetLength));
        totalFrames += numFramesAvailable;
        
        VERIFY_SUCCEEDED(pCaptureClient->ReleaseBuffer(numFramesAvailable));

        VERIFY_SUCCEEDED(pCaptureClient->GetNextPacketSize(&packetLength));
    }

    Log::Comment(String().Format(L"Total frames captured: %d", totalFrames));
    VERIFY_SUCCEEDED(spAudioClient->Stop());
    
    SAFE_RELEASE(pCaptureClient)
    CloseHandle(hCompletionEvent);
}

VOID VoiceActivationManagerTests::GetPatternDataforPatternType(GUID guidPatternType, SOUNDDETECTOR_PATTERNHEADER** ppPatternData)
{
    CComPtr<IKeywordDetectorOemAdapter> spKeywordDetectorOemAdapter;
    IStream* spStream = nullptr;
    BOOL supportsUserModels = FALSE;
    KEYWORDID* pKeywordIds = nullptr;
    ULONG ulNumKeywordIds = 0;
    LANGID* pLangIds = nullptr;
    ULONG ulNumLangIds = 0;
    ComPtr<IMFMediaType> spMFType;
    KEYWORDSELECTOR pKeywordSelector;

    VERIFY_SUCCEEDED(CoCreateInstance(guidPatternType, NULL, CLSCTX_INPROC_SERVER, IID_IKeywordDetectorOemAdapter, (LPVOID *) &spKeywordDetectorOemAdapter),
        L"Creating OEM DLL keyword detector adapter");
        
    VERIFY_SUCCEEDED(CreateStreamOnHGlobal(NULL, TRUE, &spStream));
        
    VERIFY_SUCCEEDED(spKeywordDetectorOemAdapter->GetCapabilities(&supportsUserModels, &pKeywordIds, &ulNumKeywordIds, &pLangIds, &ulNumLangIds, &spMFType));
    VERIFY_IS_TRUE(ulNumKeywordIds > 0 && ulNumLangIds > 0);

    pKeywordSelector = {pKeywordIds[0], pLangIds[0]};

    VERIFY_SUCCEEDED(spKeywordDetectorOemAdapter->BuildArmingPatternData(spStream, &pKeywordSelector, 1, ppPatternData));
    VERIFY_IS_TRUE((ppPatternData != nullptr) && ((*ppPatternData)->PatternType == guidPatternType));
    
    CoTaskMemFree(pKeywordIds);
    CoTaskMemFree(pLangIds);
    SAFE_RELEASE(spStream);
}

// VAM initialize
BOOL VoiceActivationManagerTests::Initialize()
{    
    CComPtr<IMMDeviceEnumerator> spEnumerator;
    CComPtr<IMMDeviceCollection> spMMDeviceCollection;
    CComPtr<IMMEndpointInternal> spEndpointInternal;
    CComPtr<IMMDevice> spEndpoint;
    LPWSTR wszDeviceId;
    UINT deviceCount = 0;
    
    // Create the VAM interface
    if (!VERIFY_SUCCEEDED(CoCreateInstance(
            CLSID_VoiceActivationManager,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_IVoiceActivationManager,
            (LPVOID *) &m_spVoiceActivationManager),
            L"Creating Voice Activation Manager interface"))
    {
        return false;
    }

    VERIFY_SUCCEEDED(spEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER));
    VERIFY_SUCCEEDED(spEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &spMMDeviceCollection));
    VERIFY_SUCCEEDED(spMMDeviceCollection->GetCount(&deviceCount));
    Log::Comment(String().Format(L"%d", deviceCount));
    for (UINT i = 0; i < deviceCount; i++)
    {
        VERIFY_SUCCEEDED(spMMDeviceCollection->Item(i, &spEndpoint));
        VERIFY_SUCCEEDED(spEndpoint.QueryInterface(&spEndpointInternal));
        VERIFY_SUCCEEDED(spEndpointInternal->GetDeviceInterfacePath(&wszDeviceId));
        
        // Initialize the VAM interface
        if (SUCCEEDED(m_spVoiceActivationManager->Initialize(wszDeviceId)))
        {
            // create event
            m_hStateChanged = CreateEvent(NULL, FALSE, FALSE, NULL);
            VERIFY_IS_NOT_NULL(m_hStateChanged);
    
            // query current state
            RtlQueryWnfStateData(
                &m_changeStamp,
                WNF_AVA_SOUNDDETECTOR_PATTERN_MATCH,
                NotificationCallback,
                this,
                NULL);
        
            RtlSubscribeWnfStateChangeNotification(
                &m_pWnfSubscription,
                WNF_AVA_SOUNDDETECTOR_PATTERN_MATCH,
                m_changeStamp,
                NotificationCallback,
                this,
                NULL,
                0,
                0);

            ResetEvent(m_hStateChanged);   
            
            return true;
        }
    }

    Log::Result(WEX::Logging::TestResults::Skipped, L"Skipping test: Could not initialize the voice activation manager on any endpoints.");
    
    return false;
}

NTSTATUS VoiceActivationManagerTests::NotificationCallback(
    _In_ WNF_STATE_NAME StateName,
    _In_ WNF_CHANGE_STAMP ChangeStamp,
    _In_ PWNF_TYPE_ID TypeId,
    _In_ PVOID CallbackContext,
    _In_ const VOID* Buffer,
    _In_ ULONG Length)
{
    VoiceActivationManagerTests * driverState = (VoiceActivationManagerTests *) CallbackContext;
    VERIFY_SUCCEEDED(driverState->m_csLock.Lock());

    // Todo: switch over to RtlWnfCompareChangeStamp(a,b)==1 once it gets published to a nt*.h correctly
    if(ChangeStamp > driverState->m_changeStamp)
    {
        if (Length > 0 && Buffer != nullptr)
        {
            if (!driverState->m_bExtraMatchAllowed)
            {
                VERIFY_IS_TRUE(driverState->m_pMatchData == nullptr);
            }

            if (!driverState->m_pMatchData)
            {
                PVOID pMatchData = (PVOID)CoTaskMemAlloc(Length);
                VERIFY_IS_TRUE(pMatchData != nullptr);
                CopyMemory(pMatchData, Buffer, Length);
                driverState->m_pMatchData = pMatchData;
            }
        }
        driverState->m_changeStamp = ChangeStamp;
        SetEvent(driverState->m_hStateChanged);
    }
    
    VERIFY_SUCCEEDED(driverState->m_csLock.Unlock());
    return 0;
}

DWORD VoiceActivationManagerTests::WaitForSingleObjectWithoutLock(DWORD dwWaitTime)
{
    DWORD dwWaitRet = 0;
    VERIFY_SUCCEEDED(m_csLock.Unlock());
    dwWaitRet = WaitForSingleObject(m_hStateChanged, dwWaitTime);
    VERIFY_SUCCEEDED(m_csLock.Lock());
    return dwWaitRet;
}
