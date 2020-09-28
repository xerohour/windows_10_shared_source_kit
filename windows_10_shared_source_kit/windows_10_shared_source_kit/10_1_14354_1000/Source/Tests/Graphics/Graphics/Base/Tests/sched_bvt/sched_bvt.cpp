//-----------------------------------------------------------------------------
// File: sched_bvt.cpp
// Desc: DirectX window application created by the DirectX AppWizard
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT
//#define _WIN32_WINNT 0x0600

#ifndef D3DX_TEST_ONLY
	#define D3DX_TEST_ONLY
#endif

#define STRSAFE_NO_DEPRECATE
#include <etwConsumersRequire.h>
#include <provider.h>

#include <lddmcoreeventdefs.h>
#include <d3dkmthk.h>
#include <d3dkmthk_p.h>
#include <d3dukmdt.h>
#include <dmmdiag.h>
#include <dxetwevt.hxx>

#include <DxgKernelConsumer.h>
#include <DxgKernelLegConsumer.h>
#include <Controller.h>

#include "d3d9.h"
#include "d3dx9.h"
#include <dxerr9.h>
#include "time.h"
#include "testcomponent.h"
#include "ComPtr.h"
#include <math.h>

#include <etwLibEvents.h>
#include <CrossProcSharing.h>
#include <Quadrant.h>

#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "resource.h"
#include "sched_bvt.h"




//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_pWTTLog = NULL;
    m_hWTTTestCase = NULL;

    m_nWarmUpFrames = 500;
    m_nTestFrames = 1000;
    m_nAppInstances = 4;
    m_nAppInstance = (UINT)-1;

    m_eMode = INITIALIZING;
    m_hInstanceMutex = NULL;
    m_hReadyEvent = NULL;
    m_hTestEvent = NULL;
    m_hReadyToProcessEvent = NULL;
    m_hProcessEvent = NULL;

    m_nAppMonitor = D3DADAPTER_DEFAULT;

    m_nWarmUpFrames_count = 0;
    m_nTestFrames_count = 0;
    m_nAppInstance_count = 1;
    m_bPrimaryApp = false;
    m_fKernelTime = 0.f;

    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 500;
    m_strWindowTitle            = TEXT( "sched_bvt" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;
    m_strKernelTime[0] = 0;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_pD3DXMesh                 = NULL;

    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;

    m_pEffect = NULL;
    m_hProj = NULL;
    m_hWorld = NULL;
    m_hView = NULL;
    m_hTechnique = NULL;
    m_hTechBlur = NULL;
    m_hTechSkybox = NULL;
    m_hRTTexture = NULL;
    m_hTapOffset0 = NULL;
    m_hTapOffset1 = NULL;
    m_hTapOffset2 = NULL;
    m_hTapOffset3 = NULL;

    m_pRTTexture = NULL;
    m_pBlurTexture = NULL;
    m_pBlurSurface = NULL;
    m_pRTSurface = NULL;
    m_pSkyTexture = NULL;
    m_pBackbuffer = NULL;
    m_pMeshDiffTexture = NULL;
    m_pMeshAlphaTexture = NULL;

    m_pSkyboxMesh = NULL;

    m_bSyncWith=false;

    ParseCommandLine();

    CmdInitialize();

    if(m_bPrimaryApp)
    {
        CreateWTTLog();
    }
}




//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: Application destructor.  Paired with CMyD3DApplication()
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
    if( m_hInstanceMutex )
        CloseHandle( m_hInstanceMutex );
    if( m_hTestEvent )
        CloseHandle( m_hTestEvent );
    if( m_hReadyEvent )
        CloseHandle( m_hReadyEvent );
    if( m_hReadyToProcessEvent )
        CloseHandle( m_hReadyToProcessEvent );
    if( m_hProcessEvent )
        CloseHandle( m_hProcessEvent );

    if(m_bPrimaryApp)
    {
        DestroyWTTLog();
    }
}


//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // Drawing loading status message until app finishes loading
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}









//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
    bCapsAcceptable = TRUE;

    if(pCaps->AdapterOrdinal != m_nAppMonitor)
        bCapsAcceptable = FALSE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateOffScreenPlainSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    DEVMODE dm;
    memset( &dm, 0, sizeof( dm ) );
    dm.dmSize = sizeof( dm );
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &dm );

    int x = 0;
    int y = 0;
    int slot = m_nAppInstance%4;
    int offset = m_nAppInstance-slot;

    x += (slot&1) * (dm.dmPelsWidth>>1);
    x += 10 * offset;
    y += (slot&2) * (dm.dmPelsHeight>>2);
    y += 10 * offset;

    //get the monitor RECT that this test is running on
    //and offset x and y accordingly
    HMONITOR hMon = m_pD3D->GetAdapterMonitor(m_d3dCaps.AdapterOrdinal);
    if(NULL != hMon)
    {
        MONITORINFO Info;
        memset(&Info, 0, sizeof(Info));
        Info.cbSize = sizeof(Info);

        ::GetMonitorInfo(hMon, &Info);

        x += Info.rcMonitor.left;
        y += Info.rcMonitor.top;
    }


    MoveWindow( m_hWnd, x, y, m_dwCreationWidth, m_dwCreationHeight, TRUE);

    // TODO: create device objects
    HRESULT hr;

    // Init the font
    hr = m_pFont->InitDeviceObjects( m_pd3dDevice );
    if( FAILED( hr ) )
        return DXTRACE_ERR( "m_pFont->InitDeviceObjects", hr );
    
    LPD3DXBUFFER bufferErrors = NULL;
