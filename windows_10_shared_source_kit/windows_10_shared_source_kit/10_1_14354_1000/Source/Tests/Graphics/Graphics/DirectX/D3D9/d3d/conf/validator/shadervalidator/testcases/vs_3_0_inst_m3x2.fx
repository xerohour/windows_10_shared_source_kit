#include "Test_Include.fx"

int Test_Count = 406;

string TestInfo
<
	string TestType = "VS";
>
= "vs_3_0_inst_m3x2";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_3_0 : m3x2 source reg combination v0, c0 is allowed";
string VS_001 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0 "
	" ";

VS_CRITERIA VS_002_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_3_0 : m3x2 source reg combination -v0, c0 is allowed";
string VS_002 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, -v0, c0 "
	" ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_3_0 : m3x2 source reg combination v0_abs, c0 is allowed";
string VS_003 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0_abs, c0 "
	" ";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_3_0 : m3x2 source reg combination -v0_abs, c0 is allowed";
string VS_004 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, -v0_abs, c0 "
	" ";

VS_CRITERIA VS_005_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_3_0 : m3x2 source reg combination v0, c0 is allowed";
string VS_005 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0 "
	" ";

VS_CRITERIA VS_006_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_3_0 : m3x2 source reg combination v0.x, c0 is allowed";
string VS_006 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.x, c0 "
	" ";

VS_CRITERIA VS_007_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_3_0 : m3x2 source reg combination v0.y, c0 is allowed";
string VS_007 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.y, c0 "
	" ";

VS_CRITERIA VS_008_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_3_0 : m3x2 source reg combination v0.z, c0 is allowed";
string VS_008 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.z, c0 "
	" ";

VS_CRITERIA VS_009_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_3_0 : m3x2 source reg combination v0.w, c0 is allowed";
string VS_009 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.w, c0 "
	" ";

VS_CRITERIA VS_010_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_3_0 : m3x2 source reg combination v0.yzxw, c0 is allowed";
string VS_010 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.yzxw, c0 "
	" ";

VS_CRITERIA VS_011_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_3_0 : m3x2 source reg combination v0.zxyw, c0 is allowed";
string VS_011 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.zxyw, c0 "
	" ";

VS_CRITERIA VS_012_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_3_0 : m3x2 source reg combination v0.wzyx, c0 is allowed";
string VS_012 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.wzyx, c0 "
	" ";

VS_CRITERIA VS_013_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_3_0 : m3x2 source reg combination v0.wyxz, c0 is allowed";
string VS_013 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.wyxz, c0 "
	" ";

VS_CRITERIA VS_014_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_3_0 : m3x2 source reg combination v0.xzyw, c0 is allowed";
string VS_014 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.xzyw, c0 "
	" ";

VS_CRITERIA VS_015_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_3_0 : m3x2 source reg combination v0.xywz, c0 is allowed";
string VS_015 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.xywz, c0 "
	" ";

VS_CRITERIA VS_016_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_3_0 : m3x2 source reg combination v0.zyx, c0 is allowed";
string VS_016 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.zyx, c0 "
	" ";

VS_CRITERIA VS_017_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_3_0 : m3x2 source reg combination v0.xzy, c0 is allowed";
string VS_017 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.xzy, c0 "
	" ";

VS_CRITERIA VS_018_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_3_0 : m3x2 source reg combination v0.ywx, c0 is allowed";
string VS_018 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.ywx, c0 "
	" ";

VS_CRITERIA VS_019_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_3_0 : m3x2 source reg combination v0.yx, c0 is allowed";
string VS_019 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.yx, c0 "
	" ";

VS_CRITERIA VS_020_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_3_0 : m3x2 source reg combination v0.wz, c0 is allowed";
string VS_020 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.wz, c0 "
	" ";

VS_CRITERIA VS_021_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_3_0 : m3x2 source reg combination v0.zy, c0 is allowed";
string VS_021 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0.zy, c0 "
	" ";

VS_CRITERIA VS_022_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_3_0 : m3x2 source reg combination c0, c1 is NOT allowed";
string VS_022 = 
	"vs_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"m3x2 r0.xy, c0, c1 "
	" ";

VS_CRITERIA VS_023_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_3_0 : m3x2 source reg combination r0, v0 is NOT allowed";
string VS_023 = 
	"vs_3_0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"m3x2 r0.xy, r0, v0 "
	" ";

VS_CRITERIA VS_024_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_3_0 : m3x2 source reg combination r0, r0 is NOT allowed";
string VS_024 = 
	"vs_3_0 "
	"mov r0, c0 "
	"m3x2 r0.xy, r0, r0 "
	" ";

VS_CRITERIA VS_025_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_3_0 : m3x2 source reg combination r0, a0 is NOT allowed";
string VS_025 = 
	"vs_3_0 "
	"mov r0, c0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, r0, a0 "
	" ";

VS_CRITERIA VS_026_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_3_0 : m3x2 source reg combination r0, b0 is NOT allowed";
string VS_026 = 
	"vs_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"m3x2 r0.xy, r0, b0 "
	" ";

VS_CRITERIA VS_027_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_3_0 : m3x2 source reg combination r0, i0 is NOT allowed";
string VS_027 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"m3x2 r0.xy, r0, i0 "
	" ";

VS_CRITERIA VS_028_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_3_0 : m3x2 source reg combination r0, aL is NOT allowed";
string VS_028 = 
	"vs_3_0 "
	"mov r0, c0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, r0, aL "
	" ";

VS_CRITERIA VS_029_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_3_0 : m3x2 source reg combination r0, p0 is NOT allowed";
string VS_029 = 
	"vs_3_0 "
	"mov r0, c0 "
	"m3x2 r0.xy, r0, p0 "
	" ";

VS_CRITERIA VS_030_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_3_0 : m3x2 source reg combination r0, s0 is NOT allowed";
string VS_030 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"m3x2 r0.xy, r0, s0 "
	" ";

VS_CRITERIA VS_031_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_3_0 : m3x2 source reg combination r0, o0 is NOT allowed";
string VS_031 = 
	"vs_3_0 "
	"mov r0, c0 "
	"m3x2 r0.xy, r0, o0 "
	" ";

VS_CRITERIA VS_032_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_3_0 : m3x2 source reg combination a0, v0 is NOT allowed";
string VS_032 = 
	"vs_3_0 "
	"dcl_depth v0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, a0, v0 "
	" ";

VS_CRITERIA VS_033_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_3_0 : m3x2 source reg combination a0, r0 is NOT allowed";
string VS_033 = 
	"vs_3_0 "
	"mova a0.x, c0.x "
	"mov r0, c0 "
	"m3x2 r0.xy, a0, r0 "
	" ";

VS_CRITERIA VS_034_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_3_0 : m3x2 source reg combination a0, a0 is NOT allowed";
string VS_034 = 
	"vs_3_0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, a0, a0 "
	" ";

VS_CRITERIA VS_035_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_3_0 : m3x2 source reg combination a0, b0 is NOT allowed";
string VS_035 = 
	"vs_3_0 "
	"defb b0, true "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, a0, b0 "
	" ";

VS_CRITERIA VS_036_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_3_0 : m3x2 source reg combination a0, i0 is NOT allowed";
string VS_036 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, a0, i0 "
	" ";

VS_CRITERIA VS_037_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_3_0 : m3x2 source reg combination a0, aL is NOT allowed";
string VS_037 = 
	"vs_3_0 "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, a0, aL "
	" ";

VS_CRITERIA VS_038_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_3_0 : m3x2 source reg combination a0, p0 is NOT allowed";
string VS_038 = 
	"vs_3_0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, a0, p0 "
	" ";

VS_CRITERIA VS_039_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_3_0 : m3x2 source reg combination a0, s0 is NOT allowed";
string VS_039 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, a0, s0 "
	" ";

VS_CRITERIA VS_040_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_3_0 : m3x2 source reg combination a0, o0 is NOT allowed";
string VS_040 = 
	"vs_3_0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, a0, o0 "
	" ";

VS_CRITERIA VS_041_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_3_0 : m3x2 source reg combination b0, v0 is NOT allowed";
string VS_041 = 
	"vs_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"m3x2 r0.xy, b0, v0 "
	" ";

VS_CRITERIA VS_042_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_3_0 : m3x2 source reg combination b0, r0 is NOT allowed";
string VS_042 = 
	"vs_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"m3x2 r0.xy, b0, r0 "
	" ";

