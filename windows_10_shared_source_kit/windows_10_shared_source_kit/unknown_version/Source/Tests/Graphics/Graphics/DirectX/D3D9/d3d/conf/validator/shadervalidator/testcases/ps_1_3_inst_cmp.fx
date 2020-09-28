#include "Test_Include.fx"

int Test_Count = 375;

string TestInfo
<
	string TestType = "PS";
>
= "ps_1_3_inst_cmp";

PS_CRITERIA PS_001_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_1_3 : cmp source reg combination v0, v1, r0 is NOT allowed";
string PS_001 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r0, v0, v1, r0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_1_3 : cmp source reg combination v0, v1, c0 is allowed";
string PS_002 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"cmp r0, v0, v1, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_003_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_1_3 : cmp source reg combination v0, v1, t0 is allowed";
string PS_003 = 
	"ps_1_3 "
	"texcoord t0 "
	"cmp r0, v0, v1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_004_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_1_3 : cmp source reg combination v0, r0, v1 is NOT allowed";
string PS_004 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r0, v0, r0, v1 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_1_3 : cmp source reg combination v0, r0, r1 is NOT allowed";
string PS_005 = 
	"ps_1_3 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, v0, r0, r1 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_1_3 : cmp source reg combination v0, r0, c0 is NOT allowed";
string PS_006 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, v0, r0, c0 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_1_3 : cmp source reg combination v0, r0, t0 is NOT allowed";
string PS_007 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, v0, r0, t0 ";

PS_CRITERIA PS_008_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_1_3 : cmp source reg combination v0, c0, v1 is allowed";
string PS_008 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"cmp r0, v0, c0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_1_3 : cmp source reg combination v0, c0, r0 is NOT allowed";
string PS_009 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, v0, c0, r0 ";

PS_CRITERIA PS_010_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_1_3 : cmp source reg combination v0, c0, c1 is allowed";
string PS_010 = 
	"ps_1_3 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"cmp r0, v0, c0, c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_011_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_1_3 : cmp source reg combination v0, c0, t0 is allowed";
string PS_011 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"cmp r0, v0, c0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_012_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_1_3 : cmp source reg combination v0, t0, v1 is allowed";
string PS_012 = 
	"ps_1_3 "
	"texcoord t0 "
	"cmp r0, v0, t0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_1_3 : cmp source reg combination v0, t0, r0 is NOT allowed";
string PS_013 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, v0, t0, r0 ";

PS_CRITERIA PS_014_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_1_3 : cmp source reg combination v0, t0, c0 is allowed";
string PS_014 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"cmp r0, v0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_015_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_1_3 : cmp source reg combination v0, t0, t1 is allowed";
string PS_015 = 
	"ps_1_3 "
	"texcoord t0 "
	"texcoord t1 "
	"cmp r0, v0, t0, t1 "
	"mov r0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_1_3 : cmp source reg combination r0, v0, v1 is NOT allowed";
string PS_016 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r0, r0, v0, v1 ";

PS_CRITERIA PS_017_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_1_3 : cmp source reg combination r0, v0, r1 is NOT allowed";
string PS_017 = 
	"ps_1_3 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, v0, r1 ";

PS_CRITERIA PS_018_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_1_3 : cmp source reg combination r0, v0, c0 is NOT allowed";
string PS_018 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, r0, v0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_1_3 : cmp source reg combination r0, v0, t0 is NOT allowed";
string PS_019 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, r0, v0, t0 ";

PS_CRITERIA PS_020_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_1_3 : cmp source reg combination r0, r1, v0 is NOT allowed";
string PS_020 = 
	"ps_1_3 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, r1, v0 ";

PS_CRITERIA PS_021_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_1_3 : cmp source reg combination r0, r1, c0 is NOT allowed";
string PS_021 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, r1, c0 ";

PS_CRITERIA PS_022_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_1_3 : cmp source reg combination r0, r1, t0 is NOT allowed";
string PS_022 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, r1, t0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_1_3 : cmp source reg combination r0, c0, v0 is NOT allowed";
string PS_023 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, r0, c0, v0 ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_1_3 : cmp source reg combination r0, c0, r1 is NOT allowed";
string PS_024 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1 ";

PS_CRITERIA PS_025_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_1_3 : cmp source reg combination r0, c0, c1 is NOT allowed";
string PS_025 = 
	"ps_1_3 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, r0, c0, c1 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_1_3 : cmp source reg combination r0, c0, t0 is NOT allowed";
string PS_026 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, r0, c0, t0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_1_3 : cmp source reg combination r0, t0, v0 is NOT allowed";
string PS_027 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, r0, t0, v0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1 is NOT allowed";
string PS_028 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_1_3 : cmp source reg combination r0, t0, c0 is NOT allowed";
string PS_029 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, r0, t0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_1_3 : cmp source reg combination r0, t0, t1 is NOT allowed";
string PS_030 = 
	"ps_1_3 "
	"texcoord t0 "
	"texcoord t1 "
	"mov r0, c0 "
	"cmp r0, r0, t0, t1 ";

PS_CRITERIA PS_031_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_1_3 : cmp source reg combination c0, v0, v1 is allowed";
string PS_031 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"cmp r0, c0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_1_3 : cmp source reg combination c0, v0, r0 is NOT allowed";
string PS_032 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, c0, v0, r0 ";

PS_CRITERIA PS_033_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_1_3 : cmp source reg combination c0, v0, c1 is allowed";
string PS_033 = 
	"ps_1_3 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"cmp r0, c0, v0, c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_034_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_1_3 : cmp source reg combination c0, v0, t0 is allowed";
string PS_034 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"cmp r0, c0, v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_1_3 : cmp source reg combination c0, r0, v0 is NOT allowed";
string PS_035 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, c0, r0, v0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_1_3 : cmp source reg combination c0, r0, r1 is NOT allowed";
string PS_036 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, c0, r0, r1 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_1_3 : cmp source reg combination c0, r0, c1 is NOT allowed";
string PS_037 = 
	"ps_1_3 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, c0, r0, c1 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_1_3 : cmp source reg combination c0, r0, t0 is NOT allowed";
string PS_038 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, c0, r0, t0 ";

PS_CRITERIA PS_039_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_1_3 : cmp source reg combination c0, c1, v0 is allowed";
string PS_039 = 
	"ps_1_3 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"cmp r0, c0, c1, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_1_3 : cmp source reg combination c0, c1, r0 is NOT allowed";
string PS_040 = 
	"ps_1_3 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, c0, c1, r0 ";

PS_CRITERIA PS_041_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_1_3 : cmp source reg combination c0, c1, t0 is allowed";
string PS_041 = 
	"ps_1_3 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"cmp r0, c0, c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_042_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_1_3 : cmp source reg combination c0, t0, v0 is allowed";
string PS_042 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"cmp r0, c0, t0, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_1_3 : cmp source reg combination c0, t0, r0 is NOT allowed";
string PS_043 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, c0, t0, r0 ";

PS_CRITERIA PS_044_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_1_3 : cmp source reg combination c0, t0, c1 is allowed";
string PS_044 = 
	"ps_1_3 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"cmp r0, c0, t0, c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_045_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_1_3 : cmp source reg combination c0, t0, t1 is allowed";
string PS_045 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texcoord t1 "
	"cmp r0, c0, t0, t1 "
	"mov r0, c0 ";

PS_CRITERIA PS_046_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_1_3 : cmp source reg combination t0, v0, v1 is allowed";
string PS_046 = 
	"ps_1_3 "
	"texcoord t0 "
	"cmp r0, t0, v0, v1 "
	"mov r0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_1_3 : cmp source reg combination t0, v0, r0 is NOT allowed";
