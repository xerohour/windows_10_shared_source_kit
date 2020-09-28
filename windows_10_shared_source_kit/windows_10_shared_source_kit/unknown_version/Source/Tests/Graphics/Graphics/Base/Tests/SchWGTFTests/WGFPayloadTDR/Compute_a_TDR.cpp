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

#include <d3dcompiler.h>
#include <d3dx11.h>

#include "CS_Shaders.h"
#include "Globals.h"
#include "Compute_a_TDR.h"


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

ID3D11Device*               g_pDevices[MAX_THREADS] = {NULL};
ID3D11DeviceContext*        g_pContexts[MAX_THREADS] = {NULL};
ID3D11ComputeShader*        g_pCSs[MAX_THREADS] = {NULL};

ID3D11Buffer*               g_pBuf0[MAX_THREADS] = {NULL};
ID3D11Buffer*               g_pBuf1[MAX_THREADS] = {NULL};
ID3D11Buffer*               g_pBufResult[MAX_THREADS] = {NULL};

ID3D11ShaderResourceView*   g_pBuf0SRV[MAX_THREADS] = {NULL};
ID3D11ShaderResourceView*   g_pBuf1SRV[MAX_THREADS] = {NULL};
ID3D11UnorderedAccessView*  g_pBufResultUAV[MAX_THREADS] = {NULL};

ID3D11Texture2D*            g_pTex2DOut[MAX_THREADS] = {NULL};
ID3D11UnorderedAccessView*  g_pUAVOut[MAX_THREADS] = {NULL};
ID3D11ComputeShader*        g_pCS2[MAX_THREADS] = {NULL};
ID3D11Buffer*               g_debugbuf[MAX_THREADS] = {NULL};

BufType g_vBuf0[NUM_ELEMENTS];
BufType g_vBuf1[NUM_ELEMENTS];

WCHAR g_Shaderfile[128] = L"";


/*
  We'll have three sections here. Initialization, working and cleanup.
*/
Compute_a_TDR::Compute_a_TDR() :
    m_nThreads(10),
    m_uiTime(10),
    m_bTDRShader(false), // Command Line argument
    m_nLoopsInShader(1),
    m_uiCreateDeviceFlags(0),
    m_fl(D3D_FEATURE_LEVEL_11_0)
{
    ZeroMemory(&m_liStartTime, sizeof(m_liStartTime));
    ZeroMemory(&m_liTimerFrequency, sizeof(m_liTimerFrequency));
    ZeroMemory(&m_liEndTime, sizeof(m_liEndTime));
    m_fTotalTime = 0.0f;

    for( int i=0; i<m_nThreads; i++ )
    {        
        m_pDataArray[i] = NULL;
        m_dwThreadIdArray[i] = NULL;
        m_hThreadArray[i] = NULL;
    }
}
void Compute_a_TDR::SetTDRShaderOn()
{
    m_bTDRShader = true;
}
void Compute_a_TDR::SetNumberOfThreads(int _nThreads)
{
    m_nThreads = _nThreads;
}
void Compute_a_TDR::SetTime(UINT _uiTime)
{
    m_uiTime = _uiTime;
}
void Compute_a_TDR::SetLoopsInShader(int _nLoops)
{
    m_nLoopsInShader = _nLoops;
}

HRESULT Compute_a_TDR::SetNonInteractive()
{
    m_uiCreateDeviceFlags |= D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT; // 0x00000100 is DisableGpuTimeout
    return S_OK;
}

HRESULT Compute_a_TDR::SetFeatureLevel(D3D_FEATURE_LEVEL _fl)
{
    m_fl = _fl;
    return S_OK;
}

