#include "DXGIPresent.h"
#include "DXGIValueSets.h"
#include "TestApp.h"
#include <StrSafe.h>
#include <wincodec.h>
#include "FlipRotate.h"

#pragma warning(disable: 4355)


RELATIVEGETDSDDIMS RelativeGetDSDDims[] =
{
    {1.0f, 1.0f},
    {2.0f, 0.5f}
};

const UINT cuiNoOfDSDDims = ARRAYSIZE( RelativeGetDSDDims );

const DXGI_FORMAT CDXGIPresentSingleSC::dxgifmtBBInitTexture = DXGI_FORMAT_R8G8B8A8_UNORM; // DXGI_FORMAT_R32G32B32A32_FLOAT DXGI_FORMAT_R16G16B16A16_FLOAT

CBufferCompare CDXGIPresentSingleSC::DXGIBufferCompare;


CDXGIPresentSingleSC::CDXGIPresentSingleSC()
    :
m_iCurrentTestCase( 0 ),
    m_fMaxNonConformanceFractionWin( 0.1f ),
    m_fMaxNonConformanceFractionFS( 0.1f ),
    PrimaryAdapterDevice((CTest *)this),
    TestedSwapChain((CTest *)this, &PrimaryAdapterDevice),
    TestedSwapChainMS((CTest *)this, &PrimaryAdapterDevice),
    m_hwndFocusWindow(NULL),
    m_hwndDeviceWindow(NULL),
    m_pSwapChain(NULL),
    m_pSwapChainMS(NULL),
    pDeviceWindowOutput(NULL),
    m_MultisampleTest( FALSE ),
    m_RotationMode( DXGI_MODE_ROTATION_IDENTITY ),
    m_bNonPrerotated( FALSE ),
    m_fGamma( 2.2f ),
    m_pGetDSDTexture(NULL),
    m_pGetDSDSurface(NULL),
    m_pGetDSDTextureMS(NULL),
    m_pGetDSDSurfaceMS(NULL),
    m_UseCurrentDesktopMode(FALSE),
    m_ParameterError(0),
    m_MaxBufferCount(1),
    m_BufferCount(1),
    m_NumberOfPresents(1),
    m_TexturesAvailable(4),
    m_TexturesToLoad(1),
    m_ValidateNoExtraBuffers(false),
    m_bUseDirtyRects(false),
    m_tp_bWindowless(false),
    m_bLargeMemoryUsage(false),
    imgFactory(nullptr)
{
    m_PresentFlag.dxgiPresentFlag = 0;
    m_PresentFlag.szPresentFlagName = _T("Zero");

    ZeroMemory( &m_ScrollInfo, sizeof (m_ScrollInfo));

    ZeroMemory( &m_FactoryPointer, sizeof( m_FactoryPointer ) );
    ZeroMemory( &m_AdapterPointer, sizeof( m_AdapterPointer ) );
    ZeroMemory( &m_DevicePointer, sizeof( m_DevicePointer ) );
    ZeroMemory( &m_DeviceName, sizeof( m_DeviceName ) );

    DXGIBufferCompare.SetFloat16ToleranceULP(2);
    DXGIBufferCompare.SetFloat16Tolerance(0.15f, 0.15f);


    C_ASSERT( D3D10_FORMAT_SUPPORT_TEXTURE2D == D3D11_FORMAT_SUPPORT_TEXTURE2D );
    C_ASSERT( D3D10_FORMAT_SUPPORT_RENDER_TARGET == D3D11_FORMAT_SUPPORT_RENDER_TARGET );
    C_ASSERT( D3D10_FORMAT_SUPPORT_CPU_LOCKABLE == D3D11_FORMAT_SUPPORT_CPU_LOCKABLE );
    C_ASSERT( D3D10_FORMAT_SUPPORT_DISPLAY == D3D11_FORMAT_SUPPORT_DISPLAY );

    m_RequiredFormatSupport =
        D3D10_FORMAT_SUPPORT_TEXTURE2D |
        D3D10_FORMAT_SUPPORT_RENDER_TARGET |
        D3D10_FORMAT_SUPPORT_CPU_LOCKABLE |
        D3D10_FORMAT_SUPPORT_DISPLAY;

    for
        (
        UINT uiTexIndex = 0;
    uiTexIndex < MAX_TEXTURES;
    ++uiTexIndex
        )
    {
        pBBInitSRV[uiTexIndex] = NULL;
        pBBInitTextureVid[uiTexIndex] = NULL;
        pBBInitTextureSys[uiTexIndex] = NULL;
    }

    // Give our DCOMP isolation object a means to log.
    m_DCOMPIsolation.SetLogger(GetFramework()->GetLogWrapper());


};


CDXGIPresentSingleSC::~CDXGIPresentSingleSC()
{

    for( std::map< HMONITOR, DEVMODEW >::const_iterator itMode = m_DesktopMode.begin(); m_DesktopMode.end() != itMode; itMode++ )
    {
        MONITORINFOEXW monitorinfo;
        ZeroMemory( (LPVOID)&monitorinfo, sizeof(MONITORINFOEXW) );
        monitorinfo.cbSize = sizeof( MONITORINFOEXW );
        GetMonitorInfoW( (HMONITOR)itMode->first, &monitorinfo );

        ChangeDisplaySettingsExW( monitorinfo.szDevice, (LPDEVMODEW)&( itMode->second ), NULL, CDS_UPDATEREGISTRY, NULL );
    }
};


HRESULT CDXGIPresentSingleSC::ActivateWindow
    (
    HWND hWindow
    )
{
    //
    // Initialize locals.
    //
    HRESULT hOverallResult = E_FAIL;

    //
    // Validate parameters.
    //

    if ( hWindow == NULL )
    {
        hOverallResult = E_INVALIDARG;
        goto Cleanup;
    };


    //
    // Start by making sure that the window is visible.
    //

    ShowWindow
        (
        hWindow,
        SW_SHOWNORMAL
        );

    UpdateWindow
        (
        hWindow
        );

    ProcessMessages();


    SetForegroundWindow
        (
        m_hwndFocusWindow
        );

    SetWindowPos
        (
        hWindow,
        HWND_TOP,
        0,
        0,
        0,
        0,
        SWP_NOSIZE | SWP_NOMOVE
        );


    //
    // Now see whether the window is already at the top of the
    // z-order.
    //

    HWND hForegroundWindow = GetForegroundWindow();

    if ( hForegroundWindow == hWindow )
    {
        hOverallResult = S_OK;
        goto Cleanup;
    };


#if 0
    //
    // N.B. [SEdmison]:
    // Here's a hacky way to bring a window to the front.
    //
    // We'll hit-test our way across the desktop until we
    // find the window, and then we'll move the mouse
    // cursor to that point and trigger a mouse event in
    // hopes that the system will see that as user activity
    // and thus let the window come to the foreground.
    //
    // Yes, this is exceeedingly hacky and ugly.  Don't
    // blame me.  Blame the guys who decided to lock down
    // SetForegroundWindow without giving tests and kind of
    // (clean) workaround.
    //

    RECT WindowRect;
    BOOL bGetWindowRectResult = GetWindowRect
        (
        hWindow,
        &WindowRect
        );

    if ( bGetWindowRectResult != TRUE )
    {
        hOverallResult = E_FAIL;
        goto Cleanup;
    };


    LONG Top = WindowRect.top;
    LONG Bottom = WindowRect.bottom;
    LONG Left = WindowRect.left;
    LONG Right = WindowRect.right;

    if ( Bottom < Top )
    {
        Top = WindowRect.bottom;
        Bottom = WindowRect.top;
    };

    if ( Right < Left )
    {
        Left = WindowRect.right;
        Right = WindowRect.left;
    };


    POINT CurrentPoint;
    for
        (
        CurrentPoint.x = Left + 20;
    CurrentPoint.x < Right;
    ++CurrentPoint.x
        )
    {
        for
            (
            CurrentPoint.y = Top + 20;
        CurrentPoint.y < Bottom;
        ++CurrentPoint.y
            )
        {
            HWND hHitTestWindow = WindowFromPoint
                (
                CurrentPoint
                );

            if ( hHitTestWindow == hWindow )
            {
                mouse_event
                    (
                    MOUSEEVENTF_MOVE
                    | MOUSEEVENTF_ABSOLUTE,
                    CurrentPoint.x,
                    CurrentPoint.y,
                    0,
                    0
                    );

                mouse_event
                    (
                    MOUSEEVENTF_WHEEL,
                    0,
                    0,
                    0,
                    0
                    );

#if 0
                mouse_event
                    (
                    MOUSEEVENTF_MIDDLEDOWN,
                    0,
                    0,
                    0,
                    0
                    );

                mouse_event
                    (
                    MOUSEEVENTF_MIDDLEUP,
                    0,
                    0,
                    0,
                    0
                    );
#endif

                hOverallResult = S_OK;
                goto Cleanup;
            };
        };
    };
#endif


    hOverallResult = S_FALSE;


Cleanup:
    return hOverallResult;
};


