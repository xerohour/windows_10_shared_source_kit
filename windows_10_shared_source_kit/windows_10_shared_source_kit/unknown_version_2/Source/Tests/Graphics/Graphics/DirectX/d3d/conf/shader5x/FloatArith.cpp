//////////////////////////////////////////////////////////////////////////
//  FloatArith.cpp
//  created:	2005/03/11
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include <float.h>
#include "FloatArith.h"
#include "assert.h"
#include "FloatUtils.h"

const char *resultClassNames[] =
{
	"Unknown",
	"-INF",	
	"-REAL",	
	"-0",	
	"0",	
	"REAL",	
	"INF",	
	"NAN",	
	"REAL",	
	"+SRC0",	
	"-SRC0",	
	"+SRC1",	
	"-SRC1",	
	"+SRC2",	
	"-SRC2",	
	"+1",	
	"-1",	
	"0",	
	"EXACT",
	"+DENZ",	
	"-DENZ",	
	"DENZ",	
	"ANY",	
	"ANY_REAL",
};


// Utility Functions
static float round_ni( float f )
{
	return floorf( f );
}

static float round_pi( float f )
{
	return ceilf( f );
}

static float round_z( float f )
{
	return f > 0 ? floorf( f ) : ceilf( f );
}

static float round_ne( float f )
{
	UINT SignBit = 0x80000000 & (*(UINT32*)&f); 
	if( f >= 0 )
	{
		f += 0.5f;
	}
	else
	{
		f -= 0.5f;
	}
	float remainder = modff( f, &f );
	if( remainder == 0 )
	{
		float parity = fmodf( f, 2 );
		if( ((*(UINT32*)&parity)&(~SignBit)) == 0x3f800000 ) // -1 or +1
		{
			f -= parity;
		}
	}
	*(UINT*)&f &= 0x7fffffff; //reapply sign in case lost above.
	*(UINT*)&f |= SignBit; 
	return f;
}

bool IsDenorm( float f )
{
	DWORD dwF = *( (DWORD *) &f );
	
	return dwF && ( dwF & 0x7fffffff ) < 0x007fffff;
}



