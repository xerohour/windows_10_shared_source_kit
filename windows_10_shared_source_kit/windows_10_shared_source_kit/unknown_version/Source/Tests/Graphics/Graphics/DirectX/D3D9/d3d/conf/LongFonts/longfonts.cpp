#include <math.h>
#include "LongFonts.h"

CD3DWindowFramework g_App;
CLHFonts_ComposeOp g_ComposeOpTest;
CLHFonts_ComposeRects g_ComposeRectsTest;
CLHFonts_SetConvolutionMonoKernel g_SetConvolutionMonoKernelTest;
CLHFonts_ConvolutionStateBlock g_ConvolutionStateBlockTest;
CLHFonts_API g_APITest;

UINT g_uOps[] = {	D3DCOMPOSERECTS_COPY,
				D3DCOMPOSERECTS_OR,
				D3DCOMPOSERECTS_AND,
				D3DCOMPOSERECTS_NEG };
UINT g_uNumOps = sizeof(g_uOps) / sizeof(UINT);

LPTSTR g_szOpName[] = {	"COMPOSE_OP: COPY",
						"COMPOSE_OP: OR",
						"COMPOSE_OP: AND",
						"COMPOSE_OP: NEG" };

// Define printable character range
// Start at space, #32
#define CHARSTART   ' '
// Stop and end bracket, #125
#define CHARSTOP    '}'


//
// CLHFonts_Base - base functionality shared by all the longhorn fonts tests
//

CLHFonts_Base::CLHFonts_Base() :
	m_pSrcVB( NULL ),
	m_pDestVB( NULL ),
	m_pTexture( NULL ),
	m_pTexSurface( NULL ),
	m_pFontTexture( NULL ),
	m_pFontSurface( NULL ),
	m_uTextureSize( 256 ),
	m_uVBRectOffset( CHARSTART ),
	m_srcVBLength( sizeof(D3DCOMPOSERECTDESC) * 256 ),
	m_destVBLength( sizeof(D3DCOMPOSERECTDESTINATION) * 256 )
{
	SetTestType( TESTTYPE_CONF );
	if( KeySet( _T("stress") ) )
	{
		SetTestType( TESTTYPE_STRESS );
		m_Options.ModeOptions.TextureFmt = FMT_ANY;
		m_Options.ModeOptions.ZBufferFmt = FMT_ANY;
		m_Options.ModeOptions.CubeFmt = FMT_ANY;
		m_Options.ModeOptions.VolumeFmt = FMT_ANY;
		m_Options.ModeOptions.BackBufferFmt = FMT_ANY;
		m_Options.ModeOptions.MultisampType = MULTISAMPLE_ANY;
		m_Options.ModeOptions.uMaxDisplayModes = 1;
	}

	m_Options.D3DOptions.fMinDXVersion = 9.1f;
	m_Options.D3DOptions.nBackBufferWidth = 256;
	m_Options.D3DOptions.nBackBufferHeight = 256;

	m_pFramework->GetImageCmp()->SetPassRequired(.98f);

	m_bUseManagedSurfaces = KeySet( _T("Managed") );
}

CLHFonts_Base::~CLHFonts_Base()
{
}

void CLHFonts_Base::CommandLineHelp()
{
	WriteCommandLineHelp( "Required Caps: LDDM, D3DPTFILTERCAPS_CONVOLUTIONMONO" );
	WriteCommandLineHelp( "The test verifies the functionality of the ComposeRects API" );
	WriteCommandLineHelp( "2 VB created with the D3DUSAGE_TEXTAPI flag store D3DCOMPOSERECTDESC & D3DCOMPOSERECTDESTINATION arrays" );
	WriteCommandLineHelp( "Font textures are created with D3DFMT_A1 format" );
	WriteCommandLineHelp( "The font texture is alpha blended on top of an existing background" );
}

bool CLHFonts_Base::CapsCheck()
{
	for( UINT iDevice = 0; iDevice < 16; iDevice++ )
	{
		CDevice *pDevice = GetDevice( iDevice );
		if( pDevice )
		{
			// TextAPI is not available in XDDM
			CAPS deviceCaps;
			if( FAILED( pDevice->GetDeviceCaps( &deviceCaps ) ) )
			{
				WriteToLog( _T("Failed to get device caps. Test cannot continue.") );
				return false;
			}

			if( DEVICETYPE_HAL == pDevice->GetDevType() && DRIVERMODEL_LDDM != m_pD3D->GetDriverModel() )
			{
				WriteToLog( _T("XDDM Driver detected. Test skipped.") );
				return false;
			}

			if( 0 == ( deviceCaps.TextureFilterCaps & D3DPTFILTERCAPS_CONVOLUTIONMONO ) )
			{
				WriteToLog( _T("Device %d does not support required texture filter: D3DPTFILTERCAPS_CONVOLUTIONMONO"), iDevice );
				return false;
			}

			// Must support convolution mono filter
			/*if( FAILED( pDevice->SetConvolutionMonoKernel( 0, 0, NULL, NULL ) ) )
			{
				WriteToLog( _T("SetConvolutionMonoKernel failed on device %d"), iDevice );
				return false;
			}*/
		}
	}

	// Establish the maximum texture size that we're going to use with this device.
	m_uTextureSize = 256;
	
	return true;
}

