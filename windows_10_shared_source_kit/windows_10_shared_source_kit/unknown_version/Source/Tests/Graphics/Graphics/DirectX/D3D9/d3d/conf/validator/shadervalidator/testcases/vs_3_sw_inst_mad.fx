#include "Test_Include.fx"

int Test_Count = 795;

string TestInfo
<
	string TestType = "VS";
>
= "vs_3_sw_inst_mad";

VS_CRITERIA VS_001_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_3_sw : mad source reg combination v0, v1, v2 is allowed";
string VS_001 = 
	"vs_3_sw "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"dcl_texcoord v2 "
	"mad r0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_002_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_3_sw : mad source reg combination v0, v1, r0 is allowed";
string VS_002 = 
	"vs_3_sw "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mov r0, c0 "
	"mad r0, v0, v1, r0 "
	" ";

VS_CRITERIA VS_003_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_3_sw : mad source reg combination v0, v1, c0 is allowed";
string VS_003 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mad r0, v0, v1, c0 "
	" ";

VS_CRITERIA VS_004_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_3_sw : mad source reg combination v0, r0, v1 is allowed";
string VS_004 = 
	"vs_3_sw "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mov r0, c0 "
	"mad r0, v0, r0, v1 "
	" ";

VS_CRITERIA VS_005_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_3_sw : mad source reg combination v0, r0, r1 is allowed";
string VS_005 = 
	"vs_3_sw "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, v0, r0, r1 "
	" ";

VS_CRITERIA VS_006_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_3_sw : mad source reg combination v0, r0, c0 is allowed";
string VS_006 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0 "
	" ";

VS_CRITERIA VS_007_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_3_sw : mad source reg combination v0, c0, v1 is allowed";
string VS_007 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mad r0, v0, c0, v1 "
	" ";

VS_CRITERIA VS_008_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_3_sw : mad source reg combination v0, c0, r0 is allowed";
string VS_008 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, v0, c0, r0 "
	" ";

VS_CRITERIA VS_009_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_3_sw : mad source reg combination v0, c0, c1 is allowed";
string VS_009 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mad r0, v0, c0, c1 "
	" ";

VS_CRITERIA VS_010_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_3_sw : mad source reg combination r0, v0, v1 is allowed";
string VS_010 = 
	"vs_3_sw "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mov r0, c0 "
	"mad r0, r0, v0, v1 "
	" ";

VS_CRITERIA VS_011_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_3_sw : mad source reg combination r0, v0, r1 is allowed";
string VS_011 = 
	"vs_3_sw "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, v0, r1 "
	" ";

VS_CRITERIA VS_012_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_3_sw : mad source reg combination r0, v0, c0 is allowed";
string VS_012 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, r0, v0, c0 "
	" ";

VS_CRITERIA VS_013_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_3_sw : mad source reg combination r0, r1, v0 is allowed";
string VS_013 = 
	"vs_3_sw "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, r1, v0 "
	" ";

VS_CRITERIA VS_014_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_3_sw : mad source reg combination r0, r1, r2 is allowed";
string VS_014 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"mad r0, r0, r1, r2 "
	" ";

VS_CRITERIA VS_015_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_3_sw : mad source reg combination r0, r1, c0 is allowed";
string VS_015 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, r1, c0 "
	" ";

VS_CRITERIA VS_016_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_3_sw : mad source reg combination r0, c0, v0 is allowed";
string VS_016 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, r0, c0, v0 "
	" ";

VS_CRITERIA VS_017_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_3_sw : mad source reg combination r0, c0, r1 is allowed";
string VS_017 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, c0, r1 "
	" ";

VS_CRITERIA VS_018_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_3_sw : mad source reg combination r0, c0, c1 is allowed";
string VS_018 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, r0, c0, c1 "
	" ";

VS_CRITERIA VS_019_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_3_sw : mad source reg combination c0, v0, v1 is allowed";
string VS_019 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mad r0, c0, v0, v1 "
	" ";

VS_CRITERIA VS_020_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_3_sw : mad source reg combination c0, v0, r0 is allowed";
string VS_020 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, c0, v0, r0 "
	" ";

VS_CRITERIA VS_021_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_3_sw : mad source reg combination c0, v0, c1 is allowed";
string VS_021 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mad r0, c0, v0, c1 "
	" ";

VS_CRITERIA VS_022_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_3_sw : mad source reg combination c0, r0, v0 is allowed";
string VS_022 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, c0, r0, v0 "
	" ";

VS_CRITERIA VS_023_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_3_sw : mad source reg combination c0, r0, r1 is allowed";
string VS_023 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1 "
	" ";

VS_CRITERIA VS_024_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_3_sw : mad source reg combination c0, r0, c1 is allowed";
string VS_024 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, r0, c1 "
	" ";

VS_CRITERIA VS_025_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_3_sw : mad source reg combination c0, c1, v0 is allowed";
string VS_025 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mad r0, c0, c1, v0 "
	" ";

VS_CRITERIA VS_026_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_3_sw : mad source reg combination c0, c1, r0 is allowed";
string VS_026 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0 "
	" ";

VS_CRITERIA VS_027_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_3_sw : mad source reg combination c0, c1, c2 is allowed";
string VS_027 = 
	"vs_3_sw "
	"def c2, 1, 1, 1, 1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mad r0, c0, c1, c2 "
	" ";

VS_CRITERIA VS_028_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_3_sw : mad source reg combination -c0, -c1, -r0 is allowed";
string VS_028 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, -c1, -r0 "
	" ";

VS_CRITERIA VS_029_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_3_sw : mad source reg combination -c0, -c1, r0_abs is allowed";
string VS_029 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, -c1, r0_abs "
	" ";

VS_CRITERIA VS_030_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_3_sw : mad source reg combination -c0, -c1, -r0_abs is allowed";
string VS_030 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, -c1, -r0_abs "
	" ";

VS_CRITERIA VS_031_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_3_sw : mad source reg combination -c0, -c1, r0 is allowed";
string VS_031 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, -c1, r0 "
	" ";

VS_CRITERIA VS_032_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_3_sw : mad source reg combination -c0, c1_abs, -r0 is allowed";
string VS_032 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, c1_abs, -r0 "
	" ";

VS_CRITERIA VS_033_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_3_sw : mad source reg combination -c0, c1_abs, r0_abs is allowed";
string VS_033 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, c1_abs, r0_abs "
	" ";

VS_CRITERIA VS_034_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_3_sw : mad source reg combination -c0, c1_abs, -r0_abs is allowed";
string VS_034 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, c1_abs, -r0_abs "
	" ";

VS_CRITERIA VS_035_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_3_sw : mad source reg combination -c0, c1_abs, r0 is allowed";
string VS_035 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, c1_abs, r0 "
	" ";

VS_CRITERIA VS_036_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_3_sw : mad source reg combination -c0, -c1_abs, -r0 is allowed";
string VS_036 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, -c1_abs, -r0 "
	" ";

VS_CRITERIA VS_037_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_3_sw : mad source reg combination -c0, -c1_abs, r0_abs is allowed";
string VS_037 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, -c1_abs, r0_abs "
	" ";

VS_CRITERIA VS_038_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_3_sw : mad source reg combination -c0, -c1_abs, -r0_abs is allowed";
string VS_038 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, -c1_abs, -r0_abs "
	" ";

VS_CRITERIA VS_039_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_3_sw : mad source reg combination -c0, -c1_abs, r0 is allowed";
string VS_039 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, -c1_abs, r0 "
	" ";

VS_CRITERIA VS_040_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_3_sw : mad source reg combination -c0, c1, -r0 is allowed";
string VS_040 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, c1, -r0 "
	" ";

VS_CRITERIA VS_041_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_3_sw : mad source reg combination -c0, c1, r0_abs is allowed";
string VS_041 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, c1, r0_abs "
	" ";

VS_CRITERIA VS_042_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_3_sw : mad source reg combination -c0, c1, -r0_abs is allowed";
string VS_042 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, c1, -r0_abs "
	" ";

VS_CRITERIA VS_043_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_3_sw : mad source reg combination -c0, c1, r0 is allowed";
string VS_043 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0, c1, r0 "
	" ";

VS_CRITERIA VS_044_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_3_sw : mad source reg combination c0_abs, -c1, -r0 is allowed";
string VS_044 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, -c1, -r0 "
	" ";

VS_CRITERIA VS_045_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_3_sw : mad source reg combination c0_abs, -c1, r0_abs is allowed";
string VS_045 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, -c1, r0_abs "
	" ";

VS_CRITERIA VS_046_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_3_sw : mad source reg combination c0_abs, -c1, -r0_abs is allowed";
string VS_046 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, -c1, -r0_abs "
	" ";

VS_CRITERIA VS_047_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_3_sw : mad source reg combination c0_abs, -c1, r0 is allowed";
string VS_047 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, -c1, r0 "
	" ";

VS_CRITERIA VS_048_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_3_sw : mad source reg combination c0_abs, c1_abs, -r0 is allowed";
string VS_048 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, c1_abs, -r0 "
	" ";

VS_CRITERIA VS_049_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_3_sw : mad source reg combination c0_abs, c1_abs, r0_abs is allowed";
string VS_049 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, c1_abs, r0_abs "
	" ";

VS_CRITERIA VS_050_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_3_sw : mad source reg combination c0_abs, c1_abs, -r0_abs is allowed";
string VS_050 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, c1_abs, -r0_abs "
	" ";

VS_CRITERIA VS_051_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_3_sw : mad source reg combination c0_abs, c1_abs, r0 is allowed";
string VS_051 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, c1_abs, r0 "
	" ";

VS_CRITERIA VS_052_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_3_sw : mad source reg combination c0_abs, -c1_abs, -r0 is allowed";
string VS_052 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, -c1_abs, -r0 "
	" ";

VS_CRITERIA VS_053_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_3_sw : mad source reg combination c0_abs, -c1_abs, r0_abs is allowed";
string VS_053 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, -c1_abs, r0_abs "
	" ";

VS_CRITERIA VS_054_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_3_sw : mad source reg combination c0_abs, -c1_abs, -r0_abs is allowed";
string VS_054 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, -c1_abs, -r0_abs "
	" ";

VS_CRITERIA VS_055_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_3_sw : mad source reg combination c0_abs, -c1_abs, r0 is allowed";
string VS_055 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, -c1_abs, r0 "
	" ";

VS_CRITERIA VS_056_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_3_sw : mad source reg combination c0_abs, c1, -r0 is allowed";
string VS_056 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, c1, -r0 "
	" ";

VS_CRITERIA VS_057_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_3_sw : mad source reg combination c0_abs, c1, r0_abs is allowed";
string VS_057 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, c1, r0_abs "
	" ";

VS_CRITERIA VS_058_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_3_sw : mad source reg combination c0_abs, c1, -r0_abs is allowed";
string VS_058 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, c1, -r0_abs "
	" ";

VS_CRITERIA VS_059_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_3_sw : mad source reg combination c0_abs, c1, r0 is allowed";
string VS_059 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0_abs, c1, r0 "
	" ";

VS_CRITERIA VS_060_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_3_sw : mad source reg combination -c0_abs, -c1, -r0 is allowed";
string VS_060 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, -c1, -r0 "
	" ";

VS_CRITERIA VS_061_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_3_sw : mad source reg combination -c0_abs, -c1, r0_abs is allowed";
string VS_061 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, -c1, r0_abs "
	" ";

VS_CRITERIA VS_062_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_3_sw : mad source reg combination -c0_abs, -c1, -r0_abs is allowed";
string VS_062 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, -c1, -r0_abs "
	" ";

VS_CRITERIA VS_063_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_3_sw : mad source reg combination -c0_abs, -c1, r0 is allowed";
string VS_063 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, -c1, r0 "
	" ";

VS_CRITERIA VS_064_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_3_sw : mad source reg combination -c0_abs, c1_abs, -r0 is allowed";
string VS_064 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, c1_abs, -r0 "
	" ";

VS_CRITERIA VS_065_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_3_sw : mad source reg combination -c0_abs, c1_abs, r0_abs is allowed";
string VS_065 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, c1_abs, r0_abs "
	" ";

VS_CRITERIA VS_066_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_3_sw : mad source reg combination -c0_abs, c1_abs, -r0_abs is allowed";
string VS_066 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, c1_abs, -r0_abs "
	" ";

VS_CRITERIA VS_067_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_3_sw : mad source reg combination -c0_abs, c1_abs, r0 is allowed";
string VS_067 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, c1_abs, r0 "
	" ";

VS_CRITERIA VS_068_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_3_sw : mad source reg combination -c0_abs, -c1_abs, -r0 is allowed";
string VS_068 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, -c1_abs, -r0 "
	" ";

VS_CRITERIA VS_069_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_3_sw : mad source reg combination -c0_abs, -c1_abs, r0_abs is allowed";
string VS_069 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, -c1_abs, r0_abs "
	" ";

VS_CRITERIA VS_070_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_3_sw : mad source reg combination -c0_abs, -c1_abs, -r0_abs is allowed";
string VS_070 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, -c1_abs, -r0_abs "
	" ";

VS_CRITERIA VS_071_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_3_sw : mad source reg combination -c0_abs, -c1_abs, r0 is allowed";
string VS_071 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, -c1_abs, r0 "
	" ";

VS_CRITERIA VS_072_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_3_sw : mad source reg combination -c0_abs, c1, -r0 is allowed";
string VS_072 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, c1, -r0 "
	" ";

VS_CRITERIA VS_073_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_3_sw : mad source reg combination -c0_abs, c1, r0_abs is allowed";
string VS_073 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, c1, r0_abs "
	" ";

VS_CRITERIA VS_074_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_3_sw : mad source reg combination -c0_abs, c1, -r0_abs is allowed";
string VS_074 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, c1, -r0_abs "
	" ";

VS_CRITERIA VS_075_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_3_sw : mad source reg combination -c0_abs, c1, r0 is allowed";
string VS_075 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, -c0_abs, c1, r0 "
	" ";

VS_CRITERIA VS_076_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_3_sw : mad source reg combination c0, -c1, -r0 is allowed";
string VS_076 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, -c1, -r0 "
	" ";

VS_CRITERIA VS_077_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_3_sw : mad source reg combination c0, -c1, r0_abs is allowed";
string VS_077 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, -c1, r0_abs "
	" ";

VS_CRITERIA VS_078_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_3_sw : mad source reg combination c0, -c1, -r0_abs is allowed";
string VS_078 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, -c1, -r0_abs "
	" ";

VS_CRITERIA VS_079_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_3_sw : mad source reg combination c0, -c1, r0 is allowed";
string VS_079 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, -c1, r0 "
	" ";

VS_CRITERIA VS_080_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_3_sw : mad source reg combination c0, c1_abs, -r0 is allowed";
string VS_080 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1_abs, -r0 "
	" ";

VS_CRITERIA VS_081_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_3_sw : mad source reg combination c0, c1_abs, r0_abs is allowed";
string VS_081 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1_abs, r0_abs "
	" ";

VS_CRITERIA VS_082_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_3_sw : mad source reg combination c0, c1_abs, -r0_abs is allowed";
string VS_082 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1_abs, -r0_abs "
	" ";

VS_CRITERIA VS_083_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_3_sw : mad source reg combination c0, c1_abs, r0 is allowed";
string VS_083 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1_abs, r0 "
	" ";

VS_CRITERIA VS_084_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_3_sw : mad source reg combination c0, -c1_abs, -r0 is allowed";
string VS_084 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, -c1_abs, -r0 "
	" ";

VS_CRITERIA VS_085_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_3_sw : mad source reg combination c0, -c1_abs, r0_abs is allowed";
string VS_085 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, -c1_abs, r0_abs "
	" ";

VS_CRITERIA VS_086_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_3_sw : mad source reg combination c0, -c1_abs, -r0_abs is allowed";
string VS_086 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, -c1_abs, -r0_abs "
	" ";