// Device creation time here. This should only be called once.
HRESULT Compute_a_TDR::Initialize()
{
    HRESULT hr = S_OK;
    // Would normally set things up when parsing the command line. We will simply set
    // our variables here, once we get here.

    for ( int i = 0; i < NUM_ELEMENTS; ++i ) 
    {
        g_vBuf0[i].i = i;
        g_vBuf0[i].f = (float)i;
#ifdef TEST_DOUBLE
        g_vBuf0[i].d = (double)i;
#endif
        g_vBuf1[i].i = i;
        g_vBuf1[i].f = (float)i;
#ifdef TEST_DOUBLE
        g_vBuf1[i].d = (double)i;
#endif
    }
    
    //printf( "CREATE RESOURCES FOR ALL THE WORKER THREADS RUNNING COMPUTE SHADERS...\n" );
    for( int i=0; i<m_nThreads; i++ )
    {
        hr = CreateComputeDevice( &g_pDevices[i], &g_pContexts[i], FALSE );
        if ( FAILED( hr ) )
        {
            return hr;
        }
        
        CreateComputeShader( g_Shaderfile, "CSMain", g_pDevices[i], &g_pCSs[i] );

#ifdef USE_STRUCTURED_BUFFERS
        CreateStructuredBuffer( g_pDevices[i], sizeof(BufType), NUM_ELEMENTS, &g_vBuf0[0], &g_pBuf0[i] );
        CreateStructuredBuffer( g_pDevices[i], sizeof(BufType), NUM_ELEMENTS, &g_vBuf1[0], &g_pBuf1[i] );
        CreateStructuredBuffer( g_pDevices[i], sizeof(BufType), NUM_ELEMENTS, NULL, &g_pBufResult[i] );
#else
        CreateRawBuffer( g_pDevice, NUM_ELEMENTS * sizeof(BufType), &g_vBuf0[0], &g_pBuf0 );
        CreateRawBuffer( g_pDevice, NUM_ELEMENTS * sizeof(BufType), &g_vBuf1[0], &g_pBuf1 );
        CreateRawBuffer( g_pDevice, NUM_ELEMENTS * sizeof(BufType), NULL, &g_pBufResult );
#endif

        CreateBufferSRV( g_pDevices[i], g_pBuf0[i], &g_pBuf0SRV[i] );
        CreateBufferSRV( g_pDevices[i], g_pBuf1[i], &g_pBuf1SRV[i] );
        CreateBufferUAV( g_pDevices[i], g_pBufResult[i], &g_pBufResultUAV[i] );
    }

    for( int i=0; i<m_nThreads; i++ )
    {        
        m_pDataArray[i] = NULL;
    }
    return hr;
}

HRESULT Compute_a_TDR::Start()
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    g_pBufResult[0]->GetDesc( &desc );
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;

    // Create m_nThreads or MAX_THREADS worker threads.
    for( int i=0; i<m_nThreads; i++ )
    {        
        if ( FAILED(g_pDevices[i]->CreateBuffer(&desc, NULL, &g_debugbuf[i])) )
        {
            PATH_TRAP();
            throw std::bad_alloc();
        }

        // Allocate memory for thread data.

        if( m_pDataArray[i] == NULL )
        {
            m_pDataArray[i] = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                    sizeof(MYDATA));
        }

        if( m_pDataArray[i] == NULL )
        {
           // If the array allocation fails, the system is out of memory
           // so there is no point in trying to print an error message.
           // Just terminate execution.
            ExitProcess(2);
        }

        // Generate unique data for each thread to work with.

        m_pDataArray[i]->val1 = i;
        m_pDataArray[i]->val2 = i+100;
        m_pDataArray[i]->uiTime = m_uiTime;
        m_pDataArray[i]->hr = S_OK;

        // Create the thread to begin execution on its own.

        m_hThreadArray[i] = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            ComputeThread,          // thread function name
            m_pDataArray[i],        // argument to thread function 
            0,                      // use default creation flags 
            &m_dwThreadIdArray[i]); // returns the thread identifier 


        // Check the return value for success.
        // If CreateThread fails, terminate execution. 
        // This will automatically clean up threads and memory. 

        if (m_hThreadArray[i] == NULL) 
        {
           ErrorHandler(TEXT("CreateThread"));
           PATH_TRAP();
           throw std::bad_alloc();
        }
    } // End of main thread creation loop.
    return hr;
}

HRESULT Compute_a_TDR::Work()
{
    HRESULT hr = S_OK;
    // Get Timer Frequency
    QueryPerformanceFrequency( &m_liTimerFrequency );
    // Start Time.. 
    QueryPerformanceCounter( &m_liStartTime );

    // When we enter the work routine, all the work scheduled in the Initialize routine (above)
    // will be running. We just want for it to complete here.

    // Wait until all threads have terminated.
    WaitForMultipleObjects(m_nThreads, m_hThreadArray, TRUE, INFINITE);

    // Get Finish time...
    QueryPerformanceCounter( &m_liEndTime );    
    m_fTotalTime = 1000*((float)(m_liEndTime.QuadPart - m_liStartTime.QuadPart) / (float)m_liTimerFrequency.QuadPart);
    printf( "\n\tTotal time for Compute by all threads: %f ms...\n", m_fTotalTime  );

    // Now look to see if any of the threads had an error.
    for( int i=0; i<m_nThreads; i++ )
    {
        if( m_pDataArray[i]->hr != S_OK )
        {
            // This one reported an error. We're only going to return the first error
            // that we find. 
            if( S_OK == hr )
            {
                hr = m_pDataArray[i]->hr;
            }
        }
    }

    return hr;
}

