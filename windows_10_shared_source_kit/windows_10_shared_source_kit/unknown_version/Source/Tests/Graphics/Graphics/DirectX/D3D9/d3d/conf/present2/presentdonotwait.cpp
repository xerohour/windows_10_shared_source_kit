// PresentDoNotWait.cpp: implementation of the CPresentDoNotWait class.
//
//////////////////////////////////////////////////////////////////////

#include "PresentDoNotWait.h"
#include "CSwapChain.h"
#include "CSurface.h"


const int xDivisions = 10;
const int yDivisions = 10;
const int Vertixes = 18;
		
//
// Name: FillD3DXTexture
// Desc: method called from the D3DX, fills a texture
//

VOID __stdcall FillTexture9(D3DXVECTOR4 *pOut, D3DXVECTOR2 *pTexCoord, D3DXVECTOR2 *pTexelSize, LPVOID pData)
{
	DWORD xPixel = (DWORD) ( pTexCoord->x / pTexelSize->x + 0.51 );		// current Pixel X
	DWORD yPixel = (DWORD) ( pTexCoord->y / pTexelSize->y + 0.51 );		// current Pixel Y
	DWORD xMaxPixel = (DWORD) ( 1.00f / pTexelSize->x + 0.51 );			// Max Pixel X
	DWORD yMaxPixel = (DWORD) ( 1.00f / pTexelSize->y + 0.51 );			// Max Pixel Y

	ASSERT( xPixel <= xMaxPixel && yPixel <= yMaxPixel );

	float xSpace = 4.0f * ( xPixel >> 2 ) / float( xMaxPixel );
	float ySpace = 4.0f * ( yPixel >> 2 ) / float( yMaxPixel );

	int iObject = 50;

	pOut->x = 0.5f * xSpace * iObject/100.0f + 0.5f * ySpace * (101-iObject)/100.0f;
	pOut->y = 0.75f * xSpace * iObject/100.0f + 0.25f * ySpace * (101-iObject)/100.0f;
	pOut->z = 0.25f * xSpace * iObject/100.0f + 0.75f * ySpace * (101-iObject)/100.0f;
	pOut->w = 0.6f * xSpace + 0.4f * ySpace;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPresentDoNotWait::CPresentDoNotWait() :
	m_pVB( NULL ),
	m_pTexture( NULL )
{

	SetTestType(TESTTYPE_CONF);

	//m_Options.D3DOptions.fMinDXVersion = 9.0f;

	//m_Options.D3DOptions.bCachedBackBuffer	= false;
	//m_Options.D3DOptions.bZBuffer			= false;
	//m_Options.D3DOptions.dwDevTypes			= DEVICETYPE_HAL;
	//m_Options.D3DOptions.dwPresentInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	m_Options.D3DOptions.dwWinMode			= WINMODE_FULLSCREEN;
	m_Options.D3DOptions.dwSwapEffect		= SWAPEFFECT_FLIP;


    m_Options.D3DOptions.fMinDXVersion = 9.0f;

    m_Options.D3DOptions.bReference         = false;
	m_Options.D3DOptions.bCachedBackBuffer	= false;
	m_Options.D3DOptions.bZBuffer			= false;
	m_Options.D3DOptions.dwDevTypes			= DEVICETYPE_HAL;
    m_Options.D3DOptions.dwPresentInterval	= D3DPRESENT_INTERVAL_DEFAULT;
    m_Options.D3DOptions.dwPresentFlags     = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

}


//////////////////////////////////////////////////////////////////////
// Name: CommandLineHelp()
// Desc: command line help for the test
//////////////////////////////////////////////////////////////////////
void CPresentDoNotWait::CommandLineHelp()
{
	WriteCommandLineHelp(_T("The test checks if the driver returns D3DERR_WASSTILLDRAWING\n"));
	WriteCommandLineHelp(_T("when test specifies D3DPRESENT_DONOTWAIT\n"));
}


//////////////////////////////////////////////////////////////////////
// Name: Check_HRFail( HRESULT, const char )
// Desc: if hResult is not D3D_OK, throws an exception
//////////////////////////////////////////////////////////////////////
void CPresentDoNotWait::Check_HRFail( HRESULT hResult, const char *sMethod )
{
	if( FAILED(hResult) )
	{
		WriteToLog( TEXT( "%s failed, error code %s (0x%x)\n" ),
				sMethod,
				m_pD3D->HResultToString( hResult ),
				hResult );
		Fail();
		throw hResult;
	}
}


// -------------------------------------------------------------------------------------
bool CPresentDoNotWait::CapsCheck()
{
	HRESULT hr;
    bool retVal =false;
	
    if( KeySet( _T( "MultiHead" ) ) && !m_bFullscreen )
    {
        WriteToLog(_T("Windowed tests don't support -MultiHead"));
		return false;
    }
    
    // go back windowed if test requires.  Do this here so the check for DWM wont be influenced by being in fullscreen
	if( !m_bFullscreen )
	{
		
        DWORD behaviors = m_pSrcDevice->GetBehaviors();
        m_pSrcDevice->SetBehaviors(behaviors & ~DEVICECREATE_ADAPTERGROUP_DEVICE);
        
        PRESENT_PARAMETERS presentationParameters, *pPresentParameters = m_pSrcDevice->GetPresParams();
		memcpy( &presentationParameters, pPresentParameters, sizeof( PRESENT_PARAMETERS ) );
		presentationParameters.bWindowed = true;
        presentationParameters.dwFlags &= ~D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        presentationParameters.uFullscreenRefresh = D3DPRESENT_RATE_DEFAULT;

        if( FAILED( hr = m_pSrcDevice->UpdateDevice( &presentationParameters ) ) )
		{
			WriteToLog( _T( "UpdateDevice failed, return code %s (0x%x)\n" ),
							m_pD3D->HResultToString( hr ),
							hr );
			return false;
		}

        //
        //  Wait for DWM to fire back up
        //
        Sleep(1000);
	}
 
    CAPS DeviceCaps;
	if( FAILED( hr = m_pSrcDevice->GetDeviceCaps(&DeviceCaps) ) )
	{
		WriteToLog(
				_T("Error: GetDeviceCaps failed: %s (0x%x). Some tests might show inaccurate results\n"),
				m_pD3D->HResultToString(hr),
				hr 
			);
		return false;
	}
	// reject all non-DX9 drivers
	bool bDX9Driver = 0 != ( DeviceCaps.DevCaps2 & D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES );

	// this test cannot be run on REF;
	bool bNonREF = ( D3DDEVTYPE_HAL == m_pSrcDevice->GetDevType() );

    retVal = bDX9Driver & bNonREF;
    
    
    //
    // FlipEx checks
    //
    if(!m_bFullscreen)
    {
        bool bFlipEx = true;

        OSVERSIONINFO osVer;
        ZeroMemory( &osVer, sizeof OSVERSIONINFO );
        osVer.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
        GetVersionEx( &osVer );
        if( ((osVer.dwMajorVersion == 6) && (osVer.dwMinorVersion < 1)) 
            || (osVer.dwMajorVersion != 6 && osVer.dwMajorVersion < 7))
        {
            WriteToLog("Windowed test requires FlipEx which is not supported for Os versions < 6.1  Os Version=%d.%d", osVer.dwMajorVersion, osVer.dwMinorVersion);
            bFlipEx = false;   
        }

        if( m_pSrcDevice->GetInterface() < 9.1f )
        {
            WriteToLog("Windowed test requires FlipEx which is not supported for Dx versions < 9.1.  Dx=%d", m_pSrcDevice->GetInterface());
            bFlipEx = false;  
        }

        bool isDWMEnabled = IsDWMEnabled();
        if(!isDWMEnabled)
        {
            WriteToLog("Windowed test requires FlipEx which is not supported when DWM is off.");
            bFlipEx = false;  
        }

        retVal = retVal & bFlipEx;

    }

    return retVal;
}


// -------------------------------------------------------------------------------------
UINT CPresentDoNotWait::TestInitialize()
{
	
    if(!m_bFullscreen && KeySet(_T("WHQL")) )
    {
         WriteToLog(_T("Windowed FlipEx PresentDoNotWait tests not are not for WHQL"));
         return D3DTESTINIT_SKIPALL;
    }

    UINT uResult = CD3DTest::TestInitialize();
    
	m_vecPresentIntervals.clear();
	m_vecPresentIntervals.push_back( D3DPRESENT_INTERVAL_IMMEDIATE );
	m_vecPresentIntervals.push_back( D3DPRESENT_INTERVAL_DEFAULT );
	m_vecPresentIntervals.push_back( D3DPRESENT_INTERVAL_ONE );

	SetTestRange( 1, m_vecPresentIntervals.size() );
	
	//
	// Sleep for 9 seconds before each test.  Mode changes can take this long to finish
	//
	Sleep(9000);

	return uResult;    
}

bool CPresentDoNotWait::TestTerminate()
{
	onDestroy();

	return CD3DTest::TestTerminate();
}


// -------------------------------------------------------------------------------------
bool CPresentDoNotWait::ExecuteTest(UINT uTestNumber)
{
	HRESULT hr = D3D_OK;
	CSwapChain *pSwapChain = NULL;
	LPSURFACE pBackBuffer = NULL;
	CSurface *pTempRenderTarget = NULL;


	DWORD dwClearColors[] = 
	{
		0xFF102040,
		0xFF303048,
		0xFF504050,
		0xFF705058,
		0xFF906060,
		0xFFB07068,
		0xFFD08070,
		0xFFF09078,
	};
	UINT dwClearColorsCount = sizeof( dwClearColors ) / sizeof( DWORD );

	char szTestName[64];
	_snprintf_s( szTestName, 64, 63, "Repeat 100x : Clear, Present [Interval:%d]", m_vecPresentIntervals[ uTestNumber - 1 ]  );
	BeginTestCase( szTestName, uTestNumber );

	onDestroy();

    UINT uAdapterCount = 0;

    if ( ( m_pSrcDevice->GetBehaviors() & DEVICECREATE_ADAPTERGROUP_DEVICE ) == DEVICECREATE_ADAPTERGROUP_DEVICE )
	{
        uAdapterCount = m_pSrcDevice->GetCaps()->NumberOfAdaptersInGroup;
    }
    else
    {
        uAdapterCount = 1;   
    }

	// change the presentation interval
	PRESENT_PARAMETERS *pd3dpp;
	pd3dpp = new PRESENT_PARAMETERS[uAdapterCount];
	if( NULL == pd3dpp )
	{
		Abort();
		GetLastError();
		return false;
	}

    memset( pd3dpp, 0, uAdapterCount * sizeof( PRESENT_PARAMETERS ) );
    
    
    //
    // And only copy the amount that are returned from GetPresParams
    //
    memcpy( pd3dpp, m_pSrcDevice->GetPresParams(), uAdapterCount * sizeof( PRESENT_PARAMETERS ) );

	BOOL sleepNeeded = FALSE;
	
	for( UINT uAdapter = 0; uAdapter < uAdapterCount; uAdapter++ )
	{
		if(pd3dpp[uAdapter].bWindowed != (!m_bFullscreen))
		{
			sleepNeeded = TRUE;	
		}

		pd3dpp[uAdapter].BackBufferFormat = FMT_A8R8G8B8;
		pd3dpp[uAdapter].uPresentInterval = m_vecPresentIntervals[ uTestNumber - 1 ];

		if(m_bFullscreen)
        {
            // must specify one backbuffer and FLIP swap effect
		    pd3dpp[uAdapter].uBackBufferCount = 1;
        }
        else
        {
            //  for flipex BackBufferCount is never less than 2. 
            //  D3DERR_WASSTILLDRAWING wont be returned unless SetMaximumFrameLatency is less than BackBufferCount
            pd3dpp[uAdapter].uBackBufferCount = 3;
            m_pSrcDevice->SetMaximumFrameLatency(pd3dpp[0].uBackBufferCount - 2 );
        }

		pd3dpp[uAdapter].SwapEffect = m_swapEffect;
		pd3dpp[uAdapter].bWindowed = !m_bFullscreen;
        pd3dpp[uAdapter].uFullscreenRefresh = D3DPRESENT_RATE_DEFAULT;
        

	}

    // 598319	Present2_FullScreen:  Reset fails with D3DERR_INVALIDCALL on Intel parts
    if( FAILED( hr = m_pSrcDevice->Reset( pd3dpp ) ) )
	{
        WriteToLog( _T( "CPresentDoNotWait::ExecuteTest: Reset failed, return code %s (0x%x)\n" ),
						m_pD3D->HResultToString( hr ),
						hr );
        Skip();
		delete[] pd3dpp;
        pd3dpp = NULL;

		return false;
	}

	UINT matricesWidth = pd3dpp->uWidth;
    UINT matricesHeight = pd3dpp->uHeight;

    delete[] pd3dpp;
    pd3dpp = NULL;

	m_pFramework->ProcessMessages();
	Sleep(100);
	
	if(sleepNeeded)
	{
		Sleep(5000);
	}

	onCreate();
	SetDefaultMatrices((float)matricesWidth, (float)matricesHeight);
	SetDefaultMaterials();
	SetDefaultLights();
	SetDefaultRenderStates();

	D3DADAPTER_IDENTIFIER9 adapterIdentifier;
	ZeroMemory( &adapterIdentifier, sizeof( D3DADAPTER_IDENTIFIER9 ) );
	m_pD3D->GetAdapterIdentifier(  m_pSrcDevice->GetAdapterID(), 0, (ADAPTER_IDENTIFIER*)&adapterIdentifier );

  try
  {
	for( UINT iSwapChain = 0; iSwapChain < m_pSrcDevice->GetNumberOfSwapChains(); iSwapChain++ )
	{
		Check_HRFail( m_pSrcDevice->Clear( 0, NULL, FMWK_DEFAULT, 0x00e05090, 1.f, 0 ), "Clear" );
		Check_HRFail( m_pSrcDevice->GetSwapChain( iSwapChain, &pSwapChain ), "GetSwapChain" );

		RELEASE( pBackBuffer );
		Check_HRFail( pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ), "GetBackBuffer" );
		Check_HRFail( m_pSrcDevice->SetRenderTarget( (DWORD)0, pBackBuffer ), "SetRenderTarget" );

		CVertexBuffer * pVB = NULL;
		CSurface * pOriginalRenderTarget = NULL;
	
		const UINT width = 2048;
		const UINT height = 2048;
		SetDefaultMatrices((float)width, (float)height);
	
		//
		// Create a large render target to help keep the GPU busy
		//
		SURFACEDESC originalRenderTargetDesc;
		Check_HRFail( m_pSrcDevice->GetRenderTarget(0, &pOriginalRenderTarget), "GetRenderTarget" );
		Check_HRFail( pOriginalRenderTarget->GetDesc( &originalRenderTargetDesc ), "GetDesc" );
		RELEASE( pOriginalRenderTarget );

		Check_HRFail( m_pSrcDevice->CreateRenderTarget
		(
			width,
			height,
			originalRenderTargetDesc.Format,
			MULTISAMPLE_NONE,
			0,
			FALSE,
			&pTempRenderTarget,
			NULL
		), 
		"CreateRenderTarget" );
	
		// verify Present called from the swapchain doesn't return error if the flag is 0
		int i;
		for(i = 0; i < 100; i++ )
		{
			Check_HRFail( m_pSrcDevice->Clear( 0, NULL, FMWK_DEFAULT, dwClearColors[i % dwClearColorsCount], 1.f, 0 ), "Clear" );
			onDraw();
			Check_HRFail( pSwapChain->Present( NULL, NULL, NULL, NULL, 0 ), "Present" );
		}

		m_pFramework->ProcessMessages();
		Sleep( 100 );

		// verify Present called from the swapchain returns error if the flag is DONOTWAIT
		for( i = 0; i < 100; i++ )
		{
			//
			// Render the scene to a large backbuffer to keep the GPU busy
			//
			Check_HRFail( m_pSrcDevice->SetRenderTarget( (DWORD)0, pTempRenderTarget ), "SetRenderTarget" );
			Check_HRFail( m_pSrcDevice->Clear( 0, NULL, FMWK_DEFAULT, dwClearColors[i % dwClearColorsCount], 1.f, 0 ), "Clear" );
			onDraw();

			Check_HRFail(m_pSrcDevice->StretchRect(
				pTempRenderTarget,
				NULL,
				pBackBuffer,
				NULL,
				D3DTEXF_NONE), "StretchRect" );
						
			if( D3DERR_WASSTILLDRAWING == pSwapChain->Present( NULL, NULL, NULL, NULL, D3DPRESENT_DONOTWAIT ) )
				break;
		}
		
		// under LDDM, dx9.0c devices no longer return D3DERR_WASSTILLDRAWING
		if( m_pSrcDevice->GetInterface() < 9.1f && DRIVERMODEL_LDDM == m_pD3D->GetDriverModel() )
		{
			if( i < 100 )
			{
				WriteToLog( _T( "Spare cycles detected under LDDM and dx9.0c\n" ) );
				Fail();
			}
		}
		else
		{
			if( 100 == i )
			{
				WriteToLog( _T( "No spare cycles detected D3DERR_WASSTILLDRAWING was not returned from present\n" ) );
				if(m_vecPresentIntervals[ uTestNumber - 1 ] == D3DPRESENT_INTERVAL_IMMEDIATE)
                {
                    //
                    // Faster hardware can process these presents fast enough to never return D3DERR_WASSTILLDRAWING.
                    // Instead of endlessly increasing the scene complexity to match hardware, just skip the D3DPRESENT_INTERVAL_IMMEDIATE case.
                    //
                    Skip();
                }
                else
                {
                    Fail();
                }
			}
			else
				WriteToLog( _T( "Spare cycles detected after %d frames\n" ), i );
		}

		m_pFramework->ProcessMessages();
		Sleep( 100 );
		Check_HRFail( m_pSrcDevice->SetRenderTarget( (DWORD)0, pBackBuffer ), "SetRenderTarget" );
		RELEASE( pTempRenderTarget );
		
		// establish initial conditions for verification of Flip
		Check_HRFail( m_pSrcDevice->Clear( 0, NULL, FMWK_DEFAULT, dwClearColors[0], 1.f, 0 ), "Clear" );
		Check_HRFail( pSwapChain->Present( NULL, NULL, NULL, NULL, 0 ), "Present" );
		Check_HRFail( m_pSrcDevice->Clear( 0, NULL, FMWK_DEFAULT, dwClearColors[1], 1.f, 0 ), "Clear" );
		Check_HRFail( pSwapChain->Present( NULL, NULL, NULL, NULL, 0 ), "Present" );
		Check_HRFail( m_pSrcDevice->Clear( 0, NULL, FMWK_DEFAULT, dwClearColors[2], 1.f, 0 ), "Clear" );
		Check_HRFail( pSwapChain->Present( NULL, NULL, NULL, NULL, 0 ), "Present" );
		int iLastBackBuffer = 1, iLastFrontBuffer = 2;
		
		SetDefaultMatrices((float)matricesWidth, (float)matricesHeight);
		
		for( i = 3; i < 100; i++ )
		{
			Check_HRFail( m_pSrcDevice->Clear( 0, NULL, FMWK_DEFAULT, dwClearColors[i % dwClearColorsCount], 1.f, 0 ), "Clear" );
			onDraw();

			bool bWasStillDrawing = D3DERR_WASSTILLDRAWING == pSwapChain->Present( NULL, NULL, NULL, NULL, D3DPRESENT_DONOTWAIT );

			if( !bWasStillDrawing )
			{
				// Present succeeded, so the backbuffer becomes the old front buffer after the flip
				iLastBackBuffer = iLastFrontBuffer;
				iLastFrontBuffer = i;
			}
			else
			{
				// Present failed, so the backbuffer contains the recently drawn image, but the frontbuffer is unchanged
				iLastBackBuffer = i;
			}

			bool bSaveBB = false;
			
            
            //
            // Cannot lock back buffers with FLIPEX
            //
            if(m_bFullscreen)
            {
                // examine the first pixel of the backbuffer, every nth frame
                if( ( i % 5 ) == 2 )
                {
                    LOCKED_RECT pLRect;
                    RECT bbRect;
                    bbRect.left = 0;
                    bbRect.right = 1;
                    bbRect.top = 0;
                    bbRect.bottom = 1;
                    Check_HRFail( pBackBuffer->LockRect( &pLRect, &bbRect, LOCK_READONLY ), "LockRect" );
                    DWORD dwColor = *(DWORD*)(pLRect.pBits);
                    if( dwColor != dwClearColors[iLastBackBuffer % dwClearColorsCount] )
                    {
                        UINT uColorMatch;
                        for( uColorMatch = 0; uColorMatch < dwClearColorsCount; uColorMatch++ )
                            if( dwClearColors[uColorMatch % dwClearColorsCount] == dwColor )
                                break;

                        WriteToLog( _T( "Failure: Frame: %d (WASSTILLDRAWING:%s), Expected Back Buffer: %d (MOD %d) - Color: %0X , Found: %d - Color: %0X\n" ), 
                            i,
                            bWasStillDrawing? _T( "true" ) : _T( "false" ),
                            iLastBackBuffer, dwClearColorsCount, dwClearColors[iLastBackBuffer % dwClearColorsCount],
                            ( uColorMatch == dwClearColorsCount )? -1 : uColorMatch, dwColor
                            );
                        Fail();

                        if( !bSaveBB )
                            bSaveBB = KeySet( _T( "Save" ) );
                    }

                    Check_HRFail( pBackBuffer->UnlockRect(), "UnlockRect" );

                    if( bSaveBB )
                    {
                        char szSurface[20];
                        _snprintf_s( szSurface, 20, 19, "BackBuffer%d.bmp", i );
                        m_pSrcDevice->D3DXSaveSurfaceToFile( szSurface, D3DXIFF_BMP, pBackBuffer, NULL, NULL );
                    }
                }
            }
		}

		// clear to green before moving on
		Check_HRFail( m_pSrcDevice->Clear( 0, NULL, FMWK_DEFAULT, 0xFF008040, 1.f, 0 ), "Clear" );
		Check_HRFail( pSwapChain->Present( NULL, NULL, NULL, NULL, 0 ), "Present" );

		RELEASE( pBackBuffer );
		RELEASE( pSwapChain );
	}
  }
  catch (...)
  {
	  WriteToLog (TEXT("Unhandled exception or API failed\n"));	
	  RELEASE( pTempRenderTarget );
	  RELEASE( pBackBuffer );
	  RELEASE( pSwapChain );
  }

	//Make sure the test doesn't fail because of the previous errors
	GetLastError();

	//return false so that the framework doesn't present
	return false;
}