// FloatValues - Must be multiple of 4 in size
float FloatValues[] =
{
	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANn(),
	FloatMaker::SNANn(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
	-1.f,
	1.f,
	-0.1969f,
	65535.f,
	333.3333f,
	FloatMaker( (UINT32) 0xff7fffffUL ), // -FLOAT_MAX
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX
	FloatMaker( (UINT32) 0x7735319c ),   // 3.6750466e+033

	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANp(),
	FloatMaker::SNANp(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
	-1.f,
	1.f,
	-0.0969f,
	3.14159f,
	54321.f,
	FloatMaker( (UINT32) 0xff7fffffUL ), // -FLOAT_MAX
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX
	FloatMaker( (UINT32) 0x7735319c ),   // 3.6750466e+033
};

// FloatValues - Must be multiple of 4 in size
float FloatValuesFrc[] =
{
	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANn(),
	FloatMaker::SNANn(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
	FloatMaker( (UINT32) 0x80800000UL ), // -FLOAT_MIN
	-1.f,
	1.f,
	-0.1969f,
	65535.f,
	333.3333f,
	FloatMaker( (UINT32) 0xff7fffffUL ), // -FLOAT_MAX
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX

	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANp(),
	FloatMaker::SNANp(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
	FloatMaker( (UINT32) 0x80800000UL ), // -FLOAT_MIN
	-1.f,
	1.f,
	-0.0969f,
	3.14159f,
	54321.f,
};

// FloatValues - Must be multiple of 4 in size
float FloatValuesForDpn[] =
{
	FloatMaker::QNANn(),
	FloatMaker::SNANn(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	-1.f,
	1.f,
	-0.1969f,
	65535.f,
	333.3333f,
	FloatMaker::QNANp(),
	FloatMaker::SNANp(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	-1.f,
	1.f,
	-0.0969f,
	3.14159f,
	54321.f
};

// FloatValues - Must be multiple of 4 in size
float FloatValuesForDiv[] =
{
	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANn(),
	FloatMaker::SNANn(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
	-1.f,
	1.f,
	-0.1969f,
	(float) FloatMaker( (UINT32) 0X7f7fffffUL ) / 4, // +FLOAT_MAX / 4
	333.3333f,
	FloatMaker( (UINT32) 0xff7fffffUL ), // -FLOAT_MAX
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX

	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANp(),
	FloatMaker::SNANp(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
	-1.f,
	1.f,
	-0.0969f,
	(float) FloatMaker( (UINT32) 0X7f7fffffUL ) / 4, // +FLOAT_MAX / 4
	54321.f,
	FloatMaker( (UINT32) 0xff7fffffUL ), // -FLOAT_MAX
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX
};

float FloatValuesForRounding[] =
{
	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANn(),
	FloatMaker::SNANn(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
	-1.5f,
	1.5f,
	-2.5f,
	8.5f,
	1234567890.9f,
	0.99f,
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX
};

float FloatValuesForSinCos[] =
{
	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANn(),
	FloatMaker::SNANn(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
	-1.5f,
	1500.f,
	-100 * pi,
	100 * pi,
	1234567890.9f,
	0.99f,
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX

	FloatMaker::INFn(),
	FloatMaker::INFp(),
	FloatMaker::QNANp(),
	FloatMaker::SNANp(),
	FloatMaker( (UINT32) 0x80000000UL ), // -0
	FloatMaker( (UINT32) 0x00000000UL ), // +0
	FloatMaker( (UINT32) 0x80000fffUL ), // -Denorm
	FloatMaker( (UINT32) 0x00000fffUL ), // +Denorm
	FloatMaker( (UINT32) 0x00800000UL ), // +FLOAT_MIN
	-1.f,
	1.f,
	-99 * pi,
	99 * pi,
	54321.f,
	FloatMaker( (UINT32) 0xff7fffffUL ), // -FLOAT_MAX
	FloatMaker( (UINT32) 0x7f7fffffUL ), // +FLOAT_MAX
};

CShader5xTest_FloatArith::CShader5xTest_FloatArith() : CShader5xTest( DXGI_FORMAT_R32G32B32A32_UINT ),
	m_pRTBuffer2( NULL ),
	m_pRTBufferView2( NULL ),
	m_pRTUABufferView2( NULL ),
	m_nMaxErrorULP( 1 )
{
}

TEST_RESULT CShader5xTest_FloatArith::Setup()
{
	return CShader5xTest::Setup();
}

void CShader5xTest_FloatArith::Cleanup()
{
	SAFE_RELEASE( m_pRTBufferView2 );
	SAFE_RELEASE( m_pRTBuffer2 );
	
	CShader5xTest::Cleanup();
}

FLOAT_RESULT_CLASS CShader5xTest_FloatArith::FloatInputToFloatResult( FLOAT_INPUT_CLASS fic )
{
	switch( fic )
	{
		case FIC_INFn:	// -Infinity
			return FRC_INFn;
			
		case FIC_REALn:	// -Real
			return FRC_REALn;
			
		case FIC_ONEn:	// -1
			return FRC_ONEn;
			
		case FIC_DENn:	// -Denormal
		case FIC_ZEROn:	// -0
			return FRC_ZEROn;
			
		case FIC_ZEROp:	// +0
		case FIC_DENp:	// +Denormal
			return FRC_ZEROp;
			
		case FIC_ONEp:	// +1
			return FRC_ONEp;
			
		case FIC_REALp:	// +Real
			return FRC_REALp;
			
		case FIC_INFp:	// +Infinity
			return FRC_INFp;
			
		case FIC_NAN:		// Nan
			return FRC_NAN;
	}
	
	return FRC_ZERO;
}

FLOAT_INPUT_CLASS CShader5xTest_FloatArith::FloatResultToFloatInput( FLOAT_RESULT_CLASS frc, float fSrc0, float fSrc1, float fSrc2 )
{
	switch( frc )
	{
		case FRC_INFn:
			return FIC_INFn;
			break;
		case FRC_REALn:
			return FIC_REALn;
			break;
		case FRC_ZEROn:
			return FIC_ZEROn;
			break;
		case FRC_ZEROp:
			return FIC_ZEROp;
			break;
		case FRC_REALp:
			return FIC_REALp;
			break;
		case FRC_INFp:
			return FIC_INFp;
			break;
		case FRC_NAN:
			return FIC_NAN;
			break;
		case FRC_ONEp:
			return FIC_ONEp;
			break;
		case FRC_ONEn:
			return FIC_ONEn;
			break;
		case FRC_SRC0p:
			return GetInputClass( fSrc0 );
			break;
		case FRC_SRC0n:
		{
			DWORD dwSrc = 0x80000000 ^ *( (DWORD *) &fSrc0 );
			return GetInputClass( *( (float *) &dwSrc ) );
			break;
		}
		case FRC_SRC1p:
			return GetInputClass( fSrc1 );
			break;
		case FRC_SRC1n:
		{
			DWORD dwSrc = 0x80000000 ^ *( (DWORD *) &fSrc1 );
			return GetInputClass( *( (float *) &dwSrc ) );
			break;
		}
			break;
		case FRC_SRC2p:
			return GetInputClass( fSrc2 );
			break;
		case FRC_SRC2n:
		{
			DWORD dwSrc = 0x80000000 ^ *( (DWORD *) &fSrc2 );
			return GetInputClass( *( (float *) &dwSrc ) );
			break;
		}
	}
	
	return FIC_NONE;
}

FLOAT_RESULT_CLASS CShader5xTest_FloatArith::FloatResultClassNegate( FLOAT_RESULT_CLASS frc )
{
	switch( frc )
	{
		case FRC_INFn:	// -Infinity
			return FRC_INFp;
			
		case FRC_REALn:	// -Real
			return FRC_REALp;
			
		case FRC_ZEROn:	// -0
			return FRC_ZEROp;
			
		case FRC_ZEROp:	// +0
			return FRC_ZEROn;
			
		case FRC_REALp:	// +Real
			return FRC_REALn;
			
		case FRC_INFp:	// +Infinity
			return FRC_INFp;
			
		case FRC_SRC0p:	// Src0
			return FRC_SRC0n;
			
		case FRC_SRC0n:	// -Src0
			return FRC_SRC0p;
			
		case FRC_SRC1p:	// Src1
			return FRC_SRC1n;
			
		case FRC_SRC1n:	// -Src1
			return FRC_SRC1p;
			
		case FRC_SRC2p:	// Src2
			return FRC_SRC2n;
			
		case FRC_SRC2n:	// -Src2
			return FRC_SRC2p;
			
		case FRC_ONEp:	// +1.0
			return FRC_ONEn;
			
		case FRC_ONEn:	// -1.0
			return FRC_ONEp;
	}
	
	return frc;
}

FLOAT_INPUT_CLASS CShader5xTest_FloatArith::FloatInputClassNegate( FLOAT_INPUT_CLASS fic )
{
	switch( fic )
	{
		case FIC_INFn:	// -Infinity
			return FIC_INFp;
			
		case FIC_REALn:	// -Real
			return FIC_REALp;
			
		case FIC_ONEn:	// -1
			return FIC_ONEp;
			
		case FIC_DENn:	// -Denormal
			return FIC_DENp;
			
		case FIC_ZEROn:	// -0
			return FIC_ZEROp;
			
		case FIC_ZEROp:	// +0
			return FIC_ZEROn;
			
		case FIC_DENp:	// +Denormal
			return FIC_DENn;
			
		case FIC_ONEp:	// +1
			return FIC_ONEn;
			
		case FIC_REALp:	// +Real
			return FIC_REALn;
			
		case FIC_INFp:	// +Infinity
			return FIC_INFn;
		
		default:
			return fic;
	}
}


float Flush( float f )
{
	DWORD dw = *( ( DWORD * ) &f );
	if( ( dw & 0x7fffffff ) <= 0x007fffff )
		dw = dw & 0x80000000;		
	return *( (float *) &dw );
}

TEST_RESULT CShader5xTest_FloatArith::SetupTestCase()
{
	// Call the base SetupTestCase to determine if should skip
	TEST_RESULT tRes = CShader5xTest::SetupTestCase();
	
	if( RESULT_PASS != tRes )
		return tRes;

	//
	// Determine result classification (INF, NaN, or real).
	//
	float fSrc0[4];
	float fSrc1[4];
	float fSrc2[4];
	for( int i = 0; i < 4; ++i )
	{
		fSrc0[i] = ResolveSrc( m_Src[0][i], m_SrcMod[0] );
		fSrc1[i] = ResolveSrc( m_Src[1][i], m_SrcMod[1] );
		fSrc2[i] = ResolveSrc( m_Src[2][i], m_SrcMod[2] );
	}

	m_frc[0][0] = FRC_UNKNOWN;
	m_frc[0][1] = FRC_UNKNOWN;
	m_frc[0][2] = FRC_UNKNOWN;
	m_frc[0][3] = FRC_UNKNOWN;
	GetResultClass( m_frc[0], fSrc0, fSrc1, fSrc2 );
	CalcExpectedResult( m_ExpectedResult[0], fSrc0, fSrc1, fSrc2 );
	
	m_ExpectedResult[0][0] = Flush( m_ExpectedResult[0][0] );
	m_ExpectedResult[0][1] = Flush( m_ExpectedResult[0][1] );
	m_ExpectedResult[0][2] = Flush( m_ExpectedResult[0][2] );
	m_ExpectedResult[0][3] = Flush( m_ExpectedResult[0][3] );
	
	if( GetNumDestRegisters() == 2 )
	{
		m_frc[1][0] = FRC_UNKNOWN;
		m_frc[1][1] = FRC_UNKNOWN;
		m_frc[1][2] = FRC_UNKNOWN;
		m_frc[1][3] = FRC_UNKNOWN;
		GetResultClass2( m_frc[1], fSrc0, fSrc1, fSrc2 );
		CalcExpectedResult2( m_ExpectedResult[1], fSrc0, fSrc1, fSrc2 );
		m_ExpectedResult[1][0] = Flush( m_ExpectedResult[1][0] );
		m_ExpectedResult[1][1] = Flush( m_ExpectedResult[1][1] );
		m_ExpectedResult[1][2] = Flush( m_ExpectedResult[1][2] );
		m_ExpectedResult[1][3] = Flush( m_ExpectedResult[1][3] );
	}
	

	// Fixup expected results
	for( int n = 0; n < GetNumDestRegisters(); ++n )
	{
		for( int i = 0; i < 4; ++i )
		{
			switch( m_frc[n][i] )
			{
				case FRC_SRC0p:
					m_ExpectedResult[n][i] = fSrc0[i];
					break;
				case FRC_SRC0n:
					m_ExpectedResult[n][i] = -fSrc0[i];
					break;
				case FRC_SRC1p:
					m_ExpectedResult[n][i] = fSrc1[i];
					break;
				case FRC_SRC1n:
					m_ExpectedResult[n][i] = -fSrc1[i];
					break;
				case FRC_SRC2p:
					m_ExpectedResult[n][i] = fSrc2[i];
					break;
				case FRC_SRC2n:
					m_ExpectedResult[n][i] = -fSrc2[i];
					break;
			}
		}
		
		if( m_InstMod == IMOD_SAT )
		{
			for( int i = 0; i < 4; ++i )
			{
				switch( m_frc[n][i] )
				{
					case FRC_INFn:
					case FRC_REALn:
					case FRC_ZEROn:
					case FRC_ONEn:
					case FRC_DENZn:
						m_frc[n][i] = FRC_ZERO; // BUGBUG - Saturate of -0 is ambiguous (may be -0 or +0)
						break;
					case FRC_INFp:
						m_frc[n][i] = FRC_ONEp;
						break;
					case FRC_NAN:
						m_frc[n][i] = FRC_ZEROp;
						break;
					case FRC_REAL:
						if( m_ExpectedResult[n][i] < 0 )
						{
							m_ExpectedResult[n][i] = 0; // Change expected result to zero
							m_frc[n][i] = FRC_REALp; // Leave result class as real but make sure it is >= 0
						}
						else
						if( m_ExpectedResult[n][i] > 1 )
						{
							m_frc[n][i] = FRC_REALp;
							m_ExpectedResult[n][i] = 1;
						}
						break;
					case FRC_SRC0p:
						if( fSrc0[i] >= 1 )
							m_frc[n][i] = FRC_ONEp;
						else
						if( fSrc0[i] <= 0 )
							m_frc[n][i] = FRC_ZERO;
						break;
					case FRC_SRC0n:
						if( -fSrc0[i] <= 0 )
							m_frc[n][i] = FRC_ZERO;
						else
						if( -fSrc0[i] >= 1 )
							m_frc[n][i] = FRC_ONEp;
						break;
					case FRC_SRC1p:
						if( fSrc1[i] >= 1 )
							m_frc[n][i] = FRC_ONEp;
						else
						if( fSrc1[i] <= 0 )
							m_frc[n][i] = FRC_ZERO;
						break;
					case FRC_SRC1n:
						if( -fSrc1[i] <= 0 )
							m_frc[n][i] = FRC_ZERO;
						else
						if( -fSrc1[i] >= 1 )
							m_frc[n][i] = FRC_ONEp;
						break;
					case FRC_SRC2p:
						if( fSrc2[i] >= 1 )
							m_frc[n][i] = FRC_ONEp;
						else
						if( fSrc2[i] <= 0 )
							m_frc[n][i] = FRC_ZERO;
						break;
					case FRC_SRC2n:
						if( -fSrc2[i] <= 0 )
							m_frc[n][i] = FRC_ZERO;
						else
						if( -fSrc2[i] >= 1 )
							m_frc[n][i] = FRC_ONEp;
						break;
					case FRC_EXACT:
						if( m_ExpectedResult[n][i] <= 0 )
							m_frc[n][i] = FRC_ZERO;
						else
						if( m_ExpectedResult[n][i] >= 1 )
							m_frc[n][i] = FRC_ONEp;
						break;
				}
			}
		}
			
		for( int i = 0; i < 4; ++i )
			m_ExpectedResult[n][i] = ResolveDest( m_ExpectedResult[n][i], m_InstMod );
	}
		
	return BuildShaders();
}

TEST_RESULT CShader5xTest_FloatArith::BuildShaders()
{	
	//
	// Generate and compile the shader source
	//
	const char *szInstMod;
	switch( m_InstMod )
	{
		case IMOD_SAT:
			szInstMod = "_sat";
			break;
			
		default:
			szInstMod = "";
			break;
	}

	const char *szDest;
	
	if( GetNumDestRegisters() == 2 )
	{
		if( IsDestNull( 0 ) )
		{
			if( IsDestNull( 1 ) )
			{
				szDest = " NULL, NULL";
			}
			else
			{
				szDest = " NULL, r4";
			}
		}
		else
		{
			if( IsDestNull( 1 ) )
			{
				szDest = " r3, NULL";
			}
			else
			{
				szDest = " r3, r4";
			}
		}
	}
	else
	{
		szDest = " r3";
	}
	const char *szSrc[3];
	
	switch( m_SrcMod[0] )
	{
		case SMOD_NONE:
			szSrc[0] = ", r0";
			break;
		case SMOD_ABS:
			szSrc[0] = ", abs( r0 )";
			break;
		case SMOD_NEG:
			szSrc[0] = ", -r0";
			break;
		case SMOD_ABSNEG:
			szSrc[0] = ", -abs( r0 )";
			break;
	}
	
	if( GetNumSrcRegisters() > 1 )
	{
		switch( m_SrcMod[1] )
		{
			case SMOD_NONE:
				szSrc[1] = ", r1";
				break;
			case SMOD_ABS:
				szSrc[1] = ", abs( r1 )";
				break;
			case SMOD_NEG:
				szSrc[1] = ", -r1";
				break;
			case SMOD_ABSNEG:
				szSrc[1] = ", -abs( r1 )";
				break;
		}
	}
	else
	{
		szSrc[1] = "";
	}
	
	if( GetNumSrcRegisters() > 2 )
	{
		switch( m_SrcMod[2] )
		{
			case SMOD_NONE:
				szSrc[2] = ", r2";
				break;
			case SMOD_ABS:
				szSrc[2] = ", abs( r2 )";
				break;
			case SMOD_NEG:
				szSrc[2] = ", -r2";
				break;
			case SMOD_ABSNEG:
				szSrc[2] = ", -abs( r2 )";
				break;
		}
	}
	else
	{
		szSrc[2] = "";
	}
	

	// using AssemblyHelper to build the shader
	ID3D10Blob *pErrorBuf = NULL;
	ID3D10Blob *pShaderBuf = NULL;
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	UINT numRTs = 1;

	AssemblyHelper asmHelper(3, 200, m_RTFormat, m_viewPort);
	
	asmHelper.SetupMovToTempFromCB(0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromCB(2, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW);
	asmHelper.SetupMovToTempFromLiteral(3, AssemblyHelper::xyzw, "vec4( 0.0, 0.0, 0.0, 0.0 )");
	asmHelper.SetupMovToTempFromLiteral(4, AssemblyHelper::xyzw, "vec4( 0.0, 0.0, 0.0, 0.0 )");

	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.SetupMovToRawOutputFromTemp(0, AssemblyHelper::xyzw, 3, AssemblyHelper::swizXYZW);

	// You can only have one UAV (Compute shader) bound for FL10 and 10.1
	if( g_TestApp.IsFL11OrGreater() || D3D_SHADER_STAGE_COMPUTE != m_ShaderStage )
	{
		asmHelper.SetOutputRegMaskRaw(1, AssemblyHelper::xyzw);
		asmHelper.SetupMovToRawOutputFromTemp(1, AssemblyHelper::xyzw, 4, AssemblyHelper::swizXYZW);
		numRTs = 2;
	}
	
	tstring instructionCode = tstring( GetOpCode() ) + 
		tstring( szInstMod ) +
		tstring( szDest ) +
		tstring( szSrc[0] ) +
		tstring( szSrc[1] ) +
		tstring( szSrc[2] );

	ID3D11RenderTargetView *ppRTVs[] =
	{
		m_pRTBufferView,
		m_pRTBufferView2
	};

	ID3D11UnorderedAccessView *ppUAVs[] =
	{
		m_pRTUABufferView,
		m_pRTUABufferView2
	};

	hr = asmHelper.SetupPipelineWithShaderCode(GetDevice(), GetEffectiveContext(), this,
			m_ShaderStage, g_TestApp.m_D3DOptions, 
			instructionCode, ppRTVs, ppUAVs, numRTs, &pShaderBuf,
			&m_pVS, &m_pGS, &m_pPS, &m_pHS, &m_pDS, &m_pCS);

	if ( FAILED(hr) )
	{
		WriteError( _T( "SetupPipelineWithShaderCode() failed, hr = %s" ), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
		if( g_bDumpDissasm && pShaderBuf != NULL )
			MessageDumpShaderDisassembly( (UINT *) pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize() );
		goto setupDone;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
		goto setupDone;
	}

setupDone:
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );
	
	return tRes;
}

TEST_RESULT CShader5xTest_FloatArith::ExecuteTestCase()
{

	HRESULT hr;

	// Set the constant buffer data
	FloatShaderParams pData[1];
	
	pData[0].src0 = D3DXVECTOR4( m_Src[0][0], m_Src[0][1], m_Src[0][2], m_Src[0][3] );
	pData[0].src1 = D3DXVECTOR4( m_Src[1][0], m_Src[1][1], m_Src[1][2], m_Src[1][3] );
	pData[0].src2 = D3DXVECTOR4( m_Src[2][0], m_Src[2][1], m_Src[2][2], m_Src[2][3] );

	if( FAILED( hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, pData) ) ) 
	{
		WriteToLog( _T( "CShader5xTest_FloatArith::ExecuteTestCase() - failed to map m_pConstBuffer, hr = %s" ), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	FLOAT ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	UINT ClearColorU[4] = {0, 0, 0, 0};
		
	if ( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pRTUABufferView, ClearColorU);
		if ( m_pRTUABufferView2 )
			GetEffectiveContext()->ClearUnorderedAccessViewUint(m_pRTUABufferView2, ClearColorU);
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->ClearRenderTargetView( m_pRTBufferView, ClearColor );
		if ( m_pRTBufferView2 )
			GetEffectiveContext()->ClearRenderTargetView( m_pRTBufferView2, ClearColor );
		GetEffectiveContext()->Draw( 1, 0 );
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	ResultType texmap[1];
	D3D11_BOX box;
	box.left = 0;
	box.right = 1;
	box.top = 0;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;
	
	if( !IsDestNull( 0 ) )
	{
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, m_pRTBuffer );
		
		// Lock the render target and examine the results
		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, texmap) ) )
		{
			WriteToLog( _T( "CShader5xTest_FloatArith::ExecuteTestCase() - failed to map m_pRTBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
		
		if ( !CompareResults( 0, texmap[0] ))
		{
			WriteToLog(_T("CompareResults () failed for shader stage: %s, on 1st destination"),ToString(m_ShaderStage).c_str());
			return RESULT_FAIL;
		}
	}
		
	if( !IsDestNull( 1 ) && GetNumDestRegisters() == 2 )
	{
		// BUGBUG - Compute shader on FL10 will only allow 1 UAV to be bound. Thus we must skip here.
		// The alternative is to fix the test to use only 1 RT.
		if( !g_TestApp.IsFL11OrGreater() && D3D_SHADER_STAGE_COMPUTE == m_ShaderStage )
		{
			WriteToLog( _T("CShader5xTest_FloatArith::ExecuteTestCase() - Cannot have a second UAV for Feature Levels < 11. If you want to run this testcase, please use Feature Level 11.") );
			return RESULT_SKIP;
		}

		// Lock the render target and examine the results
		GetImmediateContext()->CopyResource( m_pRTBufferCopy, m_pRTBuffer2 );
		
		if( FAILED( hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, texmap) ) )
		{
			WriteToLog( _T( "CShader5xTest_FloatArith::ExecuteTestCase() - failed to map m_pRTBufferCopy, hr = %s" ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
		
		if ( !CompareResults( 1, texmap[0] ))
		{
			WriteToLog(_T("CompareResults () failed for shader stage: %s, on 2nd destination"),ToString(m_ShaderStage).c_str());
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}

bool CShader5xTest_FloatArith::IsReal( const float &f ) const
{
	return ( *( (DWORD *) &f ) & 0x7f800000 ) != 0x7f800000;
}

bool CShader5xTest_FloatArith::CompareINFn( const float &f, int i ) const
{
	return *( (DWORD *) &f ) == 0xff800000;
}

bool CShader5xTest_FloatArith::CompareINFp( const float &f, int i ) const
{
	return *( (DWORD *) &f ) == 0x7f800000;
}

bool CShader5xTest_FloatArith::CompareReal( const float &f, const float &fref, int i ) const
{
	// Default is to compare against 1 ULP of error.
	int nDiff = *( (DWORD *) &f ) - *( (DWORD *) &fref );
	nDiff = nDiff < 0 ? -nDiff : nDiff;
	return nDiff <= 1; 
}

bool CShader5xTest_FloatArith::CompareZEROn( const float &f, int i ) const
{
	return *( (DWORD *) &f ) == 0x80000000;
}

bool CShader5xTest_FloatArith::CompareZEROp( const float &f, int i ) const
{
	return *( (DWORD *) &f ) == 0x00000000;
}

bool CShader5xTest_FloatArith::CompareZERO( const float &f, int i ) const
{
	return ( *( (DWORD *) &f ) & 0x7fffffff ) == 0x00000000;
}

bool CShader5xTest_FloatArith::CompareONEn( const float &f, int i ) const
{
	return f == -1.0f;
}

bool CShader5xTest_FloatArith::CompareONEp( const float &f, int i ) const
{
	return f == 1.0f;
}

bool CShader5xTest_FloatArith::CompareNaN( const float &f, int i ) const
{
	DWORD dw = *( (DWORD *) &f ) & 0x7fffffff; // remove sign bit
	return dw >= 0x7f800001 && dw <= 0x7fffffff;
}

bool CShader5xTest_FloatArith::CompareExact( const float &f, const float &fref, int i ) const
{
	return *( (DWORD *) &f ) == *( (DWORD *) &fref );
}

bool CShader5xTest_FloatArith::CompareDenZp( const float &f, int i ) const
{
	return CompareZEROp( Flush( f ), i );
}

bool CShader5xTest_FloatArith::CompareDenZn( const float &f, int i ) const
{
	return CompareZEROn( Flush( f ), i );
}

bool CShader5xTest_FloatArith::CompareDenZ( const float &f, int i ) const
{
	return CompareZERO( Flush( f ), i );
}

bool CShader5xTest_FloatArith::CompareUnknown( const float &f, int i )
{
	return false; // Default to no match
}

bool CShader5xTest_FloatArith::CompareResults( int nResultIndex, ResultType &Result )
{
	const char szChannel[] = "xyzw";
	bool bRVal = true;

	for( int i = 0; i < 4; ++i )
	{
		bool bPass;
			
		switch( m_frc[nResultIndex][i] )
		{
			default:
				bPass = false;
				WriteToLog( _T( "Unknown result class" ) );
				break;
			case FRC_ANY:
				bPass = true;
				break;
			case FRC_ANY_REAL:
				bPass = IsReal( Result[i] );
				break;
			case FRC_INFn:
				bPass = CompareINFn( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected -INF" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
			case FRC_REALn:
			case FRC_REALp:
			case FRC_REAL:
				bPass = CompareReal( Result[i], m_ExpectedResult[nResultIndex][i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected %s" ), szChannel[i], ToString( Result[i] ).c_str(), ToString( m_ExpectedResult[nResultIndex][i] ).c_str() );
				break;
			case FRC_ZEROn:
				bPass = CompareZEROn( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected -0" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
			case FRC_ZEROp:
				bPass = CompareZEROp( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected +0" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
			case FRC_ZERO:
				bPass = CompareZERO( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected +/-0" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
			case FRC_ONEn:
				bPass = CompareONEn( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected -1" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
			case FRC_ONEp:
				bPass = CompareONEp( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected 1" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
			case FRC_INFp:
				bPass = CompareINFp( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected INF" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
			case FRC_NAN:
				bPass = CompareNaN( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected NaN" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
			case FRC_SRC0p:
			case FRC_SRC0n:
			case FRC_SRC1p:
			case FRC_SRC1n:
			case FRC_SRC2p:
			case FRC_SRC2n:
			case FRC_EXACT:
				bPass = CompareExact( Result[i], m_ExpectedResult[nResultIndex][i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected exactly %s" ), szChannel[i], ToString( Result[i] ).c_str(), ToString( m_ExpectedResult[nResultIndex][i] ).c_str() );
				break;
				
			case FRC_DENZp:
				bPass = CompareDenZp( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected +0 or +denorm" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
				
			case FRC_DENZn:
				bPass = CompareDenZn( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected -0 or -denorm" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
				
			case FRC_DENZ:
				bPass = CompareDenZ( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected +/-0 or +/-denorm" ), szChannel[i], ToString( Result[i] ).c_str() );
				break;
			case FRC_UNKNOWN:
				bPass = CompareUnknown( Result[i], i );
				if( !bPass )
					WriteToLog( _T( "Result.%c: %s, Expected %s" ), szChannel[i], ToString( Result[i] ).c_str(), resultClassNames[m_frc[nResultIndex][i]] );
				break;
		}
		
		bRVal = bRVal && bPass;
	}
		
	return bRVal;
}

void CShader5xTest_FloatArith::CleanupTestCase()
{
}


//////////////////////////////////////////////////////////////////////////
// ADD
//////////////////////////////////////////////////////////////////////////
FLOAT_RESULT_CLASS AddResultTable[NUM_FLOAT_INPUT_CLASSES][NUM_FLOAT_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF		-Real		-1			-denorm		-0			+0			+denorm		+1			+Real		+INF		NaN
/* -INF		*/	{	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_NAN,	FRC_NAN },
/* -Real	*/	{	FRC_INFn,	FRC_REAL,	FRC_REAL,	FRC_SRC0p,	FRC_SRC0p,	FRC_SRC0p,	FRC_SRC0p,	FRC_REAL,	FRC_REAL,	FRC_INFp,	FRC_NAN },
/* -1		*/	{	FRC_INFn,	FRC_REAL,	FRC_REAL,	FRC_SRC0p,	FRC_SRC0p,	FRC_SRC0p,	FRC_SRC0p,	FRC_REAL,	FRC_REAL,	FRC_INFp,	FRC_NAN },
/* -denorm	*/	{	FRC_INFn,	FRC_SRC1p,	FRC_SRC1p,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROp,	FRC_ZEROp,	FRC_SRC1p,	FRC_SRC1p,	FRC_INFp,	FRC_NAN },
/* -0		*/	{	FRC_INFn,	FRC_SRC1p,	FRC_SRC1p,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROp,	FRC_ZEROp,	FRC_SRC1p,	FRC_SRC1p,	FRC_INFp,	FRC_NAN },
/* +0		*/	{	FRC_INFn,	FRC_SRC1p,	FRC_SRC1p,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_SRC1p,	FRC_SRC1p,	FRC_INFp,	FRC_NAN },
/* +denorm	*/	{	FRC_INFn,	FRC_SRC1p,	FRC_SRC1p,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_SRC1p,	FRC_SRC1p,	FRC_INFp,	FRC_NAN },
/* +1		*/	{	FRC_INFn,	FRC_REAL,	FRC_REAL,	FRC_SRC0p,	FRC_SRC0p,	FRC_SRC0p,	FRC_SRC0p,	FRC_REAL,	FRC_REAL,	FRC_INFp,	FRC_NAN	},
/* +Real	*/	{	FRC_INFn,	FRC_REAL,	FRC_REAL,	FRC_SRC0p,	FRC_SRC0p,	FRC_SRC0p,	FRC_SRC0p,	FRC_REAL,	FRC_REAL,	FRC_INFp,	FRC_NAN	},
/* +INF		*/	{	FRC_NAN,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_NAN },
/* NaN		*/	{	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN }
};

void CShader5xTest_add::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValues ) / ( 2 * sizeof( FloatValues[0] ) );
	size_t stride = sizeof( FloatValues[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValues[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValues[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValues[3], stride, count ) );
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_Src[1][0], new CTableValueSet< float >( &FloatValues[0], stride * 4, count / 4 ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_Src[1][1], new CTableValueSet< float >( &FloatValues[1], stride * 4, count / 4 ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_Src[1][2], new CTableValueSet< float >( &FloatValues[2], stride * 4, count / 4 ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_Src[1][3], new CTableValueSet< float >( &FloatValues[3], stride * 4, count / 4 ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Use pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;
	testfactor::RFactor src1 = src1x % src1y % src1z % src1w;
	testfactor::RFactor srcs = src0 * src1;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{srcs, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

void CShader5xTest_add::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1, const float *fSrc2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS ficSrc0 = GetInputClass( fSrc0[i] );
		FLOAT_INPUT_CLASS ficSrc1 = GetInputClass( fSrc1[i] );

		frc[i] = AddResultTable[ficSrc0][ficSrc1];
	}
}

void CShader5xTest_add::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	for( int i = 0; i < 4; ++i )
		fResult[i] = src0[i] + src1[i];
}


//////////////////////////////////////////////////////////////////////////
// DIV
//////////////////////////////////////////////////////////////////////////
FLOAT_RESULT_CLASS DivResultTable[NUM_FLOAT_INPUT_CLASSES][NUM_FLOAT_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF		-Real		-1			-denorm		-0			+0			+denorm		+1			+Real		+INF		NaN
/* -INF		*/	{	FRC_NAN,	FRC_UNKNOWN,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_UNKNOWN,	FRC_NAN,	FRC_NAN },
/* -Real	*/	{	FRC_ZEROp,	FRC_REAL,	FRC_SRC0n,	FRC_INFp,	FRC_INFp,	FRC_INFn,	FRC_INFn,	FRC_SRC0p,	FRC_REAL,	FRC_ZEROn,	FRC_NAN },
/* -1		*/	{	FRC_ZEROp,	FRC_REAL,	FRC_SRC0n,	FRC_INFp,	FRC_INFp,	FRC_INFn,	FRC_INFn,	FRC_SRC0p,	FRC_REAL,	FRC_ZEROn,	FRC_NAN },
/* -denorm	*/	{	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_NAN },
/* -0		*/	{	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_NAN },
/* +0		*/	{	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_NAN },
/* +denorm	*/	{	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_NAN },
/* +1		*/	{	FRC_ZEROn,	FRC_REAL,	FRC_SRC0n,	FRC_INFn,	FRC_INFn,	FRC_INFp,	FRC_INFp,	FRC_SRC0p,	FRC_REAL,	FRC_ZEROp,	FRC_NAN },
/* +Real	*/	{	FRC_ZEROn,	FRC_REAL,	FRC_SRC0n,	FRC_INFn,	FRC_INFn,	FRC_INFp,	FRC_INFp,	FRC_SRC0p,	FRC_REAL,	FRC_ZEROp,	FRC_NAN },
/* +INF		*/	{	FRC_NAN,	FRC_UNKNOWN,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_UNKNOWN,	FRC_NAN,	FRC_NAN },
/* NaN		*/	{	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN }
};

void CShader5xTest_div::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValuesForDiv ) / ( 2 * sizeof( FloatValuesForDiv[0] ) );
	size_t stride = sizeof( FloatValuesForDiv[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValuesForDiv[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValuesForDiv[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValuesForDiv[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValuesForDiv[3], stride, count ) );
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_Src[1][0], new CTableValueSet< float >( &FloatValuesForDiv[0], stride * 4, count / 4 ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_Src[1][1], new CTableValueSet< float >( &FloatValuesForDiv[1], stride * 4, count / 4 ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_Src[1][2], new CTableValueSet< float >( &FloatValuesForDiv[2], stride * 4, count / 4 ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_Src[1][3], new CTableValueSet< float >( &FloatValuesForDiv[3], stride * 4, count / 4 ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Use pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;
	testfactor::RFactor src1 = src1x % src1y % src1z % src1w;
	testfactor::RFactor srcs = src0 * src1;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{srcs, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

void CShader5xTest_div::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1, const float *fSrc2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS ficSrc0 = GetInputClass( fSrc0[i] );
		FLOAT_INPUT_CLASS ficSrc1 = GetInputClass( fSrc1[i] );

		frc[i] = DivResultTable[ficSrc0][ficSrc1];
	}
}

void CShader5xTest_div::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		if( src1[i] == src0[i] )
			OutputDebugString( FormatString( "x/x case %i: x == %g\n", GetFramework()->GetCurrentTestCase(), src0[i] ) );
			
		if( src1[i] )
		{
			fResult[i] = src0[i] / src1[i]; 
		}
		else
		{
			fResult[i] = FloatMaker::QNANn();
		}
	}
}

bool CShader5xTest_div::CompareUnknown( const float &f, int i )
{
	float result;
	float src0 = ResolveSrc( m_Src[0][i], m_SrcMod[0] );
	float src1 = ResolveSrc( m_Src[1][i], m_SrcMod[1] );
	
	// Alternative: flush( flush( a ) * flush( 1 / flush( b ) ) )
	float recip = 1 / Flush( src1 );
	result = ResolveDest( Flush( Flush( src0 ) * Flush( recip ) ), m_InstMod );
	if( CompareNaN( result, i ) && CompareNaN( f, i ) )
		return true;
	if( CompareINFn( result, i ) && CompareINFn( f, i ) )
		return true;
	if( CompareINFp( result, i ) && CompareINFp( f, i ) )
		return true;
	if( CompareZEROn( result, i ) && CompareZEROn( f, i ) )
		return true;
	if( CompareZEROp( result, i ) && CompareZEROp( f, i ) )
		return true;
	
	// Alternative: flush( a * ( 1 / b ) )
	result = ResolveDest( Flush( src0 * 1 / src1 ), m_InstMod );
	if( CompareNaN( result, i ) && CompareNaN( f, i ) )
		return true;
	if( CompareINFn( result, i ) && CompareINFn( f, i ) )
		return true;
	if( CompareINFp( result, i ) && CompareINFp( f, i ) )
		return true;
	if( CompareZEROn( result, i ) && CompareZEROn( f, i ) )
		return true;
	if( CompareZEROp( result, i ) && CompareZEROp( f, i ) )
		return true;
	
	// Alternative: flush( a / b )
	result = ResolveDest( Flush( src0 / src1 ), m_InstMod );
	if( CompareNaN( result, i ) && CompareNaN( f, i ) )
		return true;
	if( CompareINFn( result, i ) && CompareINFn( f, i ) )
		return true;
	if( CompareINFp( result, i ) && CompareINFp( f, i ) )
		return true;
	if( CompareZEROn( result, i ) && CompareZEROn( f, i ) )
		return true;
	if( CompareZEROp( result, i ) && CompareZEROp( f, i ) )
		return true;
	
	// Default Alternative: flush( flush( a ) / flush( b ) )
	result = ResolveDest( Flush( Flush( src0 ) / Flush( src1 ) ), m_InstMod );
	if( CompareNaN( result, i ) && CompareNaN( f, i ) )
		return true;
	if( CompareINFn( result, i ) && CompareINFn( f, i ) )
		return true;
	if( CompareINFp( result, i ) && CompareINFp( f, i ) )
		return true;
	if( CompareZEROn( result, i ) && CompareZEROn( f, i ) )
		return true;
	if( CompareZEROp( result, i ) && CompareZEROp( f, i ) )
		return true;
	
	if( CompareReal( result, f, i ) )
		return true;
	
	// Pick an expected result and return failure
	if( CompareNaN( result, i ) )
		m_frc[0][i] = FRC_NAN;
	else
	if( CompareINFn( result, i ) )
		m_frc[0][i] = FRC_INFn;
	else
	if( CompareINFp( result, i ) )
		m_frc[0][i] = FRC_INFp;
	else
	if( CompareZEROn( result, i ) )
		m_frc[0][i] = FRC_ZEROn;
	else
	if( CompareZEROp( result, i ) )
		m_frc[0][i] = FRC_ZEROp;
		
	m_frc[0][i] = FRC_REAL;
		
	return false;
}

bool CShader5xTest_div::CompareReal( const float &f, const float &fref, int i ) const
{
	// Although n/denorm may result in a non-denorm value, it is possible
	// that implementations which use src0 * 1/src1 may get a denorm result
	// from 1/src1 resulting in src0 * flush( 1/src1) or 0.
	if( IsDenorm( 1 / ResolveSrc( m_Src[1][i], m_SrcMod[1] ) ) )
	{
		if( f == 0  || f == -0 )
			return true;
	}

	bool result = CShader5xTest_FloatArith::CompareReal( f, fref, i );
	
	float src0 = ResolveSrc( m_Src[0][i], m_SrcMod[0] );
	float src1 = ResolveSrc( m_Src[1][i], m_SrcMod[1] );
	
	// Alternative results...
	if( result == false )
	{
		// We will allow src0 * 1 / src1
		float a = 1 / src1;
		float b = src0;
		float c = ResolveDest( Flush( a * b ), m_InstMod );
		
		// Allow 2-ULP of error since this is effectively now a fused operation
		int nDiff = *( (DWORD *) &f ) - *( (DWORD *) &c );
		nDiff = nDiff < 0 ? -nDiff : nDiff;
		result = nDiff <= 2; 
	}
	
	if( result == false )
	{
		// We will allow flush( src0 ) * flush( 1 / flush( src1 ) )
		float a = Flush( 1 / Flush( src1 ) );
		float b = Flush( src0 );
		float c = ResolveDest( Flush( a * b ), m_InstMod );
		
		// Allow 2-ULP of error since this is effectively now a fused operation
		int nDiff = *( (DWORD *) &f ) - *( (DWORD *) &c );
		nDiff = nDiff < 0 ? -nDiff : nDiff;
		result = nDiff <= 2; 
	}
	
	return result;
}


//////////////////////////////////////////////////////////////////////////
// DPn
//////////////////////////////////////////////////////////////////////////

// Forward declaration of MulResultTable
extern FLOAT_RESULT_CLASS MulResultTable[NUM_FLOAT_INPUT_CLASSES][NUM_FLOAT_INPUT_CLASSES];

void CShader5xTest_dpn::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1, const float *fSrc2 ) const
{
	FLOAT_INPUT_CLASS ficSrc[2][4] = {FIC_NONE};
	FLOAT_INPUT_CLASS fic[4] = {FIC_NONE};
	FLOAT_RESULT_CLASS frcMul[4] = {FRC_ZERO};
	int i;
	
	for( i = 0; i < m_nVectorSize; ++i )
	{
		ficSrc[0][i] = GetInputClass( fSrc0[i] );
		ficSrc[1][i] = GetInputClass( fSrc1[i] );
		frcMul[i] = MulResultTable[ficSrc[0][i]][ficSrc[1][i]];
		
		// Since denorm input classes may or may not be calculated at higher precision
		// the exact result may vary.
		if( ficSrc[0][i] == FIC_DENp || ficSrc[0][i] == FIC_DENn ||
			ficSrc[1][i] == FIC_DENp || ficSrc[1][i] == FIC_DENn )
		{
			frc[0] = frc[1] = frc[2] = frc[3] = FRC_ANY;
			return;
		}
		
		// Convert FLOAT_RESULT_CLASS to FLOAT_INPUT_CLASS
		switch( frcMul[i] )
		{
			case FRC_SRC0p:
				fic[i] = ficSrc[0][i];
				break;
			case FRC_SRC1p:
				fic[i] = ficSrc[1][i];
				break;
			case FRC_SRC0n:
				fic[i] = FloatInputClassNegate( ficSrc[0][i] );
				break;
			case FRC_SRC1n:
				fic[i] = FloatInputClassNegate( ficSrc[1][i] );
				break;
			case FRC_REAL:
			case FRC_REALp:
			case FRC_REALn:
			{
				// Check for overflow
				float f = fSrc0[i] * fSrc1[i];
				if( !_finite( f ) )
				{
					frc[0] = frc[1] = frc[2] = frc[3] = FRC_ANY;
					return;
				}
				
				// Check for "underflow"
				if( fabsf( f ) > 0 && fabsf( f ) < (float) FloatMaker( (UINT32) 0x00800000UL ) )
				{
					frc[0] = frc[1] = frc[2] = frc[3] = FRC_ANY;
					return;
				}

				switch( frcMul[i] )
				{				
					default:
					case FRC_REALp:
						fic[i] = FIC_REALp;
						break;
					case FRC_REALn:
						fic[i] = FIC_REALn;
						break;
				}
				break;
			}
			default:
				fic[i] = FloatResultToFloatInput( frcMul[i], fSrc0[i], fSrc1[i], 0 );
				break;
		}
	}
	
	FLOAT_RESULT_CLASS frcAdd = frcMul[0];
	FLOAT_INPUT_CLASS ficAdd = fic[0];
	bool bLostSign = false;
	for( int i = 1; i < m_nVectorSize; ++i )
	{
		int n = 0;
		
		frcAdd = AddResultTable[ficAdd][fic[i]];
		
		switch( frcAdd )
		{
			case FRC_SRC0p:
				n = i - 1;
				frcAdd = frcMul[n];
				break;
			case FRC_SRC1p:
				n = i;
				frcAdd = frcMul[n];
				break;
			case FRC_SRC0n:
				n = i - 1;
				frcAdd = FloatResultClassNegate( frcMul[n] );
				break;
			case FRC_SRC1n:
				n = i;
				frcAdd = FloatResultClassNegate( frcMul[n] );
				break;
		}
		
		switch( frcAdd )
		{
			case FRC_SRC0p:
				frcAdd = FloatInputToFloatResult( ficSrc[0][n] );
				break;
			case FRC_SRC0n:
				frcAdd = FloatResultClassNegate( FloatInputToFloatResult( ficSrc[0][n] ) );
				break;
			case FRC_SRC1p:
				frcAdd = FloatInputToFloatResult( ficSrc[1][n] );
				break;
			case FRC_SRC1n:
				frcAdd = FloatResultClassNegate( FloatInputToFloatResult( ficSrc[1][n] ) );
				break;
		}
		
		if( frcAdd == FRC_REAL )
		{
			frcAdd = FRC_REALp;
			bLostSign = true;
		}
		
		if( i == m_nVectorSize - 1 )
			break;
			
		// Convert FLOAT_RESULT_CLASS to FLOAT_INPUT_CLASS
		ficAdd = FloatResultToFloatInput( frcAdd, fSrc0[i], fSrc1[i], 0 );
	}
	
	if( bLostSign && ( frcAdd == FRC_REALp || frcAdd == FRC_REALn ) )
		frcAdd = FRC_REAL;
	
	frc[0] = frc[1] = frc[2] = frc[3] = frcAdd;
}



//////////////////////////////////////////////////////////////////////////
// DP2
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_dp2::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValuesForDpn ) / ( 2 * sizeof( FloatValuesForDpn[0] ) );
	size_t stride = sizeof( FloatValuesForDpn[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValuesForDpn[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValuesForDpn[4], stride, count ) );
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_Src[1][0], new CTableValueSet< float >( &FloatValuesForDpn[3], stride, count ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_Src[1][1], new CTableValueSet< float >( &FloatValuesForDpn[8], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::XFACTOR modsPairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Create the pairwise factor
	testfactor::RFactor rModsPairwiseRoot = NewCrossFactor(modsPairwiseTable, _countof(modsPairwiseTable), "PairwiseRoot", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCases = ( src0x % src0y ) * ( src1x % src1y ) * rModsPairwiseRoot;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	m_Src[0][2] = FloatMaker::SNANn();
	m_Src[0][3] = FloatMaker::SNANn();
	m_Src[1][2] = FloatMaker::SNANn();
	m_Src[1][3] = FloatMaker::SNANn();

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

bool CShader5xTest_dp2::CompareReal( const float &f, const float &fref, int i ) const
{
	// 1-ULP for each of the 3 intermediate operations.
	int nDiff = *( (DWORD *) &f ) - *( (DWORD *) &fref );
	nDiff = nDiff < 0 ? -nDiff : nDiff;
	return nDiff <= 3; 
}

void CShader5xTest_dp2::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	fResult[0] = fResult[1] = fResult[2] = fResult[3] = src0[0] * src1[0] + src0[1] * src1[1];
}


//////////////////////////////////////////////////////////////////////////
// DP3
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_dp3::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValuesForDpn ) / ( 2 * sizeof( FloatValuesForDpn[0] ) );
	size_t stride = sizeof( FloatValuesForDpn[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValuesForDpn[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValuesForDpn[4], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValuesForDpn[1], stride, count ) );
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_Src[1][0], new CTableValueSet< float >( &FloatValuesForDpn[3], stride, count ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_Src[1][1], new CTableValueSet< float >( &FloatValuesForDpn[8], stride, count ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_Src[1][2], new CTableValueSet< float >( &FloatValuesForDpn[5], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::XFACTOR modsPairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Create the pairwise factor
	testfactor::RFactor rModsPairwiseRoot = NewCrossFactor(modsPairwiseTable, _countof(modsPairwiseTable), "PairwiseRoot", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCases = ( src0x % src0y % src0z ) * ( src1x % src1y % src1z ) * rModsPairwiseRoot;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	m_Src[0][3] = FloatMaker::SNANn();
	m_Src[1][3] = FloatMaker::SNANn();

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}


bool CShader5xTest_dp3::CompareReal( const float &f, const float &fref, int i ) const
{
	// 1-ULP for each of the 5 intermediate operations.
	int nDiff = *( (DWORD *) &f ) - *( (DWORD *) &fref );
	nDiff = nDiff < 0 ? -nDiff : nDiff;
	return nDiff <= 5; 
}

void CShader5xTest_dp3::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	fResult[0] = fResult[1] = fResult[2] = fResult[3] = src0[0] * src1[0] + src0[1] * src1[1] + src0[2] * src1[2];
}


//////////////////////////////////////////////////////////////////////////
// DP4
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_dp4::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValuesForDpn ) / ( 2 * sizeof( FloatValuesForDpn[0] ) );
	size_t stride = sizeof( FloatValuesForDpn[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValuesForDpn[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValuesForDpn[4], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValuesForDpn[1], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValuesForDpn[2], stride, count ) );
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_Src[1][0], new CTableValueSet< float >( &FloatValuesForDpn[3], stride, count ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_Src[1][1], new CTableValueSet< float >( &FloatValuesForDpn[8], stride, count ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_Src[1][2], new CTableValueSet< float >( &FloatValuesForDpn[5], stride, count ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_Src[1][3], new CTableValueSet< float >( &FloatValuesForDpn[6], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::XFACTOR modsPairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Create the pairwise factor
	testfactor::RFactor rModsPairwiseRoot = NewCrossFactor(modsPairwiseTable, _countof(modsPairwiseTable), "PairwiseRoot", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCases = ( src0x % src0y % src0z % src0w ) * ( src1x % src1y % src1z % src1w ) * rModsPairwiseRoot;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	m_Src[0][3] = FloatMaker::SNANn();
	m_Src[1][3] = FloatMaker::SNANn();

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}


bool CShader5xTest_dp4::CompareReal( const float &f, const float &fref, int i ) const
{
	// 1-ULP for each of the 7 intermediate operations.
	int nDiff = *( (DWORD *) &f ) - *( (DWORD *) &fref );
	nDiff = nDiff < 0 ? -nDiff : nDiff;
	return nDiff <= 7; 
}

void CShader5xTest_dp4::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	fResult[0] = fResult[1] = fResult[2] = fResult[3] = src0[0] * src1[0] + src0[1] * src1[1] + src0[2] * src1[2] + src0[3] * src1[3];
}


//////////////////////////////////////////////////////////////////////////
// EXP
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_exp::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValues ) / ( 2 * sizeof( FloatValues[0] ) );
	size_t stride = sizeof( FloatValues[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValues[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValues[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValues[3], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Use pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

bool CShader5xTest_exp::CompareReal( const float &f, const float &fref, int i ) const
{
	// Compute the relative error (tolerance is 2^-21).
	int nDiff = *( (DWORD *) &f ) - *( (DWORD *) &fref );
	nDiff = nDiff < 0 ? -nDiff : nDiff;
	return nDiff <= 4; 
}

void CShader5xTest_exp::CalcExpectedResult( float fResult[4], const float *src0, const float *src1 /* = NULL */, const float *src2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = powf( 2.0f, src0[i] );
	}
}

const FLOAT_RESULT_CLASS ExpResultTable[] =
	{
		FRC_ZEROp,	// -Infinity
		FRC_REALp,	// -Real
		FRC_REALp,	// -1
		FRC_ONEp,	// -Denormal
		FRC_ONEp,	// -0
		FRC_ONEp,	// +0
		FRC_ONEp,	// +Denormal
		FRC_REALp,	// +1
		FRC_REALp,	// +Real
		FRC_INFp,	// +Infinity
		FRC_NAN,		// Nan
	};
	
void CShader5xTest_exp::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 /* = NULL */, const float *fSrc2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS fic = GetInputClass( fSrc0[i] );
		
		frc[i] = ExpResultTable[fic];
	}
}




//////////////////////////////////////////////////////////////////////////
// FRC
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_frc::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	
	size_t count = sizeof( FloatValuesFrc ) / ( 2 * sizeof( FloatValuesFrc[0] ) );
	size_t stride = sizeof( FloatValuesFrc[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValuesFrc[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValuesFrc[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValuesFrc[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValuesFrc[3], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Use pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

void CShader5xTest_frc::CalcExpectedResult( float fResult[4], const float *src0, const float *src1 /* = NULL */, const float *src2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = src0[i] - round_ni( src0[i] );
		if( fResult[i] == 1 )
		{
			// fix for small negative values mapping to 1.0f
			// Select the largest representable floating point value
			// less than 1.0
			DWORD dw = *( (DWORD *) &fResult[i] ) - 1;
			fResult[i] = *( (float *) &dw );
		}
	}
}

const FLOAT_RESULT_CLASS FrcResultTable[] =
	{
		FRC_NAN,	// -Infinity
		FRC_REALp,	// -Real
		FRC_REALp,	// -1
		FRC_ZEROp,	// -Denormal
		FRC_ZEROp,	// -0
		FRC_ZEROp,	// +0
		FRC_ZEROp,	// +Denormal
		FRC_REALp,	// +1
		FRC_REALp,	// +Real
		FRC_NAN,	// +Infinity
		FRC_NAN,		// Nan
	};
	
void CShader5xTest_frc::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 /* = NULL */, const float *fSrc2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS fic = GetInputClass( fSrc0[i] );
		
		frc[i] = FrcResultTable[fic];
	}
}


//////////////////////////////////////////////////////////////////////////
// LOG
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_log::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValues ) / ( 2 * sizeof( FloatValues[0] ) );
	size_t stride = sizeof( FloatValues[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValues[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValues[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValues[3], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Use pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

bool CShader5xTest_log::CompareReal( const float &f, const float &fref, int i ) const
{
	// Compute the absolute error (tolerance is 2^-21).
	float fDiff = fref - f;
	if( fDiff < (float) FloatMaker( -21, 0 ) )
		return true;
		
	// Compute the relative error (tolerance is 2^-21).
	int nDiff = *( (DWORD *) &f ) - *( (DWORD *) &fref );
	nDiff = nDiff < 0 ? -nDiff : nDiff;
	return nDiff <= 4; 
}

void CShader5xTest_log::CalcExpectedResult( float fResult[4], const float *src0, const float *src1 /* = NULL */, const float *src2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		static const double ooLn2 = 1.4426950408889634073599246810019;
		fResult[i] =  (float) ( log( src0[i] ) * ooLn2 );
	}
}

const FLOAT_RESULT_CLASS LogResultTable[] =
	{
		FRC_NAN,	// -Infinity
		FRC_NAN,	// -Real
		FRC_NAN,	// -1
		FRC_INFn,	// -Denormal
		FRC_INFn,	// -0
		FRC_INFn,	// +0
		FRC_INFn,	// +Denormal
		FRC_REALp,	// +1
		FRC_REALp,	// +Real
		FRC_INFp,	// +Infinity
		FRC_NAN,		// Nan
	};
	
void CShader5xTest_log::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 /* = NULL */, const float *fSrc2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS fic = GetInputClass( fSrc0[i] );
		
		frc[i] = LogResultTable[fic];
	}
}

//////////////////////////////////////////////////////////////////////////
// MAD
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_mad::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValues ) / ( 2 * sizeof( FloatValues[0] ) );
	size_t stride = sizeof( FloatValues[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValues[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValues[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValues[3], stride, count ) );
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_Src[1][0], new CTableValueSet< float >( &FloatValues[0], stride * 4, count / 4 ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_Src[1][1], new CTableValueSet< float >( &FloatValues[1], stride * 4, count / 4 ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_Src[1][2], new CTableValueSet< float >( &FloatValues[2], stride * 4, count / 4 ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_Src[1][3], new CTableValueSet< float >( &FloatValues[3], stride * 4, count / 4 ) );
	testfactor::RFactor src2x = AddParameter( "Src2.x", &m_Src[2][0], new CTableValueSet< float >( &FloatValues[8], stride, count ) );
	testfactor::RFactor src2y = AddParameter( "Src2.y", &m_Src[2][1], new CTableValueSet< float >( &FloatValues[9], stride, count ) );
	testfactor::RFactor src2z = AddParameter( "Src2.z", &m_Src[2][2], new CTableValueSet< float >( &FloatValues[10], stride, count ) );
	testfactor::RFactor src2w = AddParameter( "Src2.w", &m_Src[2][3], new CTableValueSet< float >( &FloatValues[11], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );
	testfactor::RFactor src2mod = AddParameter( "Src2Mod", &m_SrcMod[2], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::XFACTOR modsPairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
		{src2mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rModsPairwiseRoot = NewCrossFactor(modsPairwiseTable, _countof(modsPairwiseTable), "PairwiseRoot", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCases = ( ( ( src0x % src0y % src0z % src0w ) * ( src1x % src1y % src1z % src1w ) ) % ( src2x % src2y % src2z % src2w ) ) * rModsPairwiseRoot;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src2Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src2Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 2.0f );
}

void CShader5xTest_mad::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1, const float *fSrc2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS ficSrc[3] = {FIC_NONE};
		FLOAT_INPUT_CLASS fic = {FIC_NONE};
		FLOAT_RESULT_CLASS frcMul = {FRC_ZERO};

		ficSrc[0] = GetInputClass( fSrc0[i] );
		ficSrc[1] = GetInputClass( fSrc1[i] );
		ficSrc[2] = GetInputClass( fSrc2[i] );
		
		// Since denorm input classes may or may not be calculated at higher precision
		// the exact result may vary.
		if( ( ficSrc[0] == FIC_DENp || ficSrc[0] == FIC_DENn ) ||
			( ficSrc[1] == FIC_DENp || ficSrc[1] == FIC_DENn ) ||
			( ficSrc[2] == FIC_DENp || ficSrc[2] == FIC_DENn ) )
			frc[i] = FRC_ANY;
		
		frcMul = MulResultTable[ficSrc[0]][ficSrc[1]];
		
		// Convert FLOAT_RESULT_CLASS to FLOAT_INPUT_CLASS
		switch( frcMul )
		{
			case FRC_SRC0p:
				fic = ficSrc[0];
				break;
			case FRC_SRC1p:
				fic = ficSrc[1];
				break;
			case FRC_SRC0n:
				fic = FloatInputClassNegate( ficSrc[0] );
				break;
			case FRC_SRC1n:
				fic = FloatInputClassNegate( ficSrc[1] );
				break;
			case FRC_REAL:
			case FRC_REALp:
			case FRC_REALn:
			{
				// Check for overflow
				float f = fSrc0[i] * fSrc1[i];
				if( !CFloatUtils<float>::IsReal(f) )
				{
					frc[i] = FRC_ANY;
					break;
				}
				
				// Check for underflow
				if( fabsf( f ) > 0 && fabsf( f ) < (float) FloatMaker( (UINT32) 0x00800000UL ) )
				{
					frc[i] = FRC_ANY;
					break;
				}

				switch( frcMul )
				{				
					default:
					case FRC_REALp:
						fic = FIC_REALp;
						break;
					case FRC_REALn:
						fic = FIC_REALn;
						break;
				}
				break;
			}
			default:
				fic = FloatResultToFloatInput( frcMul, fSrc0[i], fSrc1[i], fSrc2[i] );
				break;
		}
		
		if( frc[i] == FRC_ANY )
			continue;
			
		frc[i] = AddResultTable[fic][ficSrc[2]];

		switch( frc[i] )
		{
			case FRC_SRC0p:
				frc[i] = frcMul;
				break;
			case FRC_SRC1p:
				frc[i] = FRC_SRC2p;
				break;
			case FRC_SRC0n:
				frc[i] = FloatResultClassNegate( frcMul );
				break;
			case FRC_SRC1n:
				frc[i] = FRC_SRC2n;
				break;
		}
	}
}

bool CShader5xTest_mad::CompareReal( const float &f, const float &fref, int i ) const
{
	// 2 ULP tolerance (1 ULP for each intermediate operation).
	int nDiff = *( (DWORD *) &f ) - *( (DWORD *) &fref );
	nDiff = nDiff < 0 ? -nDiff : nDiff;
	return nDiff <= 2; 
}

void CShader5xTest_mad::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		const float mulresult = src0[i] * src1[i];
		fResult[i] = mulresult + src2[i];
	}
}


//////////////////////////////////////////////////////////////////////////
// MAX
//////////////////////////////////////////////////////////////////////////
FLOAT_RESULT_CLASS MaxResultTable[NUM_FLOAT_INPUT_CLASSES][NUM_FLOAT_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF		-Real		-1			-denorm		-0			+0			+denorm		+1			+Real		+INF		NaN
/* -INF		*/	{	FRC_INFn,	FRC_SRC1p,	FRC_SRC1p,	FRC_SRC1p,	FRC_SRC1p,	FRC_SRC1p,	FRC_SRC1p,	FRC_SRC1p,	FRC_SRC1p,	FRC_INFp,	FRC_INFn },
/* -Real	*/	{	FRC_SRC0p,	FRC_EXACT,	FRC_EXACT,	FRC_DENZn,	FRC_EXACT,	FRC_EXACT,	FRC_DENZp,	FRC_EXACT,	FRC_EXACT,	FRC_INFp,	FRC_SRC0p },
/* -1		*/	{	FRC_SRC0p,	FRC_EXACT,	FRC_EXACT,	FRC_DENZn,	FRC_EXACT,	FRC_EXACT,	FRC_DENZp,	FRC_EXACT,	FRC_EXACT,	FRC_INFp,	FRC_SRC0p },
/* -denorm	*/	{	FRC_DENZn,	FRC_DENZn,	FRC_DENZn,	FRC_DENZn,	FRC_DENZn,	FRC_DENZ,	FRC_DENZ,	FRC_EXACT,	FRC_EXACT,	FRC_INFp,	FRC_DENZn },
/* -0		*/	{	FRC_SRC0p,	FRC_EXACT,	FRC_EXACT,	FRC_DENZn,	FRC_ZEROn,	FRC_ZERO,	FRC_DENZ,	FRC_EXACT,	FRC_EXACT,	FRC_INFp,	FRC_SRC0p },
/* +0		*/	{	FRC_SRC0p,	FRC_EXACT,	FRC_EXACT,	FRC_DENZ,	FRC_ZERO,	FRC_ZEROp,	FRC_DENZp,	FRC_EXACT,	FRC_EXACT,	FRC_INFp,	FRC_SRC0p },
/* +denorm	*/	{	FRC_DENZp,	FRC_DENZp,	FRC_DENZp,	FRC_DENZ,	FRC_DENZ,	FRC_DENZp,	FRC_DENZp,	FRC_EXACT,	FRC_EXACT,	FRC_INFp,	FRC_DENZp },
/* +1		*/	{	FRC_SRC0p,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_INFp,	FRC_SRC0p },
/* +Real	*/	{	FRC_SRC0p,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_INFp,	FRC_SRC0p },
/* +INF		*/	{	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_INFp },
/* NaN		*/	{	FRC_INFn,	FRC_SRC1p,	FRC_SRC1p,	FRC_DENZn,	FRC_SRC1p,	FRC_SRC1p,	FRC_DENZp,	FRC_SRC1p,	FRC_SRC1p,	FRC_INFp,	FRC_NAN },
};


void CShader5xTest_max::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValues ) / ( 2 * sizeof( FloatValues[0] ) );
	size_t stride = sizeof( FloatValues[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValues[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValues[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValues[3], stride, count ) );
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_Src[1][0], new CTableValueSet< float >( &FloatValues[0], stride * 4, count / 4 ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_Src[1][1], new CTableValueSet< float >( &FloatValues[1], stride * 4, count / 4 ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_Src[1][2], new CTableValueSet< float >( &FloatValues[2], stride * 4, count / 4 ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_Src[1][3], new CTableValueSet< float >( &FloatValues[3], stride * 4, count / 4 ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );
	
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::XFACTOR modsPairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rModsPairwiseRoot = NewCrossFactor(modsPairwiseTable, _countof(modsPairwiseTable), "PairwiseRoot", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCases = ( src0x % src0y % src0z % src0w ) * ( src1x % src1y % src1z % src1w ) * rModsPairwiseRoot;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );
	
	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

void CShader5xTest_max::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1, const float *fSrc2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS ficSrc0 = GetInputClass( fSrc0[i] );
		FLOAT_INPUT_CLASS ficSrc1 = GetInputClass( fSrc1[i] );

		frc[i] = MaxResultTable[ficSrc0][ficSrc1];
	}
}

void CShader5xTest_max::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = Flush( src0[i] ) >= Flush( src1[i] ) ? src0[i] : src1[i];
	}
}

bool CShader5xTest_max::CompareExact( const float &f, const float &fref, int i ) const
{
	return CShader5xTest_FloatArith::CompareExact( Flush( f ), Flush( fref ), i );
}



//////////////////////////////////////////////////////////////////////////
// MIN
//////////////////////////////////////////////////////////////////////////

FLOAT_RESULT_CLASS MinResultTable[NUM_FLOAT_INPUT_CLASSES][NUM_FLOAT_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF		-Real		-1			-denorm		-0			+0			+denorm		+1			+Real		+INF		NaN
/* -INF		*/	{	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_INFn	},
/* -Real	*/	{	FRC_INFn,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_SRC0p,	FRC_SRC0p	},
/* -1		*/	{	FRC_INFn,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_EXACT,	FRC_SRC0p,	FRC_SRC0p	},
/* -denorm	*/	{	FRC_INFn,	FRC_EXACT,	FRC_EXACT,	FRC_DENZn,	FRC_DENZn,	FRC_DENZ,	FRC_DENZ,	FRC_DENZn,	FRC_DENZn,	FRC_DENZn,	FRC_DENZn	},
/* -0		*/	{	FRC_INFn,	FRC_EXACT,	FRC_EXACT,	FRC_DENZn,	FRC_ZEROn,	FRC_ZERO,	FRC_DENZ,	FRC_EXACT,	FRC_EXACT,	FRC_SRC0p,	FRC_SRC0p	},
/* +0		*/	{	FRC_INFn,	FRC_EXACT,	FRC_EXACT,	FRC_DENZ,	FRC_ZERO,	FRC_ZEROp,	FRC_DENZp,	FRC_EXACT,	FRC_EXACT,	FRC_SRC0p,	FRC_SRC0p	},
/* +denorm	*/	{	FRC_INFn,	FRC_EXACT,	FRC_EXACT,	FRC_DENZ,	FRC_DENZ,	FRC_DENZp,	FRC_DENZp,	FRC_DENZp,	FRC_DENZp,	FRC_DENZp,	FRC_DENZp	},
/* +1		*/	{	FRC_INFn,	FRC_EXACT,	FRC_EXACT,	FRC_DENZn,	FRC_EXACT,	FRC_EXACT,	FRC_DENZp,	FRC_EXACT,	FRC_EXACT,	FRC_SRC0p,	FRC_SRC0p	},
/* +Real	*/	{	FRC_INFn,	FRC_EXACT,	FRC_EXACT,	FRC_DENZn,	FRC_EXACT,	FRC_EXACT,	FRC_DENZp,	FRC_EXACT,	FRC_EXACT,	FRC_SRC0p,	FRC_SRC0p	},
/* +INF		*/	{	FRC_INFn,	FRC_SRC1p,	FRC_SRC1p,	FRC_DENZn,	FRC_SRC1p,	FRC_SRC1p,	FRC_DENZp,	FRC_SRC1p,	FRC_SRC1p,	FRC_INFp,	FRC_INFp	},
/* NaN		*/	{	FRC_INFn,	FRC_SRC1p,	FRC_SRC1p,	FRC_DENZn,	FRC_SRC1p,	FRC_SRC1p,	FRC_DENZp,	FRC_SRC1p,	FRC_SRC1p,	FRC_INFp,	FRC_NAN		},
};

void CShader5xTest_min::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValues ) / ( 2 * sizeof( FloatValues[0] ) );
	size_t stride = sizeof( FloatValues[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValues[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValues[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValues[3], stride, count ) );
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_Src[1][0], new CTableValueSet< float >( &FloatValues[0], stride * 4, count / 4 ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_Src[1][1], new CTableValueSet< float >( &FloatValues[1], stride * 4, count / 4 ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_Src[1][2], new CTableValueSet< float >( &FloatValues[2], stride * 4, count / 4 ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_Src[1][3], new CTableValueSet< float >( &FloatValues[3], stride * 4, count / 4 ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::XFACTOR modsPairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rModsPairwiseRoot = NewCrossFactor(modsPairwiseTable, _countof(modsPairwiseTable), "PairwiseRoot", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCases = ( src0x % src0y % src0z % src0w ) * ( src1x % src1y % src1z % src1w ) * rModsPairwiseRoot;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );
	
	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

void CShader5xTest_min::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1, const float *fSrc2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS ficSrc0 = GetInputClass( fSrc0[i] );
		FLOAT_INPUT_CLASS ficSrc1 = GetInputClass( fSrc1[i] );

		frc[i] = MinResultTable[ficSrc0][ficSrc1];
	}
}

void CShader5xTest_min::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = Flush( src0[i] ) < Flush( src1[i] ) ? src0[i] : src1[i];
	}
}


bool CShader5xTest_min::CompareExact( const float &f, const float &fref, int i ) const
{
	return CShader5xTest_FloatArith::CompareExact( Flush( f ), Flush( fref ), i );
}


//////////////////////////////////////////////////////////////////////////
// MUL
//////////////////////////////////////////////////////////////////////////
FLOAT_RESULT_CLASS MulResultTable[NUM_FLOAT_INPUT_CLASSES][NUM_FLOAT_INPUT_CLASSES] =
{				//											SRC1
/* SRC0 */		//	-INF		-Real		-1			-denorm		-0			+0			+denorm		+1			+Real		+INF		NaN
/* -INF		*/	{	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_NAN },
/* -Real	*/	{	FRC_INFp,	FRC_REALp,	FRC_SRC0n,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROn,	FRC_ZEROn,	FRC_SRC0p,	FRC_REALn,	FRC_INFn,	FRC_NAN },
/* -1		*/	{	FRC_INFp,	FRC_SRC1n,	FRC_ONEp,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROn,	FRC_ZEROn,	FRC_ONEn,	FRC_SRC1n,	FRC_INFn,	FRC_NAN },
/* -denorm	*/	{	FRC_NAN,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_NAN,	FRC_NAN },
/* -0		*/	{	FRC_NAN,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_NAN,	FRC_NAN },
/* +0		*/	{	FRC_NAN,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_NAN,	FRC_NAN },
/* +denorm	*/	{	FRC_NAN,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_ZEROp,	FRC_NAN,	FRC_NAN },
/* +1		*/	{	FRC_INFn,	FRC_SRC1p,	FRC_ONEn,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROp,	FRC_ZEROp,	FRC_ONEp,	FRC_SRC1p,	FRC_INFp,	FRC_NAN },
/* +Real	*/	{	FRC_INFn,	FRC_REALn,	FRC_SRC0n,	FRC_ZEROn,	FRC_ZEROn,	FRC_ZEROp,	FRC_ZEROp,	FRC_SRC0p,	FRC_REALp,	FRC_INFp,	FRC_NAN },
/* +INF		*/	{	FRC_INFn,	FRC_INFn,	FRC_INFn,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_INFp,	FRC_INFp,	FRC_INFp,	FRC_NAN },
/* NaN		*/	{	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN,	FRC_NAN }
};

void CShader5xTest_mul::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValues ) / ( 2 * sizeof( FloatValues[0] ) );
	size_t stride = sizeof( FloatValues[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValues[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValues[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValues[3], stride, count ) );
	testfactor::RFactor src1x = AddParameter( "Src1.x", &m_Src[1][0], new CTableValueSet< float >( &FloatValues[0], stride * 4, count / 4 ) );
	testfactor::RFactor src1y = AddParameter( "Src1.y", &m_Src[1][1], new CTableValueSet< float >( &FloatValues[1], stride * 4, count / 4 ) );
	testfactor::RFactor src1z = AddParameter( "Src1.z", &m_Src[1][2], new CTableValueSet< float >( &FloatValues[2], stride * 4, count / 4 ) );
	testfactor::RFactor src1w = AddParameter( "Src1.w", &m_Src[1][3], new CTableValueSet< float >( &FloatValues[3], stride * 4, count / 4 ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor src1mod = AddParameter( "Src1Mod", &m_SrcMod[1], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();
	
	testfactor::XFACTOR modsPairwiseTable[] =
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
		{src1mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rModsPairwiseRoot = NewCrossFactor(modsPairwiseTable, _countof(modsPairwiseTable), "PairwiseRoot", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCases = ( src0x % src0y % src0z % src0w ) * ( src1x % src1y % src1z % src1w ) * rModsPairwiseRoot;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );
	
	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 3.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

void CShader5xTest_mul::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1, const float *fSrc2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS ficSrc0 = GetInputClass( fSrc0[i] );
		FLOAT_INPUT_CLASS ficSrc1 = GetInputClass( fSrc1[i] );

		frc[i] = MulResultTable[ficSrc0][ficSrc1];
	}
}

void CShader5xTest_mul::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = src0[i] * src1[i]; 
	}
}



//////////////////////////////////////////////////////////////////////////
// ROUND
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_round::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValuesForRounding ) / ( sizeof( FloatValues[0] ) );
	size_t stride = sizeof( FloatValuesForRounding[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValuesForRounding[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValuesForRounding[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValuesForRounding[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValuesForRounding[3], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Use pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

void CShader5xTest_round::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1, const float *fSrc2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		switch( GetInputClass( fSrc0[i] ) )
		{
			case FIC_INFn:
				frc[i] = FRC_INFn;
				break;
				
			case FIC_REALn:
			case FIC_ONEn:
				frc[i] = FRC_REALn;
				break;
				
			case FIC_DENn:
			case FIC_ZEROn:
				frc[i] = FRC_ZEROn;
				break;
				
			case FIC_ZEROp:
			case FIC_DENp:
				frc[i] = FRC_ZEROp;
				break;
				
			case FIC_REALp:
			case FIC_ONEp:
				frc[i] = FRC_REALp;
				break;
				
			case FIC_INFp:
				frc[i] = FRC_INFp;
				break;
				
			case FIC_NAN:
				frc[i] = FRC_NAN;
				break;
				
			default:
				frc[i] = FRC_UNKNOWN;
				break;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// ROUND_NE
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_round_ne::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = round_ne( src0[i] ); 
	}
}


//////////////////////////////////////////////////////////////////////////
// ROUND_NI
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_round_ni::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = round_ni( src0[i] ); 
	}
}


//////////////////////////////////////////////////////////////////////////
// ROUND_PI
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_round_pi::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = round_pi( src0[i] ); 
	}
}


//////////////////////////////////////////////////////////////////////////
// ROUND_Z
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_round_z::CalcExpectedResult( float fResult[4], const float *src0, const float *src1, const float *src2 ) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = round_z( src0[i] ); 
	}
}



//////////////////////////////////////////////////////////////////////////
// RSQ
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_rsq::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValues ) / ( 2 * sizeof( FloatValues[0] ) );
	size_t stride = sizeof( FloatValues[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValues[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValues[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValues[3], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Use pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

bool CShader5xTest_rsq::CompareReal( const float &f, const float &fref, int i ) const
{
/*
	// Compute the absolute error (tolerance is 2^-21).
	float fDiff = fref - f;
	if( fDiff < FloatMaker( -21, 0 ) )
		return true;
*/		
	// Compute the relative error (tolerance is 2^-21).
	int nDiff = *( (DWORD *) &f ) - *( (DWORD *) &fref );
	nDiff = nDiff < 0 ? -nDiff : nDiff;
	return nDiff <= 4; 
}

void CShader5xTest_rsq::CalcExpectedResult( float fResult[4], const float *src0, const float *src1 /* = NULL */, const float *src2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = (float) ( 1.0 / sqrt( src0[i] ) );
	}
}

const FLOAT_RESULT_CLASS RsqResultTable[] =
	{
		FRC_NAN,	// -Infinity
		FRC_NAN,	// -Real
		FRC_NAN,	// -1
		FRC_INFn,	// -Denormal
		FRC_INFn,	// -0
		FRC_INFp,	// +0
		FRC_INFp,	// +Denormal
		FRC_REALp,	// +1
		FRC_REALp,	// +Real
		FRC_ZEROp,	// +Infinity
		FRC_NAN,	// Nan
	};
	
void CShader5xTest_rsq::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 /* = NULL */, const float *fSrc2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS fic = GetInputClass( fSrc0[i] );
		
		frc[i] = RsqResultTable[fic];
	}
}

//////////////////////////////////////////////////////////////////////////
// RCP
//////////////////////////////////////////////////////////////////////////

TEST_RESULT CShader5xTest_rcp::Setup()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;

	// Call the parent setup
	tRes = CShader5xTest_rsq::Setup();

	if( RESULT_PASS != tRes )
		goto setupDone;

	// This is a FL11+ only instruction
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("The instruction rcp, is only for Feature Level 11 or greater.") );
		tRes = RESULT_SKIP;
		goto setupDone;
	}

setupDone:
	return tRes;
}

void CShader5xTest_rcp::CalcExpectedResult( float fResult[4], const float *src0, const float *src1 /* = NULL */, const float *src2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = (float) ( 1.0 / src0[i] );
	}
}

const FLOAT_RESULT_CLASS RcpResultTable[] =
	{
		FRC_ZEROn,	// -Infinity
		FRC_REALn,	// -Real
		FRC_REALn,	// -1
		FRC_INFn,	// -Denormal
		FRC_INFn,	// -0
		FRC_INFp,	// +0
		FRC_INFp,	// +Denormal
		FRC_REALp,	// +1
		FRC_REALp,	// +Real
		FRC_ZEROp,	// +Infinity
		FRC_NAN,	// Nan
	};
	
void CShader5xTest_rcp::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 /* = NULL */, const float *fSrc2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS fic = GetInputClass( fSrc0[i] );
		
		frc[i] = RcpResultTable[fic];
	}
}

//////////////////////////////////////////////////////////////////////////
// SQRT
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_sqrt::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	size_t count = sizeof( FloatValues ) / ( 2 * sizeof( FloatValues[0] ) );
	size_t stride = sizeof( FloatValues[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValues[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValues[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValues[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValues[3], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Use pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders));
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

void CShader5xTest_sqrt::CalcExpectedResult( float fResult[4], const float *src0, const float *src1 /* = NULL */, const float *src2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = (float) ( sqrt( src0[i] ) );
	}
}

const FLOAT_RESULT_CLASS SqrtResultTable[] =
	{
		FRC_NAN,	// -Infinity
		FRC_NAN,	// -Real
		FRC_NAN,	// -1
		FRC_ZEROn,	// -Denormal
		FRC_ZEROn,	// -0
		FRC_ZEROp,	// +0
		FRC_ZEROp,	// +Denormal
		FRC_REALp,	// +1
		FRC_REALp,	// +Real
		FRC_INFp,	// +Infinity
		FRC_NAN,	// Nan
	};
	
void CShader5xTest_sqrt::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 /* = NULL */, const float *fSrc2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS fic = GetInputClass( fSrc0[i] );
		
		frc[i] = SqrtResultTable[fic];
	}
}

BEGIN_NAMED_VALUES( CShader5xTest_sincos::SINCOSOUTPUT )
	NAMED_VALUE( _T( "SinOnly" ), CShader5xTest_sincos::SINONLY )
	NAMED_VALUE( _T( "CosOnly" ), CShader5xTest_sincos::COSONLY )
	NAMED_VALUE( _T( "Both" ), CShader5xTest_sincos::BOTH )
END_NAMED_VALUES( CShader5xTest_sincos::SINCOSOUTPUT );

void CShader5xTest_sincos::InitTestParameters()
{
	CUserValueSet< SRC_MOD_TYPE > *pSrcModValues = new CUserValueSet< SRC_MOD_TYPE >();
	pSrcModValues->AddValue( SMOD_NONE );
	pSrcModValues->AddValue( SMOD_NEG );
	pSrcModValues->AddValue( SMOD_ABS );
	pSrcModValues->AddValue( SMOD_ABSNEG );
	
	CUserValueSet< INST_MOD_TYPE > *pInstModValues = new CUserValueSet< INST_MOD_TYPE >();
	pInstModValues->AddValue( IMOD_NONE );
	pInstModValues->AddValue( IMOD_SAT );
	
	CUserValueSet< SINCOSOUTPUT > *pSinCosOutputValues = new CUserValueSet< SINCOSOUTPUT >();
	pSinCosOutputValues->AddValue( BOTH );
	pSinCosOutputValues->AddValue( SINONLY );
	pSinCosOutputValues->AddValue( COSONLY );
	
	size_t count = sizeof( FloatValuesForSinCos ) / ( 2 * sizeof( FloatValuesForSinCos[0] ) );
	size_t stride = sizeof( FloatValuesForSinCos[0] );
	
	testfactor::RFactor instmod = AddParameter( "InstMod", &m_InstMod, pInstModValues );
	testfactor::RFactor src0x = AddParameter( "Src0.x", &m_Src[0][0], new CTableValueSet< float >( &FloatValuesForSinCos[0], stride, count ) );
	testfactor::RFactor src0y = AddParameter( "Src0.y", &m_Src[0][1], new CTableValueSet< float >( &FloatValuesForSinCos[1], stride, count ) );
	testfactor::RFactor src0z = AddParameter( "Src0.z", &m_Src[0][2], new CTableValueSet< float >( &FloatValuesForSinCos[2], stride, count ) );
	testfactor::RFactor src0w = AddParameter( "Src0.w", &m_Src[0][3], new CTableValueSet< float >( &FloatValuesForSinCos[3], stride, count ) );
	testfactor::RFactor src0mod = AddParameter( "Src0Mod", &m_SrcMod[0], pSrcModValues );
	testfactor::RFactor sincosout = AddParameter( "SinCosOutput", &m_SinCosOutput, pSinCosOutputValues );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Use pairwise to reduce the number of testcases
	testfactor::RFactor src0 = src0x % src0y % src0z % src0w;

	testfactor::XFACTOR pairwiseTable[] = 
	{
		// Factor, Group, pNotGrps, cNotGrps
		{instmod, 1, NULL, 0},
		{src0, 1, NULL, 0},
		{src0mod, 1, NULL, 0},
	};

	size_t groupOrders[] = 
	{
		XGROUPORDER(1, 2), // First number is the group. 2nd number is the degree to n-wise. Thus 2 = pairwise.
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = NewCrossFactor(pairwiseTable, _countof(pairwiseTable), "PairwiseTable", groupOrders, _countof(groupOrders)) * sincosout;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstMod"), IMOD_NONE), WeightFactorLarge);
	
	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

bool CShader5xTest_sincos::IsDestNull( int nIndex ) const
{
	switch( nIndex )
	{
		case 0:
			return m_SinCosOutput == COSONLY;
			break;
			
		case 1:
			return m_SinCosOutput == SINONLY;
			break;
	}
	
	return false;
}

TEST_RESULT CShader5xTest_sincos::SetupRenderTarget()
{
	// Local variables
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
	const UINT rtFormatElementSizeInBytes = GetBitsPerElement( m_RTFormat ) / 8;

	//
	// Setup RenderTarget
	//

	// Setup the RTBuffer description
	bufferDesc.ByteWidth			= rtFormatElementSizeInBytes * (UINT)(m_viewPort.Height) * (UINT)(m_viewPort.Width);
	bufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags			= D3D11_BIND_RENDER_TARGET;
	bufferDesc.BindFlags			|= (g_TestApp.UAVIsAllowed()) ? D3D11_BIND_UNORDERED_ACCESS : 0;
	bufferDesc.CPUAccessFlags		= 0;
	bufferDesc.MiscFlags			= 0;
	bufferDesc.MiscFlags			|= (g_TestApp.UAVIsAllowed()) ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;
	bufferDesc.StructureByteStride	= 0;

	// Create the RTBuffer
	if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pRTBuffer) ) )
	{
		WriteToLog( _T("CShader5xTest_sincos::SetupRenderTarget() - failed to create RT buffer (m_pRTBuffer). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	// Create the RTBuffer
	if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pRTBuffer2) ) )
	{
		WriteToLog( _T("CShader5xTest_sincos::SetupRenderTarget() - failed to create RT buffer (m_pRTBuffer2). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	// Setup the staging RTBuffer description
	bufferDesc.BindFlags = 0;
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.MiscFlags = 0;
	
	// Create the staging RTBuffer
	if( FAILED( hr = GetDevice()->CreateBuffer( &bufferDesc, NULL, &m_pRTBufferCopy ) ) )
	{
		WriteToLog( _T("CShader5xTest_sincos::SetupRenderTarget() - failed to create staging RT buffer (m_pRTBufferCopy). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	// Create the Render Target View (RTV)
	rtViewDesc.Format = m_RTFormat;
	rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
	rtViewDesc.Buffer.FirstElement = 0;
	rtViewDesc.Buffer.NumElements = bufferDesc.ByteWidth / rtFormatElementSizeInBytes;

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTBuffer, &rtViewDesc, &m_pRTBufferView ) ) )
	{
		WriteToLog( _T("CShader5xTest_sincos::SetupRenderTarget() - failed to create Render Target View (m_pRTBufferView). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	if( FAILED( hr = GetDevice()->CreateRenderTargetView( m_pRTBuffer2, &rtViewDesc, &m_pRTBufferView2 ) ) )
	{
		WriteToLog( _T("CShader5xTest_sincos::SetupRenderTarget() - failed to create Render Target View (m_pRTBufferView2). HR = %s"), D3DHResultToString(hr) );
		return RESULT_FAIL;
	}

	// Only create a UAV if the FL allows it
	if( g_TestApp.UAVIsAllowed() )
	{
		// Since the UAV is R32 Typeless, we need to account for the different number of elements
		const DXGI_FORMAT uavFormat = DXGI_FORMAT_R32_TYPELESS;
		const UINT uavFormatElementSizeInBytes = GetBitsPerElement( uavFormat ) / 8;
		assert( rtFormatElementSizeInBytes >= uavFormatElementSizeInBytes);
		const UINT elementDifference = rtFormatElementSizeInBytes / uavFormatElementSizeInBytes; // This accounts for the fact the 4x R32_Typeless = 1 R32B32G32A32_UINT etc...	

		D3D11_UNORDERED_ACCESS_VIEW_DESC rtUAVDesc;
		rtUAVDesc.Format = uavFormat;
		rtUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		rtUAVDesc.Buffer.FirstElement = 0;
		rtUAVDesc.Buffer.NumElements = rtViewDesc.Buffer.NumElements * elementDifference;
		rtUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

		if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pRTBuffer, &rtUAVDesc, &m_pRTUABufferView ) ) )
		{
			WriteToLog( _T("CShader5xTest_sincos::SetupRenderTarget() - failed to create Unordered Access View (m_pRTUABufferView). HR = %s"), D3DHResultToString(hr) );
			return RESULT_FAIL;
		}

		if( FAILED( hr = GetDevice()->CreateUnorderedAccessView( m_pRTBuffer2, &rtUAVDesc, &m_pRTUABufferView2 ) ) )
		{
			WriteToLog( _T("CShader5xTest_sincos::SetupRenderTarget() - failed to create Unordered Access View (m_pRTUABufferView2). HR = %s"), D3DHResultToString(hr) );
			return RESULT_FAIL;
		}
	}

	ID3D11RenderTargetView *views[] =
	{
		m_pRTBufferView,
		m_pRTBufferView2
	};

	GetEffectiveContext()->OMSetRenderTargets( 2, views, NULL );
	
	return RESULT_PASS;
}

void CShader5xTest_sincos::CalcExpectedResult( float fResult[4], const float *src0, const float *src1 /* = NULL */, const float *src2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = (float) ( sin( src0[i] ) );
	}
}

const FLOAT_RESULT_CLASS SinResultTable[] =
	{
		FRC_NAN,	// -Infinity
		FRC_REAL,	// -Real
		FRC_REAL,	// -1
		FRC_ZEROn,	// -Denormal
		FRC_ZEROn,	// -0
		FRC_ZEROp,	// +0
		FRC_ZEROp,	// +Denormal
		FRC_REAL,	// +1
		FRC_REAL,	// +Real
		FRC_NAN,	// +Infinity
		FRC_NAN,	// Nan
	};
	
void CShader5xTest_sincos::GetResultClass( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 /* = NULL */, const float *fSrc2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS fic = GetInputClass( fSrc0[i] );

		if( ( fic == FIC_REALn || fic == FIC_REALp ) && fabsf( fSrc0[i] ) > 100 * pi )
		{
			// Input values outside the range [-100*pi,100*pi]
			frc[i] = FRC_ANY_REAL;
		}
		else
		{
			frc[i] = SinResultTable[fic];
		}
	}
}


void CShader5xTest_sincos::CalcExpectedResult2( float fResult[4], const float *src0, const float *src1 /* = NULL */, const float *src2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		fResult[i] = (float) ( cos( src0[i] ) );
	}
}

const FLOAT_RESULT_CLASS CosResultTable[] =
	{
		FRC_NAN,	// -Infinity
		FRC_REAL,	// -Real
		FRC_REAL,	// -1
		FRC_ONEp,	// -Denormal
		FRC_ONEp,	// -0
		FRC_ONEp,	// +0
		FRC_ONEp,	// +Denormal
		FRC_REAL,	// +1
		FRC_REAL,	// +Real
		FRC_NAN,	// +Infinity
		FRC_NAN,	// Nan
	};
	
void CShader5xTest_sincos::GetResultClass2( FLOAT_RESULT_CLASS frc[4], const float *fSrc0, const float *fSrc1 /* = NULL */, const float *fSrc2 /* = NULL  */) const
{
	for( int i = 0; i < 4; ++i )
	{
		FLOAT_INPUT_CLASS fic = GetInputClass( fSrc0[i] );

		if( ( fic == FIC_REALn || fic == FIC_REALp ) && ( fabsf( fSrc0[i] ) ) > 100 * pi )
		{
			// Input values outside the range [-100*pi,100*pi]
			frc[i] = FRC_ANY_REAL;
		}
		else
		{
			frc[i] = CosResultTable[fic];
		}
	}
}

bool CShader5xTest_sincos::CompareReal( const float &f, const float &fref, int i ) const
{
	// Make certain that f <= -1 <= 1.  No sloppiness (ie cos(0) = 1.000000000001)
	if( f < -1.f )
		return false;
		
	if( f > 1.f )
		return false;
		
	assert( fabsf( m_Src[0][i] ) <= 100 * pi );
		
	float fdiff = fabsf( fref - f );
	if( fdiff > 0.0008f )
		return false;
		
	return true;
}
