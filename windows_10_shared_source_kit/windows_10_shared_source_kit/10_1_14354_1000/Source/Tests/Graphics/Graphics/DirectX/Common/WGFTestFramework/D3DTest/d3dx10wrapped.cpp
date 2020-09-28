//////////////////////////////////////////////////////////////////////////
//  D3DX10Wrapped.cpp
//  created:	2005/06/08
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////
#include "pch.h"

#include <shlwapi.h>
#include <shlobj.h>
#include <direct.h>

#include "D3DX10Wrapped.h"

typedef HRESULT (WGFTESTAPI CompileFromFileProcA)(
		LPCSTR pSrcFile,
		CONST D3D10_SHADER_MACRO* pDefines, 
		LPD3D10INCLUDE pInclude,
		LPCSTR pFunctionName, 
		LPCSTR pProfile, 
		UINT Flags1, 
		UINT Flags2, 
		ID3DX10ThreadPump* pPump, 
		ID3D10Blob** ppShader, 
		ID3D10Blob** ppErrorMsgs,
		HRESULT* pHResult);

typedef HRESULT (WGFTESTAPI CompileFromFileProcW)(
		LPCWSTR pSrcFile,
		CONST D3D10_SHADER_MACRO* pDefines, 
		LPD3D10INCLUDE pInclude,
		LPCSTR pFunctionName, 
		LPCSTR pProfile, 
		UINT Flags1, 
		UINT Flags2, 
		ID3DX10ThreadPump* pPump, 
		ID3D10Blob** ppShader, 
		ID3D10Blob** ppErrorMsgs,
		HRESULT* pHResult);

typedef HRESULT (WGFTESTAPI CompileFromResourceProcA)(
		HMODULE hSrcModule,
		LPCSTR pSrcResource,
		LPCSTR pSrcFileName,
		CONST D3D10_SHADER_MACRO* pDefines, 
		LPD3D10INCLUDE pInclude,
		LPCSTR pFunctionName, 
		LPCSTR pProfile, 
		UINT Flags1, 
		UINT Flags2, 
		ID3DX10ThreadPump* pPump, 
		ID3D10Blob** ppShader, 
		ID3D10Blob** ppErrorMsgs,
		HRESULT* pHResult);

typedef HRESULT (WGFTESTAPI CompileFromResourceProcW)(
		HMODULE hSrcModule,
		LPCWSTR pSrcResource,
		LPCWSTR pSrcFileName,
		CONST D3D10_SHADER_MACRO* pDefines, 
		LPD3D10INCLUDE pInclude,
		LPCSTR pFunctionName, 
		LPCSTR pProfile, 
		UINT Flags1, 
		UINT Flags2, 
		ID3DX10ThreadPump* pPump, 
		ID3D10Blob** ppShader, 
		ID3D10Blob** ppErrorMsgs,
		HRESULT* pHResult);

typedef HRESULT (WGFTESTAPI CompileFromMemoryProc)(
		LPCSTR pSrcData,
		SIZE_T SrcDataLen,
		LPCSTR pFileName,
		CONST D3D10_SHADER_MACRO* pDefines, 
		LPD3D10INCLUDE pInclude,
		LPCSTR pFunctionName, 
		LPCSTR pProfile, 
		UINT Flags1, 
		UINT Flags2, 
		ID3DX10ThreadPump* pPump, 
		ID3D10Blob** ppShader, 
		ID3D10Blob** ppErrorMsgs,
		HRESULT* pHResult);

typedef HRESULT (WGFTESTAPI GetImageInfoFromMemory)(
        LPCVOID                   pSrcData,
        SIZE_T                    SrcDataSize,
        ID3DX10ThreadPump*        pPump,
        D3DX10_IMAGE_INFO*        pSrcInfo,
        HRESULT*                  pHResult);

typedef HRESULT (WGFTESTAPI SaveTextureToMemory)(
        ID3D10Resource*            pSrcTexture,
        D3DX10_IMAGE_FILE_FORMAT   DestFormat,
        LPD3D10BLOB*               ppDestBuf,
        UINT                       Flags);

