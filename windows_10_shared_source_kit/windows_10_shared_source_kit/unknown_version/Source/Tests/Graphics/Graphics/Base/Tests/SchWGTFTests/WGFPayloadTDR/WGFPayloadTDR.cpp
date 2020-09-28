/******************************Module*Header*******************************
* Module Name: WGFPayloadTDR.h
*
* D3D11 Graphics and Compute TDR workload test
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/

#include "internal.h"

#define DEFINE_TYPES
#include "KernelAPIThunkTypes.h"
#include "KernelAPIThunks.h"

// Other globals
CKernelAPIThunks* gpBlueFish=NULL;;


#include "Globals.h"
#include "Compute_a_TDR.h"
#include <Quadrant.h>
#include "Graphics_TDR.h"
#include "WGFPayloadTDRResources.h"

#include <lddmcoreeventdefs.h>
#include <dxetwevt.hxx>
#include <DXGKernelConsumer.h>
#include <etwStatistics.h>
#include <DXGKernelStatistics.h>

#include "WGFPayloadTDR.h"
#include "etwPayload.h"

// this extern is defined in TestXKit which is referenced by CrossProceSharing. Define it here
// to some value, so that we don't have to link to the whole TestXKit
extern bool g_bCommonCtrlInit = false;

//------------------------------------------------------------------------------



using namespace testfactor;

//////////////////////////////////////////////////////////////////////////
// CFooTest
//////////////////////////////////////////////////////////////////////////

// Command line or User input values
RUN_TYPE grtOption=RUN_TYPE_GRAPHICS;
bool gbCalculateTDRWorkload=false;
UINT guiPayloadMS=0;
bool gbPoliceDDI=false;
bool gbTDR=false;
bool gbExpectTDR=false;

bool gbNonInteractive=false; // default is interactive apps - just like today's behavior.

bool gbNullAdapter=true;
bool gbMultiTDR=false;
bool gbMultiTDRSoftware=true; // Default is to use software device after 6 TDRs
bool gbValidateDelay=false;
bool gbWDDM12=false; // we will not check this support by default.
bool gbStress=false; // we will run for the specified time causing TDRs

UINT guiProcCount=0;
tstring gwszSyncWith;

UINT guiTime=30;
UINT guiLoop=1;

UINT gnLoopsInShader=1;

// Compute only options.
UINT gnThreads=10;

// Graphics only options
UINT gPixelsPerTriangle=1;
UINT gTrianglesPerPrimitive=1;
UINT gPrimitivesPerFrame=1;
UINT gQuadrant=0; // no quadrant


GRAPHICS_TYPE gGraphicsPreemptionGranularityRequested;
COMPUTE_TYPE gComputePreemptionGranularityRequested;

// Other globals



CPreemptionTest::CPreemptionTest() :
    m_rtOption(RUN_TYPE_GRAPHICS),
    m_bCalculateTDRWorkload(false),
    m_uiPayloadMS(0),
    m_bPoliceDDI(false),
    m_bTDR(false),
    m_bExpectTDR(false),
    m_bNonInteractive(false),
    m_bNullAdapter(true),
    m_bLockedOut(false),
    m_bNoLockOutEvent(true),
    m_bValidateDelay(false),
    m_bMultiTDRSoftware(true),
    m_bWDDM12(false),
    m_bStress(false),
    m_ulTdrLimitCount(EXPECTED_TDR_COUNT),
    m_uiProcCount(0),
    m_pSyncWith(NULL),
    m_uiLoop(1),
    m_uiTime(30),
    m_nThreads(0),
    m_nLoopsInShader(0),
    m_GraphicsPreemptionGranularityRequested(D3DKMDT_GRAPHICS_PREEMPTION_NONE),
    m_ComputePreemptionGranularityRequested(D3DKMDT_COMPUTE_PREEMPTION_NONE),
    m_DXGIFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
    m_Width(0),
    m_Height(0),
    m_SizeInBytes(0),
    m_Count(0),
    m_pData1(NULL),
    m_pData2(NULL),
    m_pDevice(NULL),
    m_pEffectiveContext(NULL),
    m_pDXGIDevice2(NULL),
    m_bIsSoftwareRenderDevice(false)
{
    ZeroMemory(m_pTex, sizeof(m_pTex[0]) * MAX_RESOURCE_COUNT);
    ZeroMemory(m_pResources, sizeof(m_pResources[0]) * MAX_RESOURCE_COUNT);
    ZeroMemory(m_pShaderResourceViews, sizeof(m_pShaderResourceViews[0]) * MAX_RESOURCE_COUNT);
    ZeroMemory(m_pRenderTargetViews, sizeof(m_pRenderTargetViews[0]) * MAX_RESOURCE_COUNT);
    ZeroMemory(m_pDiscarded, sizeof(m_pDiscarded[0]) * MAX_RESOURCE_COUNT);
    GetFramework()->m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;
}


void CPreemptionTest::InitTestParameters()
{
    // Create globals that are used in the RegisterOptionValues routine that get
    // set from the command line. Then, here, copy those values into local member
    // variables that can be used to build the factor of permutations that turn
    // out to be one.
    m_Width = 1024;
    m_Height = 1024;

    // The command line has already been parsed. Get our values here.
    m_rtOption = grtOption;
    m_bCalculateTDRWorkload= gbCalculateTDRWorkload;
    m_uiPayloadMS = guiPayloadMS;
    m_bPoliceDDI = gbPoliceDDI;
    m_bTDR = gbTDR;
    m_bExpectTDR=gbExpectTDR;
    m_bNonInteractive = gbNonInteractive;
    m_bNullAdapter = gbNullAdapter;
    m_bMultiTDR= gbMultiTDR;
    m_bMultiTDRSoftware = gbMultiTDRSoftware;
    m_GraphicsPreemptionGranularityRequested = (D3DKMDT_GRAPHICS_PREEMPTION_GRANULARITY)gGraphicsPreemptionGranularityRequested;
    m_ComputePreemptionGranularityRequested = (D3DKMDT_COMPUTE_PREEMPTION_GRANULARITY)gComputePreemptionGranularityRequested;
    m_uiProcCount = guiProcCount;
    m_pSyncWith = &gwszSyncWith;
    m_uiLoop = guiLoop;
    m_uiTime = guiTime;
    m_nThreads = gnThreads;
    m_nLoopsInShader = gnLoopsInShader;

    m_PixelsPerTriangle=gPixelsPerTriangle;
    m_TrianglesPerPrimitive=gTrianglesPerPrimitive;
    m_PrimitivesPerFrame = gPrimitivesPerFrame;
    m_Quadrant = gQuadrant;
    m_bValidateDelay = gbValidateDelay;
    m_bWDDM12 = gbWDDM12;
    m_bStress = gbStress;

    RFactor rfDummy = AddParameter( _T( "Dummy" ), &m_bDummy, new CUserValueSet<bool>(false,true) );
    SetRootTestFactor( rfDummy );
}

TEST_RESULT CPreemptionTest::Setup()
{
    TEST_RESULT tr = CD3D11Test::Setup( );
    if( tr != RESULT_PASS )
        return tr;

    m_pDevice = GetDevice();
    m_pEffectiveContext = GetEffectiveContext();

    HRESULT hr = m_pDevice->QueryInterface(__uuidof(*m_pDXGIDevice2), reinterpret_cast<void**>(&m_pDXGIDevice2));

    if(FAILED(hr))
    {
        WriteToLog(_T("CWGFPayloadTDRTest::Setup() - failed to create m_pDXGIDevice2, hr = 0x%X"), hr);
        return RESULT_BLOCKED;
    }

    return RESULT_PASS;
}

TEST_RESULT CPreemptionTest::SetupTestCase()
{
    assert(m_Width);
    assert(m_Height);
    
    UINT bytesPerPixel = GetBitsPerElement(m_DXGIFormat) / 8;
    m_SizeInBytes = m_Width * m_Height * bytesPerPixel;

    assert(!m_pData1);
    assert(!m_pData2);

    UINT DataSize = m_SizeInBytes/sizeof(ULONG);
    m_pData1 = new ULONG[DataSize];
    m_pData2 = new ULONG[DataSize];

    //m_pData2 is used to update texture data in resource
    for (UINT i = 0; i < DataSize; ++i)
    {
        m_pData2[i] = RESOURCE_PATTERN + i;
        m_pData1[i] = RESOURCE_PATTERN + i;
    }

    return RESULT_PASS;
}

void CPreemptionTest::CleanupTestCase()
{
    // Flush references to our resources
    if(m_pEffectiveContext)
    {
        m_pEffectiveContext->Flush();
    }

    for(UINT i = 0; i < m_Count; i++)
    {
        ClearIndex(i);
    }

    if(m_pData1)
    {
        delete[] m_pData1;
        m_pData1 = NULL;
    }

    if(m_pData2)
    {
        delete[] m_pData2;
        m_pData2 = NULL;
    }
}

void CPreemptionTest::ClearIndex(UINT Index)
{
    assert(Index < MAX_RESOURCE_COUNT);

    SAFE_RELEASE(m_pTex[Index]);
    SAFE_RELEASE(m_pResources[Index]);
    SAFE_RELEASE(m_pShaderResourceViews[Index]);
    SAFE_RELEASE(m_pRenderTargetViews[Index]);

    m_pTex[Index] = NULL;
    m_pResources[Index] = NULL;
    m_pShaderResourceViews[Index] = NULL;
    m_pRenderTargetViews[Index] = NULL;
}

// This routine checks the registry to validate that TdrDelay is not set to anything other
// than 2 (the standard two second delay).
TEST_RESULT CPreemptionTest::RegistryCheck()
{
    TEST_RESULT tr = RESULT_PASS;
    std::wstring msg;
    UINT uiTDrLimitTime=0;

    struct _AUTOKEY
    {
        _AUTOKEY()
        {
            hTdrKey = NULL;
            RegOpenKeyExW( HKEY_LOCAL_MACHINE, 
                           L"SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers", 
                           0, 
                           KEY_READ, 
                           &hTdrKey );
            hTdrKeyScheduler = NULL;
            RegOpenKeyExW( HKEY_LOCAL_MACHINE, 
                           L"SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers\\scheduler", 
                           0, 
                           KEY_READ, 
                           &hTdrKeyScheduler );
        }
        ~_AUTOKEY()
        {
            if( hTdrKey != NULL )
            {
                RegCloseKey( hTdrKey );
                hTdrKey = NULL;
            }
            if( hTdrKeyScheduler != NULL )
            {
                RegCloseKey( hTdrKeyScheduler );
                hTdrKeyScheduler = NULL;
            }
            
        }
        HKEY hTdrKey;
        HKEY hTdrKeyScheduler;
    }AUTO;

    
    if( AUTO.hTdrKey == NULL )
    {
        return RESULT_SKIP;
    }

    DWORD dwVar = 0;

    // Pass if key not found or if it equals the default 2.  Else, we'll have to skip.

    LONG lResult = ERROR_SUCCESS;
    DWORD dataSize = sizeof( DWORD );
    lResult = RegQueryValueExW( AUTO.hTdrKey, 
                                L"TdrDelay", 
                                NULL, 
                                NULL, 
                                (LPBYTE)&dwVar, 
                                &dataSize );

    if( lResult != ERROR_FILE_NOT_FOUND )
    {
        // Found the key. In the normal cases, we expect it to be 2 (seconds). The only 
        // speical case is when we're testing the delay functionality in the scheduler. When we do,
        // we have to bump the Delay time greater than 2 (to 10). 

        if( m_bValidateDelay )
        {
            if( dwVar != 10 )
            {
                tr = RESULT_SKIP;
                WriteToLog(_T(__FUNCTION__ "Expected the registry key TDRDelay to be set to 10."));
            }
        }
        else
        {
            if( dwVar != 2 )
            {
                tr = RESULT_SKIP;
                WriteToLog(_T(__FUNCTION__ "Expected the registry key TDRDelay to be set to 2."));
            }
        }
    }
    else
    {
        // didn't find the key, if we need it, we error out.
        if( m_bValidateDelay )
        {
            tr = RESULT_SKIP;
            WriteToLog(_T(__FUNCTION__ "Expected the registry key TDRDelay to be set to 10."));
        }
    }

    if(tr == RESULT_PASS)
    {
        // Now validate that if we're running the multi TDR case, we've got a TDRLimit longer
        // than normal.

        lResult = ERROR_SUCCESS;
        dataSize = sizeof( DWORD );
        lResult = RegQueryValueExW( AUTO.hTdrKey, 
                                    L"TdrLimitTime", 
                                    NULL, 
                                    NULL, 
                                    (LPBYTE)&dwVar, 
                                    &dataSize );
        if( lResult != ERROR_FILE_NOT_FOUND )
        {
            uiTDrLimitTime = dwVar;

            // Found the key, if we're performing multi TDRs, we expect it to be set to 600 (10 minutes)
            // else, we expect this to be the default 60 seconds.

            if( m_bMultiTDR | m_bValidateDelay )
            {
                if( dwVar < 600 )
                {
                    // The test was not setup correctly, we've got to skip.
                    tr = RESULT_SKIP;
                    WriteToLog(_T(__FUNCTION__ "Expected the registry key TdrLimitTime to be set to 600."));
                }
            }
            else
            {
                // the key must be 60 (the default if it's not set)

                if( dwVar != 60 )
                {
                    tr = RESULT_SKIP;
                    // Why is it not 60?
                    WriteToLog(_T(__FUNCTION__ "Expected the registry key TdrLimitTime to be set to 60."));
                }
            }
        }
        else
        {
            // didn't find the key, if we need it, we error out.
            if( m_bMultiTDR | m_bValidateDelay )
            {
                // The test was not setup correctly, we've got to skip.
                tr = RESULT_SKIP;
                WriteToLog(_T(__FUNCTION__ "Expected the registry key TdrLimitTime to be set to 600."));
            }
        }
    }

    if(tr == RESULT_PASS && m_bMultiTDR)
    {
        // We also need to get the TdrLimitCount for the multi-TDR tests.

        lResult = ERROR_SUCCESS;
        dataSize = sizeof( DWORD );
        lResult = RegQueryValueExW( AUTO.hTdrKey, 
                                    L"TdrLimitCount", 
                                    NULL, 
                                    NULL, 
                                    (LPBYTE)&dwVar, 
                                    &dataSize );
        if( lResult != ERROR_FILE_NOT_FOUND )
        {
            // We found the key, this is not the normal path. This means that someone adjusted this
            // registry key for testing TDR functionality. We're going to take this value and
            // use it as our default.
            m_ulTdrLimitCount = dwVar;

            // Now, we're going to put a limit on this value. We're going to assume that 
            // each TDR takes 15 seconds to process. Thus, we can't have a number that is 
            // larger then m_ulTdrLimitCount * 15 < uiTDrLimitTime

            if( (m_ulTdrLimitCount * 15) > uiTDrLimitTime )
            {
                tr = RESULT_SKIP;
                char szMsg[MAX_PATH];
                StringCchPrintf(szMsg,MAX_PATH,
                                __FUNCTION__ " - TdrLimitCount=%d, TdrLimitTime=%d Skipping for TdrLimitCount*15 > TdrLimitTime. Adjust and rerun!",
                                m_ulTdrLimitCount,uiTDrLimitTime);
                WriteToLog(szMsg);
            }
        }
        // else - Didn't find the key, this is the normal path. The default is already set
        // in the constructor.
    }

    // if we're running testing the penalty delay functionality, we'll need these keys 
    // to be standard values. In other words:
    //
    //  MaximumAllowedPreemptionDelay = 900 (or not exist)
    //  ContextSchedulingPenaltyDelay = 1000 (or not exist)
    //  EnableContextDelay = 1 (or not exist)
    //
    // Because no one should set these keys, we're going to validate that they are
    // not set.

    if( AUTO.hTdrKeyScheduler != NULL )
    {
        // We have the scheduler key to look at.
        if(tr == RESULT_PASS)
        {
            lResult = ERROR_SUCCESS;
            dataSize = sizeof( DWORD );
            lResult = RegQueryValueExW( AUTO.hTdrKeyScheduler, 
                                        L"MaximumAllowedPreemptionDelay", 
                                        NULL, 
                                        NULL, 
                                        (LPBYTE)&dwVar, 
                                        &dataSize );
            if( lResult != ERROR_FILE_NOT_FOUND )
            {
                // Found the key, if it's not 10 minutes, skip this test.
                if( dwVar < DELAY_MaximumAllowedPreemptionDelay )
                {
                    tr = RESULT_SKIP;
                }
            }
            if( tr != RESULT_PASS )
            {
                WriteToLog(_T(__FUNCTION__ "Found MaximumAllowedPreemptionDelay was not set to 900."));
            }
        }
        if(tr == RESULT_PASS)
        {
            lResult = ERROR_SUCCESS;
            dataSize = sizeof( DWORD );
            lResult = RegQueryValueExW( AUTO.hTdrKeyScheduler, 
                                        L"ContextSchedulingPenaltyDelay", 
                                        NULL, 
                                        NULL, 
                                        (LPBYTE)&dwVar, 
                                        &dataSize );
            if( lResult != ERROR_FILE_NOT_FOUND )
            {
                // Found the key, if it's not 10 minutes, skip this test.
                if( dwVar < DELAY_ContextSchedulingPenaltyDelay )
                {
                    tr = RESULT_SKIP;
                }
            }
            if( tr != RESULT_PASS )
            {
                WriteToLog(_T(__FUNCTION__ "Found ContextSchedulingPenaltyDelay was not set to 1000."));
            }
        }
        if(tr == RESULT_PASS)
        {
            lResult = ERROR_SUCCESS;
            dataSize = sizeof( DWORD );
            lResult = RegQueryValueExW( AUTO.hTdrKeyScheduler, 
                                        L"EnableContextDelay", 
                                        NULL, 
                                        NULL, 
                                        (LPBYTE)&dwVar, 
                                        &dataSize );
            if( lResult != ERROR_FILE_NOT_FOUND )
            {
                // Found the key, if it's not 10 minutes, skip this test.
                if( dwVar != DELAY_EnableContextDelay )
                {
                    tr = RESULT_SKIP;
                }
            }
            if( tr != RESULT_PASS )
            {
                WriteToLog(_T(__FUNCTION__ "Found EnableContextDelay was not set to 1."));
            }
        }
    }
    return tr;
}


// this routine logs information about the device that we're running on.
void CPreemptionTest::OutputDeviceCapabilities(bool _bRender)
{
    // What type of device are we displaying?
    if( _bRender )
    {
        WriteToLog(_T(__FUNCTION__ " - Dumping Render Device characteristics:"));
    }
    else
    {
        WriteToLog(_T(__FUNCTION__ " - Dumping Display Device characteristics:"));
    }

    D3DKMT_WDDM_1_2_CAPS Caps;
    gpBlueFish->GetCaps(&Caps, _bRender);

    WriteToLog(_T(__FUNCTION__ " - Graphics Preemption Level %d, %S"), 
               gpBlueFish->GetGraphicsPreemptionGranularity(_bRender),
               gpBlueFish->GraphicsPreemptionGranularityStr(_bRender));
    WriteToLog(_T(__FUNCTION__ " - Compute Preemption Level %d, %S"), 
               gpBlueFish->GetComputePreemptionGranularity(_bRender),
               gpBlueFish->ComputePreemptionGranularityStr(_bRender));

#define ISCAPSET(_Cap_) if(Caps._Cap_) {WriteToLog(_T(__FUNCTION__ " - Caps."#_Cap_"=1"));}

    ISCAPSET(SupportNonVGA);
    ISCAPSET(SupportSmoothRotation);
    ISCAPSET(SupportPerEngineTDR);
    ISCAPSET(SupportKernelModeCommandBuffer);
    ISCAPSET(SupportCCD);
    ISCAPSET(SupportSoftwareDeviceBitmaps);
    ISCAPSET(SupportGammaRamp);
    ISCAPSET(SupportHWCursor);
    ISCAPSET(SupportHWVSync);

#define ISPROPSET(_Prop_) if( gpBlueFish->##_Prop_(_bRender) ) { WriteToLog(_T(__FUNCTION__ " - " #_Prop_ "=true")); }

    // Output the flag combination for this machine
    ISPROPSET(IsSoftwareDevice);
    ISPROPSET(IsRenderOnly);
    ISPROPSET(IsDisplayOnly);
    ISPROPSET(IsRenderAndDisplay);
    ISPROPSET(IsPostDevice);
    ISPROPSET(IsBDAorBDD);
    
    if( _bRender )
    {
        WriteToLog(_T(__FUNCTION__ " - End (Render Device characteristics)"));
    }
    else
    {
        WriteToLog(_T(__FUNCTION__ " - End (Display Device characteristics)"));
    }
}

void CPreemptionTest::TimeLimitStart()
{
    m_dwTestStartTime = GetTickCount();
}

// This routine returns RESULT_SKIP if time limit has passed, RESULT_PASS otherwise.
TEST_RESULT CPreemptionTest::TimeLimit()
{
    if( (GetTickCount()-m_dwTestStartTime) > FIFTEEN_MINUTES_MS )
    {
        return RESULT_SKIP;
    }
    return RESULT_PASS;
}




#define RUN_COMPUTE false
#define RUN_GRAPHICS true

//------------------------------------------------------------------------------
//
// Command line options:
//
//   -RunType:graphics - run a graphics based workload
//            compute - run a compute based workload
//
//            default graphics if not specified.
//
//   -CalculateTDRWorkload:true - to have the test generate a TDR-able workload of the given
//                         RunType. false to use the parameters of the particular type. Note
//                         that if this parameter is used, the other workload variable parameters
//                         are ignored.
//
//                         default: false
//
//   -PayloadMS:N - Number of milliseconds that the workload should be. The actual should be within
//                  + or - 10% of the stated value.
//
//             default 0 - not used.
//
//   -TDR:true - to have the test generate a TDR of the given type.
//
//        default: false
//
//   -ExpectTDR:true - if we do not get a TDR, log failure
//              false - if we do get a TDR, log failure
//
//              default: false
//
//   -ValidateTDR:true - to have the test pass of fail based on the system TDRing.
//
//        default: false
//
//   -PoliceDDI:true - used to fail long QueryCurrentFence calls
//
//              default: false if not specified.
//
//   -Graphics:none - User is not requesting preemption level
//             dma_buffer - User wants preemption with dma buffer boundary validation
//             primitive - User wants preemption with primitive validation
//             triangle - User wants preemption with triangle validation
//             pixel - User wants preemption with pixel validation
//             shader - User wants preemption with shader validation
//
//             default: none if not specified.
//             
//   -Compute:none - User is not requesting preemption level
//            dma_buffer - User wants compute preemption with dma_buffer boundary validation
//            dispatch - User wants compute preemption with dispatch boundary validation
//            thread_group - User wants compute preemption with thread_group boundary validation
//            thread - User wants compute preemption with thread boundary validation
//            shader - User wants compute preemption with shader boundary validation
//
//            default: none if not specified.
//
//   -NonInteractive:true - The App says it will be scheduling large DMA packets
//                   false - The app is not preemption aware, all packets must be small
//
//                default false if not specified.
//
//   -Time:<X> - Number of seconds to run the workload, like "-Time:30"
//
//               default 30 seconds if not specified.
//
//   -Loop:<Y> - Number of times to loop over the workload. Total time will be Time(above)*Y. Use like: "-Loop:4"
//
//               default 1 if not specified.
//
//   -LoopsInShader:<N> - number of times to loop in the shader. This will determine the amount of 
//                      time spent in the hardware processing the work. Default 1. Numbers can be
//                      in the hundreds (or thousands).
//
//               default: 1 if not specified.
//
// For CrossProcSharing if that is used:
//
//   -SyncWith:string - a user generated string that is used to enable CrossProcSharing, if it's
//                      desired by the caller. Use like: -SyncWith:234fds543
//
//   -ProcCount:<N> - Number of processes the CrossProcSharing functionality will sync with. This
//                    should be a natural number greater than 1.
//
// Compute only options:
//
//   -Threads:<N> - Number of threads to create. Default 10 (range 1-100)
//
// Graphics only options:
//
//   -PixelsPerTriangle:<N> - Number of pixels for the triangle
//
//                            default: 1 if not specified.
//
//   -TrianglesPerPrimitive:<N> - Number of triangle for the triangle list (primitive)
//
//                            default: 1 if not specified.
//
//   -PrimitivesPerFrame:<N> - Number of times to call draw before each present
//
//                            default: 1 if not specified.
//
// Run like:
//
// For Graphics:
//
// WGFPayloadTDR.exe -NonInteractive:true -Graphics:dma_buffer -SyncWith:123rew432 -ProcCount:2 -Time:30 -Loop:3 
//
// WGFPayloadTDR.exe -NonInteractive:false -Graphics:dma_buffer -Time:30 -Loop:3 
//
// For Compute:
//
// WGFPayloadTDR.exe -Compute:dispatch -NonInteractive:false -Time:30 -ShaderLoop:100 -Threads:2
//
TEST_RESULT CPreemptionTest::ExecuteTestCase()
{
    TEST_RESULT tr = RESULT_PASS;

    WriteToLog(_T(__FUNCTION__ " - !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Start Test !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
    if( m_bDummy )
    {
        // We're always going to pass at least one test.
        WriteToLog(_T(__FUNCTION__ " - !!!!!!!!!!!!!!!!!!!!!!!!!!!! End Dummy Test !!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
        return tr;
    }


    // If someone has played with the TdrDelay time in the registry, the tests will
    // not execute as expected, thus we'll skip this test.
    tr = RegistryCheck();
    if( tr != RESULT_PASS )
    {
        TerminateCrossProsSharing();
        WriteToLog(_T(__FUNCTION__ " - !!!!!!!!!!!!!!!!!!!!!!!!!!!! Skip Test !!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
        return tr;
    }

    // Test should never run more then 15 minutes
    TimeLimitStart();

    // Determine the supported feature level 
    m_fl = GetDevice()->GetFeatureLevel();

    // TODO: this is the routine that's called for all the test routines. This is 
    // where the real work should occur. May be that this ExecuteTestCase routine
    // should live in the individual classes below. maybe not.
#ifdef DBG_MAX
    _DPF( "Test Case: %d, m_bNonInteractive=%S, Time=%d\n",
          GetFramework()->GetCurrentTestCase(),
          (m_bNonInteractive?L"true":L"false"),
          guiTime );
#endif
    // Here is where we need to check for the hardware capabilities. If the hardware is
    // not capable of preemption, we need to know about it.

    // Log preemption level
    WriteToLog(_T(__FUNCTION__ " - Graphics Preemption Level %d, %S"), 
               gpBlueFish->GetGraphicsPreemptionGranularity(true),
               gpBlueFish->GraphicsPreemptionGranularityStr(true));
    WriteToLog(_T(__FUNCTION__ " - Compute Preemption Level %d, %S"), 
               gpBlueFish->GetComputePreemptionGranularity(true),
               gpBlueFish->ComputePreemptionGranularityStr(true));
    WriteToLog(_T(__FUNCTION__ " - Dx Feature Level %d (0x%x)"), m_fl,m_fl);

    m_bIsSoftwareRenderDevice = gpBlueFish->IsSoftwareDevice(true);
    WriteToLog(_T(__FUNCTION__ " - IsSoftwareRenderDevice=%S"), 
               ( m_bIsSoftwareRenderDevice ? L"true":L"false" ));

    m_bSupportPerEngineTDR = gpBlueFish->SupportPerEngineTDR(true);
    // If the user places -WDDM12:true on the command line, we'll end up here to simply check flags
    if( m_bWDDM12 )
    {
        if( !m_bSupportPerEngineTDR )
        {
            WriteToLog(_T(__FUNCTION__ " - SupportPerEngineTDR not supported!"));
            tr = RESULT_FAIL;
        }
        else
        {
            WriteToLog(_T(__FUNCTION__ " - SupportPerEngineTDR is supported!"));
        }

        if( D3DKMDT_GRAPHICS_PREEMPTION_NONE == gpBlueFish->GetGraphicsPreemptionGranularity(true) )
        {
            WriteToLog(_T(__FUNCTION__ " - Graphics Preemption Granularity not reported! zero."));
            tr = RESULT_FAIL;
        }
        if( D3DKMDT_COMPUTE_PREEMPTION_NONE == gpBlueFish->GetComputePreemptionGranularity(true) )
        {
            WriteToLog(_T(__FUNCTION__ " - Compute Preemption Granularity not reported! zero."));
            tr = RESULT_FAIL;
        }

        // first we dump out the render device than the display device.
        OutputDeviceCapabilities(true);
        OutputDeviceCapabilities(false);

        WriteToLog(_T(__FUNCTION__ " - !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! End Test !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
        return tr;
    }

    // Before going further, if we have a software render device and the user is expecting
    // a TDR, we have a combination that will never happen. Also, because we're measuring 
    // DMA packets, software devices don't generate DMA packets. Thus, we can't run that test.
    if(  m_bIsSoftwareRenderDevice )
    {
        TerminateCrossProsSharing();
        WriteToLog(_T(__FUNCTION__ " - Will not get TDRs on with Software Render Device - Skipping."));
        WriteToLog(_T(__FUNCTION__ " - !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! End Test !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
        return RESULT_SKIP;
    }


    if( RUN_TYPE_COMPUTE == m_rtOption )
    {
        // If we need Compute preemption capabilities, but it's not available, we exit
        IF_TRUE_ACTION_JUMP(m_fl < D3D_FEATURE_LEVEL_11_0,
                            WriteToLog(_T(__FUNCTION__ " - Compute Requires Dx Feature level 11.0 or higher, Current Level=%d(0x%x)"),m_fl,m_fl);
                            tr = RESULT_SKIP;
                            TerminateCrossProsSharing(),
                                ExecuteTestCase_Ret);

        // If we need Compute preemption capabilities, but it's not available, we exit
        IF_TRUE_ACTION_JUMP(m_ComputePreemptionGranularityRequested > gpBlueFish->GetComputePreemptionGranularity(true),
                            WriteToLog(_T(__FUNCTION__ " - Requested Compute Preemption level %d too high for HW %d"), 
                                       m_ComputePreemptionGranularityRequested,
                                       gpBlueFish->GetComputePreemptionGranularity(true));
                            tr = RESULT_SKIP;
                            TerminateCrossProsSharing(),
                                ExecuteTestCase_Ret);

        // If we're expecting a TDR, yet the hardware supports preemption, we're going to gracefully fail
        IF_TRUE_ACTION_JUMP((m_bExpectTDR && 
                             ( gpBlueFish->GetComputePreemptionGranularity(true) > D3DKMDT_COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY)),
                            WriteToLog(_T(__FUNCTION__ " - Can't expect TDR on preemption capable hardware: HW level %d"), 
                                       gpBlueFish->GetComputePreemptionGranularity(true));
                            tr = RESULT_SKIP;
                            TerminateCrossProsSharing(),
                                ExecuteTestCase_Ret);

        if( m_bCalculateTDRWorkload )
        {
            UINT uiPayload=0;
            tr = CalculateComputeTDRWorkload(&uiPayload,true);
        }
        else
        {
            tr = Run(RUN_COMPUTE);
        }
    }
    else
    {
        // If we need Compute preemption capabilities, but it's not available, we exit
        IF_TRUE_ACTION_JUMP(m_fl < D3D_FEATURE_LEVEL_9_1,
                            WriteToLog(_T(__FUNCTION__ " - Graphics Requires Dx Feature level 9.1 or higher, Current Level=%d(0x%x)"),m_fl,m_fl);
                            tr = RESULT_SKIP;
                            TerminateCrossProsSharing(),
                                ExecuteTestCase_Ret);

        // If we need Graphics preemption capabilities, but it's not available, we exit
        IF_TRUE_ACTION_JUMP(m_GraphicsPreemptionGranularityRequested > gpBlueFish->GetGraphicsPreemptionGranularity(true),
                            WriteToLog(_T(__FUNCTION__ " - Requested Graphics Preemption level %d too high for HW %d"), 
                                       m_GraphicsPreemptionGranularityRequested,
                                       gpBlueFish->GetGraphicsPreemptionGranularity(true));
                            tr = RESULT_SKIP;
                            TerminateCrossProsSharing(),
                                ExecuteTestCase_Ret);

        // If we're expecting a TDR, yet the hardware supports preemption, we're going to gracefully fail
        IF_TRUE_ACTION_JUMP((m_bExpectTDR && 
                             (gpBlueFish->GetGraphicsPreemptionGranularity(true) > D3DKMDT_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY)),
                            WriteToLog(_T(__FUNCTION__ " - Can't expect TDR on preemption capable hardware: HW level %d"), 
                                       gpBlueFish->GetGraphicsPreemptionGranularity(true));
                            tr = RESULT_SKIP;
                            TerminateCrossProsSharing(),
                                ExecuteTestCase_Ret);
        
        if( m_bCalculateTDRWorkload )
        {
            UINT uiPayload=0;
            tr = CalculateGraphicsTDRWorkload(&uiPayload,true);
        }
        else
        {
            tr = Run(RUN_GRAPHICS);
        }
    }


ExecuteTestCase_Ret:
    WriteToLog(_T(__FUNCTION__ " - !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! End Test !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
    return tr;
}

// This routine checks to see if we got the lockout etw event during our processing pass.
void CPreemptionTest::LockOutCheck(ULONG _uiLoop)
{
    if( m_bLockedOut && m_ulTdrLimitCount == _uiLoop )
    {
        // expected
        char szMsg[MAX_PATH];
        StringCchPrintf(szMsg,MAX_PATH,__FUNCTION__ " - Recieved BlockGpuAccess as expected.");
        WriteToLog(szMsg);
        m_bNoLockOutEvent=false;
    }

    if( !m_bLockedOut && m_ulTdrLimitCount == _uiLoop )
    {
        // Error we should not have recieved one
        char szMsg[MAX_PATH];
        StringCchPrintf(szMsg,MAX_PATH,__FUNCTION__ " - Did not recieved BlockGpuAccess as expected!");
        WriteToLog(szMsg);
        m_bNoLockOutEvent=true;
    }

    if( m_bLockedOut && m_ulTdrLimitCount != _uiLoop )
    {
        // we got locked out on a non-expected TDR
        char szMsg[MAX_PATH];
        StringCchPrintf(szMsg,MAX_PATH,__FUNCTION__ " - Did not expected Lockout!");
        WriteToLog(szMsg);
        m_bNoLockOutEvent=true;
    }
}


TEST_RESULT CPreemptionTest::Run(bool _bGraphics)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;

    WriteToLog(_T(__FUNCTION__ " - _bGraphics %S"), (_bGraphics?L"true":L"false"));

    // If the user is asking for a particular DMA time, we calculate it before continuing.
    UINT uiActualPayloadMS = m_uiPayloadMS;
    if( m_uiPayloadMS )
    {
        if( _bGraphics )
        {
            tr = CalculateGraphicsTDRWorkload(&uiActualPayloadMS,false);
        }
        else
        {
            // if this comes back as success, uiActualPayloadMS will be the number of shader
            // instructions that we need to run to generate the workload that we're interested in
            // running.
            tr = CalculateComputeTDRWorkload(&uiActualPayloadMS,false);
        }
    }

    if( tr == RESULT_PASS )
    {
        WriteToLog(_T(__FUNCTION__ " - Run starting work.") );
        CXProcSyncClient* pxProcSync=NULL;

        if( WantCrossProcSharing() )
        {
            // The user want so synchronize via the CrossProcSharing class. We'll dynamically create one
            // here.
            pxProcSync = (CXProcSyncClient*) new CXProcSyncClient((char*)m_pSyncWith->c_str(),m_uiProcCount);
        }

        // Default is to run
        STATE_TYPE stRequest = eStateRun;

        // We're synchronizing cross process, we'll only want to work when the manager tells
        // us too.
        if( pxProcSync )
        {
            // If we synchronizing, we'll want to wait for the manager.
            stRequest = pxProcSync->TransitionTo(eStateReady);
        }

        if( stRequest != eStateTerminate )
        {
            if( m_bStress )
            {
                WriteToLog(_T(__FUNCTION__ " - Run starting stress.") );
                // loop for the user defined amount of time causing TDRs. We don't want to cause more
                // than 3 TDRs per minute in order to prevent the process from getting shut down (at
                // count of 6). Thus, we'll delay between each TDR in order to make it last at least 20 
                // seconds.
                //
                // Note that -Time: will come in as seconds. Thus expect values like 30*60=1800 for 
                // 30 minutes.
                ULONG ulStartLoopTime = GetTickCount(); // Get millisecond count
                ULONG ulCurTime = ulStartLoopTime;
                // have we exceeded our time?
                while( ((ulCurTime-ulStartLoopTime)/1000) < m_uiTime)
                {
                    // Perform our TDR
                    tr = Process(_bGraphics,pxProcSync,&stRequest,uiActualPayloadMS,false);
                    // We really don't care about the return code.

#define TDR_LOOP_TIME 20000
                    // We started at ulCurTime. so we're going to add 20 seconds (20000ms)
                    // and subtract the time it took in process in order to figure out
                    // the point in time we need to wake up.
                    ULONG ulSleepTime=0;
                    ULONG ulTimeToEndWait = (ulCurTime+TDR_LOOP_TIME); // Time before process call plus 20 seconds
                    ULONG ulNow = GetTickCount();
                    ULONG ulDiff = (ulNow-ulCurTime); // Time it took to call process (in milliseconds)
                     
                    if( ulNow < ulTimeToEndWait )
                    {
                        // If the current time is less than when we wanted to complete,
                        // we'll need to sleep. To make that caculation, we'll remove the time
                        // we've already run from our interval and sleep the rest.
                        ulSleepTime = TDR_LOOP_TIME - ulDiff;

                        // Stall until we've completed 20 seconds.
                        Sleep(ulSleepTime);
                    }

                    // Prepare for next loop
                    ulCurTime = GetTickCount();
                }
            }

            if( m_bMultiTDR )
            {
                WriteToLog(_T(__FUNCTION__ " - Run starting MultiTDR.") );
                UINT uiLoop = 1;
                // Now actually perform our test pass that creates the device and renders.
                // When we do, we expect that we'll get 6 TDRs and the 7th attempt will
                // fail due to the OS locking us out.

                char szMsg[MAX_PATH];

                // If we're using the NULL adapter, we should never get past m_ulTdrLimitCount
                // iterations in this loop.  If we're using an enumerated adapter, the m_ulTdrLimitCount+2th time 
                // will load the software driver by default. Thus, we'll set our loop limit
                // based on the adapter we're testing for.

                UINT uiTimesToLoop = m_ulTdrLimitCount;
                if( m_bNullAdapter )
                {
                    uiTimesToLoop += 2;
                }

                while( RESULT_PASS == tr && uiLoop <= uiTimesToLoop)
                {
                    tr = Process(_bGraphics,pxProcSync,&stRequest,uiActualPayloadMS,false);
                    StringCchPrintf(szMsg,MAX_PATH,"Processed Loop=%d, tr = %d, TimesToLoop=%d\n\r",
                                    uiLoop,tr,uiTimesToLoop);
#ifdef DBG
                    OutputDebugString(szMsg);
#endif
                    WriteToLog(szMsg);

                    // break before adjusting the count for uiLoop represents the number of successful
                    // Process calls.
                    if( tr != RESULT_PASS )
                    {
                        break;
                    }

                    // Never want to run longer then our time limit
                    tr = TimeLimit();
                    if( RESULT_PASS != tr )
                    {
                        PATH_TRAP();
                        WriteToLog(_T(__FUNCTION__ " - Exceeded Global Time Limit!"));
                        break;
                    }

                    // Did we recieve a lockout event?  we should not recieve one of these
                    // events unless we've hit the expected count.
                    LockOutCheck(uiLoop);

                    if( m_bLockedOut )
                    {
                        break;
                    }

                    uiLoop++;
                }


                // After m_ulTdrLimitCount TDRs, we should get a failure condition returned from Process().
                // At that point, we should be able to render to a software device.
                if( uiLoop < m_ulTdrLimitCount )
                {
                    PATH_TRAP();
                    // we should have run the loop 6 times. Why did we not?
                    char szMsg[MAX_PATH];
                    StringCchPrintf(szMsg,MAX_PATH,__FUNCTION__ " - Expected to loop more than %d times.",uiLoop);
                    WriteToLog(szMsg);
                }
                else if( uiLoop > m_ulTdrLimitCount )
                {
                    // we should not be allowed to loop more than 6 times. Why did we?
                    char szMsg[MAX_PATH];
                    StringCchPrintf(szMsg,MAX_PATH,__FUNCTION__ " - Expected to loop only %d times, actually looped %d times.",
                                    m_ulTdrLimitCount,uiLoop);
                    WriteToLog(szMsg);
                }
                else if( uiLoop == m_ulTdrLimitCount )
                {
                    // So far, everything is as expected.
                    tr = RESULT_PASS;

                    WriteToLog(_T(" - Recieved correct number of TDRs for process."));
                    // we TDR'd 6 times. This is good.  We got locked out as expected. Now, we are
                    // going to try one more time if the user asked us to use software. We should
                    // be able to successfully render to a software device here.

                    if( m_bMultiTDRSoftware )
                    {
                        WriteToLog(__FUNCTION__ " - Rendering to Software Device after lockout.");
                        // Now, we should be able to access a software device on this machine
                        // and render something. Thus we're going to do that here. We don't want
                        // to TDR, so let's make sure by overriding the booleans.
                        m_bTDR = false;
                        m_bExpectTDR = false;

                        tr = Process(_bGraphics,pxProcSync,&stRequest,1,true);
#ifdef DBG
                        char szDbg[MAX_PATH];
                        StringCchPrintf(szDbg,MAX_PATH,"Processed Lockout software request, tr = %d\n\r",tr);
                        OutputDebugString(szDbg);
#endif
                    }
                }

                // we're done with our loop, tell the manager to terminate.
                if( pxProcSync )
                {
                    pxProcSync->TransitionTo(eStateTerminate);
                    OutputDebugString(__FUNCTION__ " - transitioning to terminate\n\r");
                }
            }
            else
            {
                WriteToLog(__FUNCTION__ " - Standard Path.");
                // Common single pass path
                tr = Process(_bGraphics,pxProcSync,&stRequest,uiActualPayloadMS,false);
            }
        }
        else
        {
            WriteToLog(_T(__FUNCTION__ " - Run transitioning to terminate.") );
        }

        // cleanup.
        if( pxProcSync )
        {
            delete pxProcSync;
        }
    }
    else
    {
        TerminateCrossProsSharing();
    }
    WriteToLog(_T(__FUNCTION__ " - Leave, tr = %d"), tr);
    return tr;
}

#ifdef WANT_SIMULATE
int giCountPassOne=0;
int giCountProcess=0;
#endif



TEST_RESULT CPreemptionTest::Process(bool _bGraphics,
                                     CXProcSyncClient* pxProcSync,
                                     STATE_TYPE* _pstRequest,
                                     UINT uiActualPayloadMS,
                                     bool _bUseSoftwareDevice)
{
    TEST_RESULT tr = RESULT_PASS;
    UINT uiLoop = m_uiLoop;
    HRESULT hr = S_OK;

    WriteToLog(_T(__FUNCTION__ " - Enter _bGraphics %S, uiActualPayloadMS=%d, _bUseSoftwareDevice=%S"), 
               (_bGraphics?L"true":L"false"),uiActualPayloadMS,(_bUseSoftwareDevice?L"true":L"false"));

    // Create our Graphics class
    Graphics_TDR GraphicsTDR;
    // Create our Compute class
    Compute_a_TDR ComputeTDR;
    Base_TDR* pBaseTDR;

    if( _bGraphics )
    {
        pBaseTDR = &GraphicsTDR;

        // -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
        // If the user asks for a TDR workload
        GraphicsTDR.SetTDRShaderOn();
        GraphicsTDR.SetQuadrant(m_Quadrant);
        if( m_bTDR )
        {
            GraphicsTDR.SetLoopsInShader(2000000);
            GraphicsTDR.SetTime(5);
            GraphicsTDR.SetPixelsPerTriangle(250000); // Sets window size
            if( m_fl < D3D_FEATURE_LEVEL_10_0 )
            {
                GraphicsTDR.SetTrianglesPerPrimitive(10000);
            }
            else
            {
                GraphicsTDR.SetTrianglesPerPrimitive(100000);
            }
            GraphicsTDR.SetTrianglesPerPrimitive(100000);
            GraphicsTDR.SetPrimitivesPerFrame(250);
        }
        else
        {
            if( m_uiPayloadMS )
            {
                // if the actual payload in milliseconds equals one we'll start with a really 
                // small workload
                if( 1 == uiActualPayloadMS )
                {
                    GraphicsTDR.SetLoopsInShader(1);
                    GraphicsTDR.SetPixelsPerTriangle(500); // Sets window size
                    GraphicsTDR.SetTrianglesPerPrimitive(25);
                    GraphicsTDR.SetPrimitivesPerFrame(1);
                }
                else
                {
                    GraphicsTDR.SetLoopsInShader(20);
                    GraphicsTDR.SetPixelsPerTriangle(50000); // Sets window size
                    GraphicsTDR.SetTrianglesPerPrimitive(5000);
                    GraphicsTDR.SetPrimitivesPerFrame(uiActualPayloadMS);
                }
                GraphicsTDR.SetTime(m_uiTime);

            }
            else
            {
                GraphicsTDR.SetLoopsInShader(m_nLoopsInShader);
                GraphicsTDR.SetTime(m_uiTime);
                GraphicsTDR.SetPixelsPerTriangle(m_PixelsPerTriangle); // Sets window size
                GraphicsTDR.SetTrianglesPerPrimitive(m_TrianglesPerPrimitive);
                GraphicsTDR.SetPrimitivesPerFrame(m_PrimitivesPerFrame);
            }
        }
        if( m_bNonInteractive )
        {
            GraphicsTDR.SetNonInteractive();
        }

        GraphicsTDR.SelectAdapter(m_bNullAdapter);
        GraphicsTDR.SelectDeviceType(_bUseSoftwareDevice);
        GraphicsTDR.SetFeatureLevel(m_fl);
        // -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
    }
    else
    {
        pBaseTDR=&ComputeTDR;

        // -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
        ComputeTDR.SetTDRShaderOn();
        if( m_bTDR )
        {
            ComputeTDR.SetNumberOfThreads(1);
            ComputeTDR.SetTime(5);
            ComputeTDR.SetLoopsInShader(2000000);
        }
        else
        {
            if( m_uiPayloadMS )
            {
                // if we know the payload, apply it here.
                ComputeTDR.SetNumberOfThreads(1);
                ComputeTDR.SetTime(m_uiTime);
                ComputeTDR.SetLoopsInShader(uiActualPayloadMS);
            }
            else
            {
                // normal path
                ComputeTDR.SetNumberOfThreads(m_nThreads);
                ComputeTDR.SetTime(m_uiTime);
                ComputeTDR.SetLoopsInShader(m_nLoopsInShader);
            }
        }
        if( m_bNonInteractive )
        {
            ComputeTDR.SetNonInteractive();
        }
        ComputeTDR.SetFeatureLevel(m_fl);
        // -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
    }


    // *** --- ... --- *** --- ... --- *** --- ... --- *** --- ... --- *** --- ... --- 
    // Every time we call Initialize, we want to validate that the correct interactive flags
    // are set in the CreateDevie event. Thus, we'll listen for events around this call.
    CController etwController;
    CPayloadConsumer etwPayloadConsumer(&etwController); // To track that our device flags are set.
    etwController.AddConsumer(&etwPayloadConsumer);
    CDdiProfilerConsumer etwProfilerConsumer(&etwController); // To track Ddi times
    etwController.AddConsumer(&etwProfilerConsumer);

    // etwDMAConsumer is only used for Delay testing
    CDMAStandardTime etwDMAConsumer(&etwController,true,false); // To measure actual DMA packet time on hardware
    etwController.AddConsumer(&etwDMAConsumer);    

    etwController.StartProcessing();

    etwPayloadConsumer.Reset();
    etwPayloadConsumer.Start();
    etwProfilerConsumer.Reset();
    etwProfilerConsumer.Start();
    etwDMAConsumer.Reset();

    // Init must set things up, but it can not start processing until we call the Start() method!
    hr = pBaseTDR->Initialize();

#ifdef WANT_SIMULATE
    giCountProcess++;
    if( E_ABORT == hr )
    {
        char szDbg[MAX_PATH];
        StringCchPrintf(szDbg,MAX_PATH,
                        "%d " __FUNCTION__ " - Faking that we can not create a device.",
                        giCountProcess);

         // if we get back E_ABORT, we're simulating the failure, so we'll log that here.
        WriteToLog(szDbg);
        tr = RESULT_FAIL;

        etwPayloadConsumer.Stop();
    }
    else 
#endif
    // if we're unable to create a device, we'll have a failure code here
    if( S_OK != hr )
    {
        char szDbg[MAX_PATH];
        StringCchPrintf(szDbg,MAX_PATH,
                        "%d " __FUNCTION__ " - Unable to create device, hr=0x%08x.",
                        hr);

        // This flag should not be set.
        WriteToLog(szDbg);
        tr = RESULT_FAIL;

        etwPayloadConsumer.Stop();
    }
    else
    {
        // need to wait a little while for events to drain, It takes a while if we're running on checked bits.
        LetEventsDrain(&etwPayloadConsumer,3000,NULL);
        
        etwPayloadConsumer.Stop();

        // Did we get our CreateDevice event? We only check this on hardware devices because the
        // software device is not fully functional.
        if( !m_bMultiTDRSoftware && !_bUseSoftwareDevice)
        {
            bool bDisableGPUTimeout = etwPayloadConsumer.IsDisableGPUTimeoutSet();
            if(  m_bNonInteractive && !bDisableGPUTimeout ) 
            {
                // didn't see the creation flags that we were expecting.
                WriteToLog(_T(__FUNCTION__ " - FAIL - Expected to find DISABLE_GPU_TIMEOUT flag SET, but it was not."));
                tr = RESULT_FAIL;
            }
            if( !m_bNonInteractive && bDisableGPUTimeout )
            {
                // This flag should not be set.
                WriteToLog(_T(__FUNCTION__ " - FAIL - Expected to find DISABLE_GPU_TIMEOUT flag CLEAR, but it was not."));
                tr = RESULT_FAIL;
            }
        }
    }

    // *** --- ... --- *** --- ... --- *** --- ... --- *** --- ... --- *** --- ... --- 
    ULONG ulDelayCount = 0;        // Used to hold the count of our process delay notifications
    ULONG ulGlobalDelayCount = 0;  // Used to hold the count of all delay notifications
    ULONGLONG ullExecutionTime = 0;
    ULONG ulStartTime = 0;         // Used to calculate the actual running time
    ULONG ulEndTime = 0;           // Used to calculate the actual running time
    ULONG ulIntentionalDelay = 0;  // holds the total time we intentionally delay (usually waiting for events to drain).

    if( RESULT_PASS == tr )
    {
        if( m_bMultiTDR || m_bValidateDelay )
        {
            // Let's reset the Payload consumer and listen for our lockout event
            etwPayloadConsumer.Reset();
            etwPayloadConsumer.Start();
            etwDMAConsumer.Reset();
            etwDMAConsumer.Start();
            ulStartTime = GetTickCount();
        }
#ifdef DBG
        OutputDebugString("Process - starting while loop\n");
#endif
        // We're going to loop here as long as the manager doesn't ask us to terminate.
        while( *_pstRequest != eStateTerminate )
        {
            tr = OnePass(pBaseTDR,&etwController,&etwProfilerConsumer,&ulIntentionalDelay);
            if( RESULT_PASS != tr )
            {
                // Had a failure above, no need to continue looping.
                if( pxProcSync )
                {
                    pxProcSync->TransitionTo(eStateTerminate);
                    OutputDebugString(__FUNCTION__ " - transitioning to terminate\n\r");
                }
                break;
            }

            // !!NOTE!! the only variable that we can modified if we loop is PrimitivesPerFrame. 
            // The others are used during setup so they can't be changed.

            if( pxProcSync )
            {
                // Check to see if the manager wants us to finish
                *_pstRequest = pxProcSync->CheckForRequest();
            }


            // now determine if we've been through the loop the correct number of times.
            uiLoop--;
            if( 0 == uiLoop )
            {
                WriteToLog(_T(__FUNCTION__ " - Leaving due to Loop iterator"));
                break;
            }

            if( pxProcSync )
            {
                PATH_TRAP();
                // Note that if we time out in the above loop, our state will be eStateRun. Thus
                // we will not ask for a state change. Because of this, the manager timeout should
                // be shorter than the timeout here.
                if( *_pstRequest != eStateRun )
                {
                    PATH_TRAP();
                    // The Manager has asked us to transition into another state, let's do that.
                    // If that request is to go into a ready state, we'll go there and loop until
                    // the manager switches us back into a Running state. If we do get switched 
                    // back into a running state, we're going to perform the same workload that we
                    // did originally.
                    *_pstRequest = pxProcSync->TransitionTo(eStateReady);
                }
            }
        }

#ifdef DBG
        OutputDebugString("Process - done with while loop\n");
#endif
        if( m_bMultiTDR || m_bValidateDelay )
        {
            etwDMAConsumer.Stop();

            // need to wait a little while for events to drain, It takes a while if we're running on checked bits.
            LetEventsDrain(&etwDMAConsumer,3000,&ulIntentionalDelay);
            etwPayloadConsumer.Stop();

            ulEndTime = GetTickCount();
            // did we recieve our lock out event or not?
            m_bLockedOut = etwPayloadConsumer.IsBlockGpuAccessBlocked();
            if( m_bLockedOut )
            {
                char szMsg[MAX_PATH];
                StringCchPrintf(szMsg,MAX_PATH,__FUNCTION__ " - Recieved BlockGpuAccess ETW Event.");
                WriteToLog(szMsg);
            }

            // Read the number of times we were delayed here.
            ulDelayCount = etwPayloadConsumer.GetDelayContextSchedulingCount();
            ulGlobalDelayCount = etwPayloadConsumer.GetGlobalDelayContextSchedulingCount();

            // Get the total execution time
            ullExecutionTime = etwDMAConsumer.GetTotalValue();
#ifdef DBG
            {
                char szMsg[MAX_PATH];
                StringCchPrintf(szMsg,MAX_PATH,
                                __FUNCTION__ " - MultiTDR=%s, ValidateDelay=%s, ulDelayCount=%d, GlobalDelayCount=%d, ExecutionTime=%I64d",
                                (m_bMultiTDR?"true":"false"),
                                (m_bValidateDelay?"true":"false"),
                                ulDelayCount,ulGlobalDelayCount,ullExecutionTime);

                // This flag should not be set.
                WriteToLog(szMsg);
            }
#endif
        }
    }
    else
    {
        if( pxProcSync 
#ifdef WANT_SIMULATE
            && E_ABORT != hr 
#endif
            )
        {
            pxProcSync->TransitionTo(eStateTerminate);
            OutputDebugString(__FUNCTION__ " - transitioning to terminate\n\r");
        }
    }

    etwProfilerConsumer.Stop();
    etwController.StopProcessing();

    // Calculate our delay time
    if( ulDelayCount )
    {
        // Here, our payload has generated delay notifications, if this is NOT expected, we'll
        // error out. But we may get a random delay or two depending our the workload.
        if( !m_bValidateDelay && ulDelayCount > 2 )
        {
            char szMsg[MAX_PATH];
            StringCchPrintf(szMsg,MAX_PATH,
                            "%d " __FUNCTION__ " - We got Delay notifications when we were not expecting them.");

            // This flag should not be set.
            WriteToLog(szMsg);
            tr = RESULT_FAIL;
        }
        else
        {
            // If we don't get enough delay notifications, we probably calculated a workload
            // that is too light. we're expecting somewhere near one every couple seconds. This would
            // give us m_uiTime/2 or so events. In any case, we'll make sure we're getting 
            // at least 30 events. If not, we'll skip
            if( ulDelayCount < 30 )
            {
                char szMsg[MAX_PATH];
                StringCchPrintf(szMsg,MAX_PATH,
                                __FUNCTION__ ": To few samples with this workload (ulDelayCount=%d)\n\r",
                                ulDelayCount);
#ifdef DBG
                OutputDebugString(szMsg);
#endif
                // Fail we're outside the boundary
                WriteToLog(szMsg);
                tr = RESULT_SKIP;
            }
            else
            {
                // m_uiTime - amount of time user asked to run.
                char szMsg[MAX_PATH];

                ULONGLONG ullTotalTimeMS = 0;
                // now convert into milliseconds
                ullExecutionTime = etwController.TimeStampToMilliseconds(ullExecutionTime);

                // It was expected, now determine if the delay time is what we were expecting.
                // To do this, we'll multiple the delay count by the default delay threshold (1 second).
                // To that we'll add our running time (Standard DMAConsumer) the result should be
                // within a small tolerance of the full running time.  For instance, if we
                // run for 5 minutes (300seconds) our calculation should fall between
                //  300-x% < Result < 300+x% or something like that. We will expect the 
                // total time to be greater than or equal to 5 minutes of runtime.

                ullTotalTimeMS = ullExecutionTime + 
                    ((ULONGLONG)ulDelayCount*(ULONGLONG)DELAY_ContextSchedulingPenaltyDelay) +
                    ulIntentionalDelay;
                // What is our tolerance? 5%
#define TOLERANCE_PERCENT 0.10
                ULONGLONG ullTolerance = (ULONGLONG)((float)(m_uiTime*1000) * TOLERANCE_PERCENT);

                // Want:
                //
                // Expected time - time that we want the test to run
                // Actual time - how long did the test actually run
                // GPU Used - The amount of time we use on the GPU
                // Delay time - The amount of time we were delayed by the scheduler
                // Waiting time - the (ulIntentionalDelay) time we spend waiting for events to drain.
                // Bounds (min&max) - upper and lower bounds for the test range

                StringCchPrintf(szMsg,MAX_PATH,"Expected Time=%d ms",(m_uiTime*1000));
                WriteToLog(szMsg);
                StringCchPrintf(szMsg,MAX_PATH,"Actual Time=%d ms",(ulEndTime - ulStartTime));
                WriteToLog(szMsg);
                StringCchPrintf(szMsg,MAX_PATH,"GPU used=%I64d ms",ullExecutionTime);
                WriteToLog(szMsg);
                StringCchPrintf(szMsg,MAX_PATH,"Delay Time=%I64d ms, Count=%d",
                                ((ULONGLONG)ulDelayCount*(ULONGLONG)DELAY_ContextSchedulingPenaltyDelay),
                                ulDelayCount);
                WriteToLog(szMsg);
                StringCchPrintf(szMsg,MAX_PATH,"Waiting time=%d ms",ulIntentionalDelay);
                WriteToLog(szMsg);
                // Min and max 
                StringCchPrintf(szMsg,MAX_PATH,"Min bounds=%I64d,Max bounds=%I64d",
                                ((ULONGLONG)(m_uiTime*1000) - ullTolerance),
                                ((ULONGLONG)(m_uiTime*1000) + ullTolerance));
                WriteToLog(szMsg);

                // Now what we measure
                StringCchPrintf(szMsg,MAX_PATH,"(GPU Used + Delay Time) equals Total Time=%I64d ms",ullTotalTimeMS);
                WriteToLog(szMsg);


                if( ( ullTotalTimeMS < ( (ULONGLONG)((m_uiTime*1000)) - ullTolerance ) ) ||
                    ( ullTotalTimeMS > ( (ULONGLONG)((m_uiTime*1000)) + ullTolerance ) ) )
                {
                    StringCchPrintf(szMsg,MAX_PATH,
            __FUNCTION__ ": Delay time (%I64dms) resides outside %2.2f percent tolerance zone (DelayCount=%d)\n\r",
                                    ullTotalTimeMS,
                                    TOLERANCE_PERCENT,
                                    ulDelayCount);
#ifdef DBG
                    OutputDebugString(szMsg);
#endif
                    // Fail we're outside the boundary
                    WriteToLog(szMsg);
                    tr = RESULT_FAIL;
                }
                else
                {
                    StringCchPrintf(szMsg,MAX_PATH,
                                    __FUNCTION__ ": Delay time resides within bounds. WorkloadTime=%I64d, DelayCount=%d",
                                    ullTotalTimeMS,
                                    ulDelayCount);
                    WriteToLog(szMsg);
                }
            }
        }
    }
    else
    {
        char szMsg[MAX_PATH];
        StringCchPrintf(szMsg,MAX_PATH,
                        __FUNCTION__ ": GlobalDelayCount=%d\n\r",ulGlobalDelayCount);
        WriteToLog(szMsg);


        // Did we expect a Delay Count? If so, we didn't get any. 
        if( m_bValidateDelay )
        {
            StringCchPrintf(szMsg,MAX_PATH,
                            __FUNCTION__ ": Recieved no DelayContextScheduling Events\n\r");
#ifdef DBG
            OutputDebugString(szMsg);
#endif
            // Fail we're outside the boundary
            WriteToLog(szMsg);

            // Why did we not get any events?  If our workload was too small, this is expected.
            // If that is the case, we should skip rather than fail.

            ULONGLONG ullLongestConsecutiveTimens = etwDMAConsumer.LongestConsecutiveTime();
            // Now convert the time value into Microsecond values.
            ULONGLONG ullTotalConsecutiveCountms = etwController.TimeStampToMilliseconds(ullLongestConsecutiveTimens);

            if( ullTotalConsecutiveCountms <= DELAY_MaximumAllowedPreemptionDelay )
            {
                StringCchPrintf(szMsg,MAX_PATH,
                                __FUNCTION__ ": Longest time of execution %I64d, thus skipping.\n\r",ullTotalConsecutiveCountms);
#ifdef DBG
                OutputDebugString(szMsg);
#endif
                WriteToLog(szMsg);
                // We never had a packet run at least 1 second, this is in line with 
                // not getting any delay notifications
                tr = RESULT_SKIP;
            }
            else
            {
                StringCchPrintf(szMsg,MAX_PATH,
                                __FUNCTION__ ": Longest time of execution %I64d, thus failing.\n\r",ullTotalConsecutiveCountms);
#ifdef DBG
                OutputDebugString(szMsg);
#endif
                WriteToLog(szMsg);
                tr = RESULT_FAIL;
            }
        }
    }

//#ifdef DBG
    // For debugging, we'll enable this because it doesn't make too much noise and it's, well, useful.
    {
        OutputDebugString("Process - Dumping the etwProfilerConsumer hit counts\n");

        // output statistics about the session.
        char szBuf[MAX_PATH];

        UINT uiHit;
        for(ULONG ulIndex=0;ulIndex<10100;ulIndex++)
        {
            uiHit=etwProfilerConsumer.GetHitCount(ulIndex);

            if( uiHit > 0 )
            {
                StringCchPrintf(szBuf,MAX_PATH,"%d: Hit %d\n\r",ulIndex,uiHit);
                WriteToLog(szBuf);
            }
        }
    }
//#endif
    pBaseTDR->Cleanup();

    WriteToLog(_T(__FUNCTION__ " - Leave, tr = %d"), tr);

    return tr;
}


TEST_RESULT CPreemptionTest::OnePass(Base_TDR* _pBaseTDR,
                                     CController* _petwController,
                                     CDdiProfilerConsumer* _petwProfilerConsumer,
                                     ULONG* _pulIntentionalDelay)
{
    TEST_RESULT tr=RESULT_PASS;

    WriteToLog(_T(__FUNCTION__ " - Enter"));

    //------------------------- Work -----------------------------------
    // Perform work here
    HRESULT hr = _pBaseTDR->Start();
    if( S_OK != hr )
    {
        WriteToLog(_T(__FUNCTION__ " - Failed Start()."));
        tr = RESULT_FAIL;
    }
    else
    {
        hr = _pBaseTDR->Work();
#ifdef WANT_SIMULATE
        giCountPassOne++;
        if( m_bExpectTDR )
        {
            char szDbg[MAX_PATH];
            StringCchPrintf(szDbg,MAX_PATH,
                            "%d " __FUNCTION__ " - Faking that we got a TDR so we can test code.",
                            giCountPassOne);

            // We're going to simulate that we recieved a TDR even though we didn't.
            hr = E_OUTOFMEMORY;
            WriteToLog(szDbg);
        }
#endif
        //------------------------------------------------------------------
        if( S_OK != hr )
        {
            // We got an error condition. This usually means that we TDR'd, but not for compute.
            // Let's wait and let our events drain and then validate our contidions.

            LetEventsDrain(_petwProfilerConsumer,15000,_pulIntentionalDelay);
            // Check to see if we got our ETW events and that they confirm a TDR or not.
            tr = ValidateThroughETW(_petwProfilerConsumer);

            // In the graphics only case, we'll also confirm that we were expecting this based
            // on the hresult.
            if( m_bExpectTDR )
            {
                // if we're expecting a TDR and we get one, we end up here.
                WriteToLog(_T(__FUNCTION__ " - Expecting TDR and recieved one."));
            }
        }
        else
        {
            // Got a success error condition back from work(). If we're expecting a TDR, in the 
            // graphics case we should have ended up above, but in the compute case we end up here.
            // Thus we'll validate through ETW. If that 

            LetEventsDrain(_petwProfilerConsumer,15000,_pulIntentionalDelay);
            tr = ValidateThroughETW(_petwProfilerConsumer);
        }

        if( RESULT_PASS == tr )
        {
            // Start with our QueryCurrentFence results
            tr = FailLongDdiCalls(_petwController, _petwProfilerConsumer);

#ifdef WANT_SIMULATE
            // If we're simulating a TDR, we don't care about long Ddi calls.
            tr = RESULT_PASS;
#endif
        }
    }

    WriteToLog(_T(__FUNCTION__ " - Leave, tr = %d"), tr);
    return tr;
}

// This routine returns success if the TDR condition matches what we expect.
TEST_RESULT CPreemptionTest::ValidateThroughETW(CDdiProfilerConsumer* _petwProfilerConsumer)
{
    TEST_RESULT tr=RESULT_PASS;

    // If ResetEngine got called, we experienced a TDR
    UINT uiResetEngineHit=_petwProfilerConsumer->GetHitCount(ETW_PROFILER_DdiResetEngine); // 5051
    UINT uiResetFromTimeoutHit=_petwProfilerConsumer->GetHitCount(ETW_PROFILER_DdiResetFromTimeout); // 5021
    UINT uiRestartFromTimeoutHit=_petwProfilerConsumer->GetHitCount(ETW_PROFILER_DdiRestartFromTimeout); // 5020
    if( uiResetEngineHit > 0 || uiResetFromTimeoutHit > 0 || uiRestartFromTimeoutHit > 0 )
    {
        // we hit a TDR during this pass.
        if( m_bExpectTDR )
        {
            // if we're expecting a TDR and we get one, we end up here.
            WriteToLog(_T(__FUNCTION__ " - Expecting TDR and recieved one (ETW)."));
        }
    }
    else
    {
        // we did not hit a TDR
        if( m_bExpectTDR )
        {
            // if we're expecting a TDR and we didn't get one, we end up here.
            WriteToLog(_T(__FUNCTION__ " - Expecting TDR but did not recieve one (ETW)."));
            tr = RESULT_FAIL;
        }
    }

    // if the device supports per engine TDRs, we're going to validate that that routine
    // was called here - but only if we're expecting a TDR.
    if( m_bSupportPerEngineTDR && m_bExpectTDR )
    {
        if( 0 == uiResetEngineHit )
        {
            WriteToLog(_T(__FUNCTION__ " - Did not recieved DdiResetEngine(5051) event when SupportPerEngineTDR is true."));
            tr = RESULT_FAIL;
        }
    }

    // Individual checks for the log file.
    if( uiResetEngineHit > 0 )
    {
        // recieved a ResetEngine event.
        WriteToLog(_T(__FUNCTION__ " - Recieved DdiResetEngine(5051) event."));
    }
    if( uiResetFromTimeoutHit > 0 )
    {
        // recieved a ResetFromTimeout event.
        WriteToLog(_T(__FUNCTION__ " - Recieved DdiResetFromTimeout(5021) event."));
    }
    if( uiRestartFromTimeoutHit > 0 )
    {
        // recieved a RestartFromTimeout event.
        WriteToLog(_T(__FUNCTION__ " - Recieved DdiRestartFromTimeout(5022) event."));
    }
    return tr;
}


void CPreemptionTest::TerminateCrossProsSharing()
{
    if( WantCrossProcSharing() )
    {

        // If we're syncing with other processes, create our cross proc sync class and terminate
        CXProcSyncClient xProcSync((char*)m_pSyncWith->c_str(),m_uiProcCount);

        xProcSync.TransitionTo(eStateReady);
        xProcSync.TransitionTo(eStateTerminate);
        OutputDebugString(__FUNCTION__ " - transitioning to terminate\n\r");
    }
}


// This routine walks the bucket list looking for the longest packets in each bucket. It will average
// those values together and return it.            
ULONGLONG CPreemptionTest::GetAverageDMALengthMS(CController *_petwController,
                                                 CDMAStandardTime* _petwDMAConsumer)
{
    ULONGLONG ullAveValue = 0;
    ULONGLONG ullFullSecondLongestCount=0;
    ULONGLONG ullFullSecondLongestValue=0;

    // Get the index to the most recent item in the list
    ULONG ulCurrentSecond = _petwDMAConsumer->GetCurrentSecondIndex();
    // Get the count of seconds in the queue
    ULONG ulQueueCount = _petwDMAConsumer->GetSecondsInQueue();


    // Walk from the most recent to the oldest
    for( int i=0;i<=(int)ulQueueCount;i++)
    {
        if( ((int)ulCurrentSecond - i) > 0 )
        {
            // if we have data in this bucket, show it.
            ULONG ulCount = _petwDMAConsumer->GetFullSecondCount((ulCurrentSecond-(ULONG)i));
            if( ulCount )
            {
                ullFullSecondLongestValue += _petwDMAConsumer->GetFullSecondLongest((ulCurrentSecond-(ULONG)i));
                ullFullSecondLongestCount++;
            }
        }
    }
    if( ullFullSecondLongestCount )
    {
        ullAveValue = ullFullSecondLongestValue/ullFullSecondLongestCount;
        // Convert to milliseconds
        ullAveValue = _petwController->TimeStampToMilliseconds(ullAveValue);
    }
    return ullAveValue;
}





// This routine monitors the DMA packets to see how long the current workload takes
// and if it doesn't take long enough, it adjusts the number of primitives per frame to
// grow the DMA packet.
//
//  If _puiPayload points to 0, we'll run forever. If it points to anything else,
// we'll run until we hit that workload value (close to it).
//
TEST_RESULT CPreemptionTest::CalculateGraphicsTDRWorkload(UINT* _puiPayloadMS,
                                                          bool _bEnableSyncWith)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr;
    UINT uiTime=5; // Five seconds at most for this discovery
    UINT nLoopsInShader=20;
    UINT PixelsPerTriangle=50000; // Just under 500x500 in size
    UINT TrianglesPerPrimitive=5000;
    UINT PrimitivesPerFrame=5; // This is what we grow.

    ULONG ulStartTick = GetTickCount();

    UINT uiPayloadMS = *_puiPayloadMS;
    UINT uiPayloadMSLower;  // Say, 3000-300 millisecond payloads.
    UINT uiPayloadMSUpper;  // Say, 3000+300 millisecond payloads.
    if( 0 != uiPayloadMS )
    {
        // +- 10% for the DMA packet.
        uiPayloadMSLower = uiPayloadMS - (UINT)((float)uiPayloadMS*0.1f);
        uiPayloadMSUpper = uiPayloadMS + (UINT)((float)uiPayloadMS*0.1f);

        // How long will we have to test?
        uiTime = max(((uiPayloadMS*2)/1000),20); // We'll work up to 20 seconds for every long packets
    }
    else
    {
        uiPayloadMS = (UINT)-1;
        uiPayloadMSLower = (UINT)-1; 
        uiPayloadMSUpper = (UINT)-1; 
    }

    CXProcSyncClient* pxProcSync=NULL;

    // only enable SyncWith functionality if we're on a path where we want it.
    if( _bEnableSyncWith && WantCrossProcSharing() )
    {
        // The user want so synchronize via the CrossProcSharing class. We'll dynamically create one
        // here.
        pxProcSync = (CXProcSyncClient*) new CXProcSyncClient((char*)m_pSyncWith->c_str(),m_uiProcCount);
    }

    // Default is to run
    STATE_TYPE stRequest = eStateRun;

    // We're synchronizing cross process, we'll only want to work when the manager tells
    // us too.
    if( pxProcSync )
    {
        // If we synchronizing, we'll want to wait for the manager.
        stRequest = pxProcSync->TransitionTo(eStateReady);
    }

    if( stRequest != eStateTerminate )
    {
        // now setup an ETW class to listen for etw events. We're going to monitor the 
        // length of time on the DMA packets from our process.
        // setup our event capturing infrastructure, add our counter and start events flowing
        CController etwController;
        CDMAStandardTime etwDMAConsumer(&etwController,true,false); // To measure actual DMA packet time on hardware
        etwController.AddConsumer(&etwDMAConsumer);    
        CDdiProfilerConsumer etwProfilerConsumer(&etwController); // To track Ddi times
        etwController.AddConsumer(&etwProfilerConsumer);
        etwController.StartProcessing();

        Graphics_TDR  GraphicsTDR;
        GraphicsTDR.SetTime(uiTime);
        GraphicsTDR.SetLoopsInShader(nLoopsInShader);
        GraphicsTDR.SetPixelsPerTriangle(PixelsPerTriangle);
        GraphicsTDR.SetTrianglesPerPrimitive(TrianglesPerPrimitive);
        GraphicsTDR.SetQuadrant(m_Quadrant);
        if( m_bNonInteractive )
        {
            GraphicsTDR.SetNonInteractive();
        }
        GraphicsTDR.SelectAdapter(m_bNullAdapter);
        GraphicsTDR.SetFeatureLevel(m_fl);
        GraphicsTDR.Initialize();

        bool bHitTDR=false;
        ULONGLONG ullAverageTime = 0; // A breakdown of the work that we did during our pass.
        ULONGLONG ullTotalConsecutiveCountms=0;
        ULONGLONG ullLongestConsecutiveTimens=0;
        ULONGLONG ullLastTimeThroughms=0;
        ULONG ulLastTimeThroughPrimitivesPerFrame=0;
        UINT iLoop=0;
#define AVE_PAYLOAD_COUNT 1 // If we want to average multiple passes, increase this number. One seems fairly accurate.
        UINT uiAveragePayloadCount=AVE_PAYLOAD_COUNT;
        UINT uiPrimitivesPerFrameMax=0xFFFFFFFF;
        do
        {
            GraphicsTDR.SetPrimitivesPerFrame(PrimitivesPerFrame);
            // Now for each loop, we'll reset our data

            WriteToLog(_T(__FUNCTION__ 
    " - Generating workload with PrimitivesPerFrame: %d, PixelsPerTriangle: %d, TrianglesPerPrimitive: %d, LoopsInShader: %d, iLoop=%d"),
                       PrimitivesPerFrame,
                       PixelsPerTriangle,
                       TrianglesPerPrimitive,
                       nLoopsInShader,
                       iLoop);

            etwDMAConsumer.Reset();
            etwDMAConsumer.Start();
            etwProfilerConsumer.Reset();
            etwProfilerConsumer.Start();

            // render for the given time period.
            hr = GraphicsTDR.Start();
            if( S_OK != hr )
            {
                WriteToLog(_T(__FUNCTION__ " - Failed Start()."));
                tr = RESULT_FAIL;
                break;
            }

            hr = GraphicsTDR.Work();
            if( S_OK == hr )
            {
                hr = GraphicsTDR.WaitForDMABufferCompletion();
            }

            ULONG ulWaitForEvents = 5000;  // five second wait for checked builds
            if( S_OK != hr )
            {
                // we must have TDR'd. Did we expect this or not?
                ulWaitForEvents = 5000; // Wait for at least 5 seconds for system to process TDR and reset driver
                bHitTDR = true;
            }
            LetEventsDrain(&etwProfilerConsumer,ulWaitForEvents,NULL);

            // determine results.

            // Start with our QueryCurrentFence results
            tr = FailLongDdiCalls(&etwController, &etwProfilerConsumer);
            if( RESULT_PASS != tr )
            {
                WriteToLog(_T(__FUNCTION__ " - Failed Long Ddi."));
                // Had a failure above, no need to continue looping.
                etwDMAConsumer.Stop();
                etwProfilerConsumer.Stop();
                break;
            }

            // Now look at our DMA packets
            ULONG ulPacketsProcessed = etwDMAConsumer.GetTotalCount();
            if( ulPacketsProcessed == 0 )
            {
                WriteToLog(_T(__FUNCTION__ " - Hardware not responsive enough to process 1 packet in 20 seconds!"));
                // Had a failure above, no need to continue looping.
                etwDMAConsumer.Stop();
                etwProfilerConsumer.Stop();
                tr = RESULT_SKIP;
                break;
            }

            tr = TimeLimit();
            if( RESULT_PASS != tr )
            {
                etwDMAConsumer.Stop();
                etwProfilerConsumer.Stop();

                WriteToLog(_T(__FUNCTION__ " - Exceeded Global Time Limit!"));
                break;
            }

            // Based on our last pass, go figure out the average DMA length
            ullAverageTime = GetAverageDMALengthMS(&etwController,&etwDMAConsumer);

            ullLongestConsecutiveTimens = etwDMAConsumer.LongestConsecutiveTime();
            // Now convert the time value into Microsecond values.
            ullTotalConsecutiveCountms = etwController.TimeStampToMilliseconds(ullLongestConsecutiveTimens);

            if( (ullTotalConsecutiveCountms == 0) && (ullLastTimeThroughms > 0) && (ulPacketsProcessed != 0) )
            {
                ullTotalConsecutiveCountms = ullLastTimeThroughms;
            }

            // Now it's time to determine if we can get a TDR-able workload. Each time through
            // the loop we're keeping the longest packet from the last time through so we can
            // compare it to the current longest time. basically, the difference should corrispond
            // to the parameters that we changed. If the last time, we had 1000 Triangles and
            // a longest packet that took 700ms and this time we have 2000 Triangles and a longest packet
            // of 1400ms we want to be able to say that if we add 1000 Triangles to our 2000 (making
            // 3000) we'll get a longest packet of 2100ms. 
            //
            // 1000/700 ~= 2000/1400 -> 3000 should generate 2100ms.
#ifdef DBG
            char szBuf[MAX_PATH];
            StringCchPrintf(szBuf,
                            MAX_PATH,
                            "%d: PrimitivesPerFrame=%d, LongestTime=%I64dms, LastLongestTime=%I64d hr=0x%x\n\r",
                            iLoop,
                            PrimitivesPerFrame,
                            ullTotalConsecutiveCountms,
                            ullLastTimeThroughms,
                            hr);
            OutputDebugStringA( szBuf );
            if( bHitTDR )
            {
                // we issued too much work.
                OutputDebugStringA( "TDR threshold has been reached!\n\r" );
            }
            else
            {
                // we need to issue more work.
                OutputDebugStringA( "Adjusting workload!\n\r" );
            }
#endif
            // If we are Interactive, no DMA packet should ever take more than two seconds
            if( !m_bNonInteractive && 
                (ullTotalConsecutiveCountms > 3000) && 
                !m_bValidateDelay )
            {
                // We should only fail if we're expecting a TDR.
                if( m_bExpectTDR)
                {
                    // we expected a TDR but did not recieve one. 
                    if( !m_bTDR )
                    {
                        // log an error that we're a standard interactive application and we're seeing
                        // DMA packets taking longer than 2 seconds.
                        WriteToLog(_T(__FUNCTION__ " - FAIL: Interactive with DMA packets greater than 2 seconds: %dms"),
                                   ullTotalConsecutiveCountms);
                        tr = RESULT_FAIL;
                    }
                    else
                    {
                        // expecting a TDR and recieved one
                        WriteToLog(_T(__FUNCTION__ " - Interactive with long DMA packet: %dms, recieved TDR"),
                                   ullTotalConsecutiveCountms);
                    }
                }
            }


            // Have we hit our out desired workload time?
            if( ullAverageTime > uiPayloadMSUpper )
            {
                // we're greater then the upper bounds, reduce our workload.

                // First set our upper limit to this calculation.
                if( uiPrimitivesPerFrameMax > PrimitivesPerFrame)
                {
                    // If our current upper limit is greater than our previous one, we'll lower it here.
                    uiPrimitivesPerFrameMax = PrimitivesPerFrame;
                }

                // Lower half way to our previous number

                if( (PrimitivesPerFrame-ulLastTimeThroughPrimitivesPerFrame) > 0 )
                {
                    PrimitivesPerFrame = (PrimitivesPerFrame-ulLastTimeThroughPrimitivesPerFrame)/2 + ulLastTimeThroughPrimitivesPerFrame;
                }
                else
                {
                    // last time through we used the same number, thus this time we'll drop by a brown number.
                    PrimitivesPerFrame -= (UINT)((float)PrimitivesPerFrame*0.08);
                }

                // when we calculate a new payload, we set the first time true
                uiAveragePayloadCount=AVE_PAYLOAD_COUNT;
                WriteToLog(_T(__FUNCTION__ " - Shrinking Payload, Longest DMA packet during pass: %I64dms, Average=%I64d, PrimitivesPerFrame=%d, Count=%d"),
                           ullTotalConsecutiveCountms,ullAverageTime,PrimitivesPerFrame,ulPacketsProcessed);
            }
            else if( ullAverageTime < uiPayloadMSLower )
            {
                ULONG ulTech=0;
                // we're lower then the lower bounds, increase our workload.
                // grow the number of triangles lists per frame

                // When we go to grow, we will make an assumption here about the workload 
                // and the amount of time it will take to process it on the hardware. That is,
                // if we double the workload, it will take twice as long on the hardware. Thus,
                // we'll use this info to make sure we never exceed our upper bounds with
                // the workload calculation.
                //
                // ullTotalConsecutiveCountms is the longest time that we just spent on the hardware
                //
                if( ullAverageTime*2 > uiPayloadMS )
                {
                    // if we double the workload we'll exceed our upper limit, thus we'll lighten the load
                    // by looking at the ratios of the work.
                    //
                    // ullTotalConsecutiveCountms/XppFrame = ullWantedTime/Y or
                    //
                    // Y = (ullWantedTime*XppFrame)/ullTotalConsecutiveCountms

                    UINT uiNew = (uiPayloadMS*PrimitivesPerFrame)/(ULONG)ullAverageTime;

                    if( uiNew > uiPrimitivesPerFrameMax )
                    {
                        // never exceed our unkown max. If our new calucation takes us too high
                        // simply more half way towards to known max

                        PrimitivesPerFrame += (uiPrimitivesPerFrameMax-PrimitivesPerFrame)/2;
                        ulTech=1;
                    }
                    else
                    {
                        PrimitivesPerFrame = uiNew;
                        ulTech=2;
                    }
                }
                else
                {
                    // when growing, if the calculation will generate a number above
                    // max, then we're going to take the number 1/2 way to max from where we
                    // are. binary search the sweet spot.

                    if( PrimitivesPerFrame*2 > uiPrimitivesPerFrameMax )
                    {
                        PrimitivesPerFrame += (uiPrimitivesPerFrameMax-PrimitivesPerFrame)/2;
                        ulTech=3;
                    }
                    else
                    {
                        // Just double our workload.
                        PrimitivesPerFrame *= 2;
                        ulTech=4;
                    }
                }
                // when we calculate a new payload, we set the first time true
                uiAveragePayloadCount=AVE_PAYLOAD_COUNT;
                WriteToLog(_T(__FUNCTION__ 
                " - Growing Payload, Longest DMA packet during pass: %I64dms, Average=%I64d, PrimitivesPerFrame=%d, Count=%d, ulTech=%d"),
                           ullTotalConsecutiveCountms,ullAverageTime,PrimitivesPerFrame,ulPacketsProcessed,ulTech);
            }
            else
            {
                // To double check that we've got the correct workload, we're going to continue if 
                // this is the first time through.
                if( 0 == uiAveragePayloadCount  )
                {
                    // we've generated a packet that is close enough. Let's write back our into
                    // and return RESULT_PASS.
                    etwDMAConsumer.Stop();
                    etwProfilerConsumer.Stop();
                    *_puiPayloadMS = PrimitivesPerFrame;

                    // What was the longest packet length?
                    WriteToLog(_T(__FUNCTION__ 
                    " - Longest DMA packet during pass: %I64dms, Average=%I64d, PrimitivesPerFrame=%d, Count=%d"),
                               ullTotalConsecutiveCountms,ullAverageTime,PrimitivesPerFrame,ulPacketsProcessed);

                    tr = RESULT_PASS;
                    break;
                }
                else
                {
                    // Don't adjust our workload and try it again. If it comes back with the 
                    // same influence, we'll use this packet.
                    WriteToLog(_T(__FUNCTION__ 
                    " - Found correct payload - %d, Average=%I64d, Longest DMA packet during pass: %I64dms, PrimitivesPerFrame=%d, Count=%d"),
                               uiAveragePayloadCount,ullAverageTime,ullTotalConsecutiveCountms,PrimitivesPerFrame,ulPacketsProcessed);
                    uiAveragePayloadCount--;
                }
            }

            ullLastTimeThroughms = ullTotalConsecutiveCountms;
            ulLastTimeThroughPrimitivesPerFrame = PrimitivesPerFrame;
            iLoop++;

            etwDMAConsumer.Stop();
            etwProfilerConsumer.Stop();

            if( ulPacketsProcessed == 0 )
            {
                PATH_TRAP();
                tr = RESULT_FAIL;
            }

            WriteToLog(_T(__FUNCTION__ " - End of While loop(%d), bHitTDR=%s, tr=%d"),iLoop,(bHitTDR?"true":"false"),tr);

        }while(iLoop < 20 && !bHitTDR  && tr == RESULT_PASS);

        if( iLoop == 20 )
        {
            PATH_TRAP();
        }

        etwController.StopProcessing();

#ifdef DBG_MAX
        {
            // output statistics about the session.
            char szBuf[MAX_PATH];

            UINT uiHit;
            for(ULONG ulIndex=0;ulIndex<10100;ulIndex++)
            {
                uiHit=etwProfilerConsumer.GetHitCount(ulIndex);

                if( uiHit > 0 )
                {
                    StringCchPrintf(szBuf,MAX_PATH,"%d: Hit %d\n\r",ulIndex,uiHit);
                    OutputDebugString(szBuf);
                }
            }
        }
#endif

        GraphicsTDR.Cleanup();

        // cleanup.
        if( pxProcSync )
        {
            delete pxProcSync;
        }
    }
    else
    {
        TerminateCrossProsSharing();
    }

    WriteToLog(_T(__FUNCTION__ " - Calculation process took %d seconds"),(GetTickCount()-ulStartTick)/1000);

    return tr;
}

TEST_RESULT CPreemptionTest::CalculateComputeTDRWorkload(UINT* _puiPayloadMS, 
                                                         bool _bEnableSyncWith)
{
    TEST_RESULT tr = RESULT_PASS;
    HRESULT hr = S_OK;
    UINT uiTime=5; // Five seconds at most for this discovery
    UINT nLoopsInShader=20;  // we'll 
    UINT nThreads=1;

    UINT uiPayloadMS = *_puiPayloadMS;
    UINT uiPayloadMSLower;  // Say, 3000-300 millisecond payloads.
    UINT uiPayloadMSUpper;  // Say, 3000+300 millisecond payloads.
    if( 0 != uiPayloadMS )
    {
        // +- 10% for the DMA packet.
        uiPayloadMSLower = uiPayloadMS - (UINT)((float)uiPayloadMS*0.1f);
        uiPayloadMSUpper = uiPayloadMS + (UINT)((float)uiPayloadMS*0.1f);

        // How long will we have to test?
        uiTime = max(((uiPayloadMS*2)/1000),5);
    }
    else
    {
        uiPayloadMS = (UINT)-1;
        uiPayloadMSLower = (UINT)-1; 
        uiPayloadMSUpper = (UINT)-1; 
    }

    CXProcSyncClient* pxProcSync=NULL;

    // only enable SyncWith functionality if we're on a path where we want it.
    if( _bEnableSyncWith && WantCrossProcSharing() )
    {
        // The user want so synchronize via the CrossProcSharing class. We'll dynamically create one
        // here.
        pxProcSync = (CXProcSyncClient*) new CXProcSyncClient((char*)m_pSyncWith->c_str(),m_uiProcCount);
    }

    // Default is to run
    STATE_TYPE stRequest = eStateRun;

    // We're synchronizing cross process, we'll only want to work when the manager tells
    // us too.
    if( pxProcSync )
    {
        // If we synchronizing, we'll want to wait for the manager.
        stRequest = pxProcSync->TransitionTo(eStateReady);
    }

    if( stRequest != eStateTerminate )
    {
        // now setup an ETW class to listen for etw events. We're going to monitor the 
        // length of time on the DMA packets from our process.
        // setup our event capturing infrastructure, add our counter and start events flowing
        CController etwController;
        CDMAStandardTime etwDMAConsumer(&etwController,true,false); // To measure actual DMA packet time on hardware
        etwController.AddConsumer(&etwDMAConsumer);    
        CDdiProfilerConsumer etwProfilerConsumer(&etwController); // To track Ddi Times
        etwController.AddConsumer(&etwProfilerConsumer);
        etwController.StartProcessing();

        // In order to loop on the compute class, we'll have to init, work and cleanup the class
        // before doing it again. We might have to wait for previous work to finish before looping.


        bool bHitTDR=false;
        ULONGLONG ullTotalConsecutiveCountms=0;
        ULONGLONG ullLongestConsecutiveTimens=0;
        ULONGLONG ullLastTimeThroughms=0;
        UINT iLoop=0;
        do
        {
            Compute_a_TDR  ComputeTDR;

            ComputeTDR.SetTDRShaderOn();
            ComputeTDR.SetNumberOfThreads(nThreads);
            ComputeTDR.SetTime(uiTime);
            ComputeTDR.SetLoopsInShader(nLoopsInShader);

            if( m_bNonInteractive )
            {
                ComputeTDR.SetNonInteractive();
            }
            ComputeTDR.SetFeatureLevel(m_fl);

            // Now for each loop, we'll reset our data

            WriteToLog(_T(__FUNCTION__ " - Generating workload with LoopsInshader: %d "),nLoopsInShader);

            etwDMAConsumer.Reset();
            etwDMAConsumer.Start();
            etwProfilerConsumer.Reset();
            etwProfilerConsumer.Start();

            // compute for the given time period.
            ComputeTDR.Initialize();
            hr = ComputeTDR.Start();
            if( S_OK != hr )
            {
                WriteToLog(_T(__FUNCTION__ " - Failed Start()."));
                tr = RESULT_FAIL;
                break;
            }
            hr = ComputeTDR.Work();

            ULONG ulWaitForEvents = 3000;  // few second wait
            if( S_OK != hr )
            {
                // we must have TDR'd. Did we expect this or not?
                ulWaitForEvents = 5000; // Wait for at least 5 seconds for system to process TDR and reset driver
                bHitTDR = true;
            }
            LetEventsDrain(&etwProfilerConsumer,ulWaitForEvents,NULL);


            // Start with our QueryCurrentFence results
            tr = FailLongDdiCalls(&etwController, &etwProfilerConsumer);
            if( RESULT_PASS != tr )
            {
                // Had a failure above, no need to continue looping.
                etwDMAConsumer.Stop();
                etwProfilerConsumer.Stop();
                ComputeTDR.Cleanup();
                break;
            }

            // If ResetEngine got called, we experienced a TDR
            UINT uiResetEngineHit=etwProfilerConsumer.GetHitCount(ETW_PROFILER_DdiResetEngine); // 5051
            UINT uiResetFromTimeoutHit=etwProfilerConsumer.GetHitCount(ETW_PROFILER_DdiResetFromTimeout); // 5021
            UINT uiRestartFromTimeoutHit=etwProfilerConsumer.GetHitCount(ETW_PROFILER_DdiRestartFromTimeout); // 5020
            if( uiResetEngineHit > 0 || uiResetFromTimeoutHit > 0 || uiRestartFromTimeoutHit > 0 )
            {
                // we hit a TDR during this pass.
                bHitTDR = true;
            }

            // Now look at our DMA packets

            ullLongestConsecutiveTimens = etwDMAConsumer.LongestConsecutiveTime();
            // Now convert the time value into Microsecond values.
            ullTotalConsecutiveCountms = etwController.TimeStampToMilliseconds(ullLongestConsecutiveTimens);

            // Now it's time to determine if we can get a TDR-able workload. Each time through
            // the loop we're keeping the longest packet from the last time through so we can
            // compare it to the current longest time. basically, the difference should corrispond
            // to the parameters that we changed. If the last time through we had 10 shader
            // instructions, and the longest packet of 1400ms we want to be able to say that
            // if we double the instructions we should get a packet of 2800ms.
#ifdef DBG
            char szBuf[MAX_PATH];
            StringCchPrintf(szBuf,
                            MAX_PATH,
                            "%d: LoopsInshader=%d, LongestTime=%I64dms, LastLongestTime=%I64d\n\r",
                            iLoop,
                            nLoopsInShader,
                            ullTotalConsecutiveCountms,
                            ullLastTimeThroughms);
            OutputDebugStringA( szBuf );
            if( bHitTDR )
            {
                // we issued too much work.
                OutputDebugStringA( "TDR threshold has been reached!\n\r" );
            }
            else
            {
                // we need to issue more work.
                OutputDebugStringA( "Increasing workload!\n\r" );
            }
#endif

            WriteToLog(_T(__FUNCTION__ " - Workload Generated longest packet of: %dms "),ullTotalConsecutiveCountms);

            // If we are Interactive, no DMA packet should ever take more than two seconds
            if( !m_bNonInteractive && 
                (ullTotalConsecutiveCountms > 3000) )
            {
                // log an error that we're a standard interactive application and we're seeing
                // DMA packets taking longer than 2 seconds.
                WriteToLog(_T(__FUNCTION__ " - FAIL: Interactive with DMA packets greater than 2 seconds: %dms"),
                           ullTotalConsecutiveCountms);
                tr = RESULT_FAIL;
            }

            // Have we hit our out desired workload time?
            if( ullTotalConsecutiveCountms > uiPayloadMSUpper )
            {
                // we're greater then the upper bounds, reduce our workload.

                // return to number slightly larger than previous number
                nLoopsInShader = (nLoopsInShader/2) + (nLoopsInShader/4);
            }
            else if( ullTotalConsecutiveCountms < uiPayloadMSLower )
            {
                // we're lower then the lower bounds, increase our workload.
                // grow the number of instructions in shader
                nLoopsInShader *= 2;

                // TODO: Might want to make the workload guess at the outcome of doubling the shader loops.
                // If we can guess that we'll exceed the upper limit, we shouldn't increase the workload
                // as much. This should be monitored.
            }
            else
            {
#ifdef DBG
                char szBuf[MAX_PATH];
                StringCchPrintf(szBuf,MAX_PATH,"Determined workload Length ms=%I64d\n\r",ullTotalConsecutiveCountms);
                WriteToLog(szBuf);
#endif
                // we've generated a packet that is close enough. Let's write back our into
                // and return RESULT_PASS.
                etwDMAConsumer.Stop();
                etwProfilerConsumer.Stop();
                *_puiPayloadMS = nLoopsInShader;
                tr = RESULT_PASS;
                break;
            }

            ullLastTimeThroughms = ullTotalConsecutiveCountms;
            iLoop++;

            etwDMAConsumer.Stop();
            etwProfilerConsumer.Stop();

            ComputeTDR.Cleanup();

            // do we need to wait for work to drain?

        }while(iLoop < 20 && !bHitTDR  && tr == RESULT_PASS);

        if( iLoop == 20 ) 
        {
            PATH_TRAP();
        }

        etwController.StopProcessing();

#ifdef DBG_MAX
        {
            // output statistics about the session.
            char szBuf[MAX_PATH];

            UINT uiHit;
            for(ULONG ulIndex=0;ulIndex<10100;ulIndex++)
            {
                uiHit=etwProfilerConsumer.GetHitCount(ulIndex);

                if( uiHit > 0 )
                {
                    StringCchPrintf(szBuf,MAX_PATH,"%d: Hit %d\n\r",ulIndex,uiHit);
                    OutputDebugString(szBuf);
                }
            }
        }
#endif

        // cleanup.
        if( pxProcSync )
        {
            delete pxProcSync;
        }
    }
    else
    {
        TerminateCrossProsSharing();
    }
    return tr;
}

void CPreemptionTest::LetEventsDrain(CConsumer* _pConsumer, 
                                     ULONG _ulTimeout,
                                     ULONG* _pulActualDelay)
{
    ULONG ulDelay = GetTickCount();
/*
     Name: WaitForEvents
     Param:  Timeout - Timeout in miliseconds to process events. Must be greater than 100 (ms)
     Desc:   Waits for the event to be processed for the current time.
     Return: S_OK(0) - Consumer has processed events with timestamps equal or greater than calling this method
             CONSUMER_ERROR(0x4000) - Failed QueryPerformanceCounter or Timeout < 100ms (default poll)
             CONSUMER_ERROR_TIMEOUT(0x4001) - Reached timeout condition waiting for LastEventTimestamp
             CONSUMER_ERROR_NOEVENTS(0x4002) - LastEventTimestamp did not change while waiting
*/
    ULONG ulEtwReturn = _pConsumer->WaitForEvents(_ulTimeout);

    switch(ulEtwReturn)
    {
    case S_OK:
        // This is the expected path.
        break;
    case CONSUMER_ERROR:
        PATH_TRAP(); 
        // Not much that we can do, call sleep() for the same amount of time 
        // as the WaitForEvents call.
        Sleep(_ulTimeout);
        break;
    case CONSUMER_ERROR_TIMEOUT:
        // Timed out because no event came through to match the current time
        break;
    case CONSUMER_ERROR_NOEVENTS:
        // Timed out because no events came through.
        break;
    default:
        PATH_TRAP();
        break;
    }

    // Convert the times in a delta and write it back to the caller if they want it.
    if( _pulActualDelay )
    {
        *_pulActualDelay += (GetTickCount() - ulDelay);
    }
}


