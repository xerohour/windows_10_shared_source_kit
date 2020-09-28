//--------------------------------------------------------------------------
//
//  File: TestCases.h
//
//  Copyright (C) Microsoft. All rights reserved.
//
//  Abstract:
//      Header for test cases.
//
//
//--------------------------------------------------------------------------
#pragma once

// Audio Render
DWORD WINAPI TC_RenderToAllEndpoints();
DWORD WINAPI TC_VerifyMuteOnCompressedEndpoint();

//Glitch Free
DWORD WINAPI TC_VerifyMultiRenderGlitchFree();
DWORD WINAPI TC_VerifyMultiCaptureGlitchFree();
DWORD WINAPI TC_VerifyStressedRenderGlitchFree();
DWORD WINAPI TC_VerifyStressedCaptureGlitchFree();

// Pull Mode
DWORD WINAPI TC_RenderExclusivePullMode();
DWORD WINAPI TC_CaptureExclusivePullMode();


// Generic
DWORD WINAPI TC_VerifyAllEndpointsAvailable();
DWORD WINAPI TC_Verify_BTH();
DWORD WINAPI TC_VerifyMicArrayRawFormat();