string PS_047 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, t0, v0, r0 ";

PS_CRITERIA PS_048_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_1_3 : cmp source reg combination t0, v0, c0 is allowed";
string PS_048 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"cmp r0, t0, v0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_049_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_1_3 : cmp source reg combination t0, v0, t1 is allowed";
string PS_049 = 
	"ps_1_3 "
	"texcoord t0 "
	"texcoord t1 "
	"cmp r0, t0, v0, t1 "
	"mov r0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_1_3 : cmp source reg combination t0, r0, v0 is NOT allowed";
string PS_050 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, t0, r0, v0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_1_3 : cmp source reg combination t0, r0, r1 is NOT allowed";
string PS_051 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, t0, r0, r1 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_1_3 : cmp source reg combination t0, r0, c0 is NOT allowed";
string PS_052 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, t0, r0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_1_3 : cmp source reg combination t0, r0, t1 is NOT allowed";
string PS_053 = 
	"ps_1_3 "
	"texcoord t0 "
	"texcoord t1 "
	"mov r0, c0 "
	"cmp r0, t0, r0, t1 ";

PS_CRITERIA PS_054_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_1_3 : cmp source reg combination t0, c0, v0 is allowed";
string PS_054 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"cmp r0, t0, c0, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_1_3 : cmp source reg combination t0, c0, r0 is NOT allowed";
string PS_055 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"mov r0, c0 "
	"cmp r0, t0, c0, r0 ";

PS_CRITERIA PS_056_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_1_3 : cmp source reg combination t0, c0, c1 is allowed";
string PS_056 = 
	"ps_1_3 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"cmp r0, t0, c0, c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_057_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_1_3 : cmp source reg combination t0, c0, t1 is allowed";
string PS_057 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texcoord t1 "
	"cmp r0, t0, c0, t1 "
	"mov r0, c0 ";

PS_CRITERIA PS_058_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_1_3 : cmp source reg combination t0, t1, v0 is allowed";
string PS_058 = 
	"ps_1_3 "
	"texcoord t0 "
	"texcoord t1 "
	"cmp r0, t0, t1, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_1_3 : cmp source reg combination t0, t1, r0 is NOT allowed";
string PS_059 = 
	"ps_1_3 "
	"texcoord t0 "
	"texcoord t1 "
	"mov r0, c0 "
	"cmp r0, t0, t1, r0 ";

PS_CRITERIA PS_060_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_1_3 : cmp source reg combination t0, t1, c0 is allowed";
string PS_060 = 
	"ps_1_3 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texcoord t1 "
	"cmp r0, t0, t1, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_061_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_1_3 : cmp source reg combination t0, t1, t2 is allowed";
string PS_061 = 
	"ps_1_3 "
	"texcoord t0 "
	"texcoord t1 "
	"texcoord t2 "
	"cmp r0, t0, t1, t2 "
	"mov r0, c0 ";

PS_CRITERIA PS_062_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bias, r1_bias is NOT allowed";
string PS_062 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bias, r1_bias ";

PS_CRITERIA PS_063_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bias, 1 - r1 is NOT allowed";
string PS_063 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bias, 1 - r1 ";

PS_CRITERIA PS_064_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bias, -r1 is NOT allowed";
string PS_064 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bias, -r1 ";

PS_CRITERIA PS_065_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bias, r1_bx2 is NOT allowed";
string PS_065 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bias, r1_bx2 ";

PS_CRITERIA PS_066_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bias, -r1_bx2 is NOT allowed";
string PS_066 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bias, -r1_bx2 ";

PS_CRITERIA PS_067_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bias, r1 is NOT allowed";
string PS_067 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bias, r1 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_1_3 : cmp source reg combination r0_bias, 1 - t0, r1_bias is NOT allowed";
string PS_068 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, 1 - t0, r1_bias ";

PS_CRITERIA PS_069_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_1_3 : cmp source reg combination r0_bias, 1 - t0, 1 - r1 is NOT allowed";
string PS_069 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, 1 - t0, 1 - r1 ";

PS_CRITERIA PS_070_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_1_3 : cmp source reg combination r0_bias, 1 - t0, -r1 is NOT allowed";
string PS_070 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, 1 - t0, -r1 ";

PS_CRITERIA PS_071_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_1_3 : cmp source reg combination r0_bias, 1 - t0, r1_bx2 is NOT allowed";
string PS_071 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, 1 - t0, r1_bx2 ";

PS_CRITERIA PS_072_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_1_3 : cmp source reg combination r0_bias, 1 - t0, -r1_bx2 is NOT allowed";
string PS_072 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, 1 - t0, -r1_bx2 ";

PS_CRITERIA PS_073_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_1_3 : cmp source reg combination r0_bias, 1 - t0, r1 is NOT allowed";
string PS_073 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, 1 - t0, r1 ";

PS_CRITERIA PS_074_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0, r1_bias is NOT allowed";
string PS_074 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0, r1_bias ";

PS_CRITERIA PS_075_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0, 1 - r1 is NOT allowed";
string PS_075 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0, 1 - r1 ";

PS_CRITERIA PS_076_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0, -r1 is NOT allowed";
string PS_076 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0, -r1 ";

PS_CRITERIA PS_077_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0, r1_bx2 is NOT allowed";
string PS_077 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0, r1_bx2 ";

PS_CRITERIA PS_078_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0, -r1_bx2 is NOT allowed";
string PS_078 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0, -r1_bx2 ";

PS_CRITERIA PS_079_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0, r1 is NOT allowed";
string PS_079 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0, r1 ";

PS_CRITERIA PS_080_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bx2, r1_bias is NOT allowed";
string PS_080 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bx2, r1_bias ";

PS_CRITERIA PS_081_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bx2, 1 - r1 is NOT allowed";
string PS_081 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bx2, 1 - r1 ";

PS_CRITERIA PS_082_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bx2, -r1 is NOT allowed";
string PS_082 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bx2, -r1 ";

PS_CRITERIA PS_083_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bx2, r1_bx2 is NOT allowed";
string PS_083 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bx2, r1_bx2 ";

PS_CRITERIA PS_084_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bx2, -r1_bx2 is NOT allowed";
string PS_084 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_085_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0_bx2, r1 is NOT allowed";
string PS_085 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0_bx2, r1 ";

PS_CRITERIA PS_086_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0_bx2, r1_bias is NOT allowed";
string PS_086 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0_bx2, r1_bias ";

PS_CRITERIA PS_087_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0_bx2, 1 - r1 is NOT allowed";
string PS_087 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0_bx2, 1 - r1 ";

PS_CRITERIA PS_088_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0_bx2, -r1 is NOT allowed";
string PS_088 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0_bx2, -r1 ";

PS_CRITERIA PS_089_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0_bx2, r1_bx2 is NOT allowed";
string PS_089 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0_bx2, r1_bx2 ";

PS_CRITERIA PS_090_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0_bx2, -r1_bx2 is NOT allowed";
string PS_090 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_091_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_1_3 : cmp source reg combination r0_bias, -t0_bx2, r1 is NOT allowed";
string PS_091 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, -t0_bx2, r1 ";

PS_CRITERIA PS_092_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0, r1_bias is NOT allowed";
string PS_092 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0, r1_bias ";

PS_CRITERIA PS_093_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0, 1 - r1 is NOT allowed";
string PS_093 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0, 1 - r1 ";

