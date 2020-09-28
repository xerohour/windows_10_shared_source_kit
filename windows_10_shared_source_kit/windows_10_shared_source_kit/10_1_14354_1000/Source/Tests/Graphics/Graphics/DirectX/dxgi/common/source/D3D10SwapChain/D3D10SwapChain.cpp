#include "D3D10SwapChain.h"
#include <StrSafe.h>


#undef MSG_LEN
#define MSG_LEN	 1024


////////////////////////////////////////////////////////////////////////////////
// Definitions of vector structures.

typedef struct _Vec4
{
	float x;
	float y;
	float z;
	float w;
}
Vec4;

typedef struct _Vec2
{
	float u;
	float v;
}
Vec2;

typedef struct _Vec3
{
	float u;
	float v;
	float w;
}
Vec3;

typedef struct _Vtx
{
	Vec4 Position;
	Vec2 Texcoord;
	//Vec3 Texcoord2;
}
Vtx;

// maps unsigned 8 bits/channel to D3DCOLOR
/*#define D3DCOLOR_ARGB(a,r,g,b) \
	((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)
#define D3DCOLOR_XRGB(r,g,b)   D3DCOLOR_ARGB(0xff,r,g,b)*/


////////////////////////////////////////////////////////////////////////////////
// clsD3D9Device Methods

clsD3D9Device::clsD3D9Device()
:
	m_hD3D9( NULL ),
	m_pDirect3DCreate9Ex( NULL ),
	m_pD3DEx( NULL ),
    m_RecreateDevice(false)
{
};


clsD3D9Device::~clsD3D9Device()
{
	Cleanup();
};