VS_CRITERIA VS_043_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_3_0 : m3x2 source reg combination b0, a0 is NOT allowed";
string VS_043 = 
	"vs_3_0 "
	"defb b0, true "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, b0, a0 "
	" ";

VS_CRITERIA VS_044_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_3_0 : m3x2 source reg combination b0, b0 is NOT allowed";
string VS_044 = 
	"vs_3_0 "
	"defb b0, true "
	"m3x2 r0.xy, b0, b0 "
	" ";

VS_CRITERIA VS_045_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_3_0 : m3x2 source reg combination b0, i0 is NOT allowed";
string VS_045 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"m3x2 r0.xy, b0, i0 "
	" ";

VS_CRITERIA VS_046_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_3_0 : m3x2 source reg combination b0, aL is NOT allowed";
string VS_046 = 
	"vs_3_0 "
	"defb b0, true "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, b0, aL "
	" ";

VS_CRITERIA VS_047_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_3_0 : m3x2 source reg combination b0, p0 is NOT allowed";
string VS_047 = 
	"vs_3_0 "
	"defb b0, true "
	"m3x2 r0.xy, b0, p0 "
	" ";

VS_CRITERIA VS_048_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_3_0 : m3x2 source reg combination b0, s0 is NOT allowed";
string VS_048 = 
	"vs_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"m3x2 r0.xy, b0, s0 "
	" ";

VS_CRITERIA VS_049_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_3_0 : m3x2 source reg combination b0, o0 is NOT allowed";
string VS_049 = 
	"vs_3_0 "
	"defb b0, true "
	"m3x2 r0.xy, b0, o0 "
	" ";

VS_CRITERIA VS_050_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_3_0 : m3x2 source reg combination i0, v0 is NOT allowed";
string VS_050 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"m3x2 r0.xy, i0, v0 "
	" ";

VS_CRITERIA VS_051_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_3_0 : m3x2 source reg combination i0, r0 is NOT allowed";
string VS_051 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"m3x2 r0.xy, i0, r0 "
	" ";

VS_CRITERIA VS_052_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_3_0 : m3x2 source reg combination i0, a0 is NOT allowed";
string VS_052 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, i0, a0 "
	" ";

VS_CRITERIA VS_053_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_3_0 : m3x2 source reg combination i0, b0 is NOT allowed";
string VS_053 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"m3x2 r0.xy, i0, b0 "
	" ";

VS_CRITERIA VS_054_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_3_0 : m3x2 source reg combination i0, i0 is NOT allowed";
string VS_054 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"m3x2 r0.xy, i0, i0 "
	" ";

VS_CRITERIA VS_055_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_3_0 : m3x2 source reg combination i0, aL is NOT allowed";
string VS_055 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, i0, aL "
	" ";

VS_CRITERIA VS_056_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_3_0 : m3x2 source reg combination i0, p0 is NOT allowed";
string VS_056 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"m3x2 r0.xy, i0, p0 "
	" ";

VS_CRITERIA VS_057_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_3_0 : m3x2 source reg combination i0, s0 is NOT allowed";
string VS_057 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"m3x2 r0.xy, i0, s0 "
	" ";

VS_CRITERIA VS_058_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_3_0 : m3x2 source reg combination i0, o0 is NOT allowed";
string VS_058 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"m3x2 r0.xy, i0, o0 "
	" ";

VS_CRITERIA VS_059_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_3_0 : m3x2 source reg combination aL, v0 is NOT allowed";
string VS_059 = 
	"vs_3_0 "
	"dcl_depth v0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, aL, v0 "
	" ";

VS_CRITERIA VS_060_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_3_0 : m3x2 source reg combination aL, r0 is NOT allowed";
string VS_060 = 
	"vs_3_0 "
	"mova aL.x, c0.x "
	"mov r0, c0 "
	"m3x2 r0.xy, aL, r0 "
	" ";

VS_CRITERIA VS_061_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_3_0 : m3x2 source reg combination aL, a0 is NOT allowed";
string VS_061 = 
	"vs_3_0 "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, aL, a0 "
	" ";

VS_CRITERIA VS_062_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_3_0 : m3x2 source reg combination aL, b0 is NOT allowed";
string VS_062 = 
	"vs_3_0 "
	"defb b0, true "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, aL, b0 "
	" ";

VS_CRITERIA VS_063_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_3_0 : m3x2 source reg combination aL, i0 is NOT allowed";
string VS_063 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, aL, i0 "
	" ";

VS_CRITERIA VS_064_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_3_0 : m3x2 source reg combination aL, aL is NOT allowed";
string VS_064 = 
	"vs_3_0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, aL, aL "
	" ";

VS_CRITERIA VS_065_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_3_0 : m3x2 source reg combination aL, p0 is NOT allowed";
string VS_065 = 
	"vs_3_0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, aL, p0 "
	" ";

VS_CRITERIA VS_066_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_3_0 : m3x2 source reg combination aL, s0 is NOT allowed";
string VS_066 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, aL, s0 "
	" ";

VS_CRITERIA VS_067_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_3_0 : m3x2 source reg combination aL, o0 is NOT allowed";
string VS_067 = 
	"vs_3_0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, aL, o0 "
	" ";

VS_CRITERIA VS_068_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_3_0 : m3x2 source reg combination p0, v0 is NOT allowed";
string VS_068 = 
	"vs_3_0 "
	"dcl_depth v0 "
	"m3x2 r0.xy, p0, v0 "
	" ";

VS_CRITERIA VS_069_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_3_0 : m3x2 source reg combination p0, r0 is NOT allowed";
string VS_069 = 
	"vs_3_0 "
	"mov r0, c0 "
	"m3x2 r0.xy, p0, r0 "
	" ";

VS_CRITERIA VS_070_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_3_0 : m3x2 source reg combination p0, a0 is NOT allowed";
string VS_070 = 
	"vs_3_0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, p0, a0 "
	" ";

VS_CRITERIA VS_071_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_3_0 : m3x2 source reg combination p0, b0 is NOT allowed";
string VS_071 = 
	"vs_3_0 "
	"defb b0, true "
	"m3x2 r0.xy, p0, b0 "
	" ";

VS_CRITERIA VS_072_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_3_0 : m3x2 source reg combination p0, i0 is NOT allowed";
string VS_072 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"m3x2 r0.xy, p0, i0 "
	" ";

VS_CRITERIA VS_073_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_3_0 : m3x2 source reg combination p0, aL is NOT allowed";
string VS_073 = 
	"vs_3_0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, p0, aL "
	" ";

VS_CRITERIA VS_074_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_3_0 : m3x2 source reg combination p0, p0 is NOT allowed";
string VS_074 = 
	"vs_3_0 "
	"m3x2 r0.xy, p0, p0 "
	" ";

VS_CRITERIA VS_075_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_3_0 : m3x2 source reg combination p0, s0 is NOT allowed";
string VS_075 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"m3x2 r0.xy, p0, s0 "
	" ";

VS_CRITERIA VS_076_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_3_0 : m3x2 source reg combination p0, o0 is NOT allowed";
string VS_076 = 
	"vs_3_0 "
	"m3x2 r0.xy, p0, o0 "
	" ";

VS_CRITERIA VS_077_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_3_0 : m3x2 source reg combination s0, v0 is NOT allowed";
string VS_077 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"dcl_depth v0 "
	"m3x2 r0.xy, s0, v0 "
	" ";

VS_CRITERIA VS_078_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_3_0 : m3x2 source reg combination s0, r0 is NOT allowed";
string VS_078 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"m3x2 r0.xy, s0, r0 "
	" ";

VS_CRITERIA VS_079_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_3_0 : m3x2 source reg combination s0, a0 is NOT allowed";
string VS_079 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, s0, a0 "
	" ";

VS_CRITERIA VS_080_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_3_0 : m3x2 source reg combination s0, b0 is NOT allowed";
string VS_080 = 
	"vs_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"m3x2 r0.xy, s0, b0 "
	" ";

VS_CRITERIA VS_081_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_3_0 : m3x2 source reg combination s0, i0 is NOT allowed";
string VS_081 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"m3x2 r0.xy, s0, i0 "
	" ";

VS_CRITERIA VS_082_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_3_0 : m3x2 source reg combination s0, aL is NOT allowed";
string VS_082 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, s0, aL "
	" ";

VS_CRITERIA VS_083_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_3_0 : m3x2 source reg combination s0, p0 is NOT allowed";
string VS_083 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"m3x2 r0.xy, s0, p0 "
	" ";

VS_CRITERIA VS_084_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_3_0 : m3x2 source reg combination s0, s0 is NOT allowed";
string VS_084 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"m3x2 r0.xy, s0, s0 "
	" ";