bool CDXGIPresentSingleSC::UpdateOutputWindowInformation(IDXGISwapChain * pSwapChain, WINDOWINFO deviceWindowInfo, DXGI_MODE_ROTATION SwapChainRotation)
{
    //
    // Initialize locals.
    //

    bool bOverallResult = false;

    //
    // Validate parameters.
    //

    if ( pSwapChain == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::UpdateOutputWindowInformation:  pSwapChain is NULL."
            );

        goto Cleanup;
    };


    //
    // Free our output.
    //

    if ( pDeviceWindowOutput != NULL )
    {
        pDeviceWindowOutput->Release();
        pDeviceWindowOutput = NULL;
    };

    //
    // Determine the output on which our swap chain resides for fullscreen verification
    //

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory( &swapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );
    pSwapChain->GetDesc( &swapChainDesc );


    if(swapChainDesc.Windowed == FALSE)
    {
        HRESULT hGetContainingOutputResult = pSwapChain->GetContainingOutput
            (
            &pDeviceWindowOutput
            );

        if ( FAILED( hGetContainingOutputResult ) )
        {
            if ( GetSystemMetrics( SM_REMOTESESSION ) == 0 )
            {
                WriteToLog
                    (
                    "CDXGIPresentSingleSC::UpdateOutputWindowInformation:  GetContainingOutput failed with error 0x%x.",
                    hGetContainingOutputResult
                    );

                goto Cleanup;
            };
        }
        else
        {
            //
            // Sanity-check our adapter pointer.
            //

            if ( pDeviceWindowOutput == NULL )
            {
                WriteToLog
                    (
                    "CDXGIPresentSingleSC::UpdateOutputWindowInformation:  GetContainingOutput succeeded but returned NULL output pointer."
                    );

                goto Cleanup;
            };
        };
    };


    //
    // Determine the User32 display device on which our
    // swap chain window resides.
    //

    for
        (
        DWORD iCurrentDevice = 0;
    iCurrentDevice < UINT_MAX;
    ++iCurrentDevice
        )
    {
        DISPLAY_DEVICEW CurrentDevice;
        ZeroMemory( &CurrentDevice, sizeof( CurrentDevice ) );
        CurrentDevice.cb = sizeof( CurrentDevice );

        BOOL bEnumerateDisplayDeviceResult = EnumDisplayDevicesW
            (
            NULL,
            iCurrentDevice,
            &CurrentDevice,
            0
            );

        if ( bEnumerateDisplayDeviceResult != TRUE )
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::UpdateOutputWindowInformation:  EnumerateDisplayDevices on device %u failed.",
                iCurrentDevice
                );

            goto Cleanup;
        };


        // Skip devices that are not part of the desktop.
        if ( ( CurrentDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0 )
        {
            continue;
        };


        //
        // Enumerate the device's current mode.
        //

        DEVMODEW CurrentDeviceMode;
        ZeroMemory( &CurrentDeviceMode, sizeof( CurrentDeviceMode ) );
        CurrentDeviceMode.dmSize = sizeof( CurrentDeviceMode );

        BOOL bGetDeviceModeResult = EnumDisplaySettingsW
            (
            CurrentDevice.DeviceName,
            ENUM_CURRENT_SETTINGS,
            &CurrentDeviceMode
            );

        if ( bGetDeviceModeResult != TRUE )
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::UpdateOutputWindowInformation:  EnumDisplaySettings on device %u failed.",
                iCurrentDevice
                );

            goto Cleanup;
        };


        // If the top left window corner intersects the device's
        // desktop coordinates...
        if
            (
            deviceWindowInfo.rcWindow.left >= (LONG) CurrentDeviceMode.dmPosition.x &&
            deviceWindowInfo.rcWindow.left < (LONG) CurrentDeviceMode.dmPosition.x + (LONG) CurrentDeviceMode.dmPelsWidth &&
            deviceWindowInfo.rcWindow.top >= (LONG) CurrentDeviceMode.dmPosition.y &&
            deviceWindowInfo.rcWindow.top < (LONG) CurrentDeviceMode.dmPosition.y + (LONG) CurrentDeviceMode.dmPelsHeight
            )
        {
            // ...then this is the device.
            StringCchCopyW
                (
                m_DeviceName,
                ARRAYSIZE( m_DeviceName ),
                CurrentDevice.DeviceName
                );

            uiDeviceOutputWidth = CurrentDeviceMode.dmPelsWidth;
            uiDeviceOutputHeight = CurrentDeviceMode.dmPelsHeight;

            rectDeviceWindow.left = deviceWindowInfo.rcClient.left - CurrentDeviceMode.dmPosition.x;
            rectDeviceWindow.top = deviceWindowInfo.rcClient.top - CurrentDeviceMode.dmPosition.y;
            rectDeviceWindow.right = deviceWindowInfo.rcClient.right - CurrentDeviceMode.dmPosition.x;
            rectDeviceWindow.bottom = deviceWindowInfo.rcClient.bottom - CurrentDeviceMode.dmPosition.y;

            break;
        };
    };

    //
    //  UpdateOutputWindowInformation sets uiDeviceOutputWidth and rectDeviceWindow based on the current display settings
    //  if m_bNonPrerotated == 0 app doesn't know the monitor is rotated. In that case info from UpdateOutputWindowInformation
    //  needs to be swapped
    //
    if(m_bNonPrerotated == 0 &&
        ( DXGI_MODE_ROTATION_ROTATE90 == m_RotationMode ||
        DXGI_MODE_ROTATION_ROTATE270 == m_RotationMode))
    {
        std::swap( uiDeviceOutputWidth, uiDeviceOutputHeight );
        std::swap( rectDeviceWindow.right, rectDeviceWindow.bottom );
        std::swap( rectDeviceWindow.left, rectDeviceWindow.top );
    }

    rectDeviceWindow = Helper::RotationAdjust(rectDeviceWindow, SwapChainRotation);

    bOverallResult = true;


Cleanup:
    return bOverallResult;
}


HRESULT CDXGIPresentSingleSC::CreateTextures
    (
    DXGI_FORMAT bbFormat,
    UINT windowWidth,
    UINT windowHeight,
    UINT displayWidth,
    UINT displayHeight,
    UINT numberOfTextures
    )
{
    //
    // Initialize locals.
    //

    bool bOverallResult = false;
    HRESULT hresult = E_FAIL;

    D3DX10_IMAGE_LOAD_INFO d3dx10_texdesc_Sys;
    ZeroMemory( &d3dx10_texdesc_Sys, sizeof( d3dx10_texdesc_Sys ) );

    D3D_TEXTURE2D_DESC d3d10_texdesc_Vid;
    ZeroMemory( &d3d10_texdesc_Vid, sizeof( d3d10_texdesc_Vid ) );

    D3D_SHADER_RESOURCE_VIEW_DESC d3d10srvdesc;
    ZeroMemory( &d3d10srvdesc, sizeof( d3d10srvdesc ) );


    //
    // Release existing textures.
    //

    ReleaseTextures();


    //
    // Validate members and state.
    //

    if ( m_DevicePointer.pDevice == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateTextures:  Direct3D 10 device is not initialized."
            );

        goto Cleanup;
    };

    //
    // Create the system memory texture and initialize it with an image.
    //

    DebugOut.Write("numberOfTextures %d MAX_TEXTURES %d\n",numberOfTextures,MAX_TEXTURES);
    assert( numberOfTextures < MAX_TEXTURES );

    WriteToLog
        (
        "CDXGIPresentSingleSC::CreateTextures:  System and video memory textures:  %d x %d.",
        windowWidth,
        windowHeight
        );

    for(UINT i = 0; i < numberOfTextures; i++ )
    {
        d3dx10_texdesc_Sys.Width = windowWidth;
        d3dx10_texdesc_Sys.Height = windowHeight;
        d3dx10_texdesc_Sys.Depth = 1;
        d3dx10_texdesc_Sys.FirstMipLevel = 0;
        d3dx10_texdesc_Sys.MipLevels = 1;
        d3dx10_texdesc_Sys.Usage = D3D10_USAGE_STAGING;
        d3dx10_texdesc_Sys.BindFlags = 0;
        d3dx10_texdesc_Sys.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
        d3dx10_texdesc_Sys.MiscFlags = 0;
        d3dx10_texdesc_Sys.Format = dxgifmtBBInitTexture;
        d3dx10_texdesc_Sys.Filter = D3DX10_DEFAULT;
        d3dx10_texdesc_Sys.MipFilter = D3DX10_DEFAULT;
        d3dx10_texdesc_Sys.pSrcInfo = NULL;


        TCHAR textureName[ MAX_PATH ];
        if( windowWidth == 1 && windowHeight == 1 )
        {
            // 1x1 case: use single colored texture
            StringCchPrintf
                (
                textureName,
                ARRAYSIZE( textureName ),
                _T( "TestTex1x1_0%u.dds" ),
                (i % m_TexturesAvailable)
                );
        }
        else
        {
            StringCchPrintf
                (
                textureName,
                ARRAYSIZE( textureName ),
                _T( "TestTex0%u.dds" ),
                (i % m_TexturesAvailable)
                );
        }


        hresult = D3DX10CreateTextureFromResource
            (
            m_DevicePointer.pDevice,
            GetModuleHandle(NULL),
            textureName,
            &d3dx10_texdesc_Sys,
            NULL,
            (ID3DResource **) &pBBInitTextureSys[i],
            NULL
            );

        if ( FAILED( hresult ) )
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::CreateTexture:  D3DX10CreateTextureFromResource on system memory texture failed with error 0x%x.",
                hresult
                );

            goto Cleanup;
        };

        if ( pBBInitTextureSys[i] == NULL )
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::CreateTextures:  D3DX10CreateTextureFromResource succeeded but returned a NULL texture pointer."
                );

            goto Cleanup;
        };


        //
        // Create the video memory texture.
        //

        d3d10_texdesc_Vid.Width = windowWidth;
        d3d10_texdesc_Vid.Height = windowHeight;
        d3d10_texdesc_Vid.MipLevels = 1;
        d3d10_texdesc_Vid.ArraySize = 1;
        d3d10_texdesc_Vid.Format = dxgifmtBBInitTexture;
        d3d10_texdesc_Vid.SampleDesc.Count = 1;
        d3d10_texdesc_Vid.SampleDesc.Quality = 0;

        C_ASSERT( D3D10_USAGE_DEFAULT == D3D11_USAGE_DEFAULT );
        d3d10_texdesc_Vid.Usage = (D3D_USAGE) D3D10_USAGE_DEFAULT;
        //d3d10_texdesc_Vid.BindFlags = D3D10_BIND_SHADER_RESOURCE;
        d3d10_texdesc_Vid.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
        d3d10_texdesc_Vid.CPUAccessFlags = 0;
        d3d10_texdesc_Vid.MiscFlags = 0;

        hresult = m_DevicePointer.pDevice->CreateTexture2D
            (
            &d3d10_texdesc_Vid,
            NULL,
            &pBBInitTextureVid[i]
        );


        if ( FAILED( hresult ) )
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::CreateTextures:  CreateTexture2D failed with error 0x%x",
                hresult
                );
            goto Cleanup;
        };

        if ( pBBInitTextureVid[i] == NULL )
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::CreateTextures:  CreateTexture2D on video memory texture succeeded but returned a NULL texture pointer."
                );

            goto Cleanup;
        };


        //
        // Use the system memory texture to update the video memory texture.
        //
        m_DevicePointer.pDevice->CopyResource
            (
            pBBInitTextureVid[i],
            pBBInitTextureSys[i]
        );

        //
        // Create a shader resource view of the video memory texture.
        //

        d3d10srvdesc.Format = dxgifmtBBInitTexture;

        C_ASSERT( D3D10_SRV_DIMENSION_TEXTURE2D == D3D11_SRV_DIMENSION_TEXTURE2D );
        d3d10srvdesc.ViewDimension = (D3D_SRV_DIMENSION) D3D10_SRV_DIMENSION_TEXTURE2D;
        d3d10srvdesc.Texture2D.MostDetailedMip = 0;
        d3d10srvdesc.Texture2D.MipLevels = 1;
        //(d3d10srvdesc.Texture2D).FirstArraySlice = 0;
        //(d3d10srvdesc.Texture2D).ArraySize = 1;

        hresult = m_DevicePointer.pDevice->CreateShaderResourceView
            (
            pBBInitTextureVid[i],
            &d3d10srvdesc,
            &pBBInitSRV[i]
        );

        if ( FAILED( hresult ) )
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::CreateTextures:  CreateShaderResourceView failed with error 0x%x.",
                hresult
                );

            goto Cleanup;
        };

        if ( pBBInitSRV[i] == NULL )
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::CreateTextures:  CreateShaderResourceView succeeded but returned a NULL shader resource view pointer."
                );

            goto Cleanup;
        };
    }

    //
    // Create a system memory texture to receive the result of GetDisplaySurfaceData.
    //

    D3D_TEXTURE2D_DESC d3d10_texdesc_Get;

    //d3d10_texdesc_Get.Width = (UINT)((float)uiDeviceOutputWidth * (RelativeGetDSDDims[iDSDDimIndex]).fWidthFactor);
    //d3d10_texdesc_Get.Height = (UINT)((float)uiDeviceOutputHeight * (RelativeGetDSDDims[iDSDDimIndex]).fHeightFactor);

    d3d10_texdesc_Get.Width = displayWidth;
    d3d10_texdesc_Get.Height = displayHeight;

    d3d10_texdesc_Get.MipLevels = 1;
    d3d10_texdesc_Get.ArraySize = 1;

    //d3d10_texdesc_Get.Format = GetDSDDestFormats[iDSDFmtIndex];

    if(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB == bbFormat || DXGI_FORMAT_B8G8R8A8_UNORM_SRGB == bbFormat)
    {
        d3d10_texdesc_Get.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    }
    else
    {
        d3d10_texdesc_Get.Format = dxgifmtBBInitTexture;
    }

    d3d10_texdesc_Get.SampleDesc.Count = 1;
    d3d10_texdesc_Get.SampleDesc.Quality = 0;

    C_ASSERT( D3D10_USAGE_STAGING == D3D11_USAGE_STAGING );
    d3d10_texdesc_Get.Usage = (D3D_USAGE) D3D10_USAGE_STAGING;
    d3d10_texdesc_Get.BindFlags = 0;

    C_ASSERT( D3D10_CPU_ACCESS_READ  == D3D11_CPU_ACCESS_READ );
    C_ASSERT( D3D10_CPU_ACCESS_WRITE == D3D11_CPU_ACCESS_WRITE );
    d3d10_texdesc_Get.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    d3d10_texdesc_Get.MiscFlags = 0;

    WriteToLog
        (
        "CDXGIPresentSingleSC::CreateTextures:  GetDSD texture:  %d x %d.",
        d3d10_texdesc_Get.Width,
        d3d10_texdesc_Get.Height
        );


    hresult = m_DevicePointer.pDevice->CreateTexture2D
        (
        &d3d10_texdesc_Get,
        NULL,
        &m_pGetDSDTexture
        );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateTextures:  CreateTexture2D on GetDSD texture failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( m_pGetDSDTexture == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateTextures:  CreateTexture2D on GetDSD texture succeeded but returned a NULL texture pointer."
            );

        goto Cleanup;
    };


    hresult = m_DevicePointer.pDevice->CreateTexture2D
        (
        &d3d10_texdesc_Get,
        NULL,
        &m_pGetDSDTextureMS
        );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateTextures:  CreateTexture2D on multisampled GetDSD texture failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( m_pGetDSDTextureMS == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateTextures:  CreateTexture2D on multisampled GetDSD texture succeeded but returned a NULL texture pointer."
            );

        goto Cleanup;
    };


    //
    // Retrieve an IDXGISurface interface from the texture.
    //
    // This will be used as the destination texture for
    // GetDisplaySurfaceData operations.
    //

    hresult = m_pGetDSDTexture->QueryInterface
        (
        IID_IDXGISurface,
        (void **) &m_pGetDSDSurface
        );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateTextures:  QueryInterface on GetDSD texture for IID_IDXGISurface failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( m_pGetDSDSurface == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateTextures:  QueryInterface on GetDSD texture for IID_IDXGISurface succeeded but returned NULL surface pointer."
            );

        goto Cleanup;
    };


    hresult = m_pGetDSDTextureMS->QueryInterface
        (
        IID_IDXGISurface,
        (void **) &m_pGetDSDSurfaceMS
        );

    if ( FAILED( hresult ) )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateTextures:  QueryInterface on multisampled GetDSD texture for IID_IDXGISurface failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( m_pGetDSDSurfaceMS == NULL )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateTextures:  QueryInterface on multisampled GetDSD texture for IID_IDXGISurface succeeded but returned NULL surface pointer."
            );

        goto Cleanup;
    };