typedef HRESULT (WGFTESTAPI CreateTextureFromMemory)(
        ID3D10Device*             pDevice,
        LPCVOID                   pSrcData,
        SIZE_T                    SrcDataSize,
        D3DX10_IMAGE_LOAD_INFO*   pLoadInfo,    
        ID3DX10ThreadPump*        pPump,    
        ID3D10Resource**          ppTexture,
        HRESULT*                  pHResult);

HMODULE g_hD3DXDLL = NULL;

struct D3DXDLLInfo
{
	bool bWDKAvailable;
	int  LatestDebugVersion;
	int  LatestRetailVersion;
	TCHAR szLatestDebugDLLName[MAX_PATH];
	TCHAR szLatestRetailDLLName[MAX_PATH];
	TCHAR szLoadedDLLName[MAX_PATH];
	//HMODULE hLoadedDLL;
};

D3DXDLLInfo g_D3DXDLLInfo;

bool WINAPI FindLatestD3DXDLLInFolder(TCHAR *szPath)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError;

	PathAppend(szPath, TEXT("D3DX10*.dll"));

	hFind = FindFirstFile(szPath, &FindFileData);

	g_D3DXDLLInfo.LatestDebugVersion = -1;
	g_D3DXDLLInfo.LatestRetailVersion = -1;
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		//WriteError ("Problem with FindFirstFile\n");
		PathRemoveFileSpec(szPath);
		return false;
	} 
	else 
	{
		bool bDllFound = false;
		do
		{
			char* pDest = strchr(FindFileData.cFileName, '_');
			if (pDest)
			{
				pDest++;
				//Remove extension
				PathRemoveExtension(pDest);

				if (_stricmp(pDest, "wdk") == 0)
				{
					bDllFound = true;
					g_D3DXDLLInfo.bWDKAvailable = true;
				}
				else
				{
					bool bDebug = false;
					if (StrStrI(FindFileData.cFileName, "d3dx10d") != NULL)
						bDebug = true;

					int temp = _ttoi(pDest);
					if (bDebug)
					{
						if (temp != 0 && temp > g_D3DXDLLInfo.LatestDebugVersion)
						{
							bDllFound = true;
							g_D3DXDLLInfo.LatestDebugVersion = temp;
						}
					}
					else
					{
						if (temp != 0 && temp > g_D3DXDLLInfo.LatestRetailVersion)
						{
							bDllFound = true;
							g_D3DXDLLInfo.LatestRetailVersion = temp;
						}
					}
				}
			}
		}
		while (FindNextFile(hFind, &FindFileData) != 0);

		PathRemoveFileSpec(szPath);

		dwError = GetLastError();
		FindClose(hFind);
		if (dwError != ERROR_NO_MORE_FILES || !bDllFound) 
		{
			//WriteError("FindNextFile error. Error is %u\n", dwError);
			return false;
		}
	}

	if (g_D3DXDLLInfo.LatestRetailVersion > 0)
	{
		_sntprintf(g_D3DXDLLInfo.szLatestRetailDLLName, _countof(g_D3DXDLLInfo.szLatestRetailDLLName), _T("%s\\d3dx10_%i.dll"), szPath, g_D3DXDLLInfo.LatestRetailVersion);
	}
	if (g_D3DXDLLInfo.LatestDebugVersion > 0)
	{
		_sntprintf(g_D3DXDLLInfo.szLatestDebugDLLName, _countof(g_D3DXDLLInfo.szLatestDebugDLLName), _T("%s\\d3dx10d_%i.dll"), szPath, g_D3DXDLLInfo.LatestDebugVersion);
	}
	return true;
}

