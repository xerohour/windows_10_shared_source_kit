#include "Test_Include.fx"

int Test_Count = 670;

string TestInfo
<
	string TestType = "PS";
>
= "ps_2_sw_inst_mad";

PS_CRITERIA PS_001_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_2_sw : mad source reg combination v0, v1, r0 is allowed";
string PS_001 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_2_sw : mad source reg combination v0, v1, c0 is allowed";
string PS_002 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"mad r0, v0, v1, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_003_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_2_sw : mad source reg combination v0, v1, t0 is allowed";
string PS_003 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dcl t0 "
	"mad r0, v0, v1, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_004_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_2_sw : mad source reg combination v0, r0, v1 is allowed";
string PS_004 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0, v0, r0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_005_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_2_sw : mad source reg combination v0, r0, r1 is allowed";
string PS_005 = 
	"ps_2_sw "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, v0, r0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_006_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_2_sw : mad source reg combination v0, r0, c0 is allowed";
string PS_006 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_007_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_2_sw : mad source reg combination v0, r0, t0 is allowed";
string PS_007 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, v0, r0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_008_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_2_sw : mad source reg combination v0, c0, v1 is allowed";
string PS_008 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"mad r0, v0, c0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_009_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_2_sw : mad source reg combination v0, c0, r0 is allowed";
string PS_009 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"mov r0, c0 "
	"mad r0, v0, c0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_010_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_2_sw : mad source reg combination v0, c0, c1 is allowed";
string PS_010 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"mad r0, v0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_011_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_2_sw : mad source reg combination v0, c0, t0 is allowed";
string PS_011 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl t0 "
	"mad r0, v0, c0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_012_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_2_sw : mad source reg combination v0, t0, v1 is allowed";
string PS_012 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl t0 "
	"dcl v1 "
	"mad r0, v0, t0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_013_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_2_sw : mad source reg combination v0, t0, r0 is allowed";
string PS_013 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, v0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_014_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_2_sw : mad source reg combination v0, t0, c0 is allowed";
string PS_014 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl t0 "
	"mad r0, v0, t0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_015_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_015_Desc = "ps_2_sw : mad source reg combination v0, t0, t0 is allowed";
string PS_015 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl t0 "
	"mad r0, v0, t0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_016_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_2_sw : mad source reg combination v0, t0, t1 is allowed";
string PS_016 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl t0 "
	"dcl t1 "
	"mad r0, v0, t0, t1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_017_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_2_sw : mad source reg combination r0, v0, v1 is allowed";
string PS_017 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0, r0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_018_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_2_sw : mad source reg combination r0, v0, r1 is allowed";
string PS_018 = 
	"ps_2_sw "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, v0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_019_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_2_sw : mad source reg combination r0, v0, c0 is allowed";
string PS_019 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"mov r0, c0 "
	"mad r0, r0, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_020_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_2_sw : mad source reg combination r0, v0, t0 is allowed";
string PS_020 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, r0, v0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_2_sw : mad source reg combination r0, r1, v0 is allowed";
string PS_021 = 
	"ps_2_sw "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, r1, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_2_sw : mad source reg combination r0, r1, r2 is allowed";
string PS_022 = 
	"ps_2_sw "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"mad r0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_2_sw : mad source reg combination r0, r1, c0 is allowed";
string PS_023 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, r1, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_2_sw : mad source reg combination r0, r1, t0 is allowed";
string PS_024 = 
	"ps_2_sw "
	"dcl t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, r1, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_2_sw : mad source reg combination r0, c0, v0 is allowed";
string PS_025 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"mov r0, c0 "
	"mad r0, r0, c0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_2_sw : mad source reg combination r0, c0, r1 is allowed";
string PS_026 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_sw : mad source reg combination r0, c0, c1 is allowed";
string PS_027 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, r0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_sw : mad source reg combination r0, c0, t0 is allowed";
string PS_028 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, r0, c0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_2_sw : mad source reg combination r0, t0, v0 is allowed";
string PS_029 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl v0 "
	"mov r0, c0 "
	"mad r0, r0, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_2_sw : mad source reg combination r0, t0, r1 is allowed";
string PS_030 = 
	"ps_2_sw "
	"dcl t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, t0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_2_sw : mad source reg combination r0, t0, c0 is allowed";
string PS_031 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, r0, t0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_032_Desc = "ps_2_sw : mad source reg combination r0, t0, t0 is allowed";
string PS_032 = 
	"ps_2_sw "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, r0, t0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_2_sw : mad source reg combination r0, t0, t1 is allowed";
string PS_033 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl t1 "
	"mov r0, c0 "
	"mad r0, r0, t0, t1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_2_sw : mad source reg combination c0, v0, v1 is allowed";
string PS_034 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl v1 "
	"mad r0, c0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_2_sw : mad source reg combination c0, v0, r0 is allowed";
string PS_035 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"mov r0, c0 "
	"mad r0, c0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_2_sw : mad source reg combination c0, v0, c1 is allowed";
string PS_036 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"mad r0, c0, v0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_2_sw : mad source reg combination c0, v0, t0 is allowed";
string PS_037 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"dcl t0 "
	"mad r0, c0, v0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_2_sw : mad source reg combination c0, r0, v0 is allowed";
string PS_038 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"mov r0, c0 "
	"mad r0, c0, r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_2_sw : mad source reg combination c0, r0, r1 is allowed";
string PS_039 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_2_sw : mad source reg combination c0, r0, c1 is allowed";
string PS_040 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, r0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_2_sw : mad source reg combination c0, r0, t0 is allowed";
string PS_041 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, c0, r0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_2_sw : mad source reg combination c0, c1, v0 is allowed";
string PS_042 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl v0 "
	"mad r0, c0, c1, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_2_sw : mad source reg combination c0, c1, r0 is allowed";
string PS_043 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_2_sw : mad source reg combination c0, c0, c0 is allowed";
string PS_044 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"mad r0, c0, c0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_2_sw : mad source reg combination c0, c1, c2 is allowed";
string PS_045 = 
	"ps_2_sw "
	"def c2, 1, 1, 1, 1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mad r0, c0, c1, c2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_2_sw : mad source reg combination c0, c1, t0 is allowed";
string PS_046 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mad r0, c0, c1, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_2_sw : mad source reg combination c0, t0, v0 is allowed";
string PS_047 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_2_sw : mad source reg combination c0, t0, r0 is allowed";
string PS_048 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, c0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_049_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_2_sw : mad source reg combination c0, t0, c1 is allowed";
string PS_049 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mad r0, c0, t0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_050_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_050_Desc = "ps_2_sw : mad source reg combination c0, t0, t0 is allowed";
string PS_050 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mad r0, c0, t0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_2_sw : mad source reg combination c0, t0, t1 is allowed";
string PS_051 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl t1 "
	"mad r0, c0, t0, t1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_2_sw : mad source reg combination t0, v0, v1 is allowed";
string PS_052 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl v0 "
	"dcl v1 "
	"mad r0, t0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_2_sw : mad source reg combination t0, v0, r0 is allowed";
string PS_053 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl v0 "
	"mov r0, c0 "
	"mad r0, t0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_2_sw : mad source reg combination t0, v0, c0 is allowed";
string PS_054 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, t0, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_055_Desc = "ps_2_sw : mad source reg combination t0, v0, t0 is allowed";
string PS_055 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl v0 "
	"mad r0, t0, v0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_2_sw : mad source reg combination t0, v0, t1 is allowed";
string PS_056 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl v0 "
	"dcl t1 "
	"mad r0, t0, v0, t1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_2_sw : mad source reg combination t0, r0, v0 is allowed";
string PS_057 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl v0 "
	"mov r0, c0 "
	"mad r0, t0, r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_2_sw : mad source reg combination t0, r0, r1 is allowed";
string PS_058 = 
	"ps_2_sw "
	"dcl t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, t0, r0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_2_sw : mad source reg combination t0, r0, c0 is allowed";
string PS_059 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, t0, r0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_060_Desc = "ps_2_sw : mad source reg combination t0, r0, t0 is allowed";
string PS_060 = 
	"ps_2_sw "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, t0, r0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_2_sw : mad source reg combination t0, r0, t1 is allowed";
string PS_061 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl t1 "
	"mov r0, c0 "
	"mad r0, t0, r0, t1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_2_sw : mad source reg combination t0, c0, v0 is allowed";
string PS_062 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, t0, c0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_2_sw : mad source reg combination t0, c0, r0 is allowed";
string PS_063 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, t0, c0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_2_sw : mad source reg combination t0, c0, c1 is allowed";
string PS_064 = 
	"ps_2_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mad r0, t0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_065_Desc = "ps_2_sw : mad source reg combination t0, c0, t0 is allowed";
string PS_065 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mad r0, t0, c0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_2_sw : mad source reg combination t0, c0, t1 is allowed";
string PS_066 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl t1 "
	"mad r0, t0, c0, t1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_067_Desc = "ps_2_sw : mad source reg combination t0, t0, v0 is allowed";
string PS_067 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl v0 "
	"mad r0, t0, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_2_sw : mad source reg combination t0, t1, v0 is allowed";
string PS_068 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl t1 "
	"dcl v0 "
	"mad r0, t0, t1, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_069_Desc = "ps_2_sw : mad source reg combination t0, t0, r0 is allowed";
string PS_069 = 
	"ps_2_sw "
	"dcl t0 "
	"mov r0, c0 "
	"mad r0, t0, t0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_2_sw : mad source reg combination t0, t1, r0 is allowed";
string PS_070 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl t1 "
	"mov r0, c0 "
	"mad r0, t0, t1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_071_Desc = "ps_2_sw : mad source reg combination t0, t0, c0 is allowed";
string PS_071 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"mad r0, t0, t0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_2_sw : mad source reg combination t0, t1, c0 is allowed";
string PS_072 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl t1 "
	"mad r0, t0, t1, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_073_Desc = "ps_2_sw : mad source reg combination t0, t0, t0 is allowed";
string PS_073 = 
	"ps_2_sw "
	"dcl t0 "
	"mad r0, t0, t0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_2_sw : mad source reg combination t0, t1, t2 is allowed";
string PS_074 = 
	"ps_2_sw "
	"dcl t0 "
	"dcl t1 "
	"dcl t2 "
	"mad r0, t0, t1, t2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_2_sw : mad source reg combination -c0, -t0, -v0 is allowed";