bool CLHFonts_Base::CreateFontImage( UINT uSize )
{
	m_pFontImage = new CImage;
	
	if( NULL == m_pFontImage )
		return false;
	ZeroMemory( m_FontInfo, sizeof(m_FontInfo) );
	
	// Request to create a bitmap in X8R8G8B8 format.
	BITMAPINFO Info;
	ZeroMemory( &Info, sizeof(BITMAPINFO) );
	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.bmiHeader.biWidth = uSize;
	Info.bmiHeader.biHeight = -(int)uSize;
	Info.bmiHeader.biPlanes = 1;
	Info.bmiHeader.biBitCount = 32;
	Info.bmiHeader.biCompression = BI_RGB;
	
	// Create the DC and Bitmap to render fonts into. We use this instead of GetGlyphOutline
	// because it's more directly compatible with the framework's CImage/FColor format info.
	
	PDWORD pBits;
	HDC hDC = CreateCompatibleDC( NULL );
	HBITMAP hBmp = CreateDIBSection( hDC, &Info, DIB_RGB_COLORS, (PVOID *)&pBits, NULL, 0 );
	HGDIOBJ hOldBmp = ::SelectObject( hDC,hBmp );
	
	// Zero out the bitmap surface... BLACK
	ZeroMemory( pBits, sizeof(DWORD) * uSize * uSize );
	
	SetMapMode( hDC, MM_TEXT );
	SetTextColor( hDC, RGB( 255, 255, 255 ) );
	SetBkColor( hDC, 0x00000000 );
	SetTextAlign( hDC, TA_TOP );
	
	// Create a font object so we can render the font for test purposes
	UINT uMaxFontSize = 150;
	HGDIOBJ hOldFont = NULL;
	HFONT hFont = NULL;
	
	do
	{
		hFont = ::CreateFont( uMaxFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
		hOldFont = ::SelectObject( hDC, hFont );
		
		SIZE Size;
		::GetTextExtentPoint32( hDC, "W", 1, &Size );

		// round Size.cx up to be divisible by 8
		if( Size.cx % 8 > 0 )
			Size.cx += 8 - ( Size.cx % 8 );
		
		// Determine if a sufficient number of these characters will fit on the image. If not, reduce it in size.
		UINT uRows = uSize / Size.cy;
		UINT uChars = (uSize / Size.cx) * uRows;
		
		if( uChars >= ( CHARSTOP - CHARSTART ) )
			break;
		
		--uMaxFontSize;
		
		::SelectObject( hDC, hOldFont );
		::DeleteObject( hFont );
	}
	while( 1 );
	
	// Render the fonts
	UINT uX = 0;
	UINT uY = 0;
	TCHAR szChar[2]={ 0, 0 };
	UINT uHeight = 0;
	
	for( UINT uCount = CHARSTART; uCount <= CHARSTOP; ++uCount )
	{
		SIZE Size;
		
		szChar[0] = (char) uCount;
		GetTextExtentPoint32( hDC, szChar, 1, &Size );
		
		// Ensure the font fits width-wise
		if( uX + Size.cx >= uSize )
		{
			uX = 0;
			uY += uHeight;
		}
		
		// Record the maximum text height
		if( uHeight < Size.cy )
			uHeight = Size.cy;
		
		// Record the rect info
		m_FontInfo[uCount].uX1 = uX;
		m_FontInfo[uCount].uY1 = uY;
		m_FontInfo[uCount].uX2 = uX + Size.cx;
		m_FontInfo[uCount].uY2 = uY + Size.cy;
		
		// Record the UV info
		m_FontInfo[uCount].fU1 = (float) uX / (float) uSize;
		m_FontInfo[uCount].fV1 = (float) uY / (float) uSize;
		m_FontInfo[uCount].fU2 = (float) (uX + Size.cx) / (float) uSize;
		m_FontInfo[uCount].fV2 = (float) (uY + Size.cy) / (float) uSize;
		
		// Record the stepping info
		m_FontInfo[uCount].uWidth = Size.cx;
		m_FontInfo[uCount].uHeight = Size.cy;
		
		// Actually draw the text onto the bitmap
		ExtTextOut( hDC, uX, uY, 0, NULL, szChar, 1, NULL );
		
		// Move to the next font location.
		uX += Size.cx;

		// round uX up to be divisible by 8
		if( uX % 8 > 0 )
			uX += 8 - ( uX % 8 );
	}
	
	// Allocate the color set
	FColor *pColors = new FColor[ uSize * uSize ];
	if( NULL != pColors )
	{
		// Copy colors from the bitmap into the color list to set to the CImage. Use the
		// red component (the bitmap is in BGR format) to set the alpha.
		for( UINT uCount = 0; uCount < ( uSize * uSize ); ++uCount )
			pColors[uCount] = FColor( pBits[uCount] | (pBits[uCount] & 0xFF ) << 24 );
		
		// Copy the data into the frameworks image class to be loaded into a surface later.
		m_pFontImage->SetData( uSize, uSize, pColors );
		
		// Release the color set.
		delete[] pColors;
	}
	else
	{
		delete m_pFontImage;
		m_pFontImage = NULL;
	}
	
	// Delete the GDI stuff. pBits will automatically be deleted when the bitmap is deleted
	::DeleteObject( hBmp );
	::SelectObject( hDC, hOldFont );
	::SelectObject( hDC, hOldBmp );
	::DeleteDC( hDC );
	::DeleteObject( hFont );
	
	return( NULL != m_pFontImage ) ? true : false;
}

bool CLHFonts_Base::Setup()
{
	CreateFontImage( m_uTextureSize );

	D3DPOOL resPool = D3DPOOL_MANAGED;
	DWORD resUsage = 0;
	if( !m_bUseManagedSurfaces )
	{
		resPool = D3DPOOL_DEFAULT;
		resUsage = D3DUSAGE_DYNAMIC;
	}

	bool bResult = true;
	bResult =  CreateVertexBuffer( m_srcVBLength, D3DUSAGE_TEXTAPI | resUsage, 0, resPool, &m_pSrcVB );
	if( bResult )
		bResult = CreateVertexBuffer( m_destVBLength, D3DUSAGE_TEXTAPI | resUsage, 0, resPool, &m_pDestVB );
	
	// Create the textures to render with
	if( bResult )
		bResult = CreateTexture( m_uTextureSize, m_uTextureSize, 1, D3DUSAGE_DYNAMIC, FMT_A1, D3DPOOL_DEFAULT, &m_pTexture );
	
	// Extract the top level surfaces from the textures.
	if( bResult )
		bResult = m_pTexture->GetSurfaceLevel( 0, &m_pTexSurface );

	// Create source surfaces
	// Cannot create an offscreen surface yet. Create the textures and extract a surface.
	// hResult = CreateOffscreenPlainSurface(m_uTextureSize,m_uTextureSize,FMT_A1,D3DPOOL_DEFAULT,&m_FontSurface);

	if( bResult )
		bResult = CreateTexture( m_uTextureSize, m_uTextureSize, 1, D3DUSAGE_TEXTAPI | resUsage, FMT_A1, resPool, &m_pFontTexture );
	if( bResult )
		bResult = m_pFontTexture->GetSurfaceLevel( 0, &m_pFontSurface );
	if( bResult )
		for( UINT nDevice = 0; nDevice < 16; nDevice++ )
		{
			CSurface *pSurface = NULL;
			if( m_pFontSurface->GetSurface( &pSurface, nDevice ) )
			{
				if( FAILED( pSurface->LoadImage( m_pFontImage  ) ) )
					bResult = false;
			}
			RELEASE( pSurface );
		}

	// Build the source font information into the vertex buffer
	D3DCOMPOSERECTDESC *pSrcRect = NULL;    
	if( bResult )
		bResult = m_pSrcVB->Lock( 0, 0, (PVOID *)&pSrcRect, 0 );
	if( bResult )
	{
		memset( pSrcRect, 0xcd, m_srcVBLength );
		for( UINT uCount = 0; uCount <= CHARSTOP - CHARSTART; ++uCount )
		{
			pSrcRect[m_uVBRectOffset + uCount].X = (USHORT)m_FontInfo[CHARSTART + uCount].uX1;
			pSrcRect[m_uVBRectOffset + uCount].Y = (USHORT)m_FontInfo[CHARSTART + uCount].uY1;
			pSrcRect[m_uVBRectOffset + uCount].Width = (USHORT)m_FontInfo[CHARSTART + uCount].uWidth;
			pSrcRect[m_uVBRectOffset + uCount].Height = (USHORT)m_FontInfo[CHARSTART + uCount].uHeight;
		}
	}
	if( bResult )
		bResult = m_pSrcVB->Unlock();

	return bResult;
}

void CLHFonts_Base::Cleanup()
{
	delete m_pFontImage;
	m_pFontImage = NULL;

	RELEASE( m_pSrcVB );
	RELEASE( m_pDestVB );
	RELEASE( m_pTexture );
	RELEASE( m_pTexSurface );
	RELEASE( m_pFontTexture );
	RELEASE( m_pFontSurface );
}

bool CLHFonts_Base::TestTerminate()
{
	Cleanup();
	
	return true;
}

UINT CLHFonts_Base::TestInitialize()
{
    // Tests:
    
    //  ComposeRects: Sequential characters, cycle op-codes
    //  ComposeRects: Random characters, cycle op-codes

    //  Texture filtering:
    //      Scale textures from up/down using ConvolutionMono
    //      Filter ranges from 1x1 to 8x8
    //      Identity - No settings is covered in first 1 tests
    //      Sin values for all settings
    //      Weighted values from original sample

	ProcessMessages();
	
	return D3DTESTINIT_RUN;
}

bool CLHFonts_Base::ExecuteTest( UINT uTest )
{
	if( !ExecuteTestOnDevice( uTest ) )
		return false;
	
	ProcessMessages();
	
	return true;
}

void CLHFonts_Base::DrawText( int uX, int uY, LPTSTR szText, UINT uOp,int nOffsetX, int nOffsetY )
{
	// Take the given device/vb and compose the vertex buffers to get
	// render the text. Then use ComposeRects() to update the information.
	
	D3DCOMPOSERECTDESTINATION *pDestRect;
	UINT uMax = lstrlen( szText );
	
	// Lock the destination buffer
	ASSERT( NULL != m_pDestVB );
	if( !m_pDestVB->Lock( 0, 0, (PVOID *)&pDestRect, 0 ) )
	{
		Fail();
		WriteToLog( _T("Unable to lock the destination buffer for drawing text") );
		return;
	}

	memset( pDestRect, 0xfe, m_destVBLength );

	for( UINT uCount = 0; uCount < uMax; ++uCount )
	{
		if( uX + m_FontInfo[*szText].uWidth > m_uTextureSize )
		{
			uX = 0;
			uY += m_FontInfo[*szText].uHeight;
			if( uY + m_FontInfo[*szText].uHeight > m_uTextureSize )
			{
				uMax = uCount;
				break;
			}
		}
		
		pDestRect->SrcRectIndex = *szText - CHARSTART + m_uVBRectOffset;  // Quite simply, it's an index into the text buffer.
		pDestRect->X = (USHORT)uX;
		pDestRect->Y = (USHORT)uY;
		
		++pDestRect;
		
		uX += m_FontInfo[*szText].uWidth;
		++szText;
	}
	m_pDestVB->Unlock();
	
	if( 0 == uMax )
		return;

	// Compose the rects ->move this information onto the target buffer
	if( !ComposeRects(	m_pFontSurface,
						m_pTexSurface,
						m_pSrcVB,
						uMax,
						m_pDestVB,
						uOp,
						nOffsetX,
						nOffsetY ) )
	{
		Fail();
		WriteToLog( _T("ComposeRects failed.") );
		return;
	}
}

void CLHFonts_Base::DisplayImage( UINT uColor, float fScale )
{
	for( UINT i = 0; i < m_uDeviceList; i++ )
	{
		if( SkipDevice( i, CD3D_ALL ) )
			continue;
		DisplayDeviceImage( i, uColor, fScale );
	}
}

void CLHFonts_Base::DisplayDeviceImage( UINT iDevice, UINT uColor, float fScale )
{
	PRESENT_PARAMETERS *pD = m_DeviceList[iDevice]->GetPresParams();
	float fWidth = ( pD->uWidth - 1 ) * fScale;
	float fHeight = ( pD->uHeight - 1 ) * fScale;
	
	DVERTEX Vertices[6] = { {0.0f,      0.0f,       0.0f, 0.5f, uColor, 0.0f, 0.0f},
							{fWidth,    0.0f,       0.0f, 0.5f, uColor, 1.0f, 0.0f},
							{0.0f,      fHeight,    0.0f, 0.5f, uColor, 0.0f, 1.0f},
							{fWidth,    0.0f,       0.0f, 0.5f, uColor, 1.0f, 0.0f},
							{fWidth,    fHeight,    0.0f, 0.5f, uColor, 1.0f, 1.0f},
							{0.0f,      fHeight,    0.0f, 0.5f, uColor, 0.0f, 1.0f}};
	
	// Render to the back buffer. First the source device
	bool bResult = true;
	bResult = SUCCEEDED( m_DeviceList[iDevice]->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) );
	bResult &= SUCCEEDED( m_DeviceList[iDevice]->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA ) );
	bResult &= SUCCEEDED( m_DeviceList[iDevice]->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ) );
	
	CTexture *pTexture = NULL;
	bResult &= m_pTexture->GetTexture( &pTexture, iDevice );
	if( bResult )
		bResult &= SUCCEEDED( m_DeviceList[iDevice]->SetTexture( 0, pTexture ) );
	bResult &= SUCCEEDED( m_DeviceList[iDevice]->SetVertexShader( (CVertexShader*) NULL ) );
	bResult &= SUCCEEDED( m_DeviceList[iDevice]->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 ) );
	bResult &= SUCCEEDED( m_DeviceList[iDevice]->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, Vertices, sizeof(DVERTEX) ) );

	if( !bResult )
	{
		Fail();
		WriteToLog( _T("Rendering Error.") );
	}

	RELEASE( pTexture );
}

