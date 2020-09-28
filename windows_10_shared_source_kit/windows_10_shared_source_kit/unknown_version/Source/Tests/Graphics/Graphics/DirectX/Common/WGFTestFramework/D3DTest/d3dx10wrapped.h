//////////////////////////////////////////////////////////////////////////
//  D3DX10Wrapped.h
//  created:	2005/06/08
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>

//Need to include d3d10_1.h before anything that includes d3d10.h until the header is fixed
#include "D3D10_1.h"

#include "new_off.h"
#include "D3DX10.h"
#include "new_on.h"

#include <WGFTestCore.h>
#include "d3dtest.h"
#include "D3D10ValueSets.h"
#include "DXGIValueSets.h"

extern bool WINAPI LoadD3DXDLL();


extern HRESULT WINAPI D3DXCompileShaderFromFileA(LPCSTR pSrcFile,CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
											 LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, UINT Flags2 = 0, HRESULT* pHResult = NULL);
extern HRESULT WINAPI D3DXCompileShaderFromFileW( LPCWSTR pSrcFile, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
											 LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, UINT Flags2 = 0, HRESULT* pHResult = NULL );
#if 0
#ifdef UNICODE
#define D3DXCompileShaderFromFile D3DXCompileShaderFromFileW
#else
#define D3DXCompileShaderFromFile D3DXCompileShaderFromFileA
#endif
#endif

extern HRESULT WINAPI D3DXCompileShaderFromResourceA(HMODULE hSrcModule, LPCSTR pSrcResource, LPCSTR pSrcFileName, CONST D3D10_SHADER_MACRO* pDefines, 
												 LPD3D10INCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, UINT Flags2 = 0, HRESULT* pHResult = NULL);
extern HRESULT WINAPI D3DXCompileShaderFromResourceW(HMODULE hSrcModule, LPCWSTR pSrcResource, LPCWSTR pSrcFileName, CONST D3D10_SHADER_MACRO* pDefines, 
													LPD3D10INCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, UINT Flags2 = 0, HRESULT* pHResult = NULL);
#if 0
#ifdef UNICODE
#define D3DXCompileShaderFromResource D3DXCompileShaderFromResourceW
#else
#define D3DXCompileShaderFromResource D3DXCompileShaderFromResourceA
#endif
#endif

extern HRESULT WINAPI D3DXCompileShaderFromMemory(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude, 
											  LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, UINT Flags2 = 0, HRESULT* pHResult = NULL);

extern HRESULT WINAPI D3DXGetImageInfoFromMemory(
        LPCVOID                   pSrcData,
        SIZE_T                    SrcDataSize,
        ID3DX10ThreadPump*        pPump,
        D3DX10_IMAGE_INFO*        pSrcInfo,
        HRESULT*                  pHResult);

extern HRESULT WINAPI D3DXSaveTextureToMemory(
        ID3D10Resource*            pSrcTexture,
        D3DX10_IMAGE_FILE_FORMAT   DestFormat,
        LPD3D10BLOB*               ppDestBuf,
        UINT                       Flags);

extern HRESULT WINAPI D3DXCreateTextureFromMemory(
        ID3D10Device*             pDevice,
        LPCVOID                   pSrcData,
        SIZE_T                    SrcDataSize,
        D3DX10_IMAGE_LOAD_INFO*   pLoadInfo,    
        ID3DX10ThreadPump*        pPump,    
        ID3D10Resource**          ppTexture,
        HRESULT*                  pHResult);