Cleanup:
    return hresult;
};


void CDXGIPresentSingleSC::ReleaseTextures()
{
    if( m_pGetDSDSurface )
    {
        m_pGetDSDSurface -> Release();
        m_pGetDSDSurface = NULL;
    }

    if( m_pGetDSDTexture )
    {
        m_pGetDSDTexture -> Release();
        m_pGetDSDTexture = NULL;
    }

    if( m_pGetDSDSurfaceMS )
    {
        m_pGetDSDSurfaceMS -> Release();
        m_pGetDSDSurfaceMS = NULL;
    }

    if( m_pGetDSDTextureMS )
    {
        m_pGetDSDTextureMS -> Release();
        m_pGetDSDTextureMS = NULL;
    }

    /*
    if (pRefFBSurface)
    {
    pRefFBSurface -> Release();
    pRefFBSurface = NULL;
    }

    if (pRefFBTexture)
    {
    pRefFBTexture -> Release();
    pRefFBTexture = NULL;
    }

    if (pRefBBSurface)
    {
    pRefBBSurface -> Release();
    pRefBBSurface = NULL;
    }

    if (pRefBBTexture)
    {
    pRefBBTexture -> Release();
    pRefBBTexture = NULL;
    }
    */
    for
        (
        UINT uiTexIndex = 0;
    uiTexIndex < MAX_TEXTURES;
    ++uiTexIndex
        )
    {
        if (pBBInitSRV[uiTexIndex])
        {
            pBBInitSRV[uiTexIndex] -> Release();
            pBBInitSRV[uiTexIndex] = NULL;
        }

        if (pBBInitTextureVid[uiTexIndex])
        {
            pBBInitTextureVid[uiTexIndex] -> Release();
            pBBInitTextureVid[uiTexIndex] = NULL;
        }

        if (pBBInitTextureSys[uiTexIndex])
        {
            pBBInitTextureSys[uiTexIndex] -> Release();
            pBBInitTextureSys[uiTexIndex] = NULL;
        }
    }
}

bool CDXGIPresentSingleSC::InitializeLockableTexture
    (
    ID3DTexture2D * pTex
    )
{
    //
    // Initialize locals.
    //

    HRESULT hresult = E_FAIL;

    UINT uiNoOfRows = 0;
    UINT uiNoOfBytesPerRow = 0;
    bool bTexturePropertiesRetrieved = false;

    {
        D3D_TEXTURE2D_DESC texdesc;

        pTex -> GetDesc(&texdesc);

        {
            const DXGIFormatInfo *pTexFormatInfo = NULL;

            uiNoOfRows = (UINT)(texdesc.Height);

            if (!(pTexFormatInfo = CFormatInfoMap::Find((m_SCDesc.BufferDesc).Format)))
            {
                WriteToLog
                    (
                    "CDXGIPresentSingleSC::InitializeLockableTexture:  CFormatInfoMap::Find failed."
                    );
            }
            else
            {
                uiNoOfBytesPerRow = (UINT)(((DWORD)(pTexFormatInfo -> uBitsPerElement)) >> 3) *
                    (UINT)(texdesc.Width);
                bTexturePropertiesRetrieved = true;
            }
        }
    }


    if (bTexturePropertiesRetrieved)
    {
        //
        // Map the first mip of the texture.
        //

        D3D10_MAPPED_TEXTURE2D texmap;
        hresult = pTex->Map
            (
            0,
            D3D10_MAP_WRITE,
            0,
            &texmap
            );

        if ( FAILED( hresult ) )
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::InitializeLockableTexture:  Map failed with error 0x%x.",
                hresult
                );

            return false;
        };


        //
        // Initialize it with consecutive byte values.
        //

        BYTE bInitVal = 0;
        UINT uiRowsRemaining = uiNoOfRows;

        BYTE *pRowStart = (BYTE *) texmap.pData;

        do
        {
            BYTE *pInitByte = pRowStart;
            UINT uiRowBytesRemaining = uiNoOfBytesPerRow;

            do
            {
                *(pInitByte++) = bInitVal++;
            } while (--uiRowBytesRemaining);

            pRowStart += texmap.RowPitch;
        } while (--uiRowsRemaining);


        //
        // Unmap the texture.
        //

        pTex -> Unmap(0);
        return true;
    }

    return false;
};


bool CDXGIPresentSingleSC::CheckLockableTexture
    (
    ID3DTexture2D * pTex,
    UINT * pDiscrepancyCount
    )
{
    HRESULT hresult = E_FAIL;

    UINT uiNoOfRows = 0;
    UINT uiNoOfBytesPerRow = 0;
    bool bTexturePropertiesRetrieved = false;

    {
        D3D_TEXTURE2D_DESC texdesc;

        pTex -> GetDesc(&texdesc);

        {
            const DXGIFormatInfo *pTexFormatInfo = NULL;

            uiNoOfRows = (UINT)(texdesc.Height);

            if (!(pTexFormatInfo = CFormatInfoMap::Find((m_SCDesc.BufferDesc).Format)))
            {
                WriteToLog
                    (
                    "CDXGIPresentSingleSC::CheckLockableTexture:  CFormatInfoMap::Find failed."
                    );
            }
            else
            {
                uiNoOfBytesPerRow = (UINT)(((DWORD)(pTexFormatInfo -> uBitsPerElement)) >> 3) *
                    (UINT)(texdesc.Width);
                bTexturePropertiesRetrieved = true;
            }
        }
    }

    if (bTexturePropertiesRetrieved)
    {
        //
        // Map the first mip of the texture.
        //

        D3D10_MAPPED_TEXTURE2D texmap;

        hresult = pTex->Map
            (
            0,
            D3D10_MAP_READ,
            0,
            &texmap
            );

        if ( FAILED( hresult ) )
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::CheckLockableTexture:  Map failed with error 0x%x.",
                hresult
                );

            return false;
        };


        //
        // Check the texture.
        //

        UINT uiDiscrepancies = 0;
        BYTE bCheckVal = 0;
        UINT uiRowsRemaining = uiNoOfRows;

        BYTE *pRowStart = (BYTE *)(texmap.pData);

        do
        {
            BYTE *pCheckByte = pRowStart;
            UINT uiRowBytesRemaining = uiNoOfBytesPerRow;

            do
            {
                if (*(pCheckByte++) != bCheckVal++)
                    ++uiDiscrepancies;
            } while (--uiRowBytesRemaining);

            pRowStart += texmap.RowPitch;
        } while (--uiRowsRemaining);


        //
        // Unmap the texture.
        //

        pTex->Unmap(0);
        *pDiscrepancyCount = uiDiscrepancies;
        return true;
    };


    return false;
};


void CDXGIPresentSingleSC::ReleaseTestCaseObjects()
{
    DebugOut.Write("Entering ReleaseTestCaseObjects.\n");

    if ( pDeviceWindowOutput != NULL )
    {
        pDeviceWindowOutput->Release();
        pDeviceWindowOutput = NULL;
    }


    ReleaseTextures();


    if ( m_pSwapChain != NULL )
    {
        m_pSwapChain->Release();
        m_pSwapChain = NULL;
    };

    if ( m_pSwapChainMS != NULL )
    {
        m_pSwapChainMS->Release();
        m_pSwapChainMS = NULL;
    };


    TestedSwapChain.ReleaseAllObjects();
    if ( m_MultisampleTest )
    {
        TestedSwapChainMS.ReleaseAllObjects();
    }

    PrimaryAdapterDevice.ReleaseTexRenderResources();
    //PrimaryAdapterDevice.ReleaseAllObjects();

    if(m_DevicePointer.pDevice)
    {
        m_DevicePointer.pDevice->Release();
        m_DevicePointer.pDevice = NULL;
    }

    if ( m_AdapterPointer.pAdapter != NULL )
    {
        m_AdapterPointer.pAdapter->Release();
        m_AdapterPointer.pAdapter = NULL;
    }

    m_D3D9Device.Cleanup();

    DebugOut.Write("Exiting ReleaseTestCaseObjects.\n");
}