string PS_075 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, -c0, -t0, -v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_2_sw : mad source reg combination -c0, -t0, v0 is allowed";
string PS_076 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, -c0, -t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_2_sw : mad source reg combination -c0, t0, -v0 is allowed";
string PS_077 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, -c0, t0, -v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_2_sw : mad source reg combination -c0, t0, v0 is allowed";
string PS_078 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, -c0, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_2_sw : mad source reg combination c0, -t0, -v0 is allowed";
string PS_079 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, -t0, -v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_2_sw : mad source reg combination c0, -t0, v0 is allowed";
string PS_080 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, -t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_081_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_2_sw : mad source reg combination c0, t0, -v0 is allowed";
string PS_081 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, -v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_082_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_2_sw : mad source reg combination c0, t0, v0 is allowed";
string PS_082 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_083_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_2_sw : mad source reg combination c0.r, t0, v0 is allowed";
string PS_083 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.r, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_084_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_2_sw : mad source reg combination c0.g, t0, v0 is allowed";
string PS_084 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.g, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_085_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_2_sw : mad source reg combination c0.b, t0, v0 is allowed";
string PS_085 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.b, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_086_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_2_sw : mad source reg combination c0.a, t0, v0 is allowed";
string PS_086 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.a, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_087_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_2_sw : mad source reg combination c0.gbra, t0, v0 is allowed";
string PS_087 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.gbra, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_088_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_2_sw : mad source reg combination c0.brga, t0, v0 is allowed";
string PS_088 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.brga, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_089_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_2_sw : mad source reg combination c0.abgr, t0, v0 is allowed";
string PS_089 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.abgr, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_090_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_2_sw : mad source reg combination c0.agrb, t0, v0 is allowed";
string PS_090 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.agrb, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_091_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_2_sw : mad source reg combination c0.rbga, t0, v0 is allowed";
string PS_091 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.rbga, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_092_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_2_sw : mad source reg combination c0.rgab, t0, v0 is allowed";
string PS_092 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.rgab, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_093_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_2_sw : mad source reg combination c0.bgr, t0, v0 is allowed";
string PS_093 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.bgr, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_094_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_2_sw : mad source reg combination c0.rbg, t0, v0 is allowed";
string PS_094 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.rbg, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_095_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_2_sw : mad source reg combination c0.gar, t0, v0 is allowed";
string PS_095 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.gar, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_096_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_2_sw : mad source reg combination c0.gr, t0, v0 is allowed";
string PS_096 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.gr, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_097_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_2_sw : mad source reg combination c0.ab, t0, v0 is allowed";
string PS_097 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.ab, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_098_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_2_sw : mad source reg combination c0.bg, t0, v0 is allowed";
string PS_098 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0.bg, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_099_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_2_sw : mad source reg combination c0, t0.r, v0 is allowed";
string PS_099 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.r, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_100_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_2_sw : mad source reg combination c0, t0.g, v0 is allowed";
string PS_100 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.g, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_101_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_2_sw : mad source reg combination c0, t0.b, v0 is allowed";
string PS_101 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.b, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_102_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_2_sw : mad source reg combination c0, t0.a, v0 is allowed";
string PS_102 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.a, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_103_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_2_sw : mad source reg combination c0, t0.gbra, v0 is allowed";
string PS_103 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.gbra, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_104_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_2_sw : mad source reg combination c0, t0.brga, v0 is allowed";
string PS_104 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.brga, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_105_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_2_sw : mad source reg combination c0, t0.abgr, v0 is allowed";
string PS_105 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.abgr, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_106_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_2_sw : mad source reg combination c0, t0.agrb, v0 is allowed";
string PS_106 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.agrb, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_107_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_2_sw : mad source reg combination c0, t0.rbga, v0 is allowed";
string PS_107 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.rbga, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_108_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_2_sw : mad source reg combination c0, t0.rgab, v0 is allowed";
string PS_108 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.rgab, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_109_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_2_sw : mad source reg combination c0, t0.bgr, v0 is allowed";
string PS_109 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.bgr, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_110_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_2_sw : mad source reg combination c0, t0.rbg, v0 is allowed";
string PS_110 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.rbg, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_111_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_2_sw : mad source reg combination c0, t0.gar, v0 is allowed";
string PS_111 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.gar, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_112_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_2_sw : mad source reg combination c0, t0.gr, v0 is allowed";
string PS_112 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.gr, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_113_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_2_sw : mad source reg combination c0, t0.ab, v0 is allowed";
string PS_113 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.ab, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_114_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_2_sw : mad source reg combination c0, t0.bg, v0 is allowed";
string PS_114 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0.bg, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_115_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.r is allowed";
string PS_115 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.r "
	"mov oC0, c0 ";

PS_CRITERIA PS_116_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.g is allowed";
string PS_116 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_117_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.b is allowed";
string PS_117 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_118_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.a is allowed";
string PS_118 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_119_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.gbra is allowed";
string PS_119 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.gbra "
	"mov oC0, c0 ";

PS_CRITERIA PS_120_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.brga is allowed";
string PS_120 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.brga "
	"mov oC0, c0 ";

PS_CRITERIA PS_121_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.abgr is allowed";
string PS_121 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.abgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_122_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.agrb is allowed";
string PS_122 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.agrb "
	"mov oC0, c0 ";

PS_CRITERIA PS_123_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.rbga is allowed";
string PS_123 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.rbga "
	"mov oC0, c0 ";

PS_CRITERIA PS_124_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.rgab is allowed";
string PS_124 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.rgab "
	"mov oC0, c0 ";

PS_CRITERIA PS_125_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_125_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.bgr is allowed";
string PS_125 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.bgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_126_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.rbg is allowed";
string PS_126 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.rbg "
	"mov oC0, c0 ";

PS_CRITERIA PS_127_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.gar is allowed";
string PS_127 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.gar "
	"mov oC0, c0 ";

PS_CRITERIA PS_128_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.gr is allowed";
string PS_128 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.gr "
	"mov oC0, c0 ";

PS_CRITERIA PS_129_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.ab is allowed";
string PS_129 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.ab "
	"mov oC0, c0 ";

PS_CRITERIA PS_130_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_2_sw : mad source reg combination c0, t0, v0.bg is allowed";
string PS_130 = 
	"ps_2_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl t0 "
	"dcl v0 "
	"mad r0, c0, t0, v0.bg "
	"mov oC0, c0 ";

PS_CRITERIA PS_131_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_2_sw : mad source reg combination v0, v1, v2 is NOT allowed";
string PS_131 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"dcl v2 "
	"mad r0, v0, v1, v2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_132_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_2_sw : mad source reg combination b0, b0, b0 is NOT allowed";
string PS_132 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_133_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_2_sw : mad source reg combination b0, b0, i0 is NOT allowed";
string PS_133 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_134_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_2_sw : mad source reg combination b0, b0, p0 is NOT allowed";
string PS_134 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_135_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_2_sw : mad source reg combination b0, b0, s0 is NOT allowed";
string PS_135 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_136_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_136_Desc = "ps_2_sw : mad source reg combination b0, b0, oC0 is NOT allowed";
string PS_136 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_137_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_137_Desc = "ps_2_sw : mad source reg combination b0, b0, oDepth is NOT allowed";
string PS_137 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_138_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_138_Desc = "ps_2_sw : mad source reg combination b0, i0, b0 is NOT allowed";
string PS_138 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_139_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_139_Desc = "ps_2_sw : mad source reg combination b0, i0, i0 is NOT allowed";
string PS_139 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_140_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_140_Desc = "ps_2_sw : mad source reg combination b0, i0, p0 is NOT allowed";
string PS_140 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_141_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_141_Desc = "ps_2_sw : mad source reg combination b0, i0, s0 is NOT allowed";
string PS_141 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_142_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_142_Desc = "ps_2_sw : mad source reg combination b0, i0, oC0 is NOT allowed";
string PS_142 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_143_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_143_Desc = "ps_2_sw : mad source reg combination b0, i0, oDepth is NOT allowed";
string PS_143 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_144_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_144_Desc = "ps_2_sw : mad source reg combination b0, p0, b0 is NOT allowed";
string PS_144 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_145_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_145_Desc = "ps_2_sw : mad source reg combination b0, p0, i0 is NOT allowed";
string PS_145 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_146_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_146_Desc = "ps_2_sw : mad source reg combination b0, p0, p0 is NOT allowed";
string PS_146 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_147_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_147_Desc = "ps_2_sw : mad source reg combination b0, p0, s0 is NOT allowed";
string PS_147 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_148_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_148_Desc = "ps_2_sw : mad source reg combination b0, p0, oC0 is NOT allowed";
string PS_148 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_149_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_149_Desc = "ps_2_sw : mad source reg combination b0, p0, oDepth is NOT allowed";
string PS_149 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_150_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_150_Desc = "ps_2_sw : mad source reg combination b0, s0, b0 is NOT allowed";
string PS_150 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_151_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_151_Desc = "ps_2_sw : mad source reg combination b0, s0, i0 is NOT allowed";
string PS_151 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_152_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_152_Desc = "ps_2_sw : mad source reg combination b0, s0, p0 is NOT allowed";
string PS_152 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_153_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_153_Desc = "ps_2_sw : mad source reg combination b0, s0, s0 is NOT allowed";
string PS_153 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_154_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_154_Desc = "ps_2_sw : mad source reg combination b0, s0, oC0 is NOT allowed";
string PS_154 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_155_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_155_Desc = "ps_2_sw : mad source reg combination b0, s0, oDepth is NOT allowed";
string PS_155 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_156_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_156_Desc = "ps_2_sw : mad source reg combination b0, oC0, b0 is NOT allowed";
string PS_156 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_157_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_157_Desc = "ps_2_sw : mad source reg combination b0, oC0, i0 is NOT allowed";
string PS_157 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_158_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_158_Desc = "ps_2_sw : mad source reg combination b0, oC0, p0 is NOT allowed";
string PS_158 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_159_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_159_Desc = "ps_2_sw : mad source reg combination b0, oC0, s0 is NOT allowed";
string PS_159 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_160_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_160_Desc = "ps_2_sw : mad source reg combination b0, oC0, oC0 is NOT allowed";
string PS_160 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_161_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_161_Desc = "ps_2_sw : mad source reg combination b0, oC0, oDepth is NOT allowed";
string PS_161 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_162_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_162_Desc = "ps_2_sw : mad source reg combination b0, oDepth, b0 is NOT allowed";
string PS_162 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_163_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_163_Desc = "ps_2_sw : mad source reg combination b0, oDepth, i0 is NOT allowed";
string PS_163 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_164_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_164_Desc = "ps_2_sw : mad source reg combination b0, oDepth, p0 is NOT allowed";
string PS_164 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_165_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_165_Desc = "ps_2_sw : mad source reg combination b0, oDepth, s0 is NOT allowed";
string PS_165 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_166_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_166_Desc = "ps_2_sw : mad source reg combination b0, oDepth, oC0 is NOT allowed";
string PS_166 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_167_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_167_Desc = "ps_2_sw : mad source reg combination b0, oDepth, oDepth is NOT allowed";
string PS_167 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, b0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_168_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_168_Desc = "ps_2_sw : mad source reg combination i0, b0, b0 is NOT allowed";
string PS_168 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_169_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_169_Desc = "ps_2_sw : mad source reg combination i0, b0, i0 is NOT allowed";
string PS_169 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_170_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_170_Desc = "ps_2_sw : mad source reg combination i0, b0, p0 is NOT allowed";
string PS_170 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_171_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_171_Desc = "ps_2_sw : mad source reg combination i0, b0, s0 is NOT allowed";
string PS_171 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mad r0, i0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_172_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_172_Desc = "ps_2_sw : mad source reg combination i0, b0, oC0 is NOT allowed";
string PS_172 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_173_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_173_Desc = "ps_2_sw : mad source reg combination i0, b0, oDepth is NOT allowed";
string PS_173 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_174_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_174_Desc = "ps_2_sw : mad source reg combination i0, i0, b0 is NOT allowed";
string PS_174 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_175_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_175_Desc = "ps_2_sw : mad source reg combination i0, i0, i0 is NOT allowed";
string PS_175 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_176_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_176_Desc = "ps_2_sw : mad source reg combination i0, i0, p0 is NOT allowed";
string PS_176 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_177_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_177_Desc = "ps_2_sw : mad source reg combination i0, i0, s0 is NOT allowed";
string PS_177 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mad r0, i0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_178_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_178_Desc = "ps_2_sw : mad source reg combination i0, i0, oC0 is NOT allowed";
string PS_178 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_179_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_179_Desc = "ps_2_sw : mad source reg combination i0, i0, oDepth is NOT allowed";
string PS_179 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_180_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_180_Desc = "ps_2_sw : mad source reg combination i0, p0, b0 is NOT allowed";
string PS_180 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_181_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_181_Desc = "ps_2_sw : mad source reg combination i0, p0, i0 is NOT allowed";
string PS_181 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_182_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_182_Desc = "ps_2_sw : mad source reg combination i0, p0, p0 is NOT allowed";
string PS_182 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_183_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_183_Desc = "ps_2_sw : mad source reg combination i0, p0, s0 is NOT allowed";
string PS_183 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mad r0, i0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_184_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_184_Desc = "ps_2_sw : mad source reg combination i0, p0, oC0 is NOT allowed";
string PS_184 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_185_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_185_Desc = "ps_2_sw : mad source reg combination i0, p0, oDepth is NOT allowed";
string PS_185 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_186_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_186_Desc = "ps_2_sw : mad source reg combination i0, s0, b0 is NOT allowed";
string PS_186 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, i0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_187_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_187_Desc = "ps_2_sw : mad source reg combination i0, s0, i0 is NOT allowed";
string PS_187 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, i0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_188_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_188_Desc = "ps_2_sw : mad source reg combination i0, s0, p0 is NOT allowed";
string PS_188 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, i0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_189_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_189_Desc = "ps_2_sw : mad source reg combination i0, s0, s0 is NOT allowed";
string PS_189 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, i0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_190_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_190_Desc = "ps_2_sw : mad source reg combination i0, s0, oC0 is NOT allowed";
string PS_190 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, i0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_191_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_191_Desc = "ps_2_sw : mad source reg combination i0, s0, oDepth is NOT allowed";
string PS_191 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, i0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_192_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_192_Desc = "ps_2_sw : mad source reg combination i0, oC0, b0 is NOT allowed";
string PS_192 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_193_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_193_Desc = "ps_2_sw : mad source reg combination i0, oC0, i0 is NOT allowed";
string PS_193 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_194_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_194_Desc = "ps_2_sw : mad source reg combination i0, oC0, p0 is NOT allowed";
string PS_194 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_195_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_195_Desc = "ps_2_sw : mad source reg combination i0, oC0, s0 is NOT allowed";
string PS_195 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, i0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_196_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_196_Desc = "ps_2_sw : mad source reg combination i0, oC0, oC0 is NOT allowed";
string PS_196 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_197_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_197_Desc = "ps_2_sw : mad source reg combination i0, oC0, oDepth is NOT allowed";
string PS_197 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_198_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_198_Desc = "ps_2_sw : mad source reg combination i0, oDepth, b0 is NOT allowed";
string PS_198 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_199_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_199_Desc = "ps_2_sw : mad source reg combination i0, oDepth, i0 is NOT allowed";
string PS_199 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_200_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_200_Desc = "ps_2_sw : mad source reg combination i0, oDepth, p0 is NOT allowed";
string PS_200 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_201_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_201_Desc = "ps_2_sw : mad source reg combination i0, oDepth, s0 is NOT allowed";
string PS_201 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, i0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_202_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_202_Desc = "ps_2_sw : mad source reg combination i0, oDepth, oC0 is NOT allowed";
string PS_202 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_203_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_203_Desc = "ps_2_sw : mad source reg combination i0, oDepth, oDepth is NOT allowed";
string PS_203 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_204_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_204_Desc = "ps_2_sw : mad source reg combination p0, b0, b0 is NOT allowed";
string PS_204 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, p0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_205_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_205_Desc = "ps_2_sw : mad source reg combination p0, b0, i0 is NOT allowed";
string PS_205 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, p0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_206_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_206_Desc = "ps_2_sw : mad source reg combination p0, b0, p0 is NOT allowed";
string PS_206 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, p0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_207_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_207_Desc = "ps_2_sw : mad source reg combination p0, b0, s0 is NOT allowed";
string PS_207 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_volume s0 "
	"mad r0, p0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_208_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_208_Desc = "ps_2_sw : mad source reg combination p0, b0, oC0 is NOT allowed";
