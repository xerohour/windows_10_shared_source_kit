#include "depthstencilreadback.h"

#include "d3d9.h"

inline bool operator==( const FORMAT &rhs, const FORMAT &lhs )
{
    return rhs.d3dfFormat == lhs.d3dfFormat;
}

//
// Declare the framework
//

CD3DWindowFramework App;

//
// Define the groups here
//
CDepthStencilReadBack_Support spt;
CDepthStencilReadBack tst;


/*
 *  CAutoPtr - simple smart pointer to automatically release resources
 */

template<class T>
class CAutoPtr
{
public:
    explicit CAutoPtr(T *t=0) :
        m_t(t)
    {
    }
    ~CAutoPtr()
    {
        if (m_t) m_t->Release();
    }
    T *operator->()
    {
        return m_t;
    }
    T &operator*()
    {
        return *m_t;
    }
    T **operator&()
    {
        return &m_t;
    }
    operator T*()
    {
        return m_t;
    }
    operator const T*() const
    {
        return m_t;
    }
private:
    T *m_t;
};


/*
 *  CHRESULT - encapsulates HRESULT error exception handling
 */

class CHRESULT 
{
public:
    CHRESULT(HRESULT hr = S_OK)
    {
        *this = hr;
    }
    CHRESULT &operator=(HRESULT hr)
    {
        if (FAILED(m_hr = hr))
        {
            throw *this;
        }
        return *this;
    }
    CHRESULT &operator=(bool b)
    {
        return *this = b ? S_OK : E_FAIL;
    }
    CHRESULT &operator=(BOOL b)
    {
        return *this = b ? S_OK : E_FAIL;
    }
    operator HRESULT() const
    {
        return m_hr;
    }
	bool setTestResult(CD3DTest *test) const
	{
		if ( test && FAILED(m_hr) )
		{
			if (D3DERR_OUTOFVIDEOMEMORY == m_hr)
			{
				test->Skip();
			}
			else
			{
				test->Fail();
			}
			return false;
		}
		return true;
	}
private:
    HRESULT m_hr;
};


/*
 *  CSurfaceArray - provides typed access to surface pixelvalues
 */

template<class T> class CSurfaceArray
{
public:
    CSurfaceArray( void *base, DWORD stride, DWORD length = sizeof( T ) ) :
        m_base( static_cast< BYTE* >( base ) ),
        m_stride( stride ),
        m_length( length )
    {
    }
    CSurfaceArray( const LOCKED_RECT &lockRect, DWORD length = sizeof( T ) ) :
        m_base( static_cast< BYTE* >( lockRect.pBits ) ),
        m_stride( static_cast< DWORD >( lockRect.iPitch ) ),
        m_length( length )
    {
    }
    T *operator()(unsigned x, unsigned y) const
    {
        return reinterpret_cast<T*>(m_base + y * m_stride + x * m_length);
    }
private:
    BYTE *m_base;
    DWORD m_stride;
    DWORD m_length;
};


//
// CProportions Implementation
//

CProportions::CProportions() :
    m_Left( 0.0f ),
    m_Top( 0.0f ),
    m_Right( 1.0f ),
    m_Bottom( 1.0f ),
    m_X( 0.0f ),
    m_Y( 0.0f )
{
}

CProportions::CProportions( float left,
                float top,
                float right,
                float bottom,
                float x,
                float y ) :
    m_Left( left ),
    m_Top( top ),
    m_Right( right ),
    m_Bottom( bottom ),
    m_X( x ),
    m_Y( y )
{
}

RECT &CProportions::getRect( LONG width, LONG height, RECT &rect ) const
{
    RECT temp = { LONG(m_Left * width), LONG(m_Top * height), LONG(m_Right * width), LONG(m_Bottom * height) };
    return rect = temp;
}

POINT &CProportions::getPoint( LONG width, LONG height, POINT &point ) const
{
    POINT temp = { LONG(m_X * width), LONG(m_Y * height) };
    return point = temp;
}


//
// Common Base Class Implementation
//