void CLHFonts_Base::PrepareDestinationSurface( UINT uOp )
{
	RECT rRect={ 0, 0, m_uTextureSize, m_uTextureSize };
	LOCKED_RECT Lock;
	
	for( UINT nDevice = 0; nDevice < 16; nDevice++ )
	{
		CSurface *pSurface = NULL;
		if( m_pTexSurface->GetSurface( &pSurface, nDevice ) )
		{
			if( FAILED( pSurface->LockRect( &Lock, &rRect, D3DLOCK_NOSYSLOCK ) ) )
			{
				Fail();
				WriteToLog( _T("Unable to lock the destination surface for writing prep") );
				break;
			}
	
			UINT uWidth = m_uTextureSize / ( 8 * sizeof(DWORD) );
			for( UINT uY = 0; uY < m_uTextureSize; ++uY )
			{
				DWORD dwColor;
				PDWORD  pdwBits = (PDWORD)( (PBYTE)Lock.pBits + ( uY * Lock.iPitch ) );
		
				switch(uOp)
				{
				case 0:
					dwColor = 0;
					break;
				case D3DCOMPOSERECTS_COPY:
				case D3DCOMPOSERECTS_NEG:
				case D3DCOMPOSERECTS_AND:
				case D3DCOMPOSERECTS_OR:
					dwColor = ( (uY % 2 )==1 ) ? 0 : 0xFFFFFFFF;
					break;
				}
				
				for( UINT uX = 0; uX < uWidth; ++uX )
				{
					*pdwBits = dwColor;
					++pdwBits;
				}
			}
			pSurface->UnlockRect();

			RELEASE( pSurface );
		}
	}
}