VS_CRITERIA VS_087_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_3_sw : mad source reg combination c0, -c1_abs, r0 is allowed";
string VS_087 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, -c1_abs, r0 "
	" ";

VS_CRITERIA VS_088_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_3_sw : mad source reg combination c0, c1, -r0 is allowed";
string VS_088 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, -r0 "
	" ";

VS_CRITERIA VS_089_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_089_Desc = "vs_3_sw : mad source reg combination c0, c1, r0_abs is allowed";
string VS_089 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0_abs "
	" ";

VS_CRITERIA VS_090_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_3_sw : mad source reg combination c0, c1, -r0_abs is allowed";
string VS_090 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, -r0_abs "
	" ";

VS_CRITERIA VS_091_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_3_sw : mad source reg combination c0, c1, r0 is allowed";
string VS_091 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0 "
	" ";

VS_CRITERIA VS_092_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_3_sw : mad source reg combination c0.x, c1, r0 is allowed";
string VS_092 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.x, c1, r0 "
	" ";

VS_CRITERIA VS_093_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_3_sw : mad source reg combination c0.y, c1, r0 is allowed";
string VS_093 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.y, c1, r0 "
	" ";

VS_CRITERIA VS_094_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_3_sw : mad source reg combination c0.z, c1, r0 is allowed";
string VS_094 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.z, c1, r0 "
	" ";

VS_CRITERIA VS_095_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_3_sw : mad source reg combination c0.w, c1, r0 is allowed";
string VS_095 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.w, c1, r0 "
	" ";

VS_CRITERIA VS_096_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_3_sw : mad source reg combination c0.yzxw, c1, r0 is allowed";
string VS_096 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.yzxw, c1, r0 "
	" ";

VS_CRITERIA VS_097_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_3_sw : mad source reg combination c0.zxyw, c1, r0 is allowed";
string VS_097 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.zxyw, c1, r0 "
	" ";

VS_CRITERIA VS_098_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_3_sw : mad source reg combination c0.wzyx, c1, r0 is allowed";
string VS_098 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.wzyx, c1, r0 "
	" ";

VS_CRITERIA VS_099_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_3_sw : mad source reg combination c0.wyxz, c1, r0 is allowed";
string VS_099 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.wyxz, c1, r0 "
	" ";

VS_CRITERIA VS_100_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_3_sw : mad source reg combination c0.xzyw, c1, r0 is allowed";
string VS_100 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.xzyw, c1, r0 "
	" ";

VS_CRITERIA VS_101_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_3_sw : mad source reg combination c0.xywz, c1, r0 is allowed";
string VS_101 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.xywz, c1, r0 "
	" ";

VS_CRITERIA VS_102_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_3_sw : mad source reg combination c0.zyx, c1, r0 is allowed";
string VS_102 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.zyx, c1, r0 "
	" ";

VS_CRITERIA VS_103_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_3_sw : mad source reg combination c0.xzy, c1, r0 is allowed";
string VS_103 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.xzy, c1, r0 "
	" ";

VS_CRITERIA VS_104_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_3_sw : mad source reg combination c0.ywx, c1, r0 is allowed";
string VS_104 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.ywx, c1, r0 "
	" ";

VS_CRITERIA VS_105_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_3_sw : mad source reg combination c0.yx, c1, r0 is allowed";
string VS_105 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.yx, c1, r0 "
	" ";

VS_CRITERIA VS_106_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_3_sw : mad source reg combination c0.wz, c1, r0 is allowed";
string VS_106 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.wz, c1, r0 "
	" ";

VS_CRITERIA VS_107_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_3_sw : mad source reg combination c0.zy, c1, r0 is allowed";
string VS_107 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0.zy, c1, r0 "
	" ";

VS_CRITERIA VS_108_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_3_sw : mad source reg combination c0, c1.x, r0 is allowed";
string VS_108 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.x, r0 "
	" ";

VS_CRITERIA VS_109_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_3_sw : mad source reg combination c0, c1.y, r0 is allowed";
string VS_109 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.y, r0 "
	" ";

VS_CRITERIA VS_110_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_3_sw : mad source reg combination c0, c1.z, r0 is allowed";
string VS_110 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.z, r0 "
	" ";

VS_CRITERIA VS_111_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_3_sw : mad source reg combination c0, c1.w, r0 is allowed";
string VS_111 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.w, r0 "
	" ";

VS_CRITERIA VS_112_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_3_sw : mad source reg combination c0, c1.yzxw, r0 is allowed";
string VS_112 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.yzxw, r0 "
	" ";

VS_CRITERIA VS_113_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_3_sw : mad source reg combination c0, c1.zxyw, r0 is allowed";
string VS_113 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.zxyw, r0 "
	" ";

VS_CRITERIA VS_114_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_3_sw : mad source reg combination c0, c1.wzyx, r0 is allowed";
string VS_114 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.wzyx, r0 "
	" ";

VS_CRITERIA VS_115_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_3_sw : mad source reg combination c0, c1.wyxz, r0 is allowed";
string VS_115 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.wyxz, r0 "
	" ";

VS_CRITERIA VS_116_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_3_sw : mad source reg combination c0, c1.xzyw, r0 is allowed";
string VS_116 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.xzyw, r0 "
	" ";

VS_CRITERIA VS_117_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_3_sw : mad source reg combination c0, c1.xywz, r0 is allowed";
string VS_117 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.xywz, r0 "
	" ";

VS_CRITERIA VS_118_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_3_sw : mad source reg combination c0, c1.zyx, r0 is allowed";
string VS_118 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.zyx, r0 "
	" ";

VS_CRITERIA VS_119_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_3_sw : mad source reg combination c0, c1.xzy, r0 is allowed";
string VS_119 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.xzy, r0 "
	" ";

VS_CRITERIA VS_120_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_3_sw : mad source reg combination c0, c1.ywx, r0 is allowed";
string VS_120 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.ywx, r0 "
	" ";

VS_CRITERIA VS_121_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_3_sw : mad source reg combination c0, c1.yx, r0 is allowed";
string VS_121 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.yx, r0 "
	" ";

VS_CRITERIA VS_122_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_3_sw : mad source reg combination c0, c1.wz, r0 is allowed";
string VS_122 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.wz, r0 "
	" ";

VS_CRITERIA VS_123_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_3_sw : mad source reg combination c0, c1.zy, r0 is allowed";
string VS_123 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1.zy, r0 "
	" ";

VS_CRITERIA VS_124_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.x is allowed";
string VS_124 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.x "
	" ";

VS_CRITERIA VS_125_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.y is allowed";
string VS_125 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.y "
	" ";

VS_CRITERIA VS_126_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.z is allowed";
string VS_126 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.z "
	" ";

VS_CRITERIA VS_127_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.w is allowed";
string VS_127 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.w "
	" ";

VS_CRITERIA VS_128_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.yzxw is allowed";
string VS_128 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.yzxw "
	" ";

VS_CRITERIA VS_129_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.zxyw is allowed";
string VS_129 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.zxyw "
	" ";

VS_CRITERIA VS_130_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_130_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.wzyx is allowed";
string VS_130 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.wzyx "
	" ";

VS_CRITERIA VS_131_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.wyxz is allowed";
string VS_131 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.wyxz "
	" ";

VS_CRITERIA VS_132_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.xzyw is allowed";
string VS_132 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.xzyw "
	" ";

VS_CRITERIA VS_133_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.xywz is allowed";
string VS_133 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.xywz "
	" ";

VS_CRITERIA VS_134_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.zyx is allowed";
string VS_134 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.zyx "
	" ";

VS_CRITERIA VS_135_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.xzy is allowed";
string VS_135 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.xzy "
	" ";

VS_CRITERIA VS_136_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.ywx is allowed";
string VS_136 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.ywx "
	" ";

VS_CRITERIA VS_137_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.yx is allowed";
string VS_137 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.yx "
	" ";

VS_CRITERIA VS_138_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.wz is allowed";
string VS_138 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.wz "
	" ";

VS_CRITERIA VS_139_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_3_sw : mad source reg combination c0, c1, r0.zy is allowed";
string VS_139 = 
	"vs_3_sw "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0.zy "
	" ";

VS_CRITERIA VS_140_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_3_sw : mad source reg combination a0, a0, a0 is NOT allowed";
string VS_140 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, a0 "
	" ";

VS_CRITERIA VS_141_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_3_sw : mad source reg combination a0, a0, b0 is NOT allowed";
string VS_141 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, b0 "
	" ";

VS_CRITERIA VS_142_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_3_sw : mad source reg combination a0, a0, i0 is NOT allowed";
string VS_142 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, i0 "
	" ";

VS_CRITERIA VS_143_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_3_sw : mad source reg combination a0, a0, aL is NOT allowed";
string VS_143 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, a0, aL "
	" ";

VS_CRITERIA VS_144_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_3_sw : mad source reg combination a0, a0, p0 is NOT allowed";
string VS_144 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, p0 "
	" ";

VS_CRITERIA VS_145_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_3_sw : mad source reg combination a0, a0, s0 is NOT allowed";
string VS_145 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, s0 "
	" ";

VS_CRITERIA VS_146_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_3_sw : mad source reg combination a0, a0, o0 is NOT allowed";
string VS_146 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, o0 "
	" ";

VS_CRITERIA VS_147_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_3_sw : mad source reg combination a0, b0, a0 is NOT allowed";
string VS_147 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, a0 "
	" ";

VS_CRITERIA VS_148_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_3_sw : mad source reg combination a0, b0, b0 is NOT allowed";
string VS_148 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, b0 "
	" ";

VS_CRITERIA VS_149_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_149_Desc = "vs_3_sw : mad source reg combination a0, b0, i0 is NOT allowed";
string VS_149 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, i0 "
	" ";

VS_CRITERIA VS_150_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_3_sw : mad source reg combination a0, b0, aL is NOT allowed";
string VS_150 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, b0, aL "
	" ";

VS_CRITERIA VS_151_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_3_sw : mad source reg combination a0, b0, p0 is NOT allowed";
string VS_151 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, p0 "
	" ";

VS_CRITERIA VS_152_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_3_sw : mad source reg combination a0, b0, s0 is NOT allowed";
string VS_152 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_volume s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, s0 "
	" ";

VS_CRITERIA VS_153_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_3_sw : mad source reg combination a0, b0, o0 is NOT allowed";
string VS_153 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, o0 "
	" ";

VS_CRITERIA VS_154_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_3_sw : mad source reg combination a0, i0, a0 is NOT allowed";
string VS_154 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, a0 "
	" ";

VS_CRITERIA VS_155_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_3_sw : mad source reg combination a0, i0, b0 is NOT allowed";
string VS_155 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, b0 "
	" ";

VS_CRITERIA VS_156_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_156_Desc = "vs_3_sw : mad source reg combination a0, i0, i0 is NOT allowed";
string VS_156 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, i0 "
	" ";

VS_CRITERIA VS_157_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_3_sw : mad source reg combination a0, i0, aL is NOT allowed";
string VS_157 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, i0, aL "
	" ";

VS_CRITERIA VS_158_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_3_sw : mad source reg combination a0, i0, p0 is NOT allowed";
string VS_158 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, p0 "
	" ";

VS_CRITERIA VS_159_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_159_Desc = "vs_3_sw : mad source reg combination a0, i0, s0 is NOT allowed";
string VS_159 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, s0 "
	" ";

VS_CRITERIA VS_160_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_3_sw : mad source reg combination a0, i0, o0 is NOT allowed";
string VS_160 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, o0 "
	" ";

VS_CRITERIA VS_161_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_3_sw : mad source reg combination a0, aL, a0 is NOT allowed";
string VS_161 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, a0 "
	" ";

VS_CRITERIA VS_162_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_3_sw : mad source reg combination a0, aL, b0 is NOT allowed";
string VS_162 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, b0 "
	" ";

VS_CRITERIA VS_163_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_3_sw : mad source reg combination a0, aL, i0 is NOT allowed";
string VS_163 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, i0 "
	" ";

VS_CRITERIA VS_164_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_3_sw : mad source reg combination a0, aL, aL is NOT allowed";
string VS_164 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, aL "
	" ";

VS_CRITERIA VS_165_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_3_sw : mad source reg combination a0, aL, p0 is NOT allowed";
string VS_165 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, p0 "
	" ";

VS_CRITERIA VS_166_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_3_sw : mad source reg combination a0, aL, s0 is NOT allowed";
string VS_166 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, s0 "
	" ";

VS_CRITERIA VS_167_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_3_sw : mad source reg combination a0, aL, o0 is NOT allowed";
string VS_167 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, o0 "
	" ";

VS_CRITERIA VS_168_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_168_Desc = "vs_3_sw : mad source reg combination a0, p0, a0 is NOT allowed";
string VS_168 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, a0 "
	" ";

VS_CRITERIA VS_169_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_169_Desc = "vs_3_sw : mad source reg combination a0, p0, b0 is NOT allowed";
string VS_169 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, b0 "
	" ";

VS_CRITERIA VS_170_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_170_Desc = "vs_3_sw : mad source reg combination a0, p0, i0 is NOT allowed";
string VS_170 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, i0 "
	" ";

VS_CRITERIA VS_171_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_171_Desc = "vs_3_sw : mad source reg combination a0, p0, aL is NOT allowed";
string VS_171 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, p0, aL "
	" ";

VS_CRITERIA VS_172_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_172_Desc = "vs_3_sw : mad source reg combination a0, p0, p0 is NOT allowed";
string VS_172 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, p0 "
	" ";

VS_CRITERIA VS_173_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_173_Desc = "vs_3_sw : mad source reg combination a0, p0, s0 is NOT allowed";
string VS_173 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, s0 "
	" ";

VS_CRITERIA VS_174_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_174_Desc = "vs_3_sw : mad source reg combination a0, p0, o0 is NOT allowed";
string VS_174 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, o0 "
	" ";

VS_CRITERIA VS_175_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_175_Desc = "vs_3_sw : mad source reg combination a0, s0, a0 is NOT allowed";
string VS_175 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, s0, a0 "
	" ";

VS_CRITERIA VS_176_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_176_Desc = "vs_3_sw : mad source reg combination a0, s0, b0 is NOT allowed";
string VS_176 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, s0, b0 "
	" ";

VS_CRITERIA VS_177_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_177_Desc = "vs_3_sw : mad source reg combination a0, s0, i0 is NOT allowed";
string VS_177 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, s0, i0 "
	" ";

VS_CRITERIA VS_178_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_178_Desc = "vs_3_sw : mad source reg combination a0, s0, aL is NOT allowed";
string VS_178 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, s0, aL "
	" ";

VS_CRITERIA VS_179_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_179_Desc = "vs_3_sw : mad source reg combination a0, s0, p0 is NOT allowed";
string VS_179 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, s0, p0 "
	" ";

VS_CRITERIA VS_180_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_180_Desc = "vs_3_sw : mad source reg combination a0, s0, s0 is NOT allowed";
string VS_180 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, s0, s0 "
	" ";

VS_CRITERIA VS_181_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_181_Desc = "vs_3_sw : mad source reg combination a0, s0, o0 is NOT allowed";
string VS_181 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, s0, o0 "
	" ";

VS_CRITERIA VS_182_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_182_Desc = "vs_3_sw : mad source reg combination a0, o0, a0 is NOT allowed";
string VS_182 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, a0, o0, a0 "
	" ";

VS_CRITERIA VS_183_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_183_Desc = "vs_3_sw : mad source reg combination a0, o0, b0 is NOT allowed";
string VS_183 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, o0, b0 "
	" ";

VS_CRITERIA VS_184_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_184_Desc = "vs_3_sw : mad source reg combination a0, o0, i0 is NOT allowed";
string VS_184 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, o0, i0 "
	" ";

VS_CRITERIA VS_185_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_185_Desc = "vs_3_sw : mad source reg combination a0, o0, aL is NOT allowed";
string VS_185 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, o0, aL "
	" ";