CDepthStencilReadBack::CDepthStencilReadBack( ) :
	m_bRunInvalid( FALSE )
{
	SetTestType( TESTTYPE_CONF );

    m_szTestName = _T("DepthStencilReadback");
    m_szCommandKey = _T("dsrb");

	m_Options.D3DOptions.fMinDXVersion = 9.1f;
    m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;

    m_Options.ModeOptions.uMaxDisplayModes = 1;
}

bool CDepthStencilReadBack::ParseCmdLine()
{
    if( KeySet( _T( "INV" ) ) )
    {
       m_bRunInvalid = TRUE;
    }

    return CD3DTest::ParseCmdLine();
}

bool CDepthStencilReadBack::FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount)
{
    return CD3DTest::FilterFormats(pFmtIn, dwInclFlags, dwExclFlags, pFmtOut, uCount);
}

bool CDepthStencilReadBack::FilterResolution(PDISPLAYMODE pMode)
{
    return CD3DTest::FilterResolution(pMode);
}

bool CDepthStencilReadBack::CapsCheck()
{
    return CD3DTest::CapsCheck();
}

bool CDepthStencilReadBack::Setup()
{
    if ( m_bRunInvalid )
    {
        // define the invalid format conversions
        AddFormatConversion(FMT_D16, FMT_S8_LOCKABLE);
        AddFormatConversion(FMT_D32, FMT_S8_LOCKABLE);
        AddFormatConversion(FMT_D24X8, FMT_S8_LOCKABLE);

        AddFormatConversion(FMT_D16_LOCKABLE, FMT_D16_LOCKABLE);
        AddFormatConversion(FMT_D16_LOCKABLE, FMT_D32_LOCKABLE);
        AddFormatConversion(FMT_D16_LOCKABLE, FMT_D32F_LOCKABLE);

        AddFormatConversion(FMT_D32F_LOCKABLE, FMT_D16_LOCKABLE);
        AddFormatConversion(FMT_D32F_LOCKABLE, FMT_D32_LOCKABLE);
        AddFormatConversion(FMT_D32F_LOCKABLE, FMT_D32F_LOCKABLE);
    }
    else
    {        
        // define the valid format conversions
        AddFormatConversion(FMT_D16, FMT_D16_LOCKABLE);
        AddFormatConversion(FMT_D16, FMT_D32_LOCKABLE);
        AddFormatConversion(FMT_D16, FMT_D32F_LOCKABLE);

        AddFormatConversion(FMT_D32, FMT_D16_LOCKABLE);
        AddFormatConversion(FMT_D32, FMT_D32_LOCKABLE);
        AddFormatConversion(FMT_D32, FMT_D32F_LOCKABLE);

        AddFormatConversion(FMT_D15S1, FMT_D16_LOCKABLE);
        AddFormatConversion(FMT_D15S1, FMT_D32_LOCKABLE);
        AddFormatConversion(FMT_D15S1, FMT_D32F_LOCKABLE);
        AddFormatConversion(FMT_D15S1, FMT_S8_LOCKABLE);

        AddFormatConversion(FMT_D24S8, FMT_D16_LOCKABLE);
        AddFormatConversion(FMT_D24S8, FMT_D32_LOCKABLE);
        AddFormatConversion(FMT_D24S8, FMT_D32F_LOCKABLE);
        AddFormatConversion(FMT_D24S8, FMT_S8_LOCKABLE);

        AddFormatConversion(FMT_D24X8, FMT_D16_LOCKABLE);
        AddFormatConversion(FMT_D24X8, FMT_D32_LOCKABLE);
        AddFormatConversion(FMT_D24X8, FMT_D32F_LOCKABLE);
    }

    // initialize surface pointers
    m_pDepthStencilSurface = m_pSrcSurface = m_pDestSurface = NULL;


    return CD3DTest::Setup();
}

bool CDepthStencilReadBack::AddFormatConversion( FORMAT fmtSrc, FORMAT fmtDes )
{
    if( false == IsSurfaceFormatSupported(fmtDes) || false == IsDepthStencilFormatSupported(fmtSrc) )
        return false;

    m_Formats.push_back( FormatSetType::value_type( fmtSrc, fmtDes ) );

    return true;
}