//
// CLHFonts_ComposeOp - cycles on D3DCOMPOSEOP with random text
//

CLHFonts_ComposeOp::CLHFonts_ComposeOp()
{
	m_szTestName="ComposeOp";
	m_szCommandKey="ComposeOp";

	m_uVBRectOffset = 0;
	m_srcVBLength = sizeof(D3DCOMPOSERECTDESC) * ( CHARSTOP - CHARSTART + 1 );
}

void CLHFonts_ComposeOp::CommandLineHelp()
{
	CLHFonts_Base::CommandLineHelp();
	WriteCommandLineHelp( "ComposeOp uses several text buffers to verify the composition" );
	WriteCommandLineHelp( "Cycling through D3DCOMPOSERECTS_COPY, D3DCOMPOSERECTS_OR, D3DCOMPOSERECTS_AND, D3DCOMPOSERECTS_NEG" );
}

UINT CLHFonts_ComposeOp::TestInitialize()
{
	UINT uResult = CLHFonts_Base::TestInitialize();
	SetTestRange( 1,32 );
	return uResult;
}

bool CLHFonts_ComposeOp::ExecuteTestOnDevice( UINT uTest )
{
	BeginTestCase( "CLHFonts_ComposeOp", uTest );
	
	static LPCTSTR  szText[]={	{" Microsoft Longhorn Team"}, // Intial random text. Touches 1'st character
								{"Mars attacks! :}"}, // Touches last character
								{"Ripe tomatoes are best"},
								{"Direct3D swizzles your bits"},
								{"Whales sing louder"},
								{"Pizza Pizza, order Pizza"},
								{"V(t) = Vt + 0.5 * At"}, // Covers non-text characters
								{"Robots step with style"} };
	
	UINT uTextNum = ( uTest - 1 ) % ( sizeof(szText) / sizeof(LPTSTR) );
	UINT uOp = ( uTest - 1 ) / ( sizeof(szText) / sizeof(LPTSTR) );
	TCHAR szBuiltText[128];
	wsprintf( szBuiltText,"%s: %s", szText[uTextNum], g_szOpName[uOp] );
	WriteStatus( "Test", szBuiltText );

	bool bResult = true;
	bResult = BeginScene();
	if( bResult )
		bResult = Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,0xFF000080, 1.0f, 0 );
	
	if( bResult )
	{
		PrepareDestinationSurface( g_uOps[uOp] );
		DrawText( 10, 10, szBuiltText, g_uOps[uOp], 0, 0 );
		static DWORD dwColor[]={ 0xFFFFFFFF, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
		UINT uColor = uTest % ( sizeof(dwColor) / sizeof(DWORD) );
		DisplayImage( dwColor[uColor], 1.0f );
	}
	
	if( bResult )
		EndScene();

	if( !bResult )
	{
		Fail();
		WriteToLog( _T("Rendering Error.") );
	}
	
	return bResult;
}


//
// CLHFonts_ComposeRects - main ComposeRects test, cycles on all params except D3DCOMPOSEOP with random text
//

CLHFonts_ComposeRects::CLHFonts_ComposeRects()
{
	m_szTestName="ComposeRects";
	m_szCommandKey="ComposeRects";
}

void CLHFonts_ComposeRects::CommandLineHelp()
{
	CLHFonts_Base::CommandLineHelp();
	WriteCommandLineHelp( "ComposeRects cycles through all the parameters except for D3DCOMPOSEOP (D3DCOMPOSERECTS_COPY is used)" );
	WriteCommandLineHelp( "including: x offset, y offset, x position, y position" );
}

UINT CLHFonts_ComposeRects::TestInitialize()
{
	UINT uResult = CLHFonts_Base::TestInitialize();
	SetTestRange( 1, 19 * 7 );

	// it's ok for this test to render black window
	// m_pFramework->GetImageCmp()->SetIncClearColor( true );

	return uResult;
}