bool WINAPI FindLatestD3DXDLL()
{
	TCHAR szPath[MAX_PATH];

	ZeroMemory(&g_D3DXDLLInfo, sizeof(g_D3DXDLLInfo));

	// first search EXE directory for parallel support DLL
	if (0 == GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		WriteError ("Problem with GetModuleFileName\n");
		return false;
	}
	PathRemoveFileSpec(szPath);

	if (FindLatestD3DXDLLInFolder(szPath))
	{
		return true;
	}

	ZeroMemory(&g_D3DXDLLInfo, sizeof(g_D3DXDLLInfo));

	// next search working directory for support DLL
	if (0 == _getcwd(szPath, MAX_PATH))
	{
		WriteError ("Problem with _getcwd\n");
		return false;
	}

	if (FindLatestD3DXDLLInFolder(szPath))
	{
		return true;
	}

	ZeroMemory(&g_D3DXDLLInfo, sizeof(g_D3DXDLLInfo));

	// next search system directory for support DLL
	if(FAILED(SHGetFolderPath(NULL, 
								 CSIDL_SYSTEM, 
								 NULL, 
								 0, 
								 szPath))) 
	{
		WriteError ("Problem with SHGetFolderPath\n");
		return false;
	}

	if (FindLatestD3DXDLLInFolder(szPath))
	{
		return true;
	}

	WriteError ("Could not find a D3DX10 dll\n");

	_sntprintf(g_D3DXDLLInfo.szLatestRetailDLLName, _countof(g_D3DXDLLInfo.szLatestRetailDLLName), _T("'Could not find a retail d3dx10 dll'"));
	_sntprintf(g_D3DXDLLInfo.szLatestDebugDLLName, _countof(g_D3DXDLLInfo.szLatestDebugDLLName), _T("'Could not find a debug d3dx10 dll'"));
	return false;
}

bool WINAPI LoadD3DXDLL()
{
	if (g_hD3DXDLL != NULL)
		return true;

#if 0 // we don't want to find version-numbered DLLs anymore, only _test DLLs
	if (!FindLatestD3DXDLL())
		return false;
#else
	#ifdef _DEBUG
		strncpy( g_D3DXDLLInfo.szLatestDebugDLLName, _T("D3DX10_TEST.DLL"), _countof(g_D3DXDLLInfo.szLatestDebugDLLName) - 1 );
	#else
		strncpy( g_D3DXDLLInfo.szLatestRetailDLLName, _T("D3DX10_TEST.DLL"), _countof(g_D3DXDLLInfo.szLatestRetailDLLName) - 1 );
	#endif
#endif

#ifdef _DEBUG
	#pragma warning( suppress: 6054) // Bogus PreFast error - there is no chance that szLatestDebugDLLName is not NULL terminated
	strncpy( g_D3DXDLLInfo.szLoadedDLLName, g_D3DXDLLInfo.szLatestDebugDLLName, _countof(g_D3DXDLLInfo.szLoadedDLLName) - 1 );
#else
	#pragma warning( suppress: 6054) // Bogus PreFast error - there is no chance that szLatestDebugDLLName is not NULL terminated
	strncpy( g_D3DXDLLInfo.szLoadedDLLName, g_D3DXDLLInfo.szLatestRetailDLLName, _countof(g_D3DXDLLInfo.szLoadedDLLName) - 1 );
#endif

	// Guarantee NULL terminator after copy
	g_D3DXDLLInfo.szLoadedDLLName[_countof(g_D3DXDLLInfo.szLoadedDLLName)-1] = 0; 

	g_hD3DXDLL = LoadLibrary( g_D3DXDLLInfo.szLoadedDLLName );

	if( NULL == g_hD3DXDLL )
	{
		WriteError( _T( "D3DX DLL '%s' Not found" ), g_D3DXDLLInfo.szLoadedDLLName );
		return false;
	}

	return true;
}

HRESULT WINAPI D3DXCompileShaderFromFileA(LPCSTR pSrcFile,CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
									  LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, UINT Flags2, HRESULT* pHResult)
{
	// If the D3D_SHADER_DEBUG flag is specified and d3d10d.dll is missing then
	// stomp on the D3D_SHADER_DEBUG flag.
	HMODULE d3d10d_module = LoadLibrary( "D3D10D.DLL" );
	if( ( Flags & D3D_SHADER_DEBUG ) && NULL == d3d10d_module )
		Flags &= ~D3D_SHADER_DEBUG;
	FreeLibrary(d3d10d_module);

	LoadD3DXDLL();

	CompileFromFileProcA *pProc = (CompileFromFileProcA *) GetProcAddress( g_hD3DXDLL, "D3DX10CompileFromFileA" );

	if( !pProc )
	{
		WriteError( _T( "D3DX10CompileFromFileA - Function not found in %s" ), g_D3DXDLLInfo.szLoadedDLLName );
		return E_FAIL;
	}

	return pProc( pSrcFile, pDefines, pInclude, pFunctionName, pProfile, Flags, Flags2, pPump, ppShader, ppErrorMsgs, pHResult );

}