PS_CRITERIA PS_094_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0, -r1 is NOT allowed";
string PS_094 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0, -r1 ";

PS_CRITERIA PS_095_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0, r1_bx2 is NOT allowed";
string PS_095 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0, r1_bx2 ";

PS_CRITERIA PS_096_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0, -r1_bx2 is NOT allowed";
string PS_096 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0, -r1_bx2 ";

PS_CRITERIA PS_097_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_1_3 : cmp source reg combination r0_bias, t0, r1 is NOT allowed";
string PS_097 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bias, t0, r1 ";

PS_CRITERIA PS_098_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bias, r1_bias is NOT allowed";
string PS_098 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bias, r1_bias ";

PS_CRITERIA PS_099_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bias, 1 - r1 is NOT allowed";
string PS_099 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bias, 1 - r1 ";

PS_CRITERIA PS_100_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bias, -r1 is NOT allowed";
string PS_100 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bias, -r1 ";

PS_CRITERIA PS_101_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bias, r1_bx2 is NOT allowed";
string PS_101 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bias, r1_bx2 ";

PS_CRITERIA PS_102_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bias, -r1_bx2 is NOT allowed";
string PS_102 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bias, -r1_bx2 ";

PS_CRITERIA PS_103_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bias, r1 is NOT allowed";
string PS_103 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bias, r1 ";

PS_CRITERIA PS_104_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_1_3 : cmp source reg combination 1 - r0, 1 - t0, r1_bias is NOT allowed";
string PS_104 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, 1 - t0, r1_bias ";

PS_CRITERIA PS_105_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_1_3 : cmp source reg combination 1 - r0, 1 - t0, 1 - r1 is NOT allowed";
string PS_105 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, 1 - t0, 1 - r1 ";

PS_CRITERIA PS_106_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_1_3 : cmp source reg combination 1 - r0, 1 - t0, -r1 is NOT allowed";
string PS_106 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, 1 - t0, -r1 ";

PS_CRITERIA PS_107_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_1_3 : cmp source reg combination 1 - r0, 1 - t0, r1_bx2 is NOT allowed";
string PS_107 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, 1 - t0, r1_bx2 ";

PS_CRITERIA PS_108_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_1_3 : cmp source reg combination 1 - r0, 1 - t0, -r1_bx2 is NOT allowed";
string PS_108 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, 1 - t0, -r1_bx2 ";

PS_CRITERIA PS_109_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_1_3 : cmp source reg combination 1 - r0, 1 - t0, r1 is NOT allowed";
string PS_109 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, 1 - t0, r1 ";

PS_CRITERIA PS_110_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0, r1_bias is NOT allowed";
string PS_110 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0, r1_bias ";

PS_CRITERIA PS_111_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0, 1 - r1 is NOT allowed";
string PS_111 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0, 1 - r1 ";

PS_CRITERIA PS_112_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0, -r1 is NOT allowed";
string PS_112 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0, -r1 ";

PS_CRITERIA PS_113_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0, r1_bx2 is NOT allowed";
string PS_113 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0, r1_bx2 ";

PS_CRITERIA PS_114_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0, -r1_bx2 is NOT allowed";
string PS_114 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0, -r1_bx2 ";

PS_CRITERIA PS_115_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0, r1 is NOT allowed";
string PS_115 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0, r1 ";

PS_CRITERIA PS_116_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bx2, r1_bias is NOT allowed";
string PS_116 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bx2, r1_bias ";

PS_CRITERIA PS_117_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bx2, 1 - r1 is NOT allowed";
string PS_117 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bx2, 1 - r1 ";

PS_CRITERIA PS_118_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bx2, -r1 is NOT allowed";
string PS_118 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bx2, -r1 ";

PS_CRITERIA PS_119_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bx2, r1_bx2 is NOT allowed";
string PS_119 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bx2, r1_bx2 ";

PS_CRITERIA PS_120_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bx2, -r1_bx2 is NOT allowed";
string PS_120 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_121_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0_bx2, r1 is NOT allowed";
string PS_121 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0_bx2, r1 ";

PS_CRITERIA PS_122_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0_bx2, r1_bias is NOT allowed";
string PS_122 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0_bx2, r1_bias ";

PS_CRITERIA PS_123_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0_bx2, 1 - r1 is NOT allowed";
string PS_123 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0_bx2, 1 - r1 ";

PS_CRITERIA PS_124_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0_bx2, -r1 is NOT allowed";
string PS_124 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0_bx2, -r1 ";

PS_CRITERIA PS_125_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_125_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0_bx2, r1_bx2 is NOT allowed";
string PS_125 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0_bx2, r1_bx2 ";

PS_CRITERIA PS_126_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0_bx2, -r1_bx2 is NOT allowed";
string PS_126 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_127_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_1_3 : cmp source reg combination 1 - r0, -t0_bx2, r1 is NOT allowed";
string PS_127 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, -t0_bx2, r1 ";

PS_CRITERIA PS_128_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0, r1_bias is NOT allowed";
string PS_128 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0, r1_bias ";

PS_CRITERIA PS_129_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0, 1 - r1 is NOT allowed";
string PS_129 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0, 1 - r1 ";

PS_CRITERIA PS_130_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0, -r1 is NOT allowed";
string PS_130 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0, -r1 ";

PS_CRITERIA PS_131_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0, r1_bx2 is NOT allowed";
string PS_131 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0, r1_bx2 ";

PS_CRITERIA PS_132_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0, -r1_bx2 is NOT allowed";
string PS_132 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0, -r1_bx2 ";

PS_CRITERIA PS_133_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_1_3 : cmp source reg combination 1 - r0, t0, r1 is NOT allowed";
string PS_133 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, 1 - r0, t0, r1 ";

PS_CRITERIA PS_134_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bias, r1_bias is NOT allowed";
string PS_134 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bias, r1_bias ";

PS_CRITERIA PS_135_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bias, 1 - r1 is NOT allowed";
string PS_135 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bias, 1 - r1 ";

PS_CRITERIA PS_136_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_136_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bias, -r1 is NOT allowed";
string PS_136 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bias, -r1 ";

PS_CRITERIA PS_137_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_137_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bias, r1_bx2 is NOT allowed";
string PS_137 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bias, r1_bx2 ";

PS_CRITERIA PS_138_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_138_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bias, -r1_bx2 is NOT allowed";
string PS_138 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bias, -r1_bx2 ";

PS_CRITERIA PS_139_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_139_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bias, r1 is NOT allowed";
string PS_139 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bias, r1 ";

PS_CRITERIA PS_140_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_140_Desc = "ps_1_3 : cmp source reg combination -r0, 1 - t0, r1_bias is NOT allowed";
string PS_140 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, 1 - t0, r1_bias ";

PS_CRITERIA PS_141_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_141_Desc = "ps_1_3 : cmp source reg combination -r0, 1 - t0, 1 - r1 is NOT allowed";
string PS_141 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, 1 - t0, 1 - r1 ";

PS_CRITERIA PS_142_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_142_Desc = "ps_1_3 : cmp source reg combination -r0, 1 - t0, -r1 is NOT allowed";
string PS_142 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, 1 - t0, -r1 ";

PS_CRITERIA PS_143_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_143_Desc = "ps_1_3 : cmp source reg combination -r0, 1 - t0, r1_bx2 is NOT allowed";
string PS_143 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, 1 - t0, r1_bx2 ";

PS_CRITERIA PS_144_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_144_Desc = "ps_1_3 : cmp source reg combination -r0, 1 - t0, -r1_bx2 is NOT allowed";
string PS_144 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, 1 - t0, -r1_bx2 ";