bool CDXGIPresentSingleSC::PerformPresent( IDXGISwapChain *pSwapChain, UINT syncInterval, UINT flags )
{
    DXGI_RESULT dxgihr( pSwapChain );

    /*
    // Infer the arguments for the Present call:

    RECT rectBBRelativeSource;
    RECT *pSourceRectArg;
    RECT rectBBRelativeDest;
    RECT *pDestRectArg;
    RECT *pDirtyRectsArg;
    UINT uiNoOfRectsArg;

    // Infer the Source Rect argument:

    if (!iSourceRectIndexArg)
    pSourceRectArg = NULL;
    else
    {
    rectBBRelativeSource.left = (UINT)((float)((m_SCDesc.BufferDesc).Width) * (TestedSourceRects[(iSourceRectIndexArg - 1)]).left);
    rectBBRelativeSource.top = (UINT)((float)((m_SCDesc.BufferDesc).Height) * (TestedSourceRects[(iSourceRectIndexArg - 1)]).top);
    rectBBRelativeSource.right = (UINT)((float)((m_SCDesc.BufferDesc).Width) * (TestedSourceRects[(iSourceRectIndexArg - 1)]).right);
    rectBBRelativeSource.bottom = (UINT)((float)((m_SCDesc.BufferDesc).Height) * (TestedSourceRects[(iSourceRectIndexArg - 1)]).bottom);
    pSourceRectArg = &rectBBRelativeSource;
    }

    // Infer the Dest Rect argument:

    if (!iDestRectIndexArg)
    pDestRectArg = NULL;
    else
    {
    rectBBRelativeDest.left = (UINT)((float)((m_SCDesc.BufferDesc).Width) * (TestedDestRects[(iDestRectIndexArg - 1)]).left);
    rectBBRelativeDest.top = (UINT)((float)((m_SCDesc.BufferDesc).Height) * (TestedDestRects[(iDestRectIndexArg - 1)]).top);
    rectBBRelativeDest.right = (UINT)((float)((m_SCDesc.BufferDesc).Width) * (TestedDestRects[(iDestRectIndexArg - 1)]).right);
    rectBBRelativeDest.bottom = (UINT)((float)((m_SCDesc.BufferDesc).Height) * (TestedDestRects[(iDestRectIndexArg - 1)]).bottom);
    pDestRectArg = &rectBBRelativeDest;
    }

    // Infer the dirty rects-related arguments:

    {
    const RelativeRect *pDirtyRectsRelative;

    if (!(pDirtyRectsRelative = (TestedDirtyRectSets[iDirtyRectSetIndexArg]).pRectArray))
    {
    pDirtyRectsArg = NULL;
    uiNoOfRectsArg = 0;
    }
    else
    {
    UINT uiIndex;

    uiNoOfRectsArg = (TestedDirtyRectSets[iDirtyRectSetIndexArg]).uiNoOfRects;

    // Allocate and initialize an array of back-buffer-relative rects.

    pDirtyRectsArg = new RECT[uiNoOfRectsArg];

    // Initialize the back-buffer-relative rects.

    for (uiIndex = 0; uiIndex < uiNoOfRectsArg; ++uiIndex)
    {
    (pDirtyRectsArg[uiIndex]).left = (UINT)((float)((m_SCDesc.BufferDesc).Width) * (pDirtyRectsRelative[uiIndex]).left);
    (pDirtyRectsArg[uiIndex]).top = (UINT)((float)((m_SCDesc.BufferDesc).Height) * (pDirtyRectsRelative[uiIndex]).top);
    (pDirtyRectsArg[uiIndex]).right = (UINT)((float)((m_SCDesc.BufferDesc).Width) * (pDirtyRectsRelative[uiIndex]).right);
    (pDirtyRectsArg[uiIndex]).bottom = (UINT)((float)((m_SCDesc.BufferDesc).Height) * (pDirtyRectsRelative[uiIndex]).bottom);
    }
    }
    }

    // Present the back buffer.

    DebugOut.Write("+++++ Attempting Present +++++\n");

    if (FAILED((hresult = m_pSwapChain -> Present(pSourceRectArg, pDestRectArg, pDirtyRectsArg, uiNoOfRectsArg, 0, 0))))
    WriteToLog("Present call failed.  Return code: %d.", hresult);
    else
    test_result = RESULT_PASS;

    */
    bool bPresentSucceeded = true;
    int iRetry=0;

    for( iRetry; iRetry < MAX_PRESENT_RETRY_COUNT; ++iRetry )
    {
        ActivateWindow
            (
            m_hwndDeviceWindow
            );

        dxgihr = pSwapChain->Present(syncInterval, flags);
        DebugOut.Write("+++++ Present called. hresult=0x%x +++++\n", (HRESULT)dxgihr);
        if( FAILED(dxgihr) )
        {
            WriteToLog
                (
                "Present failed with error 0x%x.",
                (HRESULT)dxgihr
                );

            bPresentSucceeded = false;
            break;
        }
        else if ( dxgihr == DXGI_STATUS_OCCLUDED)
        {
            CheckForegroundWindow( m_szFocusWindowClassName );
            // Dump DXGI Journal to get clues as to why DXGI_STATUS_OCCLUDED is returned
            if( g_Application.m_bLogDXGIJounrnal )
                g_Application.InitiateJournalDump( true );

            if( iRetry == MAX_PRESENT_RETRY_COUNT -1 )
            {
                WriteToLog
                    (
                    "Present returned DXGI_STATUS_OCCLUDED.  This is unexpected, "
                    "and it usually means that a screensaver or the secure desktop"
                    "was active. Retry count: %d"
                    , iRetry
                    );
            }
            else
            {
                Sleep( 1000 );
            }
            // Always return failed result if Present() gets DXGI_STATUS_OCCLUDED even if the retry succeeds
            bPresentSucceeded = false;
        }
        else
        {
            if( iRetry > 0 )
            {
                WriteToLog
                    (
                    "Present() called more than once before succeeding due to DXGI_STATUS_OCCLUDED being returned. "
                    "Retry count: %d"
                    , iRetry
                    );
            }
            break;
        }
    }

    return bPresentSucceeded;
}


bool CDXGIPresentSingleSC::GetDisplaySurfaceData
    (
    IDXGISurface *pGetDSDSurface,
    bool bMSSwapChain,
    bool isWindowed
    )
{
    //
    // Initialize locals.
    //

    bool bReturn = false;
    HRESULT hresult = E_FAIL;
    HANDLE hEvent = NULL;

    //
    // Hide the mouse pointer so it's not in our screen-scrape.
    //

    ShowCursor( FALSE );


    //
    // Get a pointer to the right swap chain.
    //

    IDXGISwapChain * pOutputSwapChain =
        bMSSwapChain
        ? m_pSwapChainMS
        : m_pSwapChain;


    if ( pOutputSwapChain == NULL )
    {
        WriteToLog
            (
            "Swap chain pointer is NULL."
            );

        goto Cleanup;
    };

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory( &swapChainDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );
    pOutputSwapChain->GetDesc( &swapChainDesc );

    //
    // Flush the pipeline to ensure the image is presented.
    //

    m_DevicePointer.pDevice->Flush();
    if( isWindowed )
    {
        CHandle<IDXGIDevice2> pDXGIDevice2;
        hresult = m_DevicePointer.pDevice->QueryInterface
            (
            IID_PPV_ARGS(&pDXGIDevice2)
            );

        if (FAILED(hresult))
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::GetDisplaySurfaceData:  QueryInterface for IID_IDXGIDevice2 failed with error 0x%x.",
                hresult
                );
            goto Cleanup;
        };

        hEvent = CreateEvent(NULL, TRUE, FALSE, FALSE);

        if (hEvent == NULL)
        {
            hresult = HRESULT_FROM_WIN32(GetLastError());
            WriteToLog
                (
                "CDXGIPresentSingleSC::GetDisplaySurfaceData:  CreateEvent failed with error 0x%x.",
                hresult
                );
            goto Cleanup;
        }

        hresult = pDXGIDevice2->EnqueueSetEvent(hEvent);
        if (FAILED(hresult))
        {
            WriteToLog
                (
                "CDXGIPresentSingleSC::GetDisplaySurfaceData:  EnqueueSetEvent failed with error 0x%x.",
                hresult
                );
            goto Cleanup;
        }

        if(WAIT_OBJECT_0 != WaitForSingleObject(hEvent, 5000))
        {
            hresult = E_FAIL;
            WriteToLog
                (
                "CDXGIPresentSingleSC::GetDisplaySurfaceData:  WaitForSingleObject failed."
                );
            goto Cleanup;

        }

        // flush DWM
        ::DwmFlush();
    }

    //
    // Determine how to capture the screen.
    //

    bool bNeedD3D9 = false;
    bool retVal = 0;




    if( (isWindowed == false) && (swapChainDesc.Windowed == TRUE))
    {
        WriteToLog( "CDXGIPresentSingleSC::GetDisplaySurfaceData: FullScreen swapchain is windowed.");
        if( CheckForegroundWindow( m_szFocusWindowClassName ) == S_OK )
        // Dump DXGI Journal to get clues as to why FullScreen swapchain is windowed
        if( g_Application.m_bLogDXGIJounrnal )
            g_Application.InitiateJournalDump( true );

        goto Cleanup;
    }

    if
        (
        (swapChainDesc.Windowed == TRUE ||
        pDeviceWindowOutput == NULL)
        )
    {
        bNeedD3D9 = true;
    };

    if ( bNeedD3D9 )
    {
        //
        // Dont use d3d9 validation in stress.
        //
        if(g_Application.GetTestMode() != WGFTEST_MODE_STRESS)
        {
            retVal = m_D3D9Device.GetFrontBufferData
                (
                m_DeviceName,
                pGetDSDSurface
                );
        }
        else
        {
            //
            //
            //
            retVal = 1;
        }

    }
    else
    {
        hresult = pDeviceWindowOutput->GetDisplaySurfaceData
            (
            pGetDSDSurface
            );
    };



    if ( (bNeedD3D9 && !retVal) || ( !bNeedD3D9 && FAILED( hresult )) )
    {
        WriteToLog( "CDXGIPresentSingleSC::GetDisplaySurfaceData:  Failed to capture screen contents. 0x%x", hresult );
        CheckForegroundWindow( m_szFocusWindowClassName );
        if(!bNeedD3D9)
        {
            // Dump DXGI Journal to get clues as to why DXGI GetDisplaySurfaceData failed
            if( g_Application.m_bLogDXGIJounrnal )
                g_Application.InitiateJournalDump( true );
        }
        goto Cleanup;
    };


    bReturn = true;


Cleanup:

    if (hEvent)
    {
        CloseHandle(hEvent);
        hEvent = NULL;
    }

    //
    // Restore the mouse pointer.
    //


    ShowCursor( TRUE );

    return bReturn;
};

class MappedSurface : public D3D_MAPPED_TEXTURE2D
{
    public:
        MappedSurface(ID3DTexture2D& t) : p(t), hr(t.Map(0, D3D10_MAP_READ, 0, this))
        {
        }
        ~MappedSurface()
        {
            if (SUCCEEDED(hr))
            {
                p.Unmap(0);
            }
        }
        HRESULT Map()
        {
            return hr;
        }
    private:
        ID3DTexture2D& p;
        const HRESULT hr;
};

bool CDXGIPresentSingleSC::EvaluatePresentedImage
    (
    __in ID3DTexture2D * pSrcTexture2D,
    __in ID3DTexture2D * pRefTexture2D,
    __out_opt float * pNonConformanceFraction,
    __in_opt bool bVerbose,
    __out_opt HRESULT * phr
    )
{
    return EvaluatePresentedImage(pSrcTexture2D, pRefTexture2D, pNonConformanceFraction, bVerbose, phr, Vector2D<int>(0, 0), DXGI_MODE_ROTATION_IDENTITY);
}