VS_CRITERIA VS_085_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_3_0 : m3x2 source reg combination s0, o0 is NOT allowed";
string VS_085 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"m3x2 r0.xy, s0, o0 "
	" ";

VS_CRITERIA VS_086_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_3_0 : m3x2 source reg combination o0, v0 is NOT allowed";
string VS_086 = 
	"vs_3_0 "
	"dcl_depth v0 "
	"m3x2 r0.xy, o0, v0 "
	" ";

VS_CRITERIA VS_087_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_3_0 : m3x2 source reg combination o0, r0 is NOT allowed";
string VS_087 = 
	"vs_3_0 "
	"mov r0, c0 "
	"m3x2 r0.xy, o0, r0 "
	" ";

VS_CRITERIA VS_088_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_3_0 : m3x2 source reg combination o0, a0 is NOT allowed";
string VS_088 = 
	"vs_3_0 "
	"mova a0.x, c0.x "
	"m3x2 r0.xy, o0, a0 "
	" ";

VS_CRITERIA VS_089_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_089_Desc = "vs_3_0 : m3x2 source reg combination o0, b0 is NOT allowed";
string VS_089 = 
	"vs_3_0 "
	"defb b0, true "
	"m3x2 r0.xy, o0, b0 "
	" ";

VS_CRITERIA VS_090_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_3_0 : m3x2 source reg combination o0, i0 is NOT allowed";
string VS_090 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"m3x2 r0.xy, o0, i0 "
	" ";

VS_CRITERIA VS_091_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_3_0 : m3x2 source reg combination o0, aL is NOT allowed";
string VS_091 = 
	"vs_3_0 "
	"mova aL.x, c0.x "
	"m3x2 r0.xy, o0, aL "
	" ";

VS_CRITERIA VS_092_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_3_0 : m3x2 source reg combination o0, p0 is NOT allowed";
string VS_092 = 
	"vs_3_0 "
	"m3x2 r0.xy, o0, p0 "
	" ";

VS_CRITERIA VS_093_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_3_0 : m3x2 source reg combination o0, s0 is NOT allowed";
string VS_093 = 
	"vs_3_0 "
	"dcl_volume s0 "
	"m3x2 r0.xy, o0, s0 "
	" ";

VS_CRITERIA VS_094_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_3_0 : m3x2 source reg combination o0, o0 is NOT allowed";
string VS_094 = 
	"vs_3_0 "
	"m3x2 r0.xy, o0, o0 "
	" ";

VS_CRITERIA VS_095_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_3_0 : m3x2 source reg combination v0, -c0 is NOT allowed";
string VS_095 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, -c0 "
	" ";

VS_CRITERIA VS_096_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_3_0 : m3x2 source reg combination v0, c0_abs is NOT allowed";
string VS_096 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0_abs "
	" ";

VS_CRITERIA VS_097_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_3_0 : m3x2 source reg combination v0, -c0_abs is NOT allowed";
string VS_097 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, -c0_abs "
	" ";

VS_CRITERIA VS_098_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.x is NOT allowed";
string VS_098 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.x "
	" ";

VS_CRITERIA VS_099_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.y is NOT allowed";
string VS_099 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.y "
	" ";

VS_CRITERIA VS_100_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.z is NOT allowed";
string VS_100 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.z "
	" ";

VS_CRITERIA VS_101_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.w is NOT allowed";
string VS_101 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.w "
	" ";

VS_CRITERIA VS_102_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.yzxw is NOT allowed";
string VS_102 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.yzxw "
	" ";

VS_CRITERIA VS_103_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.zxyw is NOT allowed";
string VS_103 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.zxyw "
	" ";

VS_CRITERIA VS_104_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.wzyx is NOT allowed";
string VS_104 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.wzyx "
	" ";

VS_CRITERIA VS_105_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.wyxz is NOT allowed";
string VS_105 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.wyxz "
	" ";

VS_CRITERIA VS_106_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.xzyw is NOT allowed";
string VS_106 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.xzyw "
	" ";

VS_CRITERIA VS_107_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.xywz is NOT allowed";
string VS_107 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.xywz "
	" ";

VS_CRITERIA VS_108_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.zyx is NOT allowed";
string VS_108 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.zyx "
	" ";

VS_CRITERIA VS_109_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.xzy is NOT allowed";
string VS_109 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.xzy "
	" ";

VS_CRITERIA VS_110_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.ywx is NOT allowed";
string VS_110 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.ywx "
	" ";

VS_CRITERIA VS_111_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.yx is NOT allowed";
string VS_111 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.yx "
	" ";

VS_CRITERIA VS_112_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.wz is NOT allowed";
string VS_112 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.wz "
	" ";

VS_CRITERIA VS_113_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_3_0 : m3x2 source reg combination v0, c0.zy is NOT allowed";
string VS_113 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0.zy "
	" ";

VS_CRITERIA VS_114_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_3_0 : m3x2 write mask .xy is allowed";
string VS_114 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xy, v0, c0 "
	" ";

VS_CRITERIA VS_115_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_3_0 : m3x2 write mask .x is NOT allowed";
string VS_115 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.x, v0, c0 "
	" ";

VS_CRITERIA VS_116_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_3_0 : m3x2 write mask .y is NOT allowed";
string VS_116 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.y, v0, c0 "
	" ";

VS_CRITERIA VS_117_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_3_0 : m3x2 write mask .z is NOT allowed";
string VS_117 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.z, v0, c0 "
	" ";

VS_CRITERIA VS_118_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_3_0 : m3x2 write mask .w is NOT allowed";
string VS_118 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.w, v0, c0 "
	" ";

VS_CRITERIA VS_119_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_3_0 : m3x2 write mask .yz is NOT allowed";
string VS_119 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.yz, v0, c0 "
	" ";

VS_CRITERIA VS_120_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_3_0 : m3x2 write mask .zw is NOT allowed";
string VS_120 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.zw, v0, c0 "
	" ";

VS_CRITERIA VS_121_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_3_0 : m3x2 write mask .xz is NOT allowed";
string VS_121 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xz, v0, c0 "
	" ";

VS_CRITERIA VS_122_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_3_0 : m3x2 write mask .xw is NOT allowed";
string VS_122 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xw, v0, c0 "
	" ";

VS_CRITERIA VS_123_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_3_0 : m3x2 write mask .yw is NOT allowed";
string VS_123 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.yw, v0, c0 "
	" ";

VS_CRITERIA VS_124_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_3_0 : m3x2 write mask .xyz is NOT allowed";
string VS_124 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xyz, v0, c0 "
	" ";

VS_CRITERIA VS_125_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_3_0 : m3x2 write mask .yzw is NOT allowed";
string VS_125 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.yzw, v0, c0 "
	" ";

VS_CRITERIA VS_126_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_3_0 : m3x2 write mask .xzw is NOT allowed";
string VS_126 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xzw, v0, c0 "
	" ";

VS_CRITERIA VS_127_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_3_0 : m3x2 write mask .xyzw is NOT allowed";
string VS_127 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.xyzw, v0, c0 "
	" ";

VS_CRITERIA VS_128_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_3_0 : m3x2 write mask .yx is NOT allowed";
string VS_128 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.yx, v0, c0 "
	" ";

VS_CRITERIA VS_129_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_3_0 : m3x2 write mask .zx is NOT allowed";
string VS_129 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.zx, v0, c0 "
	" ";

VS_CRITERIA VS_130_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_130_Desc = "vs_3_0 : m3x2 write mask .zy is NOT allowed";
string VS_130 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.zy, v0, c0 "
	" ";

VS_CRITERIA VS_131_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_3_0 : m3x2 write mask .wx is NOT allowed";
string VS_131 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.wx, v0, c0 "
	" ";

VS_CRITERIA VS_132_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_3_0 : m3x2 write mask .wz is NOT allowed";
string VS_132 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.wz, v0, c0 "
	" ";

VS_CRITERIA VS_133_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_3_0 : m3x2 write mask .wy is NOT allowed";
string VS_133 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.wy, v0, c0 "
	" ";

VS_CRITERIA VS_134_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_3_0 : m3x2 write mask .zyx is NOT allowed";
string VS_134 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.zyx, v0, c0 "
	" ";

VS_CRITERIA VS_135_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_3_0 : m3x2 write mask .wzy is NOT allowed";
string VS_135 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.wzy, v0, c0 "
	" ";

