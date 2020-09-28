#include "Test_Include.fx"

int Test_Count = 467;

string TestInfo
<
	string TestType = "PS";
>
= "ps_3_0_inst_dp3";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_3_0 : dp3 source reg combination v0, r0 is allowed";
string PS_001 = 
	"ps_3_0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"dp3 r0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_3_0 : dp3 source reg combination v0, c0 is allowed";
string PS_002 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"dp3 r0, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_003_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_3_0 : dp3 source reg combination v0, vPos.xy is allowed";
string PS_003 = 
	"ps_3_0 "
	"dcl_tangent v0 "
	"dcl vPos.xy "
	"dp3 r0, v0, vPos.xy "
	"mov oC0, c0 ";

PS_CRITERIA PS_004_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_3_0 : dp3 source reg combination r0, v0 is allowed";
string PS_004 = 
	"ps_3_0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"dp3 r0, r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_005_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_3_0 : dp3 source reg combination r0, r1 is allowed";
string PS_005 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"dp3 r0, r0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_006_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_3_0 : dp3 source reg combination r0, c0 is allowed";
string PS_006 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, r0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_007_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_3_0 : dp3 source reg combination r0, vPos.xy is allowed";
string PS_007 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"dp3 r0, r0, vPos.xy "
	"mov oC0, c0 ";

PS_CRITERIA PS_008_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_3_0 : dp3 source reg combination c0, v0 is allowed";
string PS_008 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"dp3 r0, c0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_009_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_3_0 : dp3 source reg combination c0, r0 is allowed";
string PS_009 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_010_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_3_0 : dp3 source reg combination c0, vPos.xy is allowed";
string PS_010 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"dp3 r0, c0, vPos.xy "
	"mov oC0, c0 ";

PS_CRITERIA PS_011_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_3_0 : dp3 source reg combination vPos.xy, v0 is allowed";
string PS_011 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"dcl_tangent v0 "
	"dp3 r0, vPos.xy, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_012_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_3_0 : dp3 source reg combination vPos.xy, r0 is allowed";
string PS_012 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"dp3 r0, vPos.xy, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_013_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_3_0 : dp3 source reg combination vPos.xy, c0 is allowed";
string PS_013 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"dp3 r0, vPos.xy, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_014_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_3_0 : dp3 source reg combination -c0, -r0 is allowed";
string PS_014 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, -c0, -r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_015_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_3_0 : dp3 source reg combination -c0, r0_abs is allowed";
string PS_015 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, -c0, r0_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_016_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_3_0 : dp3 source reg combination -c0, -r0_abs is allowed";
string PS_016 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, -c0, -r0_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_017_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_3_0 : dp3 source reg combination -c0, r0 is allowed";
string PS_017 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, -c0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_018_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_3_0 : dp3 source reg combination c0_abs, -r0 is allowed";
string PS_018 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0_abs, -r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_019_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_3_0 : dp3 source reg combination c0_abs, r0_abs is allowed";
string PS_019 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0_abs, r0_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_020_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_3_0 : dp3 source reg combination c0_abs, -r0_abs is allowed";
string PS_020 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0_abs, -r0_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_3_0 : dp3 source reg combination c0_abs, r0 is allowed";
string PS_021 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0_abs, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_3_0 : dp3 source reg combination -c0_abs, -r0 is allowed";
string PS_022 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, -c0_abs, -r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_3_0 : dp3 source reg combination -c0_abs, r0_abs is allowed";
string PS_023 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, -c0_abs, r0_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_3_0 : dp3 source reg combination -c0_abs, -r0_abs is allowed";
string PS_024 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, -c0_abs, -r0_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_3_0 : dp3 source reg combination -c0_abs, r0 is allowed";
string PS_025 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, -c0_abs, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_3_0 : dp3 source reg combination c0, -r0 is allowed";
string PS_026 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, -r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_3_0 : dp3 source reg combination c0, r0_abs is allowed";
string PS_027 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_3_0 : dp3 source reg combination c0, -r0_abs is allowed";
string PS_028 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, -r0_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_3_0 : dp3 source reg combination c0, r0 is allowed";
string PS_029 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_3_0 : dp3 source reg combination c0.r, r0 is allowed";
string PS_030 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.r, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_3_0 : dp3 source reg combination c0.g, r0 is allowed";
string PS_031 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.g, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_3_0 : dp3 source reg combination c0.b, r0 is allowed";
string PS_032 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.b, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_3_0 : dp3 source reg combination c0.a, r0 is allowed";
string PS_033 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.a, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_3_0 : dp3 source reg combination c0.gbra, r0 is allowed";
string PS_034 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.gbra, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_3_0 : dp3 source reg combination c0.brga, r0 is allowed";
string PS_035 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.brga, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_3_0 : dp3 source reg combination c0.abgr, r0 is allowed";
string PS_036 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.abgr, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_3_0 : dp3 source reg combination c0.agrb, r0 is allowed";
string PS_037 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.agrb, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_3_0 : dp3 source reg combination c0.rbga, r0 is allowed";
string PS_038 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.rbga, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_3_0 : dp3 source reg combination c0.rgab, r0 is allowed";
string PS_039 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.rgab, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_3_0 : dp3 source reg combination c0.bgr, r0 is allowed";
string PS_040 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.bgr, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_3_0 : dp3 source reg combination c0.rbg, r0 is allowed";
string PS_041 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.rbg, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_3_0 : dp3 source reg combination c0.gar, r0 is allowed";
string PS_042 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.gar, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_3_0 : dp3 source reg combination c0.gr, r0 is allowed";
string PS_043 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.gr, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_3_0 : dp3 source reg combination c0.ab, r0 is allowed";
string PS_044 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.ab, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_3_0 : dp3 source reg combination c0.bg, r0 is allowed";
string PS_045 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0.bg, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_3_0 : dp3 source reg combination c0, r0.r is allowed";
string PS_046 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.r "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_3_0 : dp3 source reg combination c0, r0.g is allowed";
string PS_047 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_3_0 : dp3 source reg combination c0, r0.b is allowed";
string PS_048 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_049_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_3_0 : dp3 source reg combination c0, r0.a is allowed";
string PS_049 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_050_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_3_0 : dp3 source reg combination c0, r0.gbra is allowed";
string PS_050 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.gbra "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_3_0 : dp3 source reg combination c0, r0.brga is allowed";
string PS_051 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.brga "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_3_0 : dp3 source reg combination c0, r0.abgr is allowed";
string PS_052 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.abgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_3_0 : dp3 source reg combination c0, r0.agrb is allowed";
string PS_053 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.agrb "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_3_0 : dp3 source reg combination c0, r0.rbga is allowed";
string PS_054 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.rbga "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_3_0 : dp3 source reg combination c0, r0.rgab is allowed";
string PS_055 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.rgab "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_3_0 : dp3 source reg combination c0, r0.bgr is allowed";
string PS_056 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.bgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_3_0 : dp3 source reg combination c0, r0.rbg is allowed";
string PS_057 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.rbg "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_3_0 : dp3 source reg combination c0, r0.gar is allowed";
string PS_058 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.gar "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_3_0 : dp3 source reg combination c0, r0.gr is allowed";
string PS_059 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.gr "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_3_0 : dp3 source reg combination c0, r0.ab is allowed";
string PS_060 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.ab "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_3_0 : dp3 source reg combination c0, r0.bg is allowed";
string PS_061 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"dp3 r0, c0, r0.bg "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_3_0 : dp3 source reg combination v0, v1 is NOT allowed";
string PS_062 = 
	"ps_3_0 "
	"dcl_tangent v0 "
	"dcl_texcoord v1 "
	"dp3 r0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_3_0 : dp3 source reg combination c0, c1 is NOT allowed";