VS_CRITERIA VS_186_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_186_Desc = "vs_3_sw : mad source reg combination a0, o0, p0 is NOT allowed";
string VS_186 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, a0, o0, p0 "
	" ";

VS_CRITERIA VS_187_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_187_Desc = "vs_3_sw : mad source reg combination a0, o0, s0 is NOT allowed";
string VS_187 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, a0, o0, s0 "
	" ";

VS_CRITERIA VS_188_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_188_Desc = "vs_3_sw : mad source reg combination a0, o0, o0 is NOT allowed";
string VS_188 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, a0, o0, o0 "
	" ";

VS_CRITERIA VS_189_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_189_Desc = "vs_3_sw : mad source reg combination b0, a0, a0 is NOT allowed";
string VS_189 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, a0 "
	" ";

VS_CRITERIA VS_190_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_190_Desc = "vs_3_sw : mad source reg combination b0, a0, b0 is NOT allowed";
string VS_190 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, b0 "
	" ";

VS_CRITERIA VS_191_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_191_Desc = "vs_3_sw : mad source reg combination b0, a0, i0 is NOT allowed";
string VS_191 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, i0 "
	" ";

VS_CRITERIA VS_192_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_192_Desc = "vs_3_sw : mad source reg combination b0, a0, aL is NOT allowed";
string VS_192 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, b0, a0, aL "
	" ";

VS_CRITERIA VS_193_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_193_Desc = "vs_3_sw : mad source reg combination b0, a0, p0 is NOT allowed";
string VS_193 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, p0 "
	" ";

VS_CRITERIA VS_194_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_194_Desc = "vs_3_sw : mad source reg combination b0, a0, s0 is NOT allowed";
string VS_194 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, s0 "
	" ";

VS_CRITERIA VS_195_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_195_Desc = "vs_3_sw : mad source reg combination b0, a0, o0 is NOT allowed";
string VS_195 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, o0 "
	" ";

VS_CRITERIA VS_196_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_196_Desc = "vs_3_sw : mad source reg combination b0, b0, a0 is NOT allowed";
string VS_196 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, b0, a0 "
	" ";

VS_CRITERIA VS_197_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_197_Desc = "vs_3_sw : mad source reg combination b0, b0, b0 is NOT allowed";
string VS_197 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, b0, b0, b0 "
	" ";

VS_CRITERIA VS_198_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_198_Desc = "vs_3_sw : mad source reg combination b0, b0, i0 is NOT allowed";
string VS_198 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, b0, i0 "
	" ";

VS_CRITERIA VS_199_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_199_Desc = "vs_3_sw : mad source reg combination b0, b0, aL is NOT allowed";
string VS_199 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, b0, aL "
	" ";

VS_CRITERIA VS_200_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_200_Desc = "vs_3_sw : mad source reg combination b0, b0, p0 is NOT allowed";
string VS_200 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, b0, b0, p0 "
	" ";

VS_CRITERIA VS_201_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_201_Desc = "vs_3_sw : mad source reg combination b0, b0, s0 is NOT allowed";
string VS_201 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, b0, b0, s0 "
	" ";

VS_CRITERIA VS_202_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_202_Desc = "vs_3_sw : mad source reg combination b0, b0, o0 is NOT allowed";
string VS_202 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, b0, b0, o0 "
	" ";

VS_CRITERIA VS_203_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_203_Desc = "vs_3_sw : mad source reg combination b0, i0, a0 is NOT allowed";
string VS_203 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, i0, a0 "
	" ";

VS_CRITERIA VS_204_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_204_Desc = "vs_3_sw : mad source reg combination b0, i0, b0 is NOT allowed";
string VS_204 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, b0 "
	" ";

VS_CRITERIA VS_205_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_205_Desc = "vs_3_sw : mad source reg combination b0, i0, i0 is NOT allowed";
string VS_205 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, i0 "
	" ";

VS_CRITERIA VS_206_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_206_Desc = "vs_3_sw : mad source reg combination b0, i0, aL is NOT allowed";
string VS_206 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, i0, aL "
	" ";

VS_CRITERIA VS_207_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_207_Desc = "vs_3_sw : mad source reg combination b0, i0, p0 is NOT allowed";
string VS_207 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, p0 "
	" ";

VS_CRITERIA VS_208_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_208_Desc = "vs_3_sw : mad source reg combination b0, i0, s0 is NOT allowed";
string VS_208 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, i0, s0 "
	" ";

VS_CRITERIA VS_209_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_209_Desc = "vs_3_sw : mad source reg combination b0, i0, o0 is NOT allowed";
string VS_209 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, o0 "
	" ";

VS_CRITERIA VS_210_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_210_Desc = "vs_3_sw : mad source reg combination b0, aL, a0 is NOT allowed";
string VS_210 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, b0, aL, a0 "
	" ";

VS_CRITERIA VS_211_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_211_Desc = "vs_3_sw : mad source reg combination b0, aL, b0 is NOT allowed";
string VS_211 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, b0 "
	" ";

VS_CRITERIA VS_212_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_212_Desc = "vs_3_sw : mad source reg combination b0, aL, i0 is NOT allowed";
string VS_212 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, i0 "
	" ";

VS_CRITERIA VS_213_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_213_Desc = "vs_3_sw : mad source reg combination b0, aL, aL is NOT allowed";
string VS_213 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, aL "
	" ";

VS_CRITERIA VS_214_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_214_Desc = "vs_3_sw : mad source reg combination b0, aL, p0 is NOT allowed";
string VS_214 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, p0 "
	" ";

VS_CRITERIA VS_215_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_215_Desc = "vs_3_sw : mad source reg combination b0, aL, s0 is NOT allowed";
string VS_215 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, s0 "
	" ";

VS_CRITERIA VS_216_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_216_Desc = "vs_3_sw : mad source reg combination b0, aL, o0 is NOT allowed";
string VS_216 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, o0 "
	" ";

VS_CRITERIA VS_217_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_217_Desc = "vs_3_sw : mad source reg combination b0, p0, a0 is NOT allowed";
string VS_217 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, p0, a0 "
	" ";

VS_CRITERIA VS_218_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_218_Desc = "vs_3_sw : mad source reg combination b0, p0, b0 is NOT allowed";
string VS_218 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, b0, p0, b0 "
	" ";

VS_CRITERIA VS_219_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_219_Desc = "vs_3_sw : mad source reg combination b0, p0, i0 is NOT allowed";
string VS_219 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, p0, i0 "
	" ";

VS_CRITERIA VS_220_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_220_Desc = "vs_3_sw : mad source reg combination b0, p0, aL is NOT allowed";
string VS_220 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, p0, aL "
	" ";

VS_CRITERIA VS_221_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_221_Desc = "vs_3_sw : mad source reg combination b0, p0, p0 is NOT allowed";
string VS_221 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, b0, p0, p0 "
	" ";

VS_CRITERIA VS_222_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_222_Desc = "vs_3_sw : mad source reg combination b0, p0, s0 is NOT allowed";
string VS_222 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, p0, s0 "
	" ";

VS_CRITERIA VS_223_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_223_Desc = "vs_3_sw : mad source reg combination b0, p0, o0 is NOT allowed";
string VS_223 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, b0, p0, o0 "
	" ";

VS_CRITERIA VS_224_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_224_Desc = "vs_3_sw : mad source reg combination b0, s0, a0 is NOT allowed";
string VS_224 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mova a0.x, c0.x "
	"mad r0, b0, s0, a0 "
	" ";

VS_CRITERIA VS_225_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_225_Desc = "vs_3_sw : mad source reg combination b0, s0, b0 is NOT allowed";
string VS_225 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, b0 "
	" ";

VS_CRITERIA VS_226_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_226_Desc = "vs_3_sw : mad source reg combination b0, s0, i0 is NOT allowed";
string VS_226 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, i0 "
	" ";

VS_CRITERIA VS_227_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_227_Desc = "vs_3_sw : mad source reg combination b0, s0, aL is NOT allowed";
string VS_227 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, b0, s0, aL "
	" ";

VS_CRITERIA VS_228_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_228_Desc = "vs_3_sw : mad source reg combination b0, s0, p0 is NOT allowed";
string VS_228 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, p0 "
	" ";

VS_CRITERIA VS_229_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_229_Desc = "vs_3_sw : mad source reg combination b0, s0, s0 is NOT allowed";
string VS_229 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, s0 "
	" ";

VS_CRITERIA VS_230_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_230_Desc = "vs_3_sw : mad source reg combination b0, s0, o0 is NOT allowed";
string VS_230 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, s0, o0 "
	" ";

VS_CRITERIA VS_231_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_231_Desc = "vs_3_sw : mad source reg combination b0, o0, a0 is NOT allowed";
string VS_231 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, o0, a0 "
	" ";

VS_CRITERIA VS_232_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_232_Desc = "vs_3_sw : mad source reg combination b0, o0, b0 is NOT allowed";
string VS_232 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, b0, o0, b0 "
	" ";

VS_CRITERIA VS_233_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_233_Desc = "vs_3_sw : mad source reg combination b0, o0, i0 is NOT allowed";
string VS_233 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, o0, i0 "
	" ";

VS_CRITERIA VS_234_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_234_Desc = "vs_3_sw : mad source reg combination b0, o0, aL is NOT allowed";
string VS_234 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, o0, aL "
	" ";

VS_CRITERIA VS_235_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_235_Desc = "vs_3_sw : mad source reg combination b0, o0, p0 is NOT allowed";
string VS_235 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, b0, o0, p0 "
	" ";

VS_CRITERIA VS_236_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_236_Desc = "vs_3_sw : mad source reg combination b0, o0, s0 is NOT allowed";
string VS_236 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, b0, o0, s0 "
	" ";

VS_CRITERIA VS_237_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_237_Desc = "vs_3_sw : mad source reg combination b0, o0, o0 is NOT allowed";
string VS_237 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, b0, o0, o0 "
	" ";

VS_CRITERIA VS_238_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_238_Desc = "vs_3_sw : mad source reg combination i0, a0, a0 is NOT allowed";
string VS_238 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, a0 "
	" ";

VS_CRITERIA VS_239_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_239_Desc = "vs_3_sw : mad source reg combination i0, a0, b0 is NOT allowed";
string VS_239 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, b0 "
	" ";

VS_CRITERIA VS_240_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_240_Desc = "vs_3_sw : mad source reg combination i0, a0, i0 is NOT allowed";
string VS_240 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, i0 "
	" ";

VS_CRITERIA VS_241_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_241_Desc = "vs_3_sw : mad source reg combination i0, a0, aL is NOT allowed";
string VS_241 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, i0, a0, aL "
	" ";

VS_CRITERIA VS_242_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_242_Desc = "vs_3_sw : mad source reg combination i0, a0, p0 is NOT allowed";
string VS_242 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, p0 "
	" ";

VS_CRITERIA VS_243_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_243_Desc = "vs_3_sw : mad source reg combination i0, a0, s0 is NOT allowed";
string VS_243 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, s0 "
	" ";

VS_CRITERIA VS_244_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_244_Desc = "vs_3_sw : mad source reg combination i0, a0, o0 is NOT allowed";
string VS_244 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, o0 "
	" ";

VS_CRITERIA VS_245_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_245_Desc = "vs_3_sw : mad source reg combination i0, b0, a0 is NOT allowed";
string VS_245 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, b0, a0 "
	" ";

VS_CRITERIA VS_246_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_246_Desc = "vs_3_sw : mad source reg combination i0, b0, b0 is NOT allowed";
string VS_246 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, b0 "
	" ";

VS_CRITERIA VS_247_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_247_Desc = "vs_3_sw : mad source reg combination i0, b0, i0 is NOT allowed";
string VS_247 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, i0 "
	" ";

VS_CRITERIA VS_248_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_248_Desc = "vs_3_sw : mad source reg combination i0, b0, aL is NOT allowed";
string VS_248 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, b0, aL "
	" ";

VS_CRITERIA VS_249_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_249_Desc = "vs_3_sw : mad source reg combination i0, b0, p0 is NOT allowed";
string VS_249 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, p0 "
	" ";

VS_CRITERIA VS_250_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_250_Desc = "vs_3_sw : mad source reg combination i0, b0, s0 is NOT allowed";
string VS_250 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, i0, b0, s0 "
	" ";

VS_CRITERIA VS_251_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_251_Desc = "vs_3_sw : mad source reg combination i0, b0, o0 is NOT allowed";
string VS_251 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, o0 "
	" ";

VS_CRITERIA VS_252_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_252_Desc = "vs_3_sw : mad source reg combination i0, i0, a0 is NOT allowed";
string VS_252 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, i0, a0 "
	" ";

VS_CRITERIA VS_253_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_253_Desc = "vs_3_sw : mad source reg combination i0, i0, b0 is NOT allowed";
string VS_253 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, b0 "
	" ";

VS_CRITERIA VS_254_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_254_Desc = "vs_3_sw : mad source reg combination i0, i0, i0 is NOT allowed";
string VS_254 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, i0 "
	" ";

VS_CRITERIA VS_255_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_255_Desc = "vs_3_sw : mad source reg combination i0, i0, aL is NOT allowed";
string VS_255 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, i0, aL "
	" ";

VS_CRITERIA VS_256_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_256_Desc = "vs_3_sw : mad source reg combination i0, i0, p0 is NOT allowed";
string VS_256 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, p0 "
	" ";

VS_CRITERIA VS_257_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_257_Desc = "vs_3_sw : mad source reg combination i0, i0, s0 is NOT allowed";
string VS_257 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, i0, i0, s0 "
	" ";

VS_CRITERIA VS_258_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_258_Desc = "vs_3_sw : mad source reg combination i0, i0, o0 is NOT allowed";
string VS_258 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, o0 "
	" ";

VS_CRITERIA VS_259_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_259_Desc = "vs_3_sw : mad source reg combination i0, aL, a0 is NOT allowed";
string VS_259 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, i0, aL, a0 "
	" ";

VS_CRITERIA VS_260_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_260_Desc = "vs_3_sw : mad source reg combination i0, aL, b0 is NOT allowed";
string VS_260 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, b0 "
	" ";

VS_CRITERIA VS_261_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_261_Desc = "vs_3_sw : mad source reg combination i0, aL, i0 is NOT allowed";
string VS_261 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, i0 "
	" ";

VS_CRITERIA VS_262_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_262_Desc = "vs_3_sw : mad source reg combination i0, aL, aL is NOT allowed";
string VS_262 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, aL "
	" ";

VS_CRITERIA VS_263_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_263_Desc = "vs_3_sw : mad source reg combination i0, aL, p0 is NOT allowed";
string VS_263 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, p0 "
	" ";

VS_CRITERIA VS_264_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_264_Desc = "vs_3_sw : mad source reg combination i0, aL, s0 is NOT allowed";
string VS_264 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, s0 "
	" ";

VS_CRITERIA VS_265_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_265_Desc = "vs_3_sw : mad source reg combination i0, aL, o0 is NOT allowed";
string VS_265 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, o0 "
	" ";

VS_CRITERIA VS_266_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_266_Desc = "vs_3_sw : mad source reg combination i0, p0, a0 is NOT allowed";
string VS_266 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, p0, a0 "
	" ";

VS_CRITERIA VS_267_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_267_Desc = "vs_3_sw : mad source reg combination i0, p0, b0 is NOT allowed";
string VS_267 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, b0 "
	" ";

VS_CRITERIA VS_268_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_268_Desc = "vs_3_sw : mad source reg combination i0, p0, i0 is NOT allowed";
string VS_268 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, i0 "
	" ";

VS_CRITERIA VS_269_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_269_Desc = "vs_3_sw : mad source reg combination i0, p0, aL is NOT allowed";
string VS_269 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, p0, aL "
	" ";

VS_CRITERIA VS_270_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_270_Desc = "vs_3_sw : mad source reg combination i0, p0, p0 is NOT allowed";
string VS_270 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, p0 "
	" ";

