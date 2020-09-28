 "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								if b0 nop else "
	"																									frc r0, v0 "
	"																								endif "
	"																							endif "
	"																						endif "
	"																					endif "
	"																				endif "
	"																			endif "
	"																		endif "
	"																	endif "
	"																endif "
	"															endif "
	"														endif "
	"													endif "
	"												endif "
	"											endif "
	"										endif "
	"									endif "
	"								endif "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_078_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_2_0 : frc is allowed in a 1 level loop aL, i0 block";
string VS_078 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	frc r0, v0 "
	"endloop ";

VS_CRITERIA VS_079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_2_0 : frc is NOT allowed in a 2 level loop aL, i0 block";
string VS_079 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		frc r0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_2_0 : frc is NOT allowed in a 3 level loop aL, i0 block";
string VS_080 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			frc r0, v0 "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_2_0 : frc is NOT allowed in a 4 level loop aL, i0 block";
string VS_081 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				frc r0, v0 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_2_0 : frc is NOT allowed in a 5 level loop aL, i0 block";
string VS_082 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					frc r0, v0 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_083_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_2_0 : frc is allowed in a 1 level rep i0 block";
string VS_083 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 "
	"	frc r0, v0 "
	"endrep ";

VS_CRITERIA VS_084_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_2_0 : frc is NOT allowed in a 2 level rep i0 block";
string VS_084 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		frc r0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_085_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_2_0 : frc is NOT allowed in a 3 level rep i0 block";
string VS_085 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			frc r0, v0 "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_086_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_2_0 : frc is NOT allowed in a 4 level rep i0 block";
string VS_086 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				frc r0, v0 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_087_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_2_0 : frc is NOT allowed in a 5 level rep i0 block";
string VS_087 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					frc r0, v0 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_088_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_2_0 : frc is allowed in a if b0 and if b0 block";
string VS_088 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		frc r0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_089_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_089_Desc = "vs_2_0 : frc is allowed in a if b0 and if b0 nop else block";
string VS_089 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 nop else "
	"		frc r0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_090_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_2_0 : frc is allowed in a if b0 and loop aL, i0 block";
string VS_090 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 "
	"	loop aL, i0 "
	"		frc r0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_091_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_2_0 : frc is allowed in a if b0 and rep i0 block";
string VS_091 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 "
	"	rep i0 "
	"		frc r0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_092_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_2_0 : frc is allowed in a if b0 nop else and if b0 block";
string VS_092 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 "
	"		frc r0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_093_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_2_0 : frc is allowed in a if b0 nop else and if b0 nop else block";
string VS_093 = 
	"vs_2_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		frc r0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_094_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_2_0 : frc is allowed in a if b0 nop else and loop aL, i0 block";
string VS_094 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		frc r0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_095_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_2_0 : frc is allowed in a if b0 nop else and rep i0 block";
string VS_095 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 nop else "
	"	rep i0 "
	"		frc r0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_096_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_2_0 : frc is allowed in a loop aL, i0 and if b0 block";
string VS_096 = 
	"vs_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	if b0 "
	"		frc r0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_097_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_2_0 : frc is allowed in a loop aL, i0 and if b0 nop else block";
string VS_097 = 
	"vs_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		frc r0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_098_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_2_0 : frc is NOT allowed in a loop aL, i0 and loop aL, i0 block";
string VS_098 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		frc r0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_099_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_2_0 : frc is NOT allowed in a loop aL, i0 and rep i0 block";
string VS_099 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	rep i0 "
	"		frc r0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_100_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_2_0 : frc is allowed in a rep i0 and if b0 block";
string VS_100 = 
	"vs_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 "
	"	if b0 "
	"		frc r0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_101_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_2_0 : frc is allowed in a rep i0 and if b0 nop else block";
string VS_101 = 
	"vs_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 "
	"	if b0 nop else "
	"		frc r0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_102_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_2_0 : frc is NOT allowed in a rep i0 and loop aL, i0 block";
string VS_102 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 "
	"	loop aL, i0 "
	"		frc r0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_103_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_2_0 : frc is NOT allowed in a rep i0 and rep i0 block";
