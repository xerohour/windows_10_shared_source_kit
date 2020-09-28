//*****************************************************************************
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// File:         EffectsDiscoveryCPPTests.cpp
//
//*****************************************************************************
#include "EffectsDiscoveryCPPTests.h"


template <typename T>
struct lessObject : public std::binary_function<T, T, bool> {
    bool operator()(
        const T& _Left,
        const T& _Right
    ) const {
        return (void*)_Left < (void*)_Right;
    }
};

// Constructor/Destructor/Initializer
CEffectsDiscoveryCPPTest::CEffectsDiscoveryCPPTest(): CEffectsDiscoveryCPPTestCore() 
{}

CEffectsDiscoveryCPPTest::~CEffectsDiscoveryCPPTest()
{}


void CEffectsDiscoveryCPPTest::AudioTestsCapture()
{    
    DeviceInformationCollection^ pList = nullptr;
    EnumerateCaptureDevices(false, &pList);
    Log::Comment(L"\nEnumerating Devices Succeeded\n");
    DeviceInformation^ pDevice = nullptr;

    for (int i = 0; i < (signed)pList->Size; i++)
    {   
        pDevice = pList->GetAt(i);             
        LOG_OUTPUT(L"\tDeviceName: %s", pDevice->Name->Data());
        VerifyRawSupport(pDevice);
    }
}

void CEffectsDiscoveryCPPTest::AudioTestsRender()
{    
    DeviceInformationCollection^ pList = nullptr;
    EnumerateRenderDevices(&pList);
    Log::Comment(L"\nEnumerating Devices Succeeded\n");
    DeviceInformation^ pDevice = nullptr;

    for (int i = 0; i < (signed)pList->Size; i++)
    {   
        pDevice = pList->GetAt(i);             
        LOG_OUTPUT(L"\tDeviceName: %s", pDevice->Name->Data());
        VerifyRawSupport(pDevice);
	}
}

void CEffectsDiscoveryCPPTest::DiscoverCaptureEffects()
{    
    DeviceInformationCollection^ pList = nullptr;
    
    EnumerateCaptureDevices(false, &pList);    
    Windows::Devices::Enumeration::DeviceInformation ^pDevice = nullptr;
    Windows::Media::Effects::AudioCaptureEffectsManager ^captureEffectsManager;
    
    for (int i = 0; i < (signed)pList->Size; i++)
    {   
        pDevice = pList->GetAt(i);
        LOG_OUTPUT(L"\tDeviceName: %s", pDevice->Name->Data());
        bool rawProcessingSupported = ReadRawSupportSettings(pDevice->Id);
        for ( UINT c = (UINT)Windows::Media::Capture::MediaCategory::Other;
            c <= (UINT)Windows::Media::Capture::MediaCategory::Communications;
            c++
        ){
            Windows::Media::Capture::MediaCategory category = (Windows::Media::Capture::MediaCategory)c;            
            for (UINT ap = (UINT)Windows::Media::AudioProcessing::Default;
                ap <= (UINT)Windows::Media::AudioProcessing::Raw;
                ap++
            ) {
                Windows::Media::AudioProcessing audioprocessing = (Windows::Media::AudioProcessing)ap;                                
                if (Windows::Media::AudioProcessing::Raw == audioprocessing && !rawProcessingSupported) {
                    continue;
                }
                captureEffectsManager = AudioEffectsManager::CreateAudioCaptureEffectsManager(pDevice->Id, category, audioprocessing);
                ListEffectsAndVerifyCapture(captureEffectsManager, pDevice, category, audioprocessing); 
            }
        }
    }    
 }

