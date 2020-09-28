#include "Test_Include.fx"

int Test_Count = 117;

string TestInfo
<
	string TestType = "PS";
>
= "ps_2_0_inst_pow";

PS_CRITERIA PS_001_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_2_0 : pow source reg combination v0, v1 is NOT allowed";
string PS_001 = 
	"ps_2_0 "
	"dcl v0 "
	"dcl v1 "
	"pow r0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_002_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_2_0 : pow source reg combination c0, c1 is NOT allowed";
string PS_002 = 
	"ps_2_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"pow r0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_003_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_2_0 : pow source reg combination b0, r0 is NOT allowed";
string PS_003 = 
	"ps_2_0 "
	"defb b0, true "
	"mov r0, c0 "
	"pow r0, b0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_004_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_2_0 : pow source reg combination b0, b0 is NOT allowed";
string PS_004 = 
	"ps_2_0 "
	"defb b0, true "
	"pow r0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_2_0 : pow source reg combination b0, i0 is NOT allowed";
string PS_005 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"pow r0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_2_0 : pow source reg combination b0, p0 is NOT allowed";
string PS_006 = 
	"ps_2_0 "
	"defb b0, true "
	"pow r0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_2_0 : pow source reg combination b0, s0 is NOT allowed";
string PS_007 = 
	"ps_2_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"pow r0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_2_0 : pow source reg combination b0, t0 is NOT allowed";
string PS_008 = 
	"ps_2_0 "
	"defb b0, true "
	"dcl t0 "
	"pow r0, b0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_2_0 : pow source reg combination b0, oC0 is NOT allowed";
string PS_009 = 
	"ps_2_0 "
	"defb b0, true "
	"pow r0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_2_0 : pow source reg combination b0, oDepth is NOT allowed";
string PS_010 = 
	"ps_2_0 "
	"defb b0, true "
	"pow r0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_2_0 : pow source reg combination i0, r0 is NOT allowed";
string PS_011 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"pow r0, i0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_012_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_2_0 : pow source reg combination i0, b0 is NOT allowed";
string PS_012 = 
	"ps_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"pow r0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_2_0 : pow source reg combination i0, i0 is NOT allowed";
string PS_013 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"pow r0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_014_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_2_0 : pow source reg combination i0, p0 is NOT allowed";
string PS_014 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"pow r0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_015_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_2_0 : pow source reg combination i0, s0 is NOT allowed";
string PS_015 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"pow r0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_2_0 : pow source reg combination i0, t0 is NOT allowed";
string PS_016 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl t0 "
	"pow r0, i0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_017_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_2_0 : pow source reg combination i0, oC0 is NOT allowed";
string PS_017 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"pow r0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_018_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_2_0 : pow source reg combination i0, oDepth is NOT allowed";
string PS_018 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"pow r0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_2_0 : pow source reg combination p0, r0 is NOT allowed";
string PS_019 = 
	"ps_2_0 "
	"mov r0, c0 "
	"pow r0, p0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_020_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_2_0 : pow source reg combination p0, b0 is NOT allowed";
string PS_020 = 
	"ps_2_0 "
	"defb b0, true "
	"pow r0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_2_0 : pow source reg combination p0, i0 is NOT allowed";
string PS_021 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"pow r0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_2_0 : pow source reg combination p0, p0 is NOT allowed";
string PS_022 = 
	"ps_2_0 "
	"pow r0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_2_0 : pow source reg combination p0, s0 is NOT allowed";
string PS_023 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"pow r0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_2_0 : pow source reg combination p0, t0 is NOT allowed";
string PS_024 = 
	"ps_2_0 "
	"dcl t0 "
	"pow r0, p0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_2_0 : pow source reg combination p0, oC0 is NOT allowed";
string PS_025 = 
	"ps_2_0 "
	"pow r0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_2_0 : pow source reg combination p0, oDepth is NOT allowed";
string PS_026 = 
	"ps_2_0 "
	"pow r0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_0 : pow source reg combination s0, r0 is NOT allowed";
string PS_027 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"mov r0, c0 "
	"pow r0, s0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_0 : pow source reg combination s0, b0 is NOT allowed";
string PS_028 = 
	"ps_2_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"pow r0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_2_0 : pow source reg combination s0, i0 is NOT allowed";
string PS_029 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"pow r0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_2_0 : pow source reg combination s0, p0 is NOT allowed";
string PS_030 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"pow r0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_2_0 : pow source reg combination s0, s0 is NOT allowed";
string PS_031 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"pow r0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_2_0 : pow source reg combination s0, t0 is NOT allowed";
string PS_032 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"dcl t0 "
	"pow r0, s0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_2_0 : pow source reg combination s0, oC0 is NOT allowed";