// Compare the "Presented" image with the texture used to initialize the back buffer.
// The return value is the fraction of texels that failed conformance.
bool CDXGIPresentSingleSC::EvaluatePresentedImage
    (
    __in ID3DTexture2D * pSrcTexture2D,
    __in ID3DTexture2D * pRefTexture2D,
    __out_opt float * pNonConformanceFraction,
    __in_opt bool bVerbose,
    __out_opt HRESULT * phr,
    const Vector2D<int>& Offset,
    DXGI_MODE_ROTATION Rotation
    )
{
    HRESULT localhr;
    HRESULT& hresult = phr ? *phr : localhr;
    hresult = S_OK;

    const auto rect = Helper::RotationAdjust(rectDeviceWindow, Rotation);
    if( rect.right - rect.left <= 0 ||
        rect.bottom - rect.top <= 0 )
    {
        // if device window area is smaller than 1x1 pixel, than skip image evaluation
        if ( pNonConformanceFraction )
            *pNonConformanceFraction = 0.0f;
        return true;
    }

    //
    // Map the texture containing the reference image.
    //
    MappedSurface RefMapped(*pRefTexture2D);
    hresult = RefMapped.Map();
    if ( FAILED( hresult ) )
    {
        WriteToLog( _T("CDXGIPresentSingleSC::EvaluatePresentedImage:  Map on reference texture failed with error 0x%x."), hresult);
        return false;
    }

    const D3D_MAPPED_TEXTURE2D& TexMapRef = RefMapped;

    //
    // Map the texture containing the test image.
    //
    MappedSurface TestMapped(*pSrcTexture2D);
    hresult = TestMapped.Map();
    if ( FAILED( hresult ) )
    {
        WriteToLog( _T("CDXGIPresentSingleSC::EvaluatePresentedImage:  Map on test texture failed with error 0x%x."), hresult);
        return false;
    };

    const D3D_MAPPED_TEXTURE2D& TexMapTest = TestMapped;

    //
    // Look up the back buffer format.
    //

    const DXGIFormatInfo * pBBInitFormatInfo = CFormatInfoMap::Find
        (
        dxgifmtBBInitTexture
        );

    if ( pBBInitFormatInfo == NULL )
    {
        WriteToLog( _T("CDXGIPresentSingleSC::EvaluatePresentedImage:  Could not find format in format info map."));
        return false;
    };


    //
    // Compute the address of the first byte of the tested
    // image data that corresponds to the client area of the
    // device window.
    //
    const unsigned BPP = pBBInitFormatInfo->uBitsPerElement >> 3;
    BYTE* pRefImageData;

    BYTE* const pRefImageStart = reinterpret_cast<BYTE*>(TexMapRef.pData);
    const auto RefAdjust = Vector2D<unsigned>( Offset.x() < 0 ? -1 * Offset.x() : 0, Offset.y() < 0 ? -1 * Offset.y() : 0 );

    BYTE* const pTestImageStart = reinterpret_cast<BYTE*>(TexMapTest.pData);
    const auto TestAdjust = Vector2D<unsigned>( Offset.x() > 0 ? Offset.x() : 0, Offset.y() > 0 ? Offset.y() : 0 );
    BYTE* pTestImageData = pTestImageStart + (TexMapTest.RowPitch * (rect.top + TestAdjust.y())) + (BPP * (rect.left + TestAdjust.x()));

    const RECT RefImageRect = {rect.left + RefAdjust.x(), rect.top + RefAdjust.y(), rect.right - TestAdjust.x(), rect.bottom - TestAdjust.y()};
    const RECT TestImageRect = {rect.left + TestAdjust.x(), rect.top + TestAdjust.y(), rect.right - RefAdjust.x(), rect.bottom - RefAdjust.y()};

    if ( m_MultisampleTest )
    {
        assert(Offset.x() == 0 && Offset.y() == 0);

        if( bVerbose )
        {
            WriteToLog( _T("Evaluating (%d, %d) to (%d, %d) in reference image (multisample case)."), rect.left, rect.top, rect.right, rect.bottom);
        }

        pRefImageData = pRefImageStart + (TexMapRef.RowPitch * rect.top) + (rect.left * BPP);
    }
    else
    {
        if( bVerbose )
        {
            WriteToLog( _T("Evaluating (0, 0) to (%d, %d) in reference image."), RefImageRect.right - RefImageRect.left, RefImageRect.bottom - RefImageRect.top);
        }
        pRefImageData = pRefImageStart + (RefAdjust.x() * BPP) + (RefAdjust.y() * TexMapRef.RowPitch);
    }

    if( bVerbose )
    {
        WriteToLog( _T("Evaluating (%d, %d) to (%d, %d) in test image."), TestImageRect.left, TestImageRect.top, TestImageRect.right, TestImageRect.bottom);
    }

    //
    // Call the framework's buffer compare function.
    //

    UINT uiFailingElements = DXGIBufferCompare.Compare
        (
        pTestImageData,
        pRefImageData,
        dxgifmtBBInitTexture,
        0,
        BPP, // UINT uElementStride (Same as element width?)
        RefImageRect.right - RefImageRect.left,
        RefImageRect.bottom - RefImageRect.top,
        TexMapTest.RowPitch,
        TexMapRef.RowPitch,
        1,
        0,
        0
        );


    //
    // Calculate the percentage of non-conformance pixels.
    //

    if ( pNonConformanceFraction )
    {
        *pNonConformanceFraction =
            (float) uiFailingElements /
            (float)
            (
            (rect.right - rect.left) *
            (rect.bottom - rect.top)
            );
    };

    return true;
}


HRESULT CDXGIPresentSingleSC::LinearToGamma
    (
    ID3DTexture2D * pResource
    )
{
    //
    // Initialize locals.
    //

    HRESULT hr = E_FAIL;


    //
    // Get a descriptor for the texture.
    //

    D3D_TEXTURE2D_DESC descInitTexture;
    pResource->GetDesc( &descInitTexture);


    //
    // Map the texture.
    //

    D3D_MAPPED_TEXTURE2D MappedTex2D;
    hr = pResource->Map
        (
        0,
        D3D10_MAP_READ_WRITE,
        0,
        &MappedTex2D
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::UpdatesRGBTexture:  Map failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };

    //
    // Apply gamma correction for all the pixels in the surface.
    //

    POINT Pixel;
    MYD3DCOLOR colorGamma;
    float Red = 0.f;
    float Green = 0.f;
    float Blue = 0.f;

    for ( Pixel.y = 0; Pixel.y < (LONG)descInitTexture.Height; Pixel.y++)
    {
        for ( Pixel.x = 0; Pixel.x < (LONG)descInitTexture.Width; Pixel.x++)
        {
            colorGamma.color  = * (DWORD*)( (UINT_PTR)MappedTex2D.pData + Pixel.y * MappedTex2D.RowPitch + Pixel.x * 4);

            // original color clamped to [0..1)
            Red	= colorGamma.red / 256.0f;
            Green = colorGamma.green / 256.0f;
            Blue = colorGamma.blue / 256.0f;

            // apply gamma correction
            Red = pow( Red, 1 / m_fGamma);
            Green = pow( Green, 1 / m_fGamma);
            Blue = pow( Blue, 1 / m_fGamma);

            // corrected color in [0..255) range
            colorGamma.red	= (BYTE)(256.0f * Red);
            colorGamma.green = (BYTE)(256.0f * Green);
            colorGamma.blue	= (BYTE)(256.0f * Blue);
            * (DWORD*)( (UINT_PTR)MappedTex2D.pData + Pixel.y * MappedTex2D.RowPitch + Pixel.x * 4) = colorGamma.color;
        }
    }


    //
    // Unmap the texture.
    //

    pResource->Unmap( 0);

    //
    // Save the texture to a file.
    //
    //
    // D3DX10SaveTextureToFile( (ID3DResource *)pResource, D3DX10_IFF_BMP, "GammaImage.bmp" );



Cleanup:
    return hr;
}

HRESULT CDXGIPresentSingleSC::MaskAlphaChannel
    (
    ID3DTexture2D * pResource
    )
{
    //
    // Initialize locals.
    //

    HRESULT hr = E_FAIL;


    //
    // Get a descriptor for the texture.
    //

    D3D_TEXTURE2D_DESC descInitTexture;
    pResource->GetDesc( &descInitTexture);


    //
    // Map the texture.
    //

    D3D_MAPPED_TEXTURE2D MappedTex2D;
    hr = pResource->Map
        (
        0,
        D3D10_MAP_READ_WRITE,
        0,
        &MappedTex2D
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::UpdatesRGBTexture:  Map failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };

    //
    // Apply gamma correction for all the pixels in the surface.
    //

    POINT Pixel;
    MYD3DCOLOR colorGamma;
    BYTE Alpha = 0x0;

    for ( Pixel.y = 0; Pixel.y < (LONG)descInitTexture.Height; Pixel.y++)
    {
        for ( Pixel.x = 0; Pixel.x < (LONG)descInitTexture.Width; Pixel.x++)
        {
            colorGamma.color  = * (DWORD*)( (UINT_PTR)MappedTex2D.pData + Pixel.y * MappedTex2D.RowPitch + Pixel.x * 4);
            colorGamma.alpha	= 0x0;
            * (DWORD*)( (UINT_PTR)MappedTex2D.pData + Pixel.y * MappedTex2D.RowPitch + Pixel.x * 4) = colorGamma.color;
        }
    }


    //
    // Unmap the texture.
    //

    pResource->Unmap( 0);

Cleanup:
    return hr;
}

HRESULT  CDXGIPresentSingleSC::DrawRect
    (
    ID3DTexture2D * pResource,
    UINT x,
    UINT y,
    UINT width,
    UINT height,
    MYD3DCOLOR color
    )
{
    //
    // Initialize locals.
    //

    HRESULT hr = E_FAIL;


    //
    // Get a descriptor for the swap chain.
    //

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    hr = m_pSwapChain->GetDesc( &swapChainDesc);
    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::DrawRect:  GetDesc on swap chain failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };


    //
    // Get a descriptor for the texture.
    //

    D3D_TEXTURE2D_DESC descInitTexture;
    pResource->GetDesc( &descInitTexture);


    //
    // Calculate coordinates for the rectangle.
    //

    POINT pixelStop;
    pixelStop.x = ((x + width) > descInitTexture.Width) ? descInitTexture.Width : (x + width);
    pixelStop.y = ((y + height) > descInitTexture.Height) ? descInitTexture.Height : (y + height);


    //
    // Map the first mip of the texture.
    //

    D3D_MAPPED_TEXTURE2D MappedTex2D;
    hr = pResource->Map
        (
        0,
        D3D10_MAP_READ_WRITE,
        0,
        &MappedTex2D
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::DrawRect:  Map failed with error 0x%x.",
            hr
            );

        goto Cleanup;
    };


    //
    // Draw the rectangle.
    //

    POINT Pixel;
    for ( Pixel.y = y; Pixel.y < pixelStop.y; Pixel.y++)
    {
        for ( Pixel.x = x; Pixel.x < pixelStop.x; Pixel.x++)
        {
            * (DWORD*)( (UINT_PTR)MappedTex2D.pData + Pixel.y * MappedTex2D.RowPitch + Pixel.x * 4) = color.color;
        }
    }


    //
    // Unmap the texture.
    //

    pResource->Unmap( 0 );


Cleanup:
    return hr;
}

//////////////////////////////////////////////////////////////////////
//
// Name:    IsRGB
//
// Description:
//	Returns true if a pixel format is SRGB, otherwise
//  false.
//
//////////////////////////////////////////////////////////////////////
bool CDXGIPresentSingleSC::IsSRGB(DXGI_FORMAT Format)
{
    switch (Format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
        return true;
    default:
        return false;
    }
}