VS_CRITERIA VS_136_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_3_0 : m3x2 write mask .wzx is NOT allowed";
string VS_136 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.wzx, v0, c0 "
	" ";

VS_CRITERIA VS_137_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_3_0 : m3x2 write mask .wyx is NOT allowed";
string VS_137 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.wyx, v0, c0 "
	" ";

VS_CRITERIA VS_138_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_3_0 : m3x2 write mask .yxw is NOT allowed";
string VS_138 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.yxw, v0, c0 "
	" ";

VS_CRITERIA VS_139_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_3_0 : m3x2 write mask .wzyx is NOT allowed";
string VS_139 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.wzyx, v0, c0 "
	" ";

VS_CRITERIA VS_140_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_3_0 : m3x2 write mask .zxwy is NOT allowed";
string VS_140 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"m3x2 r0.zxwy, v0, c0 "
	" ";

VS_CRITERIA VS_141_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_3_0 : m3x2 is allowed in a 1 level if b0 block";
string VS_141 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 "
	"	m3x2 r0.xy, v0, c0 "
	"endif ";

VS_CRITERIA VS_142_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_3_0 : m3x2 is allowed in a 8 level if b0 block";
string VS_142 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								m3x2 r0.xy, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_143_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_3_0 : m3x2 is allowed in a 16 level if b0 block";
string VS_143 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_144_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_3_0 : m3x2 is allowed in a 24 level if b0 block";
string VS_144 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_145_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level if b0 block";
string VS_145 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								if b0 "
	"									if b0 "
	"										if b0 "
	"											if b0 "
	"												if b0 "
	"													if b0 "
	"														if b0 "
	"															if b0 "
	"																if b0 "
	"																	if b0 "
	"																		if b0 "
	"																			if b0 "
	"																				if b0 "
	"																					if b0 "
	"																						if b0 "
	"																							if b0 "
	"																								if b0 "
	"																									m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_146_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_3_0 : m3x2 is allowed in a 1 level if b0 nop else block";
string VS_146 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 nop else "
	"	m3x2 r0.xy, v0, c0 "
	"endif ";

VS_CRITERIA VS_147_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_3_0 : m3x2 is allowed in a 8 level if b0 nop else block";
string VS_147 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								m3x2 r0.xy, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_148_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_3_0 : m3x2 is allowed in a 16 level if b0 nop else block";
string VS_148 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_149_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_149_Desc = "vs_3_0 : m3x2 is allowed in a 24 level if b0 nop else block";
string VS_149 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_150_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level if b0 nop else block";
string VS_150 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								if b0 nop else "
	"									if b0 nop else "
	"										if b0 nop else "
	"											if b0 nop else "
	"												if b0 nop else "
	"													if b0 nop else "
	"														if b0 nop else "
	"															if b0 nop else "
	"																if b0 nop else "
	"																	if b0 nop else "
	"																		if b0 nop else "
	"																			if b0 nop else "
	"																				if b0 nop else "
	"																					if b0 nop else "
	"																						if b0 nop else "
	"																							if b0 nop else "
	"																								if b0 nop else "
	"																									m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_151_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_3_0 : m3x2 is allowed in a 1 level loop aL, i0 block";
string VS_151 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	m3x2 r0.xy, v0, c0 "
	"endloop ";

VS_CRITERIA VS_152_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_3_0 : m3x2 is allowed in a 2 level loop aL, i0 block";
string VS_152 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_153_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_3_0 : m3x2 is allowed in a 3 level loop aL, i0 block";
string VS_153 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			m3x2 r0.xy, v0, c0 "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_154_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_3_0 : m3x2 is allowed in a 4 level loop aL, i0 block";
string VS_154 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				m3x2 r0.xy, v0, c0 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_155_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_3_0 : m3x2 is NOT allowed in a 5 level loop aL, i0 block";
string VS_155 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					m3x2 r0.xy, v0, c0 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_156_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_156_Desc = "vs_3_0 : m3x2 is allowed in a 1 level rep i0 block";
string VS_156 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 "
	"	m3x2 r0.xy, v0, c0 "
	"endrep ";

VS_CRITERIA VS_157_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_3_0 : m3x2 is allowed in a 2 level rep i0 block";
string VS_157 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_158_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_3_0 : m3x2 is allowed in a 3 level rep i0 block";
string VS_158 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			m3x2 r0.xy, v0, c0 "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_159_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_159_Desc = "vs_3_0 : m3x2 is allowed in a 4 level rep i0 block";
string VS_159 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				m3x2 r0.xy, v0, c0 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_160_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_3_0 : m3x2 is NOT allowed in a 5 level rep i0 block";
string VS_160 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					m3x2 r0.xy, v0, c0 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_161_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_3_0 : m3x2 is allowed in a 1 level if_ge c0.x, c0.y block";
string VS_161 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ge c0.x, c0.y "
	"	m3x2 r0.xy, v0, c0 "
	"endif ";

VS_CRITERIA VS_162_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_3_0 : m3x2 is allowed in a 8 level if_ge c0.x, c0.y block";
string VS_162 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		if_ge c0.x, c0.y "
	"			if_ge c0.x, c0.y "
	"				if_ge c0.x, c0.y "
	"					if_ge c0.x, c0.y "
	"						if_ge c0.x, c0.y "
	"							if_ge c0.x, c0.y "
	"								m3x2 r0.xy, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_163_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_3_0 : m3x2 is allowed in a 16 level if_ge c0.x, c0.y block";
string VS_163 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		if_ge c0.x, c0.y "
	"			if_ge c0.x, c0.y "
	"				if_ge c0.x, c0.y "
	"					if_ge c0.x, c0.y "
	"						if_ge c0.x, c0.y "
	"							if_ge c0.x, c0.y "
	"								if_ge c0.x, c0.y "
	"									if_ge c0.x, c0.y "
	"										if_ge c0.x, c0.y "
	"											if_ge c0.x, c0.y "
	"												if_ge c0.x, c0.y "
	"													if_ge c0.x, c0.y "
	"														if_ge c0.x, c0.y "
	"															if_ge c0.x, c0.y "
	"																m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_164_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_3_0 : m3x2 is allowed in a 24 level if_ge c0.x, c0.y block";
string VS_164 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		if_ge c0.x, c0.y "
	"			if_ge c0.x, c0.y "
	"				if_ge c0.x, c0.y "
	"					if_ge c0.x, c0.y "
	"						if_ge c0.x, c0.y "
	"							if_ge c0.x, c0.y "
	"								if_ge c0.x, c0.y "
	"									if_ge c0.x, c0.y "
	"										if_ge c0.x, c0.y "
	"											if_ge c0.x, c0.y "
	"												if_ge c0.x, c0.y "
	"													if_ge c0.x, c0.y "
	"														if_ge c0.x, c0.y "
	"															if_ge c0.x, c0.y "
	"																if_ge c0.x, c0.y "
	"																	if_ge c0.x, c0.y "
	"																		if_ge c0.x, c0.y "
	"																			if_ge c0.x, c0.y "
	"																				if_ge c0.x, c0.y "
	"																					if_ge c0.x, c0.y "
	"																						if_ge c0.x, c0.y "
	"																							if_ge c0.x, c0.y "
	"																								m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_165_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level if_ge c0.x, c0.y block";
string VS_165 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		if_ge c0.x, c0.y "
	"			if_ge c0.x, c0.y "
	"				if_ge c0.x, c0.y "
	"					if_ge c0.x, c0.y "
	"						if_ge c0.x, c0.y "
	"							if_ge c0.x, c0.y "
	"								if_ge c0.x, c0.y "
	"									if_ge c0.x, c0.y "
	"										if_ge c0.x, c0.y "
	"											if_ge c0.x, c0.y "
	"												if_ge c0.x, c0.y "
	"													if_ge c0.x, c0.y "
	"														if_ge c0.x, c0.y "
	"															if_ge c0.x, c0.y "
	"																if_ge c0.x, c0.y "
	"																	if_ge c0.x, c0.y "
	"																		if_ge c0.x, c0.y "
	"																			if_ge c0.x, c0.y "
	"																				if_ge c0.x, c0.y "
	"																					if_ge c0.x, c0.y "
	"																						if_ge c0.x, c0.y "
	"																							if_ge c0.x, c0.y "
	"																								if_ge c0.x, c0.y "
	"																									m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_166_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_3_0 : m3x2 is allowed in a 1 level if_ge c0.x, c0.y nop else block";
string VS_166 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ge c0.x, c0.y nop else "
	"	m3x2 r0.xy, v0, c0 "
	"endif ";

