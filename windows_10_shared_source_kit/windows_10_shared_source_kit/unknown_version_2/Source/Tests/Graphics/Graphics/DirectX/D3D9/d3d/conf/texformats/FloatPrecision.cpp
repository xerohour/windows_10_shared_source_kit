/*==========================================================================;
 *
 *  Copyright (C) 1994-2004 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	FloatPrecision.cpp
 *
 ***************************************************************************/

#include <d3dx9.h>
#include <types.h>
#include "cd3dx.h"		//for D3DXAssembleShader9
#include "strsafe.h"

#include "FloatPrecision.h"

#define TEXSIZE 8

#if FAST_TEST
const FloatPrecisionParam f16PrecisionTable[] =
{
    FloatPrecisionParam( 0, -14 + 10, 1.1f, 10 ),
    FloatPrecisionParam( 0, -14 + 10, 1.6666f, 10 ),
    FloatPrecisionParam( 0, 0, 1.1f, 10 ),
    FloatPrecisionParam( 0, 0, 1.6666f, 10 ),
    FloatPrecisionParam( 1, 15, 1.1f, 10 ),
    FloatPrecisionParam( 1, 15, 1.6666f, 10 ),
    FloatPrecisionParam( 1, -14 + 10, 1.1f, 10 ),
    FloatPrecisionParam( 1, -14 + 10, 1.6666f, 10 ),
    FloatPrecisionParam( 1, 0, 1.1f, 10 ),
    FloatPrecisionParam( 1, 0, 1.6666f, 10 ),
    FloatPrecisionParam( 1, 15, 1.1f, 10 ),
    FloatPrecisionParam( 1, 15, 1.6666f, 10 ),
};

const FloatPrecisionParam f24PrecisionTable[] = 
{
    FloatPrecisionParam( 0, -62 + 16, 1.1f, 15 ),
    FloatPrecisionParam( 0, -62 + 16, 1.6666f, 15 ),
    FloatPrecisionParam( 0, 0, 1.1f, 15 ),
    FloatPrecisionParam( 0, 0, 1.6666f, 15 ),
    FloatPrecisionParam( 0, 63, 1.1f, 15 ),
    FloatPrecisionParam( 0, 63, 1.6666f, 15 ),
    FloatPrecisionParam( 1, -62 + 16, 1.1f, 15 ),
    FloatPrecisionParam( 1, -62 + 16, 1.6666f, 15 ),
    FloatPrecisionParam( 1, 0, 1.1f, 15 ),
    FloatPrecisionParam( 1, 0, 1.6666f, 15 ),
    FloatPrecisionParam( 1, 63, 1.1f, 15 ),
    FloatPrecisionParam( 1, 63, 1.6666f, 15 ),
};

const FloatPrecisionParam f32PrecisionTable[] = 
{
    FloatPrecisionParam( 0, -126 + 23, 1.1f, 22 ),
    FloatPrecisionParam( 0, -126 + 23, 1.6666f, 22 ),
    FloatPrecisionParam( 0, 0, 1.1f, 22 ),
    FloatPrecisionParam( 0, 0, 1.6666f, 22 ),
    FloatPrecisionParam( 0, 127, 1.1f, 22 ),
    FloatPrecisionParam( 0, 127, 1.6666f, 22 ),
    FloatPrecisionParam( 1, -126 + 23, 1.1f, 22 ),
    FloatPrecisionParam( 1, -126 + 23, 1.6666f, 22 ),
    FloatPrecisionParam( 1, 0, 1.1f, 22 ),
    FloatPrecisionParam( 1, 0, 1.6666f, 22 ),
    FloatPrecisionParam( 1, 127, 1.1f, 22 ),
    FloatPrecisionParam( 1, 127, 1.6666f, 22 ),
};

const FloatPrecisionParam f16BadValuesTable[] = 
{
    FloatPrecisionParam( (DWORD) 0xffeeffee, .1f ), // QNAN
    FloatPrecisionParam( (DWORD) 0x7f80ffff, .1f ), // SNAN
    FloatPrecisionParam( (DWORD) 0x7f800000, .1f ), // +INF
    FloatPrecisionParam( (DWORD) 0xff800000, .1f ), // -INF
    FloatPrecisionParam( 0, 16, 1.5, 10 ),
    FloatPrecisionParam( 1, 16, 1.5, 10 ),
    FloatPrecisionParam( 0, -16, 1.5, 10 ),
    FloatPrecisionParam( 1, -16, 1.5, 10 ),
};

const FloatPrecisionParam f24BadValuesTable[] = 
{
    FloatPrecisionParam( (DWORD) 0xffeeffee, .1f ), // QNAN
    FloatPrecisionParam( (DWORD) 0x7f80ffff, .1f ), // SNAN
    FloatPrecisionParam( (DWORD) 0x7f800000, .1f ), // +INF
    FloatPrecisionParam( (DWORD) 0xff800000, .1f ), // -INF
    FloatPrecisionParam( 0, 68, 1.5, 10 ),
    FloatPrecisionParam( 1, 68, 1.5, 10 ),
    FloatPrecisionParam( 0, -68, 1.5, 10 ),
    FloatPrecisionParam( 1, -68, 1.5, 10 ),
};