VS_CRITERIA VS_271_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_271_Desc = "vs_3_sw : mad source reg combination i0, p0, s0 is NOT allowed";
string VS_271 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, i0, p0, s0 "
	" ";

VS_CRITERIA VS_272_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_272_Desc = "vs_3_sw : mad source reg combination i0, p0, o0 is NOT allowed";
string VS_272 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, o0 "
	" ";

VS_CRITERIA VS_273_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_273_Desc = "vs_3_sw : mad source reg combination i0, s0, a0 is NOT allowed";
string VS_273 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, i0, s0, a0 "
	" ";

VS_CRITERIA VS_274_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_274_Desc = "vs_3_sw : mad source reg combination i0, s0, b0 is NOT allowed";
string VS_274 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, i0, s0, b0 "
	" ";

VS_CRITERIA VS_275_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_275_Desc = "vs_3_sw : mad source reg combination i0, s0, i0 is NOT allowed";
string VS_275 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, i0, s0, i0 "
	" ";

VS_CRITERIA VS_276_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_276_Desc = "vs_3_sw : mad source reg combination i0, s0, aL is NOT allowed";
string VS_276 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, i0, s0, aL "
	" ";

VS_CRITERIA VS_277_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_277_Desc = "vs_3_sw : mad source reg combination i0, s0, p0 is NOT allowed";
string VS_277 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, i0, s0, p0 "
	" ";

VS_CRITERIA VS_278_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_278_Desc = "vs_3_sw : mad source reg combination i0, s0, s0 is NOT allowed";
string VS_278 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, i0, s0, s0 "
	" ";

VS_CRITERIA VS_279_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_279_Desc = "vs_3_sw : mad source reg combination i0, s0, o0 is NOT allowed";
string VS_279 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, i0, s0, o0 "
	" ";

VS_CRITERIA VS_280_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_280_Desc = "vs_3_sw : mad source reg combination i0, o0, a0 is NOT allowed";
string VS_280 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, o0, a0 "
	" ";

VS_CRITERIA VS_281_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_281_Desc = "vs_3_sw : mad source reg combination i0, o0, b0 is NOT allowed";
string VS_281 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, o0, b0 "
	" ";

VS_CRITERIA VS_282_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_282_Desc = "vs_3_sw : mad source reg combination i0, o0, i0 is NOT allowed";
string VS_282 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, o0, i0 "
	" ";

VS_CRITERIA VS_283_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_283_Desc = "vs_3_sw : mad source reg combination i0, o0, aL is NOT allowed";
string VS_283 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, o0, aL "
	" ";

VS_CRITERIA VS_284_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_284_Desc = "vs_3_sw : mad source reg combination i0, o0, p0 is NOT allowed";
string VS_284 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, o0, p0 "
	" ";

VS_CRITERIA VS_285_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_285_Desc = "vs_3_sw : mad source reg combination i0, o0, s0 is NOT allowed";
string VS_285 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, i0, o0, s0 "
	" ";

VS_CRITERIA VS_286_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_286_Desc = "vs_3_sw : mad source reg combination i0, o0, o0 is NOT allowed";
string VS_286 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, o0, o0 "
	" ";

VS_CRITERIA VS_287_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_287_Desc = "vs_3_sw : mad source reg combination aL, a0, a0 is NOT allowed";
string VS_287 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, a0 "
	" ";

VS_CRITERIA VS_288_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_288_Desc = "vs_3_sw : mad source reg combination aL, a0, b0 is NOT allowed";
string VS_288 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, b0 "
	" ";

VS_CRITERIA VS_289_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_289_Desc = "vs_3_sw : mad source reg combination aL, a0, i0 is NOT allowed";
string VS_289 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, i0 "
	" ";

VS_CRITERIA VS_290_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_290_Desc = "vs_3_sw : mad source reg combination aL, a0, aL is NOT allowed";
string VS_290 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, aL "
	" ";

VS_CRITERIA VS_291_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_291_Desc = "vs_3_sw : mad source reg combination aL, a0, p0 is NOT allowed";
string VS_291 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, p0 "
	" ";

VS_CRITERIA VS_292_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_292_Desc = "vs_3_sw : mad source reg combination aL, a0, s0 is NOT allowed";
string VS_292 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, s0 "
	" ";

VS_CRITERIA VS_293_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_293_Desc = "vs_3_sw : mad source reg combination aL, a0, o0 is NOT allowed";
string VS_293 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, o0 "
	" ";

VS_CRITERIA VS_294_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_294_Desc = "vs_3_sw : mad source reg combination aL, b0, a0 is NOT allowed";
string VS_294 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, b0, a0 "
	" ";

VS_CRITERIA VS_295_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_295_Desc = "vs_3_sw : mad source reg combination aL, b0, b0 is NOT allowed";
string VS_295 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, b0 "
	" ";

VS_CRITERIA VS_296_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_296_Desc = "vs_3_sw : mad source reg combination aL, b0, i0 is NOT allowed";
string VS_296 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, i0 "
	" ";

VS_CRITERIA VS_297_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_297_Desc = "vs_3_sw : mad source reg combination aL, b0, aL is NOT allowed";
string VS_297 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, aL "
	" ";

VS_CRITERIA VS_298_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_298_Desc = "vs_3_sw : mad source reg combination aL, b0, p0 is NOT allowed";
string VS_298 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, p0 "
	" ";

VS_CRITERIA VS_299_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_299_Desc = "vs_3_sw : mad source reg combination aL, b0, s0 is NOT allowed";
string VS_299 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_volume s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, s0 "
	" ";

VS_CRITERIA VS_300_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_300_Desc = "vs_3_sw : mad source reg combination aL, b0, o0 is NOT allowed";
string VS_300 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, o0 "
	" ";

VS_CRITERIA VS_301_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_301_Desc = "vs_3_sw : mad source reg combination aL, i0, a0 is NOT allowed";
string VS_301 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, i0, a0 "
	" ";

VS_CRITERIA VS_302_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_302_Desc = "vs_3_sw : mad source reg combination aL, i0, b0 is NOT allowed";
string VS_302 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, b0 "
	" ";

VS_CRITERIA VS_303_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_303_Desc = "vs_3_sw : mad source reg combination aL, i0, i0 is NOT allowed";
string VS_303 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, i0 "
	" ";

VS_CRITERIA VS_304_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_304_Desc = "vs_3_sw : mad source reg combination aL, i0, aL is NOT allowed";
string VS_304 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, aL "
	" ";

VS_CRITERIA VS_305_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_305_Desc = "vs_3_sw : mad source reg combination aL, i0, p0 is NOT allowed";
string VS_305 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, p0 "
	" ";

VS_CRITERIA VS_306_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_306_Desc = "vs_3_sw : mad source reg combination aL, i0, s0 is NOT allowed";
string VS_306 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, s0 "
	" ";

VS_CRITERIA VS_307_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_307_Desc = "vs_3_sw : mad source reg combination aL, i0, o0 is NOT allowed";
string VS_307 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, o0 "
	" ";

VS_CRITERIA VS_308_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_308_Desc = "vs_3_sw : mad source reg combination aL, aL, a0 is NOT allowed";
string VS_308 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, aL, a0 "
	" ";

VS_CRITERIA VS_309_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_309_Desc = "vs_3_sw : mad source reg combination aL, aL, b0 is NOT allowed";
string VS_309 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, b0 "
	" ";

VS_CRITERIA VS_310_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_310_Desc = "vs_3_sw : mad source reg combination aL, aL, i0 is NOT allowed";
string VS_310 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, i0 "
	" ";

VS_CRITERIA VS_311_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_311_Desc = "vs_3_sw : mad source reg combination aL, aL, aL is NOT allowed";
string VS_311 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, aL "
	" ";

VS_CRITERIA VS_312_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_312_Desc = "vs_3_sw : mad source reg combination aL, aL, p0 is NOT allowed";
string VS_312 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, p0 "
	" ";

VS_CRITERIA VS_313_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_313_Desc = "vs_3_sw : mad source reg combination aL, aL, s0 is NOT allowed";
string VS_313 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, s0 "
	" ";

VS_CRITERIA VS_314_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_314_Desc = "vs_3_sw : mad source reg combination aL, aL, o0 is NOT allowed";
string VS_314 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, o0 "
	" ";

VS_CRITERIA VS_315_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_315_Desc = "vs_3_sw : mad source reg combination aL, p0, a0 is NOT allowed";
string VS_315 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, p0, a0 "
	" ";

VS_CRITERIA VS_316_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_316_Desc = "vs_3_sw : mad source reg combination aL, p0, b0 is NOT allowed";
string VS_316 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, b0 "
	" ";

VS_CRITERIA VS_317_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_317_Desc = "vs_3_sw : mad source reg combination aL, p0, i0 is NOT allowed";
string VS_317 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, i0 "
	" ";

VS_CRITERIA VS_318_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_318_Desc = "vs_3_sw : mad source reg combination aL, p0, aL is NOT allowed";
string VS_318 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, aL "
	" ";

VS_CRITERIA VS_319_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_319_Desc = "vs_3_sw : mad source reg combination aL, p0, p0 is NOT allowed";
string VS_319 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, p0 "
	" ";

VS_CRITERIA VS_320_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_320_Desc = "vs_3_sw : mad source reg combination aL, p0, s0 is NOT allowed";
string VS_320 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, s0 "
	" ";

VS_CRITERIA VS_321_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_321_Desc = "vs_3_sw : mad source reg combination aL, p0, o0 is NOT allowed";
string VS_321 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, o0 "
	" ";

VS_CRITERIA VS_322_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_322_Desc = "vs_3_sw : mad source reg combination aL, s0, a0 is NOT allowed";
string VS_322 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, s0, a0 "
	" ";

VS_CRITERIA VS_323_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_323_Desc = "vs_3_sw : mad source reg combination aL, s0, b0 is NOT allowed";
string VS_323 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, s0, b0 "
	" ";

VS_CRITERIA VS_324_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_324_Desc = "vs_3_sw : mad source reg combination aL, s0, i0 is NOT allowed";
string VS_324 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, s0, i0 "
	" ";

VS_CRITERIA VS_325_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_325_Desc = "vs_3_sw : mad source reg combination aL, s0, aL is NOT allowed";
string VS_325 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, s0, aL "
	" ";

VS_CRITERIA VS_326_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_326_Desc = "vs_3_sw : mad source reg combination aL, s0, p0 is NOT allowed";
string VS_326 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, s0, p0 "
	" ";

VS_CRITERIA VS_327_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_327_Desc = "vs_3_sw : mad source reg combination aL, s0, s0 is NOT allowed";
string VS_327 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, s0, s0 "
	" ";

VS_CRITERIA VS_328_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_328_Desc = "vs_3_sw : mad source reg combination aL, s0, o0 is NOT allowed";
string VS_328 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, s0, o0 "
	" ";

VS_CRITERIA VS_329_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_329_Desc = "vs_3_sw : mad source reg combination aL, o0, a0 is NOT allowed";
string VS_329 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, o0, a0 "
	" ";

VS_CRITERIA VS_330_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_330_Desc = "vs_3_sw : mad source reg combination aL, o0, b0 is NOT allowed";
string VS_330 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, o0, b0 "
	" ";

VS_CRITERIA VS_331_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_331_Desc = "vs_3_sw : mad source reg combination aL, o0, i0 is NOT allowed";
string VS_331 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, o0, i0 "
	" ";

VS_CRITERIA VS_332_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_332_Desc = "vs_3_sw : mad source reg combination aL, o0, aL is NOT allowed";
string VS_332 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, aL, o0, aL "
	" ";

VS_CRITERIA VS_333_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_333_Desc = "vs_3_sw : mad source reg combination aL, o0, p0 is NOT allowed";
string VS_333 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, aL, o0, p0 "
	" ";

VS_CRITERIA VS_334_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_334_Desc = "vs_3_sw : mad source reg combination aL, o0, s0 is NOT allowed";
string VS_334 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, aL, o0, s0 "
	" ";

VS_CRITERIA VS_335_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_335_Desc = "vs_3_sw : mad source reg combination aL, o0, o0 is NOT allowed";
string VS_335 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, aL, o0, o0 "
	" ";

VS_CRITERIA VS_336_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_336_Desc = "vs_3_sw : mad source reg combination p0, a0, a0 is NOT allowed";
string VS_336 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, a0 "
	" ";

VS_CRITERIA VS_337_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_337_Desc = "vs_3_sw : mad source reg combination p0, a0, b0 is NOT allowed";
string VS_337 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, b0 "
	" ";

VS_CRITERIA VS_338_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_338_Desc = "vs_3_sw : mad source reg combination p0, a0, i0 is NOT allowed";
string VS_338 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, i0 "
	" ";

VS_CRITERIA VS_339_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_339_Desc = "vs_3_sw : mad source reg combination p0, a0, aL is NOT allowed";
string VS_339 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, p0, a0, aL "
	" ";

VS_CRITERIA VS_340_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_340_Desc = "vs_3_sw : mad source reg combination p0, a0, p0 is NOT allowed";
string VS_340 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, p0 "
	" ";

VS_CRITERIA VS_341_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_341_Desc = "vs_3_sw : mad source reg combination p0, a0, s0 is NOT allowed";
string VS_341 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, s0 "
	" ";

VS_CRITERIA VS_342_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_342_Desc = "vs_3_sw : mad source reg combination p0, a0, o0 is NOT allowed";
string VS_342 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, o0 "
	" ";

VS_CRITERIA VS_343_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_343_Desc = "vs_3_sw : mad source reg combination p0, b0, a0 is NOT allowed";
string VS_343 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, p0, b0, a0 "
	" ";

VS_CRITERIA VS_344_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_344_Desc = "vs_3_sw : mad source reg combination p0, b0, b0 is NOT allowed";
string VS_344 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, p0, b0, b0 "
	" ";

VS_CRITERIA VS_345_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_345_Desc = "vs_3_sw : mad source reg combination p0, b0, i0 is NOT allowed";
string VS_345 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, p0, b0, i0 "
	" ";

VS_CRITERIA VS_346_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_346_Desc = "vs_3_sw : mad source reg combination p0, b0, aL is NOT allowed";
string VS_346 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, p0, b0, aL "
	" ";

VS_CRITERIA VS_347_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_347_Desc = "vs_3_sw : mad source reg combination p0, b0, p0 is NOT allowed";
string VS_347 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, p0, b0, p0 "
	" ";

VS_CRITERIA VS_348_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_348_Desc = "vs_3_sw : mad source reg combination p0, b0, s0 is NOT allowed";
string VS_348 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_volume s0 "
	"mad r0, p0, b0, s0 "
	" ";

VS_CRITERIA VS_349_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_349_Desc = "vs_3_sw : mad source reg combination p0, b0, o0 is NOT allowed";
string VS_349 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, p0, b0, o0 "
	" ";

VS_CRITERIA VS_350_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_350_Desc = "vs_3_sw : mad source reg combination p0, i0, a0 is NOT allowed";
string VS_350 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, p0, i0, a0 "
	" ";

VS_CRITERIA VS_351_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_351_Desc = "vs_3_sw : mad source reg combination p0, i0, b0 is NOT allowed";
string VS_351 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, b0 "
	" ";

VS_CRITERIA VS_352_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_352_Desc = "vs_3_sw : mad source reg combination p0, i0, i0 is NOT allowed";
string VS_352 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, i0 "
	" ";

VS_CRITERIA VS_353_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_353_Desc = "vs_3_sw : mad source reg combination p0, i0, aL is NOT allowed";
string VS_353 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, p0, i0, aL "
	" ";

VS_CRITERIA VS_354_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_354_Desc = "vs_3_sw : mad source reg combination p0, i0, p0 is NOT allowed";
string VS_354 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, p0 "
	" ";

