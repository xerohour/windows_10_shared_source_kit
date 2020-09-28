// ColorConvertingPresent.cpp: implementation of the CPresentII class.
//
//////////////////////////////////////////////////////////////////////

#include <algorithm>
#include "PresentII.h"
#include <ShellApi.h>
#include <StrSafe.h>
#include <VersionHelpers.h>

DWORD ConvertDeviceType(D3DDEVTYPE DeviceType);

//////////////////////////////////////////////////////////////////////
// Name: MyWindowProc
// Desc: window message processing function, used to prevent screensavers
//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK MyWindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_SYSCOMMAND:
        switch( wParam )
        {
        case SC_SCREENSAVE:    // prevent screen savers
            return FALSE;
        }
        break;
    case WM_ERASEBKGND:
        break;
    }

    return DefWindowProc( hWnd, Msg, wParam, lParam );
}


//////////////////////////////////////////////////////////////////////
// Name: FillD3DXTexture
// Desc: method called from the D3DX, fills a texture
//////////////////////////////////////////////////////////////////////
VOID __stdcall FillD3DXTexture(D3DXVECTOR4 *pOut, D3DXVECTOR2 *pTexCoord, D3DXVECTOR2 *pTexelSize, LPVOID pData)
{
    DWORD xPixel = (DWORD) ( pTexCoord->x / pTexelSize->x + 0.51 );        // current Pixel X
    DWORD yPixel = (DWORD) ( pTexCoord->y / pTexelSize->y + 0.51 );        // current Pixel Y
    DWORD xMaxPixel = (DWORD) ( 1.00f / pTexelSize->x + 0.51 );            // Max Pixel X
    DWORD yMaxPixel = (DWORD) ( 1.00f / pTexelSize->y + 0.51 );            // Max Pixel Y

    ASSERT( xPixel <= xMaxPixel && yPixel <= yMaxPixel );

    if( NULL == pData || (*(TEST_TYPE*)pData != TEST_SETDIALOGBOXMODE && *(TEST_TYPE*)pData != TEST_CLIPPINGWINDOWS) )
    {
        float xSpace = 16.0f * ( xPixel >> 4 ) / float( xMaxPixel );
        float ySpace = 16.0f * ( yPixel >> 4 ) / float( yMaxPixel );
    
        pOut->x = 0.50f * xSpace + 0.50f * ySpace;
        pOut->y = 0.75f * xSpace + 0.25f * ySpace;
        pOut->z = 0.25f * xSpace + 0.75f * ySpace;
        pOut->w = 0.35f * xSpace + 0.65f * ySpace;
    }
    else
    {
        pOut->x = COLOR_BGROUND.red / 255.0f;
        pOut->y = COLOR_BGROUND.green / 255.0f;
        pOut->z = COLOR_BGROUND.blue / 255.0f;
        pOut->w = COLOR_BGROUND.alpha / 255.0f;
    }
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPresentII::CPresentII() :
    m_bCanRender( false ),
    m_fDPIFactor( 1.0f ),
    m_hWndReset( 0L ),
    m_hWndOverride( 0L ),
    m_hWndPresent( 0L ),
    m_pTexture( NULL ),
    m_pVB( NULL ),
    m_pDirtyRegion( NULL ),
    m_pDestDirtyRegion( NULL ),
    m_uNumberSwapChains( 1 ),
    m_SwapEffect( SWAPEFFECT_COPY),
    m_pRenderingDevice(NULL),
    m_pPShader(NULL),
    m_nIndexDeviceReleaseBeforeVerify(0),
    m_fGamma(2.2f)
{
    m_szTestName = _T("PresentII");

    SetTestType(TESTTYPE_CONF);
    m_Options.D3DOptions.fMinDXVersion        = 9.0f;
    m_Options.D3DOptions.bReference           = false;
    m_Options.D3DOptions.bZBuffer             = false;
    m_Options.D3DOptions.bCachedBackBuffer    = false;
    //m_Options.D3DOptions.dwDevTypes            = DEVICETYPE_HAL | DEVICETYPE_REF;
    //m_Options.D3DOptions.dwBehaviors        = DEVICECREATE_STANDARD | DEVICECREATE_SW_OR_HW_VP;
    
    m_bDontHideCursor    = KeySet(_T("ShowCursor"));

    ZeroMemory( &m_SwapChainData, MAX_SWAPCHAINS * sizeof( SWAPCHAINDATA ) );

    ZeroMemory( &m_osVersionInfoEx, sizeof OSVERSIONINFOEX );
    m_osVersionInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx ((LPOSVERSIONINFO)&m_osVersionInfoEx);
    m_bQuitOnFailure = KeySet(_T("Save"));
    m_bSaveAll = KeySet(_T("SaveAll"));
}


CPresentII::~CPresentII()
{
    TestTerminate();
}


//////////////////////////////////////////////////////////////////////
// Name: CommandLineHelp()
// Desc: command line help for the test
//////////////////////////////////////////////////////////////////////
void CPresentII::CommandLineHelp()
{
    WriteCommandLineHelp(_T("Command line parameters: -save"));
    WriteCommandLineHelp(_T("The test will stop after the first failing test in the range"));

    WriteCommandLineHelp(_T("Command line parameters: -saveall"));
    WriteCommandLineHelp(_T("Save bitmaps for all failures and continues running tests\n"));
   
    WriteCommandLineHelp(_T("Both commands save image (.bmp and .dds) files for the failure in the in the local folder.\n"));

    WriteCommandLineHelp(_T("Command line parameters: -debugpause:SleepTime"));
    WriteCommandLineHelp(_T("Sleep in miliseconds after present"));
}


//////////////////////////////////////////////////////////////////////
// Name: IsDWMEnabled()
// Desc: returns true if DWM is enabled, false otherwise
//////////////////////////////////////////////////////////////////////
bool CPresentII::IsDWMEnabled()
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

    return bCompositionEnabled? true : false;
}


//////////////////////////////////////////////////////////////////////
// Name: Check_HRFail( HRESULT, const char )
//
// Desc: Checks whether the specified HRESULT is a failure, and if so,
// throws it as an exception.
//////////////////////////////////////////////////////////////////////
void CPresentII::Check_HRFail( HRESULT hResult, const char *sMethod )
{
    if( FAILED(hResult) )
    {
        WriteToLog( TEXT( "%s failed, error code %s (0x%x)\n" ),
            sMethod,
            m_pD3D->HResultToString( hResult ),
            hResult );

        if( m_pFramework->m_DXGLog.InTestCase() )
            Fail();

        throw hResult;
    }
}


UINT GetBitCountForFormat
(
    FORMAT Format
)
{
    DWORD dwBPP = 0;

    switch ( Format.d3dfFormat )
    {
        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
        {
            dwBPP = 16;
        } break;


        case D3DFMT_X8R8G8B8:
        case D3DFMT_A2R10G10B10:
        {
            dwBPP = 32;
        } break;


        default:
        {
        } break;
    };

    return dwBPP;
};


//////////////////////////////////////////////////////////////////////
// Name: HandlePresentReturnCode( HRESULT )
// Desc: handle the results from Present and PresentEx
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::HandlePresentReturnCode( HRESULT hrPresent )
{
    switch( hrPresent )
    {
    case S_PRESENT_MODE_CHANGED:
        if( m_pRenderingDevice->GetInterface() < 9.1f )
        {
            WriteToLog( _T( "Unexpected result S_PRESENT_MODE_CHANGED for legacy d3d9 interface!" ) );
        }
        else
        {
            WriteToLog( _T( "Unexpected result S_PRESENT_MODE_CHANGED; either driver failed to change the display mode or another application changed the display mode!" ) );
            WriteToLog( _T( "Attempted mode: Width = %d, Height = %d, Refresh = %d, Format = %s\n" ), 
                                m_vecPresentParameters[m_nIndexPresentParameters].uWidth,
                                m_vecPresentParameters[m_nIndexPresentParameters].uHeight,
                                m_vecPresentParameters[m_nIndexPresentParameters].uFullscreenRefresh,
                                FORMAT2STRING( m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat )
                        );
        }
        Abort();
        return E_ABORT;
    case S_PRESENT_OCCLUDED:
        if( m_pRenderingDevice->GetInterface() < 9.1f )
        {
            WriteToLog( _T( "Unexpected result S_PRESENT_OCCLUDED for legacy d3d9 interface!" ) );
        }
        else
        {
            WriteToLog( _T( "Unexpected result S_PRESENT_OCCLUDED; another application occludes the present window, please close and rerun the test!" ) );
        }
        Abort();
        return E_ABORT;
    case D3DERR_OUTOFVIDEOMEMORY:
        {
            DWORD dwBytesPerPixel = GetBitCountForFormat( SuppressAlphaChannel( m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat ) );
            // this might be acceptable if a primary is larger that 12Mb (only 4 would fit in a 64Meg card
            if( dwBytesPerPixel * m_vecPresentParameters[m_nIndexPresentParameters].uWidth * m_vecPresentParameters[m_nIndexPresentParameters].uHeight < 12 * 1024 * 1024 )
            {
                WriteToLog( _T( "Present returned OUTOFVIDEOMEMORY, and the swapchain is smaller than 12Mb. Failing." ) );
                return hrPresent;
            }
            else
            {
                WriteToLog( _T( "Present returned OUTOFVIDEOMEMORY, but the swapchain was larger than 12Mb. Skipping." ) );
                Skip();
                return E_OUTOFMEMORY;
            }
        }
    default:
        return hrPresent;
    }
}


//////////////////////////////////////////////////////////////////////
// Name: SuppressAlphaChannel( FORMAT )
// Desc: converts alpha formats into non-alpha formats
//////////////////////////////////////////////////////////////////////
FORMAT CPresentII::SuppressAlphaChannel( FORMAT fmtWithAlpha )
{
    switch( fmtWithAlpha.d3dfFormat )
    {
    case D3DFMT_X8R8G8B8:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_R5G6B5:
    case D3DFMT_A2R10G10B10:
        return fmtWithAlpha;
    case D3DFMT_A8R8G8B8:
        return FMT_X8R8G8B8;
    case D3DFMT_A1R5G5B5:
        return FMT_X1R5G5B5;
    default:
        return FMT_UNKNOWN;
    }
}


//////////////////////////////////////////////////////////////////////
// Name: FailTestCase()
// Desc: fail & end the testcase
//////////////////////////////////////////////////////////////////////
bool CPresentII::FailTestCase()
{
    // clean the error code so the framework won't report error outside of test
    GetLastError();

    ASSERT( m_pFramework->m_DXGLog.InTestCase() );
    Fail();
    EndTestCase();
    return false;
}


//////////////////////////////////////////////////////////////////////
// Name:    CreateRect( int, int, int )
// Desc:    build a rect from scale and offset constants
//////////////////////////////////////////////////////////////////////
RECT CPresentII::CreateRect( int nXScale, int nYScale, int nOffset )
{
    RECT rect;
    rect.left    = round( c_Scale[nXScale] * c_Offsets[nOffset].left );
    rect.top     = round( c_Scale[nYScale] * c_Offsets[nOffset].top );
    rect.right   = round( c_Scale[nXScale] * c_Offsets[nOffset].right );
    rect.bottom  = round( c_Scale[nYScale] * c_Offsets[nOffset].bottom );
    return rect;
}


//////////////////////////////////////////////////////////////////////
// Name:    CreateRect( int, int, int, int )
// Desc:    build a rect from the parameters
//////////////////////////////////////////////////////////////////////
RECT CPresentII::CreateRect( int left, int top, int right, int bottom )
{
    RECT rect;
    rect.left    = left;
    rect.top    = top;
    rect.right    = right;
    rect.bottom    = bottom;
    return rect;
}


//////////////////////////////////////////////////////////////////////
// Name:    CreateRect( RECT, int, int )
// Desc:    scale the width and height and offset by the rect parameter
//////////////////////////////////////////////////////////////////////
RECT CPresentII::CreateRect( RECT rectScale, int nWidth, int nHeight )
{
    RECT rect;
    rect.left    = round( nWidth * rectScale.left / 100.0f );
    rect.top    = round( nHeight * rectScale.top / 100.0f );
    rect.right    = round( nWidth * rectScale.right / 100.0f );
    rect.bottom = round( nHeight * rectScale.bottom / 100.0f );
    return rect;
}


//////////////////////////////////////////////////////////////////////
// Name: CheckDeviceFormat( FORMAT )
// Desc: check if the adapter supports backbuffer fmt
//////////////////////////////////////////////////////////////////////
bool CPresentII::CheckDeviceFormat( FORMAT fmt )
{
    char sBBFormat[32], sFBFormat[32];
    StringCchCopy( sBBFormat, 32, FORMAT2STRING( fmt.d3dfFormat ) );
    StringCchCopy( sFBFormat, 32, FORMAT2STRING( m_fmtFrontBuffer.d3dfFormat ) );
        
    // check for device capability to support the selected backbuffer format
    if( FAILED( m_pD3D->CheckDeviceType(    m_pRenderingDevice->GetAdapterID(),
                                            m_pRenderingDevice->GetDevType(),
                                            m_fmtFrontBuffer,
                                            fmt,
                                            !m_pRenderingDevice->IsFullscreen() )
                )
        )
    {
        WriteToLog( _T( "CheckDeviceType failed. The driver doesn't support rendering to %s.\n" ),
                        sBBFormat
                         );
        return false;
    }
    else
    {
        if( SUCCEEDED( m_pD3D->CheckDeviceFormatConversion( m_pRenderingDevice->GetAdapterID(), 
                                                            m_pRenderingDevice->GetDevType(),
                                                            fmt,
                                                            m_fmtFrontBuffer
                                                        )
                    )
            )
            WriteToLog( _T( "Color-Converting from %s to %s is HW accelerated\n" ),
                            sBBFormat,
                            sFBFormat 
                    );
        else
        {
            WriteToLog( _T( "Color-Converting from %s to %s is SW emulated\n" ),
                            sBBFormat,
                            sFBFormat 
                    );

            if( DEVICETYPE_REF != m_pRenderingDevice->GetDevType() )
            {
                WriteToLog( _T("The test won't run SW emulation for HAL\n") );
                return false;
            }
        }
    }

    return true;
}


//////////////////////////////////////////////////////////////////////
// Name:    CreateDirtyRegion( vector<RegionDescription>& )
// Desc:    create a region from the parameters
//////////////////////////////////////////////////////////////////////
RGNDATA* CPresentII::CreateDirtyRegion( vector<RegionDescription>& vecRegions )
{
    char *pDirtyRegion = NULL;
    if( !vecRegions.empty() )
    {
        pDirtyRegion = new char[sizeof RGNDATAHEADER + sizeof RECT * vecRegions.size()];
        ((RGNDATA*)pDirtyRegion)->rdh.dwSize    = sizeof RGNDATAHEADER;
        ((RGNDATA*)pDirtyRegion)->rdh.iType        = RDH_RECTANGLES;
        ((RGNDATA*)pDirtyRegion)->rdh.nCount    = vecRegions.size();
        ((RGNDATA*)pDirtyRegion)->rdh.nRgnSize    = sizeof RECT * vecRegions.size();

        for( vector<RegionDescription>::iterator itRegionRect = vecRegions.begin(); itRegionRect < vecRegions.end(); itRegionRect++ )
        {
            RECT rect = (*itRegionRect).bAbsoluteRect?
                    (*itRegionRect).rect:
                    CreateRect( (*itRegionRect).indexXScale, (*itRegionRect).indexYScale, (*itRegionRect).indexOffset );
            int nOffsetRect = (int)(itRegionRect - vecRegions.begin());
            memcpy( ((RGNDATA*)pDirtyRegion)->Buffer + sizeof RECT * nOffsetRect, &rect, sizeof RECT );
        }
    }
    return (RGNDATA*)pDirtyRegion;
}


//////////////////////////////////////////////////////////////////////
// Name:    round( float )
// Desc:    rounds the float to the closest int
//////////////////////////////////////////////////////////////////////
int CPresentII::round( float f )
{
    int iResult;
    //iResult = f >= 0? (int)(f + 0.5f):(int)(f - 0.5f);
    iResult = (int)ceil(f);

    return iResult;
}

//////////////////////////////////////////////////////////////////////
// Name:    InitBackBufferRects( vector<RECT>& )
// Desc:    build a rect from scale.
//////////////////////////////////////////////////////////////////////
void CPresentII::InitBackBufferRects( vector<RECT>& vecRects, int nWidth, int nHeight )
{
    vecRects.clear();

    RECT rectScale;
    rectScale.top = 0;
    rectScale.left = 0;

    //
    // Larger 
    //
    rectScale.right = 200;
    rectScale.bottom = 200;
    vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // double area

    rectScale.right = 200;
    rectScale.bottom = 100;
    vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // double width, full height
    
    rectScale.right = 100;
    rectScale.bottom = 200;
    vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // full width, double height
    
    //
    // Bug in verification code for device clip
    //
    //rectScale.right = 133;
    //rectScale.bottom = 133;
    //vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // 1 + third width/height

    rectScale.right = 100;
    rectScale.bottom = 400;
    vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // vertical strip
    
    rectScale.right = 400;
    rectScale.bottom = 100;
    vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // horizontal strip

    
    //
    // Smaller
    //
    rectScale.right = 100;
    rectScale.bottom = 100;
    vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // full area
    
    rectScale.right = 50;
    rectScale.bottom = 50;
    vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // half width, half height
   
    rectScale.right = 100;
    rectScale.bottom = 50;
    vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // full width, half height
    
    rectScale.right = 50;
    rectScale.bottom = 100;
    vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // half width, full height
   
    rectScale.right = 33;
    rectScale.bottom = 33;
    vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // third width/height
   
    //
    // Bug in verification code for device clip
    //
    //rectScale.right = 100;
    //rectScale.bottom = 25;
    //vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // vertical strip
    
    //
    // Bug in verification code for device clip
    //
    //rectScale.right = 25;
    //rectScale.bottom = 100;
    //vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // horizontal strip

    
    //
    // Problems verifying small textures
    //
    //rectScale.right = 4;
    //rectScale.bottom = 4;
    //vecRects.push_back( CreateRect( rectScale, nWidth, nHeight ) ); // full area
 
   // vecRects.push_back( CreateRect( 0, 0, nWidth - 107, nHeight - 71 ) );// unaligned rect smaller
  //  vecRects.push_back( CreateRect( 0, 0, nWidth + 107, nHeight + 71 ) );// unaligned rect larger
}


//////////////////////////////////////////////////////////////////////
// Name:    InitPresentRects( vector<RECT>& )
// Desc:    build a rect from scale and offset constants
//////////////////////////////////////////////////////////////////////
void CPresentII::InitPresentRects( vector<RECT>& vecRects )
{
    vecRects.clear();

    vecRects.push_back( CreateRect( 0, 0, 0, 0 ) );        // NULL
    vecRects.push_back( CreateRect( 1, 1, 1 ) );        // full area, origin
    vecRects.push_back( CreateRect( 1, 1, 6 ) );        // full area, offset on top left
    vecRects.push_back( CreateRect( 1, 1, 9 ) );        // full area, offset on bottom right
    vecRects.push_back( CreateRect( 1, 1, 7 ) );        // full area, offset top right
    vecRects.push_back( CreateRect( 1, 1, 8 ) );        // full area, offset bottom left
    vecRects.push_back( CreateRect( 1, 1, 4 ) );        // full area, offset top
    vecRects.push_back( CreateRect( 1, 1, 5 ) );        // full area, offset bottom
    vecRects.push_back( CreateRect( 1, 1, 2 ) );        // full area, offset left
    vecRects.push_back( CreateRect( 1, 1, 3 ) );        // full area, offset right
    vecRects.push_back( CreateRect( 2, 2, 1 ) );        // half width, half height, origin
#ifndef FAST_TEST
    vecRects.push_back( CreateRect( 1, 2, 1 ) );        // full width, half height, origin
    vecRects.push_back( CreateRect( 2, 1, 1 ) );        // half width, full height, origin
#endif
    vecRects.push_back( CreateRect( 2, 2, 6 ) );        // half width/height, offset on top left
    vecRects.push_back( CreateRect( 4, 4, 9 ) );        // third width/height, offset on bottom right
    //vecRects.push_back( CreateRect( 6, 1, 1 ) );        // vertical strip, origin
    //vecRects.push_back( CreateRect( 1, 6, 1 ) );        // horizontal strip, origin
    vecRects.push_back( CreateRect( 3, 3, 6 ) );        // double height/width, offset top left
    vecRects.push_back( CreateRect( 5, 5, 9 ) );        // 1 & half height/width, offset bottom right
    vecRects.push_back( CreateRect( 15, 15, 122, 86 ) );// unaligned rect
}


//////////////////////////////////////////////////////////////////////
// Name:    InitPresentSourceRect()
// Desc:    build the vector of present source rects to test
//////////////////////////////////////////////////////////////////////
void CPresentII::InitPresentSourceRect()
{
    InitPresentRects( m_vecRectPresentSource );
}


//////////////////////////////////////////////////////////////////////
// Name:    InitPresentDestRect()
// Desc:    build the vector of present dest rects to test
//////////////////////////////////////////////////////////////////////
void CPresentII::InitPresentDestRect()
{
    InitPresentRects( m_vecRectPresentDest );
}

    
//////////////////////////////////////////////////////////////////////
// Name:    InitDirtyRegions()
// Desc:    build the vector of regions to test
//////////////////////////////////////////////////////////////////////
void CPresentII::InitDirtyRegions()
{
    vector<RegionDescription> vecRegDescr;

    m_vecRegData.clear();

    // NULL
    vecRegDescr.clear();
    m_vecRegData.push_back( CreateDirtyRegion( vecRegDescr ) );
}


//////////////////////////////////////////////////////////////////////
// Name:    InitPrivateData()
// Desc:    build the vector of special data
//////////////////////////////////////////////////////////////////////
void CPresentII::InitPrivateData()
{
    m_vecPrivateData.clear();
    // add one meaningless value
    m_vecPrivateData.push_back(0);
}


//////////////////////////////////////////////////////////////////////
// Name:    InitWindows()
// Desc:    build the vector of windows (dialog boxes or clipping windows) to test
//////////////////////////////////////////////////////////////////////
void CPresentII::InitWindows()
{
    m_vecIntWindows.clear();
    m_vecWindowDescription.clear();
}


//////////////////////////////////////////////////////////////////////
// Name:    InitTextures()
// Desc:    build the vector of textures to test
//////////////////////////////////////////////////////////////////////
void CPresentII::InitTextures()
{
    m_vecStringTexture.clear();
    m_vecStringTexture.push_back( "" );    // generated texture
}


//////////////////////////////////////////////////////////////////////
// Name:    InitPresentParameters()
// Desc:    build the vector of present parameters to test
//////////////////////////////////////////////////////////////////////
void CPresentII::InitPresentParameters()
{
    PRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    GetFrontBufferRect();

    m_vecPresentParameters.clear();
    m_vecPresentParametersRotation.clear();


    d3dpp.bWindowed                       = true;
    d3dpp.uBackBufferCount                = 1;
    d3dpp.SwapEffect                      = m_SwapEffect;
    d3dpp.hDeviceWindow                   = m_hWndReset;
    d3dpp.dwFlags                         = 0;
    d3dpp.uPresentInterval                = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.uWidth                          = m_vecAdapterMonitors[0].m_rectFrontBuffer.right - m_vecAdapterMonitors[0].m_rectFrontBuffer.left;
    d3dpp.uHeight                         = m_vecAdapterMonitors[0].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[0].m_rectFrontBuffer.top;
    d3dpp.BackBufferFormat                = m_fmtFrontBuffer;
    d3dpp.uFullscreenRefresh              = D3DPRESENT_RATE_DEFAULT;
    d3dpp.AutoDepthStencilFormat          = FMT_UNKNOWN;
    d3dpp.bAutoDepthStencil               = false;
    d3dpp.MultiSample                     = MULTISAMPLE_NONE;

    d3dpp.bCachedBackBuffers              = false;
    d3dpp.DesktopFormat                   = m_fmtFrontBuffer;

    m_vecPresentParameters.push_back( d3dpp );
}


//////////////////////////////////////////////////////////////////////
// Name:    InitPresentWindows()
// Desc:    build the vector of Present windows to test
//////////////////////////////////////////////////////////////////////
void CPresentII::InitPresentWindows()
{
    m_vecPresentWindow.clear();
    m_vecPresentWindow.push_back( NULL );
    m_vecPresentWindow.push_back( m_hWndOverride );
}


//////////////////////////////////////////////////////////////////////
// Name:    InitFlagsEx()
// Desc:    LONGHORN ONLY: build the vector of Ex flags to test
//////////////////////////////////////////////////////////////////////
void CPresentII::InitFlagsEx()
{
    m_vecFlagsEx.clear();
}

//////////////////////////////////////////////////////////////////////
// Name:    InitDeviceReleaseBeforeVerify()
// Desc:    Used by FlipEx Tests to validate DWM frame reuse after device change.
//////////////////////////////////////////////////////////////////////
void CPresentII::InitDeviceReleaseBeforeVerify()
{
    m_vecDeviceReleaseBeforeVerify.clear();
    m_vecDeviceReleaseBeforeVerify.push_back( NoRelease );

}

//////////////////////////////////////////////////////////////////////
// Name:    InitWindowTrees()
// Desc:    Used by Create Window heirarchy
//////////////////////////////////////////////////////////////////////
void CPresentII::InitWindowTrees()
{
    m_vecWindowTrees.clear();
}