string PS_033 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"pow r0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_2_0 : pow source reg combination s0, oDepth is NOT allowed";
string PS_034 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"pow r0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_2_0 : pow source reg combination t0, r0 is NOT allowed";
string PS_035 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"pow r0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_2_0 : pow source reg combination t0, b0 is NOT allowed";
string PS_036 = 
	"ps_2_0 "
	"defb b0, true "
	"dcl t0 "
	"pow r0, t0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_2_0 : pow source reg combination t0, i0 is NOT allowed";
string PS_037 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl t0 "
	"pow r0, t0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_2_0 : pow source reg combination t0, p0 is NOT allowed";
string PS_038 = 
	"ps_2_0 "
	"dcl t0 "
	"pow r0, t0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_2_0 : pow source reg combination t0, s0 is NOT allowed";
string PS_039 = 
	"ps_2_0 "
	"dcl t0 "
	"dcl_volume s0 "
	"pow r0, t0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_040_Desc = "ps_2_0 : pow source reg combination t0, t0 is NOT allowed";
string PS_040 = 
	"ps_2_0 "
	"dcl t0 "
	"pow r0, t0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_2_0 : pow source reg combination t0, oC0 is NOT allowed";
string PS_041 = 
	"ps_2_0 "
	"dcl t0 "
	"pow r0, t0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_2_0 : pow source reg combination t0, oDepth is NOT allowed";
string PS_042 = 
	"ps_2_0 "
	"dcl t0 "
	"pow r0, t0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_2_0 : pow source reg combination oC0, r0 is NOT allowed";
string PS_043 = 
	"ps_2_0 "
	"mov r0, c0 "
	"pow r0, oC0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_2_0 : pow source reg combination oC0, b0 is NOT allowed";
string PS_044 = 
	"ps_2_0 "
	"defb b0, true "
	"pow r0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_2_0 : pow source reg combination oC0, i0 is NOT allowed";
string PS_045 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"pow r0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_2_0 : pow source reg combination oC0, p0 is NOT allowed";
string PS_046 = 
	"ps_2_0 "
	"pow r0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_2_0 : pow source reg combination oC0, s0 is NOT allowed";
string PS_047 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"pow r0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_2_0 : pow source reg combination oC0, t0 is NOT allowed";
string PS_048 = 
	"ps_2_0 "
	"dcl t0 "
	"pow r0, oC0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_2_0 : pow source reg combination oC0, oC0 is NOT allowed";
string PS_049 = 
	"ps_2_0 "
	"pow r0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_2_0 : pow source reg combination oC0, oDepth is NOT allowed";
string PS_050 = 
	"ps_2_0 "
	"pow r0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_2_0 : pow source reg combination oDepth, r0 is NOT allowed";
string PS_051 = 
	"ps_2_0 "
	"mov r0, c0 "
	"pow r0, oDepth, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_2_0 : pow source reg combination oDepth, b0 is NOT allowed";
string PS_052 = 
	"ps_2_0 "
	"defb b0, true "
	"pow r0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_2_0 : pow source reg combination oDepth, i0 is NOT allowed";
string PS_053 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"pow r0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_2_0 : pow source reg combination oDepth, p0 is NOT allowed";
string PS_054 = 
	"ps_2_0 "
	"pow r0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_2_0 : pow source reg combination oDepth, s0 is NOT allowed";
string PS_055 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"pow r0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_2_0 : pow source reg combination oDepth, t0 is NOT allowed";
string PS_056 = 
	"ps_2_0 "
	"dcl t0 "
	"pow r0, oDepth, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_2_0 : pow source reg combination oDepth, oC0 is NOT allowed";
string PS_057 = 
	"ps_2_0 "
	"pow r0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_2_0 : pow source reg combination oDepth, oDepth is NOT allowed";
string PS_058 = 
	"ps_2_0 "
	"pow r0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_2_0 : pow source reg combination -c0, v0 is NOT allowed";