#define LONGEST_ACCEPTABLE_QUERYCURRENTFENCE_TIME_MS 100 
#define LONGEST_ACCEPTABLE_RESETENGINE_TIME_MS  500
#define LONGEST_ACCEPTABLE_QUERYENGINESTATUS_TIME_MS 100
#define LONGEST_MAXIMUM_MS 2000  // no Ddi call should ever take more than 2 seconds

TEST_RESULT CPreemptionTest::FailLongDdiCalls(CController* _petwController,
                                                            CDdiProfilerConsumer* _petwProfilerConsumer)
{
    TEST_RESULT tr = RESULT_PASS;

    if (_petwController->QueryForLostEvents(_petwProfilerConsumer))
    {
        PATH_TRAP();
    }

    // First look at QueryCurrentFence to make sure that's within limits. We will fail this test
    // if -PoliceDDI is true.
    ULONGLONG ullMaxms = _petwProfilerConsumer->GetLongestTimeMS(ETW_PROFILER_DdiQueryCurrentFence);
#ifdef DBG
        char szBuf[MAX_PATH];
        StringCchPrintf(szBuf,
                        MAX_PATH,
                        "Max DdiQueryCurrentFence took %dms\n\r",
                        ullMaxms);
        OutputDebugStringA( szBuf );
#endif

    if( m_bPoliceDDI &&  (ullMaxms > LONGEST_ACCEPTABLE_QUERYCURRENTFENCE_TIME_MS ))
    {
        WriteToLog(_T(__FUNCTION__ " - FAIL DdiQueryCurrentFence took an unreasonable amount of time: %dms"),
                   ullMaxms);
        tr = RESULT_FAIL;
    }

    if( RESULT_PASS == tr )
    {
        ullMaxms = _petwProfilerConsumer->GetLongestTimeMS(ETW_PROFILER_DdiResetEngine);
#ifdef DBG
            char szBuf[MAX_PATH];
            StringCchPrintf(szBuf,
                            MAX_PATH,
                            "Max DdiResetEngine took %dms\n\r",
                            ullMaxms);
            OutputDebugStringA( szBuf );
#endif

        if( m_bPoliceDDI &&  (ullMaxms > LONGEST_ACCEPTABLE_RESETENGINE_TIME_MS ))
        {
            WriteToLog(_T(__FUNCTION__ " - FAIL DdiResetEngine took an unreasonable amount of time: %dms"),
                       ullMaxms);
            tr = RESULT_FAIL;
        }
    }

    if( RESULT_PASS == tr )
    {
        ullMaxms = _petwProfilerConsumer->GetLongestTimeMS(ETW_PROFILER_DdiQueryEngineStatus);
#ifdef DBG
            char szBuf[MAX_PATH];
            StringCchPrintf(szBuf,
                            MAX_PATH,
                            "Max DdiQueryEngineStatus took %dms\n\r",
                            ullMaxms);
            OutputDebugStringA( szBuf );
#endif

        if( m_bPoliceDDI &&  (ullMaxms > LONGEST_ACCEPTABLE_QUERYENGINESTATUS_TIME_MS ))
        {
            WriteToLog(_T(__FUNCTION__ " - FAIL DdiQueryEngineStatus took an unreasonable amount of time: %dms"),
                       ullMaxms);
            tr = RESULT_FAIL;
        }
    }

    if( RESULT_PASS == tr )
    {
        // Now check every other Ddi that we know about.
        for(ULONG ulIndex=ETW_PROFILER_DdiQueryAdapterInfo;
            ulIndex<ETW_PROFILER_DmmInterfaceAcquiredPreferredMonitorSourceMode;
            ulIndex++)
        {
            ullMaxms=_petwProfilerConsumer->GetLongestTimeMS(ulIndex);

            if( m_bPoliceDDI && ( ullMaxms > 0 ) && (ullMaxms > LONGEST_MAXIMUM_MS ))
            {
                WriteToLog(_T(__FUNCTION__ " - FAIL Ddi (%d) took more than 2 seconds, time: %dms"),
                           ulIndex, ullMaxms);
                tr = RESULT_FAIL;
            }
        }
    }


    return tr;
}




