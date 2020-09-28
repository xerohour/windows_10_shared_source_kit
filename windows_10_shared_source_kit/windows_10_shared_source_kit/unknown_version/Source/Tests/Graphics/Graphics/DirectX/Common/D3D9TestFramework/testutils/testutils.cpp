//=======================================================================================
//
//  TestUtils.cpp
//
//=======================================================================================


#include "TestUtilsPCH.h"

#include "TestUtils.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "Framework.h"
#include "D3DX9.h"		// Include this now until headers stabilize, then switch to CD3DDef.h


// --------------------------------------------------------------------------------------

//
// D3D Utils
//
typedef enum _D3DVSD_TOKENTYPE
{
    D3DVSD_TOKEN_NOP        = 0,    // NOP or extension
    D3DVSD_TOKEN_STREAM,            // stream selector
    D3DVSD_TOKEN_STREAMDATA,        // stream data definition (map to vertex input memory)
    D3DVSD_TOKEN_TESSELLATOR,       // vertex input memory from tessellator
    D3DVSD_TOKEN_CONSTMEM,          // constant memory from shader
    D3DVSD_TOKEN_EXT,               // extension
    D3DVSD_TOKEN_END = 7,           // end-of-array (requires all DWORD bits to be 1)
    D3DVSD_FORCE_DWORD = 0x7fffffff,// force 32-bit size enum
} D3DVSD_TOKENTYPE;

// Generates END token
//
#define D3DVSD_END() 0xFFFFFFFF

#define D3DVSD_TOKENTYPESHIFT   29
#define D3DVSD_TOKENTYPEMASK    (7 << D3DVSD_TOKENTYPESHIFT)

#define D3DVSD_STREAMNUMBERSHIFT 0
#define D3DVSD_STREAMNUMBERMASK (0xF << D3DVSD_STREAMNUMBERSHIFT)

#define D3DVSD_DATALOADTYPESHIFT 28
#define D3DVSD_DATALOADTYPEMASK (0x1 << D3DVSD_DATALOADTYPESHIFT)

#define D3DVSD_DATATYPESHIFT 16
#define D3DVSD_DATATYPEMASK (0xF << D3DVSD_DATATYPESHIFT)

#define D3DVSD_SKIPCOUNTSHIFT 16
#define D3DVSD_SKIPCOUNTMASK (0xF << D3DVSD_SKIPCOUNTSHIFT)

#define D3DVSD_VERTEXREGSHIFT 0
#define D3DVSD_VERTEXREGMASK (0x1F << D3DVSD_VERTEXREGSHIFT)

#define D3DVSD_VERTEXREGINSHIFT 20
#define D3DVSD_VERTEXREGINMASK (0xF << D3DVSD_VERTEXREGINSHIFT)

#define D3DVSD_CONSTCOUNTSHIFT 25
#define D3DVSD_CONSTCOUNTMASK (0xF << D3DVSD_CONSTCOUNTSHIFT)

#define D3DVSD_CONSTADDRESSSHIFT 0
#define D3DVSD_CONSTADDRESSMASK (0x7F << D3DVSD_CONSTADDRESSSHIFT)

#define D3DVSD_CONSTRSSHIFT 16
#define D3DVSD_CONSTRSMASK (0x1FFF << D3DVSD_CONSTRSSHIFT)

#define D3DVSD_EXTCOUNTSHIFT 24
#define D3DVSD_EXTCOUNTMASK (0x1F << D3DVSD_EXTCOUNTSHIFT)

#define D3DVSD_EXTINFOSHIFT 0
#define D3DVSD_EXTINFOMASK (0xFFFFFF << D3DVSD_EXTINFOSHIFT)

#define D3DVSD_MAKETOKENTYPE(tokenType) ((tokenType << D3DVSD_TOKENTYPESHIFT) & D3DVSD_TOKENTYPEMASK)

#define D3DVSD_STREAMTESSSHIFT 28
#define D3DVSD_STREAMTESSMASK (1 << D3DVSD_STREAMTESSSHIFT)


#define D3DVSD_GETREG( Token, Register, Type ) \
    Type = ((D3DVSD_DATATYPEMASK & Token) >> D3DVSD_DATATYPESHIFT);\
	Register = ((D3DVSD_VERTEXREGMASK & Token) >> D3DVSD_VERTEXREGSHIFT)

#define D3DVSD_CONST( _ConstantAddress, _Count ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_CONSTMEM) | \
     ((_Count) << D3DVSD_CONSTCOUNTSHIFT) | (_ConstantAddress))

#define D3DVSD_GETTESSNORMAL( Token, RegisterIn, RegisterOut ) \
	RegisterIn  = (D3DVSD_VERTEXREGMASK & (Token >> D3DVSD_VERTEXREGINSHIFT)); \
	RegisterOut = ((D3DVSD_VERTEXREGMASK & Token) >> D3DVSD_VERTEXREGSHIFT)

#define D3DVSD_GETTESSUV( Token, Register) \
	Register = ((D3DVSD_VERTEXREGMASK & Token) >> D3DVSD_VERTEXREGSHIFT)

#define D3DVSD_GETSTREAM( Token ) \
    ((D3DVSD_STREAMNUMBERMASK & Token) >> D3DVSD_STREAMNUMBERSHIFT)

#define D3DVSD_GETSKIP( Token ) \
    ((D3DVSD_SKIPCOUNTMASK & Token) >> D3DVSD_SKIPCOUNTSHIFT)

// --------------------------------------------------------------------------------------

UINT GetSizeOfVertexElement(DECLUSAGE Type)
{
    switch(Type)
    {
	case DECLTYPE_FLOAT1:
	case DECLTYPE_UDEC3:
	case DECLTYPE_DEC3N:
	case DECLTYPE_FLOAT16_2:
        return sizeof(float);
    case DECLTYPE_FLOAT2:
	case DECLTYPE_FLOAT16_4:
        return sizeof(float)*2;
    case DECLTYPE_FLOAT3:
        return sizeof(float)*3;
	case DECLTYPE_FLOAT4:
        return sizeof(float)*4;
	case DECLTYPE_D3DCOLOR:
    case DECLTYPE_UBYTE4:
	case DECLTYPE_UBYTE4N:
        return sizeof(BYTE)*4;
	case DECLTYPE_SHORT2:
	case DECLTYPE_SHORT2N:
	case DECLTYPE_USHORT2N:
        return sizeof(short)*2;
    case DECLTYPE_SHORT4:
	case DECLTYPE_SHORT4N:
	case DECLTYPE_USHORT4N:
        return sizeof(short)*4;
	default:
		return 0;
    }
}

BOOL FindVertexElementOffset(VERTEXELEMENT *pDecl, DECLUSAGE Usage, UINT uUsageIndex, UINT *pOffset)
{
	UINT uIndex = 0;

	while(pDecl[uIndex].Stream != 0xFF)
    {
		if(pDecl[uIndex].Usage == Usage && pDecl[uIndex].UsageIndex == uUsageIndex)
		{
			*pOffset = pDecl[uIndex].Offset;
			return TRUE;
		}
		uIndex++;
	}
	return FALSE;
}

// --------------------------------------------------------------------------------------

void DecodeRegister(DWORD dwReg, DECLUSAGE *pUsage, BYTE *pUsageIndex)
{
    DECLUSAGE Usage;
    BYTE UsageIndex;

    if(dwReg < 5)
    {
        Usage = (DECLUSAGE)dwReg;
        UsageIndex = 0;
    }
    else if (dwReg == 5)
    {
        Usage = DECLUSAGE_COLOR;
        UsageIndex = 0;
    }
    else if (dwReg == 6)
    {
        Usage = DECLUSAGE_COLOR;
        UsageIndex = 1;
    }
    else if(dwReg >= 7 && dwReg <= 14)
    {
        Usage = DECLUSAGE_TEXCOORD;
        UsageIndex = (BYTE)(dwReg - 7);
    }
    else if(dwReg == 15)
    {
        Usage = DECLUSAGE_POSITION;
        UsageIndex = 1;
    }
    else if(dwReg == 16)
    {
        Usage = DECLUSAGE_NORMAL;
        UsageIndex = 1;
    }

    if(pUsage)
        *pUsage = Usage;
    if(pUsageIndex)
        *pUsageIndex = UsageIndex;
}

// --------------------------------------------------------------------------------------

WORD GetOffset(DWORD dwReg, VERTEXELEMENT *pVert, UINT nVert)
{
    BYTE 		UsageIndex;
    DECLUSAGE 	Usage;

    DecodeRegister(dwReg, &Usage, &UsageIndex);

    for(UINT i=0; i<nVert; i++)
    {
        if(pVert[i].Usage == Usage && pVert[i].UsageIndex == UsageIndex)
        {
            return pVert[i].Offset;
        }
    }
    return 0xFFFF;
}

// --------------------------------------------------------------------------------------