bool CLHFonts_ComposeRects::ExecuteTestOnDevice( UINT uTest )
{
	int X[] = { 0, -10, 10, 0, 0, 10, -10 };
	int Y[] = { 0, 0, 0, -10, 10, -10, 10 };
	int OffsetX[] = { -100, -50, 0, 50, 100, 250, 0, 0, 0, 0, 0, -20, 50, 100, 250, 250, 100, 50, -20 };
	int OffsetY[] = { 0, 0, 0, 0, 0, 0, -100, -50, 0, 50, 100, 250, -20, 50, 100, 250, -20, 50, 100, 250 };
	TCHAR szText[80];
	
	int nPos = ( uTest - 1 ) / ( sizeof(OffsetX) / sizeof(int) );
	int nOffsetIndex = ( uTest - 1 ) - ( nPos * ( sizeof(OffsetX) / sizeof(int) ) );
	
	sprintf( szText,"Pos: %d,%d Off: %d, %d", X[nPos], Y[nPos], OffsetX[nOffsetIndex], OffsetY[nOffsetIndex] );
	
	BeginTestCase( szText );
	
	bool bResult = true;
	bResult = BeginScene();
	if( bResult )
		bResult = Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF000080, 1.0f, 0 );
	
	if( bResult )
	{
		PrepareDestinationSurface( D3DCOMPOSERECTS_COPY );
		DrawText( X[nPos], Y[nPos], szText, D3DCOMPOSERECTS_COPY, OffsetX[nOffsetIndex], OffsetY[nOffsetIndex] );
		DisplayImage( 0xFFFFFFFF, 1.0f );
	}
	
	if( bResult )
		bResult = EndScene();

	if( !bResult )
	{
		Fail();
		WriteToLog( _T("Rendering Error.") );
	}
	
	return true;
}


//
// CLHFonts_SetConvolutionMonoKernel - main test for SetConvolutionKernelMode API
//

CLHFonts_SetConvolutionMonoKernel::CLHFonts_SetConvolutionMonoKernel()
{
	m_szTestName="SetConvolutionMonoKernel";
	m_szCommandKey="SetConvolutionMonoKernel";
}

void CLHFonts_SetConvolutionMonoKernel::CommandLineHelp()
{
	CLHFonts_Base::CommandLineHelp();
	WriteCommandLineHelp( "SetConvolutionMonoKernel cycles through the parameters for this API" );
	WriteCommandLineHelp( "Width & Height from 1 to 7" );
	WriteCommandLineHelp( "The weight arrays must be NULL" );
}

UINT CLHFonts_SetConvolutionMonoKernel::TestInitialize()
{
	UINT uResult = CLHFonts_Base::TestInitialize();
	
	SetTestRange( 1, 49 );
	
	return uResult;
}

bool CLHFonts_SetConvolutionMonoKernel::ExecuteTestOnDevice( UINT uTest )
{
	UINT Width = ( ( uTest - 1 ) / 7 ) + 1;
	UINT Height = ( ( uTest - 1 )% 7 ) + 1;

	char szText[80];
	sprintf/*_s*/( szText, /*80,*/ "SetConvolutionMonoKernel( Width = %d, Height = %d )", Width, Height );
	BeginTestCase( szText, uTest );
	
	bool bResult = true;
	bResult = BeginScene();
	if( bResult )
		bResult = Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF000080, 1.0f, 0 );
	
	// Compose the rects
	if( bResult )
	{
		PrepareDestinationSurface( 0 );
		for( UINT XYPos = 0; XYPos < m_uTextureSize; XYPos += 24 )
		{
			sprintf/*_s*/( szText, /*80,*/ "Microsoft Direct3D( %d,%d )", XYPos, XYPos );
			DrawText( XYPos, XYPos, szText, g_uOps[0], 0, 0 );
		}
	}
	
	/*
	// Establish the filter model
	float fSin[D3DCONVOLUTIONMONO_MAXWIDTH];
	float fPI = -3.1415927f;
	float fStep = ( 3.1415927f * 2.0f ) / D3DCONVOLUTIONMONO_MAXWIDTH;
	fPI += (uTest - 32) * fStep;
	float fScale = 1.0f / ( (float)uTest / D3DCONVOLUTIONMONO_MAXWIDTH );
	
	for( UINT uX = 0; uX < D3DCONVOLUTIONMONO_MAXWIDTH; ++uX )
	{
		fSin[uX] = sinf(fPI) * fScale;
		fPI += fStep;
	}*/
	
	// Do the filtering
	bResult |= SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_CONVOLUTIONMONO );
	bResult |= SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_CONVOLUTIONMONO );
	
	bResult |= SetConvolutionMonoKernel( Width, Height, NULL, NULL );
	
	if( bResult )
		DisplayImage( 0xFFFFFFFF, 1.0f );
	
	if( bResult )
		bResult = EndScene();

	if( !bResult )
	{
		Fail();
		WriteToLog( _T("Rendering Error.") );
	}
	
	return true;
}


//
// CLHFonts_ConvolutionStateBlock - test that SetConvolutionMonoKernel parameters get saved in the state block
//

CLHFonts_ConvolutionStateBlock::CLHFonts_ConvolutionStateBlock()
{
	m_szTestName="ConvolutionStateBlock";
	m_szCommandKey="ConvolutionStateBlock";
}

void CLHFonts_ConvolutionStateBlock::CommandLineHelp()
{
	CLHFonts_Base::CommandLineHelp();
	WriteCommandLineHelp( "ConvolutionStateBlock verifies that SetConvolutionMonoKernel is stored in the state blocks" );
	WriteCommandLineHelp( "Except for vertex state blocks" );
	WriteCommandLineHelp( "The test cycles Widht & Height parameters of the API" );
}

UINT CLHFonts_ConvolutionStateBlock::TestInitialize()
{
	UINT uResult = CLHFonts_Base::TestInitialize();
	
	SetTestRange( 1, 3 * 48 + 1 );
	
	return uResult;
}