VS_CRITERIA VS_355_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_355_Desc = "vs_3_sw : mad source reg combination p0, i0, s0 is NOT allowed";
string VS_355 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, p0, i0, s0 "
	" ";

VS_CRITERIA VS_356_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_356_Desc = "vs_3_sw : mad source reg combination p0, i0, o0 is NOT allowed";
string VS_356 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, o0 "
	" ";

VS_CRITERIA VS_357_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_357_Desc = "vs_3_sw : mad source reg combination p0, aL, a0 is NOT allowed";
string VS_357 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, p0, aL, a0 "
	" ";

VS_CRITERIA VS_358_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_358_Desc = "vs_3_sw : mad source reg combination p0, aL, b0 is NOT allowed";
string VS_358 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, b0 "
	" ";

VS_CRITERIA VS_359_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_359_Desc = "vs_3_sw : mad source reg combination p0, aL, i0 is NOT allowed";
string VS_359 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, i0 "
	" ";

VS_CRITERIA VS_360_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_360_Desc = "vs_3_sw : mad source reg combination p0, aL, aL is NOT allowed";
string VS_360 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, aL "
	" ";

VS_CRITERIA VS_361_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_361_Desc = "vs_3_sw : mad source reg combination p0, aL, p0 is NOT allowed";
string VS_361 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, p0 "
	" ";

VS_CRITERIA VS_362_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_362_Desc = "vs_3_sw : mad source reg combination p0, aL, s0 is NOT allowed";
string VS_362 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, s0 "
	" ";

VS_CRITERIA VS_363_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_363_Desc = "vs_3_sw : mad source reg combination p0, aL, o0 is NOT allowed";
string VS_363 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, o0 "
	" ";

VS_CRITERIA VS_364_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_364_Desc = "vs_3_sw : mad source reg combination p0, p0, a0 is NOT allowed";
string VS_364 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, p0, p0, a0 "
	" ";

VS_CRITERIA VS_365_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_365_Desc = "vs_3_sw : mad source reg combination p0, p0, b0 is NOT allowed";
string VS_365 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, p0, p0, b0 "
	" ";

VS_CRITERIA VS_366_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_366_Desc = "vs_3_sw : mad source reg combination p0, p0, i0 is NOT allowed";
string VS_366 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, p0, i0 "
	" ";

VS_CRITERIA VS_367_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_367_Desc = "vs_3_sw : mad source reg combination p0, p0, aL is NOT allowed";
string VS_367 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, p0, p0, aL "
	" ";

VS_CRITERIA VS_368_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_368_Desc = "vs_3_sw : mad source reg combination p0, p0, p0 is NOT allowed";
string VS_368 = 
	"vs_3_sw "
	"mad r0, p0, p0, p0 "
	" ";

VS_CRITERIA VS_369_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_369_Desc = "vs_3_sw : mad source reg combination p0, p0, s0 is NOT allowed";
string VS_369 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mad r0, p0, p0, s0 "
	" ";

VS_CRITERIA VS_370_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_370_Desc = "vs_3_sw : mad source reg combination p0, p0, o0 is NOT allowed";
string VS_370 = 
	"vs_3_sw "
	"mad r0, p0, p0, o0 "
	" ";

VS_CRITERIA VS_371_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_371_Desc = "vs_3_sw : mad source reg combination p0, s0, a0 is NOT allowed";
string VS_371 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, p0, s0, a0 "
	" ";

VS_CRITERIA VS_372_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_372_Desc = "vs_3_sw : mad source reg combination p0, s0, b0 is NOT allowed";
string VS_372 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, p0, s0, b0 "
	" ";

VS_CRITERIA VS_373_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_373_Desc = "vs_3_sw : mad source reg combination p0, s0, i0 is NOT allowed";
string VS_373 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, p0, s0, i0 "
	" ";

VS_CRITERIA VS_374_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_374_Desc = "vs_3_sw : mad source reg combination p0, s0, aL is NOT allowed";
string VS_374 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, p0, s0, aL "
	" ";

VS_CRITERIA VS_375_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_375_Desc = "vs_3_sw : mad source reg combination p0, s0, p0 is NOT allowed";
string VS_375 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mad r0, p0, s0, p0 "
	" ";

VS_CRITERIA VS_376_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_376_Desc = "vs_3_sw : mad source reg combination p0, s0, s0 is NOT allowed";
string VS_376 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mad r0, p0, s0, s0 "
	" ";

VS_CRITERIA VS_377_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_377_Desc = "vs_3_sw : mad source reg combination p0, s0, o0 is NOT allowed";
string VS_377 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mad r0, p0, s0, o0 "
	" ";

VS_CRITERIA VS_378_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_378_Desc = "vs_3_sw : mad source reg combination p0, o0, a0 is NOT allowed";
string VS_378 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, p0, o0, a0 "
	" ";

VS_CRITERIA VS_379_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_379_Desc = "vs_3_sw : mad source reg combination p0, o0, b0 is NOT allowed";
string VS_379 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, p0, o0, b0 "
	" ";

VS_CRITERIA VS_380_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_380_Desc = "vs_3_sw : mad source reg combination p0, o0, i0 is NOT allowed";
string VS_380 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, o0, i0 "
	" ";

VS_CRITERIA VS_381_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_381_Desc = "vs_3_sw : mad source reg combination p0, o0, aL is NOT allowed";
string VS_381 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, p0, o0, aL "
	" ";

VS_CRITERIA VS_382_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_382_Desc = "vs_3_sw : mad source reg combination p0, o0, p0 is NOT allowed";
string VS_382 = 
	"vs_3_sw "
	"mad r0, p0, o0, p0 "
	" ";

VS_CRITERIA VS_383_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_383_Desc = "vs_3_sw : mad source reg combination p0, o0, s0 is NOT allowed";
string VS_383 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mad r0, p0, o0, s0 "
	" ";

VS_CRITERIA VS_384_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_384_Desc = "vs_3_sw : mad source reg combination p0, o0, o0 is NOT allowed";
string VS_384 = 
	"vs_3_sw "
	"mad r0, p0, o0, o0 "
	" ";

VS_CRITERIA VS_385_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_385_Desc = "vs_3_sw : mad source reg combination s0, a0, a0 is NOT allowed";
string VS_385 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, a0, a0 "
	" ";

VS_CRITERIA VS_386_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_386_Desc = "vs_3_sw : mad source reg combination s0, a0, b0 is NOT allowed";
string VS_386 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, a0, b0 "
	" ";

VS_CRITERIA VS_387_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_387_Desc = "vs_3_sw : mad source reg combination s0, a0, i0 is NOT allowed";
string VS_387 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, a0, i0 "
	" ";

VS_CRITERIA VS_388_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_388_Desc = "vs_3_sw : mad source reg combination s0, a0, aL is NOT allowed";
string VS_388 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, s0, a0, aL "
	" ";

VS_CRITERIA VS_389_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_389_Desc = "vs_3_sw : mad source reg combination s0, a0, p0 is NOT allowed";
string VS_389 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, a0, p0 "
	" ";

VS_CRITERIA VS_390_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_390_Desc = "vs_3_sw : mad source reg combination s0, a0, s0 is NOT allowed";
string VS_390 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, a0, s0 "
	" ";

VS_CRITERIA VS_391_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_391_Desc = "vs_3_sw : mad source reg combination s0, a0, o0 is NOT allowed";
string VS_391 = 
	"vs_3_sw "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, a0, o0 "
	" ";

VS_CRITERIA VS_392_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_392_Desc = "vs_3_sw : mad source reg combination s0, b0, a0 is NOT allowed";
string VS_392 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, b0, a0 "
	" ";

VS_CRITERIA VS_393_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_393_Desc = "vs_3_sw : mad source reg combination s0, b0, b0 is NOT allowed";
string VS_393 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, b0 "
	" ";

VS_CRITERIA VS_394_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_394_Desc = "vs_3_sw : mad source reg combination s0, b0, i0 is NOT allowed";
string VS_394 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, i0 "
	" ";

VS_CRITERIA VS_395_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_395_Desc = "vs_3_sw : mad source reg combination s0, b0, aL is NOT allowed";
string VS_395 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, b0, aL "
	" ";

VS_CRITERIA VS_396_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_396_Desc = "vs_3_sw : mad source reg combination s0, b0, p0 is NOT allowed";
string VS_396 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, p0 "
	" ";

VS_CRITERIA VS_397_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_397_Desc = "vs_3_sw : mad source reg combination s0, b0, s0 is NOT allowed";
string VS_397 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, s0 "
	" ";

VS_CRITERIA VS_398_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_398_Desc = "vs_3_sw : mad source reg combination s0, b0, o0 is NOT allowed";
string VS_398 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_2d s0 "
	"mad r0, s0, b0, o0 "
	" ";

VS_CRITERIA VS_399_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_399_Desc = "vs_3_sw : mad source reg combination s0, i0, a0 is NOT allowed";
string VS_399 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, i0, a0 "
	" ";

VS_CRITERIA VS_400_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_400_Desc = "vs_3_sw : mad source reg combination s0, i0, b0 is NOT allowed";
string VS_400 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, i0, b0 "
	" ";

VS_CRITERIA VS_401_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_401_Desc = "vs_3_sw : mad source reg combination s0, i0, i0 is NOT allowed";
string VS_401 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, i0, i0 "
	" ";

VS_CRITERIA VS_402_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_402_Desc = "vs_3_sw : mad source reg combination s0, i0, aL is NOT allowed";
string VS_402 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, i0, aL "
	" ";

VS_CRITERIA VS_403_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_403_Desc = "vs_3_sw : mad source reg combination s0, i0, p0 is NOT allowed";
string VS_403 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, i0, p0 "
	" ";

VS_CRITERIA VS_404_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_404_Desc = "vs_3_sw : mad source reg combination s0, i0, s0 is NOT allowed";
string VS_404 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"mad r0, s0, i0, s0 "
	" ";

VS_CRITERIA VS_405_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_405_Desc = "vs_3_sw : mad source reg combination s0, i0, o0 is NOT allowed";
string VS_405 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mad r0, s0, i0, o0 "
	" ";

VS_CRITERIA VS_406_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_406_Desc = "vs_3_sw : mad source reg combination s0, aL, a0 is NOT allowed";
string VS_406 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, s0, aL, a0 "
	" ";

VS_CRITERIA VS_407_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_407_Desc = "vs_3_sw : mad source reg combination s0, aL, b0 is NOT allowed";
string VS_407 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, aL, b0 "
	" ";

VS_CRITERIA VS_408_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_408_Desc = "vs_3_sw : mad source reg combination s0, aL, i0 is NOT allowed";
string VS_408 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, aL, i0 "
	" ";

VS_CRITERIA VS_409_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_409_Desc = "vs_3_sw : mad source reg combination s0, aL, aL is NOT allowed";
string VS_409 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, aL, aL "
	" ";

VS_CRITERIA VS_410_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_410_Desc = "vs_3_sw : mad source reg combination s0, aL, p0 is NOT allowed";
string VS_410 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, aL, p0 "
	" ";

VS_CRITERIA VS_411_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_411_Desc = "vs_3_sw : mad source reg combination s0, aL, s0 is NOT allowed";
string VS_411 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, aL, s0 "
	" ";

VS_CRITERIA VS_412_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_412_Desc = "vs_3_sw : mad source reg combination s0, aL, o0 is NOT allowed";
string VS_412 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, aL, o0 "
	" ";

VS_CRITERIA VS_413_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_413_Desc = "vs_3_sw : mad source reg combination s0, p0, a0 is NOT allowed";
string VS_413 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, p0, a0 "
	" ";

VS_CRITERIA VS_414_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_414_Desc = "vs_3_sw : mad source reg combination s0, p0, b0 is NOT allowed";
string VS_414 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, s0, p0, b0 "
	" ";

VS_CRITERIA VS_415_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_415_Desc = "vs_3_sw : mad source reg combination s0, p0, i0 is NOT allowed";
string VS_415 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mad r0, s0, p0, i0 "
	" ";

VS_CRITERIA VS_416_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_416_Desc = "vs_3_sw : mad source reg combination s0, p0, aL is NOT allowed";
string VS_416 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, p0, aL "
	" ";

VS_CRITERIA VS_417_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_417_Desc = "vs_3_sw : mad source reg combination s0, p0, p0 is NOT allowed";
string VS_417 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mad r0, s0, p0, p0 "
	" ";

VS_CRITERIA VS_418_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_418_Desc = "vs_3_sw : mad source reg combination s0, p0, s0 is NOT allowed";
string VS_418 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mad r0, s0, p0, s0 "
	" ";

VS_CRITERIA VS_419_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_419_Desc = "vs_3_sw : mad source reg combination s0, p0, o0 is NOT allowed";
string VS_419 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mad r0, s0, p0, o0 "
	" ";

VS_CRITERIA VS_420_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_420_Desc = "vs_3_sw : mad source reg combination s0, s0, a0 is NOT allowed";
string VS_420 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, s0, a0 "
	" ";

VS_CRITERIA VS_421_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_421_Desc = "vs_3_sw : mad source reg combination s0, s0, b0 is NOT allowed";
string VS_421 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, s0, s0, b0 "
	" ";

VS_CRITERIA VS_422_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_422_Desc = "vs_3_sw : mad source reg combination s0, s0, i0 is NOT allowed";
string VS_422 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mad r0, s0, s0, i0 "
	" ";

VS_CRITERIA VS_423_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_423_Desc = "vs_3_sw : mad source reg combination s0, s0, aL is NOT allowed";
string VS_423 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, s0, aL "
	" ";

VS_CRITERIA VS_424_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_424_Desc = "vs_3_sw : mad source reg combination s0, s0, p0 is NOT allowed";
string VS_424 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mad r0, s0, s0, p0 "
	" ";

VS_CRITERIA VS_425_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_425_Desc = "vs_3_sw : mad source reg combination s0, s0, s0 is NOT allowed";
string VS_425 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mad r0, s0, s0, s0 "
	" ";

VS_CRITERIA VS_426_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_426_Desc = "vs_3_sw : mad source reg combination s0, s0, o0 is NOT allowed";
string VS_426 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mad r0, s0, s0, o0 "
	" ";

VS_CRITERIA VS_427_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_427_Desc = "vs_3_sw : mad source reg combination s0, o0, a0 is NOT allowed";
string VS_427 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mova a0.x, c0.x "
	"mad r0, s0, o0, a0 "
	" ";

VS_CRITERIA VS_428_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_428_Desc = "vs_3_sw : mad source reg combination s0, o0, b0 is NOT allowed";
string VS_428 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_volume s0 "
	"mad r0, s0, o0, b0 "
	" ";

VS_CRITERIA VS_429_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_429_Desc = "vs_3_sw : mad source reg combination s0, o0, i0 is NOT allowed";
string VS_429 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, s0, o0, i0 "
	" ";

VS_CRITERIA VS_430_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_430_Desc = "vs_3_sw : mad source reg combination s0, o0, aL is NOT allowed";
string VS_430 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mova aL.x, c0.x "
	"mad r0, s0, o0, aL "
	" ";

VS_CRITERIA VS_431_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_431_Desc = "vs_3_sw : mad source reg combination s0, o0, p0 is NOT allowed";
string VS_431 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mad r0, s0, o0, p0 "
	" ";

VS_CRITERIA VS_432_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_432_Desc = "vs_3_sw : mad source reg combination s0, o0, s0 is NOT allowed";
string VS_432 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mad r0, s0, o0, s0 "
	" ";

VS_CRITERIA VS_433_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_433_Desc = "vs_3_sw : mad source reg combination s0, o0, o0 is NOT allowed";
string VS_433 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mad r0, s0, o0, o0 "
	" ";

VS_CRITERIA VS_434_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_434_Desc = "vs_3_sw : mad source reg combination o0, a0, a0 is NOT allowed";
string VS_434 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, o0, a0, a0 "
	" ";