HRESULT AddVESorted(VERTEXELEMENT *pList, UINT nCount, VERTEXELEMENT *pAdd)
{

    for(int i=0; i<nCount; i++)
    {
        if(pAdd->Stream <= pList[i].Stream && pAdd->Offset <= pList[i].Offset)
        {
            for(int j=nCount-1; j>=i; j--)
            {
                memcpy(pList+j+1, pList+j, sizeof(VERTEXELEMENT));
            }
            memcpy(pList+i, pAdd, sizeof(VERTEXELEMENT));
            return S_OK;
        }
    }

    // Add to tail
    memcpy(pList+nCount, pAdd, sizeof(VERTEXELEMENT));
    return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT VDecl8ToVDecl9(DWORD *pDecl, VERTEXELEMENT **ppVE)
{
    UINT nIndex = 0;
    UINT nVEIndex = 0, nVEAlloc = 10;
    VERTEXELEMENT *pVert = NULL, vTemp;
    D3DVERTEXELEMENT9 veEnd = D3DDECL_END();
    DECLMETHOD Method = DECLMETHOD_DEFAULT;
    DECLUSAGE Usage;
    BYTE UsageIndex;
    BYTE nCurrentStream = 0xFF;
    WORD nOffset = 0;
    DWORD dwReg, dwReg2, dwType;

    if(NULL == pDecl || NULL == ppVE)
        return E_FAIL;

    pVert = new VERTEXELEMENT[nVEAlloc];
    if(NULL == pVert)
        return E_OUTOFMEMORY;
    memset(pVert, 0, sizeof(VERTEXELEMENT) * nVEAlloc);

    while(pDecl[nIndex] != D3DVSD_END())
    {
        // Check if we need to grow the array.  We want to guarrantee
        // room for the terminator, so we check for size + 1.
        if(nVEIndex+1 >= nVEAlloc)
        {
            VERTEXELEMENT *pTemp = new VERTEXELEMENT[nVEAlloc * 2];
            if(NULL == pTemp)
            {
                SAFEDELETEA(pVert);
                return E_OUTOFMEMORY;
            }
            memset(pTemp, 0, sizeof(VERTEXELEMENT) * nVEAlloc * 2);
            memcpy(pTemp, pVert, sizeof(VERTEXELEMENT) * nVEAlloc);
            nVEAlloc *= 2;
            SAFEDELETEA(pVert);
            pVert = pTemp;
        }

        // Parse the DECL tokens
        switch(pDecl[nIndex] & (D3DVSD_TOKENTYPEMASK | 0x10000000))
        {
            case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAM):
                nCurrentStream = (BYTE)D3DVSD_GETSTREAM(pDecl[nIndex]);
                nOffset = 0;
                break;
            case (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAMDATA) | 0x10000000):			// SKIP
                nOffset += (WORD)(4*D3DVSD_GETSKIP(pDecl[nIndex]));
                break;
            case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_CONSTMEM):							// CONST
                nIndex += 4*((pDecl[nIndex] >> D3DVSD_CONSTCOUNTSHIFT) & 0x0f);
                break;
            case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAMDATA):							// REG
                D3DVSD_GETREG(pDecl[nIndex], dwReg, dwType);
                DecodeRegister(dwReg, &Usage, &UsageIndex);
                vTemp.Method = Method;
                vTemp.Stream = nCurrentStream;
                vTemp.Type   = (DECLTYPE)dwType;
                vTemp.Offset = nOffset;
                vTemp.UsageIndex = UsageIndex;
                vTemp.Usage   = Usage;
                nOffset += (WORD)GetSizeOfVertexElement((DECLUSAGE)vTemp.Type);
                AddVESorted(pVert, nVEIndex, &vTemp);
                nVEIndex++;
                break;
            case D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_TESSELLATOR):						// TESSNORMAL
                D3DVSD_GETTESSNORMAL(pDecl[nIndex], dwReg2, dwReg);
                DecodeRegister(dwReg, &Usage, &UsageIndex);
                vTemp.Stream = nCurrentStream;
                vTemp.Method = DECLMETHOD_CROSSUV;
                vTemp.Type   = DECLTYPE_FLOAT3;
                vTemp.Offset = GetOffset(dwReg2, pVert, nVEIndex);
                vTemp.UsageIndex = UsageIndex;
                vTemp.Usage   = Usage;
                AddVESorted(pVert, nVEIndex, &vTemp);
                nVEIndex++;
                break;
            case (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_TESSELLATOR) | 0x10000000):			// TESSUV
                D3DVSD_GETTESSUV(pDecl[nIndex], dwReg);
                DecodeRegister(dwReg, &Usage, &UsageIndex);
                vTemp.Stream = 0;
                vTemp.Method = DECLMETHOD_UV;
                vTemp.Type   = DECLTYPE_UNUSED;
                vTemp.UsageIndex = UsageIndex;
                vTemp.Usage   = Usage;
                vTemp.Offset = 0;
                AddVESorted(pVert, nVEIndex, &vTemp);
                nVEIndex++;
                break;
            case (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAM) | (D3DVSD_STREAMTESSMASK)): // STREAM_TESS
                nOffset = 0;
                break;
        }
		
        nIndex++;
    }
	
    // Append the terminator
    memcpy(pVert + nVEIndex++, &veEnd, sizeof(VERTEXELEMENT));

    *ppVE = pVert;

    return S_OK;
}

// --------------------------------------------------------------------------------------

HRESULT VS8ToVS9(TCHAR *sShaderIn, TCHAR** sShaderOut)
{
    HRESULT    hr = S_OK;
    CONST UINT uiMaxRegisters = 16;
    BOOL       bRegisters[uiMaxRegisters];
    TCHAR      sDCL[16 * 32], sTemp[32];
    UINT       uiLen       = 0,
        uiCount_Num = 0,
        uiDCL_Start = 0,
        uiPtr       = 0;
    TCHAR      sBuffer[6];
    INT        i;

    INT        nSrcIndex   = 0,
        nSrcCount   = 0,
        nVRegNum    = 0;
    BOOL       bIsMatrixOp = FALSE;
	
    ZeroMemory(bRegisters, sizeof(BOOL) * uiMaxRegisters);
    uiLen = _tcslen(sShaderIn) + 1;

    while (sShaderIn[uiPtr] != '\0')
    {
        switch (sShaderIn[uiPtr])
        {
            case ';':

                while (sShaderIn[uiPtr++] != '\n');
                break;

            case '/':

                if (sShaderIn[uiPtr + 1] == '/')   // comment "//" till end of line
                {
                    while (sShaderIn[uiPtr++] != '\n') {}
                }
                else  // comment "/* */"
                {
                    uiPtr += 2;
                    while (sShaderIn[uiPtr] != '*' || sShaderIn[uiPtr + 1] != '/')  { uiPtr += 1; }
                    uiPtr += 2;
                }

                break;

            case 'p':

                if (sShaderIn[uiPtr + 1] == 's')
                {
                    *sShaderOut = new TCHAR[uiLen];
                    CopyMemory(*sShaderOut, sShaderIn, uiLen * sizeof(TCHAR));
                    return S_OK;
                }
            
                uiPtr += 1;

                break;

            case 'v':

                uiPtr += 1;
                uiCount_Num = 0;

                // get number string
                while (_istdigit(((PTBYTE)sShaderIn)[uiPtr]) && uiCount_Num < uiLen)
                {
                    sBuffer[uiCount_Num++] = sShaderIn[uiPtr];
                    uiPtr += 1;
                }

                if (uiCount_Num > 0)  // convert string to number and record
                {
                    sBuffer[uiCount_Num] = '\0';
                    nVRegNum = _ttoi(sBuffer);

                    bRegisters[nVRegNum] = TRUE;

                    if (bIsMatrixOp == TRUE && nSrcIndex == 2)
                    {
                        for (i = 1; i < nSrcCount; ++i)
                        {
                            bRegisters[nVRegNum + i] = TRUE;
                        }

                        bIsMatrixOp = FALSE;
                        nSrcIndex   = 0;
                    }
                }
                else if (sShaderIn[uiPtr] == 's')  // vs.1.#
                {
                    while (sShaderIn[uiPtr++] != '.')     {}
                    while (sShaderIn[uiPtr++] != '.')     {}
                    while (_istspace(((PTBYTE)sShaderIn)[uiPtr++])) {}
                    while (_istdigit(((PTBYTE)sShaderIn)[uiPtr++])) {}
                    while (_istspace(((PTBYTE)sShaderIn)[uiPtr++])) {}

                    uiPtr -= 1;
                    uiDCL_Start = uiPtr;
                }

                break;

            default:

                if (!_istspace(((PTBYTE)sShaderIn)[uiPtr]))
                {
                    if (_tcsncmp(&(sShaderIn[uiPtr]), _T("m3x2"), 4) == 0)
                    {
                        bIsMatrixOp = TRUE;
                        nSrcCount   = 2;
                        nSrcIndex   = 0;
                        uiPtr += 4;
                        break;
                    }
                    else if (_tcsncmp(&(sShaderIn[uiPtr]), _T("m3x3"), 4) == 0 ||
                             _tcsncmp(&(sShaderIn[uiPtr]), _T("m4x3"), 4) == 0)
                    {
                        bIsMatrixOp = TRUE;
                        nSrcCount   = 3;
                        nSrcIndex   = 0;
                        uiPtr += 4;
                        break;
                    }
                    else if (_tcsncmp(&(sShaderIn[uiPtr]), _T("m3x4"), 4) == 0 ||
                             _tcsncmp(&(sShaderIn[uiPtr]), _T("m4x4"), 4) == 0)
                    {
                        bIsMatrixOp = TRUE;
                        nSrcCount   = 4;
                        nSrcIndex   = 0;
                        uiPtr += 4;
                        break;
                    }
                    else if (sShaderIn[uiPtr] == ',')
                    {
                        nSrcIndex += 1;
                    }
                }
                    
                uiPtr += 1;

                break;
        }
    }

    sDCL[0] = '\0';
    for (i = 0; i < 16; ++i)
    {
        if (bRegisters[i] == TRUE)
        {
            DECLUSAGE Usage;
            BYTE      UsageIndex;
            TCHAR     *sUsage = NULL;

            DecodeRegister(i, (DECLUSAGE*)&Usage, &UsageIndex);

            switch(Usage)
            {
                case DECLUSAGE_POSITION:
                    sUsage = _T("dcl_position");
                    break;
                case DECLUSAGE_COLOR:
                    sUsage = _T("dcl_color");
                    break;
                case DECLUSAGE_BLENDWEIGHT:
                    sUsage = _T("dcl_blendweight");
                    break;
                case DECLUSAGE_BLENDINDICES:
                    sUsage = _T("dcl_blendindices");
                    break;
                case DECLUSAGE_NORMAL:
                    sUsage = _T("dcl_normal");
                    break;
                case DECLUSAGE_PSIZE:
                    sUsage = _T("dcl_psize");
                    break;
                case DECLUSAGE_TEXCOORD:
                    sUsage = _T("dcl_texcoord");
                    break;
            };

            _stprintf(sTemp, _T("%s%d v%d\r\n"), sUsage, UsageIndex, i);
            _tcscat(sDCL, sTemp);
        }
    }

    *sShaderOut = NULL;
    *sShaderOut = new TCHAR[uiLen + _tcslen(sDCL) + 4];

    if (*sShaderOut == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto Exit;
    }

    _tcsncpy(*sShaderOut, sShaderIn, uiDCL_Start);     // vs.1.#
    (*sShaderOut)[uiDCL_Start] = '\0';                 // _tcsncpy() does NOT append NULL to DstString
    _tcscat (*sShaderOut, _T("\r\n"));                 // empty line
    _tcscat (*sShaderOut, sDCL);                       // dcl_* v# block
    _tcscat (*sShaderOut, _T("\r\n"));                 // empty line
    _tcscat (*sShaderOut, &(sShaderIn[uiDCL_Start]));  // rest of shader body

  Exit:

    return hr;
}

