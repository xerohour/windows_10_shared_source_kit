//*****************************************************************************
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// File:         EffectsDiscoveryCPPTests.h
//
//*****************************************************************************
#pragma once

#include "EffectsDiscoveryCPPTestCore.h"



class CEffectsDiscoveryCPPTest: public CEffectsDiscoveryCPPTestCore
{
private:
       

public:
    CEffectsDiscoveryCPPTest();
    ~CEffectsDiscoveryCPPTest(); 
    
    void AudioTestsCapture();    
    void AudioTestsRender();  
    void AudioEffectsTests();
    void DiscoverCaptureEffects();
    void DiscoverRenderEffects();
    void VerifyEnhancementsFromUIForCapture();
    void VerifyEnhancementsFromUIForRender();
    void ToggleLoudnessEq();    
    void ToggleLoudnessEqAfterDisable();
    void TriggerEffectsDisableRender();

};