VS_CRITERIA VS_435_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_435_Desc = "vs_3_sw : mad source reg combination o0, a0, b0 is NOT allowed";
string VS_435 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, o0, a0, b0 "
	" ";

VS_CRITERIA VS_436_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_436_Desc = "vs_3_sw : mad source reg combination o0, a0, i0 is NOT allowed";
string VS_436 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, o0, a0, i0 "
	" ";

VS_CRITERIA VS_437_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_437_Desc = "vs_3_sw : mad source reg combination o0, a0, aL is NOT allowed";
string VS_437 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, o0, a0, aL "
	" ";

VS_CRITERIA VS_438_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_438_Desc = "vs_3_sw : mad source reg combination o0, a0, p0 is NOT allowed";
string VS_438 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, o0, a0, p0 "
	" ";

VS_CRITERIA VS_439_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_439_Desc = "vs_3_sw : mad source reg combination o0, a0, s0 is NOT allowed";
string VS_439 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mova a0.x, c0.x "
	"mad r0, o0, a0, s0 "
	" ";

VS_CRITERIA VS_440_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_440_Desc = "vs_3_sw : mad source reg combination o0, a0, o0 is NOT allowed";
string VS_440 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, o0, a0, o0 "
	" ";

VS_CRITERIA VS_441_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_441_Desc = "vs_3_sw : mad source reg combination o0, b0, a0 is NOT allowed";
string VS_441 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, o0, b0, a0 "
	" ";

VS_CRITERIA VS_442_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_442_Desc = "vs_3_sw : mad source reg combination o0, b0, b0 is NOT allowed";
string VS_442 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, o0, b0, b0 "
	" ";

VS_CRITERIA VS_443_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_443_Desc = "vs_3_sw : mad source reg combination o0, b0, i0 is NOT allowed";
string VS_443 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, o0, b0, i0 "
	" ";

VS_CRITERIA VS_444_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_444_Desc = "vs_3_sw : mad source reg combination o0, b0, aL is NOT allowed";
string VS_444 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, o0, b0, aL "
	" ";

VS_CRITERIA VS_445_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_445_Desc = "vs_3_sw : mad source reg combination o0, b0, p0 is NOT allowed";
string VS_445 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, o0, b0, p0 "
	" ";

VS_CRITERIA VS_446_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_446_Desc = "vs_3_sw : mad source reg combination o0, b0, s0 is NOT allowed";
string VS_446 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_cube s0 "
	"mad r0, o0, b0, s0 "
	" ";

VS_CRITERIA VS_447_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_447_Desc = "vs_3_sw : mad source reg combination o0, b0, o0 is NOT allowed";
string VS_447 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, o0, b0, o0 "
	" ";

VS_CRITERIA VS_448_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_448_Desc = "vs_3_sw : mad source reg combination o0, i0, a0 is NOT allowed";
string VS_448 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, o0, i0, a0 "
	" ";

VS_CRITERIA VS_449_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_449_Desc = "vs_3_sw : mad source reg combination o0, i0, b0 is NOT allowed";
string VS_449 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, o0, i0, b0 "
	" ";

VS_CRITERIA VS_450_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_450_Desc = "vs_3_sw : mad source reg combination o0, i0, i0 is NOT allowed";
string VS_450 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, o0, i0, i0 "
	" ";

VS_CRITERIA VS_451_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_451_Desc = "vs_3_sw : mad source reg combination o0, i0, aL is NOT allowed";
string VS_451 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, o0, i0, aL "
	" ";

VS_CRITERIA VS_452_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_452_Desc = "vs_3_sw : mad source reg combination o0, i0, p0 is NOT allowed";
string VS_452 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, o0, i0, p0 "
	" ";

VS_CRITERIA VS_453_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_453_Desc = "vs_3_sw : mad source reg combination o0, i0, s0 is NOT allowed";
string VS_453 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"mad r0, o0, i0, s0 "
	" ";

VS_CRITERIA VS_454_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_454_Desc = "vs_3_sw : mad source reg combination o0, i0, o0 is NOT allowed";
string VS_454 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, o0, i0, o0 "
	" ";

VS_CRITERIA VS_455_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_455_Desc = "vs_3_sw : mad source reg combination o0, aL, a0 is NOT allowed";
string VS_455 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, o0, aL, a0 "
	" ";

VS_CRITERIA VS_456_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_456_Desc = "vs_3_sw : mad source reg combination o0, aL, b0 is NOT allowed";
string VS_456 = 
	"vs_3_sw "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, o0, aL, b0 "
	" ";

VS_CRITERIA VS_457_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_457_Desc = "vs_3_sw : mad source reg combination o0, aL, i0 is NOT allowed";
string VS_457 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, o0, aL, i0 "
	" ";

VS_CRITERIA VS_458_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_458_Desc = "vs_3_sw : mad source reg combination o0, aL, aL is NOT allowed";
string VS_458 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, o0, aL, aL "
	" ";

VS_CRITERIA VS_459_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_459_Desc = "vs_3_sw : mad source reg combination o0, aL, p0 is NOT allowed";
string VS_459 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, o0, aL, p0 "
	" ";

VS_CRITERIA VS_460_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_460_Desc = "vs_3_sw : mad source reg combination o0, aL, s0 is NOT allowed";
string VS_460 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova aL.x, c0.x "
	"mad r0, o0, aL, s0 "
	" ";

VS_CRITERIA VS_461_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_461_Desc = "vs_3_sw : mad source reg combination o0, aL, o0 is NOT allowed";
string VS_461 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, o0, aL, o0 "
	" ";

VS_CRITERIA VS_462_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_462_Desc = "vs_3_sw : mad source reg combination o0, p0, a0 is NOT allowed";
string VS_462 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, o0, p0, a0 "
	" ";

VS_CRITERIA VS_463_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_463_Desc = "vs_3_sw : mad source reg combination o0, p0, b0 is NOT allowed";
string VS_463 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, o0, p0, b0 "
	" ";

VS_CRITERIA VS_464_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_464_Desc = "vs_3_sw : mad source reg combination o0, p0, i0 is NOT allowed";
string VS_464 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, o0, p0, i0 "
	" ";

VS_CRITERIA VS_465_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_465_Desc = "vs_3_sw : mad source reg combination o0, p0, aL is NOT allowed";
string VS_465 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, o0, p0, aL "
	" ";

VS_CRITERIA VS_466_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_466_Desc = "vs_3_sw : mad source reg combination o0, p0, p0 is NOT allowed";
string VS_466 = 
	"vs_3_sw "
	"mad r0, o0, p0, p0 "
	" ";

VS_CRITERIA VS_467_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_467_Desc = "vs_3_sw : mad source reg combination o0, p0, s0 is NOT allowed";
string VS_467 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mad r0, o0, p0, s0 "
	" ";

VS_CRITERIA VS_468_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_468_Desc = "vs_3_sw : mad source reg combination o0, p0, o0 is NOT allowed";
string VS_468 = 
	"vs_3_sw "
	"mad r0, o0, p0, o0 "
	" ";

VS_CRITERIA VS_469_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_469_Desc = "vs_3_sw : mad source reg combination o0, s0, a0 is NOT allowed";
string VS_469 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mova a0.x, c0.x "
	"mad r0, o0, s0, a0 "
	" ";

VS_CRITERIA VS_470_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_470_Desc = "vs_3_sw : mad source reg combination o0, s0, b0 is NOT allowed";
string VS_470 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_volume s0 "
	"mad r0, o0, s0, b0 "
	" ";

VS_CRITERIA VS_471_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_471_Desc = "vs_3_sw : mad source reg combination o0, s0, i0 is NOT allowed";
string VS_471 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"mad r0, o0, s0, i0 "
	" ";

VS_CRITERIA VS_472_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_472_Desc = "vs_3_sw : mad source reg combination o0, s0, aL is NOT allowed";
string VS_472 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mova aL.x, c0.x "
	"mad r0, o0, s0, aL "
	" ";

VS_CRITERIA VS_473_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_473_Desc = "vs_3_sw : mad source reg combination o0, s0, p0 is NOT allowed";
string VS_473 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mad r0, o0, s0, p0 "
	" ";

VS_CRITERIA VS_474_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_474_Desc = "vs_3_sw : mad source reg combination o0, s0, s0 is NOT allowed";
string VS_474 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mad r0, o0, s0, s0 "
	" ";

VS_CRITERIA VS_475_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_475_Desc = "vs_3_sw : mad source reg combination o0, s0, o0 is NOT allowed";
string VS_475 = 
	"vs_3_sw "
	"dcl_volume s0 "
	"mad r0, o0, s0, o0 "
	" ";

VS_CRITERIA VS_476_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_476_Desc = "vs_3_sw : mad source reg combination o0, o0, a0 is NOT allowed";
string VS_476 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mad r0, o0, o0, a0 "
	" ";

VS_CRITERIA VS_477_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_477_Desc = "vs_3_sw : mad source reg combination o0, o0, b0 is NOT allowed";
string VS_477 = 
	"vs_3_sw "
	"defb b0, true "
	"mad r0, o0, o0, b0 "
	" ";

VS_CRITERIA VS_478_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_478_Desc = "vs_3_sw : mad source reg combination o0, o0, i0 is NOT allowed";
string VS_478 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, o0, o0, i0 "
	" ";

VS_CRITERIA VS_479_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_479_Desc = "vs_3_sw : mad source reg combination o0, o0, aL is NOT allowed";
string VS_479 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mad r0, o0, o0, aL "
	" ";

VS_CRITERIA VS_480_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_480_Desc = "vs_3_sw : mad source reg combination o0, o0, p0 is NOT allowed";
string VS_480 = 
	"vs_3_sw "
	"mad r0, o0, o0, p0 "
	" ";

VS_CRITERIA VS_481_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_481_Desc = "vs_3_sw : mad source reg combination o0, o0, s0 is NOT allowed";
string VS_481 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mad r0, o0, o0, s0 "
	" ";

VS_CRITERIA VS_482_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_482_Desc = "vs_3_sw : mad source reg combination o0, o0, o0 is NOT allowed";
string VS_482 = 
	"vs_3_sw "
	"mad r0, o0, o0, o0 "
	" ";

VS_CRITERIA VS_483_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_483_Desc = "vs_3_sw : mad dest r# is allowed";
string VS_483 = 
	"vs_3_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_color v2 "
	"mad r0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_484_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_484_Desc = "vs_3_sw : mad _sat dest r# is allowed";
string VS_484 = 
	"vs_3_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_color v2 "
	"mad_sat r0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_485_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_485_Desc = "vs_3_sw : mad dest o# is allowed";
string VS_485 = 
	"vs_3_sw "
	"dcl_texcoord o0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_color v2 "
	"mad o0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_486_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_486_Desc = "vs_3_sw : mad _sat dest o# is allowed";
string VS_486 = 
	"vs_3_sw "
	"dcl_texcoord o0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_color v2 "
	"mad_sat o0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_487_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_487_Desc = "vs_3_sw : mad dest v# is NOT allowed";
string VS_487 = 
	"vs_3_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_color v2 "
	"mad v0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_488_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_488_Desc = "vs_3_sw : mad _sat dest v# is NOT allowed";
string VS_488 = 
	"vs_3_sw "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_color v2 "
	"mad_sat v0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_489_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_489_Desc = "vs_3_sw : mad dest c# is NOT allowed";
string VS_489 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad c1, v0, v1, v2 "
	" ";

VS_CRITERIA VS_490_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_490_Desc = "vs_3_sw : mad _sat dest c# is NOT allowed";
string VS_490 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad_sat c1, v0, v1, v2 "
	" ";

VS_CRITERIA VS_491_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_491_Desc = "vs_3_sw : mad dest a0 is NOT allowed";
string VS_491 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad a0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_492_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_492_Desc = "vs_3_sw : mad _sat dest a0 is NOT allowed";
string VS_492 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad_sat a0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_493_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_493_Desc = "vs_3_sw : mad dest b# is NOT allowed";
string VS_493 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad b0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_494_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_494_Desc = "vs_3_sw : mad _sat dest b# is NOT allowed";
string VS_494 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad_sat b0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_495_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_495_Desc = "vs_3_sw : mad dest i# is NOT allowed";
string VS_495 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad i0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_496_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_496_Desc = "vs_3_sw : mad _sat dest i# is NOT allowed";
string VS_496 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad_sat i0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_497_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_497_Desc = "vs_3_sw : mad dest aL is NOT allowed";
string VS_497 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad aL, v0, v1, v2 "
	" ";

VS_CRITERIA VS_498_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_498_Desc = "vs_3_sw : mad _sat dest aL is NOT allowed";
string VS_498 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad_sat aL, v0, v1, v2 "
	" ";

VS_CRITERIA VS_499_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_499_Desc = "vs_3_sw : mad dest p0 is NOT allowed";
string VS_499 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad p0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_500_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_500_Desc = "vs_3_sw : mad _sat dest p0 is NOT allowed";
string VS_500 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad_sat p0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_501_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_501_Desc = "vs_3_sw : mad dest s# is NOT allowed";
string VS_501 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad s0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_502_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_502_Desc = "vs_3_sw : mad _sat dest s# is NOT allowed";
string VS_502 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad_sat s0, v0, v1, v2 "
	" ";

VS_CRITERIA VS_503_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_503_Desc = "vs_3_sw : mad write mask .x is allowed";
string VS_503 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.x, v0, v1, v2 "
	" ";

VS_CRITERIA VS_504_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_504_Desc = "vs_3_sw : mad write mask .y is allowed";
string VS_504 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.y, v0, v1, v2 "
	" ";

VS_CRITERIA VS_505_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_505_Desc = "vs_3_sw : mad write mask .z is allowed";
string VS_505 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.z, v0, v1, v2 "
	" ";

VS_CRITERIA VS_506_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_506_Desc = "vs_3_sw : mad write mask .w is allowed";
string VS_506 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.w, v0, v1, v2 "
	" ";

VS_CRITERIA VS_507_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_507_Desc = "vs_3_sw : mad write mask .xy is allowed";
string VS_507 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.xy, v0, v1, v2 "
	" ";

VS_CRITERIA VS_508_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_508_Desc = "vs_3_sw : mad write mask .yz is allowed";
string VS_508 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.yz, v0, v1, v2 "
	" ";

VS_CRITERIA VS_509_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_509_Desc = "vs_3_sw : mad write mask .zw is allowed";
string VS_509 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.zw, v0, v1, v2 "
	" ";

VS_CRITERIA VS_510_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_510_Desc = "vs_3_sw : mad write mask .xz is allowed";
string VS_510 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.xz, v0, v1, v2 "
	" ";

VS_CRITERIA VS_511_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_511_Desc = "vs_3_sw : mad write mask .xw is allowed";
string VS_511 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.xw, v0, v1, v2 "
	" ";

VS_CRITERIA VS_512_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_512_Desc = "vs_3_sw : mad write mask .yw is allowed";
string VS_512 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.yw, v0, v1, v2 "
	" ";

VS_CRITERIA VS_513_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_513_Desc = "vs_3_sw : mad write mask .xyz is allowed";
string VS_513 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.xyz, v0, v1, v2 "
	" ";

VS_CRITERIA VS_514_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_514_Desc = "vs_3_sw : mad write mask .yzw is allowed";
string VS_514 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.yzw, v0, v1, v2 "
	" ";

VS_CRITERIA VS_515_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_515_Desc = "vs_3_sw : mad write mask .xzw is allowed";
string VS_515 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.xzw, v0, v1, v2 "
	" ";

VS_CRITERIA VS_516_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_516_Desc = "vs_3_sw : mad write mask .xyzw is allowed";
string VS_516 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.xyzw, v0, v1, v2 "
	" ";

VS_CRITERIA VS_517_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_517_Desc = "vs_3_sw : mad write mask .yx is NOT allowed";
string VS_517 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.yx, v0, v1, v2 "
	" ";