bool CDepthStencilReadBack::SetDefaultViewport()
{
    return CD3DTest::SetDefaultViewport();
}

bool CDepthStencilReadBack::SetDefaultMaterials()
{
    MATERIAL material;

	ZeroMemory(&material,sizeof(MATERIAL));
	material.diffuse.r  = 1.0f;
	material.diffuse.g  = 1.0f;
	material.diffuse.b  = 1.0f;
	material.diffuse.a  = 1.0f;
	
	material.ambient.r  = 1.0f;
	material.ambient.g  = 1.0f;
	material.ambient.b  = 1.0f;
	material.ambient.a  = 1.0f;
	
	material.specular.r = 1.0f;
	material.specular.g = 1.0f;
	material.specular.b = 1.0f;
	material.specular.a = 1.0f;

	material.emissive.r = 0.0f;
	material.emissive.g = 0.0f;
	material.emissive.b = 0.0f;
	material.emissive.a = 0.0f;
	
	material.power      = 20.0f;
	
    SetMaterial(&material);

    return CD3DTest::SetDefaultMaterials();
}

bool CDepthStencilReadBack::SetDefaultLights()
{
    LIGHT light;

	ZeroMemory(&light,sizeof(LIGHT));
	light.dltType		= D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r		= 1.0f;
	light.Diffuse.g		= 1.0f;
	light.Diffuse.b		= 1.0f;
	light.Diffuse.a		= 1.0f;

	light.Specular.r	= 1.0f;
	light.Specular.g	= 1.0f;
	light.Specular.b	= 1.0f;
	light.Specular.a	= 1.0f;

	light.Position.x	= 0.0f;
	light.Position.y	= 0.0f;
	light.Position.z	= 0.0f;

	light.Direction.x	= 0.0f;
	light.Direction.y	= 0.0f;
	light.Direction.z	= 1.0f;

	light.Range			= 1000.0f;

    SetLight(0,&light);

    LightEnable(0,TRUE);

    return CD3DTest::SetDefaultLights();
}

bool CDepthStencilReadBack::SetDefaultTextures()
{
    return CD3DTest::SetDefaultTextures();
}

UINT CDepthStencilReadBack::TestInitialize()
{
    if( 0 == m_Formats.size() )
    {
 	SetTestRange( 1, 1);
    }
    else
    {
        SetTestRange( 1, m_Formats.size() * ( m_bRunInvalid ? 1 : ( m_Subsets.size() + 1 ) ) );
    }
    
    return CD3DTest::TestInitialize();
}

bool CDepthStencilReadBack::ClearFrame(void)
{
    return CD3DTest::ClearFrame();
}