HRESULT VS9ToVS8(TCHAR *sShaderIn, TCHAR** sShaderOut)
{
    HRESULT hr = S_OK;
    TCHAR   *sShaderTemp;
    INT     nLen  = _tcslen(sShaderIn) + 1,
        uiPtr = 0,
        i     = -1;
    BOOL    bIsDCL = FALSE;

    BOOL	bProcessing = TRUE;

    *sShaderOut = NULL;
	
    sShaderTemp = NULL;
    sShaderTemp = new TCHAR[nLen];

    if (NULL == sShaderTemp)
    {
        hr = E_OUTOFMEMORY;
    }
    else
    {
        while ((sShaderIn[uiPtr] != '\0') && (TRUE==bProcessing))
        {
            switch (sShaderIn[uiPtr])
            {
                case ';':
                case '/':

                    if (sShaderIn[uiPtr + 1] == '/' || sShaderIn[uiPtr] == ';')   // comment "//" or ";" till end of line
                    {
                        while ((sShaderIn[uiPtr++] != '\n') && (sShaderIn[uiPtr] != '\0') && (i+1) < nLen) 
                        {
                            sShaderTemp[++i] = sShaderIn[uiPtr - 1]; 
                        }
                        sShaderTemp[++i] = sShaderIn[uiPtr - 1];
                    }
                    else if (sShaderIn[uiPtr + 1] == '*') // comment "/* */"
                    {
                        uiPtr += 2;
                        sShaderTemp[++i] = sShaderIn[uiPtr - 2];
                        sShaderTemp[++i] = sShaderIn[uiPtr - 1];
                        sShaderTemp[++i] = sShaderIn[uiPtr];
                        while ((sShaderIn[uiPtr] != '*' || sShaderIn[uiPtr + 1] != '/' || sShaderIn[uiPtr + 1] != 0x00) && (i+1) < nLen)
						{ 
							sShaderTemp[++i] = sShaderIn[++uiPtr]; 
						}
                        uiPtr += 2;
                        sShaderTemp[++i] = sShaderIn[uiPtr - 2];
                        sShaderTemp[++i] = sShaderIn[uiPtr - 1];
                    }

                    break;

                case 'p':

                    if (sShaderIn[uiPtr + 1] == 's')
                    {
                        *sShaderOut = new TCHAR[nLen];
					
                        if(NULL!=(*sShaderOut))
                        {
                            CopyMemory(*sShaderOut, sShaderIn, nLen * sizeof(TCHAR));
                            hr = S_OK;	
                        }
                        else
                        {
                            hr = E_OUTOFMEMORY;
                        }
                        bProcessing = FALSE;
                    }
                    else
                    {
                        sShaderTemp[++i] = 'p';

                        uiPtr += 1;
                    }

                    break;

                case 'v':

                    if (bIsDCL == FALSE)
                    {
                        sShaderTemp[++i] = 'v';
                    }

                    while (_istdigit(((PTBYTE)sShaderIn)[++uiPtr]) && (i+1) < nLen)
                    {
                        if (bIsDCL == FALSE)
                        {
                            sShaderTemp[++i] = sShaderIn[uiPtr];
                        }
                    }

                    if (bIsDCL == TRUE)
                    {
                        bIsDCL = FALSE;
                    }

                    if (sShaderIn[uiPtr] == 's')  // vs.2.0
                    {
                        while (!_istspace(((PTBYTE)sShaderIn)[uiPtr++])  && (i+1) < nLen)
                        {
                            sShaderTemp[++i] = sShaderIn[uiPtr - 1];
                        }
                    }

                    break;

                default:

                    if (!_istspace(((PTBYTE)sShaderIn)[uiPtr]))
                    {
                        if (_tcsncmp(&(sShaderIn[uiPtr]), _T("dcl"), 3) == 0)
                        {
                            bIsDCL = TRUE;
                            uiPtr += 4;
                            break;
                        }
                    }
	                   
                    if (bIsDCL == FALSE)
                    {
                        sShaderTemp[++i] = sShaderIn[uiPtr];
                    }

                    uiPtr += 1;

                    break;
            }
        }

        if( (NULL == (*sShaderOut)) && (S_OK == hr))
        {
            sShaderTemp[++i] = '\0';

            *sShaderOut = new TCHAR[_tcslen(sShaderTemp) + 1];

            if (*sShaderOut == NULL)
            {
                hr = E_OUTOFMEMORY;
            }
            else
            {
                _tcscpy(*sShaderOut, sShaderTemp);
                hr = S_OK;
            }
        }
    }

    
    delete [] sShaderTemp;
    return hr;
}