void CEffectsDiscoveryCPPTest::DiscoverRenderEffects()
{    
    DeviceInformationCollection^ pList = nullptr;    
    EnumerateRenderDevices(&pList);    
    Windows::Devices::Enumeration::DeviceInformation ^pDevice = nullptr;
    Windows::Media::Effects::AudioRenderEffectsManager ^renderEffectsManager;
    
    for (int i = 0; i < (signed)pList->Size; i++)
    {   
        pDevice = pList->GetAt(i);
        LOG_OUTPUT(L"\tDeviceName: %s", pDevice->Name->Data());
        bool rawProcessingSupported = ReadRawSupportSettings(pDevice->Id);
        for ( UINT c = (UINT)Windows::Media::Render::AudioRenderCategory::Other;
            c <= (UINT)Windows::Media::Render::AudioRenderCategory::GameMedia;
            c++
        ){
            Windows::Media::Render::AudioRenderCategory category = (Windows::Media::Render::AudioRenderCategory)c;            
                for (UINT ap = (UINT)Windows::Media::AudioProcessing::Default;
                    ap <= (UINT)Windows::Media::AudioProcessing::Raw;
                    ap++
                ) {
                    Windows::Media::AudioProcessing audioprocessing = (Windows::Media::AudioProcessing)ap;                                
                    if (Windows::Media::AudioProcessing::Raw == audioprocessing && !rawProcessingSupported) {
                        continue;
                    }
                    renderEffectsManager = AudioEffectsManager::CreateAudioRenderEffectsManager(pDevice->Id, category, audioprocessing);
                    ListEffectsAndVerifyRender(renderEffectsManager, pDevice, category, audioprocessing); 
                }
        }
    }    
 }

 #ifndef BUILD_FOR_MOBILECORE
void CEffectsDiscoveryCPPTest::VerifyEnhancementsFromUIForCapture()
{    
    DeviceInformationCollection^ pList = nullptr;           
    Windows::Devices::Enumeration::DeviceInformation ^pDevice = nullptr;
    bool yPressed = false;
    bool nPressed = false;
    Windows::Media::Effects::AudioCaptureEffectsManager ^captureEffectsManager;  
    WCHAR pStr[65536];
    auto captureRegistrations = ref new
    Platform::Collections::Map<
        Platform::String ^, // device ID
        Windows::Foundation::Collections::IMap<
            Windows::Media::Effects::AudioCaptureEffectsManager ^, // manager object
            Windows::Foundation::EventRegistrationToken // token for the callback
        > ^
    >();
    EnumerateCaptureDevices(false, &pList);    
   
    for (int i = 0; i < (signed)pList->Size; i++)
    {   
        pDevice = pList->GetAt(i);    
        captureRegistrations->Insert(
                            pDevice->Id,
                            ref new Platform::Collections::Map<
                                Windows::Media::Effects::AudioCaptureEffectsManager ^,
                                Windows::Foundation::EventRegistrationToken,
                                lessObject< Windows::Media::Effects::AudioCaptureEffectsManager ^ >
                            >());                     
               
        bool rawProcessingSupported = ReadRawSupportSettings(pDevice->Id);
        Log::Comment(L"*****************************************************************");
        Log::Comment(L"*******Before proceeding please read instructions below********");
        Log::Comment(L"*****************************************************************");
        Log::Comment(L"Please launch the control panel and select all enhancements in the UI for capture device name that will be displayed below");
        Log::Comment(L"If you are unable to select all enhancements for a device at once please select one at a time");
        Log::Comment(L"Check the effects list returned");
        Log::Comment(L"Press Enter when you are done selecting all enhancements for the capture device"); 
        LOG_OUTPUT(L"\tStarting test on: %s", pDevice->Name->Data());
        for (
            UINT c = (UINT)Windows::Media::Capture::MediaCategory::Other;
            c <= (UINT)Windows::Media::Capture::MediaCategory::Communications;
            c++
            ){
                Windows::Media::Capture::MediaCategory category = (Windows::Media::Capture::MediaCategory)c;                
                for (
                    UINT ap = (UINT)Windows::Media::AudioProcessing::Default;
                    ap <= (UINT)Windows::Media::AudioProcessing::Raw;
                    ap++
                    ) {
                        Windows::Media::AudioProcessing audioprocessing = (Windows::Media::AudioProcessing)ap;                                
                        if (Windows::Media::AudioProcessing::Raw == audioprocessing && !rawProcessingSupported) {
                            continue;
                        }
                        captureEffectsManager = AudioEffectsManager::CreateAudioCaptureEffectsManager(pDevice->Id, category, audioprocessing);
                        auto callback = ref new
                                    Windows::Foundation::TypedEventHandler<
                                        AudioCaptureEffectsManager ^,
                                        Object ^
                                    >(
                                        [=](AudioCaptureEffectsManager ^source, Object ^args) {
                                                //need to capture return value of verifications
                                            ListEffectsAndVerifyCapture(captureEffectsManager, pDevice, category, audioprocessing); 
                                        }
                                    );     
                        auto token = captureEffectsManager->AudioCaptureEffectsChanged +=callback;
                        captureRegistrations->Lookup(pDevice->Id)->Insert(captureEffectsManager,token);
                        ListEffectsAndVerifyCapture(captureEffectsManager, pDevice, category, audioprocessing);
                    }
            }
			        
            WaitForEnter();
            Log::Comment(L"Were the enhancements listed accurately in all cases? Please enter Y or N:");
            WaitForYorN(&yPressed, &nPressed);
            if (nPressed)
            {
                Log::Comment(L"Please Enter more details on the failures observed. When done press enter:");
                ReadUserInput(pStr);
                Log::Error(pStr);
            }
            //unregister token
            if (!captureRegistrations->HasKey(pDevice->Id)) {
                Log::Warning(L"Couldn't find the registered token");
            }
                        
            auto registrations = captureRegistrations->Lookup(pDevice->Id);                        
            for ( auto iter = registrations->First();  iter->HasCurrent; iter->MoveNext()) {
            auto watcher = iter->Current->Key;
            auto token = iter->Current->Value;
            watcher->AudioCaptureEffectsChanged -= token;
            }
            captureRegistrations->Remove(pDevice->Id);
        }                       
}