HRESULT clsD3D9Device::Initialize
(
	bool bTerminalServices,
    bool recreateDevice
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;
    m_RecreateDevice = recreateDevice;

	//
	// Clean up existing state.
	//

	Cleanup();


	//
	// Load Direct3D 9 or the remote prototype.
	//

	if
	(
		bTerminalServices == true &&
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		m_hD3D9 = LoadLibraryExW( L"d3d9rdp.dll", NULL, 0 );
	}
	else
	{
		m_hD3D9 = LoadLibraryExW( L"d3d9.dll", NULL, 0 );
	};

	if ( m_hD3D9 == NULL )
	{
		DWORD LoadLibraryError = GetLastError();

		if ( LoadLibraryError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( LoadLibraryError );
		};

		goto Cleanup;
	};


	//
	// Locate our Direct3D 9 entry point.
	//

	m_pDirect3DCreate9Ex = (PDIRECT3DCREATE9EX) GetProcAddress
	(
		m_hD3D9,
		"Direct3DCreate9Ex"
	);

	if ( m_pDirect3DCreate9Ex == NULL )
	{
		DWORD GetProcAddressError = GetLastError();

		if ( GetProcAddressError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( GetProcAddressError );
		};

		goto Cleanup;
	};


	//
	// Create our Direct3D 9 enumerator.
	//

	HRESULT hCreateEnumeratorResult = (*m_pDirect3DCreate9Ex)
	(
		D3D_SDK_VERSION,
		&m_pD3DEx
	);

	if ( FAILED( hCreateEnumeratorResult ) )
	{
		hOverallResult = hCreateEnumeratorResult;
		goto Cleanup;
	};

	if ( m_pD3DEx == NULL )
	{
		goto Cleanup;
	};


	//
	// Determine how many hardware adapters are installed.
	//

	UINT uAdapterCount = m_pD3DEx->GetAdapterCount();

	if ( uAdapterCount == 0 )
	{
		goto Cleanup;
	};


	//
	// Create a device off of each adapter.
	//

	for( UINT uAdapter = 0; uAdapter < uAdapterCount; uAdapter++ )
	{
		HMONITOR hMonitor = m_pD3DEx->GetAdapterMonitor( uAdapter );
		m_MonitorMap[ hMonitor ] = uAdapter;
		m_SurfaceMap[ hMonitor ] = NULL;

		D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
		HRESULT hGetAdapterIdentifier = m_pD3DEx->GetAdapterIdentifier
		(
			uAdapter,
			0,
			&AdapterIdentifier
		);

		if ( SUCCEEDED( hGetAdapterIdentifier ) )
		{
			WCHAR DeviceName[ 32 ];
			int nDeviceName = MultiByteToWideChar
			(
				CP_ACP,
				0,
				AdapterIdentifier.DeviceName,
				-1,
				DeviceName,
				ARRAYSIZE( DeviceName )
			);

			if ( nDeviceName == 0 )
			{
				DebugOut.Write
				(
					"clsD3D9Device::Initialize:  MultiByteToWideChar failed."
				);

				goto Cleanup;
			};

			std::wstring DeviceNameString( DeviceName );
			m_DeviceNameToMonitorMap[ DeviceNameString ] = hMonitor;
		};
		
		D3DPRESENT_PARAMETERS PresentationParameters;
		ZeroMemory( &PresentationParameters, sizeof( D3DPRESENT_PARAMETERS ) );
		PresentationParameters.BackBufferWidth = PresentationParameters.BackBufferHeight = 1;
		PresentationParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
		PresentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		PresentationParameters.Windowed = TRUE;
		PresentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

		IDirect3DDevice9Ex *pDeviceEx;
		m_pD3DEx->CreateDeviceEx
		(
			uAdapter,
			D3DDEVTYPE_HAL,
			NULL,
			D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&PresentationParameters,
			NULL,
			&pDeviceEx
		);

		 m_DeviceExVector.push_back( pDeviceEx );
	}


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


void clsD3D9Device::Cleanup()
{
	ULONG uRefCount = 0;

	std::map< HMONITOR, IDirect3DSurface9* >::const_iterator iterSurface;
	for( iterSurface = m_SurfaceMap.begin(); iterSurface != m_SurfaceMap.end(); iterSurface++ )
		if( NULL != iterSurface->second )
		{
			uRefCount = iterSurface->second->Release();
			_ASSERT( 0 == uRefCount );
		}
	m_SurfaceMap.clear();

	for( std::vector< IDirect3DDevice9Ex* >::const_iterator iterDevice = m_DeviceExVector.begin(); iterDevice != m_DeviceExVector.end(); iterDevice++ )
		if( NULL != (*iterDevice) )
		{
			uRefCount = (*iterDevice)->Release();
			_ASSERT( 0 == uRefCount );
		}
	m_DeviceExVector.clear();

	if( NULL != m_pD3DEx )
		uRefCount = m_pD3DEx->Release();
	_ASSERT( 0 == uRefCount );
	m_pD3DEx = NULL;


	//
	// Free Direct3D 9.
	//

	m_pDirect3DCreate9Ex = NULL;
	if ( m_hD3D9 != NULL )
	{
		FreeLibrary( m_hD3D9 );
		m_hD3D9 = NULL;
	};
};

bool clsD3D9Device::GetFrontBufferData
(
	LPCWSTR DeviceName,
	IDXGISurface * pDXGISurface
)
{
	if ( DeviceName == NULL )
	{
		DebugOut.Write
		(
			"clsD3D9Device::GetFrontBufferData:  DeviceName is NULL.\n"
		);

		return false;
	};


	HMONITOR hMonitor = m_DeviceNameToMonitorMap[ DeviceName ];

	return GetFrontBufferData
	(
		hMonitor,
		pDXGISurface
	);
};

bool clsD3D9Device::GetFrontBufferData( 
    HMONITOR hMonitor,
    IDXGISurface *pDXGISurface)
{
	//
	// Initialize locals.
	//

	HRESULT hr = S_OK;

	D3DSURFACE_DESC surfaceDesc;
	ZeroMemory( &surfaceDesc, sizeof( D3DSURFACE_DESC ) );

	IDirect3DDevice9Ex * pDeviceEx = NULL;

	IDirect3DSurface9 * pSurface = NULL;


	//
	// Validate parameters.
	//

	if ( pDXGISurface == NULL )
	{
		DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  DXGI surface pointer was NULL.\n" );
		return false;
	};


	pSurface = m_SurfaceMap[ hMonitor ];
	if ( pSurface != NULL )
	{
		if ( FAILED( pSurface->GetDesc( &surfaceDesc ) ) )
		{
			DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  Failed to get surface descriptor.\n" );
			return false;
		};
	};
    
    UINT uAdapter = m_MonitorMap[ hMonitor ];
	if ( uAdapter >= m_DeviceExVector.size() )
	{
        uAdapter = D3DADAPTER_DEFAULT;
	}

	pDeviceEx = m_DeviceExVector[ uAdapter ];
    
    
    //
    //  Mode changes or fullscreen to windowed transitions may confuse this device and 
    //  cause it to start returning black for GetFrontBufferData.  Since it doesn't have
    //  a window, reset fails.  Just recreating it seems to work.
    //
    if (m_RecreateDevice)
    {
        pDeviceEx->Release();

        D3DPRESENT_PARAMETERS PresentationParameters;
		ZeroMemory( &PresentationParameters, sizeof( D3DPRESENT_PARAMETERS ) );
		PresentationParameters.BackBufferWidth = PresentationParameters.BackBufferHeight = 1;
		PresentationParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
		PresentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		PresentationParameters.Windowed = TRUE;
		PresentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

		m_pD3DEx->CreateDeviceEx
		(
			uAdapter,
			D3DDEVTYPE_HAL,
			NULL,
			D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&PresentationParameters,
			NULL,
			&pDeviceEx
		);    
        
        m_DeviceExVector[ uAdapter ] = pDeviceEx;
    }


	if ( pDeviceEx == NULL )
	{
		DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  Failed to get the device for the monitor.\n" );
		return false;
	};

    hr = pDeviceEx->CheckDeviceState( NULL );
	DebugOut.Write
	(
		"clsD3D9Device::GetFrontBufferData:  CheckDeviceState 0x%x\n", hr
	);


    UINT displayModeWidth = 0; 
	UINT displayModeHeight = 0;

	MONITORINFOEX mi;
	ZeroMemory( &mi, sizeof( MONITORINFOEX ) );
	mi.cbSize = sizeof( MONITORINFOEX );
	
   
	//
	//  GetAdapterDisplayModeEx has a bug and always returns the D3DADAPTER_DEFAULT
	//
	//  if( FAILED( hr = m_pD3DEx->GetAdapterDisplayModeEx( uAdapter < m_DeviceExVector.size() ? uAdapter :  D3DADAPTER_DEFAULT, &displayModeEx, &Rotation ) ) )
	if( !GetMonitorInfo(hMonitor, &mi))
	{
		DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  Failed to get the display mode for the monitor. 0x%x\n", GetLastError() );
		return false;
	}

	displayModeWidth = mi.rcMonitor.right - mi.rcMonitor.left;
	displayModeHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

	// surface may need to be recreated, if NULL or if it doesn't match current mode, or if we just recreated the device.
	if
	(
		m_RecreateDevice ||
		NULL == pSurface ||
		surfaceDesc.Width != displayModeWidth ||
		surfaceDesc.Height != displayModeHeight
	)
	{
		if( NULL != pSurface )
		{
			pSurface->Release();
			pSurface = NULL;
		}

		hr = pDeviceEx->CreateOffscreenPlainSurface(	displayModeWidth, 
														displayModeHeight,
														D3DFMT_A8R8G8B8,
														D3DPOOL_SYSTEMMEM,
														&pSurface,
														NULL );

		// update the surface
		m_SurfaceMap[ hMonitor ] = pSurface;

		if (FAILED(hr))
		{
			DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  Failed to create offscreen plain surface.\n" );
			return false;
		}
	}

	DXGI_SURFACE_DESC dxgiSurfaceDesc;
	if( FAILED( hr = pDXGISurface->GetDesc( &dxgiSurfaceDesc ) ) )
	{
		DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  Failed to get descriptor for the DXGISurface.\n" );
		return false;
	}

	if( dxgiSurfaceDesc.Width != displayModeWidth || dxgiSurfaceDesc.Height != displayModeHeight )
	{
		DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  DXGISurface doesn't match the frontbuffer size. displayModeWidth=%d, displayModeHeight=%d\n",displayModeWidth,displayModeHeight );
		return false;
	}

	// short delay to ensure the d3d10 data is presented onscreen
	Sleep( 25 );

	// now it's safe to get the front buffer surface
	if( FAILED( hr = pDeviceEx->GetFrontBufferData( 0, pSurface ) ) )
	{
		DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  Failed to get the frontbuffer data.\n" );
		return false;
	}

	// move the D3D9 surface into the DXGI surface

	D3DLOCKED_RECT LockedRect;
	if( FAILED( hr = pSurface->LockRect( &LockedRect, NULL, D3DLOCK_READONLY ) ) )
	{
		DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  Failed to lock the D3D9 surface.\n" );
		return false;
	}

	DXGI_MAPPED_RECT DXGILockedRect;
	if( FAILED( hr = pDXGISurface->Map( &DXGILockedRect, DXGI_MAP_WRITE ) ) )
	{
		DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  Failed to map the DXGI surface.\n" );
		pSurface->UnlockRect();
		return false;
	}

	switch( dxgiSurfaceDesc.Format )
	{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		{
			for( UINT y = 0; y < dxgiSurfaceDesc.Height; y++ )
				for( UINT x = 0; x < dxgiSurfaceDesc.Width; x++ )
				{
					DWORD dwD3D9Surface = *(DWORD*)( (UINT_PTR)LockedRect.pBits + 
														 y * LockedRect.Pitch + 
														 ( (UINT_PTR)x << 2 ) );

					// exchange blue and red channels
					BYTE alpha = (BYTE)( 0xff/*dwD3D9Surface >> 24*/ );	// always set alpha to 1 to match dxgi
					BYTE red = (BYTE)( dwD3D9Surface >> 16 );
					BYTE green = (BYTE)( dwD3D9Surface >> 8 );
					BYTE blue = (BYTE)( dwD3D9Surface >> 0 );
					
                    if(dxgiSurfaceDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM ||
                       dxgiSurfaceDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB )
                    {
                        dwD3D9Surface = D3DCOLOR_ARGB( alpha, blue, green, red );
                    }
                    else
                    {
                        dwD3D9Surface = D3DCOLOR_ARGB( alpha, red, green, blue );
                    }

					// assign to DXGI surface
					DWORD *pdwDXGISurface = (DWORD*)( (UINT_PTR)DXGILockedRect.pBits +
														y * DXGILockedRect.Pitch + 
														 ( (UINT_PTR)x << 2 ) );
					*pdwDXGISurface = dwD3D9Surface;
				}
		}
			break;

		default:
			DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  Unsupported DXGI format.\n" );
			pDXGISurface->Unmap();
			pSurface->UnlockRect();
			return false;
	}

	if( FAILED( hr = pDXGISurface->Unmap() ) )
	{
		DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  WARNING:  Failed to unmap the dxgi surface.\n" );
	}

	if( FAILED( hr = pSurface->UnlockRect() ) )
	{
		DebugOut.Write( "clsD3D9Device::GetFrontBufferData:  WARNING:  Failed to unlock the d3d9 surface.\n" );
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////
// clsD3D10Device Methods

clsD3D10Device::clsD3D10Device(CTest *pInitLoggingGroup)
:
	pLoggingGroup( pInitLoggingGroup ),
	pD3D10Device( NULL ),
	m_RotationMode( DXGI_MODE_ROTATION_UNSPECIFIED ),
	pAuxiliarySprite( NULL ),
	m_bAlternateRender( false ),
	m_nxDivisions( 64 ),
	m_nyDivisions( 8 ),
	m_nNumVertices( 0 ),
	m_nNumIndixes( 0 ),
	m_nStride( 0 ),
	bReadyForTexRender( false ),
	m_pVB( NULL ),
	m_pIB( NULL ),
	m_pVSBuffer( NULL ),
	m_pEL( NULL ),
	m_pPixelShader( NULL ),
	m_pVertexShader( NULL ),
	m_pSampler( NULL ),
	m_pRastState( NULL ),
	m_pBlendState( NULL ),
	m_pDepthState( NULL )
{
};


clsD3D10Device::~clsD3D10Device()
{
	ReleaseAllObjects();
};


HRESULT clsD3D10Device::InitializeDevice
(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE hSoftware,
	UINT Flags,
	UINT32 SDKVersion,
	TEST_FEATURE_LEVEL TestFeatureLevel, // = TEST_FEATURE_10_0_LEVEL_10_0,
	PFN_D3D10_CREATE_DEVICE1 pD3D10CreateDevice1 // = NULL
)
{
	HRESULT hresult = E_FAIL;

	ReleaseAllObjects();


	//
	// Validate parameters.
	//

	if ( pD3D10Device != NULL )
	{
		if ( pLoggingGroup )
		{
			pLoggingGroup->WriteToLog
			(
				"clsD3D10Device::InitializeDevice:  pD3D10Device is not NULL; creating a device into this pointer would cause a leak."
			);
		};

		hresult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Get a pointer to the framework.
	//

	CDXGITestFramework * pFramework = (CDXGITestFramework *) GetFramework();

	if ( pFramework == NULL )
	{
		if ( pLoggingGroup )
		{
			pLoggingGroup->WriteToLog
			(
				"clsD3D10Device::InitializeDevice:  GetFramework returned NULL."
			);
		};

		hresult = E_FAIL;
		goto Cleanup;
	};


	//
	// Hand off to the framework's CreateDevice method.
	//

	hresult = pFramework->CreateDevice
	(
		pAdapter,
		DriverType,
		hSoftware,
		Flags,
		TestFeatureLevel,
		SDKVersion,
		&pD3D10Device,
		false
	);


Cleanup:
	return hresult;
};


HRESULT clsD3D10Device::CreateAuxiliarySprite()
{
	HRESULT hresult = S_OK;

	if (!pAuxiliarySprite)
		if (FAILED((hresult = D3DX10CreateSprite(pD3D10Device, 0, &pAuxiliarySprite))))
			if (pLoggingGroup)
				pLoggingGroup->WriteToLog("D3DX10CreateSprite failed in clsD3D10Device::CreateAuxiliarySprite.\n");

	return hresult;
}


HRESULT clsD3D10Device::LoadShader
(
	LPCSTR ShaderCode,
	__out void ** ppShader,
	LPCSTR pFuncName,
	LPCSTR pProfile,
	__out_opt ID3D10Blob ** ppRetainCompiledShader
)
{
	//
	// Initialize locals.
	//

	HRESULT hr = E_FAIL;
	ID3D10Blob * pBuf = NULL;
	ID3D10Blob * pErrorMessages = NULL;

	DWORD dwFlags = 0;


	//
	// Compile the shader.
	//

	HRESULT hDummyResult = S_OK;
	hr = D3DX10CompileFromMemory
	(
		ShaderCode,
		strlen( ShaderCode ),
		NULL,
		NULL,
		NULL, 
		pFuncName,
		pProfile,
		dwFlags,
		0,
		NULL,
		&pBuf,
		&pErrorMessages,
		&hDummyResult
	);

	if ( FAILED( hr ) )
	{
		if ( pLoggingGroup )
		{
			pLoggingGroup->WriteToLog
			(
				_T( "LoadShader:  D3D10CompileShader failed with error 0x%x and reported the following error:  %s." ),
				hr,
				(
					pErrorMessages != NULL &&
					pErrorMessages->GetBufferSize() > 0
				)
					? pErrorMessages->GetBufferPointer()
					:  _T( "[none]" )
			);
		};

		if
		(
			pErrorMessages != NULL &&
			pErrorMessages->GetBufferSize() > 0
		)
		{
			DebugOut.Write( "D3D10CompileShader reports the following error:\n" );
			DebugOut.Write( (LPCTSTR) pErrorMessages->GetBufferPointer() );
			DebugOut.Write( "\n" );
		};

		goto Cleanup;
	};


	if ( pErrorMessages != NULL )
	{
		pErrorMessages->Release();
		pErrorMessages = NULL;
	};


	if
	(
		strcmp(pProfile, "vs_4_0") == 0 ||
		strcmp(pProfile, "vs_4_0_level_9_1") == 0 ||
		strcmp(pProfile, "vs_4_0_level_9_3") == 0
	)
	{
		hr = pD3D10Device->CreateVertexShader
		(
			(DWORD *) pBuf->GetBufferPointer(),
			pBuf->GetBufferSize(),
			(ID3D10VertexShader**)ppShader
		);
	}
	else if
	(
		strcmp(pProfile, "gs_4_0") == 0
	)
	{
		hr = pD3D10Device->CreateGeometryShader
		(
			(DWORD *) pBuf->GetBufferPointer(),
			pBuf->GetBufferSize(),
			(ID3D10GeometryShader**) ppShader
		);
	}
	else if
	(
		strcmp(pProfile, "ps_4_0") == 0 ||
		strcmp(pProfile, "ps_4_0_level_9_1") == 0 ||
		strcmp(pProfile, "ps_4_0_level_9_3") == 0
	)
	{
		hr = pD3D10Device->CreatePixelShader
		(
			(DWORD *) pBuf->GetBufferPointer(),
			pBuf->GetBufferSize(),
			(ID3D10PixelShader**) ppShader
		);
	}
	else
	{
		hr = E_INVALIDARG;
	};


Cleanup:
	if ( ppRetainCompiledShader )
	{
		*ppRetainCompiledShader = pBuf;
	}
	else
	{
		if ( pBuf != NULL )
		{
			pBuf->Release();
			pBuf = NULL;
		};
	};

	if ( pErrorMessages != NULL )
	{
		pErrorMessages->Release();
		pErrorMessages = NULL;
	};


	return hr;
};


bool clsD3D10Device::CreateTexRenderResources
(
	LPCSTR VertexShaderModel,
	LPCSTR PixelShaderModel
)
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;
	Vtx verts[ 4 ];
	WORD indices[ 4 ];
	HRESULT hr = S_OK;
	D3DXMATRIX matrix;
	ID3D10Blob * pCode = NULL;


	//
	// Release any currently-held resources.
	//

	ReleaseTexRenderResources();


	if ( m_bAlternateRender )
	{
		//
		// Calculate number of vertixes and indixes.
		//

		m_nNumVertices = 6 * m_nxDivisions * m_nyDivisions;
		m_nNumIndixes = 6 * m_nxDivisions * m_nyDivisions;
		m_nStride = 12 + 4;


		//
		// Create the vertex buffer.
		//

		D3D10_BUFFER_DESC DescBuffer;
		ZeroMemory( &DescBuffer, sizeof( D3D10_BUFFER_DESC ) );

		DescBuffer.ByteWidth = m_nStride * m_nNumVertices;
		DescBuffer.Usage = D3D10_USAGE_DEFAULT;
		DescBuffer.BindFlags = D3D10_BIND_VERTEX_BUFFER;

		hr = pD3D10Device->CreateBuffer
		(
			&DescBuffer,
			NULL,
			&m_pVB
		);

		if ( FAILED( hr ) )
		{
			if ( pLoggingGroup )
			{
				pLoggingGroup->WriteToLog
				(
					"CreateBuffer on vertex buffer failed with error 0x%x.",
					hr
				);
			};

			goto Cleanup;
		};


		//
		// Create the index buffer.
		//

		DescBuffer.ByteWidth = m_nNumIndixes * sizeof( WORD );
		DescBuffer.BindFlags = D3D10_BIND_INDEX_BUFFER;

		hr = pD3D10Device->CreateBuffer
		(
			&DescBuffer,
			NULL,
			&m_pIB
		);

		if ( FAILED( hr ) )
		{
			if ( pLoggingGroup )
			{
				pLoggingGroup->WriteToLog
				(
					"CreateBuffer on index buffer failed with error 0x%x.",
					hr
				);
			};

			goto Cleanup;
		};


		//
		// Create the pixel shader.
		//

		char PixelShader[] =
			"struct PS_INPUT								\n"
			"{												\n"
			"	float4 Position :  SV_Position;				\n"
			"	linear float4 Color :  Color;				\n"
			"};												\n"
			"float4 PS( PS_INPUT In ) :  SV_Target			\n"
			"{												\n"
			"	return In.Color;							\n"
			"}												\n";
		
		hr = LoadShader
		(
			PixelShader,
			(void**) &m_pPixelShader,
			"PS",
			PixelShaderModel,
			NULL
		);

		if ( FAILED( hr ) )
		{
			if ( pLoggingGroup )
			{
				pLoggingGroup->WriteToLog
				(
					"clsD3D10Device::CreateTexRenderResources:  LoadShader on pixel shader failed with error 0x%x.",
					hr
				);
			};

			goto Cleanup;
		};


		//
		// Create the vertex shader.
		//

		char VertexShader[] =
			"cbuffer cb0 :  register(b0)						\n"
			"{												\n"
			"	float4x4 modelViewProj :  WORLDVIEWPROJ;		\n"
			"};												\n"
			"struct VS_INPUT								\n"
			"{												\n"
			"	float3 Position :  Position;					\n"
			"	float4 Color :  Color;						\n"
			"};												\n"
			"struct VS_OUTPUT								\n"
			"{												\n"
			"	float4 Position :  SV_Position;				\n"
			"	linear float4 Color :  Color;				\n"
			"};												\n"
			"VS_OUTPUT VS( in VS_INPUT In )					\n"
			"{												\n"
			"	VS_OUTPUT Out = (VS_OUTPUT)0;				\n"
			"	Out.Position = mul( modelViewProj, float4( In.Position, 1.0f ) );	\n"
			"	Out.Color = In.Color;						\n"
			"	return Out;									\n"
			"}												\n";
		
		hr = LoadShader
		(
			VertexShader,
			(void**) &m_pVertexShader,
			"VS",
			VertexShaderModel,
			&pCode
		);

		if ( FAILED( hr ) )
		{
			if ( pLoggingGroup )
			{
				pLoggingGroup->WriteToLog
				(
					"clsD3D10Device::CreateTexRenderResources:  LoadShader on vertex shader failed with error 0x%x.",
					hr
				);
			};

			goto Cleanup;
		};


		//
		// Create the input layout.
		//

		const NumVertexElements = 2;
		D3D10_INPUT_ELEMENT_DESC Desc[NumVertexElements];
		ZeroMemory( &Desc, NumVertexElements * sizeof( D3D10_INPUT_ELEMENT_DESC ) );

		Desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		Desc[0].SemanticName = "Position";
		Desc[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;

		Desc[1].AlignedByteOffset = 12;
		Desc[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Desc[1].SemanticName = "Color";
		Desc[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;

		hr = pD3D10Device->CreateInputLayout
		(
			Desc,
			NumVertexElements,
			pCode->GetBufferPointer(),
			pCode->GetBufferSize(),
			&m_pEL
		);

		if ( FAILED( hr ) )
		{
			if ( pLoggingGroup )
			{
				pLoggingGroup->WriteToLog
				(
					"CreateInputLayout failed with error 0x%x.",
					hr
				);
			};

			goto Cleanup;
		};


		pCode->Release();
		pCode = NULL;


		//
		// Populate the vertex buffer.
		//

		if ( NULL != m_pVB )
		{
			D3D10_BUFFER_DESC bufferDesc;
			m_pVB->GetDesc( &bufferDesc );
			bufferDesc.Usage = D3D10_USAGE_STAGING;
			bufferDesc.BindFlags = 0;
			bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;

			ID3D10Buffer * pStagingBuffer = NULL;
			hr = pD3D10Device->CreateBuffer
			(
				&bufferDesc,
				NULL,
				&pStagingBuffer
			);

			if ( FAILED( hr ) )
			{
				if ( pLoggingGroup )
				{
					pLoggingGroup->WriteToLog
					(
						"CreateBuffer on vertex staging buffer failed with error 0x%x.",
						hr
					);
				};

				goto Cleanup;
			};

			BYTE *pVertixes;
			hr = pStagingBuffer->Map( D3D10_MAP_READ_WRITE, 0, (void**) &pVertixes );
			if( FAILED( hr ) )
				return false;

			// call the derived class to fill the buffer

			// arrange the grids in x-y matrix, fill the x axis first
			int nGridNoSquareRoot = 1;
			int nxGrids = 1;
			int nyGrids = 1;
			
			float yDim	= 2.0f / (float)nxGrids;
			float xDim	= 2.0f / (float)nyGrids;
			float yOffset = ( 0 / nGridNoSquareRoot ) * yDim - 1.0f;
			float xOffset = ( 0 % nGridNoSquareRoot ) * xDim - 1.0f;
			
			// fdx = width of one square divided by 2 (length and depth = 2)
			// UNUSED:  float fdx = 2.0f * 1.0f / (float)m_nxDivisions;
			// UNUSED:  float fdy = 2.0f * 1.0f / (float)m_nyDivisions;
			
			srand( 666 );
		
			for( UINT y = 0; y < m_nyDivisions; y++ )
				for( UINT x = 0; x < m_nxDivisions;  x++ )
				{
					DWORD dwColor = D3DCOLOR_ARGB((BYTE)( rand() % 256 ),
													(BYTE)( rand() % 256 ),
													(BYTE)( rand() % 256 ),
													(BYTE)( rand() % 256 ) );
					DWORD dwColor2 = D3DCOLOR_ARGB((BYTE)( rand() % 256 ),
													(BYTE)( rand() % 256 ),
													(BYTE)( rand() % 256 ),
													(BYTE)( rand() % 256 ) );

					UINT vecX[] = { x, x + 1, x, x, x + 1, x + 1 };
					UINT vecY[] = { y, y, y + 1, y + 1, y, y + 1 };

					for( UINT uIndex = 0; uIndex < 6; uIndex++ )
					{
						D3DXVECTOR3 *ppositionVertixes = (D3DXVECTOR3*) ( pVertixes + 0 );
						DWORD *pcolorVertixes = (DWORD*) ( pVertixes + 12 );

						ppositionVertixes->x = xDim * ( vecX[uIndex] ) / (float)m_nxDivisions + xOffset;
						ppositionVertixes->y = yDim * ( vecY[uIndex] ) / (float)m_nyDivisions + yOffset;
						ppositionVertixes->z = 0.5f;

						*pcolorVertixes = uIndex < 3? dwColor :  dwColor2;

						pVertixes += m_nStride;
					}
				}

			pStagingBuffer->Unmap();

			pD3D10Device->CopyResource( m_pVB, pStagingBuffer );

			pStagingBuffer->Release();
			pStagingBuffer = NULL;
		};


		//
		// Populate the index buffer.
		//

		if ( m_pIB != NULL )
		{
			//
			// Create an index staging buffer.
			//

			D3D10_BUFFER_DESC bufferDesc;
			m_pIB->GetDesc( &bufferDesc );
			bufferDesc.Usage = D3D10_USAGE_STAGING;
			bufferDesc.BindFlags = 0;
			bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;

			ID3D10Buffer * pStagingBuffer = NULL;
			hr = pD3D10Device->CreateBuffer
			(
				&bufferDesc,
				NULL,
				&pStagingBuffer
			);

			if ( FAILED( hr ) )
			{
				if ( pLoggingGroup )
				{
					pLoggingGroup->WriteToLog
					(
						"CreateBuffer on index staging buffer failed with error 0x%x.",
						hr
					);
				};

				goto Cleanup;
			};

			WORD * pIndices = NULL;
			hr = pStagingBuffer->Map
			(
				D3D10_MAP_READ_WRITE,
				0,
				(void**) &pIndices
			);

			if( FAILED( hr ) )
			{
				pLoggingGroup->WriteToLog
				(
					"Map on staging buffer failed with error 0x%x.",
					hr
				);

				goto Cleanup;
			};

			// call the derived class to fill the buffer
			for
			(
				UINT uIndex = 0;
				uIndex < 6 * m_nxDivisions * m_nyDivisions;
				++uIndex
			)
			{
				*pIndices = (WORD) uIndex;
				++pIndices;
			};

			pStagingBuffer->Unmap();

			pD3D10Device->CopyResource
			(
				m_pIB,
				pStagingBuffer
			);

			pStagingBuffer->Release();
			pStagingBuffer = NULL;
		};
	}
	else
	{
		//
		// Set up the arrays used to initialize the vertex and index buffers.
		//

		m_nNumVertices = 4;
		m_nNumIndixes = 4;

		// a quad
		// ------------------------------
		verts[0].Position.x = -1.0f;
		verts[0].Position.y = 1.0f;
		verts[0].Position.z = 1.0f;
		verts[0].Position.w = 1.0f;

		//verts[0].Texcoord.u = 2.5f;
		//verts[0].Texcoord.v = 2.5f;

		verts[0].Texcoord.u = 0.0f;
		verts[0].Texcoord.v = 0.0f;

		//verts[0].Texcoord2.u = 0.0f;
		//verts[0].Texcoord2.v = 0.0f;
		//verts[0].Texcoord2.w = 0.0f;

		// ------------------------------
		verts[1].Position.x = 1.0f;
		verts[1].Position.y = 1.0f;
		verts[1].Position.z = 1.0f;
		verts[1].Position.w = 1.0f;

		//verts[1].Texcoord.u = -1.5f;
		//verts[1].Texcoord.v = 2.5f;

		verts[1].Texcoord.u = 1.0f;
		verts[1].Texcoord.v = 0.0f;

		//verts[1].Texcoord2.u = 0.0f;
		//verts[1].Texcoord2.v = 0.0f;
		//verts[1].Texcoord2.w = 0.0f;

		// ------------------------------
		verts[2].Position.x = -1.0f;
		verts[2].Position.y = -1.0f;
		verts[2].Position.z = 1.0f;
		verts[2].Position.w = 1.0f;

		//verts[2].Texcoord.u = 2.5f;
		//verts[2].Texcoord.v = -1.5f;

		verts[2].Texcoord.u = 0.0f;
		verts[2].Texcoord.v = 1.0f;

		//verts[2].Texcoord2.u = 0.0f;
		//verts[2].Texcoord2.v = 0.0f;
		//verts[2].Texcoord2.w = 0.0f;

		// ------------------------------
		verts[3].Position.x = 1.0f;
		verts[3].Position.y = -1.0f;
		verts[3].Position.z = 1.0f;
		verts[3].Position.w = 1.0f;

		//verts[3].Texcoord.u = -1.5f;
		//verts[3].Texcoord.v = -1.5f;

		verts[3].Texcoord.u = 1.0f;
		verts[3].Texcoord.v = 1.0f;

		//verts[3].Texcoord2.u = 0.0f;
		//verts[3].Texcoord2.v = 0.0f;
		//verts[3].Texcoord2.w = 0.0f;

		// ------------------------------

		indices[ 0 ] = 0;
		indices[ 1 ] = 1;
		indices[ 2 ] = 2;
		indices[ 3 ] = 3;


		//
		// Create and initialize the vertex buffer.
		//

		m_nStride = sizeof( Vtx );

		{
			D3D10_SUBRESOURCE_DATA InitialData;
			D3D10_BUFFER_DESC DescBuffer;

			InitialData.pSysMem = verts;
			InitialData.SysMemPitch = 0;
			InitialData.SysMemSlicePitch = 0;

			DescBuffer.ByteWidth = m_nStride * m_nNumVertices;
			DescBuffer.Usage = D3D10_USAGE_IMMUTABLE;
			DescBuffer.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			DescBuffer.CPUAccessFlags = 0;
			DescBuffer.MiscFlags = 0;

			hr = pD3D10Device->CreateBuffer
			(
				&DescBuffer,
				&InitialData,
				&m_pVB
			);

			if ( FAILED( hr ) )
			{
				if ( pLoggingGroup )
				{
					pLoggingGroup->WriteToLog
					(
						"CreateBuffer on vertex buffer failed with error 0x%x.",
						hr
					);
				};

				goto Cleanup;
			};
		}


		//
		// Create and initialize the index buffer.
		//

		{
			D3D10_SUBRESOURCE_DATA InitialData;
			D3D10_BUFFER_DESC DescBuffer;

			InitialData.pSysMem = indices;
			InitialData.SysMemPitch = 0;
			InitialData.SysMemSlicePitch = 0;

			DescBuffer.ByteWidth = m_nNumIndixes * sizeof( WORD );
			DescBuffer.Usage = D3D10_USAGE_IMMUTABLE;
			DescBuffer.BindFlags = D3D10_BIND_INDEX_BUFFER;
			DescBuffer.CPUAccessFlags = 0;
			DescBuffer.MiscFlags = 0;

			hr = pD3D10Device->CreateBuffer
			(
				&DescBuffer,
				&InitialData,
				&m_pIB
			);

			if ( FAILED( hr ) )
			{
				if ( pLoggingGroup )
				{
					pLoggingGroup->WriteToLog
					(
						"CreateBuffer on index buffer failed with error 0x%x.",
						hr
					);
				};

				goto Cleanup;
			};
		}


		//
		// Create the pixel shader.
		//

		char PixelShader[] =
			"struct PS_INPUT								\n"
			"{												\n"
			"	float4 Position :  SV_Position;				\n"
			"	linear float2 Texcoord :  Texcoord;			\n"
			"};												\n"
			"struct PS_OUTPUT								\n"
			"{												\n"
			"	float4 Color :  SV_Target;					\n"
			"};												\n"
			"sampler defaultSampler :  register(s0);			\n"
			"Texture2D myTex :  register(t0);				\n"
			"PS_OUTPUT main( in PS_INPUT In )				\n"
			"{												\n"
			"	PS_OUTPUT Out = (PS_OUTPUT)0;				\n"
			"	Out.Color = myTex.Sample(defaultSampler, In.Texcoord); \n"
			"	return Out;									\n"
			"}												\n";

	// output.Color = myTex.Sample(defaultSampler, input.Texcoord); \n"
	// output.Color = float4(0.0f, 1.0f, 0.0f, 1.0f); \n

		hr = LoadShader
		(
			PixelShader,
			(void**) &m_pPixelShader,
			"main",
			PixelShaderModel,
			NULL
		);

		if ( FAILED( hr ) )
		{
			if ( pLoggingGroup )
			{
				pLoggingGroup->WriteToLog
				(
					"clsD3D10Device::CreateTexRenderResources:  LoadShader on pixel shader failed with error 0x%x.",
					hr
				);
			};

			goto Cleanup;
		};


		//
		// Create the vertex shader.
		//

		char VertexShader[] =
			"cbuffer cb0 :  register(b0)						\n"
			"{												\n"
			"	float4x4 modelViewProj :  WORLDVIEWPROJ;		\n"
			"};												\n"
			"struct VS_INPUT								\n"
			"{												\n"
			"	float3 Position :  Position;					\n"
			"	float2 Texcoord :  Texcoord;					\n"
			//"	float3 Texcoord2 :  Texcoord2;				\n"
			"};												\n"
			"struct VS_OUTPUT								\n"
			"{												\n"
			"	linear float4 Position :  SV_Position;		\n"
			"	linear float2 Texcoord :  Texcoord;			\n"
//			"	linear float3 Texcoord2 :  Texcoord2;	\n"
			"};											 \n"
			"VS_OUTPUT main( in VS_INPUT In )				\n"
			"{												\n"
			"	VS_OUTPUT Out = (VS_OUTPUT)0;				\n"
			"	Out.Position = mul( modelViewProj, float4( In.Position, 1.0f ) );	\n"
			"	Out.Texcoord = In.Texcoord;					\n"
			"	return Out;									\n"
			"}												\n";

		hr = LoadShader
		(
			VertexShader,
			(void**) &m_pVertexShader,
			"main",
			VertexShaderModel,
			&pCode
		);

		if ( FAILED( hr ) )
		{
			if ( pLoggingGroup )
			{
				pLoggingGroup->WriteToLog
				(
					"clsD3D10Device::CreateTexRenderResources:  LoadShader on vertex shader failed with error 0x%x.",
					hr
				);
			};

			goto Cleanup;
		};


		//
		// Create the input layout.
		//

		const NumVertexElements = 2;
		D3D10_INPUT_ELEMENT_DESC Desc[NumVertexElements];
		Desc[0].InputSlot = 0;
		Desc[0].AlignedByteOffset = 0;
		Desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		Desc[0].SemanticName = "Position";
		Desc[0].SemanticIndex = 0;
		Desc[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		Desc[0].InstanceDataStepRate = 0;

		Desc[1].InputSlot = 0;
		Desc[1].AlignedByteOffset = 16;
		Desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		Desc[1].SemanticName = "Texcoord";
		Desc[1].SemanticIndex = 0;
		Desc[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		Desc[1].InstanceDataStepRate = 0;

		/*
		Desc[2].InputSlot = 0;
		Desc[2].AlignedByteOffset = 24;
		Desc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		Desc[2].SemanticName = "Texcoord2";
		Desc[2].SemanticIndex = 0;
		Desc[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		Desc[2].InstanceDataStepRate = 0;
		*/

		hr = pD3D10Device->CreateInputLayout
		(
			Desc,
			NumVertexElements,
			pCode->GetBufferPointer(),
			pCode->GetBufferSize(),
			&m_pEL
		);

		pCode->Release();
		pCode = NULL;

		if ( FAILED( hr ) )
		{
			if ( pLoggingGroup )
			{
				pLoggingGroup->WriteToLog
				(
					"CreateInputLayout failed with error 0x%x.",
					hr
				);
			};

			goto Cleanup;
		};
	};


	//
	// Set the vertex shader resources.
	//

	D3D10_BUFFER_DESC DescBuffer;
	ZeroMemory( &DescBuffer, sizeof( D3D10_BUFFER_DESC ) );
	DescBuffer.ByteWidth = 64;
	DescBuffer.Usage = D3D10_USAGE_DEFAULT;
	DescBuffer.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		
	switch( m_RotationMode )
	{
		case DXGI_MODE_ROTATION_UNSPECIFIED :
		case DXGI_MODE_ROTATION_IDENTITY :
			D3DXMatrixIdentity( &matrix );
		break;

		case DXGI_MODE_ROTATION_ROTATE90 :
			D3DXMatrixRotationZ( &matrix, (float)D3DX_PI / 2.0f );
		break;

		case DXGI_MODE_ROTATION_ROTATE180 :
			D3DXMatrixRotationZ( &matrix, (float)D3DX_PI );
		break;

		case DXGI_MODE_ROTATION_ROTATE270 :
			D3DXMatrixRotationZ( &matrix, -(float)D3DX_PI / 2.0f );
		break;

		default:
		{
			assert( FALSE );
		} break;
	};

	D3D10_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem = &matrix;
	InitialData.SysMemPitch = 0;
	InitialData.SysMemSlicePitch = 0;

	hr = pD3D10Device->CreateBuffer
	(
		&DescBuffer,
		&InitialData,
		&m_pVSBuffer
	);

	if( FAILED( hr ) )
	{
		if ( pLoggingGroup )
		{
			pLoggingGroup->WriteToLog
			(
				"CreateBuffer failed with error 0x%x.",
				hr
			);
		};

		goto Cleanup;
	};

	pD3D10Device->VSSetConstantBuffers
	(
		0,
		1,
		&m_pVSBuffer
	);


	//
	// Create our sampler state object.
	//

	D3D10_SAMPLER_DESC SamplerDesc;
	ZeroMemory( &SamplerDesc, sizeof( D3D10_SAMPLER_DESC ) );
	SamplerDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D10_TEXTURE_ADDRESS_BORDER;
	SamplerDesc.MipLODBias = 0;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D10_COMPARISON_NEVER;
	SamplerDesc.BorderColor[0] = 1.0f;
	SamplerDesc.BorderColor[1] = 1.0f;
	SamplerDesc.BorderColor[2] = 1.0f;
	SamplerDesc.BorderColor[3] = 1.0f;

	SamplerDesc.MinLOD = -FLT_MAX;
	if
	(
		strcmp( VertexShaderModel, "vs_4_0_level_9_1" ) == 0 ||
		strcmp( VertexShaderModel, "vs_4_0_level_9_3" ) == 0
	)
	{
		SamplerDesc.MinLOD = 0.f;
	};

	SamplerDesc.MaxLOD = FLT_MAX;

	hr = pD3D10Device->CreateSamplerState
	(
		&SamplerDesc,
		&m_pSampler
	);

	if ( FAILED( hr ) )
	{
		if ( pLoggingGroup )
		{
			pLoggingGroup->WriteToLog
			(
				"CreateSamplerState failed with error 0x%x.",
				hr
			);
		};

		goto Cleanup;
	};


	//
	// Get prior states.
	//
	//pD3D10Device ->RSGetState(&m_pRastState);
	//pD3D10Device->OMGetBlendState(&m_pBlendState, NULL, NULL);
	//pD3D10Device->OMGetDepthStencilState(&m_pDepthState, NULL);


	//
	// Create our rasterization state object.
	//

	D3D10_RASTERIZER_DESC Rasterizer;
	ZeroMemory( &Rasterizer, sizeof( D3D10_RASTERIZER_DESC ) );
	Rasterizer.AntialiasedLineEnable = false;
	Rasterizer.CullMode = D3D10_CULL_NONE;
	Rasterizer.DepthBias = 0;
	Rasterizer.DepthBiasClamp = 0;
	Rasterizer.FillMode = D3D10_FILL_SOLID;
	Rasterizer.FrontCounterClockwise = false;
	Rasterizer.MultisampleEnable = true;
	Rasterizer.ScissorEnable = false;
	Rasterizer.SlopeScaledDepthBias = 0;
	Rasterizer.DepthClipEnable = true;

	hr = pD3D10Device->CreateRasterizerState
	(
		&Rasterizer,
		&m_pRastState
	);

	if ( FAILED( hr ) )
	{
		if ( pLoggingGroup )
		{
			pLoggingGroup->WriteToLog
			(
				"CreateRasterizerState failed with error 0x%x.",
				hr
			);
		};

		goto Cleanup;
	};


	//
	// Create our blend state object.
	//

	D3D10_BLEND_DESC Blend;
	ZeroMemory( &Blend, sizeof( D3D10_BLEND_DESC ) );
	Blend.AlphaToCoverageEnable = false;
	Blend.BlendOp = D3D10_BLEND_OP_ADD;
	Blend.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	Blend.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	Blend.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
	Blend.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	Blend.SrcBlendAlpha = D3D10_BLEND_SRC_ALPHA;
	Blend.RenderTargetWriteMask[0] = 0xf;
	Blend.RenderTargetWriteMask[1] = 0xf;
	Blend.RenderTargetWriteMask[2] = 0xf;
	Blend.RenderTargetWriteMask[3] = 0xf;
	Blend.RenderTargetWriteMask[4] = 0xf;
	Blend.RenderTargetWriteMask[5] = 0xf;
	Blend.RenderTargetWriteMask[6] = 0xf;
	Blend.RenderTargetWriteMask[7] = 0xf;

	hr = pD3D10Device->CreateBlendState
	(
		&Blend,
		&m_pBlendState
	);

	if ( FAILED( hr ) )
	{
		if ( pLoggingGroup )
		{
			pLoggingGroup->WriteToLog
			(
				"CreateBlendState failed with error 0x%x.",
				hr
			);
		};

		goto Cleanup;
	};


	//
	// Create our depth stencil states.
	//

	D3D10_DEPTH_STENCIL_DESC Depth;
	ZeroMemory( &Depth, sizeof( D3D10_DEPTH_STENCIL_DESC ) );
	Depth.DepthEnable = false;
	Depth.DepthFunc = D3D10_COMPARISON_LESS;
	Depth.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
	Depth.StencilEnable = false;
	Depth.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	Depth.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	Depth.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	Depth.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
	Depth.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
	Depth.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
	Depth.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	Depth.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
	Depth.StencilReadMask = 0;
	Depth.StencilWriteMask = 0;

	hr = pD3D10Device->CreateDepthStencilState
	(
		&Depth,
		&m_pDepthState
	);

	if ( FAILED( hr ) )
	{
		if ( pLoggingGroup )
		{
			pLoggingGroup->WriteToLog
			(
				"CreateDepthStencilState failed with error 0x%x.",
				hr
			);
		};

		goto Cleanup;
	};


	bReadyForTexRender = true;
	bOverallResult = true;


Cleanup:
	if ( pCode != NULL )
	{
		pCode->Release();
		pCode = NULL;
	};


	return bOverallResult;
};


void clsD3D10Device::ReleaseTexRenderResources()
{
	bReadyForTexRender = false;

	if ( m_pVB != NULL )
	{
		if ( pD3D10Device != NULL )
		{
			// First unbind it...
			UINT Strides[] = { 0 };
			UINT Offsets[] = { 0 };
			ID3D10Buffer * pNullBuffer = NULL;
			pD3D10Device->IASetVertexBuffers
			(
				0,
				1,
				&pNullBuffer,
				Strides,
				Offsets
			);
		};

		// ...then release it.
		m_pVB->Release();
		m_pVB = NULL;
	};


	if ( m_pIB != NULL )
	{
		if ( pD3D10Device != NULL )
		{
			// First unbind it...
			pD3D10Device->IASetIndexBuffer
			(
				NULL,
				DXGI_FORMAT_UNKNOWN,
				0
			);
		};

		// ...then release it.
		m_pIB->Release();
		m_pIB = NULL;
	};


	if ( m_pVSBuffer != NULL )
	{
		if ( pD3D10Device != NULL )
		{
			// First unbind it...
			ID3D10Buffer * pNullBuffer = NULL;
			pD3D10Device->VSSetConstantBuffers
			(
				0,
				1,
				&pNullBuffer
			);
		};

		// ...then release it.
		m_pVSBuffer->Release();
		m_pVSBuffer = NULL;
	};


	if ( m_pEL != NULL )
	{
		if ( pD3D10Device != NULL )
		{
			// First unbind it...
			pD3D10Device->IASetInputLayout( NULL );
		};

		// ...then release it.
		m_pEL->Release();
		m_pEL = NULL;
	};


	if ( m_pPixelShader != NULL )
	{
		if ( pD3D10Device != NULL )
		{
			// First unbind it...
			pD3D10Device->PSSetShader( NULL );
		};

		// ...then release it.
		m_pPixelShader->Release();
		m_pPixelShader = NULL;
	};


	if ( m_pVertexShader != NULL )
	{
		if ( pD3D10Device != NULL )
		{
			// First unbind it...
			pD3D10Device->VSSetShader( NULL );
		};

		// ...then release it.
		m_pVertexShader->Release();
		m_pVertexShader = NULL;
	};


	if ( m_pSampler != NULL )
	{
		if ( pD3D10Device != NULL )
		{
			// First unbind it...
			ID3D10SamplerState * pNullSampler = NULL;
			pD3D10Device->PSSetSamplers
			(
				0,
				1,
				&pNullSampler
			);
		};

		// ...then release it.
		m_pSampler->Release();
		m_pSampler = NULL;
	};


	if ( m_pRastState != NULL )
	{
		if ( pD3D10Device != NULL )
		{
			// First unbind it...
			pD3D10Device->RSSetState( NULL );
		};

		// ...then release it.
		m_pRastState->Release();
		m_pRastState = NULL;
	};


	if ( m_pBlendState != NULL )
	{
		m_pBlendState->Release();
		m_pBlendState = NULL;
	};


	if ( m_pDepthState != NULL )
	{
		if ( pD3D10Device != NULL )
		{
			// First unbind it...
			pD3D10Device->OMSetDepthStencilState
			(
				NULL,
				0
			);
		};

		// ...then release it.
		m_pDepthState->Release();
		m_pDepthState = NULL;
	};
};


bool clsD3D10Device::RenderTexture
(
	ID3D10RenderTargetView *pRTView,
	ID3D10ShaderResourceView *pTexSRV,
	const D3D10_VIEWPORT *pD3D10ViewPort
)
{
	// Verify that the necessary resources (VBs, shaders, etc.) have been initialized.
	if( !bReadyForTexRender )
	{
		if( pLoggingGroup )
			pLoggingGroup->WriteToLog( "Aborting clsD3D10Device::SetBBInitDeviceStates:  Resources not initialized." );
		return false;
	}

	// Set the device state.

	{
		UINT Offset = 0;

		pD3D10Device->IASetVertexBuffers( 0, 1, &m_pVB, &m_nStride, &Offset );
	}

	pD3D10Device->IASetIndexBuffer
	(
		m_pIB,
		DXGI_FORMAT_R16_UINT,
		0
	);
	pD3D10Device->PSSetShader( m_pPixelShader );
	pD3D10Device->VSSetShader( m_pVertexShader );
	pD3D10Device->IASetInputLayout( m_pEL );
	pD3D10Device->IASetPrimitiveTopology
	(
		m_bAlternateRender
		? D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST
		:  D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	);

	if ( !m_bAlternateRender )
	{
		pD3D10Device->PSSetSamplers
		(
			0,
			1,
			&m_pSampler
		);

		// Assign the texture's shader resource view to the zero register.
		pD3D10Device->PSSetShaderResources( 0, 1, &pTexSRV );
	};

	pD3D10Device->RSSetState( m_pRastState );

	float aBlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };  

//#pragma prefast( suppress:6309, "NULL is valid for parameter 2 - SAL needs updating for OMSetBlendState() in d3d10.h" )
	pD3D10Device->OMSetBlendState
	(
		m_pBlendState,
		aBlendFactor,
		D3D10_DEFAULT_SAMPLE_MASK
	);

	pD3D10Device->OMSetDepthStencilState( m_pDepthState, 0 );

	// Set the render target and depth stencil.
	pD3D10Device->OMSetRenderTargets
	(
		1,
		&pRTView,
		NULL
	);

	// Set the view port.
	pD3D10Device->RSSetViewports
	(
		1,
		pD3D10ViewPort
	);

	// Render the texture.
	if( m_bAlternateRender )
	{
		pD3D10Device->DrawIndexed
		(
			m_nNumIndixes,
			0,
			0
		);
	}
	else
	{
		pD3D10Device->Draw
		(
			m_nNumVertices,
			0
		);
	};

	return true;
};


bool clsD3D10Device::GetFactory
(
	IDXGIFactory **pFactoryPointer
)
{
	HRESULT hresult;
	bool bReturn = false;

	if (!pD3D10Device)
	{
		if (pLoggingGroup)
			pLoggingGroup->WriteToLog("clsD3D10Device::GetFactory:  Device not initialized.");
	}
	else
	{
		IDXGIDevice *pDXGIDevice = NULL;

		if (FAILED((hresult = pD3D10Device->QueryInterface(__uuidof(IDXGIDevice), (void **)(&pDXGIDevice)))))
		{
			if (pLoggingGroup)
				pLoggingGroup->WriteToLog("clsD3D10Device::GetFactory:  Unable to retrieve IDXGIDevice interface from D3D10 device.");
		}
		else
		{
			IDXGIAdapter *pDeviceAdapter = NULL;

			if (FAILED((hresult = pDXGIDevice->GetAdapter(&pDeviceAdapter))))
			{
				if (pLoggingGroup)
					pLoggingGroup->WriteToLog("clsD3D10Device::GetFactory:  GetAdapter failed with error 0x%x.", hresult);
			}
			else
			{
				if (SUCCEEDED((hresult = pDeviceAdapter->GetParent(__uuidof(IDXGIFactory), (void **)pFactoryPointer))))
					bReturn = true;
				else
					if (pLoggingGroup)
						pLoggingGroup->WriteToLog("clsD3D10Device::GetFactory:  GetParent failed with error 0x%x.", hresult);

				pDeviceAdapter->Release();
			}

			pDXGIDevice->Release();
		}
	}

	return bReturn;
}


BOOL  clsD3D10Device::GetDevicePtr
(
	ID3D10Device **pDevicePointer
)
{
	if ( pDevicePointer == NULL )
	{
		return FALSE;
	}

	*pDevicePointer = NULL;

	if ( pD3D10Device == NULL )
	{
		return FALSE;
	}

	pD3D10Device->AddRef();
	*pDevicePointer = pD3D10Device;

	return TRUE;
}

BOOL  clsD3D10Device::SetDevicePtr
(
	__in ID3D10Device *pD3DDevice
)
{
	if ( pD3DDevice == NULL )
	{
		return FALSE;
	}

	pD3DDevice->AddRef();
	pD3D10Device = pD3DDevice;

	return TRUE;
}

void clsD3D10Device::ReleaseAllObjects()
{
	ReleaseTexRenderResources();

	if (pAuxiliarySprite)
	{
		pAuxiliarySprite->Release();
		pAuxiliarySprite = NULL;
	}

	if (pD3D10Device)
	{
		UINT uiRefCount;
		
		if (uiRefCount = pD3D10Device->Release())
			if (pLoggingGroup)
				pLoggingGroup->WriteToLog("Warning:  Non-zero reference count after releasing device in clsD3D10Device::ReleaseAllObjects.");

		pD3D10Device = NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////
// clsDXGISwapChain Methods

clsDXGISwapChain::~clsDXGISwapChain()
{
	ReleaseAllObjects();
}

// original DXGI 1.0 test usage
BOOL clsDXGISwapChain::InitializeSwapChain
(
	DXGI_SWAP_CHAIN_DESC * pSwapChainDesc
)
{
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = CopyToSwapChainDesc1(*pSwapChainDesc, &fullscreenDesc);

	return InitializeSwapChainImpl(pSwapChainDesc->OutputWindow, &swapChainDesc1, pSwapChainDesc->Windowed ? NULL :  &fullscreenDesc, DXGI_VERSION_1_0);
}

// useful for tests that want to hit new interface path without changing descs in test
BOOL clsDXGISwapChain::InitializeSwapChain1
(
	DXGI_SWAP_CHAIN_DESC * pSwapChainDesc
)
{
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = CopyToSwapChainDesc1(*pSwapChainDesc, &fullscreenDesc);

	return InitializeSwapChainImpl(pSwapChainDesc->OutputWindow, &swapChainDesc1, pSwapChainDesc->Windowed ? NULL :  &fullscreenDesc, DXGI_VERSION_1_2);
}

// new desc on new interfaces
BOOL clsDXGISwapChain::InitializeSwapChain1
(
        HWND hWnd,
	DXGI_SWAP_CHAIN_DESC1 * pSwapChainDesc,
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pFullscreeDesc
)
{
	return InitializeSwapChainImpl(hWnd, pSwapChainDesc, pFullscreeDesc, DXGI_VERSION_1_2);
}

BOOL clsDXGISwapChain::InitializeSwapChainImpl
(
        HWND hWnd,
	DXGI_SWAP_CHAIN_DESC1 * pSwapChainDesc,
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pFullscreeDesc,
	DXGI_VERSION dxgiVersion
)
{
	//
	// Initialize locals.
	//

	HRESULT hresult = S_OK;
	BOOL bReturn = FALSE;
	char szMsg[ MSG_LEN ] = {0};	
	ULONG ulRefCt = 0;

	IDXGIDevice * pDXGIDevice = NULL;
	IDXGIAdapter * pDXGIAdapter = NULL;
	IDXGIFactory * pDXGIFactory = NULL;
	IDXGIFactory2 * pDXGIFactory2 = NULL;


	//
	// Validate members and state.
	//

	if ( pLoggingGroup == NULL )
	{
		StringCchPrintfA
		(
			szMsg,
			ARRAYSIZE( szMsg ),
			"%s at line %d.  FAILED:  pLoggingGroup is NULL.\n",
			__FILE__,
			__LINE__
		);

		DebugOut.Write( szMsg );

		goto Cleanup;
	};


	if ( pParentD3D10Device == NULL )
	{
		pLoggingGroup->WriteToLog
		(
			"clsDXGISwapChain::InitializeSwapChain:  Parent device object pointer is NULL."
		);

		goto Cleanup;
	};


	if ( pParentD3D10Device->pD3D10Device == NULL )
	{
		pLoggingGroup->WriteToLog
		(
			"clsDXGISwapChain::InitializeSwapChain:  Parent device object is not initialized."
		);

		goto Cleanup;
	};


	//
	// Validate parameters.
	//

	if ( pSwapChainDesc == NULL )
	{
		if ( pLoggingGroup )
		{
			pLoggingGroup->WriteToLog
			(
				"clsDXGISwapChain::InitializeSwapChain:  Swap chain descriptor pointer is NULL."
			);
		};

		goto Cleanup;
	};


	//
	// Release objects that we might be holding.
	//

	ReleaseAllObjects();


	//
	// Query the device for its IDXGIDevice interface pointer.
	//

	hresult = pParentD3D10Device->pD3D10Device->QueryInterface
	(
		IID_IDXGIDevice,
		(void **) &pDXGIDevice
	);

	if ( FAILED( hresult ) )
	{
		pLoggingGroup->WriteToLog
		(
			"clsDXGISwapChain::InitializeSwapChain:  QueryInterface on device for IDXGIDevice interface failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( pDXGIDevice == NULL )
	{
		pLoggingGroup->WriteToLog
		(
			"clsDXGISwapChain::InitializeSwapChain:  QueryInterface on device for IDXGIDevice succeeded but returned NULL interface pointer."
		);

		goto Cleanup;
	};


	//
	// Get the device's adapter.
	//

	hresult = pDXGIDevice->GetAdapter
	(
		&pDXGIAdapter
	);

	if ( FAILED( hresult ) )
	{
		pLoggingGroup->WriteToLog
		(
			"clsDXGISwapChain::InitializeSwapChain:  GetAdapter failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( pDXGIAdapter == NULL )
	{
		pLoggingGroup->WriteToLog
		(
			"clsDXGISwapChain::InitializeSwapChain:  GetAdapter succeeded but returned a NULL adapter pointer."
		);

		goto Cleanup;
	};


	//
	// Get the adapter's factory.
	//

	hresult = pDXGIAdapter->GetParent
	(
		IID_IDXGIFactory,
		(void **) &pDXGIFactory
	);

	if ( FAILED( hresult ) )
	{
		pLoggingGroup->WriteToLog
		(
			"clsDXGISwapChain::InitializeSwapChain:  GetParent failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( pDXGIFactory == NULL )
	{
		pLoggingGroup->WriteToLog
		(
			"clsDXGISwapChain::InitializeSwapChain:  GetParent succeeded but returned a NULL factory pointer."
		);

		goto Cleanup;
	};


	//
	// Create the swap chain.
	//

	if( dxgiVersion >= DXGI_VERSION_1_2 )
	{
		if( FAILED( hresult = pDXGIFactory->QueryInterface(__uuidof(IDXGIFactory2), (void**)&pDXGIFactory2) ) )
		{
			pLoggingGroup->WriteToLog
			(
				"clsDXGISwapChain::InitializeSwapChain:  QueryInterface for IDXGIFactory2 failed with error 0x%x.",
				hresult
			);

			goto Cleanup;
		}
		
		IDXGISwapChain1 *pSwapChain1 = NULL;

		if( hWnd == NULL )
		{
			hresult = pDXGIFactory2->CreateSwapChainForComposition
			(
				pDXGIDevice,
				pSwapChainDesc,
				NULL,
				&pSwapChain1
			);
		}
		else
		{
			hresult = pDXGIFactory2->CreateSwapChainForHwnd
			(
				pDXGIDevice,
				hWnd,
				pSwapChainDesc,
				pFullscreeDesc,
				NULL,
				&pSwapChain1
			);
		}

		pSwapChain = pSwapChain1;
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = CopyToSwapChainDesc(hWnd, *pSwapChainDesc, pFullscreeDesc);
		hresult = pDXGIFactory->CreateSwapChain
		(
			pDXGIDevice,
			&swapChainDesc,
			&pSwapChain
		);
	}


	if ( FAILED( hresult ) )
	{
		pLoggingGroup->WriteToLog
		(
			"clsDXGISwapChain::InitializeSwapChain:  CreateSwapChain failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( pSwapChain == NULL )
	{
		pLoggingGroup->WriteToLog
		(
			"clsDXGISwapChain::InitializeSwapChain:  CreateSwapChain succeeded but returned a NULL swap chain pointer."
		);

		goto Cleanup;
	};


	bReturn = TRUE;


Cleanup:
	SAFE_RELEASE(pDXGIDevice);
	SAFE_RELEASE(pDXGIAdapter);
	SAFE_RELEASE(pDXGIFactory);
	SAFE_RELEASE(pDXGIFactory2);

	return bReturn;
};


// original DXGI 1.0 test usage
BOOL clsDXGISwapChain::InitializeFullscreenSwapChain
(
	DXGI_SWAP_CHAIN_DESC * pSwapChainDesc
)
{
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = CopyToSwapChainDesc1(*pSwapChainDesc, &fullscreenDesc);

	return InitializeFullscreenSwapChainImpl(pSwapChainDesc->OutputWindow, &swapChainDesc1, pSwapChainDesc->Windowed ? NULL :  &fullscreenDesc, DXGI_VERSION_1_0);
}

// useful for tests that want to hit new interface path without changing descs in test
BOOL clsDXGISwapChain::InitializeFullscreenSwapChain1
(
	DXGI_SWAP_CHAIN_DESC * pSwapChainDesc
)
{
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = CopyToSwapChainDesc1(*pSwapChainDesc, &fullscreenDesc);

	return InitializeFullscreenSwapChainImpl(pSwapChainDesc->OutputWindow, &swapChainDesc1, pSwapChainDesc->Windowed ? NULL :  &fullscreenDesc, DXGI_VERSION_1_2);
}

// new desc on new interfaces
BOOL clsDXGISwapChain::InitializeFullscreenSwapChain1
(
        HWND hWnd,
	DXGI_SWAP_CHAIN_DESC1 * pSwapChainDesc,
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pFullscreeDesc
)
{
	return InitializeFullscreenSwapChainImpl(hWnd, pSwapChainDesc, pFullscreeDesc, DXGI_VERSION_1_2);
}

BOOL  clsDXGISwapChain::InitializeFullscreenSwapChainImpl
(
    HWND hWnd,
	DXGI_SWAP_CHAIN_DESC1* pSwapChainDesc,
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreeDesc, 
	DXGI_VERSION dxgiVersion
)
{
	HRESULT	 hresult = S_OK;
	BOOL		bReturn = TRUE;
	char		szMsg[MSG_LEN] = {0};	

	IDXGIFactory2 *pDXGIFactory2 = NULL;

	ReleaseAllObjects();

	if ( pParentD3D10Device == NULL )
	{
		bReturn = FALSE;

		if ( pLoggingGroup != NULL )
		{
			pLoggingGroup->WriteToLog("clsDXGISwapChain::InitializeFullscreenSwapChain:  No parent device.");
		}
	}
	else
	{
		ID3D10Device   *pD3D10Device = NULL;
		IDXGIDevice	*pDXGIDevice = NULL;

		if ( (pD3D10Device = pParentD3D10Device->pD3D10Device) == NULL )
		{
			bReturn = FALSE;

			if ( pLoggingGroup != NULL )
			{
				pLoggingGroup->WriteToLog("clsDXGISwapChain::InitializeFullscreenSwapChain:  Parent device not initialized.");
			}
		}
		else
		if ( FAILED ( hresult = pD3D10Device->QueryInterface( __uuidof(IDXGIDevice), (void **)&pDXGIDevice ) ) )
		{
			bReturn = FALSE;

			if ( pLoggingGroup != NULL )
			{
				pLoggingGroup->WriteToLog("clsDXGISwapChain::InitializeFullscreenSwapChain:  Unable to retrieve IDXGIDevice interface from D3D10 device.");
			}
		}

		if ( pDXGIDevice == NULL )
		{
			bReturn = FALSE;
		}
		else
		{
			IDXGIAdapter   *pDeviceAdapter = NULL;
			IDXGIFactory   *pParentFactory = NULL;

			if ( FAILED ( hresult = pDXGIDevice->GetAdapter( &pDeviceAdapter ) ) )
			{
				bReturn = FALSE;

				if ( pLoggingGroup != NULL )
				{
					pLoggingGroup->WriteToLog("clsDXGISwapChain::InitializeFullscreenSwapChain:  GetAdapter failed with error 0x%x.", hresult);
				}
			}
			else
			{
				if ( FAILED ( hresult = pDeviceAdapter->GetParent( __uuidof(IDXGIFactory), (void **)&pParentFactory ) ) )
				{
					bReturn = FALSE;

					if ( pLoggingGroup != NULL )
					{
						pLoggingGroup->WriteToLog("clsDXGISwapChain::InitializeFullscreenSwapChain:  GetParent failed with error 0x%x.", hresult);
					}
				}

				pDeviceAdapter->Release();
			}

			if ( pParentFactory == NULL )
			{
				bReturn = FALSE;
			}
			else
			{
				if( dxgiVersion >= DXGI_VERSION_1_2 )
				{
					if( FAILED( hresult = pParentFactory->QueryInterface(__uuidof(IDXGIFactory2), (void**)&pDXGIFactory2) ) )
					{
						pLoggingGroup->WriteToLog
						(
							"clsDXGISwapChain::InitializeSwapChain:  QueryInterface for IDXGIFactory2 failed with error 0x%x.",
							hresult
						);

						goto Cleanup;
					}

					IDXGISwapChain1 *pSwapChain1 = NULL;

					hresult = pDXGIFactory2->CreateSwapChainForHwnd( pDXGIDevice, hWnd, pSwapChainDesc, pFullscreeDesc, NULL, &pSwapChain1 );

					pSwapChain = pSwapChain1;
				}
				else
				{
					DXGI_SWAP_CHAIN_DESC swapChainDesc = CopyToSwapChainDesc(hWnd, *pSwapChainDesc, pFullscreeDesc);
					hresult = pParentFactory->CreateSwapChain( pDXGIDevice, &swapChainDesc, &pSwapChain );
				}

				if ( FAILED(hresult) )
				{
					bReturn = FALSE;

					if ( pLoggingGroup != NULL )
					{
						pLoggingGroup->WriteToLog("clsDXGISwapChain::InitializeFullscreenSwapChain:  CreateSwapChain failed with error 0x%x.", hresult);
					}
				}

   				SAFE_RELEASE(pParentFactory);
			}

			pDXGIDevice->Release();	 // The RefCount here is too variable to check.  It depends on # display devices and other factors.
		}
	}

Cleanup:
	SAFE_RELEASE(pDXGIFactory2);

	return bReturn;
}

DXGI_SWAP_CHAIN_DESC clsDXGISwapChain::CopyToSwapChainDesc( HWND hWnd, const DXGI_SWAP_CHAIN_DESC1 &swapChainDesc1, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc )
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	swapChainDesc.BufferDesc.Width = swapChainDesc1.Width;
	swapChainDesc.BufferDesc.Height = swapChainDesc1.Height;
	swapChainDesc.BufferDesc.Format = swapChainDesc1.Format;
        if (pFullscreenDesc)
        {
	    swapChainDesc.Windowed = FALSE;
	    swapChainDesc.BufferDesc.RefreshRate = pFullscreenDesc->RefreshRate;
	    swapChainDesc.BufferDesc.ScanlineOrdering = pFullscreenDesc->ScanlineOrdering;
	    swapChainDesc.BufferDesc.Scaling = pFullscreenDesc->Scaling;
        }
        else
        {
	    swapChainDesc.Windowed = TRUE;
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        }
	swapChainDesc.SampleDesc = swapChainDesc1.SampleDesc;
	swapChainDesc.BufferUsage = swapChainDesc1.BufferUsage;
	swapChainDesc.BufferCount = swapChainDesc1.BufferCount;
	swapChainDesc.SwapEffect = swapChainDesc1.SwapEffect;
	swapChainDesc.Flags = swapChainDesc1.Flags;
	
        swapChainDesc.OutputWindow = hWnd;

	return swapChainDesc;
}

DXGI_SWAP_CHAIN_DESC1 clsDXGISwapChain::CopyToSwapChainDesc1( const DXGI_SWAP_CHAIN_DESC &swapChainDesc, DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreeDesc )
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc1 = { 0 };

	swapChainDesc1.Width = swapChainDesc.BufferDesc.Width;
	swapChainDesc1.Height = swapChainDesc.BufferDesc.Height;
	swapChainDesc1.Format = swapChainDesc.BufferDesc.Format;
	swapChainDesc1.Stereo = FALSE;
	swapChainDesc1.SampleDesc = swapChainDesc.SampleDesc;
	swapChainDesc1.BufferUsage = swapChainDesc.BufferUsage;
	swapChainDesc1.BufferCount = swapChainDesc.BufferCount;
	swapChainDesc1.SwapEffect = swapChainDesc.SwapEffect;
	swapChainDesc1.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc1.Flags = swapChainDesc.Flags;

        if (pFullscreeDesc)
        {
	    pFullscreeDesc->RefreshRate = swapChainDesc.BufferDesc.RefreshRate;
	    pFullscreeDesc->ScanlineOrdering = swapChainDesc.BufferDesc.ScanlineOrdering;
	    pFullscreeDesc->Scaling = swapChainDesc.BufferDesc.Scaling;
        pFullscreeDesc->Windowed = FALSE;
        }
	return swapChainDesc1;
}

DXGI_MODE_DESC1 clsDXGISwapChain::CopyToModeDesc1( const DXGI_MODE_DESC &modeDesc )
{
	DXGI_MODE_DESC1 modeDesc1;

	modeDesc1.Width = modeDesc.Width;
	modeDesc1.Height = modeDesc.Height;
	modeDesc1.RefreshRate = modeDesc.RefreshRate;
	modeDesc1.Format = modeDesc.Format;
	modeDesc1.ScanlineOrdering = modeDesc.ScanlineOrdering;
	modeDesc1.Scaling = modeDesc.Scaling;
	modeDesc1.Stereo = FALSE;

	return modeDesc1;
}

DXGI_MODE_DESC clsDXGISwapChain::CopyToModeDesc( const DXGI_MODE_DESC1 &modeDesc1 )
{
	DXGI_MODE_DESC modeDesc;

	modeDesc.Width = modeDesc1.Width;
	modeDesc.Height = modeDesc1.Height;
	modeDesc.RefreshRate = modeDesc1.RefreshRate;
	modeDesc.Format = modeDesc1.Format;
	modeDesc.ScanlineOrdering = modeDesc1.ScanlineOrdering;
	modeDesc.Scaling = modeDesc1.Scaling;
	
	return modeDesc;
}


BOOL  clsDXGISwapChain::GetSwapChainPtr
(
	IDXGISwapChain **pSwapChainPointer
)
{
	if ( pSwapChainPointer == NULL )
	{
		return FALSE;
	}

	*pSwapChainPointer = NULL;

	if ( pSwapChain == NULL )
	{
		return FALSE;
	}

	pSwapChain->AddRef();
	*pSwapChainPointer = pSwapChain;

	return TRUE;
}


void clsDXGISwapChain::ReleaseAllObjects()
{
	if ( pSwapChain != NULL )
	{
		//
		// Restore the swap chain to windowed mode if necessary.
		// We can't arbitrarily set it to windowed mode as the
        // SetFullscreenState() call always fails when called from
        // a windowless (DCOMP bound) swapchain.

        BOOL bFullscreen = TRUE; // Assume we're fullscreen as a precaution.
                                 // worse that will happen is that the SetFullscreenState
                                 // call will fail for windowless swapchains.   That's 
                                 // better than crashing if we try to release a swapchain
                                 // in fullscreen mode.
        //
        // Try to obtain the actual fullscreen state to determine whether or not to reset things 
        // to windowed mode.
        //
        HRESULT hr = pSwapChain->GetFullscreenState(&bFullscreen, NULL);
        if (FAILED(hr))
        {
				pLoggingGroup->WriteToLog
				(
					"clsDXGISwapChain::ReleaseAllObjects:  GettFullscreenState failed with error 0x%x.",
					hr
				);
        }

        //
        // If we're in fullscreen mode, switch to windowed before releasing our swapchain.  If we don't we'll crash.
        //
        if (bFullscreen)
        {
		    HRESULT hSetFullscreenStateResult = pSwapChain->SetFullscreenState
		    (
			    FALSE,
			    NULL
		    );

		    if ( FAILED( hSetFullscreenStateResult ) )
		    {
			    if ( pLoggingGroup )
			    {
				    pLoggingGroup->WriteToLog
				    (
					    "clsDXGISwapChain::ReleaseAllObjects:  SetFullscreenState failed with error 0x%x.",
					    hSetFullscreenStateResult
				    );
			    }
		    };

        }

		//
		// Release the swap chain.
		//

		UINT uiRefCount = pSwapChain->Release();
		if ( uiRefCount > 0 )
		{
			if ( pLoggingGroup )
			{
				pLoggingGroup->WriteToLog
				(
					"clsDXGISwapChain::ReleaseAllObjects:  WARNING:  Non-zero reference count after releasing swap chain."
				);
			};
		};


		pSwapChain = NULL;
	};
};


bool clsDXGISwapChain::InitBBFrom2DTexture
(
	UINT uiBBIndex,
	__in ID3D10ShaderResourceView *pTexSRV,
	LPCSTR VertexShaderModel,
	LPCSTR PixelShaderModel
)
{
	HRESULT hresult;
	bool bReturn = false;

	if (!pSwapChain)
	{
		if (pLoggingGroup)
			pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBFrom2DTexture:  Swap chain not initialized.");
	}
	else
		if (!pParentD3D10Device)
		{
			if (pLoggingGroup)
				pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBFrom2DTexture:  No parent device.");
		}
		else
		{
			ID3D10Device *pD3D10DeviceFromSC = NULL;

			if (!(pD3D10DeviceFromSC = pParentD3D10Device->pD3D10Device))
			{
				if (pLoggingGroup)
					pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBFrom2DTexture:  Parent device not initialized.");
			}
			else
			{
				ID3D10Device *pD3D10DeviceFromTex = NULL;

				// Verify that the source texture belongs to the device that created the swap
				// chain's back buffers.

				{
					ID3D10Resource *pTextureResource = NULL;

					pTexSRV->GetResource(&pTextureResource);

					if (!pTextureResource)
					{
						if (pLoggingGroup)
							pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBFrom2DTexture:  GetResource on shader resource view returned NULL.");
					}
					else
					{
						pTextureResource->GetDevice(&pD3D10DeviceFromTex);
						pTextureResource->Release();
					}
				}

				if (!pD3D10DeviceFromTex)
				{
					if (pLoggingGroup)
						pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBFrom2DTexture:  GetDevice on texture returned NULL.");
				}
				else
				{
					if (pD3D10DeviceFromTex != pD3D10DeviceFromSC)
					{
						if (pLoggingGroup)
							pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBFrom2DTexture:  Texture device does not match swap chain device.");
					}
					else
					{
						bool bParentDeviceReady = false;

						// Verify that the resources necessary for rendering have been
						// created.

						if (!(bParentDeviceReady = pParentD3D10Device->IsReadyForTexRender()))
						{
							bParentDeviceReady = pParentD3D10Device->CreateTexRenderResources
							(
								VertexShaderModel,
								PixelShaderModel
							);
						}

						if (!bParentDeviceReady)
						{
							if (pLoggingGroup)
								pLoggingGroup->WriteToLog("Aborting clsDXGISwapChain::InitBBFrom2DTexture:  Device resources unavailable.");
						}
						else
						{
							ID3D10Texture2D *pBBAsTexture = NULL;

							// Obtain an ID3D10Texture2D interface on the back buffer.
							if (FAILED((hresult = pSwapChain->GetBuffer(uiBBIndex, __uuidof(ID3D10Texture2D), (void **)(&pBBAsTexture)))))
							{
								if (pLoggingGroup)
									pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBFrom2DTexture:  GetBuffer on buffer index %u failed with error 0x%x.", uiBBIndex, hresult);
							}
							else
							{
								// Create a render target view based on the back buffer.

								D3D10_RENDER_TARGET_VIEW_DESC d3d10RTDesc;
								ID3D10RenderTargetView *pRTView = NULL;

								DXGI_SWAP_CHAIN_DESC scdescInit;
								pSwapChain->GetDesc( &scdescInit );

								d3d10RTDesc.Format = scdescInit.BufferDesc.Format;
								//d3d10RTDesc.ResourceType = D3D10_RESOURCE_TEXTURE2D;

								d3d10RTDesc.ViewDimension = scdescInit.SampleDesc.Count > 1? D3D10_RTV_DIMENSION_TEXTURE2DMS :  D3D10_RTV_DIMENSION_TEXTURE2D;

								//d3d10RTDesc.Texture2D.ArraySize = 1;
								//d3d10RTDesc.Texture2D.FirstArraySlice = 0;
								d3d10RTDesc.Texture2D.MipSlice = 0;

								if (FAILED((hresult = pD3D10DeviceFromTex->CreateRenderTargetView(pBBAsTexture, &d3d10RTDesc, &pRTView))))
								{
									if (pLoggingGroup)
										pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBFrom2DTexture:  CreateRenderTargetView failed with error 0x%x.", hresult);
								}
								else
								{
									// Set viewport to span entire back buffer.

									D3D10_VIEWPORT d3d10vp;

									d3d10vp.TopLeftX = 0;
									d3d10vp.TopLeftY = 0;
									d3d10vp.MinDepth = 0.0f;
									d3d10vp.MaxDepth = 1.0f;

									ID3D10Resource *pBuffer = NULL;
									IDXGISurface *pSurface = NULL;
									DXGI_SURFACE_DESC sd;
									ZeroMemory( &sd, sizeof( DXGI_SURFACE_DESC ) );

									pSwapChain->GetBuffer( 0, __uuidof(pBuffer), reinterpret_cast<void**>( &pBuffer ) );
									if( NULL != pBuffer )
										pBuffer->QueryInterface( __uuidof(pSurface), reinterpret_cast<void**>( &pSurface ) );							
									if( NULL != pSurface )
									{
										pSurface->GetDesc( &sd );
										pSurface->Release();
									}
									if( NULL != pBuffer )
										pBuffer->Release();

									if( 0 != sd.Width && 0 != sd.Height )
									{
										d3d10vp.Width = sd.Width;
										d3d10vp.Height = sd.Height;
									}
									else
									{
										d3d10vp.Width = ((scdescInit.BufferDesc).Width);
										d3d10vp.Height = ((scdescInit.BufferDesc).Height);
										// workaround for rotation bug with NONPREROTATED, landscape mode
										// the viewport needs to be rotated
										if( 0 != ( scdescInit.Flags & DXGI_SWAP_CHAIN_FLAG_NONPREROTATED ) )
										{
											IDXGIOutput *pOutput = NULL;
											DXGI_OUTPUT_DESC outputDesc;
											if( SUCCEEDED( pSwapChain->GetContainingOutput( &pOutput ) ) &&
												SUCCEEDED( pOutput->GetDesc( &outputDesc ) ) &&
												( ( DXGI_MODE_ROTATION_ROTATE90 == outputDesc.Rotation ) ||
												(	DXGI_MODE_ROTATION_ROTATE270 == outputDesc.Rotation )
											  )
											)
												std::swap( d3d10vp.Width, d3d10vp.Height );

											if( NULL != pOutput )
												pOutput->Release();
										}
									}

									bReturn = pParentD3D10Device->RenderTexture(pRTView, pTexSRV, &d3d10vp);

									// Unbind the render target view...
									ID3D10RenderTargetView * pNullRenderTargetView = NULL;
									pD3D10DeviceFromTex->OMSetRenderTargets
									(
										0,
										&pNullRenderTargetView,
										NULL
									);

									// ...and release it.
									pRTView->Release();
								}

								pBBAsTexture->Release();
							}
						}
					}

					pD3D10DeviceFromTex->Release();
				}
			}
		}

	return bReturn;
}


bool clsDXGISwapChain::InitBBViaClear
(
	UINT uiBBIndex
)
{
	HRESULT hresult = S_OK;
	bool bReturn = false;

	if (!pSwapChain)
	{
		if (pLoggingGroup)
		{
			pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear:  Swap chain not initialized.");
		}
		hresult = E_INVALIDARG;
		goto Cleanup;
	}

	if (!pParentD3D10Device)
	{
		if (pLoggingGroup)
		{
			pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear:  No parent device.");
		}
		hresult = E_INVALIDARG;
		goto Cleanup;
	}

	ID3D10Device *pD3D10DeviceFromSC = NULL;
	if (!(pD3D10DeviceFromSC = pParentD3D10Device->pD3D10Device))
	{
		if (pLoggingGroup)
		{
			pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear:  Parent device not initialized.");
		}
		hresult = E_INVALIDARG;
		goto Cleanup;
	}

	ID3D10Resource * pBuffer = NULL;
	hresult = pSwapChain->GetBuffer(uiBBIndex, __uuidof(pBuffer), reinterpret_cast<void**>(&pBuffer));
	if (FAILED(hresult))
	{
		if (pLoggingGroup)
		{
			pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear:  GetBuffer on buffer index %u failed with error 0x%x.", uiBBIndex, hresult);
		}
		goto Cleanup;
	}

	IDXGISurface * pSurface = NULL;
	hresult = pBuffer->QueryInterface(__uuidof(pSurface), reinterpret_cast<void**>(&pSurface));
	if (FAILED(hresult))
	{
		if (pLoggingGroup)
		{
			pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear:  QueryInterface (surface) failed with error 0x%x.", hresult);
		}
		goto Cleanup;
	}

	DXGI_SURFACE_DESC sd;
	ZeroMemory( &sd, sizeof( DXGI_SURFACE_DESC ) );
	pSurface->GetDesc(&sd);
	
	pSurface->Release();
	pSurface = NULL;
	
	ID3D10RenderTargetView * pRTView = NULL;
	hresult = pD3D10DeviceFromSC->CreateRenderTargetView( pBuffer, NULL, reinterpret_cast<ID3D10RenderTargetView**> (&pRTView) );
	pBuffer->Release();
	pBuffer = NULL;
	if (FAILED(hresult))
	{
		if (pLoggingGroup)
		{
			pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear:  CreateRenderTargetView failed with error 0x%x.", hresult);
		}
		goto Cleanup;
	}

	pD3D10DeviceFromSC->OMSetRenderTargets(1, &pRTView, 0);

	D3D10_VIEWPORT Viewport;
	ZeroMemory( &Viewport, sizeof( Viewport ) );

	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = sd.Width;
	Viewport.Height = sd.Height;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	pD3D10DeviceFromSC->RSSetViewports(1, &Viewport);

	float ClearColor[] = {0.7f, 0.2f, 0.4f, 0.9f};
	pD3D10DeviceFromSC->ClearRenderTargetView(pRTView, ClearColor);
	
	//
	// Clean up RTView by binding NULL and releasing
	//
	pD3D10DeviceFromSC->OMSetRenderTargets(0, 0, 0);
	pRTView->Release();
	pRTView = NULL;

	if(FAILED(hresult))
	{
		if (pLoggingGroup)
		{
			pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear:  ClearRenderTargetView failed with error 0x%x.", hresult);
		}
		goto Cleanup;
	}

	if (pLoggingGroup)
	{
		pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear:  Clear failed with error 0x%x.", hresult);
	}

	hresult = pSwapChain->Present( 0, 0 );
	if(FAILED(hresult))
	{		
		if (pLoggingGroup)
		{
			pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear:  Present failed with error 0x%x.", hresult);
		}
		goto Cleanup;
	}


Cleanup:

	if( SUCCEEDED( hresult ))
	{
		bReturn = true;
	}

	return bReturn;
}
