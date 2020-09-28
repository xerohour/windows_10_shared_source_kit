//*@@@+++@@@@******************************************************************
//
// Microsoft Windows
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//

#include "EffectsDiscoveryTaefClass.h"
#include <pplx.cpp>

using namespace Windows::Foundation;


namespace EffectsDiscovery { 
    namespace Tests     {         
        namespace CPP     {


                CEffectsDiscoveryCPPTest* pEffectsDiscoveryTest = NULL; 
                ////////////////////////////////////////////////////////////////////////////
                bool TaefEffectsDiscoveryCppTests::WinRTClassSetup()
                { 
                    SetVerifyOutput verifySettings( VerifyOutputSettings::LogOnlyFailures );
                    VERIFY_SUCCEEDED(Windows::Foundation::Initialize( RO_INIT_MULTITHREADED ) ); // Async objects work better in MTAs     
                    VERIFY_IS_NOT_NULL(pEffectsDiscoveryTest = new CEffectsDiscoveryCPPTest());                    
                    return true;
                }
 
                ////////////////////////////////////////////////////////////////////////////
                bool TaefEffectsDiscoveryCppTests::WinRTClassCleanup()
                {
                    delete pEffectsDiscoveryTest;
                    Windows::Foundation::Uninitialize();                    
                    return true;
                }

                
                void TaefEffectsDiscoveryCppTests::AudioTestsCapture() 
                {                       

                    pEffectsDiscoveryTest->AudioTestsCapture();       
                }     

                void TaefEffectsDiscoveryCppTests::AudioTestsRender() 
                {                       
                    pEffectsDiscoveryTest->AudioTestsRender();       
                } 

                void TaefEffectsDiscoveryCppTests::DiscoverCaptureEffects() 
                {  
                    pEffectsDiscoveryTest->DiscoverCaptureEffects();   
                }

                void TaefEffectsDiscoveryCppTests::DiscoverRenderEffects() 
                {  
                    pEffectsDiscoveryTest->DiscoverRenderEffects();   
                }

 #ifndef BUILD_FOR_MOBILECORE
                void TaefEffectsDiscoveryCppTests::VerifyEnhancementsFromUIForCapture() 
                {   
                    pEffectsDiscoveryTest->VerifyEnhancementsFromUIForCapture();  
                }
                
                void TaefEffectsDiscoveryCppTests::VerifyEnhancementsFromUIForRender() 
                {   
                    pEffectsDiscoveryTest->VerifyEnhancementsFromUIForRender();  
                }
				
		        void TaefEffectsDiscoveryCppTests::ToggleLoudnessEq() 
                {                     
                    pEffectsDiscoveryTest->ToggleLoudnessEq(); 
                } 

                void TaefEffectsDiscoveryCppTests::ToggleLoudnessEqAfterDisable() 
                {                     
                    pEffectsDiscoveryTest->ToggleLoudnessEqAfterDisable(); 
                } 

                void TaefEffectsDiscoveryCppTests::TriggerEffectsDisableRender() 
                {   
                    pEffectsDiscoveryTest->TriggerEffectsDisableRender();
                }
#endif
        } /*CPP*/ 
    } /*Tests*/ 
} /*EffectsDiscovery*/ 