void CEffectsDiscoveryCPPTest::VerifyEnhancementsFromUIForRender()
{    
    DeviceInformationCollection^ pList = nullptr;           
    Windows::Devices::Enumeration::DeviceInformation ^pDevice = nullptr;
    Windows::Media::Effects::AudioRenderEffectsManager ^renderEffectsManager;  
    bool yPressed = false;
    bool nPressed = false;
    Windows::Media::Effects::AudioCaptureEffectsManager ^captureEffectsManager;  
    WCHAR pStr[65536];
    auto renderRegistrations = ref new
    Platform::Collections::Map<
        Platform::String ^, // device ID
        Windows::Foundation::Collections::IMap<
            Windows::Media::Effects::AudioRenderEffectsManager ^, // manager object
            Windows::Foundation::EventRegistrationToken // token for the callback
        > ^
    >();
    EnumerateRenderDevices(&pList);
  
  
    for (int i = 0; i < (signed)pList->Size; i++)
    {   
        pDevice = pList->GetAt(i);    
        renderRegistrations->Insert(
                            pDevice->Id,
                            ref new Platform::Collections::Map<
                                Windows::Media::Effects::AudioRenderEffectsManager ^,
                                Windows::Foundation::EventRegistrationToken,
                                lessObject< Windows::Media::Effects::AudioRenderEffectsManager ^ >
                            >());                     
               
        bool rawProcessingSupported = ReadRawSupportSettings(pDevice->Id);

        Log::Comment(L"*****************************************************************");
        Log::Comment(L"*******Before proceeding please read instructions below********");
        Log::Comment(L"*****************************************************************");
        Log::Comment(L"Please launch the control panel and select all enhancements in the UI for each render device name that will be displayed below");
        Log::Comment(L"If you are unable to select all enhancements for a device at once please select one at a time");
        Log::Comment(L"Check the effects list returned");
        Log::Comment(L"Press Enter when you are done selecting all enhancements for render device");  
        LOG_OUTPUT(L"\tStarting test on: %s", pDevice->Name->Data());  

        for (
            UINT c = (UINT)Windows::Media::Render::AudioRenderCategory::Other;
            c <= (UINT)Windows::Media::Render::AudioRenderCategory::GameMedia;
            c++
            ){
                Windows::Media::Render::AudioRenderCategory category = (Windows::Media::Render::AudioRenderCategory)c;                
                for (
                    UINT ap = (UINT)Windows::Media::AudioProcessing::Default;
                    ap <= (UINT)Windows::Media::AudioProcessing::Raw;
                    ap++
                    ) {
                        Windows::Media::AudioProcessing audioprocessing = (Windows::Media::AudioProcessing)ap;                                
                        if (Windows::Media::AudioProcessing::Raw == audioprocessing && !rawProcessingSupported) {
                            continue;
                        }
                        renderEffectsManager = AudioEffectsManager::CreateAudioRenderEffectsManager(pDevice->Id, category, audioprocessing);
                        auto callback = ref new
                                    Windows::Foundation::TypedEventHandler<
                                        AudioRenderEffectsManager ^,
                                        Object ^
                                    >(
                                        [=](AudioRenderEffectsManager ^source, Object ^args) {                                                
                                            ListEffectsAndVerifyRender(renderEffectsManager, pDevice, category, audioprocessing); 
                                        }
                                    );     
                        auto token = renderEffectsManager->AudioRenderEffectsChanged +=callback;
                        renderRegistrations->Lookup(pDevice->Id)->Insert(renderEffectsManager,token);
                        ListEffectsAndVerifyRender(renderEffectsManager, pDevice, category, audioprocessing); 
                    }

            }
            
            WaitForEnter();
            Log::Comment(L"Were the enhancements listed accurately in all cases? Please enter Y or N:");
            WaitForYorN(&yPressed, &nPressed);
            if (nPressed)
            {
                Log::Comment(L"Please Enter more details on the failures observed. When done press enter:");
                ReadUserInput(pStr);
                Log::Error(pStr);
            }
            //unregister token
            if (!renderRegistrations->HasKey(pDevice->Id)) {
                Log::Warning(L"Couldn't find the registered token");
            }
                        
            auto registrations = renderRegistrations->Lookup(pDevice->Id);                        
            for ( auto iter = registrations->First();  iter->HasCurrent; iter->MoveNext()) {
            auto watcher = iter->Current->Key;
            auto token = iter->Current->Value;
            watcher->AudioRenderEffectsChanged -= token;
            }
            renderRegistrations->Remove(pDevice->Id);
        }                       
}
    