HRESULT WINAPI D3DXCompileShaderFromFileW( LPCWSTR pSrcFile, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
									  LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, UINT Flags2, HRESULT* pHResult )
{
	// If the D3D_SHADER_DEBUG flag is specified and d3d10d.dll is missing then
	// stomp on the D3D_SHADER_DEBUG flag.
	HMODULE d3d10d_module = LoadLibrary( "D3D10D.DLL" );
	if( ( Flags & D3D_SHADER_DEBUG ) && NULL == d3d10d_module )
		Flags &= ~D3D_SHADER_DEBUG;
	FreeLibrary(d3d10d_module);

	LoadD3DXDLL();

	CompileFromFileProcW *pProc = (CompileFromFileProcW *) GetProcAddress( g_hD3DXDLL, "D3DX10CompileFromFileW" );

	if( !pProc )
	{
		WriteError( _T( "D3DX10CompileFromFileW - Function not found in %s" ), g_D3DXDLLInfo.szLoadedDLLName );
		return E_FAIL;
	}

	return pProc( pSrcFile, pDefines, pInclude, pFunctionName, pProfile, Flags, Flags2, pPump, ppShader, ppErrorMsgs, pHResult );
}

HRESULT WINAPI D3DXCompileShaderFromResourceA(HMODULE hSrcModule, LPCSTR pSrcResource, LPCSTR pSrcFileName, CONST D3D10_SHADER_MACRO* pDefines, 
										  LPD3D10INCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, UINT Flags2, HRESULT* pHResult)
{
	// If the D3D_SHADER_DEBUG flag is specified and d3d10d.dll is missing then
	// stomp on the D3D_SHADER_DEBUG flag.
	HMODULE d3d10d_module = LoadLibrary( "D3D10D.DLL" );
	if( ( Flags & D3D_SHADER_DEBUG ) && NULL == d3d10d_module )
		Flags &= ~D3D_SHADER_DEBUG;
	FreeLibrary(d3d10d_module);

	LoadD3DXDLL();

	CompileFromResourceProcA *pProc = (CompileFromResourceProcA *) GetProcAddress( g_hD3DXDLL, "D3DX10CompileFromResourceA" );

	if( !pProc )
	{
		WriteError( _T( "D3DX10CompileFromResourceA - Function not found in %s" ), g_D3DXDLLInfo.szLoadedDLLName );
		return E_FAIL;
	}

	return pProc( hSrcModule, pSrcResource, pSrcFileName, pDefines, pInclude, pFunctionName, pProfile, Flags, Flags2, pPump, ppShader, ppErrorMsgs, pHResult );
}

HRESULT WINAPI D3DXCompileShaderFromResourceW(HMODULE hSrcModule, LPCWSTR pSrcResource, LPCWSTR pSrcFileName, CONST D3D10_SHADER_MACRO* pDefines, 
											 LPD3D10INCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, UINT Flags2, HRESULT* pHResult)
{
	// If the D3D_SHADER_DEBUG flag is specified and d3d10d.dll is missing then
	// stomp on the D3D_SHADER_DEBUG flag.
	HMODULE d3d10d_module = LoadLibrary( "D3D10D.DLL" );
	if( ( Flags & D3D_SHADER_DEBUG ) && NULL == d3d10d_module )
		Flags &= ~D3D_SHADER_DEBUG;
	FreeLibrary(d3d10d_module);

	LoadD3DXDLL();

	CompileFromResourceProcW *pProc = (CompileFromResourceProcW *) GetProcAddress( g_hD3DXDLL, "D3DX10CompileFromResourceW" );

	if( !pProc )
	{
		WriteError( _T( "D3DX10CompileFromResourceW - Function not found in %s" ), g_D3DXDLLInfo.szLoadedDLLName );
		return E_FAIL;
	}

	return pProc( hSrcModule, pSrcResource, pSrcFileName, pDefines, pInclude, pFunctionName, pProfile, Flags, Flags2, pPump, ppShader, ppErrorMsgs, pHResult );
}