VS_CRITERIA VS_167_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_3_0 : m3x2 is allowed in a 8 level if_ge c0.x, c0.y nop else block";
string VS_167 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		if_ge c0.x, c0.y nop else "
	"			if_ge c0.x, c0.y nop else "
	"				if_ge c0.x, c0.y nop else "
	"					if_ge c0.x, c0.y nop else "
	"						if_ge c0.x, c0.y nop else "
	"							if_ge c0.x, c0.y nop else "
	"								m3x2 r0.xy, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_168_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_168_Desc = "vs_3_0 : m3x2 is allowed in a 16 level if_ge c0.x, c0.y nop else block";
string VS_168 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		if_ge c0.x, c0.y nop else "
	"			if_ge c0.x, c0.y nop else "
	"				if_ge c0.x, c0.y nop else "
	"					if_ge c0.x, c0.y nop else "
	"						if_ge c0.x, c0.y nop else "
	"							if_ge c0.x, c0.y nop else "
	"								if_ge c0.x, c0.y nop else "
	"									if_ge c0.x, c0.y nop else "
	"										if_ge c0.x, c0.y nop else "
	"											if_ge c0.x, c0.y nop else "
	"												if_ge c0.x, c0.y nop else "
	"													if_ge c0.x, c0.y nop else "
	"														if_ge c0.x, c0.y nop else "
	"															if_ge c0.x, c0.y nop else "
	"																m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_169_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_169_Desc = "vs_3_0 : m3x2 is allowed in a 24 level if_ge c0.x, c0.y nop else block";
string VS_169 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		if_ge c0.x, c0.y nop else "
	"			if_ge c0.x, c0.y nop else "
	"				if_ge c0.x, c0.y nop else "
	"					if_ge c0.x, c0.y nop else "
	"						if_ge c0.x, c0.y nop else "
	"							if_ge c0.x, c0.y nop else "
	"								if_ge c0.x, c0.y nop else "
	"									if_ge c0.x, c0.y nop else "
	"										if_ge c0.x, c0.y nop else "
	"											if_ge c0.x, c0.y nop else "
	"												if_ge c0.x, c0.y nop else "
	"													if_ge c0.x, c0.y nop else "
	"														if_ge c0.x, c0.y nop else "
	"															if_ge c0.x, c0.y nop else "
	"																if_ge c0.x, c0.y nop else "
	"																	if_ge c0.x, c0.y nop else "
	"																		if_ge c0.x, c0.y nop else "
	"																			if_ge c0.x, c0.y nop else "
	"																				if_ge c0.x, c0.y nop else "
	"																					if_ge c0.x, c0.y nop else "
	"																						if_ge c0.x, c0.y nop else "
	"																							if_ge c0.x, c0.y nop else "
	"																								m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_170_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_170_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level if_ge c0.x, c0.y nop else block";
string VS_170 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		if_ge c0.x, c0.y nop else "
	"			if_ge c0.x, c0.y nop else "
	"				if_ge c0.x, c0.y nop else "
	"					if_ge c0.x, c0.y nop else "
	"						if_ge c0.x, c0.y nop else "
	"							if_ge c0.x, c0.y nop else "
	"								if_ge c0.x, c0.y nop else "
	"									if_ge c0.x, c0.y nop else "
	"										if_ge c0.x, c0.y nop else "
	"											if_ge c0.x, c0.y nop else "
	"												if_ge c0.x, c0.y nop else "
	"													if_ge c0.x, c0.y nop else "
	"														if_ge c0.x, c0.y nop else "
	"															if_ge c0.x, c0.y nop else "
	"																if_ge c0.x, c0.y nop else "
	"																	if_ge c0.x, c0.y nop else "
	"																		if_ge c0.x, c0.y nop else "
	"																			if_ge c0.x, c0.y nop else "
	"																				if_ge c0.x, c0.y nop else "
	"																					if_ge c0.x, c0.y nop else "
	"																						if_ge c0.x, c0.y nop else "
	"																							if_ge c0.x, c0.y nop else "
	"																								if_ge c0.x, c0.y nop else "
	"																									m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_171_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_171_Desc = "vs_3_0 : m3x2 is allowed in a 1 level if p0.x block";
string VS_171 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	m3x2 r0.xy, v0, c0 "
	"endif ";

VS_CRITERIA VS_172_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_172_Desc = "vs_3_0 : m3x2 is allowed in a 8 level if p0.x block";
string VS_172 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								m3x2 r0.xy, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_173_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_173_Desc = "vs_3_0 : m3x2 is allowed in a 16 level if p0.x block";
string VS_173 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_174_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_174_Desc = "vs_3_0 : m3x2 is allowed in a 24 level if p0.x block";
string VS_174 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_175_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_175_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level if p0.x block";
string VS_175 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								if p0.x "
	"									if p0.x "
	"										if p0.x "
	"											if p0.x "
	"												if p0.x "
	"													if p0.x "
	"														if p0.x "
	"															if p0.x "
	"																if p0.x "
	"																	if p0.x "
	"																		if p0.x "
	"																			if p0.x "
	"																				if p0.x "
	"																					if p0.x "
	"																						if p0.x "
	"																							if p0.x "
	"																								if p0.x "
	"																									m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_176_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_176_Desc = "vs_3_0 : m3x2 is allowed in a 1 level if p0.x nop else block";
string VS_176 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x nop else "
	"	m3x2 r0.xy, v0, c0 "
	"endif ";

VS_CRITERIA VS_177_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_177_Desc = "vs_3_0 : m3x2 is allowed in a 8 level if p0.x nop else block";
string VS_177 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								m3x2 r0.xy, v0, c0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_178_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_178_Desc = "vs_3_0 : m3x2 is allowed in a 16 level if p0.x nop else block";
string VS_178 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_179_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_179_Desc = "vs_3_0 : m3x2 is allowed in a 24 level if p0.x nop else block";
string VS_179 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_180_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_180_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level if p0.x nop else block";
string VS_180 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								if p0.x nop else "
	"									if p0.x nop else "
	"										if p0.x nop else "
	"											if p0.x nop else "
	"												if p0.x nop else "
	"													if p0.x nop else "
	"														if p0.x nop else "
	"															if p0.x nop else "
	"																if p0.x nop else "
	"																	if p0.x nop else "
	"																		if p0.x nop else "
	"																			if p0.x nop else "
	"																				if p0.x nop else "
	"																					if p0.x nop else "
	"																						if p0.x nop else "
	"																							if p0.x nop else "
	"																								if p0.x nop else "
	"																									m3x2 r0.xy, v0, c0 "
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

VS_CRITERIA VS_181_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_181_Desc = "vs_3_0 : m3x2 is allowed in a 1 level loop aL, i0 break block";
string VS_181 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break "
	"	m3x2 r0.xy, v0, c0 "
	"endloop ";

VS_CRITERIA VS_182_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_182_Desc = "vs_3_0 : m3x2 is NOT allowed in a 8 level loop aL, i0 break block";
string VS_182 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								m3x2 r0.xy, v0, c0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_183_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_183_Desc = "vs_3_0 : m3x2 is NOT allowed in a 16 level loop aL, i0 break block";
string VS_183 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																m3x2 r0.xy, v0, c0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_184_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_184_Desc = "vs_3_0 : m3x2 is NOT allowed in a 24 level loop aL, i0 break block";
string VS_184 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								m3x2 r0.xy, v0, c0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_185_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_185_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level loop aL, i0 break block";
string VS_185 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								loop aL, i0 break "
	"									loop aL, i0 break "
	"										loop aL, i0 break "
	"											loop aL, i0 break "
	"												loop aL, i0 break "
	"													loop aL, i0 break "
	"														loop aL, i0 break "
	"															loop aL, i0 break "
	"																loop aL, i0 break "
	"																	loop aL, i0 break "
	"																		loop aL, i0 break "
	"																			loop aL, i0 break "
	"																				loop aL, i0 break "
	"																					loop aL, i0 break "
	"																						loop aL, i0 break "
	"																							loop aL, i0 break "
	"																								loop aL, i0 break "
	"																									m3x2 r0.xy, v0, c0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_186_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_186_Desc = "vs_3_0 : m3x2 is allowed in a 1 level rep i0 break block";
string VS_186 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break "
	"	m3x2 r0.xy, v0, c0 "
	"endrep ";

