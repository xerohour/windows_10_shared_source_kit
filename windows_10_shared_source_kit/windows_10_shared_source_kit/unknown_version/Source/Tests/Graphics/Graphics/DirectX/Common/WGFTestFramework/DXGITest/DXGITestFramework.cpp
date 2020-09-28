//////////////////////////////////////////////////////////////////////
// File:  DXGITestFramework.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a DXGI test framework base class.
//
// History:
// 11 Sep 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "DXGITestFramework.h"

#include <d3dcommon.h>

#include <strsafe.h>

#include "CreateDevice.h"

///////////////////////////////////////////////////////
// Macro:  STRING_TO_ENUM
//
// Purpose:
// Maps a test a stringized enum name to an actual enum
// value.
///////////////////////////////////////////////////////
#define STRING_TO_ENUM(x, y, z) \
	if ( (x) == _T( #y ) ) \
	{ \
		*z = y; \
		return true; \
	}


///////////////////////////////////////////////////////
// Macro:  ENUM_TO_STRING
//
// Purpose:
// Maps an enum value to a string (the stringized
// version of the enum value's name).
///////////////////////////////////////////////////////
#define ENUM_TO_STRING(x, y) \
	if ( x == y ) \
	{ \
		return tstring( _T( #y ) ); \
	}

///////////////////////////////////////////////////////
// Function:  FromString( DXGI_MODE_ROTATION * )
//
// Purpose:
// Converts a tstring to a DXGI_MODE_ROTATION.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	DXGI_MODE_ROTATION * pModeRotation,
	const tstring & ModeRotationString
)
{
	STRING_TO_ENUM( ModeRotationString, DXGI_MODE_ROTATION_IDENTITY, pModeRotation );
	STRING_TO_ENUM( ModeRotationString, DXGI_MODE_ROTATION_ROTATE90, pModeRotation );
	STRING_TO_ENUM( ModeRotationString, DXGI_MODE_ROTATION_ROTATE180, pModeRotation );
	STRING_TO_ENUM( ModeRotationString, DXGI_MODE_ROTATION_ROTATE270, pModeRotation );

	return FromString
	(
		(UINT *) pModeRotation,
		ModeRotationString
	);
};


///////////////////////////////////////////////////////
// Function:  ToString( DXGI_MODE_ROTATION )
//
// Purpose:
// Converts a DXGI_MODE_ROTATION to a tstring.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const DXGI_MODE_ROTATION & ModeRotation
)
{
	ENUM_TO_STRING( ModeRotation, DXGI_MODE_ROTATION_IDENTITY );
	ENUM_TO_STRING( ModeRotation, DXGI_MODE_ROTATION_ROTATE90 );
	ENUM_TO_STRING( ModeRotation, DXGI_MODE_ROTATION_ROTATE180 );
	ENUM_TO_STRING( ModeRotation, DXGI_MODE_ROTATION_ROTATE270 );


	return tstring( _T( "" ) );
};


///////////////////////////////////////////////////////
// Function:  FromString( DXGI_MODE_SCALING * )
//
// Purpose:
// Converts a tstring to a DXGI_MODE_SCALING.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	DXGI_MODE_SCALING * pModeScaling,
	const tstring & ModeScalingString
)
{
	STRING_TO_ENUM( ModeScalingString, DXGI_MODE_SCALING_UNSPECIFIED, pModeScaling );
	STRING_TO_ENUM( ModeScalingString, DXGI_MODE_SCALING_CENTERED, pModeScaling );
	STRING_TO_ENUM( ModeScalingString, DXGI_MODE_SCALING_STRETCHED, pModeScaling );

	return FromString
	(
		(UINT *) pModeScaling,
		ModeScalingString
	);
};


///////////////////////////////////////////////////////
// Function:  ToString( DXGI_MODE_SCALING )
//
// Purpose:
// Converts a DXGI_MODE_SCALING to a tstring.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const DXGI_MODE_SCALING & ModeScaling
)
{
	ENUM_TO_STRING( ModeScaling, DXGI_MODE_SCALING_UNSPECIFIED );
	ENUM_TO_STRING( ModeScaling, DXGI_MODE_SCALING_CENTERED );
	ENUM_TO_STRING( ModeScaling, DXGI_MODE_SCALING_STRETCHED );


	return tstring( _T( "" ) );
};


///////////////////////////////////////////////////////
// Function:  FromString( DXGI_MODE_SCANLINE_ORDER * )
//
// Purpose:
// Converts a tstring to a DXGI_MODE_SCANLINE_ORDER.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	DXGI_MODE_SCANLINE_ORDER * pModeScanlineOrder,
	const tstring & ModeScanlineOrderString
)
{
	STRING_TO_ENUM( ModeScanlineOrderString, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, pModeScanlineOrder );
	STRING_TO_ENUM( ModeScanlineOrderString, DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE, pModeScanlineOrder );
	STRING_TO_ENUM( ModeScanlineOrderString, DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST, pModeScanlineOrder );
	STRING_TO_ENUM( ModeScanlineOrderString, DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST, pModeScanlineOrder );

	return FromString
	(
		(UINT *) pModeScanlineOrder,
		ModeScanlineOrderString
	);
};


///////////////////////////////////////////////////////
// Function:  ToString( DXGI_MODE_SCANLINE_ORDER )
//
// Purpose:
// Converts a DXGI_MODE_SCANLINE_ORDER to a tstring.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const DXGI_MODE_SCANLINE_ORDER & ModeScanlineOrder
)
{
	ENUM_TO_STRING( ModeScanlineOrder, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED );
	ENUM_TO_STRING( ModeScanlineOrder, DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE );
	ENUM_TO_STRING( ModeScanlineOrder, DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST );
	ENUM_TO_STRING( ModeScanlineOrder, DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST );


	return tstring( _T( "" ) );
};


///////////////////////////////////////////////////////
// Function:  FromString( TEST_FEATURE_LEVEL * )
//
// Purpose:
// Converts a tstring to a TEST_FEATURE_LEVEL.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	TEST_FEATURE_LEVEL * pTestFeatureLevel,
	const tstring & TestFeatureLevelString
)
{
	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_10_0_LEVEL_10_0, pTestFeatureLevel );

	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_10_1_LEVEL_9_1,  pTestFeatureLevel );
	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_10_1_LEVEL_9_2,  pTestFeatureLevel );
	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_10_1_LEVEL_9_3,  pTestFeatureLevel );
	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_10_1_LEVEL_10_0, pTestFeatureLevel );
	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_10_1_LEVEL_10_1, pTestFeatureLevel );

	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_11_0_LEVEL_9_1, pTestFeatureLevel );
	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_11_0_LEVEL_9_2, pTestFeatureLevel );
	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_11_0_LEVEL_9_3, pTestFeatureLevel );
	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_11_0_LEVEL_10_0, pTestFeatureLevel );
	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_11_0_LEVEL_10_1, pTestFeatureLevel );
	STRING_TO_ENUM( TestFeatureLevelString, TEST_FEATURE_11_0_LEVEL_11_0, pTestFeatureLevel );

	return FromString
	(
		(UINT *) pTestFeatureLevel,
		TestFeatureLevelString
	);
};


///////////////////////////////////////////////////////
// Function:  ToString( TEST_FEATURE_LEVEL )
//
// Purpose:
// Converts a TEST_FEATURE_LEVEL to a tstring.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const TEST_FEATURE_LEVEL & TestFeatureLevel
)
{
	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_10_0_LEVEL_10_0 );

	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_10_1_LEVEL_9_1 );
	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_10_1_LEVEL_9_2 );
	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_10_1_LEVEL_9_3 );
	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_10_1_LEVEL_10_0 );
	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_10_1_LEVEL_10_1 );

	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_11_0_LEVEL_9_1 );
	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_11_0_LEVEL_9_2 );
	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_11_0_LEVEL_9_3 );
	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_11_0_LEVEL_10_0 );
	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_11_0_LEVEL_10_1 );
	ENUM_TO_STRING( TestFeatureLevel, TEST_FEATURE_11_0_LEVEL_11_0 );

	return tstring( _T( "" ) );
};


///////////////////////////////////////////////////////
// Function:  FromString( TEST_DXGI_VERSION * )
//
// Purpose:
// Converts a tstring to a TEST_DXGI_VERSION.
///////////////////////////////////////////////////////
bool __stdcall FromString
(
	TEST_DXGI_VERSION * pTestDXGIVersion,
	const tstring & TestDXGIVersionString
)
{
	STRING_TO_ENUM( TestDXGIVersionString, TEST_DXGI_VERSION_1_0, pTestDXGIVersion );
	STRING_TO_ENUM( TestDXGIVersionString, TEST_DXGI_VERSION_1_1, pTestDXGIVersion );
	STRING_TO_ENUM( TestDXGIVersionString, TEST_DXGI_VERSION_1_2, pTestDXGIVersion );
	STRING_TO_ENUM( TestDXGIVersionString, TEST_DXGI_VERSION_1_3, pTestDXGIVersion );
	STRING_TO_ENUM( TestDXGIVersionString, TEST_DXGI_VERSION_BESTAVAILABLE, pTestDXGIVersion );

	return FromString
	(
		(UINT *) pTestDXGIVersion,
		TestDXGIVersionString
	);
};


///////////////////////////////////////////////////////
// Function:  ToString( TEST_DXGI_VERSION )
//
// Purpose:
// Converts a TEST_DXGI_VERSION to a tstring.
///////////////////////////////////////////////////////
tstring __stdcall ToString
(
	const TEST_DXGI_VERSION & TestDXGIVersion
)
{
	ENUM_TO_STRING( TestDXGIVersion, TEST_DXGI_VERSION_1_0 );
	ENUM_TO_STRING( TestDXGIVersion, TEST_DXGI_VERSION_1_1 );
	ENUM_TO_STRING( TestDXGIVersion, TEST_DXGI_VERSION_1_2 );
	ENUM_TO_STRING( TestDXGIVersion, TEST_DXGI_VERSION_1_3 );
	ENUM_TO_STRING( TestDXGIVersion, TEST_DXGI_VERSION_BESTAVAILABLE );

	return tstring( _T( "" ) );
};


///////////////////////////////////////////////////////
// Function:  GetD3DDriverTypeForD3D10DriverType()
//
// Purpose:
// Converts D3D10 driver type to D3D driver type.
///////////////////////////////////////////////////////
D3D_DRIVER_TYPE GetD3DDriverTypeForD3D10DriverType
(
	D3D10_DRIVER_TYPE DriverType
)
{
	switch ( DriverType )
	{
		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			return D3D_DRIVER_TYPE_HARDWARE;
		} break;

		case D3D10_DRIVER_TYPE_SOFTWARE:
		{
			return D3D_DRIVER_TYPE_SOFTWARE;
		} break;

		case D3D10_DRIVER_TYPE_NULL:
		{
			return D3D_DRIVER_TYPE_NULL;
		} break;

		case D3D10_DRIVER_TYPE_REFERENCE:
		{
			return D3D_DRIVER_TYPE_REFERENCE;
		} break;

		case D3D10_DRIVER_TYPE_WARP:
		{
			return D3D_DRIVER_TYPE_WARP;
		} break;

		default:
		{
			assert( FALSE );
			return D3D_DRIVER_TYPE_UNKNOWN;
		} break;
	};
};

///////////////////////////////////////////////////////
// Function:  GetSDKVersionForTestFeatureLevel
//
// Purpose:
// Returns an SDK version for a corresponding
// TEST_FEATURE_LEVEL enum value.
///////////////////////////////////////////////////////
UINT GetSDKVersionForTestFeatureLevel
(
	TEST_FEATURE_LEVEL TestFeatureLevel
)
{
	switch ( TestFeatureLevel )
	{
		case TEST_FEATURE_10_0_LEVEL_10_0:
		{
			return D3D10_SDK_VERSION;
		} break;


		case TEST_FEATURE_10_1_LEVEL_9_1:
		case TEST_FEATURE_10_1_LEVEL_9_2:
		case TEST_FEATURE_10_1_LEVEL_9_3:
		case TEST_FEATURE_10_1_LEVEL_10_0:
		case TEST_FEATURE_10_1_LEVEL_10_1:
		{
			return D3D10_1_SDK_VERSION;
		} break;


		case TEST_FEATURE_11_0_LEVEL_9_1:
		case TEST_FEATURE_11_0_LEVEL_9_2:
		case TEST_FEATURE_11_0_LEVEL_9_3:
		case TEST_FEATURE_11_0_LEVEL_10_0:
		case TEST_FEATURE_11_0_LEVEL_10_1:
		case TEST_FEATURE_11_0_LEVEL_11_0:
		{
			return D3D11_SDK_VERSION;
		} break;


		default:
		{
			assert( TestFeatureLevel != TestFeatureLevel );
			return 0;
		} break;
	};
};


///////////////////////////////////////////////////////
// Function:  GetD3D10FeatureLevelForTestFeatureLevel
//
// Purpose:
// Returns a D3D10_FEATURE_LEVEL1 enum value for a
// corresponding TEST_FEATURE_LEVEL enum value.
///////////////////////////////////////////////////////
D3D10_FEATURE_LEVEL1 GetD3D10FeatureLevelForTestFeatureLevel
(
	TEST_FEATURE_LEVEL TestFeatureLevel
)
{
	switch ( TestFeatureLevel )
	{
		case TEST_FEATURE_10_1_LEVEL_9_1:
		case TEST_FEATURE_11_0_LEVEL_9_1:
		{
			return D3D10_FEATURE_LEVEL_9_1;
		} break;

		case TEST_FEATURE_10_1_LEVEL_9_2:
		case TEST_FEATURE_11_0_LEVEL_9_2:
		{
			return D3D10_FEATURE_LEVEL_9_2;
		} break;

		case TEST_FEATURE_10_1_LEVEL_9_3:
		case TEST_FEATURE_11_0_LEVEL_9_3:
		{
			return D3D10_FEATURE_LEVEL_9_3;
		} break;

		case TEST_FEATURE_10_0_LEVEL_10_0:
		case TEST_FEATURE_10_1_LEVEL_10_0:
		case TEST_FEATURE_11_0_LEVEL_10_0:
		{
			return D3D10_FEATURE_LEVEL_10_0;
		} break;

		case TEST_FEATURE_10_1_LEVEL_10_1:
		case TEST_FEATURE_11_0_LEVEL_10_1:
		{
			return D3D10_FEATURE_LEVEL_10_1;
		} break;

		default:
		{
			assert( TestFeatureLevel != TestFeatureLevel );
			return D3D10_FEATURE_LEVEL_10_0;
		} break;
	};
};