#ifdef RESOURCEFROMRC
    hr = D3DXCreateEffectFromResource(  m_pd3dDevice,
                                        NULL,
                                        _T("sched_bvt.fx"),
                                        NULL, NULL, 0, NULL,
                                        &m_pEffect,
                                        &bufferErrors );
#else
    hr = D3DXCreateEffectFromFile( m_pd3dDevice,
                                    _T("sched_bvt.fx"),
                                    NULL, NULL, 0, NULL,
                                    &m_pEffect,
                                    &bufferErrors );
#endif

    if( bufferErrors )
    {
        DXTRACE( (LPTSTR) bufferErrors->GetBufferPointer() );
    }

    SAFE_RELEASE( bufferErrors );

    if( FAILED( hr ) )
        return hr;


#ifdef RESOURCEFROMRC
    // Create a flying saucer mesh using D3DX
    hr = D3DXLoadMeshFromXResource( NULL,
                            "saucer02.x", MAKEINTRESOURCEA(10),
                            D3DXMESH_MANAGED,
                            m_pd3dDevice,
                            NULL, NULL, NULL, NULL,
                            &m_pD3DXMesh );
                            
    if( FAILED( hr ) )
        return DXTRACE_ERR( "D3DXLoadMeshFromXResource", hr );

    // Create a skybox mesh using D3DX
    hr = D3DXLoadMeshFromXResource( NULL,
                            "skybox2.x", MAKEINTRESOURCEA(10),
                            D3DXMESH_MANAGED,
                            m_pd3dDevice,
                            NULL, NULL, NULL, NULL,
                            &m_pSkyboxMesh );

    
    if( FAILED( hr ) )
        return DXTRACE_ERR( "D3DXLoadMeshFromXResource", hr );
#else
    // Create a flying saucer mesh using D3DX
    hr = D3DXLoadMeshFromX( _T("saucer02.x"),
                            D3DXMESH_MANAGED,
                            m_pd3dDevice,
                            NULL, NULL, NULL, NULL,
                            &m_pD3DXMesh );
                            
    if( FAILED( hr ) )
        return DXTRACE_ERR( "D3DXCreateMeshFromX", hr );

    // Create a skybox mesh using D3DX
    hr = D3DXLoadMeshFromX( _T("skybox2.x"),
                            D3DXMESH_MANAGED,
                            m_pd3dDevice,
                            NULL, NULL, NULL, NULL,
                            &m_pSkyboxMesh );

    
    if( FAILED( hr ) )
        return DXTRACE_ERR( "D3DXCreateMeshFromX", hr );
#endif

    m_hWorld = m_pEffect->GetParameterBySemantic( NULL, "WORLD" );
    m_hView  = m_pEffect->GetParameterBySemantic( NULL, "VIEW" );
    m_hProj  = m_pEffect->GetParameterBySemantic( NULL, "PROJECTION" );
    if( NULL == ( m_hTechnique = m_pEffect->GetTechniqueByName( "TVertexAndPixelShader" ) ) )
        return DXTRACE_ERR( "GetTechniqueByName", E_FAIL );
    if( NULL == ( m_hTechBlur = m_pEffect->GetTechniqueByName( "Blur" ) ) )
        return DXTRACE_ERR( "GetTechniqueByName", E_FAIL );
    if( NULL == ( m_hTechSkybox = m_pEffect->GetTechniqueByName( "Skybox" ) ) )
        return DXTRACE_ERR( "GetTechniqueByName", E_FAIL );

    if( NULL == ( m_hRTTexture = m_pEffect->GetParameterByName( NULL, "rt_tex" ) ) )
        return DXTRACE_ERR( "GetParameterByName", E_FAIL );

    if( NULL == ( m_hTapOffset0 = m_pEffect->GetParameterByName( NULL, "TapOffset0" ) ) )
        return DXTRACE_ERR( "GetParameterByName", E_FAIL );
    if( NULL == ( m_hTapOffset1 = m_pEffect->GetParameterByName( NULL, "TapOffset1" ) ) )
        return DXTRACE_ERR( "GetParameterByName", E_FAIL );
    if( NULL == ( m_hTapOffset2 = m_pEffect->GetParameterByName( NULL, "TapOffset2" ) ) )
        return DXTRACE_ERR( "GetParameterByName", E_FAIL );
    if( NULL == ( m_hTapOffset3 = m_pEffect->GetParameterByName( NULL, "TapOffset3" ) ) )
        return DXTRACE_ERR( "GetParameterByName", E_FAIL );

    if( FAILED( hr = CreateSkyBox( &m_pSkyTexture ) ) )
        return hr;

    D3DXHANDLE hSkybox;
    if( NULL == ( hSkybox = m_pEffect->GetParameterByName( NULL, "skybox_tex" ) ) )
        return DXTRACE_ERR( "GetParameterByName", E_FAIL );

    m_pEffect->SetTexture( hSkybox, m_pSkyTexture );

#ifdef RESOURCEFROMRC
    if( FAILED( hr = D3DXCreateTextureFromResource( m_pd3dDevice,
                                                    NULL,
                                                    _T("saucer_color.jpg"),
                                                    &m_pMeshDiffTexture ) ) )
        return DXTRACE_ERR( "D3DXCreateTextureFromResource", E_FAIL );