string PS_208 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, p0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_209_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_209_Desc = "ps_2_sw : mad source reg combination p0, b0, oDepth is NOT allowed";
string PS_209 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, p0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_210_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_210_Desc = "ps_2_sw : mad source reg combination p0, i0, b0 is NOT allowed";
string PS_210 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_211_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_211_Desc = "ps_2_sw : mad source reg combination p0, i0, i0 is NOT allowed";
string PS_211 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_212_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_212_Desc = "ps_2_sw : mad source reg combination p0, i0, p0 is NOT allowed";
string PS_212 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_213_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_213_Desc = "ps_2_sw : mad source reg combination p0, i0, s0 is NOT allowed";
string PS_213 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, p0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_214_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_214_Desc = "ps_2_sw : mad source reg combination p0, i0, oC0 is NOT allowed";
string PS_214 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_215_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_215_Desc = "ps_2_sw : mad source reg combination p0, i0, oDepth is NOT allowed";
string PS_215 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_216_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_216_Desc = "ps_2_sw : mad source reg combination p0, p0, b0 is NOT allowed";
string PS_216 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, p0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_217_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_217_Desc = "ps_2_sw : mad source reg combination p0, p0, i0 is NOT allowed";
string PS_217 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_218_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_218_Desc = "ps_2_sw : mad source reg combination p0, p0, p0 is NOT allowed";
string PS_218 = 
	"ps_2_sw "
	"mad r0, p0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_219_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_219_Desc = "ps_2_sw : mad source reg combination p0, p0, s0 is NOT allowed";
string PS_219 = 
	"ps_2_sw "
	"dcl_volume s0 "
	"mad r0, p0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_220_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_220_Desc = "ps_2_sw : mad source reg combination p0, p0, oC0 is NOT allowed";
string PS_220 = 
	"ps_2_sw "
	"mad r0, p0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_221_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_221_Desc = "ps_2_sw : mad source reg combination p0, p0, oDepth is NOT allowed";
string PS_221 = 
	"ps_2_sw "
	"mad r0, p0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_222_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_222_Desc = "ps_2_sw : mad source reg combination p0, s0, b0 is NOT allowed";
string PS_222 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_volume s0 "
	"mad r0, p0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_223_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_223_Desc = "ps_2_sw : mad source reg combination p0, s0, i0 is NOT allowed";
string PS_223 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, p0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_224_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_224_Desc = "ps_2_sw : mad source reg combination p0, s0, p0 is NOT allowed";
string PS_224 = 
	"ps_2_sw "
	"dcl_volume s0 "
	"mad r0, p0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_225_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_225_Desc = "ps_2_sw : mad source reg combination p0, s0, s0 is NOT allowed";
string PS_225 = 
	"ps_2_sw "
	"dcl_volume s0 "
	"mad r0, p0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_226_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_226_Desc = "ps_2_sw : mad source reg combination p0, s0, oC0 is NOT allowed";
string PS_226 = 
	"ps_2_sw "
	"dcl_volume s0 "
	"mad r0, p0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_227_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_227_Desc = "ps_2_sw : mad source reg combination p0, s0, oDepth is NOT allowed";
string PS_227 = 
	"ps_2_sw "
	"dcl_volume s0 "
	"mad r0, p0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_228_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_228_Desc = "ps_2_sw : mad source reg combination p0, oC0, b0 is NOT allowed";
string PS_228 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, p0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_229_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_229_Desc = "ps_2_sw : mad source reg combination p0, oC0, i0 is NOT allowed";
string PS_229 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_230_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_230_Desc = "ps_2_sw : mad source reg combination p0, oC0, p0 is NOT allowed";
string PS_230 = 
	"ps_2_sw "
	"mad r0, p0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_231_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_231_Desc = "ps_2_sw : mad source reg combination p0, oC0, s0 is NOT allowed";
string PS_231 = 
	"ps_2_sw "
	"dcl_volume s0 "
	"mad r0, p0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_232_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_232_Desc = "ps_2_sw : mad source reg combination p0, oC0, oC0 is NOT allowed";
string PS_232 = 
	"ps_2_sw "
	"mad r0, p0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_233_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_233_Desc = "ps_2_sw : mad source reg combination p0, oC0, oDepth is NOT allowed";
string PS_233 = 
	"ps_2_sw "
	"mad r0, p0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_234_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_234_Desc = "ps_2_sw : mad source reg combination p0, oDepth, b0 is NOT allowed";
string PS_234 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, p0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_235_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_235_Desc = "ps_2_sw : mad source reg combination p0, oDepth, i0 is NOT allowed";
string PS_235 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_236_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_236_Desc = "ps_2_sw : mad source reg combination p0, oDepth, p0 is NOT allowed";
string PS_236 = 
	"ps_2_sw "
	"mad r0, p0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_237_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_237_Desc = "ps_2_sw : mad source reg combination p0, oDepth, s0 is NOT allowed";
string PS_237 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, p0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_238_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_238_Desc = "ps_2_sw : mad source reg combination p0, oDepth, oC0 is NOT allowed";
string PS_238 = 
	"ps_2_sw "
	"mad r0, p0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_239_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_239_Desc = "ps_2_sw : mad source reg combination p0, oDepth, oDepth is NOT allowed";
string PS_239 = 
	"ps_2_sw "
	"mad r0, p0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_240_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_240_Desc = "ps_2_sw : mad source reg combination s0, b0, b0 is NOT allowed";
string PS_240 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_241_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_241_Desc = "ps_2_sw : mad source reg combination s0, b0, i0 is NOT allowed";
string PS_241 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_242_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_242_Desc = "ps_2_sw : mad source reg combination s0, b0, p0 is NOT allowed";
string PS_242 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_243_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_243_Desc = "ps_2_sw : mad source reg combination s0, b0, s0 is NOT allowed";
string PS_243 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_244_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_244_Desc = "ps_2_sw : mad source reg combination s0, b0, oC0 is NOT allowed";
string PS_244 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_245_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_245_Desc = "ps_2_sw : mad source reg combination s0, b0, oDepth is NOT allowed";
string PS_245 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_246_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_246_Desc = "ps_2_sw : mad source reg combination s0, i0, b0 is NOT allowed";
string PS_246 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_247_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_247_Desc = "ps_2_sw : mad source reg combination s0, i0, i0 is NOT allowed";
string PS_247 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_248_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_248_Desc = "ps_2_sw : mad source reg combination s0, i0, p0 is NOT allowed";
string PS_248 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_249_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_249_Desc = "ps_2_sw : mad source reg combination s0, i0, s0 is NOT allowed";
string PS_249 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_250_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_250_Desc = "ps_2_sw : mad source reg combination s0, i0, oC0 is NOT allowed";
string PS_250 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_251_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_251_Desc = "ps_2_sw : mad source reg combination s0, i0, oDepth is NOT allowed";
string PS_251 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_252_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_252_Desc = "ps_2_sw : mad source reg combination s0, p0, b0 is NOT allowed";
string PS_252 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_253_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_253_Desc = "ps_2_sw : mad source reg combination s0, p0, i0 is NOT allowed";
string PS_253 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_254_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_254_Desc = "ps_2_sw : mad source reg combination s0, p0, p0 is NOT allowed";
string PS_254 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_255_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_255_Desc = "ps_2_sw : mad source reg combination s0, p0, s0 is NOT allowed";
string PS_255 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_256_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_256_Desc = "ps_2_sw : mad source reg combination s0, p0, oC0 is NOT allowed";
string PS_256 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_257_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_257_Desc = "ps_2_sw : mad source reg combination s0, p0, oDepth is NOT allowed";
string PS_257 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_258_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_258_Desc = "ps_2_sw : mad source reg combination s0, s0, b0 is NOT allowed";
string PS_258 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_259_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_259_Desc = "ps_2_sw : mad source reg combination s0, s0, i0 is NOT allowed";
string PS_259 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_260_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_260_Desc = "ps_2_sw : mad source reg combination s0, s0, p0 is NOT allowed";
string PS_260 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_261_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_261_Desc = "ps_2_sw : mad source reg combination s0, s0, s0 is NOT allowed";
string PS_261 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_262_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_262_Desc = "ps_2_sw : mad source reg combination s0, s0, oC0 is NOT allowed";
string PS_262 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_263_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_263_Desc = "ps_2_sw : mad source reg combination s0, s0, oDepth is NOT allowed";
string PS_263 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_264_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_264_Desc = "ps_2_sw : mad source reg combination s0, oC0, b0 is NOT allowed";
string PS_264 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_265_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_265_Desc = "ps_2_sw : mad source reg combination s0, oC0, i0 is NOT allowed";
string PS_265 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_266_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_266_Desc = "ps_2_sw : mad source reg combination s0, oC0, p0 is NOT allowed";
string PS_266 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_267_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_267_Desc = "ps_2_sw : mad source reg combination s0, oC0, s0 is NOT allowed";
string PS_267 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_268_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_268_Desc = "ps_2_sw : mad source reg combination s0, oC0, oC0 is NOT allowed";
string PS_268 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_269_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_269_Desc = "ps_2_sw : mad source reg combination s0, oC0, oDepth is NOT allowed";
string PS_269 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_270_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_270_Desc = "ps_2_sw : mad source reg combination s0, oDepth, b0 is NOT allowed";
string PS_270 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_271_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_271_Desc = "ps_2_sw : mad source reg combination s0, oDepth, i0 is NOT allowed";
string PS_271 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_272_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_272_Desc = "ps_2_sw : mad source reg combination s0, oDepth, p0 is NOT allowed";
string PS_272 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_273_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_273_Desc = "ps_2_sw : mad source reg combination s0, oDepth, s0 is NOT allowed";
string PS_273 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_274_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_274_Desc = "ps_2_sw : mad source reg combination s0, oDepth, oC0 is NOT allowed";
string PS_274 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_275_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_275_Desc = "ps_2_sw : mad source reg combination s0, oDepth, oDepth is NOT allowed";
string PS_275 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, s0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_276_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_276_Desc = "ps_2_sw : mad source reg combination oC0, b0, b0 is NOT allowed";
string PS_276 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oC0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_277_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_277_Desc = "ps_2_sw : mad source reg combination oC0, b0, i0 is NOT allowed";
string PS_277 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, oC0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_278_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_278_Desc = "ps_2_sw : mad source reg combination oC0, b0, p0 is NOT allowed";
string PS_278 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oC0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_279_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_279_Desc = "ps_2_sw : mad source reg combination oC0, b0, s0 is NOT allowed";
string PS_279 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, oC0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_280_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_280_Desc = "ps_2_sw : mad source reg combination oC0, b0, oC0 is NOT allowed";
string PS_280 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oC0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_281_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_281_Desc = "ps_2_sw : mad source reg combination oC0, b0, oDepth is NOT allowed";
string PS_281 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oC0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_282_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_282_Desc = "ps_2_sw : mad source reg combination oC0, i0, b0 is NOT allowed";
string PS_282 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oC0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_283_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_283_Desc = "ps_2_sw : mad source reg combination oC0, i0, i0 is NOT allowed";
string PS_283 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oC0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_284_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_284_Desc = "ps_2_sw : mad source reg combination oC0, i0, p0 is NOT allowed";
string PS_284 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oC0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_285_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_285_Desc = "ps_2_sw : mad source reg combination oC0, i0, s0 is NOT allowed";
string PS_285 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, oC0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_286_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_286_Desc = "ps_2_sw : mad source reg combination oC0, i0, oC0 is NOT allowed";
string PS_286 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oC0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_287_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_287_Desc = "ps_2_sw : mad source reg combination oC0, i0, oDepth is NOT allowed";
string PS_287 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oC0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_288_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_288_Desc = "ps_2_sw : mad source reg combination oC0, p0, b0 is NOT allowed";
string PS_288 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oC0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_289_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_289_Desc = "ps_2_sw : mad source reg combination oC0, p0, i0 is NOT allowed";
string PS_289 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oC0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_290_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_290_Desc = "ps_2_sw : mad source reg combination oC0, p0, p0 is NOT allowed";
string PS_290 = 
	"ps_2_sw "
	"mad r0, oC0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_291_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_291_Desc = "ps_2_sw : mad source reg combination oC0, p0, s0 is NOT allowed";