//////////////////////////////////////////////////////////////////////
//
// Name:    CopyRegion
//
//////////////////////////////////////////////////////////////////////
HRESULT CDXGIPresentSingleSC::CopyRegion
    (
    __in ID3DTexture2D * pSource,
    __in ID3DTexture2D * pDest,
    __in LPRGNDATA pRegionData,
    __in bool bDisableSRGBConversion
    )
{
    HRESULT hr = E_FAIL;
    D3DX10_TEXTURE_LOAD_INFO tlinfo;
    D3D_TEXTURE2D_DESC desc;
    RECT textureRect;
    RECT intersection;
    D3D10_BOX srcBox;
    D3D10_BOX dstBox;

    assert(pSource != NULL);
    assert(pDest != NULL);

    ZeroMemory(&srcBox, sizeof(srcBox));
    ZeroMemory(&dstBox, sizeof(dstBox));

    ZeroMemory(&tlinfo, sizeof(tlinfo));
    tlinfo.NumMips         = D3DX10_DEFAULT;
    tlinfo.NumElements     = D3DX10_DEFAULT;
    //
    // Should we disable gamma correction?
    //
    // We need this to generate dirty rect reference images
    // compatible with images presented from SRGB back-buffers.
    //
    if (bDisableSRGBConversion)
    {
        tlinfo.Filter          = D3DX10_FILTER_LINEAR | D3DX10_FILTER_SRGB_OUT;
        tlinfo.MipFilter       = D3DX10_FILTER_LINEAR | D3DX10_FILTER_SRGB_OUT;
    }
    else // Perform a normal copy.
    {
        tlinfo.Filter          = D3DX10_FILTER_LINEAR;
        tlinfo.MipFilter       = D3DX10_FILTER_LINEAR;
    }
    tlinfo.pSrcBox = &srcBox;
    tlinfo.pDstBox = &dstBox;

    pSource->GetDesc( &desc);
    textureRect.left = 0;
    textureRect.top = 0;
    textureRect.right = desc.Width;
    textureRect.bottom = desc.Height;

    //WriteToLog("Source Texture Rect: %d %d %d %d", textureRect.left, textureRect.top, textureRect.right, textureRect.bottom);

    for ( UINT i = 0; i < pRegionData->rdh.nCount; i++)
    {
        RECT rect = * (RECT*)(pRegionData->Buffer + i * sizeof RECT);

        if (IntersectRect(&intersection, &rect, &textureRect))
        {
            //WriteToLog("%d %d %d %d", intersection.left, intersection.top, intersection.right, intersection.bottom);
            srcBox.left = dstBox.left = intersection.left;
            srcBox.top = dstBox.top = intersection.top;
            srcBox.right = dstBox.right = intersection.right;
            srcBox.bottom = dstBox.bottom = intersection.bottom;

            hr = D3DX10LoadTextureFromTexture
                (
                pSource,
                &tlinfo,
                pDest);

            if (FAILED(hr))
            {
                WriteToLog
                    (
                    _T("D3DX10LoadTextureFromTexture failed with error 0x%x."),
                    hr
                    );

                break;
            }
        } // End of IntersectRect verification
    } // End of Copy rects loop

    return hr;
}