VS_CRITERIA VS_518_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_518_Desc = "vs_3_sw : mad write mask .zx is NOT allowed";
string VS_518 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.zx, v0, v1, v2 "
	" ";

VS_CRITERIA VS_519_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_519_Desc = "vs_3_sw : mad write mask .zy is NOT allowed";
string VS_519 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.zy, v0, v1, v2 "
	" ";

VS_CRITERIA VS_520_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_520_Desc = "vs_3_sw : mad write mask .wx is NOT allowed";
string VS_520 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.wx, v0, v1, v2 "
	" ";

VS_CRITERIA VS_521_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_521_Desc = "vs_3_sw : mad write mask .wz is NOT allowed";
string VS_521 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.wz, v0, v1, v2 "
	" ";

VS_CRITERIA VS_522_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_522_Desc = "vs_3_sw : mad write mask .wy is NOT allowed";
string VS_522 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.wy, v0, v1, v2 "
	" ";

VS_CRITERIA VS_523_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_523_Desc = "vs_3_sw : mad write mask .zyx is NOT allowed";
string VS_523 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.zyx, v0, v1, v2 "
	" ";

VS_CRITERIA VS_524_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_524_Desc = "vs_3_sw : mad write mask .wzy is NOT allowed";
string VS_524 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.wzy, v0, v1, v2 "
	" ";

VS_CRITERIA VS_525_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_525_Desc = "vs_3_sw : mad write mask .wzx is NOT allowed";
string VS_525 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.wzx, v0, v1, v2 "
	" ";

VS_CRITERIA VS_526_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_526_Desc = "vs_3_sw : mad write mask .wyx is NOT allowed";
string VS_526 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.wyx, v0, v1, v2 "
	" ";

VS_CRITERIA VS_527_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_527_Desc = "vs_3_sw : mad write mask .yxw is NOT allowed";
string VS_527 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.yxw, v0, v1, v2 "
	" ";

VS_CRITERIA VS_528_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_528_Desc = "vs_3_sw : mad write mask .wzyx is NOT allowed";
string VS_528 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.wzyx, v0, v1, v2 "
	" ";

VS_CRITERIA VS_529_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_529_Desc = "vs_3_sw : mad write mask .zxwy is NOT allowed";
string VS_529 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"mad r0.zxwy, v0, v1, v2 "
	" ";

VS_CRITERIA VS_530_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_530_Desc = "vs_3_sw : mad is allowed in a 1 level if b0 block";
string VS_530 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if b0 "
	"	mad r0, v0, v1, v2 "
	"endif ";

VS_CRITERIA VS_531_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_531_Desc = "vs_3_sw : mad is allowed in a 8 level if b0 block";
string VS_531 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								mad r0, v0, v1, v2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_532_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_532_Desc = "vs_3_sw : mad is allowed in a 16 level if b0 block";
string VS_532 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_533_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_533_Desc = "vs_3_sw : mad is allowed in a 24 level if b0 block";
string VS_533 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_534_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_534_Desc = "vs_3_sw : mad is NOT allowed in a 25 level if b0 block";
string VS_534 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_535_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_535_Desc = "vs_3_sw : mad is allowed in a 1 level if b0 nop else block";
string VS_535 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if b0 nop else "
	"	mad r0, v0, v1, v2 "
	"endif ";

VS_CRITERIA VS_536_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_536_Desc = "vs_3_sw : mad is allowed in a 8 level if b0 nop else block";
string VS_536 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								mad r0, v0, v1, v2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_537_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_537_Desc = "vs_3_sw : mad is allowed in a 16 level if b0 nop else block";
string VS_537 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_538_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_538_Desc = "vs_3_sw : mad is allowed in a 24 level if b0 nop else block";
string VS_538 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_539_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_539_Desc = "vs_3_sw : mad is NOT allowed in a 25 level if b0 nop else block";
string VS_539 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_540_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_540_Desc = "vs_3_sw : mad is allowed in a 1 level loop aL, i0 block";
string VS_540 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"loop aL, i0 "
	"	mad r0, v0, v1, v2 "
	"endloop ";

VS_CRITERIA VS_541_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_541_Desc = "vs_3_sw : mad is allowed in a 2 level loop aL, i0 block";
string VS_541 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_542_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_542_Desc = "vs_3_sw : mad is allowed in a 3 level loop aL, i0 block";
string VS_542 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			mad r0, v0, v1, v2 "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_543_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_543_Desc = "vs_3_sw : mad is allowed in a 4 level loop aL, i0 block";
string VS_543 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				mad r0, v0, v1, v2 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_544_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_544_Desc = "vs_3_sw : mad is NOT allowed in a 5 level loop aL, i0 block";
string VS_544 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					mad r0, v0, v1, v2 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_545_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_545_Desc = "vs_3_sw : mad is allowed in a 1 level rep i0 block";
string VS_545 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"rep i0 "
	"	mad r0, v0, v1, v2 "
	"endrep ";

VS_CRITERIA VS_546_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_546_Desc = "vs_3_sw : mad is allowed in a 2 level rep i0 block";
string VS_546 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"rep i0 "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_547_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_547_Desc = "vs_3_sw : mad is allowed in a 3 level rep i0 block";
string VS_547 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			mad r0, v0, v1, v2 "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_548_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_548_Desc = "vs_3_sw : mad is allowed in a 4 level rep i0 block";
string VS_548 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				mad r0, v0, v1, v2 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_549_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_549_Desc = "vs_3_sw : mad is NOT allowed in a 5 level rep i0 block";
string VS_549 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					mad r0, v0, v1, v2 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_550_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_550_Desc = "vs_3_sw : mad is allowed in a 1 level if_lt c0.x, c0.y block";
string VS_550 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if_lt c0.x, c0.y "
	"	mad r0, v0, v1, v2 "
	"endif ";

VS_CRITERIA VS_551_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_551_Desc = "vs_3_sw : mad is allowed in a 8 level if_lt c0.x, c0.y block";
string VS_551 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								mad r0, v0, v1, v2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_552_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_552_Desc = "vs_3_sw : mad is allowed in a 16 level if_lt c0.x, c0.y block";
string VS_552 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_553_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_553_Desc = "vs_3_sw : mad is allowed in a 24 level if_lt c0.x, c0.y block";
string VS_553 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_554_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_554_Desc = "vs_3_sw : mad is NOT allowed in a 25 level if_lt c0.x, c0.y block";
string VS_554 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_555_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_555_Desc = "vs_3_sw : mad is allowed in a 1 level if_lt c0.x, c0.y nop else block";
string VS_555 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if_lt c0.x, c0.y nop else "
	"	mad r0, v0, v1, v2 "
	"endif ";

VS_CRITERIA VS_556_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_556_Desc = "vs_3_sw : mad is allowed in a 8 level if_lt c0.x, c0.y nop else block";
string VS_556 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								mad r0, v0, v1, v2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_557_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_557_Desc = "vs_3_sw : mad is allowed in a 16 level if_lt c0.x, c0.y nop else block";
string VS_557 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_558_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_558_Desc = "vs_3_sw : mad is allowed in a 24 level if_lt c0.x, c0.y nop else block";
string VS_558 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_559_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_559_Desc = "vs_3_sw : mad is NOT allowed in a 25 level if_lt c0.x, c0.y nop else block";
string VS_559 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_560_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_560_Desc = "vs_3_sw : mad is allowed in a 1 level if p0.x block";
string VS_560 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	mad r0, v0, v1, v2 "
	"endif ";

VS_CRITERIA VS_561_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_561_Desc = "vs_3_sw : mad is allowed in a 8 level if p0.x block";
string VS_561 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								mad r0, v0, v1, v2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_562_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_562_Desc = "vs_3_sw : mad is allowed in a 16 level if p0.x block";
string VS_562 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_563_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_563_Desc = "vs_3_sw : mad is allowed in a 24 level if p0.x block";
string VS_563 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_564_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_564_Desc = "vs_3_sw : mad is NOT allowed in a 25 level if p0.x block";
string VS_564 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_565_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_565_Desc = "vs_3_sw : mad is allowed in a 1 level if p0.x nop else block";
string VS_565 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	mad r0, v0, v1, v2 "
	"endif ";

VS_CRITERIA VS_566_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_566_Desc = "vs_3_sw : mad is allowed in a 8 level if p0.x nop else block";
string VS_566 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								mad r0, v0, v1, v2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_567_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_567_Desc = "vs_3_sw : mad is allowed in a 16 level if p0.x nop else block";
string VS_567 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_568_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_568_Desc = "vs_3_sw : mad is allowed in a 24 level if p0.x nop else block";
string VS_568 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_569_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_569_Desc = "vs_3_sw : mad is NOT allowed in a 25 level if p0.x nop else block";
string VS_569 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_570_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_570_Desc = "vs_3_sw : mad is allowed in a 1 level loop aL, i0 break block";
string VS_570 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"loop aL, i0 break "
	"	mad r0, v0, v1, v2 "
	"endloop ";

VS_CRITERIA VS_571_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_571_Desc = "vs_3_sw : mad is NOT allowed in a 8 level loop aL, i0 break block";
string VS_571 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								mad r0, v0, v1, v2 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_572_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_572_Desc = "vs_3_sw : mad is NOT allowed in a 16 level loop aL, i0 break block";
string VS_572 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_573_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_573_Desc = "vs_3_sw : mad is NOT allowed in a 24 level loop aL, i0 break block";
string VS_573 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_574_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_574_Desc = "vs_3_sw : mad is NOT allowed in a 25 level loop aL, i0 break block";
string VS_574 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_575_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_575_Desc = "vs_3_sw : mad is allowed in a 1 level rep i0 break block";
string VS_575 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	mad r0, v0, v1, v2 "
	"endrep ";

VS_CRITERIA VS_576_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_576_Desc = "vs_3_sw : mad is NOT allowed in a 8 level rep i0 break block";
string VS_576 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								mad r0, v0, v1, v2 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_577_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_577_Desc = "vs_3_sw : mad is NOT allowed in a 16 level rep i0 break block";
string VS_577 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_578_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_578_Desc = "vs_3_sw : mad is NOT allowed in a 24 level rep i0 break block";
string VS_578 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_579_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_579_Desc = "vs_3_sw : mad is NOT allowed in a 25 level rep i0 break block";
string VS_579 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_580_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_580_Desc = "vs_3_sw : mad is allowed in a 1 level loop aL, i0 breakp p0.x block";
string VS_580 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	mad r0, v0, v1, v2 "
	"endloop ";

VS_CRITERIA VS_581_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_581_Desc = "vs_3_sw : mad is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string VS_581 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								mad r0, v0, v1, v2 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_582_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_582_Desc = "vs_3_sw : mad is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string VS_582 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_583_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_583_Desc = "vs_3_sw : mad is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string VS_583 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_584_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_584_Desc = "vs_3_sw : mad is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string VS_584 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_585_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_585_Desc = "vs_3_sw : mad is allowed in a 1 level rep i0 breakp p0.x block";
string VS_585 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"rep i0 breakp p0.x "
	"	mad r0, v0, v1, v2 "
	"endrep ";

VS_CRITERIA VS_586_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_586_Desc = "vs_3_sw : mad is NOT allowed in a 8 level rep i0 breakp p0.x block";
string VS_586 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								mad r0, v0, v1, v2 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_587_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_587_Desc = "vs_3_sw : mad is NOT allowed in a 16 level rep i0 breakp p0.x block";
string VS_587 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_fog v1 "
	"dcl_depth v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_588_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_588_Desc = "vs_3_sw : mad is NOT allowed in a 24 level rep i0 breakp p0.x block";
string VS_588 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_589_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_589_Desc = "vs_3_sw : mad is NOT allowed in a 25 level rep i0 breakp p0.x block";
string VS_589 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_590_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_590_Desc = "vs_3_sw : mad is allowed in a 1 level loop aL, i0 break_le c0.x, c0.y block";
string VS_590 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	mad r0, v0, v1, v2 "
	"endloop ";

VS_CRITERIA VS_591_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_591_Desc = "vs_3_sw : mad is NOT allowed in a 8 level loop aL, i0 break_le c0.x, c0.y block";
string VS_591 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		loop aL, i0 break_le c0.x, c0.y "
	"			loop aL, i0 break_le c0.x, c0.y "
	"				loop aL, i0 break_le c0.x, c0.y "
	"					loop aL, i0 break_le c0.x, c0.y "
	"						loop aL, i0 break_le c0.x, c0.y "
	"							loop aL, i0 break_le c0.x, c0.y "
	"								mad r0, v0, v1, v2 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_592_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_592_Desc = "vs_3_sw : mad is NOT allowed in a 16 level loop aL, i0 break_le c0.x, c0.y block";
string VS_592 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_593_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_593_Desc = "vs_3_sw : mad is NOT allowed in a 24 level loop aL, i0 break_le c0.x, c0.y block";
string VS_593 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_594_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_594_Desc = "vs_3_sw : mad is NOT allowed in a 25 level loop aL, i0 break_le c0.x, c0.y block";
string VS_594 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_595_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_595_Desc = "vs_3_sw : mad is allowed in a 1 level rep i0 break_le c0.x, c0.y block";
string VS_595 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"rep i0 break_le c0.x, c0.y "
	"	mad r0, v0, v1, v2 "
	"endrep ";

VS_CRITERIA VS_596_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_596_Desc = "vs_3_sw : mad is NOT allowed in a 8 level rep i0 break_le c0.x, c0.y block";
string VS_596 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		rep i0 break_le c0.x, c0.y "
	"			rep i0 break_le c0.x, c0.y "
	"				rep i0 break_le c0.x, c0.y "
	"					rep i0 break_le c0.x, c0.y "
	"						rep i0 break_le c0.x, c0.y "
	"							rep i0 break_le c0.x, c0.y "
	"								mad r0, v0, v1, v2 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_597_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_597_Desc = "vs_3_sw : mad is NOT allowed in a 16 level rep i0 break_le c0.x, c0.y block";
string VS_597 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
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
	"																mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_598_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_598_Desc = "vs_3_sw : mad is NOT allowed in a 24 level rep i0 break_le c0.x, c0.y block";
string VS_598 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
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
	"																								mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_599_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_599_Desc = "vs_3_sw : mad is NOT allowed in a 25 level rep i0 break_le c0.x, c0.y block";
string VS_599 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
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
	"																									mad r0, v0, v1, v2 "
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

