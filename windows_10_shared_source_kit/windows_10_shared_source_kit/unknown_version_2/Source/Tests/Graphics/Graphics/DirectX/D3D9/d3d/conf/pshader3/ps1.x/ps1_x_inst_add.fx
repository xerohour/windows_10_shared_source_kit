sition v0  \n"
		"dcl_normal   v1  \n"
		//"dcl_color    v2  \n"
		"dcl_texcoord v3  \n"

		"mov oPos, v0     \n"
		"mov oT0,  v3     \n"
		);
	hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
	if (FAILED(hr))
	{
		WriteToLog(_T("DisplacementMapping::GenerateVertexShader -- D3DXAssembleShader6() failed, hr=%s\n"),
					m_pD3D->HResultToString(hr));
		RELEASE(m_pDecls[6]);
		return false;
	}
	if (!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShaders[6])))
	{
		WriteToLog("DisplacementMapping::GenerateVertexShader -- CreateVertexShader6() failed.\n");
		pShader->Release();
		RELEASE(m_pDecls[6]);
		return false;
	}
	pShader->Release();

	return true;
}

bool DisplacementMapping::CheckFilterSupport(D3DTEXTUREFILTERTYPE Filter, DWORD FilterCaps, WORD id)
{
	HRESULT hr;
	FORMAT Format;
	Format = m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
		
	//check if any filters but point are valid for current texture format
	int nAdapter = m_pSrcDevice->GetAdapterID();
	DISPLAYMODE Mode;
	hr = m_pD3D->GetAdapterDisplayMode( m_pSrcDevice->GetAdapterID(), &Mode );
	if( FAILED(hr) )
	{
		WriteToLog("CheckFilterSupport: GetAdapterDisplayMode() on src device returned %s (0x%x)\n", m_pD3D->HResultToString(hr), hr );
		return false;
	}
	hr = m_pD3D->CheckDeviceFormat(nAdapter, m_pSrcDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_FILTER, RTYPE_TEXTURE, Format);
	
	switch (Filter)
	{
		case D3DTEXF_NONE:
		{
			switch (id)
			{
				case 0: case 1: case 2:
					return true;
				default:
					WriteToLog("Unknow filter ID\n");
					return false;
			}
		}
		case D3DTEXF_ANISOTROPIC:
		{
			switch (id)
			{
				case 0:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC))
						return false;
					break;
				case 1:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC))
						return false;
					break;
				default:
					WriteToLog("Unknow filter ID\n");
					return false;
			}
			//if hr fails that means that any filtering but point is not supported for this format
			if (FAILED(hr))
			{
				//WriteToLog("Current format can not be used with Linear Filter.\n");
				return false;
			}
			else
				return true;
		}
		case D3DTEXF_LINEAR:
		{
			switch (id)
			{
				case 0:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
						return false;
					break;
				case 1:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
						return false;
					break;
				case 2:
					if (!(FilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
						return false;
					break;
				default:
					WriteToLog("Unknow filter ID\n");
					return false;
			}
			//if hr fails that means that any filtering but point is not supported for this format
			if (FAILED(hr))
			{
				//WriteToLog("Current format can not be used with Linear Filter.\n");
				return false;
			}
			else
				return true;
		}
		case D3DTEXF_POINT:
		{
			switch (id)
			{
				case 0:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MAGFPOINT))
						return false;
					else 
						return true;
				case 1:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MINFPOINT))
						return false;
					else 
						return true;
				case 2:
                    if (!(FilterCaps & D3DPTFILTERCAPS_MIPFPOINT))
						return false;
					else 
						return true;
				default:
					WriteToLog("Unknow filter ID\n");
					return false;
			}
		}
		default:
		{
			WriteToLog("Unknown filter type\n");
			return false;
		}
	}
}
	