bool CDepthStencilReadBack::ExecuteTest(UINT nTest)
{
    CHRESULT hr;

    if( 0 == m_Formats.size() )
    {
     	BeginTestCase( _T("Formats"), nTest );
        SetResult(SKIP);
        WriteToLog(_T("ExecuteTest: No Formats to test!\n"));
        return false;
    }

    try
    {
        RECT rect, *pRECT = NULL;
        POINT point, *pPOINT = NULL;

        // compute format and optional subset (0 subset is NULL case)
        UINT iFormat = ( nTest - 1 ) % m_Formats.size();
        UINT iSubset = ( nTest - 1 ) / m_Formats.size();

        // save the depth-stencil Surface owned by the Device
        hr = GetDepthStencilSurface( &m_pDepthStencilSurface );

        CAutoPtr< CSurface > pSurface;
        hr = m_pDepthStencilSurface->GetSurface( &pSurface, 0 );

        SURFACEDESC surfaceDesc;
        hr = pSurface->GetDesc( &surfaceDesc );

        TCHAR title[256] = TEXT( "{ " ), str[128];
        FORMAT srcFormat = m_Formats[iFormat].first, destFormat = m_Formats[iFormat].second;

        // create testcase name based on format conversion
        FmtToString(FORMAT_TEXTURE, &srcFormat, str);
        strcat( title, str );
        strcat( title, TEXT( ", " ) );
        FmtToString(FORMAT_TEXTURE, &destFormat, str );
        strcat( title, str );

        if ( iSubset )
        {
            const CProportions &prop = m_Subsets[iSubset - 1];
            pRECT = &prop.getRect( surfaceDesc.Width, surfaceDesc.Height, rect );
            pPOINT = &prop.getPoint( surfaceDesc.Width, surfaceDesc.Height, point );

            sprintf( str, TEXT( ", { { %d, %d, %d, %d }, { %d, %d } }" ), pRECT->left, pRECT->top, pRECT->right, pRECT->bottom, pPOINT->x, pPOINT->y ), 
            strcat( title, str );
        }

        strcat( title, TEXT( " } " ) );

        BeginTestCase( title, nTest );

        // create the source depth-stencil surface
        hr = CreateDepthStencilSurface( surfaceDesc.Width,
                                        surfaceDesc.Height,
                                        srcFormat,
                                        MULTISAMPLE_NONE,
                                        0,
                                        FALSE,
                                        &m_pSrcSurface );

        // create the destination depth-stencil surface
        hr = CreateOffscreenPlainSurface( surfaceDesc.Width,
                                          surfaceDesc.Height,
                                          destFormat,
                                          POOL_SYSTEMMEM,
                                          &m_pDestSurface );

        // replace the device's depth-stencil surface with the one just created
        hr = SetDepthStencilSurface( m_pSrcSurface );

	    struct XYZVertex
	    {
		    float x;
		    float y;
		    float z;
	    };

        CAutoPtr< CSurface > pSurface2;
        hr = m_pSrcSurface->GetSurface( &pSurface2, 0 );

        hr = pSurface2->GetDesc( &surfaceDesc );

        static FORMAT stencilSet[] =
        {
            FMT_D15S1,
            FMT_D24S8,
            FMT_D24X4S4,
            FMT_D24FS8
        };
        
        FORMAT *const pStencilSetEnd = stencilSet + sizeof( stencilSet ) / sizeof( stencilSet[0] );
        bool isStencilSurface = std::find( stencilSet, pStencilSetEnd, surfaceDesc.Format ) != pStencilSetEnd;

        if ( isStencilSurface )
        {
            // define two triangles
            XYZVertex stencilVertices[] =
            {
                {0.0f,0.8f,0.0f},
                {0.8f,-0.8f,0.0f},
                {-0.8f,-0.8f,0.0f},
                {0.5f,0.5f,0.0f},
                {0.8f,0.8f,0.0f},
                {0.8f,0.2f,0.0f}
            };

            // create a vertex buffer for the stencil content
		    CAutoPtr< CnVertexBuffer > pVertexBuffer;
		    hr = CreateVertexBuffer( sizeof( stencilVertices ), USAGE_WRITEONLY, D3DFVF_XYZ, POOL_DEFAULT, &pVertexBuffer );

            // fill the vertex buffer with triangle data
		    void *pVertices;
		    hr = pVertexBuffer->Lock(0, 0, &pVertices, 0);
            memcpy( pVertices, stencilVertices, sizeof( stencilVertices ) );
            hr = pVertexBuffer->Unlock();

            // define render states for stencil write
		    hr = SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		    hr = SetRenderState( D3DRS_STENCILENABLE, TRUE );
		    hr = SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
		    hr = SetRenderState( D3DRS_STENCILREF, 0x00000001 );
		    hr = SetRenderState( D3DRS_STENCILMASK, 0x00000001 );
		    hr = SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILCAPS_REPLACE );
		    hr = SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILCAPS_REPLACE );
		    hr = SetRenderState( D3DRS_STENCILPASS, D3DSTENCILCAPS_REPLACE );
		    hr = SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

            // render the triangles
            hr = Clear( 0, NULL, CLEARF_STENCIL | CLEARF_ZBUFFER, 0, 1.0f, 0 );
		    hr = SetStreamSource( 0, pVertexBuffer, sizeof( XYZVertex ), 0 );
		    hr = SetFVF( D3DFVF_XYZ );
            hr = BeginScene();
            hr = DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
            hr = EndScene();

            // define render states for ARGB write
	        hr = SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );
	        hr = SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILCAPS_KEEP );
	        hr = SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILCAPS_KEEP );
	        hr = SetRenderState( D3DRS_STENCILPASS, D3DSTENCILCAPS_KEEP );
        }
        else
        {
            hr = SetRenderState( D3DRS_STENCILENABLE, FALSE );
        }

        // define render states
        hr = SetRenderState( D3DRS_ZWRITEENABLE, TRUE ); 
	    hr = SetRenderState( D3DRS_LIGHTING, FALSE );

        struct XYZCVertex
        {
            XYZVertex xyz;
            DWORD c;
        };

        XYZCVertex renderTargetVertices[] = 
        {
            {{-1.0f,-1.0f,0.0f},0xffff0000},
            {{-1.0f,1.0f,1.0f},0xff00ff00},
            {{1.0f,-1.0f,0.0f},0xff0000ff},
            {{1.0f,1.0f,1.0f},0xffffffff}
        };

        // create a vertex buffer for the render-target content
        CAutoPtr< CnVertexBuffer > pVertexBuffer;
	    hr = CreateVertexBuffer( sizeof( renderTargetVertices ), USAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, POOL_DEFAULT, &pVertexBuffer );

        // fill the vertex buffer with triangle data
	    void *pVertices;
	    hr = pVertexBuffer->Lock(0, 0, &pVertices, 0);
        memcpy( pVertices, renderTargetVertices, sizeof( renderTargetVertices ) );
        hr = pVertexBuffer->Unlock();

        // setup source of vertex data
	    hr = SetStreamSource( 0, pVertexBuffer, sizeof( XYZCVertex ), 0 );
	    hr = SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

        D3DCOLOR Color = 0x00000032;
	    m_pFramework->GetImageCmp()->SetClearColor( Color );

        if ( iSubset )
        {
            // setup to render only in subregion specified
            DWORD dwFlags = CLEARF_ZBUFFER;
            if ( isStencilSurface )
            {
                dwFlags |= CLEARF_STENCIL;
            }

            // clear all destination surface to "off"
            hr = Clear( 0, NULL, dwFlags, Color, 0.0f, 0 );
            hr = UpdateSurface( m_pSrcSurface, NULL, m_pDestSurface, NULL );

            // clear source to to "on"
            hr = Clear( 0, NULL, dwFlags | CLEARF_TARGET, Color, 1.0f, 1 );

            // copy subregion of source to destination
            hr = UpdateSurface( m_pSrcSurface, pRECT, m_pDestSurface, pPOINT );

            // and copy back to source
            hr = UpdateSurface( m_pDestSurface, NULL, m_pSrcSurface, NULL );
        }
        else
        {
            // draw quad to load depth buffer
            hr = Clear( 0, NULL, CLEARF_TARGET | CLEARF_ZBUFFER, Color, 1.0f, 0 );
            hr = BeginScene();
            hr = DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
            hr = EndScene();

            if ( m_bRunInvalid )
            {
                hr = !UpdateSurface( m_pSrcSurface, NULL, m_pDestSurface, NULL );
                GetLastError();
            }

            // copy pool default surface to system memory surface
            hr = UpdateSurface( m_pSrcSurface, NULL, m_pDestSurface, NULL );

            // flip depthstencil system memory surface vertically
            for( unsigned i = 0; i < 2; i++ )
            {
                CAutoPtr< CSurface > pSurface;
                hr = m_pDestSurface->GetSurface( &pSurface, i );

                SURFACEDESC surfaceDesc;
                hr = pSurface->GetDesc( &surfaceDesc );

                LOCKED_RECT lockRect;
                hr = pSurface->LockRect( &lockRect, NULL, 0 );
                CSurfaceArray< BYTE > srcBits( lockRect );
                
                unsigned y1 = 0;
                unsigned y2 = surfaceDesc.Height - 1;
                std::auto_ptr< BYTE > ptr( new BYTE[ lockRect.iPitch ] );

                while ( y1 < y2 )
                {
                    memcpy( ptr.get(), srcBits( 0, y1 ), lockRect.iPitch );
                    memcpy( srcBits(0, y1 ), srcBits( 0, y2 ), lockRect.iPitch );
                    memcpy( srcBits(0, y2 ), ptr.get(), lockRect.iPitch );
                    y1++;
                    y2--;
                }

                hr = pSurface->UnlockRect();
            }

            // copy flipped system memory surface back to pool default surface
            hr = UpdateSurface( m_pDestSurface, NULL, m_pSrcSurface, NULL );

            // redraw quad using flipped depthstencil buffer
            hr = Clear( 0, NULL, CLEARF_TARGET, Color, 1.0f, 0 );
        }

        // render quad
        hr = BeginScene();
        hr = DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
        hr = EndScene();
    }
    catch( CHRESULT hr )
    {
        return hr.setTestResult(this);
    }

    return CD3DTest::ExecuteTest(nTest);
}