///////////////////////////////////////////////////////
// Function:  GetD3DFeatureLevelForTestFeatureLevel
//
// Purpose:
// Returns a D3D_FEATURE_LEVEL enum value for a
// corresponding TEST_FEATURE_LEVEL enum value.
///////////////////////////////////////////////////////
D3D_FEATURE_LEVEL GetD3DFeatureLevelForTestFeatureLevel
(
	TEST_FEATURE_LEVEL TestFeatureLevel
)
{
	switch ( TestFeatureLevel )
	{
		case TEST_FEATURE_10_1_LEVEL_9_1:
		case TEST_FEATURE_11_0_LEVEL_9_1:
		{
			return D3D_FEATURE_LEVEL_9_1;
		} break;

		case TEST_FEATURE_10_1_LEVEL_9_2:
		case TEST_FEATURE_11_0_LEVEL_9_2:
		{
			return D3D_FEATURE_LEVEL_9_2;
		} break;

		case TEST_FEATURE_10_1_LEVEL_9_3:
		case TEST_FEATURE_11_0_LEVEL_9_3:
		{
			return D3D_FEATURE_LEVEL_9_3;
		} break;

		case TEST_FEATURE_10_0_LEVEL_10_0:
		case TEST_FEATURE_10_1_LEVEL_10_0:
		case TEST_FEATURE_11_0_LEVEL_10_0:
		{
			return D3D_FEATURE_LEVEL_10_0;
		} break;

		case TEST_FEATURE_10_1_LEVEL_10_1:
		case TEST_FEATURE_11_0_LEVEL_10_1:
		{
			return D3D_FEATURE_LEVEL_10_1;
		} break;

		case TEST_FEATURE_11_0_LEVEL_11_0:
		{
			return D3D_FEATURE_LEVEL_11_0;
		} break;

		default:
		{
			assert( TestFeatureLevel != TestFeatureLevel );
			return D3D_FEATURE_LEVEL_10_0;
		} break;
	};
};


/////////////////////////////////////////////////////////////////
//
// CDXGITestFramework implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CDXGITestFramework
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CDXGITestFramework::CDXGITestFramework()
:
	CGraphicsTestFramework
	(
		&m_D3DOptions
	),
	m_bRegisterFeatureLevel( true ),
	m_bRegisterDriverType( true ),
	m_hD3D10_1( NULL ),
	m_pD3D10CreateDevice1( NULL ),
	m_pD3D10CreateDeviceAndSwapChain1( NULL ),
	m_hD3D11Layer( NULL ),
	m_pD3D11LayerCreateDevice( NULL ),
	m_pD3D11LayerCreateDeviceAndSwapChain( NULL )
{
	HRESULT hresult = S_OK;

	//
	// Load D3D11RuntimeLayers.dll.
	//

	hresult = LoadD3D11RuntimeLayer();
	if ( FAILED( hresult ) )
	{
		WriteToLogF
		(
			_T( "CDXGITestFramework::CDXGITestFramework:  LoadD3D11RuntimeLayer failed with error 0x%x." ),
			hresult
		);
	};


	//
	// Initialize our requested feature level array.
	//

	ZeroMemory( m_RequestedFeatureLevels, sizeof( m_RequestedFeatureLevels ) );

	//
	// Load D3D10_1.dll.
	//

	m_hD3D10_1 = LoadLibraryExW
	(
		L"d3d10_1.dll",
		NULL,
		0
	);

	if ( m_hD3D10_1 != NULL )
	{
		m_pD3D10CreateDevice1 = (PFN_D3D10_CREATE_DEVICE1) GetProcAddress
		(
			m_hD3D10_1,
			"D3D10CreateDevice1"
		);

		m_pD3D10CreateDeviceAndSwapChain1 = (PFN_D3D10_CREATE_DEVICE_AND_SWAP_CHAIN1) GetProcAddress
		(
			m_hD3D10_1,
			"D3D10CreateDeviceAndSwapChain1"
		);
	};


	//
	// Initialize our D3D options.
	//

	ZeroMemory( &m_D3DOptions, sizeof( m_D3DOptions ) );
	m_D3DOptions.RefDriverType = D3D10_DRIVER_TYPE_REFERENCE;
	m_D3DOptions.SrcDriverType = D3D10_DRIVER_TYPE_HARDWARE;
};


///////////////////////////////////////////////////////
// Function:  ~CDXGITestFramework
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CDXGITestFramework::~CDXGITestFramework()
{
	//
	// Free D3D11RuntimeLayers.dll.
	//

	if ( m_hD3D11Layer != NULL )
	{
		m_pD3D11LayerCreateDevice = NULL;
		m_pD3D11LayerCreateDeviceAndSwapChain = NULL;

		FreeLibrary( m_hD3D11Layer );
		m_hD3D11Layer = NULL;
	};

	//
	// Free D3D10_1.dll.
	//

	if ( m_hD3D10_1 != NULL )
	{
		m_pD3D10CreateDevice1 = NULL;
		m_pD3D10CreateDeviceAndSwapChain1 = NULL;

		FreeLibrary( m_hD3D10_1 );
		m_hD3D10_1 = NULL;
	};


	//
	// Free our reference rasterizer DLL.
	//
	// @REVIEW [SEdmison]:
	// Is there a better framework cleanup function for this?
	//

	UnloadReferenceRasterizer();


	//
	// Free our software rasterizer DLL.
	//

	UnloadSoftwareRasterizer();
};


////////////////////////////////////////////////////////////
//
// Framework override methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  InitOptionVariables
//
// Purpose:
// Establishes application-wide options.  This is done
// by registering options with the test framework,
// which then handles the parsing of the command line
// and packages them up and passes the result to our
// designated member variables or callback functions.
///////////////////////////////////////////////////////
void CDXGITestFramework::InitOptionVariables()
{
	//
	// Call the base class implementation.
	//

	CGraphicsTestFramework::InitOptionVariables();


	//
	// Register Direct3D-related options.
	//
	// N.B. [SEdmison]:
	// I started with the basic list from CD3DTest,
	// and then removed the ones which I think we're
	// unlikely to need or want.
	//

	int CategoryD3D = AddOptionCategory( "D3D" );
    
	if ( m_bRegisterOrdinals == true )
	{
		//
		// Unregister framework's Adapter variable that modifies AdapterNum* variables and re-register it
		// to modify AdapterID* variables directly with ADAPTER_UNDEFINED for the default
		//

		UnRegisterOptionVariable( "Adapter" );
		RegisterOptionVariable<UINT32>( "Adapter", "Adapter ID of the adapter to use (global EnumAdapters ordinal number, no matter what AdapterType subset is chosen)", OptionCallback_AdapterID, ADAPTER_UNDEFINED, OPTION_VALUE_CONSTANT, CategoryD3D );
	};

	if ( m_bRegisterDriverType == true )
	{
		RegisterOptionVariable
		(
			"Src",
			"Source driver type",
			OptionCallback_SourceDriverType,
			tstring(_T("default")),
			OPTION_VALUE_CONSTANT,
			CategoryD3D,
			_T( "-src:[REF|HW|SW|NULL|WARP]" )
		);
	};

	if ( m_bRegisterFeatureLevel == true )
	{
		RegisterOptionVariable< tstring >
		(
			"FeatureLevel",
			"Specify Feature Level (9.1, 9.2, 9.3, 9all, 10.0, 10.1, all10, or 10_1all); default is 10.1.  "
			"Implies -DX10.1:true.",
			OptionCallback_FeatureLevel,
			tstring(_T("default")),
			OPTION_VALUE_CONSTANT,
			CategoryD3D
		);
	};

	RegisterOptionVariable
	(
		_T("DX10"),
		_T("Test the highest available Direct3D 10 version and feature level"),
		&m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE ],
		false,
		OPTION_VALUE_CONSTANT,
		CategoryD3D
	);


	RegisterOptionVariable
	(
		_T("DX10.0"),
		_T("Test Direct3D 10.0"),
		&m_RequestedFeatureLevels[ TEST_FEATURE_10_0_LEVEL_10_0 ],
		false,
		OPTION_VALUE_CONSTANT,
		CategoryD3D
	);


	RegisterOptionVariable
	(
		_T("DX10.1"),
		_T("Test Direct3D 10.1"),
		&m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ],
		false,
		OPTION_VALUE_CONSTANT,
		CategoryD3D
	);


	RegisterOptionVariable
	(
		"StrictValidation",
		"Enable Stricter Validation Layer",
		&m_D3DOptions.bStricterValidationLayer,
		false,
		OPTION_VALUE_CONSTANT,
		CategoryD3D
	);


	RegisterOptionVariable
	(
		_T("DX11"),
		_T("Test the highest available Direct3D 11 version and feature level"),
		&m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ],
		false,
		OPTION_VALUE_CONSTANT,
		CategoryD3D
	);

	RegisterOptionVariable
	(
		_T("DX11.0"),
		_T("Test Direct3D 11.0"),
		&m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_0 ],
		false,
		OPTION_VALUE_CONSTANT,
		CategoryD3D
	);
};