HRESULT Compute_a_TDR::Cleanup()
{
    HRESULT hr = S_OK;
    // Close all thread handles and free memory allocations.
    for(int i=0; i<m_nThreads; i++)
    {
        CloseHandle(m_hThreadArray[i]);
        if(m_pDataArray[i] != NULL)
        {
            HeapFree(GetProcessHeap(), 0, m_pDataArray[i]);
            m_pDataArray[i] = NULL;    // Ensure address is not reused.
        }
    }

    for( int i=0; i<m_nThreads; i++ )
    {
        SAFE_RELEASE( g_pBuf0SRV[i] );
        SAFE_RELEASE( g_pBuf1SRV[i] );
        SAFE_RELEASE( g_pBufResultUAV[i] );
        SAFE_RELEASE( g_pBuf0[i] );
        SAFE_RELEASE( g_pBuf1[i] );
        SAFE_RELEASE( g_pBufResult[i] );
        SAFE_RELEASE( g_pTex2DOut[i] );
        SAFE_RELEASE( g_pUAVOut[i] );
        SAFE_RELEASE( g_pCSs[i] );
        SAFE_RELEASE( g_pContexts[i] );
        SAFE_RELEASE( g_pDevices[i] );
        SAFE_RELEASE( g_pCS2[i] );
        SAFE_RELEASE( g_debugbuf[i] );
    }
    return hr;
}



//--------------------------------------------------------------------------------------
// Worker Thread , this will loop on compute shader to keep Gpu busy.
//--------------------------------------------------------------------------------------
DWORD WINAPI ComputeThread( LPVOID lpParam ) 
{ 
    HANDLE hStdout;
    PMYDATA pDataArray;
    HRESULT hr=S_OK;

    TCHAR msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

    // Make sure there is a console to receive output results. 

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if( hStdout == INVALID_HANDLE_VALUE )
        return 1;

    // Cast the parameter to the correct data type.
    // The pointer is known to be valid because 
	// it was checked for NULL before the thread was created.
 
    pDataArray = (PMYDATA)lpParam;

    // Print the parameter values using thread-safe functions.
    int i = pDataArray->val1;
    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Parameters = %d, %d\n"), 
        pDataArray->val1, pDataArray->val2); 
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    //WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);

    /*------------------------------------------------------------------*/
    
    ID3D11ShaderResourceView* aRViews[2] = { g_pBuf0SRV[i], g_pBuf1SRV[i] };    

    // Looping on the compute shader to keep gpu busy We will run for the user specified period
    // of time.
    time_t StartTime = time(NULL);
    time_t Diff;

    do
    {
        // Perform work here.
        //printf( "Running Compute Shader Thread %d...", i );
        RunComputeShader( g_pContexts[i], g_pCSs[i], 2, aRViews, NULL, NULL, 0, g_pBufResultUAV[i], NUM_ELEMENTS, 1, 1 );
        //printf( "done\n" );

        // Accessing the map methods makes each DMA packet wait for the previous one
        // to complete. If we do not wait, everything gets scheduled all at one time
        // and then the queue drains. The real difference is that if we schedule everything
        // all at once, the threads that perform this work immediately exit and our
        // work() routine is done - even through there is work in the queue waiting to 
        // finish. Thus, these calls prevent the Work() method from finishing before
        // we're done.
        g_pContexts[i]->CopyResource( g_debugbuf[i], g_pBufResult[i] );
        D3D11_MAPPED_SUBRESOURCE MappedResource; 
        BufType *p;
        hr = g_pContexts[i]->Map( g_debugbuf[i], 0, D3D11_MAP_READ, 0, &MappedResource );
        if( S_OK == hr )
        {
            p = (BufType*)MappedResource.pData;
            g_pContexts[i]->Unmap( g_debugbuf[i], 0 );
        }
        else
        {
            // For some reason, we can not map this resource. Could be that we've TDR'd. If so,
            // we're going to return this error code to the caller. so they can evaluate the
            // situation.

            if( S_OK == pDataArray->hr )  // E_OUTOFMEMORY is returned when a TDR occurs
            {
                pDataArray->hr = hr;
            }
        }

        // Have we run long enough?
        time_t CurrentTime = time(NULL);
        Diff = CurrentTime - StartTime;

    } while(((UINT)Diff < pDataArray->uiTime) && (S_OK == hr));

    return 0; 
}