void CPresentDoNotWait::onDestroy()
{
	RELEASE( m_pTexture );
	RELEASE( m_pVB );
}

void CPresentDoNotWait::onCreate()
{
	try
	{
		Check_HRFail( m_pSrcDevice->CreateTexture(	256,
													256,
													0,
													0,
													FMT_A8R8G8B8,
													POOL_DEFAULT,
													&m_pTexture
												), 
						"CreateTexture" );

		Check_HRFail( m_pSrcDevice->D3DXFillTexture( m_pTexture, (LPD3DXFILL2D)FillTexture9, NULL ), "D3DXFillTexture" );

		Check_HRFail( m_pSrcDevice->CreateVertexBuffer( Vertixes * xDivisions * yDivisions * sizeof FVFVERTEX,
														USAGE_DYNAMIC,
														FVFVERTEXHANDLE,
														POOL_DEFAULT,
														&m_pVB ),
						"CreateVertexBuffer" );			
		
		// copy vertixes
		// define a basic pyramid, centered at the origin.
		FVFVERTEX VertexList[Vertixes] = {
			//  x      y      z      xN        yN        zN       u     v
			{  0.5f, -0.5f, -0.5f,  0.0000f,  0.4472f, -0.8944f, 1.0f, 1.0f }, // face 1 (front)
			{ -0.5f, -0.5f, -0.5f,  0.0000f,  0.4472f, -0.8944f, 0.0f, 1.0f },
			{  0.0f,  0.5f,  0.0f,  0.0000f,  0.4472f, -0.8944f, 0.5f, 0.5f },

			{  0.5f, -0.5f,  0.5f,  0.8944f,  0.4472f,  0000.0f, 1.0f, 0.0f },  // face 2 (right)
			{  0.5f, -0.5f, -0.5f,  0.8944f,  0.4472f,  0000.0f, 1.0f, 1.0f },
			{  0.0f,  0.5f,  0.0f,  0.8944f,  0.4472f,  0000.0f, 0.5f, 0.5f },

			{ -0.5f, -0.5f,  0.5f,  0.0000f,  0.4472f,  0.8944f, 0.0f, 0.0f },  // face 3 (back)
			{  0.5f, -0.5f,  0.5f,  0.0000f,  0.4472f,  0.8944f, 1.0f, 0.0f },
			{  0.0f,  0.5f,  0.0f,  0.0000f,  0.4472f,  0.8944f, 0.5f, 0.5f },

			{ -0.5f, -0.5f, -0.5f, -0.8944f,  0.4472f,  0000.0f, 0.0f, 1.0f }, // face 4 (left)
			{ -0.5f, -0.5f,  0.5f, -0.8944f,  0.4472f,  0000.0f, 0.0f, 0.0f },
			{  0.0f,  0.5f,  0.0f, -0.8944f,  0.4472f,  0000.0f, 0.5f, 0.5f },
		
			{  0.5f, -0.5f,  0.5f,  0.0000f, -1.0000f,  0.0000f, 1.0f, 0.0f },  // face 5 (bottom)
			{ -0.5f, -0.5f,  0.5f,  0.0000f, -1.0000f,  0.0000f, 0.0f, 0.0f },
			{ -0.5f, -0.5f, -0.5f,  0.0000f, -1.0000f,  0.0000f, 0.0f, 1.0f },

			{  0.5f, -0.5f,  0.5f,  0.0000f, -1.0000f,  0.0000f, 1.0f, 0.0f },  // face 6 (bottom)
			{ -0.5f, -0.5f, -0.5f,  0.0000f, -1.0000f,  0.0000f, 0.0f, 1.0f },
			{  0.5f, -0.5f, -0.5f,  0.0000f, -1.0000f,  0.0000f, 1.0f, 1.0f },
		};

		FVFVERTEX *pVertices;
		Check_HRFail( m_pVB->Lock( 0, 0, (BYTE**)&pVertices, LOCK_DISCARD ), "Lock" );

		for( int xDivision = 0; xDivision < xDivisions; xDivision++ )
			for( int yDivision = 0; yDivision < yDivisions; yDivision++ )
				for( int iVertex = 0; iVertex < Vertixes; iVertex++ )
				{
					// blend the vertixes
					FVFVERTEX vertex = VertexList[iVertex];
					vertex.x = -0.5f + xDivision / static_cast<float>( xDivisions ) + VertexList[iVertex].x / xDivisions;
					vertex.y = -0.5f + yDivision / static_cast<float>( yDivisions ) + VertexList[iVertex].y / yDivisions;
					memcpy( pVertices, &vertex, sizeof FVFVERTEX );
					pVertices++;
				}

		Check_HRFail( m_pVB->Unlock(), "Unlock" );
	}
	catch( HRESULT hError )
	{
		WriteToLog( _T( "Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString( hError ) );
	}
}

void CPresentDoNotWait::onDraw()
{
	try
	{
		// Test the cooperative level to see if it's okay to render
		Check_HRFail( m_pSrcDevice->TestCooperativeLevel(), "TestCooperativeLevel" );

		// Begin the scene
		Check_HRFail( m_pSrcDevice->BeginScene(), "BeginScene" );

		// draw the scene
		Check_HRFail( m_pSrcDevice->SetVertexShader( FVFVERTEXHANDLE ), "SetFVF" );
		
		Check_HRFail( m_pSrcDevice->SetTexture( 0, m_pTexture ), "SetTexture" );
		Check_HRFail( m_pSrcDevice->SetStreamSource( 0, m_pVB, sizeof FVFVERTEX ), "SetStreamSource" );

		// draw xxx objects to keep the GPU busy, even with Driver Verifier on
		for( int uxSphere = -1; uxSphere <= 1; uxSphere++ )
			for( int uySphere = -1; uySphere <= 1; uySphere++ )
			{
				// World matrix
				D3DXMATRIX WorldMatrix;
				D3DXMatrixTranslation( &WorldMatrix, static_cast<float>(uxSphere), static_cast<float>(uySphere), 0.0f );
				Check_HRFail( m_pSrcDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&WorldMatrix ), "SetTransform" );

				Check_HRFail( m_pSrcDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, xDivisions * yDivisions * Vertixes / 3 ), "DrawIndexedPrimitive" );
			}

		Check_HRFail( m_pSrcDevice->SetTexture( 0, NULL ), "SetTexture" );

        // End the scene
		Check_HRFail( m_pSrcDevice->EndScene(), "EndScene" );
	}
	catch( HRESULT hError )
	{
		WriteToLog( _T( "Unhandled exception caught: 0x%x (%s)"), hError, m_pD3D->HResultToString( hError ) );
	}
}