HRESULT GetVSDCLInfo(TCHAR *sShaderIn, VS_DCL_INFO pDCLInfo[16], DWORD *pdwDCLCount)
{
    CONST UINT uiMaxRegisters = 16;
    BOOL       bRegisters[uiMaxRegisters];
    UINT       uiLen       = 0,
        uiPtr       = 0,
        uiCount_Num = 0;
    TCHAR      sBuffer[6];
    INT        i,
        nSrcIndex   = 0,
        nSrcCount   = 0,
        nVRegNum    = 0;
    BOOL       bIsMatrixOp = FALSE;

    *pdwDCLCount = 0;

    ZeroMemory(bRegisters, sizeof(BOOL) * uiMaxRegisters);
    uiLen = _tcslen(sShaderIn) + 1;

    while (sShaderIn[uiPtr] != '\0')
    {
        switch (sShaderIn[uiPtr])
        {
            case '/':

                if (sShaderIn[uiPtr + 1] == '/')   // comment "//" till end of line
                {
                    while (sShaderIn[uiPtr++] != '\n') {}
                }
                else  // comment "/* */"
                {
                    uiPtr += 2;
                    while ((sShaderIn[uiPtr] != '*' || sShaderIn[uiPtr + 1] != '/'))  
					{ 
						uiPtr += 1; 
					}
                    uiPtr += 2;
                }

                break;

            case 'p':

                if (sShaderIn[uiPtr + 1] == 's')
                {
                    ZeroMemory(pDCLInfo, sizeof(VS_DCL_INFO) * 16);
                    return S_OK;
                }
            
                uiPtr += 1;

                break;

            case 'v':

                uiPtr += 1;
                uiCount_Num = 0;

                // get number string
                while (_istdigit(((PTBYTE)sShaderIn)[uiPtr]) && uiCount_Num < 6)
                {
                    sBuffer[uiCount_Num++] = sShaderIn[uiPtr];
                    uiPtr += 1;
                }

                if (uiCount_Num > 0)  // convert string to number and record
                {
                    sBuffer[uiCount_Num] = '\0';
                    nVRegNum = _ttoi(sBuffer);

                    bRegisters[nVRegNum] = TRUE;

                    if (bIsMatrixOp == TRUE && nSrcIndex == 2)
                    {
                        for (i = 1; i < nSrcCount; ++i)
                        {
                            bRegisters[nVRegNum + i] = TRUE;
                        }

                        bIsMatrixOp = FALSE;
                        nSrcIndex   = 0;
                    }
                }

                break;

            default:

                if (!_istspace(((PTBYTE)sShaderIn)[uiPtr]))
                {
                    if (_tcsncmp(&(sShaderIn[uiPtr]), _T("m3x2"), 4) == 0)
                    {
                        bIsMatrixOp = TRUE;
                        nSrcCount   = 2;
                        nSrcIndex   = 0;
                        uiPtr += 4;
                        break;
                    }
                    else if (_tcsncmp(&(sShaderIn[uiPtr]), _T("m3x3"), 4) == 0 ||
                             _tcsncmp(&(sShaderIn[uiPtr]), _T("m4x3"), 4) == 0)
                    {
                        bIsMatrixOp = TRUE;
                        nSrcCount   = 3;
                        nSrcIndex   = 0;
                        uiPtr += 4;
                        break;
                    }
                    else if (_tcsncmp(&(sShaderIn[uiPtr]), _T("m3x4"), 4) == 0 ||
                             _tcsncmp(&(sShaderIn[uiPtr]), _T("m4x4"), 4) == 0)
                    {
                        bIsMatrixOp = TRUE;
                        nSrcCount   = 4;
                        nSrcIndex   = 0;
                        uiPtr += 4;
                        break;
                    }
                    else if (sShaderIn[uiPtr] == ',')
                    {
                        nSrcIndex += 1;
                    }
                }
                    
                uiPtr += 1;

                break;
        }
    }

    for (i = 0; i < 16; ++i)
    {
        if (bRegisters[i] == TRUE)
        {
            DECLUSAGE Usage;
            BYTE      UsageIndex;
            TCHAR     *sUsage;

            DecodeRegister(i, (DECLUSAGE*)&Usage, &UsageIndex);

            switch(Usage)
            {
                case DECLUSAGE_POSITION:
                    sUsage = _T("dcl_position");
                    break;
                case DECLUSAGE_BLENDWEIGHT:
                    sUsage = _T("dcl_blendweight");
                    break;
                case DECLUSAGE_BLENDINDICES:
                    sUsage = _T("dcl_blendindices");
                    break;
                case DECLUSAGE_NORMAL:
                    sUsage = _T("dcl_normal");
                    break;
                case DECLUSAGE_PSIZE:
                    sUsage = _T("dcl_psize");
                    break;
                case DECLUSAGE_TEXCOORD:
                    sUsage = _T("dcl_texcoord");
                    break;
            };

            pDCLInfo[i].bIsUsed      = TRUE;
            pDCLInfo[i].Usage        = Usage;
            pDCLInfo[i].dwUsageIndex = UsageIndex;
            pDCLInfo[i].dwRegNum     = i;

            *pdwDCLCount += 1;
        }
        else
        {
            pDCLInfo[i].bIsUsed = FALSE;
        }
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: D3DXQuaternionUnitAxisToUnitAxis2
// Desc: Axis to axis quaternion double angle (no normalization)
//       Takes two points on unit sphere an angle THETA apart, returns
//       quaternion that represents a rotation around cross product by 2*THETA.
//-----------------------------------------------------------------------------
inline D3DXQUATERNION* WINAPI D3DXQuaternionUnitAxisToUnitAxis2
( D3DXQUATERNION *pOut, const D3DXVECTOR3 *pvFrom, const D3DXVECTOR3 *pvTo)
{
    D3DXVECTOR3 vAxis;
    D3DXVec3Cross(&vAxis, pvFrom, pvTo);    // proportional to sin(theta)
    pOut->x = vAxis.x;
    pOut->y = vAxis.y;
    pOut->z = vAxis.z;
    pOut->w = D3DXVec3Dot( pvFrom, pvTo );
    return pOut;
}




//-----------------------------------------------------------------------------
// Name: D3DXQuaternionAxisToAxis
// Desc: Axis to axis quaternion 
//       Takes two points on unit sphere an angle THETA apart, returns
//       quaternion that represents a rotation around cross product by theta.
//-----------------------------------------------------------------------------
inline D3DXQUATERNION* WINAPI D3DXQuaternionAxisToAxis
( D3DXQUATERNION *pOut, const D3DXVECTOR3 *pvFrom, const D3DXVECTOR3 *pvTo)
{
    D3DXVECTOR3 vA, vB;
    D3DXVec3Normalize(&vA, pvFrom);
    D3DXVec3Normalize(&vB, pvTo);
    D3DXVECTOR3 vHalf(vA + vB);
    D3DXVec3Normalize(&vHalf, &vHalf);
    return D3DXQuaternionUnitAxisToUnitAxis2(pOut, &vA, &vHalf);
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DArcBall::CD3DArcBall()
{
    m_qDown = new D3DXQUATERNION;
    m_qNow = new D3DXQUATERNION;
    m_matRotation = new D3DXMATRIX;
    m_matRotationDelta = new D3DXMATRIX;
    m_matTranslation = new D3DXMATRIX;
    m_matTranslationDelta = new D3DXMATRIX;

    Reset();
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CD3DArcBall::~CD3DArcBall()
{
    SAFEDELETE(m_qDown);
    SAFEDELETE(m_qNow);
    SAFEDELETE(m_matRotation);
    SAFEDELETE(m_matRotationDelta);
    SAFEDELETE(m_matTranslation);
    SAFEDELETE(m_matTranslationDelta);
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
VOID CD3DArcBall::Reset()
{
    D3DXQuaternionIdentity( m_qDown );
    D3DXQuaternionIdentity( m_qNow );
    D3DXMatrixIdentity( m_matRotation );
    D3DXMatrixIdentity( m_matRotationDelta );
    D3DXMatrixIdentity( m_matTranslation );
    D3DXMatrixIdentity( m_matTranslationDelta );
    m_bDrag = FALSE;
    m_fRadiusTranslation = 1.0f;
    m_bRightHanded = FALSE;
}

//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
VOID CD3DArcBall::SetWindow( int iWidth, int iHeight, float fRadius )
{
    // Set ArcBall info
    m_iWidth  = iWidth;
    m_iHeight = iHeight;
    m_fRadius = fRadius;
}


void CD3DArcBall::SetRotationMatrix(D3DXMATRIX *pMat)    {
    D3DXMatrixIdentity(m_matRotation);
    D3DXMatrixMultiply(m_matRotation, m_matRotation, pMat);
    D3DXQuaternionRotationMatrix(m_qNow, pMat);
}
void CD3DArcBall::SetTranslationMatrix(D3DXMATRIX *pMat) {
    D3DXMatrixIdentity(m_matTranslation);
    D3DXMatrixMultiply(m_matTranslation, m_matTranslation, pMat);
}



//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
void CD3DArcBall::ScreenToVector( int sx, int sy, D3DXVECTOR3 *pVecOut)
{
    // Scale to screen
    FLOAT x   = -(sx - m_iWidth/2)  / (m_fRadius*m_iWidth/2);
    FLOAT y   =  (sy - m_iHeight/2) / (m_fRadius*m_iHeight/2);

    if( m_bRightHanded )
    {
        x = -x;
        y = -y;
    }

    FLOAT z   = 0.0f;
    FLOAT mag = x*x + y*y;

    if( mag > 1.0f )
    {
        FLOAT scale = 1.0f/sqrtf(mag);
        x *= scale;
        y *= scale;
    }
    else
        z = sqrtf( 1.0f - mag );

    // Return vector
    *pVecOut  = D3DXVECTOR3( x, y, z );
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
VOID CD3DArcBall::SetRadius( FLOAT fRadius )
{
    m_fRadiusTranslation = fRadius;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
LRESULT CD3DArcBall::HandleMouseMessages( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    static int         iCurMouseX;      // Saved mouse position
    static int         iCurMouseY;
    static D3DXVECTOR3 s_vDown;         // Button down vector

    // Current mouse position
    int iMouseX = LOWORD(lParam);
    int iMouseY = HIWORD(lParam);

    switch( uMsg )
    {
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            // Store off the position of the cursor when the button is pressed
            iCurMouseX = iMouseX;
            iCurMouseY = iMouseY;
            return TRUE;

        case WM_LBUTTONDOWN:
            // Start drag mode
            m_bDrag = TRUE;
            ScreenToVector( iMouseX, iMouseY, &s_vDown);
            *m_qDown = *m_qNow;
            return TRUE;

        case WM_LBUTTONUP:
            // End drag mode
            m_bDrag = FALSE;
            return TRUE;

        case WM_MOUSEMOVE:
            // Drag object
            if( MK_LBUTTON&wParam )
            {
                if( m_bDrag )
                {
                    // recompute m_qNow
                    D3DXVECTOR3 vCur;
                    ScreenToVector( iMouseX, iMouseY, &vCur );
                    D3DXQUATERNION qAxisToAxis;
                    D3DXQuaternionAxisToAxis(&qAxisToAxis, &s_vDown, &vCur);
                    *m_qNow = *m_qDown;
                    *m_qNow *= qAxisToAxis;
                    D3DXMatrixRotationQuaternion(m_matRotationDelta, &qAxisToAxis);
                }
                else
                    D3DXMatrixIdentity(m_matRotationDelta);
                D3DXMatrixRotationQuaternion(m_matRotation, m_qNow);
                m_bDrag = TRUE;
            }
            else if( (MK_RBUTTON&wParam) || (MK_MBUTTON&wParam) )
            {
                // Normalize based on size of window and bounding sphere radius
                FLOAT fDeltaX = ( iCurMouseX-iMouseX ) * m_fRadiusTranslation / m_iWidth;
                FLOAT fDeltaY = ( iCurMouseY-iMouseY ) * m_fRadiusTranslation / m_iHeight;

                if( wParam & MK_RBUTTON )
                {
                    D3DXMatrixTranslation( m_matTranslationDelta, -2*fDeltaX, 2*fDeltaY, 0.0f );
                    D3DXMatrixMultiply( m_matTranslation, m_matTranslation, m_matTranslationDelta );
                }
                else  // wParam & MK_MBUTTON
                {
                    D3DXMatrixTranslation( m_matTranslationDelta, 0.0f, 0.0f, 5*fDeltaY );
                    D3DXMatrixMultiply( m_matTranslation, m_matTranslation, m_matTranslationDelta );
                }

                // Store mouse coordinate
                iCurMouseX = iMouseX;
                iCurMouseY = iMouseY;
            }
            return TRUE;
    }

    return FALSE;
}

//
// --------------------------------------------------------------------------------------

void ExpandDDPF(FMT d3dFormat, PIXELFORMAT *pddpfFormat)
{
	switch(d3dFormat)
	{
		case FMT_DXT1:
        case FMT_DXT2:
        case FMT_DXT3:
        case FMT_DXT4:
        case FMT_DXT5:
			pddpfFormat->dwRowsPerByteSequence = 4;
            break;
		default:
			pddpfFormat->dwRowsPerByteSequence = 1;
			break;
	}
}

//
// --------------------------------------------------------------------------------------

bool d3df2ddpf(D3DFORMAT d3dfFormat, PIXELFORMAT *pddpfFormat)
{
    FMT fmt = (FMT)d3dfFormat;
    ZeroMemory(pddpfFormat, sizeof(PIXELFORMAT));
    pddpfFormat->dwSize = 32;	// Set the size to the DDPIXELFORMAT, these will pass directly to DDRAW
    switch(fmt)
    {
        // Z / Stencil buffer formats
        case FMT_D16:
            pddpfFormat->dwFlags = PF_ZBUFFER;
            pddpfFormat->dwZBufferBitDepth = 16;
            pddpfFormat->dwZBitMask = 0x0000ffff;
            break;
        case FMT_D15S1:
            pddpfFormat->dwFlags = PF_ZBUFFER | PF_STENCILBUFFER;
            pddpfFormat->dwZBufferBitDepth = 16;
            pddpfFormat->dwStencilBitDepth = 1;
            pddpfFormat->dwZBitMask =       0x0000fffe;
            pddpfFormat->dwStencilBitMask = 0x00000001;
            break;
        case FMT_D24X8:
            pddpfFormat->dwFlags = PF_ZBUFFER;
            pddpfFormat->dwZBufferBitDepth = 32;
            pddpfFormat->dwZBitMask = 0xffffff00;
            break;
        case FMT_D24S8:
            pddpfFormat->dwFlags = PF_ZBUFFER | PF_STENCILBUFFER;
            pddpfFormat->dwZBufferBitDepth = 32;
            pddpfFormat->dwStencilBitDepth = 8;
            pddpfFormat->dwZBitMask = 0xffffff00;
            pddpfFormat->dwStencilBitMask = 0x000000ff;
            break;
        case FMT_D24FS8:
            pddpfFormat->dwFlags = PF_ZBUFFER | PF_STENCILBUFFER;
            pddpfFormat->dwZBufferBitDepth = 32;
            pddpfFormat->dwStencilBitDepth = 8;
            pddpfFormat->dwZBitMask = 0;
            pddpfFormat->dwStencilBitMask = 0x000000ff;
            break;
        case FMT_D24X4S4:
            pddpfFormat->dwFlags = PF_ZBUFFER | PF_STENCILBUFFER;
            pddpfFormat->dwZBufferBitDepth = 32;
            pddpfFormat->dwStencilBitDepth = 4;
            pddpfFormat->dwZBitMask = 0xffffff00;
            pddpfFormat->dwStencilBitMask = 0x0000000f;
            break;
        case FMT_D32:
            pddpfFormat->dwFlags = PF_ZBUFFER;
            pddpfFormat->dwZBufferBitDepth = 32;
            pddpfFormat->dwZBitMask = 0xffffffff;
            break;
        case FMT_D16_LOCKABLE:
            pddpfFormat->dwFlags = PF_ZBUFFER | PF_LOCKABLE;
            pddpfFormat->dwZBufferBitDepth = 16;
            pddpfFormat->dwZBitMask = 0x0000ffff;
            break;
        case FMT_D32F_LOCKABLE:
            pddpfFormat->dwFlags = PF_ZBUFFER | PF_LOCKABLE;
            pddpfFormat->dwZBufferBitDepth = 32;
            pddpfFormat->dwZBitMask = 0;
            break;
	case FMT_S1D15:
            pddpfFormat->dwFlags = PF_ZBUFFER | PF_STENCILBUFFER;
            pddpfFormat->dwZBufferBitDepth = 16;
            pddpfFormat->dwStencilBitDepth = 1;
            pddpfFormat->dwZBitMask =       0x00007fff;
            pddpfFormat->dwStencilBitMask = 0x00008000;
            break;
	case FMT_S8D24:
            pddpfFormat->dwFlags = PF_ZBUFFER | PF_STENCILBUFFER;
            pddpfFormat->dwZBufferBitDepth = 32;
            pddpfFormat->dwStencilBitDepth = 8;
            pddpfFormat->dwZBitMask = 0x00ffffff;
            pddpfFormat->dwStencilBitMask = 0xff000000;
            break;
	case FMT_X8D24:
            pddpfFormat->dwFlags = PF_ZBUFFER | PF_STENCILBUFFER;
            pddpfFormat->dwZBufferBitDepth = 32;
            pddpfFormat->dwStencilBitDepth = 8;
            pddpfFormat->dwZBitMask = 0x00ffffff;
            pddpfFormat->dwStencilBitMask = 0x00000000;
            break;
	case FMT_X4S4D24:
            pddpfFormat->dwFlags = PF_ZBUFFER | PF_STENCILBUFFER;
            pddpfFormat->dwZBufferBitDepth = 32;
            pddpfFormat->dwStencilBitDepth = 4;
            pddpfFormat->dwZBitMask = 0x00ffffff;
            pddpfFormat->dwStencilBitMask = 0x0f000000;
            break;
    
            // Texture formats
        case FMT_R8G8B8:
            pddpfFormat->dwFlags = PF_RGB;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 24;
            pddpfFormat->dwRBitMask = 0x00ff0000;
            pddpfFormat->dwGBitMask = 0x0000ff00;
            pddpfFormat->dwBBitMask = 0x000000ff;
            pddpfFormat->dwRGBAlphaBitMask = 0;
            break;
        case FMT_A8R8G8B8:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwRBitMask = 0x00ff0000;
            pddpfFormat->dwGBitMask = 0x0000ff00;
            pddpfFormat->dwBBitMask = 0x000000ff;
            pddpfFormat->dwRGBAlphaBitMask = 0xff000000;
            break;
        case FMT_X8R8G8B8:
            pddpfFormat->dwFlags = PF_RGB;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwRBitMask = 0x00ff0000;
            pddpfFormat->dwGBitMask = 0x0000ff00;
            pddpfFormat->dwBBitMask = 0x000000ff;
            pddpfFormat->dwRGBAlphaBitMask = 0;
            break;
        case FMT_R5G6B5:
            pddpfFormat->dwFlags = PF_RGB;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwRBitMask = 0xf800;
            pddpfFormat->dwGBitMask = 0x07e0;
            pddpfFormat->dwBBitMask = 0x001f;
            pddpfFormat->dwRGBAlphaBitMask = 0;
            break;
        case FMT_X1R5G5B5:
            pddpfFormat->dwFlags = PF_RGB;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwRBitMask = 0x7c00;
            pddpfFormat->dwGBitMask = 0x03e0;
            pddpfFormat->dwBBitMask = 0x001f;
            pddpfFormat->dwRGBAlphaBitMask = 0;
            break;
        case FMT_A1R5G5B5:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwRBitMask = 0x7c00;
            pddpfFormat->dwGBitMask = 0x03e0;
            pddpfFormat->dwBBitMask = 0x001f;
            pddpfFormat->dwRGBAlphaBitMask = 0x8000;
            break;
        case FMT_A4R4G4B4:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwRBitMask = 0x0f00;
            pddpfFormat->dwGBitMask = 0x00f0;
            pddpfFormat->dwBBitMask = 0x000f;
            pddpfFormat->dwRGBAlphaBitMask = 0xf000;
            break;
        case FMT_R3G3B2:
            pddpfFormat->dwFlags = PF_RGB;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 8;
            pddpfFormat->dwRBitMask = 0xe0;
            pddpfFormat->dwGBitMask = 0x1c;
            pddpfFormat->dwBBitMask = 0x03;
            pddpfFormat->dwRGBAlphaBitMask = 0;
            break;
        case FMT_A8:
            pddpfFormat->dwFlags = PF_ALPHA;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwAlphaBitDepth = 8;
            pddpfFormat->dwRBitMask = 0;
            pddpfFormat->dwGBitMask = 0;
            pddpfFormat->dwBBitMask = 0;
            pddpfFormat->dwRGBAlphaBitMask = 0xff;
            break;
        case FMT_A1:
            pddpfFormat->dwFlags = PF_ALPHA;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 1;
            pddpfFormat->dwRBitMask = 0;
            pddpfFormat->dwGBitMask = 0;
            pddpfFormat->dwBBitMask = 0;
            pddpfFormat->dwRGBAlphaBitMask = 0x1;
            break;
        case FMT_A8R3G3B2:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwRBitMask = 0x00e0;
            pddpfFormat->dwGBitMask = 0x001c;
            pddpfFormat->dwBBitMask = 0x0003;
            pddpfFormat->dwRGBAlphaBitMask = 0xff00;
            break;
        case FMT_X4R4G4B4:
            pddpfFormat->dwFlags = PF_RGB;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwRBitMask = 0x0f00;
            pddpfFormat->dwGBitMask = 0x00f0;
            pddpfFormat->dwBBitMask = 0x000f;
            pddpfFormat->dwRGBAlphaBitMask = 0;
            break;
        case FMT_A2B10G10R10:
		case FMT_A2B10G10R10_XR_BIAS:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwRBitMask = 0x000003ff;
            pddpfFormat->dwGBitMask = 0x000ffc00;
            pddpfFormat->dwBBitMask = 0x3ff00000;
            pddpfFormat->dwRGBAlphaBitMask = 0xc0000000;
            break;
        case FMT_A8B8G8R8:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwRBitMask = 0x000000ff;
            pddpfFormat->dwGBitMask = 0x0000ff00;
            pddpfFormat->dwBBitMask = 0x00ff0000;
            pddpfFormat->dwRGBAlphaBitMask = 0xff000000;
            break;
        case FMT_X8B8G8R8:
            pddpfFormat->dwFlags = PF_RGB;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwRBitMask = 0x000000ff;
            pddpfFormat->dwGBitMask = 0x0000ff00;
            pddpfFormat->dwBBitMask = 0x00ff0000;
            pddpfFormat->dwRGBAlphaBitMask = 0;
            break;
        case FMT_G16R16:
            pddpfFormat->dwFlags = PF_RGB;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwRBitMask = 0x0000ffff;
            pddpfFormat->dwGBitMask = 0xffff0000;
            pddpfFormat->dwBBitMask = 0;
            pddpfFormat->dwRGBAlphaBitMask = 0;
            break;
        case FMT_A2R10G10B10:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwRBitMask = 0x3ff00000;
            pddpfFormat->dwGBitMask = 0x000ffc00;
            pddpfFormat->dwBBitMask = 0x000003ff;
            pddpfFormat->dwRGBAlphaBitMask = 0xc0000000;
            break;
        case FMT_A16B16G16R16:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 64;
            pddpfFormat->dwRBitMask = 0x0;
            pddpfFormat->dwGBitMask = 0x0;
            pddpfFormat->dwBBitMask = 0x0;
            pddpfFormat->dwRGBAlphaBitMask = 0x0;
            break;
        case FMT_R16F:
            pddpfFormat->dwFlags = PF_RGB | PF_FLOATTEX;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwRBitMask = 0x0;
            pddpfFormat->dwGBitMask = 0x0;
            pddpfFormat->dwBBitMask = 0x0;
            pddpfFormat->dwRGBAlphaBitMask = 0x0;
            break;
		case FMT_G16R16F:
            pddpfFormat->dwFlags = PF_RGB | PF_FLOATTEX;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwRBitMask = 0x0;
            pddpfFormat->dwGBitMask = 0x0;
            pddpfFormat->dwBBitMask = 0x0;
            pddpfFormat->dwRGBAlphaBitMask = 0x0;
            break;
		case FMT_A16B16G16R16F:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS | PF_FLOATTEX;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 64;
            pddpfFormat->dwRBitMask = 0x0;
            pddpfFormat->dwGBitMask = 0x0;
            pddpfFormat->dwBBitMask = 0x0;
            pddpfFormat->dwRGBAlphaBitMask = 0x0;
            break;
        case FMT_R32F:
            pddpfFormat->dwFlags = PF_RGB | PF_FLOATTEX;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwRBitMask = 0x0;
            pddpfFormat->dwGBitMask = 0x0;
            pddpfFormat->dwBBitMask = 0x0;
            pddpfFormat->dwRGBAlphaBitMask = 0x0;
            break;
		case FMT_G32R32F:
            pddpfFormat->dwFlags = PF_RGB | PF_FLOATTEX;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 64;
            pddpfFormat->dwRBitMask = 0x0;
            pddpfFormat->dwGBitMask = 0x0;
            pddpfFormat->dwBBitMask = 0x0;
            pddpfFormat->dwRGBAlphaBitMask = 0x0;
            break;
		case FMT_A32B32G32R32F:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS | PF_FLOATTEX;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 128;
            pddpfFormat->dwRBitMask = 0x0;
            pddpfFormat->dwGBitMask = 0x0;
            pddpfFormat->dwBBitMask = 0x0;
            pddpfFormat->dwRGBAlphaBitMask = 0x0;
            break;
        case FMT_A8P8:
            pddpfFormat->dwFlags = PF_RGB | PF_ALPHAPIXELS | PF_PALETTEINDEXED8;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwRBitMask = 0;
            pddpfFormat->dwGBitMask = 0;
            pddpfFormat->dwBBitMask = 0;
            pddpfFormat->dwRGBAlphaBitMask = 0xff00;
            break;
        case FMT_P8:
            pddpfFormat->dwFlags = PF_RGB | PF_PALETTEINDEXED8;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 8;
            pddpfFormat->dwRBitMask = 0;
            pddpfFormat->dwGBitMask = 0;
            pddpfFormat->dwBBitMask = 0;
            pddpfFormat->dwRGBAlphaBitMask = 0;
            break;
        case FMT_P4:
            pddpfFormat->dwFlags = PF_RGB | PF_PALETTEINDEXED4;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwRGBBitCount = 4;
            pddpfFormat->dwRBitMask = 0;
            pddpfFormat->dwGBitMask = 0;
            pddpfFormat->dwBBitMask = 0;
            pddpfFormat->dwRGBAlphaBitMask = 0;
            break;

        case FMT_L8:
            pddpfFormat->dwFlags = PF_LUMINANCE | PF_DMAP;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwLuminanceBitCount = 8;
            pddpfFormat->dwLuminanceBitMask = 0xff;
            break;
        case FMT_A8L8:
            pddpfFormat->dwFlags = PF_LUMINANCE | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwLuminanceBitCount = 16;
            pddpfFormat->dwLuminanceBitMask = 0x00ff;
            pddpfFormat->dwLuminanceAlphaBitMask = 0xff00;
            break;
        case FMT_A4L4:
            pddpfFormat->dwFlags = PF_LUMINANCE | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwLuminanceBitCount = 8;
            pddpfFormat->dwLuminanceBitMask = 0x0f;
            pddpfFormat->dwLuminanceAlphaBitMask = 0xf0;
            break;
        case FMT_L16:
            pddpfFormat->dwFlags = PF_LUMINANCE | PF_DMAP;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwLuminanceBitCount = 16;
            pddpfFormat->dwLuminanceBitMask = 0xffff;
            break;

        case FMT_V8U8:
            pddpfFormat->dwFlags = PF_BUMPDUDV;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwBumpBitCount = 16;
            pddpfFormat->dwBumpDuBitMask = 0x00ff;
            pddpfFormat->dwBumpDvBitMask = 0xff00;
            break;
	case FMT_CxV8U8:
            pddpfFormat->dwFlags = PF_BUMPDUDV;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwBumpBitCount = 16;
            pddpfFormat->dwBumpDuBitMask = 0x00ff;
            pddpfFormat->dwBumpDvBitMask = 0xff00;
            break;
        case FMT_L6V5U5:
            pddpfFormat->dwFlags = PF_BUMPDUDV | PF_BUMPLUMINANCE;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwBumpBitCount = 16;
            pddpfFormat->dwBumpDuBitMask = 0x001f;
            pddpfFormat->dwBumpDvBitMask = 0x03e0;
            pddpfFormat->dwBumpLuminanceBitMask = 0xfc00;
            break;
        case FMT_X8L8V8U8:
            pddpfFormat->dwFlags = PF_BUMPDUDV | PF_BUMPLUMINANCE;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwBumpBitCount = 32;
            pddpfFormat->dwBumpDuBitMask = 0x0000ff;
            pddpfFormat->dwBumpDvBitMask = 0x00ff00;
            pddpfFormat->dwBumpLuminanceBitMask = 0xff0000;
            break;
        case FMT_Q8W8V8U8:
            pddpfFormat->dwFlags = PF_BUMPDUDV;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwBumpBitCount = 32;
            pddpfFormat->dwBumpDuBitMask = 0x0000ff;
            pddpfFormat->dwBumpDvBitMask = 0x00ff00;
            pddpfFormat->dwBumpLuminanceBitMask = 0xff0000;
            pddpfFormat->dwLuminanceAlphaBitMask = 0xff000000;
            break;
        case FMT_V16U16:
            pddpfFormat->dwFlags = PF_BUMPDUDV;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwBumpBitCount = 32;
            pddpfFormat->dwBumpDuBitMask = 0x0000ffff;
            pddpfFormat->dwBumpDvBitMask = 0xffff0000;
            break;
        case FMT_W11V11U10:
            pddpfFormat->dwFlags = PF_BUMPDUDV;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwBumpBitCount = 32;
            pddpfFormat->dwBumpDuBitMask = 0x000003ff;
            pddpfFormat->dwBumpDvBitMask = 0x001ffc00;
            pddpfFormat->dwBumpLuminanceBitMask = 0xffe00000;
            break;
        case FMT_W10V11U11:
            pddpfFormat->dwFlags = PF_BUMPDUDV;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwBumpBitCount = 32;
            pddpfFormat->dwBumpDuBitMask = 0x000007ff;
            pddpfFormat->dwBumpDvBitMask = 0x003ff800;
            pddpfFormat->dwBumpLuminanceBitMask = 0xffc00000;
            break;
        case FMT_A2W10V10U10:
            pddpfFormat->dwFlags = PF_BUMPDUDV | PF_ALPHAPIXELS;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwBumpBitCount = 32;
            pddpfFormat->dwBumpDuBitMask = 0x000003ff;
            pddpfFormat->dwBumpDvBitMask = 0x000ffc00;
            pddpfFormat->dwBumpLuminanceBitMask = 0x3ff00000;
            pddpfFormat->dwLuminanceAlphaBitMask = 0xc0000000;
            break;
        case FMT_Q16W16V16U16:
            pddpfFormat->dwFlags = PF_BUMPDUDV;
            pddpfFormat->dwFourCC = 0;
            pddpfFormat->dwBumpBitCount = 64;
            pddpfFormat->dwBumpDuBitMask = 0x0;
            pddpfFormat->dwBumpDvBitMask = 0x0;
            pddpfFormat->dwBumpLuminanceBitMask = 0x0;
            pddpfFormat->dwLuminanceAlphaBitMask = 0x0;
            break;

	case FMT_UYVY:
			pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwFlags = PF_FOURCC | PF_YUV;
            pddpfFormat->dwFourCC = MAKEFOURCC('U', 'Y', 'V', 'Y');
            break;
	case FMT_R8G8_B8G8:
			pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwFlags = PF_FOURCC;
            pddpfFormat->dwFourCC = MAKEFOURCC('R', 'G', 'B', 'G');
            break;
        case FMT_YUY2:
			pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwFlags = PF_FOURCC | PF_YUV;
            pddpfFormat->dwFourCC = MAKEFOURCC('Y', 'U', 'Y', '2');
            break;
	case FMT_G8R8_G8B8:
			pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwFlags = PF_FOURCC;
            pddpfFormat->dwFourCC = MAKEFOURCC('G', 'R', 'G', 'B');
            break;
        case FMT_DXT1:
			pddpfFormat->dwRGBBitCount = 16;
            pddpfFormat->dwFlags = PF_FOURCC | PF_DXTN;
            pddpfFormat->dwFourCC = MAKEFOURCC('D', 'X', 'T', '1');
            break;
        case FMT_DXT2:
			pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwFlags = PF_FOURCC | PF_DXTN;
            pddpfFormat->dwFourCC = MAKEFOURCC('D', 'X', 'T', '2');
            break;
        case FMT_DXT3:
			pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwFlags = PF_FOURCC | PF_DXTN;
            pddpfFormat->dwFourCC = MAKEFOURCC('D', 'X', 'T', '3');
            break;
        case FMT_DXT4:
			pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwFlags = PF_FOURCC | PF_DXTN;
            pddpfFormat->dwFourCC = MAKEFOURCC('D', 'X', 'T', '4');
            break;
        case FMT_DXT5:
			pddpfFormat->dwRGBBitCount = 32;
            pddpfFormat->dwFlags = PF_FOURCC | PF_DXTN;
            pddpfFormat->dwFourCC = MAKEFOURCC('D', 'X', 'T', '5');
            break;

        case FMT_MULTI2_ARGB8:
			pddpfFormat->dwRGBBitCount = 64;
            pddpfFormat->dwFlags = PF_FOURCC | PF_METN;
            pddpfFormat->dwFourCC = MAKEFOURCC('M', 'E', 'T', '1');
            break;
		
            // Unknown format
        default:
            return false;
    }

	ExpandDDPF(fmt, pddpfFormat);
    return true;
}

// -------------------------------------------------------------------------------------

bool FormatToMask(FORMAT *pFmt, MASK *pMask)
{

    if(NULL == pMask)
        return false;

    memset(pMask, 0, sizeof(MASK));

    // Compute masks for RGB surfaces
 
    if (0 != (pFmt->ddpfPixelFormat.dwFlags & (PF_RGB | PF_LUMINANCE | PF_BUMPDUDV)) && 0 == (pFmt->ddpfPixelFormat.dwFlags & (PF_PALETTEINDEXED1 | PF_PALETTEINDEXED2 | PF_PALETTEINDEXED4 | PF_PALETTEINDEXED8)))
    {
        int s;
        ULONG m;

        // We have a non palettized surface. Compute the RGB masks to use.

        if (0 != pFmt->ddpfPixelFormat.dwRBitMask)
        { 
            for (s = 0, m = pFmt->ddpfPixelFormat.dwRBitMask; !(m & 1) && m; s++, m >>= 1);

            pMask->dwRedMask = m;
            pMask->nRedShift = s;

            for (pMask->nRedBits = 0; m & 1; pMask->nRedBits++, m >>= 1);
        }

        if (0 != pFmt->ddpfPixelFormat.dwGBitMask)
        {
            for (s = 0, m = pFmt->ddpfPixelFormat.dwGBitMask; !(m & 1) && m; s++, m >>= 1);

            pMask->dwGreenMask = m;
            pMask->nGreenShift = s;

            for (pMask->nGreenBits = 0; m & 1; pMask->nGreenBits++, m >>= 1);
        }

        if (0 != pFmt->ddpfPixelFormat.dwBBitMask)
        {
            for (s = 0, m = pFmt->ddpfPixelFormat.dwBBitMask; !(m & 1) && m; s++, m >>= 1);

            pMask->dwBlueMask = m;
            pMask->nBlueShift = s;

            for (pMask->nBlueBits = 0; m & 1; pMask->nBlueBits++, m >>= 1);
        }

        if (0 != pFmt->ddpfPixelFormat.dwRGBAlphaBitMask)
        {
            for (s = 0, m = pFmt->ddpfPixelFormat.dwRGBAlphaBitMask; !(m & 1) && m; s++, m >>= 1);

            pMask->dwAlphaMask = m;
            pMask->nAlphaShift = s;

            for (pMask->nAlphaBits = 0; m & 1; pMask->nAlphaBits++, m >>= 1);
        }
    }
	
    return true;
}

// -------------------------------------------------------------------------------------

DWORD ReadColor(PVOID * ppSurfacePtr, FORMAT *pFmt, MASK *pMask, bool bInc)
{
    DWORD dwColor = 0;

    if (ppSurfacePtr == NULL || pFmt == NULL || pMask == NULL)
    {
        return 0;
    }
    
    FColor fTemp;

    fTemp.ConvertFrom((FMT)pFmt->d3dfFormat, (char*)(*ppSurfacePtr), FALSE, ELEMENTINDEX_0);

    if (bInc)
        *((BYTE**)ppSurfacePtr) += (pFmt->ddpfPixelFormat.dwRGBBitCount / 8);

    return (UINT32)fTemp;
}

// -------------------------------------------------------------------------------------

DWORD ColorConvert(DWORD dwSource, LPMASK pMask)
{
    DWORD   dwRed = RGBA_GETRED(dwSource);
    DWORD   dwGreen = RGBA_GETGREEN(dwSource);
    DWORD   dwBlue = RGBA_GETBLUE(dwSource);
    DWORD   dwAlpha = RGBA_GETALPHA(dwSource);

    // Special case the "large bit count" formats
    if (pMask->nRedBits > 8)
        dwRed = (DWORD)(dwRed/255.0f * ((2 << (pMask->nRedBits-1)) - 1));

    if (pMask->nGreenBits > 8)
        dwGreen = (DWORD)(dwGreen/255.0f * ((2 << (pMask->nGreenBits-1)) - 1));

    if (pMask->nBlueBits > 8)
        dwBlue = (DWORD)(dwBlue/255.0f * ((2 << (pMask->nBlueBits-1)) - 1));

    if (pMask->nAlphaBits > 8)
        dwAlpha = (DWORD)(dwAlpha/255.0f * ((2 << (pMask->nAlphaBits-1)) - 1));

    dwRed = dwRed & pMask->dwRedMask;
    dwGreen = dwGreen & pMask->dwGreenMask;
    dwBlue = dwBlue & pMask->dwBlueMask;
    dwAlpha = dwAlpha & pMask->dwAlphaMask;

    if (pMask->nRedShift > 0)
        dwRed <<= pMask->nRedShift;
    else
        dwRed >>= (-pMask->nRedShift);

    if (pMask->nGreenShift > 0)
        dwGreen <<= pMask->nGreenShift;
    else
        dwGreen >>= (-pMask->nGreenShift);

    if (pMask->nBlueShift > 0)
        dwBlue <<= pMask->nBlueShift;
    else
        dwBlue >>= (-pMask->nBlueShift);

    if (pMask->nAlphaShift > 0)
        dwAlpha <<= pMask->nAlphaShift;
    else
        dwAlpha >>= (-pMask->nAlphaShift);

    return dwAlpha | dwBlue | dwRed | dwGreen;
}

// -------------------------------------------------------------------------------------

void StringToFmt(LPTSTR szFmt, FMT *pFormat)
{
    if(pFormat == NULL || NULL == szFmt)
    {
        DPF(1, _T("FormatStrToFmt() - invalid params.\n"));
        return;
    }

    if(0 == _tcsicmp(szFmt, _T("D16")))
        *pFormat = FMT_D16;
    else if(0 == _tcsicmp(szFmt, _T("D15S1")))
        *pFormat = FMT_D15S1;
    else if(0 == _tcsicmp(szFmt, _T("D24X8")))
        *pFormat = FMT_D24X8;
    else if(0 == _tcsicmp(szFmt, _T("D24S8")))
        *pFormat = FMT_D24S8;
    else if(0 == _tcsicmp(szFmt, _T("D24FS8")))
        *pFormat = FMT_D24FS8;
    else if(0 == _tcsicmp(szFmt, _T("D24X4S4")))
        *pFormat = FMT_D24X4S4;
    else if(0 == _tcsicmp(szFmt, _T("D32")))
        *pFormat = FMT_D32;
    else if(0 == _tcsicmp(szFmt, _T("D16_LOCKABLE")))
        *pFormat = FMT_D16_LOCKABLE;
    else if(0 == _tcsicmp(szFmt, _T("D32F_LOCKABLE")))
        *pFormat = FMT_D32F_LOCKABLE;
    else if(0 == _tcsicmp(szFmt, _T("D32_LOCKABLE")))
        *pFormat = FMT_D32_LOCKABLE;
    else if(0 == _tcsicmp(szFmt, _T("S8_LOCKABLE")))
        *pFormat = FMT_S8_LOCKABLE;
    else if(0 == _tcsicmp(szFmt, _T("S1D15")))
        *pFormat = FMT_S1D15;
    else if(0 == _tcsicmp(szFmt, _T("S8D24")))
        *pFormat = FMT_S8D24;
    else if(0 == _tcsicmp(szFmt, _T("X8D24")))
        *pFormat = FMT_X8D24;
    else if(0 == _tcsicmp(szFmt, _T("X4S4D24")))
        *pFormat = FMT_X4S4D24;
    else if(0 == _tcsicmp(szFmt, _T("R8G8B8")))
        *pFormat = FMT_R8G8B8;
    else if(0 == _tcsicmp(szFmt, _T("A8R8G8B8")))
        *pFormat = FMT_A8R8G8B8;
    else if(0 == _tcsicmp(szFmt, _T("X8R8G8B8")))
        *pFormat = FMT_X8R8G8B8;
    else if(0 == _tcsicmp(szFmt, _T("R5G6B5")))
        *pFormat = FMT_R5G6B5;
    else if(0 == _tcsicmp(szFmt, _T("X1R5G5B5")))
        *pFormat = FMT_X1R5G5B5;
    else if(0 == _tcsicmp(szFmt, _T("A1R5G5B5")))
        *pFormat = FMT_A1R5G5B5;
    else if(0 == _tcsicmp(szFmt, _T("A4R4G4B4")))
        *pFormat = FMT_A4R4G4B4;
    else if(0 == _tcsicmp(szFmt, _T("R3G3B2")))
        *pFormat = FMT_R3G3B2;
    else if(0 == _tcsicmp(szFmt, _T("A8")))
        *pFormat = FMT_A8;
    else if(0 == _tcsicmp(szFmt, _T("A1")))
        *pFormat = FMT_A1;
    else if(0 == _tcsicmp(szFmt, _T("A8R3G3B2")))
        *pFormat = FMT_A8R3G3B2;
    else if(0 == _tcsicmp(szFmt, _T("X4R4G4B4")))
        *pFormat = FMT_X4R4G4B4;
    else if(0 == _tcsicmp(szFmt, _T("A2B10G10R10")))
        *pFormat = FMT_A2B10G10R10;
    else if(0 == _tcsicmp(szFmt, _T("A8B8G8R8")))
        *pFormat = FMT_A8B8G8R8;
    else if(0 == _tcsicmp(szFmt, _T("X8B8G8R8")))
        *pFormat = FMT_X8B8G8R8;
    else if(0 == _tcsicmp(szFmt, _T("G16R16")))
        *pFormat = FMT_G16R16;
    else if(0 == _tcsicmp(szFmt, _T("A2R10G10B10")))
        *pFormat = FMT_A2R10G10B10;
	else if(0 == _tcsicmp(szFmt, _T("A2B10G10R10_XR_BIAS")))
        *pFormat = FMT_A2B10G10R10_XR_BIAS;
    else if(0 == _tcsicmp(szFmt, _T("A16B16G16R16")))
        *pFormat = FMT_A16B16G16R16;
    else if(0 == _tcsicmp(szFmt, _T("R16F")))
        *pFormat = FMT_R16F;
    else if(0 == _tcsicmp(szFmt, _T("G16R16F")))
        *pFormat = FMT_G16R16F;
    else if(0 == _tcsicmp(szFmt, _T("A16B16G16R16F")))
        *pFormat = FMT_A16B16G16R16F;
    else if(0 == _tcsicmp(szFmt, _T("R32F")))
        *pFormat = FMT_R32F;
    else if(0 == _tcsicmp(szFmt, _T("G32R32F")))
        *pFormat = FMT_G32R32F;
    else if(0 == _tcsicmp(szFmt, _T("A32B32G32R32F")))
        *pFormat = FMT_A32B32G32R32F;
    else if(0 == _tcsicmp(szFmt, _T("A8P8")))
        *pFormat = FMT_A8P8;
    else if(0 == _tcsicmp(szFmt, _T("P4")))
        *pFormat = FMT_P4;
    else if(0 == _tcsicmp(szFmt, _T("P8")))
        *pFormat = FMT_P8;
    else if(0 == _tcsicmp(szFmt, _T("L8")))
        *pFormat = FMT_L8;
    else if(0 == _tcsicmp(szFmt, _T("A8L8")))
        *pFormat = FMT_A8L8;
    else if(0 == _tcsicmp(szFmt, _T("A4L4")))
        *pFormat = FMT_A4L4;
    else if(0 == _tcsicmp(szFmt, _T("L16")))
        *pFormat = FMT_L16;
    else if(0 == _tcsicmp(szFmt, _T("V8U8")))
        *pFormat = FMT_V8U8;
    else if(0 == _tcsicmp(szFmt, _T("CxV8U8")))
        *pFormat = FMT_CxV8U8;
    else if(0 == _tcsicmp(szFmt, _T("L6V5U5")))
        *pFormat = FMT_L6V5U5;
    else if(0 == _tcsicmp(szFmt, _T("X8L8V8U8")))
        *pFormat = FMT_X8L8V8U8;
    else if(0 == _tcsicmp(szFmt, _T("Q8W8V8U8")))
        *pFormat = FMT_Q8W8V8U8;
    else if(0 == _tcsicmp(szFmt, _T("V16U16")))
        *pFormat = FMT_V16U16;
    else if(0 == _tcsicmp(szFmt, _T("W11V11U10")))
        *pFormat = FMT_W11V11U10;
    else if(0 == _tcsicmp(szFmt, _T("W10V11U11")))
        *pFormat = FMT_W10V11U11;
    else if(0 == _tcsicmp(szFmt, _T("A2W10V10U10")))
        *pFormat = FMT_A2W10V10U10;
    else if(0 == _tcsicmp(szFmt, _T("Q16W16V16U16")))
        *pFormat = FMT_Q16W16V16U16;
    else if(0 == _tcsicmp(szFmt, _T("UYVY")))
        *pFormat = FMT_UYVY;
    else if(0 == _tcsicmp(szFmt, _T("RGBG")))
        *pFormat = FMT_R8G8_B8G8;
    else if(0 == _tcsicmp(szFmt, _T("YUY2")))
        *pFormat = FMT_YUY2;
    else if(0 == _tcsicmp(szFmt, _T("GRGB")))
        *pFormat = FMT_G8R8_G8B8;
    else if(0 == _tcsicmp(szFmt, _T("DXT1")))
        *pFormat = FMT_DXT1;
    else if(0 == _tcsicmp(szFmt, _T("DXT2")))
        *pFormat = FMT_DXT2;
    else if(0 == _tcsicmp(szFmt, _T("DXT3")))
        *pFormat = FMT_DXT3;
    else if(0 == _tcsicmp(szFmt, _T("DXT4")))
        *pFormat = FMT_DXT4;
    else if(0 == _tcsicmp(szFmt, _T("DXT5")))
        *pFormat = FMT_DXT5;
    else if(0 == _tcsicmp(szFmt, _T("MET1")))
        *pFormat = FMT_MULTI2_ARGB8;
    else if(0 == _tcsicmp(szFmt, _T("S1D15")))
        *pFormat = FMT_S1D15;
    else if(0 == _tcsicmp(szFmt, _T("S8D24")))
        *pFormat = FMT_S8D24;
    else if(0 == _tcsicmp(szFmt, _T("X8D24")))
        *pFormat = FMT_X8D24;
    else if(0 == _tcsicmp(szFmt, _T("X4S4D24")))
        *pFormat = FMT_X4S4D24;
    else if(0 == _tcsicmp(szFmt, _T("ANY")))
        *pFormat = FMT_ANY;
    else if(0 == _tcsicmp(szFmt, _T("ALL")))
        *pFormat = FMT_ALL;
}

// -------------------------------------------------------------------------------------

void StringToFmt(LPTSTR szFmt, MULTISAMPLE_TYPE *pFormat)
{
    if(pFormat == NULL || NULL == szFmt)
    {
        DPF(1, _T("FormatStrToFmt() - invalid params.\n"));
        return;
    }

    if(0 == _tcsicmp(szFmt, _T("0")))
        *pFormat = MULTISAMPLE_NONE;
    else if(0 == _tcsicmp(szFmt, _T("1")))
        *pFormat = MULTISAMPLE_NONMASKABLE;
    else if(0 == _tcsicmp(szFmt, _T("2")))
        *pFormat = MULTISAMPLE_2_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("3")))
        *pFormat = MULTISAMPLE_3_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("4")))
        *pFormat = MULTISAMPLE_4_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("5")))
        *pFormat = MULTISAMPLE_5_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("6")))
        *pFormat = MULTISAMPLE_6_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("7")))
        *pFormat = MULTISAMPLE_7_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("8")))
        *pFormat = MULTISAMPLE_8_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("9")))
        *pFormat = MULTISAMPLE_9_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("10")))
        *pFormat = MULTISAMPLE_10_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("11")))
        *pFormat = MULTISAMPLE_11_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("12")))
        *pFormat = MULTISAMPLE_12_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("13")))
        *pFormat = MULTISAMPLE_13_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("14")))
        *pFormat = MULTISAMPLE_14_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("15")))
        *pFormat = MULTISAMPLE_15_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("16")))
        *pFormat = MULTISAMPLE_16_SAMPLES;
    else if(0 == _tcsicmp(szFmt, _T("ANY")))
        *pFormat = MULTISAMPLE_ANY;
    else if(0 == _tcsicmp(szFmt, _T("ALL")))
        *pFormat = MULTISAMPLE_ALL;
}