void CEffectsDiscoveryCPPTest::TriggerEffectsDisableRender()
{    
    DeviceInformationCollection^ pList = nullptr;  
    Windows::Devices::Enumeration::DeviceInformation ^pDevice = nullptr;
    Windows::Media::Effects::AudioRenderEffectsManager ^renderEffectsManager;
 
    
    EnumerateRenderDevices( &pList);
    
    SetupCommunications(eRender);

    for (int i = 0; i < (signed)pList->Size; i++)
    {  
        pDevice = pList->GetAt(i);            
        LOG_OUTPUT(L"\tDeviceName: %s", pDevice->Name->Data());

        auto obj = pDevice->Properties->Lookup(PKeyAudioDeviceGUID); 
        CPropVariant var;   
        if(!SetDisableSystemEffectsSetting(true, eRender, obj->ToString()->Data(), &var))
        {
            continue;
        } 

        if ( var.ulVal == ENDPOINT_SYSFX_DISABLED)
        {
            //The effects were already disabled no notifications will be raised
            continue;
        }
        //Wait for notifications to come in
        Sleep(10000);	

        CArrivalNotificationHandler* pNotify = GetNotificationHandler();


        Vector<Platform::String^>^ effectsList = pNotify->GetEffectsListInNotification();
        

        LOG_OUTPUT(L"Number of notifications received so far: %d",effectsList->Size);
        if ((signed)effectsList->Size != 0)
        {
            for (int i = 0;  i < (signed)effectsList->Size; i++)
            {
                LOG_OUTPUT(L"1st Notification is: %s", effectsList->GetAt(i)->Data());
                ProcessRenderEffectsList(effectsList->GetAt(i)->Data(), var, Verification::VerifyDisabled);            
            }	
            
            effectsList->Clear();
            pNotify->ClearNotifications();
        }
        else
        {
            Log::Error(L"No notifications were received");
        }	       

        if(!RestoreSystemEffectsSetting(var, eRender, obj->ToString()->Data()))
        {    
            Log::Error(L"Could not restore registry settings");    
        }
        //Wait for notifications
        Sleep(10000);	
        effectsList->Clear();
        
    }     

    TeardownCommunications();
} 