//--------------------------------------------------------------------------------------
// This is equivalent to D3D11CreateDevice, except it dynamically loads d3d11.dll,
// this gives us a graceful way to message the user on systems with no d3d11 installed
//--------------------------------------------------------------------------------------
HRESULT WINAPI Dynamic_D3D11CreateDevice( IDXGIAdapter* pAdapter,
                                          D3D_DRIVER_TYPE DriverType,
                                          HMODULE Software,
                                          UINT32 Flags,
                                          CONST D3D_FEATURE_LEVEL* pFeatureLevels,
                                          UINT FeatureLevels,
                                          UINT32 SDKVersion,
                                          ID3D11Device** ppDevice,
                                          D3D_FEATURE_LEVEL* pFeatureLevel,
                                          ID3D11DeviceContext** ppImmediateContext )
{
    typedef HRESULT (WINAPI * LPD3D11CREATEDEVICE)( IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT32, CONST D3D_FEATURE_LEVEL*, UINT, UINT32, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext** );
    static LPD3D11CREATEDEVICE  s_DynamicD3D11CreateDevice = NULL;
    
    if ( s_DynamicD3D11CreateDevice == NULL )
    {            
        HMODULE hModD3D11 = LoadLibrary( "d3d11.dll" );

        if ( hModD3D11 == NULL )
        {
            return E_FAIL;
        }

        s_DynamicD3D11CreateDevice = ( LPD3D11CREATEDEVICE )GetProcAddress( hModD3D11, "D3D11CreateDevice" );           
    }

    return s_DynamicD3D11CreateDevice( pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels,
                                       SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext );
}

//--------------------------------------------------------------------------------------
// Create the D3D device and device context suitable for running Compute Shaders(CS)
//--------------------------------------------------------------------------------------
HRESULT Compute_a_TDR::CreateComputeDevice( ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut, BOOL bForceRef )
{    
    *ppDeviceOut = NULL;
    *ppContextOut = NULL;
    
    HRESULT hr = S_OK;

    D3D_FEATURE_LEVEL flOut;
    // Must have feature level 11 for compute functionality.
    static const D3D_FEATURE_LEVEL flvl[] = { D3D_FEATURE_LEVEL_11_0 };
    
    BOOL bNeedRefDevice = FALSE;
    if ( !bForceRef )
    {
        hr = Dynamic_D3D11CreateDevice( NULL,                        // Use default graphics card
                                        D3D_DRIVER_TYPE_HARDWARE,    // Try to create a hardware accelerated device
                                        NULL,                        // Do not use external software rasterizer module
                                        m_uiCreateDeviceFlags,       // Device creation flags
                                        &m_fl,
                                        1,
                                        D3D11_SDK_VERSION,           // SDK version
                                        ppDeviceOut,                 // Device out
                                        &flOut,                      // Actual feature level created
                                        ppContextOut );              // Context out
        
        if ( SUCCEEDED( hr ) )
        {
            // A hardware accelerated device has been created, so check for Compute Shader support

            // If we have a device >= D3D_FEATURE_LEVEL_11_0 created, full CS5.0 support is guaranteed, no need for further checks
            if ( flOut < D3D_FEATURE_LEVEL_11_0 )            
            {
#ifdef TEST_DOUBLE
                bNeedRefDevice = TRUE;
                printf( "No hardware Compute Shader 5.0 capable device found (required for doubles), trying to create ref device.\n" );
#else
                // Otherwise, we need further check whether this device support CS4.x (Compute on 10)
                D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts;
                (*ppDeviceOut)->CheckFeatureSupport( D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts) );
                if ( !hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
                {
                    bNeedRefDevice = TRUE;
                    printf( "No hardware Compute Shader capable device found, trying to create ref device.\n" );
                }
#endif
            }

#ifdef TEST_DOUBLE
            else
            {
                // Double-precision support is an optional feature of CS 5.0
                D3D11_FEATURE_DATA_DOUBLES hwopts;
                (*ppDeviceOut)->CheckFeatureSupport( D3D11_FEATURE_DOUBLES, &hwopts, sizeof(hwopts) );
                if ( !hwopts.DoublePrecisionFloatShaderOps )
                {
                    bNeedRefDevice = TRUE;
                    printf( "No hardware double-precision capable device found, trying to create ref device.\n" );
                }
            }
#endif
        }
    }
    
    if ( bForceRef || FAILED(hr) || bNeedRefDevice )
    {
        // Either because of failure on creating a hardware device or hardware lacking CS capability, we create a ref device here

        SAFE_RELEASE( (*ppDeviceOut) );
        SAFE_RELEASE( (*ppContextOut) );
        
        hr = Dynamic_D3D11CreateDevice( NULL,                        // Use default graphics card
                                        D3D_DRIVER_TYPE_REFERENCE,   // Try to create a hardware accelerated device
                                        NULL,                        // Do not use external software rasterizer module
                                        m_uiCreateDeviceFlags,       // Device creation flags
                                        flvl,
                                        sizeof(flvl) / sizeof(D3D_FEATURE_LEVEL),
                                        D3D11_SDK_VERSION,           // SDK version
                                        ppDeviceOut,                 // Device out
                                        &flOut,                      // Actual feature level created
                                        ppContextOut );              // Context out
        if ( FAILED(hr) )
        {
            printf( "Reference rasterizer device create failure\n" );
            return hr;
        }
    }

    return hr;
}