string PS_291 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, oC0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_292_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_292_Desc = "ps_2_sw : mad source reg combination oC0, p0, oC0 is NOT allowed";
string PS_292 = 
	"ps_2_sw "
	"mad r0, oC0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_293_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_293_Desc = "ps_2_sw : mad source reg combination oC0, p0, oDepth is NOT allowed";
string PS_293 = 
	"ps_2_sw "
	"mad r0, oC0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_294_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_294_Desc = "ps_2_sw : mad source reg combination oC0, s0, b0 is NOT allowed";
string PS_294 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, oC0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_295_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_295_Desc = "ps_2_sw : mad source reg combination oC0, s0, i0 is NOT allowed";
string PS_295 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, oC0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_296_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_296_Desc = "ps_2_sw : mad source reg combination oC0, s0, p0 is NOT allowed";
string PS_296 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, oC0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_297_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_297_Desc = "ps_2_sw : mad source reg combination oC0, s0, s0 is NOT allowed";
string PS_297 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, oC0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_298_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_298_Desc = "ps_2_sw : mad source reg combination oC0, s0, oC0 is NOT allowed";
string PS_298 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, oC0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_299_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_299_Desc = "ps_2_sw : mad source reg combination oC0, s0, oDepth is NOT allowed";
string PS_299 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, oC0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_300_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_300_Desc = "ps_2_sw : mad source reg combination oC0, oC0, b0 is NOT allowed";
string PS_300 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oC0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_301_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_301_Desc = "ps_2_sw : mad source reg combination oC0, oC0, i0 is NOT allowed";
string PS_301 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oC0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_302_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_302_Desc = "ps_2_sw : mad source reg combination oC0, oC0, p0 is NOT allowed";
string PS_302 = 
	"ps_2_sw "
	"mad r0, oC0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_303_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_303_Desc = "ps_2_sw : mad source reg combination oC0, oC0, s0 is NOT allowed";
string PS_303 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, oC0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_304_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_304_Desc = "ps_2_sw : mad source reg combination oC0, oC0, oC0 is NOT allowed";
string PS_304 = 
	"ps_2_sw "
	"mad r0, oC0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_305_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_305_Desc = "ps_2_sw : mad source reg combination oC0, oC0, oDepth is NOT allowed";
string PS_305 = 
	"ps_2_sw "
	"mad r0, oC0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_306_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_306_Desc = "ps_2_sw : mad source reg combination oC0, oDepth, b0 is NOT allowed";
string PS_306 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oC0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_307_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_307_Desc = "ps_2_sw : mad source reg combination oC0, oDepth, i0 is NOT allowed";
string PS_307 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oC0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_308_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_308_Desc = "ps_2_sw : mad source reg combination oC0, oDepth, p0 is NOT allowed";
string PS_308 = 
	"ps_2_sw "
	"mad r0, oC0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_309_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_309_Desc = "ps_2_sw : mad source reg combination oC0, oDepth, s0 is NOT allowed";
string PS_309 = 
	"ps_2_sw "
	"dcl_cube s0 "
	"mad r0, oC0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_310_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_310_Desc = "ps_2_sw : mad source reg combination oC0, oDepth, oC0 is NOT allowed";
string PS_310 = 
	"ps_2_sw "
	"mad r0, oC0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_311_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_311_Desc = "ps_2_sw : mad source reg combination oC0, oDepth, oDepth is NOT allowed";
string PS_311 = 
	"ps_2_sw "
	"mad r0, oC0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_312_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_312_Desc = "ps_2_sw : mad source reg combination oDepth, b0, b0 is NOT allowed";
string PS_312 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oDepth, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_313_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_313_Desc = "ps_2_sw : mad source reg combination oDepth, b0, i0 is NOT allowed";
string PS_313 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, oDepth, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_314_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_314_Desc = "ps_2_sw : mad source reg combination oDepth, b0, p0 is NOT allowed";
string PS_314 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oDepth, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_315_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_315_Desc = "ps_2_sw : mad source reg combination oDepth, b0, s0 is NOT allowed";
string PS_315 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, oDepth, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_316_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_316_Desc = "ps_2_sw : mad source reg combination oDepth, b0, oC0 is NOT allowed";
string PS_316 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oDepth, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_317_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_317_Desc = "ps_2_sw : mad source reg combination oDepth, b0, oDepth is NOT allowed";
string PS_317 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oDepth, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_318_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_318_Desc = "ps_2_sw : mad source reg combination oDepth, i0, b0 is NOT allowed";
string PS_318 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oDepth, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_319_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_319_Desc = "ps_2_sw : mad source reg combination oDepth, i0, i0 is NOT allowed";
string PS_319 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oDepth, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_320_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_320_Desc = "ps_2_sw : mad source reg combination oDepth, i0, p0 is NOT allowed";
string PS_320 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oDepth, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_321_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_321_Desc = "ps_2_sw : mad source reg combination oDepth, i0, s0 is NOT allowed";
string PS_321 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mad r0, oDepth, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_322_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_322_Desc = "ps_2_sw : mad source reg combination oDepth, i0, oC0 is NOT allowed";
string PS_322 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oDepth, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_323_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_323_Desc = "ps_2_sw : mad source reg combination oDepth, i0, oDepth is NOT allowed";
string PS_323 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oDepth, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_324_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_324_Desc = "ps_2_sw : mad source reg combination oDepth, p0, b0 is NOT allowed";
string PS_324 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oDepth, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_325_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_325_Desc = "ps_2_sw : mad source reg combination oDepth, p0, i0 is NOT allowed";
string PS_325 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oDepth, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_326_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_326_Desc = "ps_2_sw : mad source reg combination oDepth, p0, p0 is NOT allowed";
string PS_326 = 
	"ps_2_sw "
	"mad r0, oDepth, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_327_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_327_Desc = "ps_2_sw : mad source reg combination oDepth, p0, s0 is NOT allowed";
string PS_327 = 
	"ps_2_sw "
	"dcl_cube s0 "
	"mad r0, oDepth, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_328_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_328_Desc = "ps_2_sw : mad source reg combination oDepth, p0, oC0 is NOT allowed";
string PS_328 = 
	"ps_2_sw "
	"mad r0, oDepth, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_329_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_329_Desc = "ps_2_sw : mad source reg combination oDepth, p0, oDepth is NOT allowed";
string PS_329 = 
	"ps_2_sw "
	"mad r0, oDepth, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_330_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_330_Desc = "ps_2_sw : mad source reg combination oDepth, s0, b0 is NOT allowed";
string PS_330 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, oDepth, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_331_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_331_Desc = "ps_2_sw : mad source reg combination oDepth, s0, i0 is NOT allowed";
string PS_331 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mad r0, oDepth, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_332_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_332_Desc = "ps_2_sw : mad source reg combination oDepth, s0, p0 is NOT allowed";
string PS_332 = 
	"ps_2_sw "
	"dcl_cube s0 "
	"mad r0, oDepth, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_333_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_333_Desc = "ps_2_sw : mad source reg combination oDepth, s0, s0 is NOT allowed";
string PS_333 = 
	"ps_2_sw "
	"dcl_cube s0 "
	"mad r0, oDepth, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_334_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_334_Desc = "ps_2_sw : mad source reg combination oDepth, s0, oC0 is NOT allowed";
string PS_334 = 
	"ps_2_sw "
	"dcl_cube s0 "
	"mad r0, oDepth, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_335_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_335_Desc = "ps_2_sw : mad source reg combination oDepth, s0, oDepth is NOT allowed";
string PS_335 = 
	"ps_2_sw "
	"dcl_cube s0 "
	"mad r0, oDepth, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_336_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_336_Desc = "ps_2_sw : mad source reg combination oDepth, oC0, b0 is NOT allowed";
string PS_336 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oDepth, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_337_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_337_Desc = "ps_2_sw : mad source reg combination oDepth, oC0, i0 is NOT allowed";
string PS_337 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oDepth, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_338_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_338_Desc = "ps_2_sw : mad source reg combination oDepth, oC0, p0 is NOT allowed";
string PS_338 = 
	"ps_2_sw "
	"mad r0, oDepth, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_339_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_339_Desc = "ps_2_sw : mad source reg combination oDepth, oC0, s0 is NOT allowed";
string PS_339 = 
	"ps_2_sw "
	"dcl_cube s0 "
	"mad r0, oDepth, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_340_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_340_Desc = "ps_2_sw : mad source reg combination oDepth, oC0, oC0 is NOT allowed";
string PS_340 = 
	"ps_2_sw "
	"mad r0, oDepth, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_341_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_341_Desc = "ps_2_sw : mad source reg combination oDepth, oC0, oDepth is NOT allowed";
string PS_341 = 
	"ps_2_sw "
	"mad r0, oDepth, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_342_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_342_Desc = "ps_2_sw : mad source reg combination oDepth, oDepth, b0 is NOT allowed";
string PS_342 = 
	"ps_2_sw "
	"defb b0, true "
	"mad r0, oDepth, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_343_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_343_Desc = "ps_2_sw : mad source reg combination oDepth, oDepth, i0 is NOT allowed";
string PS_343 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oDepth, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_344_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_344_Desc = "ps_2_sw : mad source reg combination oDepth, oDepth, p0 is NOT allowed";
string PS_344 = 
	"ps_2_sw "
	"mad r0, oDepth, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_345_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_345_Desc = "ps_2_sw : mad source reg combination oDepth, oDepth, s0 is NOT allowed";