////////////////////////////////////////////////////////////
//
// Static helper methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  OptionCallback_FeatureLevel
//
// Purpose:
// Called by the test framework in response to a
// feature level specified on the command line.
///////////////////////////////////////////////////////
void CDXGITestFramework::OptionCallback_FeatureLevel
(
	const tstring &FeatureLevel
)
{
	//
	// Get a pointer to the framework.
	//

	CDXGITestFramework * pFramework = dynamic_cast< CDXGITestFramework * >( GetFramework() );
	assert( pFramework != NULL );


	//
	// Process the specified feature level string.
	//
	// N.B. [SEdmison]:
	// Yes, we should probably standardize on a particular
	// naming scheme for these features levels, and remove
	// some of the yuckier permutations.  However, in the
	// meantime, I'm trying to support the way that the D3D
	// core test team uses feature levels, while also
	// paving the way to some future date at which we have
	// to also keep track of D3D 11 feature levels in this
	// same system.
	//
	// Since I am creating a monster here, I suppose that
	// it's worth documenting exactly what all of these
	// switches mean.  So here goes:
	//
	// Command-line switch                      | Corresponding enum value                 | D3D10 |            D3D10_1          |            D3D11_0
	//                                          |                                          | 10_0  | 9_1   9_2   9_3  10_0  10_1 | 9_1  9_2  9_3  10_0  10_1  11_0
	// _________________________________________|__________________________________________|_______|_____________________________|________________________________
	// TEST_FEATURE_11_0_LEVEL_11_0             | TEST_FEATURE_11_0_LEVEL_11_0             |       |                             |                              X
	// TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE | TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE |       |                             |                              X
	// TEST_FEATURE_11_0_LEVEL_11_ALL           | [None.]                                  |       |                             |                              X
	// TEST_FEATURE_11_0_LEVEL_10_1             | TEST_FEATURE_11_0_LEVEL_10_1             |       |                             |                        X
	// TEST_FEATURE_11_0_LEVEL_10_0             | TEST_FEATURE_11_0_LEVEL_10_0             |       |                             |                  X
	// TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE | TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE |       |                             |                *- best -*
	// TEST_FEATURE_11_0_LEVEL_10_ALL           | [None.]                                  |       |                             |                  X     X
	// TEST_FEATURE_11_0_LEVEL_9_3              | TEST_FEATURE_11_0_LEVEL_9_3              |       |                             |            X
	// TEST_FEATURE_11_0_LEVEL_9_2              | TEST_FEATURE_11_0_LEVEL_9_2              |       |                             |       X
	// TEST_FEATURE_11_0_LEVEL_9_1              | TEST_FEATURE_11_0_LEVEL_9_1              |       |                             |  X
	// TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE  | TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE  |       |                             | *---- best ---*
	// TEST_FEATURE_11_0_LEVEL_9_ALL            | [None.]                                  |       |                             |  X    X    X
	// TEST_FEATURE_11_ALL_LEVEL_BESTAVAILABLE  | TEST_FEATURE_11_ALL_LEVEL_BESTAVAILABLE  |       |                             | *------------ best -----------*
	// TEST_FEATURE_11_ALL_LEVEL_ALL            | [None.]                                  |       |                             |  X    X    X     X     X     X
	// TEST_FEATURE_10_1_LEVEL_10_1             | TEST_FEATURE_10_1_LEVEL_10_1             |       |                          X  |
	// TEST_FEATURE_10_1_LEVEL_10_0             | TEST_FEATURE_10_1_LEVEL_10_0             |       |                    X        |
	// TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE | TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE |       |                  *- best -* |
	// TEST_FEATURE_10_1_LEVEL_10_ALL           | [None.]                                  |       |                    X     X  |
	// TEST_FEATURE_10_1_LEVEL_9_3              | TEST_FEATURE_10_1_LEVEL_9_3              |       |              X              |
	// TEST_FEATURE_10_1_LEVEL_9_2              | TEST_FEATURE_10_1_LEVEL_9_2              |       |        X                    |
	// TEST_FEATURE_10_1_LEVEL_9_1              | TEST_FEATURE_10_1_LEVEL_9_1              |       |  X                          |
	// TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE  | TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE  |       | *---- best ----*            |
	// TEST_FEATURE_10_1_LEVEL_9_ALL            | [None.]                                  |       |  X     X     X
	// TEST_FEATURE_10_0_LEVEL_10_0             | TEST_FEATURE_10_0_LEVEL_10_0             |   X   |                             |
	// TEST_FEATURE_10_ALL_LEVEL_BESTAVAILABLE  | TEST_FEATURE_10_ALL_LEVEL_BESTAVAILABLE  | *-------------- best -------------* |
	// TEST_FEATURE_10_ALL_LEVEL_ALL            | [None.]                                  |   X   |  X     X     X     X     X  |
	// TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE     | TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE     | *-------------- best -------------* OR *----------- best -----------*
	// TEST_FEATURE_ALL_LEVEL_ALL               | [None.]                                  |   X   |  X     X     X     X     X  OR X    X    X     X     X     X
	//
	// In the case of the BEST entries in this table, we
	// select the highest feature level available on the
	// machine at runtime and use that.  We don't run
	// multiple feature levels.


	// Default level passed by the framework at startup.
	if
	(
		FeatureLevel == _T("default")
	)
	{
		// Just ignore this one.
	}


	//////////////////////////////////////////////////
	//
	// 11.0 feature levels
	//
	//////////////////////////////////////////////////

	// 11.0 level 11_0
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_11_0") ||
		FeatureLevel == _T("D3D_FEATURE_LEVEL_11_0") ||

		FeatureLevel == _T("11.0level11.0") ||

		FeatureLevel == _T("11_0_level_11.0") ||
		FeatureLevel == _T("11_0_level_11_0") ||
		FeatureLevel == _T("11_0level11_0") ||
		FeatureLevel == _T("11_level_11.0") ||
		FeatureLevel == _T("11_level_11_0") ||

		FeatureLevel == _T("11level11.0") ||
		FeatureLevel == _T("11level11_0") ||

		FeatureLevel == _T("11.0")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 11 all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_11_ALL") ||

		FeatureLevel == _T("11.0level11all") ||
		FeatureLevel == _T("11.0level_11all") ||
		FeatureLevel == _T("11.0level_11_all") ||
		FeatureLevel == _T("11.0_level_11_all") ||

		FeatureLevel == _T("11all") ||
		FeatureLevel == _T("11.all") ||
		FeatureLevel == _T("11.*")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 10_1
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_10_1") ||

		FeatureLevel == _T("11.0level10.1") ||

		FeatureLevel == _T("11_0_level_10.1") ||
		FeatureLevel == _T("11_0_level_10_1") ||
		FeatureLevel == _T("11_0level10_1") ||
		FeatureLevel == _T("11_level_10.1") ||
		FeatureLevel == _T("11_level_10_1") ||

		FeatureLevel == _T("11level10.1") ||
		FeatureLevel == _T("11level10_1")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 10_0
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_10_0") ||

		FeatureLevel == _T("11.0level10.0") ||

		FeatureLevel == _T("11_0level10_0") ||
		FeatureLevel == _T("11_0level10.0") ||
		FeatureLevel == _T("11_0level10_0") ||
		FeatureLevel == _T("11_level_10.0") ||
		FeatureLevel == _T("11_level_10_0") ||

		FeatureLevel == _T("11level10.0") ||
		FeatureLevel == _T("11level10_0")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 10 best available
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE" ) ||

		FeatureLevel == _T("11.0level10best") ||
		FeatureLevel == _T("11_0level10best") ||
		FeatureLevel == _T("11.0_level_10_best") ||
		FeatureLevel == _T("11_0_level_10_best") ||
		FeatureLevel == _T("11.0_level_10best") ||
		FeatureLevel == _T("11_0_level_10best") ||
		FeatureLevel == _T("11.0level10") ||
		FeatureLevel == _T("11_0level10") ||
		FeatureLevel == _T("11.0_level_10") ||
		FeatureLevel == _T("11_0_level_10")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 10 all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_10_ALL") ||

		FeatureLevel == _T("11.0level10all") ||
		FeatureLevel == _T("11.0_level_10all") ||
		FeatureLevel == _T("11.0_level_10_all") ||
		FeatureLevel == _T("11_0_level_10all") ||
		FeatureLevel == _T("11_0_level_10_all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 9_3
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_9_3") ||

		FeatureLevel == _T("11.0level9.3") ||

		FeatureLevel == _T("11_0level9_3") ||
		FeatureLevel == _T("11_0level9.3") ||
		FeatureLevel == _T("11_0level9_3") ||
		FeatureLevel == _T("11_level_9.3") ||
		FeatureLevel == _T("11_level_9_3") ||

		FeatureLevel == _T("11level9.3") ||
		FeatureLevel == _T("11level9_3")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_3 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 9_2
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_9_2") ||

		FeatureLevel == _T("11.0level9.2") ||

		FeatureLevel == _T("11_0level9_2") ||
		FeatureLevel == _T("11_0level9.2") ||
		FeatureLevel == _T("11_0level9_2") ||
		FeatureLevel == _T("11_level_9.2") ||
		FeatureLevel == _T("11_level_9_2") ||

		FeatureLevel == _T("11level9.2") ||
		FeatureLevel == _T("11level9_2")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_2 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 9_1
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_9_1") ||

		FeatureLevel == _T("11.0level9.1") ||

		FeatureLevel == _T("11_0level9_1") ||
		FeatureLevel == _T("11_0level9.1") ||
		FeatureLevel == _T("11_0level9_1") ||
		FeatureLevel == _T("11_level_9.1") ||
		FeatureLevel == _T("11_level_9_1") ||

		FeatureLevel == _T("11level9.1") ||
		FeatureLevel == _T("11level9_1")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 9 best available
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE") ||

		FeatureLevel == _T("11.0_level_9_best") ||
		FeatureLevel == _T("11_0_level_9_best") ||
		FeatureLevel == _T("11.0_level_9best") ||
		FeatureLevel == _T("11_0_level_9best") ||
		FeatureLevel == _T("11.0level9") ||
		FeatureLevel == _T("11.0_level_9") ||
		FeatureLevel == _T("11_0level9") ||
		FeatureLevel == _T("11_0_level_9")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 9 all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_9_ALL") ||

		FeatureLevel == _T("11.0level9all") ||
		FeatureLevel == _T("11.0_level_9all") ||
		FeatureLevel == _T("11_0_level_9all") ||
		FeatureLevel == _T("11_0_level_9_all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_2 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_3 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.* level all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_ALL") ||
		FeatureLevel == _T("11levelall") ||
		FeatureLevel == _T("11_level_all") ||
		FeatureLevel == _T("11_levelall") ||

		FeatureLevel == _T("11all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// ? level 10.1
	else if
	(
		FeatureLevel == _T("10.1") ||
		FeatureLevel == _T("10_1")
	)
	{
		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
		}
		else
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_1 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
		};
	}
	// ? level 10.0
	else if
	(
		FeatureLevel == _T("10.0") ||
		FeatureLevel == _T("10_0")
	)
	{
		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
		}
		else
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_0 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
		};
	}
	// ? level 9.3
	else if
	(
		FeatureLevel == _T("9.3") ||
		FeatureLevel == _T("9_3")
	)
	{
		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_3 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
		}
		else
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_3 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
		};
	}
	// ? level 9.2
	else if
	(
		FeatureLevel == _T("9.2") ||
		FeatureLevel == _T("9_2")
	)
	{
		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_2 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
		}
		else
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_2 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
		};
	}
	// ? level 9.1
	else if
	(
		FeatureLevel == _T("9.1") ||
		FeatureLevel == _T("9_1")
	)
	{
		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_1 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
		}
		else
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_1 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
		};
	}
	// ? level 10 best available
	else if
	(
		FeatureLevel == _T( "10" ) ||
		FeatureLevel == _T( "10best") ||
		FeatureLevel == _T( "10.best" ) ||
		FeatureLevel == _T( "10_best" )
	)
	{
		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
		}
		else
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
		};
	}
	// ? level 10 all
	else if
	(
		FeatureLevel == _T("10all") ||
		FeatureLevel == _T("10.all") ||
		FeatureLevel == _T("10_all") ||
		FeatureLevel == _T("10.*") ||
		FeatureLevel == _T("10_*")
	)
	{
		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] = true;
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
		}
		else
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_1 ] = true;
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_0 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
		};
	}
	// ? level 9 best available
	else if
	(
		FeatureLevel == _T( "9" ) ||
		FeatureLevel == _T( "9best") ||
		FeatureLevel == _T( "9.best" ) ||
		FeatureLevel == _T( "9_best")
	)
	{
		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
		}
		else
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
		};
	}
	// ? level 9 all
	else if
	(
		FeatureLevel == _T("9all") ||
		FeatureLevel == _T("9.all") ||
		FeatureLevel == _T("9_all") ||
		FeatureLevel == _T("9.*") ||
		FeatureLevel == _T("9_*")
	)
	{
		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_3 ] = true;
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_2 ] = true;
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_1 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
		}
		else
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_3 ] = true;
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_2 ] = true;
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_1 ] = true;

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
		};
	}
	// All features, best available level
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	else if
	(
		FeatureLevel == _T("bestavailable") ||
		FeatureLevel == _T("all_level_bestavailable") ||
		FeatureLevel == _T("all_level_best") ||
		FeatureLevel == _T("best")
	)
	{
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] == false &&
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == false
		)
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = true;
		}
	}
	// all level all
	else if
	(
		FeatureLevel == _T( "all" ) ||
		FeatureLevel == _T( "*")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_0 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_3 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_2 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_0 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_3 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_2 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_0_LEVEL_10_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}


	//////////////////////////////////////////////////
	//
	// 10.1 feature levels
	//
	//////////////////////////////////////////////////

	// 10.1 level 10_1
	if  
	(
		FeatureLevel == _T("TEST_FEATURE_10_1_LEVEL_10_1") ||
		FeatureLevel == _T("D3D10_FEATURE_LEVEL_10_1") ||
		FeatureLevel == _T("10.1level10.1") ||
		FeatureLevel == _T("10.1_level_10.1") ||
		FeatureLevel == _T("10.1_level_10_1") ||
		FeatureLevel == _T("10_1level10_1") ||
		FeatureLevel == _T("10_1_level_10_1")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.1 level 10_0
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_1_LEVEL_10_0") ||
		FeatureLevel == _T("D3D10_FEATURE_LEVEL_10_0") ||
		FeatureLevel == _T("10.1level10.0") ||
		FeatureLevel == _T("10.1_level_10.0") ||
		FeatureLevel == _T("10.1_level_10_0") ||
		FeatureLevel == _T("10_1level10_0") ||
		FeatureLevel == _T("10_1_level_10_0")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.1 level 10 best available
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE" ) ||
		FeatureLevel == _T("10.1level10best") ||
		FeatureLevel == _T("10_1level10best") ||
		FeatureLevel == _T("10.1_level_10_best") ||
		FeatureLevel == _T("10_1_level_10_best") ||
		FeatureLevel == _T("10.1_level_10best") ||
		FeatureLevel == _T("10_1_level_10best") ||
		FeatureLevel == _T("10.1level10") ||
		FeatureLevel == _T("10_1level10") ||
		FeatureLevel == _T("10.1_level_10") ||
		FeatureLevel == _T("10_1_level_10")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.1 level 10 all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_1_LEVEL_10_ALL") ||
		FeatureLevel == _T("10.1level10all") ||
		FeatureLevel == _T("10.1_level_10all") ||
		FeatureLevel == _T("10.1_level_10_all") ||
		FeatureLevel == _T("10_1_level_10all") ||
		FeatureLevel == _T("10_1_level_10_all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.1 level 9_3
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_1_LEVEL_9_3") ||
		FeatureLevel == _T("D3D10_FEATURE_LEVEL_9_3") ||
		FeatureLevel == _T("10.1level9.3") ||
		FeatureLevel == _T("10.1_level_9.3") ||
		FeatureLevel == _T("10.1_level_9_3") ||
		FeatureLevel == _T("10_1level9_3") ||
		FeatureLevel == _T("10_1_level_9_3")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_3 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.1 level 9_2
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_1_LEVEL_9_2") ||
		FeatureLevel == _T("D3D10_FEATURE_LEVEL_9_2") ||
		FeatureLevel == _T("10.1level9.2") ||
		FeatureLevel == _T("10.1_level_9.2") ||
		FeatureLevel == _T("10.1_level_9_2") ||
		FeatureLevel == _T("10_1level9_2") ||
		FeatureLevel == _T("10_1_level_9_2")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_2 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.1 level 9_1
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_1_LEVEL_9_1") ||
		FeatureLevel == _T("D3D10_FEATURE_LEVEL_9_1") ||
		FeatureLevel == _T("10.1level9.1") ||
		FeatureLevel == _T("10.1_level_9.1") ||
		FeatureLevel == _T("10.1_level_9_1") ||
		FeatureLevel == _T("10_1level9_1") ||
		FeatureLevel == _T("10_1_level_9_1")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.1 level 9 best available
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE") ||
		FeatureLevel == _T("10.1level9") ||
		FeatureLevel == _T("10.1_level_9") ||
		FeatureLevel == _T("10_1level9") ||
		FeatureLevel == _T("10_1_level_9")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.1 level 9 all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_1_LEVEL_9_ALL") ||
		FeatureLevel == _T("10.1level9all") ||
		FeatureLevel == _T("10.1_level_9all") ||
		FeatureLevel == _T("10_1_level_9all") ||
		FeatureLevel == _T("10_1_level_9_all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_3 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_2 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.1 level all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_1_LEVEL_ALL") ||
		FeatureLevel == _T("10.1all") ||
		FeatureLevel == _T("10.1_all") ||
		FeatureLevel == _T("10.1_level_all") ||
		FeatureLevel == _T("10_1_all") ||
		FeatureLevel == _T("10_1all") ||
		FeatureLevel == _T("10_1_level_all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_0 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_3 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_2 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.* level 10.*
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_ALL_LEVEL_10_ALL") ||
		FeatureLevel == _T("10alllevel10all") ||
		FeatureLevel == _T("10all_level_10all") ||
		FeatureLevel == _T("10_all_level_10_all") ||
		FeatureLevel == _T("10_*_level_10_*") ||
		FeatureLevel == _T("10.*_level_10.*")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_0_LEVEL_10_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}
	// 10.* level all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_ALL_LEVEL_ALL") ||
		FeatureLevel == _T("10alllevelall") ||
		FeatureLevel == _T("10all_level_all") ||
		FeatureLevel == _T("10_all_level_all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_0 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_3 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_2 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_0_LEVEL_10_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}


	//////////////////////////////////////////////////
	//
	// 10.0 feature levels
	//
	//////////////////////////////////////////////////

	// 10.0 level 10_0
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_10_0_LEVEL_10_0") ||
		FeatureLevel == _T("TEST_FEATURE_10_0_LEVEL_ALL") ||
		FeatureLevel == _T("10level10") ||
		FeatureLevel == _T("10_0_level_10_0") ||
		FeatureLevel == _T("10.0_level_10.0") ||
		FeatureLevel == _T("10.0_level_10_0")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_0_LEVEL_10_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;
	}


	//////////////////////////////////////////////////
	//
	// 11.0 feature levels
	//
	//////////////////////////////////////////////////

	// 11.0 level 11_0
	if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_11_0") ||
		FeatureLevel == _T("D3D_FEATURE_LEVEL_11_0") ||

		FeatureLevel == _T("11.0level11.0") ||

		FeatureLevel == _T("11_0_level_11.0") ||
		FeatureLevel == _T("11_0_level_11_0") ||
		FeatureLevel == _T("11_0level11_0") ||
		FeatureLevel == _T("11_level_11.0") ||
		FeatureLevel == _T("11_level_11_0") ||

		FeatureLevel == _T("11level11.0") ||
		FeatureLevel == _T("11level11_0") ||

		FeatureLevel == _T("11.0")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 11 all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_11_ALL") ||

		FeatureLevel == _T("11.0level11all") ||
		FeatureLevel == _T("11.0level_11all") ||
		FeatureLevel == _T("11.0level_11_all") ||
		FeatureLevel == _T("11.0_level_11_all") ||

		FeatureLevel == _T("11all") ||
		FeatureLevel == _T("11.all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 10_1
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_10_1") ||

		FeatureLevel == _T("11.0level10.1") ||

		FeatureLevel == _T("11_0_level_10.1") ||
		FeatureLevel == _T("11_0_level_10_1") ||
		FeatureLevel == _T("11_0level10_1") ||
		FeatureLevel == _T("11_level_10.1") ||
		FeatureLevel == _T("11_level_10_1") ||

		FeatureLevel == _T("11level10.1") ||
		FeatureLevel == _T("11level10_1")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 10_0
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_10_0") ||

		FeatureLevel == _T("11.0level10.0") ||

		FeatureLevel == _T("11_0level10_0") ||
		FeatureLevel == _T("11_0level10.0") ||
		FeatureLevel == _T("11_0level10_0") ||
		FeatureLevel == _T("11_level_10.0") ||
		FeatureLevel == _T("11_level_10_0") ||

		FeatureLevel == _T("11level10.0") ||
		FeatureLevel == _T("11level10_0")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 10 best available
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE" ) ||

		FeatureLevel == _T("11.0level10best") ||
		FeatureLevel == _T("11_0level10best") ||
		FeatureLevel == _T("11.0_level_10_best") ||
		FeatureLevel == _T("11_0_level_10_best") ||
		FeatureLevel == _T("11.0_level_10best") ||
		FeatureLevel == _T("11_0_level_10best") ||
		FeatureLevel == _T("11.0level10") ||
		FeatureLevel == _T("11_0level10") ||
		FeatureLevel == _T("11.0_level_10") ||
		FeatureLevel == _T("11_0_level_10")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 10 all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_10_ALL") ||

		FeatureLevel == _T("11.0level10all") ||
		FeatureLevel == _T("11.0_level_10all") ||
		FeatureLevel == _T("11.0_level_10_all") ||
		FeatureLevel == _T("11_0_level_10all") ||
		FeatureLevel == _T("11_0_level_10_all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 9_3
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_9_3") ||

		FeatureLevel == _T("11.0level9.3") ||

		FeatureLevel == _T("11_0level9_3") ||
		FeatureLevel == _T("11_0level9.3") ||
		FeatureLevel == _T("11_0level9_3") ||
		FeatureLevel == _T("11_level_9.3") ||
		FeatureLevel == _T("11_level_9_3") ||

		FeatureLevel == _T("11level9.3") ||
		FeatureLevel == _T("11level9_3")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_3 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 9_2
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_9_2") ||

		FeatureLevel == _T("11.0level9.2") ||

		FeatureLevel == _T("11_0level9_2") ||
		FeatureLevel == _T("11_0level9.2") ||
		FeatureLevel == _T("11_0level9_2") ||
		FeatureLevel == _T("11_level_9.2") ||
		FeatureLevel == _T("11_level_9_2") ||

		FeatureLevel == _T("11level9.2") ||
		FeatureLevel == _T("11level9_2")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_2 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 9_1
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_9_1") ||

		FeatureLevel == _T("11.0level9.1") ||

		FeatureLevel == _T("11_0level9_1") ||
		FeatureLevel == _T("11_0level9.1") ||
		FeatureLevel == _T("11_0level9_1") ||
		FeatureLevel == _T("11_level_9.1") ||
		FeatureLevel == _T("11_level_9_1") ||

		FeatureLevel == _T("11level9.1") ||
		FeatureLevel == _T("11level9_1")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_1 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 9 best available
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE") ||

		FeatureLevel == _T("11.0_level_9_best") ||
		FeatureLevel == _T("11_0_level_9_best") ||
		FeatureLevel == _T("11.0_level_9best") ||
		FeatureLevel == _T("11_0_level_9best") ||
		FeatureLevel == _T("11.0level9") ||
		FeatureLevel == _T("11.0_level_9") ||
		FeatureLevel == _T("11_0level9") ||
		FeatureLevel == _T("11_0_level_9")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.0 level 9 all
	else if
	(
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_9_ALL") ||

		FeatureLevel == _T("11.0level9all") ||
		FeatureLevel == _T("11.0_level_9all") ||
		FeatureLevel == _T("11_0_level_9all") ||
		FeatureLevel == _T("11_0_level_9_all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_2 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_3 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	}
	// 11.* level all
	else if
	(   
		FeatureLevel == _T("TEST_FEATURE_11_0_LEVEL_ALL") ||
		FeatureLevel == _T("11levelall") ||
		FeatureLevel == _T("11_level_all") ||
		FeatureLevel == _T("11_levelall") ||

		FeatureLevel == _T("11all")
	)
	{
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] = true;
		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_0 ] = true;

		pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;
	};
};


///////////////////////////////////////////////////////
// Function:  OptionCallback_RasterizerDLL
//
// Purpose:
// Called by the test framework in response to a
// rasterizer DLL name specified on the command line.
///////////////////////////////////////////////////////
void CDXGITestFramework::OptionCallback_RasterizerDLL
(
	const tstring &RasterizerDLL
)
{
	CDXGITestFramework * pFramework = dynamic_cast< CDXGITestFramework * >( GetFramework() );

#if defined(UNICODE)
	StringCchCopyW
	(
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName,
		ARRAYSIZE( pFramework->m_D3DOptions.SoftwareRasterizerDLLName ),
		RasterizerDLL.c_str()
	);
#else
	MultiByteToWideChar
	(
		CP_ACP,
		MB_ERR_INVALID_CHARS,
		RasterizerDLL.c_str(),
		-1,
		pFramework->m_D3DOptions.SoftwareRasterizerDLLName,
		ARRAYSIZE( pFramework->m_D3DOptions.SoftwareRasterizerDLLName )
	);
#endif
};


///////////////////////////////////////////////////////
// Function:  OptionCallback_SourceDriverType
//
// Purpose:
// Called by the test framework in response to a
// source driver type name specified on the command
// line.
///////////////////////////////////////////////////////
void CDXGITestFramework::OptionCallback_SourceDriverType
(
	const tstring &SourceDriverType
)
{
	CDXGITestFramework * pFramework = dynamic_cast< CDXGITestFramework * >( GetFramework() );

	if
	(
		SourceDriverType == _T( "default" )
	)
	{
		pFramework->m_D3DOptions.DriverType = D3D10_DRIVER_TYPE_HARDWARE;
	}
	else if
	(
		SourceDriverType == _T( "D3D10_DRIVER_TYPE_HARDWARE" ) ||
		SourceDriverType == _T( "HW" ) ||
		SourceDriverType == _T( "hw" )
	)
	{
		pFramework->m_D3DOptions.DriverType = D3D10_DRIVER_TYPE_HARDWARE;
	}
	else if
	(
		SourceDriverType == _T( "D3D10_DRIVER_TYPE_SOFTWARE" ) ||
		SourceDriverType == _T( "SW" ) ||
		SourceDriverType == _T( "sw" )
	)
	{
		pFramework->m_D3DOptions.DriverType = D3D10_DRIVER_TYPE_SOFTWARE;
	}
	else if
	(
		SourceDriverType == _T( "D3D10_DRIVER_TYPE_REFERENCE" ) ||
		SourceDriverType == _T( "REF" ) ||
		SourceDriverType == _T( "ref" )
	)
	{
		pFramework->m_D3DOptions.DriverType = D3D10_DRIVER_TYPE_REFERENCE;
	}
	else if
	(
		SourceDriverType == _T( "D3D10_DRIVER_TYPE_NULL" ) ||
		SourceDriverType == _T( "NULL" ) ||
		SourceDriverType == _T( "null" )
	)
	{
		pFramework->m_D3DOptions.DriverType = D3D10_DRIVER_TYPE_NULL;
	}
	else if
	(
		SourceDriverType == _T( "D3D10_DRIVER_TYPE_WARP" ) ||
		SourceDriverType == _T( "WARP" ) ||
		SourceDriverType == _T( "warp" )
	)
	{
		pFramework->m_D3DOptions.DriverType = D3D10_DRIVER_TYPE_SOFTWARE;

		StringCchCopyW
		(
			pFramework->m_D3DOptions.SoftwareRasterizerDLLName,
			ARRAYSIZE( pFramework->m_D3DOptions.SoftwareRasterizerDLLName ),
			L"d3d10warp.dll"
		);
	}
	else
	{
		pFramework->m_D3DOptions.DriverType = D3D10_DRIVER_TYPE_HARDWARE;
	};
};


///////////////////////////////////////////////////////
// Function:  UpdateCurrentDeviceInfo
//
// Purpose:
// Updates device information for the log.
///////////////////////////////////////////////////////
bool CDXGITestFramework::UpdateCurrentDeviceInfo
(
	__out DXGLOG_DEVICEINFO * pDeviceInfo
)
{
	//
	// Initialize local variables.
	//

	bool bOverallResult = false;

	HMODULE hD3D10Ref = NULL;

	DXGI_FACTORY_POINTER Factory;
	ZeroMemory( &Factory, sizeof( Factory ) );

	IDXGIAdapter * pAdapter = NULL;


	//
	// Validate parameters.
	//

	if ( pDeviceInfo == NULL )
	{
		WriteToLog
		(
			_T( "UpdateCurrentDeviceInfo:  Device info pointer is NULL." )
		);

		goto Cleanup;
	};


	//
	// Load D3D10Ref.dll.
	//

	hD3D10Ref = LoadLibraryExW
	(
		D3D_REF_DLLNAME,
		NULL,
		0
	);

	if ( hD3D10Ref == NULL )
	{
		WriteToLogF
		(
			L"UpdateCurrentDeviceInfo:  Failed to load reference rasterizer DLL %s.",
			D3D_REF_DLLNAME
		);

		goto Cleanup;
	};


	//
	// Create a DXGI factory.
	//

	HRESULT hLoadFactoryResult = CreateDXGIFactory
	(
		&Factory
	);

	if ( FAILED( hLoadFactoryResult ) )
	{
		WriteToLogF
		(
			_T( "UpdateCurrentDeviceInfo:  CreateDXGIFactory failed with error 0x%x." ),
			hLoadFactoryResult
		);

		goto Cleanup;
	};


	switch ( m_D3DOptions.SrcDriverType )
	{
		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			//
			// Enumerate the first adapter.
			//

			HRESULT hEnumerateAdapterSlotResult = Factory.pFactory->EnumAdapters
			(
				0,
				&pAdapter
			);

			if ( FAILED( hEnumerateAdapterSlotResult ) )
			{
				WriteToLog
				(
					_T( "EnumAdapters failed." )
				);

				goto Cleanup;
			};

			if ( pAdapter == NULL )
			{
				WriteToLog
				(
					_T( "EnumAdapters succeeded, but adapter pointer is NULL." )
				);

				goto Cleanup;
			};
		} break;


		case D3D10_DRIVER_TYPE_SOFTWARE:
		case D3D10_DRIVER_TYPE_REFERENCE:
		case D3D10_DRIVER_TYPE_NULL:
		{
			//
			// Create a software adapter.
			//

			HRESULT hCreateAdapterResult = Factory.pFactory->CreateSoftwareAdapter
			(
				hD3D10Ref,
				&pAdapter
			);

			if ( FAILED( hCreateAdapterResult ) )
			{
				WriteToLog
				(
					_T( "CreateSoftwareAdapter failed." )
				);

				goto Cleanup;
			};


			if ( pAdapter == NULL )
			{
				WriteToLog
				(
					_T( "CreateSoftwareAdapter succeeded, but adapter pointer is NULL." )
				);

				goto Cleanup;
			};


			HRESULT hSetPrivateDataResult = pAdapter->SetPrivateData
			(
				GUID_DeviceType,
				sizeof( m_D3DOptions.SrcDriverType ),
				&m_D3DOptions.SrcDriverType
			);

			if ( FAILED( hSetPrivateDataResult ) )
			{
				WriteToLog
				(
					_T( "SetPrivateData failed." )
				);

				goto Cleanup;
			};
		} break;


		default:
		{
			assert( FALSE );
		} break;
	};


	//
	// Get a descriptor for the adapter.
	//

	assert( pAdapter != NULL );
	DXGI_ADAPTER_DESC AdapterDescriptor;
	ZeroMemory( &AdapterDescriptor, sizeof( AdapterDescriptor ) );
	HRESULT hGetAdapterDescriptorResult = pAdapter->GetDesc
	(
		&AdapterDescriptor
	);

	if ( FAILED( hGetAdapterDescriptorResult ) )
	{
		WriteToLogF
		(
			_T( "UpdateCurrentDeviceInfo:  IDXGIAdapter::GetDesc failed with error %u (0x%x)." ),
			hGetAdapterDescriptorResult,
			hGetAdapterDescriptorResult
		);

		pAdapter->Release();
		pAdapter = NULL;

		goto Cleanup;
	};


	//
	// Fill in DXGLOG parameters.
	//

	pDeviceInfo->fMinDXVersion = 10.0f;
	pDeviceInfo->fMaxDXVersion = 10.0f;
	pDeviceInfo->bPSGP = false;

	pDeviceInfo->sDisplay = new TCHAR[ 128 ];
	if ( pDeviceInfo->sDisplay == NULL )
	{
		return false;
	};
#ifdef UNICODE
	StringCchCopy
	(
		pDeviceInfo->sDisplay,
		128,
		AdapterDescriptor.Description
	);
#else
	int cCharactersConverted = WideCharToMultiByte
	(
		CP_ACP,
		WC_NO_BEST_FIT_CHARS,
		AdapterDescriptor.Description,
		-1,
		pDeviceInfo->sDisplay,
		128,
		NULL,
		NULL
	);

	assert( cCharactersConverted > 0 );
#endif


	//
	// Fill in the user mode driver version.
	//

	LARGE_INTEGER userModeDriverVersion;
	ZeroMemory( &userModeDriverVersion, sizeof(userModeDriverVersion) );
	if( SUCCEEDED( pAdapter->CheckInterfaceSupport( __uuidof( IDXGIDevice ), &userModeDriverVersion ) ) )
	{
		pDeviceInfo->sDriverVersion = new TCHAR[ 256 ];
		if ( pDeviceInfo->sDriverVersion == NULL )
		{
			return false;
		};
		StringCchPrintf
		(
			pDeviceInfo->sDriverVersion,
			256,
			_T( "%d.%d.%d.%d" ),
			HIWORD( userModeDriverVersion.HighPart ), // product
			LOWORD( userModeDriverVersion.HighPart ), // version
			HIWORD( userModeDriverVersion.LowPart ),  // sub-version
			LOWORD( userModeDriverVersion.LowPart ) // build
		);
	}

// @TODO [SEdmison]:
/*
	pDeviceInfo->sDriver = new TCHAR[ MAX_DEVICE_IDENTIFIER_STRING ];
	if ( pDeviceInfo->sDriver == NULL )
	{
		return false;
	};
#ifdef UNICODE
	cCharactersConverted = MultiByteToWideChar
	(
		CP_ACP,
		0,
		AdapterIdentifier.Driver,
		-1,
		pDeviceInfo->sDriver,
		MAX_DEVICE_IDENTIFIER_STRING
	);
#else
	StringCchCopy
	(
		pDeviceInfo->sDriver,
		MAX_DEVICE_IDENTIFIER_STRING,
		AdapterIdentifier.Driver
	);
#endif
*/

	pDeviceInfo->bDriverDebug = false;
	pDeviceInfo->dwVendorID = AdapterDescriptor.VendorId;
	pDeviceInfo->dwDeviceID = AdapterDescriptor.DeviceId;


	pDeviceInfo->sPnPID = new TCHAR[ 48 ];
	if ( pDeviceInfo->sPnPID == NULL )
	{
		return false;
	};
	StringCchPrintf
	(
		pDeviceInfo->sPnPID,
		48,
		_T("VEN_%04X&DEV_%04X&SUBSYS_%08X&REV_%02X"),
		AdapterDescriptor.VendorId,
		AdapterDescriptor.DeviceId,
		AdapterDescriptor.SubSysId,
		AdapterDescriptor.Revision
	);


	pDeviceInfo->sResolution = new TCHAR[ 32 ];
	if ( pDeviceInfo->sResolution == NULL )
	{
		return false;
	};
	StringCchPrintf
	(
		pDeviceInfo->sResolution,
		32,
		_T( "%u x %u x %s @ %u Hz" ),
		1024,
		768,
		_T( "X8R8G8B8" ),
		60
	);


	pDeviceInfo->nDDI = 10;
	pDeviceInfo->fVersion = 10.0f;
	pDeviceInfo->fBuild = GetSDKBuild();
	pDeviceInfo->bDebug = false;
	pDeviceInfo->fInterface = 10.0f;


	pDeviceInfo->sDevice = new TCHAR[ 256 ];
	if ( pDeviceInfo->sDevice == NULL )
	{
		return false;
	};

	switch ( m_D3DOptions.SrcDriverType )
	{
		case D3D10_DRIVER_TYPE_REFERENCE:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "REF" )
			);
		} break;

		case D3D10_DRIVER_TYPE_NULL:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "NULL" )
			);
		} break;

		case D3D10_DRIVER_TYPE_SOFTWARE:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "SW" )
			);
		} break;

		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			StringCchPrintf
			(
				pDeviceInfo->sDevice,
				256,
				_T( "HW" )
			);
		} break;

		default:
		{
			assert( FALSE );
		} break;
	};


	pDeviceInfo->dwDispMode = 0;
	pDeviceInfo->wDesktopBPP = 32;


	//
	// If we made it this far, we're good to go.
	//

	bOverallResult = true;