string PS_063 = 
	"ps_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dp3 r0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_3_0 : dp3 source reg combination vPos, vPos is NOT allowed";
string PS_064 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"dp3 r0, vPos, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_3_0 : dp3 source reg combination b0, b0 is NOT allowed";
string PS_065 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_3_0 : dp3 source reg combination b0, i0 is NOT allowed";
string PS_066 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dp3 r0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_3_0 : dp3 source reg combination b0, aL is NOT allowed";
string PS_067 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, b0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_3_0 : dp3 source reg combination b0, p0 is NOT allowed";
string PS_068 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_3_0 : dp3 source reg combination b0, s0 is NOT allowed";
string PS_069 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_cube s0 "
	"dp3 r0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_3_0 : dp3 source reg combination b0, vFace is NOT allowed";
string PS_070 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, b0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_3_0 : dp3 source reg combination b0, oC0 is NOT allowed";
string PS_071 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_3_0 : dp3 source reg combination b0, oDepth is NOT allowed";
string PS_072 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_3_0 : dp3 source reg combination i0, b0 is NOT allowed";
string PS_073 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_3_0 : dp3 source reg combination i0, i0 is NOT allowed";
string PS_074 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_3_0 : dp3 source reg combination i0, aL is NOT allowed";
string PS_075 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, i0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_3_0 : dp3 source reg combination i0, p0 is NOT allowed";
string PS_076 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_3_0 : dp3 source reg combination i0, s0 is NOT allowed";
string PS_077 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dp3 r0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_3_0 : dp3 source reg combination i0, vFace is NOT allowed";
string PS_078 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, i0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_3_0 : dp3 source reg combination i0, oC0 is NOT allowed";
string PS_079 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_3_0 : dp3 source reg combination i0, oDepth is NOT allowed";
string PS_080 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_081_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_3_0 : dp3 source reg combination aL, b0 is NOT allowed";
string PS_081 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, aL, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_082_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_3_0 : dp3 source reg combination aL, i0 is NOT allowed";
string PS_082 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, aL, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_083_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_3_0 : dp3 source reg combination aL, aL is NOT allowed";
string PS_083 = 
	"ps_3_0 "
	"dp3 r0, aL, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_084_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_3_0 : dp3 source reg combination aL, p0 is NOT allowed";
string PS_084 = 
	"ps_3_0 "
	"dp3 r0, aL, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_085_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_3_0 : dp3 source reg combination aL, s0 is NOT allowed";
string PS_085 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, aL, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_086_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_3_0 : dp3 source reg combination aL, vFace is NOT allowed";
string PS_086 = 
	"ps_3_0 "
	"dp3 r0, aL, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_087_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_3_0 : dp3 source reg combination aL, oC0 is NOT allowed";
string PS_087 = 
	"ps_3_0 "
	"dp3 r0, aL, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_088_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_3_0 : dp3 source reg combination aL, oDepth is NOT allowed";
string PS_088 = 
	"ps_3_0 "
	"dp3 r0, aL, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_089_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_3_0 : dp3 source reg combination p0, b0 is NOT allowed";
string PS_089 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_090_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_3_0 : dp3 source reg combination p0, i0 is NOT allowed";
string PS_090 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_091_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_3_0 : dp3 source reg combination p0, aL is NOT allowed";
string PS_091 = 
	"ps_3_0 "
	"dp3 r0, p0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_092_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_3_0 : dp3 source reg combination p0, p0 is NOT allowed";
string PS_092 = 
	"ps_3_0 "
	"dp3 r0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_093_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_3_0 : dp3 source reg combination p0, s0 is NOT allowed";
string PS_093 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_094_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_3_0 : dp3 source reg combination p0, vFace is NOT allowed";
string PS_094 = 
	"ps_3_0 "
	"dp3 r0, p0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_095_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_3_0 : dp3 source reg combination p0, oC0 is NOT allowed";
string PS_095 = 
	"ps_3_0 "
	"dp3 r0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_096_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_3_0 : dp3 source reg combination p0, oDepth is NOT allowed";
string PS_096 = 
	"ps_3_0 "
	"dp3 r0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_097_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_3_0 : dp3 source reg combination s0, b0 is NOT allowed";
string PS_097 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"dp3 r0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_098_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_3_0 : dp3 source reg combination s0, i0 is NOT allowed";
string PS_098 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"dp3 r0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_099_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_3_0 : dp3 source reg combination s0, aL is NOT allowed";
string PS_099 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, s0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_100_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_3_0 : dp3 source reg combination s0, p0 is NOT allowed";
string PS_100 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_101_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_3_0 : dp3 source reg combination s0, s0 is NOT allowed";
string PS_101 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_102_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_3_0 : dp3 source reg combination s0, vFace is NOT allowed";
string PS_102 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, s0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_103_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_3_0 : dp3 source reg combination s0, oC0 is NOT allowed";
string PS_103 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_104_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_3_0 : dp3 source reg combination s0, oDepth is NOT allowed";
string PS_104 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_105_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_3_0 : dp3 source reg combination vFace, b0 is NOT allowed";
string PS_105 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, vFace, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_106_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_3_0 : dp3 source reg combination vFace, i0 is NOT allowed";
string PS_106 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, vFace, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_107_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_3_0 : dp3 source reg combination vFace, aL is NOT allowed";
string PS_107 = 
	"ps_3_0 "
	"dp3 r0, vFace, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_108_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_3_0 : dp3 source reg combination vFace, p0 is NOT allowed";
string PS_108 = 
	"ps_3_0 "
	"dp3 r0, vFace, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_109_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_3_0 : dp3 source reg combination vFace, s0 is NOT allowed";
string PS_109 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, vFace, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_110_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_3_0 : dp3 source reg combination vFace, vFace is NOT allowed";
string PS_110 = 
	"ps_3_0 "
	"dp3 r0, vFace, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_111_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_3_0 : dp3 source reg combination vFace, oC0 is NOT allowed";
string PS_111 = 
	"ps_3_0 "
	"dp3 r0, vFace, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_112_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_3_0 : dp3 source reg combination vFace, oDepth is NOT allowed";
string PS_112 = 
	"ps_3_0 "
	"dp3 r0, vFace, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_113_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_3_0 : dp3 source reg combination oC0, b0 is NOT allowed";
string PS_113 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_114_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_3_0 : dp3 source reg combination oC0, i0 is NOT allowed";
string PS_114 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_115_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_3_0 : dp3 source reg combination oC0, aL is NOT allowed";
string PS_115 = 
	"ps_3_0 "
	"dp3 r0, oC0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_116_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_3_0 : dp3 source reg combination oC0, p0 is NOT allowed";
string PS_116 = 
	"ps_3_0 "
	"dp3 r0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_117_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_3_0 : dp3 source reg combination oC0, s0 is NOT allowed";
string PS_117 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_118_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_3_0 : dp3 source reg combination oC0, vFace is NOT allowed";
string PS_118 = 
	"ps_3_0 "
	"dp3 r0, oC0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_119_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_3_0 : dp3 source reg combination oC0, oC0 is NOT allowed";
string PS_119 = 
	"ps_3_0 "
	"dp3 r0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_120_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_3_0 : dp3 source reg combination oC0, oDepth is NOT allowed";
string PS_120 = 
	"ps_3_0 "
	"dp3 r0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_121_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_3_0 : dp3 source reg combination oDepth, b0 is NOT allowed";
string PS_121 = 
	"ps_3_0 "
	"defb b0, true "
	"dp3 r0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_122_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_3_0 : dp3 source reg combination oDepth, i0 is NOT allowed";
string PS_122 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dp3 r0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_123_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_3_0 : dp3 source reg combination oDepth, aL is NOT allowed";
string PS_123 = 
	"ps_3_0 "
	"dp3 r0, oDepth, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_124_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_3_0 : dp3 source reg combination oDepth, p0 is NOT allowed";
