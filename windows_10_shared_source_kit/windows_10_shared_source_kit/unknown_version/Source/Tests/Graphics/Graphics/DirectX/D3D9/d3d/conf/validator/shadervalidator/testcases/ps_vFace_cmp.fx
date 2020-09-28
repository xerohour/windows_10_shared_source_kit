#include "Test_Include.fx"

int Test_Count = 42;

string TestInfo
<
    string TestType = "PS";
>
= "ps_vFace_cmp";


string PS_001_Desc = "ps_3_0 : cmp vFace allowed";
string PS_001 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_001_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_002_Desc = "ps_3_0 : cmp vFace_abs - abs on vFace not allowed";
string PS_002 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_002_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_003_Desc = "ps_3_sw : cmp vFace allowed";
string PS_003 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_003_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_004_Desc = "ps_3_sw : cmp vFace_abs - abs on vFace not allowed";
string PS_004 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_004_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_005_Desc = "ps_3_0 : cmp vFace.x swizzle not allowed";
string PS_005 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.x, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_005_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_006_Desc = "ps_3_0 : cmp vFace.x_abs - abs on vFace not allowed";
string PS_006 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.x_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_006_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_007_Desc = "ps_3_sw : cmp vFace.x swizzle not allowed";
string PS_007 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.x, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_007_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_008_Desc = "ps_3_sw : cmp vFace.x_abs - abs on vFace not allowed";
string PS_008 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.x_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_008_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_009_Desc = "ps_3_0 : cmp vFace.xyzw swizzle not allowed";
string PS_009 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.xyzw, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_009_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_010_Desc = "ps_3_0 : cmp vFace.xyzw_abs - abs on vFace not allowed";
string PS_010 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.xyzw_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_010_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_011_Desc = "ps_3_sw : cmp vFace.xyzw swizzle not allowed";
string PS_011 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.xyzw, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_011_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_012_Desc = "ps_3_sw : cmp vFace.xyzw_abs - abs on vFace not allowed";
string PS_012 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.xyzw_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_012_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_013_Desc = "ps_3_0 : cmp vFace.xy swizzle not allowed";
string PS_013 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.xy, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_013_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_014_Desc = "ps_3_0 : cmp vFace.xy_abs - abs on vFace not allowed";
string PS_014 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.xy_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_014_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_015_Desc = "ps_3_sw : cmp vFace.xy swizzle not allowed";
string PS_015 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.xy, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_015_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_016_Desc = "ps_3_sw : cmp vFace.xy_abs - abs on vFace not allowed";
string PS_016 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace.xy_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_016_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_017_Desc = "ps_3_0 : cmp -vFace allowed";
string PS_017 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_017_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_018_Desc = "ps_3_0 : cmp -vFace_abs - abs on vFace not allowed";
string PS_018 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_018_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_019_Desc = "ps_3_sw : cmp -vFace allowed";
string PS_019 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_019_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_020_Desc = "ps_3_sw : cmp -vFace_abs - abs on vFace not allowed";
string PS_020 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_020_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_021_Desc = "ps_3_0 : cmp -vFace.x swizzle not allowed";
string PS_021 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.x, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_021_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_022_Desc = "ps_3_0 : cmp -vFace.x_abs - abs on vFace not allowed";
string PS_022 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.x_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_022_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_023_Desc = "ps_3_sw : cmp -vFace.x swizzle not allowed";
string PS_023 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.x, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_023_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_024_Desc = "ps_3_sw : cmp -vFace.x_abs - abs on vFace not allowed";
string PS_024 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.x_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_024_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_025_Desc = "ps_3_0 : cmp -vFace.xyzw swizzle not allowed";
string PS_025 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.xyzw, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_025_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_026_Desc = "ps_3_0 : cmp -vFace.xyzw_abs - abs on vFace not allowed";
string PS_026 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.xyzw_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_026_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_027_Desc = "ps_3_sw : cmp -vFace.xyzw swizzle not allowed";
string PS_027 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.xyzw, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_027_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_028_Desc = "ps_3_sw : cmp -vFace.xyzw_abs - abs on vFace not allowed";
string PS_028 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.xyzw_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_028_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_029_Desc = "ps_3_0 : cmp -vFace.xy swizzle not allowed";
string PS_029 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.xy, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_029_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_030_Desc = "ps_3_0 : cmp -vFace.xy_abs - abs on vFace not allowed";
string PS_030 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.xy_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_030_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_031_Desc = "ps_3_sw : cmp -vFace.xy swizzle not allowed";
string PS_031 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.xy, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_031_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_032_Desc = "ps_3_sw : cmp -vFace.xy_abs - abs on vFace not allowed";
string PS_032 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, -vFace.xy_abs, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_032_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_033_Desc = "ps_3_0 : no decl vFace - error";
string PS_033 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"cmp r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_033_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_034_Desc = "ps_3_sw : no decl vFace - error";
string PS_034 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"cmp r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_034_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };


string PS_035_Desc = "ps_3_0 : cmp vFace allowed";
string PS_035 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_035_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_036_Desc = "ps_3_0 : cmp_sat vFace allowed";
string PS_036 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp_sat r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_036_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_037_Desc = "ps_3_sw : cmp vFace allowed";
string PS_037 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_037_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_038_Desc = "ps_3_sw : cmp_sat vFace allowed";
string PS_038 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp_sat r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_038_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_039_Desc = "ps_3_0 : cmp_pp vFace allowed";
string PS_039 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp_pp r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_039_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_040_Desc = "ps_3_0 : cmp_sat_pp vFace allowed";
string PS_040 =
	"ps_3_0 "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp_sat_pp r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_040_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_041_Desc = "ps_3_sw : cmp_pp vFace allowed";
string PS_041 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp_pp r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_041_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


string PS_042_Desc = "ps_3_sw : cmp_sat_pp vFace allowed";
string PS_042 =
	"ps_3_sw "
	"def c0, 1, 0, 0, 1 "
	"dcl vFace "
	"cmp_sat_pp r0, vFace, c0.x, c0.y "
	"mov oC0, r0 ";
PS_CRITERIA PS_042_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };


// End ps_vFace_cmp