Cleanup:
	if ( pAdapter != NULL )
	{
		pAdapter->Release();
		pAdapter = NULL;
	};


	if ( Factory.pFactory != NULL )
	{
		Factory.pFactory->Release();
		Factory.pFactory = NULL;
	};


	if ( hD3D10Ref != NULL )
	{
		FreeLibrary( hD3D10Ref );
		hD3D10Ref = NULL;
	};


	return bOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateAdapter
//
// Purpose:
// Creates a DXGI adapter.
///////////////////////////////////////////////////////
HRESULT CDXGITestFramework::CreateAdapter
(
	DXGI_FACTORY_POINTER FactoryPointer,
	UINT AdapterOrdinal,
	__out DXGI_ADAPTER_POINTER * pAdapterPointer
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	HMODULE hSoftwareRasterizer = NULL;


	//
	// Validate parameters.
	//

	if ( FactoryPointer.pFactory == NULL )
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if ( pAdapterPointer == NULL )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};


	assert( pAdapterPointer->pAdapter == NULL );
	if ( pAdapterPointer->pAdapter != NULL )
	{
		WriteToLogF
		(
			_T( "CDXGITestFramework::CreateAdapter:  pAdapterPointer points to a non-NULL adapter pointer, which would otherwise have caused a leak." )
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	pAdapterPointer->pAdapter = NULL;


	switch ( m_D3DOptions.SrcDriverType )
	{
		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			hOverallResult = FactoryPointer.pFactory->EnumAdapters
			(
				AdapterOrdinal,
				&pAdapterPointer->pAdapter
			);

			goto Cleanup;
		} break;


		case D3D10_DRIVER_TYPE_SOFTWARE:
		{
			hSoftwareRasterizer = LoadSoftwareRasterizer();
			if ( hSoftwareRasterizer == NULL )
			{
				WriteToLog
				(
					_T( "CDXGITestFramework::CreateAdapter:  LoadSoftwareRasterizer failed." )
				);

				hOverallResult = HRESULT_FROM_WIN32( ERROR_MOD_NOT_FOUND );
				goto Cleanup;
			};


			hOverallResult = FactoryPointer.pFactory->CreateSoftwareAdapter
			(
				hSoftwareRasterizer,
				&pAdapterPointer->pAdapter
			);

			goto Cleanup;
		} break;


		case D3D10_DRIVER_TYPE_NULL:
		case D3D10_DRIVER_TYPE_REFERENCE:
		{
			HRESULT hCreateSoftwareAdapterResult = FactoryPointer.pFactory->CreateSoftwareAdapter
			(
				LoadReferenceRasterizer(),
				&pAdapterPointer->pAdapter
			);

			if ( FAILED( hCreateSoftwareAdapterResult ) )
			{
				hOverallResult = hCreateSoftwareAdapterResult;
				goto Cleanup;
			};

			if ( pAdapterPointer->pAdapter == NULL )
			{
				hOverallResult = E_FAIL;
				goto Cleanup;
			};


			HRESULT hSetPrivateDataResult = pAdapterPointer->pAdapter->SetPrivateData
			(
				GUID_DeviceType,
				sizeof( m_D3DOptions.SrcDriverType ),
				&m_D3DOptions.SrcDriverType
			);

			if ( FAILED( hSetPrivateDataResult ) )
			{
				hOverallResult = hSetPrivateDataResult;
				goto Cleanup;
			};
		} break;


		default:
		{
			assert( FALSE );
		} break;
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateDevice
//
// Purpose:
// Provides a simple wrapper for the D3D10CreateDevice
// and D3D10CreateDevice1 entry points, but abstracted
// to use a TEST_FEATURE_LEVEL instead of the actual
// D3D10_FEATURE_LEVEL1 enum.
//
// Additional information:
// Included here are 4 specializations for Direct3D 10
// (returning ID3D10Device or ID3D10Device1, and with
// or without an explicit driver type) and 2 for
// Direct3D 11 (with or without an explicit driver
// type).
///////////////////////////////////////////////////////
HRESULT CDXGITestFramework::CreateDevice
(
	IDXGIAdapter * pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE hSoftware,
	UINT Flags,
	TEST_FEATURE_LEVEL TestFeatureLevel,
	UINT SDKVersion,
	ID3D10Device ** ppDevice,
	bool bIgnoreRequestedFeatureLevels, // = false
	bool bAutomaticallyLoadSoftwareRasterizer // = true
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	ID3D11Device * pD3D11Device = NULL;


	//
	// Validate parameters.
	//

	if ( ppDevice == NULL )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};


	assert( *ppDevice == NULL );
	if ( *ppDevice != NULL )
	{
		WriteToLogF
		(
			_T( "CDXGITestFramework::CreateDevice:  ppDevice pointed at a non-NULL device pointer which would otherwise have caused a leak." )
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	*ppDevice = NULL;


	//
	// Load the software rasterizer if needed.
	//

	if 
	( 
		DriverType == D3D10_DRIVER_TYPE_SOFTWARE &&
		hSoftware == NULL &&
		bAutomaticallyLoadSoftwareRasterizer == true
	)
	{
		hSoftware = LoadSoftwareRasterizer();
		if ( hSoftware == NULL )
		{
			WriteToLog
			(
				_T( "CDXGITestFramework::CreateDevice:  LoadSoftwareRasterizer failed." )
			);

			hOverallResult = HRESULT_FROM_WIN32( ERROR_MOD_NOT_FOUND );
			goto Cleanup;
		};
	};


    if (m_D3DOptions.Force11On12)
    {
        // Use 11on12 if requested on the command line
        ID3D11Device* pDevice11 = NULL;
        ID3D11DeviceContext* pImmediateContext = NULL;
        ID3D11DeviceContext1* pImmediateContext1 = NULL;
        ID3D11Device1* pDevice11_1 = NULL;
        ID3DDeviceContextState* pDeviceContextState = NULL;

        D3D_DRIVER_TYPE DriverType11 = D3D_DRIVER_TYPE_UNKNOWN;

        switch (DriverType)
        {
        case D3D10_DRIVER_TYPE_HARDWARE: DriverType11 = D3D_DRIVER_TYPE_HARDWARE; break;
        case D3D10_DRIVER_TYPE_REFERENCE: DriverType11 = D3D_DRIVER_TYPE_REFERENCE; break;
        case D3D10_DRIVER_TYPE_NULL: DriverType11 = D3D_DRIVER_TYPE_NULL; break;
        case D3D10_DRIVER_TYPE_SOFTWARE: DriverType11 = D3D_DRIVER_TYPE_SOFTWARE; break;
        case D3D10_DRIVER_TYPE_WARP: DriverType11 = D3D_DRIVER_TYPE_WARP; break;
        default:
            assert(false);
        }

        bool Used11on12 = false;

        hOverallResult = D3D11CreateDevice_11or12(
            UseD3D11On12_HighNodeMask,
            pAdapter,
            pAdapter ? D3D_DRIVER_TYPE_UNKNOWN : DriverType11,
            hSoftware,
            ( m_D3DOptions.Debug ? D3D11_CREATE_DEVICE_DEBUG : 0 ),
            nullptr,
            0,
            D3D11_SDK_VERSION,
			&pDevice11,
            nullptr,
            &pImmediateContext,
            &Used11on12
            );

        if (SUCCEEDED(hOverallResult))
        {
            hOverallResult = pDevice11->QueryInterface(&pDevice11_1);
        }

        if (SUCCEEDED(hOverallResult))
        {
            hOverallResult = pImmediateContext->QueryInterface(&pImmediateContext1);
        }

        if (SUCCEEDED(hOverallResult))
        {
            const D3D_FEATURE_LEVEL CDCSFeatureLevel = D3D_FEATURE_LEVEL_10_0;

            // Enable QI to D3D10
            hOverallResult = pDevice11_1->CreateDeviceContextState(
                0,
                &CDCSFeatureLevel,
                1,
                D3D11_SDK_VERSION,
                _uuidof(ID3D10Device),
                nullptr,
                &pDeviceContextState
                );
        }

        if (SUCCEEDED(hOverallResult))
        {
            pImmediateContext1->SwapDeviceContextState(pDeviceContextState, NULL);
        }

        if (SUCCEEDED(hOverallResult))
        {
            hOverallResult = pDevice11->QueryInterface(IID_PPV_ARGS(ppDevice));
        }

        SAFE_RELEASE(pDevice11);
        SAFE_RELEASE(pImmediateContext);
        SAFE_RELEASE(pImmediateContext1);
        SAFE_RELEASE(pDevice11_1);
        SAFE_RELEASE(pDeviceContextState);
        
        if (SUCCEEDED(hOverallResult))
        {
            WriteToLogF
			(
				_T( "CDXGITestFramework::CreateDevice:  Using 11on12." )
			);
        }
        else
        {
            WriteToLogF
			(
				_T( "CDXGITestFramework::CreateDevice:  D3D11CreateDevice_11on12 failed with error 0x%x." ),
				hOverallResult
			);
        }
    }
    else
    {
	    //
	    // Validate the requested feature level.
	    //

	    switch ( TestFeatureLevel )
	    {
		    case TEST_FEATURE_10_0_LEVEL_10_0:
		    {
			    if
			    (
				    m_RequestedFeatureLevels[ TestFeatureLevel ] != true &&
				    bIgnoreRequestedFeatureLevels != true
			    )
			    {
				    hOverallResult = DXGI_ERROR_UNSUPPORTED;
				    goto Cleanup;
			    };


			    if ( SDKVersion == 0 )
			    {
				    SDKVersion = GetSDKVersionForTestFeatureLevel( TestFeatureLevel );
			    };


			    //
			    // Call the Direct3D 10.0 API.
			    //

			    hOverallResult = D3D10CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags |
					    ( ( m_D3DOptions.Debug == true ) ? D3D10_CREATE_DEVICE_DEBUG : 0 ),
				    SDKVersion,
				    ppDevice
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE &&
				    hOverallResult != E_NOTIMPL
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  D3D10CreateDevice failed with error 0x%x." ),
					    hOverallResult
				    );

				    break;
			    };
		    } break;


		    case TEST_FEATURE_10_1_LEVEL_10_1:
		    case TEST_FEATURE_10_1_LEVEL_10_0:
		    case TEST_FEATURE_10_1_LEVEL_9_3:
		    case TEST_FEATURE_10_1_LEVEL_9_2:
		    case TEST_FEATURE_10_1_LEVEL_9_1:
		    {
			    if
			    (
				    m_RequestedFeatureLevels[ TestFeatureLevel ] != true &&
				    bIgnoreRequestedFeatureLevels != true
			    )
			    {
				    hOverallResult = DXGI_ERROR_UNSUPPORTED;
				    goto Cleanup;
			    };


			    if ( SDKVersion == 0 )
			    {
				    SDKVersion = GetSDKVersionForTestFeatureLevel( TestFeatureLevel );
			    };


			    //
			    // Check our function pointer.
			    //

			    if ( m_pD3D10CreateDevice1 == NULL )
			    {
				    WriteToLog
				    (
					    _T( "CDXGITestFramework::CreateDevice:  Could not find D3D10CreateDevice1 entry point." )
				    );

				    hOverallResult = HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );
				    goto Cleanup;
			    };


			    //
			    // Call the API.
			    //

			    D3D10_FEATURE_LEVEL1 D3DFeatureLevel = GetD3D10FeatureLevelForTestFeatureLevel( TestFeatureLevel );
			    ID3D10Device1 * pDevice = NULL;

			    hOverallResult = (*m_pD3D10CreateDevice1)
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags |	( ( m_D3DOptions.Debug == true ) ? D3D10_CREATE_DEVICE_DEBUG : 0 ),
				    D3DFeatureLevel,
				    SDKVersion,
				    &pDevice
			    );

			    *ppDevice = (ID3D10Device *) pDevice;


			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE &&
				    hOverallResult != E_NOTIMPL
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  D3D10CreateDevice1 failed with error 0x%x." ),
					    hOverallResult
				    );

				    break;
			    };
		    } break;


		    case TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE:
		    {
			    //
			    // Try feature level 10.1.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_10_1_LEVEL_10_1,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE &&
				    hOverallResult != E_NOTIMPL
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 10.1 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };


			    //
			    // Try feature level 10.0.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_10_1_LEVEL_10_0,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE &&
				    hOverallResult != E_NOTIMPL
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 10.0 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };
		    } break;


		    case TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE:
		    {
			    //
			    // Try feature level 9_3.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_10_1_LEVEL_9_3,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE &&
				    hOverallResult != E_NOTIMPL
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 9.3 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };


			    //
			    // Try feature level 9_2.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_10_1_LEVEL_9_2,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE &&
				    hOverallResult != E_NOTIMPL
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 9.2 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };


			    //
			    // Try feature level 9_1.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_10_1_LEVEL_9_1,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE &&
				    hOverallResult != E_NOTIMPL
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 9.1 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };
		    } break;


		    case TEST_FEATURE_10_ALL_LEVEL_BESTAVAILABLE:
		    {
			    //
			    // Try the best available 10.1 level.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE &&
				    hOverallResult != E_NOTIMPL
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 10_bestavailable device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };


			    //
			    // Try feature level 10.0.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_10_0_LEVEL_10_0,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE &&
				    hOverallResult != E_NOTIMPL
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.0 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };


			    //
			    // Try the best available 9 feature level.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE &&
				    hOverallResult != E_NOTIMPL
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 9_bestavailable device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };
		    } break;


		    case TEST_FEATURE_11_0_LEVEL_11_0:
		    case TEST_FEATURE_11_0_LEVEL_10_1:
		    case TEST_FEATURE_11_0_LEVEL_10_0:
		    case TEST_FEATURE_11_0_LEVEL_9_3:
		    case TEST_FEATURE_11_0_LEVEL_9_2:
		    case TEST_FEATURE_11_0_LEVEL_9_1:
		    {
			    if
			    (
				    m_RequestedFeatureLevels[ TestFeatureLevel ] != true &&
				    bIgnoreRequestedFeatureLevels != true
			    )
			    {
				    hOverallResult = DXGI_ERROR_UNSUPPORTED;
				    goto Cleanup;
			    };


			    if ( SDKVersion == 0 )
			    {
				    SDKVersion = GetSDKVersionForTestFeatureLevel( TestFeatureLevel );
			    };


			    //
			    // Check our function pointer.
			    //

			    if ( m_pD3D11LayerCreateDevice == NULL )
			    {
				    WriteToLog
				    (
					    _T( "CDXGITestFramework::CreateDevice:  Could not find D3D11LayerCreateDevice entry point." )
				    );

				    hOverallResult = HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );
				    goto Cleanup;
			    };


			    //
			    // Call the API.
			    //

			    D3D_FEATURE_LEVEL D3DFeatureLevel = GetD3DFeatureLevelForTestFeatureLevel( TestFeatureLevel );
			    D3D_FEATURE_LEVEL FeatureLevelReturned = D3DFeatureLevel;

			    hOverallResult = m_pD3D11LayerCreateDevice
			    (
				    pAdapter,
				    pAdapter != NULL ? D3D_DRIVER_TYPE_UNKNOWN : GetD3DDriverTypeForD3D10DriverType( DriverType ),
				    hSoftware,
				    Flags |
					    ( ( m_D3DOptions.Debug == true ) ? D3D11_CREATE_DEVICE_DEBUG : 0 ),
				    &D3DFeatureLevel,
				    1,
				    SDKVersion,
				    &pD3D11Device,
				    &FeatureLevelReturned,
				    NULL
			    );

			    if
			    (
				    hOverallResult == DXGI_ERROR_UNSUPPORTED ||
				    hOverallResult == E_NOINTERFACE ||
				    hOverallResult == E_NOTIMPL
			    )
			    {
				    break;
			    }
			    else if
			    (
				    FAILED( hOverallResult )
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  D3D11LayerCreateDevice failed with error 0x%x." ),
					    hOverallResult
				    );

				    break;
			    };


			    ID3D11Device1* pD3D11Device1 = NULL;
			    if( FAILED(pD3D11Device->QueryInterface( __uuidof(ID3D11Device1), (void **) &pD3D11Device1 )) )
			    {
				    WriteToLogF( _T( "CDXGITestFramework::CreateDevice:  QueryInterface on D3D 11 device for D3D11_1 Device failed." )	);
				    goto Cleanup;
			    }

            
			    D3D_FEATURE_LEVEL FeatureLevelCreated;
			    D3D_FEATURE_LEVEL featureLevels[2];
			    featureLevels[0] = pD3D11Device1->GetFeatureLevel();
			    featureLevels[1] = D3D_FEATURE_LEVEL_10_0;
			    ID3DDeviceContextState* pDCS;

			    hOverallResult = pD3D11Device1->CreateDeviceContextState( Flags & ~(D3D11_CREATE_DEVICE_DEBUG), featureLevels, 2, D3D11_SDK_VERSION, __uuidof(*ppDevice), &FeatureLevelCreated, &pDCS );
			    if( FAILED( hOverallResult ) )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice: CreateDeviceContextState failed with error 0x%x." ),
					    hOverallResult
				    );
				    (*ppDevice)->Release();
				    *ppDevice = NULL;
			    }
			    else
			    {
				    ID3D11DeviceContext1* immediateContext1;
				    pD3D11Device1->GetImmediateContext1( &immediateContext1 );
				    immediateContext1->SwapDeviceContextState( pDCS, NULL );
				    SAFE_RELEASE( immediateContext1 );
				    SAFE_RELEASE( pDCS );

				    hOverallResult = pD3D11Device->QueryInterface
				    (
					    IID_ID3D10Device,
					    (void **) ppDevice
				    );

				    if
				    (
					    FAILED( hOverallResult )
				    )
				    {
					    WriteToLogF
					    (
						    _T( "CDXGITestFramework::CreateDevice:  QueryInterface on D3D 11 device for IID_ID3D10Device failed with error 0x%x." ),
						    hOverallResult
					    );

					    break;
				    };

			    }
			    SAFE_RELEASE( pD3D11Device1 );

		    } break;


		    case TEST_FEATURE_11_ALL_LEVEL_BESTAVAILABLE:
		    case TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE:
		    {
			    //
			    // Try feature level 11_0.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_11_0_LEVEL_11_0,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 11.0 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };
		    } break;


		    case TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE:
		    {
			    //
			    // Try feature level 10.1.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_11_0_LEVEL_10_1,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 10.1 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };


			    //
			    // Try feature level 10.0.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_11_0_LEVEL_10_0,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 10.0 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };
		    } break;


		    case TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE:
		    {
			    //
			    // Try feature level 9_3.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_11_0_LEVEL_9_3,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 9.3 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };


			    //
			    // Try feature level 9_2.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_11_0_LEVEL_9_2,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 9.2 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };


			    //
			    // Try feature level 9_1.
			    //

			    hOverallResult = CreateDevice
			    (
				    pAdapter,
				    DriverType,
				    hSoftware,
				    Flags,
				    TEST_FEATURE_11_0_LEVEL_9_1,
				    SDKVersion,
				    ppDevice,
				    bIgnoreRequestedFeatureLevels,
				    bAutomaticallyLoadSoftwareRasterizer
			    );

			    if ( SUCCEEDED( hOverallResult ) )
			    {
				    break;
			    }
			    else if
			    (
				    hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				    hOverallResult != E_NOINTERFACE
			    )
			    {
				    WriteToLogF
				    (
					    _T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 9.1 device failed with error 0x%x." ),
					    hOverallResult
				    );
			    };
		    } break;


		    default:
		    {
			    WriteToLogF
			    (
				    _T( "CDXGITestFramework::CreateDevice:  Unexpected test feature level %u (%s)." ),
				    TestFeatureLevel,
				    ToString( TestFeatureLevel ).c_str()
			    );

			    hOverallResult = E_NOTIMPL;
			    goto Cleanup;
		    } break;
	    };
    }