string PS_124 = 
	"ps_3_0 "
	"dp3 r0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_125_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_125_Desc = "ps_3_0 : dp3 source reg combination oDepth, s0 is NOT allowed";
string PS_125 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"dp3 r0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_126_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_3_0 : dp3 source reg combination oDepth, vFace is NOT allowed";
string PS_126 = 
	"ps_3_0 "
	"dp3 r0, oDepth, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_127_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_3_0 : dp3 source reg combination oDepth, oC0 is NOT allowed";
string PS_127 = 
	"ps_3_0 "
	"dp3 r0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_128_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_3_0 : dp3 source reg combination oDepth, oDepth is NOT allowed";
string PS_128 = 
	"ps_3_0 "
	"dp3 r0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_129_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_3_0 : dp3 dest r# is allowed";
string PS_129 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_130_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_3_0 : dp3 _pp dest r# is allowed";
string PS_130 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp r0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_131_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_3_0 : dp3 _sat dest r# is allowed";
string PS_131 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_sat r0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_132_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_3_0 : dp3 _centroid dest r# is NOT allowed";
string PS_132 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid r0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_133_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_3_0 : dp3 dest oC# is allowed";
string PS_133 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 oC0, v0, r0 ";

PS_CRITERIA PS_134_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_3_0 : dp3 _pp dest oC# is allowed";
string PS_134 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp oC0, v0, r0 ";

PS_CRITERIA PS_135_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_3_0 : dp3 _centroid dest oC# is NOT allowed";
string PS_135 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid oC0, v0, r0 ";

PS_CRITERIA PS_136_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_136_Desc = "ps_3_0 : dp3 dest oDepth is allowed";
string PS_136 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 oDepth, v0.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_137_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_137_Desc = "ps_3_0 : dp3 _pp dest oDepth is allowed";
string PS_137 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp oDepth, v0.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_138_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_138_Desc = "ps_3_0 : dp3 _centroid dest oDepth is NOT allowed";
string PS_138 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid oDepth, v0.x, r0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_139_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_139_Desc = "ps_3_0 : dp3 dest v# is NOT allowed";
string PS_139 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 v0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_140_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_140_Desc = "ps_3_0 : dp3 _pp dest v# is NOT allowed";
string PS_140 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp v0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_141_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_141_Desc = "ps_3_0 : dp3 _centroid dest v# is NOT allowed";
string PS_141 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid v0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_142_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_142_Desc = "ps_3_0 : dp3 _sat dest v# is NOT allowed";
string PS_142 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_sat v0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_143_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_143_Desc = "ps_3_0 : dp3 dest c# is NOT allowed";
string PS_143 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 c1, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_144_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_144_Desc = "ps_3_0 : dp3 _pp dest c# is NOT allowed";
string PS_144 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp c1, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_145_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_145_Desc = "ps_3_0 : dp3 _centroid dest c# is NOT allowed";
string PS_145 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid c1, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_146_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_146_Desc = "ps_3_0 : dp3 _sat dest c# is NOT allowed";
string PS_146 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_sat c1, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_147_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_147_Desc = "ps_3_0 : dp3 dest b# is NOT allowed";
string PS_147 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 b0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_148_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_148_Desc = "ps_3_0 : dp3 _pp dest b# is NOT allowed";
string PS_148 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp b0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_149_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_149_Desc = "ps_3_0 : dp3 _centroid dest b# is NOT allowed";
string PS_149 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid b0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_150_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_150_Desc = "ps_3_0 : dp3 _sat dest b# is NOT allowed";
string PS_150 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_sat b0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_151_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_151_Desc = "ps_3_0 : dp3 dest i# is NOT allowed";
string PS_151 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 i0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_152_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_152_Desc = "ps_3_0 : dp3 _pp dest i# is NOT allowed";
string PS_152 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp i0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_153_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_153_Desc = "ps_3_0 : dp3 _centroid dest i# is NOT allowed";
string PS_153 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid i0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_154_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_154_Desc = "ps_3_0 : dp3 _sat dest i# is NOT allowed";
string PS_154 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_sat i0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_155_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_155_Desc = "ps_3_0 : dp3 dest aL is NOT allowed";
string PS_155 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 aL, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_156_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_156_Desc = "ps_3_0 : dp3 _pp dest aL is NOT allowed";
string PS_156 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp aL, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_157_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_157_Desc = "ps_3_0 : dp3 _centroid dest aL is NOT allowed";
string PS_157 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid aL, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_158_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_158_Desc = "ps_3_0 : dp3 _sat dest aL is NOT allowed";
string PS_158 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_sat aL, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_159_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_159_Desc = "ps_3_0 : dp3 dest p0 is NOT allowed";
string PS_159 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 p0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_160_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_160_Desc = "ps_3_0 : dp3 _pp dest p0 is NOT allowed";
string PS_160 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp p0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_161_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_161_Desc = "ps_3_0 : dp3 _centroid dest p0 is NOT allowed";
string PS_161 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid p0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_162_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_162_Desc = "ps_3_0 : dp3 _sat dest p0 is NOT allowed";
string PS_162 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_sat p0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_163_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_163_Desc = "ps_3_0 : dp3 dest s# is NOT allowed";
string PS_163 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 s0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_164_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_164_Desc = "ps_3_0 : dp3 _pp dest s# is NOT allowed";
string PS_164 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp s0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_165_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_165_Desc = "ps_3_0 : dp3 _centroid dest s# is NOT allowed";
string PS_165 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid s0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_166_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_166_Desc = "ps_3_0 : dp3 _sat dest s# is NOT allowed";
string PS_166 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_sat s0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_167_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_167_Desc = "ps_3_0 : dp3 dest vFace is NOT allowed";
string PS_167 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 vFace, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_168_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_168_Desc = "ps_3_0 : dp3 _pp dest vFace is NOT allowed";
string PS_168 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp vFace, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_169_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_169_Desc = "ps_3_0 : dp3 _centroid dest vFace is NOT allowed";
string PS_169 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid vFace, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_170_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_170_Desc = "ps_3_0 : dp3 _sat dest vFace is NOT allowed";
string PS_170 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_sat vFace, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_171_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_171_Desc = "ps_3_0 : dp3 dest vPos is NOT allowed";
string PS_171 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 vPos, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_172_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_172_Desc = "ps_3_0 : dp3 _pp dest vPos is NOT allowed";
string PS_172 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_pp vPos, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_173_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_173_Desc = "ps_3_0 : dp3 _centroid dest vPos is NOT allowed";
string PS_173 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_centroid vPos, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_174_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_174_Desc = "ps_3_0 : dp3 _sat dest vPos is NOT allowed";
string PS_174 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3_sat vPos, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_175_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_175_Desc = "ps_3_0 : dp3 write mask .r is allowed";
string PS_175 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.r, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_176_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_176_Desc = "ps_3_0 : dp3 write mask .g is allowed";
string PS_176 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.g, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_177_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_177_Desc = "ps_3_0 : dp3 write mask .b is allowed";
string PS_177 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.b, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_178_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_178_Desc = "ps_3_0 : dp3 write mask .a is allowed";
string PS_178 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.a, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_179_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_179_Desc = "ps_3_0 : dp3 write mask .rg is allowed";
string PS_179 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.rg, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_180_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_180_Desc = "ps_3_0 : dp3 write mask .gb is allowed";
string PS_180 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.gb, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_181_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_181_Desc = "ps_3_0 : dp3 write mask .ba is allowed";
string PS_181 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.ba, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_182_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_182_Desc = "ps_3_0 : dp3 write mask .rb is allowed";
string PS_182 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.rb, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_183_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_183_Desc = "ps_3_0 : dp3 write mask .ra is allowed";
string PS_183 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.ra, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_184_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_184_Desc = "ps_3_0 : dp3 write mask .ga is allowed";
string PS_184 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.ga, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_185_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_185_Desc = "ps_3_0 : dp3 write mask .rgb is allowed";
string PS_185 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.rgb, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_186_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_186_Desc = "ps_3_0 : dp3 write mask .gba is allowed";
string PS_186 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.gba, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_187_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_187_Desc = "ps_3_0 : dp3 write mask .rba is allowed";
string PS_187 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.rba, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_188_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_188_Desc = "ps_3_0 : dp3 write mask .rgba is allowed";
string PS_188 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.rgba, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_189_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_189_Desc = "ps_3_0 : dp3 write mask .yx is NOT allowed";
string PS_189 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.yx, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_190_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_190_Desc = "ps_3_0 : dp3 write mask .zx is NOT allowed";
string PS_190 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.zx, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_191_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_191_Desc = "ps_3_0 : dp3 write mask .zy is NOT allowed";
string PS_191 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.zy, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_192_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_192_Desc = "ps_3_0 : dp3 write mask .wx is NOT allowed";
string PS_192 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.wx, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_193_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_193_Desc = "ps_3_0 : dp3 write mask .wz is NOT allowed";
string PS_193 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.wz, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_194_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_194_Desc = "ps_3_0 : dp3 write mask .wy is NOT allowed";
string PS_194 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.wy, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_195_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_195_Desc = "ps_3_0 : dp3 write mask .zyx is NOT allowed";
string PS_195 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.zyx, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_196_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_196_Desc = "ps_3_0 : dp3 write mask .wzy is NOT allowed";
string PS_196 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.wzy, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_197_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_197_Desc = "ps_3_0 : dp3 write mask .wzx is NOT allowed";
string PS_197 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.wzx, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_198_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_198_Desc = "ps_3_0 : dp3 write mask .wyx is NOT allowed";
string PS_198 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.wyx, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_199_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_199_Desc = "ps_3_0 : dp3 write mask .yxw is NOT allowed";
string PS_199 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.yxw, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_200_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_200_Desc = "ps_3_0 : dp3 write mask .wzyx is NOT allowed";
string PS_200 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.wzyx, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_201_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_201_Desc = "ps_3_0 : dp3 write mask .zxwy is NOT allowed";
string PS_201 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"dp3 r0.zxwy, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_202_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_202_Desc = "ps_3_0 : dp3 is allowed in a 1 level if b0 block";
string PS_202 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 "
	"	dp3 r0, v0, r0 "
	"endif ";