void CDXGIPresentSingleSC::ProcessMessages()
{
    MSG msg;

    while( PeekMessage( &msg,NULL,0,0,PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );

        // If this is a quit message, re-post it.
        if( WM_QUIT == msg.message )
        {
            PostQuitMessage( (int)msg.wParam );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//
// CreateTestWindows
//
// Must set m_hwndFocusWindow
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CDXGIPresentSingleSC::CreateTestWindows(
    UINT focusWindowLeft,
    UINT focusWindowTop,
    UINT focusWindowWidth,
    UINT focusWindowHeight)
{
    HRESULT hr = S_OK;
    DWORD dwStyleEx = 0;

    if ( g_Application.IsRTL())
        dwStyleEx |= WS_EX_LAYOUTRTL;
    DWORD dwStyle = WS_POPUP | WS_VISIBLE;

    //Adjust the window size to include title bar ect
    RECT clientRect = { focusWindowLeft , focusWindowTop, focusWindowWidth, focusWindowHeight };
    ::AdjustWindowRectEx( &clientRect, dwStyle, false /* bool menu*/ , dwStyleEx );

    //
    // Create a swap chain window.
    //

    m_hwndFocusWindow = CreateWindowEx
        (
        dwStyleEx,
        m_szFocusWindowClassName,
        "Focus Window",
        dwStyle,
        clientRect.left,
        clientRect.top,
        clientRect.right,
        clientRect.bottom,
        NULL,
        NULL,
        m_hProgInst,
        NULL
        );

    if (m_hwndFocusWindow == NULL)
    {
        DWORD error = GetLastError();
        hr = (ERROR_SUCCESS == error) ? E_FAIL : HRESULT_FROM_WIN32(hr);
    }

    GetClientRect( m_hwndFocusWindow, &clientRect );
    WriteToLog(_T("CreateTestWindows: Client Rect (left:%d,top:%d,width:%d,height:%d) "),
        clientRect.left,
        clientRect.top,
        clientRect.right,
        clientRect.bottom);

    return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// GetMultisampleQualityLevels
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CDXGIPresentSingleSC::GetMultisampleQualityLevels
    (
    D3D_DEVICE_POINTER devicePointer,
    __in std::vector< ModeInformation >* pModes,
    __out std::vector< ModeInformation >* pMultisampleIndex,
    UINT maxMultisampleCount,
    UINT maxMultisampleQualityIndex
    )
{
    HRESULT hr = S_OK;
    std::vector< ModeInformation >::iterator it;

    for (it = pModes->begin();
        it != pModes->end();
        ++it)
    {
        for ( UINT uMultisampleCount = 2; uMultisampleCount <= maxMultisampleCount; ++uMultisampleCount)
        {
            UINT uMultisampleQuality = 0;
            hr = devicePointer.pDevice->CheckMultisampleQualityLevels(
                it->ModeDesc.Format,
                uMultisampleCount,
                &uMultisampleQuality);



            if ( SUCCEEDED( hr) )
            {
                UINT numMultisampleQualityIndex = min(uMultisampleQuality, maxMultisampleQualityIndex);

                for ( UINT uMultisampleQualityIndex = 0;
                    uMultisampleQualityIndex < numMultisampleQualityIndex;
                    ++uMultisampleQualityIndex)
                {
                    ModeInformation temp = *it;
                    temp.uiMultisampleQuality = uMultisampleQualityIndex;
                    temp.uiMultisampleCount = uMultisampleCount;
                    memcpy( &temp.devmode, &it->devmode, sizeof(DEVMODEW));

                    if ( pMultisampleIndex->end() == find(
                        pMultisampleIndex->begin(),
                        pMultisampleIndex->end(),
                        temp))
                    {
                        pMultisampleIndex->push_back( temp);
                    }
                }
            }
            else
            {
                if(uMultisampleQuality == 0)
                {
                    hr = S_OK;
                }
                else
                {
                    goto Cleanup;
                }
            }
        }
    }

Cleanup:

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
//
// GetSupportedFormats
//
///////////////////////////////////////////////////////////////////////////////
HRESULT CDXGIPresentSingleSC::GetSupportedFormats(
    D3D_DEVICE_POINTER devicePointer,
    __in std::vector< ModeInformation >* pModes,
    __out std::vector< ModeInformation >* pMultisampleIndex,
    UINT requiredSupport)
{
    HRESULT hr = S_OK;

    std::vector< ModeInformation >::iterator it;

    for (it = pModes->begin();
        it != pModes->end();
        ++it)
    {

        UINT FormatSupport = 0;
        hr = devicePointer.pDevice->CheckFormatSupport
            (
            it->ModeDesc.Format,
            &FormatSupport
            );

        if ( hr == E_FAIL )
        {
            /*WriteToLog
            (
            "Device does not support this format."
            );*/

            hr = S_OK;
            continue;
        }
        else if ( FAILED( hr ) )
        {
            WriteToLog
                (
                "CPresentFS::GetSupportedFormats:  CheckFormatSupport failed with error 0x%x.",
                hr
                );

            goto Cleanup;
        };

        //
        // DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM does not support Render target.
        // Will need to create a DXGI_FORMAT_R10G10B10_A2_UNORM render target.
        //
        if(it->ModeDesc.Format == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
        {
            requiredSupport = requiredSupport & ~D3D10_FORMAT_SUPPORT_RENDER_TARGET;
        }

        if ( ( FormatSupport & requiredSupport ) == requiredSupport )
        {
            pMultisampleIndex->push_back( *it );
        }
    }

Cleanup:

    return hr;
}

HRESULT CDXGIPresentSingleSC::CreateAdapterDevice
    (
    TEST_FEATURE_LEVEL testFeatureLevel,
    UINT adapter,
    D3D_DEVICE_POINTER *pDevicePointer
    )
{
    HRESULT hCreateDeviceResult = S_OK;
    DXGI_FACTORY_POINTER pFactoryPointer;
    DXGI_ADAPTER_POINTER pAdapterPointer;
    ZeroMemory(&pFactoryPointer, sizeof(pFactoryPointer));
    ZeroMemory(&pAdapterPointer, sizeof(pAdapterPointer));


    hCreateDeviceResult = g_Application.CreateFactoryAdapterDevice
        (
        &pFactoryPointer,
        adapter,
        &pAdapterPointer,
        g_Application.m_D3DOptions.SrcDriverType,
        D3D10_CREATE_DEVICE_SINGLETHREADED,
        testFeatureLevel,
        0,
        pDevicePointer
        );

    if
        (
        hCreateDeviceResult == DXGI_ERROR_UNSUPPORTED ||
        hCreateDeviceResult == E_NOINTERFACE ||
        hCreateDeviceResult == E_NOTIMPL
        )
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateAdapterDevice Could not create device used for CheckMultisampleQualityLevels.  0x%x", hCreateDeviceResult
            );

        goto Cleanup;
    }
    else if ( FAILED( hCreateDeviceResult))
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateAdapterDevice:  CreateDevice failed with error 0x%x.",
            hCreateDeviceResult
            );

        goto Cleanup;
    };

    if ( pDevicePointer->pDevice == NULL)
    {
        WriteToLog
            (
            "CDXGIPresentSingleSC::CreateAdapterDevice:  CreateDevice succeeded but returned a NULL device pointer."
            );

        goto Cleanup;
    };

Cleanup:
    if ( NULL != pFactoryPointer.pFactory)
    {
        pFactoryPointer.pFactory->Release();
        pFactoryPointer.pFactory = NULL;
    };

    if ( NULL != pAdapterPointer.pAdapter)
    {
        pAdapterPointer.pAdapter->Release();
        pAdapterPointer.pAdapter = NULL;
    };

    return hCreateDeviceResult;
}


///////////////////////////////////////////////////////////////////////////////
//
// DxgiModeRotationToDisplayOrientation
//
///////////////////////////////////////////////////////////////////////////////
DWORD  CDXGIPresentSingleSC::DxgiModeRotationToDisplayOrientation
    (
    DXGI_MODE_ROTATION dxgiModeRotation
    )
{
    DWORD displayOrientation = DMDO_DEFAULT;

    switch ( dxgiModeRotation )
    {
    case DXGI_MODE_ROTATION_IDENTITY:
        {
            displayOrientation = DMDO_DEFAULT;
        } break;

    case DXGI_MODE_ROTATION_ROTATE90:
        {
            displayOrientation = DMDO_90;
        } break;

    case DXGI_MODE_ROTATION_ROTATE180:
        {
            displayOrientation = DMDO_180;
        } break;

    case DXGI_MODE_ROTATION_ROTATE270:
        {
            displayOrientation = DMDO_270;
        } break;
    };

    return displayOrientation;
}

///////////////////////////////////////////////////////////////////////////////
//
// DisplayOrientationToDxgiModeRotation
//
///////////////////////////////////////////////////////////////////////////////
DXGI_MODE_ROTATION  CDXGIPresentSingleSC::DisplayOrientationToDxgiModeRotation
    (
    DWORD displayOrientation
    )
{
    DXGI_MODE_ROTATION dxgiModeRotation = DXGI_MODE_ROTATION_UNSPECIFIED;

    switch ( displayOrientation )
    {
    case DMDO_DEFAULT:
        {
            dxgiModeRotation = DXGI_MODE_ROTATION_IDENTITY;
        } break;

    case DMDO_90:
        {
            dxgiModeRotation = DXGI_MODE_ROTATION_ROTATE90;
        } break;

    case DMDO_180:
        {
            dxgiModeRotation = DXGI_MODE_ROTATION_ROTATE180;
        } break;

    case DMDO_270:
        {
            dxgiModeRotation = DXGI_MODE_ROTATION_ROTATE270;
        } break;
    };

    return dxgiModeRotation;
}

DirectX::Image Image(const D3D_TEXTURE2D_DESC& d, const D3D_MAPPED_TEXTURE2D& m)
{
    DirectX::Image i;
    i.width = d.Width;
    i.height = d.Height;
    i.format = d.Format;
    i.rowPitch = m.RowPitch;
    i.slicePitch = m.RowPitch * d.Height;
    i.pixels = reinterpret_cast<decltype(i.pixels)>(m.pData);
    return i;
}

HRESULT CDXGIPresentSingleSC::AdjustForStretchAndRotation(const Vector2D<float>& StretchFactor, DXGI_MODE_ROTATION Rotation, CHandle<ID3DTexture2D>& Texture)
{
    D3D_TEXTURE2D_DESC texDesc;
    Texture->GetDesc(&texDesc);

    if (StretchFactor.signbit())
    {
        // Need to flip horizontally, vertically or both.
        CHandle<ID3DTexture2D> DestTextureAdjusted;
        {
            MappedSurface Src(*Texture);
            HRESULT hr = Src.Map();
            if (FAILED(hr))
            {
                WriteToLog(_T("CPresentWinConvertStretch::AdjustForStretchAndRotation: Mapping staging surface failed with error 0x%x."), hr);
                return hr;
            }

            auto factory = ImagingFactory();
            if(!factory)
            {
                WriteToLog(_T("IWICImagingFactory creation failed with error 0x%x"), ImagingFactoryResult());
                return ImagingFactoryResult();
            }

            DirectX::ScratchImage s;
            hr = DirectX::FlipRotate(*factory, Image(texDesc, Src), FlipFlags(StretchFactor), s);
            if (FAILED(hr))
            {
                WriteToLog(_T("CPresentWinConvertStretch::AdjustForStretchAndRotation: Flipping (mirroring) staging surface failed with error 0x%x."), hr);
                return hr;
            }

            const D3D10_SUBRESOURCE_DATA InitialData = { s.GetPixels(), s.RowPitch() };
            hr = m_DevicePointer.pDevice->CreateTexture2D(&texDesc, &InitialData, reinterpret_cast<ID3DTexture2D**>(&DestTextureAdjusted));
            if (FAILED(hr))
            {
                WriteToLog(_T("CPresentWinConvertStretch::AdjustForStretchAndRotation: CreateTexture2D on staging surface failed with error 0x%x."), hr);
                return hr;
            }
        }
        Texture = DestTextureAdjusted;
    }

    if (Rotation != DXGI_MODE_ROTATION_IDENTITY)
    {
        // Need to rotate.
        CHandle<ID3DTexture2D> DestTextureAdjusted;
        {
            MappedSurface Src(*Texture);
            HRESULT hr = Src.Map();
            if (FAILED(hr))
            {
                WriteToLog(_T("CPresentWinConvertStretch::AdjustForStretchAndRotation: Mapping staging surface failed with error 0x%x."), hr);
                return hr;
            }

            auto factory = ImagingFactory();
            if(!factory)
            {
                WriteToLog(_T("IWICImagingFactory creation failed with error 0x%x"), ImagingFactoryResult());
                return ImagingFactoryResult();
            }

            DirectX::ScratchImage s;
            hr = DirectX::FlipRotate(*factory, Image(texDesc, Src), RotateFlags(Rotation), s);
            if (FAILED(hr))
            {
                WriteToLog(_T("CPresentWinConvertStretch::AdjustForStretchAndRotation: Rotating staging surface failed with error 0x%x."), hr);
                return hr;
            }

            const D3D10_SUBRESOURCE_DATA InitialData = { s.GetPixels(), s.RowPitch() };
            if (Helper::OddRotation(Rotation)) std::swap(texDesc.Width, texDesc.Height);
            hr = m_DevicePointer.pDevice->CreateTexture2D(&texDesc, &InitialData, reinterpret_cast<ID3DTexture2D**>(&DestTextureAdjusted));
            if (FAILED(hr))
            {
                WriteToLog(_T("CPresentWinConvertStretch::AdjustForStretchAndRotation: CreateTexture2D on staging surface failed with error 0x%x."), hr);
                return hr;
            }
        }
        Texture = DestTextureAdjusted;
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// CreateTextureAndCopyBackBuffer
//
// Creates a texture that caller will need to release.
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentSingleSC::CreateTextureAndCopyBackBuffer
    (
    __out ID3DTexture2D ** pDestTexture,
    DXGI_FORMAT format,
    const Vector2D<unsigned>* SourceSize
    )
{
    //
    // Initialize locals.
    //

    HRESULT hr = S_OK;

    if(*pDestTexture != NULL)
    {
        (*pDestTexture)->Release();
        *pDestTexture = NULL;
    }

    DXGI_SURFACE_DESC sd;
    ZeroMemory( &sd, sizeof( DXGI_SURFACE_DESC));

    D3D_TEXTURE2D_DESC texDesc;
    ZeroMemory( &texDesc, sizeof( texDesc ) );

    D3DX10_TEXTURE_LOAD_INFO tlinfo;
    ZeroMemory( &tlinfo, sizeof( tlinfo ) );

    // default texture load info settings
    tlinfo.pSrcBox 		= 0;
    tlinfo.pDstBox 		= 0;
    tlinfo.SrcFirstMip 	= 0;
    tlinfo.DstFirstMip 	= 0;
    tlinfo.NumMips 		= D3DX10_DEFAULT;
    tlinfo.SrcFirstElement = 0;
    tlinfo.DstFirstElement = 0;
    tlinfo.NumElements 	= D3DX10_DEFAULT;

    //
    // Get a reference to buffer 0.
    //

    CHandle<ID3DTexture2D> pBackBuffer;
    hr = m_pSwapChain->GetBuffer
        (
        0,
        __uuidof(pBackBuffer),
        reinterpret_cast <void**>( &pBackBuffer)
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CPresentWinConvertStretch::CreateTextureAndCopyBackBuffer:  GetBuffer 0 failed with error 0x%x.",
            hr
            );
        return hr;
    };


    //
    // Query the buffer for its IDXGISurface interface.
    //
    CHandle<IDXGISurface> pSurface;
    hr = pBackBuffer->QueryInterface
        (
        IID_IDXGISurface,
        reinterpret_cast <void**>( &pSurface)
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CPresentWinConvertStretch::CreateTextureAndCopyBackBuffer:  QueryInterface for IID_IDXGISurface failed with error 0x%x.",
            hr
            );
        return hr;
    };


    //
    // Get a descriptor for the surface.
    //

    hr = pSurface->GetDesc( &sd );
    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CPresentWinConvertStretch::CreateTextureAndCopyBackBuffer:  GetDesc on back buffer surface failed with error 0x%x.",
            hr
            );
        return hr;
    };

    if( sd.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB || sd.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB )
    {
        // first use D3DX10LoadTextureFromTexture with D3DX10_FILTER_SRGB only.
        // D3DX10_FILTER_LINEAR | D3DX10_FILTER_SRGB causes a crash, so using them together needs to be avoided
        // The sRGB flag is used to turn off D3DX sRGB conversion. A shader is used to do this in the test.

        texDesc.Format = format;
        texDesc.Width = sd.Width;
        texDesc.Height = sd.Height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.SampleDesc.Count = sd.SampleDesc.Count;
        texDesc.SampleDesc.Quality = sd.SampleDesc.Quality;
        texDesc.Usage = (D3D_USAGE) D3D10_USAGE_STAGING;
        texDesc.BindFlags = 0;
        texDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
        texDesc.MiscFlags = 0;

        CHandle<ID3DTexture2D> pBackBuffer_SRGB;
        hr = m_DevicePointer.pDevice->CreateTexture2D
            (
            &texDesc,
            NULL,
            reinterpret_cast <ID3DTexture2D**>( &pBackBuffer_SRGB )
            );

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                "CPresentWinConvertStretch::CreateTextureAndCopyBackBuffer:  CreateTexture2D on SRGB staging surface failed with error 0x%x.",
                hr
                );
            return hr;
        };


        //
        // Perform a straight data format conversion without linear filters or sRGB conversion
        //
        tlinfo.Filter  		= D3DX10_FILTER_POINT | D3DX10_FILTER_SRGB ;
        tlinfo.MipFilter   	= D3DX10_FILTER_POINT | D3DX10_FILTER_SRGB ;

        hr = D3DX10LoadTextureFromTexture
            (
            pBackBuffer,
            &tlinfo,
            pBackBuffer_SRGB
            );

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                "CPresentWinConvertStretch::CreateTextureAndCopyBackBuffer:  D3DX10LoadTextureFromTexture failed with error 0x%x.",
                hr
                );
            return hr;
        };

        // replace pBackBuffer texture with new non-sRGB formatted texture
        pBackBuffer = pBackBuffer_SRGB;
    }

    //
    // Create a staging texture.
    //
    texDesc.Width = rectDeviceWindow.right - rectDeviceWindow.left;
    if (texDesc.Width <= 0)
    {
        texDesc.Width = 1;
    }

    texDesc.Height = rectDeviceWindow.bottom - rectDeviceWindow.top;
    if(texDesc.Height <= 0)
    {
        texDesc.Height = 1;
    }

    texDesc.Format = format;

    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = sd.SampleDesc.Count;
    texDesc.SampleDesc.Quality = sd.SampleDesc.Quality;

    C_ASSERT( D3D10_USAGE_STAGING == D3D11_USAGE_STAGING );
    texDesc.Usage = (D3D_USAGE) D3D10_USAGE_STAGING;
    texDesc.BindFlags = 0;

    C_ASSERT( D3D10_CPU_ACCESS_READ == D3D11_CPU_ACCESS_READ );
    C_ASSERT( D3D10_CPU_ACCESS_WRITE == D3D11_CPU_ACCESS_WRITE );
    texDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
    texDesc.MiscFlags = 0;

    CHandle<ID3DTexture2D> DestTexture;
    hr = m_DevicePointer.pDevice->CreateTexture2D
        (
        &texDesc,
        NULL,
        reinterpret_cast <ID3DTexture2D**>(&DestTexture)
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CPresentWinConvertStretch::CreateTextureAndCopyBackBuffer:  CreateTexture2D on staging surface failed with error 0x%x.",
            hr
            );
        return hr;
    };


    //
    // Load the buffer data into our staging texture using linear filter
    //
    tlinfo.Filter  		= D3DX10_FILTER_LINEAR ;
    tlinfo.MipFilter   	= D3DX10_FILTER_LINEAR ;

    D3D10_BOX SrcBox = {};
    if (SourceSize)
    {
        SrcBox.right = SourceSize->x();
        SrcBox.bottom = SourceSize->y();
        SrcBox.back = 1;
        tlinfo.pSrcBox = &SrcBox;
    }

    hr = D3DX10LoadTextureFromTexture
        (
        pBackBuffer,
        &tlinfo,
        DestTexture
        );

    if ( FAILED( hr ) )
    {
        WriteToLog
            (
            "CPresentWinConvertStretch::CreateTextureAndCopyBackBuffer:  D3DX10LoadTextureFromTexture failed with error 0x%x.",
            hr
            );
        return hr;
    }

    *pDestTexture = DestTexture.Detach();

    return hr;
}