string VS_103 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		frc r0, v0 "
	"	endrep "
	"endrep ";


!include ..\sources.inc

TARGETNAME=DXGIPresent
TARGETTYPE=LIBRARY
UMTYPE=windows
UMENTRY=winmain

C_DEFINES=$(C_DEFINES)

INCLUDES=\
	$(INCLUDES); \
	$(GRAPHICS_TST_ROOT)\DirectX\Common\WGFTestFramework\DXGITest; \
	$(GRAPHICS_TST_ROOT)\DirectX\Common\formathelper; \

TARGETLIBS=\
	$(TARGETLIBS) \
	$(SDK_LIB_PATH)\d3d10.lib \
  	$(SDK_LIB_PATH)\d3dx10_test.lib \
	$(SDK_LIB_PATH)\d3d9.lib \
	$(SDK_LIB_PATH)\dwmapi.lib \
	$(SDK_LIB_PATH)\dxgi.lib \
	$(SDK_LIB_PATH)\dxguid.lib \
	$(SDK_LIB_PATH)\uuid.lib \
    $(WINDOWS_LIB_PATH)\dwmapip.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\Common\WGFTestFramework\DXGITest\$(O)\DXGITest.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\Common\WGFTestFramework\D3DTest\$(O)\D3DTest.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\d3d10refutil\$O\D3D10RefUtil.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\d3d10swapchain\$O\D3D10SwapChain.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\devicewrapper\$O\DeviceWrapper.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\dxgiwndclass\$O\DXGIWndClass.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\dxgibbfmt\$O\DXGIBBFMT.LIB \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\dxgifmt\$O\DXGIFMT.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\DXGISwapChainFlag\$O\DXGISwapChainFlag.lib \
        $(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\dxgiswapeffects\$O\DXGISwapEffects.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\dxgisurfutils\$O\DXGISurfUtils.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\dxgiscanoutformats\$O\DXGIScanoutFormats.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\enummodesgdi\$O\EnumModesGDI.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\getdeviceinfo\$O\GetDeviceInfo9.lib \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\DXGI\Common\source\modeset\$O\ModeSet.lib \

TARGET_DESTINATION=Graphics\D3D\Conf

USE_RTTI=1


SOURCES=\
	BBDims.cpp \
	PresentSingleSC.cpp \
	WindowTree.cpp \
	ParameterHelper.cpp \
	RegionHelper.cpp \
        Adapter.cpp \
        ModeInformation.cpp \
       	PresentFS.cpp \
	PresentSingleSC.cpp\
	SyncTransformTest.cpp\
	PresentWinCS.cpp\
        Rects.cpp\
	ScrollInfo.cpp \
	FlipRotate.cpp \UNIVERSAL_WGF_TEST=1

!include ..\Conf11_Core.mk

TARGETNAME=WGF11Filter


USE_STL=1
STL_VER=100


INCLUDES=\
	.\; \
	$(GRAPHICS_TST_ROOT)\DirectX\Common\FormatHelper; \
	$(INCLUDES)


SOURCES=\
	Filter.cpp \
	FilterCode.cpp \
	Filter.rc \
	BasicFilter.cpp \
	MipFilter.cpp \
	TextureCode.cpp \
	MipGen.cpp \
	AnisoFilter.cpp \
	SamplerStates.cpp \
	MinLOD.cpp \

SOURCES=$(SOURCES) kitContent.kml

VARIABLE_OVERRIDES=$(VARIABLE_OVERRIDES);OBJ_PATH=$(OBJ_PATH)
VARIABLE_OVERRIDES=$(VARIABLE_OVERRIDES);TARGETNAME=$(TARGETNAME)
SPKG_SOURCES=..\sources.pkg.xml
SPKG_TARGET_DESTINATION=$(SPKG_DEFAULT_TARGET_DESTINATION)\D3DTest#include "Test_Include.fx"

int Test_Count = 68;

string TestInfo
<
	string TestType = "PS";
>
= "ps_1_3_inst_texm3x3";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_1_3 : texm3x3 source reg combination t0 is allowed";
string PS_001 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_1_3 : texm3x3 source reg combination t0_bx2 is allowed";
string PS_002 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0_bx2 "
	"mov r0, c0 ";

PS_CRITERIA PS_003_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_1_3 : texm3x3 source reg combination v0 is NOT allowed";
string PS_003 = 
	"ps_1_3 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_004_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_1_3 : texm3x3 source reg combination r0 is NOT allowed";
string PS_004 = 
	"ps_1_3 "
	"mov r0, c0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, r0 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_1_3 : texm3x3 source reg combination c0 is NOT allowed";
string PS_005 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_1_3 : texm3x3 source reg combination t0.r is NOT allowed";
string PS_006 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.r "
	"mov r0, c0 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_1_3 : texm3x3 source reg combination t0.g is NOT allowed";
string PS_007 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.g "
	"mov r0, c0 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_1_3 : texm3x3 source reg combination t0.b is NOT allowed";
string PS_008 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.b "
	"mov r0, c0 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_1_3 : texm3x3 source reg combination t0.a is NOT allowed";
string PS_009 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.a "
	"mov r0, c0 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_1_3 : texm3x3 source reg combination t0.gbra is NOT allowed";
string PS_010 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.gbra "
	"mov r0, c0 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_1_3 : texm3x3 source reg combination t0.brga is NOT allowed";
string PS_011 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.brga "
	"mov r0, c0 ";

PS_CRITERIA PS_012_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_1_3 : texm3x3 source reg combination t0.abgr is NOT allowed";
string PS_012 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.abgr "
	"mov r0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_1_3 : texm3x3 source reg combination t0.agrb is NOT allowed";
string PS_013 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.agrb "
	"mov r0, c0 ";

PS_CRITERIA PS_014_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_1_3 : texm3x3 source reg combination t0.rbga is NOT allowed";
string PS_014 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.rbga "
	"mov r0, c0 ";

PS_CRITERIA PS_015_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_1_3 : texm3x3 source reg combination t0.rgab is NOT allowed";
string PS_015 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.rgab "
	"mov r0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_1_3 : texm3x3 source reg combination t0.bgr is NOT allowed";
string PS_016 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.bgr "
	"mov r0, c0 ";

PS_CRITERIA PS_017_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_1_3 : texm3x3 source reg combination t0.rbg is NOT allowed";
string PS_017 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.rbg "
	"mov r0, c0 ";

PS_CRITERIA PS_018_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_1_3 : texm3x3 source reg combination t0.gar is NOT allowed";
string PS_018 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.gar "
	"mov r0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_1_3 : texm3x3 source reg combination t0.gr is NOT allowed";
string PS_019 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.gr "
	"mov r0, c0 ";

PS_CRITERIA PS_020_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_1_3 : texm3x3 source reg combination t0.ab is NOT allowed";
string PS_020 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.ab "
	"mov r0, c0 ";

PS_CRITERIA PS_021_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_1_3 : texm3x3 source reg combination t0.bg is NOT allowed";
string PS_021 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0.bg "
	"mov r0, c0 ";

PS_CRITERIA PS_022_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_1_3 : texm3x3 dest t# is allowed";
string PS_022 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_1_3 : texm3x3 _x2 dest t# is NOT allowed";
string PS_023 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_x2 t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_1_3 : texm3x3 _x4 dest t# is NOT allowed";
string PS_024 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_x4 t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_025_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_1_3 : texm3x3 _d2 dest t# is NOT allowed";
string PS_025 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_d2 t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_1_3 : texm3x3 _sat dest t# is NOT allowed";
string PS_026 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_sat t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_1_3 : texm3x3 dest v# is NOT allowed";
string PS_027 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3 v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_1_3 : texm3x3 _x2 dest v# is NOT allowed";
string PS_028 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_x2 v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_1_3 : texm3x3 _x4 dest v# is NOT allowed";
string PS_029 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_x4 v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_1_3 : texm3x3 _d2 dest v# is NOT allowed";
string PS_030 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_d2 v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_1_3 : texm3x3 _sat dest v# is NOT allowed";
string PS_031 = 
	"ps_1_3 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3_sat v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_032_