PS_CRITERIA PS_203_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_203_Desc = "ps_3_0 : dp3 is allowed in a 8 level if b0 block";
string PS_203 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								dp3 r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_204_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_204_Desc = "ps_3_0 : dp3 is allowed in a 16 level if b0 block";
string PS_204 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
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
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_205_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_205_Desc = "ps_3_0 : dp3 is allowed in a 24 level if b0 block";
string PS_205 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
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
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_206_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_206_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level if b0 block";
string PS_206 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
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
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_207_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_207_Desc = "ps_3_0 : dp3 is allowed in a 1 level if b0 nop else block";
string PS_207 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	dp3 r0, v0, r0 "
	"endif ";

PS_CRITERIA PS_208_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_208_Desc = "ps_3_0 : dp3 is allowed in a 8 level if b0 nop else block";
string PS_208 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								dp3 r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_209_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_209_Desc = "ps_3_0 : dp3 is allowed in a 16 level if b0 nop else block";
string PS_209 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
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
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_210_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_210_Desc = "ps_3_0 : dp3 is allowed in a 24 level if b0 nop else block";
string PS_210 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
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
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_211_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_211_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level if b0 nop else block";
string PS_211 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
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
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_212_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_212_Desc = "ps_3_0 : dp3 is allowed in a 1 level loop aL, i0 block";
string PS_212 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	dp3 r0, v0, r0 "
	"endloop ";

PS_CRITERIA PS_213_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_213_Desc = "ps_3_0 : dp3 is allowed in a 2 level loop aL, i0 block";
string PS_213 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_214_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_214_Desc = "ps_3_0 : dp3 is allowed in a 3 level loop aL, i0 block";
string PS_214 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			dp3 r0, v0, r0 "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_215_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_215_Desc = "ps_3_0 : dp3 is allowed in a 4 level loop aL, i0 block";
string PS_215 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				dp3 r0, v0, r0 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_216_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_216_Desc = "ps_3_0 : dp3 is NOT allowed in a 5 level loop aL, i0 block";
string PS_216 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					dp3 r0, v0, r0 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_217_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_217_Desc = "ps_3_0 : dp3 is allowed in a 1 level rep i0 block";
string PS_217 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	dp3 r0, v0, r0 "
	"endrep ";

PS_CRITERIA PS_218_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_218_Desc = "ps_3_0 : dp3 is allowed in a 2 level rep i0 block";
string PS_218 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_219_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_219_Desc = "ps_3_0 : dp3 is allowed in a 3 level rep i0 block";
string PS_219 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			dp3 r0, v0, r0 "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_220_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_220_Desc = "ps_3_0 : dp3 is allowed in a 4 level rep i0 block";
string PS_220 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				dp3 r0, v0, r0 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_221_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_221_Desc = "ps_3_0 : dp3 is NOT allowed in a 5 level rep i0 block";
string PS_221 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					dp3 r0, v0, r0 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_222_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_222_Desc = "ps_3_0 : dp3 is allowed in a 1 level if_gt c0.x, c0.y block";
string PS_222 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	dp3 r0, v0, r0 "
	"endif ";

PS_CRITERIA PS_223_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_223_Desc = "ps_3_0 : dp3 is allowed in a 8 level if_gt c0.x, c0.y block";
string PS_223 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								dp3 r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_224_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_224_Desc = "ps_3_0 : dp3 is allowed in a 16 level if_gt c0.x, c0.y block";
string PS_224 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								if_gt c0.x, c0.y "
	"									if_gt c0.x, c0.y "
	"										if_gt c0.x, c0.y "
	"											if_gt c0.x, c0.y "
	"												if_gt c0.x, c0.y "
	"													if_gt c0.x, c0.y "
	"														if_gt c0.x, c0.y "
	"															if_gt c0.x, c0.y "
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_225_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_225_Desc = "ps_3_0 : dp3 is allowed in a 24 level if_gt c0.x, c0.y block";
string PS_225 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								if_gt c0.x, c0.y "
	"									if_gt c0.x, c0.y "
	"										if_gt c0.x, c0.y "
	"											if_gt c0.x, c0.y "
	"												if_gt c0.x, c0.y "
	"													if_gt c0.x, c0.y "
	"														if_gt c0.x, c0.y "
	"															if_gt c0.x, c0.y "
	"																if_gt c0.x, c0.y "
	"																	if_gt c0.x, c0.y "
	"																		if_gt c0.x, c0.y "
	"																			if_gt c0.x, c0.y "
	"																				if_gt c0.x, c0.y "
	"																					if_gt c0.x, c0.y "
	"																						if_gt c0.x, c0.y "
	"																							if_gt c0.x, c0.y "
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_226_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_226_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level if_gt c0.x, c0.y block";
string PS_226 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		if_gt c0.x, c0.y "
	"			if_gt c0.x, c0.y "
	"				if_gt c0.x, c0.y "
	"					if_gt c0.x, c0.y "
	"						if_gt c0.x, c0.y "
	"							if_gt c0.x, c0.y "
	"								if_gt c0.x, c0.y "
	"									if_gt c0.x, c0.y "
	"										if_gt c0.x, c0.y "
	"											if_gt c0.x, c0.y "
	"												if_gt c0.x, c0.y "
	"													if_gt c0.x, c0.y "
	"														if_gt c0.x, c0.y "
	"															if_gt c0.x, c0.y "
	"																if_gt c0.x, c0.y "
	"																	if_gt c0.x, c0.y "
	"																		if_gt c0.x, c0.y "
	"																			if_gt c0.x, c0.y "
	"																				if_gt c0.x, c0.y "
	"																					if_gt c0.x, c0.y "
	"																						if_gt c0.x, c0.y "
	"																							if_gt c0.x, c0.y "
	"																								if_gt c0.x, c0.y "
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_227_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_227_Desc = "ps_3_0 : dp3 is allowed in a 1 level if_gt c0.x, c0.y nop else block";
string PS_227 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	dp3 r0, v0, r0 "
	"endif ";