//////////////////////////////////////////////////////////////////////
// Name:    GrabEraseColor()
// Desc:    grab the erase color, could be altered by color conversion
//////////////////////////////////////////////////////////////////////
bool CPresentII::GrabEraseColor()
{
    HRESULT hResult;

    //
    // Sync color with the background brush color 
    //
    if( TEST_CLIPPINGWINDOWS == m_TestType )
    {
        m_COLOR_WINDOW.color    = 0xffffffff;
    }
    else if( TEST_DEVICECLIP == m_TestType )
    {
        m_COLOR_WINDOW.color    = 0xff808080;    
    }
 
    
    if(m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseSwapChains)
    {
        for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
        {

            LPSURFACE pBBSurface = NULL;

            Check_HRFail( m_vecAdapterMonitors[uSwapChain].m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBBSurface ), "GetBackBuffer" );
            Check_HRFail( m_pRenderingDevice->SetRenderTarget( (DWORD)0, pBBSurface ), "SetRenderTarget" );

            if( FAILED( hResult = m_pRenderingDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, COLOR_ERASE.color, 1.0f, 0L ) ) )
            {
                WriteToLog( TEXT( "Clear failed, error code %s (0x%x)\n" ),
                    m_pD3D->HResultToString( hResult ),
                    hResult );
                PrepareNextTest();
                return FailTestCase();
            }
            hResult = m_vecAdapterMonitors[uSwapChain].m_pSwapChain->Present( NULL, NULL, m_vecPresentWindow[m_nIndexPresentWindow], NULL );
            RELEASE(pBBSurface);
        }
    }
    else
    {
        // Clear the viewport
        if( FAILED( hResult = m_pRenderingDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, COLOR_ERASE.color, 1.0f, 0L ) ) )
        {
            WriteToLog( TEXT( "Clear failed, error code %s (0x%x)\n" ),
                m_pD3D->HResultToString( hResult ),
                hResult );
            PrepareNextTest();
            return FailTestCase();
        }

        hResult = m_pRenderingDevice->Present( NULL, NULL, m_vecPresentWindow[m_nIndexPresentWindow], NULL );
    }

    HRESULT handledCode = HandlePresentReturnCode( hResult );
    switch( handledCode )
    {
        case E_ABORT:
        case E_OUTOFMEMORY:
        default:
            if( FAILED( hResult ) || FAILED(handledCode))
            {
                WriteToLog( TEXT( "In GrabEraseColor Present failed, error code %s (0x%x). HandlePresentReturnCode code %s (0x%x)\n" ),
                    m_pD3D->HResultToString( hResult ),
                    hResult,
                    m_pD3D->HResultToString( handledCode ),
                    handledCode);

                PrepareNextTest();
                return FailTestCase();
            }    
    };

    for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
    {
        if( IsRectEmpty( &( m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer ) ) )
            continue;

        LPSURFACE pBBSurface = NULL;
        if(m_vecPresentParameters[m_nIndexPresentParameters + uAdapter].dwFlags & PRESENTFLAG_LOCKABLE_BACKBUFFER)
        {
            Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBBSurface ), "GetBackBuffer" );
            // flush the pipeline
            LOCKED_RECT pLRect;
            Check_HRFail( pBBSurface->LockRect( &pLRect, NULL, LOCK_NOSYSLOCK ), "LockRect" );
            Check_HRFail( pBBSurface->UnlockRect(), "UnlockRect" );
            pBBSurface->Release();
        }

        Sleep( 25 );
		::DwmFlush();
  
		Sleep( GetDebugPause() );

        RECT rectFrontBuffer = m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer;
        
        //
        //  test assumes only one swapchain if windowes
        //
        if( m_vecPresentParameters[m_nIndexPresentParameters].bWindowed )
        {

            //
            // BUG: This will not work for multihead windowed rotated cases. 
            // In rotated case Frontbuffer is in non rotated space and the monitor is rotated. 
            //
                 
            // offset frontbuffer rect by monitor top, left corner
            OffsetRect( &rectFrontBuffer, 
                -(LONG)( m_fDPIFactor * m_vecAdapterMonitors[uAdapter].rcMonitor.left ), 
                -(LONG)( m_fDPIFactor * m_vecAdapterMonitors[uAdapter].rcMonitor.top ) 
            );
        }
        else
        {
            //
            // In fullscreen we dont care about the monitor offsets Top,Left of the buffer is always (0,0)
            //
            UINT bufferWidth = (UINT)( rectFrontBuffer.right - rectFrontBuffer.left);
            UINT bufferHeight = (UINT)( rectFrontBuffer.bottom - rectFrontBuffer.top);
            rectFrontBuffer.left = 0;
            rectFrontBuffer.top = 0;
            rectFrontBuffer.right = bufferWidth;
            rectFrontBuffer.bottom = bufferHeight;
        }

       
        MYD3DCOLOR expectedColor;
        expectedColor.color = COLOR_ERASE.color;
        if(IsDeviceClip(0, uAdapter, m_nIndexPresentParameters))
        {     
            if(IsRestrictToMonitor(0, uAdapter, m_nIndexPresentParameters) && m_bDWMEnabled)
            {
                
                // The present above for the clear doesn't have flags set so D3DPRESENTFLAG_VIDE0 is not
                // in effect. The present will not be restricted and the clear will succeed.
                // The background wont be black as expected with the flag and DWM on.
                
                //expectedColor.color = COLOR_BLACK.color;
                expectedColor.color = COLOR_ERASE.color;
            }
            else
            {
                expectedColor.color = m_COLOR_WINDOW.color;
            }

        }
           

        LOCKED_RECT    lrFrontBuffer;

        //
        // Check if the erase color is in the ballpark.  If not sleep for a refresh and try to snap it again.
        //
        BYTE colorError = (BYTE) ( 255 * (100 - m_iConformance) / 100.0f );
        UINT numberOfChecks = 4;
        bool colorFound = false;
        for(UINT i = 0; i < numberOfChecks; i++)
        {
            Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pSwapChain->GetFrontBufferData( m_vecAdapterMonitors[uAdapter].m_pFrontBuffer ), "GetFrontBufferData" );
            Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pFrontBuffer->LockRect( &lrFrontBuffer, &( rectFrontBuffer ), D3DLOCK_READONLY ), "LockRect (FrontBuffer COLOR ERASE)" );

            if(m_TestType != TEST_DEVICECLIP )
            {
                colorFound = true;
                break;    
            }
            else
            {
                m_COLOR_ERASE.color    = *(DWORD*)( (UINT_PTR)lrFrontBuffer.pBits );

                if( (( m_COLOR_ERASE.blue > (expectedColor.blue - colorError) ) && (m_COLOR_ERASE.blue <(expectedColor.blue + colorError) ) &&
                    (m_COLOR_ERASE.red > (expectedColor.red - colorError) ) && (m_COLOR_ERASE.red <(expectedColor.red + colorError) ) &&
                    (m_COLOR_ERASE.green > (expectedColor.green - colorError) ) && (m_COLOR_ERASE.green <(expectedColor.green + colorError) )) )
                {
                    colorFound = true;
                    break;    
                }
                else
                {
                    DISPLAYMODE displaymode = {0};
                    float refreshInterval = 0;

                    if( FAILED(m_pD3D->GetAdapterDisplayMode(uAdapter, &displaymode)) )
                    {
                        refreshInterval = 100;
                    }
                    else
                    {
                        refreshInterval = 1000.0f / (float)(displaymode.dwRefreshRate);
                    }

                    WriteToLog( _T( "ERASE COLOR NOT MATCHED uAdapter %d m_COLOR_ERASE.color (%d,%d,%d). expectedColor.color (%d,%d,%d).\n" ), uAdapter,m_COLOR_ERASE.red, m_COLOR_ERASE.green,m_COLOR_ERASE.blue, expectedColor.red,expectedColor.green,expectedColor.blue);   
                    if(i < numberOfChecks )
                    {
                        WriteToLog( _T( "GrabEraseColor Attempt %u. Sleep for %f\n" ), i + 1, refreshInterval);   

                        //
                        // + 1 for rounding.
                        //
						if( i < numberOfChecks - 1 )
							Sleep((UINT)refreshInterval + 1);
						else
							Sleep( 2000 );
                    }

                    Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pFrontBuffer->UnlockRect(), "UnlockRect" );
                }
            }
        }       
         
        if(!colorFound)
        {
            WriteToLog( _T("Erase color match not found. Failing test case\n") );
            PrepareNextTest();
            return FailTestCase();
        }
        
        RELEASE( m_vecAdapterMonitors[uAdapter].m_pClearBuffer );
        if( FAILED( hResult = m_vecAdapterMonitors[uAdapter].m_pDevice->CreateImageSurface(
                        m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.right - m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.left,
                        m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.top,
                        FMT_A8R8G8B8,
                        &( m_vecAdapterMonitors[uAdapter].m_pClearBuffer ) ) 
                    )
            )
        {
            WriteToLog( _T( "CreateImageSurface failed, return code %s (0x%x)\n" ),
                m_pD3D->HResultToString( hResult ),
                hResult );
            PrepareNextTest();
            return FailTestCase();
        }
        LOCKED_RECT    lrClearBuffer;
        Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pClearBuffer->LockRect( &lrClearBuffer, NULL, 0 ), "LockRect (ClearBuffer)" );

        for( LONG uRow = 0; uRow < m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.top; uRow++ )
        {
            BYTE *pFrontBuffer = (BYTE*)lrFrontBuffer.pBits + uRow * lrFrontBuffer.iPitch;
            BYTE *pClearBuffer = (BYTE*)lrClearBuffer.pBits + uRow * lrClearBuffer.iPitch;

            _ASSERT( ( ( m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.right - m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.left ) << 2 ) <= lrClearBuffer.iPitch &&
                     ( ( m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.right - m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.left ) << 2 ) <= lrFrontBuffer.iPitch );
            
            memcpy( pClearBuffer, pFrontBuffer, ( m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.right - m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.left ) << 2 );
        }

        Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pClearBuffer->UnlockRect(), "UnlockRect" );
        Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pFrontBuffer->UnlockRect(), "UnlockRect" );
    }

    return true;
}


//////////////////////////////////////////////////////////////////////
// Name:    GetFrontBufferRect()
// Desc:    get window and monitor info to determine the backbuffer position on the desktop
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::GetFrontBufferRect()
{
    for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
    {
        // get window information
        WINDOWINFO wi;
        LPVOID lpError = NULL;

        ZeroMemory( &wi, sizeof WINDOWINFO );
        wi.cbSize = sizeof WINDOWINFO;

        // for multihead swapchains, we must grab window associated with the swapchain
        HWND hWnd = m_uNumberSwapChains > 1 && m_nIndexPresentParameters + uAdapter < m_vecPresentParameters.size()?
                        m_vecPresentParameters[m_nIndexPresentParameters + uAdapter].hDeviceWindow :
                        m_hWndPresent;

        if( !GetWindowInfo( hWnd, &wi ) )
        {
            FormatMessage(    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, ::GetLastError(),
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpError, 0, NULL);
            WriteToLog( _T( "GetFrontBufferRect - GetWindowInfo failed:%s\n" ), lpError );
            LocalFree( lpError );
            return E_FAIL;
        }

        // adjust the returned rect struct into device coords
        /*HMONITOR hMonitor = MonitorFromWindow( m_hWndPresent, MONITOR_DEFAULTTONEAREST );
        if( !hMonitor )
        {
            WriteToLog( _T( "GetFrontBufferRect - MonitorFromWindow failed\n" ) );
            return E_FAIL;
        }*/

        MONITORINFO mi;
        ZeroMemory( &mi, sizeof( mi ) );
        mi.cbSize = sizeof( mi );
        if( !GetMonitorInfo( m_vecAdapterMonitors[uAdapter].hMonitor, &mi ) )
        {
             FormatMessage(    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, ::GetLastError(),
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpError, 0, NULL);
            WriteToLog( _T( "GetFrontBufferRect - GetMonitorInfo failed:%s\n" ), lpError );
            LocalFree( lpError );
            return E_FAIL;
        }

        if( m_pRenderingDevice->GetPresParams()->bWindowed )
        {
            if( 0 == uAdapter )
            {
                UINT MonitorWidth = (UINT)( mi.rcMonitor.right - mi.rcMonitor.left );
                UINT MonitorHeight = (UINT)( mi.rcMonitor.bottom - mi.rcMonitor.top );
                m_fDPIFactor = 1.0f;
                if( MonitorWidth != m_vecAdapterMonitors[uAdapter].m_modeDisplay.dwWidth && MonitorHeight != m_vecAdapterMonitors[uAdapter].m_modeDisplay.dwHeight )
                {
                    m_fDPIFactor = m_vecAdapterMonitors[uAdapter].m_modeDisplay.dwWidth / (float)MonitorWidth;

                    float f100DPIFactor = m_fDPIFactor * 100.0f;
                    int i100DPIFactor = (int)( f100DPIFactor + 0.5f);

                    m_fDPIFactor = i100DPIFactor / 100.0f;
                }
            }

            m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.left = (LONG)( m_fDPIFactor * (float)( wi.rcClient.left ) );
            m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.top = (LONG)( m_fDPIFactor * (float)( wi.rcClient.top ) );
            m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.right = (LONG)( m_fDPIFactor * (float)( wi.rcClient.right ) );
            m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.bottom = (LONG)( m_fDPIFactor * (float)( wi.rcClient.bottom ) );

            mi.rcMonitor.left = (LONG)( m_fDPIFactor * (float)mi.rcMonitor.left );
            mi.rcMonitor.right = (LONG)( m_fDPIFactor * (float)mi.rcMonitor.right );
            mi.rcMonitor.top = (LONG)( m_fDPIFactor * (float)mi.rcMonitor.top );
            mi.rcMonitor.bottom = (LONG)( m_fDPIFactor * (float)mi.rcMonitor.bottom );

            IntersectRect(  &( m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer ), 
                            &( m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped ),
                            &mi.rcMonitor
                        );
        }
        else
        {
            DWORD dwDisplayOrientation = GetGDIDisplayOrientation();
            
            //
            // If the monitor is rotated, unrotate to keep frontbuffer unrotated
            //
            if( ( m_nIndexPresentParameters + uAdapter ) < m_vecPresentParameters.size() &&
                ( DMDO_90 == dwDisplayOrientation || DMDO_270 == dwDisplayOrientation ) &&
                0 != ( D3DPRESENTFLAG_NOAUTOROTATE & m_vecPresentParameters[m_nIndexPresentParameters + uAdapter].dwFlags ) )
            {
                std::swap( mi.rcMonitor.left, mi.rcMonitor.top );
                std::swap( mi.rcMonitor.right, mi.rcMonitor.bottom );
            }

            m_fDPIFactor = 1.0f;
            m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped = 
                m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer = mi.rcMonitor;
        }
    }

    // get window dimensions
    m_dwWindowWidth        = m_vecAdapterMonitors[0].m_rectFrontBufferUnclipped.right - m_vecAdapterMonitors[0].m_rectFrontBufferUnclipped.left;
    m_dwWindowHeight    = m_vecAdapterMonitors[0].m_rectFrontBufferUnclipped.bottom - m_vecAdapterMonitors[0].m_rectFrontBufferUnclipped.top;

    return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:    CapsCheck()
// Desc:    check caps
//////////////////////////////////////////////////////////////////////
bool CPresentII::CapsCheck()
{
    return CD3DTest::CapsCheck();
}


//////////////////////////////////////////////////////////////////////
// Name:    ClearBackBuffers()
// Desc:    true if the backbuffer(s) should be cleared
//////////////////////////////////////////////////////////////////////
bool CPresentII::ClearBackBuffers()
{
    return true;
}


//////////////////////////////////////////////////////////////////////
// Name:    UpdateBackBuffer()
// Desc:    true if the backbuffer contents should be updated in the REF image
//////////////////////////////////////////////////////////////////////
bool CPresentII::UpdateBackBuffer()
{
    return true;
}

//////////////////////////////////////////////////////////////////////
// Name:    RenderScene( UINT, int )
// Desc:    renders the scene
//////////////////////////////////////////////////////////////////////
void CPresentII::RenderScene( UINT uSwapChain, int indexBB )
{
    // Begin the scene
    Check_HRFail( m_pRenderingDevice->BeginScene(), "BeginScene");

    // set default render states
    Check_HRFail(m_pRenderingDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1), "SetTextureStageState");
    Check_HRFail(m_pRenderingDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE), "SetTextureStageState");
    Check_HRFail(m_pRenderingDevice->SetRenderState( D3DRS_LIGHTING, FALSE), "SetRenderState");

    // draw
    Check_HRFail( m_pRenderingDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2), "DrawPrimitive");

    // End the scene
    Check_HRFail( m_pRenderingDevice->EndScene(), "EndScene");
    if ( indexBB >= 0)
    {
        LPSURFACE pBBSurface = NULL;
        HRESULT hr = S_OK;

        if (m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseSwapChains)
        {
            hr = m_vecAdapterMonitors[uSwapChain].m_pSwapChain->GetBackBuffer(indexBB, D3DBACKBUFFER_TYPE_MONO, &pBBSurface);
        }
        else
        {
            hr = m_pRenderingDevice->GetBackBuffer( uSwapChain, indexBB, D3DBACKBUFFER_TYPE_MONO, &pBBSurface);
        }

        if ( SUCCEEDED( hr) )
        { 
            if (m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].dwFlags & PRESENTFLAG_LOCKABLE_BACKBUFFER)
            {
                // flush the pipeline
                LOCKED_RECT pLRect;
                if ( SUCCEEDED( pBBSurface->LockRect( &pLRect, NULL, LOCK_NOSYSLOCK)))
                    pBBSurface->UnlockRect();
            }

            if ( FAILED( m_pRenderingDevice->D3DXLoadSurfaceFromSurface(    
                m_SwapChainData[uSwapChain].m_vecBB[indexBB],
                NULL,
                NULL,
                pBBSurface,
                NULL,
                NULL,
                D3DX_FILTER_NONE,
                0)))
                FailTestCase();

            // m_pRenderingDevice->D3DXSaveSurfaceToFile( "BBSurface.bmp", D3DXIFF_BMP, pBBSurface, NULL, NULL );
            // m_pRenderingDevice->D3DXSaveSurfaceToFile( "BBCopy.bmp", D3DXIFF_BMP, pBBCopy, NULL, NULL );

            RELEASE( pBBSurface);
        }
        else
            FailTestCase();
    }
}


//////////////////////////////////////////////////////////////////////
// Name:    FillAdapterMonitorInfo( CDevice *, UINT, ADAPTERMONITORINFO * )
// Desc:    gathers adapter information
//////////////////////////////////////////////////////////////////////
bool CPresentII::FillAdapterMonitorInfo( CDevice *pDevice, UINT uAdapterOffset, ADAPTERMONITORINFO *pAdapterMonitorInfo )
{
    bool bResult = true;
    _ASSERT( NULL != pDevice && NULL != pAdapterMonitorInfo );

    ZeroMemory( pAdapterMonitorInfo, sizeof( ADAPTERMONITORINFO ) );

    DEVICE_CREATION_PARAMETERS deviceCreationParameters;
    pDevice->GetCreationParameters( &deviceCreationParameters );

    pAdapterMonitorInfo->uAdapter = deviceCreationParameters.AdapterOrdinal + uAdapterOffset;
    pAdapterMonitorInfo->m_pDevice = pDevice;
    bResult &= SUCCEEDED( pAdapterMonitorInfo->m_pDevice->GetSwapChain( uAdapterOffset, &( pAdapterMonitorInfo->m_pSwapChain ) ) );
    if(!bResult)
    {
        WriteToLog(_T("CPresentII::FillAdapterMonitorInfo GetSwapChain bResult=%d"), bResult);
        goto Exit;
    }
    
    bResult &= SUCCEEDED( pAdapterMonitorInfo->m_pSwapChain->GetDisplayMode( &( pAdapterMonitorInfo->m_modeDisplay ) ) );
    if(!bResult)
    {
        WriteToLog(_T("CPresentII::FillAdapterMonitorInfo GetDisplayMode bResult=%d"), bResult);
        goto Exit;
    }

    pAdapterMonitorInfo->hMonitor = m_pD3D->GetAdapterMonitor( pAdapterMonitorInfo->uAdapter );
    
    MONITORINFOEX monitorInfoEx;
    ZeroMemory( &monitorInfoEx, sizeof( MONITORINFOEX ) );
    monitorInfoEx.cbSize = sizeof( MONITORINFOEX );
    if( bResult = ( TRUE == GetMonitorInfo( pAdapterMonitorInfo->hMonitor, &monitorInfoEx ) ) )
    {
        pAdapterMonitorInfo->rcMonitor = monitorInfoEx.rcMonitor;
        pAdapterMonitorInfo->bPrimaryMonitor = ( monitorInfoEx.dwFlags & MONITORINFOF_PRIMARY ) ? true : false;
    }

    if(!bResult)
    {
        WriteToLog(_T("CPresentII::FillAdapterMonitorInfo GetMonitorInfo pAdapterMonitorInfo->hMonitor=%d bResult=%d"),pAdapterMonitorInfo->hMonitor, bResult);
        goto Exit;
    }

    
Exit:

    return bResult;
}


//////////////////////////////////////////////////////////////////////
// Name:    ClearAdapterMonitors()
// Desc:
//////////////////////////////////////////////////////////////////////
void CPresentII::ClearAdapterMonitors()
{
    for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
    {
        RELEASE( m_vecAdapterMonitors[uAdapter].m_pFrontBuffer );
        RELEASE( m_vecAdapterMonitors[uAdapter].m_pClearBuffer );
        RELEASE( m_vecAdapterMonitors[uAdapter].m_pSwapChain );
        // don't free the device for multihead scenarios (it was never allocated in the first place)
        if( 0 != uAdapter && 1 == m_uNumberSwapChains )
        {
            RELEASE( m_vecAdapterMonitors[uAdapter].m_pDevice );
        }
    }
}

//////////////////////////////////////////////////////////////////////
// Name:    InitAdapterMonitors()
// Desc:
//////////////////////////////////////////////////////////////////////
UINT CPresentII::InitAdapterMonitors()
{
    ClearAdapterMonitors();

    HRESULT hResult = S_OK;

    ADAPTERMONITORINFO AdapterMonitorInfo;
    if( !FillAdapterMonitorInfo( m_pRenderingDevice, 0, &AdapterMonitorInfo ) )
    {
        WriteToLog(
            _T( "CPresentII::InitAdapterMonitors. FillAdapterMonitorInfo failed, tests will be skipped\n" )
        );
        return D3DTESTINIT_SKIPALL;
    }
    m_fmtFrontBuffer = AdapterMonitorInfo.m_modeDisplay.Format;

    DEVICE_CREATION_PARAMETERS deviceCreationParameters;
    m_pRenderingDevice->GetCreationParameters( &deviceCreationParameters );
    
  
    PRESENT_PARAMETERS *pPresentParameters = m_pRenderingDevice->GetPresParams();

    m_vecAdapterMonitors.clear();
    m_vecAdapterMonitors.push_back( AdapterMonitorInfo );

    UINT uMaxAdapter = pPresentParameters->bWindowed? m_pD3D->GetAdapterCount() : m_uNumberSwapChains;

    // early out condition, one adapter has already been added
    if( 1 == uMaxAdapter )
        return 0;

    for( UINT uAdapter = 0; uAdapter < uMaxAdapter; uAdapter++ )
    {
        if( uAdapter == deviceCreationParameters.AdapterOrdinal )
            continue;

        UINT uAdapterOffset = 0;
    
        CDevice *pDevice = NULL;
        // multihead tests are fullscreen only so we can't have additional devices.
        // These devices are only for resourcce creation and front buffer capture, they are
        // never presented from.
        if( 1 == m_uNumberSwapChains )
        {
            PRESENT_PARAMETERS presentParameters;
            memcpy( &presentParameters, pPresentParameters, sizeof( PRESENT_PARAMETERS ) );
            
            presentParameters.bWindowed = true;
            presentParameters.uFullscreenRefresh = 0;
            //
            // Make sure this is not FLIPEX because only one FLIPEX swapchain per hwnd.
            //
            if(presentParameters.SwapEffect == SWAPEFFECT_FLIPEX)
            {
                presentParameters.SwapEffect = SWAPEFFECT_DISCARD;
            }

            if( FAILED( hResult = m_pD3D->CreateDevice(
                                                        uAdapter,
                                                        deviceCreationParameters.DeviceType,
                                                        deviceCreationParameters.hFocusWindow,
                                                        deviceCreationParameters.BehaviorFlags,
                                                        &presentParameters,
                                                        &pDevice
                                                    )
                        )
                )
            {
                
                WriteToLog(
                    _T( "CPresentII::InitAdapterMonitors. CreateDevice() returned %s (0x%x)\n" ),
                    m_pD3D->HResultToString(hResult),
                    hResult
                );
                return D3DTESTINIT_SKIPALL;
            }
        }
        else
        {
            pDevice = m_pRenderingDevice;
            uAdapterOffset = uAdapter;
        }

        if( !FillAdapterMonitorInfo( pDevice, uAdapterOffset, &AdapterMonitorInfo ) )
        {
            WriteToLog(
                _T( "CPresentII::InitAdapterMonitors. FillAdapterMonitorInfo failed, tests will be skipped\n" )
            );
            return D3DTESTINIT_SKIPALL;
        }

        m_vecAdapterMonitors.push_back( AdapterMonitorInfo );
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////
// Name:    ChangeDisplaySettings()
// Desc:    change the mode if necessary
//////////////////////////////////////////////////////////////////////
void CPresentII::ChangeDisplaySettings()
{
}


//////////////////////////////////////////////////////////////////////
// Name:    GetGDIDisplayOrientation()
// Desc:    
//////////////////////////////////////////////////////////////////////
DWORD CPresentII::GetGDIDisplayOrientation()
{
    DEVMODE Mode;
    Mode.dmSize = sizeof( DEVMODE );
    EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &Mode );
    return Mode.dmDisplayOrientation;
}

//////////////////////////////////////////////////////////////////////
// Name:    GetGDIDisplayOrientation()
// Desc:    
//////////////////////////////////////////////////////////////////////
DWORD CPresentII::GetGDIDisplayOrientation(UINT adapterID)
{
    DEVMODE Mode;
    Mode.dmSize = sizeof( DEVMODE );
    
    D3DADAPTER_IDENTIFIER9 adapterIndentifier;
    
    HRESULT hr = m_pD3D->GetAdapterIdentifier(
        adapterID,
        0,
        (ADAPTER_IDENTIFIER*)&adapterIndentifier);
    
    if(SUCCEEDED(hr))
    {
        EnumDisplaySettings( adapterIndentifier.DeviceName, ENUM_CURRENT_SETTINGS, &Mode );    
    }
    
    return Mode.dmDisplayOrientation;
}


//////////////////////////////////////////////////////////////////////
// Name:    TestInitialize()
// Desc:    Init the testcase
//////////////////////////////////////////////////////////////////////
UINT CPresentII::TestInitialize()
{
    HRESULT hResult = D3D_OK;
    
    //
    // Sleep for 5 seconds before each test.  Mode changes can take this long to finish
    //
    Sleep(5000);

    SetTestRange( 0, 1 );    // workaround for the silly framework issue with abort in initialize

    m_uNumberSwapChains  =    0 != ( m_pSrcDevice->GetBehaviors() & DEVICECREATE_ADAPTERGROUP_DEVICE ) ?
                                m_pSrcDevice->GetCaps()->NumberOfAdaptersInGroup : 1;
    ASSERT( m_uNumberSwapChains > 0 );
    ASSERT( m_uNumberSwapChains <= MAX_SWAPCHAINS );

    if( m_uNumberSwapChains > MAX_SWAPCHAINS )
        return D3DTESTINIT_SKIPALL;
    //
    // Clone the device
    //
    DEVICE_CREATION_PARAMETERS deviceCreationParameters;
    m_pSrcDevice->GetCreationParameters( &deviceCreationParameters );
    
    //
    // GetCreationParameters returns D3DDeviceType but we need frameworks DWORD DeviceType
    // in order to use in CreateDevice
    //
    DWORD deviceType = ConvertDeviceType(deviceCreationParameters.DeviceType);
    deviceCreationParameters.DeviceType = (D3DDEVTYPE)deviceType;
    
    
    PRESENT_PARAMETERS *pPresentParameters = m_pSrcDevice->GetPresParams();
    
    PRESENT_PARAMETERS *pSrcDevicePresentParameters = new PRESENT_PARAMETERS[m_uNumberSwapChains];
    PRESENT_PARAMETERS *pRenderingDevicePresentParameters = new PRESENT_PARAMETERS[m_uNumberSwapChains];
    
    ZeroMemory( pSrcDevicePresentParameters, sizeof( PRESENT_PARAMETERS)  * m_uNumberSwapChains );
    ZeroMemory( pRenderingDevicePresentParameters, sizeof( PRESENT_PARAMETERS)  * m_uNumberSwapChains );
    
    memcpy( pSrcDevicePresentParameters, pPresentParameters, sizeof( PRESENT_PARAMETERS) * m_uNumberSwapChains  );
    memcpy( pRenderingDevicePresentParameters, pPresentParameters, sizeof( PRESENT_PARAMETERS) * m_uNumberSwapChains );
    
    DWORD behaviors = m_pSrcDevice->GetBehaviors();
   
    //
    // Make sure it is windowed so when it is cloned we can go fullscreen with the clone
    //
    if(pSrcDevicePresentParameters[0].bWindowed == false)
    {
        pSrcDevicePresentParameters[0].bWindowed = true;
        pSrcDevicePresentParameters[0].uFullscreenRefresh = 0;
        m_pSrcDevice->SetBehaviors(behaviors & ~DEVICECREATE_ADAPTERGROUP_DEVICE);

        if( FAILED( hResult = m_pSrcDevice->UpdateDevice(pSrcDevicePresentParameters)))
        {
            WriteToLog(
                _T( "UpdateDevice() returned %s (0x%x)\n" ),
                m_pD3D->HResultToString(hResult),
                hResult
                );
            return false;
        } 

        if( m_pD3D->DetectCaps(m_pSrcDevice) != true)
        {
            WriteToLog("DetectCaps() failed.\n");

            return false;
        }
    }
    else 
    {
        if(pSrcDevicePresentParameters[0].SwapEffect == SWAPEFFECT_FLIPEX)
        {
            
            //
            // Disable SWAPEFFECT_FLIPEX for the default device because only one flipex swapchain at a time
            // can be bound to a window
            //
            pSrcDevicePresentParameters[0].SwapEffect = SWAPEFFECT_DISCARD;
            if( FAILED( hResult = m_pSrcDevice->UpdateDevice(pSrcDevicePresentParameters)))
            {
                WriteToLog(
                    _T( "UpdateDevice() returned %s (0x%x)\n" ),
                    m_pD3D->HResultToString(hResult),
                    hResult
                    );
                return false;
            } 
            
        }   
    }
      
    if( FAILED( hResult = m_pD3D->CreateDevice(
                                deviceCreationParameters.AdapterOrdinal,
                                deviceCreationParameters.DeviceType,
                                m_pFramework->m_hWindow,
                                behaviors,
                                pRenderingDevicePresentParameters,
                                &m_pRenderingDevice
                                )
        )
    )
    {        
        WriteToLog(
            _T( "CreateDevice() returned %s (0x%x)\n" ),
            m_pD3D->HResultToString(hResult),
            hResult
            );
        return false;
    }

    SAFEDELETEA(pSrcDevicePresentParameters);
    SAFEDELETEA(pRenderingDevicePresentParameters);

    UINT retVal = InitAdapterMonitors();
    if(retVal != 0)
    {
        WriteToLog( _T("InitAdapterMonitors failed retVal=%d.\n"), retVal );
        return retVal;   
    }

    if( FAILED( hResult = m_pSrcDevice->GetDeviceCaps(&m_deviceCaps) ) )
    {
        WriteToLog(
            _T( "GetDeviceCaps() returned %s (0x%x)\n" ),
            m_pD3D->HResultToString(hResult),
            hResult
        );
        return D3DTESTINIT_SKIPALL;
    }

    // create hwnds
    // Register the windows class for the clipping windows
    WNDCLASS wndClass = {    0, 
                            MyWindowProc,
                            0, 
                            0, 
                            NULL, 
                            NULL, 
                            LoadCursor( NULL, IDC_ARROW ),
                            (HBRUSH)GetStockObject(WHITE_BRUSH), 
                            NULL,
                            _T( "Clipping" ) 
    };
    if( 0 == RegisterClass( &wndClass ) )
    {
        DWORD dwErr = GetLastError();
        if( (dwErr != 0) && (dwErr != ERROR_CLASS_ALREADY_EXISTS) )
        {
            WriteToLog(    TEXT("RegisterClass() failed, last error %d (0x%x)\n"),
                            dwErr, dwErr );
            hResult = E_FAIL;
        }
    }

    INT brushType = 0;
    if( TEST_DEVICECLIP == m_TestType )
    {
        //
        //  Deviceclip verifies against the background of the window. So we need a real brush
        //
        brushType = GRAY_BRUSH;   
    }
    else
    {
        //
        // Other tests clear and draw themselves so dont want GDI interfering
        //
        brushType = NULL_BRUSH;
    }
    
    
    // Register the windows class for the present windows
    WNDCLASS wndClass1 = {    0, 
                            MyWindowProc,
                            0, 
                            0, 
                            NULL, 
                            NULL, 
                            LoadCursor( NULL, IDC_ARROW ),
                            (HBRUSH)GetStockObject(brushType), 
                            NULL,
                            _T( "Present2" ) 
    };
    if( 0 == RegisterClass( &wndClass1 ) )
    {
        DWORD dwErr = GetLastError();
        if( (dwErr != 0) && (dwErr != ERROR_CLASS_ALREADY_EXISTS) )
        {
            WriteToLog(    TEXT("RegisterClass() failed, last error %d (0x%x)\n"),
                            dwErr, dwErr );
            hResult = E_FAIL;
        }
    }
    
    // place the window on whatever adapter the test is running on
    HMONITOR hMon = MonitorFromWindow( m_pFramework->m_hWindow, MONITOR_DEFAULTTONEAREST );
    MONITORINFO mi;
    ZeroMemory( &mi, sizeof( mi ) );
    mi.cbSize = sizeof( mi );
    GetMonitorInfo( hMon, &mi );

    DWORD dwExStyle = WS_EX_TOPMOST;
    if( KeySet( _T ( "RTL" ) ) )
        dwExStyle |= WS_EX_LAYOUTRTL;
    if( KeySet( _T( "Layered" ) ) )
        dwExStyle |= WS_EX_LAYERED;

    int xWindow = mi.rcMonitor.left + 10;
    int yWindow = mi.rcMonitor.top + 10;
    while( ( xWindow % 4 ) > 0 )
        xWindow++;
    while( ( yWindow % 4 ) > 0 )
        yWindow++;

    // Create the reset window
    if( NULL == ( m_hWndPresent = m_hWndReset = CreateWindowEx(
                                    dwExStyle,
                                    _T( "Present2" ), 
                                    _T("Reset (default)"),
                                    WS_POPUP|WS_VISIBLE,
                                    xWindow,
                                    yWindow,
                                    DEFAULT_WINDOW_WIDTH,
                                    DEFAULT_WINDOW_HEIGHT,
                                    m_pFramework->m_hWindow,
                                    0L,
                                    0L, 
                                    0L ) 
                ) 
        )
    {
        WriteToLog(    TEXT("CreateWindow() failed, last error %d (0x%x)\n"),
                    GetLastError(),
                    GetLastError()
                    );
        hResult = E_FAIL;
    }

    if( KeySet( _T( "Layered" ) ) )
        SetLayeredWindowAttributes( m_hWndReset, 0, 255, LWA_ALPHA );

    xWindow = ( mi.rcMonitor.left + mi.rcMonitor.right - DEFAULT_WINDOW_WIDTH ) - 10;
    while( ( xWindow % 4 ) > 0 )
        xWindow++;
    
    // create the override window
    if( NULL == ( m_hWndOverride = CreateWindowEx(
                                    dwExStyle,
                                    _T( "Present2" ), 
                                    _T("Present Override"),
                                    WS_POPUP|WS_VISIBLE,
                                    xWindow,
                                    yWindow,
                                    DEFAULT_WINDOW_WIDTH,
                                    DEFAULT_WINDOW_HEIGHT,
                                    NULL,
                                    0L,
                                    0L, 
                                    0L ) 
                ) 
        )
    {
        WriteToLog(    TEXT("CreateWindow() failed, last error %d (0x%x)\n"),
                    GetLastError(),
                    GetLastError()
                    );
        hResult = E_FAIL;
    }

    if( KeySet( _T( "Layered" ) ) )
        SetLayeredWindowAttributes( m_hWndOverride, 0, 255, LWA_ALPHA );

    ShowWindow( m_hWndReset, SW_SHOW );
    ShowWindow( m_hWndOverride, SW_SHOW );

    // hide the mouse cursor
    if( !m_bDontHideCursor )
        ::ShowCursor( FALSE );
    
    // init present parameters
    InitPresentParameters();

    //
    // Filtering only implented for FullScreenPresent test.  Filtering requires m_vecPresentParametersRotation vector to
    // contain rotation mode for each m_vecPresentParameters entry.  Filtering also requires rotated modes to not be
    // pre-swaped and to contain the raw data from EnumAdapterModes
    //
    if(KeySet(_T("TrimModes")) && 
        m_TestType == TEST_FULLSCREENPRESENT &&  
        (CSTR_EQUAL == CompareString(LOCALE_INVARIANT,NORM_IGNORECASE,m_szTestName, -1,_T("FullScreenPresent"), -1)))
    {

       /* WriteToLog("****************RAW***MODES*******************************");
        for(int i = 0; i < m_vecPresentParameters.size(); i++)
        {
             
            WriteToLog("uWidth %u uHeight %u uFullscreenRefresh %u MultiSample %d uPresentInterval %u SwapEffect %u  BackBufferFormat %s rotation %u" ,
                m_vecPresentParameters[i].uWidth,
                m_vecPresentParameters[i].uHeight,
                m_vecPresentParameters[i].uFullscreenRefresh,
                m_vecPresentParameters[i].MultiSample,
                m_vecPresentParameters[i].uPresentInterval,
                m_vecPresentParameters[i].SwapEffect,
                FORMAT2STRING(m_vecPresentParameters[i].BackBufferFormat.d3dfFormat),
                m_vecPresentParametersRotation[i]);

        }*/

        FilterLowResolutionModes();
        GetHighLowResolutionModes();
    }

    
    // init available textures
    InitTextures();
    
    // init Present windows
    InitPresentWindows();
    
    // init source and destination rects
    InitPresentSourceRect();
    InitPresentDestRect();
    
    // init the dirty region
    InitDirtyRegions();

    // init private data section
    InitPrivateData();

    // init additional windows (the dialog boxes or clipping windows)
    InitWindows();

    // init the PresentEx flags (LONGHORN ONLY)
    InitFlagsEx();

    InitDeviceReleaseBeforeVerify();

    InitWindowTrees();

    // compute the total number of tests
    m_nTests =    m_vecPrivateData.size() * 
                ( m_vecPresentParameters.size() / m_uNumberSwapChains ) * 
                m_vecPresentWindow.size() * 
                m_vecStringTexture.size() * 
                m_vecRectPresentSource.size() * 
                m_vecRectPresentDest.size() * 
                m_vecRegData.size();
    if( m_vecIntWindows.size() )
        m_nTests *= m_vecIntWindows.size();
    if( m_vecFlagsEx.size() )
        m_nTests *= m_vecFlagsEx.size();
    if( m_vecDeviceReleaseBeforeVerify.size() )
        m_nTests *= m_vecDeviceReleaseBeforeVerify.size();
    if( m_vecWindowTrees.size() )
        m_nTests *= m_vecWindowTrees.size();
     

    m_nLastIndexPresentParameters    = 99999;
    m_nLastIndexTexture              = 99999;
    m_nLastIndexWindowTrees          = 99999;
    m_LastDeviceReleaseBeforeVerify  = NoRelease;

    
    m_bTestFailed = false;
    m_bCanRender = false;
    SetTestRange( 1, m_nTests );

    return m_nTests? D3DTESTINIT_RUN:D3DTESTINIT_SKIPALL;
}

//////////////////////////////////////////////////////////////////////
// Name:    PrepareNextTest()
// Desc:    
//////////////////////////////////////////////////////////////////////
void CPresentII::PrepareNextTest()
{
    m_nLastIndexPresentParameters = m_nIndexPresentParameters;
    m_nLastIndexTexture = m_nIndexTexture;
    m_LastDeviceReleaseBeforeVerify = m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify];
    m_nLastIndexWindowTrees = m_nIndexWindowTrees;

}