HRESULT WINAPI D3DXCompileShaderFromMemory(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude, 
									   LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, UINT Flags2, HRESULT* pHResult)
{
	// If the D3D_SHADER_DEBUG flag is specified and d3d10d.dll is missing then
	// stomp on the D3D_SHADER_DEBUG flag.
	HMODULE d3d10d_module = LoadLibrary( "D3D10D.DLL" );
	if( ( Flags & D3D_SHADER_DEBUG ) && NULL == d3d10d_module )
		Flags &= ~D3D_SHADER_DEBUG;
	FreeLibrary(d3d10d_module);

	LoadD3DXDLL();

	CompileFromMemoryProc *pProc = (CompileFromMemoryProc *) GetProcAddress( g_hD3DXDLL, "D3DX10CompileFromMemory" );

	if( !pProc )
	{
		WriteError( _T( "D3DX10CompileFromMemory - Function not found in %s" ), g_D3DXDLLInfo.szLoadedDLLName );
		return E_FAIL;
	}

	return pProc( pSrcData, SrcDataLen, pFileName, pDefines, pInclude, pFunctionName, pProfile, Flags, Flags2, pPump, ppShader, ppErrorMsgs, pHResult );
}

HRESULT WINAPI D3DXGetImageInfoFromMemory(
        LPCVOID                   pSrcData,
        SIZE_T                    SrcDataSize,
        ID3DX10ThreadPump*        pPump,
        D3DX10_IMAGE_INFO*        pSrcInfo,
        HRESULT*                  pHResult)
{
	LoadD3DXDLL();

	GetImageInfoFromMemory *pProc = (GetImageInfoFromMemory *) GetProcAddress( g_hD3DXDLL, "D3DX10GetImageInfoFromMemory" );

	if( !pProc )
	{
		WriteError( _T( "D3DX10GetImageInfoFromMemory - Function not found in %s" ), g_D3DXDLLInfo.szLoadedDLLName );
		return E_FAIL;
	}

	return pProc( pSrcData, SrcDataSize, pPump, pSrcInfo, pHResult );
}

HRESULT WINAPI D3DXSaveTextureToMemory(
        ID3D10Resource*            pSrcTexture,
        D3DX10_IMAGE_FILE_FORMAT   DestFormat,
        LPD3D10BLOB*               ppDestBuf,
        UINT                       Flags)
{
	LoadD3DXDLL();

	SaveTextureToMemory *pProc = (SaveTextureToMemory *) GetProcAddress( g_hD3DXDLL, "D3DX10SaveTextureToMemory" );

	if( !pProc )
	{
		WriteError( _T( "D3DX10SaveTextureToMemory - Function not found in %s" ), g_D3DXDLLInfo.szLoadedDLLName );
		return E_FAIL;
	}

	return pProc( pSrcTexture, DestFormat, ppDestBuf, Flags );
}

HRESULT WINAPI D3DXCreateTextureFromMemory(
        ID3D10Device*             pDevice,
        LPCVOID                   pSrcData,
        SIZE_T                    SrcDataSize,
        D3DX10_IMAGE_LOAD_INFO*   pLoadInfo,    
        ID3DX10ThreadPump*        pPump,    
        ID3D10Resource**          ppTexture,
        HRESULT*                  pHResult)
{
	LoadD3DXDLL();

	CreateTextureFromMemory *pProc = (CreateTextureFromMemory *) GetProcAddress( g_hD3DXDLL, "D3DX10CreateTextureFromMemory" );

	if( !pProc )
	{
		WriteError( _T( "D3DX10CreateTextureFromMemory - Function not found in %s" ), g_D3DXDLLInfo.szLoadedDLLName );
		return E_FAIL;
	}

	return pProc( pDevice, pSrcData, SrcDataSize, pLoadInfo,  pPump, ppTexture, pHResult );
}