PS_CRITERIA PS_228_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_228_Desc = "ps_3_0 : dp3 is allowed in a 8 level if_gt c0.x, c0.y nop else block";
string PS_228 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								dp3 r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_229_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_229_Desc = "ps_3_0 : dp3 is allowed in a 16 level if_gt c0.x, c0.y nop else block";
string PS_229 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								if_gt c0.x, c0.y nop else "
	"									if_gt c0.x, c0.y nop else "
	"										if_gt c0.x, c0.y nop else "
	"											if_gt c0.x, c0.y nop else "
	"												if_gt c0.x, c0.y nop else "
	"													if_gt c0.x, c0.y nop else "
	"														if_gt c0.x, c0.y nop else "
	"															if_gt c0.x, c0.y nop else "
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_230_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_230_Desc = "ps_3_0 : dp3 is allowed in a 24 level if_gt c0.x, c0.y nop else block";
string PS_230 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								if_gt c0.x, c0.y nop else "
	"									if_gt c0.x, c0.y nop else "
	"										if_gt c0.x, c0.y nop else "
	"											if_gt c0.x, c0.y nop else "
	"												if_gt c0.x, c0.y nop else "
	"													if_gt c0.x, c0.y nop else "
	"														if_gt c0.x, c0.y nop else "
	"															if_gt c0.x, c0.y nop else "
	"																if_gt c0.x, c0.y nop else "
	"																	if_gt c0.x, c0.y nop else "
	"																		if_gt c0.x, c0.y nop else "
	"																			if_gt c0.x, c0.y nop else "
	"																				if_gt c0.x, c0.y nop else "
	"																					if_gt c0.x, c0.y nop else "
	"																						if_gt c0.x, c0.y nop else "
	"																							if_gt c0.x, c0.y nop else "
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_231_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_231_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level if_gt c0.x, c0.y nop else block";
string PS_231 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		if_gt c0.x, c0.y nop else "
	"			if_gt c0.x, c0.y nop else "
	"				if_gt c0.x, c0.y nop else "
	"					if_gt c0.x, c0.y nop else "
	"						if_gt c0.x, c0.y nop else "
	"							if_gt c0.x, c0.y nop else "
	"								if_gt c0.x, c0.y nop else "
	"									if_gt c0.x, c0.y nop else "
	"										if_gt c0.x, c0.y nop else "
	"											if_gt c0.x, c0.y nop else "
	"												if_gt c0.x, c0.y nop else "
	"													if_gt c0.x, c0.y nop else "
	"														if_gt c0.x, c0.y nop else "
	"															if_gt c0.x, c0.y nop else "
	"																if_gt c0.x, c0.y nop else "
	"																	if_gt c0.x, c0.y nop else "
	"																		if_gt c0.x, c0.y nop else "
	"																			if_gt c0.x, c0.y nop else "
	"																				if_gt c0.x, c0.y nop else "
	"																					if_gt c0.x, c0.y nop else "
	"																						if_gt c0.x, c0.y nop else "
	"																							if_gt c0.x, c0.y nop else "
	"																								if_gt c0.x, c0.y nop else "
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_232_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_232_Desc = "ps_3_0 : dp3 is allowed in a 1 level if p0.x block";
string PS_232 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x "
	"	dp3 r0, v0, r0 "
	"endif ";

PS_CRITERIA PS_233_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_233_Desc = "ps_3_0 : dp3 is allowed in a 8 level if p0.x block";
string PS_233 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								dp3 r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_234_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_234_Desc = "ps_3_0 : dp3 is allowed in a 16 level if p0.x block";
string PS_234 = 
	"ps_3_0 "
	"dcl_color v0 "
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
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_235_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_235_Desc = "ps_3_0 : dp3 is allowed in a 24 level if p0.x block";
string PS_235 = 
	"ps_3_0 "
	"dcl_color v0 "
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
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_236_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_236_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level if p0.x block";
string PS_236 = 
	"ps_3_0 "
	"dcl_color v0 "
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
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_237_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_237_Desc = "ps_3_0 : dp3 is allowed in a 1 level if p0.x nop else block";
string PS_237 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	dp3 r0, v0, r0 "
	"endif ";

PS_CRITERIA PS_238_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_238_Desc = "ps_3_0 : dp3 is allowed in a 8 level if p0.x nop else block";
string PS_238 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								dp3 r0, v0, r0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_239_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_239_Desc = "ps_3_0 : dp3 is allowed in a 16 level if p0.x nop else block";
string PS_239 = 
	"ps_3_0 "
	"dcl_color v0 "
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
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_240_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_240_Desc = "ps_3_0 : dp3 is allowed in a 24 level if p0.x nop else block";
string PS_240 = 
	"ps_3_0 "
	"dcl_color v0 "
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
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_241_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_241_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level if p0.x nop else block";
string PS_241 = 
	"ps_3_0 "
	"dcl_color v0 "
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
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_242_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_242_Desc = "ps_3_0 : dp3 is allowed in a 1 level loop aL, i0 break block";
string PS_242 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	dp3 r0, v0, r0 "
	"endloop ";

PS_CRITERIA PS_243_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_243_Desc = "ps_3_0 : dp3 is NOT allowed in a 8 level loop aL, i0 break block";
string PS_243 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								dp3 r0, v0, r0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_244_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_244_Desc = "ps_3_0 : dp3 is NOT allowed in a 16 level loop aL, i0 break block";
string PS_244 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
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
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_245_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_245_Desc = "ps_3_0 : dp3 is NOT allowed in a 24 level loop aL, i0 break block";
string PS_245 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
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
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_246_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_246_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level loop aL, i0 break block";
string PS_246 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
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
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_247_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_247_Desc = "ps_3_0 : dp3 is allowed in a 1 level rep i0 break block";
string PS_247 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	dp3 r0, v0, r0 "
	"endrep ";

PS_CRITERIA PS_248_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_248_Desc = "ps_3_0 : dp3 is NOT allowed in a 8 level rep i0 break block";
string PS_248 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								dp3 r0, v0, r0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_249_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_249_Desc = "ps_3_0 : dp3 is NOT allowed in a 16 level rep i0 break block";
string PS_249 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
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
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_250_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_250_Desc = "ps_3_0 : dp3 is NOT allowed in a 24 level rep i0 break block";
string PS_250 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
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
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_251_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_251_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level rep i0 break block";
string PS_251 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
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
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_252_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_252_Desc = "ps_3_0 : dp3 is allowed in a 1 level loop aL, i0 breakp p0.x block";
string PS_252 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	dp3 r0, v0, r0 "
	"endloop ";

PS_CRITERIA PS_253_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_253_Desc = "ps_3_0 : dp3 is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string PS_253 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								dp3 r0, v0, r0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_254_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_254_Desc = "ps_3_0 : dp3 is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string PS_254 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
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
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_255_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_255_Desc = "ps_3_0 : dp3 is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string PS_255 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_256_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_256_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string PS_256 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_257_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_257_Desc = "ps_3_0 : dp3 is allowed in a 1 level rep i0 breakp p0.x block";
string PS_257 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	dp3 r0, v0, r0 "
	"endrep ";