const FloatPrecisionParam f32BadValuesTable[] = 
{
    FloatPrecisionParam( (DWORD) 0xffeeffee, .1f ), // QNAN
    FloatPrecisionParam( (DWORD) 0x7f80ffff, .1f ), // SNAN
    FloatPrecisionParam( (DWORD) 0x7f800000, .1f ), // +INF
    FloatPrecisionParam( (DWORD) 0xff800000, .1f ), // -INF
    FloatPrecisionParam( (DWORD) 0x000fffff, .1f ), // +Denorm
    FloatPrecisionParam( (DWORD) 0x800fffff, .1f ), // -Denorm
};
#else // FAST_TEST
const FloatPrecisionParam f16PrecisionTable[] = 
{
	FloatPrecisionParam( 0, -14 + 10, 1.1f, 10 ),
	FloatPrecisionParam( 0, -14 + 10, 1.123f, 10 ),
	FloatPrecisionParam( 0, -14 + 10, 1.5f, 10 ),
	FloatPrecisionParam( 0, -14 + 10, 1.333f, 10 ),
	FloatPrecisionParam( 0, -14 + 10, 1.9f, 10 ),
	FloatPrecisionParam( 0, -14 + 10, 1.0001f, 10 ),
	FloatPrecisionParam( 0, -14 + 10, 1.125f, 10 ),
	FloatPrecisionParam( 0, -14 + 10, 1.6666f, 10 ),
	FloatPrecisionParam( 0, 0, 1.1f, 10 ),
	FloatPrecisionParam( 0, 0, 1.123f, 10 ),
	FloatPrecisionParam( 0, 0, 1.5f, 10 ),
	FloatPrecisionParam( 0, 0, 1.333f, 10 ),
	FloatPrecisionParam( 0, 0, 1.9f, 10 ),
	FloatPrecisionParam( 0, 0, 1.0001f, 10 ),
	FloatPrecisionParam( 0, 0, 1.125f, 10 ),
	FloatPrecisionParam( 0, 0, 1.6666f, 10 ),
	FloatPrecisionParam( 1, 15, 1.1f, 10 ),
	FloatPrecisionParam( 1, 15, 1.123f, 10 ),
	FloatPrecisionParam( 1, 15, 1.5f, 10 ),
	FloatPrecisionParam( 1, 15, 1.333f, 10 ),
	FloatPrecisionParam( 1, 15, 1.9f, 10 ),
	FloatPrecisionParam( 1, 15, 1.0001f, 10 ),
	FloatPrecisionParam( 1, 15, 1.125f, 10 ),
	FloatPrecisionParam( 1, 15, 1.6666f, 10 ),
	FloatPrecisionParam( 1, -14 + 10, 1.1f, 10 ),
	FloatPrecisionParam( 1, -14 + 10, 1.123f, 10 ),
	FloatPrecisionParam( 1, -14 + 10, 1.5f, 10 ),
	FloatPrecisionParam( 1, -14 + 10, 1.333f, 10 ),
	FloatPrecisionParam( 1, -14 + 10, 1.9f, 10 ),
	FloatPrecisionParam( 1, -14 + 10, 1.0001f, 10 ),
	FloatPrecisionParam( 1, -14 + 10, 1.125f, 10 ),
	FloatPrecisionParam( 1, -14 + 10, 1.6666f, 10 ),
	FloatPrecisionParam( 1, 0, 1.1f, 10 ),
	FloatPrecisionParam( 1, 0, 1.123f, 10 ),
	FloatPrecisionParam( 1, 0, 1.5f, 10 ),
	FloatPrecisionParam( 1, 0, 1.333f, 10 ),
	FloatPrecisionParam( 1, 0, 1.9f, 10 ),
	FloatPrecisionParam( 1, 0, 1.0001f, 10 ),
	FloatPrecisionParam( 1, 0, 1.125f, 10 ),
	FloatPrecisionParam( 1, 0, 1.6666f, 10 ),
	FloatPrecisionParam( 1, 15, 1.1f, 10 ),
	FloatPrecisionParam( 1, 15, 1.123f, 10 ),
	FloatPrecisionParam( 1, 15, 1.5f, 10 ),
	FloatPrecisionParam( 1, 15, 1.333f, 10 ),
	FloatPrecisionParam( 1, 15, 1.9f, 10 ),
	FloatPrecisionParam( 1, 15, 1.0001f, 10 ),
	FloatPrecisionParam( 1, 15, 1.125f, 10 ),
	FloatPrecisionParam( 1, 15, 1.6666f, 10 ),
};

const FloatPrecisionParam f24PrecisionTable[] = 
{
	FloatPrecisionParam( 0, -62 + 16, 1.1f, 15 ),
	FloatPrecisionParam( 0, -62 + 16, 1.123f, 15 ),
	FloatPrecisionParam( 0, -62 + 16, 1.5f, 15 ),
	FloatPrecisionParam( 0, -62 + 16, 1.333f, 15 ),
	FloatPrecisionParam( 0, -62 + 16, 1.9f, 15 ),
	FloatPrecisionParam( 0, -62 + 16, 1.0001f, 15 ),
	FloatPrecisionParam( 0, -62 + 16, 1.125f, 15 ),
	FloatPrecisionParam( 0, -62 + 16, 1.6666f, 15 ),
	FloatPrecisionParam( 0, 0, 1.1f, 15 ),
	FloatPrecisionParam( 0, 0, 1.123f, 15 ),
	FloatPrecisionParam( 0, 0, 1.5f, 15 ),
	FloatPrecisionParam( 0, 0, 1.333f, 15 ),
	FloatPrecisionParam( 0, 0, 1.9f, 15 ),
	FloatPrecisionParam( 0, 0, 1.0001f, 15 ),
	FloatPrecisionParam( 0, 0, 1.125f, 15 ),
	FloatPrecisionParam( 0, 0, 1.6666f, 15 ),
	FloatPrecisionParam( 0, 63, 1.1f, 15 ),
	FloatPrecisionParam( 0, 63, 1.123f, 15 ),
	FloatPrecisionParam( 0, 63, 1.5f, 15 ),
	FloatPrecisionParam( 0, 63, 1.333f, 15 ),
	FloatPrecisionParam( 0, 63, 1.9f, 15 ),
	FloatPrecisionParam( 0, 63, 1.0001f, 15 ),
	FloatPrecisionParam( 0, 63, 1.125f, 15 ),
	FloatPrecisionParam( 0, 63, 1.6666f, 15 ),
	FloatPrecisionParam( 1, -62 + 16, 1.1f, 15 ),
	FloatPrecisionParam( 1, -62 + 16, 1.123f, 15 ),
	FloatPrecisionParam( 1, -62 + 16, 1.5f, 15 ),
	FloatPrecisionParam( 1, -62 + 16, 1.333f, 15 ),
	FloatPrecisionParam( 1, -62 + 16, 1.9f, 15 ),
	FloatPrecisionParam( 1, -62 + 16, 1.0001f, 15 ),
	FloatPrecisionParam( 1, -62 + 16, 1.125f, 15 ),
	FloatPrecisionParam( 1, -62 + 16, 1.6666f, 15 ),
	FloatPrecisionParam( 1, 0, 1.1f, 15 ),
	FloatPrecisionParam( 1, 0, 1.123f, 15 ),
	FloatPrecisionParam( 1, 0, 1.5f, 15 ),
	FloatPrecisionParam( 1, 0, 1.333f, 15 ),
	FloatPrecisionParam( 1, 0, 1.9f, 15 ),
	FloatPrecisionParam( 1, 0, 1.0001f, 15 ),
	FloatPrecisionParam( 1, 0, 1.125f, 15 ),
	FloatPrecisionParam( 1, 0, 1.6666f, 15 ),
	FloatPrecisionParam( 1, 63, 1.1f, 15 ),
	FloatPrecisionParam( 1, 63, 1.123f, 15 ),
	FloatPrecisionParam( 1, 63, 1.5f, 15 ),
	FloatPrecisionParam( 1, 63, 1.333f, 15 ),
	FloatPrecisionParam( 1, 63, 1.9f, 15 ),
	FloatPrecisionParam( 1, 63, 1.0001f, 15 ),
	FloatPrecisionParam( 1, 63, 1.125f, 15 ),
	FloatPrecisionParam( 1, 63, 1.6666f, 15 ),
};