//////////////////////////////////////////////////////////////////////
// Name:    TestTerminate()
// Desc:    cleanup
//////////////////////////////////////////////////////////////////////
bool CPresentII::TestTerminate()
{
     WriteToLog(_T("TestTerminate"));
    if(m_vecWindowTrees.size())
    {
        WriteToLog(_T("TestTerminate CleanUpWindowTree"));
        for(int i = 0; i < m_vecWindowTrees.size(); i++)
        {
            HRESULT hr = CleanUpWindowTree(&m_vecWindowTrees[i]);
            if(FAILED(hr))
            {
                WriteToLog(_T("CleanUpWindowTree[%d] Failed hr = 0x%x"), i,  hr);
            }

        }
    }

    RELEASE( m_pVB );
    if(m_pRenderingDevice)
    {
        m_pRenderingDevice->DeletePixelShader( m_pPShader );
        m_pPShader = NULL;
    }

    RELEASE( m_pTexture );
    for( UINT uSwapChain = 0; uSwapChain < MAX_SWAPCHAINS; uSwapChain++ )
    {
        RELEASE( m_SwapChainData[uSwapChain].m_pD3DXRefBuffer );
        RELEASE( m_SwapChainData[uSwapChain].m_pSourceSurface );
        RELEASE( m_SwapChainData[uSwapChain].m_pDestSurface );
        RELEASE( m_SwapChainData[uSwapChain].m_pDiffSurface );
        for( UINT uBB = 0; uBB < D3DPRESENT_BACK_BUFFERS_MAX; uBB++ )
        {
            RELEASE( m_SwapChainData[uSwapChain].m_vecBB[uBB] );
        }
    }
   
    ClearAdapterMonitors();
    
    RELEASE(m_pRenderingDevice);
    
    SAFEDELETEA( m_pDirtyRegion );
    SAFEDELETEA( m_pDestDirtyRegion );

    for( vector<RGNDATA*>::iterator itRegion = m_vecRegData.begin(); itRegion < m_vecRegData.end(); itRegion++ )
        if( NULL != (*itRegion) )
            delete[] (char*)(*itRegion);

    m_vecPrivateData.clear();
    m_vecPresentParameters.clear();
    m_vecPresentParametersRotation.clear();
    m_vecStringTexture.clear();
    m_vecRectPresentSource.clear();
    m_vecRectPresentDest.clear();
    m_vecRegData.clear();
    m_vecIntWindows.clear();
    m_vecWindowTrees.clear();

    for( vector<DialogBoxDescription>::iterator itHwnd = m_vecWindowDescription.begin(); itHwnd < m_vecWindowDescription.end(); itHwnd++ )
        DestroyWindow( (*itHwnd ).hDlg );
    if( m_hWndReset )
        DestroyWindow( m_hWndReset );
    if( m_hWndOverride )
        DestroyWindow( m_hWndOverride );
    m_vecWindowDescription.clear();
    m_vecFlagsEx.clear();

    return true;
}


//////////////////////////////////////////////////////////////////////
// Name:    ExecuteTest( UINT )
// Desc:    Execute the testcase
//////////////////////////////////////////////////////////////////////
bool CPresentII::ExecuteTest(UINT nTest)
{
    char szTestName[256];
    HRESULT hResult = D3D_OK;
    m_nTestId = nTest;
    if( m_bQuitOnFailure && m_bTestFailed )
    {
        Skip();
        SkipTests( m_nTests - nTest + 1 );
        SaveBuffers();
        return false;
    }

    // compute indexes into vectors from nTest
    UINT nTempIndex = m_nTests / ( m_vecPresentParameters.size() / m_uNumberSwapChains ), nTempTest = nTest - 1;
    m_nIndexPresentParameters = m_uNumberSwapChains * ( nTempTest / nTempIndex );                // index in the PresentParameters vector

    nTempTest = nTempTest % nTempIndex;
    nTempIndex = nTempIndex / m_vecPresentWindow.size();
    m_nIndexPresentWindow = nTempTest / nTempIndex;            // index in the PresentWindow vector

    nTempTest = nTempTest % nTempIndex;
    nTempIndex = nTempIndex / m_vecStringTexture.size();
    m_nIndexTexture = nTempTest / nTempIndex;            // index in the StringTexture vector

    nTempTest = nTempTest % nTempIndex;
    nTempIndex = nTempIndex / m_vecRectPresentSource.size();
    m_nIndexRectSource = nTempTest / nTempIndex;        // index in the RectPresentSource vector

    nTempTest = nTempTest % nTempIndex;
    nTempIndex = nTempIndex / m_vecRectPresentDest.size();
    m_nIndexRectDest = nTempTest / nTempIndex;            // index in the RectPresentDest vector

    nTempTest = nTempTest % nTempIndex;
    nTempIndex = nTempIndex / m_vecRegData.size();
    m_nIndexDirtyRegion = nTempTest / nTempIndex;            // index in the DirtyRegion vector

    if( m_vecIntWindows.size() )
    {
        nTempTest = nTempTest % nTempIndex;
        nTempIndex = nTempIndex / m_vecIntWindows.size();
        m_nIndexDialogs = nTempTest / nTempIndex;            // index in the DialogBoxMode vector
    }

    if( m_vecFlagsEx.size() )
    {
        nTempTest = nTempTest % nTempIndex;
        nTempIndex = nTempIndex / m_vecFlagsEx.size();
        m_nIndexFlagsEx = nTempTest / nTempIndex;            // index in the FlagsEx vector
    }

    if( m_vecDeviceReleaseBeforeVerify.size() )
    {
        nTempTest = nTempTest % nTempIndex;
        nTempIndex = nTempIndex / m_vecDeviceReleaseBeforeVerify.size();
        m_nIndexDeviceReleaseBeforeVerify = nTempTest / nTempIndex;            // index in the m_vecDeviceReleaseBeforeVerify vector
    }

    if( m_vecWindowTrees.size() )
    {
        nTempTest = nTempTest % nTempIndex;
        nTempIndex = nTempIndex / m_vecWindowTrees.size();
        m_nIndexWindowTrees = nTempTest / nTempIndex;            // index in the WindowTrees vector
    }


    nTempTest = nTempTest % nTempIndex;
    nTempIndex = nTempIndex / m_vecPrivateData.size();
    m_nIndexPrivateData = nTempTest / nTempIndex;

    // release variables used in previous tests
    for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
    {
        RELEASE( m_SwapChainData[uSwapChain].m_pD3DXRefBuffer );
        RELEASE( m_SwapChainData[uSwapChain].m_pSourceSurface );
        RELEASE( m_SwapChainData[uSwapChain].m_pDestSurface );
        RELEASE( m_SwapChainData[uSwapChain].m_pDiffSurface );

        if ((m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] != ReleaseSwapChains) && 
            (m_LastDeviceReleaseBeforeVerify == ReleaseSwapChains))
        {
            RELEASE(m_vecAdapterMonitors[uSwapChain].m_pSwapChain);
        }
    }


    BeginTestCase( Name( szTestName ), nTest );
    HWND lastVisible = m_hWndPresent;

    m_iConformance    = (m_vecPresentParameters[m_nIndexPresentParameters].bWindowed)? 85:98;


    m_hWndPresent = m_vecPresentWindow[m_nIndexPresentWindow]? m_vecPresentWindow[m_nIndexPresentWindow]:m_hWndReset;

    // show the test window, hide the other window
    ShowWindow( m_hWndReset, (m_hWndPresent == m_hWndReset)? SW_SHOW : SW_HIDE );
    ShowWindow( m_hWndOverride, (m_hWndPresent == m_hWndOverride)? SW_SHOW : SW_HIDE );

    if( m_vecWindowTrees.size() && ( (m_nIndexWindowTrees != m_nLastIndexWindowTrees) || (lastVisible != m_hWndPresent)) )
    {
        //
        //  Clean up the window tree from the last test if there is a new index
        //
        if(m_nLastIndexPresentParameters != 99999)
        {
            HRESULT hr = CleanUpWindowTree(&m_vecWindowTrees[m_nLastIndexWindowTrees]);
            if(FAILED(hr))
            {
                WriteToLog(_T("CleanUpWindowTree Failed hr = 0x%x"),  hr);
            }
        }

        //
        //  Build a new tree
        //
        hResult = BuildWindowTree(NULL, m_hWndPresent, &m_vecWindowTrees[m_nIndexWindowTrees]);
        if(FAILED(hResult))
        {
            WriteToLog( _T( "BuildWindowTree, return code %s (0x%x)\n" ),
                m_pD3D->HResultToString( hResult ),
                hResult );
            Abort();
            PrepareNextTest();     
            return FailTestCase();
        }

        hResult = PumpMessagesWindowTree( &m_vecWindowTrees[m_nIndexWindowTrees] );
        if(FAILED(hResult))
        {
            WriteToLog( _T( "PumpMessagesWindowTree, return code %s (0x%x)\n" ),
                m_pD3D->HResultToString( hResult ),
                hResult );
            Abort();
            PrepareNextTest();     
            return FailTestCase();
        }  
    }

    PrivateDataProcessing();

    //
    //  Make sure the m_hWndPresent window is in front
    //
    Sleep(100);
    if(SUCCEEDED(hResult))
    {
        if (!SetWindowPos( m_hWndPresent, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE))
        {
            DWORD e = GetLastError();
            hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
            WriteToLog(TEXT("SetWindowPos() failed, last error 0x%x\n"),hResult);
        }
    }

    MSG msg;
    while( ::PeekMessage( &msg, m_hWndPresent, 0, 0, PM_NOREMOVE ) && msg.message != WM_QUIT )
    {
        if( ::PeekMessage( &msg, m_hWndPresent, 0, 0, PM_REMOVE ) )
        {
            ::TranslateMessage( &msg );
            ::DispatchMessage( &msg );
        }
    }

    if( FAILED( GetFrontBufferRect() ) )
    {
        Dump();
        Abort();
        PrepareNextTest();
        return FailTestCase();   
    }