PS_CRITERIA PS_145_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_145_Desc = "ps_1_3 : cmp source reg combination -r0, 1 - t0, r1 is NOT allowed";
string PS_145 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, 1 - t0, r1 ";

PS_CRITERIA PS_146_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_146_Desc = "ps_1_3 : cmp source reg combination -r0, -t0, r1_bias is NOT allowed";
string PS_146 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0, r1_bias ";

PS_CRITERIA PS_147_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_147_Desc = "ps_1_3 : cmp source reg combination -r0, -t0, 1 - r1 is NOT allowed";
string PS_147 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0, 1 - r1 ";

PS_CRITERIA PS_148_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_148_Desc = "ps_1_3 : cmp source reg combination -r0, -t0, -r1 is NOT allowed";
string PS_148 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0, -r1 ";

PS_CRITERIA PS_149_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_149_Desc = "ps_1_3 : cmp source reg combination -r0, -t0, r1_bx2 is NOT allowed";
string PS_149 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0, r1_bx2 ";

PS_CRITERIA PS_150_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_150_Desc = "ps_1_3 : cmp source reg combination -r0, -t0, -r1_bx2 is NOT allowed";
string PS_150 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0, -r1_bx2 ";

PS_CRITERIA PS_151_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_151_Desc = "ps_1_3 : cmp source reg combination -r0, -t0, r1 is NOT allowed";
string PS_151 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0, r1 ";

PS_CRITERIA PS_152_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_152_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bx2, r1_bias is NOT allowed";
string PS_152 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bx2, r1_bias ";

PS_CRITERIA PS_153_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_153_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bx2, 1 - r1 is NOT allowed";
string PS_153 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bx2, 1 - r1 ";

PS_CRITERIA PS_154_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_154_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bx2, -r1 is NOT allowed";
string PS_154 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bx2, -r1 ";

PS_CRITERIA PS_155_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_155_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bx2, r1_bx2 is NOT allowed";
string PS_155 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bx2, r1_bx2 ";

PS_CRITERIA PS_156_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_156_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bx2, -r1_bx2 is NOT allowed";
string PS_156 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_157_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_157_Desc = "ps_1_3 : cmp source reg combination -r0, t0_bx2, r1 is NOT allowed";
string PS_157 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0_bx2, r1 ";

PS_CRITERIA PS_158_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_158_Desc = "ps_1_3 : cmp source reg combination -r0, -t0_bx2, r1_bias is NOT allowed";
string PS_158 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0_bx2, r1_bias ";

PS_CRITERIA PS_159_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_159_Desc = "ps_1_3 : cmp source reg combination -r0, -t0_bx2, 1 - r1 is NOT allowed";
string PS_159 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0_bx2, 1 - r1 ";

PS_CRITERIA PS_160_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_160_Desc = "ps_1_3 : cmp source reg combination -r0, -t0_bx2, -r1 is NOT allowed";
string PS_160 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0_bx2, -r1 ";

PS_CRITERIA PS_161_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_161_Desc = "ps_1_3 : cmp source reg combination -r0, -t0_bx2, r1_bx2 is NOT allowed";
string PS_161 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0_bx2, r1_bx2 ";

PS_CRITERIA PS_162_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_162_Desc = "ps_1_3 : cmp source reg combination -r0, -t0_bx2, -r1_bx2 is NOT allowed";
string PS_162 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_163_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_163_Desc = "ps_1_3 : cmp source reg combination -r0, -t0_bx2, r1 is NOT allowed";
string PS_163 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -t0_bx2, r1 ";

PS_CRITERIA PS_164_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_164_Desc = "ps_1_3 : cmp source reg combination -r0, t0, r1_bias is NOT allowed";
string PS_164 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0, r1_bias ";

PS_CRITERIA PS_165_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_165_Desc = "ps_1_3 : cmp source reg combination -r0, t0, 1 - r1 is NOT allowed";
string PS_165 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0, 1 - r1 ";

PS_CRITERIA PS_166_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_166_Desc = "ps_1_3 : cmp source reg combination -r0, t0, -r1 is NOT allowed";
string PS_166 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0, -r1 ";

PS_CRITERIA PS_167_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_167_Desc = "ps_1_3 : cmp source reg combination -r0, t0, r1_bx2 is NOT allowed";
string PS_167 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0, r1_bx2 ";

PS_CRITERIA PS_168_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_168_Desc = "ps_1_3 : cmp source reg combination -r0, t0, -r1_bx2 is NOT allowed";
string PS_168 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0, -r1_bx2 ";

PS_CRITERIA PS_169_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_169_Desc = "ps_1_3 : cmp source reg combination -r0, t0, r1 is NOT allowed";
string PS_169 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, t0, r1 ";

PS_CRITERIA PS_170_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_170_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bias, r1_bias is NOT allowed";
string PS_170 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bias, r1_bias ";

PS_CRITERIA PS_171_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_171_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bias, 1 - r1 is NOT allowed";
string PS_171 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bias, 1 - r1 ";

PS_CRITERIA PS_172_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_172_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bias, -r1 is NOT allowed";
string PS_172 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bias, -r1 ";

PS_CRITERIA PS_173_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_173_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bias, r1_bx2 is NOT allowed";
string PS_173 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bias, r1_bx2 ";

PS_CRITERIA PS_174_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_174_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bias, -r1_bx2 is NOT allowed";
string PS_174 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bias, -r1_bx2 ";

PS_CRITERIA PS_175_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_175_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bias, r1 is NOT allowed";
string PS_175 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bias, r1 ";

PS_CRITERIA PS_176_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_176_Desc = "ps_1_3 : cmp source reg combination r0_bx2, 1 - t0, r1_bias is NOT allowed";
string PS_176 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, 1 - t0, r1_bias ";

PS_CRITERIA PS_177_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_177_Desc = "ps_1_3 : cmp source reg combination r0_bx2, 1 - t0, 1 - r1 is NOT allowed";
string PS_177 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, 1 - t0, 1 - r1 ";

PS_CRITERIA PS_178_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_178_Desc = "ps_1_3 : cmp source reg combination r0_bx2, 1 - t0, -r1 is NOT allowed";
string PS_178 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, 1 - t0, -r1 ";

PS_CRITERIA PS_179_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_179_Desc = "ps_1_3 : cmp source reg combination r0_bx2, 1 - t0, r1_bx2 is NOT allowed";
string PS_179 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, 1 - t0, r1_bx2 ";

PS_CRITERIA PS_180_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_180_Desc = "ps_1_3 : cmp source reg combination r0_bx2, 1 - t0, -r1_bx2 is NOT allowed";
string PS_180 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, 1 - t0, -r1_bx2 ";

PS_CRITERIA PS_181_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_181_Desc = "ps_1_3 : cmp source reg combination r0_bx2, 1 - t0, r1 is NOT allowed";
string PS_181 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, 1 - t0, r1 ";

PS_CRITERIA PS_182_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_182_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0, r1_bias is NOT allowed";
string PS_182 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0, r1_bias ";

PS_CRITERIA PS_183_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_183_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0, 1 - r1 is NOT allowed";
string PS_183 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0, 1 - r1 ";

PS_CRITERIA PS_184_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_184_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0, -r1 is NOT allowed";
string PS_184 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0, -r1 ";

PS_CRITERIA PS_185_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_185_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0, r1_bx2 is NOT allowed";
string PS_185 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0, r1_bx2 ";

PS_CRITERIA PS_186_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_186_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0, -r1_bx2 is NOT allowed";
string PS_186 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0, -r1_bx2 ";

