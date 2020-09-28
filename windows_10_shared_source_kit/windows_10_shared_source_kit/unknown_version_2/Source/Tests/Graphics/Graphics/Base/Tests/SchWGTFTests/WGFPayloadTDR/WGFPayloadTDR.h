/******************************Module*Header*******************************
* Module Name: WGFPayloadTDR.h
*
* D3D11 Graphics and Compute TDR workload test
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#pragma once

#define MAX_RESOURCE_COUNT 4
#define RESOURCE_PATTERN 0x0ffe0ffe


// These are for command line options. They are used like:
//
//  -RunType:group
// 
enum RUN_TYPE
{
    RUN_TYPE_GRAPHICS = 1,
    RUN_TYPE_COMPUTE  = 2,
};

DECLARE_NAMED_VALUES( RUN_TYPE );

BEGIN_NAMED_VALUES( RUN_TYPE )
    NAMED_VALUE( _T( "graphics" ), RUN_TYPE_GRAPHICS )
    NAMED_VALUE( _T( "compute" ),  RUN_TYPE_COMPUTE )

END_NAMED_VALUES( RUN_TYPE )


                       
//------------------------------------------------------------------------------

class CPreemptionTest : public CD3D11Test // : public CTest
{
public:

    CPreemptionTest();

    void InitTestParameters();
    TEST_RESULT Setup();
    TEST_RESULT ExecuteTestCase();

    virtual TEST_RESULT SetupTestCase();
    virtual void CleanupTestCase();

protected:
    TEST_RESULT Run(bool _bGraphics);

    void ClearIndex(UINT Index);

    TEST_RESULT ValidateThroughETW(CDdiProfilerConsumer* _petwProfilerConsumer);
    TEST_RESULT RegistryCheck();
    // When policing QueryCurrentFence, we use this routine.
    TEST_RESULT FailLongDdiCalls(CController* _petwController,
                                 CDdiProfilerConsumer* _petwProfileConsumer);
    bool m_bPoliceDDI;
    void LetEventsDrain(CConsumer* _pConsumer, 
                        ULONG _ulTimeout, 
                        ULONG* _pulActualDelay);

    TEST_RESULT Process(bool _bGraphics,
                        CXProcSyncClient* pxProcSync,
                        STATE_TYPE* _pstRequest,
                        UINT uiActualPayloadMS,
                        bool _bUseSoftwareDevice);

    TEST_RESULT OnePass(Base_TDR* _pBaseTDR,
                        CController* _petwController,
                        CDdiProfilerConsumer* _petwProfilerConsumer,
                        ULONG* _pulIntentionalDelay);

    void LockOutCheck(ULONG _uiLoop);

    ULONGLONG GetAverageDMALengthMS(CController *_petwController,
                                    CDMAStandardTime* _petwDMAConsumer);

    RUN_TYPE m_rtOption; // Either Graphics or Compute

    UINT m_uiPayloadMS; // set if the user wants a DMA buffer of a particular size.
    bool m_bCalculateTDRWorkload; // true to have the test generate a TDR workload on this machine.
    bool m_bTDR; // true to have the test TDR the system.
    TEST_RESULT CalculateGraphicsTDRWorkload(UINT* _puiPayLoad, bool _bEnableSyncWith);
    TEST_RESULT CalculateComputeTDRWorkload(UINT* _puiPayLoad, bool _bEnableSyncWith);
    bool m_bExpectTDR; // If true,  log failure if we do NOT get a TDR. 
                       // If false, log failure if we do     get a TDR.

    bool m_bNonInteractive; // Sets flag on CreateDevice
    bool m_bNullAdapter; // Determines what adapter to use for test
    bool m_bMultiTDR;    // Loop expecting TDRs until we're locked out
    bool m_bMultiTDRSoftware; // When locked out, use software and continue rendering
    bool m_bLockedOut; // Set when we recieve the BlockGpuAccess Event
    bool m_bNoLockOutEvent;
    bool m_bValidateDelay;
    bool m_bWDDM12;
    bool m_bStress;

    UINT m_uiProcCount;   // CrossProcSharing: number of sharing processes
    tstring* m_pSyncWith; // CrossProcSharing: shared memory key (string)
    bool WantCrossProcSharing()
    {
        if( m_pSyncWith->length() > 0 && m_uiProcCount > 1 )
        {
            return true;
        }
        return false;
    }
    void TerminateCrossProsSharing();

    // if this is true, we're running on a software renderer. This would indicate that
    // we should not get TDRs.
    bool m_bIsSoftwareRenderDevice;
    void OutputDeviceCapabilities(bool _bRender);


    UINT m_uiLoop; // Number of times to loop running m_uiTime length.
    UINT m_uiTime; // Total time to run

    D3DKMDT_GRAPHICS_PREEMPTION_GRANULARITY m_GraphicsPreemptionGranularityRequested; // Requested preemption level
    D3DKMDT_COMPUTE_PREEMPTION_GRANULARITY m_ComputePreemptionGranularityRequested;   // Requested preemption level
    bool m_bSupportPerEngineTDR;

    UINT m_nLoopsInShader; // Times to loop within the shader

    // Compute specific values
    UINT m_nThreads;  // How many threads do we want working at the same time.

    //Graphics specific values
    UINT m_PixelsPerTriangle;     // size of triangle (in pixels)
    UINT m_TrianglesPerPrimitive; // size of triangle list
    UINT m_PrimitivesPerFrame;    // number of times to call draw before present

    UINT m_Quadrant; // screen location for window

    UINT m_ulTdrLimitCount; // Used for Multi-TDR loop. The BlockGpuAccess event will come
                            // after this many TDRs default is EXPECTED_TDR_COUNT.

    // Global Items.
    ID3D11Device*           m_pDevice;
    ID3D11DeviceContext*	m_pEffectiveContext;
    IDXGIDevice2*           m_pDXGIDevice2;

    // Resources
    ID3D11Texture2D* 		m_pTex[MAX_RESOURCE_COUNT];
    IDXGIResource*          m_pResources[MAX_RESOURCE_COUNT];
    ID3D11ShaderResourceView* m_pShaderResourceViews[MAX_RESOURCE_COUNT];
    ID3D11RenderTargetView* m_pRenderTargetViews[MAX_RESOURCE_COUNT];
    BOOL                    m_pDiscarded[MAX_RESOURCE_COUNT];
    DXGI_FORMAT				m_DXGIFormat;
    UINT					m_Width;
    UINT					m_Height;
    UINT                    m_SizeInBytes;
    ULONG*					m_pData1;	// data used to fill resources
    ULONG*					m_pData2;	// data used to check resources
    UINT                    m_Count;

    D3D_FEATURE_LEVEL m_fl; // Used to hold desired feature level

    bool m_bDummy; // Used to get called twice.

#define FIFTEEN_MINUTES_MS 60*15*1000 // 15 minutes as seen in milliseconds
    DWORD m_dwTestStartTime; // Used to make sure the test never runs more than 15 minutes
    void TimeLimitStart();
    TEST_RESULT TimeLimit();

private:
};


class CMyPreemptionTestApp : public CD3D11TestFramework //public CWGFTestFramework
{
public:
    CMyPreemptionTestApp(){};
    ~CMyPreemptionTestApp()
    {
        if( gpBlueFish )
        {
            delete gpBlueFish;
        }
    };

    TESTTYPE GetTestType() const { return TESTTYPE_CONF; }

    void InitOptionVariables();
    bool InitTestGroups();
    bool Setup();
};