Cleanup:
	if ( pD3D11Device != NULL )
	{
		pD3D11Device->Release();
		pD3D11Device = NULL;
	};


	return hOverallResult;
};


HRESULT CDXGITestFramework::CreateDevice
(
	IDXGIAdapter * pAdapter,
	HMODULE hSoftware,
	UINT Flags,
	TEST_FEATURE_LEVEL TestFeatureLevel,
	UINT SDKVersion,
	ID3D10Device ** ppDevice,
	bool bIgnoreRequestedFeatureLevels, // = false
	bool bAutomaticallyLoadSoftwareRasterizer // = true
)
{
	return CreateDevice
	(
		pAdapter,
		m_D3DOptions.DriverType,
		hSoftware,
		Flags,
		TestFeatureLevel,
		SDKVersion,
		ppDevice,
		bIgnoreRequestedFeatureLevels,
		bAutomaticallyLoadSoftwareRasterizer
	);
};


HRESULT CDXGITestFramework::CreateDevice
(
	IDXGIAdapter * pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE hSoftware,
	UINT Flags,
	TEST_FEATURE_LEVEL TestFeatureLevel,
	UINT SDKVersion,
	ID3D10Device1 ** ppDevice,
	bool bIgnoreRequestedFeatureLevels, // = false
	bool bAutomaticallyLoadSoftwareRasterizer // = true
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	ID3D11Device * pD3D11Device = NULL;


	//
	// Validate parameters.
	//

	if ( ppDevice == NULL )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};


	assert( *ppDevice == NULL );
	if ( *ppDevice != NULL )
	{
		WriteToLogF
		(
			_T( "CDXGITestFramework::CreateDevice:  ppDevice pointed at a non-NULL device pointer which would otherwise have caused a leak." )
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	*ppDevice = NULL;


	//
	// Load the software rasterizer if needed.
	//

	if 
	( 
		DriverType == D3D10_DRIVER_TYPE_SOFTWARE &&
		hSoftware == NULL &&
		bAutomaticallyLoadSoftwareRasterizer == true
	)
	{
		hSoftware = LoadSoftwareRasterizer();
		if ( hSoftware == NULL )
		{
			WriteToLog
			(
				_T( "CDXGITestFramework::CreateDevice:  LoadSoftwareRasterizer failed." )
			);

			hOverallResult = HRESULT_FROM_WIN32( ERROR_MOD_NOT_FOUND );
			goto Cleanup;
		};
	};


	//
	// Validate the requested feature level.
	//

	switch ( TestFeatureLevel )
	{
		case TEST_FEATURE_10_0_LEVEL_10_0:
		{
			WriteToLog
			(
				_T( "CDXGITestFramework::CreateDevice:  Direct3D 10.1 version of CreateDevice cannot create Direct3D 10.0 device." )
			);

			hOverallResult = E_INVALIDARG;
			goto Cleanup;
		} break;


		case TEST_FEATURE_10_1_LEVEL_10_1:
		case TEST_FEATURE_10_1_LEVEL_10_0:
		case TEST_FEATURE_10_1_LEVEL_9_3:
		case TEST_FEATURE_10_1_LEVEL_9_2:
		case TEST_FEATURE_10_1_LEVEL_9_1:
		{
			if
			(
				m_RequestedFeatureLevels[ TestFeatureLevel ] != true &&
				bIgnoreRequestedFeatureLevels != true
			)
			{
				hOverallResult = DXGI_ERROR_UNSUPPORTED;
				goto Cleanup;
			};


			if ( SDKVersion == 0 )
			{
				SDKVersion = GetSDKVersionForTestFeatureLevel( TestFeatureLevel );
			};


			//
			// Check our function pointer.
			//

			if ( m_pD3D10CreateDevice1 == NULL )
			{
				WriteToLog
				(
					_T( "CDXGITestFramework::CreateDevice:  D3D10CreateDevice1 entry point not found." )
				);

				hOverallResult = HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );
				goto Cleanup;
			};


			//
			// Call the API.
			//

			D3D10_FEATURE_LEVEL1 D3DFeatureLevel = GetD3D10FeatureLevelForTestFeatureLevel( TestFeatureLevel );

			hOverallResult = (*m_pD3D10CreateDevice1)
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags |	( ( m_D3DOptions.Debug == true ) ? D3D10_CREATE_DEVICE_DEBUG : 0 ),
				D3DFeatureLevel,
				SDKVersion,
				ppDevice
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE &&
				hOverallResult != E_NOTIMPL
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  D3D10CreateDevice1 failed with error 0x%x." ),
					hOverallResult
				);

				break;
			};
		} break;


		case TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE:
		{
			//
			// Try feature level 10.1.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_10_1_LEVEL_10_1,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE &&
				hOverallResult != E_NOTIMPL
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 10.1 device failed with error 0x%x." ),
					hOverallResult
				);
			};


			//
			// Try feature level 10.0.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_10_1_LEVEL_10_0,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE &&
				hOverallResult != E_NOTIMPL
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 10.0 device failed with error 0x%x." ),
					hOverallResult
				);
			};
		} break;


		case TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE:
		{
			//
			// Try feature level 9_3.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_10_1_LEVEL_9_3,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE &&
				hOverallResult != E_NOTIMPL
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 9.3 device failed with error 0x%x." ),
					hOverallResult
				);
			};


			//
			// Try feature level 9_2.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_10_1_LEVEL_9_2,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE &&
				hOverallResult != E_NOTIMPL
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 9.2 device failed with error 0x%x." ),
					hOverallResult
				);
			};


			//
			// Try feature level 9_1.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_10_1_LEVEL_9_1,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE &&
				hOverallResult != E_NOTIMPL
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 9.1 device failed with error 0x%x." ),
					hOverallResult
				);
			};
		} break;


		case TEST_FEATURE_10_ALL_LEVEL_BESTAVAILABLE:
		{
			//
			// Try the best available 10.1 level.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE,
				SDKVersion,
				ppDevice,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE &&
				hOverallResult != E_NOTIMPL
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 10_bestavailable device failed with error 0x%x." ),
					hOverallResult
				);
			};


			//
			// Try the best available 10.0 feature level.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_10_0_LEVEL_10_0,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE &&
				hOverallResult != E_NOTIMPL
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.0 device failed with error 0x%x." ),
					hOverallResult
				);
			};


			//
			// Try the best available 9 feature level.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE &&
				hOverallResult != E_NOTIMPL
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 10.1 feature level 9_bestavailable device failed with error 0x%x." ),
					hOverallResult
				);
			};
		} break;


		case TEST_FEATURE_11_0_LEVEL_11_0:
		case TEST_FEATURE_11_0_LEVEL_10_1:
		case TEST_FEATURE_11_0_LEVEL_10_0:
		case TEST_FEATURE_11_0_LEVEL_9_3:
		case TEST_FEATURE_11_0_LEVEL_9_2:
		case TEST_FEATURE_11_0_LEVEL_9_1:
		{
			if
			(
				m_RequestedFeatureLevels[ TestFeatureLevel ] != true &&
				bIgnoreRequestedFeatureLevels != true
			)
			{
				hOverallResult = DXGI_ERROR_UNSUPPORTED;
				goto Cleanup;
			};


			if ( SDKVersion == 0 )
			{
				SDKVersion = GetSDKVersionForTestFeatureLevel( TestFeatureLevel );
			};


			//
			// Check our function pointer.
			//

			if ( m_pD3D11LayerCreateDevice == NULL )
			{
				WriteToLog
				(
					_T( "CDXGITestFramework::CreateDevice:  Could not find D3D11LayerCreateDevice entry point." )
				);

				hOverallResult = HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );
				goto Cleanup;
			};


			//
			// Call the API.
			//

			D3D_FEATURE_LEVEL D3DFeatureLevel = GetD3DFeatureLevelForTestFeatureLevel( TestFeatureLevel );
			D3D_FEATURE_LEVEL FeatureLevelReturned = D3DFeatureLevel;

			hOverallResult = m_pD3D11LayerCreateDevice
			(
				pAdapter,
				pAdapter != NULL ? D3D_DRIVER_TYPE_UNKNOWN : GetD3DDriverTypeForD3D10DriverType( DriverType ),
				hSoftware,
				Flags |
					( ( m_D3DOptions.Debug == true ) ? D3D11_CREATE_DEVICE_DEBUG : 0 ),
				&D3DFeatureLevel,
				1,
				SDKVersion,
				&pD3D11Device,
				&FeatureLevelReturned,
				NULL
			);

			if
			(
				hOverallResult == DXGI_ERROR_UNSUPPORTED ||
				hOverallResult == E_NOINTERFACE ||
				hOverallResult == E_NOTIMPL
			)
			{
				break;
			}
			else if
			(
				FAILED( hOverallResult )
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  D3D11LayerCreateDevice failed with error 0x%x." ),
					hOverallResult
				);

				break;
			};


			ID3D11Device1* pD3D11Device1 = NULL;
			if( FAILED(pD3D11Device->QueryInterface( __uuidof(ID3D11Device1), (void **) &pD3D11Device1 )) )
			{
				WriteToLogF( _T( "CDXGITestFramework::CreateDevice:  QueryInterface on D3D 11 device for D3D11_1 Device failed." )	);
				goto Cleanup;
			}

			D3D_FEATURE_LEVEL FeatureLevelCreated;
			D3D_FEATURE_LEVEL featureLevels[2];
			featureLevels[0] = pD3D11Device1->GetFeatureLevel();
			featureLevels[1] = D3D_FEATURE_LEVEL_10_1;
			ID3DDeviceContextState* pDCS;

			hOverallResult = pD3D11Device1->CreateDeviceContextState( Flags & ~(D3D11_CREATE_DEVICE_DEBUG), featureLevels, 2, D3D11_SDK_VERSION, __uuidof(*ppDevice), &FeatureLevelCreated, &pDCS );
			if( FAILED( hOverallResult ) )
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDeviceContextState failed with error 0x%x." ),
					hOverallResult
				);
				(*ppDevice)->Release();
				*ppDevice = NULL;
			}
			else
			{
				ID3D11DeviceContext1* immediateContext1;
				pD3D11Device1->GetImmediateContext1( &immediateContext1 );
				immediateContext1->SwapDeviceContextState( pDCS, NULL );
				SAFE_RELEASE( immediateContext1 );
				SAFE_RELEASE( pDCS );

				hOverallResult = pD3D11Device->QueryInterface
				(
					IID_ID3D10Device1,
					(void **) ppDevice
				);

				if
				(
					FAILED( hOverallResult )
				)
				{
					WriteToLogF
					(
						_T( "CDXGITestFramework::CreateDevice:  QueryInterface on D3D 11 device for IID_ID3D10Device1 failed with error 0x%x." ),
						hOverallResult
					);

					break;
				};
			}
			SAFE_RELEASE( pD3D11Device1 );

		} break;


		case TEST_FEATURE_11_ALL_LEVEL_BESTAVAILABLE:
		case TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE:
		{
			//
			// Try feature level 11.0.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_11_0_LEVEL_11_0,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 11.0 device failed with error 0x%x." ),
					hOverallResult
				);
			};
		} break;


		case TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE:
		{
			//
			// Try feature level 10.1.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_11_0_LEVEL_10_1,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 10.1 device failed with error 0x%x." ),
					hOverallResult
				);
			};


			//
			// Try feature level 10.0.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_11_0_LEVEL_10_0,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 10.0 device failed with error 0x%x." ),
					hOverallResult
				);
			};
		} break;


		case TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE:
		{
			//
			// Try feature level 9_3.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_11_0_LEVEL_9_3,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 9.3 device failed with error 0x%x." ),
					hOverallResult
				);
			};


			//
			// Try feature level 9_2.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_11_0_LEVEL_9_2,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 9.2 device failed with error 0x%x." ),
					hOverallResult
				);
			};


			//
			// Try feature level 9_1.
			//

			hOverallResult = CreateDevice
			(
				pAdapter,
				DriverType,
				hSoftware,
				Flags,
				TEST_FEATURE_11_0_LEVEL_9_1,
				SDKVersion,
				ppDevice,
				bIgnoreRequestedFeatureLevels,
				bAutomaticallyLoadSoftwareRasterizer
			);

			if ( SUCCEEDED( hOverallResult ) )
			{
				break;
			}
			else if
			(
				hOverallResult != DXGI_ERROR_UNSUPPORTED &&
				hOverallResult != E_NOINTERFACE
			)
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateDevice:  CreateDevice on Direct3D 11.0 feature level 9.1 device failed with error 0x%x." ),
					hOverallResult
				);
			};
		} break;


		default:
		{
			WriteToLogF
			(
				_T( "CDXGITestFramework::CreateDevice:  Unexpected test feature level %u (%s)." ),
				TestFeatureLevel,
				ToString( TestFeatureLevel ).c_str()
			);

			hOverallResult = E_NOTIMPL;
			goto Cleanup;
		} break;
	};