//-----------------------------------------------------------------------------
//
//InitializeBackBufferWithDirtyRects
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentSingleSC::InitializeBackBufferWithDirtyRects(
    CDeviceWrapper10* pDeviceWrapper,
    CSwapChainWrapper* pSwapchainWrapper,
    UINT textureIndex,
    RGNDATA* pDirtyData)
{
    //
    // Set up an intermediate texture to copy the src texture into.  This stagins texture
    // will be the same size/format as the backbuffer and handle stretching/format
    // conversion.
    //
    // They copy the dirty rects from the staging texture to the swapchain buffer
    //

    HRESULT hr;

    ID3D10Texture2D* pTexture2D = NULL;
    ID3D10RenderTargetView* pRTView = NULL;
    IDXGISwapChain* pSwapchain = NULL;
    ID3D10Resource* pSwapchainAsResource = NULL;
    ID3D10Device* pDevice = NULL;

    DXGI_SWAP_CHAIN_DESC scDesc;
    D3D10_TEXTURE2D_DESC tex2DDesc;
    D3D10_RENDER_TARGET_VIEW_DESC d3d10RTDesc;
    D3D10_VIEWPORT viewPort;
    BOOL renderXR_BIAS = FALSE;

    ID3D10ShaderResourceView* pSRView = pBBInitSRV[textureIndex];

    // Set swapchain/device references

    if (!pSwapchainWrapper->GetSwapChainPtr(&pSwapchain))
    {
        WriteToLog("CDXGIPresentSingleSC::InitializeBackBufferWithDirtyRects: Could not get swapchain out of test wrapper object.");
        hr = E_FAIL;
        goto End;
    }
    if (!pDeviceWrapper->GetDevicePtr(&pDevice))
    {
        WriteToLog("CDXGIPresentSingleSC::InitializeBackBufferWithDirtyRects: Could not get device out of test wrapper object.");
        hr = E_FAIL;
        goto End;
    }

    // Set up the intermediate texture
    pSwapchain->GetDesc(&scDesc);
    tex2DDesc.Width = scDesc.BufferDesc.Width;
    tex2DDesc.Height = scDesc.BufferDesc.Height;
    tex2DDesc.MipLevels = 1;
    tex2DDesc.ArraySize = 1;
    tex2DDesc.Format = scDesc.BufferDesc.Format;
    tex2DDesc.SampleDesc = scDesc.SampleDesc;
    tex2DDesc.Usage = D3D10_USAGE_DEFAULT;
    tex2DDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
    tex2DDesc.CPUAccessFlags = 0;
    tex2DDesc.MiscFlags = 0;

    hr =  m_DevicePointer.pDevice->CreateTexture2D(&tex2DDesc, NULL, &pTexture2D);
    if (FAILED(hr))
    {
        WriteToLog("CDXGIPresentSingleSC::InitializeBackBufferWithDirtyRects: Could not create intermediate texture: 0x%x", hr);
        goto End;
    }

    // Set up render target view
    d3d10RTDesc.Format = scDesc.BufferDesc.Format;
    d3d10RTDesc.ViewDimension = scDesc.SampleDesc.Count > 1 ? D3D10_RTV_DIMENSION_TEXTURE2DMS : D3D10_RTV_DIMENSION_TEXTURE2D;
    d3d10RTDesc.Texture2D.MipSlice = 0;

    if (scDesc.BufferDesc.Format == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
    {
        d3d10RTDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
        renderXR_BIAS = TRUE;
    }

    hr = m_DevicePointer.pDevice->CreateRenderTargetView(pTexture2D, &d3d10RTDesc, &pRTView);
    if (FAILED(hr))
    {
        WriteToLog("CDXGIPresentSingleSC::InitializeBackBufferWithDirtyRects: Could not create intermediate render target view: 0x%x", hr);
        goto End;
    }

    // Set up viewport
    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;
    viewPort.Width = scDesc.BufferDesc.Width;
    viewPort.Height = scDesc.BufferDesc.Height;


    // Render to intermediate texture
    bool bRenderTextureResult = pDeviceWrapper->RenderTexture(pRTView, pSRView, &viewPort, renderXR_BIAS);
    if (bRenderTextureResult != true)
    {
        WriteToLog("CDXGIPresentSingleSC::InitializeBackBufferWithDirtyRects: Could not render to intermediate texture.");
        goto End;
    }

    // Copy from intermediate texture to back buffer width dirty rects
    hr = pSwapchain->GetBuffer(0, __uuidof(ID3D10Resource), (void**)&pSwapchainAsResource);
    assert(SUCCEEDED(hr));

    D3D10_BOX srcBox;

    if (!pDirtyData)
    {
        srcBox.left = 0;
        srcBox.top = 0;
        srcBox.front = 0;
        srcBox.right = scDesc.BufferDesc.Width;
        srcBox.bottom = scDesc.BufferDesc.Height;
        srcBox.back = 1;

        pDevice->CopySubresourceRegion(
            pSwapchainAsResource,   // dst
            0,                      // dst sub resource,
            0,                      // dst x
            0,                      // dst y
            0,                      // dst z
            pTexture2D,             // src
            0,                      // src sub resource
            &srcBox
            );
    }
    else
    {
        UINT nDirtyRects = pDirtyData->rdh.nCount;
        RECT* pRects = (RECT*)pDirtyData->Buffer;


        for (UINT i = 0; i < nDirtyRects; i++)
        {
            srcBox.left = pRects[i].left;
            srcBox.right = pRects[i].right;
            srcBox.top = pRects[i].top;
            srcBox.bottom = pRects[i].bottom;
            srcBox.front = 0;
            srcBox.back = 1;

            pDevice->CopySubresourceRegion(
                pSwapchainAsResource,   // dst
                0,                      // dst sub resource,
                pRects[i].left,         // dst x
                pRects[i].top,          // dst y
                0,                      // dst z
                pTexture2D,             // src
                0,                      // src sub resource
                &srcBox
            );
        }
    }

    hr = S_OK;

End:
    if (pTexture2D)
    {
        pTexture2D->Release();
    }
    if (pRTView)
    {
        pRTView->Release();
    }
    if (pSwapchainAsResource)
    {
        pSwapchainAsResource->Release();
    }
    if (pSwapchain)
    {
        pSwapchain->Release();
    }
    if (pDevice)
    {
        pDevice->Release();
    }

    return hr;
}

//-----------------------------------------------------------------------------
//
//InitializeBackBuffer
//
//-----------------------------------------------------------------------------
HRESULT CDXGIPresentSingleSC::InitializeBackBuffer(
    CSwapChainWrapper* pSwapchainWrapper,
    UINT textureIndex,
    const Vector2D<unsigned>* Dimension)
{
    bool bInitializeBackBufferResult = false;

    //
    // This will create a rendertargetview so the buffer needs to be writeable.
    // Only index 0 not DXGI_USAGE_READ_ONLY
    //
    UINT bbIndex = 0;

    switch ( m_TestFeatureLevel )
    {
    case TEST_FEATURE_11_0_LEVEL_9_1:
    case TEST_FEATURE_11_0_LEVEL_9_2:
    case TEST_FEATURE_11_0_LEVEL_9_3:
    case TEST_FEATURE_10_1_LEVEL_9_1:
    case TEST_FEATURE_10_1_LEVEL_9_2:
    case TEST_FEATURE_10_1_LEVEL_9_3:
        {
            bInitializeBackBufferResult = pSwapchainWrapper->InitBBFrom2DTexture
                (
                bbIndex,
                pBBInitSRV[textureIndex],
                "vs_4_0_level_9_1",
                "ps_4_0_level_9_1",
                Dimension
                );
        } break;


    default:
        {
            bInitializeBackBufferResult = pSwapchainWrapper->InitBBFrom2DTexture
                (
                bbIndex,
                pBBInitSRV[textureIndex],
                "vs_4_0",
                "ps_4_0",
                Dimension
                );
        } break;
    };

    if ( bInitializeBackBufferResult != true )
    {
        WriteToLog
            (
            "InitBBFrom2DTexture on swap chain back buffer failed."
            );
        if( pSwapchainWrapper->GetLastError() == E_OUTOFMEMORY )
            return E_OUTOFMEMORY;
        return E_FAIL;
    };

    return S_OK;
}



///////////////////////////////////////////////////////
// Function:  LogForegroundWindow
//
// Purpose:
// Logs the foreground window for diagnostic purposes.
///////////////////////////////////////////////////////
HRESULT CDXGIPresentSingleSC::CheckForegroundWindow
    (
    LPCTSTR ExpectedClassName
    )
{
    HRESULT hr = E_FAIL;
    //
    // Determine the foreground window.
    //

    HWND hForegroundWindow = GetForegroundWindow();
    TCHAR pForegroundWindowText[ 128 ] = _T( "" );
    int nForegroundWindowText = GetWindowText
        (
        hForegroundWindow,
        pForegroundWindowText,
        ARRAYSIZE( pForegroundWindowText )
        );


    TCHAR pForegroundWindowClass[ 128 ] = _T( "" );
    /* int nForegroundWindowClass = */ GetClassName
        (
        hForegroundWindow,
        pForegroundWindowClass,
        ARRAYSIZE( pForegroundWindowClass )
        );


    if ( hForegroundWindow == NULL )
    {
        WriteToLog
            (
            _T( "Foreground window is NULL!" )
            );
    }
    else
    {
        WriteToLog
            (
            _T( "Foreground window is 0x%x of class \"%s\" with caption \"%s\"." ),
            hForegroundWindow,
            pForegroundWindowClass,
            nForegroundWindowText > 0 ? pForegroundWindowText : _T( "[No caption]" )
            );
    };


    if
        (
        hForegroundWindow != NULL &&
        _tcsicmp
        (
        pForegroundWindowClass,
        ExpectedClassName
        ) != 0
        )
    {
        WriteToLog
            (
            _T( "\"%s\" : This likely indicates that another application has popped up over the test.  If so, please close the other app, and re-run the test." ),
            ExpectedClassName
            );
    }
    else
    {
            WriteToLog
            (
            _T( "Foreground window matches expected application window. " )
            );
            hr = S_OK;
    }

    return hr;
};

unsigned FlipFlags(const Vector2D<float>& v)
{
    return (Helper::signbit(v.x()) ? WICBitmapTransformFlipHorizontal : 0) | (Helper::signbit(v.y()) ? WICBitmapTransformFlipVertical : 0);
}

unsigned RotateFlags(DXGI_MODE_ROTATION r)
{
    switch(r)
    {
        case DXGI_MODE_ROTATION_IDENTITY: return WICBitmapTransformRotate0;
        case DXGI_MODE_ROTATION_ROTATE90: return WICBitmapTransformRotate270;
        case DXGI_MODE_ROTATION_ROTATE270: return WICBitmapTransformRotate90;
        case DXGI_MODE_ROTATION_ROTATE180: return WICBitmapTransformRotate180;
    }
    assert(false);
    return WICBitmapTransformRotate0;
}