VS_CRITERIA VS_187_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_187_Desc = "vs_3_0 : m3x2 is NOT allowed in a 8 level rep i0 break block";
string VS_187 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								m3x2 r0.xy, v0, c0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_188_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_188_Desc = "vs_3_0 : m3x2 is NOT allowed in a 16 level rep i0 break block";
string VS_188 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																m3x2 r0.xy, v0, c0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_189_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_189_Desc = "vs_3_0 : m3x2 is NOT allowed in a 24 level rep i0 break block";
string VS_189 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								m3x2 r0.xy, v0, c0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_190_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_190_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level rep i0 break block";
string VS_190 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								rep i0 break "
	"									rep i0 break "
	"										rep i0 break "
	"											rep i0 break "
	"												rep i0 break "
	"													rep i0 break "
	"														rep i0 break "
	"															rep i0 break "
	"																rep i0 break "
	"																	rep i0 break "
	"																		rep i0 break "
	"																			rep i0 break "
	"																				rep i0 break "
	"																					rep i0 break "
	"																						rep i0 break "
	"																							rep i0 break "
	"																								rep i0 break "
	"																									m3x2 r0.xy, v0, c0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_191_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_191_Desc = "vs_3_0 : m3x2 is allowed in a 1 level loop aL, i0 breakp p0.x block";
string VS_191 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 breakp p0.x "
	"	m3x2 r0.xy, v0, c0 "
	"endloop ";

VS_CRITERIA VS_192_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_192_Desc = "vs_3_0 : m3x2 is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string VS_192 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								m3x2 r0.xy, v0, c0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_193_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_193_Desc = "vs_3_0 : m3x2 is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string VS_193 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																m3x2 r0.xy, v0, c0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_194_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_194_Desc = "vs_3_0 : m3x2 is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string VS_194 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								m3x2 r0.xy, v0, c0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_195_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_195_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string VS_195 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								loop aL, i0 breakp p0.x "
	"									loop aL, i0 breakp p0.x "
	"										loop aL, i0 breakp p0.x "
	"											loop aL, i0 breakp p0.x "
	"												loop aL, i0 breakp p0.x "
	"													loop aL, i0 breakp p0.x "
	"														loop aL, i0 breakp p0.x "
	"															loop aL, i0 breakp p0.x "
	"																loop aL, i0 breakp p0.x "
	"																	loop aL, i0 breakp p0.x "
	"																		loop aL, i0 breakp p0.x "
	"																			loop aL, i0 breakp p0.x "
	"																				loop aL, i0 breakp p0.x "
	"																					loop aL, i0 breakp p0.x "
	"																						loop aL, i0 breakp p0.x "
	"																							loop aL, i0 breakp p0.x "
	"																								loop aL, i0 breakp p0.x "
	"																									m3x2 r0.xy, v0, c0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_196_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_196_Desc = "vs_3_0 : m3x2 is allowed in a 1 level rep i0 breakp p0.x block";
string VS_196 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 breakp p0.x "
	"	m3x2 r0.xy, v0, c0 "
	"endrep ";

VS_CRITERIA VS_197_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_197_Desc = "vs_3_0 : m3x2 is NOT allowed in a 8 level rep i0 breakp p0.x block";
string VS_197 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								m3x2 r0.xy, v0, c0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_198_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_198_Desc = "vs_3_0 : m3x2 is NOT allowed in a 16 level rep i0 breakp p0.x block";
string VS_198 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																m3x2 r0.xy, v0, c0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_199_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_199_Desc = "vs_3_0 : m3x2 is NOT allowed in a 24 level rep i0 breakp p0.x block";
string VS_199 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								m3x2 r0.xy, v0, c0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_200_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_200_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level rep i0 breakp p0.x block";
string VS_200 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								rep i0 breakp p0.x "
	"									rep i0 breakp p0.x "
	"										rep i0 breakp p0.x "
	"											rep i0 breakp p0.x "
	"												rep i0 breakp p0.x "
	"													rep i0 breakp p0.x "
	"														rep i0 breakp p0.x "
	"															rep i0 breakp p0.x "
	"																rep i0 breakp p0.x "
	"																	rep i0 breakp p0.x "
	"																		rep i0 breakp p0.x "
	"																			rep i0 breakp p0.x "
	"																				rep i0 breakp p0.x "
	"																					rep i0 breakp p0.x "
	"																						rep i0 breakp p0.x "
	"																							rep i0 breakp p0.x "
	"																								rep i0 breakp p0.x "
	"																									m3x2 r0.xy, v0, c0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_201_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_201_Desc = "vs_3_0 : m3x2 is allowed in a 1 level loop aL, i0 break_ge c0.x, c0.y block";
string VS_201 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	m3x2 r0.xy, v0, c0 "
	"endloop ";

VS_CRITERIA VS_202_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_202_Desc = "vs_3_0 : m3x2 is NOT allowed in a 8 level loop aL, i0 break_ge c0.x, c0.y block";
string VS_202 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								m3x2 r0.xy, v0, c0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_203_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_203_Desc = "vs_3_0 : m3x2 is NOT allowed in a 16 level loop aL, i0 break_ge c0.x, c0.y block";
string VS_203 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								loop aL, i0 break_ge c0.x, c0.y "
	"									loop aL, i0 break_ge c0.x, c0.y "
	"										loop aL, i0 break_ge c0.x, c0.y "
	"											loop aL, i0 break_ge c0.x, c0.y "
	"												loop aL, i0 break_ge c0.x, c0.y "
	"													loop aL, i0 break_ge c0.x, c0.y "
	"														loop aL, i0 break_ge c0.x, c0.y "
	"															loop aL, i0 break_ge c0.x, c0.y "
	"																m3x2 r0.xy, v0, c0 "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_204_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_204_Desc = "vs_3_0 : m3x2 is NOT allowed in a 24 level loop aL, i0 break_ge c0.x, c0.y block";
string VS_204 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								loop aL, i0 break_ge c0.x, c0.y "
	"									loop aL, i0 break_ge c0.x, c0.y "
	"										loop aL, i0 break_ge c0.x, c0.y "
	"											loop aL, i0 break_ge c0.x, c0.y "
	"												loop aL, i0 break_ge c0.x, c0.y "
	"													loop aL, i0 break_ge c0.x, c0.y "
	"														loop aL, i0 break_ge c0.x, c0.y "
	"															loop aL, i0 break_ge c0.x, c0.y "
	"																loop aL, i0 break_ge c0.x, c0.y "
	"																	loop aL, i0 break_ge c0.x, c0.y "
	"																		loop aL, i0 break_ge c0.x, c0.y "
	"																			loop aL, i0 break_ge c0.x, c0.y "
	"																				loop aL, i0 break_ge c0.x, c0.y "
	"																					loop aL, i0 break_ge c0.x, c0.y "
	"																						loop aL, i0 break_ge c0.x, c0.y "
	"																							loop aL, i0 break_ge c0.x, c0.y "
	"																								m3x2 r0.xy, v0, c0 "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_205_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_205_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level loop aL, i0 break_ge c0.x, c0.y block";
string VS_205 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								loop aL, i0 break_ge c0.x, c0.y "
	"									loop aL, i0 break_ge c0.x, c0.y "
	"										loop aL, i0 break_ge c0.x, c0.y "
	"											loop aL, i0 break_ge c0.x, c0.y "
	"												loop aL, i0 break_ge c0.x, c0.y "
	"													loop aL, i0 break_ge c0.x, c0.y "
	"														loop aL, i0 break_ge c0.x, c0.y "
	"															loop aL, i0 break_ge c0.x, c0.y "
	"																loop aL, i0 break_ge c0.x, c0.y "
	"																	loop aL, i0 break_ge c0.x, c0.y "
	"																		loop aL, i0 break_ge c0.x, c0.y "
	"																			loop aL, i0 break_ge c0.x, c0.y "
	"																				loop aL, i0 break_ge c0.x, c0.y "
	"																					loop aL, i0 break_ge c0.x, c0.y "
	"																						loop aL, i0 break_ge c0.x, c0.y "
	"																							loop aL, i0 break_ge c0.x, c0.y "
	"																								loop aL, i0 break_ge c0.x, c0.y "
	"																									m3x2 r0.xy, v0, c0 "
	"																								endloop "
	"																							endloop "
	"																						endloop "
	"																					endloop "
	"																				endloop "
	"																			endloop "
	"																		endloop "
	"																	endloop "
	"																endloop "
	"															endloop "
	"														endloop "
	"													endloop "
	"												endloop "
	"											endloop "
	"										endloop "
	"									endloop "
	"								endloop "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_206_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_206_Desc = "vs_3_0 : m3x2 is allowed in a 1 level rep i0 break_ge c0.x, c0.y block";