PS_CRITERIA PS_187_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_187_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0, r1 is NOT allowed";
string PS_187 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0, r1 ";

PS_CRITERIA PS_188_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_188_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bx2, r1_bias is NOT allowed";
string PS_188 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bx2, r1_bias ";

PS_CRITERIA PS_189_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_189_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bx2, 1 - r1 is NOT allowed";
string PS_189 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bx2, 1 - r1 ";

PS_CRITERIA PS_190_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_190_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bx2, -r1 is NOT allowed";
string PS_190 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bx2, -r1 ";

PS_CRITERIA PS_191_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_191_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bx2, r1_bx2 is NOT allowed";
string PS_191 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bx2, r1_bx2 ";

PS_CRITERIA PS_192_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_192_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bx2, -r1_bx2 is NOT allowed";
string PS_192 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_193_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_193_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0_bx2, r1 is NOT allowed";
string PS_193 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0_bx2, r1 ";

PS_CRITERIA PS_194_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_194_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0_bx2, r1_bias is NOT allowed";
string PS_194 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0_bx2, r1_bias ";

PS_CRITERIA PS_195_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_195_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0_bx2, 1 - r1 is NOT allowed";
string PS_195 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0_bx2, 1 - r1 ";

PS_CRITERIA PS_196_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_196_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0_bx2, -r1 is NOT allowed";
string PS_196 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0_bx2, -r1 ";

PS_CRITERIA PS_197_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_197_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0_bx2, r1_bx2 is NOT allowed";
string PS_197 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0_bx2, r1_bx2 ";

PS_CRITERIA PS_198_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_198_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0_bx2, -r1_bx2 is NOT allowed";
string PS_198 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_199_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_199_Desc = "ps_1_3 : cmp source reg combination r0_bx2, -t0_bx2, r1 is NOT allowed";
string PS_199 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, -t0_bx2, r1 ";

PS_CRITERIA PS_200_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_200_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0, r1_bias is NOT allowed";
string PS_200 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0, r1_bias ";

PS_CRITERIA PS_201_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_201_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0, 1 - r1 is NOT allowed";
string PS_201 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0, 1 - r1 ";

PS_CRITERIA PS_202_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_202_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0, -r1 is NOT allowed";
string PS_202 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0, -r1 ";

PS_CRITERIA PS_203_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_203_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0, r1_bx2 is NOT allowed";
string PS_203 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0, r1_bx2 ";

PS_CRITERIA PS_204_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_204_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0, -r1_bx2 is NOT allowed";
string PS_204 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0, -r1_bx2 ";

PS_CRITERIA PS_205_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_205_Desc = "ps_1_3 : cmp source reg combination r0_bx2, t0, r1 is NOT allowed";
string PS_205 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_bx2, t0, r1 ";

PS_CRITERIA PS_206_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_206_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bias, r1_bias is NOT allowed";
string PS_206 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bias, r1_bias ";

PS_CRITERIA PS_207_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_207_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bias, 1 - r1 is NOT allowed";
string PS_207 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bias, 1 - r1 ";

PS_CRITERIA PS_208_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_208_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bias, -r1 is NOT allowed";
string PS_208 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bias, -r1 ";

PS_CRITERIA PS_209_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_209_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bias, r1_bx2 is NOT allowed";
string PS_209 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bias, r1_bx2 ";

PS_CRITERIA PS_210_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_210_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bias, -r1_bx2 is NOT allowed";
string PS_210 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bias, -r1_bx2 ";

PS_CRITERIA PS_211_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_211_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bias, r1 is NOT allowed";
string PS_211 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bias, r1 ";

PS_CRITERIA PS_212_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_212_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, 1 - t0, r1_bias is NOT allowed";
string PS_212 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, 1 - t0, r1_bias ";

PS_CRITERIA PS_213_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_213_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, 1 - t0, 1 - r1 is NOT allowed";
string PS_213 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, 1 - t0, 1 - r1 ";

PS_CRITERIA PS_214_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_214_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, 1 - t0, -r1 is NOT allowed";
string PS_214 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, 1 - t0, -r1 ";

PS_CRITERIA PS_215_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_215_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, 1 - t0, r1_bx2 is NOT allowed";
string PS_215 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, 1 - t0, r1_bx2 ";

PS_CRITERIA PS_216_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_216_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, 1 - t0, -r1_bx2 is NOT allowed";
string PS_216 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, 1 - t0, -r1_bx2 ";

PS_CRITERIA PS_217_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_217_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, 1 - t0, r1 is NOT allowed";
string PS_217 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, 1 - t0, r1 ";

PS_CRITERIA PS_218_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_218_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0, r1_bias is NOT allowed";
string PS_218 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0, r1_bias ";

PS_CRITERIA PS_219_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_219_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0, 1 - r1 is NOT allowed";
string PS_219 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0, 1 - r1 ";

PS_CRITERIA PS_220_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_220_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0, -r1 is NOT allowed";
string PS_220 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0, -r1 ";

PS_CRITERIA PS_221_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_221_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0, r1_bx2 is NOT allowed";
string PS_221 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0, r1_bx2 ";

PS_CRITERIA PS_222_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_222_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0, -r1_bx2 is NOT allowed";
string PS_222 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0, -r1_bx2 ";

PS_CRITERIA PS_223_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_223_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0, r1 is NOT allowed";
string PS_223 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0, r1 ";

PS_CRITERIA PS_224_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_224_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bx2, r1_bias is NOT allowed";
string PS_224 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bx2, r1_bias ";

PS_CRITERIA PS_225_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_225_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bx2, 1 - r1 is NOT allowed";
string PS_225 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bx2, 1 - r1 ";

PS_CRITERIA PS_226_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_226_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bx2, -r1 is NOT allowed";
string PS_226 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bx2, -r1 ";

PS_CRITERIA PS_227_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_227_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bx2, r1_bx2 is NOT allowed";
string PS_227 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bx2, r1_bx2 ";

PS_CRITERIA PS_228_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_228_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bx2, -r1_bx2 is NOT allowed";
string PS_228 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_229_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_229_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0_bx2, r1 is NOT allowed";
string PS_229 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0_bx2, r1 ";

PS_CRITERIA PS_230_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_230_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0_bx2, r1_bias is NOT allowed";
string PS_230 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0_bx2, r1_bias ";

PS_CRITERIA PS_231_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_231_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0_bx2, 1 - r1 is NOT allowed";
string PS_231 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0_bx2, 1 - r1 ";

PS_CRITERIA PS_232_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_232_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0_bx2, -r1 is NOT allowed";
string PS_232 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0_bx2, -r1 ";

PS_CRITERIA PS_233_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_233_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0_bx2, r1_bx2 is NOT allowed";
string PS_233 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0_bx2, r1_bx2 ";

PS_CRITERIA PS_234_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_234_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0_bx2, -r1_bx2 is NOT allowed";
string PS_234 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_235_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_235_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, -t0_bx2, r1 is NOT allowed";
string PS_235 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, -t0_bx2, r1 ";

PS_CRITERIA PS_236_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_236_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0, r1_bias is NOT allowed";
string PS_236 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0, r1_bias ";

PS_CRITERIA PS_237_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_237_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0, 1 - r1 is NOT allowed";
string PS_237 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0, 1 - r1 ";

PS_CRITERIA PS_238_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_238_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0, -r1 is NOT allowed";
string PS_238 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0, -r1 ";

PS_CRITERIA PS_239_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_239_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0, r1_bx2 is NOT allowed";
string PS_239 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0, r1_bx2 ";