bool CLHFonts_ConvolutionStateBlock::ExecuteTestOnDevice( UINT uTest )
{
	UINT Width = ( ( ( uTest - 1 ) / 3 ) / 7 ) + 1;
	UINT Height = ( ( ( uTest - 1 ) / 3 ) % 7 ) + 1;
	UINT SBType = ( uTest - 1 ) % 3;

	D3DSTATEBLOCKTYPE stateBlockType[3] = { D3DSBT_ALL, D3DSBT_PIXELSTATE, D3DSBT_VERTEXSTATE };

	char szText[80];
	sprintf/*_s*/( szText, /*80,*/ "ConvolutionStateBlock( SB = %d, Width = %d, Height = %d )", stateBlockType[SBType], Width, Height );
	BeginTestCase( szText, uTest );

	bool bResult = true;
	HRESULT hResult;
	UINT_PTR pSB = 0;
	bResult = (D3D_OK == ( hResult = m_pSrcDevice->CreateStateBlock( stateBlockType[SBType], &pSB ) ) );
	
	bResult &= BeginScene();
	if( bResult )
		bResult = Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF000080, 1.0f, 0 );
	
	// Compose the rects
	if( bResult )
	{
		PrepareDestinationSurface( 0 );
		for( UINT XYPos = 0; XYPos < m_uTextureSize; XYPos += 24 )
		{
			sprintf/*_s*/( szText, /*80,*/ "Microsoft Direct3D( %d,%d )", XYPos, XYPos );
			DrawText( XYPos, XYPos, szText, g_uOps[0], 0, 0 );
		}
	}
	
	// Do the filtering
	bResult &= SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_CONVOLUTIONMONO );
	bResult &= SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_CONVOLUTIONMONO );
	
	bResult &= SetConvolutionMonoKernel( Width, Height, NULL, NULL );

	// capture the state block on the Src device
	bResult &= D3D_OK == m_pSrcDevice->CaptureStateBlock( pSB );
	
	if( bResult )
	{
		// if it's a vertex state block, adjust the convolution
		if( D3DSBT_VERTEXSTATE == stateBlockType[SBType] )
			bResult &= D3D_OK == m_pRefDevice->SetConvolutionMonoKernel( ( Width + 3 ) % 7 + 1, ( Height + 3 ) % 7 + 1, NULL, NULL );

		// Display the reference image
		DisplayDeviceImage( 1, 0xFFFFFFFF, 1.0f );

		// alter the current state
		bResult &= D3D_OK == m_pSrcDevice->SetConvolutionMonoKernel( ( Width + 3 ) % 7 + 1, ( Height + 3 ) % 7 + 1, NULL, NULL );

		// apply the state block
		bResult &= D3D_OK == m_pSrcDevice->ApplyStateBlock( pSB );

		// Display the source image
		// The previous call to SetConvolutionMonoKernel should have been overwritten if it's a pixel state block
		DisplayDeviceImage( 0, 0xFFFFFFFF, 1.0f );
	}

	m_pSrcDevice->DeleteStateBlock( pSB );
	
	if( bResult )
		bResult = EndScene();

	if( !bResult )
	{
		Fail();
		WriteToLog( _T("Rendering Error.") );
	}
	
	return true;
}


//
// CLHFonts_API - API validation
//

CLHFonts_API::CLHFonts_API()
{
	m_szTestName="API";
	m_szCommandKey="API";
	
	SetTestType( TESTTYPE_API );
	m_Options.D3DOptions.fMinDXVersion = 9.1f;
}

UINT CLHFonts_API::TestInitialize()
{
	UINT uResult = CLHFonts_Base::TestInitialize();
	
	SetTestRange( 1, 15 );
	
	return uResult;
}