//////////////////////////////////////////////////////////////////////////
// CMyTestApp - An instance of a single object derived from CWGFTestFramework
// is required by the framework.
//////////////////////////////////////////////////////////////////////////

void CMyPreemptionTestApp::InitOptionVariables()
{
    // TODO: this is the first point hit in the code.
    CD3D11TestFramework::InitOptionVariables();

    UnRegisterOptionVariable( _T("SrcOnly") );
    m_D3DOptions.SrcOnly = true;

    // Here is where we set all the command line parameters that may change how the 
    // test runs.
    int nPreemption = AddOptionCategory( _T( "Preemption" ) );

    RegisterOptionVariable( "RunType", 
                            "Either: graphics or compute (default graphics)", 
                            &grtOption, 
                            grtOption,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);

    RegisterOptionVariable( "CalculateTDRWorkload", 
                            "If true, a TDR workload will be generated", 
                            &gbCalculateTDRWorkload, 
                            gbCalculateTDRWorkload,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);
    RegisterOptionVariable( "Payload", 
                            "Time, in Milliseconds, of the workload that should be used for testing", 
                            &guiPayloadMS, 
                            guiPayloadMS,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);

    RegisterOptionVariable( "PoliceDDI", 
                            "If true, test will validate that QueryCurrentFence will return within 100ms", 
                            &gbPoliceDDI, 
                            gbPoliceDDI,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);

    RegisterOptionVariable( "TDR", 
                            "If true, a TDR workload will be generated", 
                            &gbTDR, 
                            gbTDR,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);
    RegisterOptionVariable( "ExpectTDR", 
                            "If true, if the system does not TDR, a failure will be generated", 
                            &gbExpectTDR, 
                            gbExpectTDR,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);

    RegisterOptionVariable( "Graphics", 
                            "Min requested Graphics Preemption level supported by HW: none, dma_buffer, primitive, triangle, pixel, shader", 
                            &gGraphicsPreemptionGranularityRequested, 
                            gGraphicsPreemptionGranularityRequested,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);
    RegisterOptionVariable( "Compute", 
                            "Min requested Compute Preemption level supported by HW: none, dma_buffer, dispatch, thread_group, thread, shader", 
                            &gComputePreemptionGranularityRequested, 
                            gComputePreemptionGranularityRequested,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);

    RegisterOptionVariable( "NonInteractive", 
                            "Not an interactive application, allow long DMA packets", 
                            &gbNonInteractive, 
                            gbNonInteractive,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);
    RegisterOptionVariable( "Time", 
                            "Time, in seconds, to perform work", 
                            &guiTime, 
                            guiTime,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);
    RegisterOptionVariable( "Loop", 
                            "Number of times to loop and perform work", 
                            &guiLoop, 
                            guiLoop,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);
    RegisterOptionVariable( "LoopsInShader", 
                            "Compute or Graphics: Number of times for shader to loop on the workload", 
                            &gnLoopsInShader, 
                            gnLoopsInShader, 
                            OPTION_VALUE_CONSTANT, 
                            nPreemption );

    RegisterOptionVariable( "ProcCount", 
                            "Number of processes to sync with: natural number", 
                            &guiProcCount, 
                            guiProcCount,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);
    RegisterOptionVariable( "SyncWith", 
                            "String to use for syncing with SchManager", 
                            &gwszSyncWith, 
                            tstring( _T("") ), 
                            OPTION_VALUE_CONSTANT, 
                            nPreemption );

    // Compute only options
    RegisterOptionVariable( "Threads", 
                            "Compute only: Number of threads to create (1-100)", 
                            &gnThreads, 
                            gnThreads, 
                            OPTION_VALUE_CONSTANT, 
                            nPreemption );

    // Graphics only options
    RegisterOptionVariable( "PixelsPerTriangle", 
                            "Graphics only: Number of pixels for each triangle", 
                            &gPixelsPerTriangle, 
                            gPixelsPerTriangle, 
                            OPTION_VALUE_CONSTANT, 
                            nPreemption );
    RegisterOptionVariable( "TrianglesPerPrimitive", 
                            "Graphics only: Number of Triangles per Primitive", 
                            &gTrianglesPerPrimitive, 
                            gTrianglesPerPrimitive, 
                            OPTION_VALUE_CONSTANT, 
                            nPreemption );
    RegisterOptionVariable( "PrimitivesPerFrame", 
                            "Graphics only: Number of Triangle lists per Frame", 
                            &gPrimitivesPerFrame, 
                            gPrimitivesPerFrame, 
                            OPTION_VALUE_CONSTANT, 
                            nPreemption );

    RegisterOptionVariable( "Quadrant", 
                            "Graphics only: Window location on screen, either 1,2, 3 or 4", 
                            &gQuadrant, 
                            gQuadrant, 
                            OPTION_VALUE_CONSTANT, 
                            nPreemption );

    RegisterOptionVariable( "NullAdapter", 
                            "If false, we will enumerate to find an IDXGIAdapter when calling D3D11CreateDeviceAndSwapChain()",
                            &gbNullAdapter, 
                            gbNullAdapter,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);
    RegisterOptionVariable( "MultiTDR", 
                            "If true, we will loop seven times expecting 6 TDRs and a lockout at 7",
                            &gbMultiTDR, 
                            gbMultiTDR,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);
    RegisterOptionVariable( "MultiTDRSoftware", 
                            "If true, we will render using software after lockout",
                            &gbMultiTDRSoftware, 
                            gbMultiTDRSoftware,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);

    RegisterOptionVariable( "ValidateDelay", 
                            "If true, we will validate that the delay is active",
                            &gbValidateDelay, 
                            gbValidateDelay,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);

    RegisterOptionVariable( "WDDM12", 
                            "If true, we will validate that the correct flags are set",
                            &gbWDDM12, 
                            gbWDDM12,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);

    RegisterOptionVariable( "Stress", 
                            "If true, we will run for [Time] seconds and case TDRs every once in a while",
                            &gbStress, 
                            gbStress,
                            OPTION_VALUE_CONSTANT,
                            nPreemption);


    m_bForceFeatureLevel = false;
    m_bDeferredContextUsed = false;

    // determine preemption capabilities

    gpBlueFish = new CKernelAPIThunks();
    if( !gpBlueFish )
    {
        PATH_TRAP();
        // throw on error here.
    }
    // Now get the capabilities
    HRESULT hr = gpBlueFish->QueryForCapabilities();
}