/******************************Module*Header*******************************
* Module Name: fpo_Check.h
*
* Header for code that validates stackwalk events (via ETW)
*
* Author: David Flenniken [davidfl]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#ifndef __STACKWALK_LISTENER_H_
#define __STACKWALK_LISTENER_H_

HRESULT PerfinfoGuidEnabled(HANDLE _Event,
                            CLogWriter* pLog);


#endif
# this line brings in 3dsg.lib.
WANT_GRAPHICS_TESTCOMPONENTS_3DSG_LIBS = 1
!include ..\unicodetests.mk

TARGETNAME=surfboard

_NT_TARGET_VERSION = $(_NT_TARGET_VERSION_WIN7)

UMTYPE=console
UMENTRY=main

INCLUDES = $(INCLUDES); \
           $(PUBLIC_ROOT)\internal\windows\test\inc; \
           $(BASE_VID_TST_ROOT)\common\Modules\inc; \
           $(WINDOWS_TST_OBJ_ROOT)\Common\etw\manifest\$O; \

TARGETLIBS=$(TARGETLIBS) \
    $(PUBLIC_ROOT)\internal\windows\lib\*\$(ANSI_UNICODE_BUILD_TYPE)\etwDxgKernelStatistics.lib \
    $(PUBLIC_ROOT)\internal\windows\lib\*\$(ANSI_UNICODE_BUILD_TYPE)\etwDxgKernelConsumer.lib \
    $(WINDOWS_TST_COMMON)\etwStatistics\base\lib\$(ANSI_UNICODE_BUILD_TYPE)\$O\etwBaseStatistics.lib \
    $(BASE_VID_TST_OBJ_ROOT)\common\Modules\lib\unicode\$O\TestModules.lib \
    

SOURCES=\
    surfboardcomponent.cpp \
    main.cpp \
    Main.rc \

SOURCES=$(SOURCES) kitContent.kml

#include "Test_Include.fx"

int Test_Count = 109;

string TestInfo
<
	string TestType = "PS";
>
= "ps_1_4_inst_mul";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_1_4 : mul source reg combination v0, v1 is allowed";
string PS_001 = 
	"ps_1_4 "
	"mul r0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_1_4 : mul source reg combination v0, r0 is allowed";
string PS_002 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mul r0, v0, r0 ";

PS_CRITERIA PS_003_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_1_4 : mul source reg combination v0, c0 is allowed";
string PS_003 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_004_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_1_4 : mul source reg combination r0, v0 is allowed";
string PS_004 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mul r0, r0, v0 ";

PS_CRITERIA PS_005_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_1_4 : mul source reg combination r0, r1 is allowed";
string PS_005 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mul r0, r0, r1 ";

PS_CRITERIA PS_006_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_1_4 : mul source reg combination r0, c0 is allowed";
string PS_006 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mul r0, r0, c0 ";

PS_CRITERIA PS_007_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_1_4 : mul source reg combination c0, v0 is allowed";
string PS_007 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, c0, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_008_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_1_4 : mul source reg combination c0, r0 is allowed";
string PS_008 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mul r0, c0, r0 ";

PS_CRITERIA PS_009_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_1_4 : mul source reg combination c0, c1 is allowed";
string PS_009 = 
	"ps_1_4 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, c0, c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_010_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_1_4 : mul source reg combination v0_bias, c0 is allowed";
string PS_010 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0_bias, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_011_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_1_4 : mul source reg combination 1 - v0, c0 is allowed";
string PS_011 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, 1 - v0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_012_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_1_4 : mul source reg combination -v0, c0 is allowed";
string PS_012 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, -v0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_013_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_1_4 : mul source reg combination v0_x2, c0 is allowed";
string PS_013 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0_x2, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_014_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_1_4 : mul source reg combination v0_bx2, c0 is allowed";
string PS_014 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0_bx2, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_015_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_1_4 : mul source reg combination -v0_x2, c0 is allowed";
string PS_015 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, -v0_x2, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_016_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_1_4 : mul source reg combination -v0_bx2, c0 is allowed";
string PS_016 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, -v0_bx2, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_017_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_1_4 : mul source reg combination v0, c0 is allowed";
string PS_017 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_018_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_1_4 : mul source reg combination v0.r, c0 is allowed";
string PS_018 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.r, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_019_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_1_4 : mul source reg combination v0.g, c0 is allowed";
string PS_019 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.g, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_020_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_1_4 : mul source reg combination v0.b, c0 is allowed";
string PS_020 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.b, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_021_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_1_4 : mul source reg combination v0.a, c0 is allowed";
string PS_021 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.a, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_022_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_1_4 : mul source reg combination v0, c0.r is allowed";
string PS_022 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.r "
	"mov r0, c0 ";

PS_CRITERIA PS_023_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_1_4 : mul source reg combination v0, c0.g is allowed";
string PS_023 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.g "
	"mov r0, c0 ";

PS_CRITERIA PS_024_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_1_4 : mul source reg combination v0, c0.b is allowed";
string PS_024 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.b "
	"mov r0, c0 ";

PS_CRITERIA PS_025_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_1_4 : mul source reg combination v0, c0.a is allowed";
string PS_025 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0, c0.a "
	"mov r0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_1_4 : mul source reg combination t0, t0 is NOT allowed";
string PS_026 = 
	"ps_1_4 "
	"texcrd r0, t0 "
	"mul r0, t0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_1_4 : mul source reg combination v0.gbra, c0 is NOT allowed";
string PS_027 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.gbra, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_1_4 : mul source reg combination v0.brga, c0 is NOT allowed";
string PS_028 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.brga, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_1_4 : mul source reg combination v0.abgr, c0 is NOT allowed";
string PS_029 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.abgr, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_030_Desc = "ps_1_4 : mul source reg combination v0.agrb, c0 is NOT allowed";
string PS_030 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.agrb, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_031_Desc = "ps_1_4 : mul source reg combination v0.rbga, c0 is NOT allowed";
string PS_031 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.rbga, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_032_Desc = "ps_1_4 : mul source reg combination v0.rgab, c0 is NOT allowed";
string PS_032 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.rgab, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0104, -1, -1, -1, -1, CAPS_ARBITRARYSWIZZLE, -1, 0.f };
string PS_033_Desc = "ps_1_4 : mul source reg combination v0.bgr, c0 is NOT allowed";
string PS_033 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mul r0, v0.bgr, c0 "