void CDepthStencilReadBack::SceneRefresh()
{
    CD3DTest::SceneRefresh();
}

bool CDepthStencilReadBack::ProcessFrame(void)
{
    return CD3DTest::ProcessFrame();
}

bool CDepthStencilReadBack::DisplayFrame(void)
{
    return CD3DTest::DisplayFrame();
}

void CDepthStencilReadBack::EndTestCase(void)
{
    if( 0 != m_Formats.size() )
    {
 	// restore device's original depth-stencil surface
        SetDepthStencilSurface( m_pDepthStencilSurface );
    }

    // release all surface handles
    RELEASE( m_pDepthStencilSurface );
    RELEASE( m_pSrcSurface );
    RELEASE( m_pDestSurface );

    CD3DTest::EndTestCase();
}

void CDepthStencilReadBack::UpdateStatus()
{
    CD3DTest::UpdateStatus();
}

void CDepthStencilReadBack::UpdateDetails()
{
    CD3DTest::UpdateDetails();
}

bool CDepthStencilReadBack::TestTerminate(void)
{
    return CD3DTest::TestTerminate();
}

void CDepthStencilReadBack::Cleanup()
{
    m_Formats.clear();

    CD3DTest::Cleanup();
}

void CDepthStencilReadBack::OnDeviceLost()
{
    CD3DTest::OnDeviceLost();
}