void FmtToString(FORMATTYPE Type, FORMAT *pFormat, LPTSTR szOut)
{
    if(pFormat == NULL)
    {
        DPF(1, _T("FmtToString() - invalid params.\n"));
        return;
    }

    if(Type == FORMAT_MULTISAMPLE)
    {
        MULTISAMPLE_TYPE MSType = *((MULTISAMPLE_TYPE *)pFormat);

        switch(MSType)
        {
            case MULTISAMPLE_NONE:
	     	_stprintf(szOut, _T("NONE"));
                break;
            case MULTISAMPLE_NONMASKABLE:
	     	_stprintf(szOut, _T("NONMASKABLE"));
                break;
            case MULTISAMPLE_2_SAMPLES:
	     	_stprintf(szOut, _T("2_SAMPLES"));
                break;
            case MULTISAMPLE_3_SAMPLES:
           	_stprintf(szOut, _T("3_SAMPLES"));
                break;
            case MULTISAMPLE_4_SAMPLES:
	     	_stprintf(szOut, _T("4_SAMPLES"));
                break;
            case MULTISAMPLE_5_SAMPLES:
                _stprintf(szOut, _T("5_SAMPLES"));
                break;
            case MULTISAMPLE_6_SAMPLES:
                _stprintf(szOut, _T("6_SAMPLES"));
                break;
            case MULTISAMPLE_7_SAMPLES:
                _stprintf(szOut, _T("7_SAMPLES"));
                break;
            case MULTISAMPLE_8_SAMPLES:
                _stprintf(szOut, _T("8_SAMPLES"));
                break;
            case MULTISAMPLE_9_SAMPLES:
                _stprintf(szOut, _T("9_SAMPLES"));
                break;
            case MULTISAMPLE_10_SAMPLES:
                _stprintf(szOut, _T("10_SAMPLES"));
                break;
            case MULTISAMPLE_11_SAMPLES:
                _stprintf(szOut, _T("11_SAMPLES"));
                break;
            case MULTISAMPLE_12_SAMPLES:
                _stprintf(szOut, _T("12_SAMPLES"));
                break;
            case MULTISAMPLE_13_SAMPLES:
                _stprintf(szOut, _T("13_SAMPLES"));
                break;
            case MULTISAMPLE_14_SAMPLES:
                _stprintf(szOut, _T("14_SAMPLES"));
                break;
            case MULTISAMPLE_15_SAMPLES:
                _stprintf(szOut, _T("15_SAMPLES"));
                break;
            case MULTISAMPLE_16_SAMPLES:
                _stprintf(szOut, _T("16_SAMPLES"));
                break;
            default:
                _stprintf(szOut, _T("Unknown"));
                break;
        }
    }        
    else     
    {
        switch(pFormat->d3dfFormat)
        {
            case FMT_D16:
                _tcscpy(szOut, _T("D16"));
                break;
            case FMT_D15S1:
                _tcscpy(szOut, _T("D15S1"));
                break;
            case FMT_D24X8:
                _tcscpy(szOut, _T("D24X8"));
                break;
            case FMT_D24S8:
                _tcscpy(szOut, _T("D24S8"));
                break;
            case FMT_D24FS8:
                _tcscpy(szOut, _T("D24FS8"));
                break;
            case FMT_D24X4S4:
                _tcscpy(szOut, _T("D24X4S4"));
                break;
            case FMT_D32:
                _tcscpy(szOut, _T("D32"));
                break;
            case FMT_D16_LOCKABLE:
                _tcscpy(szOut, _T("D16_LOCKABLE"));
                break;
            case FMT_D32F_LOCKABLE:
                _tcscpy(szOut, _T("D32F_LOCKABLE"));
                break;
            case FMT_S1D15:
                _tcscpy(szOut, _T("S1D15"));
                break;
            case FMT_D32_LOCKABLE:
                _tcscpy(szOut, _T("D32_LOCKABLE"));
                break;
            case FMT_S8_LOCKABLE:
                _tcscpy(szOut, _T("S8_LOCKABLE"));
                break;
            case FMT_S8D24:
                _tcscpy(szOut, _T("S8D24"));
                break;
            case FMT_X8D24:
                _tcscpy(szOut, _T("X8D24"));
                break;
            case FMT_X4S4D24:
                _tcscpy(szOut, _T("X4S4D24"));
                break;
            case FMT_R8G8B8:
                _tcscpy(szOut, _T("R8G8B8"));
                break;
            case FMT_A8R8G8B8:
                _tcscpy(szOut, _T("A8R8G8B8"));
                break;
            case FMT_X8R8G8B8:
                _tcscpy(szOut, _T("X8R8G8B8"));
                break;
            case FMT_R5G6B5:
                _tcscpy(szOut, _T("R5G6B5"));
                break;
            case FMT_X1R5G5B5:
                _tcscpy(szOut, _T("X1R5G5B5"));
                break;
            case FMT_A1R5G5B5:
                _tcscpy(szOut, _T("A1R5G5B5"));
                break;
            case FMT_A4R4G4B4:
                _tcscpy(szOut, _T("A4R4G4B4"));
                break;
            case FMT_R3G3B2:
                _tcscpy(szOut, _T("R3G3B2"));
                break;
            case FMT_A8:
                _tcscpy(szOut, _T("A8"));
                break;
            case FMT_A1:
                _tcscpy(szOut, _T("A1"));
                break;
            case FMT_A8R3G3B2:
                _tcscpy(szOut, _T("A8R3G3B2"));
                break;
            case FMT_X4R4G4B4:
                _tcscpy(szOut, _T("X4R4G4B4"));
                break;
            case FMT_A2B10G10R10:
                _tcscpy(szOut, _T("A2B10G10R10"));
                break;
            case FMT_A8B8G8R8:
                _tcscpy(szOut, _T("A8B8G8R8"));
                break;
            case FMT_X8B8G8R8:
        	_tcscpy(szOut, _T("X8B8G8R8"));
                break;
            case FMT_G16R16:
                _tcscpy(szOut, _T("G16R16"));
                break;
            case FMT_A2R10G10B10:
                _tcscpy(szOut, _T("A2R10G10B10"));
                break;
			case FMT_A2B10G10R10_XR_BIAS:
                _tcscpy(szOut, _T("A2B10G10R10_XR_BIAS"));
                break;
            case FMT_A16B16G16R16:
                _tcscpy(szOut, _T("A16B16G16R16"));
                break;
            case FMT_R16F:
                _tcscpy(szOut, _T("R16F"));
                break;
            case FMT_G16R16F:
                _tcscpy(szOut, _T("G16R16F"));
                break;
            case FMT_A16B16G16R16F:
                _tcscpy(szOut, _T("A16B16G16R16F"));
                break;
	        case FMT_R32F:
                _tcscpy(szOut, _T("R32F"));
                break;
            case FMT_G32R32F:
                _tcscpy(szOut, _T("G32R32F"));
                break;
            case FMT_A32B32G32R32F:
                _tcscpy(szOut, _T("A32B32G32R32F"));
                break;
            case FMT_A8P8:
                _tcscpy(szOut, _T("A8P8"));
                break;
            case FMT_P4:
                _tcscpy(szOut, _T("P4"));
                break;
            case FMT_P8:
                _tcscpy(szOut, _T("P8"));
                break;
            case FMT_L8:
                _tcscpy(szOut, _T("L8"));
                break;
            case FMT_A8L8:
                _tcscpy(szOut, _T("A8L8"));
                break;
            case FMT_A4L4:
                _tcscpy(szOut, _T("A4L4"));
                break;
            case FMT_L16:
                _tcscpy(szOut, _T("L16"));
                break;
            case FMT_V8U8:
                _tcscpy(szOut, _T("V8U8"));
                break;
            case FMT_CxV8U8:
                _tcscpy(szOut, _T("CxV8U8"));
                break;
            case FMT_L6V5U5:
                _tcscpy(szOut, _T("L6V5U5"));
                break;
            case FMT_X8L8V8U8:
                _tcscpy(szOut, _T("X8L8V8U8"));
                break;
            case FMT_Q8W8V8U8:
                _tcscpy(szOut, _T("Q8W8V8U8"));
                break;
            case FMT_V16U16:
                _tcscpy(szOut, _T("V16U16"));
                break;
            case FMT_W11V11U10:
                _tcscpy(szOut, _T("W11V11U10"));
                break;
            case FMT_W10V11U11:
                _tcscpy(szOut, _T("W10V11U11"));
                break;
            case FMT_A2W10V10U10:
                _tcscpy(szOut, _T("A2W10V10U10"));
                break;
            case FMT_Q16W16V16U16:
                _tcscpy(szOut, _T("Q16W16V16U16"));
                break;
            case FMT_UYVY:
                _tcscpy(szOut, _T("UYVY"));
                break;
            case FMT_R8G8_B8G8:
                _tcscpy(szOut, _T("R8G8_B8G8"));
                break;
            case FMT_YUY2:
                _tcscpy(szOut, _T("YUY2"));
                break;
            case FMT_G8R8_G8B8:
                _tcscpy(szOut, _T("G8R8_G8B8"));
                break;
            case FMT_DXT1:
                _tcscpy(szOut, _T("DXT1"));
                break;
            case FMT_DXT2:
                _tcscpy(szOut, _T("DXT2"));
                break;
            case FMT_DXT3:
                _tcscpy(szOut, _T("DXT3"));
                break;
            case FMT_DXT4:
                _tcscpy(szOut, _T("DXT4"));
                break;
            case FMT_DXT5:
                _tcscpy(szOut, _T("DXT5"));
                break;
            case FMT_MULTI2_ARGB8:
                _tcscpy(szOut, _T("MULTI2_ARGB8"));
                break;
            default:
                _tcscpy(szOut, _T("Unknown"));
                break;
        }
    }
}