const FloatPrecisionParam f32PrecisionTable[] = 
{
	FloatPrecisionParam( 0, -126 + 23, 1.1f, 22 ),
	FloatPrecisionParam( 0, -126 + 23, 1.123f, 22 ),
	FloatPrecisionParam( 0, -126 + 23, 1.5f, 22 ),
	FloatPrecisionParam( 0, -126 + 23, 1.333f, 22 ),
	FloatPrecisionParam( 0, -126 + 23, 1.9f, 22 ),
	FloatPrecisionParam( 0, -126 + 23, 1.0001f, 22 ),
	FloatPrecisionParam( 0, -126 + 23, 1.125f, 22 ),
	FloatPrecisionParam( 0, -126 + 23, 1.6666f, 22 ),
	FloatPrecisionParam( 0, 0, 1.1f, 22 ),
	FloatPrecisionParam( 0, 0, 1.123f, 22 ),
	FloatPrecisionParam( 0, 0, 1.5f, 22 ),
	FloatPrecisionParam( 0, 0, 1.333f, 22 ),
	FloatPrecisionParam( 0, 0, 1.9f, 22 ),
	FloatPrecisionParam( 0, 0, 1.0001f, 22 ),
	FloatPrecisionParam( 0, 0, 1.125f, 22 ),
	FloatPrecisionParam( 0, 0, 1.6666f, 22 ),
	FloatPrecisionParam( 0, 127, 1.1f, 22 ),
	FloatPrecisionParam( 0, 127, 1.123f, 22 ),
	FloatPrecisionParam( 0, 127, 1.5f, 22 ),
	FloatPrecisionParam( 0, 127, 1.333f, 22 ),
	FloatPrecisionParam( 0, 127, 1.9f, 22 ),
	FloatPrecisionParam( 0, 127, 1.0001f, 22 ),
	FloatPrecisionParam( 0, 127, 1.125f, 22 ),
	FloatPrecisionParam( 0, 127, 1.6666f, 22 ),
	FloatPrecisionParam( 1, -126 + 23, 1.1f, 22 ),
	FloatPrecisionParam( 1, -126 + 23, 1.123f, 22 ),
	FloatPrecisionParam( 1, -126 + 23, 1.5f, 22 ),
	FloatPrecisionParam( 1, -126 + 23, 1.333f, 22 ),
	FloatPrecisionParam( 1, -126 + 23, 1.9f, 22 ),
	FloatPrecisionParam( 1, -126 + 23, 1.0001f, 22 ),
	FloatPrecisionParam( 1, -126 + 23, 1.125f, 22 ),
	FloatPrecisionParam( 1, -126 + 23, 1.6666f, 22 ),
	FloatPrecisionParam( 1, 0, 1.1f, 22 ),
	FloatPrecisionParam( 1, 0, 1.123f, 22 ),
	FloatPrecisionParam( 1, 0, 1.5f, 22 ),
	FloatPrecisionParam( 1, 0, 1.333f, 22 ),
	FloatPrecisionParam( 1, 0, 1.9f, 22 ),
	FloatPrecisionParam( 1, 0, 1.0001f, 22 ),
	FloatPrecisionParam( 1, 0, 1.125f, 22 ),
	FloatPrecisionParam( 1, 0, 1.6666f, 22 ),
	FloatPrecisionParam( 1, 127, 1.1f, 22 ),
	FloatPrecisionParam( 1, 127, 1.123f, 22 ),
	FloatPrecisionParam( 1, 127, 1.5f, 22 ),
	FloatPrecisionParam( 1, 127, 1.333f, 22 ),
	FloatPrecisionParam( 1, 127, 1.9f, 22 ),
	FloatPrecisionParam( 1, 127, 1.0001f, 22 ),
	FloatPrecisionParam( 1, 127, 1.125f, 22 ),
	FloatPrecisionParam( 1, 127, 1.6666f, 22 ),
};

const FloatPrecisionParam f16BadValuesTable[] = 
{
	FloatPrecisionParam( (DWORD) 0xffeeffee, .1f ), // QNAN
	FloatPrecisionParam( (DWORD) 0x7f80ffff, .1f ), // SNAN
	FloatPrecisionParam( (DWORD) 0x7f800000, .1f ), // +INF
	FloatPrecisionParam( (DWORD) 0xff800000, .1f ), // -INF
	FloatPrecisionParam( 0, 16, 1.5, 10 ),
	FloatPrecisionParam( 1, 16, 1.5, 10 ),
	FloatPrecisionParam( 0, -16, 1.5, 10 ),
	FloatPrecisionParam( 1, -16, 1.5, 10 ),
};

const FloatPrecisionParam f24BadValuesTable[] = 
{
	FloatPrecisionParam( (DWORD) 0xffeeffee, .1f ), // QNAN
	FloatPrecisionParam( (DWORD) 0x7f80ffff, .1f ), // SNAN
	FloatPrecisionParam( (DWORD) 0x7f800000, .1f ), // +INF
	FloatPrecisionParam( (DWORD) 0xff800000, .1f ), // -INF
	FloatPrecisionParam( 0, 68, 1.5, 10 ),
	FloatPrecisionParam( 1, 68, 1.5, 10 ),
	FloatPrecisionParam( 0, -68, 1.5, 10 ),
	FloatPrecisionParam( 1, -68, 1.5, 10 ),
};