//--------------------------------------------------------------------------------------
// Compile and create the CS
//--------------------------------------------------------------------------------------
HRESULT Compute_a_TDR::CreateComputeShader( LPCWSTR pSrcFile, LPCSTR pFunctionName, 
                             ID3D11Device* pDevice, ID3D11ComputeShader** ppShaderOut )
{
    HRESULT hr=S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

    const D3D_SHADER_MACRO defines[] = 
    {
#ifdef USE_STRUCTURED_BUFFERS
        "USE_STRUCTURED_BUFFERS", "1",
#endif

#ifdef TEST_DOUBLE
        "TEST_DOUBLE", "1",
#endif
        NULL, NULL
    };

    // We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
    LPCSTR pProfile = ( pDevice->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ) ? "cs_5_0" : "cs_4_0";

    ID3DBlob* pErrorBlob = NULL;
    ID3DBlob* pBlob = NULL;

    if(m_bTDRShader)
    {
        // Dynamically adjust the number of loops in the shader based on user input.
        size_t tSize = strlen(SHADER_TDR)*2;
        size_t tActualSize=0;

        char* pShaderString = (char*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, tSize);
        if( NULL == pShaderString )
        {
            PATH_TRAP();
            throw std::bad_alloc();
        }
                       
        // now read our string and add in our loop count
        hr = StringCchPrintf(pShaderString,tSize,SHADER_TDR,m_nLoopsInShader);
        if( S_OK != hr )
        {
            PATH_TRAP();
            throw std::bad_alloc();
        }
        hr = StringCchLength(pShaderString,tSize,&tActualSize);
        if( S_OK != hr )
        {
            PATH_TRAP();
            throw std::bad_alloc();
        }

        // Now create our shader

        hr = D3DX11CompileFromMemory( pShaderString, tActualSize, NULL, 
            defines, NULL, pFunctionName, pProfile, dwShaderFlags, NULL, NULL, &pBlob, &pErrorBlob, NULL );
    }
    else
    {
        PATH_TRAP();
        hr = D3DX11CompileFromMemory( CSCONST_SHADER, strlen(CSCONST_SHADER), NULL, 
            defines, NULL, pFunctionName, pProfile, dwShaderFlags, NULL, NULL, &pBlob, &pErrorBlob, NULL );
    }

    if ( FAILED(hr) )
    {
        if ( pErrorBlob )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );

        SAFE_RELEASE( pErrorBlob );
        SAFE_RELEASE( pBlob );    

        return hr;
    }    

    hr = pDevice->CreateComputeShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppShaderOut );

    SAFE_RELEASE( pErrorBlob );
    SAFE_RELEASE( pBlob );

    return hr;
}

//--------------------------------------------------------------------------------------
// Create Structured Buffer
//--------------------------------------------------------------------------------------
HRESULT Compute_a_TDR::CreateStructuredBuffer( ID3D11Device* pDevice, UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut )
{
    *ppBufOut = NULL;

    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    desc.ByteWidth = uElementSize * uCount;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = uElementSize;

    if ( pInitData )
    {
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = pInitData;
        return pDevice->CreateBuffer( &desc, &InitData, ppBufOut );
    } else
        return pDevice->CreateBuffer( &desc, NULL, ppBufOut );
}