try
{

    m_bSnapshot = false;

    if( m_nIndexPresentParameters != m_nLastIndexPresentParameters 
        || (m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseSwapChains && m_LastDeviceReleaseBeforeVerify != ReleaseSwapChains)
        || (m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] != ReleaseSwapChains  && (m_LastDeviceReleaseBeforeVerify == ReleaseSwapChains)  ))
    {
        RELEASE( m_pVB );
        m_pRenderingDevice->DeletePixelShader(m_pPShader );
        m_pPShader = NULL;
        RELEASE( m_pTexture );
        ClearAdapterMonitors();

        // set dialog box mode
        if( TEST_SETDIALOGBOXMODE == m_TestType && 
            FAILED( hResult = m_pRenderingDevice->SetDialogBoxMode( TEST_SETDIALOGBOXMODE == m_TestType ) ) )
        {
            WriteToLog( _T( "SetDialogBoxMode failed, return code %s (0x%x)\n" ),
                        m_pD3D->HResultToString( hResult ),
                        hResult );
            Abort();
            PrepareNextTest();
            return FailTestCase();
        }

        // Update the device
        m_pRenderingDevice->D3DXFontOnLostDevice();

        // allow the test to change the mode before calling Reset
        // useful for Rotation cases
        ChangeDisplaySettings();
        
        if( m_pRenderingDevice->GetInterface() < 9.1f )
        {
            WriteToLog( _T( "Reset mode: Width = %d, Height = %d, Refresh = %d, Format = %s\n" ),        
                m_vecPresentParameters[m_nIndexPresentParameters].uWidth,
                m_vecPresentParameters[m_nIndexPresentParameters].uHeight,
                m_vecPresentParameters[m_nIndexPresentParameters].uFullscreenRefresh,
                FORMAT2STRING( m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat )
            );

            hResult = m_pRenderingDevice->Reset( &m_vecPresentParameters[m_nIndexPresentParameters] );
        }
        else
        {
            D3DDISPLAYMODEEX displayModeEx[MAX_SWAPCHAINS], *pDisplayModeEx;
            D3DDISPLAYROTATION displayRotation;
            pDisplayModeEx = m_vecPresentParameters[m_nIndexPresentParameters].bWindowed? NULL : displayModeEx;

            DWORD dwDisplayOrientation = GetGDIDisplayOrientation();

            //
            // ResetEx in the framework expects PRESENT_PARAMETERS array to be the size of NumberOfAdaptersInGroup
            //
            //UINT uMaxAdapter = m_vecPresentParameters[m_nIndexPresentParameters].bWindowed? m_pD3D->GetAdapterCount() : m_uNumberSwapChains;
            
            PRESENT_PARAMETERS *pTempCopy = new PRESENT_PARAMETERS[m_uNumberSwapChains];
            ZeroMemory( pTempCopy, sizeof( PRESENT_PARAMETERS)  * m_uNumberSwapChains );
           
            memcpy( pTempCopy, &m_vecPresentParameters[m_nIndexPresentParameters], sizeof( PRESENT_PARAMETERS) * m_uNumberSwapChains  );
            WriteToLog("m_uNumberSwapChains %d", m_uNumberSwapChains);
            for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
            {
                displayModeEx[uSwapChain].Size = sizeof( D3DDISPLAYMODEEX );
                displayModeEx[uSwapChain].Width = m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uWidth;
                displayModeEx[uSwapChain].Height = m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uHeight;
                displayModeEx[uSwapChain].RefreshRate = 0; //m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uFullscreenRefresh;
                displayModeEx[uSwapChain].Format = m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].BackBufferFormat.d3dfFormat;
                displayModeEx[uSwapChain].ScanLineOrdering = D3DSCANLINEORDERING_UNKNOWN;

                if( ( DMDO_90 == dwDisplayOrientation || DMDO_270 == dwDisplayOrientation ) &&
                    0 != ( D3DPRESENTFLAG_NOAUTOROTATE & m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].dwFlags ) )
                    std::swap( displayModeEx[uSwapChain].Width, displayModeEx[uSwapChain].Height );
            
                
                //
                // If we are in the ReleaseSwapChains test need to unbind the devices default swapchain 
                //
                if (m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseSwapChains)
                {
                    pTempCopy[uSwapChain].SwapEffect = SWAPEFFECT_COPY;
                }
                
                WriteToLog( _T( "ResetEx displayModeEx. SwapChain %d mode: Width = %d, Height = %d, Refresh = %d, Format = %s  ScanLineOrdering=%d size = %d \n" ), 
                    uSwapChain,
                    displayModeEx[uSwapChain].Width,
                    displayModeEx[uSwapChain].Height,
                    displayModeEx[uSwapChain].RefreshRate,
                    FORMAT2STRING( displayModeEx[uSwapChain].Format ),
                    displayModeEx[uSwapChain].ScanLineOrdering,
                    displayModeEx[uSwapChain].Size
                    );

                WriteToLog( _T( "ResetEx presentparams. SwapChain %d: Width = %d, Height = %d, Refresh = %d, Format = %s MultiSample=%d MultiSampleQuality=%d SwapEffect=%d hDeviceWindow=0x%x bWindowed=%d dwFlags=%d\n" ),        
                    uSwapChain,
                    m_vecPresentParameters[m_nIndexPresentParameters].uWidth,
                    m_vecPresentParameters[m_nIndexPresentParameters].uHeight,
                    m_vecPresentParameters[m_nIndexPresentParameters].uFullscreenRefresh,
                    FORMAT2STRING( m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat),
                    m_vecPresentParameters[m_nIndexPresentParameters].MultiSample,
                    m_vecPresentParameters[m_nIndexPresentParameters].MultiSampleQuality,
                    m_vecPresentParameters[m_nIndexPresentParameters].SwapEffect,
                    m_vecPresentParameters[m_nIndexPresentParameters].hDeviceWindow,
                    m_vecPresentParameters[m_nIndexPresentParameters].bWindowed,
                    m_vecPresentParameters[m_nIndexPresentParameters].dwFlags);
             
            }
        
            hResult = m_pRenderingDevice->ResetEx( pTempCopy, pDisplayModeEx );

            SAFEDELETEA(pTempCopy);
        }

        if( (m_nIndexPresentParameters != m_nLastIndexPresentParameters  && m_vecPresentParameters[m_nIndexPresentParameters].bWindowed == false))
        {
            //
            // Need to sleep after mode change.
            // 
            Sleep(100);
        }

        if(hResult == S_OK)
        {
            if( m_pRenderingDevice->GetInterface() < 9.1f )
            {
                hResult = m_pRenderingDevice->TestCooperativeLevel();
            }
            else
            {
                hResult = m_pRenderingDevice->CheckDeviceState(m_hWndPresent);
            }
        }
        
        if(FAILED( hResult ) ||  hResult == S_PRESENT_MODE_CHANGED)
        {
            WriteToLog( _T( "CPresentII::ExecuteTest: Reset failed, return code %s (0x%x)\n" ),
                        m_pD3D->HResultToString( hResult ),
                        hResult );

            Skip();
            return false;
        }

        m_pRenderingDevice->D3DXFontOnResetDevice();

        MSG        msg;
        while( PeekMessage( &msg,m_vecPresentParameters[m_nIndexPresentParameters].hDeviceWindow,0,0,PM_REMOVE ) )
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // update DWM status
        m_bDWMEnabled = IsDWMEnabled();

        UINT retVal = InitAdapterMonitors();
        if(retVal != 0)
        {
            WriteToLog( _T("InitAdapterMonitors failed retVal=%d.\n"), retVal );
            Dump();
            Abort();
            PrepareNextTest();
            return FailTestCase();   
        }

        // get front buffer rect
        if( FAILED( GetFrontBufferRect() ) )
        {
            Dump();
            Abort();
            PrepareNextTest();
            return FailTestCase();   
        }
        // release the backbuffer cached surfaces
        for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
        {
            for( UINT uBB = 0; uBB < D3DPRESENT_BACK_BUFFERS_MAX; uBB++ )
            {
                RELEASE( m_SwapChainData[uSwapChain].m_vecBB[uBB] );
            }

            // verify the backbuffers
            for( UINT uBB = 0; uBB < m_vecPresentParameters[m_nIndexPresentParameters].uBackBufferCount; uBB++ )
            {
                LPSURFACE pBackBuffer = NULL;
                if( FAILED( hResult = m_pRenderingDevice->GetBackBuffer(  uSwapChain,
                                                                    uBB, 
                                                                    D3DBACKBUFFER_TYPE_MONO,
                                                                    &pBackBuffer ) ) )
                {
                    WriteToLog( _T( "GetBackBuffer failed, return code %s (0x%x)\n" ),
                                m_pD3D->HResultToString( hResult ),
                                hResult );
                    return FailTestCase();
                }
                // verify the back buffer format is the same with the selected format
                SURFACEDESC sdBackBuffer;
                if( FAILED( hResult = pBackBuffer->GetDesc( &sdBackBuffer ) ) )
                {
                    WriteToLog( _T( "GetDesc failed, return code %s (0x%x)\n" ),
                                m_pD3D->HResultToString( hResult ),
                                hResult );
                    RELEASE( pBackBuffer );
                    return FailTestCase();
                }
                
                if( sdBackBuffer.Format.d3dfFormat != m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].BackBufferFormat.d3dfFormat || 
                    sdBackBuffer.Width != m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uWidth || 
                    sdBackBuffer.Height != m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uHeight )
                {
                    WriteToLog( _T( "Unexpected BackBuffer description: %d x %d x %d instead of %d x %d x %d\n" ),
                                sdBackBuffer.Width,
                                sdBackBuffer.Height,
                                sdBackBuffer.Format.d3dfFormat,
                                m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uWidth,
                                m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uHeight,
                                m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].BackBufferFormat.d3dfFormat );
                    
                    bool bIgnoreFormatMismatch = false;
                    if( m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].bWindowed == false )
                    {
                        OSVERSIONINFO windowsVersionInfo;
                        windowsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
                        GetVersionEx(&windowsVersionInfo);

                        if(windowsVersionInfo.dwMajorVersion > 6 || (windowsVersionInfo.dwMajorVersion == 6 && windowsVersionInfo.dwMinorVersion >= 2))
                        {
                            if( GetBitCountForFormat( m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].BackBufferFormat.d3dfFormat ) < 32 )
                            {
                                if ( D3DFMT_X8R8G8B8 == sdBackBuffer.Format.d3dfFormat )
                                {
                                    WriteToLog( _T( "Win8 appcompat shim: DWM8And16BitMitigation: Fullscreen 16-bit backbuffer formats changed to X8R8G8B8. " ) );
                                    bIgnoreFormatMismatch = true;
                                }
                            }
                        }
                    }

                    if( !bIgnoreFormatMismatch )
                    {
                        RELEASE( pBackBuffer );
                        return FailTestCase();
                    }
                }

                // create a surface to hold the backbuffer

                if( FAILED( hResult = m_pRenderingDevice->CreateImageSurface(
                                                                sdBackBuffer.Width,
                                                                sdBackBuffer.Height,
                                                                sdBackBuffer.Format,
                                                                &( m_SwapChainData[uSwapChain].m_vecBB[uBB] )
                                                            )
                            )
                    )
                {
                    WriteToLog( _T( "CreateImageSurface failed, return code %s (0x%x)\n" ),
                                    m_pD3D->HResultToString( hResult ),
                                    hResult );
                    RELEASE( pBackBuffer );
                    return FailTestCase();
                }

                // release the backbuffer
                RELEASE( pBackBuffer );
            }

            // reset backbuffer index
            m_nIndexBB = 0;
        }

        // create the front buffer surfaces
        for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
        {
            RELEASE( m_vecAdapterMonitors[uAdapter].m_pFrontBuffer );

            DWORD dwFBWidth, dwFBHeight;
            if( uAdapter >= m_uNumberSwapChains ||
                m_vecPresentParameters[ m_nIndexPresentParameters + uAdapter ].bWindowed )
            {
                dwFBWidth    = m_vecAdapterMonitors[uAdapter].m_modeDisplay.dwWidth;
                dwFBHeight    = m_vecAdapterMonitors[uAdapter].m_modeDisplay.dwHeight;
            }
            else
            {
                dwFBWidth    = m_vecPresentParameters[ m_nIndexPresentParameters + uAdapter ].uWidth;
                dwFBHeight    = m_vecPresentParameters[ m_nIndexPresentParameters + uAdapter ].uHeight;
            }

            if( FAILED( hResult = m_vecAdapterMonitors[uAdapter].m_pDevice->CreateImageSurface(
                                                                    dwFBWidth,
                                                                    dwFBHeight,
                                                                    FMT_A8R8G8B8,
                                                                    &( m_vecAdapterMonitors[uAdapter].m_pFrontBuffer )
                                                                ) 
                        )
                )
            {
                WriteToLog( _T( "CreateImageSurface failed, return code %s (0x%x)\n" ),
                                m_pD3D->HResultToString( hResult ),
                                hResult );
                return FailTestCase();
            }
        }
    }

    if (m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseSwapChains)
    {

        for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
        {

            RELEASE(m_vecAdapterMonitors[uSwapChain].m_pSwapChain);

            if FAILED( hResult =  m_pRenderingDevice->CreateAdditionalSwapChain( &m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain], &m_vecAdapterMonitors[uSwapChain].m_pSwapChain ) )
            {
                WriteToLog( _T( "CreateAdditionalSwapChain failed, return code %s (0x%x)\n" ),
                    m_pD3D->HResultToString( hResult ),
                    hResult );
                Abort();
                PrepareNextTest();
                return FailTestCase();
            }
        }
    }

    // Get the window size
    for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
    {
        if( !m_vecPresentParameters[m_nIndexPresentParameters].bWindowed )
            SetRect(    &( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow ), 
                        0, 
                        0, 
                        m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uWidth, 
                        m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uHeight
                    );
        else
        {
            SetRect(    &( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow ), 
                        0,
                        0, 
                        m_dwWindowWidth, 
                        m_dwWindowHeight );
        }

        DISPLAYMODE mode = {0};
        m_pRenderingDevice->GetDisplayMode(
            uSwapChain,
            &mode);
    
        WriteToLog( _T( "Current displaymode swapChain %u mode: Width = %u, Height = %u, Refresh = %u, Format = %s, \n" ), 
                uSwapChain,
                mode.dwWidth,
                mode.dwHeight,
                mode.dwRefreshRate,
                FORMAT2STRING( FORMAT(mode.Format).d3dfFormat)
                );   
    }
    
    

    // erase color used to be grabbed only when the mode changed, for performance reasons
    // however some cards can change the dithering algorithm (for color converting) between tests - for performance reasons
    // so the method is called before every test, instead of per mode change
    if( !GrabEraseColor() )
    {
        PrepareNextTest();     
        return false;
    }

    for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
    {
        // set rect parameters used in Present (to correctly create the ref surface)
        m_SwapChainData[uSwapChain].m_rectPresentSource = CreateRect(   m_vecRectPresentSource[m_nIndexRectSource], 
                                                                        m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uWidth,
                                                                        m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uHeight
                                                                    );
        m_SwapChainData[uSwapChain].m_rectPresentDest    = CreateRect(  m_vecRectPresentDest[m_nIndexRectDest], 
                                                                        m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.right - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.left,
                                                                        m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.bottom - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.top
                                                                    );
        m_SwapChainData[uSwapChain].m_rectDPIPresentDest= CreateRect(    m_vecRectPresentDest[m_nIndexRectDest], 
                                                                        (DWORD)( ( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.right - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.left ) / m_fDPIFactor ), 
                                                                        (DWORD)( ( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.bottom - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.top ) / m_fDPIFactor )
                                                                    );

        if( 0 == m_SwapChainData[uSwapChain].m_rectPresentSource.right - m_SwapChainData[uSwapChain].m_rectPresentSource.left && 
            0 == m_SwapChainData[uSwapChain].m_rectPresentSource.bottom - m_SwapChainData[uSwapChain].m_rectPresentSource.top )
        {
            m_prectPresentSource        = NULL;
            m_SwapChainData[uSwapChain].m_rectPresentSource.left = m_SwapChainData[uSwapChain].m_rectPresentSource.top = 0;
            m_SwapChainData[uSwapChain].m_rectPresentSource.right = m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uWidth;
            m_SwapChainData[uSwapChain].m_rectPresentSource.bottom = m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uHeight;
        }
        else
            m_prectPresentSource        = &( m_SwapChainData[uSwapChain].m_rectPresentSource );
        if( 0 == m_SwapChainData[uSwapChain].m_rectPresentDest.right - m_SwapChainData[uSwapChain].m_rectPresentDest.left && 
            0 == m_SwapChainData[uSwapChain].m_rectPresentDest.bottom - m_SwapChainData[uSwapChain].m_rectPresentDest.top )
        {
            m_prectPresentDest            = NULL;
            m_SwapChainData[uSwapChain].m_rectPresentDest.left = m_SwapChainData[uSwapChain].m_rectPresentDest.top = 0;
            m_SwapChainData[uSwapChain].m_rectPresentDest.right    = !m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].bWindowed? 
                                                                        m_SwapChainData[uSwapChain].m_rectPresentSource.right : ( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.right - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.left );
            m_SwapChainData[uSwapChain].m_rectPresentDest.bottom = !m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].bWindowed? 
                                                                        m_SwapChainData[uSwapChain].m_rectPresentSource.bottom : ( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.bottom - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.top );
        }
        else
        {
            m_prectPresentDest = &( m_SwapChainData[uSwapChain].m_rectDPIPresentDest );
        }
    }

    if( m_nIndexTexture != m_nLastIndexTexture || m_nIndexPresentParameters != m_nLastIndexPresentParameters || 
         (m_LastDeviceReleaseBeforeVerify == ReleaseDevice))
    {
        // first try to create the texture from the path
        if( !m_vecStringTexture[m_nIndexTexture].empty() )
        {
           
            //
            // FMT_A2B10G10R10_XR_BIAS is not a valid buffer format just make it FMT_A2B10G10R10
            //
            FORMAT tmpFormat = (m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat == FMT_A2B10G10R10_XR_BIAS) ? FMT_A2B10G10R10 : m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat;
            if( FAILED( hResult = m_pRenderingDevice->D3DXCreateTextureFromResourceEx(
                                                                NULL,
                                                                m_vecStringTexture[m_nIndexTexture].c_str(),
                                                                m_vecPresentParameters[m_nIndexPresentParameters].uWidth,
                                                                m_vecPresentParameters[m_nIndexPresentParameters].uHeight,
                                                                1,
                                                                0,
                                                                tmpFormat.d3dfFormat,
                                                                D3DPOOL_DEFAULT,
                                                                D3DX_DEFAULT,
                                                                D3DX_DEFAULT,
                                                                0,
                                                                NULL,
                                                                NULL,
                                                                &m_pTexture )
                        )
                )
            {
                WriteToLog( _T( "D3DXCreateTextureFromResourceEx failed, return code %s (0x%x)\n" ),
                            m_pD3D->HResultToString( hResult ),
                            hResult );
                Abort();
                PrepareNextTest();     
                return FailTestCase();
            }
        }
        // create and the texture
        else
        {        
            //
            // FMT_A2B10G10R10_XR_BIAS is not a valid buffer format just make it FMT_A2B10G10R10
            //
            FORMAT tmpFormat = (m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat == FMT_A2B10G10R10_XR_BIAS) ? FMT_A2B10G10R10 : m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat;
            if( FAILED( hResult = m_pRenderingDevice->D3DXCreateTexture( 
                                                    m_vecPresentParameters[m_nIndexPresentParameters].uWidth,
                                                    m_vecPresentParameters[m_nIndexPresentParameters].uHeight,
                                                    1,
                                                    0,
                                                    tmpFormat.d3dfFormat,
                                                    D3DPOOL_DEFAULT,
                                                    &m_pTexture ) 
                        )
                )
            {
                WriteToLog( _T( "D3DXCreateTexture failed, return code %s (0x%x)\n" ),
                            m_pD3D->HResultToString( hResult ),
                            hResult );
                PrepareNextTest();     
                return FailTestCase();
            }

            // fill the texture
            if( FAILED( hResult =  m_pRenderingDevice->D3DXFillTexture( m_pTexture, (LPD3DXFILL2D)FillD3DXTexture, (LPVOID)&m_TestType ) ) )
            {
                WriteToLog( _T( "D3DXFillTexture failed, return code %s (0x%x)\n" ),
                            m_pD3D->HResultToString( hResult ),
                            hResult );             
                PrepareNextTest();     
                return FailTestCase();
            }
        }

        /*UINT uMaxWidth = m_vecPresentParameters[m_nIndexPresentParameters].uWidth;
        UINT uMaxHeight = m_vecPresentParameters[m_nIndexPresentParameters].uHeight;
        for( UINT uSwapChain = 1; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
        {
            if( uMaxWidth < m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uWidth )
                uMaxWidth = m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uWidth;

            if( uMaxHeight < m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uHeight )
                uMaxHeight = m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uHeight;
        }*/

    
        // set up the vertex buffer
        CCVERTEX v_Vertices[] =
        {    // x                y                    z        tu        tv
            /*{ 0.0f,                (float)uMaxHeight,    0.0f,1.0f,    0.0f,    1.0f }, 
            { 0.0f,                0.0f,                0.0f,1.0f,    0.0f,    0.0f },
            { (float)uMaxWidth,    (float)uMaxHeight,    0.0f,1.0f,    1.0f,    1.0f }, 
            { (float)uMaxWidth,    0.0f,                0.0f,1.0f,    1.0f,    0.0f },*/
            { -1.0f,            1.0f,                0.0f,    0.0f,    0.0f }, 
            { 1.0f,                1.0f,                0.0f,    1.0f,    0.0f },
            { -1.0f,            -1.0f,                0.0f,    0.0f,    1.0f }, 
            { 1.0f,                -1.0f,                0.0f,    1.0f,    1.0f },
        };

        if( FAILED( hResult = m_pRenderingDevice->CreateVertexBuffer( 
                                                        sizeof(v_Vertices),
                                                        0, 
                                                        D3DFVF_CCVERTEX,
                                                        D3DPOOL_DEFAULT,
                                                        &m_pVB ) 
                    )
            )
        {
            WriteToLog( _T( "CreateVertexBuffer failed, return code %s (0x%x)\n" ),
                        m_pD3D->HResultToString( hResult ),
                        hResult );
            PrepareNextTest();
            return FailTestCase();
        }
        
        VOID* pVertices;
        if( FAILED( hResult = m_pVB->Lock( 0, sizeof(v_Vertices), (BYTE**)&pVertices, 0 ) ) )
        {
            WriteToLog( _T( "Lock failed, return code %s (0x%x)\n" ),
                            m_pD3D->HResultToString( hResult ),
                            hResult );
            PrepareNextTest();
            return FailTestCase();
        }
        memcpy( pVertices, v_Vertices, sizeof(v_Vertices) );
        if( FAILED( hResult = m_pVB->Unlock() ) )
        {
            WriteToLog( _T( "Unlock failed, return code %s (0x%x)\n" ),
                            m_pD3D->HResultToString( hResult ),
                            hResult );
            PrepareNextTest();
            return FailTestCase();
        }

        // select the texture and texture stages
        Check_HRFail( m_pRenderingDevice->SetTexture( 0, m_pTexture ), "SetTexture" );

        // set stream source
        Check_HRFail( m_pRenderingDevice->SetStreamSource( 0, m_pVB, sizeof CCVERTEX ), "SetStreamSource" );
        Check_HRFail( m_pRenderingDevice->SetVertexShader( D3DFVF_CCVERTEX ), "SetVertexShader" );
        
    //
    // Only initailize the shader if FMT_A2B10G10R10_XR_BIAS
    //
        if (m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat == FMT_A2B10G10R10_XR_BIAS)
        {
            //
            // xr bias values into the shader are range is [-0.750 .. 1.250] scaled to [0 - 1]
            // [0-1] becomes [0-0.5] when you /2, which then becomes [0.0.375,0.875] when you  +=0.375
            //
            char pixelShaderXR_BIAS2[] =
                "sampler samplerEnv : register(s0);                             \n"
                "struct PSData                                                  \n"
                "{                                                              \n"
                "   float4 Pos : TEXCOORD0;                                     \n"
                "   float4 Color : COLOR;                                       \n"
                "};                                                             \n"
                "float4 main ( PSData input) : COLOR                            \n"
                "{                                                              \n"
                "	float4 color = tex2D( samplerEnv, input.Pos );              \n"
                "   color.r = (tex2D( samplerEnv, input.Pos ).r / 2.0) +.375;   \n"
                "   color.g = (tex2D( samplerEnv, input.Pos ).g / 2.0) +.375;   \n"
                "   color.b = (tex2D( samplerEnv, input.Pos ).b / 2.0) +.375;   \n"
                "   color.a = 1;                                                \n"
                "   return color;                                               \n"
                "};                                                             \n" ;

            LPD3DXBUFFER pShaderBuffer;
            LPD3DXBUFFER pErrorBuffer;

            hResult =  D3DXCompileShader(
                pixelShaderXR_BIAS2,
                sizeof(pixelShaderXR_BIAS2),
                NULL,
                NULL,
                _T("main"),
                _T("ps_2_0"),
                NULL,
                &pShaderBuffer,
                &pErrorBuffer,
                NULL);

            if(FAILED(hResult))
            {
                WriteToLog(
                    _T( "D3DXCompileShader() Error: %s. Returned %s (0x%x)\n" ),
                    (char *)(pErrorBuffer->GetBufferPointer()),
                    m_pD3D->HResultToString(hResult),
                    hResult
                    );

                PrepareNextTest();     
                return FailTestCase();
            }

            hResult = m_pRenderingDevice->CreatePixelShader(
                (DWORD*)pShaderBuffer->GetBufferPointer(),
                &m_pPShader);

            if(FAILED(hResult))
            {
                WriteToLog(
                    _T( "CreatePixelShader() returned %s (0x%x)\n" ),
                    m_pD3D->HResultToString(hResult),
                    hResult
                    );
                PrepareNextTest();     
                return FailTestCase();        }

            if(pShaderBuffer)
            {
                pShaderBuffer->Release();
            }

            if(pErrorBuffer)
            {
                pErrorBuffer->Release();
            }

            Check_HRFail( m_pRenderingDevice->SetPixelShader( m_pPShader ), "SetPixelShader" );
        }        
     

        D3DXMATRIX matrixWorld;
        D3DXMatrixIdentity( &matrixWorld );
        Check_HRFail( m_pRenderingDevice->SetTransform( D3DTS_WORLDMATRIX( 0 ), &matrixWorld ), "SetTransform(WORLD)" );

        D3DXMATRIX matrixView;
        D3DXMatrixIdentity( &matrixView );
        Check_HRFail( m_pRenderingDevice->SetTransform( D3DTS_VIEW, &matrixView ), "SetTransform(VIEW)" );

        D3DXMATRIX matrixProjection;
        D3DXMatrixIdentity( &matrixProjection );
        DWORD dwDisplayOrientation = GetGDIDisplayOrientation();
        if( 0 != ( D3DPRESENTFLAG_NOAUTOROTATE & m_vecPresentParameters[m_nIndexPresentParameters].dwFlags ) )
            switch( dwDisplayOrientation )
            {
                case DMDO_DEFAULT:
                    D3DXMatrixIdentity( &matrixProjection );
                    break;
                case DMDO_90 :
                    D3DXMatrixRotationZ( &matrixProjection, (float)D3DX_PI / 2.0f );
                    break;
                case DMDO_180 :
                    D3DXMatrixRotationZ( &matrixProjection, (float)D3DX_PI );
                    break;
                case DMDO_270 :
                    D3DXMatrixRotationZ( &matrixProjection, (float)3.0f * D3DX_PI / 2.0f );
                    break;
            }

        Check_HRFail( m_pRenderingDevice->SetTransform( D3DTS_PROJECTION, &matrixProjection ), "SetTransform(PROJECTION)" );
        m_bCanRender = true;
    }
    
    float fXStretch = 1.0f, fYStretch = 1.0f;

    for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
    {
        // backbuffer rectangle, used in IntersectRect
        RECT    rectBackbuffer;
        SetRect(    &rectBackbuffer, 
                    0, 
                    0, 
                    m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uWidth, 
                    m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].uHeight );

        // create the Source surface
        if( !IntersectRect( &( m_SwapChainData[uSwapChain].m_rectSource ),
                            &( m_SwapChainData[uSwapChain].m_rectPresentSource ),
                            &rectBackbuffer 
                        ) 
            )
            return false;        // bad source rectangle

        
        if( FAILED( hResult =  m_pRenderingDevice->CreateImageSurface(
                                                        m_SwapChainData[uSwapChain].m_rectSource.right - m_SwapChainData[uSwapChain].m_rectSource.left,
                                                        m_SwapChainData[uSwapChain].m_rectSource.bottom - m_SwapChainData[uSwapChain].m_rectSource.top,
                                                        m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain].BackBufferFormat,
                                                        &( m_SwapChainData[uSwapChain].m_pSourceSurface )
                                                    ) 
                    )
            )
        {
            WriteToLog( _T( "CreateImageSurface failed, return code %s (0x%x)\n" ),
                            m_pD3D->HResultToString( hResult ),
                            hResult );
            PrepareNextTest();
            return FailTestCase();
        }

        // stretch factors
        fXStretch =    ( m_SwapChainData[uSwapChain].m_rectPresentDest.right - m_SwapChainData[uSwapChain].m_rectPresentDest.left ) / 
                    (float)( m_SwapChainData[uSwapChain].m_rectPresentSource.right - m_SwapChainData[uSwapChain].m_rectPresentSource.left );
        fYStretch =    ( m_SwapChainData[uSwapChain].m_rectPresentDest.bottom - m_SwapChainData[uSwapChain].m_rectPresentDest.top ) / 
                    (float)( m_SwapChainData[uSwapChain].m_rectPresentSource.bottom - m_SwapChainData[uSwapChain].m_rectPresentSource.top );

        // create the Dest surface
        m_SwapChainData[uSwapChain].m_rectDest = m_SwapChainData[uSwapChain].m_rectPresentDest;
        m_SwapChainData[uSwapChain].m_rectDest.left += (LONG) ceil( fXStretch * ( m_SwapChainData[uSwapChain].m_rectSource.left - m_SwapChainData[uSwapChain].m_rectPresentSource.left ) );
        m_SwapChainData[uSwapChain].m_rectDest.right -= (LONG) floor( fXStretch * ( -m_SwapChainData[uSwapChain].m_rectSource.right + m_SwapChainData[uSwapChain].m_rectPresentSource.right ) );
        m_SwapChainData[uSwapChain].m_rectDest.top += (LONG) ceil( fYStretch * ( m_SwapChainData[uSwapChain].m_rectSource.top - m_SwapChainData[uSwapChain].m_rectPresentSource.top ) );
        m_SwapChainData[uSwapChain].m_rectDest.bottom -= (LONG) floor( fYStretch * ( -m_SwapChainData[uSwapChain].m_rectSource.bottom + m_SwapChainData[uSwapChain].m_rectPresentSource.bottom ) );
 
        if( FAILED( hResult = m_pRenderingDevice->CreateImageSurface(    
                                                        m_SwapChainData[uSwapChain].m_rectDest.right - m_SwapChainData[uSwapChain].m_rectDest.left,
                                                        m_SwapChainData[uSwapChain].m_rectDest.bottom - m_SwapChainData[uSwapChain].m_rectDest.top,
                                                        FMT_A8R8G8B8,
                                                        &m_SwapChainData[uSwapChain].m_pDestSurface ) 
                    )
            )
        {
            WriteToLog( _T( "CreateImageSurface failed, return code %s (0x%x)\n" ),
                            m_pD3D->HResultToString( hResult ),
                            hResult );
            PrepareNextTest();
            return FailTestCase();
        }
        
        // create the D3DX sysmem surface
        if( !IntersectRect( &( m_SwapChainData[uSwapChain].m_rectDestVisible ), 
                            &( m_SwapChainData[uSwapChain].m_rectDest ),
                            &( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow )
                        ) 
            )
            return false;
        // offset D3DX buffer rectangle, to specify valid coordinates in dest surface
        m_SwapChainData[uSwapChain].m_rectD3DXRef = m_SwapChainData[uSwapChain].m_rectDestVisible;
        OffsetRect( &( m_SwapChainData[uSwapChain].m_rectD3DXRef ),
                    -m_SwapChainData[uSwapChain].m_rectDest.left,
                    -m_SwapChainData[uSwapChain].m_rectDest.top );
      
        if( FAILED( hResult = m_pRenderingDevice->CreateImageSurface(    
                                                        m_SwapChainData[uSwapChain].m_rectD3DXRef.right - m_SwapChainData[uSwapChain].m_rectD3DXRef.left,
                                                        m_SwapChainData[uSwapChain].m_rectD3DXRef.bottom - m_SwapChainData[uSwapChain].m_rectD3DXRef.top,
                                                        FMT_A8R8G8B8,
                                                        &( m_SwapChainData[uSwapChain].m_pD3DXRefBuffer ) ) 
                    )
            )
        {
            WriteToLog( _T( "CreateImageSurface failed, return code %s (0x%x)\n" ),
                            m_pD3D->HResultToString( hResult ),
                            hResult );
            PrepareNextTest();
            return FailTestCase();
        }

        // create diff surface
        if( FAILED( hResult = m_pRenderingDevice->CreateImageSurface(    
                                                        m_SwapChainData[uSwapChain].m_rectDest.right - m_SwapChainData[uSwapChain].m_rectDest.left,
                                                        m_SwapChainData[uSwapChain].m_rectDest.bottom - m_SwapChainData[uSwapChain].m_rectDest.top,
                                                        FMT_A8R8G8B8,
                                                        &( m_SwapChainData[uSwapChain].m_pDiffSurface ) ) 
                    )
            )
        {
            WriteToLog( _T( "CreateImageSurface failed, return code %s (0x%x)\n" ),
                            m_pD3D->HResultToString( hResult ),
                            hResult );
            PrepareNextTest();
            return FailTestCase();
        }
    }

    // update dirty region
    SAFEDELETEA( m_pDirtyRegion );
    SAFEDELETEA( m_pDestDirtyRegion );
    if( NULL != m_vecRegData[m_nIndexDirtyRegion] )
    {

        UINT regionSize = sizeof(m_vecRegData[m_nIndexDirtyRegion]->rdh) + m_vecRegData[m_nIndexDirtyRegion]->rdh.nCount * sizeof(RECT);

        m_pDirtyRegion = (RGNDATA*)new char[regionSize];
        m_pDestDirtyRegion = (RGNDATA*)new char[regionSize];
        if( NULL == m_pDirtyRegion || NULL == m_pDestDirtyRegion )
        {
            WriteToLog( "Out of memory\n" );
            PrepareNextTest();
            return FailTestCase();
        }
        memcpy( m_pDirtyRegion, m_vecRegData[m_nIndexDirtyRegion], regionSize );
        memcpy( m_pDestDirtyRegion, m_pDirtyRegion, regionSize );

        // compute all the rects in the region
        int i;
        for(i = 0; i < m_pDirtyRegion->rdh.nCount; i++ )
        {
            // compute rect in source
            RECT srcRect, clippedRect, destRect;
            if( NULL == m_prectPresentSource )
                srcRect = CreateRect( *(RECT*)(m_pDirtyRegion->Buffer + i * sizeof RECT), m_vecPresentParameters[m_nIndexPresentParameters].uWidth, m_vecPresentParameters[m_nIndexPresentParameters].uHeight );
            else
            {
                srcRect.left    = m_SwapChainData[0].m_rectPresentSource.left + ( m_SwapChainData[0].m_rectPresentSource.right - m_SwapChainData[0].m_rectPresentSource.left) * ((RECT*)(m_pDirtyRegion->Buffer + i * sizeof RECT))->left / 100;
                srcRect.top     = m_SwapChainData[0].m_rectPresentSource.top + ( m_SwapChainData[0].m_rectPresentSource.bottom - m_SwapChainData[0].m_rectPresentSource.top) * ((RECT*)(m_pDirtyRegion->Buffer + i * sizeof RECT))->top / 100;
                srcRect.right   = m_SwapChainData[0].m_rectPresentSource.left + ( m_SwapChainData[0].m_rectPresentSource.right - m_SwapChainData[0].m_rectPresentSource.left) * ((RECT*)(m_pDirtyRegion->Buffer + i * sizeof RECT))->right / 100;
                srcRect.bottom  = m_SwapChainData[0].m_rectPresentSource.top + ( m_SwapChainData[0].m_rectPresentSource.bottom - m_SwapChainData[0].m_rectPresentSource.top) * ((RECT*)(m_pDirtyRegion->Buffer + i * sizeof RECT))->bottom / 100;
            }
            *(RECT*)(m_pDirtyRegion->Buffer + i * sizeof RECT) = srcRect;

            // clip dirty source by visible source
            IntersectRect( &clippedRect, &srcRect, &m_SwapChainData[0].m_rectSource );
    
            // translate and stretch clippedRect in dest
            destRect        = m_SwapChainData[0].m_rectPresentDest;
            destRect.left   += (LONG) ceil( fXStretch * ( clippedRect.left  - m_SwapChainData[0].m_rectPresentSource.left) );

            destRect.right  -= (LONG) floor( fXStretch * ( -clippedRect.right + m_SwapChainData[0].m_rectPresentSource.right) );
            destRect.top    += (LONG) ceil( fYStretch * ( clippedRect.top - m_SwapChainData[0].m_rectPresentSource.top) );
            destRect.bottom -= (LONG) floor( fYStretch * (-clippedRect.bottom + m_SwapChainData[0].m_rectPresentSource.bottom) );

            *(RECT*)(m_pDestDirtyRegion->Buffer + i * sizeof RECT) = destRect;

        }

        // compute bounding rectangle
        m_pDirtyRegion->rdh.rcBound = *(RECT*)(m_pDirtyRegion->Buffer);
        for( i = 1; i < m_pDirtyRegion->rdh.nCount; i++ )
        {
            RECT rect = m_pDirtyRegion->rdh.rcBound;
            UnionRect( &m_pDirtyRegion->rdh.rcBound, &rect, (RECT*)(m_pDirtyRegion->Buffer + i * sizeof RECT) );
        }
    }
    else
        m_pDirtyRegion = NULL;

    /*if( ClearBackBuffers() )
    {
        // Clear the viewport
        if( FAILED( hResult = m_pRenderingDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, COLOR_ERASE.color, 1.0f, 0L ) ) )
        {
            WriteToLog( TEXT( "Clear failed, error code %s (0x%x)\n" ),
                        m_pD3D->HResultToString( hResult ),
                        hResult );
            PrepareNextTest();
            return FailTestCase();
        }
        if( FAILED( hResult = m_pRenderingDevice->Present( NULL, NULL, m_vecPresentWindow[m_nIndexPresentWindow], NULL ) ) )
        {
            WriteToLog( TEXT( "Present failed, error code %s (0x%x)\n" ),
                        m_pD3D->HResultToString( hResult ),
                        hResult );
            PrepareNextTest();
            return FailTestCase();
        }
    }*/

    // compute the absolute rects for all the dialogs
    for( vector<DialogBoxDescription>::iterator itDialog = m_vecWindowDescription.begin(); itDialog < m_vecWindowDescription.end(); itDialog++ )
        if( !(*itDialog).bAbsolutePosition )
        {
            RECT rectWnd;
            GetWindowRect( m_hWndPresent, &rectWnd );
                
            // create relative rect
            RECT rectRelative = CreateRect( (*itDialog).indexXScale, (*itDialog).indexYScale, (*itDialog).indexOffset );
            // create absolute rect
            (*itDialog).rect = CreateRect( rectRelative, rectWnd.right - rectWnd.left, rectWnd.bottom - rectWnd.top );
            
            // offset by the window position
            OffsetRect( &(*itDialog).rect, rectWnd.left, rectWnd.top );
        }

    // hide all the dlgs not in the current test and display all the others
    for( int iDlgDescription = 0; iDlgDescription < m_vecWindowDescription.size(); iDlgDescription++ )
        if( m_vecIntWindows[m_nIndexDialogs].end() ==
            find( m_vecIntWindows[m_nIndexDialogs].begin(), m_vecIntWindows[m_nIndexDialogs].end(), iDlgDescription ) 
          )
            ShowWindow( m_vecWindowDescription[iDlgDescription].hDlg, SW_HIDE );
        else    // dialog in the test
            {
                ShowWindow( m_vecWindowDescription[iDlgDescription].hDlg, SW_SHOW );
                RECT rect = m_vecWindowDescription[iDlgDescription].rect;
                MoveWindow( m_vecWindowDescription[iDlgDescription].hDlg,
                            rect.left,
                            rect.top,
                            rect.right - rect.left,
                            rect.bottom - rect.top,
                            TRUE );
                InvalidateRect( m_vecWindowDescription[iDlgDescription].hDlg, NULL, TRUE );
                UpdateWindow(  m_vecWindowDescription[iDlgDescription].hDlg );
                Sleep( 100 );
                
                MSG msg;
                while( ::PeekMessage( &msg, m_vecWindowDescription[iDlgDescription].hDlg, 0, 0, PM_NOREMOVE ) && msg.message != WM_QUIT )
                {
                    if( ::PeekMessage( &msg, m_vecWindowDescription[iDlgDescription].hDlg, 0, 0, PM_REMOVE ) )
                    {
                        ::TranslateMessage( &msg );
                        ::DispatchMessage( &msg );
                    }
                }
            }

    m_pFramework->ProcessMessages();
    
    // display the frame and capture the backbuffer
    m_bSnapshot = true;
    DisplayFrame();
    m_bSnapshot = false;

    m_nLastIndexPresentParameters = m_nIndexPresentParameters;
    m_nLastIndexTexture = m_nIndexTexture;
    m_LastDeviceReleaseBeforeVerify = m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify];
    m_nLastIndexWindowTrees = m_nIndexWindowTrees;

    // see winos bug #1896386 - DWM incorrectly overrides window with black with new D3DPRESENT_RESTRICT_TO_MONITOR flag even if window is fully contained in the main monitor
    /*if
    ( 
        D3DPRESENTFLAG_VIDEO & m_vecPresentParameters[m_nIndexPresentParameters].dwFlags &&
        NULL != m_pDirtyRegion &&
        m_bDWMEnabled
    )
    {
        Skip();
        return false;
    }*/
}
catch(...)
{
    WriteToLog( _T( "Exception caught during test execution\n" ) );    
}

    //EndTestCase();
    return true;
}