bool CPresentDoNotWait::SetDefaultMatrices(float width, float height)
{
	PRESENT_PARAMETERS *pd3dpp = m_pSrcDevice->GetPresParams();
	if( NULL == pd3dpp || 0 == pd3dpp->uHeight )
		return false;

	D3DXMATRIX		Matrix;
	D3DXVECTOR3		from( 0.0f, 0.0f, -4.0f ), at( 0.0f, 0.0f, 0.0f ), up( 0.0f, 1.0f, 0.0f ); 
	float			fAspect = (width/height);
	float			fZn = 0.0f;
	float			fZf = 1.0f;

	// Projection matrix
	D3DXMatrixPerspectiveFovLH( &Matrix, pi/4, fAspect, fZn, fZf ); 
	if( FAILED( m_pSrcDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&Matrix ) ) )
		return false;

	// World matrix
	D3DXMatrixIdentity( &Matrix );
	if( FAILED( m_pSrcDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&Matrix ) ) )
		return false;

	// View matrix
	D3DXMatrixLookAtLH( &Matrix, &from, &at, &up );
	if( FAILED( m_pSrcDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)&Matrix ) ) )
		return false;
	
	return true;
}

bool CPresentDoNotWait::SetDefaultMaterials()
{
	ZeroMemory( &m_sMaterial,sizeof( MATERIAL ) );
	m_sMaterial.diffuse.r  = 1.0f;
	m_sMaterial.diffuse.g  = 1.0f;
	m_sMaterial.diffuse.b  = 1.0f;
	m_sMaterial.diffuse.a  = 1.0f;
	
	m_sMaterial.ambient.r  = 1.0f;
	m_sMaterial.ambient.g  = 1.0f;
	m_sMaterial.ambient.b  = 1.0f;
	m_sMaterial.ambient.a  = 1.0f;
	
	m_sMaterial.specular.r = 1.0f;
	m_sMaterial.specular.g = 1.0f;
	m_sMaterial.specular.b = 1.0f;
	m_sMaterial.specular.a = 1.0f;

	m_sMaterial.emissive.r = 0.0f;
	m_sMaterial.emissive.g = 0.0f;
	m_sMaterial.emissive.b = 0.0f;
	m_sMaterial.emissive.a = 0.0f;
	
	m_sMaterial.power      = 20.0f;
	
	if( FAILED( m_pSrcDevice->SetMaterial( &m_sMaterial ) ) )
		return false;
	
	return true;
}