//--------------------------------------------------------------------------------------
// Create Raw Buffer
//--------------------------------------------------------------------------------------
HRESULT Compute_a_TDR::CreateRawBuffer( ID3D11Device* pDevice, UINT uSize, VOID* pInitData, ID3D11Buffer** ppBufOut )
{
    *ppBufOut = NULL;

    D3D11_BUFFER_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER | D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = uSize;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

    if ( pInitData )
    {
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = pInitData;
        return pDevice->CreateBuffer( &desc, &InitData, ppBufOut );
    } else
        return pDevice->CreateBuffer( &desc, NULL, ppBufOut );
}
//--------------------------------------------------------------------------------------
// Create Shader Resource View for Structured or Raw Buffers
//--------------------------------------------------------------------------------------
HRESULT Compute_a_TDR::CreateBufferSRV( ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut )
{
    D3D11_BUFFER_DESC descBuf;
    ZeroMemory( &descBuf, sizeof(descBuf) );
    pBuffer->GetDesc( &descBuf );

    D3D11_SHADER_RESOURCE_VIEW_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    desc.BufferEx.FirstElement = 0;

    if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
    {
        // This is a Raw Buffer

        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
        desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
    } else
    if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
    {
        // This is a Structured Buffer

        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
    } else
    {
        return E_INVALIDARG;
    }

    return pDevice->CreateShaderResourceView( pBuffer, &desc, ppSRVOut );
}

//--------------------------------------------------------------------------------------
// Create Unordered Access View for Structured or Raw Buffers
//-------------------------------------------------------------------------------------- 
HRESULT Compute_a_TDR::CreateBufferUAV( ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut )
{
    D3D11_BUFFER_DESC descBuf;
    ZeroMemory( &descBuf, sizeof(descBuf) );
    pBuffer->GetDesc( &descBuf );
        
    D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
    ZeroMemory( &desc, sizeof(desc) );
    desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    desc.Buffer.FirstElement = 0;

    if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
    {
        // This is a Raw Buffer

        desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
        desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        desc.Buffer.NumElements = descBuf.ByteWidth / 4; 
    } else
    if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
    {
        // This is a Structured Buffer

        desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
        desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride; 
    } else
    {
        return E_INVALIDARG;
    }
    
    return pDevice->CreateUnorderedAccessView( pBuffer, &desc, ppUAVOut );
}


//--------------------------------------------------------------------------------------
// Run CS
//-------------------------------------------------------------------------------------- 
void RunComputeShader(ID3D11DeviceContext* pd3dImmediateContext,
                      ID3D11ComputeShader* pComputeShader,
                      UINT nNumViews, 
                      ID3D11ShaderResourceView** pShaderResourceViews,
                      ID3D11Buffer* pCBCS, 
                      void* pCSData, 
                      DWORD dwNumDataBytes,
                      ID3D11UnorderedAccessView* pUnorderedAccessView,
                      UINT X, 
                      UINT Y, 
                      UINT Z )
{
    pd3dImmediateContext->CSSetShader( pComputeShader, NULL, 0 );
    pd3dImmediateContext->CSSetShaderResources( 0, nNumViews, pShaderResourceViews );
    pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, &pUnorderedAccessView, NULL );
    if ( pCBCS )
    {
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        pd3dImmediateContext->Map( pCBCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource );
        memcpy( MappedResource.pData, pCSData, dwNumDataBytes );
        pd3dImmediateContext->Unmap( pCBCS, 0 );
        ID3D11Buffer* ppCB[1] = { pCBCS };
        pd3dImmediateContext->CSSetConstantBuffers( 0, 1, ppCB );
    }

    pd3dImmediateContext->Dispatch( X, Y, Z );

    pd3dImmediateContext->CSSetShader( NULL, NULL, 0 );

    ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
    pd3dImmediateContext->CSSetUnorderedAccessViews( 0, 1, ppUAViewNULL, NULL );

    ID3D11ShaderResourceView* ppSRVNULL[2] = { NULL, NULL };
    pd3dImmediateContext->CSSetShaderResources( 0, 2, ppSRVNULL );

    ID3D11Buffer* ppCBNULL[1] = { NULL };
    pd3dImmediateContext->CSSetConstantBuffers( 0, 1, ppCBNULL );
}

//--------------------------------------------------------------------------------------
// ErrorHandler routine to print failure messages
//-------------------------------------------------------------------------------------- 
void Compute_a_TDR::ErrorHandler(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s\n\r"), 
        lpszFunction, dw, lpMsgBuf); 
    OutputDebugString( (char*)lpDisplayBuf );

	// Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}