PS_CRITERIA PS_258_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_258_Desc = "ps_3_0 : dp3 is NOT allowed in a 8 level rep i0 breakp p0.x block";
string PS_258 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								dp3 r0, v0, r0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_259_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_259_Desc = "ps_3_0 : dp3 is NOT allowed in a 16 level rep i0 breakp p0.x block";
string PS_259 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_260_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_260_Desc = "ps_3_0 : dp3 is NOT allowed in a 24 level rep i0 breakp p0.x block";
string PS_260 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_261_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_261_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level rep i0 breakp p0.x block";
string PS_261 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_262_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_262_Desc = "ps_3_0 : dp3 is allowed in a 1 level loop aL, i0 break_eq c0.x, c0.y block";
string PS_262 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	dp3 r0, v0, r0 "
	"endloop ";

PS_CRITERIA PS_263_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_263_Desc = "ps_3_0 : dp3 is NOT allowed in a 8 level loop aL, i0 break_eq c0.x, c0.y block";
string PS_263 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								dp3 r0, v0, r0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_264_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_264_Desc = "ps_3_0 : dp3 is NOT allowed in a 16 level loop aL, i0 break_eq c0.x, c0.y block";
string PS_264 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								loop aL, i0 break_eq c0.x, c0.y "
	"									loop aL, i0 break_eq c0.x, c0.y "
	"										loop aL, i0 break_eq c0.x, c0.y "
	"											loop aL, i0 break_eq c0.x, c0.y "
	"												loop aL, i0 break_eq c0.x, c0.y "
	"													loop aL, i0 break_eq c0.x, c0.y "
	"														loop aL, i0 break_eq c0.x, c0.y "
	"															loop aL, i0 break_eq c0.x, c0.y "
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_265_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_265_Desc = "ps_3_0 : dp3 is NOT allowed in a 24 level loop aL, i0 break_eq c0.x, c0.y block";
string PS_265 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								loop aL, i0 break_eq c0.x, c0.y "
	"									loop aL, i0 break_eq c0.x, c0.y "
	"										loop aL, i0 break_eq c0.x, c0.y "
	"											loop aL, i0 break_eq c0.x, c0.y "
	"												loop aL, i0 break_eq c0.x, c0.y "
	"													loop aL, i0 break_eq c0.x, c0.y "
	"														loop aL, i0 break_eq c0.x, c0.y "
	"															loop aL, i0 break_eq c0.x, c0.y "
	"																loop aL, i0 break_eq c0.x, c0.y "
	"																	loop aL, i0 break_eq c0.x, c0.y "
	"																		loop aL, i0 break_eq c0.x, c0.y "
	"																			loop aL, i0 break_eq c0.x, c0.y "
	"																				loop aL, i0 break_eq c0.x, c0.y "
	"																					loop aL, i0 break_eq c0.x, c0.y "
	"																						loop aL, i0 break_eq c0.x, c0.y "
	"																							loop aL, i0 break_eq c0.x, c0.y "
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_266_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_266_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level loop aL, i0 break_eq c0.x, c0.y block";
string PS_266 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								loop aL, i0 break_eq c0.x, c0.y "
	"									loop aL, i0 break_eq c0.x, c0.y "
	"										loop aL, i0 break_eq c0.x, c0.y "
	"											loop aL, i0 break_eq c0.x, c0.y "
	"												loop aL, i0 break_eq c0.x, c0.y "
	"													loop aL, i0 break_eq c0.x, c0.y "
	"														loop aL, i0 break_eq c0.x, c0.y "
	"															loop aL, i0 break_eq c0.x, c0.y "
	"																loop aL, i0 break_eq c0.x, c0.y "
	"																	loop aL, i0 break_eq c0.x, c0.y "
	"																		loop aL, i0 break_eq c0.x, c0.y "
	"																			loop aL, i0 break_eq c0.x, c0.y "
	"																				loop aL, i0 break_eq c0.x, c0.y "
	"																					loop aL, i0 break_eq c0.x, c0.y "
	"																						loop aL, i0 break_eq c0.x, c0.y "
	"																							loop aL, i0 break_eq c0.x, c0.y "
	"																								loop aL, i0 break_eq c0.x, c0.y "
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_267_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_267_Desc = "ps_3_0 : dp3 is allowed in a 1 level rep i0 break_eq c0.x, c0.y block";
string PS_267 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	dp3 r0, v0, r0 "
	"endrep ";

PS_CRITERIA PS_268_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_268_Desc = "ps_3_0 : dp3 is NOT allowed in a 8 level rep i0 break_eq c0.x, c0.y block";
string PS_268 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		rep i0 break_eq c0.x, c0.y "
	"			rep i0 break_eq c0.x, c0.y "
	"				rep i0 break_eq c0.x, c0.y "
	"					rep i0 break_eq c0.x, c0.y "
	"						rep i0 break_eq c0.x, c0.y "
	"							rep i0 break_eq c0.x, c0.y "
	"								dp3 r0, v0, r0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_269_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_269_Desc = "ps_3_0 : dp3 is NOT allowed in a 16 level rep i0 break_eq c0.x, c0.y block";
string PS_269 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		rep i0 break_eq c0.x, c0.y "
	"			rep i0 break_eq c0.x, c0.y "
	"				rep i0 break_eq c0.x, c0.y "
	"					rep i0 break_eq c0.x, c0.y "
	"						rep i0 break_eq c0.x, c0.y "
	"							rep i0 break_eq c0.x, c0.y "
	"								rep i0 break_eq c0.x, c0.y "
	"									rep i0 break_eq c0.x, c0.y "
	"										rep i0 break_eq c0.x, c0.y "
	"											rep i0 break_eq c0.x, c0.y "
	"												rep i0 break_eq c0.x, c0.y "
	"													rep i0 break_eq c0.x, c0.y "
	"														rep i0 break_eq c0.x, c0.y "
	"															rep i0 break_eq c0.x, c0.y "
	"																dp3 r0, v0, r0 "
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

PS_CRITERIA PS_270_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_270_Desc = "ps_3_0 : dp3 is NOT allowed in a 24 level rep i0 break_eq c0.x, c0.y block";
string PS_270 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		rep i0 break_eq c0.x, c0.y "
	"			rep i0 break_eq c0.x, c0.y "
	"				rep i0 break_eq c0.x, c0.y "
	"					rep i0 break_eq c0.x, c0.y "
	"						rep i0 break_eq c0.x, c0.y "
	"							rep i0 break_eq c0.x, c0.y "
	"								rep i0 break_eq c0.x, c0.y "
	"									rep i0 break_eq c0.x, c0.y "
	"										rep i0 break_eq c0.x, c0.y "
	"											rep i0 break_eq c0.x, c0.y "
	"												rep i0 break_eq c0.x, c0.y "
	"													rep i0 break_eq c0.x, c0.y "
	"														rep i0 break_eq c0.x, c0.y "
	"															rep i0 break_eq c0.x, c0.y "
	"																rep i0 break_eq c0.x, c0.y "
	"																	rep i0 break_eq c0.x, c0.y "
	"																		rep i0 break_eq c0.x, c0.y "
	"																			rep i0 break_eq c0.x, c0.y "
	"																				rep i0 break_eq c0.x, c0.y "
	"																					rep i0 break_eq c0.x, c0.y "
	"																						rep i0 break_eq c0.x, c0.y "
	"																							rep i0 break_eq c0.x, c0.y "
	"																								dp3 r0, v0, r0 "
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