bool CPresentDoNotWait::SetDefaultLights()
{
	D3DVECTOR	vLightPos;
	D3DVECTOR	vLightDir;
	
	vLightPos.x = 0.f; vLightPos.y = 2.f; vLightPos.z = -2.f;
	vLightDir.x = 0.f; vLightDir.y = -0.707f; vLightDir.z = 0.707f;

	ZeroMemory( &m_sLight,sizeof( LIGHT ) );
	m_sLight.dltType		= D3DLIGHT_SPOT;
	m_sLight.Diffuse.r		= 0.9f;
	m_sLight.Diffuse.g		= 0.7f;
	m_sLight.Diffuse.b		= 0.5f;
	m_sLight.Diffuse.a		= 1.0f;
	m_sLight.Specular.r		= 0.3f;
	m_sLight.Specular.g		= 0.5f;
	m_sLight.Specular.b		= 0.9f;
	m_sLight.Specular.a		= 1.0f;
	m_sLight.Position		= vLightPos;
	m_sLight.Direction		= vLightDir;
	m_sLight.Range			= 1000.f;
	m_sLight.Attenuation0	= 0.7f;
	m_sLight.Attenuation1	= 0.0f;
	m_sLight.Attenuation2	= 0.0f;
	m_sLight.Theta			= pi/10;
	m_sLight.Phi			= pi/2;

	if( FAILED( m_pSrcDevice->SetLight( 0,&m_sLight ) ) )
		return false;
	if( FAILED( m_pSrcDevice->LightEnable( 0,TRUE ) ) )
		return false;
	
	return true;
}