void CDepthStencilReadBack::OnReset()
{
    CD3DTest::OnReset();
}

bool CDepthStencilReadBack::DrawStats(int nHeight, int nWidth, CDevice *pDevice)
{
    return CD3DTest::DrawStats(nHeight, nWidth, pDevice);
}

bool CDepthStencilReadBack::SetDefaultRenderStates()
{
    return CD3DTest::SetDefaultRenderStates();
}

bool CDepthStencilReadBack::SetDefaultMatrices()
{
    return CD3DTest::SetDefaultMatrices();
}

void CDepthStencilReadBack::CommandLineHelp()
{
    WriteCommandLineHelp("$y-inv\t$wRun invalid testcases");

    CD3DTest::CommandLineHelp();
}

void CDepthStencilReadBack::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
    CD3DTest::SetTestOptions(dwTestScenarios, fMinVersion, bPure, bRequireRefRast);
}


CDepthStencilReadBack_Support::CDepthStencilReadBack_Support()
{
    SetTestType( TESTTYPE_CONF );

    m_szTestName = _T("DepthStencilReadback_Support");
    m_szCommandKey = _T("dsrbs");

    m_Options.D3DOptions.fMinDXVersion = 9.1f;
    m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;

    m_Options.ModeOptions.uMaxDisplayModes = 1;
    m_Options.D3DOptions.bReference = false;
};