bool CLHFonts_API::ExecuteTestOnDevice( UINT uTest )
{
	HRESULT hr = S_OK;
	LPTEXTURE pTextTexture;
	LPSURFACE pTextSurface;

	switch( uTest )
	{
	case 1:
		{
			BeginTestCase( "Create D3DFMT_A1 texture of 2K x 2K", uTest );
			hr = m_pSrcDevice->CreateTexture( 2048, 2048, 1, D3DUSAGE_TEXTAPI, FMT_A1, D3DPOOL_DEFAULT, &pTextTexture );

			if( FAILED( hr ) )
			{
				Fail();
				WriteToLog( _T("Failed to create 2K x 2K D3DFMT_A1 texture") );
			}
			else
			{
				RELEASE( pTextTexture );
			}

			break;
		}
	case 2:
		{
			BeginTestCase( "Create D3DFMT_A1 texture greater than 2K x 2K", uTest );
			hr = m_pSrcDevice->CreateTexture( 8193, 2048, 1, D3DUSAGE_TEXTAPI, FMT_A1, D3DPOOL_DEFAULT, &pTextTexture );

			if( SUCCEEDED( hr ) )
			{
				Fail();
				WriteToLog( _T("Succeeded to create greater than 2K x 2K D3DFMT_A1 texture, expecting failure") );
				RELEASE( pTextTexture );
			}

			break;
		}
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:	
		{
			BeginTestCase( "ColorFill D3DFMT_A1 textures", uTest );
			hr = m_pSrcDevice->CreateTexture( 256, 256, 1, D3DUSAGE_TEXTAPI | D3DUSAGE_DYNAMIC, FMT_A1, D3DPOOL_DEFAULT, &pTextTexture );
			
			if( FAILED( hr ) )
			{
				Fail();
				WriteToLog( _T("Failed to create D3DFMT_A1 texture") );
			}
			else
			{
				if( FAILED( pTextTexture->GetSurfaceLevel( 0, &pTextSurface ) ) )
				{
					Fail();
					WriteToLog( _T("Failed to get the text surface") );
				}
				else
				{
					LOCKED_RECT lockedRect;
					if( FAILED( pTextSurface->LockRect( &lockedRect, NULL, D3DLOCK_DISCARD ) ) )
					{
						Fail();
						WriteToLog( _T("Failed to lock the surface" ) );
					}
					else
					{
						for( UINT y = 0; y < 256; y++ )
						{
							BYTE *pBits = (BYTE*)lockedRect.pBits + y * lockedRect.iPitch;
							memset( (LPVOID)pBits, 0xdeadbeef, 256 / 8 );
						}
						pTextSurface->UnlockRect();

						// do a color fill
						D3DCOLOR d3dColor[] = { D3DCOLOR( D3DCOLOR_RGBA( 0xFF, 0xFF, 0xFF, 0xFF ) ),
												D3DCOLOR( D3DCOLOR_RGBA( 0xFF, 0x00, 0x00, 0x00 ) ),
												D3DCOLOR( D3DCOLOR_RGBA( 0x00, 0xFF, 0xFF, 0xFF ) ),
												D3DCOLOR( D3DCOLOR_RGBA( 0x7F, 0xFF, 0xFF, 0xFF ) ),
												D3DCOLOR( D3DCOLOR_RGBA( 0x80, 0x70, 0x60, 0x50 ) ),
												D3DCOLOR( D3DCOLOR_RGBA( 0x00, 0xFF, 0xFF, 0xFF ) ),
												D3DCOLOR( D3DCOLOR_RGBA( 0x00, 0x00, 0x00, 0x00 ) ) };
						if( FAILED( m_pSrcDevice->ColorFill( pTextSurface, NULL, d3dColor[uTest - 3] ) ) )
						{
							Fail();
							WriteToLog( _T("ColorFill failed" ) );
						}
						else
						{
							if( FAILED( pTextSurface->LockRect( &lockedRect, NULL, 0 ) ) )
							{
								Fail();
								WriteToLog( _T("Failed to lock the surface" ) );
							}
							else
							{
								DWORD dwFillColor = ( d3dColor[uTest - 3] & 0x00FF0000 ) > 0x007F0000? 0xFFFFFFFF : 0x00000000;
								UINT uNumErrors = 0;
								for( UINT y = 0; y < 256; y++ )
								{
									DWORD *pBits = (DWORD*)((BYTE*)lockedRect.pBits + y * lockedRect.iPitch);
									for( UINT x = 0; x < 256 / 32; x++ )
									{
										if( *(pBits + x) != dwFillColor && ++uNumErrors < 10 )
										{
											Fail();
											WriteToLog( _T("Expected FillColor: %d, Actual: %d"), dwFillColor, *(pBits + x) );
										}
									}
								}
								pTextSurface->UnlockRect();
							}
						}
					}

					RELEASE( pTextSurface );
				}

				RELEASE( pTextTexture );
			}
			break;
		}
	case 10:
		{
			BeginTestCase( "Create 2 x 2 D3DFMT_A1 texture", uTest );
			hr = m_pSrcDevice->CreateTexture( 2, 2, 1, D3DUSAGE_TEXTAPI | D3DUSAGE_DYNAMIC, FMT_A1, D3DPOOL_DEFAULT, &pTextTexture );
			if( SUCCEEDED( hr ) )
			{
				Fail();
				WriteToLog( _T("Succeeded to create 2 x 2 D3DFMT_A1 texture, expecting failure") );
				RELEASE( pTextTexture );
			}

			break;
		}
	case 11:
		{
			BeginTestCase( "Create D3DFMT_A1 texture with mip levels", uTest );
			hr = m_pSrcDevice->CreateTexture( 256, 256, 8, D3DUSAGE_TEXTAPI, FMT_A1, D3DPOOL_DEFAULT, &pTextTexture );

			if( SUCCEEDED( hr ) )
			{
				Fail();
				WriteToLog( _T("Succeeded to create D3DFMT_A1 texture with mip levels, expecting failure") );
				RELEASE( pTextTexture );
			}
			break;
		}
	case 12:
		{
			BeginTestCase( "Create D3DFMT_A1 cube texture", uTest );
			LPCUBETEXTURE pCubeTextTexture;
			hr = m_pSrcDevice->CreateCubeTexture( 256, 1, D3DUSAGE_TEXTAPI, FMT_A1, D3DPOOL_DEFAULT, &pCubeTextTexture );

			if( SUCCEEDED( hr ) )
			{
				Fail();
				WriteToLog( _T("Succeeded to create D3DFMT_A1 cube texture, expecting failure") );
				RELEASE( pCubeTextTexture );
			}

			break;
		}
	case 13:
		{
			BeginTestCase( "Create D3DFMT_A1 volume texture", uTest );
			LPVOLUMETEXTURE pVolumeTextTexture;
			hr = m_pSrcDevice->CreateVolumeTexture( 256, 256, 4, 1, D3DUSAGE_TEXTAPI, FMT_A1, D3DPOOL_DEFAULT, &pVolumeTextTexture );

			if( SUCCEEDED( hr ) )
			{
				Fail();
				WriteToLog( _T("Succeeded to create D3DFMT_A1 volume texture, expecting failure") );
				RELEASE( pVolumeTextTexture );
			}

			break;
		}
	case 14:
	case 15:
		{
			BeginTestCase( "ColorFill D3DFMT_A1 textures with rect size not multiple of 8", uTest );
			hr = m_pSrcDevice->CreateTexture( 256, 256, 1, D3DUSAGE_TEXTAPI, FMT_A1, D3DPOOL_DEFAULT, &pTextTexture );
			
			if( FAILED( hr ) )
			{
				Fail();
				WriteToLog( _T("Failed to create D3DFMT_A1 texture") );
			}
			else
			{
				if( FAILED( pTextTexture->GetSurfaceLevel( 0, &pTextSurface ) ) )
				{
					Fail();
					WriteToLog( _T("Failed to get the text surface") );
				}
				else
				{
					RECT colorRect[] = { {0, 0, 20, 16}, {20, 0, 40, 16} };
					if( SUCCEEDED( m_pSrcDevice->ColorFill( pTextSurface, &(colorRect[uTest-14]), D3DCOLOR( D3DCOLOR_RGBA( 0xFF, 0xFF, 0xFF, 0xFF ) ) ) ) )
					{
						Fail();
						WriteToLog( _T("ColorFill succeeded for RECT with invalid size, expected to fail" ) );
					}

					LOCKED_RECT lockedRect;
					if( SUCCEEDED( pTextSurface->LockRect( &lockedRect, &(colorRect[uTest-14]), D3DLOCK_DISCARD ) ) )
					{
						Fail();
						WriteToLog( _T("Lock succeeded for RECT with invalid size, expected to fail" ) );
					}
					else
						pTextSurface->UnlockRect();

					RELEASE( pTextSurface );
				}

				RELEASE( pTextTexture );
			}

			break;
		}
	default:
		return false;
	}

	GetLastError();

	return true;
}


