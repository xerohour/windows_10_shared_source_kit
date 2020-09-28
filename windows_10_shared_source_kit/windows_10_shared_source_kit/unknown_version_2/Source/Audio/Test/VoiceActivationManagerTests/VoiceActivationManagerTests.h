/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#if (_MSC_VER >= 1000)
#pragma once
#endif

#include "precomp.h"

class VoiceActivationManagerTests: public WEX::TestClass<VoiceActivationManagerTests>
{
public:
    // Declare this class as a TestClass, and supply metadata if necessary.
    BEGIN_TEST_CLASS(VoiceActivationManagerTests)
        TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x86")
        TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client x64")
        TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client ARM")
        TEST_CLASS_PROPERTY(L"Kits.SupportedOS", L"Windows v10.0 Client ARM64")
        TEST_CLASS_PROPERTY(L"Kits.MinRelease", L"TH1")
        TEST_CLASS_PROPERTY(L"Kits.CorePackageComposition", L"Full")
        TEST_CLASS_PROPERTY(L"Kits.CorePackageComposition", L"Mobile")
        TEST_CLASS_PROPERTY(L"Kits.PublishingOrganization", L"Microsoft Corporation")
        TEST_CLASS_PROPERTY(L"Kits.TestType", L"Development")
        TEST_CLASS_PROPERTY(L"Kits.Specification", L"Device.Audio.Base.Discretional")
        TEST_CLASS_PROPERTY(L"Kits.DevelopmentPhase", L"Bring Up")
        TEST_CLASS_PROPERTY(L"Kits.TimeoutInMinutes", L"5")
        TEST_CLASS_PROPERTY(L"Kits.TestId", L"6834A4AC-94E8-4995-BE44-009835D0625C")
        TEST_CLASS_PROPERTY(L"Kits.TestId2", L"ee6dca31-f710-421a-bedc-0b3c01b95073")
        TEST_CLASS_PROPERTY(L"Kits.TestName", L"Audio Codec - Voice Activation Manager Tests")
        TEST_CLASS_PROPERTY(L"Kits.Description", L"Validate HW support for KSPROPSETID_SoundDetector properties")
        TEST_CLASS_PROPERTY(L"Kits.ExpectedRuntime", L"2")
    END_TEST_CLASS()

    // Test setup
    TEST_CLASS_SETUP(InitTests);
    TEST_CLASS_CLEANUP(DeinitTests);

    TEST_METHOD_SETUP(TestSetup);
    TEST_METHOD_CLEANUP(TestCleanUp);
    
    // Basic Property get tests
    TEST_METHOD(GetSupportedWakePatternTypesTest)
    TEST_METHOD(IsArmedTest)
    
    // Set Pattern tests
    TEST_METHOD(ProgramValidWakePatternsTest)
    TEST_METHOD(ProgramInvalidWakePatternsTest)
    
    // Arm&Disarm tests
    TEST_METHOD(ArmTest)
    TEST_METHOD(DoubleArmTest)
    TEST_METHOD(DoubleDisarmTest)
    TEST_METHOD(ProgramDuringArmTest)
    
    // Match Detection Test
    TEST_METHOD(MatchDetectionTest)
    
    // Match Detection Test
    TEST_METHOD(StreamTest)
    
private:
    CComPtr<IVoiceActivationManager> m_spVoiceActivationManager;

    CComCriticalSection m_csLock;
    PWNF_USER_SUBSCRIPTION m_pWnfSubscription;
    HANDLE m_hStateChanged;
    WNF_CHANGE_STAMP m_changeStamp;
    PVOID m_pMatchData;
    BOOL m_bExtraMatchAllowed;
    
    static NTSTATUS NotificationCallback(
        _In_ WNF_STATE_NAME StateName,
        _In_ WNF_CHANGE_STAMP ChangeStamp,
        _In_ PWNF_TYPE_ID TypeId,
        _In_ PVOID CallbackContext,
        _In_ const VOID* Buffer,
        _In_ ULONG Length);
    
    DWORD WaitForSingleObjectWithoutLock(DWORD dwWaitTime);
    VOID GetPatternDataforPatternType(GUID guidPatternType, SOUNDDETECTOR_PATTERNHEADER** ppPatternData);
    BOOL Initialize();
};