void CEffectsDiscoveryCPPTest::ToggleLoudnessEqAfterDisable()
{    
    DeviceInformationCollection^ pList = nullptr;  
    Windows::Devices::Enumeration::DeviceInformation ^pDevice = nullptr;
    Windows::Media::Effects::AudioRenderEffectsManager ^renderEffectsManager;
  
    EnumerateRenderDevices( &pList);

    for (int i = 0; i < (signed)pList->Size; i++)
    {  
        pDevice = pList->GetAt(i);            
        LOG_OUTPUT(L"\tDeviceName: %s", pDevice->Name->Data());

        auto obj = pDevice->Properties->Lookup(PKeyAudioDeviceGUID); 
        CPropVariant var,disablevar; 

        if(!SetDisableSystemEffectsSetting(true, eRender, obj->ToString()->Data(), &disablevar))
        {
            continue;
        } 

        if ( disablevar.ulVal == ENDPOINT_SYSFX_DISABLED)
        {
            //The effects were already disabled no notifications will be raised
            continue;
        }
        
        SetupCommunications(eRender);

        if(!RestoreSystemEffectsSetting(disablevar, eRender, obj->ToString()->Data()))
        {    
            Log::Error(L"Could not restore registry settings");    
        }

        //Wait for notifications to come in
        Sleep(10000);
        CArrivalNotificationHandler* pNotify = GetNotificationHandler();
        pNotify->ClearNotifications();

        if (!CheckforPresenceOfWMALFXGFXDSP(eRender, obj->ToString()->Data()))
        {
            TeardownCommunications();
            continue;
        }
        if(!ChangeLoudnessEq(eRender, obj->ToString()->Data(), &var))
        {
            TeardownCommunications();
            continue;
        }           
        //Wait for notifications to come in
        Sleep(10000);	
        
        Vector<Platform::String^>^ effectsList = pNotify->GetEffectsListInNotification();
        


        LOG_OUTPUT(L"Number of notifications received so far: %d",effectsList->Size);

        if ((signed)effectsList->Size != 0)
        {
            for (int i = 0;  i < (signed)effectsList->Size; i++)
            {
                LOG_OUTPUT(L"1st Notification is: %s", effectsList->GetAt(i)->Data());
                ProcessRenderEffectsList(effectsList->GetAt(i)->Data(), var, Verification::VerifyLoudnessEq);            
            }	
            
            effectsList->Clear();
            pNotify->ClearNotifications();
        }
        else
        {
            Log::Error(L"No notifications were received");
        }

        if(!RestoreLoudnessEq(var, eRender, obj->ToString()->Data()))
        {    
            Log::Error(L"Could not restore registry settings");    
        }
        //Wait for notifications
        Sleep(10000);	
        effectsList->Clear();
        TeardownCommunications(); 
        //Wait for teardown to complete. Else we will get error when we try to relaunch app
        Sleep(10000);
    }     
    TeardownCommunications(); 

} 


void CEffectsDiscoveryCPPTest::ToggleLoudnessEq()
{    
    DeviceInformationCollection^ pList = nullptr;  
    Windows::Devices::Enumeration::DeviceInformation ^pDevice = nullptr;
    Windows::Media::Effects::AudioRenderEffectsManager ^renderEffectsManager;
    
    
    EnumerateRenderDevices( &pList);

    
    SetupCommunications(eRender);

    for (int i = 0; i < (signed)pList->Size; i++)
    {  
        pDevice = pList->GetAt(i);            
        LOG_OUTPUT(L"\tDeviceName: %s", pDevice->Name->Data());

        auto obj = pDevice->Properties->Lookup(PKeyAudioDeviceGUID);                 
        CPropVariant disablevar;
        if(!SetDisableSystemEffectsSetting(false, eRender, obj->ToString()->Data(), &disablevar))
        {
            continue;
        } 

        if ( disablevar.ulVal == ENDPOINT_SYSFX_DISABLED)
        {
            //The effects were already disabled no notifications will be raised
            continue;
        }

        if (!CheckforPresenceOfWMALFXGFXDSP(eRender, obj->ToString()->Data()))
        {
            continue;
        }
        CPropVariant var; 
        if(!ChangeLoudnessEq(eRender, obj->ToString()->Data(), &var))
        {
            continue;
        }           
        //Wait for notifications to come in
        Sleep(10000);	

        CArrivalNotificationHandler* pNotify = GetNotificationHandler();
        Vector<Platform::String^>^ effectsList = pNotify->GetEffectsListInNotification();


        LOG_OUTPUT(L"Number of notifications received so far: %d",effectsList->Size);

        if ((signed)effectsList->Size != 0)
        {
            for (int i = 0;  i < (signed)effectsList->Size; i++)
            {
                LOG_OUTPUT(L"1st Notification is: %s", effectsList->GetAt(i)->Data());
                ProcessRenderEffectsList(effectsList->GetAt(i)->Data(), var, Verification::VerifyLoudnessEq);            
            }	
            VerifyNotificationsReceived();
            effectsList->Clear();
            pNotify->ClearNotifications();
        }
        else
        {
            Log::Error(L"No notifications were received");
        }
        if(!RestoreLoudnessEq(var, eRender, obj->ToString()->Data()))
        {    
            Log::Error(L"Could not restore registry settings");    
        }
        //Wait for notifications
        Sleep(10000);	
        effectsList->Clear();
        pNotify->ClearNotifications();
    }     

    TeardownCommunications();  
} 
#endif

   


    
  
    