PS_CRITERIA PS_271_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_271_Desc = "ps_3_0 : dp3 is NOT allowed in a 25 level rep i0 break_eq c0.x, c0.y block";
string PS_271 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		rep i0 break_eq c0.x, c0.y "
	"			rep i0 break_eq c0.x, c0.y "
	"				rep i0 break_eq c0.x, c0.y "
	"					rep i0 break_eq c0.x, c0.y "
	"						rep i0 break_eq c0.x, c0.y "
	"							rep i0 break_eq c0.x, c0.y "
	"								rep i0 break_eq c0.x, c0.y "
	"									rep i0 break_eq c0.x, c0.y "
	"										rep i0 break_eq c0.x, c0.y "
	"											rep i0 break_eq c0.x, c0.y "
	"												rep i0 break_eq c0.x, c0.y "
	"													rep i0 break_eq c0.x, c0.y "
	"														rep i0 break_eq c0.x, c0.y "
	"															rep i0 break_eq c0.x, c0.y "
	"																rep i0 break_eq c0.x, c0.y "
	"																	rep i0 break_eq c0.x, c0.y "
	"																		rep i0 break_eq c0.x, c0.y "
	"																			rep i0 break_eq c0.x, c0.y "
	"																				rep i0 break_eq c0.x, c0.y "
	"																					rep i0 break_eq c0.x, c0.y "
	"																						rep i0 break_eq c0.x, c0.y "
	"																							rep i0 break_eq c0.x, c0.y "
	"																								rep i0 break_eq c0.x, c0.y "
	"																									dp3 r0, v0, r0 "
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

PS_CRITERIA PS_272_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_272_Desc = "ps_3_0 : dp3 is allowed in a if b0 and if b0 block";
string PS_272 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_273_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_273_Desc = "ps_3_0 : dp3 is allowed in a if b0 and if b0 nop else block";
string PS_273 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_274_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_274_Desc = "ps_3_0 : dp3 is allowed in a if b0 and loop aL, i0 block";
string PS_274 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_275_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_275_Desc = "ps_3_0 : dp3 is allowed in a if b0 and rep i0 block";
string PS_275 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_276_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_276_Desc = "ps_3_0 : dp3 is allowed in a if b0 and if_eq c0.x, c0.y block";
string PS_276 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	if_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_277_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_277_Desc = "ps_3_0 : dp3 is allowed in a if b0 and if_eq c0.x, c0.y nop else block";
string PS_277 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	if_eq c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_278_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_278_Desc = "ps_3_0 : dp3 is allowed in a if b0 and if p0.x block";
string PS_278 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_279_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_279_Desc = "ps_3_0 : dp3 is allowed in a if b0 and if p0.x nop else block";
string PS_279 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_280_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_280_Desc = "ps_3_0 : dp3 is allowed in a if b0 and loop aL, i0 break block";
string PS_280 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_281_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_281_Desc = "ps_3_0 : dp3 is allowed in a if b0 and rep i0 break block";
string PS_281 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_282_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_282_Desc = "ps_3_0 : dp3 is allowed in a if b0 and loop aL, i0 breakp p0.x block";
string PS_282 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_283_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_283_Desc = "ps_3_0 : dp3 is allowed in a if b0 and rep i0 breakp p0.x block";
string PS_283 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_284_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_284_Desc = "ps_3_0 : dp3 is allowed in a if b0 and loop aL, i0 break_eq c0.x, c0.y block";
string PS_284 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_285_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_285_Desc = "ps_3_0 : dp3 is allowed in a if b0 and rep i0 break_eq c0.x, c0.y block";
string PS_285 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_286_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_286_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and if b0 block";
string PS_286 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_287_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_287_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and if b0 nop else block";
string PS_287 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_288_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_288_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and loop aL, i0 block";
string PS_288 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_289_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_289_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and rep i0 block";
string PS_289 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_290_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_290_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and if_eq c0.x, c0.y block";
string PS_290 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_291_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_291_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and if_eq c0.x, c0.y nop else block";
string PS_291 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if_eq c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_292_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_292_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and if p0.x block";
string PS_292 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_293_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_293_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and if p0.x nop else block";
string PS_293 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_294_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_294_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and loop aL, i0 break block";
string PS_294 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_295_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_295_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and rep i0 break block";
string PS_295 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_296_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_296_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string PS_296 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_297_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_297_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string PS_297 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_298_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_298_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and loop aL, i0 break_eq c0.x, c0.y block";
string PS_298 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_299_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_299_Desc = "ps_3_0 : dp3 is allowed in a if b0 nop else and rep i0 break_eq c0.x, c0.y block";
string PS_299 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"if b0 nop else "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_300_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_300_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and if b0 block";
string PS_300 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_301_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_301_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and if b0 nop else block";
string PS_301 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_302_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_302_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and loop aL, i0 block";
string PS_302 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_303_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_303_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and rep i0 block";
string PS_303 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_304_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_304_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and if_lt c0.x, c0.y block";
string PS_304 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_305_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_305_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and if_lt c0.x, c0.y nop else block";
string PS_305 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_306_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_306_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and if p0.x block";
string PS_306 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_307_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_307_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and if p0.x nop else block";
string PS_307 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_308_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_308_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and loop aL, i0 break block";
string PS_308 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_309_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_309_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and rep i0 break block";
string PS_309 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_310_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_310_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string PS_310 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_311_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_311_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string PS_311 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_312_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_312_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and loop aL, i0 break_eq c0.x, c0.y block";
string PS_312 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_313_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_313_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 and rep i0 break_eq c0.x, c0.y block";
string PS_313 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_314_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_314_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and if b0 block";
string PS_314 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_315_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_315_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and if b0 nop else block";
string PS_315 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_316_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_316_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and loop aL, i0 block";
string PS_316 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_317_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_317_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and rep i0 block";
string PS_317 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_318_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_318_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and if_lt c0.x, c0.y block";
string PS_318 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if_lt c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_319_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_319_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and if_lt c0.x, c0.y nop else block";
string PS_319 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if_lt c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_320_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_320_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and if p0.x block";
string PS_320 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_321_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_321_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and if p0.x nop else block";
string PS_321 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_322_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_322_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and loop aL, i0 break block";
string PS_322 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_323_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_323_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and rep i0 break block";
string PS_323 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_324_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_324_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string PS_324 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_325_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_325_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and rep i0 breakp p0.x block";
string PS_325 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_326_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_326_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and loop aL, i0 break_eq c0.x, c0.y block";
string PS_326 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_327_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_327_Desc = "ps_3_0 : dp3 is allowed in a rep i0 and rep i0 break_eq c0.x, c0.y block";
string PS_327 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_328_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_328_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and if b0 block";
string PS_328 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_329_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_329_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and if b0 nop else block";
string PS_329 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_330_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_330_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and loop aL, i0 block";
string PS_330 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_331_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_331_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and rep i0 block";
string PS_331 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_332_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_332_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and if_gt c0.x, c0.y block";
string PS_332 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_333_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_333_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and if_gt c0.x, c0.y nop else block";
string PS_333 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if_gt c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_334_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_334_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and if p0.x block";
string PS_334 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_335_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_335_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and if p0.x nop else block";
string PS_335 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_336_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_336_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and loop aL, i0 break block";
string PS_336 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_337_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_337_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and rep i0 break block";
string PS_337 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_338_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_338_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_338 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_339_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_339_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and rep i0 breakp p0.x block";
string PS_339 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_340_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_340_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string PS_340 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_341_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_341_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y and rep i0 break_eq c0.x, c0.y block";
string PS_341 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_342_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_342_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and if b0 block";
string PS_342 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_343_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_343_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and if b0 nop else block";
string PS_343 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_344_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_344_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and loop aL, i0 block";
string PS_344 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_345_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_345_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and rep i0 block";
string PS_345 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_346_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_346_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and if_gt c0.x, c0.y block";
string PS_346 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_347_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_347_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and if_gt c0.x, c0.y nop else block";
string PS_347 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if_gt c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_348_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_348_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and if p0.x block";
string PS_348 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_349_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_349_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and if p0.x nop else block";
string PS_349 = 
	"ps_3_0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_350_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_350_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and loop aL, i0 break block";