Cleanup:
	if ( pD3D11Device != NULL )
	{
		pD3D11Device->Release();
		pD3D11Device = NULL;
	};


	return hOverallResult;
};


HRESULT CDXGITestFramework::CreateDevice
(
	IDXGIAdapter * pAdapter,
	HMODULE hSoftware,
	UINT Flags,
	TEST_FEATURE_LEVEL TestFeatureLevel,
	UINT SDKVersion,
	ID3D10Device1 ** ppDevice,
	bool bIgnoreRequestedFeatureLevels, // = false
	bool bAutomaticallyLoadSoftwareRasterizer // = true
)
{
	return CreateDevice
	(
		pAdapter,
		m_D3DOptions.DriverType,
		hSoftware,
		Flags,
		TestFeatureLevel,
		SDKVersion,
		ppDevice,
		bIgnoreRequestedFeatureLevels,
		bAutomaticallyLoadSoftwareRasterizer
	);
};


///////////////////////////////////////////////////////
// Function:  CreateFactoryAdapter
//
// Purpose:
// Creates a DXGI factory and adapter and returns the
// interface pointers.
///////////////////////////////////////////////////////
HRESULT CDXGITestFramework::CreateFactoryAdapter
(
	__inout DXGI_FACTORY_POINTER * ppFactory,
	UINT AdapterOrdinal,
	__out DXGI_ADAPTER_POINTER * ppAdapter
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate parameters.
	//

	if ( ppFactory == NULL )
	{
		WriteToLog
		(
			_T( "CDXGITestFramework::CreateFactoryAdapter:  ppFactory is NULL." )
		);

		hOverallResult = E_POINTER;
		goto Cleanup;
	};

	if ( ppAdapter == NULL )
	{
		WriteToLog
		(
			_T( "CDXGITestFramework::CreateFactoryAdapter:  ppAdapter is NULL." )
		);

		hOverallResult = E_POINTER;
		goto Cleanup;
	};


	//
	// Create a factory.
	//

	if ( ppFactory->pFactory == NULL )
	{
		hOverallResult = CreateDXGIFactory( ppFactory );

		if ( FAILED( hOverallResult ) )
		{
			WriteToLogF
			(
				_T("CDXGITestFramework::CreateFactoryAdapter:  CreateDXGIFactory failed with error 0x%x."),
				hOverallResult
			);

			goto Cleanup;
		};
	};


	//
	// Create an adapter.
	//

	hOverallResult = CreateAdapter
	(
		*ppFactory,
		AdapterOrdinal,
		ppAdapter
	);

	if ( FAILED( hOverallResult ) )
	{
		WriteToLogF
		(
			_T("CDXGITestFramework::CreateFactoryAdapter:  CreateAdapter failed with error 0x%x." ),
			hOverallResult
		);

		goto Cleanup;
	};


Cleanup:
	if ( FAILED( hOverallResult ) )
	{
		if ( ppAdapter->pAdapter != NULL )
		{
			ppAdapter->pAdapter->Release();
			ppAdapter->pAdapter = NULL;
		};

		if ( ppFactory->pFactory != NULL )
		{
			ppFactory->pFactory->Release();
			ppFactory->pFactory = NULL;
		};
	};


	return hOverallResult;
};