string VS_206 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	m3x2 r0.xy, v0, c0 "
	"endrep ";

VS_CRITERIA VS_207_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_207_Desc = "vs_3_0 : m3x2 is NOT allowed in a 8 level rep i0 break_ge c0.x, c0.y block";
string VS_207 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								m3x2 r0.xy, v0, c0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_208_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_208_Desc = "vs_3_0 : m3x2 is NOT allowed in a 16 level rep i0 break_ge c0.x, c0.y block";
string VS_208 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								rep i0 break_ge c0.x, c0.y "
	"									rep i0 break_ge c0.x, c0.y "
	"										rep i0 break_ge c0.x, c0.y "
	"											rep i0 break_ge c0.x, c0.y "
	"												rep i0 break_ge c0.x, c0.y "
	"													rep i0 break_ge c0.x, c0.y "
	"														rep i0 break_ge c0.x, c0.y "
	"															rep i0 break_ge c0.x, c0.y "
	"																m3x2 r0.xy, v0, c0 "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_209_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_209_Desc = "vs_3_0 : m3x2 is NOT allowed in a 24 level rep i0 break_ge c0.x, c0.y block";
string VS_209 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								rep i0 break_ge c0.x, c0.y "
	"									rep i0 break_ge c0.x, c0.y "
	"										rep i0 break_ge c0.x, c0.y "
	"											rep i0 break_ge c0.x, c0.y "
	"												rep i0 break_ge c0.x, c0.y "
	"													rep i0 break_ge c0.x, c0.y "
	"														rep i0 break_ge c0.x, c0.y "
	"															rep i0 break_ge c0.x, c0.y "
	"																rep i0 break_ge c0.x, c0.y "
	"																	rep i0 break_ge c0.x, c0.y "
	"																		rep i0 break_ge c0.x, c0.y "
	"																			rep i0 break_ge c0.x, c0.y "
	"																				rep i0 break_ge c0.x, c0.y "
	"																					rep i0 break_ge c0.x, c0.y "
	"																						rep i0 break_ge c0.x, c0.y "
	"																							rep i0 break_ge c0.x, c0.y "
	"																								m3x2 r0.xy, v0, c0 "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_210_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_210_Desc = "vs_3_0 : m3x2 is NOT allowed in a 25 level rep i0 break_ge c0.x, c0.y block";
string VS_210 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								rep i0 break_ge c0.x, c0.y "
	"									rep i0 break_ge c0.x, c0.y "
	"										rep i0 break_ge c0.x, c0.y "
	"											rep i0 break_ge c0.x, c0.y "
	"												rep i0 break_ge c0.x, c0.y "
	"													rep i0 break_ge c0.x, c0.y "
	"														rep i0 break_ge c0.x, c0.y "
	"															rep i0 break_ge c0.x, c0.y "
	"																rep i0 break_ge c0.x, c0.y "
	"																	rep i0 break_ge c0.x, c0.y "
	"																		rep i0 break_ge c0.x, c0.y "
	"																			rep i0 break_ge c0.x, c0.y "
	"																				rep i0 break_ge c0.x, c0.y "
	"																					rep i0 break_ge c0.x, c0.y "
	"																						rep i0 break_ge c0.x, c0.y "
	"																							rep i0 break_ge c0.x, c0.y "
	"																								rep i0 break_ge c0.x, c0.y "
	"																									m3x2 r0.xy, v0, c0 "
	"																								endrep "
	"																							endrep "
	"																						endrep "
	"																					endrep "
	"																				endrep "
	"																			endrep "
	"																		endrep "
	"																	endrep "
	"																endrep "
	"															endrep "
	"														endrep "
	"													endrep "
	"												endrep "
	"											endrep "
	"										endrep "
	"									endrep "
	"								endrep "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_211_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_211_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and if b0 block";
string VS_211 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if b0 "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_212_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_212_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and if b0 nop else block";
string VS_212 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if b0 "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_213_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_213_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and loop aL, i0 block";
string VS_213 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if b0 "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_214_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_214_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and rep i0 block";
string VS_214 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if b0 "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_215_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_215_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and if_ge c0.x, c0.y block";
string VS_215 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if b0 "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_216_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_216_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and if_ge c0.x, c0.y nop else block";
string VS_216 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"if b0 "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_217_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_217_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and if p0.x block";
string VS_217 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_218_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_218_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and if p0.x nop else block";
string VS_218 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_219_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_219_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and loop aL, i0 break block";
string VS_219 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_220_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_220_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and rep i0 break block";
string VS_220 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_221_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_221_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and loop aL, i0 breakp p0.x block";
string VS_221 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_222_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_222_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and rep i0 breakp p0.x block";
string VS_222 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_223_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_223_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and loop aL, i0 break_ge c0.x, c0.y block";
string VS_223 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_224_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_224_Desc = "vs_3_0 : m3x2 is allowed in a if b0 and rep i0 break_ge c0.x, c0.y block";
string VS_224 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_225_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_225_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and if b0 block";
string VS_225 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_226_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_226_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and if b0 nop else block";
string VS_226 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_227_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_227_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and loop aL, i0 block";
string VS_227 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_228_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_228_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and rep i0 block";
string VS_228 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_229_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_229_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and if_ge c0.x, c0.y block";
string VS_229 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_230_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_230_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and if_ge c0.x, c0.y nop else block";
string VS_230 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_231_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_231_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and if p0.x block";
string VS_231 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_232_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_232_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and if p0.x nop else block";
string VS_232 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_233_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_233_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and loop aL, i0 break block";
string VS_233 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_234_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_234_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and rep i0 break block";
string VS_234 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_235_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_235_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string VS_235 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_236_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_236_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string VS_236 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_237_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_237_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and loop aL, i0 break_ge c0.x, c0.y block";
string VS_237 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_238_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_238_Desc = "vs_3_0 : m3x2 is allowed in a if b0 nop else and rep i0 break_ge c0.x, c0.y block";
string VS_238 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if b0 nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_239_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_239_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and if b0 block";
string VS_239 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_240_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_240_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and if b0 nop else block";
string VS_240 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_241_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_241_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and loop aL, i0 block";
string VS_241 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_242_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_242_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and rep i0 block";
string VS_242 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_243_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_243_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and if_ge c0.x, c0.y block";
string VS_243 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_244_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_244_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and if_ge c0.x, c0.y nop else block";
string VS_244 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_245_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_245_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and if p0.x block";
string VS_245 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_246_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_246_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and if p0.x nop else block";
string VS_246 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_247_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_247_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and loop aL, i0 break block";
string VS_247 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_248_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_248_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and rep i0 break block";
string VS_248 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_249_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_249_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string VS_249 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_250_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_250_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string VS_250 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_251_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_251_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and loop aL, i0 break_ge c0.x, c0.y block";
string VS_251 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_252_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_252_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 and rep i0 break_ge c0.x, c0.y block";
string VS_252 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_253_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_253_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and if b0 block";
string VS_253 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_254_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_254_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and if b0 nop else block";
string VS_254 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_255_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_255_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and loop aL, i0 block";
string VS_255 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_256_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_256_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and rep i0 block";
string VS_256 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_257_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_257_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and if_ge c0.x, c0.y block";
string VS_257 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_258_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_258_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and if_ge c0.x, c0.y nop else block";
string VS_258 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_259_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_259_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and if p0.x block";
string VS_259 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_260_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_260_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and if p0.x nop else block";
string VS_260 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_261_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_261_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and loop aL, i0 break block";
string VS_261 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_262_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_262_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and rep i0 break block";
string VS_262 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_263_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_263_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string VS_263 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_264_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_264_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and rep i0 breakp p0.x block";
string VS_264 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_265_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_265_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and loop aL, i0 break_ge c0.x, c0.y block";
string VS_265 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_266_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_266_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 and rep i0 break_ge c0.x, c0.y block";
string VS_266 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_267_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_267_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and if b0 block";
string VS_267 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_268_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_268_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and if b0 nop else block";
string VS_268 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_269_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_269_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and loop aL, i0 block";
string VS_269 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_270_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_270_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and rep i0 block";
string VS_270 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_271_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_271_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and if_ge c0.x, c0.y block";
string VS_271 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_272_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_272_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string VS_272 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_273_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_273_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and if p0.x block";
string VS_273 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_274_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_274_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and if p0.x nop else block";
string VS_274 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_275_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_275_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and loop aL, i0 break block";
string VS_275 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_276_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_276_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and rep i0 break block";
string VS_276 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_277_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_277_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_277 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_278_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_278_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and rep i0 breakp p0.x block";
string VS_278 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_279_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_279_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string VS_279 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_280_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_280_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string VS_280 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_281_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_281_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and if b0 block";
string VS_281 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_282_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_282_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and if b0 nop else block";
string VS_282 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_283_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_283_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 block";
string VS_283 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_284_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_284_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and rep i0 block";
string VS_284 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_285_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_285_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and if_ge c0.x, c0.y block";
string VS_285 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_286_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_286_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and if_ge c0.x, c0.y nop else block";
string VS_286 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_287_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_287_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and if p0.x block";
string VS_287 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_288_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_288_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and if p0.x nop else block";
string VS_288 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_289_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_289_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 break block";
string VS_289 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_290_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_290_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and rep i0 break block";
string VS_290 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_291_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_291_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string VS_291 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_292_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_292_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and rep i0 breakp p0.x block";
string VS_292 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_293_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_293_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 break_ge c0.x, c0.y block";
string VS_293 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_294_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_294_Desc = "vs_3_0 : m3x2 is allowed in a if_ge c0.x, c0.y nop else and rep i0 break_ge c0.x, c0.y block";
string VS_294 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_295_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_295_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and if b0 block";
string VS_295 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_296_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_296_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and if b0 nop else block";
string VS_296 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_297_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_297_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and loop aL, i0 block";
string VS_297 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_298_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_298_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and rep i0 block";
string VS_298 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_299_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_299_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and if_ge c0.x, c0.y block";
string VS_299 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_300_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_300_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and if_ge c0.x, c0.y nop else block";
string VS_300 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_301_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_301_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and if p0.x block";
string VS_301 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_302_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_302_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and if p0.x nop else block";
string VS_302 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_303_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_303_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and loop aL, i0 break block";
string VS_303 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_304_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_304_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and rep i0 break block";
string VS_304 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_305_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_305_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string VS_305 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_306_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_306_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and rep i0 breakp p0.x block";
string VS_306 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_307_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_307_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and loop aL, i0 break_ge c0.x, c0.y block";
string VS_307 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_308_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_308_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x and rep i0 break_ge c0.x, c0.y block";
string VS_308 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"if p0.x "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_309_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_309_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and if b0 block";
string VS_309 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_310_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_310_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and if b0 nop else block";
string VS_310 = 
	"vs_3_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_311_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_311_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and loop aL, i0 block";