string PS_350 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_351_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_351_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and rep i0 break block";
string PS_351 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_352_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_352_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string PS_352 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_353_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_353_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and rep i0 breakp p0.x block";
string PS_353 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_354_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_354_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and loop aL, i0 break_eq c0.x, c0.y block";
string PS_354 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_355_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_355_Desc = "ps_3_0 : dp3 is allowed in a if_gt c0.x, c0.y nop else and rep i0 break_eq c0.x, c0.y block";
string PS_355 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"if_gt c0.x, c0.y nop else "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_356_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_356_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and if b0 block";
string PS_356 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_357_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_357_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and if b0 nop else block";
string PS_357 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_358_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_358_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and loop aL, i0 block";
string PS_358 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_359_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_359_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and rep i0 block";
string PS_359 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_360_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_360_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and if_ge c0.x, c0.y block";
string PS_360 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if_ge c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_361_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_361_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and if_ge c0.x, c0.y nop else block";
string PS_361 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_362_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_362_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and if p0.x block";
string PS_362 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_363_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_363_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and if p0.x nop else block";
string PS_363 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_364_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_364_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and loop aL, i0 break block";
string PS_364 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_365_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_365_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and rep i0 break block";
string PS_365 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_366_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_366_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string PS_366 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_367_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_367_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and rep i0 breakp p0.x block";
string PS_367 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_368_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_368_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_368 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_369_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_369_Desc = "ps_3_0 : dp3 is allowed in a if p0.x and rep i0 break_eq c0.x, c0.y block";
string PS_369 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_370_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_370_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and if b0 block";
string PS_370 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_371_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_371_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and if b0 nop else block";
string PS_371 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_372_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_372_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and loop aL, i0 block";
string PS_372 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_373_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_373_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and rep i0 block";
string PS_373 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_374_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_374_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and if_ge c0.x, c0.y block";
string PS_374 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if_ge c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_375_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_375_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and if_ge c0.x, c0.y nop else block";
string PS_375 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if_ge c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_376_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_376_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and if p0.x block";
string PS_376 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_377_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_377_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and if p0.x nop else block";
string PS_377 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endif ";

PS_CRITERIA PS_378_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_378_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and loop aL, i0 break block";
string PS_378 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_379_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_379_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and rep i0 break block";
string PS_379 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_380_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_380_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string PS_380 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_381_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_381_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string PS_381 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_382_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_382_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and loop aL, i0 break_eq c0.x, c0.y block";
string PS_382 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_383_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_383_Desc = "ps_3_0 : dp3 is allowed in a if p0.x nop else and rep i0 break_eq c0.x, c0.y block";
string PS_383 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_384_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_384_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and if b0 block";
string PS_384 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_385_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_385_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and if b0 nop else block";
string PS_385 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_386_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_386_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_386 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_387_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_387_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and rep i0 block";
string PS_387 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_388_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_388_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and if_ne c0.x, c0.y block";
string PS_388 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if_ne c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_389_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_389_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and if_ne c0.x, c0.y nop else block";
string PS_389 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if_ne c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_390_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_390_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and if p0.x block";
string PS_390 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_391_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_391_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_391 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_392_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_392_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_392 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_393_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_393_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and rep i0 break block";
string PS_393 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_394_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_394_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_394 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_395_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_395_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_395 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_396_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_396_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and loop aL, i0 break_eq c0.x, c0.y block";
string PS_396 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_397_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_397_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break and rep i0 break_eq c0.x, c0.y block";
string PS_397 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_398_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_398_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and if b0 block";
string PS_398 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_399_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_399_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and if b0 nop else block";
string PS_399 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_400_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_400_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and loop aL, i0 block";
string PS_400 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_401_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_401_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and rep i0 block";
string PS_401 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_402_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_402_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and if_ne c0.x, c0.y block";
string PS_402 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if_ne c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_403_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_403_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and if_ne c0.x, c0.y nop else block";
string PS_403 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if_ne c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_404_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_404_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and if p0.x block";
string PS_404 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_405_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_405_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and if p0.x nop else block";
string PS_405 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_406_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_406_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and loop aL, i0 break block";
string PS_406 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_407_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_407_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and rep i0 break block";
string PS_407 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_408_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_408_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_408 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_409_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_409_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_409 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_410_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_410_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and loop aL, i0 break_eq c0.x, c0.y block";
string PS_410 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_411_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_411_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break and rep i0 break_eq c0.x, c0.y block";
string PS_411 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_412_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_412_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_412 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_413_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_413_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_413 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_414_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_414_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_414 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_415_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_415_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_415 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_416_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_416_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and if_le c0.x, c0.y block";
string PS_416 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_le c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_417_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_417_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and if_le c0.x, c0.y nop else block";
string PS_417 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_le c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_418_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_418_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_418 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_419_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_419_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_419 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_420_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_420_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_420 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_421_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_421_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_421 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_422_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_422_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_422 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_423_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_423_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_423 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_424_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_424_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_424 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_425_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_425_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 breakp p0.x and rep i0 break_eq c0.x, c0.y block";
string PS_425 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_426_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_426_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_426 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_427_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_427_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_427 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_428_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_428_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_428 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_429_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_429_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_429 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_430_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_430_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y block";
string PS_430 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_431_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_431_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y nop else block";
string PS_431 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_432_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_432_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_432 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_433_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_433_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_433 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_434_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_434_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_434 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_435_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_435_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_435 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_436_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_436_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_436 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_437_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_437_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_437 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_438_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_438_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_438 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_439_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_439_Desc = "ps_3_0 : dp3 is allowed in a rep i0 breakp p0.x and rep i0 break_eq c0.x, c0.y block";
string PS_439 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_440_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_440_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 block";
string PS_440 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_441_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_441_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 nop else block";
string PS_441 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_442_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_442_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 block";
string PS_442 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_443_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_443_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 block";
string PS_443 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_444_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_444_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and if_eq c0.x, c0.y block";
string PS_444 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_445_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_445_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and if_eq c0.x, c0.y nop else block";
string PS_445 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_446_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_446_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x block";
string PS_446 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_447_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_447_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x nop else block";
string PS_447 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_448_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_448_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 break block";
string PS_448 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_449_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_449_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 break block";
string PS_449 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_450_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_450_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_450 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_451_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_451_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 breakp p0.x block";
string PS_451 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_452_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_452_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string PS_452 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_453_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_453_Desc = "ps_3_0 : dp3 is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 break_eq c0.x, c0.y block";
string PS_453 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_454_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_454_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and if b0 block";
string PS_454 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if b0 "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_455_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_455_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and if b0 nop else block";
string PS_455 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if b0 nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_456_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_456_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 block";
string PS_456 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_457_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_457_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 block";
string PS_457 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_458_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_458_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and if_lt c0.x, c0.y block";
string PS_458 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_459_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_459_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string PS_459 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_460_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_460_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and if p0.x block";
string PS_460 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if p0.x "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_461_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_461_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and if p0.x nop else block";
string PS_461 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if p0.x nop else "
	"		dp3 r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_462_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_462_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 break block";
string PS_462 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_463_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_463_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 break block";
string PS_463 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_464_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_464_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_464 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_465_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_465_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 breakp p0.x block";
string PS_465 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_466_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_466_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string PS_466 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_467_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_467_Desc = "ps_3_0 : dp3 is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 break_eq c0.x, c0.y block";
string PS_467 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		dp3 r0, v0, r0 "
	"	endrep "
	"endrep ";