/////////////////////////////////////////////////////////
// Function:  GetAdapter
//
// Purpose:
// Get adapter interface handle from the device interface
/////////////////////////////////////////////////////////
HRESULT CDXGITestFramework::GetAdapter
(
	__in  D3D10_DEVICE_POINTER *ppDevice,
	__out DXGI_ADAPTER_POINTER &AdapterPointer
)
{
	HRESULT		hrResult = S_OK;
	IDXGIDevice *pDXGIDevice = NULL;

	assert (ppDevice != NULL);
	assert ( ppDevice->pDevice != NULL );

	hrResult = ppDevice->pDevice->QueryInterface
	(
		IID_IDXGIDevice,
		(void **) &pDXGIDevice
	);

	if ( FAILED( hrResult ) )
	{
		WriteToLogF
		(
			_T( "CDXGITestFramework::GetAdapter: D3D10 QueryInterface for IID_IDXGIDevice failed with error 0x%x." ),
			hrResult
		);

		goto Cleanup;
	}

	if ( pDXGIDevice == NULL )
	{
		WriteToLog
		(
			_T("CDXGITestFramework::GetAdapter:  D3D10 QueryInterface for IID_IDXGIDevice succeeded but returned NULL device pointer.")
		);

		goto Cleanup;
	}

	//
	// Get the adapter.
	//

	hrResult = pDXGIDevice->GetAdapter
	(
		&AdapterPointer.pAdapter
	);

	pDXGIDevice->Release();
	pDXGIDevice = NULL;

	if ( FAILED( hrResult ) )
	{
		WriteToLogF
		(
			_T("CDXGITestFramework::GetAdapter:  D3D10 GetAdapter failed with error 0x%x."),
			hrResult
		);

		goto Cleanup;
	}

	if ( AdapterPointer.pAdapter == NULL )
	{
		WriteToLog
		(
			_T("CDXGITestFramework::GetAdapter:  D3D10 GetAdapter succeeded but returned NULL adapter pointer.")
		);

		goto Cleanup;
	}

Cleanup:
	return hrResult;
}