//////////////////////////////////////////////////////////////////////
// Name:    DisplayFrame()
// Desc:    override present call
//////////////////////////////////////////////////////////////////////
bool CPresentII::DisplayFrame()
{
    if( !m_bCanRender || !m_bSnapshot )
        return true;

    try
    {
        // Test the cooperative level to see if it's okay to render
        Check_HRFail( m_pRenderingDevice->TestCooperativeLevel(), "TestCooperativeLevel" );

        for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
        {
            LPSURFACE pBBSurface = NULL;

            ASSERT( uSwapChain < m_vecAdapterMonitors.size() );
            ASSERT( NULL != m_vecAdapterMonitors[uSwapChain].m_pSwapChain );

            // set the render target to the swapchain's backbuffer
            if( m_uNumberSwapChains > 1 || m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseSwapChains)
            {
                Check_HRFail( m_vecAdapterMonitors[uSwapChain].m_pSwapChain->GetBackBuffer( m_nIndexBB, D3DBACKBUFFER_TYPE_MONO, &pBBSurface ), "GetBackBuffer" );
                Check_HRFail( m_pRenderingDevice->SetRenderTarget( (DWORD)0, pBBSurface ), "SetRenderTarget" );
            }
    
            // Clear the viewport
            if( ClearBackBuffers() )
            {
                Check_HRFail( m_pRenderingDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, COLOR_ERASE.color, 1.0f, 0L ), "Clear" );
                HRESULT hrPresent = m_vecAdapterMonitors[uSwapChain].m_pSwapChain->Present( NULL, NULL, m_vecPresentWindow[m_nIndexPresentWindow], NULL );
                
                HRESULT handledCode = HandlePresentReturnCode( hrPresent );
                switch( handledCode )
                {
                    case E_ABORT:
                    case E_OUTOFMEMORY:
                    default:
                        if( FAILED( hrPresent ) || FAILED(handledCode))
                        {
                            WriteToLog( TEXT( "Present (DisplayFrame) failed, error code %s (0x%x). HandlePresentReturnCode code %s (0x%x)\n" ),
                                m_pD3D->HResultToString( hrPresent ),
                                hrPresent,
                                m_pD3D->HResultToString( handledCode ),
                                handledCode);
                            PrepareNextTest();
                            return FailTestCase();
                        }    
                };

            }
            
            // render the scene
            RenderScene( uSwapChain, m_nIndexBB );

            // release the swapchain's backbuffer
            if( m_uNumberSwapChains > 1  || m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseSwapChains)
            {
                RELEASE( pBBSurface );
            }

            if( m_bSnapshot )
            {
                if( UpdateBackBuffer() )
                {    
                    // copy the backbuffer to SourceSurface with no color conversion here and no filtering
                    Check_HRFail( m_pRenderingDevice->D3DXLoadSurfaceFromSurface(    
                                                            m_SwapChainData[uSwapChain].m_pSourceSurface,
                                                            NULL,
                                                            NULL,
                                                            m_SwapChainData[uSwapChain].m_vecBB[m_nIndexBB],
                                                            NULL,
                                                            &m_SwapChainData[uSwapChain].m_rectSource,
                                                            D3DX_FILTER_NONE,
                                                            0 ),
                            "D3DXLoadSurfaceFromSurface" );
                }
            }

        }
        // Show the frame on the primary surface.
        Present();
        DWORD pause = GetDebugPause();

        // Work around for Windows 7 Bug : 696107
        // Wait for DWM animation to finish transitioning the dialog box to its correct size and position

        if ( m_TestType == TEST_SETDIALOGBOXMODE && m_bDWMEnabled )
            pause = 1000;


        {
            Sleep( pause );
        };
        if ( m_TestType == TEST_SETDIALOGBOXMODE && m_bDWMEnabled )
            pause = 1000;

        if( pause > 0)
        {
            Sleep( pause );
        };
    }
    catch( HRESULT hError )
    {
        return false;
    }
    return true;
}


//////////////////////////////////////////////////////////////////////
// Name:    Present()
// Desc:    implementation of Present (either using the device's shortcut or swapchain's method)
//////////////////////////////////////////////////////////////////////
void CPresentII::Present()
{
    HRESULT hrPresent = S_OK;

    ////
    ////  Print out dirty regions
    ////
    //if(m_pDirtyRegion  != NULL)
    //{
    //    for( int i = 0; i < m_pDirtyRegion->rdh.nCount; i++ )
    //    {
    //        RECT rectDest = *(RECT*)(m_pDirtyRegion->Buffer + i * sizeof RECT);
    //        WriteToLog("At present: InBB coordinates. m_pDirtyRegion.left = %d,m_pDirtyRegion.top = %d,m_pDirtyRegion.right = %d,m_pDirtyRegion.bottom = %d",rectDest.left,rectDest.top,rectDest.right,rectDest.bottom);
    //    }
    //}
    
    //
    //  Present from the swapchain if we are testing releasing the swapchain
    //
    if( m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseSwapChains)
    {        
        for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
        {
            hrPresent =  m_vecAdapterMonitors[uSwapChain].m_pSwapChain->Present(  m_prectPresentSource, 
                m_prectPresentDest, 
                m_vecPresentWindow[m_nIndexPresentWindow], 
                m_pDirtyRegion
                );
            
            if( FAILED(hrPresent))
            {
                break;
            }
        }
    }
    else
    {
        hrPresent = m_pRenderingDevice->Present(  m_prectPresentSource, 
            m_prectPresentDest, 
            m_vecPresentWindow[m_nIndexPresentWindow], 
            m_pDirtyRegion
            );
    }

    HRESULT handledCode = HandlePresentReturnCode( hrPresent );
    switch( handledCode )
    {
        case E_ABORT:
        case E_OUTOFMEMORY:
        default:
            if( FAILED( hrPresent ) || FAILED(handledCode))
            {
                Check_HRFail(hrPresent,TEXT( "Present (CPresentII::Present)" ));
                Check_HRFail(handledCode,TEXT( "Present (CPresentII::Present)" ));
            }    
    };
}


//////////////////////////////////////////////////////////////////////
// Name:    ProcessFrame()
// Desc:    process the frame
//////////////////////////////////////////////////////////////////////
bool CPresentII::ProcessFrame()
{
    static DWORD d3dxAllFilters[] = { D3DX_FILTER_TRIANGLE, D3DX_FILTER_LINEAR, D3DX_FILTER_POINT };
    // config parameters
    m_d3dxFilter    = 0;
    // fullscreen apps don't have stretching so we demand more conformance

    try
    {
        CDevice *pTempDevice = NULL;
        DEVICE_CREATION_PARAMETERS deviceCreationParameters = { 0 };
        DWORD originalBehaviors = 0;
        HRESULT hResult = S_OK;

        //
        // Code assumes there is only one swapchain
        //
        if(m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseDevice)
        {
            
            //
            // Clone the device as a temporarily holder for the test device's resources, so the test device can be deleted
            //
            m_pRenderingDevice->GetCreationParameters( &deviceCreationParameters );

            //
            // GetCreationParameters returns D3DDeviceType but we need framework's DWORD DeviceType
            // in order to use in CreateDevice
            //
            DWORD deviceType = ConvertDeviceType(deviceCreationParameters.DeviceType);
            deviceCreationParameters.DeviceType = (D3DDEVTYPE)deviceType;

            originalBehaviors = m_pRenderingDevice->GetBehaviors();

            PRESENT_PARAMETERS *pSrcDevicePresentParameters = new PRESENT_PARAMETERS[m_uNumberSwapChains];
            ZeroMemory( pSrcDevicePresentParameters, sizeof( PRESENT_PARAMETERS)  * m_uNumberSwapChains );
            memcpy( pSrcDevicePresentParameters, &m_vecPresentParameters[m_nIndexPresentParameters], sizeof( PRESENT_PARAMETERS) * m_uNumberSwapChains  );
   
            
            if(m_vecPresentParameters[m_nIndexPresentParameters].SwapEffect == SWAPEFFECT_FLIPEX)
            {
                pSrcDevicePresentParameters[0].SwapEffect = SWAPEFFECT_DISCARD;
            }


            if( FAILED( hResult = m_pD3D->CreateDevice(
                        deviceCreationParameters.AdapterOrdinal,
                        deviceCreationParameters.DeviceType,
                        deviceCreationParameters.hFocusWindow,
                        originalBehaviors,
                        pSrcDevicePresentParameters,
                        &pTempDevice
                        )
                    )
                )
            {
                WriteToLog(
                    _T( "CreateDevice() returned %s (0x%x)\n" ),
                    m_pD3D->HResultToString(hResult),
                    hResult
                    );
                SAFEDELETEA(pSrcDevicePresentParameters);
                return false;
            }
            
            SAFEDELETEA(pSrcDevicePresentParameters);
        }
        
        //
        //  Get the window region and its region data
        //
        LPRGNDATA pRegionData = NULL;
        HRGN testWindowRegion;
        if( FAILED( hResult = GetWindowRegionData(m_hWndPresent, &testWindowRegion, &pRegionData)))
        {
            WriteToLog(
                _T( "GetWindowRegionData() returned %s (0x%x)\n" ),
                m_pD3D->HResultToString(hResult),
                hResult
                );
            return false;      
        }
        
        
        
        bool dwmEnabled = IsDWMEnabled();

        if(m_SwapEffect == SWAPEFFECT_FLIPEX && !dwmEnabled)
        {
            WriteToLog( _T("Skip: DWM was off for flipex tests. Not really valid test of flipex") );
            TestFinish(D3DTESTFINISH_SKIPREMAINDER);

        }

        //
        // m_uNumberSwapChains is either 1 for windowed or 1 per adapter for fullscreen
        //
        for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
        {
           
              // reset output parameters
            m_ColorBalance.color    = 0;
            m_fLocalConformance        = 0.0f;
            m_fOverallRedConformance = m_fOverallGreenConformance = m_fOverallBlueConformance = 0.0f;
            m_bPreserveBackground    = true;

            //
            // copy the source surface to dest surface, with color conversion and filtering.
            // copy whole surface if dirty region is NULL or dirtyregion count is >= LOCAL_D3DKMT_MAX_PRESENT_HISTORY_RECTS 
            // with FLipEX
            //
            if( NULL == m_pDirtyRegion ||
                ( m_vecPresentParameters[m_nIndexPresentParameters].SwapEffect == SWAPEFFECT_FLIPEX &&
                    m_pDestDirtyRegion->rdh.nCount >= LOCAL_D3DKMT_MAX_PRESENT_HISTORY_RECTS && m_bDWMEnabled))
                Check_HRFail( m_pRenderingDevice->D3DXLoadSurfaceFromSurface(    
                                                        m_SwapChainData[uSwapChain].m_pDestSurface,    // LPDIRECT3DSURFACE8 pDestSurface,
                                                        NULL,                    // CONST PALETTEENTRY* pDestPalette,
                                                        NULL,                    // CONST RECT* pDestRect,
                                                        m_SwapChainData[uSwapChain].m_pSourceSurface,        // LPDIRECT3DSURFACE8 pSrcSurface,
                                                        NULL,                    // CONST PALETTEENTRY* pSrcPalette,
                                                        NULL,                    // CONST RECT* pSrcRect,
                                                        d3dxAllFilters[m_d3dxFilter], // DWORD Filter,
                                                        0 ),                    // D3DCOLOR ColorKey
                            "D3DXLoadSurfaceFromSurface" );
            else
            {
                // clear the surface and copy in the region parts from the source surface
                LOCKED_RECT        lrSurface;
                Check_HRFail( m_SwapChainData[uSwapChain].m_pDestSurface->LockRect( &lrSurface, NULL, 0 ), "LockRect(DestSurface - COLOR_ERASE)" );
            
                
                DWORD eraseColor;
                //
                // m_pRenderingDevice->GetAdapterID() + 1, just so IsRestrictToMonitor will give general answer
                // of if its enabled, not if its enable for an adapter.  
                // see winos bug #1896386 - DWM incorrectly overrides window with black with new D3DPRESENT_RESTRICT_TO_MONITOR flag even if window is fully contained in the main monitor
                //                    
                if(IsRestrictToMonitor(0, m_pRenderingDevice->GetAdapterID() + 1, m_nIndexPresentParameters) && m_bDWMEnabled)
                {
                    //
                    // Window background will be black if DWM is on.
                    //
                    eraseColor = COLOR_BLACK.color;
                }
                else
                {
                    eraseColor = COLOR_ERASE.color;
                }

                POINT Pixel;
                for( Pixel.y = m_SwapChainData[uSwapChain].m_rectDest.top; Pixel.y < m_SwapChainData[uSwapChain].m_rectDest.bottom ; Pixel.y++ )
                    for( Pixel.x = m_SwapChainData[uSwapChain].m_rectDest.left; Pixel.x < m_SwapChainData[uSwapChain].m_rectDest.right; Pixel.x++ )
                        *(DWORD*)( (UINT_PTR)lrSurface.pBits + 
                                        ( Pixel.y - m_SwapChainData[uSwapChain].m_rectDest.top ) * lrSurface.iPitch + 
                                        ( Pixel.x - m_SwapChainData[uSwapChain].m_rectDest.left ) * 4 ) = eraseColor;

                Check_HRFail( m_SwapChainData[uSwapChain].m_pDestSurface->UnlockRect(), "UnlockRect" );

                if(m_vecPresentParameters[m_nIndexPresentParameters].SwapEffect == SWAPEFFECT_FLIPEX &&
                    m_bDWMEnabled)
                {

                    if( ( m_pDirtyRegion->rdh.nCount >= LOCAL_c_maxDirtyRegions )  && 
                        ( m_pDirtyRegion->rdh.nCount < LOCAL_D3DKMT_MAX_PRESENT_HISTORY_RECTS ) )
                    {
                        //
                        //  DWM limit reached. Expect 1 unioned rect
                        //
                        RECT unionedDestRect = *((RECT*)(m_pDestDirtyRegion->Buffer));

                        for( UINT i = 1; i < m_pDestDirtyRegion->rdh.nCount; i++ )
                        {
                            RECT rect = unionedDestRect;
                            UnionRect( &unionedDestRect, &rect, (RECT*)(m_pDestDirtyRegion->Buffer + i * sizeof RECT) );
                        }


                        RECT unionedSrcRect = *((RECT*)(m_pDirtyRegion->Buffer));

                        for( UINT i = 1; i < m_pDirtyRegion->rdh.nCount; i++ )
                        {
                            RECT rect = unionedSrcRect;
                            UnionRect( &unionedSrcRect, &rect, (RECT*)(m_pDirtyRegion->Buffer + i * sizeof RECT) );
                        }

                        //
                        // Regions are not reused pre test case so we can union them in place.
                        //
                        SAFEDELETEA( m_pDirtyRegion );
                        SAFEDELETEA( m_pDestDirtyRegion );


                        UINT size = 1;
                        UINT regionSize = sizeof(m_pDestDirtyRegion->rdh) +size * sizeof(RECT);
                        m_pDestDirtyRegion = (RGNDATA*)new char[regionSize];
                        m_pDestDirtyRegion->rdh.nCount = size;

                        *(RECT*)(m_pDestDirtyRegion->Buffer) = unionedDestRect;

                        regionSize = sizeof(m_pDirtyRegion->rdh) + size * sizeof(RECT);
                        m_pDirtyRegion = (RGNDATA*)new char[regionSize];
                        m_pDirtyRegion->rdh.nCount = size;

                        *(RECT*)(m_pDirtyRegion->Buffer) = unionedSrcRect;
                    }
                }

                // fill the surface with rects from dirty region
                for( int i = 0; i < m_pDestDirtyRegion->rdh.nCount; i++ )
                {
                    RECT rectDest = *(RECT*)(m_pDestDirtyRegion->Buffer + i * sizeof RECT), rectSource;
                    
                    // clip dirty source by visible source
                    IntersectRect( &rectSource, (RECT*)(m_pDirtyRegion->Buffer + i * sizeof RECT), &m_SwapChainData[uSwapChain].m_rectSource );
                    OffsetRect( &rectSource, -m_SwapChainData[uSwapChain].m_rectSource.left, -m_SwapChainData[uSwapChain].m_rectSource.top );
                    OffsetRect( &rectDest, -m_SwapChainData[uSwapChain].m_rectDest.left, -m_SwapChainData[uSwapChain].m_rectDest.top );

                    if( rectSource.right - rectSource.left > 0 &&
                        rectSource.bottom - rectSource.top > 0 &&
                        rectDest.right - rectDest.left > 0 &&
                        rectDest.bottom - rectDest.top > 0 )
                        Check_HRFail( m_pRenderingDevice->D3DXLoadSurfaceFromSurface(    
                                                            m_SwapChainData[uSwapChain].m_pDestSurface,  // LPDIRECT3DSURFACE8 pDestSurface,
                                                            NULL,                                        // CONST PALETTEENTRY* pDestPalette,
                                                            &rectDest,                                   // CONST RECT* pDestRect,
                                                            m_SwapChainData[uSwapChain].m_pSourceSurface,// LPDIRECT3DSURFACE8 pSrcSurface,
                                                            NULL,                                        // CONST PALETTEENTRY* pSrcPalette,
                                                            &rectSource,                                 // CONST RECT* pSrcRect,
                                                            d3dxAllFilters[m_d3dxFilter],                // DWORD Filter,
                                                            0 ),                                         // D3DCOLOR ColorKey
                                "D3DXLoadSurfaceFromSurface" );
                }
            

            }
            //
            // Copy the device clip color into the right place in destination surface
            //
            for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
            {       
                RECT clearRect;
                clearRect.left = min( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.right - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.left, 
                    m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.left - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.left );
                clearRect.top = min( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.bottom - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.top, 
                    m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.top - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.top );
                clearRect.right = min(    m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.right, 
                    m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.right - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.left );
                clearRect.bottom = min(    m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.bottom, 
                    m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.top );

                if(IsDeviceClip(uSwapChain, uAdapter, m_nIndexPresentParameters))
                {

                    OffsetRect(&clearRect, -m_SwapChainData[uSwapChain].m_rectDest.left, -m_SwapChainData[uSwapChain].m_rectDest.top);  

                    RECT surfaceSize = {0};
                    surfaceSize.left = 0;
                    surfaceSize.top = 0;
                    surfaceSize.right = m_SwapChainData[uSwapChain].m_rectDest.right - m_SwapChainData[uSwapChain].m_rectDest.left;
                    surfaceSize.bottom = m_SwapChainData[uSwapChain].m_rectDest.bottom - m_SwapChainData[uSwapChain].m_rectDest.top;

                    DWORD clippedColor;
                    if(IsRestrictToMonitor(uSwapChain, uAdapter, m_nIndexPresentParameters) && m_bDWMEnabled)
                    {
                        //
                        // Window background will be black if DWM is on.
                        //
                        clippedColor = COLOR_BLACK.color;
                    }
                    else
                    {
                        clippedColor = m_COLOR_WINDOW.color;
                    }

                    WriteToLog("fillcolor %x", clippedColor);
                    FillSubRectColor(m_SwapChainData[uSwapChain].m_pDestSurface, surfaceSize, clearRect, clippedColor);
                }

            }
            //
            // If there is a window region, get the inverse and copy the background to m_pDestSurface.
            //
            //
            if(NULL != testWindowRegion)
            {
                //
                // Get the inverted part of this region
                //
                HRGN wholeWindowRegion = CreateRectRgn(
                    m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.left,
                    m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.top,
                    m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.right,
                    m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.bottom );

                HRGN invertedRgn = CreateRectRgn(0, 0, 0, 0 );
                int ret =  CombineRgn(
                    invertedRgn,
                    wholeWindowRegion,
                    testWindowRegion,
                    RGN_DIFF);    

                if(ret == ERROR || ret == NULLREGION)
                {
                    DeleteObject(invertedRgn);
                    invertedRgn = NULL;
                    DWORD e = GetLastError();
                    HRESULT hr = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 

                    WriteToLog( _T("CombineRgn failed hr=0x%x\n"),  hr);
                    PrepareNextTest();
                    return FailTestCase();
                }

                UINT invertedRegionSize = GetRegionData(invertedRgn, 0, NULL);
                LPRGNDATA pInvertedRegionData = (RGNDATA*)new char[invertedRegionSize];
                int bytes = GetRegionData(invertedRgn, invertedRegionSize, pInvertedRegionData);
                if(!bytes)
                {
                    DWORD e = GetLastError();
                    HRESULT hr = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                    WriteToLog( "GetRegionData failed\n" );
                    WriteToLog( "GetRegionData bytes %d regionSize %d %x\n",bytes, invertedRegionSize, hr );

                    PrepareNextTest();
                    return FailTestCase();  
                }

                //
                // Scale region by the dpi factor
                //
                XFORM xf = { m_fDPIFactor, 0, 0, m_fDPIFactor, 0, 0 };
                HRGN scaledInvertedRgn = ExtCreateRegion(&xf, invertedRegionSize, pInvertedRegionData);
                if(!scaledInvertedRgn)
                {
                    DWORD e = GetLastError();
                    HRESULT hr = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                    WriteToLog( "ExtCreateRegion failed\n" );

                    PrepareNextTest();
                    return FailTestCase(); 
                }

                if(!GetRegionData(scaledInvertedRgn, invertedRegionSize, pInvertedRegionData))
                {
                    DWORD e = GetLastError();
                    HRESULT hr = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                    WriteToLog( "GetRegionData failed\n" );
                    WriteToLog( "GetRegionData bytes %d regionSize %d %x\n",bytes, invertedRegionSize, hr );

                    PrepareNextTest();
                    return FailTestCase();  
                }

                for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
                {

                    if( IsRectEmpty( &( m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer ) ) )
                        continue;

                    //
                    // a rect with no top left offsets
                    //
                    RECT noOffset = { 0 };
                    noOffset.left = 0;
                    noOffset.top = 0;
                    noOffset.right = m_SwapChainData[uSwapChain].m_rectDest.right - m_SwapChainData[uSwapChain].m_rectDest.left;
                    noOffset.bottom = m_SwapChainData[uSwapChain].m_rectDest.bottom - m_SwapChainData[uSwapChain].m_rectDest.top;

                    
                    //
                    // Find the clear rect dimmensions on this adapter
                    //
                    RECT clearRect;

                    clearRect.left = min( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.right - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.left, 
                        m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.left - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.left );
                    clearRect.top = min( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.bottom - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.top, 
                        m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.top - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.top );
                    clearRect.right = min(    m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.right, 
                        m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.right - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.left );
                    clearRect.bottom = min(    m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.bottom, 
                        m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.top );

                    Check_HRFail(CopyClippedRegion(
                        pInvertedRegionData,
                        m_SwapChainData[uSwapChain].m_rectDest,
                        clearRect,
                        m_vecAdapterMonitors[uAdapter].m_pClearBuffer,
                        noOffset,
                        m_SwapChainData[uSwapChain].m_pDestSurface,
                        d3dxAllFilters[m_d3dxFilter]),_T("CopyClippedRegion"));
                }

                DeleteObject(wholeWindowRegion);
                SAFEDELETEA(pInvertedRegionData);
                DeleteObject(invertedRgn);
                DeleteObject(scaledInvertedRgn);
            }

            // copy the dest surface to D3DXRef with no color conversion and no filtering
            Check_HRFail( m_pRenderingDevice->D3DXLoadSurfaceFromSurface(    
                m_SwapChainData[uSwapChain].m_pD3DXRefBuffer,
                NULL,
                NULL,
                m_SwapChainData[uSwapChain].m_pDestSurface,
                NULL,
                &( m_SwapChainData[uSwapChain].m_rectD3DXRef ),
                D3DX_FILTER_NONE,
                0 ),
                "D3DXLoadSurfaceFromSurface" );

            // Do any postprocessing to the REF buffer (like gamma correct it)
            ProcessREFBuffer( uSwapChain );

            if (m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseSwapChains)
            {
                for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
                {
                    RELEASE(m_vecAdapterMonitors[uSwapChain].m_pSwapChain);
                    
                    Check_HRFail(
                        m_pRenderingDevice->CreateAdditionalSwapChain( 
                            &m_vecPresentParameters[m_nIndexPresentParameters + uSwapChain], 
                            &m_vecAdapterMonitors[uSwapChain].m_pSwapChain ), "CreateAdditionalSwapChain" );
                }
            }
            else if(m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseDevice)
            {
            WriteToLog(_T("Releasing device and resources.  m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseDevice=%d"),m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseDevice);
            LPSURFACE pTempD3DXRefBuffer = NULL;

            Check_HRFail(CreateAndCopySurfaceFromSurface(
                m_SwapChainData[uSwapChain].m_pD3DXRefBuffer,
                pTempDevice,
                &pTempD3DXRefBuffer),"CreateAndCopySurfaceFromSurface");

            RELEASE(m_SwapChainData[uSwapChain].m_pD3DXRefBuffer);

            LPSURFACE pTempSourceSurface = NULL;

            Check_HRFail(CreateAndCopySurfaceFromSurface(
                m_SwapChainData[uSwapChain].m_pSourceSurface,
                pTempDevice,
                &pTempSourceSurface),"CreateAndCopySurfaceFromSurface");

            RELEASE(m_SwapChainData[uSwapChain].m_pSourceSurface);

            LPSURFACE pTempDestSurface = NULL;

            Check_HRFail(CreateAndCopySurfaceFromSurface(
                m_SwapChainData[uSwapChain].m_pDestSurface,
                pTempDevice,
                &pTempDestSurface),"CreateAndCopySurfaceFromSurface");

            RELEASE(m_SwapChainData[uSwapChain].m_pDestSurface);

            const int size = m_vecAdapterMonitors.size();
            LPSURFACE* ppTempClearBuffer = new LPSURFACE[m_vecAdapterMonitors.size()];

            for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
            {
                
                if( IsRectEmpty( &( m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer ) ) )
                    continue;
                
                Check_HRFail(CreateAndCopySurfaceFromSurface(
                    m_vecAdapterMonitors[uAdapter].m_pClearBuffer,
                    pTempDevice,
                    &ppTempClearBuffer[uAdapter]),"CreateAndCopySurfaceFromSurface");

                RELEASE(m_vecAdapterMonitors[uAdapter].m_pClearBuffer);
            }

            LPSURFACE pTempDiffSurface = NULL;

            Check_HRFail(CreateAndCopySurfaceFromSurface(
                m_SwapChainData[uSwapChain].m_pDiffSurface,
                pTempDevice,
                &pTempDiffSurface),"CreateAndCopySurfaceFromSurface");

            RELEASE(m_SwapChainData[uSwapChain].m_pDiffSurface);

            RELEASE( m_pVB );
            m_pRenderingDevice->DeletePixelShader( m_pPShader );
            m_pPShader = NULL;
            RELEASE( m_pTexture );
            RELEASE( m_vecAdapterMonitors[0].m_pSwapChain );
            RELEASE(m_vecAdapterMonitors[0].m_pFrontBuffer);

            LPSURFACE tempBB[D3DPRESENT_BACK_BUFFERS_MAX];

            for( UINT uBB = 0; uBB < D3DPRESENT_BACK_BUFFERS_MAX; uBB++ )
            {
                if(m_SwapChainData[uSwapChain].m_vecBB[uBB] != NULL)
                {
                    Check_HRFail(CreateAndCopySurfaceFromSurface(
                        m_SwapChainData[uSwapChain].m_vecBB[uBB],
                        pTempDevice,
                        &tempBB[uBB]),"CreateAndCopySurfaceFromSurface");

                    RELEASE( m_SwapChainData[uSwapChain].m_vecBB[uBB] );
                }
                else
                {
                    tempBB[uBB] = NULL; 
                }
            }

            //
            // Release and Recreate
            //
            RELEASE(m_pRenderingDevice);

            HRESULT hr = S_OK;
            hr = m_pD3D->CreateDevice(
                deviceCreationParameters.AdapterOrdinal,
                deviceCreationParameters.DeviceType,
                deviceCreationParameters.hFocusWindow,
                originalBehaviors,
                &m_vecPresentParameters[m_nIndexPresentParameters],
                &m_pRenderingDevice
                );
            
            if(FAILED(hr))
            {
                //
                // Cant recover from this failure
                //
                WriteToLog( _T("After Release CreateDevice() returned %s (0x%x)\n"),m_pD3D->HResultToString( hr ),hr);
                
                if(m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseDevice)
                {
                    RELEASE(pTempDevice);
                }

                RELEASE(pTempD3DXRefBuffer);
                RELEASE(pTempSourceSurface);
                RELEASE(pTempDestSurface);
                for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
                {
                    RELEASE(ppTempClearBuffer[uAdapter]);
                }
                for( UINT uBB = 0; uBB < D3DPRESENT_BACK_BUFFERS_MAX; uBB++ )
                {
                    RELEASE( tempBB[uBB] );
                }

                SAFEDELETEA(ppTempClearBuffer);
                RELEASE(pTempDiffSurface);
                
                DeleteObject(testWindowRegion);
                testWindowRegion = NULL;
                SAFEDELETEA(pRegionData);
                pRegionData = NULL;
               
                TestFinish(D3DTESTFINISH_SKIPREMAINDER);
                return false;
            }
            
            //
            // Copy resources back to device
            //
            m_vecAdapterMonitors[0].m_pDevice = m_pRenderingDevice;
            Check_HRFail( m_pRenderingDevice->GetSwapChain( 
                deviceCreationParameters.AdapterOrdinal,
                &m_vecAdapterMonitors[0].m_pSwapChain),"m_pRenderingDevice->GetSwapChain" );

            Check_HRFail(CreateAndCopySurfaceFromSurface(
                pTempD3DXRefBuffer,
                m_pRenderingDevice,
                &m_SwapChainData[uSwapChain].m_pD3DXRefBuffer),"CreateAndCopySurfaceFromSurface");

            RELEASE(pTempD3DXRefBuffer);

            Check_HRFail(CreateAndCopySurfaceFromSurface(
                pTempSourceSurface,
                m_pRenderingDevice,
                &m_SwapChainData[uSwapChain].m_pSourceSurface),"CreateAndCopySurfaceFromSurface");

            RELEASE(pTempSourceSurface);

            Check_HRFail(CreateAndCopySurfaceFromSurface(
                pTempDestSurface,
                m_pRenderingDevice,
                &m_SwapChainData[uSwapChain].m_pDestSurface),"CreateAndCopySurfaceFromSurface");

            RELEASE(pTempDestSurface);

            for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
            {
                if( IsRectEmpty( &( m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer ) ) )
                    continue;

                Check_HRFail(CreateAndCopySurfaceFromSurface(
                    ppTempClearBuffer[uAdapter],
                    m_pRenderingDevice,
                    &m_vecAdapterMonitors[uAdapter].m_pClearBuffer),"CreateAndCopySurfaceFromSurface");

                RELEASE(ppTempClearBuffer[uAdapter]);
            }

            SAFEDELETEA(ppTempClearBuffer);
            Check_HRFail(CreateAndCopySurfaceFromSurface(
                pTempDiffSurface,
                m_pRenderingDevice,
                &m_SwapChainData[uSwapChain].m_pDiffSurface),"CreateAndCopySurfaceFromSurface");

            RELEASE(pTempDiffSurface);

            for( UINT uBB = 0; uBB < D3DPRESENT_BACK_BUFFERS_MAX; uBB++ )
            {
                if(tempBB[uBB] != NULL)
                {
                    Check_HRFail(CreateAndCopySurfaceFromSurface(
                        tempBB[uBB],
                        m_pRenderingDevice,
                        &m_SwapChainData[uSwapChain].m_vecBB[uBB]),"CreateAndCopySurfaceFromSurface");

                    RELEASE( tempBB[uBB] );
                }
            }

            Check_HRFail(m_pRenderingDevice->CreateImageSurface(
                m_vecAdapterMonitors[0].m_modeDisplay.dwWidth,
                m_vecAdapterMonitors[0].m_modeDisplay.dwHeight,
                FMT_A8R8G8B8,
                &m_vecAdapterMonitors[0].m_pFrontBuffer), "CreateImageSurface failed");
            }


            AfterReleaseDeviceBeforeVerify();

            UINT numberOfChecks = 0;

            // get the refresh rate of the rendering device
            DISPLAYMODE displaymode = {0};
            float refreshInterval = 0;
            if( FAILED(m_pD3D->GetAdapterDisplayMode(0, &displaymode)) )
                refreshInterval = 100;
            else
                refreshInterval = 1000.0f / (float)(displaymode.dwRefreshRate);			

            do 
            {  
                TCHAR s[MAX_PATH];
                Check_HRFail(D3DX_FILTER2STRING( d3dxAllFilters[m_d3dxFilter], s, sizeof(s)),"D3DX_FILTER2STRING");

                if( m_d3dxFilter == 0 )
                {
                    // get the front buffer and compare it to the reference buffer
                    for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
                    {	
                        // flush the pipeline first
                        if( m_vecPresentParameters[m_nIndexPresentParameters + uAdapter].dwFlags & PRESENTFLAG_LOCKABLE_BACKBUFFER )
                        {
                            LPSURFACE pBBSurface = NULL;
                            Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pSwapChain->GetBackBuffer( m_pRenderingDevice->GetPresParams()->uBackBufferCount -1 , D3DBACKBUFFER_TYPE_MONO, &pBBSurface ), "GetBackBuffer" );
                            LOCKED_RECT pLRect;
                            Check_HRFail( pBBSurface->LockRect( &pLRect, NULL, LOCK_NOSYSLOCK ), "LockRect" );
                            Check_HRFail( pBBSurface->UnlockRect(), "UnlockRect" );
                            pBBSurface->Release();
                        }

						// flush dwm as well
						DwmFlush();
                        Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pSwapChain->GetFrontBufferData( m_vecAdapterMonitors[uAdapter].m_pFrontBuffer ), "GetFrontBufferData" );
                    }
                }
                else
                {
                    // copy the dest surface to D3DXRef with no color conversion and a different filter
                    Check_HRFail( m_pRenderingDevice->D3DXLoadSurfaceFromSurface(    
                        m_SwapChainData[uSwapChain].m_pD3DXRefBuffer,
                        NULL,
                        NULL,
                        m_SwapChainData[uSwapChain].m_pDestSurface,
                        NULL,
                        &( m_SwapChainData[uSwapChain].m_rectD3DXRef ),
                        d3dxAllFilters[m_d3dxFilter],
                        0 ),
                        "D3DXLoadSurfaceFromSurface" );
                }

                // compare surfaces
                Check_HRFail( CompareSurfaces( uSwapChain, pRegionData), "CompareSurfaces" );

                // Destination pixels match erase color
                if ( m_fEraseRedConformance > m_iConformance && m_fEraseGreenConformance > m_iConformance && m_fEraseBlueConformance > m_iConformance )
                {
					if( numberOfChecks > 4 )
                    {
                        WriteToLog(_T("Presented content is not on screen. Destination rect has clear color even after at least %i vsyncs ."), numberOfChecks );
						break;
                    }
					if( numberOfChecks < 3 )
						Sleep( (UINT) refreshInterval + 1 );
					else
						Sleep( 2000 ); // sleep 2 seconds

                    numberOfChecks++;
                }
                else
                {
                    WriteToLog(_T("CompareSurfaces with %s filter"),s);
                    if( m_pRenderingDevice->GetPresParams()->bWindowed )	// try a different filter for windowed mode tests
                        m_d3dxFilter++;
                    else
                        break;
                }

            } while(    m_d3dxFilter < 3 && 
                ( m_ColorBalance.red    > 10    ||
                m_ColorBalance.green > 10    ||
                m_ColorBalance.blue > 10    ||
                m_fOverallRedConformance < m_iConformance    ||
                m_fOverallGreenConformance < m_iConformance || 
                m_fOverallBlueConformance < m_iConformance    ||
                m_fLocalConformance < 75.00f
                )
                );

			if( !m_bPreserveBackground )
            {
                if(m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseDevice)
                {
                    RELEASE(pTempDevice);
                }
                
                DeleteObject(testWindowRegion);
                testWindowRegion = NULL;
                SAFEDELETEA(pRegionData);
                pRegionData = NULL;


                WriteToLog( _T("Present didn't preserve the background outside DestRect\n") );
                Dump();
                
                if( m_bSaveAll )
                {
                    SaveBuffers();
                }

                Fail();
                return true;
            }

            if( m_fOverallRedConformance < m_iConformance ||
                m_fOverallGreenConformance < m_iConformance || 
                m_fOverallBlueConformance < m_iConformance )
            {
                WriteToLog( _T("Too many overall pixels are different\n") );

                if(m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseDevice)
                {
                    RELEASE(pTempDevice);
                }
                
                DeleteObject(testWindowRegion);
                testWindowRegion = NULL;
                SAFEDELETEA(pRegionData);
                pRegionData = NULL;


                Dump();
                
                if( m_bSaveAll )
                {
                    SaveBuffers();
                }

                Fail();
                return true;
            }

            // require the local conformance to be at acceptable level
            if( m_fLocalConformance < 75.00f )
            {
                WriteToLog( _T("Local conformance is way off\n") );

                if(m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseDevice)
                {
                    RELEASE(pTempDevice);
                }

                DeleteObject(testWindowRegion);
                testWindowRegion = NULL;
                SAFEDELETEA(pRegionData);
                pRegionData = NULL;

                Dump();
                if( m_bSaveAll )
                {
                    SaveBuffers();
                }
                Fail();
                return true;
            }


            // extra checking to determine a tendency to darken or lighten the image
            if( ( TEST_PRESENT == m_TestType || TEST_COLOR_CONVERSION == m_TestType || TEST_GAMMA == m_TestType) &&
                ( m_ColorBalance.red > 5 ||
                m_ColorBalance.green > 5 ||
                m_ColorBalance.blue > 5 ) )
            {
                WriteToLog( _T( "Color balance is way off\n" ) );

                if(m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseDevice)
                {
                    RELEASE(pTempDevice);
                }
                
                DeleteObject(testWindowRegion);
                testWindowRegion = NULL;
                SAFEDELETEA(pRegionData);
                pRegionData = NULL;

                Dump();
                if( m_bSaveAll )
                {
                    SaveBuffers();
                }
                Fail();
                return true;
            }
        }

        if(m_vecDeviceReleaseBeforeVerify[m_nIndexDeviceReleaseBeforeVerify] == ReleaseDevice)
        {
            RELEASE(pTempDevice);
        }
        
        DeleteObject(testWindowRegion);
        testWindowRegion = NULL;
        SAFEDELETEA(pRegionData);
        pRegionData = NULL;

    }
    catch(...)
    {
        return false;
    }

    // hide all the dlgs
    for( int iDlgDescription = 0; iDlgDescription < m_vecWindowDescription.size(); iDlgDescription++ )
        if( m_vecIntWindows[m_nIndexDialogs].end() !=
            find( m_vecIntWindows[m_nIndexDialogs].begin(), m_vecIntWindows[m_nIndexDialogs].end(), iDlgDescription ) 
            )
            ShowWindow( m_vecWindowDescription[iDlgDescription].hDlg, SW_HIDE );

   
    if( m_pSrcDevice->GetInterface() < 9.1f)
    {
        if(D3DERR_DEVICELOST == m_pSrcDevice->TestCooperativeLevel())
            
            //
            // Flush D3DERR_DEVICELOST errors from m_pSrcDevice.
            // m_pSrcDevice is the framework's default device and which we dont use it.
            // In 9.0 it will D3DERR_DEVICELOST when m_pRenderingDevice goes fullscreen for a test
            // We dont care. Flush the error.
            //
            
            GetLastError();
	        m_pD3D->SetDeviceLost(false);
    }
    
    
    
    return true;
}