const FloatPrecisionParam f32BadValuesTable[] = 
{
	FloatPrecisionParam( (DWORD) 0xffeeffee, .1f ), // QNAN
	FloatPrecisionParam( (DWORD) 0x7f80ffff, .1f ), // SNAN
	FloatPrecisionParam( (DWORD) 0x7f800000, .1f ), // +INF
	FloatPrecisionParam( (DWORD) 0xff800000, .1f ), // -INF
	FloatPrecisionParam( (DWORD) 0x000fffff, .1f ), // +Denorm
	FloatPrecisionParam( (DWORD) 0x800fffff, .1f ), // -Denorm
};

#endif//FAST_TEST


CFloatTexPrecisionTest::CFloatTexPrecisionTest() :
	m_pTexFormat( NULL ),
	m_pPrecisionTable( NULL ),
	m_pPixelShader( NULL )
{
	SetTestType( TESTTYPE_CONF );
	
    // Init Framework options
	m_Options.ModeOptions.dwTexInclude = PF_FLOATTEX;
	m_Options.ModeOptions.TextureFmt = FMT_ALL;

	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ANY;
	m_Options.D3DOptions.bReference = false;
	m_Options.TestOptions.bCompareSrcRef = false;
	
	m_Options.D3DOptions.fMinDXVersion = 9.0f;
	
	m_Options.D3DOptions.nBackBufferWidth = TEXSIZE;
	m_Options.D3DOptions.nBackBufferHeight = TEXSIZE;
}

CFloatTexPrecisionTest::~CFloatTexPrecisionTest()
{
}

bool CFloatTexPrecisionTest::BuildPixelShader( const char *lpzSource, CnPixelShader **ppPixelShader )
{
	LPD3DXBUFFER pXCode = NULL;
	LPD3DXBUFFER pXError = NULL;
	
	HRESULT hr = D3DXAssembleShader9( lpzSource, (int) strlen( lpzSource ), NULL, NULL, 0, &pXCode, &pXError );
	
	if( FAILED( hr ) )
	{
		WriteToLog(_T("Pixel Shader failed to assemble. (%08X)\n"), hr);
		RELEASE(pXCode);
		RELEASE(pXError);
		return false;
	}
	
	if( !CreatePixelShader( (DWORD*) pXCode->GetBufferPointer(), ppPixelShader ) )
	{
		WriteToLog( _T( "CreatePixelShader() failed with HResult = %s.\n" ), m_pD3D->HResultToString( GetLastError() ) );
		RELEASE( pXCode );
		RELEASE( pXError );
		return false;
	}
	
	RELEASE( pXCode );
	RELEASE( pXError );
	
	return true;
}

bool CFloatTexPrecisionTest::CapsCheck()
{
	FORMAT *pFormatRT;
	
	if (m_pSrcDevice != NULL)
	{
		//check to make sure src device supports the desired pixel shader version
		if (((m_pSrcDevice->GetCaps()->dwPixelShaderVersion & 0x0000FF00) >> 8) < GetPSVersionMajor() )
		{
			WriteToLog ("Device does not support minimum pixel shader version (ps_%i_0).\n", GetPSVersionMajor() );
			return false;
		}

		// Tell framework what formats to use
		pFormatRT = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
		
		if( FAILED( m_pD3D->CheckDeviceFormat(	m_pSrcDevice->GetAdapterID(), 
												m_pSrcDevice->GetDevType(), 
												m_pCurrentMode->pDisplay->Format,
												USAGE_RENDERTARGET, 
												RTYPE_TEXTURE, 
												*pFormatRT ) ) )
		{
			return false;
		}
	}
	
	return CD3DTest::CapsCheck();
}

bool CFloatTexPrecisionTest::Setup()
{
	m_pTexFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

	// Build the shaders
	const char lpszShader20[] = 
		"ps.2.0\n"
		"dcl t0.xy\n"
		"dcl_2d s0\n"
		
		// Initialize r1 to [1,1,1,1]
		"mov r1, c2\n"
		
		// See that abs( ref_value - tex_color ) < epsilon
		"texld r2, t0, s0\n"
		"sub r0, r2, c0\n"
		"abs r0, r0\n"
		"sub r0, r0, c1\n"		
		"cmp r1, r0, c3, r1\n"

		// Now all failed channels are 0 and all passed channels are 1.
		// Invert RGB, divide by two, and add alpha error.
		// This will tell us which colors failed
		// (if alpha fails then add .75 to the other channels)
		"sub r2, c2, r1\n"		// invert error colors
		"mul r1, r2, c4\n"		// divide error colors by 2
		"mad r2, r2.a, r2.a, r2.a\n"	// add alpha error to rgb channels
		"add r1, r1, r2\n"
		
		
		// Sanity check: Make sure ref_value - epsilon < ref_value
		// If not then PS has less than the required precision.
		// Set texture coordinate as error color.
		"mov r2, c0.r\n"
		"sub r0, r2, c1.r\n"	// ref_value - epsilon
		"sub r0, r0, r2\n"		// ( ref_value - epsilon ) - ref_value
		"cmp r1.xy, r0, t0, r1\n"	// if r0 < 0 then r1 isn't changed, else fail
		
		"mov oC0, r1\n";
		
	const char lpszShader30[] =
		"ps.3.0\n"
		"dcl_texcoord0 v0.xy\n"
		"dcl_2d s0\n"
		
		// Initialize r1 to [1,1,1,1]
		"mov r1, c2\n"
		
		// See that abs( ref_value - tex_color ) < epsilon
		"texld r2, v0, s0\n"
		"sub r0, r2, c0\n"
		"abs r0, r0\n"
		"sub r0, r0, c1\n"		
		"cmp r1, r0, c3, r1\n"

		// Now all failed channels are 0 and all passed channels are 1.
		// Invert RGB, divide by two, and add alpha error.
		// This will tell us which colors failed
		// (if alpha fails then add .75 to the other channels)
		"sub r2, c2, r1\n"		// invert error colors
		"mul r1, r2, c4\n"		// divide error colors by 2
		"mad r2, r2.a, r2.a, r2.a\n"	// add alpha error to rgb channels
		"add r1, r1, r2\n"
		
		
		// Sanity check: Make sure ref_value - epsilon < ref_value
		// If not then PS has less than the required precision.
		// Set texture coordinate as error color.
		"mov r2, c0.r\n"
		"sub r0, r2, c1.r\n"	// ref_value - epsilon
		"sub r0, r0, r2\n"		// ( ref_value - epsilon ) - ref_value
		"cmp r1.xy, r0, v0, r1\n"	// if r0 < 0 then r1 isn't changed, else fail
		
		"mov oC0, r1\n";

	bool bResult;
	
	switch( GetPSVersionMajor() )
	{
		default:
		case 2:
			bResult = BuildPixelShader( lpszShader20, &m_pPixelShader );
			break;
		
		case 3:
			bResult = BuildPixelShader( lpszShader30, &m_pPixelShader );
			break;
	}
	
	return bResult;
}