#else
    if( FAILED( hr = D3DXCreateTextureFromFile( m_pd3dDevice,
                                                _T("saucer_color.jpg"),
                                                &m_pMeshDiffTexture ) ) )
        return DXTRACE_ERR( "D3DXCreateTextureFromFile", E_FAIL );
#endif
                                                
    D3DXHANDLE hTexture0;
    if( NULL == ( hTexture0 = m_pEffect->GetParameterByName( NULL, "Texture0" ) ) )
        return DXTRACE_ERR( "GetParameterByName", E_FAIL );

    m_pEffect->SetTexture( hTexture0, m_pMeshDiffTexture );

#ifdef RESOURCEFROMRC
    if( FAILED( hr = D3DXCreateTextureFromResource( m_pd3dDevice,
                                                    NULL,
                                                    _T("saucer_light.jpg"),
                                                    &m_pMeshAlphaTexture ) ) )
        return DXTRACE_ERR( "D3DXCreateTextureFromResource", E_FAIL );
#else
    if( FAILED( hr = D3DXCreateTextureFromFile( m_pd3dDevice,
                                                _T("saucer_light.jpg"),
                                                &m_pMeshAlphaTexture ) ) )
        return DXTRACE_ERR( "D3DXCreateTextureFromFile", E_FAIL );