string VS_311 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_312_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_312_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and rep i0 block";
string VS_312 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_313_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_313_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and if_ge c0.x, c0.y block";
string VS_313 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_314_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_314_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and if_ge c0.x, c0.y nop else block";
string VS_314 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_315_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_315_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and if p0.x block";
string VS_315 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_316_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_316_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and if p0.x nop else block";
string VS_316 = 
	"vs_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_317_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_317_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and loop aL, i0 break block";
string VS_317 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_318_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_318_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and rep i0 break block";
string VS_318 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_319_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_319_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string VS_319 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_320_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_320_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string VS_320 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_321_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_321_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and loop aL, i0 break_ge c0.x, c0.y block";
string VS_321 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_322_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_322_Desc = "vs_3_0 : m3x2 is allowed in a if p0.x nop else and rep i0 break_ge c0.x, c0.y block";
string VS_322 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_323_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_323_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and if b0 block";
string VS_323 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_324_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_324_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and if b0 nop else block";
string VS_324 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_325_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_325_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and loop aL, i0 block";
string VS_325 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_326_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_326_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and rep i0 block";
string VS_326 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_327_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_327_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and if_ge c0.x, c0.y block";
string VS_327 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_328_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_328_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and if_ge c0.x, c0.y nop else block";
string VS_328 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_329_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_329_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and if p0.x block";
string VS_329 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_330_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_330_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and if p0.x nop else block";
string VS_330 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_331_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_331_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and loop aL, i0 break block";
string VS_331 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_332_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_332_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and rep i0 break block";
string VS_332 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_333_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_333_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string VS_333 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_334_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_334_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string VS_334 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_335_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_335_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and loop aL, i0 break_ge c0.x, c0.y block";
string VS_335 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_336_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_336_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break and rep i0 break_ge c0.x, c0.y block";
string VS_336 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 break "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_337_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_337_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and if b0 block";
string VS_337 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_338_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_338_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and if b0 nop else block";
string VS_338 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_339_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_339_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and loop aL, i0 block";
string VS_339 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_340_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_340_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and rep i0 block";
string VS_340 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_341_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_341_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and if_ge c0.x, c0.y block";
string VS_341 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_342_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_342_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and if_ge c0.x, c0.y nop else block";
string VS_342 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_343_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_343_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and if p0.x block";
string VS_343 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_344_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_344_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and if p0.x nop else block";
string VS_344 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_345_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_345_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and loop aL, i0 break block";
string VS_345 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_346_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_346_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and rep i0 break block";
string VS_346 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_347_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_347_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string VS_347 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_348_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_348_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and rep i0 breakp p0.x block";
string VS_348 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_349_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_349_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and loop aL, i0 break_ge c0.x, c0.y block";
string VS_349 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_350_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_350_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break and rep i0 break_ge c0.x, c0.y block";
string VS_350 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"rep i0 break "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_351_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_351_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and if b0 block";
string VS_351 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_352_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_352_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string VS_352 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_353_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_353_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string VS_353 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_354_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_354_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string VS_354 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_355_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_355_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and if_ge c0.x, c0.y block";
string VS_355 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_356_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_356_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and if_ge c0.x, c0.y nop else block";
string VS_356 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_357_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_357_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string VS_357 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_358_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_358_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string VS_358 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_359_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_359_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string VS_359 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_360_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_360_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string VS_360 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_361_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_361_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string VS_361 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_362_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_362_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string VS_362 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_363_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_363_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_ge c0.x, c0.y block";
string VS_363 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_364_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_364_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 breakp p0.x and rep i0 break_ge c0.x, c0.y block";
string VS_364 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_365_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_365_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and if b0 block";
string VS_365 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_366_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_366_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string VS_366 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_367_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_367_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string VS_367 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_368_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_368_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and rep i0 block";
string VS_368 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_369_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_369_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and if_ge c0.x, c0.y block";
string VS_369 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 breakp p0.x "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_370_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_370_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and if_ge c0.x, c0.y nop else block";
string VS_370 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 breakp p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_371_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_371_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and if p0.x block";
string VS_371 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_372_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_372_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string VS_372 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_373_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_373_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string VS_373 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_374_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_374_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and rep i0 break block";
string VS_374 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_375_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_375_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string VS_375 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_376_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_376_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string VS_376 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_377_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_377_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and loop aL, i0 break_ge c0.x, c0.y block";
string VS_377 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_378_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_378_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 breakp p0.x and rep i0 break_ge c0.x, c0.y block";
string VS_378 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_379_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_379_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and if b0 block";
string VS_379 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_380_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_380_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and if b0 nop else block";
string VS_380 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_381_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_381_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 block";
string VS_381 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_382_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_382_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 block";
string VS_382 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_383_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_383_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y block";
string VS_383 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_384_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_384_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string VS_384 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_385_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_385_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and if p0.x block";
string VS_385 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_386_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_386_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and if p0.x nop else block";
string VS_386 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_387_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_387_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 break block";
string VS_387 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_388_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_388_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 break block";
string VS_388 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_389_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_389_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_389 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_390_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_390_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 breakp p0.x block";
string VS_390 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_391_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_391_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string VS_391 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_392_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_392_Desc = "vs_3_0 : m3x2 is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string VS_392 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_393_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_393_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and if b0 block";
string VS_393 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if b0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_394_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_394_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and if b0 nop else block";
string VS_394 = 
	"vs_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if b0 nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_395_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_395_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 block";
string VS_395 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_396_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_396_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 block";
string VS_396 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_397_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_397_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y block";
string VS_397 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_398_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_398_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string VS_398 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_399_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_399_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and if p0.x block";
string VS_399 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_400_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_400_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and if p0.x nop else block";
string VS_400 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		m3x2 r0.xy, v0, c0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_401_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_401_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 break block";
string VS_401 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_402_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_402_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 break block";
string VS_402 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_403_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_403_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_403 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_404_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_404_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 breakp p0.x block";
string VS_404 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_405_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_405_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string VS_405 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_406_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_406_Desc = "vs_3_0 : m3x2 is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string VS_406 = 
	"vs_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		m3x2 r0.xy, v0, c0 "
	"	endrep "
	"endrep ";