void CFloatTexPrecisionTest::Cleanup()
{
	RELEASE( m_pPixelShader );
	
	return CD3DTest::Cleanup();
}


//-----------------------------------------------------------------------------
// Name:	ExecutePrecisionTest()
// Desc:	Function is used by the precision test group.  It creates a 
//			solid color texture first with r = g = b = a = fValue then passes
//			the texture to the GPU where a shader compares the color to a 
//			reference value to determine whether the difference is less than
//			epsilon.  Ref Rast is not used in these tests.
//-----------------------------------------------------------------------------

bool CFloatTexPrecisionTest::ExecuteTest( UINT uTestNumber )
{
	bool bResult;
	bool bReturn = true;
	CnTexture* pTex = NULL;
	CImage* pImage = NULL;
	FColor *fData = NULL;
    int nWidth = m_pCurrentMode->nBackBufferWidth;
    int nHeight = m_pCurrentMode->nBackBufferHeight;

	int nIndex = uTestNumber - 1;
    float fValue  = m_pPrecisionTable[nIndex].value;
    float fEpsilon = m_pPrecisionTable[nIndex].value;
    
	//create the texture
	//width, height, levels, usage, format, pool, texture *
	bResult = CreateTexture( nWidth, nHeight, 1, FMWK_DEFAULT, *m_pTexFormat, FMWK_DEFAULT, &pTex );
	if( !bResult || (NULL == pTex))
	{
		WriteToLog ("CreateTexture() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//create the image that we are going to put into the texture
	pImage = new CImage;
	if( NULL == pImage )
	{
		WriteToLog ("ExecutePrecisionTest() failed to create new CImage: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//create the data that is going into the image that will go into the texture
	fData = new FColor[nWidth*nHeight];
	if (NULL == fData)
	{
		WriteToLog ("ExecutePrecisionTest() failed to allocate FColor data: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//fill the data
	FColor *pfData = fData;
	
	for( int i = nWidth * nHeight; i > 0; --i )
	{
		pfData->A = pfData->R = pfData->G = pfData->B = fValue;
		pfData++;
	}

	//set the data into the image
	if( !pImage->SetData( nWidth, nHeight, fData ) )
	{
		WriteToLog  ("CImage::SetData() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//set the image into the texture
	if( !pTex->LoadImage( 0, pImage ) )
	{
		WriteToLog  ("LoadImage() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	
	char buffer[128];
	if( FAILED( StringCchPrintf( buffer, sizeof( buffer ), "Value: %g, Epsilon: %g", fValue, fEpsilon ) ) )
		assert( !"String too long" );
	
	BeginTestCase( buffer, uTestNumber );

	//start the scene
	if (!BeginScene())
	{
		WriteToLog  ("BeginScene() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	
	// Set the texture
	SetTexture( 0, pTex );
	
	// Turn off blender
	SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	
	// Turn off filter
	SetSamplerState( 0, SAMP_MINFILTER, D3DTEXF_POINT );
	SetSamplerState( 0, SAMP_MAGFILTER, D3DTEXF_POINT );
	
	// Set the pixel shader
	SetPixelShader( m_pPixelShader );
	
	float c0[4];
	float c1[4];
	const float c2[4] = { 1, 1, 1, 1 };
	const float c3[4] = { 0, 0, 0, 0 };
	const float c4[4] = { .5f, .5f, .5f, .5f };
	
	c0[0] = fValue;
	c1[0] = fEpsilon;
	
	switch( (FMT)m_pTexFormat->d3dfFormat )
	{
		default:
			c0[1] = c0[2] = c0[3] = 1;
			c1[1] = c1[2] = c1[3] = .15f;
			break;
			
		case FMT_G16R16F:
		case FMT_G32R32F:
			c0[1] = fValue;
			c1[1] = fEpsilon;
			c0[2] = c0[3] = 1;
			c1[2] = c1[3] = .15f;
			break;
			
		case FMT_A16B16G16R16F:
		case FMT_A32B32G32R32F:
			c0[1] = fValue;
			c1[1] = fEpsilon;
			c0[2] = fValue;
			c1[2] = fEpsilon;
			c0[3] = fValue;
			c1[3] = fEpsilon;
			break;
	}
	
	SetPixelShaderConstant( 0, (void *) c0, 1 );
	SetPixelShaderConstant( 1, (void *) c1, 1 );
	SetPixelShaderConstant( 2, (void *) c2, 1 );
	SetPixelShaderConstant( 3, (void *) c3, 1 );
	SetPixelShaderConstant( 4, (void *) c4, 1 );

	if (!RenderPrimitive (D3DPT_TRIANGLELIST, FMT_TLVERT, m_pVertList, 6, NULL, 0, 0))
	{
		WriteToLog  ("RenderPrimitive() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}


	//end the scene
	if (!EndScene())
	{
		WriteToLog  ("EndScene() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//and finish
Exit:
	SAFEDELETEA (fData);
	RELEASE (pTex);
	SAFEDELETE (pImage);
	return bReturn;
}

void CFloatTexPrecisionTest::UpdateStatus()
{
	CD3DTest::UpdateStatus();
}

void CFloatTexPrecisionTest::UpdateDetails()
{
	CD3DTest::UpdateDetails();
}

bool CFloatTexPrecisionTest::ProcessFrame()
{
	BOOL bRes = true;
	CSurface *pSrcSurf = NULL;
	FSurface *pFSurf = NULL;
	SURFACEDESC  Desc;

	// Get the src buffer
	HRESULT hr = m_pSrcDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSrcSurf);
	
	if(FAILED(hr))
	{
		DPF( 1, _T( "CD3DTest::ProcessFrame() - GetBackBufferCached failed on SRC. hr=%s(%x).\n"),
			m_pD3D->HResultToString(hr));
		bRes = false;
		goto Exit;
	}

	hr = pSrcSurf->GetDesc(&Desc);
    if (FAILED(hr))
    {
        DPF( 1, _T( "CImageCompare::FloatCompare() - GetDescription failed surface 1. hr = %s(%x).\n"),
			m_pD3D->HResultToString( hr ), hr);
        goto Exit;
    }

	pFSurf = new FSurface();
	
	if( NULL == pFSurf)
	{
		DPF(1, _T("CImageCompare::FloatCompare() - Out of memory.\n"));
		bRes = false;
		goto Exit;
	}
	
	hr = pFSurf->ConvertFrom( pSrcSurf );
	if( FAILED( hr ) )
	{
		DPF( 1, _T( "FSurface::ConvertFrom() - failed to convert SRC surface.\n" ) );
		bRes = false;
		goto Exit;
	}
	
	FColor *color0 = pFSurf->GetData();
	FColor *color1 = pFSurf->GetData() + Desc.Width * Desc.Height - 1;
	
	if( fabsf( color0->R - color1->R ) > .5f )
	{
		Fail();
		m_fPassPercentage = 0;
		WriteToLog( _T( "PS Precision below requirement\n" ) );
	}
	else
	{
		float error = .25;
		
		bool pass = true;
		if( color0->R > .7f )
		{
			error = .9f;
			pass = false;
			WriteToLog( _T( "Alpha channel precision below requirement\n" ) );
		}
	
		if( color0->R > error )
		{
			pass = false;
			WriteToLog( _T( "Red channel precision below requirement\n" ) );
		}	
	
		if( color0->G > error )
		{
			pass = false;
			WriteToLog( _T( "Green channel precision below requirement\n" ) );
		}	
	
		if( color0->B > error )
		{
			pass = false;
			WriteToLog( _T( "Blue channel precision below requirement\n" ) );
		}
		
		if( pass )
		{
			Pass();
			m_fPassPercentage = 100;
		}
		else
		{
			Fail();
			m_fPassPercentage = 0;
		}
	}
			
Exit:
	SAFEDELETE( pFSurf );
	RELEASE( pSrcSurf );
	
	return true;
}


CFloatTexPrecisionTest20::CFloatTexPrecisionTest20()
{
	m_szTestName = "FloatPrecisionTest PS 2.0";
	m_szCommandKey = "FloatPrec20";
}

UINT CFloatTexPrecisionTest20::TestInitialize()
{
	UINT nResult = CFloatTexPrecisionTest::TestInitialize();
	
	if( nResult == D3DTESTINIT_RUN )
	{
		//determine if our current format is 16 bit or 32 bit
		if ((FMT)m_pTexFormat->d3dfFormat == FMT_R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_G16R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_A16B16G16R16F)
		{
			SetTestRange( 1, sizeof( f16PrecisionTable ) / sizeof( f16PrecisionTable[0] ) );
			m_pPrecisionTable = f16PrecisionTable;
		}
		else
		{
			SetTestRange( 1, sizeof( f24PrecisionTable ) / sizeof( f24PrecisionTable[0] ) );
			m_pPrecisionTable = f24PrecisionTable;
		}	
	}
		
	return nResult;
}

CFloatTexPrecisionTest30::CFloatTexPrecisionTest30()
{
	m_szTestName = "FloatPrecisionTest PS 3.0";
	m_szCommandKey = "FloatPrec30";
}

UINT CFloatTexPrecisionTest30::TestInitialize()
{
	UINT nResult = CFloatTexPrecisionTest::TestInitialize();
	
	if( nResult == D3DTESTINIT_RUN )
	{
		//determine if our current format is 16 bit or 32 bit
		if ((FMT)m_pTexFormat->d3dfFormat == FMT_R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_G16R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_A16B16G16R16F)
		{
			SetTestRange( 1, sizeof( f16PrecisionTable ) / sizeof( f16PrecisionTable[0] ) );
			m_pPrecisionTable = f16PrecisionTable;
		}
		else
		{
			SetTestRange( 1, sizeof( f32PrecisionTable ) / sizeof( f32PrecisionTable[0] ) );
			m_pPrecisionTable = f32PrecisionTable;
		}	
	}
		
	return nResult;
}




CFloatTexBadValuesTest20::CFloatTexBadValuesTest20()
{
	m_szTestName = "FloatBadValuesTest PS 2.0";
	m_szCommandKey = "FloatBV20";
}

UINT CFloatTexBadValuesTest20::TestInitialize()
{
	UINT nResult = CFloatTexPrecisionTest::TestInitialize();
	
	if( nResult == D3DTESTINIT_RUN )
	{
		//determine if our current format is 16 bit or 32 bit
		if ((FMT)m_pTexFormat->d3dfFormat == FMT_R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_G16R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_A16B16G16R16F)
		{
			SetTestRange( 1, sizeof( f16BadValuesTable ) / sizeof( f16BadValuesTable[0] ) );
			m_pPrecisionTable = f16BadValuesTable;
		}
		else
		{
			SetTestRange( 1, sizeof( f24BadValuesTable ) / sizeof( f24BadValuesTable[0] ) );
			m_pPrecisionTable = f24BadValuesTable;
		}	
	}
		
	return nResult;
}



bool CFloatTexBadValuesTest20::ProcessFrame()
{
	// Automatically pass.
	// We're just trying to make sure that invalid floating point
	// values in a texture don't cause the hardware/driver to lockup/crash.
	Pass();
	
	return true;
}




CFloatTexBadValuesTest30::CFloatTexBadValuesTest30()
{
	m_szTestName = "FloatBadValuesTest PS 3.0";
	m_szCommandKey = "FloatBV30";
}

UINT CFloatTexBadValuesTest30::TestInitialize()
{
	UINT nResult = CFloatTexPrecisionTest::TestInitialize();
	
	if( nResult == D3DTESTINIT_RUN )
	{
		//determine if our current format is 16 bit or 32 bit
		if ((FMT)m_pTexFormat->d3dfFormat == FMT_R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_G16R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_A16B16G16R16F)
		{
			SetTestRange( 1, sizeof( f16BadValuesTable ) / sizeof( f16BadValuesTable[0] ) );
			m_pPrecisionTable = f16BadValuesTable;
		}
		else
		{
			SetTestRange( 1, sizeof( f32BadValuesTable ) / sizeof( f32BadValuesTable[0] ) );
			m_pPrecisionTable = f32BadValuesTable;
		}	
	}
		
	return nResult;
}


bool CFloatTexBadValuesTest30::ProcessFrame()
{
	// Automatically pass.
	// We're just trying to make sure that invalid floating point
	// values in a texture don't cause the hardware/driver to lockup/crash.
	Pass();
	
	return true;
}



////////////////////////////////////////////////////////////
// CFloatTexRTPrecisionTest
//
// This class renders predefined constant color values
// out to a floating point texture and verifies that
// the required precision is used.
////////////////////////////////////////

CFloatTexRTPrecisionTest::CFloatTexRTPrecisionTest() :
	m_pTex( NULL ),
	m_pSysMemTex( NULL )
{
}

CFloatTexRTPrecisionTest::~CFloatTexRTPrecisionTest()
{
}

bool CFloatTexRTPrecisionTest::Setup()
{
	m_pTexFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];

	// Create the texture
    int nWidth = m_pCurrentMode->nBackBufferWidth;
    int nHeight = m_pCurrentMode->nBackBufferHeight;
	bool bResult = CreateTexture( nWidth, nWidth, 1, USAGE_RENDERTARGET, *m_pTexFormat, FMWK_DEFAULT, &m_pTex );
	if( !bResult || ( NULL == m_pTex ) )
	{
		WriteToLog ("CreateTexture() failed: %s.\n", GLE (GetLastError()));
		return false;
	}
	
	// Create the system memory texture
	bResult = CreateTexture( nWidth, nWidth, 1, FMWK_DEFAULT, *m_pTexFormat, POOL_SYSTEMMEM, &m_pSysMemTex );
	if( !bResult || ( NULL == m_pSysMemTex ) )
	{
		WriteToLog ("CreateTexture() failed: %s.\n", GLE (GetLastError()));
		return false;
	}

	// Build the shaders
	const char lpszShader20[] = 
		"ps.2.0\n"
		"mov r1, c0\n"
		"mov oC0, r1\n";
		
	const char lpszShader30[] =
		"ps.3.0\n"
		"mov r1, c0\n"
		"mov oC0, r1\n";

	switch( GetPSVersionMajor() )
	{
		default:
		case 2:
			bResult = BuildPixelShader( lpszShader20, &m_pPixelShader );
			break;
		
		case 3:
			bResult = BuildPixelShader( lpszShader30, &m_pPixelShader );
			break;
	}
	
	// Create the quad primitive
    float fWidth = (float) m_pCurrentMode->nBackBufferWidth;
    float fHeight = (float) m_pCurrentMode->nBackBufferHeight;
	
	m_pVertList[0] = TLVERT (0.0f,		0.0f,		0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 0.0f, 0.0f); //upper left
	m_pVertList[1] = TLVERT (fWidth,	fHeight,	0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 1.0f, 1.0f); //lower right
	m_pVertList[2] = TLVERT (0.0f,		fHeight,	0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 0.0f, 1.0f); //lower left
	m_pVertList[3] = TLVERT (0.0f,		0.0f,		0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 0.0f, 0.0f); //upper left
	m_pVertList[4] = TLVERT (fWidth,	0.0f,		0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 1.0f, 0.0f); //upper right
	m_pVertList[5] = TLVERT (fWidth,	fHeight,	0.9f, 1.0f / 0.9f, RGBA_MAKE (255,255,255,255), 1.0f, 1.0f); //lower right

	return bResult;			
}

void CFloatTexRTPrecisionTest::Cleanup()
{
	RELEASE( m_pTex );
	RELEASE( m_pSysMemTex );
	
	return CFloatTexPrecisionTest::Cleanup();
}



bool CFloatTexRTPrecisionTest::ExecuteTest( UINT uTestNumber )
{
	bool bReturn = true;
	CnSurface *pRTSurface = NULL;
	CnSurface *pRTOld = NULL;
	CnSurface *pZOld = NULL;
    int nWidth = m_pCurrentMode->nBackBufferWidth;
    int nHeight = m_pCurrentMode->nBackBufferHeight;
    
    m_nTestIndex = uTestNumber - 1;
    float fValue  = m_pPrecisionTable[m_nTestIndex].value;
    float fEpsilon = m_pPrecisionTable[m_nTestIndex].value;

	char buffer[128];
	if( FAILED( StringCchPrintf( buffer, sizeof( buffer ), "Value: %g, Epsilon: %g", fValue, fEpsilon ) ) )
		assert( !"String too long" );
	
	BeginTestCase( buffer, uTestNumber );
	
	bReturn = m_pTex->GetSurfaceLevel( 0, &pRTSurface );
	if( bReturn == false )
	{
		WriteToLog( "GetSurfaceLevel() failed with HResult = %s.\n", m_pD3D->HResultToString( GetLastError() ) );
		FAIL_AND_EXIT;
	}
	
	GetRenderTarget( &pRTOld );
	GetDepthStencilSurface( &pZOld );
	
	if( !SetRenderTarget( pRTSurface, NULL ) )
	{
		FAIL_AND_EXIT;
	}

	//start the scene
	if (!BeginScene())
	{
		WriteToLog  ("BeginScene() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	
	// Turn off post PS operations
	SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	SetRenderState( D3DRS_DITHERENABLE, false );
	SetRenderState( D3DRS_FOGENABLE, false );
	
	// Set the pixel shader
	SetPixelShader( m_pPixelShader );
	
	// Set the output color
	float c0[4];
	c0[0] = c0[1] = c0[2] = c0[3] = fValue;
	SetPixelShaderConstant( 0, (void *) c0, 1 );

	if (!RenderPrimitive (D3DPT_TRIANGLELIST, FMT_TLVERT, m_pVertList, 6, NULL, 0, 0))
	{
		WriteToLog  ("RenderPrimitive() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//end the scene
	if (!EndScene())
	{
		WriteToLog  ("EndScene() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//and finish
Exit:
	if( pRTOld )
	{
		SetRenderTarget( pRTOld, pZOld );
	}
	
	RELEASE( pRTSurface );
	RELEASE( pRTOld );
	RELEASE( pZOld );
	
	return bReturn;
}

bool CFloatTexRTPrecisionTest::ProcessFrame()
{
	BOOL bRes = true;
	CnSurface *pSrcSurf = NULL;
	CnSurface *pSysMemSurf = NULL;
	CSurface *pSurface = NULL;
	SURFACEDESC  Desc;
	FSurface *pFSurf = NULL;
	
	// Check the first pixel's value and see if 
	// it contains the correct precision

	// Get the src buffer
	bRes = m_pTex->GetSurfaceLevel( 0, &pSrcSurf );
	if( bRes == false )
	{
		WriteToLog( "GetSurfaceLevel() failed with HResult = %s.\n", m_pD3D->HResultToString( GetLastError() ) );
		goto Exit;
	}
	
	// Get the system memory surface
	bRes = m_pSysMemTex->GetSurfaceLevel( 0, &pSysMemSurf );
	if( bRes == false )
	{
		WriteToLog( "GetSurfaceLevel() failed with HResult = %s.\n", m_pD3D->HResultToString( GetLastError() ) );
		goto Exit;
	}
	
	bRes = GetRenderTargetData( pSrcSurf, pSysMemSurf );
	if( bRes == false )
	{
		WriteToLog( "GetRenderTargetData() failed with HResult = %s.\n", m_pD3D->HResultToString( GetLastError() ) );
		goto Exit;
	}

	bRes = pSysMemSurf->GetSurface( &pSurface, 0 );
	if( bRes == false )
	{
		WriteToLog( "CnSurface::GetSurface() failed\n" );
		goto Exit;
	}
	
	HRESULT hr = pSurface->GetDesc(&Desc);
    if( FAILED( hr ) )
    {
        DPF( 1, _T( "CImageCompare::FloatCompare() - GetDescription failed surface 1. hr = %s(%x).\n"),
			m_pD3D->HResultToString( hr ), hr);
        goto Exit;
    }

	pFSurf = new FSurface();
	
	if( NULL == pFSurf)
	{
		DPF(1, _T("CImageCompare::FloatCompare() - Out of memory.\n"));
		bRes = false;
		goto Exit;
	}
	
	hr = pFSurf->ConvertFrom( pSurface );
	if( FAILED( hr ) )
	{
		DPF( 1, _T( "FSurface::ConvertFrom() - failed to convert SRC surface.\n" ) );
		bRes = false;
		Fail();
		m_fPassPercentage = 0;
		goto Exit;
	}
	
	FColor *color = pFSurf->GetData();

	// See if color components are within 1 epsilon of the original color constant	
    float fValue  = m_pPrecisionTable[m_nTestIndex].value;
    float fEpsilon = m_pPrecisionTable[m_nTestIndex].epsilon;
	
	switch( m_pTexFormat->d3dfFormat )
	{
		case FMT_A16B16G16R16F:
		case FMT_A32B32G32R32F:
			if( _isnan( color->A ) || !_finite( color->A ) || fabsf( color->A - fValue ) > fEpsilon )
			{
				Fail();
				m_fPassPercentage = 0;
				WriteToLog( "Alpha channel does not meet precision requirements for RT textures.\n" );
				WriteToLog( "Expected Value %g, Actual Value: %g, Allowable Error: %g", fValue, color->A, fEpsilon );
				bRes = false;
			}
			
			if( _isnan( color->B ) || !_finite( color->B ) || fabsf( color->B - fValue ) > fEpsilon )
			{
				Fail();
				m_fPassPercentage = 0;
				WriteToLog( "Blue channel does not meet precision requirements for RT textures.\n" );
				WriteToLog( "Expected Value %g, Actual Value: %g, Allowable Error: %g", fValue, color->B, fEpsilon );
				bRes = false;
			}
			
			// Fall through to next case...

		case FMT_G16R16F:
		case FMT_G32R32F:
			if( _isnan( color->G ) || !_finite( color->G ) || fabsf( color->G - fValue ) > fEpsilon )
			{
				Fail();
				m_fPassPercentage = 0;
				WriteToLog( "Green channel does not meet precision requirements for RT textures.\n" );
				WriteToLog( "Expected Value %g, Actual Value: %g, Allowable Error: %g", fValue, color->G, fEpsilon );
				bRes = false;
			}
			
			// Fall through to next case...
			
		default:
			if( _isnan( color->R ) || !_finite( color->R ) || fabsf( color->R - fValue ) > fEpsilon )
			{
				Fail();
				m_fPassPercentage = 0;
				WriteToLog( "Red channel does not meet precision requirements for RT textures.\n" );
				WriteToLog( "Expected Value %g, Actual Value: %g, Allowable Error: %g", fValue, color->R, fEpsilon );
				bRes = false;
			}
			
			break;
	}
	
Exit:
	SAFEDELETE( pFSurf );
	RELEASE( pSurface );
	RELEASE( pSrcSurf );
	RELEASE( pSysMemSurf );
	return true;
}



CFloatTexRTPrecisionTest20::CFloatTexRTPrecisionTest20()
{
	m_szTestName = "FloatTex Render Target Precision PS 2.0";
	m_szCommandKey = "FloatRT20";
}

UINT CFloatTexRTPrecisionTest20::TestInitialize()
{
	UINT nResult = CFloatTexPrecisionTest::TestInitialize();
	
	if( nResult == D3DTESTINIT_RUN )
	{
		//determine if our current format is 16 bit or 32 bit
		if ((FMT)m_pTexFormat->d3dfFormat == FMT_R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_G16R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_A16B16G16R16F)
		{
			SetTestRange( 1, sizeof( f16PrecisionTable ) / sizeof( f16PrecisionTable[0] ) );
			m_pPrecisionTable = f16PrecisionTable;
		}
		else
		{
			SetTestRange( 1, sizeof( f24PrecisionTable ) / sizeof( f24PrecisionTable[0] ) );
			m_pPrecisionTable = f24PrecisionTable;
		}	
	}
		
	return nResult;
}



CFloatTexRTPrecisionTest30::CFloatTexRTPrecisionTest30()
{
	m_szTestName = "FloatTex Render Target Precision PS 3.0";
	m_szCommandKey = "FloatRT30";
}

UINT CFloatTexRTPrecisionTest30::TestInitialize()
{
	UINT nResult = CFloatTexPrecisionTest::TestInitialize();
	
	if( nResult == D3DTESTINIT_RUN )
	{
		//determine if our current format is 16 bit or 32 bit
		if ((FMT)m_pTexFormat->d3dfFormat == FMT_R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_G16R16F || (FMT)m_pTexFormat->d3dfFormat == FMT_A16B16G16R16F)
		{
			SetTestRange( 1, sizeof( f16PrecisionTable ) / sizeof( f16PrecisionTable[0] ) );
			m_pPrecisionTable = f16PrecisionTable;
		}
		else
		{
			SetTestRange( 1, sizeof( f32PrecisionTable ) / sizeof( f32PrecisionTable[0] ) );
			m_pPrecisionTable = f32PrecisionTable;
		}	
	}
		
	return nResult;
}

