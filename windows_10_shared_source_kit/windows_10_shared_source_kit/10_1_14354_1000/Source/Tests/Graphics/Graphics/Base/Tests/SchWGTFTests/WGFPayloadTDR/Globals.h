/******************************Module*Header*******************************
* Module Name: WGFPayloadTDR.h
*
* D3D11 Graphics and Compute TDR workload test
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/

  
// This is used to fake device lockout and TDRs. Basically, on a system that will not
// TDR for us, we'll simulate that the test does the correct thing.
//#define WANT_SIMULATE

#define EXPECTED_TDR_COUNT 5


// For delay functionality, we'll use these constants
#define DELAY_MaximumAllowedPreemptionDelay 900
#define DELAY_ContextSchedulingPenaltyDelay 1000
#define DELAY_EnableContextDelay 1

// A packet is large enough to generate a TDR if it takes more than two seconds down
// on the GPU.
#define TDR_THRESHOLD 2000

// The number of elements in a buffer to be tested
const UINT NUM_ELEMENTS = 65534;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

struct BufType
{
    int i;
    float f;
#ifdef TEST_DOUBLE
    double d;
#endif
};

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct MyData {
    int val1;
    int val2;
    UINT uiTime;
    HRESULT hr;
} MYDATA, *PMYDATA;



HRESULT WINAPI Dynamic_D3D11CreateDevice( IDXGIAdapter* pAdapter,
                                          D3D_DRIVER_TYPE DriverType,
                                          HMODULE Software,
                                          UINT32 Flags,
                                          CONST D3D_FEATURE_LEVEL* pFeatureLevels,
                                          UINT FeatureLevels,
                                          UINT32 SDKVersion,
                                          ID3D11Device** ppDevice,
                                          D3D_FEATURE_LEVEL* pFeatureLevel,
                                          ID3D11DeviceContext** ppImmediateContext );

class Base_TDR
{
public:
    Base_TDR(){};
    ~Base_TDR(){};
public:

    virtual HRESULT Initialize(){return S_OK;};
    virtual HRESULT Start(){return S_OK;};
    virtual HRESULT Work(){return S_OK;};
    virtual HRESULT Cleanup(){return S_OK;};

};