VS_CRITERIA VS_600_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_600_Desc = "vs_3_sw : mad is allowed in a if b0 and if b0 block";
string VS_600 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_601_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_601_Desc = "vs_3_sw : mad is allowed in a if b0 and if b0 nop else block";
string VS_601 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_602_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_602_Desc = "vs_3_sw : mad is allowed in a if b0 and loop aL, i0 block";
string VS_602 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_603_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_603_Desc = "vs_3_sw : mad is allowed in a if b0 and rep i0 block";
string VS_603 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_604_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_604_Desc = "vs_3_sw : mad is allowed in a if b0 and if_lt c0.x, c0.y block";
string VS_604 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_605_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_605_Desc = "vs_3_sw : mad is allowed in a if b0 and if_lt c0.x, c0.y nop else block";
string VS_605 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_606_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_606_Desc = "vs_3_sw : mad is allowed in a if b0 and if p0.x block";
string VS_606 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_607_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_607_Desc = "vs_3_sw : mad is allowed in a if b0 and if p0.x nop else block";
string VS_607 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_608_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_608_Desc = "vs_3_sw : mad is allowed in a if b0 and loop aL, i0 break block";
string VS_608 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_609_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_609_Desc = "vs_3_sw : mad is allowed in a if b0 and rep i0 break block";
string VS_609 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_610_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_610_Desc = "vs_3_sw : mad is allowed in a if b0 and loop aL, i0 breakp p0.x block";
string VS_610 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_611_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_611_Desc = "vs_3_sw : mad is allowed in a if b0 and rep i0 breakp p0.x block";
string VS_611 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_612_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_612_Desc = "vs_3_sw : mad is allowed in a if b0 and loop aL, i0 break_le c0.x, c0.y block";
string VS_612 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_613_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_613_Desc = "vs_3_sw : mad is allowed in a if b0 and rep i0 break_le c0.x, c0.y block";
string VS_613 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_614_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_614_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and if b0 block";
string VS_614 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_615_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_615_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and if b0 nop else block";
string VS_615 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_616_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_616_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and loop aL, i0 block";
string VS_616 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_617_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_617_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and rep i0 block";
string VS_617 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_618_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_618_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and if_lt c0.x, c0.y block";
string VS_618 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_619_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_619_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and if_lt c0.x, c0.y nop else block";
string VS_619 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_620_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_620_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and if p0.x block";
string VS_620 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_621_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_621_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and if p0.x nop else block";
string VS_621 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_622_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_622_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and loop aL, i0 break block";
string VS_622 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_623_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_623_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and rep i0 break block";
string VS_623 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_624_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_624_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string VS_624 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_625_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_625_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string VS_625 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_626_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_626_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and loop aL, i0 break_le c0.x, c0.y block";
string VS_626 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if b0 nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_627_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_627_Desc = "vs_3_sw : mad is allowed in a if b0 nop else and rep i0 break_le c0.x, c0.y block";
string VS_627 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if b0 nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_628_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_628_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and if b0 block";
string VS_628 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_629_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_629_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and if b0 nop else block";
string VS_629 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_630_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_630_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and loop aL, i0 block";
string VS_630 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_631_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_631_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and rep i0 block";
string VS_631 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_632_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_632_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and if_lt c0.x, c0.y block";
string VS_632 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_633_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_633_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and if_lt c0.x, c0.y nop else block";
string VS_633 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_634_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_634_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and if p0.x block";
string VS_634 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_635_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_635_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and if p0.x nop else block";
string VS_635 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_636_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_636_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and loop aL, i0 break block";
string VS_636 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_637_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_637_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and rep i0 break block";
string VS_637 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_638_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_638_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string VS_638 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_639_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_639_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string VS_639 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_640_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_640_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and loop aL, i0 break_le c0.x, c0.y block";
string VS_640 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_641_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_641_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 and rep i0 break_le c0.x, c0.y block";
string VS_641 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_642_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_642_Desc = "vs_3_sw : mad is allowed in a rep i0 and if b0 block";
string VS_642 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_643_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_643_Desc = "vs_3_sw : mad is allowed in a rep i0 and if b0 nop else block";
string VS_643 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_644_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_644_Desc = "vs_3_sw : mad is allowed in a rep i0 and loop aL, i0 block";
string VS_644 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_645_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_645_Desc = "vs_3_sw : mad is allowed in a rep i0 and rep i0 block";
string VS_645 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_646_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_646_Desc = "vs_3_sw : mad is allowed in a rep i0 and if_lt c0.x, c0.y block";
string VS_646 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_647_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_647_Desc = "vs_3_sw : mad is allowed in a rep i0 and if_lt c0.x, c0.y nop else block";
string VS_647 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_648_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_648_Desc = "vs_3_sw : mad is allowed in a rep i0 and if p0.x block";
string VS_648 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_649_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_649_Desc = "vs_3_sw : mad is allowed in a rep i0 and if p0.x nop else block";
string VS_649 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_650_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_650_Desc = "vs_3_sw : mad is allowed in a rep i0 and loop aL, i0 break block";
string VS_650 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_651_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_651_Desc = "vs_3_sw : mad is allowed in a rep i0 and rep i0 break block";
string VS_651 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_652_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_652_Desc = "vs_3_sw : mad is allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string VS_652 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_653_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_653_Desc = "vs_3_sw : mad is allowed in a rep i0 and rep i0 breakp p0.x block";
string VS_653 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_654_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_654_Desc = "vs_3_sw : mad is allowed in a rep i0 and loop aL, i0 break_le c0.x, c0.y block";
string VS_654 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_655_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_655_Desc = "vs_3_sw : mad is allowed in a rep i0 and rep i0 break_le c0.x, c0.y block";
string VS_655 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_656_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_656_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and if b0 block";
string VS_656 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_657_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_657_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and if b0 nop else block";
string VS_657 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_658_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_658_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and loop aL, i0 block";
string VS_658 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_659_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_659_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and rep i0 block";
string VS_659 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_660_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_660_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and if_lt c0.x, c0.y block";
string VS_660 = 
	"vs_3_sw "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_661_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_661_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string VS_661 = 
	"vs_3_sw "
	"dcl_texcoord v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_662_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_662_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and if p0.x block";
string VS_662 = 
	"vs_3_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_663_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_663_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and if p0.x nop else block";
string VS_663 = 
	"vs_3_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_664_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_664_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and loop aL, i0 break block";
string VS_664 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_665_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_665_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and rep i0 break block";
string VS_665 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_666_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_666_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_666 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_667_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_667_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and rep i0 breakp p0.x block";
string VS_667 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_668_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_668_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string VS_668 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_669_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_669_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string VS_669 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_670_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_670_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if b0 block";
string VS_670 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_671_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_671_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if b0 nop else block";
string VS_671 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_672_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_672_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 block";
string VS_672 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_673_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_673_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 block";
string VS_673 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_674_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_674_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if_lt c0.x, c0.y block";
string VS_674 = 
	"vs_3_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_675_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_675_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if_lt c0.x, c0.y nop else block";
string VS_675 = 
	"vs_3_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_676_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_676_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if p0.x block";
string VS_676 = 
	"vs_3_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_677_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_677_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and if p0.x nop else block";
string VS_677 = 
	"vs_3_sw "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_678_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_678_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 break block";
string VS_678 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_679_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_679_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 break block";
string VS_679 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_680_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_680_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string VS_680 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_681_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_681_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 breakp p0.x block";
string VS_681 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_682_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_682_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 break_le c0.x, c0.y block";
string VS_682 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_683_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_683_Desc = "vs_3_sw : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 break_le c0.x, c0.y block";
string VS_683 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_684_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_684_Desc = "vs_3_sw : mad is allowed in a if p0.x and if b0 block";
string VS_684 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_685_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_685_Desc = "vs_3_sw : mad is allowed in a if p0.x and if b0 nop else block";
string VS_685 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_686_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_686_Desc = "vs_3_sw : mad is allowed in a if p0.x and loop aL, i0 block";
string VS_686 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_687_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_687_Desc = "vs_3_sw : mad is allowed in a if p0.x and rep i0 block";
string VS_687 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_688_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_688_Desc = "vs_3_sw : mad is allowed in a if p0.x and if_lt c0.x, c0.y block";
string VS_688 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_689_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_689_Desc = "vs_3_sw : mad is allowed in a if p0.x and if_lt c0.x, c0.y nop else block";
string VS_689 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_690_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_690_Desc = "vs_3_sw : mad is allowed in a if p0.x and if p0.x block";
string VS_690 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_691_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_691_Desc = "vs_3_sw : mad is allowed in a if p0.x and if p0.x nop else block";
string VS_691 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_692_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_692_Desc = "vs_3_sw : mad is allowed in a if p0.x and loop aL, i0 break block";
string VS_692 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_693_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_693_Desc = "vs_3_sw : mad is allowed in a if p0.x and rep i0 break block";
string VS_693 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_694_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_694_Desc = "vs_3_sw : mad is allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string VS_694 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_695_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_695_Desc = "vs_3_sw : mad is allowed in a if p0.x and rep i0 breakp p0.x block";
string VS_695 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_696_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_696_Desc = "vs_3_sw : mad is allowed in a if p0.x and loop aL, i0 break_le c0.x, c0.y block";
string VS_696 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_697_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_697_Desc = "vs_3_sw : mad is allowed in a if p0.x and rep i0 break_le c0.x, c0.y block";
string VS_697 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_698_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_698_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and if b0 block";
string VS_698 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_699_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_699_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and if b0 nop else block";
string VS_699 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_700_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_700_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and loop aL, i0 block";
string VS_700 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_701_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_701_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and rep i0 block";
string VS_701 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_702_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_702_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and if_lt c0.x, c0.y block";
string VS_702 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_703_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_703_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and if_lt c0.x, c0.y nop else block";
string VS_703 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_704_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_704_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and if p0.x block";
string VS_704 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_705_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_705_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and if p0.x nop else block";
string VS_705 = 
	"vs_3_sw "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endif ";

VS_CRITERIA VS_706_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_706_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and loop aL, i0 break block";
string VS_706 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_707_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_707_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and rep i0 break block";
string VS_707 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"if p0.x nop else "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_708_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_708_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string VS_708 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_709_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_709_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string VS_709 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_710_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_710_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and loop aL, i0 break_le c0.x, c0.y block";
string VS_710 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"if p0.x nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_711_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_711_Desc = "vs_3_sw : mad is allowed in a if p0.x nop else and rep i0 break_le c0.x, c0.y block";
string VS_711 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"if p0.x nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_712_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_712_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and if b0 block";
string VS_712 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_713_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_713_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and if b0 nop else block";
string VS_713 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_714_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_714_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and loop aL, i0 block";
string VS_714 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_715_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_715_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and rep i0 block";
string VS_715 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_716_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_716_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and if_lt c0.x, c0.y block";
string VS_716 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_717_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_717_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and if_lt c0.x, c0.y nop else block";
string VS_717 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_718_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_718_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and if p0.x block";
string VS_718 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_719_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_719_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and if p0.x nop else block";
string VS_719 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_720_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_720_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and loop aL, i0 break block";
string VS_720 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_721_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_721_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and rep i0 break block";
string VS_721 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_722_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_722_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string VS_722 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_723_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_723_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string VS_723 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_724_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_724_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and loop aL, i0 break_le c0.x, c0.y block";
string VS_724 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_725_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_725_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break and rep i0 break_le c0.x, c0.y block";
string VS_725 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"loop aL, i0 break "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_726_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_726_Desc = "vs_3_sw : mad is allowed in a rep i0 break and if b0 block";
string VS_726 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"rep i0 break "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_727_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_727_Desc = "vs_3_sw : mad is allowed in a rep i0 break and if b0 nop else block";
string VS_727 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"rep i0 break "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_728_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_728_Desc = "vs_3_sw : mad is allowed in a rep i0 break and loop aL, i0 block";
string VS_728 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"rep i0 break "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_729_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_729_Desc = "vs_3_sw : mad is allowed in a rep i0 break and rep i0 block";
string VS_729 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_color v1 "
	"dcl_tangent v2 "
	"rep i0 break "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_730_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_730_Desc = "vs_3_sw : mad is allowed in a rep i0 break and if_lt c0.x, c0.y block";
string VS_730 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_731_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_731_Desc = "vs_3_sw : mad is allowed in a rep i0 break and if_lt c0.x, c0.y nop else block";
string VS_731 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_732_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_732_Desc = "vs_3_sw : mad is allowed in a rep i0 break and if p0.x block";
string VS_732 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_733_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_733_Desc = "vs_3_sw : mad is allowed in a rep i0 break and if p0.x nop else block";
string VS_733 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_734_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_734_Desc = "vs_3_sw : mad is allowed in a rep i0 break and loop aL, i0 break block";
string VS_734 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_735_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_735_Desc = "vs_3_sw : mad is allowed in a rep i0 break and rep i0 break block";
string VS_735 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_736_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_736_Desc = "vs_3_sw : mad is allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string VS_736 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_737_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_737_Desc = "vs_3_sw : mad is allowed in a rep i0 break and rep i0 breakp p0.x block";
string VS_737 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_738_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_738_Desc = "vs_3_sw : mad is allowed in a rep i0 break and loop aL, i0 break_le c0.x, c0.y block";
string VS_738 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_739_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_739_Desc = "vs_3_sw : mad is allowed in a rep i0 break and rep i0 break_le c0.x, c0.y block";
string VS_739 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"rep i0 break "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_740_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_740_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and if b0 block";
string VS_740 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_741_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_741_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string VS_741 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_742_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_742_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string VS_742 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_743_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_743_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string VS_743 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_744_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_744_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and if_lt c0.x, c0.y block";
string VS_744 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_745_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_745_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and if_lt c0.x, c0.y nop else block";
string VS_745 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_746_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_746_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string VS_746 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_747_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_747_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string VS_747 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_748_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_748_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string VS_748 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_749_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_749_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string VS_749 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_750_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_750_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string VS_750 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_751_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_751_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string VS_751 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"dcl_tangent v1 "
	"dcl_depth v2 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_752_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_752_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_le c0.x, c0.y block";
string VS_752 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_753_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_753_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 breakp p0.x and rep i0 break_le c0.x, c0.y block";
string VS_753 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_754_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_754_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and if b0 block";
string VS_754 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_755_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_755_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string VS_755 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_756_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_756_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string VS_756 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_757_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_757_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and rep i0 block";
string VS_757 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_758_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_758_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and if_lt c0.x, c0.y block";
string VS_758 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_759_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_759_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and if_lt c0.x, c0.y nop else block";
string VS_759 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_760_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_760_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and if p0.x block";
string VS_760 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_761_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_761_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string VS_761 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_762_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_762_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string VS_762 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_763_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_763_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and rep i0 break block";
string VS_763 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_764_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_764_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string VS_764 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_765_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_765_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string VS_765 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_766_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_766_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and loop aL, i0 break_le c0.x, c0.y block";
string VS_766 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_767_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_767_Desc = "vs_3_sw : mad is allowed in a rep i0 breakp p0.x and rep i0 break_le c0.x, c0.y block";
string VS_767 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"rep i0 breakp p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_768_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_768_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if b0 block";
string VS_768 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_769_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_769_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if b0 nop else block";
string VS_769 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_770_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_770_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 block";
string VS_770 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_771_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_771_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 block";
string VS_771 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_772_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_772_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if_lt c0.x, c0.y block";
string VS_772 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_773_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_773_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string VS_773 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_774_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_774_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if p0.x block";
string VS_774 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_775_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_775_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if p0.x nop else block";
string VS_775 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_776_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_776_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 break block";
string VS_776 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_777_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_777_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 break block";
string VS_777 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_778_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_778_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_778 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_779_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_779_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 breakp p0.x block";
string VS_779 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_780_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_780_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string VS_780 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_781_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_781_Desc = "vs_3_sw : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string VS_781 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_782_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_782_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if b0 block";
string VS_782 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 break_le c0.x, c0.y "
	"	if b0 "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_783_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_783_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if b0 nop else block";
string VS_783 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 break_le c0.x, c0.y "
	"	if b0 nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_784_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_784_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 block";
string VS_784 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_785_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_785_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and rep i0 block";
string VS_785 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_786_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_786_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if_lt c0.x, c0.y block";
string VS_786 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 break_le c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_787_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_787_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string VS_787 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 break_le c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_788_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_788_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if p0.x block";
string VS_788 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"dcl_depth v1 "
	"dcl_color v2 "
	"rep i0 break_le c0.x, c0.y "
	"	if p0.x "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_789_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_789_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and if p0.x nop else block";
string VS_789 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"dcl_tangent v2 "
	"rep i0 break_le c0.x, c0.y "
	"	if p0.x nop else "
	"		mad r0, v0, v1, v2 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_790_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_790_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 break block";
string VS_790 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"dcl_tangent v2 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 break "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_791_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_791_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and rep i0 break block";
string VS_791 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"dcl_tangent v2 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_792_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_792_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_792 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"dcl_tangent v2 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_793_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_793_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and rep i0 breakp p0.x block";
string VS_793 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"dcl_tangent v2 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_794_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_794_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string VS_794 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"dcl_tangent v2 "
	"rep i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_795_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_795_Desc = "vs_3_sw : mad is allowed in a rep i0 break_le c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string VS_795 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"dcl_tangent v2 "
	"rep i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, v1, v2 "
	"	endrep "
	"endrep ";