//////////////////////////////////////////////////////////////////////
// Name:    ProcessREFBuffer( UINT )
// Desc:    Do any postprocessing to the REF buffer (like gamma correct it)
//////////////////////////////////////////////////////////////////////
void CPresentII::ProcessREFBuffer( UINT uSwapChain )
{
    //
    // Only fullscreen and FlipEX needs converting for d3d9.  FlipEX subclass has its own ProcessREFBuffer
    //
    if( FMT_A16B16G16R16F == m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat 
        && m_vecPresentParameters[m_nIndexPresentParameters].bWindowed == false)
    {
        AddGammaToRef(uSwapChain);
    }
}


//////////////////////////////////////////////////////////////////////
// Name:    PrivateDataProcessing()
// Desc:    process private data case before rendering
//////////////////////////////////////////////////////////////////////
void CPresentII::PrivateDataProcessing()
{
}

//////////////////////////////////////////////////////////////////////
// Name:    AfterReleaseDeviceBeforeVerify()
// Desc:    called by test, so it can force a dwm repaint 
//////////////////////////////////////////////////////////////////////
void CPresentII::AfterReleaseDeviceBeforeVerify()
{
}

//////////////////////////////////////////////////////////////////////
// Name:    CompareSurfaces( UINT )
// Desc:    compare the front buffer with the reference d3dx surface
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::CompareSurfaces( UINT uSwapChain, LPRGNDATA pRegionData)
{
    HRESULT        hResult = D3D_OK;
    LOCKED_RECT    lrRefSurface, lrFrontBuffer, lrClearBuffer, lrDiffSurface;
    MYD3DCOLOR     colorError, colorRef, colorFront, colorDiff, colorDiffErase;
    float fNumberOfPixelsIn        = 0.0f, balanceRed = 0.0f, balanceGreen = 0.0f, balanceBlue = 0.0f;
    float fNumberOfPixelsErase		= 0.0f;
    UINT NumBackgroudPixels = 0;
    
    // compute acceptable error; we only deal with 32 bit A8R8G8B8 format, so [0-255] range
    colorError.red = (BYTE) ( 255 * (100 - m_iConformance) / 100.0f );
    colorError.green = (BYTE) ( 255 * (100 - m_iConformance) / 100.0f );
    colorError.blue = (BYTE) ( 255 * (100 - m_iConformance) / 100.0f );
    for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
    {
        if( uAdapter != uSwapChain && m_uNumberSwapChains > 1 )
            continue;

        if( IsRectEmpty( &( m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer ) ) )
            continue;

        RECT rectFrontBuffer = m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer;
        if( m_vecPresentParameters[m_nIndexPresentParameters].bWindowed )
        {

            //
            // BUG: This will not work for multihead windowed rotated cases. 
            // In rotated case Frontbuffer is in non rotated space and the monitor is rotated. 
            //
                 
            // offset frontbuffer rect by monitor top, left corner
            OffsetRect( &rectFrontBuffer, 
                -(LONG)( m_fDPIFactor * m_vecAdapterMonitors[uAdapter].rcMonitor.left ), 
                -(LONG)( m_fDPIFactor * m_vecAdapterMonitors[uAdapter].rcMonitor.top ) 
            );
        }
        else
        {
            //
            // In fullscreen we dont care about the monitor offsets Top,Left of the buffer is always (0,0)
            //
            UINT bufferWidth = (UINT)( rectFrontBuffer.right - rectFrontBuffer.left);
            UINT bufferHeight = (UINT)( rectFrontBuffer.bottom - rectFrontBuffer.top);
            rectFrontBuffer.left = 0;
            rectFrontBuffer.top = 0;
            rectFrontBuffer.right = bufferWidth;
            rectFrontBuffer.bottom = bufferHeight;
        }

        try
        {
            // lock the buffers
            Check_HRFail( m_SwapChainData[uSwapChain].m_pD3DXRefBuffer->LockRect( &lrRefSurface, NULL, LOCK_READONLY ), "LockRect(d3dxrefbuffer)" );
            Check_HRFail( m_SwapChainData[uSwapChain].m_pDiffSurface->LockRect( &lrDiffSurface, NULL, 0 ), "LockRect(diffsurface)" );
            Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pFrontBuffer->LockRect( &lrFrontBuffer, &( rectFrontBuffer ), D3DLOCK_READONLY ), "LockRect(frontbuffer)" );
            Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pClearBuffer->LockRect( &lrClearBuffer, NULL, D3DLOCK_READONLY ), "LockRect(frontbuffer)" );
            
            // DEVICECLIP requires pixels to be left unchanged on secondary adapters
            bool bDeviceClip = IsDeviceClip(uSwapChain, uAdapter, m_nIndexPresentParameters);;
            bool bRestrictToMonitor = IsRestrictToMonitor(uSwapChain, uAdapter, m_nIndexPresentParameters);
           
            // check the bits
            POINT Pixel;
            POINT PixelStart, PixelEnd;
            PixelStart.x = min( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.right - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.left, 
                                m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.left - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.left );
            PixelStart.y = min( m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.bottom - m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.top, 
                                m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.top - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.top );
            PixelEnd.x = min(    m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.right, 
                                m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.right - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.left );
            PixelEnd.y = min(    m_SwapChainData[uSwapChain].m_rectFrontBufferWindow.bottom, 
                                m_vecAdapterMonitors[uAdapter].m_rectFrontBuffer.bottom - m_vecAdapterMonitors[uAdapter].m_rectFrontBufferUnclipped.top );
            
            WriteToLog("BufferRect[Adapter#%u]=(%d,%d,%d,%d). Frontbuffer=(%d,%d,%d,%d). m_bDWMEnabled=%d, bDeviceClip=%d, bRestrictToMonitor=%d ",uAdapter, PixelStart.x,PixelStart.y, PixelEnd.x, PixelEnd.y, rectFrontBuffer.left, rectFrontBuffer.top, rectFrontBuffer.right, rectFrontBuffer.bottom,m_bDWMEnabled, bDeviceClip, bRestrictToMonitor);
            for( Pixel.y = PixelStart.y; Pixel.y < PixelEnd.y; Pixel.y++ )
                for( Pixel.x = PixelStart.x; Pixel.x < PixelEnd.x; Pixel.x++ )
                {
                    MYD3DCOLOR colorErase = *(MYD3DCOLOR*)( (BYTE*)lrClearBuffer.pBits + 
                        ( Pixel.y - PixelStart.y ) * lrClearBuffer.iPitch + 
                        ( Pixel.x - PixelStart.x ) * 4 );

                    // need to divide by DPIFactor since Pixel is now in backbuffer coordinates
                    POINT PixelDPI;
                    PixelDPI.x = (LONG)( Pixel.x / m_fDPIFactor );
                    PixelDPI.y = (LONG)( Pixel.y / m_fDPIFactor );

                    bool bPixelInside = PixelInsideRegion( uSwapChain, Pixel );

                    //
                    // FlipEx dirty regions is not guarenteed.  All or part of the surface may be drawn.
                    // just verifiy that at least the dirty region is verified
                    //
                    if(!bPixelInside && 
                        m_vecPresentParameters[m_nIndexPresentParameters].SwapEffect == SWAPEFFECT_FLIPEX &&
                        m_TestType == TEST_DIRTYREGION)
                    {
                        continue;
                    }
                    
                    if( bDeviceClip || PixelInsideDialogs( PixelDPI ) )
                        if( bRestrictToMonitor )
                        {
                            if( m_bDWMEnabled && bPixelInside )
                                //
                                // Window background will be black if DWM is on.
                                //
                                colorErase = COLOR_BLACK;
                        }
                        else
                            colorErase = m_COLOR_WINDOW;

                    colorFront.color = *(DWORD*)( (UINT_PTR)lrFrontBuffer.pBits + 
                        ( Pixel.y - PixelStart.y ) * lrFrontBuffer.iPitch + 
                        ( Pixel.x - PixelStart.x ) * 4 );

                    //
                    // Test if pixel is in the m_rectDestVisible ref rect. Use the ref buffer
                    //
                    if( PtInRect( &m_SwapChainData[uSwapChain].m_rectDestVisible, Pixel )? true:false)   
                    {    
                        colorRef.color = *(DWORD*)( (UINT_PTR)lrRefSurface.pBits + 
                            ( Pixel.y - m_SwapChainData[uSwapChain].m_rectDestVisible.top ) * lrRefSurface.iPitch + 
                            ( Pixel.x - m_SwapChainData[uSwapChain].m_rectDestVisible.left ) * 4 );

                        colorDiff.red = (BYTE)abs( colorRef.red - colorFront.red );
                        colorDiff.blue    = (BYTE)abs( colorRef.blue - colorFront.blue );
                        colorDiff.green    = (BYTE)abs( colorRef.green - colorFront.green );
                        
                        if( PixelInsideDialogs( PixelDPI ) )
                        {
                            if( colorDiff.red <= colorError.red && colorDiff.green <= colorError.green && colorDiff.blue <= colorError.blue )        // the dialog is not displayed, but the background
                            {
                                // for High DPI aware apps, some stretching may be involved
                                // have to be more flexible in detecting outside pixels
                                if( m_fDPIFactor > 1.0f )
                                {
                                    bool bExPixelInside = false;
                                    for( int yOffset = -2; !bExPixelInside && yOffset <= 2; yOffset++ )
                                        for( int xOffset = -2; xOffset <= 2; xOffset++ )
                                        {
                                            POINT PixelEx;
                                            PixelEx.x = PixelDPI.x + xOffset;
                                            PixelEx.y = PixelDPI.y + yOffset;
                                            if( PixelInsideDialogs( PixelEx ) )
                                            {
                                                bExPixelInside = true;
                                                break;
                                            }
                                        }

                                        if( bExPixelInside )
                                        {
                                            continue;
                                        }
                                }

                                m_bPreserveBackground = false;

                                if( ++NumBackgroudPixels <= 10 )
                                    WriteToLog( _T( "Background color( %d, %d, %d ) not with in tolerence ( %d, %d, %d ) of ref color( %d, %d, %d ) at pixel: ( %d, %d )\n" ), colorFront.red, colorFront.green, colorFront.blue, colorError.red, colorError.green, colorError.blue, colorRef.red, colorRef.green, colorRef.blue, Pixel.x, Pixel.y );

                                // do this only if debug is enabled
                                if( m_bQuitOnFailure || m_bSaveAll )
                                    *(DWORD*)(    (UINT_PTR)lrDiffSurface.pBits + 
                                    ( Pixel.y - m_SwapChainData[uSwapChain].m_rectDestVisible.top ) * lrDiffSurface.iPitch +
                                    ( Pixel.x - m_SwapChainData[uSwapChain].m_rectDestVisible.left ) * 4 ) 
                                    = colorFront.color;
                            }
                            else
                                // do this only if debug is enabled
                                if( m_bQuitOnFailure  || m_bSaveAll)
                                    *(DWORD*)(    (UINT_PTR)lrDiffSurface.pBits +
                                    ( Pixel.y - m_SwapChainData[uSwapChain].m_rectDestVisible.top ) * lrDiffSurface.iPitch +
                                    ( Pixel.x - m_SwapChainData[uSwapChain].m_rectDestVisible.left ) * 4 )
                                    = colorRef.color;
                        }
                        else
                        {
                            fNumberOfPixelsIn++;
                            balanceRed        += (colorRef.red - colorFront.red) * 100.0f / 256.0f;
                            balanceGreen    += (colorRef.green - colorFront.green) * 100.0f / 256.0f;
                            balanceBlue        += (colorRef.blue - colorFront.blue) * 100.0f / 256.0f;

                            m_fOverallRedConformance    += colorDiff.red * 100.0f / 256.0f;
                            m_fOverallGreenConformance    += colorDiff.green * 100.0f / 256.0f;
                            m_fOverallBlueConformance    += colorDiff.blue * 100.0f / 256.0f;

                            if( colorDiff.red > colorError.red ||
                                colorDiff.blue > colorError.blue ||
                                colorDiff.green > colorError.green )
                            {
                                m_fLocalConformance += 1.0f;
                                // do this only if debug is enabled
                                if( m_bQuitOnFailure  || m_bSaveAll )
                                    *(DWORD*)(    (UINT_PTR)lrDiffSurface.pBits + 
                                    ( Pixel.y - m_SwapChainData[uSwapChain].m_rectDestVisible.top ) * lrDiffSurface.iPitch +
                                    ( Pixel.x - m_SwapChainData[uSwapChain].m_rectDestVisible.left ) * 4 )
                                    = colorFront.color;

                                colorDiffErase.red = (BYTE)abs( colorRef.red - colorFront.red );
                                colorDiffErase.blue = (BYTE)abs( colorRef.blue - colorFront.blue );
                                colorDiffErase.green = (BYTE)abs( colorRef.green - colorFront.green );

                                // test if the pixel is the same as the erase color. This might indicate that the Present has not yet made it to the screen
                                if( colorDiffErase.red < colorError.red && colorDiffErase.blue < colorError.blue && colorDiffErase.green < colorError.green )
                                {
                                    fNumberOfPixelsErase++;
                                    m_fEraseRedConformance += colorDiff.red * 100.0f / 256.0f;
                                    m_fEraseGreenConformance += colorDiff.green * 100.0f / 256.0f;
                                    m_fEraseBlueConformance += colorDiff.blue * 100.0f / 256.0f;
                                }
                            }
                            else
                                // do this only if debug is enabled
                                if( m_bQuitOnFailure  || m_bSaveAll )
                                    *(DWORD*)(    (UINT_PTR)lrDiffSurface.pBits + 
                                    ( Pixel.y - m_SwapChainData[uSwapChain].m_rectDestVisible.top ) * lrDiffSurface.iPitch +
                                    ( Pixel.x - m_SwapChainData[uSwapChain].m_rectDestVisible.left ) * 4 ) = colorErase.color;
                        }
                    }
                    else    // test that the background, outside RectDest ref surface but still in the windowrect, was preserved.
                    {

                        //
                        // If there is a window region and the pixel is outside of it, no need to check it
                        //
                        if(pRegionData && !PixelInside(Pixel, pRegionData ))
                        {
                            continue;
                        }

                        if( colorFront.red        != colorErase.red ||
                            colorFront.green    != colorErase.green ||
                            colorFront.blue        != colorErase.blue )
                            if( PixelOutsideRegion( uSwapChain, Pixel ))
                            {
                                // for High DPI aware apps, some stretching may be involved
                                // have to be more flexible in detecting outside pixels
                                if( !bDeviceClip && m_fDPIFactor > 1.0f )
                                {
                                    bool bExPixelInside = false;
                                    for( int yOffset = -2; !bExPixelInside && yOffset <= 2; yOffset++ )
                                        for( int xOffset = -2; xOffset <= 2; xOffset++ )
                                        {
                                            POINT PixelEx;
                                            PixelEx.x = Pixel.x + xOffset;
                                            PixelEx.y = Pixel.y + yOffset;
                                            if( PixelInsideRegion( uSwapChain, PixelEx ) )
                                            {
                                                bExPixelInside = true;
                                                break;
                                            }
                                        }

                                        // additional case for the clipping test:
                                        // pixel could be thought to be inside window due to rounding errors
                                        // kindly allow to pass
                                        if( PixelInsideDialogs( PixelDPI ) &&
                                            colorFront.red == colorErase.red &&
                                            colorFront.green == colorErase.green &&
                                            colorFront.blue == colorErase.blue 
                                            )
                                        {
                                            for( int yOffset = -2; !bExPixelInside && yOffset <= 2; yOffset++ )
                                                for( int xOffset = -2; xOffset <= 2; xOffset++ )
                                                {
                                                    POINT PixelEx;
                                                    PixelEx.x = PixelDPI.x + xOffset;
                                                    PixelEx.y = PixelDPI.y + yOffset;
                                                    if( !PixelInsideDialogs( PixelEx ) )
                                                    {
                                                        bExPixelInside = true;
                                                        break;
                                                    }
                                                }
                                        }

                                        if( bExPixelInside )
                                        {
                                            continue;
                                        }
                                }

                                m_bPreserveBackground = false;
                                if( ++NumBackgroudPixels <= 10 )
                                    WriteToLog( _T( "Background color( %d, %d, %d ) does not match erase color( %d, %d, %d ) at pixel: ( %d, %d )\n" ), colorFront.red, colorFront.green, colorFront.blue, colorErase.red, colorErase.green, colorErase.blue, Pixel.x, Pixel.y );

                            }
                    }
                }
        }
        catch( HRESULT hError )
        {
            hResult = hError;
            m_SwapChainData[uSwapChain].m_pD3DXRefBuffer->UnlockRect();
            m_SwapChainData[uSwapChain].m_pDiffSurface->UnlockRect();
            m_vecAdapterMonitors[uAdapter].m_pFrontBuffer->UnlockRect();
            m_vecAdapterMonitors[uAdapter].m_pClearBuffer->UnlockRect();        
            break;
        }
        catch(...)
        {
            hResult = E_FAIL;
            m_SwapChainData[uSwapChain].m_pD3DXRefBuffer->UnlockRect();
            m_SwapChainData[uSwapChain].m_pDiffSurface->UnlockRect();
            m_vecAdapterMonitors[uAdapter].m_pFrontBuffer->UnlockRect();
            m_vecAdapterMonitors[uAdapter].m_pClearBuffer->UnlockRect();
            break;
        }

        // unlock the buffers
        Check_HRFail( m_SwapChainData[uSwapChain].m_pD3DXRefBuffer->UnlockRect(), "UnlockRect" );
        Check_HRFail( m_SwapChainData[uSwapChain].m_pDiffSurface->UnlockRect(), "UnlockRect" );
        Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pFrontBuffer->UnlockRect(), "UnlockRect" );
        Check_HRFail( m_vecAdapterMonitors[uAdapter].m_pClearBuffer->UnlockRect(), "UnlockRect" );
    }

    float fLimitPixelsIn = m_fDPIFactor <= 1.0f? 100.0f : 400.0f;
    if( fNumberOfPixelsIn >= fLimitPixelsIn )
    {
        m_fLocalConformance         /= fNumberOfPixelsIn;
        m_fLocalConformance         = 100.0f * ( 1.0f - m_fLocalConformance );
        m_fOverallRedConformance    /= fNumberOfPixelsIn;
        m_fOverallRedConformance    = 100.0f - m_fOverallRedConformance;
        m_fOverallGreenConformance  /= fNumberOfPixelsIn;
        m_fOverallGreenConformance  = 100.0f - m_fOverallGreenConformance;
        m_fOverallBlueConformance   /= fNumberOfPixelsIn;
        m_fOverallBlueConformance   = 100.0f - m_fOverallBlueConformance;
        m_ColorBalance.red          = (BYTE)fabs(balanceRed / fNumberOfPixelsIn );
        m_ColorBalance.green        = (BYTE)fabs(balanceGreen / fNumberOfPixelsIn );
        m_ColorBalance.blue         = (BYTE)fabs(balanceBlue / fNumberOfPixelsIn );

    }
    else
        m_fLocalConformance    = m_fOverallRedConformance = m_fOverallGreenConformance    = m_fOverallBlueConformance = 100.0f;

    m_fEraseRedConformance /= fNumberOfPixelsIn;
    m_fEraseGreenConformance /= fNumberOfPixelsIn;
    m_fEraseBlueConformance /= fNumberOfPixelsIn;

    if( NumBackgroudPixels > 0 )
    {
        WriteToLog( _T( "Number Background pixels not preserved : %d\n" ), NumBackgroudPixels );
    }

    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name:    PixelInside( UINT, POINT, RegionData )