#endif                                                
    D3DXHANDLE hTexture1;
    if( NULL == ( hTexture1 = m_pEffect->GetParameterByName( NULL, "Texture1" ) ) )
        return DXTRACE_ERR( "GetParameterByName", E_FAIL );

    m_pEffect->SetTexture( hTexture1, m_pMeshAlphaTexture );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    // TODO: setup render states

    // Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
    m_pEffect->SetMatrix( m_hWorld, &matIdentity );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -40.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
    m_pEffect->SetMatrix( m_hView, &matView );

    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 0.1f, 1000.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
    m_pEffect->SetMatrix( m_hProj, &matProj );

    // Set up lighting states
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Restore the font
    m_pFont->RestoreDeviceObjects();
    
    // Restore the effect
    m_pEffect->OnResetDevice();

    HRESULT hr;
    hr = m_pd3dDevice->GetBackBuffer(   0, 
                                        0,
                                        D3DBACKBUFFER_TYPE_MONO,
                                        &m_pBackbuffer );
    if( FAILED( hr ) )
        DXTRACE_ERR( "GetBackBuffer", hr );
 
    hr = D3DXCreateTexture( m_pd3dDevice,
                            m_d3dsdBackBuffer.Width>>2,
                            m_d3dsdBackBuffer.Height>>2,
                            1,
                            D3DUSAGE_RENDERTARGET,
                            D3DFMT_A8R8G8B8,
                            D3DPOOL_DEFAULT,
                            &m_pRTTexture );
    
    if( FAILED( hr ) )
        DXTRACE_ERR( "D3DXCreateTexture", hr );

    hr = m_pRTTexture->GetSurfaceLevel( 0, &m_pRTSurface );
    if( FAILED( hr ) )
        DXTRACE_ERR( "GetSurfaceLevel", hr );

    hr = D3DXCreateTexture( m_pd3dDevice,
                            m_d3dsdBackBuffer.Width>>2,
                            m_d3dsdBackBuffer.Height>>2,
                            1,
                            D3DUSAGE_RENDERTARGET,
                            D3DFMT_A8R8G8B8,
                            D3DPOOL_DEFAULT,
                            &m_pBlurTexture );
    
    if( FAILED( hr ) )
        DXTRACE_ERR( "D3DXCreateTexture", hr );

    hr = m_pBlurTexture->GetSurfaceLevel( 0, &m_pBlurSurface );
    if( FAILED( hr ) )
        DXTRACE_ERR( "GetSurfaceLevel", hr );
    
    // Ready to Render!
    SetEvent( m_hReadyEvent );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    // TODO: update world

    // Update the world state according to user input
    D3DXMATRIX matWorld, matRotation;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

    m_fWorldRotY += 1.0f * m_fElapsedTime;
    m_fWorldRotX = 0.4f * sinf(1.f * m_fWorldRotY);


    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );

    D3DXMatrixTranslation( &matWorld, -0.5f, -0.5f, -0.25f );
    D3DXMatrixMultiply( &matRotation, &matRotX, &matRotY );
    D3DXMatrixMultiply( &matWorld, &matWorld, &matRotation );

    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
    m_pEffect->SetMatrix( m_hWorld, &matWorld );

    D3DXMATRIX matView;
    D3DXMatrixTranslation( &matView, 0.f, 0.f, 50.f );
    D3DXMatrixRotationY( &matRotY, 0.1666f * m_fWorldRotY );
    D3DXMatrixRotationX( &matRotX, -0.2f + 0.4f * cosf(0.2f*m_fWorldRotY+2.f) );
    D3DXMatrixMultiply( &matRotation, &matRotY, &matRotX );
    D3DXMatrixMultiply( &matView, &matRotation, &matView );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
    m_pEffect->SetMatrix( m_hView, &matView );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{

    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // TODO: render world
        UINT nPasses = 0;

        m_pd3dDevice->SetRenderTarget( 0, m_pRTSurface );

        // Clear the viewport
        m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                            0x00000000, 1.0f, 0L );

        m_pEffect->SetTechnique( m_hTechnique );
        m_pEffect->Begin( &nPasses, D3DXFX_DONOTSAVESTATE  );

        for( UINT i = 0; i< nPasses; i++ )
        {
            m_pEffect->BeginPass( i );
            
            // Render the mesh
            m_pD3DXMesh->DrawSubset(0);

	    m_pEffect->EndPass();
        }

        m_pEffect->End();

        m_pd3dDevice->SetRenderTarget( 0, m_pBlurSurface );

        // Clear the viewport
        m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                            0x00000000, 1.0f, 0L );

        static const D3DXVECTOR3 verts[] =
        {
            D3DXVECTOR3(-1.f, +1.f, +1.f),
            D3DXVECTOR3(+1.f, +1.f, +1.f),
            D3DXVECTOR3(-1.f, -1.f, +1.f),

            D3DXVECTOR3(-1.f, -1.f, +1.f),
            D3DXVECTOR3(+1.f, +1.f, +1.f),
            D3DXVECTOR3(+1.f, -1.f, +1.f),
        };

        static const D3DXVECTOR2 offsets[] =
        {
            D3DXVECTOR2( -0.030f, 0.000f ),
            D3DXVECTOR2( -0.015f, 0.000f ),
            D3DXVECTOR2( +0.015f, 0.000f ),
            D3DXVECTOR2( +0.030f, 0.000f ),
            D3DXVECTOR2( 0.000f, -0.030f ),
            D3DXVECTOR2( 0.000f, -0.015f ),
            D3DXVECTOR2( 0.000f, +0.015f ),
            D3DXVECTOR2( 0.000f, +0.030f )
        };

        m_pd3dDevice->SetFVF( D3DFVF_XYZ );
        m_pEffect->SetTechnique( m_hTechBlur );
        m_pEffect->SetValue( m_hTapOffset0, &offsets[0], sizeof( D3DXVECTOR2 ) );
        m_pEffect->SetValue( m_hTapOffset1, &offsets[1], sizeof( D3DXVECTOR2 ) );
        m_pEffect->SetValue( m_hTapOffset2, &offsets[2], sizeof( D3DXVECTOR2 ) );
        m_pEffect->SetValue( m_hTapOffset3, &offsets[3], sizeof( D3DXVECTOR2 ) );
        m_pEffect->SetTexture( m_hRTTexture, m_pRTTexture );
        m_pEffect->Begin( &nPasses, D3DXFX_DONOTSAVESTATE  );

        for( UINT i = 0; i< nPasses; i++ )
        {
            m_pEffect->BeginPass( i );
            
            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST,
                                            2,
                                            verts,
                                            sizeof(D3DXVECTOR3) );
           
            m_pEffect->EndPass();
        }

        m_pEffect->End();

        // Render to the backbuffer
        m_pd3dDevice->SetRenderTarget( 0, m_pBackbuffer );
        
        // Clear the viewport
        m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                            0x000000ff, 1.0f, 0L );

        m_pEffect->SetTechnique( m_hTechSkybox );
        m_pEffect->Begin( &nPasses, D3DXFX_DONOTSAVESTATE  );

        for( UINT i = 0; i< nPasses; i++ )
        {
            m_pEffect->BeginPass( i );
            
            // Render the mesh
            m_pSkyboxMesh->DrawSubset(0);
            m_pSkyboxMesh->DrawSubset(1);
            m_pSkyboxMesh->DrawSubset(2);
            m_pSkyboxMesh->DrawSubset(3);
            m_pSkyboxMesh->DrawSubset(4);
            m_pSkyboxMesh->DrawSubset(5);

            m_pEffect->EndPass();
        }

        m_pEffect->End();

        m_pEffect->SetTechnique( m_hTechnique );
        m_pEffect->Begin( &nPasses, D3DXFX_DONOTSAVESTATE  );

        for( UINT i = 0; i< nPasses; i++ )
        {
            m_pEffect->BeginPass( i );
            
            // Render the mesh
            m_pD3DXMesh->DrawSubset(0);

            m_pEffect->EndPass();
        }

        m_pEffect->End();

        m_pEffect->SetTechnique( m_hTechBlur );
        m_pEffect->SetValue( m_hTapOffset0, &offsets[4], sizeof( D3DXVECTOR2 ) );
        m_pEffect->SetValue( m_hTapOffset1, &offsets[5], sizeof( D3DXVECTOR2 ) );
        m_pEffect->SetValue( m_hTapOffset2, &offsets[6], sizeof( D3DXVECTOR2 ) );
        m_pEffect->SetValue( m_hTapOffset3, &offsets[7], sizeof( D3DXVECTOR2 ) );
        m_pEffect->SetTexture( m_hRTTexture, m_pBlurTexture );
        m_pEffect->Begin( &nPasses, D3DXFX_DONOTSAVESTATE  );

        for( UINT i = 0; i< nPasses; i++ )
        {
            m_pEffect->BeginPass( i );
            
            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST,
                                            2,
                                            verts,
                                            sizeof(D3DXVECTOR3) );

            m_pEffect->EndPass();

        }

        m_pEffect->End();




        // Render stats and help text  
        RenderText();

        // End the scene.
        m_pd3dDevice->EndScene();
    }

    return OnFrameNotify();
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, m_strDeviceStats );

    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, m_strFrameStats );

    // Output statistics & help
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height;

    LPCTSTR szMode = L"";
    switch( m_eMode )
    {
    case INITIALIZING:
        szMode = _T("Initializing...");
        break;
    case WAITING:
        szMode = _T("Waiting on all application instances...");
        break;
    case WARMUP:
        szMode = _T("Warm-up frames...");
        break;
    case TESTING:
        szMode = _T("Test-Frames...");
        break;
    case COOLDOWN:
        szMode = _T("Cool-Down Frames...");
        break;
    case PROCESSWAIT:
        szMode = _T("Waiting To Process...");
        break;
    case PROCESSING:
        szMode = _T("Processing Data...");
        break;
    }
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMode );

    if( 0.f != m_fKernelTime )
    {
        fNextLine -= 20.0f;
        m_pFont->DrawText( 2, fNextLine, fontColor, m_strKernelTime );
    }

    // TODO: disable F2, other keys
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp && ( NULL != hWnd ) )
            {
                // Draw on the window tell the user that the app is loading
                // TODO: change as needed
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf( strMsg, TEXT("Loading... Please wait") );
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }

    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();

    // invalidate the effect
    m_pEffect->OnLostDevice();

    SAFE_RELEASE( m_pBlurTexture );
    SAFE_RELEASE( m_pBlurSurface );
    SAFE_RELEASE( m_pRTTexture );
    SAFE_RELEASE( m_pRTSurface );
    SAFE_RELEASE( m_pBackbuffer );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // TODO: Cleanup any objects created in InitDeviceObjects()
    m_pFont->DeleteDeviceObjects();
    SAFE_RELEASE( m_pD3DXMesh );
    SAFE_RELEASE( m_pSkyboxMesh );
    SAFE_RELEASE( m_pSkyTexture );
    SAFE_RELEASE( m_pMeshDiffTexture );
    SAFE_RELEASE( m_pMeshAlphaTexture );

    // Release the effect
    SAFE_RELEASE( m_pEffect );
    
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    // TODO: Perform any final cleanup needed
    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::CreateSkyBox( LPDIRECT3DCUBETEXTURE9* ppTexture )
{
    HRESULT hr;

    LPDIRECT3DCUBETEXTURE9 pCubeTexture = NULL;

    hr = D3DXCreateCubeTexture( m_pd3dDevice,
                                512,
                                1,
                                0,
                                D3DFMT_X8R8G8B8,
                                D3DPOOL_MANAGED,
                                &pCubeTexture );

    if( FAILED( hr ) )
        DXTRACE_ERR( "D3DXCreateCubeTexture", hr );

    LPDIRECT3DSURFACE9 pCubeFace = NULL;

    typedef struct _MAP_DEF
    {
        D3DCUBEMAP_FACES face;
        LPCTSTR filename;
    } MAP_DEF;

    MAP_DEF faces[] =
    {
        D3DCUBEMAP_FACE_POSITIVE_Z, _T("lunar_front.jpg"),
        D3DCUBEMAP_FACE_NEGATIVE_Z, _T("lunar_back.jpg"),
        D3DCUBEMAP_FACE_POSITIVE_X, _T("lunar_right.jpg"),
        D3DCUBEMAP_FACE_NEGATIVE_X, _T("lunar_left.jpg"),
//        D3DCUBEMAP_FACE_POSITIVE_Y, _T("lunar_top.jpg"),
        D3DCUBEMAP_FACE_NEGATIVE_Y, _T("lunar_bottom.jpg"),
    };

    UINT nFace_count = sizeof(faces)/sizeof(faces[0]);

    for( UINT i = 0; i< nFace_count; i++ )
    {

        if( SUCCEEDED( hr ) )
        {
            hr = pCubeTexture->GetCubeMapSurface( faces[i].face,
                                                0,
                                                &pCubeFace );

            if( SUCCEEDED( hr ) )
            {
#ifdef RESOURCEFROMRC
                hr = D3DXLoadSurfaceFromResource(   pCubeFace,
                                                    NULL,
                                                    NULL,
                                                    NULL,
                                                    faces[i].filename,
                                                    NULL,
                                                    D3DX_DEFAULT,
                                                    0,
                                                    NULL );
#else
                hr = D3DXLoadSurfaceFromFile(   pCubeFace,
                                                NULL,
                                                NULL,
                                                faces[i].filename,
                                                NULL,
                                                D3DX_DEFAULT,
                                                0,
                                                NULL );
#endif
            }

            SAFE_RELEASE( pCubeFace );
        }
    }

    if( FAILED( hr ) )
    {
        SAFE_RELEASE( pCubeTexture );
    }


    *ppTexture = pCubeTexture;
    return hr;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMyD3DApplication::ResetFrame()
{
    m_fWorldRotY =  m_fWorldRotX = m_fElapsedTime = 0.f;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OnFrameNotify()
{
    switch( m_eMode )
    {
    case WAITING:
        if( StatusWarmup() )
            m_eMode = WARMUP;
        break;
    case WARMUP:
        if( StatusTesting() )
            m_eMode = TESTING;
        break;
    case TESTING:
        if( StatusCooldown() )
            m_eMode = COOLDOWN;
        break;
    case COOLDOWN:
        if( StatusProcessWait() )
            m_eMode = PROCESSWAIT;
        break;
    case PROCESSWAIT:
        if( StatusProcessing() )
            m_eMode = PROCESSING;
        break;
    case PROCESSING:
        // close the app
        ProcessStats();
        if( m_bPrimaryApp )
            RollupResults();
        return E_FAIL;
    }
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMyD3DApplication::CmdInitialize()
{
    // find our instance mutex;
    TCHAR szInstanceName[MAX_PATH];
    szInstanceName[MAX_PATH-1] = _T( '\0' );

    for(UINT i = 0; i < MAX_SIMULTANEOUS_APPS_SCH_BVT; i++)
    {
        _sntprintf(szInstanceName, MAX_PATH-2, _T("Global\\sched_bvt%d"), i+1);
        szInstanceName[MAX_PATH-1] = _T( '\0' );
        HANDLE hMutex = CreateMutex( NULL, FALSE, szInstanceName );

        if( hMutex )
        {
            if( WAIT_OBJECT_0 == WaitForSingleObject( hMutex, 0 ) )
            {
                m_hInstanceMutex = hMutex;
                m_nAppInstance = i;
                if( !i )
                    m_bPrimaryApp = true;
                break;
            }
        }
        CloseHandle( hMutex );
    }

    // bail out if app instance could not be determined
    if( NULL == m_hInstanceMutex )
        return;

    if( m_bPrimaryApp )
    {
        m_hTestEvent = CreateEvent( NULL, TRUE, FALSE, _T("sched_bvt_test") );
        m_hProcessEvent = CreateEvent( NULL, TRUE, FALSE, _T("sched_bvt_process") );
        
        for( UINT i = 1; i < m_nAppInstances; i++ )
        {
            STARTUPINFO si;
            PROCESS_INFORMATION pi;

            memset( &si, 0, sizeof(si) );
            si.cb = sizeof( si );
            memset( &pi, 0, sizeof(pi) );

            // launch secondary apps
            LPCTSTR sz = GetCommandLine();
            BOOL result = CreateProcess( NULL,
                                        GetCommandLine(),
                                        NULL,
                                        NULL,
                                        FALSE,
                                        0,
                                        NULL,
                                        NULL,
                                        &si,
                                        &pi );

            if( pi.hProcess )
                m_vecChildProcesses.push_back( pi.hProcess );
            if( pi.hThread )
                CloseHandle( pi.hThread );
        }
    }

    else
    {
        m_hTestEvent = OpenEvent( SYNCHRONIZE, FALSE, _T("sched_bvt_test") );
        m_hProcessEvent = OpenEvent( SYNCHRONIZE, FALSE, _T("sched_bvt_process") );
    }

    TCHAR szEventName[MAX_PATH];
    szEventName[MAX_PATH-1] = _T( '\0' );

    _sntprintf(szEventName, MAX_PATH-2, _T("Global\\sched_bvt_ready%d"), m_nAppInstance+1);
    szEventName[MAX_PATH-1] = _T( '\0' );
    m_hReadyEvent = CreateEvent( NULL, TRUE, FALSE, szEventName );
    

    _sntprintf(szEventName, MAX_PATH-2, _T("Global\\sched_bvt_readyprocess%d"), m_nAppInstance+1);
    szEventName[MAX_PATH-1] = _T( '\0' );
    m_hReadyToProcessEvent = CreateEvent( NULL, TRUE, FALSE, szEventName );

    m_vecTimes.reserve( m_nTestFrames );

    // TODO: set focus away from app
    m_eMode = WAITING;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
bool CMyD3DApplication::StatusWarmup()
{
    HANDLE hReadyEvent;

    if( !m_bPrimaryApp )
        return true;

    bool bResult = true;

    for( UINT i = 0; i < m_nAppInstances; i++ )
    {
        TCHAR szEventName[MAX_PATH];
        szEventName[MAX_PATH-1] = _T( '\0' );

        _sntprintf(szEventName, MAX_PATH-2, _T("Global\\sched_bvt_ready%d"), i+1);
        szEventName[MAX_PATH-1] = _T( '\0' );
        hReadyEvent = OpenEvent( SYNCHRONIZE, FALSE, szEventName );

        if( hReadyEvent )
        {
            bResult &= (WAIT_OBJECT_0 == WaitForSingleObject( hReadyEvent, 0 ));
            CloseHandle( hReadyEvent );
        }
        else
            bResult = false;
    }

    return bResult;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
bool CMyD3DApplication::StatusTesting()
{
    bool bResult = false;

    m_nWarmUpFrames_count++;
    if( m_bPrimaryApp )
    {
        if( m_nWarmUpFrames_count == m_nWarmUpFrames )
        {
            SetEvent( m_hTestEvent );
            bResult =  true;
        }
    }
    else
    {
        if( WAIT_OBJECT_0 == WaitForSingleObject( m_hTestEvent, 0 ) )
            bResult = true;
    }
    
    if( bResult )
    {
        ResetFrame();
        m_nWarmUpFrames_count = 0;
    }

    return bResult;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
bool CMyD3DApplication::StatusCooldown()
{
    m_nTestFrames_count++;
    __int64 timestamp = 0;
    TimeStamp( &timestamp );
    m_vecTimes.push_back( timestamp );

    if( m_nTestFrames_count == m_nTestFrames )
    {
        SetEvent( m_hReadyToProcessEvent );
        FILETIME Creation, Exit, Kernel, User;
        if( GetThreadTimes( GetCurrentThread(), &Creation, &Exit, &Kernel, &User ) )
        {
            ULARGE_INTEGER uKernel, uUser;
            uKernel.LowPart  = Kernel.dwLowDateTime;
            uKernel.HighPart = Kernel.dwHighDateTime;
            uUser.LowPart    = User.dwLowDateTime;
            uUser.HighPart   = User.dwHighDateTime;

            m_fKernelTime = ((float)(__int64)uKernel.QuadPart) / (float)( (__int64)uKernel.QuadPart + uUser.QuadPart );
            _stprintf( m_strKernelTime, _T("Kernel Time: %.2f%%   User Time: %.2f%%"), 100.f * m_fKernelTime, 100.f * (1.f-m_fKernelTime) );
        }
        return true;
    }

    return false;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
bool CMyD3DApplication::StatusProcessWait()
{
    m_nWarmUpFrames_count++;
    
    if( m_nWarmUpFrames_count == m_nWarmUpFrames )
        return true;

    if( !m_bPrimaryApp && ( WAIT_OBJECT_0 == WaitForSingleObject( m_hProcessEvent, 0 ) ) )
        return true;

    return false;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
bool CMyD3DApplication::StatusProcessing()
{
    bool bResult = false;

    if( m_bPrimaryApp )
    {
        bResult = true;

        for( UINT i = 0; i < m_nAppInstances; i++ )
        {
            HANDLE hReadyEvent;
            TCHAR szEventName[MAX_PATH];
            szEventName[MAX_PATH-1] = _T( '\0' );

            _sntprintf(szEventName, MAX_PATH-2, _T("Global\\sched_bvt_readyprocess%d"), i+1);
            szEventName[MAX_PATH-1] = _T( '\0' );
            hReadyEvent = OpenEvent( SYNCHRONIZE, FALSE, szEventName );

            if( hReadyEvent )
            {
                bResult &= (WAIT_OBJECT_0 == WaitForSingleObject( hReadyEvent, 0 ));
                CloseHandle( hReadyEvent );
            }
            else
                bResult = false;
        }
        if( bResult )
            SetEvent( m_hProcessEvent );
    }
    else
    {
        if( WAIT_OBJECT_0 == WaitForSingleObject( m_hProcessEvent, 0 ) )
            bResult = true;
    }

    return bResult;
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMyD3DApplication::ProcessStats()
{
    TCHAR sz[MAX_PATH];
    tstring logData;

    vector<__int64> vecTimings;
    vecTimings.reserve( m_vecTimes.size() -1 );

    vector<__int64>::iterator t2 = m_vecTimes.begin();
    vector<__int64>::iterator t1 = m_vecTimes.begin();
    while( ++t2 != m_vecTimes.end() )
    {
        vecTimings.push_back( (*t2) - (*t1++) );
    }

    double mean, standard_deviation;

    getMeanAndDeviation(vecTimings, mean, standard_deviation);


    DWORD dwT1, dwT2;
    __int64 i64T1, i64T2;

    dwT1 = timeGetTime();
    TimeStamp( &i64T1 );

    Sleep(4000);
    dwT2 = timeGetTime();
    TimeStamp( &i64T2 );

    double fClocksPerMsec = (double)(i64T2-i64T1)/(double)(dwT2-dwT1);

    _stprintf( sz, _T("kernel time (%%) = %.2f\r\n"), 100.f * m_fKernelTime );
    logData += sz;
    _stprintf( sz, _T("mean(M cpu ticks) = %f\r\n"), mean*0.000001 );
    logData += sz;
    _stprintf( sz, _T("standard deviation(M cpu ticks) = %f\r\n"), standard_deviation*0.000001 );
    logData += sz;
    _stprintf( sz, _T("mean(ms) = %f\r\n"), mean / fClocksPerMsec );
    logData += sz;
    _stprintf( sz, _T("standard deviation(ms) = %f\r\n"), standard_deviation / fClocksPerMsec );
    logData += sz;
    _stprintf( sz, _T("mean(FPS) = %f\r\n"), 1000. * fClocksPerMsec / mean );
    logData += sz;


    TCHAR szLogFileName[MAX_PATH];
    _stprintf( szLogFileName, _T("sched_basic%d.log"), m_nAppInstance );

    CLog log;
    log.open( szLogFileName, log.WRITE );
    
    tstring logHeader;
    LogHeader( logHeader );
    
    log.write( logHeader.c_str() );
    log.write( logData.c_str() );
    log.write( _T("\r\n") );
    
    log.close();
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMyD3DApplication::ParseCommandLine()
{
    LPTSTR szCmdLine = GetCommandLine();

    if( !szCmdLine )
        return;

    LPTSTR szInstances;
    
    if( NULL != (szInstances = _tcsstr( szCmdLine, _T("-I:") ) ) )
    {
        szInstances += _tcslen( _T("-I:") );

        UINT nInstances;
        if( 1 == _stscanf( szInstances, _T("%d"), &nInstances ) )
        {
            m_nAppInstances = nInstances;
        }
    }

    LPTSTR szMonitor;
    if( NULL != (szMonitor = _tcsstr( szCmdLine, _T("monitor:") ) ) )
    {
        szMonitor += _tcslen( _T("monitor:") );

        UINT nMonitor;
        if( 1 == _stscanf( szMonitor, _T("%d"), &nMonitor ) )
        {
            //WHQL requires that the monitor index be 1-based
            if(0 != nMonitor)
            {
                m_nAppMonitor = nMonitor - 1;
            }
            
        }
    }

    // See if the user wants to sync. If so, we'll need to read the sync string and
    // count of processes.
    LPTSTR szProcCount;
    if( NULL != (szProcCount = _tcsstr( szCmdLine, _T("-ProcCount:") ) ) )
    {
        szProcCount += _tcslen( _T("-ProcCount:") );

        UINT nProcCount;
        if( 1 == _stscanf( szProcCount, _T("%d"), &nProcCount ) )
        {
            m_nProcCount = nProcCount;
        }
    }

    LPTSTR szSyncWith;
    if( NULL != (szSyncWith = _tcsstr( szCmdLine, _T("-SyncWith:") ) ) )
    {
        szSyncWith += _tcslen( _T("-SyncWith:") );

        if( 1 == _stscanf( szSyncWith, _T("%s"), &m_wszSyncWith ) )
        {
            m_bSyncWith = true;
        }
    }

}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
void CMyD3DApplication::LogHeader( tstring& logHeader )
{
    TCHAR sz[MAX_PATH];
    logHeader += _T("========================================================\r\n") ;
    _stprintf( sz, _T("sched_bvt%d\r\n"), m_nAppInstance );
    logHeader += sz;
    logHeader += _T("DATE: ");
    _tstrdate( sz );
    logHeader += sz;
    logHeader += _T("\r\nTIME: ");
    _tstrtime( sz );
    logHeader += sz;
    logHeader += _T("\r\n");
    logHeader += _T("========================================================\r\n");
}


//-----------------------------------------------------------------------------
// Name: 
// Desc: 
//-----------------------------------------------------------------------------
bool CMyD3DApplication::RollupResults( )
{
    vector<HANDLE>::iterator it;

    while( !m_vecChildProcesses.empty() )
    {
        if( WAIT_OBJECT_0 == WaitForSingleObject( m_vecChildProcesses.back(), INFINITE ) )
        {
            CloseHandle( m_vecChildProcesses.back() );
            m_vecChildProcesses.pop_back();
        }
    };

    vector< float > vecFrameTimes; 
    vector< float > vecDeviations; 
    tstring logMessage;
    TCHAR sz[MAX_PATH];
    sz[MAX_PATH-1] = _T('\0');
    CLog log;
    bool bResult = true;
    float frametime, deviation, kerneltime;
    for( UINT i = 0; i < m_nAppInstances; i++ )
    {
        _sntprintf( sz, MAX_PATH-2, _T("sched_basic%d.log"), i );
        sz[MAX_PATH-1] = _T('\0');
        bResult &= log.open( sz, log.READ );
        bResult &= log.readValue( _T("kernel time (%) = "), _T("%f"), kerneltime );
        bResult &= log.readValue( _T("mean(ms) = "), _T("%f"), frametime );
        bResult &= log.readValue( _T("standard deviation(ms) = "), _T("%f"), deviation );
        if( !bResult )
            return false;
        vecFrameTimes.push_back( frametime );
        vecDeviations.push_back( deviation );
        log.close();
        DeleteFile( sz );
        _stprintf( sz, _T("kernel time[%d] (%%) = %f\r\n"), i, kerneltime );
        logMessage += sz;
        _stprintf( sz, _T("mean frametime[%d] (ms) = %f\r\n"), i, frametime );
        logMessage += sz;
        _stprintf( sz, _T("standard deviation[%d] (ms) = %f\r\n"), i, deviation );
        logMessage += sz;
    }
        

    float mean_frametime = 0.f, std_dev_frametime = 0.f, mean_stddev = 0.f;
    if( m_nAppInstances > 1 )
        getMeanAndDeviation( vecFrameTimes, mean_frametime, std_dev_frametime );
    else
        getMean( vecFrameTimes, mean_frametime );

    getMean( vecDeviations, mean_stddev );

    logMessage += _T("\r\nSUMMARY:\r\n");
    _stprintf( sz, _T("Average mean frametime(FPS) = %f\r\n"), 1000.f / mean_frametime );
    logMessage += sz;
    _stprintf( sz, _T("Average mean frametime(ms) = %f\r\n"), mean_frametime );
    logMessage += sz;
    if( m_nAppInstances > 1 )
    {
        _stprintf( sz, _T("Deviation mean frametime(ms) = %f\r\n"), std_dev_frametime );
        logMessage += sz;
    }
    _stprintf( sz, _T("Average standard deviation(ms) = %f\r\n"), mean_stddev );
    logMessage += sz;

    tstring logHeader;
    LogHeader( logHeader );

    if( bResult = log.open( _T("sched_basic.log" ), log.WRITE|log.READ ) )
    {
        bResult &= log.write( logHeader.c_str() );
        bResult &= log.write( logMessage.c_str() );
        bResult &= log.write( _T("\r\n") );
        log.close();
    }

    return bResult;
}

void CMyD3DApplication::CreateWTTLog()
{
    m_pWTTLog = new CCTLLogger;
    if(NULL == m_pWTTLog)
    {
        throw std::bad_alloc();
    }

    m_hWTTTestCase = m_pWTTLog->StartTest(L"Sched_Basic test case");
}

void CMyD3DApplication::DestroyWTTLog()
{
    if(NULL != m_hWTTTestCase)
    {
        m_pWTTLog->EndTest(m_hWTTTestCase);
        m_hWTTTestCase = NULL;
    }

    if(NULL != m_pWTTLog)
    {
        delete m_pWTTLog;
        m_pWTTLog = NULL;
    }
}