// -------------------------------------------------------------------------------------

REFIID GetD3DGUID(D3DGUIDS guidIndex, float fDXVersion)
{
    if (fDXVersion >= 8.0f && fDXVersion < 9.0f)
    {
        switch (guidIndex)
        {
            case IID_D3D:
                return IID_D3D8;

            case IID_DEVICE:
                return IID_Device8;

            case IID_RESOURCE:
                return IID_Resource8;

            case IID_BASETEXTURE:
                return IID_BaseTexture8;

            case IID_TEXTURE:
                return IID_Texture8;

            case IID_CUBETEXTURE:
                return IID_CubeTexture8;

            case IID_VOLUMETEXTURE:
                return IID_VolumeTexture8;

            case IID_VERTEXBUFFER:
                return IID_VertexBuffer8;

            case IID_INDEXBUFFER:
                return IID_IndexBuffer8;

            case IID_SURFACE:
                return IID_Surface8;

            case IID_VOLUME:
                return IID_Volume8;

            case IID_SWAPCHAIN:
                return IID_SwapChain8;

            default :
                return IID_IUnknown;
        }
    }
    else if (fDXVersion >= 9.0f && fDXVersion < 10.0f)
    {
        switch (guidIndex)
        {
            case IID_D3D:
                return IID_D3D9;

            case IID_DEVICE:
                return IID_Device9;

            case IID_RESOURCE:
                return IID_Resource9;

            case IID_BASETEXTURE:
                return IID_BaseTexture9;

            case IID_TEXTURE:
                return IID_Texture9;

            case IID_CUBETEXTURE:
                return IID_CubeTexture9;

            case IID_VOLUMETEXTURE:
                return IID_VolumeTexture9;

            case IID_VERTEXBUFFER:
                return IID_VertexBuffer9;

            case IID_INDEXBUFFER:
                return IID_IndexBuffer9;

            case IID_SURFACE:
                return IID_Surface9;

            case IID_VOLUME:
                return IID_Volume9;

            case IID_SWAPCHAIN:
                return IID_SwapChain9;

            case IID_VDECL:
                return IID_VDecl9;

            case IID_VSHADER:
                return IID_VShader9;

            case IID_PSHADER:
                return IID_PShader9;

            case IID_STATEBLOCK:
                return IID_StateBlock9;

            case IID_QUERY:
                return IID_Query9;

            default :
                return IID_IUnknown;
        }
    }


    return IID_IUnknown;
}

//=======================================================================================
// EOF