// Desc:    true if the pixel is inside region (or present dest if region is null)
//////////////////////////////////////////////////////////////////////
bool CPresentII::PixelInside(POINT Pixel, LPRGNDATA pRegionData )
{
    for( int i = 0; i < pRegionData->rdh.nCount; i++ )
        if( PtInRect( (RECT*)(pRegionData->Buffer + i * sizeof RECT), Pixel ) )
            return true;
    return false;
}

//////////////////////////////////////////////////////////////////////
// Name:    PixelInsideRegion( UINT, POINT )
// Desc:    true if the pixel is inside region (or present dest if region is null)
//////////////////////////////////////////////////////////////////////
bool CPresentII::PixelInsideRegion( UINT uSwapChain, POINT Pixel )
{
    bool bInside = PtInRect( &m_SwapChainData[uSwapChain].m_rectDestVisible, Pixel )? true:false;
    if( !bInside || NULL == m_pDirtyRegion )
        return bInside;
    else
    {
        for( int i = 0; i < m_pDestDirtyRegion->rdh.nCount; i++ )
            if( PtInRect( (RECT*)(m_pDestDirtyRegion->Buffer + i * sizeof RECT), Pixel ) )
                return true;
        return false;
    }
}


//////////////////////////////////////////////////////////////////////
// Name:    PixelOutsideRegion( UINT, POINT )
// Desc:    true if the pixel is outside region (or present dest if region is null)
//////////////////////////////////////////////////////////////////////
bool CPresentII::PixelOutsideRegion( UINT uSwapChain, POINT Pixel )
{
    bool bInside = PixelInsideRegion( uSwapChain, Pixel );
    if( !bInside )
        if( NULL != m_pDirtyRegion)
        {
            for( int i = 0; i < m_pDestDirtyRegion->rdh.nCount; i++ )
            {
                RECT rect = *(RECT*)(m_pDestDirtyRegion->Buffer + i * sizeof RECT);
                if( abs( rect.left - Pixel.x ) <= 1 || abs( rect.right - Pixel.x ) <= 1 ||
                    abs( rect.top - Pixel.y ) <= 1 || abs( rect.bottom - Pixel.y ) <= 1 )
                {
                    bInside = true;
                    break;
                }
            }
        }
    return !bInside;
}


//////////////////////////////////////////////////////////////////////
// Name:    PixelInsideDialogs( POINT )
// Desc:    true if the pixel is inside Dialogs
//////////////////////////////////////////////////////////////////////
bool CPresentII::PixelInsideDialogs( POINT Pixel )
{
    for( int iDlgDescription = 0; iDlgDescription < m_vecWindowDescription.size(); iDlgDescription++ )
        if( m_vecIntWindows[m_nIndexDialogs].end() !=
            find( m_vecIntWindows[m_nIndexDialogs].begin(), m_vecIntWindows[m_nIndexDialogs].end(), iDlgDescription ) 
          )
        {    // dlg in the test
            RECT rect = m_vecWindowDescription[iDlgDescription].rect;
            // offset by the window position
            RECT rectWnd;
            GetWindowRect( m_hWndPresent, &rectWnd );

            if(IsWindows10OrGreater())
            {
                // Windows 10 doesn't draw the border window (left transparent to simulate modern look)
                int borderThicknessX = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXDLGFRAME);
                int borderThicknessY = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYDLGFRAME);
                rect.left += borderThicknessX;
                rect.right -= borderThicknessX;
                rect.bottom -= borderThicknessY;
            }

            POINT absolutePixel = Pixel;
            absolutePixel.x        += rectWnd.left;
            absolutePixel.y        += rectWnd.top;

            if( PtInRect( &rect, absolutePixel ) )
            {
                // the window in the clipping test doesn't have a border, so just return true
                if( TEST_CLIPPINGWINDOWS == m_TestType )
                    return true;

                if(!IsWindows10OrGreater())
                {
                    // for XP: current theme might clip the dialog
                    OSVERSIONINFO osVer;
                    ZeroMemory( &osVer, sizeof OSVERSIONINFO );
                    osVer.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
                    GetVersionEx( &osVer );
                    if( ( (osVer.dwMajorVersion >= 6) ||    // Longhorn
                          (osVer.dwMajorVersion == 5 && osVer.dwMinorVersion >= 1) ) &&        // WinXP
                        ( absolutePixel.y - rect.top <= 5 && ( absolutePixel.x - rect.left <= 5 || rect.right - absolutePixel.x <= 5 ) ) )
                    {
                        // the point is in the corner; check if it's more inside or out
                        float cornerDistance = (float) ( absolutePixel.y - rect.top ) * ( absolutePixel.y - rect.top ), insideDistance = (float)( absolutePixel.y - 5 - rect.top ) * ( absolutePixel.y - 5 - rect.top );
                        if( absolutePixel.x - rect.left <= 5 )    // left corner
                        {
                            cornerDistance += ( absolutePixel.x - rect.left ) * ( absolutePixel.x - rect.left );
                            insideDistance += ( absolutePixel.x - 5 - rect.left ) * ( absolutePixel.x - 5 - rect.left );
                        }
                        else                            // right corner
                        {
                            cornerDistance += ( rect.right - absolutePixel.x ) * ( rect.right - absolutePixel.x );
                            insideDistance += ( rect.right - 5 - absolutePixel.x ) * ( rect.right - 5 - absolutePixel.x );
                        }
                        if( cornerDistance <= insideDistance )
                            return false;
                    }
                }
                return true;
            }
        }
    return false;
}


//////////////////////////////////////////////////////////////////////
// Name:    Dump()
// Desc:    dump the current config (with results)
//////////////////////////////////////////////////////////////////////
void CPresentII::Dump()
{
    WriteToLog( _T("Input Parameters:\n" ) );
    WriteToLog( _T("BackBufferFormat: %s\t(%d)\nBackBuffer Size (width x height): %d x %d\t(%d)\nSwap Effect: %d\n Present Interval: %d\nTexture: %s\t(%d)\nSource Rect (left, top, right, bottom): (%d, %d, %d, %d)\t(%d)\nDest Rect (left, top, right, bottom): (%d, %d, %d, %d)\t(%d)\n"), 
                FORMAT2STRING( m_vecPresentParameters[m_nIndexPresentParameters].BackBufferFormat.d3dfFormat ), m_nIndexPresentParameters,
                m_vecPresentParameters[m_nIndexPresentParameters].uWidth, m_vecPresentParameters[m_nIndexPresentParameters].uHeight, m_nIndexPresentParameters,
                m_vecPresentParameters[m_nIndexPresentParameters].SwapEffect, m_vecPresentParameters[m_nIndexPresentParameters].uPresentInterval,
                m_vecStringTexture[m_nIndexTexture].c_str(), m_nIndexTexture,
                m_SwapChainData[0].m_rectPresentSource.left, m_SwapChainData[0].m_rectPresentSource.top, m_SwapChainData[0].m_rectPresentSource.right, m_SwapChainData[0].m_rectPresentSource.bottom, m_nIndexRectSource,
                m_SwapChainData[0].m_rectPresentDest.left, m_SwapChainData[0].m_rectPresentDest.top, m_SwapChainData[0].m_rectPresentDest.right, m_SwapChainData[0].m_rectPresentDest.bottom, m_nIndexRectDest
              );
    WriteToLog( _T("Output Parameters:\n" ) );
    WriteToLog( _T("Desktop Format: %s\nPreserve Background: %d\nLocal Conformance: %.2f\nOverall Red Conformance: %.2f\nOverall Green Conformance: %.2f\nOverall Blue Conformance: %.2f\nRed Balance: %d\nGreen Balance: %d\nBlue Balance: %d\n"),
                FORMAT2STRING( m_fmtFrontBuffer.d3dfFormat ),
                m_bPreserveBackground,
                m_fLocalConformance,
                m_fOverallRedConformance,
                m_fOverallGreenConformance,
                m_fOverallBlueConformance,
                m_ColorBalance.red,
                m_ColorBalance.green,
                m_ColorBalance.blue
              );
    m_bTestFailed = true;
}


//////////////////////////////////////////////////////////////////////
// Name:    SWAPEFFECT2STRING
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::SWAPEFFECT2STRING( 
    DWORD swapEffect,
    __in_ecount(size)  PTSTR s,
    UINT size)
{
    HRESULT hr = S_OK;    
    switch( swapEffect )
    {
    case SWAPEFFECT_DISCARD:
        hr = StringCchCopy( s, size, _T("SWAPEFFECT_DISCARD") );
        break;
    case SWAPEFFECT_FLIP:
        hr = StringCchCopy( s, size, _T("SWAPEFFECT_FLIP") );
        break;
    case SWAPEFFECT_COPY:
        hr = StringCchCopy( s, size, _T("SWAPEFFECT_COPY") );
        break;
    case SWAPEFFECT_FLIPEX:
        hr = StringCchCopy( s, size, _T("SWAPEFFECT_FLIPEX") );
        break;
    default:
        hr = StringCchCopy( s, size, _T("Unknown") );
    }

    return hr;
}

//////////////////////////////////////////////////////////////////////
// Name:    FORMAT2STRING( D3DFORMAT )
// Desc:    convert D3Dformat 2 string
//////////////////////////////////////////////////////////////////////
char* CPresentII::FORMAT2STRING( D3DFORMAT fmt )
{
    static char sD3DFormat[32] = "";
    
    switch( fmt )
    {
    case D3DFMT_X8R8G8B8:
        StringCchCopy( sD3DFormat, 32, "D3DFMT_X8R8G8B8" );
        break;
    case D3DFMT_A8R8G8B8:
        StringCchCopy( sD3DFormat, 32, "D3DFMT_A8R8G8B8" );
        break;
    case D3DFMT_R5G6B5:
        StringCchCopy( sD3DFormat, 32, "D3DFMT_R5G6B5" );
        break;
    case D3DFMT_X1R5G5B5:
        StringCchCopy( sD3DFormat, 32, "D3DFMT_X1R5G5B5" );
        break;
    case D3DFMT_A1R5G5B5:
        StringCchCopy( sD3DFormat, 32, "D3DFMT_A1R5G5B5" );
        break;
    case D3DFMT_A2R10G10B10:
        StringCchCopy( sD3DFormat, 32, "D3DFMT_A2R10G10B10" );
        break;
    case D3DFMT_A2B10G10R10:
        StringCchCopy( sD3DFormat, 32, "D3DFMT_A2B10G10R10" );
        break;
    case FMT_A16B16G16R16F:
        StringCchCopy( sD3DFormat, 32, "FMT_A16B16G16R16F" );
        break;
    case FMT_A2B10G10R10_XR_BIAS:
        StringCchCopy( sD3DFormat, 32, "FMT_A2B10G10R10_XR_BIAS" );
        break;
    default:
        StringCchCopy( sD3DFormat, 32, "Unknown" );
    }

    return sD3DFormat;
}
    

//////////////////////////////////////////////////////////////////////
// Name:    D3DX_FILTER2STRING
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::D3DX_FILTER2STRING( 
    DWORD filter,
    __in_ecount(size)  PTSTR s,
    UINT size)
{
    HRESULT hr = S_OK;    

    switch( filter )
    {
    case D3DX_FILTER_TRIANGLE:
        hr = StringCchCopy( s, size, _T("D3DX_FILTER_TRIANGLE") );
        break;
    case D3DX_FILTER_LINEAR:
        hr = StringCchCopy( s, size, _T("D3DX_FILTER_LINEAR") );
        break;
    case D3DX_FILTER_POINT:
        hr = StringCchCopy( s, size, _T("D3DX_FILTER_POINT") );
        break;
    default:
        hr = StringCchCopy( s, size, _T("Unknown") );
    }

    return hr;
}

//////////////////////////////////////////////////////////////////////
// Name:    DeviceReset2String
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::DeviceReset2String( 
    DWORD filter,
    __in_ecount(size)  PTSTR s,
    UINT size)
{
    HRESULT hr = S_OK;    

    switch( filter )
    {
    case ReleaseDevice:
        hr = StringCchCopy( s, size, _T("ReleaseDevice") );
        break;
    case ReleaseSwapChains:
        hr = StringCchCopy( s, size, _T("ReleaseSwapChains") );
        break;
    case NoRelease:
        hr = StringCchCopy( s, size, _T("NoRelease") );
        break;
    default:
        hr = StringCchCopy( s, size, _T("Unknown") );
    }

    return hr;
}

//-----------------------------------------------------------------------------
// Name: OnCommand( WPARAM, LPARAM )
// Desc: prevent mouse cursor
//-----------------------------------------------------------------------------
LRESULT CPresentII::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
    //SetCursor( NULL );
    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: HideWindow( CWindow* )
// Desc: hide the window
//-----------------------------------------------------------------------------
void CPresentII::HideWindow( CWindow* pWindow, HWND donotHideThisWindow )
{
    if( NULL == pWindow || 0L == pWindow->m_hWindow )
        return;
    if( donotHideThisWindow != pWindow->m_hWindow )
        ShowWindow( pWindow->m_hWindow, SW_HIDE );
    for( int i = 0; i < pWindow->m_nChildren; i++ )
        HideWindow( pWindow->m_pChildren[i], donotHideThisWindow );
}


bool CPresentII::IsTaskBarAlwaysOnTop()
{
    APPBARDATA abd;
    abd.cbSize = sizeof(APPBARDATA);

    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);

    if(state & ABS_ALWAYSONTOP)
        return true;
    else
        return false;

}

HRESULT CPresentII::SetTaskBarState(bool alwaysOnTop)
{
    APPBARDATA abd;
    abd.cbSize = sizeof(APPBARDATA);

    UINT state = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    
    abd.lParam = state | ABS_ALWAYSONTOP;

    if (!alwaysOnTop)
    {
        abd.lParam = abd.lParam ^ ABS_ALWAYSONTOP;
    }

    SHAppBarMessage(ABM_SETSTATE, &abd);

    return S_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:    CanUseFlipEx CapsCheck()
// Desc:    check caps for FlipEx
//////////////////////////////////////////////////////////////////////
bool CPresentII::CanUseFlipEx()
{
    bool retVal = true;

   // this test cannot be run on REF;
    retVal = ( D3DDEVTYPE_HAL == m_pSrcDevice->GetDevType() );
    if(retVal == false)
    {
        WriteToLog("FlipEx not supported on Ref");
    }

    OSVERSIONINFO osVer;
    ZeroMemory( &osVer, sizeof OSVERSIONINFO );
    osVer.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
    GetVersionEx( &osVer );
    
    if( ((osVer.dwMajorVersion == 6) && (osVer.dwMinorVersion < 1)) 
            || (osVer.dwMajorVersion != 6 && osVer.dwMajorVersion < 7))
    {
        WriteToLog("FlipEx not supported for Os versions < 6.1  Os Version=%d.%d", osVer.dwMajorVersion, osVer.dwMinorVersion);
        retVal = false;   
    }
    
    if( m_pSrcDevice->GetInterface() < 9.1f )
    {
        WriteToLog("FlipEx not supported for Dx versions < 9.1.  Dx=%d", m_pSrcDevice->GetInterface());
        retVal = false;  
    }

    return retVal;
}

//////////////////////////////////////////////////////////////////////
// Name:    CreateAndCopySurfaceFromSurface
// Desc:    check caps for FlipEx
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::CreateAndCopySurfaceFromSurface(LPSURFACE pSourceSurface, CDevice *pDestinationDevice, LPSURFACE *ppDestinationSurface)
{
    HRESULT hr = S_OK;   
    SURFACEDESC desc;
    hr = pSourceSurface->GetDesc(&desc);
    if( FAILED(hr) )
    {
        WriteToLog( _T( "Buffer->GetDesc failed, return code %s (0x%x)\n" ),
            m_pD3D->HResultToString( hr ),
            hr );
    
    }

    if( SUCCEEDED(hr) )
    {
        hr = pDestinationDevice->CreateImageSurface(
            desc.Width,
            desc.Height,
            FMT_A8R8G8B8,
            ppDestinationSurface);
         
        if( FAILED(hr) )
        {
            WriteToLog( _T( "CreateImageSurface failed, return code %s (0x%x)\n" ),
            m_pD3D->HResultToString( hr ),
            hr );
    
        }
    }
  
    if( SUCCEEDED(hr) )
    {
        hr = (*(ppDestinationSurface))->UpdateSurfaceFromSurface( pSourceSurface );
        if( FAILED(hr) )
        {
            WriteToLog( _T( "UpdateSurfaceFromSurface failed, return code %s (0x%x)\n" ),
            m_pD3D->HResultToString( hr ),
            hr );
    
        }                    
    }
              
   return hr;
}

//////////////////////////////////////////////////////////////////////
// Name:    BuildWindowTree
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::BuildWindowTree(HWND root, HWND testWindow, Node<WindowData>* pNode)
{
    HRESULT hResult = S_OK;

    for(int i = 0; i < pNode->Children.size(); i++)
    {
        if(pNode->Children[i].Data.IsTestWindow)
        {
            pNode->Children[i].Data.Hwnd = testWindow;
            pNode->Children[i].Data.RestoreStyle = GetWindowLong(testWindow, GWL_STYLE);
            if(pNode->Children[i].Data.RestoreStyle == 0)
            {
                DWORD e = GetLastError();
                hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                WriteToLog(TEXT("GetWindowLong() failed, last error 0x%x\n"),hResult);
            }

            if(SUCCEEDED(hResult))
            {
                if(!SetWindowLong(testWindow, GWL_STYLE, pNode->Children[i].Data.Style))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                    WriteToLog(TEXT("SetWindowLong() failed, last error 0x%x\n"),hResult);
                }
            } 
            
            if(SUCCEEDED(hResult))
            {
                pNode->Children[i].Data.RestoreExStyle = GetWindowLong(testWindow, GWL_EXSTYLE);
                if(!SetWindowLong(testWindow, GWL_EXSTYLE, pNode->Children[i].Data.ExStyle))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                    WriteToLog(TEXT("SetWindowLong() failed, last error 0x%x\n"),hResult);
                }
            }

            if(SUCCEEDED(hResult))
            {
                if(!GetWindowRect( testWindow, &pNode->Children[i].Data.RestoreRect ))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                    WriteToLog(TEXT("GetWindowRect() failed, last error 0x%x\n"),hResult);
                }
            }
            
            if(SUCCEEDED(hResult) && pNode->Children[i].Data.Style & WS_CHILD)
            {
                if(SUCCEEDED(hResult))
                {
                    if(!SetParent(testWindow, root ))
                    {
                        DWORD e = GetLastError();
                        hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                        WriteToLog(TEXT("SetParent() failed, last error 0x%x\n"),hResult);
                    }
                }

                if(SUCCEEDED(hResult))
                {
                    //
                    // Move to (0,0) in parents
                    //
                    if (!SetWindowPos( testWindow, NULL, 0, 0, 0, 0, SWP_NOSIZE))
                    {
                        DWORD e = GetLastError();
                        hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                        WriteToLog(TEXT("SetWindowPos() failed, last error 0x%x\n"),hResult);
                    }
                }
            }
        }
        else
        {
            RECT rect = { 0 };
            
            if( pNode->Children[i].Data.Style & WS_CHILD )
            {
                rect.left = 0;
                rect.top = 0;
            }
            else
            {
                if(!GetWindowRect( testWindow, &rect ))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                    WriteToLog(TEXT("GetWindowRect() failed,testWindow = 0x%x last error 0x%x\n"),testWindow, hResult);
                    //DebugBreak();
                }
            }
         
            if(SUCCEEDED(hResult))
            {
                pNode->Children[i].Data.Hwnd = CreateWindowEx(
                    pNode->Children[i].Data.ExStyle,
                    pNode->Children[i].Data.CreateDevice ? _T( "Present2" ) : _T( "Clipping" ), 
                    _T("WindowTree"),
                    pNode->Children[i].Data.Style,
                    rect.left,
                    rect.top,
                    DEFAULT_WINDOW_WIDTH,
                    DEFAULT_WINDOW_HEIGHT,
                    root,
                    0L,
                    0L, 
                    0L); 

                if(pNode->Children[i].Data.Hwnd == NULL)
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                    WriteToLog(TEXT("CreateWindow() failed, last error 0x%x\n"),hResult);
                }
            }
        }

        if(SUCCEEDED(hResult) && pNode->Children[i].Data.IsLayeredWindow)
        {
            SetLayeredWindowAttributes(pNode->Children[i].Data.Hwnd, 0, 255, LWA_ALPHA);
        }

        //
        // Apply the window region
        //
        if(SUCCEEDED(hResult) && pNode->Children[i].Data.IsRegionWindow)
        {

            RECT rect = { 0 };
            if(SUCCEEDED(hResult))
            {
                if(!GetClientRect( pNode->Children[i].Data.Hwnd, &rect ))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                    WriteToLog(TEXT("GetClientRect() failed, last error 0x%x\n"),hResult);
                }
            }
           
            HRGN hrgn = NULL;
            if(SUCCEEDED(hResult))
            {
                pNode->Children[i].Data.RestoreHrgn = NULL;
                GetWindowRgn(pNode->Children[i].Data.Hwnd, pNode->Children[i].Data.RestoreHrgn);
                
                hrgn = CreateEllipticRgn(20, 20, rect.right-20, rect.bottom-20);
                if(hrgn == NULL)
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                    WriteToLog(TEXT("CreateEllipticRgn() failed, last error 0x%x\n"),hResult);
                }
            }
            
            if(SUCCEEDED(hResult))
            {
                if(!SetWindowRgn(pNode->Children[i].Data.Hwnd, hrgn, true))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                    WriteToLog(TEXT("SetWindowRgn() failed, last error 0x%x\n"),hResult);
                }
            }
        }
    
        if(SUCCEEDED(hResult) && pNode->Children[i].Data.CreateDevice)
        {
            DEVICE_CREATION_PARAMETERS deviceCreationParameters = { 0 };
            m_pRenderingDevice->GetCreationParameters( &deviceCreationParameters );
            
            //
            // GetCreationParameters returns D3DDeviceType but we need framework's DWORD DeviceType
            // in order to use in CreateDevice
            //
            DWORD deviceType = ConvertDeviceType(deviceCreationParameters.DeviceType);
            deviceCreationParameters.DeviceType = (D3DDEVTYPE)deviceType;

            DWORD behaviors = m_pRenderingDevice->GetBehaviors();

            PRESENT_PARAMETERS presentParameters;
            memcpy( &presentParameters, &m_vecPresentParameters[m_nIndexPresentParameters], sizeof( PRESENT_PARAMETERS ) );

            presentParameters.SwapEffect = pNode->Children[i].Data.SwapEffect;
            presentParameters.hDeviceWindow = pNode->Children[i].Data.Hwnd;

            hResult = m_pD3D->CreateDevice(
                deviceCreationParameters.AdapterOrdinal,
                deviceCreationParameters.DeviceType,
                NULL,
                behaviors,
                &presentParameters,
                &pNode->Children[i].Data.pDevice);

            
            //
            // Render some content to the window
            //
            if(SUCCEEDED(hResult))
            {
                pNode->Children[i].Data.pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x0000ff, 1.0f, 0L );
                pNode->Children[i].Data.pDevice->Present(NULL,NULL,NULL,NULL);
            }
        } 

        //
        //  Recurse through the tree
        //
        if(SUCCEEDED(hResult))
        {
            hResult = BuildWindowTree(pNode->Children[i].Data.Hwnd, testWindow, &pNode->Children[i]);
        }

        if(FAILED(hResult))
        {
            break;
        }

    }

    return hResult;
}