///////////////////////////////////////////////////////
// Function:  CreateFactoryAdapterDevice
//
// Purpose:
// Works around the bizarre shape of the DXGI/Direct3D
// device creation API by creating a factory, adapter,
// and device in one call.
///////////////////////////////////////////////////////
HRESULT CDXGITestFramework::CreateFactoryAdapterDevice
(
	__inout DXGI_FACTORY_POINTER * ppFactory,
	UINT AdapterOrdinal,
	__inout DXGI_ADAPTER_POINTER * ppAdapter,
	D3D10_DRIVER_TYPE DriverType,
	UINT Flags,
	TEST_FEATURE_LEVEL TestFeatureLevel,
	UINT SDKVersion,
	D3D10_DEVICE_POINTER * ppDevice,
	bool bIgnoreRequestedFeatureLevels,
	bool * pbAdapterIsRemote
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	DXGI_FACTORY_POINTER FactoryPointer;
	FactoryPointer.pFactory = NULL;

	DXGI_ADAPTER_POINTER AdapterPointer;
	AdapterPointer.pAdapter = NULL;


	//
	// Validate parameters.
	//

	if ( ppFactory == NULL )
	{
		WriteToLog
		(
			_T( "CDXGITestFramework::CreateFactoryAdapterDevice:  ppFactory is NULL." )
		);

		hOverallResult = E_POINTER;
		goto Cleanup;
	};


	if ( ppAdapter == NULL )
	{
		WriteToLog
		(
			_T( "CDXGITestFramework::CreateFactoryAdapterDevice:  ppAdapter is NULL." )
		);

		hOverallResult = E_POINTER;
		goto Cleanup;
	};


	if ( ppDevice == NULL )
	{
		WriteToLog
		(
			_T( "CDXGITestFramework::CreateFactoryAdapterDevice:  ppDevice is NULL.")
		);

		hOverallResult = E_POINTER;
		goto Cleanup;
	};


	assert( ppDevice->pDevice == NULL );
	if ( ppDevice->pDevice != NULL )
	{
		WriteToLog
		(
			_T( "CDXGITestFramework::CreateFactoryAdapterDevice:  ppDevice points to a non-NULL device pointer, which would otherwise have caused a leak." )
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Copy incoming pointers.
	//

	if ( ppFactory->pFactory != NULL )
	{
		FactoryPointer = *ppFactory;
	};


	if ( ppAdapter->pAdapter != NULL )
	{
		AdapterPointer = *ppAdapter;
	};


	//
	// Pass the magic REMOTE_FORCE_REF flag if we're running in
	// a remote session and the -TS command-line switch is set.
	//

	if
	(
		DriverType == D3D10_DRIVER_TYPE_REFERENCE &&
		IsTerminalServices() &&
		GetSystemMetrics( SM_REMOTESESSION ) != 0 &&
		TestFeatureLevel > TEST_FEATURE_10_0_LEVEL_10_0
	)
	{
		DriverType = D3D10_DRIVER_TYPE_HARDWARE;
		Flags |= D3D10_CREATE_DEVICE_REMOTE_FORCE_REF;
	};


	//
	// Create our factory if needed.
	//

	switch ( DriverType )
	{
		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			if ( FactoryPointer.pFactory == NULL )
			{
				HRESULT hCreateFactoryResult = CreateDXGIFactory
				(
					&FactoryPointer
				);

				if ( FAILED( hCreateFactoryResult ) )
				{
					WriteToLogF
					(
						_T( "CDXGITestFramework::CreateFactoryAdapterDevice:  CreateFactory failed with error 0x%x." ),
						hCreateFactoryResult
					);

					hOverallResult = hCreateFactoryResult;
					goto Cleanup;
				};
			};
		} break;


		default:
		{
			assert( FactoryPointer.pFactory == NULL );
			if ( FactoryPointer.pFactory != NULL )
			{
				WriteToLog
				(
					_T( "CDXGITestFramework::CreateFactoryAdapterDevice:  Factory slot is occupied." )
				);

				hOverallResult = E_INVALIDARG;
				goto Cleanup;
			};
		} break;
	};


	//
	// Create our adapter if needed.
	//

	switch ( DriverType )
	{
		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			// don't Enum Adapter if it is a test for session 0 and DXGI 1.0
			if ( (!IsSession0() || m_DXGIOptions.Version != TEST_DXGI_VERSION_1_0) && (AdapterPointer.pAdapter == NULL) )
			{
				if(m_DXGIOptions.AdapterTypeRef == TEST_DXGI_ADAPTER_TYPE_RENDERONLY_HW || m_DXGIOptions.AdapterTypeRef == TEST_DXGI_ADAPTER_TYPE_RENDERONLY_SW)
				{
					UINT AdapterTypeRefIndex = 1;
					TEST_RESULT trSetupAdapter = SetupAdapter(AdapterTypeRefIndex, &FactoryPointer, &AdapterPointer.pAdapter); 

					if(trSetupAdapter != RESULT_PASS)
					{
						// SetupAdapter logs a failure
						hOverallResult = E_FAIL;
						goto Cleanup;
					}
				}
				else
				{
					HRESULT hCreateAdapterResult = E_FAIL;
					hCreateAdapterResult = FactoryPointer.pFactory->EnumAdapters
					(
						AdapterOrdinal,
						&AdapterPointer.pAdapter
					);

					if ( hCreateAdapterResult == DXGI_ERROR_NOT_FOUND )
					{
						hOverallResult = hCreateAdapterResult;
						goto Cleanup;
					}
					else if ( FAILED( hCreateAdapterResult ) )
					{
						WriteToLogF
						(
							_T( "CDXGITestFramework::CreateFactoryAdapterDevice:  EnumAdapters on adapter %u failed with error 0x%x." ),
							AdapterOrdinal,
							hCreateAdapterResult
						);

						hOverallResult = hCreateAdapterResult;
						goto Cleanup;
					};

					if ( AdapterPointer.pAdapter == NULL )
					{
						WriteToLogF
						(
							_T("CDXGITestFramework::CreateFactoryAdapterDevice:  EnumAdapters on adapter %u succeeded but returned NULL adapter pointer."),
							AdapterOrdinal
						);

						hOverallResult = E_FAIL;
						goto Cleanup;
					};
				}


				//
				// Sanity check:
				// Verify that the first adapter is a remote adapter.
				//

				if
				(
					m_DXGIOptions.Version == TEST_DXGI_VERSION_1_1 &&
					GetSystemMetrics( SM_REMOTESESSION ) != 0
				)
				{
					//
					// Get a (DXGI 1.1) descriptor for the adapter.
					//

					DXGI_ADAPTER_DESC1 AdapterDescriptor;
					HRESULT hGetDescriptorResult = AdapterPointer.pAdapter1->GetDesc1
					(
						&AdapterDescriptor
					);

					if ( FAILED( hGetDescriptorResult ) )
					{
						WriteToLogF
						(
							_T("CDXGITestFramework::CreateFactoryAdapterDevice:  GetDesc1 on adapter %u failed with error 0x%x."),
							AdapterOrdinal,
							hGetDescriptorResult
						);

						hOverallResult = hGetDescriptorResult;
						goto Cleanup;
					};


					//
					// Determine whether the adapter has the remote bit set.
					//

					if ( ( AdapterDescriptor.Flags & DXGI_ADAPTER_FLAG_REMOTE ) != 0 )
					{
						WriteToLog
						(
							_T( "Adapter was enumerated from remote client machine." )
						);

						if ( pbAdapterIsRemote != NULL )
						{
							*pbAdapterIsRemote = true;
						};
					}
					else
					{
						WriteToLog
						(
							_T( "Adapter was enumerated from local machine." )
						);

						if ( pbAdapterIsRemote != NULL )
						{
							*pbAdapterIsRemote = false;
						};
					};
				};
			};
		} break;


		default:
		{
			assert( AdapterPointer.pAdapter == NULL );
			if ( AdapterPointer.pAdapter != NULL )
			{
				WriteToLog
				(
					_T( "CDXGITestFramework::CreateFactoryAdapterDevice:  Adapter slot is occupied." )
				);

				hOverallResult = E_INVALIDARG;
				goto Cleanup;
			};
		} break;
	};


	//
	// Create our device.
	//

	HRESULT hCreateDeviceResult = E_FAIL;
	switch ( TestFeatureLevel )
	{
		case TEST_FEATURE_10_0_LEVEL_10_0:
		{
			hCreateDeviceResult = CreateDevice
			(
				AdapterPointer.pAdapter,
				DriverType,
				NULL,
				Flags,
				TestFeatureLevel,
				SDKVersion,
				&ppDevice->pDevice,
				bIgnoreRequestedFeatureLevels
			);
		} break;


		default:
		{
			hCreateDeviceResult = CreateDevice
			(
				AdapterPointer.pAdapter,
				DriverType,
				NULL,
				Flags,
				TestFeatureLevel,
				SDKVersion,
				&ppDevice->pDevice1,
				bIgnoreRequestedFeatureLevels
			);

			if ( SUCCEEDED( hCreateDeviceResult ) )
			{
				break;
			};

			hCreateDeviceResult = CreateDevice
			(
				AdapterPointer.pAdapter,
				DriverType,
				NULL,
				Flags,
				TestFeatureLevel,
				SDKVersion,
				&ppDevice->pDevice,
				bIgnoreRequestedFeatureLevels
			);
		} break;
	};

	if ( FAILED( hCreateDeviceResult ) )
	{
		WriteToLogF
		(
			_T("CDXGITestFramework::CreateFactoryAdapterDevice:  CreateDevice failed with error 0x%x." ),
			hCreateDeviceResult
		);

		hOverallResult = hCreateDeviceResult;
		goto Cleanup;
	};

	if ( ppDevice->pDevice == NULL )
	{
		WriteToLog
		(
			_T("CDXGITestFramework::CreateFactoryAdapterDevice:  CreateDevice succeeded but returned NULL device pointer.")
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};


	//
	// Query for our adapter if necessary.
	//

	switch ( DriverType )
	{
		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			assert( FactoryPointer.pFactory != NULL );
			if (IsSession0() && m_DXGIOptions.Version == TEST_DXGI_VERSION_1_0) // If we're running under TS and testing 1.0 DXGI interfaces we don't yet have an Adapter interface.  Lets' create one.
			{
				HRESULT hrGetAdapter = GetAdapter
				(
					ppDevice, 
					AdapterPointer
				);

				if ( FAILED( hrGetAdapter ) )
				{
					WriteToLogF
					(
						_T( "CDXGITestFramework::CreateFactoryAdapterDevice: GetAdapter, D3D10_DRIVER_TYPE_HARDWARE, failed with error 0x%x." ),
						hrGetAdapter
					);

					hOverallResult = hrGetAdapter;
					goto Cleanup;
				}
			}

		} break;


		default:
		{
			assert( FactoryPointer.pFactory == NULL );
			assert( AdapterPointer.pAdapter == NULL );

			HRESULT hrGetAdapter = GetAdapter
			(
				ppDevice, 
				AdapterPointer
			);

			if ( FAILED( hrGetAdapter ) )
			{
				WriteToLogF
				(
					_T( "CDXGITestFramework::CreateFactoryAdapterDevice: GetAdapter, default, failed with error 0x%x." ),
					hrGetAdapter
				);

				hOverallResult = hrGetAdapter;
				goto Cleanup;
			}
		};

		assert( AdapterPointer.pAdapter != NULL );

		//
		// Query for our factory if necessary.
		//

		if
		(
			FactoryPointer.pFactory == NULL
		)
		{
			assert( AdapterPointer.pAdapter != NULL );

			HRESULT hGetFactoryResult = E_FAIL;

			switch ( m_DXGIOptions.Version )
			{
				case TEST_DXGI_VERSION_1_3:
				case TEST_DXGI_VERSION_1_2:
				{
					hGetFactoryResult = AdapterPointer.pAdapter1->GetParent
					(
						IID_IDXGIFactory2,
						(void **) &FactoryPointer.pFactory2
					);
				} break;

				case TEST_DXGI_VERSION_1_1:
				{
					hGetFactoryResult = AdapterPointer.pAdapter1->GetParent
					(
						IID_IDXGIFactory1,
						(void **) &FactoryPointer.pFactory1
					);
				} break;

				case TEST_DXGI_VERSION_1_0:
				{
					hGetFactoryResult = AdapterPointer.pAdapter->GetParent
					(
						IID_IDXGIFactory,
						(void **) &FactoryPointer.pFactory
					);
				} break;

				default:
				{
					assert( FALSE );
				} break;
			};

			if ( FAILED( hGetFactoryResult ) )
			{
				WriteToLogF
				(
					_T("CDXGITestFramework::CreateFactoryAdapterDevice:  GetParent failed with error 0x%x."),
					hGetFactoryResult
				);

				hOverallResult = hGetFactoryResult;
				goto Cleanup;
			};

			if ( FactoryPointer.pFactory == NULL )
			{
				WriteToLog
				(
					_T("CDXGITestFramework::CreateFactoryAdapterDevice:  GetParent succeeded but returned NULL factory pointer.")
				);

				hOverallResult = E_FAIL;
				goto Cleanup;
			};
		};
	};


	//
	// Fill in our outgoing parameters.
	//

	if (!IsSession0() || m_DXGIOptions.Version != TEST_DXGI_VERSION_1_0)
	{
		assert( AdapterPointer.pAdapter != NULL );
		if ( ppAdapter->pAdapter == NULL )
		{
			*ppAdapter = AdapterPointer;
		}
	}

	assert( FactoryPointer.pFactory != NULL );
	if ( ppFactory->pFactory == NULL )
	{
		*ppFactory = FactoryPointer;
	};


	hOverallResult = S_OK;


Cleanup:
    
	if( hOverallResult != S_OK )
	{
		if( ppDevice->pDevice != NULL )
		{
			ppDevice->pDevice->Release();
			ppDevice->pDevice = NULL;
		}
		if( AdapterPointer.pAdapter != NULL )
		{
			AdapterPointer.pAdapter->Release();
			AdapterPointer.pAdapter = NULL;
			ppAdapter->pAdapter = NULL;
		}
		if( FactoryPointer.pFactory != NULL )
		{
			FactoryPointer.pFactory->Release();
			FactoryPointer.pFactory = NULL;
			ppFactory->pFactory = NULL;
		}

	}

	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  LoadD3D11RuntimeLayer
//
// Purpose:
// Loads the Direct3D 11 runtime layer DLL.
///////////////////////////////////////////////////////
HRESULT CDXGITestFramework::LoadD3D11RuntimeLayer()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate members and state.
	//

	assert( m_hD3D11Layer == NULL );


	//
	// Load the DLL.
	//

	//m_hD3D11Layer = LoadLibrary( "D3D11RuntimeLayers.dll" );
	m_hD3D11Layer = LoadLibraryExW
	(
		L"d3d11.dll",
		NULL,
		0
	);

	if ( m_hD3D11Layer == NULL )
	{
		DWORD LoadLibraryError = GetLastError();

		WriteToLogF
		(
			//_T( "CDXGITestFramework::LoadD3D11RuntimeLayer:  LoadLibrary(\"d3d11RuntimeLayers.dll\") failed with error %u." ),
			_T( "CDXGITestFramework::LoadD3D11RuntimeLayer:  LoadLibrary(\"d3d11.dll\") failed with error %u." ),
			LoadLibraryError
		);

		if ( LoadLibraryError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( LoadLibraryError );
		};

		goto Cleanup;
	};


	//
	// Find our entry points.
	//

	m_pD3D11LayerCreateDevice = (PFN_D3D11_CREATE_DEVICE) GetProcAddress
	(
		m_hD3D11Layer, 
		"D3D11CreateDevice"
	);

	if ( m_pD3D11LayerCreateDevice == NULL )
	{
		DWORD GetProcAddressError = GetLastError();

		WriteToLogF
		(
			_T( "CDXGITestFramework::LoadD3D11RuntimeLayer:  GetProcAddress(\"D3D11CreateDevice\") failed with error %u." ),
			GetProcAddressError
		);

		if ( GetProcAddressError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( GetProcAddressError );
		};

		goto Cleanup;
	};

        
	m_pD3D11LayerCreateDeviceAndSwapChain = (PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN) GetProcAddress
	(
		m_hD3D11Layer, 
		"D3D11CreateDeviceAndSwapChain"
	);

	if (m_pD3D11LayerCreateDeviceAndSwapChain == NULL)
	{
		DWORD GetProcAddressError = GetLastError();

		WriteToLogF
		(
			_T( "CDXGITestFramework::LoadD3D11RuntimeLayer:  GetProcAddress(\"D3D11CreateDeviceAndSwapChain\") failed with error 0x%x." ),
			GetProcAddressError
		);

		if ( GetProcAddressError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( GetProcAddressError );
		};

		goto Cleanup;
	};


	//
	// If we made it this far, then we're good to go.
	//

	hOverallResult = S_OK;


Cleanup:
	if ( FAILED( hOverallResult ) )
	{
		if ( m_hD3D11Layer != NULL )
		{
			m_pD3D11LayerCreateDevice = NULL;
			m_pD3D11LayerCreateDeviceAndSwapChain = NULL;

			FreeLibrary( m_hD3D11Layer );
			m_hD3D11Layer = NULL;
		};
	};


	return hOverallResult;
};

///////////////////////////////////////////////////////
// Function:  OptionCallback_AdapterID
//
// Purpose:
// Called from InitTestOptions in response to an
// adapter ID switch.
///////////////////////////////////////////////////////
void CDXGITestFramework::OptionCallback_AdapterID( const UINT &adapterID )
{
	CGraphicsTestFramework *pFramework = dynamic_cast< CGraphicsTestFramework *>( GetFramework() );
	pFramework->m_DXGIOptions.AdapterIDSrc = 
	pFramework->m_DXGIOptions.AdapterIDRef = adapterID;
}