string PS_345 = 
	"ps_2_sw "
	"dcl_2d s0 "
	"mad r0, oDepth, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_346_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_346_Desc = "ps_2_sw : mad source reg combination oDepth, oDepth, oC0 is NOT allowed";
string PS_346 = 
	"ps_2_sw "
	"mad r0, oDepth, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_347_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_347_Desc = "ps_2_sw : mad source reg combination oDepth, oDepth, oDepth is NOT allowed";
string PS_347 = 
	"ps_2_sw "
	"mad r0, oDepth, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_348_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_348_Desc = "ps_2_sw : mad dest r# is allowed";
string PS_348 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_349_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_349_Desc = "ps_2_sw : mad _pp dest r# is allowed";
string PS_349 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_pp r0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_350_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_350_Desc = "ps_2_sw : mad _sat dest r# is allowed";
string PS_350 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_sat r0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_351_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_351_Desc = "ps_2_sw : mad dest v# is NOT allowed";
string PS_351 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad v0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_352_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_352_Desc = "ps_2_sw : mad _pp dest v# is NOT allowed";
string PS_352 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_pp v0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_353_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_353_Desc = "ps_2_sw : mad _sat dest v# is NOT allowed";
string PS_353 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_sat v0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_354_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_354_Desc = "ps_2_sw : mad dest c# is NOT allowed";
string PS_354 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad c1, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_355_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_355_Desc = "ps_2_sw : mad _pp dest c# is NOT allowed";
string PS_355 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_pp c1, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_356_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_356_Desc = "ps_2_sw : mad _sat dest c# is NOT allowed";
string PS_356 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_sat c1, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_357_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_357_Desc = "ps_2_sw : mad dest b# is NOT allowed";
string PS_357 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad b0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_358_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_358_Desc = "ps_2_sw : mad _pp dest b# is NOT allowed";
string PS_358 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_pp b0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_359_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_359_Desc = "ps_2_sw : mad _sat dest b# is NOT allowed";
string PS_359 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_sat b0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_360_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_360_Desc = "ps_2_sw : mad dest i# is NOT allowed";
string PS_360 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad i0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_361_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_361_Desc = "ps_2_sw : mad _pp dest i# is NOT allowed";
string PS_361 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_pp i0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_362_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_362_Desc = "ps_2_sw : mad _sat dest i# is NOT allowed";
string PS_362 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_sat i0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_363_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_363_Desc = "ps_2_sw : mad dest p0 is NOT allowed";
string PS_363 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad p0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_364_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_364_Desc = "ps_2_sw : mad _pp dest p0 is NOT allowed";
string PS_364 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_pp p0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_365_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_365_Desc = "ps_2_sw : mad _sat dest p0 is NOT allowed";
string PS_365 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_sat p0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_366_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_366_Desc = "ps_2_sw : mad dest s# is NOT allowed";
string PS_366 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad s0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_367_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_367_Desc = "ps_2_sw : mad _pp dest s# is NOT allowed";
string PS_367 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_pp s0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_368_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_368_Desc = "ps_2_sw : mad _sat dest s# is NOT allowed";
string PS_368 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_sat s0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_369_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_369_Desc = "ps_2_sw : mad dest t# is NOT allowed";
string PS_369 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad t0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_370_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_370_Desc = "ps_2_sw : mad _pp dest t# is NOT allowed";
string PS_370 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_pp t0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_371_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_371_Desc = "ps_2_sw : mad _sat dest t# is NOT allowed";
string PS_371 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_sat t0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_372_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_372_Desc = "ps_2_sw : mad dest oC# is NOT allowed";
string PS_372 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad oC0, v0, v1, r0 ";

PS_CRITERIA PS_373_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_373_Desc = "ps_2_sw : mad _pp dest oC# is NOT allowed";
string PS_373 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_pp oC0, v0, v1, r0 ";

PS_CRITERIA PS_374_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_374_Desc = "ps_2_sw : mad _sat dest oC# is NOT allowed";
string PS_374 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_sat oC0, v0, v1, r0 ";

PS_CRITERIA PS_375_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_375_Desc = "ps_2_sw : mad dest oDepth is NOT allowed";
string PS_375 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad oDepth, v0.x, v1.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_376_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_376_Desc = "ps_2_sw : mad _pp dest oDepth is NOT allowed";
string PS_376 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_pp oDepth, v0.x, v1.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_377_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_377_Desc = "ps_2_sw : mad _sat dest oDepth is NOT allowed";
string PS_377 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad_sat oDepth, v0.x, v1.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_378_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_378_Desc = "ps_2_sw : mad write mask .r is allowed";
string PS_378 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.r, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_379_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_379_Desc = "ps_2_sw : mad write mask .g is allowed";
string PS_379 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.g, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_380_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_380_Desc = "ps_2_sw : mad write mask .b is allowed";
string PS_380 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.b, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_381_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_381_Desc = "ps_2_sw : mad write mask .a is allowed";
string PS_381 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.a, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_382_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_382_Desc = "ps_2_sw : mad write mask .rg is allowed";
string PS_382 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.rg, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_383_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_383_Desc = "ps_2_sw : mad write mask .gb is allowed";
string PS_383 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.gb, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_384_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_384_Desc = "ps_2_sw : mad write mask .ba is allowed";
string PS_384 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.ba, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_385_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_385_Desc = "ps_2_sw : mad write mask .rb is allowed";
string PS_385 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.rb, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_386_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_386_Desc = "ps_2_sw : mad write mask .ra is allowed";
string PS_386 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.ra, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_387_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_387_Desc = "ps_2_sw : mad write mask .ga is allowed";
string PS_387 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.ga, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_388_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_388_Desc = "ps_2_sw : mad write mask .rgb is allowed";
string PS_388 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.rgb, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_389_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_389_Desc = "ps_2_sw : mad write mask .gba is allowed";
string PS_389 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.gba, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_390_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_390_Desc = "ps_2_sw : mad write mask .rba is allowed";
string PS_390 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.rba, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_391_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_391_Desc = "ps_2_sw : mad write mask .rgba is allowed";
string PS_391 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.rgba, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_392_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_392_Desc = "ps_2_sw : mad write mask .yx is NOT allowed";
string PS_392 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.yx, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_393_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_393_Desc = "ps_2_sw : mad write mask .zx is NOT allowed";
string PS_393 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.zx, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_394_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_394_Desc = "ps_2_sw : mad write mask .zy is NOT allowed";
string PS_394 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.zy, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_395_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_395_Desc = "ps_2_sw : mad write mask .wx is NOT allowed";
string PS_395 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.wx, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_396_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_396_Desc = "ps_2_sw : mad write mask .wz is NOT allowed";
string PS_396 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.wz, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_397_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_397_Desc = "ps_2_sw : mad write mask .wy is NOT allowed";
string PS_397 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.wy, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_398_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_398_Desc = "ps_2_sw : mad write mask .zyx is NOT allowed";
string PS_398 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.zyx, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_399_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_399_Desc = "ps_2_sw : mad write mask .wzy is NOT allowed";
string PS_399 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.wzy, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_400_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_400_Desc = "ps_2_sw : mad write mask .wzx is NOT allowed";
string PS_400 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.wzx, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_401_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_401_Desc = "ps_2_sw : mad write mask .wyx is NOT allowed";
string PS_401 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.wyx, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_402_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_402_Desc = "ps_2_sw : mad write mask .yxw is NOT allowed";
string PS_402 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.yxw, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_403_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_403_Desc = "ps_2_sw : mad write mask .wzyx is NOT allowed";
string PS_403 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.wzyx, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_404_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_404_Desc = "ps_2_sw : mad write mask .zxwy is NOT allowed";
string PS_404 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"mad r0.zxwy, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_405_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_405_Desc = "ps_2_sw : mad is allowed in a 1 level if b0 block";
string PS_405 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	mad r0, v0, v1, r0 "
	"endif ";

PS_CRITERIA PS_406_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_406_Desc = "ps_2_sw : mad is allowed in a 8 level if b0 block";
string PS_406 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								mad r0, v0, v1, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_407_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_407_Desc = "ps_2_sw : mad is allowed in a 16 level if b0 block";
string PS_407 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_408_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_408_Desc = "ps_2_sw : mad is allowed in a 24 level if b0 block";
string PS_408 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_409_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_409_Desc = "ps_2_sw : mad is NOT allowed in a 25 level if b0 block";
string PS_409 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_410_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_410_Desc = "ps_2_sw : mad is allowed in a 1 level if b0 nop else block";
string PS_410 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	mad r0, v0, v1, r0 "
	"endif ";

PS_CRITERIA PS_411_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_411_Desc = "ps_2_sw : mad is allowed in a 8 level if b0 nop else block";
string PS_411 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								mad r0, v0, v1, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_412_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_412_Desc = "ps_2_sw : mad is allowed in a 16 level if b0 nop else block";
string PS_412 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_413_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_413_Desc = "ps_2_sw : mad is allowed in a 24 level if b0 nop else block";
string PS_413 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_414_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_414_Desc = "ps_2_sw : mad is NOT allowed in a 25 level if b0 nop else block";
string PS_414 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_415_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_415_Desc = "ps_2_sw : mad is NOT allowed in a 1 level loop aL, i0 block";
string PS_415 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	mad r0, v0, v1, r0 "
	"endloop ";

PS_CRITERIA PS_416_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_416_Desc = "ps_2_sw : mad is NOT allowed in a 2 level loop aL, i0 block";
string PS_416 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_417_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_417_Desc = "ps_2_sw : mad is NOT allowed in a 3 level loop aL, i0 block";
string PS_417 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			mad r0, v0, v1, r0 "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_418_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_418_Desc = "ps_2_sw : mad is NOT allowed in a 4 level loop aL, i0 block";
string PS_418 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				mad r0, v0, v1, r0 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_419_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_419_Desc = "ps_2_sw : mad is NOT allowed in a 5 level loop aL, i0 block";
string PS_419 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					mad r0, v0, v1, r0 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_420_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_420_Desc = "ps_2_sw : mad is allowed in a 1 level rep i0 block";
string PS_420 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	mad r0, v0, v1, r0 "
	"endrep ";

PS_CRITERIA PS_421_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_421_Desc = "ps_2_sw : mad is allowed in a 2 level rep i0 block";
string PS_421 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_422_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_422_Desc = "ps_2_sw : mad is allowed in a 3 level rep i0 block";
string PS_422 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			mad r0, v0, v1, r0 "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_423_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_423_Desc = "ps_2_sw : mad is allowed in a 4 level rep i0 block";
string PS_423 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				mad r0, v0, v1, r0 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_424_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_424_Desc = "ps_2_sw : mad is NOT allowed in a 5 level rep i0 block";
string PS_424 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					mad r0, v0, v1, r0 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_425_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_425_Desc = "ps_2_sw : mad is allowed in a 1 level if_lt c0.x, c0.y block";
string PS_425 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	mad r0, v0, v1, r0 "
	"endif ";