PS_CRITERIA PS_240_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_240_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0, -r1_bx2 is NOT allowed";
string PS_240 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0, -r1_bx2 ";

PS_CRITERIA PS_241_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_241_Desc = "ps_1_3 : cmp source reg combination -r0_bx2, t0, r1 is NOT allowed";
string PS_241 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_bx2, t0, r1 ";

PS_CRITERIA PS_242_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_242_Desc = "ps_1_3 : cmp source reg combination r0, t0_bias, r1_bias is NOT allowed";
string PS_242 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bias, r1_bias ";

PS_CRITERIA PS_243_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_243_Desc = "ps_1_3 : cmp source reg combination r0, t0_bias, 1 - r1 is NOT allowed";
string PS_243 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bias, 1 - r1 ";

PS_CRITERIA PS_244_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_244_Desc = "ps_1_3 : cmp source reg combination r0, t0_bias, -r1 is NOT allowed";
string PS_244 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bias, -r1 ";

PS_CRITERIA PS_245_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_245_Desc = "ps_1_3 : cmp source reg combination r0, t0_bias, r1_bx2 is NOT allowed";
string PS_245 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bias, r1_bx2 ";

PS_CRITERIA PS_246_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_246_Desc = "ps_1_3 : cmp source reg combination r0, t0_bias, -r1_bx2 is NOT allowed";
string PS_246 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bias, -r1_bx2 ";

PS_CRITERIA PS_247_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_247_Desc = "ps_1_3 : cmp source reg combination r0, t0_bias, r1 is NOT allowed";
string PS_247 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bias, r1 ";

PS_CRITERIA PS_248_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_248_Desc = "ps_1_3 : cmp source reg combination r0, 1 - t0, r1_bias is NOT allowed";
string PS_248 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, 1 - t0, r1_bias ";

PS_CRITERIA PS_249_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_249_Desc = "ps_1_3 : cmp source reg combination r0, 1 - t0, 1 - r1 is NOT allowed";
string PS_249 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, 1 - t0, 1 - r1 ";

PS_CRITERIA PS_250_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_250_Desc = "ps_1_3 : cmp source reg combination r0, 1 - t0, -r1 is NOT allowed";
string PS_250 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, 1 - t0, -r1 ";

PS_CRITERIA PS_251_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_251_Desc = "ps_1_3 : cmp source reg combination r0, 1 - t0, r1_bx2 is NOT allowed";
string PS_251 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, 1 - t0, r1_bx2 ";

PS_CRITERIA PS_252_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_252_Desc = "ps_1_3 : cmp source reg combination r0, 1 - t0, -r1_bx2 is NOT allowed";
string PS_252 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, 1 - t0, -r1_bx2 ";

PS_CRITERIA PS_253_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_253_Desc = "ps_1_3 : cmp source reg combination r0, 1 - t0, r1 is NOT allowed";
string PS_253 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, 1 - t0, r1 ";

PS_CRITERIA PS_254_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_254_Desc = "ps_1_3 : cmp source reg combination r0, -t0, r1_bias is NOT allowed";
string PS_254 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0, r1_bias ";

PS_CRITERIA PS_255_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_255_Desc = "ps_1_3 : cmp source reg combination r0, -t0, 1 - r1 is NOT allowed";
string PS_255 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0, 1 - r1 ";

PS_CRITERIA PS_256_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_256_Desc = "ps_1_3 : cmp source reg combination r0, -t0, -r1 is NOT allowed";
string PS_256 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0, -r1 ";

PS_CRITERIA PS_257_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_257_Desc = "ps_1_3 : cmp source reg combination r0, -t0, r1_bx2 is NOT allowed";
string PS_257 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0, r1_bx2 ";

PS_CRITERIA PS_258_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_258_Desc = "ps_1_3 : cmp source reg combination r0, -t0, -r1_bx2 is NOT allowed";
string PS_258 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0, -r1_bx2 ";

PS_CRITERIA PS_259_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_259_Desc = "ps_1_3 : cmp source reg combination r0, -t0, r1 is NOT allowed";
string PS_259 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0, r1 ";

PS_CRITERIA PS_260_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_260_Desc = "ps_1_3 : cmp source reg combination r0, t0_bx2, r1_bias is NOT allowed";
string PS_260 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bx2, r1_bias ";

PS_CRITERIA PS_261_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_261_Desc = "ps_1_3 : cmp source reg combination r0, t0_bx2, 1 - r1 is NOT allowed";
string PS_261 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bx2, 1 - r1 ";

PS_CRITERIA PS_262_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_262_Desc = "ps_1_3 : cmp source reg combination r0, t0_bx2, -r1 is NOT allowed";
string PS_262 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bx2, -r1 ";

PS_CRITERIA PS_263_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_263_Desc = "ps_1_3 : cmp source reg combination r0, t0_bx2, r1_bx2 is NOT allowed";
string PS_263 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bx2, r1_bx2 ";

PS_CRITERIA PS_264_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_264_Desc = "ps_1_3 : cmp source reg combination r0, t0_bx2, -r1_bx2 is NOT allowed";
string PS_264 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_265_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_265_Desc = "ps_1_3 : cmp source reg combination r0, t0_bx2, r1 is NOT allowed";
string PS_265 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0_bx2, r1 ";

PS_CRITERIA PS_266_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_266_Desc = "ps_1_3 : cmp source reg combination r0, -t0_bx2, r1_bias is NOT allowed";
string PS_266 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0_bx2, r1_bias ";

PS_CRITERIA PS_267_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_267_Desc = "ps_1_3 : cmp source reg combination r0, -t0_bx2, 1 - r1 is NOT allowed";
string PS_267 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0_bx2, 1 - r1 ";

PS_CRITERIA PS_268_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_268_Desc = "ps_1_3 : cmp source reg combination r0, -t0_bx2, -r1 is NOT allowed";
string PS_268 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0_bx2, -r1 ";

PS_CRITERIA PS_269_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_269_Desc = "ps_1_3 : cmp source reg combination r0, -t0_bx2, r1_bx2 is NOT allowed";
string PS_269 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0_bx2, r1_bx2 ";

PS_CRITERIA PS_270_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_270_Desc = "ps_1_3 : cmp source reg combination r0, -t0_bx2, -r1_bx2 is NOT allowed";
string PS_270 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0_bx2, -r1_bx2 ";

PS_CRITERIA PS_271_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_271_Desc = "ps_1_3 : cmp source reg combination r0, -t0_bx2, r1 is NOT allowed";
string PS_271 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -t0_bx2, r1 ";

PS_CRITERIA PS_272_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_272_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1_bias is NOT allowed";
string PS_272 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1_bias ";

PS_CRITERIA PS_273_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_273_Desc = "ps_1_3 : cmp source reg combination r0, t0, 1 - r1 is NOT allowed";
string PS_273 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, 1 - r1 ";

PS_CRITERIA PS_274_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_274_Desc = "ps_1_3 : cmp source reg combination r0, t0, -r1 is NOT allowed";
string PS_274 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, -r1 ";

PS_CRITERIA PS_275_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_275_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1_bx2 is NOT allowed";
string PS_275 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1_bx2 ";

PS_CRITERIA PS_276_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_276_Desc = "ps_1_3 : cmp source reg combination r0, t0, -r1_bx2 is NOT allowed";
string PS_276 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, -r1_bx2 ";

PS_CRITERIA PS_277_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_277_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1 is NOT allowed";
string PS_277 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1 ";