bool CPresentDoNotWait::SetDefaultRenderStates()
{
	m_pSrcDevice->SetRenderState( D3DRS_AMBIENT, RGBA_MAKE(128, 192, 255, 255 ) );
	m_pSrcDevice->SetRenderState( D3DRS_LOCALVIEWER,(DWORD)TRUE );
	m_pSrcDevice->SetRenderState( D3DRS_CLIPPING, (DWORD)TRUE );
	m_pSrcDevice->SetRenderState( D3DRS_LASTPIXEL, (DWORD)TRUE );
	m_pSrcDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, (DWORD)FALSE );
	m_pSrcDevice->SetRenderState( D3DRS_SRCBLEND, (DWORD)D3DBLEND_SRCALPHA );
	m_pSrcDevice->SetRenderState( D3DRS_DESTBLEND, (DWORD)D3DBLEND_ZERO );
	m_pSrcDevice->SetRenderState( D3DRS_LIGHTING, (DWORD)TRUE );
	m_pSrcDevice->SetRenderState( D3DRS_NORMALIZENORMALS, (DWORD)TRUE );
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pSrcDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	return true;
}


//////////////////////////////////////////////////////////////////////
// Name: IsDWMEnabled()
// Desc: returns true if DWM is enabled, false otherwise
//////////////////////////////////////////////////////////////////////
bool CPresentDoNotWait::IsDWMEnabled()
{
    HINSTANCE hInst = LoadLibraryExW( L"dwmapi.dll", NULL, 0 );
    if( hInst == NULL )
    {
        return false;
    }
    
    typedef HRESULT (WINAPI *fnDwmIsCompositionEnabled)( __out_ecount(1) BOOL* pfEnabled );
    fnDwmIsCompositionEnabled pfnDwmIsCompositionEnabled = (fnDwmIsCompositionEnabled)GetProcAddress( hInst, "DwmIsCompositionEnabled" );
    if( pfnDwmIsCompositionEnabled == NULL )
    {
        FreeLibrary( hInst );
        return false;
    }
    
    BOOL bCompositionEnabled = FALSE;
    if( FAILED( (*pfnDwmIsCompositionEnabled)( &bCompositionEnabled ) ) )
    {
        FreeLibrary( hInst );
        return false;
    }
    
    FreeLibrary( hInst );

    return ( bCompositionEnabled == TRUE ) ? true : false;
}