PS_CRITERIA PS_426_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_426_Desc = "ps_2_sw : mad is allowed in a 8 level if_lt c0.x, c0.y block";
string PS_426 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								mad r0, v0, v1, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_427_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_427_Desc = "ps_2_sw : mad is allowed in a 16 level if_lt c0.x, c0.y block";
string PS_427 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								if_lt c0.x, c0.y "
	"									if_lt c0.x, c0.y "
	"										if_lt c0.x, c0.y "
	"											if_lt c0.x, c0.y "
	"												if_lt c0.x, c0.y "
	"													if_lt c0.x, c0.y "
	"														if_lt c0.x, c0.y "
	"															if_lt c0.x, c0.y "
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_428_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_428_Desc = "ps_2_sw : mad is allowed in a 24 level if_lt c0.x, c0.y block";
string PS_428 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								if_lt c0.x, c0.y "
	"									if_lt c0.x, c0.y "
	"										if_lt c0.x, c0.y "
	"											if_lt c0.x, c0.y "
	"												if_lt c0.x, c0.y "
	"													if_lt c0.x, c0.y "
	"														if_lt c0.x, c0.y "
	"															if_lt c0.x, c0.y "
	"																if_lt c0.x, c0.y "
	"																	if_lt c0.x, c0.y "
	"																		if_lt c0.x, c0.y "
	"																			if_lt c0.x, c0.y "
	"																				if_lt c0.x, c0.y "
	"																					if_lt c0.x, c0.y "
	"																						if_lt c0.x, c0.y "
	"																							if_lt c0.x, c0.y "
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_429_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_429_Desc = "ps_2_sw : mad is NOT allowed in a 25 level if_lt c0.x, c0.y block";
string PS_429 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								if_lt c0.x, c0.y "
	"									if_lt c0.x, c0.y "
	"										if_lt c0.x, c0.y "
	"											if_lt c0.x, c0.y "
	"												if_lt c0.x, c0.y "
	"													if_lt c0.x, c0.y "
	"														if_lt c0.x, c0.y "
	"															if_lt c0.x, c0.y "
	"																if_lt c0.x, c0.y "
	"																	if_lt c0.x, c0.y "
	"																		if_lt c0.x, c0.y "
	"																			if_lt c0.x, c0.y "
	"																				if_lt c0.x, c0.y "
	"																					if_lt c0.x, c0.y "
	"																						if_lt c0.x, c0.y "
	"																							if_lt c0.x, c0.y "
	"																								if_lt c0.x, c0.y "
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_430_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_430_Desc = "ps_2_sw : mad is allowed in a 1 level if_lt c0.x, c0.y nop else block";
string PS_430 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	mad r0, v0, v1, r0 "
	"endif ";

PS_CRITERIA PS_431_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_431_Desc = "ps_2_sw : mad is allowed in a 8 level if_lt c0.x, c0.y nop else block";
string PS_431 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								mad r0, v0, v1, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_432_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_432_Desc = "ps_2_sw : mad is allowed in a 16 level if_lt c0.x, c0.y nop else block";
string PS_432 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								if_lt c0.x, c0.y nop else "
	"									if_lt c0.x, c0.y nop else "
	"										if_lt c0.x, c0.y nop else "
	"											if_lt c0.x, c0.y nop else "
	"												if_lt c0.x, c0.y nop else "
	"													if_lt c0.x, c0.y nop else "
	"														if_lt c0.x, c0.y nop else "
	"															if_lt c0.x, c0.y nop else "
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_433_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_433_Desc = "ps_2_sw : mad is allowed in a 24 level if_lt c0.x, c0.y nop else block";
string PS_433 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								if_lt c0.x, c0.y nop else "
	"									if_lt c0.x, c0.y nop else "
	"										if_lt c0.x, c0.y nop else "
	"											if_lt c0.x, c0.y nop else "
	"												if_lt c0.x, c0.y nop else "
	"													if_lt c0.x, c0.y nop else "
	"														if_lt c0.x, c0.y nop else "
	"															if_lt c0.x, c0.y nop else "
	"																if_lt c0.x, c0.y nop else "
	"																	if_lt c0.x, c0.y nop else "
	"																		if_lt c0.x, c0.y nop else "
	"																			if_lt c0.x, c0.y nop else "
	"																				if_lt c0.x, c0.y nop else "
	"																					if_lt c0.x, c0.y nop else "
	"																						if_lt c0.x, c0.y nop else "
	"																							if_lt c0.x, c0.y nop else "
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_434_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_434_Desc = "ps_2_sw : mad is NOT allowed in a 25 level if_lt c0.x, c0.y nop else block";
string PS_434 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								if_lt c0.x, c0.y nop else "
	"									if_lt c0.x, c0.y nop else "
	"										if_lt c0.x, c0.y nop else "
	"											if_lt c0.x, c0.y nop else "
	"												if_lt c0.x, c0.y nop else "
	"													if_lt c0.x, c0.y nop else "
	"														if_lt c0.x, c0.y nop else "
	"															if_lt c0.x, c0.y nop else "
	"																if_lt c0.x, c0.y nop else "
	"																	if_lt c0.x, c0.y nop else "
	"																		if_lt c0.x, c0.y nop else "
	"																			if_lt c0.x, c0.y nop else "
	"																				if_lt c0.x, c0.y nop else "
	"																					if_lt c0.x, c0.y nop else "
	"																						if_lt c0.x, c0.y nop else "
	"																							if_lt c0.x, c0.y nop else "
	"																								if_lt c0.x, c0.y nop else "
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_435_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_435_Desc = "ps_2_sw : mad is allowed in a 1 level if p0.x block";
string PS_435 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	mad r0, v0, v1, r0 "
	"endif ";

PS_CRITERIA PS_436_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_436_Desc = "ps_2_sw : mad is allowed in a 8 level if p0.x block";
string PS_436 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								mad r0, v0, v1, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_437_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_437_Desc = "ps_2_sw : mad is allowed in a 16 level if p0.x block";
string PS_437 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_438_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_438_Desc = "ps_2_sw : mad is allowed in a 24 level if p0.x block";
string PS_438 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_439_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_439_Desc = "ps_2_sw : mad is NOT allowed in a 25 level if p0.x block";
string PS_439 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_440_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_440_Desc = "ps_2_sw : mad is allowed in a 1 level if p0.x nop else block";
string PS_440 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	mad r0, v0, v1, r0 "
	"endif ";

PS_CRITERIA PS_441_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_441_Desc = "ps_2_sw : mad is allowed in a 8 level if p0.x nop else block";
string PS_441 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								mad r0, v0, v1, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_442_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_442_Desc = "ps_2_sw : mad is allowed in a 16 level if p0.x nop else block";
string PS_442 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_443_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_443_Desc = "ps_2_sw : mad is allowed in a 24 level if p0.x nop else block";
string PS_443 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_444_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_444_Desc = "ps_2_sw : mad is NOT allowed in a 25 level if p0.x nop else block";
string PS_444 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_445_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_445_Desc = "ps_2_sw : mad is NOT allowed in a 1 level loop aL, i0 break block";
string PS_445 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	mad r0, v0, v1, r0 "
	"endloop ";

PS_CRITERIA PS_446_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_446_Desc = "ps_2_sw : mad is NOT allowed in a 8 level loop aL, i0 break block";
string PS_446 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								mad r0, v0, v1, r0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_447_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_447_Desc = "ps_2_sw : mad is NOT allowed in a 16 level loop aL, i0 break block";
string PS_447 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_448_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_448_Desc = "ps_2_sw : mad is NOT allowed in a 24 level loop aL, i0 break block";
string PS_448 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_449_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_449_Desc = "ps_2_sw : mad is NOT allowed in a 25 level loop aL, i0 break block";
string PS_449 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_450_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_450_Desc = "ps_2_sw : mad is allowed in a 1 level rep i0 break block";
string PS_450 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	mad r0, v0, v1, r0 "
	"endrep ";

PS_CRITERIA PS_451_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_451_Desc = "ps_2_sw : mad is NOT allowed in a 8 level rep i0 break block";
string PS_451 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								mad r0, v0, v1, r0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_452_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_452_Desc = "ps_2_sw : mad is NOT allowed in a 16 level rep i0 break block";
string PS_452 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_453_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_453_Desc = "ps_2_sw : mad is NOT allowed in a 24 level rep i0 break block";
string PS_453 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_454_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_454_Desc = "ps_2_sw : mad is NOT allowed in a 25 level rep i0 break block";
string PS_454 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_455_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_455_Desc = "ps_2_sw : mad is NOT allowed in a 1 level loop aL, i0 breakp p0.x block";
string PS_455 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	mad r0, v0, v1, r0 "
	"endloop ";

PS_CRITERIA PS_456_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_456_Desc = "ps_2_sw : mad is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string PS_456 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								mad r0, v0, v1, r0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_457_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_457_Desc = "ps_2_sw : mad is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string PS_457 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_458_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_458_Desc = "ps_2_sw : mad is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string PS_458 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_459_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_459_Desc = "ps_2_sw : mad is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string PS_459 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_460_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_460_Desc = "ps_2_sw : mad is allowed in a 1 level rep i0 breakp p0.x block";
string PS_460 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	mad r0, v0, v1, r0 "
	"endrep ";

PS_CRITERIA PS_461_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_461_Desc = "ps_2_sw : mad is NOT allowed in a 8 level rep i0 breakp p0.x block";
string PS_461 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								mad r0, v0, v1, r0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_462_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_462_Desc = "ps_2_sw : mad is NOT allowed in a 16 level rep i0 breakp p0.x block";
string PS_462 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_463_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_463_Desc = "ps_2_sw : mad is NOT allowed in a 24 level rep i0 breakp p0.x block";
string PS_463 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_464_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_464_Desc = "ps_2_sw : mad is NOT allowed in a 25 level rep i0 breakp p0.x block";
string PS_464 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
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
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_465_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_465_Desc = "ps_2_sw : mad is NOT allowed in a 1 level loop aL, i0 break_le c0.x, c0.y block";
string PS_465 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	mad r0, v0, v1, r0 "
	"endloop ";

PS_CRITERIA PS_466_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_466_Desc = "ps_2_sw : mad is NOT allowed in a 8 level loop aL, i0 break_le c0.x, c0.y block";
string PS_466 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		loop aL, i0 break_le c0.x, c0.y "
	"			loop aL, i0 break_le c0.x, c0.y "
	"				loop aL, i0 break_le c0.x, c0.y "
	"					loop aL, i0 break_le c0.x, c0.y "
	"						loop aL, i0 break_le c0.x, c0.y "
	"							loop aL, i0 break_le c0.x, c0.y "
	"								mad r0, v0, v1, r0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_467_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_467_Desc = "ps_2_sw : mad is NOT allowed in a 16 level loop aL, i0 break_le c0.x, c0.y block";
string PS_467 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		loop aL, i0 break_le c0.x, c0.y "
	"			loop aL, i0 break_le c0.x, c0.y "
	"				loop aL, i0 break_le c0.x, c0.y "
	"					loop aL, i0 break_le c0.x, c0.y "
	"						loop aL, i0 break_le c0.x, c0.y "
	"							loop aL, i0 break_le c0.x, c0.y "
	"								loop aL, i0 break_le c0.x, c0.y "
	"									loop aL, i0 break_le c0.x, c0.y "
	"										loop aL, i0 break_le c0.x, c0.y "
	"											loop aL, i0 break_le c0.x, c0.y "
	"												loop aL, i0 break_le c0.x, c0.y "
	"													loop aL, i0 break_le c0.x, c0.y "
	"														loop aL, i0 break_le c0.x, c0.y "
	"															loop aL, i0 break_le c0.x, c0.y "
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_468_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_468_Desc = "ps_2_sw : mad is NOT allowed in a 24 level loop aL, i0 break_le c0.x, c0.y block";
string PS_468 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		loop aL, i0 break_le c0.x, c0.y "
	"			loop aL, i0 break_le c0.x, c0.y "
	"				loop aL, i0 break_le c0.x, c0.y "
	"					loop aL, i0 break_le c0.x, c0.y "
	"						loop aL, i0 break_le c0.x, c0.y "
	"							loop aL, i0 break_le c0.x, c0.y "
	"								loop aL, i0 break_le c0.x, c0.y "
	"									loop aL, i0 break_le c0.x, c0.y "
	"										loop aL, i0 break_le c0.x, c0.y "
	"											loop aL, i0 break_le c0.x, c0.y "
	"												loop aL, i0 break_le c0.x, c0.y "
	"													loop aL, i0 break_le c0.x, c0.y "
	"														loop aL, i0 break_le c0.x, c0.y "
	"															loop aL, i0 break_le c0.x, c0.y "
	"																loop aL, i0 break_le c0.x, c0.y "
	"																	loop aL, i0 break_le c0.x, c0.y "
	"																		loop aL, i0 break_le c0.x, c0.y "
	"																			loop aL, i0 break_le c0.x, c0.y "
	"																				loop aL, i0 break_le c0.x, c0.y "
	"																					loop aL, i0 break_le c0.x, c0.y "
	"																						loop aL, i0 break_le c0.x, c0.y "
	"																							loop aL, i0 break_le c0.x, c0.y "
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_469_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_469_Desc = "ps_2_sw : mad is NOT allowed in a 25 level loop aL, i0 break_le c0.x, c0.y block";
string PS_469 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		loop aL, i0 break_le c0.x, c0.y "
	"			loop aL, i0 break_le c0.x, c0.y "
	"				loop aL, i0 break_le c0.x, c0.y "
	"					loop aL, i0 break_le c0.x, c0.y "
	"						loop aL, i0 break_le c0.x, c0.y "
	"							loop aL, i0 break_le c0.x, c0.y "
	"								loop aL, i0 break_le c0.x, c0.y "
	"									loop aL, i0 break_le c0.x, c0.y "
	"										loop aL, i0 break_le c0.x, c0.y "
	"											loop aL, i0 break_le c0.x, c0.y "
	"												loop aL, i0 break_le c0.x, c0.y "
	"													loop aL, i0 break_le c0.x, c0.y "
	"														loop aL, i0 break_le c0.x, c0.y "
	"															loop aL, i0 break_le c0.x, c0.y "
	"																loop aL, i0 break_le c0.x, c0.y "
	"																	loop aL, i0 break_le c0.x, c0.y "
	"																		loop aL, i0 break_le c0.x, c0.y "
	"																			loop aL, i0 break_le c0.x, c0.y "
	"																				loop aL, i0 break_le c0.x, c0.y "
	"																					loop aL, i0 break_le c0.x, c0.y "
	"																						loop aL, i0 break_le c0.x, c0.y "
	"																							loop aL, i0 break_le c0.x, c0.y "
	"																								loop aL, i0 break_le c0.x, c0.y "
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_470_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_470_Desc = "ps_2_sw : mad is allowed in a 1 level rep i0 break_le c0.x, c0.y block";
string PS_470 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	mad r0, v0, v1, r0 "
	"endrep ";