/*
//-----------------------------------------------------------------------------

void ComputeTentWeights(UINT nSamples, float* pWeights)
{
    for (int i=0; i < (int)nSamples/2; i++)
    {
        pWeights[i] = pWeights[nSamples - i - 1] = (float)(i + 1);
    }
    if ((nSamples & 1))
    {
        pWeights[i] = (float)(i + 1);
    }
}

//-----------------------------------------------------------------------------

void TentFilter(UINT nSamplesX, UINT nSamplesY, float* pWeightsX, float* pWeightsY )
{
    float WeightsX[16];
    float WeightsY[16];
    
    ComputeTentWeights(nSamplesX, WeightsX);
    ComputeTentWeights(nSamplesY, WeightsY);
    
    float TotalWeightY = 0.0f;
    for (UINT y=0; y < nSamplesY; y++)
    {
        TotalWeightY += WeightsY[y];
    }
    float TotalWeightX = 0.0f;
    for (UINT x=0; x < nSamplesX; x++)
    {
        TotalWeightX += WeightsX[x];
    }
    for (UINT y=0; y < nSamplesY; y++)
    {
        pWeightsY[y] = WeightsY[y] / TotalWeightY;
    }
    for (UINT x=0; x < nSamplesX; x++)
    {
        pWeightsX[x] = WeightsX[x] / TotalWeightX;
    }
}

//-----------------------------------------------------------------------------

bool CTentFilter::ExecuteTestOnDevice(UINT uIndex, UINT uTest)
{
    float fX[16];
    float fY[16];

    if (0 == uIndex)
        BeginTestCase("Tent Filter",uTest);
    
    m_Device[uIndex].pDevice->SetConvolutionMonoKernel(uTest, uTest, fX, fY);
    
    m_Device[uIndex].pDevice->BeginScene();
    m_Device[uIndex].pDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,0xFF000080,1.0f,0);

    // Compose the rects

    PrepareDestinationSurface(uIndex,0);
    DrawText(uIndex,10,100,"Tent Filter",g_uOps[0],0,0);
    
    // Establish the filter model

    float   fSin[8];
    float   fPI = -3.1415927f;
    float   fStep = (3.1415927f*2.0f)/8.0f;
    
    fPI += (uTest - 32) * fStep;
    
    float fScale = 1.0f / ((float)uTest / 8.0f);
    
    for (UINT uX=0; uX<8; ++uX)
    {
        fSin[uX] = sinf(fPI) * fScale;
        fPI += fStep;
    }

    // Do the filtering

    m_Device[uIndex].pDevice->SetSamplerState(0, SAMP_MAGFILTER, D3DTEXF_CONVOLUTIONMONO);
    m_Device[uIndex].pDevice->SetSamplerState(0, SAMP_MINFILTER, D3DTEXF_CONVOLUTIONMONO);

    // TODO
    //
    // This needs rows only, columns only, rows and columns of different sizes.
    
    m_Device[uIndex].pDevice->SetConvolutionMonoKernel(8,8,fSin,fSin);

    // Scale the image.
    
    DisplayImage(uIndex,0xFFFFFFFF,0.5f);
    
    m_Device[uIndex].pDevice->EndScene();

    return true;
}

//-----------------------------------------------------------------------------

UINT CTentFilter::TestInitialize(void)
{
    UINT uResult = CLongFonts::TestInitialize();
 
    SetTestRange(1,8);
    
    return uResult;
}

//-----------------------------------------------------------------------------

bool CRowOnly::ExecuteTestOnDevice(UINT uIndex, UINT uTest)
{
    float fX[16];
    //float fY[16];
    
    if (0 == uIndex)
        BeginTestCase("Convolution Mono - Row/Tent filter",uTest);
    
    m_Device[uIndex].pDevice->SetConvolutionMonoKernel(uTest, 0, fX, NULL);
    
    m_Device[uIndex].pDevice->BeginScene();
    m_Device[uIndex].pDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,0xFF000080,1.0f,0);

    // Compose the rects

    PrepareDestinationSurface(uIndex,0);
    DrawText(uIndex,10,100,"Row Only",g_uOps[0],0,0);
    
    // Establish the filter model

    float   fSin[8];
    float   fPI = -3.1415927f;
    float   fStep = (3.1415927f*2.0f)/8.0f;
    
    fPI += (uTest - 32) * fStep;
    
    float fScale = 1.0f / ((float)uTest / 8.0f);
    
    for (UINT uX=0; uX<8; ++uX)
    {
        fSin[uX] = sinf(fPI) * fScale;
        fPI += fStep;
    }

    // Do the filtering

    m_Device[uIndex].pDevice->SetSamplerState(0, SAMP_MAGFILTER, D3DTEXF_CONVOLUTIONMONO);
    m_Device[uIndex].pDevice->SetSamplerState(0, SAMP_MINFILTER, D3DTEXF_CONVOLUTIONMONO);

    // TODO
    //
    // This needs rows only, columns only, rows and columns of different sizes.
    
    m_Device[uIndex].pDevice->SetConvolutionMonoKernel(8,8,fSin,fSin);

    // Scale the image.
    
    DisplayImage(uIndex,0xFFFFFFFF,0.5f);
    
    m_Device[uIndex].pDevice->EndScene();

    return true;
}

//-----------------------------------------------------------------------------

UINT CRowOnly::TestInitialize(void)
{
    UINT uResult = CLongFonts::TestInitialize();
 
    SetTestRange(1,8);
    
    return uResult;
}

//-----------------------------------------------------------------------------

bool CColOnly::ExecuteTestOnDevice(UINT uIndex, UINT uTest)
{
    //float fX[16];
    float fY[16];

    if (0 == uIndex)
        BeginTestCase("Convolution Mono - Column/Tent filter",uTest);
    
    m_Device[uIndex].pDevice->SetConvolutionMonoKernel(0, uTest, NULL, fY);

    m_Device[uIndex].pDevice->BeginScene();
    m_Device[uIndex].pDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,0xFF000080,1.0f,0);

    // Compose the rects

    PrepareDestinationSurface(uIndex,0);
    DrawText(uIndex,10,100,"Col Only",g_uOps[0],0,0);
    
    // Establish the filter model

    float   fSin[8];
    float   fPI = -3.1415927f;
    float   fStep = (3.1415927f*2.0f)/8.0f;
    
    fPI += (uTest - 32) * fStep;
    
    float fScale = 1.0f / ((float)uTest / 8.0f);
    
    for (UINT uX=0; uX<8; ++uX)
    {
        fSin[uX] = sinf(fPI) * fScale;
        fPI += fStep;
    }

    // Do the filtering

    m_Device[uIndex].pDevice->SetSamplerState(0, SAMP_MAGFILTER, D3DTEXF_CONVOLUTIONMONO);
    m_Device[uIndex].pDevice->SetSamplerState(0, SAMP_MINFILTER, D3DTEXF_CONVOLUTIONMONO);

    // TODO
    //
    // This needs rows only, columns only, rows and columns of different sizes.
    
    m_Device[uIndex].pDevice->SetConvolutionMonoKernel(8,8,fSin,fSin);

    // Scale the image.
    
    DisplayImage(uIndex,0xFFFFFFFF,0.5f);
    
    m_Device[uIndex].pDevice->EndScene();

    return true;
}

//-----------------------------------------------------------------------------

UINT CColOnly::TestInitialize(void)
{
    UINT uResult = CLongFonts::TestInitialize();
 
    SetTestRange(1,8);
    
    return uResult;
}

*/