//////////////////////////////////////////////////////////////////////
// Name:    ResetWindowTreeRegions
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::ResetWindowTreeRegions(Node<WindowData>* pNode)
{

    HRESULT hResult = S_OK;
    for(int i = 0; i < pNode->Children.size(); i++)
    {
        if(pNode->Children[i].Data.IsRegionWindow)
        {

            RECT rect = { 0 };
            if(SUCCEEDED(hResult))
            {
                if(!GetClientRect( pNode->Children[i].Data.Hwnd, &rect ))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }
            
            HRGN hrgn = NULL;
            
            if(SUCCEEDED(hResult))
            {
                if(!SetWindowRgn(pNode->Children[i].Data.Hwnd, NULL, true))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                    WriteToLog(TEXT("SetWindowRgn() failed, last error 0x%x\n"),hResult);
                }

            }
             
            if(SUCCEEDED(hResult))
            {
                hrgn = CreateEllipticRgn(20, 20, rect.right-20, rect.bottom-20);
                if(hrgn == NULL)
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                    WriteToLog(TEXT("CreateEllipticRgn() failed, last error 0x%x\n"),hResult);
                }
            }

            if(SUCCEEDED(hResult))
            {
                if(!SetWindowRgn(pNode->Children[i].Data.Hwnd, hrgn, true))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                    WriteToLog(TEXT("SetWindowRgn() failed, last error 0x%x\n"),hResult);
                }

            }
        }
         if(SUCCEEDED(hResult))
        {
            hResult = ResetWindowTreeRegions(&pNode->Children[i]);
        }

        if(FAILED(hResult))
        {
            break;
        }
    }
   
    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name:    MoveWindowTree
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::MoveWindowTree(
    Node<WindowData>* pNode,
    int x,
    int y)
{

    HRESULT hResult = S_OK;
    for(int i = 0; i < pNode->Children.size(); i++)
    {
        if (!SetWindowPos( pNode->Children[i].Data.Hwnd, pNode->Children[i].Data.IsTestWindow ? HWND_TOPMOST : NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE))
        {
            DWORD e = GetLastError();
            hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
            WriteToLog(TEXT("SetWindowPos() failed, last error 0x%x\n"),hResult);
        }

        if(SUCCEEDED(hResult))
        {
            hResult = MoveWindowTree(&pNode->Children[i], x, y);
        }

        if(FAILED(hResult))
        {
            break;
        }

    }

    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name:    FindTestWindowInWindowTree
//////////////////////////////////////////////////////////////////////
Node<WindowData>* CPresentII::FindTestWindowInWindowTree(
    Node<WindowData>* pNode,
    bool *pFound)
{

     Node<WindowData>* returnNode = NULL;
    for(int i = 0; i < pNode->Children.size(); i++)
    {
        if(pNode->Children[i].Data.IsTestWindow)
        {
            returnNode = &pNode->Children[i];
            *pFound = true;
        }

        if(*pFound == false)
        {
            returnNode = FindTestWindowInWindowTree(&pNode->Children[i], pFound);
        }
        else
        {
            break;
        }
    }
    return returnNode;
}

//////////////////////////////////////////////////////////////////////
// Name:    PumpMessagesWindowTree
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::PumpMessagesWindowTree(
    Node<WindowData>* pNode)
{

    HRESULT hResult = S_OK;
    for(int i = 0; i < pNode->Children.size(); i++)
    {
        MSG msg;
        while( ::PeekMessage( &msg, pNode->Children[i].Data.Hwnd, 0, 0, PM_NOREMOVE ) && msg.message != WM_QUIT )
        {
            if( ::PeekMessage( &msg, pNode->Children[i].Data.Hwnd, 0, 0, PM_REMOVE ) )
            {
                ::TranslateMessage( &msg );
                ::DispatchMessage( &msg );
            }
        }
  
        if(SUCCEEDED(hResult))
        {
            hResult = PumpMessagesWindowTree(&pNode->Children[i]);
        }

        if(FAILED(hResult))
        {
            break;
        }
    
    }

    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name:    CleanUpWindowTree
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::CleanUpWindowTree(Node<WindowData>* pNode)
{

    HRESULT hresult = S_OK;
    HRESULT tempHresult = S_OK;
    //
    // Reset the TestWindow
    //
    tempHresult = ResetTestWindow(pNode);
    m_pFramework->ProcessMessages();
    if(FAILED(tempHresult))
    {
        hresult = tempHresult;    
    }
    
    //
    // Restore the testwindow's position
    //
    tempHresult = ResetPosition(pNode);
    m_pFramework->ProcessMessages();
    if(FAILED(tempHresult))
    {
        hresult = tempHresult;    
    }
    
    //
    // Clean up devices and destroy windows 
    //
    hresult = CleanUpDevicesAndWindows(pNode);
    m_pFramework->ProcessMessages();
    if(FAILED(tempHresult))
    {
        hresult = tempHresult;    
    }
   
    return hresult;
}

//////////////////////////////////////////////////////////////////////
// Name:    ResetTestWindow
// 
// Reset the style and reparent to the desktop
//
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::ResetTestWindow(Node<WindowData>* pNode)
{  
    HRESULT hResult = S_OK;

    for(int i = 0; i < pNode->Children.size(); i++)
    {
        if(pNode->Children[i].Data.IsTestWindow)
        {
             if(SUCCEEDED(hResult))
            {
                if(!SetParent(pNode->Children[i].Data.Hwnd, NULL))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }
            
            if(SUCCEEDED(hResult))
            {
                if(!SetWindowLong(pNode->Children[i].Data.Hwnd, GWL_STYLE, pNode->Children[i].Data.RestoreStyle))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }

            if(SUCCEEDED(hResult))
            {
                if(!SetWindowLong(pNode->Children[i].Data.Hwnd, GWL_EXSTYLE, pNode->Children[i].Data.RestoreExStyle))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }
            
            if(SUCCEEDED(hResult))
            {
                if(!SetWindowRgn(pNode->Children[i].Data.Hwnd, pNode->Children[i].Data.RestoreHrgn, true))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }
        }

        if(SUCCEEDED(hResult))
        {
            hResult = ResetTestWindow(&pNode->Children[i]);
        }

        if(FAILED(hResult))
        {
            break;
        }
    }

    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name:    CleanUpDevicesAndWindows
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::CleanUpDevicesAndWindows(Node<WindowData>* pNode)
{  
    
    for(int i = 0; i < pNode->Children.size(); i++)
    {
        if(pNode->Children[i].Data.CreateDevice)
        {           
            RELEASE(pNode->Children[i].Data.pDevice);
            pNode->Children[i].Data.pDevice = NULL;
        }

        if(!pNode->Children[i].Data.IsTestWindow)
        {
            DestroyWindow(pNode->Children[i].Data.Hwnd);
            pNode->Children[i].Data.Hwnd = NULL;
        }

        CleanUpDevicesAndWindows(&pNode->Children[i]);
    }
    
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Name:    ResetPosition
//
// Try to get the test window to move back to its original postion.
// InvalidateRect, UpdateWindow and Message pumping may not be 
//  necessary
//
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::ResetPosition(Node<WindowData>* pNode)
{    
    HRESULT hResult = S_OK;

    for(int i = 0; i < pNode->Children.size(); i++)
    {
        if(pNode->Children[i].Data.IsTestWindow)
        {
            RECT rect = pNode->Children[i].Data.RestoreRect;
            if(!MoveWindow( pNode->Children[i].Data.Hwnd,
                rect.left,
                rect.top,
                rect.right - rect.left,
                rect.bottom - rect.top,
                TRUE ))
            {
                DWORD e = GetLastError();
                hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
            }
            if(SUCCEEDED(hResult))
            {
                if(!InvalidateRect( pNode->Children[i].Data.Hwnd, NULL, TRUE ))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }

            if(SUCCEEDED(hResult))
            {
                if(!UpdateWindow(  pNode->Children[i].Data.Hwnd ))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }

            if(SUCCEEDED(hResult))
            {
                Sleep( 100 );

                MSG msg;
                while( ::PeekMessage( &msg, pNode->Children[i].Data.Hwnd, 0, 0, PM_NOREMOVE ) && msg.message != WM_QUIT )
                {
                    if( ::PeekMessage( &msg, pNode->Children[i].Data.Hwnd, 0, 0, PM_REMOVE ) )
                    {
                        ::TranslateMessage( &msg );
                        ::DispatchMessage( &msg );
                    }
                }
            }
        }

        if(SUCCEEDED(hResult))
        {
            hResult = ResetPosition(&pNode->Children[i]);
        }

        if(FAILED(hResult))
        {
            break;
        }
    }

    return hResult;
}


//////////////////////////////////////////////////////////////////////
//
// Name:    Copy Clipped Region
//
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::CopyClippedRegion(
    RGNDATA* pSrcRgn,
    RECT srcRect,
    RECT subRect,
    LPSURFACE pSrcSurface,
    RECT destRect,
    LPSURFACE pDstSurface,
    DWORD d3dxFilter)
{    
    HRESULT hResult = S_OK;

    for( int i = 0; i < pSrcRgn->rdh.nCount; i++ )
    {
        RECT offsetSrcRect = {0};
        RECT offsetDestRect = {0};
        RECT srcRect1 = *(RECT*)(pSrcRgn->Buffer + i * sizeof RECT);

        //
        // Clip source rect to the apadter sub rect
        //
        if(IntersectRect( &offsetSrcRect, &srcRect1, &subRect )  )
        {
            
            offsetDestRect = offsetSrcRect;

            //
            // Offset into source buffer which is (0,) based
            //           
            OffsetRect( &offsetSrcRect, -subRect.left, -subRect.top );
            OffsetRect( &offsetDestRect, -srcRect.left, -srcRect.top );

            RECT temp = {0 };
            
            //
            // clip region rect offset by srcRect to the dest rect.
            //
            if(IntersectRect( &temp, &offsetDestRect, &destRect ))
            {
                offsetDestRect = temp;

                if( offsetSrcRect.right - offsetSrcRect.left > 0 &&
                    offsetSrcRect.bottom - offsetSrcRect.top > 0 &&
                    offsetDestRect.right - offsetDestRect.left > 0 &&
                    offsetDestRect.bottom - offsetDestRect.top > 0 )

                {
                    Check_HRFail( m_pRenderingDevice->D3DXLoadSurfaceFromSurface(    
                        pDstSurface,                                 // LPDIRECT3DSURFACE8 pDestSurface,
                        NULL,                                        // CONST PALETTEENTRY* pDestPalette,
                        &offsetDestRect,                             // CONST RECT* pDestRect,
                        pSrcSurface,                                 // LPDIRECT3DSURFACE8 pSrcSurface,
                        NULL,                                        // CONST PALETTEENTRY* pSrcPalette,
                        &offsetSrcRect,                              // CONST RECT* pSrcRect,
                        d3dxFilter,                                  // DWORD Filter,
                        0 ),                                         // D3DCOLOR ColorKey
                        "D3DXLoadSurfaceFromSurface" );
                }
            }

        }
    }

    return hResult;
}


//////////////////////////////////////////////////////////////////////
//
// Name:    FillSubRectColor
//
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::FillSubRectColor(LPSURFACE pSurface, RECT textureSize, RECT subRect, DWORD color)
{
    HRESULT hr = S_OK; 
    LOCKED_RECT lrSurface;
    RECT copyRect = { 0 };

    if(IntersectRect(&copyRect, &textureSize, &subRect))
    {
        Check_HRFail(pSurface->LockRect( &lrSurface, NULL, LOCK_READONLY ), "LockRect" );

        for(int i = copyRect.left; i < copyRect.right; i++)
        {
            for(int j = copyRect.top; j < copyRect.bottom; j++)
            {
                *(DWORD*)( (UINT_PTR)lrSurface.pBits + j * lrSurface.iPitch + ( i * 4)) = color;
            }
        }
        Check_HRFail( pSurface->UnlockRect(), "UnlockRect" );
    }
    return hr;
}


//////////////////////////////////////////////////////////////////////
//
// Name:    IsDeviceClip
//
//////////////////////////////////////////////////////////////////////
bool CPresentII::IsDeviceClip(UINT swapChain, UINT adapter, UINT indexPresentParameters)
{
    bool bDeviceClip = false;
    if( ( 0 != ( D3DPRESENTFLAG_DEVICECLIP & m_vecPresentParameters[indexPresentParameters + swapChain].dwFlags ) ||
        0 != ( D3DPRESENTFLAG_VIDEO & m_vecPresentParameters[indexPresentParameters + swapChain].dwFlags )
        ) &&
        m_pRenderingDevice->GetAdapterID() != m_vecAdapterMonitors[adapter].uAdapter )
    {
        bDeviceClip = true;
    }

    return bDeviceClip;
}


//////////////////////////////////////////////////////////////////////
//
// Name:    IsRestrictToMonitor
//
//////////////////////////////////////////////////////////////////////
bool CPresentII::IsRestrictToMonitor(UINT swapChain, UINT adapter, UINT indexPresentParameters)
{
    bool bRestrictToMonitor = false;
    if(IsDeviceClip(swapChain, adapter, indexPresentParameters))
    {
        bRestrictToMonitor = 0 != ( D3DPRESENTFLAG_VIDEO & m_vecPresentParameters[indexPresentParameters + swapChain].dwFlags );
    }

    return bRestrictToMonitor;
}


//////////////////////////////////////////////////////////////////////
//
// Name:    GetWindowRegionData
//
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::GetWindowRegionData(HWND hwnd, HRGN *pRgn, LPRGNDATA *pRegionData)
{

    HRESULT hr = S_OK;
    *pRgn = CreateRectRgn(0,0,0,0);
    int ret = GetWindowRgn(hwnd, *pRgn);
    if(ret == ERROR || ret == NULLREGION)
    {
        DeleteObject(*pRgn);
        *pRgn = NULL;
        pRegionData = NULL;
    }

    if(*pRgn != NULL)
    {

        UINT regionSize = GetRegionData(*pRgn, 0, NULL);
        *pRegionData = (RGNDATA*)new char[regionSize];
        int bytes = GetRegionData(*pRgn, regionSize, *pRegionData);
        if(!bytes)
        {
            DWORD e = GetLastError();
            hr = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
            WriteToLog( "GetRegionData failed\n" );
            WriteToLog( "GetRegionData bytes %d regionSize %d %x\n",bytes, regionSize, hr );
            SAFEDELETEA(*pRegionData);
            *pRegionData = NULL;
            DeleteObject(*pRgn);
            *pRgn = NULL;

        }
    }

    return hr;
}


//////////////////////////////////////////////////////////////////////
//
// Name:    SaveBuffers
//
//////////////////////////////////////////////////////////////////////
HRESULT CPresentII::SaveBuffers()
{
    for( UINT uSwapChain = 0; uSwapChain < m_uNumberSwapChains; uSwapChain++ )
    {
        char szSurface[MAX_PATH];
        _snprintf_s( szSurface, ARRAYSIZE(szSurface), _TRUNCATE , "%s_%d_Source_%d_fail.dds", m_szTestName, m_nTestId, uSwapChain );
        m_pRenderingDevice->D3DXSaveSurfaceToFile( szSurface, D3DXIFF_DDS, m_SwapChainData[uSwapChain].m_pSourceSurface, NULL, NULL );
        _snprintf_s( szSurface, ARRAYSIZE(szSurface), _TRUNCATE , "%s_%d_Dest_%d_fail.bmp", m_szTestName, m_nTestId, uSwapChain );
        m_pRenderingDevice->D3DXSaveSurfaceToFile( szSurface, D3DXIFF_BMP, m_SwapChainData[uSwapChain].m_pDestSurface, NULL, NULL );
        _snprintf_s( szSurface, ARRAYSIZE(szSurface), _TRUNCATE , "%s_%d_Ref_%d_fail.bmp", m_szTestName, m_nTestId, uSwapChain );
        m_pRenderingDevice->D3DXSaveSurfaceToFile( szSurface, D3DXIFF_BMP, m_SwapChainData[uSwapChain].m_pD3DXRefBuffer, NULL, NULL );
        _snprintf_s( szSurface, ARRAYSIZE(szSurface), _TRUNCATE , "%s_%d_Diff_%d_fail.bmp", m_szTestName, m_nTestId, uSwapChain );
        m_pRenderingDevice->D3DXSaveSurfaceToFile( szSurface, D3DXIFF_BMP, m_SwapChainData[uSwapChain].m_pDiffSurface, NULL, NULL );
    }

    for( UINT uAdapter = 0; uAdapter < m_vecAdapterMonitors.size(); uAdapter++ )
    {
        char szFrontBuffer[MAX_PATH];
        _snprintf_s( szFrontBuffer, ARRAYSIZE(szFrontBuffer), _TRUNCATE , "%s_%d_Front_%d_fail.bmp", m_szTestName,m_nTestId, uAdapter );
        m_vecAdapterMonitors[uAdapter].m_pDevice->D3DXSaveSurfaceToFile( szFrontBuffer, D3DXIFF_BMP, m_vecAdapterMonitors[uAdapter].m_pFrontBuffer, NULL, NULL );
        _snprintf_s( szFrontBuffer, ARRAYSIZE(szFrontBuffer), _TRUNCATE , "%s_%d_Clear_%d_fail.bmp", m_szTestName,m_nTestId, uAdapter );
        m_vecAdapterMonitors[uAdapter].m_pDevice->D3DXSaveSurfaceToFile( szFrontBuffer, D3DXIFF_BMP, m_vecAdapterMonitors[uAdapter].m_pClearBuffer, NULL, NULL );
    }

    return S_OK;
}


void CPresentII::AddGammaToRef(UINT uSwapChain)
{
    // apply gamma correction for all the pixels in the surface
	LOCKED_RECT		lrRefSurface;
	POINT			Pixel;
	MYD3DCOLOR		colorRef;
	float			Red, Green, Blue;
	
    Check_HRFail( m_SwapChainData[uSwapChain].m_pD3DXRefBuffer->LockRect( &lrRefSurface, NULL, 0 ), "LockRect" );
	for( Pixel.y = 0; Pixel.y < m_SwapChainData[uSwapChain].m_rectD3DXRef.bottom - m_SwapChainData[uSwapChain].m_rectD3DXRef.top; Pixel.y++ )
		for( Pixel.x = 0; Pixel.x < m_SwapChainData[uSwapChain].m_rectD3DXRef.right - m_SwapChainData[uSwapChain].m_rectD3DXRef.left; Pixel.x++ )
		{
			colorRef.color	= *(DWORD*)( (UINT_PTR)lrRefSurface.pBits + Pixel.y * lrRefSurface.iPitch + Pixel.x * 4 );
			
			// original color clamped to [0..1)
			Red		= colorRef.red / 256.0f;
			Green	= colorRef.green / 256.0f;
			Blue	= colorRef.blue / 256.0f;

			// apply gamma correction
			Red		= pow( Red, 1 / m_fGamma );
			Green	= pow( Green, 1 / m_fGamma );
			Blue	= pow( Blue, 1 / m_fGamma );

			// corrected color in [0..255) range
			colorRef.red	= (BYTE)(256.0f * Red);
			colorRef.green	= (BYTE)(256.0f * Green);
			colorRef.blue	= (BYTE)(256.0f * Blue);
			*(DWORD*)( (UINT_PTR)lrRefSurface.pBits + Pixel.y * lrRefSurface.iPitch + Pixel.x * 4 ) = colorRef.color;
		}

	Check_HRFail( m_SwapChainData[uSwapChain].m_pD3DXRefBuffer->UnlockRect(), "UnlockRect" );
}

bool CPresentII::IsDeviceLost()
{
	
    //
    // Ignore D3DERR_DEVICELOST errors from m_pSrcDevice.
    // m_pSrcDevice is the framework's default device and which we dont use it.
    // In 9.0 it will D3DERR_DEVICELOST when m_pRenderingDevice goes fullscreen for a test
    //
    
    bool bRes = false;

    bRes = CD3DTest::IsDeviceLost();

    if(bRes == true && m_pSrcDevice->GetInterface() < 9.1f)
    {
        bRes = false;
        for(UINT i=0; i< m_uDeviceList; i++)
	    {
		    if(m_DeviceList[i] != m_pSrcDevice)
            {
                bRes |= m_DeviceList[i]->IsDeviceLost();
            }
	    }         
    }

    return bRes;
}

//////////////////////////////////////////////////////////////////////
//
//  Removes Low resolution modes from 
//  member m_vecPresentParameters
//
//////////////////////////////////////////////////////////////////////
void CPresentII::FilterLowResolutionModes()
{
    
    UINT width = 800;
    UINT height = 600;
  
    ASSERT( m_vecPresentParameters.size() == m_vecPresentParametersRotation.size() );
       
    for(int i = 0; i < m_vecPresentParameters.size(); i++)
    {
        UINT swapableWidth = width;
        UINT swapableHeight = height;
        
        if(m_vecPresentParametersRotation[i] == 1 || m_vecPresentParametersRotation[i] == 3)
        {
            std::swap( swapableWidth, swapableHeight );
        }

        if( m_vecPresentParameters[ i ].uWidth < swapableWidth ||
			        m_vecPresentParameters[ i ].uHeight < swapableHeight)
        {        
            m_vecPresentParametersRotation.erase
	        (
		        m_vecPresentParametersRotation.begin() + i
	        );
            
            m_vecPresentParameters.erase
	        (
		        m_vecPresentParameters.begin() + i--
	        );
        }
    }
}

//////////////////////////////////////////////////////////////////////
//
//  Removes all modes except the highest and lowest width/height modes 
//  from member m_vecPresentParameters
//
//////////////////////////////////////////////////////////////////////
void CPresentII::GetHighLowResolutionModes()
{
    vector<PRESENT_PARAMETERS> highModes;
    vector<PRESENT_PARAMETERS> lowModes;

    PRESENT_PARAMETERS temp;
    ZeroMemory( &temp, sizeof (PRESENT_PARAMETERS) );
    
    //
    // One for each potential rotation
    //
    for(int i = 0; i < 4; i++)
    {
        highModes.push_back(temp);
        lowModes.push_back(temp);
    }

    for(int i = 0; i < m_vecPresentParameters.size(); i++)
    {
         if(highModes[m_vecPresentParametersRotation[ i ]].uWidth == 0  && highModes[m_vecPresentParametersRotation[ i ]].uHeight == 0)
         {
            highModes[m_vecPresentParametersRotation[ i ]] = m_vecPresentParameters[ i ];
         }

         if(lowModes[m_vecPresentParametersRotation[ i ]].uWidth == 0  && lowModes[m_vecPresentParametersRotation[ i ]].uHeight == 0)
         {
            lowModes[m_vecPresentParametersRotation[ i ]] = m_vecPresentParameters[ i ];
         }
        
        if( highModes[ m_vecPresentParametersRotation[ i ] ].uWidth < m_vecPresentParameters[ i ].uWidth || 
            highModes[m_vecPresentParametersRotation[ i ]].uHeight < m_vecPresentParameters[ i ].uHeight)
        {
            highModes[m_vecPresentParametersRotation[ i ]].uWidth = m_vecPresentParameters[ i ].uWidth;
            highModes[m_vecPresentParametersRotation[ i ]].uHeight = m_vecPresentParameters[ i ].uHeight;                   
        }

        if( lowModes[m_vecPresentParametersRotation[ i ]].uWidth > m_vecPresentParameters[ i ].uWidth || 
            lowModes[m_vecPresentParametersRotation[ i ]].uHeight > m_vecPresentParameters[ i ].uHeight)
        {
            lowModes[m_vecPresentParametersRotation[ i ]].uWidth = m_vecPresentParameters[ i ].uWidth;
            lowModes[m_vecPresentParametersRotation[ i ]].uHeight = m_vecPresentParameters[ i ].uHeight;
        }
    }

    //
    // Remove all but the formats for the high and low resolutions
    //
    if(m_vecPresentParameters.size() > 0)
    {
        for(UINT i = 0; i < m_vecPresentParameters.size(); i++)
        {
            if( (highModes[m_vecPresentParametersRotation[ i ]].uWidth != m_vecPresentParameters[ i ].uWidth || 
                highModes[m_vecPresentParametersRotation[ i ]].uHeight != m_vecPresentParameters[ i ].uHeight) &&
                (lowModes[m_vecPresentParametersRotation[ i ]].uWidth != m_vecPresentParameters[ i ].uWidth || 
                lowModes[m_vecPresentParametersRotation[ i ]].uHeight != m_vecPresentParameters[ i ].uHeight))

            {
                m_vecPresentParametersRotation.erase
                (
                    m_vecPresentParametersRotation.begin() + i
                );
                
                m_vecPresentParameters.erase
                (
                    m_vecPresentParameters.begin() + i--
                );

                
            }                        
        }
    }
}

//////////////////////////////////////////////////////////////////////
//
// GetDebugPause - 
//
//////////////////////////////////////////////////////////////////////
DWORD CPresentII::GetDebugPause()
{
    TCHAR tcsBuffer[512] = _T( "" );
    DWORD pause = 0; 
    if(ReadString(_T("DebugPause"), tcsBuffer, ARRAYSIZE(tcsBuffer)))
	{
        pause = _tcstoul(tcsBuffer,'\0',10);
    }

    return pause;
}

//////////////////////////////////////////////////////////////////////
//
// ConvertDeviceType - Helper function to convert D3DDEVTYPE 
// DeviceType  into a our framework DWORD DeviceType
//
//////////////////////////////////////////////////////////////////////
DWORD ConvertDeviceType(D3DDEVTYPE DeviceType)
{
	DWORD retVal = 0;
    if (DeviceType == D3DDEVTYPE_HAL)
    {
        retVal = DEVICETYPE_HAL;
    }
    else if((DeviceType == D3DDEVTYPE_REF)||(DeviceType == D3DDEVTYPE_NULLREF))
    {
        retVal = DEVICETYPE_REF;
    }
    else if(DeviceType == D3DDEVTYPE_SW)
    {
        retVal = DEVICETYPE_SW;
    }
    else
    {
        retVal = 0; // Invalid device type
    }
    
    return retVal;
}