PS_CRITERIA PS_471_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_471_Desc = "ps_2_sw : mad is NOT allowed in a 8 level rep i0 break_le c0.x, c0.y block";
string PS_471 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		rep i0 break_le c0.x, c0.y "
	"			rep i0 break_le c0.x, c0.y "
	"				rep i0 break_le c0.x, c0.y "
	"					rep i0 break_le c0.x, c0.y "
	"						rep i0 break_le c0.x, c0.y "
	"							rep i0 break_le c0.x, c0.y "
	"								mad r0, v0, v1, r0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_472_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_472_Desc = "ps_2_sw : mad is NOT allowed in a 16 level rep i0 break_le c0.x, c0.y block";
string PS_472 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		rep i0 break_le c0.x, c0.y "
	"			rep i0 break_le c0.x, c0.y "
	"				rep i0 break_le c0.x, c0.y "
	"					rep i0 break_le c0.x, c0.y "
	"						rep i0 break_le c0.x, c0.y "
	"							rep i0 break_le c0.x, c0.y "
	"								rep i0 break_le c0.x, c0.y "
	"									rep i0 break_le c0.x, c0.y "
	"										rep i0 break_le c0.x, c0.y "
	"											rep i0 break_le c0.x, c0.y "
	"												rep i0 break_le c0.x, c0.y "
	"													rep i0 break_le c0.x, c0.y "
	"														rep i0 break_le c0.x, c0.y "
	"															rep i0 break_le c0.x, c0.y "
	"																mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_473_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_473_Desc = "ps_2_sw : mad is NOT allowed in a 24 level rep i0 break_le c0.x, c0.y block";
string PS_473 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		rep i0 break_le c0.x, c0.y "
	"			rep i0 break_le c0.x, c0.y "
	"				rep i0 break_le c0.x, c0.y "
	"					rep i0 break_le c0.x, c0.y "
	"						rep i0 break_le c0.x, c0.y "
	"							rep i0 break_le c0.x, c0.y "
	"								rep i0 break_le c0.x, c0.y "
	"									rep i0 break_le c0.x, c0.y "
	"										rep i0 break_le c0.x, c0.y "
	"											rep i0 break_le c0.x, c0.y "
	"												rep i0 break_le c0.x, c0.y "
	"													rep i0 break_le c0.x, c0.y "
	"														rep i0 break_le c0.x, c0.y "
	"															rep i0 break_le c0.x, c0.y "
	"																rep i0 break_le c0.x, c0.y "
	"																	rep i0 break_le c0.x, c0.y "
	"																		rep i0 break_le c0.x, c0.y "
	"																			rep i0 break_le c0.x, c0.y "
	"																				rep i0 break_le c0.x, c0.y "
	"																					rep i0 break_le c0.x, c0.y "
	"																						rep i0 break_le c0.x, c0.y "
	"																							rep i0 break_le c0.x, c0.y "
	"																								mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_474_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_474_Desc = "ps_2_sw : mad is NOT allowed in a 25 level rep i0 break_le c0.x, c0.y block";
string PS_474 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		rep i0 break_le c0.x, c0.y "
	"			rep i0 break_le c0.x, c0.y "
	"				rep i0 break_le c0.x, c0.y "
	"					rep i0 break_le c0.x, c0.y "
	"						rep i0 break_le c0.x, c0.y "
	"							rep i0 break_le c0.x, c0.y "
	"								rep i0 break_le c0.x, c0.y "
	"									rep i0 break_le c0.x, c0.y "
	"										rep i0 break_le c0.x, c0.y "
	"											rep i0 break_le c0.x, c0.y "
	"												rep i0 break_le c0.x, c0.y "
	"													rep i0 break_le c0.x, c0.y "
	"														rep i0 break_le c0.x, c0.y "
	"															rep i0 break_le c0.x, c0.y "
	"																rep i0 break_le c0.x, c0.y "
	"																	rep i0 break_le c0.x, c0.y "
	"																		rep i0 break_le c0.x, c0.y "
	"																			rep i0 break_le c0.x, c0.y "
	"																				rep i0 break_le c0.x, c0.y "
	"																					rep i0 break_le c0.x, c0.y "
	"																						rep i0 break_le c0.x, c0.y "
	"																							rep i0 break_le c0.x, c0.y "
	"																								rep i0 break_le c0.x, c0.y "
	"																									mad r0, v0, v1, r0 "
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

PS_CRITERIA PS_475_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_475_Desc = "ps_2_sw : mad is allowed in a if b0 and if b0 block";
string PS_475 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_476_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_476_Desc = "ps_2_sw : mad is allowed in a if b0 and if b0 nop else block";
string PS_476 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_477_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_477_Desc = "ps_2_sw : mad is NOT allowed in a if b0 and loop aL, i0 block";
string PS_477 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_478_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_478_Desc = "ps_2_sw : mad is allowed in a if b0 and rep i0 block";
string PS_478 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_479_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_479_Desc = "ps_2_sw : mad is allowed in a if b0 and if_lt c0.x, c0.y block";
string PS_479 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_480_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_480_Desc = "ps_2_sw : mad is allowed in a if b0 and if_lt c0.x, c0.y nop else block";
string PS_480 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_481_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_481_Desc = "ps_2_sw : mad is allowed in a if b0 and if p0.x block";
string PS_481 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_482_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_482_Desc = "ps_2_sw : mad is allowed in a if b0 and if p0.x nop else block";
string PS_482 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_483_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_483_Desc = "ps_2_sw : mad is NOT allowed in a if b0 and loop aL, i0 break block";
string PS_483 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_484_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_484_Desc = "ps_2_sw : mad is allowed in a if b0 and rep i0 break block";
string PS_484 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_485_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_485_Desc = "ps_2_sw : mad is NOT allowed in a if b0 and loop aL, i0 breakp p0.x block";
string PS_485 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_486_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_486_Desc = "ps_2_sw : mad is allowed in a if b0 and rep i0 breakp p0.x block";
string PS_486 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_487_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_487_Desc = "ps_2_sw : mad is NOT allowed in a if b0 and loop aL, i0 break_le c0.x, c0.y block";
string PS_487 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_488_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_488_Desc = "ps_2_sw : mad is allowed in a if b0 and rep i0 break_le c0.x, c0.y block";
string PS_488 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_489_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_489_Desc = "ps_2_sw : mad is allowed in a if b0 nop else and if b0 block";
string PS_489 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_490_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_490_Desc = "ps_2_sw : mad is allowed in a if b0 nop else and if b0 nop else block";
string PS_490 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_491_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_491_Desc = "ps_2_sw : mad is NOT allowed in a if b0 nop else and loop aL, i0 block";
string PS_491 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_492_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_492_Desc = "ps_2_sw : mad is allowed in a if b0 nop else and rep i0 block";
string PS_492 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_493_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_493_Desc = "ps_2_sw : mad is allowed in a if b0 nop else and if_lt c0.x, c0.y block";
string PS_493 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_494_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_494_Desc = "ps_2_sw : mad is allowed in a if b0 nop else and if_lt c0.x, c0.y nop else block";
string PS_494 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_495_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_495_Desc = "ps_2_sw : mad is allowed in a if b0 nop else and if p0.x block";
string PS_495 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_496_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_496_Desc = "ps_2_sw : mad is allowed in a if b0 nop else and if p0.x nop else block";
string PS_496 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_497_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_497_Desc = "ps_2_sw : mad is NOT allowed in a if b0 nop else and loop aL, i0 break block";
string PS_497 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_498_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_498_Desc = "ps_2_sw : mad is allowed in a if b0 nop else and rep i0 break block";
string PS_498 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_499_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_499_Desc = "ps_2_sw : mad is NOT allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string PS_499 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_500_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_500_Desc = "ps_2_sw : mad is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string PS_500 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_501_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_501_Desc = "ps_2_sw : mad is NOT allowed in a if b0 nop else and loop aL, i0 break_le c0.x, c0.y block";
string PS_501 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_502_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_502_Desc = "ps_2_sw : mad is allowed in a if b0 nop else and rep i0 break_le c0.x, c0.y block";
string PS_502 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_503_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_503_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and if b0 block";
string PS_503 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_504_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_504_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and if b0 nop else block";
string PS_504 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_505_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_505_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and loop aL, i0 block";
string PS_505 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_506_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_506_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and rep i0 block";
string PS_506 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_507_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_507_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and if_lt c0.x, c0.y block";
string PS_507 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_508_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_508_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and if_lt c0.x, c0.y nop else block";
string PS_508 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_509_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_509_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and if p0.x block";
string PS_509 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_510_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_510_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and if p0.x nop else block";
string PS_510 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_511_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_511_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and loop aL, i0 break block";
string PS_511 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_512_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_512_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and rep i0 break block";
string PS_512 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_513_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_513_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string PS_513 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_514_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_514_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string PS_514 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_515_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_515_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and loop aL, i0 break_le c0.x, c0.y block";
string PS_515 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_516_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_516_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 and rep i0 break_le c0.x, c0.y block";
string PS_516 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_517_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_517_Desc = "ps_2_sw : mad is allowed in a rep i0 and if b0 block";
string PS_517 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_518_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_518_Desc = "ps_2_sw : mad is allowed in a rep i0 and if b0 nop else block";
string PS_518 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_519_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_519_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 and loop aL, i0 block";
string PS_519 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_520_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_520_Desc = "ps_2_sw : mad is allowed in a rep i0 and rep i0 block";
string PS_520 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_521_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_521_Desc = "ps_2_sw : mad is allowed in a rep i0 and if_lt c0.x, c0.y block";
string PS_521 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_522_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_522_Desc = "ps_2_sw : mad is allowed in a rep i0 and if_lt c0.x, c0.y nop else block";
string PS_522 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_523_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_523_Desc = "ps_2_sw : mad is allowed in a rep i0 and if p0.x block";
string PS_523 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_524_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_524_Desc = "ps_2_sw : mad is allowed in a rep i0 and if p0.x nop else block";
string PS_524 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_525_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_525_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 and loop aL, i0 break block";
string PS_525 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_526_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_526_Desc = "ps_2_sw : mad is allowed in a rep i0 and rep i0 break block";
string PS_526 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_527_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_527_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string PS_527 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_528_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_528_Desc = "ps_2_sw : mad is allowed in a rep i0 and rep i0 breakp p0.x block";
string PS_528 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_529_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_529_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 and loop aL, i0 break_le c0.x, c0.y block";
string PS_529 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_530_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_530_Desc = "ps_2_sw : mad is allowed in a rep i0 and rep i0 break_le c0.x, c0.y block";
string PS_530 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_531_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_531_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y and if b0 block";
string PS_531 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_532_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_532_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y and if b0 nop else block";
string PS_532 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_533_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_533_Desc = "ps_2_sw : mad is NOT allowed in a if_lt c0.x, c0.y and loop aL, i0 block";
string PS_533 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_534_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_534_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y and rep i0 block";
string PS_534 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_535_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_535_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y and if_lt c0.x, c0.y block";
string PS_535 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_536_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_536_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string PS_536 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_537_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_537_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y and if p0.x block";
string PS_537 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_538_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_538_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y and if p0.x nop else block";
string PS_538 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_539_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_539_Desc = "ps_2_sw : mad is NOT allowed in a if_lt c0.x, c0.y and loop aL, i0 break block";
string PS_539 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_540_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_540_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y and rep i0 break block";
string PS_540 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_541_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_541_Desc = "ps_2_sw : mad is NOT allowed in a if_lt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_541 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_542_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_542_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y and rep i0 breakp p0.x block";
string PS_542 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_543_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_543_Desc = "ps_2_sw : mad is NOT allowed in a if_lt c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string PS_543 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_544_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_544_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string PS_544 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_545_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_545_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if b0 block";
string PS_545 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_546_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_546_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if b0 nop else block";
string PS_546 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_547_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_547_Desc = "ps_2_sw : mad is NOT allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 block";
string PS_547 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_548_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_548_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 block";
string PS_548 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_549_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_549_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if_lt c0.x, c0.y block";
string PS_549 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_550_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_550_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if_lt c0.x, c0.y nop else block";
string PS_550 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_551_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_551_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if p0.x block";
string PS_551 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_552_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_552_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if p0.x nop else block";
string PS_552 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_553_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_553_Desc = "ps_2_sw : mad is NOT allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 break block";
string PS_553 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_554_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_554_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 break block";
string PS_554 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_555_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_555_Desc = "ps_2_sw : mad is NOT allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string PS_555 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_556_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_556_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 breakp p0.x block";
string PS_556 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_557_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_557_Desc = "ps_2_sw : mad is NOT allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 break_le c0.x, c0.y block";
string PS_557 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_558_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_558_Desc = "ps_2_sw : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 break_le c0.x, c0.y block";
string PS_558 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_559_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_559_Desc = "ps_2_sw : mad is allowed in a if p0.x and if b0 block";
string PS_559 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_560_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_560_Desc = "ps_2_sw : mad is allowed in a if p0.x and if b0 nop else block";
string PS_560 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_561_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_561_Desc = "ps_2_sw : mad is NOT allowed in a if p0.x and loop aL, i0 block";
string PS_561 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_562_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_562_Desc = "ps_2_sw : mad is allowed in a if p0.x and rep i0 block";
string PS_562 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_563_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_563_Desc = "ps_2_sw : mad is allowed in a if p0.x and if_lt c0.x, c0.y block";
string PS_563 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_564_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_564_Desc = "ps_2_sw : mad is allowed in a if p0.x and if_lt c0.x, c0.y nop else block";
string PS_564 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_565_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_565_Desc = "ps_2_sw : mad is allowed in a if p0.x and if p0.x block";
string PS_565 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_566_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_566_Desc = "ps_2_sw : mad is allowed in a if p0.x and if p0.x nop else block";
string PS_566 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_567_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_567_Desc = "ps_2_sw : mad is NOT allowed in a if p0.x and loop aL, i0 break block";
string PS_567 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_568_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_568_Desc = "ps_2_sw : mad is allowed in a if p0.x and rep i0 break block";
string PS_568 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_569_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_569_Desc = "ps_2_sw : mad is NOT allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string PS_569 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_570_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_570_Desc = "ps_2_sw : mad is allowed in a if p0.x and rep i0 breakp p0.x block";
string PS_570 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_571_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_571_Desc = "ps_2_sw : mad is NOT allowed in a if p0.x and loop aL, i0 break_le c0.x, c0.y block";
string PS_571 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_572_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_572_Desc = "ps_2_sw : mad is allowed in a if p0.x and rep i0 break_le c0.x, c0.y block";
string PS_572 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_573_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_573_Desc = "ps_2_sw : mad is allowed in a if p0.x nop else and if b0 block";
string PS_573 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_574_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_574_Desc = "ps_2_sw : mad is allowed in a if p0.x nop else and if b0 nop else block";
string PS_574 = 
	"ps_2_sw "
	"defb b0, true "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_575_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_575_Desc = "ps_2_sw : mad is NOT allowed in a if p0.x nop else and loop aL, i0 block";
