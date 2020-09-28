//=======================================================================================
//
//  ShaderUtils.cpp
//
//=======================================================================================

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "d3dx9.h"		// Include this now until headers stabilize, then switch to CD3DDef.h
#include "d3dx10.h"
#include "ShaderUtils.h"
#include "IncludeResource.h"

// -------------------------------------------------------------------------------------

HRESULT GetShaderOpCount( DWORD *pdwShader, DWORD dwDWORDCount, SHADER_OPCOUNT_INFO *pOpCountInfo, bool bDebugSpewEnabled )
{
	// a legal shader must be composed of at least 3 DWORD's
	if ( dwDWORDCount < 3 )
	{
		if ( bDebugSpewEnabled )
		{
			OutputDebugString( _T("######### GetShaderOpCount() failed -- shader consists of less than 3 DWORDs!\n") );
		}

		return E_FAIL;
	}

	ZeroMemory( pOpCountInfo, sizeof( SHADER_OPCOUNT_INFO ) );

	DWORD dwVersion = 0;
	D3DSHADER_INSTRUCTION_OPCODE_TYPE eOp, ePrevOp = D3DSIO_NOP;

	// lets us track flow control types, so we know how to count things like 'else'
	// 0 == invalid, 1 == static, 2 == dynamic
	// TODO: move these up
#define FC_TYPE_INVALID 0
#define FC_TYPE_STATIC  1
#define FC_TYPE_DYNAMIC 2

	DWORD * aFCOps = new DWORD[ dwDWORDCount ];
	DWORD dwFCOpCount = 0;

	for ( DWORD i = 0; i < dwDWORDCount; ++i )
	{
		if ( ( *pdwShader & 0x0000ffff ) == 0x0000fffe )  // COMMENT block
		{
			DWORD dwSkipCount = ( ( *pdwShader & 0x7fff0000 ) >> 16 );
			pdwShader += ( dwSkipCount + 1 );
			i += dwSkipCount;
			continue;
		}

		if ( ( *pdwShader & 0xfffe0000 ) == 0xfffe0000 ||  // vs_x_x
			 ( *pdwShader & 0xffff0000 ) == 0xffff0000 )   // ps_x_x
		{
			dwVersion = *pdwShader;
			++pdwShader;
			continue;
		}
			 
		if ( *pdwShader & 0x80000000 )  // non-OP token
		{
			++pdwShader;
			continue;
		}

		eOp = (D3DSHADER_INSTRUCTION_OPCODE_TYPE)( *pdwShader & 0x0000ffff );

		switch ( eOp )
		{
			case D3DSIO_DEF:
			case D3DSIO_DEFI:

				pdwShader += 5;
				i += 5;
				pOpCountInfo->dwCount_DEF += 1;
				break;

			case D3DSIO_DEFB:

				pdwShader += 2;
				i += 2;
				pOpCountInfo->dwCount_DEF += 1;
				break;

			case D3DSIO_DCL:

				pOpCountInfo->dwCount_DCL += 1;
				break;


				//-----------------------------------------------------------------------------------------------------------
				// TEX
				//-----------------------------------------------------------------------------------------------------------

			case D3DSIO_TEXCOORD:
			case D3DSIO_TEXKILL:
			case D3DSIO_TEX:

				pOpCountInfo->dwCount_Tex   += 1;
				pOpCountInfo->dwCount_Total += 1;
				break;

			case D3DSIO_TEXREG2AR:
			case D3DSIO_TEXREG2GB:
			case D3DSIO_TEXBEM:
			case D3DSIO_TEXM3x2PAD:
			case D3DSIO_TEXM3x2TEX:
			case D3DSIO_TEXM3x3PAD:
			case D3DSIO_TEXM3x3TEX:
			case D3DSIO_TEXM3x3SPEC:
			case D3DSIO_TEXM3x3VSPEC:
			case D3DSIO_TEXREG2RGB:
			case D3DSIO_TEXDP3TEX:
			case D3DSIO_TEXM3x2DEPTH:
			case D3DSIO_TEXDP3:
			case D3DSIO_TEXM3x3:
			case D3DSIO_TEXDEPTH:
			case D3DSIO_TEXLDD:
			case D3DSIO_TEXLDL:

				pOpCountInfo->dwCount_Tex   += 1;
				pOpCountInfo->dwCount_Total += 1;
				break;

			case D3DSIO_TEXBEML:

				pOpCountInfo->dwCount_Tex   += 1;
				pOpCountInfo->dwCount_Arith += 1;
				pOpCountInfo->dwCount_Total += 2;
				break;


				//-----------------------------------------------------------------------------------------------------------
				// ARITH
				//-----------------------------------------------------------------------------------------------------------

			case D3DSIO_MOV:
			case D3DSIO_ADD:
			case D3DSIO_MAD:
			case D3DSIO_MUL:
			case D3DSIO_RCP:
			case D3DSIO_RSQ:
			case D3DSIO_DP3:
			case D3DSIO_SLT:
			case D3DSIO_SGE:
			case D3DSIO_DST:
			case D3DSIO_MOVA:
			case D3DSIO_EXPP:
			case D3DSIO_LOGP:
			case D3DSIO_CND:
			case D3DSIO_SETP:
			case D3DSIO_MIN:
			case D3DSIO_MAX:

				pOpCountInfo->dwCount_Arith += ( ( *pdwShader & 0x40000000 ) ? 0 : 1 );
				pOpCountInfo->dwCount_Total += ( ( *pdwShader & 0x40000000 ) ? 0 : 1 );
				break;

			case D3DSIO_DP2ADD:

				switch( dwVersion )
				{
				case D3DPS_VERSION( 2, 1 ):
					pOpCountInfo->dwCount_Arith += 2;
					pOpCountInfo->dwCount_Total += 2;
					break;

				default:
					pOpCountInfo->dwCount_Arith += 1;
					pOpCountInfo->dwCount_Total += 1;
				}
				break;

			case D3DSIO_DSX:
			case D3DSIO_DSY:

				pOpCountInfo->dwCount_Arith += 2;
				pOpCountInfo->dwCount_Total += 2;
				break;

			case D3DSIO_LIT:

				switch ( dwVersion )
				{
					case D3DVS_VERSION( 1, 0 ):
					case D3DVS_VERSION( 1, 1 ):

						pOpCountInfo->dwCount_Arith += 1;
						pOpCountInfo->dwCount_Total += 1;
						break;

					default:

						pOpCountInfo->dwCount_Arith += 3;
						pOpCountInfo->dwCount_Total += 3;
						break;
				}
				break;

			case D3DSIO_DP4:

				switch ( dwVersion )
				{
					case D3DPS_VERSION( 1, 2 ):
					case D3DPS_VERSION( 1, 3 ):

						pOpCountInfo->dwCount_Arith += 2;
						pOpCountInfo->dwCount_Total += 2;
						break;

					default:

						pOpCountInfo->dwCount_Arith += 1;
						pOpCountInfo->dwCount_Total += 1;
						break;
				}
				break;

			case D3DSIO_CMP:

				switch ( dwVersion )
				{
					case D3DPS_VERSION( 1, 2 ):
					case D3DPS_VERSION( 1, 3 ):

						if ( *pdwShader & 0x40000000 )
						{
							if ( ePrevOp != D3DSIO_CMP )
							{
								pOpCountInfo->dwCount_Arith += 1;
								pOpCountInfo->dwCount_Total += 1;
							}
						}
						else
						{
							pOpCountInfo->dwCount_Arith += 2;
							pOpCountInfo->dwCount_Total += 2;
						}
						break;

					default:

						pOpCountInfo->dwCount_Arith += 1;
						pOpCountInfo->dwCount_Total += 1;
						break;
				}
				break;

			case D3DSIO_BEM:

				pOpCountInfo->dwCount_Arith += 2;
				pOpCountInfo->dwCount_Total += 2;
				break;

			case D3DSIO_FRC:

				switch ( dwVersion )
				{
					case D3DVS_VERSION( 1, 1 ):

						pOpCountInfo->dwCount_Arith += 3;
						pOpCountInfo->dwCount_Total += 3;
						break;

					default:

						pOpCountInfo->dwCount_Arith += 1;
						pOpCountInfo->dwCount_Total += 1;
						break;
				}
				break;

			case D3DSIO_EXP:
			case D3DSIO_LOG:

				switch ( dwVersion )
				{
					case D3DVS_VERSION( 1, 1 ):

						pOpCountInfo->dwCount_Arith += 10;
						pOpCountInfo->dwCount_Total += 10;
						break;

					default:

						pOpCountInfo->dwCount_Arith += 1;
						pOpCountInfo->dwCount_Total += 1;
						break;
				}
				break;


				//-----------------------------------------------------------------------------------------------------------
				// MACRO
				//-----------------------------------------------------------------------------------------------------------

			case D3DSIO_SUB:
			case D3DSIO_ABS:

				pOpCountInfo->dwCount_Arith += 1;
				pOpCountInfo->dwCount_Total += 1;
				break;

			case D3DSIO_LRP:

				switch ( dwVersion )
				{
					case D3DPS_VERSION( 1, 1 ):
					case D3DPS_VERSION( 1, 2 ):
					case D3DPS_VERSION( 1, 3 ):
					case D3DPS_VERSION( 1, 4 ):

						pOpCountInfo->dwCount_Arith += 1;
						pOpCountInfo->dwCount_Total += 1;
						break;

					default:

						pOpCountInfo->dwCount_Arith += 2;
						pOpCountInfo->dwCount_Total += 2;
						pOpCountInfo->dwCount_Macro += 2;
						break;
				}
				break;

			case D3DSIO_M3x2:
			case D3DSIO_CRS:

				pOpCountInfo->dwCount_Arith += 2;
				pOpCountInfo->dwCount_Total += 2;
				pOpCountInfo->dwCount_Macro += 2;
				break;

			case D3DSIO_M3x3:
			case D3DSIO_M4x3:
			case D3DSIO_POW:
			case D3DSIO_SGN:
			case D3DSIO_NRM:

				pOpCountInfo->dwCount_Arith += 3;
				pOpCountInfo->dwCount_Total += 3;
				pOpCountInfo->dwCount_Macro += 3;
				break;

			case D3DSIO_M3x4:
			case D3DSIO_M4x4:

				pOpCountInfo->dwCount_Arith += 4;
				pOpCountInfo->dwCount_Total += 4;
				pOpCountInfo->dwCount_Macro += 4;
				break;

			case D3DSIO_SINCOS:

				pOpCountInfo->dwCount_Arith += 8;
				pOpCountInfo->dwCount_Total += 8;
				pOpCountInfo->dwCount_Macro += 8;
				break;


				//-----------------------------------------------------------------------------------------------------------
				// FLOWCTRL
				//-----------------------------------------------------------------------------------------------------------

			case D3DSIO_CALL:

				aFCOps[ dwFCOpCount ++ ] = FC_TYPE_STATIC;
				pOpCountInfo->dwCount_FlowCtrl_S += 2;
				pOpCountInfo->dwCount_Total      += 2;
				break;

			case D3DSIO_REP:
			case D3DSIO_LOOP:

				aFCOps[ dwFCOpCount ++ ] = FC_TYPE_STATIC;
				pOpCountInfo->dwCount_FlowCtrl_S += 3;
				pOpCountInfo->dwCount_Total      += 3;
				break;

			case D3DSIO_IFC:

				aFCOps[ dwFCOpCount ++ ] = FC_TYPE_DYNAMIC;
				pOpCountInfo->dwCount_FlowCtrl_D += 3;
				pOpCountInfo->dwCount_Total      += 3;
				break;

			case D3DSIO_IF:
			case D3DSIO_CALLNZ:

				if ( ( (*(pdwShader + 1) & 0x70000000 ) >> 28 ) +
					 ( (*(pdwShader + 1) & 0x00001800 ) >> 8  ) == D3DSPR_PREDICATE )
				{
					aFCOps[ dwFCOpCount ++ ] = FC_TYPE_DYNAMIC;
					pOpCountInfo->dwCount_FlowCtrl_D += 3;
				}
				else
				{
					aFCOps[ dwFCOpCount ++ ] = FC_TYPE_STATIC;
					pOpCountInfo->dwCount_FlowCtrl_S += 3;
				}

				pOpCountInfo->dwCount_Total += 3;
				break;

			case D3DSIO_BREAKC:
			case D3DSIO_BREAKP:

				// these are always counted as dynamic, even in a static FC block
				pOpCountInfo->dwCount_FlowCtrl_D += 3;
				pOpCountInfo->dwCount_Total      += 3;
				break;

			case D3DSIO_ELSE:
			case D3DSIO_BREAK:

				// look at the last entry in the aFCOps stack, but do not pop
				if( aFCOps[ dwFCOpCount - 1 ] == FC_TYPE_STATIC )
				{
					pOpCountInfo->dwCount_FlowCtrl_S += 1;
				}
				else if( aFCOps[ dwFCOpCount - 1 ] == FC_TYPE_DYNAMIC )
				{
					pOpCountInfo->dwCount_FlowCtrl_D += 1;
				}
				pOpCountInfo->dwCount_Total      += 1;
				break;

			case D3DSIO_RET:
			case D3DSIO_ENDIF:

				// pop the last entry in the aFCOps stack
				if( aFCOps[ dwFCOpCount - 1 ] == FC_TYPE_STATIC )
				{
					pOpCountInfo->dwCount_FlowCtrl_S += 1;
					-- dwFCOpCount;
				}
				else if( aFCOps[ dwFCOpCount - 1 ] == FC_TYPE_DYNAMIC )
				{
					pOpCountInfo->dwCount_FlowCtrl_D += 1;
					-- dwFCOpCount;
				}
				pOpCountInfo->dwCount_Total      += 1;
				break;

			case D3DSIO_ENDLOOP:
			case D3DSIO_ENDREP:

				// pop the last entry in the aFCOps stack
				if( aFCOps[ dwFCOpCount - 1 ] == FC_TYPE_STATIC )
				{
					pOpCountInfo->dwCount_FlowCtrl_S += 2;
					-- dwFCOpCount;
				}
				else if( aFCOps[ dwFCOpCount - 1 ] == FC_TYPE_DYNAMIC )
				{
					pOpCountInfo->dwCount_FlowCtrl_D += 2;
					-- dwFCOpCount;
				}
				pOpCountInfo->dwCount_Total      += 2;
				break;

			case D3DSIO_LABEL:
			case D3DSIO_END:

				break;

				//-----------------------------------------------------------------------------------------------------------
				// MISC
				//-----------------------------------------------------------------------------------------------------------

			case D3DSIO_PHASE:
			case D3DSIO_COMMENT:

				break;

			case D3DSIO_NOP:

				pOpCountInfo->dwCount_NOP += 1;
				break;

			default:

				if ( bDebugSpewEnabled )
				{
					TCHAR szErrMsg[ 100 ];
					_stprintf( szErrMsg, _T("######### GetShaderOpCount() failed -- shader contains unrecognized OP token : 0x%8x !\n"),
							   *pdwShader );
					OutputDebugString( szErrMsg );
				}

				return E_FAIL;
		}

		ePrevOp = eOp;
		++pdwShader;
	}

	// destroy the aFCOps stack
	delete [] aFCOps;

	if ( dwVersion == 0 )
	{
		if ( bDebugSpewEnabled )
		{
			OutputDebugString( _T("######### GetShaderOpCount() failed -- shader doesn't have vs/ps_x_x token!\n") );
		}

		return E_FAIL;
	}

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// CIncludeResource::CIncludeResource
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// CIncludeResource::Open
// 
// Purpose: Callback implementation to open fx/shader files across various
//      include paths
// 
// Parameters: 
//     IncludeType: local or system include (corresponds to #include "" vs <>)
//     pFileName: file name to open
//     pParentData: parent data pointer
//     ppData: (out) data pointer for this file
//     pBytes: (out) size of this file in bytes
// 
// Return type: HRESULT - E_FAIL on error or inability to find file, 
//      S_OK otherwise
// 
// Comments: Cannot read files > 4 GB
// 
///////////////////////////////////////////////////////////////////////////////
HRESULT CIncludeResource::Open(D3D10_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	HRESULT hr          = S_OK;
	HRSRC rsrc          = NULL;
	DWORD dwSizeData    = 0;
	HGLOBAL hData       = NULL;
	LPVOID pData        = NULL;

	rsrc = FindResourceA(NULL, (LPCSTR) pFileName, (LPSTR) RT_RCDATA);
	if (rsrc == NULL)
	{
		hr = E_FAIL;
		goto cleanup;
	}

	dwSizeData = SizeofResource(NULL, rsrc);

	hData = LoadResource(NULL, rsrc);
	if (hData == NULL)
	{
		hr = E_FAIL;
		goto cleanup;
	}

	pData = LockResource(hData);
	if (pData == NULL)
	{
		hr = E_FAIL;
		goto cleanup;
	}

	*ppData = pData;
	*pBytes = dwSizeData;

cleanup:
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
// CIncludeResource::Close
// 
// Purpose: Closes a file opened with ::Open
// 
// Parameters: 
//     pData: pointer to buffer containing file data
// 
// Return type: HRESULT - E_FAIL if data is invalid, S_OK otherwise
// 
// Comments: 
// 
///////////////////////////////////////////////////////////////////////////////
HRESULT CIncludeResource::Close(LPCVOID pData)
{
	return S_OK; // no need to cleanup resources
}


//=======================================================================================
// EOF