bool CDepthStencilReadBack_Support::IsSurfaceFormatSupported( FORMAT fmt )
{
	// verify the Src device can handle the desired format
	bool bFormatHALSupported =	SUCCEEDED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pSrcDevice->GetPresParams()->DesktopFormat, 0, RTYPE_SURFACE, fmt) );
	
    if( !bFormatHALSupported )
        WriteToLog(_T("IsSurfaceFormatSupported: %d is not supported by the HAL device.\n"),fmt.d3dfFormat);
			
	// verify the Ref device can handle the desired format
	bool bFormatREFSupported =	true;
    
    if( NULL != m_pRefDevice )
    {
        bFormatREFSupported = SUCCEEDED(m_pD3D->CheckDeviceFormat(m_pRefDevice->GetAdapterID(), m_pRefDevice->GetDevType(), m_pRefDevice->GetPresParams()->DesktopFormat, 0, RTYPE_SURFACE, fmt) );
	    
        if( !bFormatREFSupported )
            WriteToLog(_T("IsSurfaceFormatSupported: %d is not supported by the REF device.\n"),fmt.d3dfFormat);
    }

	return bFormatHALSupported && bFormatREFSupported;
}

bool CDepthStencilReadBack_Support::IsDepthStencilFormatSupported( FORMAT fmtDepthStencil )
{
	// verify the Src device can handle the desired format
	bool bFormatHALSupported =	SUCCEEDED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pSrcDevice->GetPresParams()->DesktopFormat, USAGE_DEPTHSTENCIL, RTYPE_SURFACE, fmtDepthStencil) ) &&
								SUCCEEDED(m_pD3D->CheckDepthStencilMatch(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pSrcDevice->GetPresParams()->DesktopFormat, m_pSrcDevice->GetPresParams()->BackBufferFormat, fmtDepthStencil) );
	if( !bFormatHALSupported )
        WriteToLog(_T("IsDepthStencilFormatSupported: %d is not supported by the HAL device.\n"),fmtDepthStencil.d3dfFormat);
			
	// verify the Ref device can handle the desired format
	bool bFormatREFSupported =	true;
    if( NULL != m_pRefDevice )
    {
          bFormatREFSupported = SUCCEEDED(m_pD3D->CheckDeviceFormat(m_pRefDevice->GetAdapterID(), m_pRefDevice->GetDevType(), m_pRefDevice->GetPresParams()->DesktopFormat, USAGE_DEPTHSTENCIL, RTYPE_SURFACE, fmtDepthStencil) ) &&
								SUCCEEDED(m_pD3D->CheckDepthStencilMatch(m_pRefDevice->GetAdapterID(), m_pRefDevice->GetDevType(), m_pRefDevice->GetPresParams()->DesktopFormat, m_pRefDevice->GetPresParams()->BackBufferFormat, fmtDepthStencil) );
	    if( !bFormatREFSupported )
            WriteToLog(_T("IsDepthStencilFormatSupported: %d is not supported by the REF device.\n"),fmtDepthStencil.d3dfFormat);
    }

	return bFormatHALSupported && bFormatREFSupported;
}

bool CDepthStencilReadBack_Support::ExecuteTest(UINT nTest)
{
    FORMAT dsFormats[] = {  
        FMT_D16_LOCKABLE, 
        FMT_D32_LOCKABLE,
        FMT_D32F_LOCKABLE,
    };
    UINT nFormats =  0 != sizeof(FORMAT)? sizeof(dsFormats) / sizeof(FORMAT): 0;
    bool bSupported = false;

    if( 1 == nTest )
    {
        BeginTestCase( _T("D3DFMT_S8_LOCKABLE Support"), nTest );
        bSupported = IsSurfaceFormatSupported(FMT_S8_LOCKABLE);
        if( false == bSupported )
        {
            WriteToLog(_T("D3DFMT_S8_LOCKABLE support not found."));
            SetResult(SKIP);
        }
    }
    else if( 2 == nTest )
    {
        BeginTestCase( _T("DepthStencilFormat Support"), nTest );
        for(UINT i = 0; i < nFormats; i++)
        {
            if( true == IsSurfaceFormatSupported(dsFormats[i]) )
                bSupported = true;
        }
        if( false == bSupported )
        {
            SetResult(SKIP);
            WriteToLog(_T("Unable to find valied depth buffer support. Need support for one of:"));
            for(UINT j = 0; j < nFormats; j++)
                WriteToLog(_T(" - %d \n"), dsFormats[j].d3dfFormat);
        }
    }
    else
    {
        WriteToLog(_T("Unknown Test ID\n"));
        Abort();
    }

    GetLastError();

    return CD3DTest::ExecuteTest(nTest);
}