string PS_575 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_576_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_576_Desc = "ps_2_sw : mad is allowed in a if p0.x nop else and rep i0 block";
string PS_576 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_577_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_577_Desc = "ps_2_sw : mad is allowed in a if p0.x nop else and if_lt c0.x, c0.y block";
string PS_577 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_578_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_578_Desc = "ps_2_sw : mad is allowed in a if p0.x nop else and if_lt c0.x, c0.y nop else block";
string PS_578 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_579_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_579_Desc = "ps_2_sw : mad is allowed in a if p0.x nop else and if p0.x block";
string PS_579 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_580_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_580_Desc = "ps_2_sw : mad is allowed in a if p0.x nop else and if p0.x nop else block";
string PS_580 = 
	"ps_2_sw "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_581_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_581_Desc = "ps_2_sw : mad is NOT allowed in a if p0.x nop else and loop aL, i0 break block";
string PS_581 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_582_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_582_Desc = "ps_2_sw : mad is allowed in a if p0.x nop else and rep i0 break block";
string PS_582 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_583_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_583_Desc = "ps_2_sw : mad is NOT allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string PS_583 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_584_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_584_Desc = "ps_2_sw : mad is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string PS_584 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_585_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_585_Desc = "ps_2_sw : mad is NOT allowed in a if p0.x nop else and loop aL, i0 break_le c0.x, c0.y block";
string PS_585 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_586_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_586_Desc = "ps_2_sw : mad is allowed in a if p0.x nop else and rep i0 break_le c0.x, c0.y block";
string PS_586 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_587_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_587_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and if b0 block";
string PS_587 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_588_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_588_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and if b0 nop else block";
string PS_588 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_589_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_589_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_589 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_590_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_590_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and rep i0 block";
string PS_590 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_591_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_591_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and if_lt c0.x, c0.y block";
string PS_591 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_592_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_592_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and if_lt c0.x, c0.y nop else block";
string PS_592 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_593_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_593_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and if p0.x block";
string PS_593 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_594_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_594_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_594 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_595_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_595_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_595 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_596_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_596_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and rep i0 break block";
string PS_596 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_597_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_597_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_597 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_598_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_598_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_598 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_599_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_599_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and loop aL, i0 break_le c0.x, c0.y block";
string PS_599 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_600_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_600_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break and rep i0 break_le c0.x, c0.y block";
string PS_600 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_601_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_601_Desc = "ps_2_sw : mad is allowed in a rep i0 break and if b0 block";
string PS_601 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_602_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_602_Desc = "ps_2_sw : mad is allowed in a rep i0 break and if b0 nop else block";
string PS_602 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_603_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_603_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 break and loop aL, i0 block";
string PS_603 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_604_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_604_Desc = "ps_2_sw : mad is allowed in a rep i0 break and rep i0 block";
string PS_604 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_605_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_605_Desc = "ps_2_sw : mad is allowed in a rep i0 break and if_lt c0.x, c0.y block";
string PS_605 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_606_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_606_Desc = "ps_2_sw : mad is allowed in a rep i0 break and if_lt c0.x, c0.y nop else block";
string PS_606 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_607_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_607_Desc = "ps_2_sw : mad is allowed in a rep i0 break and if p0.x block";
string PS_607 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_608_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_608_Desc = "ps_2_sw : mad is allowed in a rep i0 break and if p0.x nop else block";
string PS_608 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_609_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_609_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 break and loop aL, i0 break block";
string PS_609 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_610_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_610_Desc = "ps_2_sw : mad is allowed in a rep i0 break and rep i0 break block";
string PS_610 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_611_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_611_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_611 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_612_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_612_Desc = "ps_2_sw : mad is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_612 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_613_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_613_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 break and loop aL, i0 break_le c0.x, c0.y block";
string PS_613 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_614_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_614_Desc = "ps_2_sw : mad is allowed in a rep i0 break and rep i0 break_le c0.x, c0.y block";
string PS_614 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_615_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_615_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_615 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_616_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_616_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_616 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_617_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_617_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_617 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_618_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_618_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_618 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_619_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_619_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and if_lt c0.x, c0.y block";
string PS_619 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_620_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_620_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and if_lt c0.x, c0.y nop else block";
string PS_620 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_621_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_621_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_621 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_622_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_622_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_622 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_623_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_623_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_623 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_624_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_624_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_624 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_625_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_625_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_625 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_626_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_626_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_626 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_627_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_627_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_le c0.x, c0.y block";
string PS_627 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_628_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_628_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break_le c0.x, c0.y block";
string PS_628 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_629_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_629_Desc = "ps_2_sw : mad is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_629 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_630_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_630_Desc = "ps_2_sw : mad is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_630 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_631_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_631_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_631 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_632_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_632_Desc = "ps_2_sw : mad is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_632 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_633_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_633_Desc = "ps_2_sw : mad is allowed in a rep i0 breakp p0.x and if_lt c0.x, c0.y block";
string PS_633 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_634_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_634_Desc = "ps_2_sw : mad is allowed in a rep i0 breakp p0.x and if_lt c0.x, c0.y nop else block";
string PS_634 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_635_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_635_Desc = "ps_2_sw : mad is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_635 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_636_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_636_Desc = "ps_2_sw : mad is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_636 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_637_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_637_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_637 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_638_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_638_Desc = "ps_2_sw : mad is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_638 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_639_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_639_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_639 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_640_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_640_Desc = "ps_2_sw : mad is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_640 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_641_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_641_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break_le c0.x, c0.y block";
string PS_641 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_642_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_642_Desc = "ps_2_sw : mad is allowed in a rep i0 breakp p0.x and rep i0 break_le c0.x, c0.y block";
string PS_642 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_643_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_643_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if b0 block";
string PS_643 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_644_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_644_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if b0 nop else block";
string PS_644 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_645_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_645_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 block";
string PS_645 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_646_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_646_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 block";
string PS_646 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_647_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_647_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if_lt c0.x, c0.y block";
string PS_647 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_648_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_648_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string PS_648 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_649_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_649_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if p0.x block";
string PS_649 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_650_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_650_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and if p0.x nop else block";
string PS_650 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_651_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_651_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 break block";
string PS_651 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_652_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_652_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 break block";
string PS_652 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_653_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_653_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_653 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_654_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_654_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 breakp p0.x block";
string PS_654 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_655_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_655_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string PS_655 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_656_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_656_Desc = "ps_2_sw : mad is NOT allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string PS_656 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_657_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_657_Desc = "ps_2_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if b0 block";
string PS_657 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	if b0 "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_658_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_658_Desc = "ps_2_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if b0 nop else block";
string PS_658 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	if b0 nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_659_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_659_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 block";
string PS_659 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_660_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_660_Desc = "ps_2_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and rep i0 block";
string PS_660 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_661_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_661_Desc = "ps_2_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if_lt c0.x, c0.y block";
string PS_661 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_662_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_662_Desc = "ps_2_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string PS_662 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_663_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_663_Desc = "ps_2_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if p0.x block";
string PS_663 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	if p0.x "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_664_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_664_Desc = "ps_2_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if p0.x nop else block";
string PS_664 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	if p0.x nop else "
	"		mad r0, v0, v1, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_665_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_665_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 break block";
string PS_665 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_666_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_666_Desc = "ps_2_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and rep i0 break block";
string PS_666 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_667_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_667_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_667 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_668_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_668_Desc = "ps_2_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and rep i0 breakp p0.x block";
string PS_668 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_669_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_669_Desc = "ps_2_sw : mad is NOT allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string PS_669 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_670_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_670_Desc = "ps_2_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string PS_670 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"mov r0, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, r0 "
	"	endrep "
	"endrep ";


