#pragma once

//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media Foundation
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//

// disable non-standard extension warning. This is necessary to use the 'override' 
// specifier in code that compiles at Warning level 4
// 
#pragma once
#pragma warning(disable: 4702)
#pragma warning(disable: 4100)
#pragma warning(disable: 4481)
#include "EffectsDiscoveryCPPTests.h"
#include <wchar.h>


// TAEF
#include <WexTestClass.h>
#include <wexstring.h>
using namespace WEX::Common;
using namespace WEX::Logging;
using namespace WEX::TestExecution;



#include "EffectsDiscoveryCPPTests.h"


namespace EffectsDiscovery { 
    namespace Tests     {         
        namespace CPP     {
            BEGIN_MODULE()
                MODULE_PROPERTY(L"Feature", L"EffectsDiscoveryCppTests")
                MODULE_PROPERTY(L"ThreadingModel", L"MTA")
            END_MODULE()
                class TaefEffectsDiscoveryCppTests : public WEX::TestClass<TaefEffectsDiscoveryCppTests>
                {
                public:
                    BEGIN_TEST_CLASS(TaefEffectsDiscoveryCppTests)                        
                    END_TEST_CLASS()

                    BEGIN_TEST_METHOD(AudioTestsCapture)  
                        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
                        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
                        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
                        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Capture'")
                    END_TEST_METHOD()

                    BEGIN_TEST_METHOD(AudioTestsRender) 
                        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
                        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
                        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
                        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render'")
                    END_TEST_METHOD()

                    BEGIN_TEST_METHOD(DiscoverCaptureEffects)                        
                        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
                        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
                        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
                        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Capture'")
                    END_TEST_METHOD()

                    BEGIN_TEST_METHOD(DiscoverRenderEffects)                        
                        TEST_METHOD_PROPERTY(L"HCKCategory", L"Basic")
                        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
                        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
                        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render'")
                    END_TEST_METHOD()

#ifndef BUILD_FOR_MOBILECORE
                    BEGIN_TEST_METHOD(VerifyEnhancementsFromUIForCapture)
                        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
                        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Capture'")
                    END_TEST_METHOD()

                    BEGIN_TEST_METHOD(VerifyEnhancementsFromUIForRender)                        
                        TEST_METHOD_PROPERTY(L"HCKCategory", L"Certification")
                        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render'")
                    END_TEST_METHOD()
                    
		            BEGIN_TEST_METHOD(ToggleLoudnessEq)
                        TEST_METHOD_PROPERTY(L"RunAs", L"Elevated")     
                        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
                        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render'")
                    END_TEST_METHOD()

     	            BEGIN_TEST_METHOD(ToggleLoudnessEqAfterDisable)
                        TEST_METHOD_PROPERTY(L"RunAs", L"Elevated")     
                        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
                        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render'")
                    END_TEST_METHOD()
                   
		            BEGIN_TEST_METHOD(TriggerEffectsDisableRender)
                        TEST_METHOD_PROPERTY(L"RunAs", L"Elevated")                        
                        TEST_METHOD_PROPERTY(L"TestClassification:Scope", L"Feature")
                        TEST_METHOD_PROPERTY(L"ResourceSelection", L"@Type = 'Render'")
                    END_TEST_METHOD()  
#endif

                    TEST_CLASS_SETUP(WinRTClassSetup);
                    TEST_CLASS_CLEANUP(WinRTClassCleanup);
                };       
                
        } /*CPP*/ 
    } /*Tests*/ 
} /*EffectsDiscovery*/ 