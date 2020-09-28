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

// Comment out the following line to use raw buffers instead of structured buffers
#define USE_STRUCTURED_BUFFERS

// If defined, then the hardware/driver must report support for double-precision CS 5.0 
// shaders or the sample fails to run
//#define TEST_DOUBLE
#define MAX_THREADS 100
#define BUF_SIZE 255

//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
void RunComputeShader( ID3D11DeviceContext* pd3dImmediateContext,
                       ID3D11ComputeShader* pComputeShader,
                       UINT nNumViews, ID3D11ShaderResourceView** pShaderResourceViews, 
                       ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
                       ID3D11UnorderedAccessView* pUnorderedAccessView,
                       UINT X, UINT Y, UINT Z );
DWORD WINAPI ComputeThread( LPVOID lpParam );

class Compute_a_TDR : public Base_TDR
{
public:
    Compute_a_TDR();
    ~Compute_a_TDR(){};

    // Setup options
    void SetTDRShaderOn();
    void SetNumberOfThreads(int _nThreads);
    void SetTime(UINT _uiTime);
    void SetLoopsInShader(int _nLoops);
    HRESULT SetNonInteractive();
    HRESULT SetFeatureLevel(D3D_FEATURE_LEVEL _fl);

    HRESULT Initialize();
    HRESULT Start();
    HRESULT Work();
    HRESULT Cleanup();

private:

    HRESULT CreateComputeDevice( ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut, BOOL bForceRef );
    HRESULT CreateComputeShader( LPCWSTR pSrcFile, LPCSTR pFunctionName, 
                                 ID3D11Device* pDevice, ID3D11ComputeShader** ppShaderOut );
    HRESULT CreateStructuredBuffer( ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut );
    HRESULT CreateRawBuffer( ID3D11Device* pDevice, UINT uSize, VOID* pInitData, ID3D11Buffer** ppBufOut );
    HRESULT CreateBufferSRV( ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut );
    HRESULT CreateBufferUAV( ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** pUAVOut );
    HRESULT CreateTextureUAV( ID3D11Device* pDevice, ID3D11Texture2D* pTexture, ID3D11UnorderedAccessView** ppUAVOut );
    HRESULT FindDXSDKShaderFileCch( __in_ecount(cchDest) WCHAR* strDestPath,
                                int cchDest, 
                                __in LPCWSTR strFilename );
    void ErrorHandler(LPTSTR lpszFunction);

    PMYDATA m_pDataArray[MAX_THREADS];
    DWORD   m_dwThreadIdArray[MAX_THREADS];
    HANDLE  m_hThreadArray[MAX_THREADS]; 

    LARGE_INTEGER m_liEndTime;
    LARGE_INTEGER m_liStartTime;
    LARGE_INTEGER m_liTimerFrequency; 
    float m_fTotalTime; // Time we spend waiting for the work to complete.

    // Initialization stuff
    bool m_bTDRShader; // Command Line argument
    int m_nThreads; //=10, 
    UINT m_uiTime; // number of seconds to run sending down workloads.
    int m_nLoopsInShader; // Command Line argument
    UINT m_uiCreateDeviceFlags; // Command Line argument

    D3D_FEATURE_LEVEL m_fl;
};