PS_CRITERIA PS_278_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_278_Desc = "ps_1_3 : cmp source reg combination r0.a, t0, r1 is NOT allowed";
string PS_278 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.a, t0, r1 ";

PS_CRITERIA PS_279_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_279_Desc = "ps_1_3 : cmp source reg combination r0, t0.a, r1 is NOT allowed";
string PS_279 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.a, r1 ";

PS_CRITERIA PS_280_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_280_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.a is NOT allowed";
string PS_280 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.a ";

PS_CRITERIA PS_281_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_281_Desc = "ps_1_3 : cmp source reg combination v0, v1, v2 is NOT allowed";
string PS_281 = 
	"ps_1_3 "
	"cmp r0, v0, v1, v2 "
	"mov r0, c0 ";

PS_CRITERIA PS_282_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_282_Desc = "ps_1_3 : cmp source reg combination r0, r1, r2 is NOT allowed";
string PS_282 = 
	"ps_1_3 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0, r0, r1, r2 ";

PS_CRITERIA PS_283_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_283_Desc = "ps_1_3 : cmp source reg combination c0, c1, c2 is NOT allowed";
string PS_283 = 
	"ps_1_3 "
	"def c2, 1, 1, 1, 1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"cmp r0, c0, c1, c2 "
	"mov r0, c0 ";

PS_CRITERIA PS_284_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_284_Desc = "ps_1_3 : cmp source reg combination r0.r, t0, r1 is NOT allowed";
string PS_284 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.r, t0, r1 ";

PS_CRITERIA PS_285_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_285_Desc = "ps_1_3 : cmp source reg combination r0.g, t0, r1 is NOT allowed";
string PS_285 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.g, t0, r1 ";

PS_CRITERIA PS_286_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_286_Desc = "ps_1_3 : cmp source reg combination r0.b, t0, r1 is NOT allowed";
string PS_286 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.b, t0, r1 ";

PS_CRITERIA PS_287_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_287_Desc = "ps_1_3 : cmp source reg combination r0.gbra, t0, r1 is NOT allowed";
string PS_287 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.gbra, t0, r1 ";

PS_CRITERIA PS_288_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_288_Desc = "ps_1_3 : cmp source reg combination r0.brga, t0, r1 is NOT allowed";
string PS_288 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.brga, t0, r1 ";

PS_CRITERIA PS_289_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_289_Desc = "ps_1_3 : cmp source reg combination r0.abgr, t0, r1 is NOT allowed";
string PS_289 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.abgr, t0, r1 ";

PS_CRITERIA PS_290_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_290_Desc = "ps_1_3 : cmp source reg combination r0.agrb, t0, r1 is NOT allowed";
string PS_290 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.agrb, t0, r1 ";

PS_CRITERIA PS_291_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_291_Desc = "ps_1_3 : cmp source reg combination r0.rbga, t0, r1 is NOT allowed";
string PS_291 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.rbga, t0, r1 ";

PS_CRITERIA PS_292_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_292_Desc = "ps_1_3 : cmp source reg combination r0.rgab, t0, r1 is NOT allowed";
string PS_292 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.rgab, t0, r1 ";

PS_CRITERIA PS_293_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_293_Desc = "ps_1_3 : cmp source reg combination r0.bgr, t0, r1 is NOT allowed";
string PS_293 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.bgr, t0, r1 ";

PS_CRITERIA PS_294_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_294_Desc = "ps_1_3 : cmp source reg combination r0.rbg, t0, r1 is NOT allowed";
string PS_294 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.rbg, t0, r1 ";

PS_CRITERIA PS_295_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_295_Desc = "ps_1_3 : cmp source reg combination r0.gar, t0, r1 is NOT allowed";
string PS_295 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.gar, t0, r1 ";

PS_CRITERIA PS_296_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_296_Desc = "ps_1_3 : cmp source reg combination r0.gr, t0, r1 is NOT allowed";
string PS_296 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.gr, t0, r1 ";

PS_CRITERIA PS_297_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_297_Desc = "ps_1_3 : cmp source reg combination r0.ab, t0, r1 is NOT allowed";
string PS_297 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.ab, t0, r1 ";

PS_CRITERIA PS_298_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_298_Desc = "ps_1_3 : cmp source reg combination r0.bg, t0, r1 is NOT allowed";
string PS_298 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.bg, t0, r1 ";

PS_CRITERIA PS_299_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_299_Desc = "ps_1_3 : cmp source reg combination r0, t0.r, r1 is NOT allowed";
string PS_299 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.r, r1 ";

PS_CRITERIA PS_300_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_300_Desc = "ps_1_3 : cmp source reg combination r0, t0.g, r1 is NOT allowed";
string PS_300 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.g, r1 ";

PS_CRITERIA PS_301_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_301_Desc = "ps_1_3 : cmp source reg combination r0, t0.b, r1 is NOT allowed";
string PS_301 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.b, r1 ";

PS_CRITERIA PS_302_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_302_Desc = "ps_1_3 : cmp source reg combination r0, t0.gbra, r1 is NOT allowed";
string PS_302 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.gbra, r1 ";

PS_CRITERIA PS_303_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_303_Desc = "ps_1_3 : cmp source reg combination r0, t0.brga, r1 is NOT allowed";
string PS_303 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.brga, r1 ";

PS_CRITERIA PS_304_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_304_Desc = "ps_1_3 : cmp source reg combination r0, t0.abgr, r1 is NOT allowed";
string PS_304 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.abgr, r1 ";

PS_CRITERIA PS_305_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_305_Desc = "ps_1_3 : cmp source reg combination r0, t0.agrb, r1 is NOT allowed";
string PS_305 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.agrb, r1 ";

PS_CRITERIA PS_306_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_306_Desc = "ps_1_3 : cmp source reg combination r0, t0.rbga, r1 is NOT allowed";
string PS_306 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.rbga, r1 ";

PS_CRITERIA PS_307_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_307_Desc = "ps_1_3 : cmp source reg combination r0, t0.rgab, r1 is NOT allowed";
string PS_307 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.rgab, r1 ";

PS_CRITERIA PS_308_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_308_Desc = "ps_1_3 : cmp source reg combination r0, t0.bgr, r1 is NOT allowed";
string PS_308 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.bgr, r1 ";

PS_CRITERIA PS_309_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_309_Desc = "ps_1_3 : cmp source reg combination r0, t0.rbg, r1 is NOT allowed";
string PS_309 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.rbg, r1 ";

PS_CRITERIA PS_310_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_310_Desc = "ps_1_3 : cmp source reg combination r0, t0.gar, r1 is NOT allowed";
string PS_310 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.gar, r1 ";

PS_CRITERIA PS_311_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_311_Desc = "ps_1_3 : cmp source reg combination r0, t0.gr, r1 is NOT allowed";
string PS_311 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.gr, r1 ";

PS_CRITERIA PS_312_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_312_Desc = "ps_1_3 : cmp source reg combination r0, t0.ab, r1 is NOT allowed";
string PS_312 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.ab, r1 ";

PS_CRITERIA PS_313_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_313_Desc = "ps_1_3 : cmp source reg combination r0, t0.bg, r1 is NOT allowed";
string PS_313 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0.bg, r1 ";

PS_CRITERIA PS_314_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_314_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.r is NOT allowed";
string PS_314 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.r ";

PS_CRITERIA PS_315_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_315_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.g is NOT allowed";
string PS_315 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.g ";

PS_CRITERIA PS_316_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_316_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.b is NOT allowed";
string PS_316 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.b ";