string PS_059 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0, -c0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_2_0 : pow source reg combination c0, -v0 is NOT allowed";
string PS_060 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0, c0, -v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_2_0 : pow dest r# is allowed";
string PS_061 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_2_0 : pow _pp dest r# is allowed";
string PS_062 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_pp r0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_2_0 : pow _sat dest r# is allowed";
string PS_063 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_sat r0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_2_0 : pow dest v# is NOT allowed";
string PS_064 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow v0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_2_0 : pow _pp dest v# is NOT allowed";
string PS_065 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_pp v0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_2_0 : pow _sat dest v# is NOT allowed";
string PS_066 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_sat v0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_2_0 : pow dest c# is NOT allowed";
string PS_067 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow c1, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_2_0 : pow _pp dest c# is NOT allowed";
string PS_068 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_pp c1, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_2_0 : pow _sat dest c# is NOT allowed";
string PS_069 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_sat c1, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_2_0 : pow dest b# is NOT allowed";
string PS_070 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow b0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_2_0 : pow _pp dest b# is NOT allowed";
string PS_071 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_pp b0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_2_0 : pow _sat dest b# is NOT allowed";
string PS_072 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_sat b0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_2_0 : pow dest i# is NOT allowed";
string PS_073 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow i0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_2_0 : pow _pp dest i# is NOT allowed";
string PS_074 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_pp i0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_2_0 : pow _sat dest i# is NOT allowed";
string PS_075 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_sat i0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_076_Desc = "ps_2_0 : pow dest p0 is NOT allowed";
string PS_076 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow p0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_077_Desc = "ps_2_0 : pow _pp dest p0 is NOT allowed";
string PS_077 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_pp p0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_078_Desc = "ps_2_0 : pow _sat dest p0 is NOT allowed";
string PS_078 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_sat p0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_2_0 : pow dest s# is NOT allowed";
string PS_079 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow s0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_2_0 : pow _pp dest s# is NOT allowed";
string PS_080 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_pp s0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_2_0 : pow _sat dest s# is NOT allowed";
string PS_081 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_sat s0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_2_0 : pow dest t# is NOT allowed";
string PS_082 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow t0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_083_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_2_0 : pow _pp dest t# is NOT allowed";
string PS_083 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_pp t0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_084_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_2_0 : pow _sat dest t# is NOT allowed";
string PS_084 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_sat t0, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_085_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_2_0 : pow dest oC# is NOT allowed";
string PS_085 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow oC0, v0.x, c0.x ";

PS_CRITERIA PS_086_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_2_0 : pow _pp dest oC# is NOT allowed";
string PS_086 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_pp oC0, v0.x, c0.x ";

PS_CRITERIA PS_087_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_2_0 : pow _sat dest oC# is NOT allowed";
string PS_087 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_sat oC0, v0.x, c0.x ";

PS_CRITERIA PS_088_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_2_0 : pow dest oDepth is NOT allowed";
string PS_088 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow oDepth, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_089_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_2_0 : pow _pp dest oDepth is NOT allowed";
string PS_089 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_pp oDepth, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_090_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_2_0 : pow _sat dest oDepth is NOT allowed";
string PS_090 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow_sat oDepth, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_091_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_2_0 : pow write mask .r is allowed";
string PS_091 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.r, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_092_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_2_0 : pow write mask .g is allowed";
string PS_092 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.g, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_093_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_2_0 : pow write mask .b is allowed";
string PS_093 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.b, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_094_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_2_0 : pow write mask .a is allowed";
string PS_094 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.a, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_095_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_2_0 : pow write mask .rg is allowed";
string PS_095 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.rg, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_096_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_2_0 : pow write mask .gb is allowed";
string PS_096 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.gb, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_097_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_2_0 : pow write mask .ba is allowed";
string PS_097 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.ba, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_098_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_2_0 : pow write mask .rb is allowed";
string PS_098 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.rb, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_099_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_2_0 : pow write mask .ra is allowed";
string PS_099 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.ra, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_100_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_2_0 : pow write mask .ga is allowed";
string PS_100 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.ga, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_101_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_2_0 : pow write mask .rgb is allowed";
string PS_101 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.rgb, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_102_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_2_0 : pow write mask .gba is allowed";
string PS_102 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.gba, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_103_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_2_0 : pow write mask .rba is allowed";
string PS_103 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.rba, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_104_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_2_0 : pow write mask .rgba is allowed";
string PS_104 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.rgba, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_105_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_2_0 : pow write mask .yx is NOT allowed";
string PS_105 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.yx, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_106_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_2_0 : pow write mask .zx is NOT allowed";
string PS_106 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.zx, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_107_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_2_0 : pow write mask .zy is NOT allowed";
string PS_107 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.zy, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_108_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_2_0 : pow write mask .wx is NOT allowed";
string PS_108 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.wx, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_109_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_2_0 : pow write mask .wz is NOT allowed";
string PS_109 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.wz, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_110_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_2_0 : pow write mask .wy is NOT allowed";
string PS_110 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.wy, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_111_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_2_0 : pow write mask .zyx is NOT allowed";
string PS_111 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.zyx, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_112_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_2_0 : pow write mask .wzy is NOT allowed";
string PS_112 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.wzy, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_113_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_2_0 : pow write mask .wzx is NOT allowed";
string PS_113 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.wzx, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_114_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_2_0 : pow write mask .wyx is NOT allowed";
string PS_114 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.wyx, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_115_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_2_0 : pow write mask .yxw is NOT allowed";
string PS_115 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.yxw, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_116_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_2_0 : pow write mask .wzyx is NOT allowed";
string PS_116 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.wzyx, v0.x, c0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_117_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_2_0 : pow write mask .zxwy is NOT allowed";
string PS_117 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"pow r0.zxwy, v0.x, c0.x "
	"mov oC0, c0 ";


