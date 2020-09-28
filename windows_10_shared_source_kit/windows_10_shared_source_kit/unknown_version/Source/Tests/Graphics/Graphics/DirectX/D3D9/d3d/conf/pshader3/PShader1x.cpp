// ============================================================================
//
//  pshader1x.cpp
//  Written by cliffo
//
// ============================================================================
#include <stdio.h>
#include <io.h>
#include "PShader.h"
#include "CTextures.h"
#include "CEffect.h"
#include "types.h"

#ifdef FAST_TEST
static const FMT g_cTestFormat = FMT_A8R8G8B8;
#else
static const FMT g_cTestFormat = FMT_ALL;
#endif


CPShader1_x::CPShader1_x(LPTSTR str) : CPShaderX()
{
	m_Options.D3DOptions.fMinDXVersion = 9.f; // !!! 8 maybe?

	// Setup macro for pixel shader version:
	
	static D3DXMACRO macros_1_x[] = {{"USING_PS_1_X", ""}, {NULL,NULL}};
	
	m_pMacros = macros_1_x;

	if(!_tcsicmp(str, "tex"))
	{		
		m_szTestName="PShader1.x_Textures";
		m_szCommandKey="ps1x_tex";
		m_szPSPrefix = "ps1_xtex";
		//m_Options.ModeOptions.dwTexExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV | PF_ALPHA;
		m_Options.ModeOptions.dwTexExclude = PF_FLOATTEX;
		m_Options.ModeOptions.dwTexInclude = PF_RGB;
		m_Options.ModeOptions.TextureFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "cube"))
	{
        m_szTestName="PShader1.x_Cubes";
		m_szCommandKey="ps1x_cube";
		m_szPSPrefix = "ps1_xcube";
		//m_Options.ModeOptions.dwCubeExclude |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;
		m_Options.ModeOptions.dwCubeExclude = PF_FLOATTEX;
		m_Options.ModeOptions.dwCubeInclude = PF_RGB;
		m_Options.ModeOptions.CubeFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "vol"))
	{
		m_szTestName="PShader1.x_Volumes";
		m_szCommandKey="ps1x_vol";
		m_szPSPrefix = "ps1_xvol";
		//m_Options.ModeOptions.dwVolExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;
		m_Options.ModeOptions.dwVolExclude = PF_FLOATTEX;
		m_Options.ModeOptions.dwVolInclude = PF_RGB;
		m_Options.ModeOptions.VolumeFmt = g_cTestFormat;
	}
	else
	{
		m_szTestName="PShader1.x";
		m_szCommandKey="ps1x";
		m_szPSPrefix = "ps1_x_";
	}
}

bool CPShader1_x::CapsCheck(void)
{
    if (!CPShaderX::CapsCheck())
        return false;
    
    DWORD dwSupported = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
    // Check major version
    
    if (D3DSHADER_VERSION_MAJOR(dwSupported) == 0)
    {
        WriteToLog(_T("Pixel shader version 1.0 required! Device supports %d.%d!\n"),
                   D3DSHADER_VERSION_MAJOR(dwSupported), D3DSHADER_VERSION_MINOR(dwSupported));
                   
        return false;
    }

    return true;
}

bool CPShader1_x::PreModeSetup()
{
	HKEY hRegKey;
	TCHAR ShaderRange[MAX_PATH];
	
	CPShaderX::PreModeSetup();

	if ( m_pSrcDevice->GetCaps()->dwPixelShaderVersion > 0 )
	{
	    //Now we need to set the ref MaxPixelShaderValue as this defines the range of
		//values that can be used within a pixel shader.
		//This is an un documented reg key
		_stprintf(ShaderRange, "%f", m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue);
				
		if (SUCCEEDED ( RegCreateKeyEx(HKEY_LOCAL_MACHINE,
										_T("SOFTWARE\\Microsoft\\Direct3D\\ReferenceDevice"),
										0,
										_T(""),
										REG_OPTION_NON_VOLATILE,
										KEY_WRITE,
										NULL,
										&hRegKey,
										NULL) ) )
		{
			if (m_dwVersion >= 0x0900)
			{
				if ( FAILED ( RegSetValueEx(hRegKey, _T("PixelShader1xMaxValue"), 0, REG_SZ,  
								(CONST BYTE*) ShaderRange, 
								((DWORD)_tcsclen(ShaderRange)+1)*sizeof(TCHAR) ) ) )
				{
					WriteToLog(_T("PShader: Unable to set Dx9 ref rast reg key ***Abort this test run***\n"));
					RegCloseKey(hRegKey);
					return false;
				}
			}
			else
			{
				if ( FAILED ( RegSetValueEx(hRegKey, _T("MaxPixelShaderValue"), 0, REG_SZ,  
								(CONST BYTE*) ShaderRange, 
								((DWORD)_tcsclen(ShaderRange)+1)*sizeof(TCHAR) ) ) )
				{
					WriteToLog(_T("PShader: Unable to set Dx8 ref rast reg key ***Abort this test run***\n"));
					RegCloseKey(hRegKey);
					return false;
				}
			}

			RegCloseKey(hRegKey);
		}
		else
		{
			WriteToLog(_T("PShader: Unable to set ref rast reg key ***Abort this test run***\n"));
			return false;
		}
	}
	return true;
}

void CPShader1_x::Cleanup()
{
	HKEY hRegKey;

    CPShaderX::Cleanup();

	if (SUCCEEDED (RegOpenKeyEx(HKEY_LOCAL_MACHINE,								// handle to open key
						_T("SOFTWARE\\Microsoft\\Direct3D\\ReferenceDevice"),	// subkey name
						0,														// reserved
						KEY_ALL_ACCESS,											// security access mask
						&hRegKey) )	)											// handle to open key
	{		
		if (m_dwVersion >= 0x0900)
		{
			if ( FAILED  ( RegDeleteValue(hRegKey,				// handle to open key
							_T("PixelShader1xMaxValue") ) ) )		// value name
			{
				WriteToLog(_T("PShader: Unable to restore Dx9 ref rast reg key\n"));
			}
		}
		else
		{
			if ( FAILED  ( RegDeleteValue(hRegKey,				// handle to open key
							_T("MaxPixelShaderValue") ) ) )		// value name
			{
				WriteToLog(_T("PShader: Unable to restore Dx8 ref rast reg key\n"));
			}
		}
		RegCloseKey(hRegKey);
	}
	
	return;
}