PS_CRITERIA PS_317_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_317_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.gbra is NOT allowed";
string PS_317 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.gbra ";

PS_CRITERIA PS_318_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_318_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.brga is NOT allowed";
string PS_318 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.brga ";

PS_CRITERIA PS_319_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_319_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.abgr is NOT allowed";
string PS_319 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.abgr ";

PS_CRITERIA PS_320_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_320_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.agrb is NOT allowed";
string PS_320 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.agrb ";

PS_CRITERIA PS_321_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_321_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.rbga is NOT allowed";
string PS_321 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.rbga ";

PS_CRITERIA PS_322_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_322_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.rgab is NOT allowed";
string PS_322 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.rgab ";

PS_CRITERIA PS_323_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_323_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.bgr is NOT allowed";
string PS_323 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.bgr ";

PS_CRITERIA PS_324_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_324_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.rbg is NOT allowed";
string PS_324 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.rbg ";

PS_CRITERIA PS_325_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_325_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.gar is NOT allowed";
string PS_325 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.gar ";

PS_CRITERIA PS_326_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_326_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.gr is NOT allowed";
string PS_326 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.gr ";

PS_CRITERIA PS_327_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_327_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.ab is NOT allowed";
string PS_327 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.ab ";

PS_CRITERIA PS_328_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_328_Desc = "ps_1_3 : cmp source reg combination r0, t0, r1.bg is NOT allowed";
string PS_328 = 
	"ps_1_3 "
	"texcoord t0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, t0, r1.bg ";

PS_CRITERIA PS_329_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_329_Desc = "ps_1_3 : cmp dest r# is allowed";
string PS_329 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_330_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_330_Desc = "ps_1_3 : cmp _x2 dest r# is allowed";
string PS_330 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_x2 r1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_331_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_331_Desc = "ps_1_3 : cmp _x4 dest r# is allowed";
string PS_331 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_x4 r1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_332_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_332_Desc = "ps_1_3 : cmp _d2 dest r# is allowed";
string PS_332 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_d2 r1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_333_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_333_Desc = "ps_1_3 : cmp _sat dest r# is allowed";
string PS_333 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_sat r1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_334_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_334_Desc = "ps_1_3 : cmp dest t# is allowed";
string PS_334 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_335_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_335_Desc = "ps_1_3 : cmp _x2 dest t# is allowed";
string PS_335 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_x2 r1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_336_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_336_Desc = "ps_1_3 : cmp _x4 dest t# is allowed";
string PS_336 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_x4 r1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_337_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_337_Desc = "ps_1_3 : cmp _d2 dest t# is allowed";
string PS_337 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_d2 r1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_338_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_338_Desc = "ps_1_3 : cmp _sat dest t# is allowed";
string PS_338 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_sat r1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_339_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_339_Desc = "ps_1_3 : cmp dest v# is NOT allowed";
string PS_339 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp v0, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_340_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_340_Desc = "ps_1_3 : cmp _x2 dest v# is NOT allowed";
string PS_340 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_x2 v0, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_341_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_341_Desc = "ps_1_3 : cmp _x4 dest v# is NOT allowed";
string PS_341 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_x4 v0, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_342_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_342_Desc = "ps_1_3 : cmp _d2 dest v# is NOT allowed";
string PS_342 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_d2 v0, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_343_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_343_Desc = "ps_1_3 : cmp _sat dest v# is NOT allowed";
string PS_343 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_sat v0, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_344_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_344_Desc = "ps_1_3 : cmp dest c# is NOT allowed";
string PS_344 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp c1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_345_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_345_Desc = "ps_1_3 : cmp _x2 dest c# is NOT allowed";
string PS_345 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_x2 c1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_346_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_346_Desc = "ps_1_3 : cmp _x4 dest c# is NOT allowed";
string PS_346 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_x4 c1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_347_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_347_Desc = "ps_1_3 : cmp _d2 dest c# is NOT allowed";
string PS_347 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_d2 c1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_348_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_348_Desc = "ps_1_3 : cmp _sat dest c# is NOT allowed";
string PS_348 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp_sat c1, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_349_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_349_Desc = "ps_1_3 : cmp write mask .a is allowed";
string PS_349 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.a, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_350_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_350_Desc = "ps_1_3 : cmp write mask .rgb is allowed";
string PS_350 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.rgb, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_351_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_351_Desc = "ps_1_3 : cmp write mask .rgba is allowed";
string PS_351 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.rgba, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_352_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_352_Desc = "ps_1_3 : cmp write mask .r is NOT allowed";
string PS_352 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.r, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_353_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_353_Desc = "ps_1_3 : cmp write mask .g is NOT allowed";
string PS_353 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.g, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_354_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_354_Desc = "ps_1_3 : cmp write mask .b is NOT allowed";
string PS_354 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.b, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_355_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_355_Desc = "ps_1_3 : cmp write mask .rg is NOT allowed";
string PS_355 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.rg, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_356_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_356_Desc = "ps_1_3 : cmp write mask .gb is NOT allowed";
string PS_356 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.gb, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_357_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_357_Desc = "ps_1_3 : cmp write mask .ba is NOT allowed";
string PS_357 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.ba, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_358_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_358_Desc = "ps_1_3 : cmp write mask .rb is NOT allowed";
string PS_358 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.rb, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_359_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_359_Desc = "ps_1_3 : cmp write mask .ra is NOT allowed";
string PS_359 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.ra, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_360_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_360_Desc = "ps_1_3 : cmp write mask .ga is NOT allowed";
string PS_360 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.ga, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_361_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_361_Desc = "ps_1_3 : cmp write mask .gba is NOT allowed";
string PS_361 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.gba, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_362_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_362_Desc = "ps_1_3 : cmp write mask .rba is NOT allowed";
string PS_362 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.rba, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_363_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_363_Desc = "ps_1_3 : cmp write mask .yx is NOT allowed";
string PS_363 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.yx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_364_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_364_Desc = "ps_1_3 : cmp write mask .zx is NOT allowed";
string PS_364 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.zx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_365_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_365_Desc = "ps_1_3 : cmp write mask .zy is NOT allowed";
string PS_365 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.zy, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_366_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_366_Desc = "ps_1_3 : cmp write mask .wx is NOT allowed";
string PS_366 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.wx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_367_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_367_Desc = "ps_1_3 : cmp write mask .wz is NOT allowed";
string PS_367 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.wz, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_368_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_368_Desc = "ps_1_3 : cmp write mask .wy is NOT allowed";
string PS_368 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.wy, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_369_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_369_Desc = "ps_1_3 : cmp write mask .zyx is NOT allowed";
string PS_369 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.zyx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_370_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_370_Desc = "ps_1_3 : cmp write mask .wzy is NOT allowed";
string PS_370 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.wzy, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_371_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_371_Desc = "ps_1_3 : cmp write mask .wzx is NOT allowed";
string PS_371 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.wzx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_372_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_372_Desc = "ps_1_3 : cmp write mask .wyx is NOT allowed";
string PS_372 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.wyx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_373_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_373_Desc = "ps_1_3 : cmp write mask .yxw is NOT allowed";
string PS_373 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.yxw, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_374_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_374_Desc = "ps_1_3 : cmp write mask .wzyx is NOT allowed";
string PS_374 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.wzyx, v0, v1, r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_375_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_375_Desc = "ps_1_3 : cmp write mask .zxwy is NOT allowed";
string PS_375 = 
	"ps_1_3 "
	"mov r0, c0 "
	"cmp r1.zxwy, v0, v1, r0 "
	"mov r0, c0 ";