bool CMyPreemptionTestApp::InitTestGroups()
{
    // TODO: This is where you add the groups - classes that will run. It is called 
    // before the main window is created.

    // Register test class with the group manager.
    if ( !ADD_TEST( _T( "PreemptionTest\\WorkHorse" ), CPreemptionTest ) ) return false;

    return true;
}

bool CMyPreemptionTestApp::Setup()
{

    // TODO: This is called after the main window is created. Notice that the option values
    // are dumpped to the debugger here for each variation.
    CD3D11TestFramework::Setup();

    // BUGBUG - TEST OF OPTIONS REGISTRY
//#define DBG_MAX
#ifdef DBG_MAX
    CD3D11TestFramework::OptionVarIterator it;
    for ( it = GetFramework()->BeginOptionVars(); it != GetFramework()->EndOptionVars(); ++it )
    {
        _DPF( "OPTION NAME: %s\n", GetFramework()->GetOptionVarName( it ).c_str() );
        _DPF( "OPTION DESC: %s\n", GetFramework()->GetOptionVarDesc( it ).c_str() );
        _DPF( "OPTION CMD LINE USAGE: %s\n", GetFramework()->GetOptionVarCmdLineUsage( it ).c_str() );
        _DPF( "OPTION CATEGORY: %s\n", GetFramework()->GetOptionCategoryName( GetFramework()->GetOptionVarCategoryID( it ) ).c_str() );
        _DPF( "OPTION FLAGS: 0x%8.8x\n", GetFramework()->GetOptionVarFlags( it ) );
        tstring strValue;
        GetFramework()->GetOptionValueAsString( it, strValue );
        _DPF( "OPTION VALUE: %s\n", strValue.c_str() );
        _DPF( "\n" );
    }
    // BUGBUG - END OPTIONS TEST
#endif

    return true;
}

CMyPreemptionTestApp  g_Application;

//------------------------------------------------------------------------------


