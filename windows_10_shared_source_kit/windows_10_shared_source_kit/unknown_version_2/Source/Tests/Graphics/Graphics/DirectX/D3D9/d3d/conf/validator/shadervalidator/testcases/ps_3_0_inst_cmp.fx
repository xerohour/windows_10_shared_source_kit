#include "Test_Include.fx"

int Test_Count = 1027;

string TestInfo
<
	string TestType = "PS";
>
= "ps_3_0_inst_cmp";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_3_0 : cmp source reg combination r0, r1, r2 is allowed";
string PS_001 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_3_0 : cmp source reg combination r0, r1, v0 is allowed";
string PS_002 = 
	"ps_3_0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, r1, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_003_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_3_0 : cmp source reg combination r0, r1, c0 is allowed";
string PS_003 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, r1, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_004_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_3_0 : cmp source reg combination r0, r1, vPos.xy is allowed";
string PS_004 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, r1, vPos.xy "
	"mov oC0, c0 ";

PS_CRITERIA PS_005_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_3_0 : cmp source reg combination r0, v0, r1 is allowed";
string PS_005 = 
	"ps_3_0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, v0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_006_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_3_0 : cmp source reg combination r0, v0, c0 is allowed";
string PS_006 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"cmp r0, r0, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_007_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_3_0 : cmp source reg combination r0, v0, vPos.xy is allowed";
string PS_007 = 
	"ps_3_0 "
	"dcl_texcoord v0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, r0, v0, vPos.xy "
	"mov oC0, c0 ";

PS_CRITERIA PS_008_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1 is allowed";
string PS_008 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_009_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_3_0 : cmp source reg combination r0, c0, v0 is allowed";
string PS_009 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"cmp r0, r0, c0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_010_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_3_0 : cmp source reg combination r0, c0, vPos.xy is allowed";
string PS_010 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, r0, c0, vPos.xy "
	"mov oC0, c0 ";

PS_CRITERIA PS_011_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_3_0 : cmp source reg combination r0, vPos.xy, r1 is allowed";
string PS_011 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, vPos.xy, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_012_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_3_0 : cmp source reg combination r0, vPos.xy, v0 is allowed";
string PS_012 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"cmp r0, r0, vPos.xy, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_013_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_3_0 : cmp source reg combination r0, vPos.xy, c0 is allowed";
string PS_013 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, r0, vPos.xy, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_014_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_3_0 : cmp source reg combination v0, r0, r1 is allowed";
string PS_014 = 
	"ps_3_0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, v0, r0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_015_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_3_0 : cmp source reg combination v0, r0, c0 is allowed";
string PS_015 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"cmp r0, v0, r0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_016_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_3_0 : cmp source reg combination v0, r0, vPos.xy is allowed";
string PS_016 = 
	"ps_3_0 "
	"dcl_texcoord v0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, v0, r0, vPos.xy "
	"mov oC0, c0 ";

PS_CRITERIA PS_017_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_3_0 : cmp source reg combination v0, c0, r0 is allowed";
string PS_017 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"cmp r0, v0, c0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_018_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_3_0 : cmp source reg combination v0, c0, vPos.xy is allowed";
string PS_018 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"dcl vPos.xy "
	"cmp r0, v0, c0, vPos.xy "
	"mov oC0, c0 ";

PS_CRITERIA PS_019_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_3_0 : cmp source reg combination v0, vPos.xy, r0 is allowed";
string PS_019 = 
	"ps_3_0 "
	"dcl_texcoord v0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, v0, vPos.xy, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_020_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_3_0 : cmp source reg combination v0, vPos.xy, c0 is allowed";
string PS_020 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"dcl vPos.xy "
	"cmp r0, v0, vPos.xy, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_3_0 : cmp source reg combination c0, r0, r1 is allowed";
string PS_021 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, c0, r0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_3_0 : cmp source reg combination c0, r0, v0 is allowed";
string PS_022 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"cmp r0, c0, r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_3_0 : cmp source reg combination c0, r0, vPos.xy is allowed";
string PS_023 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, c0, r0, vPos.xy "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_3_0 : cmp source reg combination c0, v0, r0 is allowed";
string PS_024 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"cmp r0, c0, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_3_0 : cmp source reg combination c0, v0, vPos.xy is allowed";
string PS_025 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_texcoord v0 "
	"dcl vPos.xy "
	"cmp r0, c0, v0, vPos.xy "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_3_0 : cmp source reg combination c0, vPos.xy, r0 is allowed";
string PS_026 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, c0, vPos.xy, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_3_0 : cmp source reg combination c0, vPos.xy, v0 is allowed";
string PS_027 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"dcl_texcoord v0 "
	"cmp r0, c0, vPos.xy, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_3_0 : cmp source reg combination vPos.xy, r0, r1 is allowed";
string PS_028 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, vPos.xy, r0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_3_0 : cmp source reg combination vPos.xy, r0, v0 is allowed";
string PS_029 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"cmp r0, vPos.xy, r0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_3_0 : cmp source reg combination vPos.xy, r0, c0 is allowed";
string PS_030 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, vPos.xy, r0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_3_0 : cmp source reg combination vPos.xy, v0, r0 is allowed";
string PS_031 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"cmp r0, vPos.xy, v0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_3_0 : cmp source reg combination vPos.xy, v0, c0 is allowed";
string PS_032 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"dcl_texcoord v0 "
	"cmp r0, vPos.xy, v0, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_3_0 : cmp source reg combination vPos.xy, c0, r0 is allowed";
string PS_033 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, vPos.xy, c0, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_3_0 : cmp source reg combination vPos.xy, c0, v0 is allowed";
string PS_034 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"dcl_texcoord v0 "
	"cmp r0, vPos.xy, c0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_3_0 : cmp source reg combination -r0, -c0, -r1 is allowed";
string PS_035 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -c0, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_3_0 : cmp source reg combination -r0, -c0, r1_abs is allowed";
string PS_036 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -c0, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_3_0 : cmp source reg combination -r0, -c0, -r1_abs is allowed";
string PS_037 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -c0, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_3_0 : cmp source reg combination -r0, -c0, r1 is allowed";
string PS_038 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_3_0 : cmp source reg combination -r0, c0_abs, -r1 is allowed";
string PS_039 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, c0_abs, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_3_0 : cmp source reg combination -r0, c0_abs, r1_abs is allowed";
string PS_040 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, c0_abs, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_3_0 : cmp source reg combination -r0, c0_abs, -r1_abs is allowed";
string PS_041 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, c0_abs, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_3_0 : cmp source reg combination -r0, c0_abs, r1 is allowed";
string PS_042 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, c0_abs, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_3_0 : cmp source reg combination -r0, -c0_abs, -r1 is allowed";
string PS_043 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -c0_abs, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_3_0 : cmp source reg combination -r0, -c0_abs, r1_abs is allowed";
string PS_044 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -c0_abs, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_3_0 : cmp source reg combination -r0, -c0_abs, -r1_abs is allowed";
string PS_045 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -c0_abs, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_3_0 : cmp source reg combination -r0, -c0_abs, r1 is allowed";
string PS_046 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, -c0_abs, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_3_0 : cmp source reg combination -r0, c0, -r1 is allowed";
string PS_047 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, c0, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_3_0 : cmp source reg combination -r0, c0, r1_abs is allowed";
string PS_048 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, c0, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_049_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_3_0 : cmp source reg combination -r0, c0, -r1_abs is allowed";
string PS_049 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, c0, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_050_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_3_0 : cmp source reg combination -r0, c0, r1 is allowed";
string PS_050 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_3_0 : cmp source reg combination r0_abs, -c0, -r1 is allowed";
string PS_051 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, -c0, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_3_0 : cmp source reg combination r0_abs, -c0, r1_abs is allowed";
string PS_052 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, -c0, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_3_0 : cmp source reg combination r0_abs, -c0, -r1_abs is allowed";
string PS_053 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, -c0, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_3_0 : cmp source reg combination r0_abs, -c0, r1 is allowed";
string PS_054 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, -c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_3_0 : cmp source reg combination r0_abs, c0_abs, -r1 is allowed";
string PS_055 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, c0_abs, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_3_0 : cmp source reg combination r0_abs, c0_abs, r1_abs is allowed";
string PS_056 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, c0_abs, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_3_0 : cmp source reg combination r0_abs, c0_abs, -r1_abs is allowed";
string PS_057 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, c0_abs, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_3_0 : cmp source reg combination r0_abs, c0_abs, r1 is allowed";
string PS_058 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, c0_abs, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_3_0 : cmp source reg combination r0_abs, -c0_abs, -r1 is allowed";
string PS_059 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, -c0_abs, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_3_0 : cmp source reg combination r0_abs, -c0_abs, r1_abs is allowed";
string PS_060 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, -c0_abs, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_3_0 : cmp source reg combination r0_abs, -c0_abs, -r1_abs is allowed";
string PS_061 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, -c0_abs, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_3_0 : cmp source reg combination r0_abs, -c0_abs, r1 is allowed";
string PS_062 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, -c0_abs, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_3_0 : cmp source reg combination r0_abs, c0, -r1 is allowed";
string PS_063 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, c0, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_3_0 : cmp source reg combination r0_abs, c0, r1_abs is allowed";
string PS_064 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, c0, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_3_0 : cmp source reg combination r0_abs, c0, -r1_abs is allowed";
string PS_065 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, c0, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_3_0 : cmp source reg combination r0_abs, c0, r1 is allowed";
string PS_066 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0_abs, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_3_0 : cmp source reg combination -r0_abs, -c0, -r1 is allowed";
string PS_067 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, -c0, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_3_0 : cmp source reg combination -r0_abs, -c0, r1_abs is allowed";
string PS_068 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, -c0, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_3_0 : cmp source reg combination -r0_abs, -c0, -r1_abs is allowed";
string PS_069 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, -c0, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_3_0 : cmp source reg combination -r0_abs, -c0, r1 is allowed";
string PS_070 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, -c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_3_0 : cmp source reg combination -r0_abs, c0_abs, -r1 is allowed";
string PS_071 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, c0_abs, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_3_0 : cmp source reg combination -r0_abs, c0_abs, r1_abs is allowed";
string PS_072 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, c0_abs, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_3_0 : cmp source reg combination -r0_abs, c0_abs, -r1_abs is allowed";
string PS_073 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, c0_abs, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_3_0 : cmp source reg combination -r0_abs, c0_abs, r1 is allowed";
string PS_074 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, c0_abs, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_3_0 : cmp source reg combination -r0_abs, -c0_abs, -r1 is allowed";
string PS_075 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, -c0_abs, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_3_0 : cmp source reg combination -r0_abs, -c0_abs, r1_abs is allowed";
string PS_076 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, -c0_abs, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_3_0 : cmp source reg combination -r0_abs, -c0_abs, -r1_abs is allowed";
string PS_077 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, -c0_abs, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_3_0 : cmp source reg combination -r0_abs, -c0_abs, r1 is allowed";
string PS_078 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, -c0_abs, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_3_0 : cmp source reg combination -r0_abs, c0, -r1 is allowed";
string PS_079 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, c0, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_3_0 : cmp source reg combination -r0_abs, c0, r1_abs is allowed";
string PS_080 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, c0, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_081_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_3_0 : cmp source reg combination -r0_abs, c0, -r1_abs is allowed";
string PS_081 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, c0, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_082_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_3_0 : cmp source reg combination -r0_abs, c0, r1 is allowed";
string PS_082 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, -r0_abs, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_083_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_3_0 : cmp source reg combination r0, -c0, -r1 is allowed";
string PS_083 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -c0, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_084_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_3_0 : cmp source reg combination r0, -c0, r1_abs is allowed";
string PS_084 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -c0, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_085_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_3_0 : cmp source reg combination r0, -c0, -r1_abs is allowed";
string PS_085 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -c0, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_086_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_3_0 : cmp source reg combination r0, -c0, r1 is allowed";
string PS_086 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_087_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_3_0 : cmp source reg combination r0, c0_abs, -r1 is allowed";
string PS_087 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0_abs, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_088_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_3_0 : cmp source reg combination r0, c0_abs, r1_abs is allowed";
string PS_088 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0_abs, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_089_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_3_0 : cmp source reg combination r0, c0_abs, -r1_abs is allowed";
string PS_089 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0_abs, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_090_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_3_0 : cmp source reg combination r0, c0_abs, r1 is allowed";
string PS_090 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0_abs, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_091_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_3_0 : cmp source reg combination r0, -c0_abs, -r1 is allowed";
string PS_091 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -c0_abs, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_092_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_3_0 : cmp source reg combination r0, -c0_abs, r1_abs is allowed";
string PS_092 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -c0_abs, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_093_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_3_0 : cmp source reg combination r0, -c0_abs, -r1_abs is allowed";
string PS_093 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -c0_abs, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_094_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_3_0 : cmp source reg combination r0, -c0_abs, r1 is allowed";
string PS_094 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, -c0_abs, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_095_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_3_0 : cmp source reg combination r0, c0, -r1 is allowed";
string PS_095 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, -r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_096_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1_abs is allowed";
string PS_096 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_097_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_3_0 : cmp source reg combination r0, c0, -r1_abs is allowed";
string PS_097 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, -r1_abs "
	"mov oC0, c0 ";

PS_CRITERIA PS_098_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1 is allowed";
string PS_098 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_099_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_3_0 : cmp source reg combination r0.r, c0, r1 is allowed";
string PS_099 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.r, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_100_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_3_0 : cmp source reg combination r0.g, c0, r1 is allowed";
string PS_100 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.g, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_101_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_3_0 : cmp source reg combination r0.b, c0, r1 is allowed";
string PS_101 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.b, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_102_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_3_0 : cmp source reg combination r0.a, c0, r1 is allowed";
string PS_102 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.a, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_103_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_3_0 : cmp source reg combination r0.gbra, c0, r1 is allowed";
string PS_103 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.gbra, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_104_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_3_0 : cmp source reg combination r0.brga, c0, r1 is allowed";
string PS_104 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.brga, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_105_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_3_0 : cmp source reg combination r0.abgr, c0, r1 is allowed";
string PS_105 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.abgr, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_106_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_3_0 : cmp source reg combination r0.agrb, c0, r1 is allowed";
string PS_106 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.agrb, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_107_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_3_0 : cmp source reg combination r0.rbga, c0, r1 is allowed";
string PS_107 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.rbga, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_108_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_3_0 : cmp source reg combination r0.rgab, c0, r1 is allowed";
string PS_108 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.rgab, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_109_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_3_0 : cmp source reg combination r0.bgr, c0, r1 is allowed";
string PS_109 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.bgr, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_110_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_3_0 : cmp source reg combination r0.rbg, c0, r1 is allowed";
string PS_110 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.rbg, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_111_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_3_0 : cmp source reg combination r0.gar, c0, r1 is allowed";
string PS_111 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.gar, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_112_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_3_0 : cmp source reg combination r0.gr, c0, r1 is allowed";
string PS_112 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.gr, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_113_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_3_0 : cmp source reg combination r0.ab, c0, r1 is allowed";
string PS_113 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.ab, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_114_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_3_0 : cmp source reg combination r0.bg, c0, r1 is allowed";
string PS_114 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0.bg, c0, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_115_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_3_0 : cmp source reg combination r0, c0.r, r1 is allowed";
string PS_115 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.r, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_116_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_3_0 : cmp source reg combination r0, c0.g, r1 is allowed";
string PS_116 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.g, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_117_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_3_0 : cmp source reg combination r0, c0.b, r1 is allowed";
string PS_117 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.b, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_118_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_3_0 : cmp source reg combination r0, c0.a, r1 is allowed";
string PS_118 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.a, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_119_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_3_0 : cmp source reg combination r0, c0.gbra, r1 is allowed";
string PS_119 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.gbra, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_120_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_3_0 : cmp source reg combination r0, c0.brga, r1 is allowed";
string PS_120 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.brga, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_121_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_3_0 : cmp source reg combination r0, c0.abgr, r1 is allowed";
string PS_121 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.abgr, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_122_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_3_0 : cmp source reg combination r0, c0.agrb, r1 is allowed";
string PS_122 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.agrb, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_123_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_3_0 : cmp source reg combination r0, c0.rbga, r1 is allowed";
string PS_123 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.rbga, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_124_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_3_0 : cmp source reg combination r0, c0.rgab, r1 is allowed";
string PS_124 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.rgab, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_125_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_125_Desc = "ps_3_0 : cmp source reg combination r0, c0.bgr, r1 is allowed";
string PS_125 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.bgr, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_126_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_3_0 : cmp source reg combination r0, c0.rbg, r1 is allowed";
string PS_126 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.rbg, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_127_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_3_0 : cmp source reg combination r0, c0.gar, r1 is allowed";
string PS_127 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.gar, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_128_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_3_0 : cmp source reg combination r0, c0.gr, r1 is allowed";
string PS_128 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.gr, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_129_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_3_0 : cmp source reg combination r0, c0.ab, r1 is allowed";
string PS_129 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.ab, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_130_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_3_0 : cmp source reg combination r0, c0.bg, r1 is allowed";
string PS_130 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0.bg, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_131_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.r is allowed";
string PS_131 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.r "
	"mov oC0, c0 ";

PS_CRITERIA PS_132_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.g is allowed";
string PS_132 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_133_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.b is allowed";
string PS_133 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_134_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.a is allowed";
string PS_134 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_135_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.gbra is allowed";
string PS_135 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.gbra "
	"mov oC0, c0 ";

PS_CRITERIA PS_136_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_136_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.brga is allowed";
string PS_136 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.brga "
	"mov oC0, c0 ";

PS_CRITERIA PS_137_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_137_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.abgr is allowed";
string PS_137 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.abgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_138_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_138_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.agrb is allowed";
string PS_138 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.agrb "
	"mov oC0, c0 ";

PS_CRITERIA PS_139_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_139_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.rbga is allowed";
string PS_139 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.rbga "
	"mov oC0, c0 ";

PS_CRITERIA PS_140_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_140_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.rgab is allowed";
string PS_140 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.rgab "
	"mov oC0, c0 ";

PS_CRITERIA PS_141_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_141_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.bgr is allowed";
string PS_141 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.bgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_142_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_142_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.rbg is allowed";
string PS_142 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.rbg "
	"mov oC0, c0 ";

PS_CRITERIA PS_143_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_143_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.gar is allowed";
string PS_143 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.gar "
	"mov oC0, c0 ";

PS_CRITERIA PS_144_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_144_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.gr is allowed";
string PS_144 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.gr "
	"mov oC0, c0 ";

PS_CRITERIA PS_145_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_145_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.ab is allowed";
string PS_145 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.ab "
	"mov oC0, c0 ";

PS_CRITERIA PS_146_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_146_Desc = "ps_3_0 : cmp source reg combination r0, c0, r1.bg is allowed";
string PS_146 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"cmp r0, r0, c0, r1.bg "
	"mov oC0, c0 ";

PS_CRITERIA PS_147_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_147_Desc = "ps_3_0 : cmp source reg combination r0, v0, v1 is NOT allowed";
string PS_147 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"dcl_color v1 "
	"mov r0, c0 "
	"cmp r0, r0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_148_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_148_Desc = "ps_3_0 : cmp source reg combination r0, c0, c1 is NOT allowed";
string PS_148 = 
	"ps_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, r0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_149_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_149_Desc = "ps_3_0 : cmp source reg combination r0, vPos, vPos is NOT allowed";
string PS_149 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, r0, vPos, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_150_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_150_Desc = "ps_3_0 : cmp source reg combination v0, r0, v1 is NOT allowed";
string PS_150 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"dcl_color v1 "
	"mov r0, c0 "
	"cmp r0, v0, r0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_151_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_151_Desc = "ps_3_0 : cmp source reg combination v0, v1, r0 is NOT allowed";
string PS_151 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"dcl_color v1 "
	"mov r0, c0 "
	"cmp r0, v0, v1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_152_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_152_Desc = "ps_3_0 : cmp source reg combination v0, v1, v2 is NOT allowed";
string PS_152 = 
	"ps_3_0 "
	"dcl_fog v0 "
	"dcl_color v1 "
	"dcl_texcoord v2 "
	"cmp r0, v0, v1, v2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_153_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_153_Desc = "ps_3_0 : cmp source reg combination v0, v1, c0 is NOT allowed";
string PS_153 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"cmp r0, v0, v1, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_154_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_154_Desc = "ps_3_0 : cmp source reg combination v0, v1, vPos is NOT allowed";
string PS_154 = 
	"ps_3_0 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"dcl vPos.xy "
	"cmp r0, v0, v1, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_155_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_155_Desc = "ps_3_0 : cmp source reg combination v0, c0, v1 is NOT allowed";
string PS_155 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"cmp r0, v0, c0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_156_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_156_Desc = "ps_3_0 : cmp source reg combination v0, c0, c1 is NOT allowed";
string PS_156 = 
	"ps_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"cmp r0, v0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_157_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_157_Desc = "ps_3_0 : cmp source reg combination v0, vPos, v1 is NOT allowed";
string PS_157 = 
	"ps_3_0 "
	"dcl_depth v0 "
	"dcl vPos.xy "
	"dcl_tangent v1 "
	"cmp r0, v0, vPos, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_158_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_158_Desc = "ps_3_0 : cmp source reg combination v0, vPos, vPos is NOT allowed";
string PS_158 = 
	"ps_3_0 "
	"dcl_depth v0 "
	"dcl vPos.xy "
	"cmp r0, v0, vPos, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_159_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_159_Desc = "ps_3_0 : cmp source reg combination c0, r0, c1 is NOT allowed";
string PS_159 = 
	"ps_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, c0, r0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_160_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_160_Desc = "ps_3_0 : cmp source reg combination c0, v0, v1 is NOT allowed";
string PS_160 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"cmp r0, c0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_161_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_161_Desc = "ps_3_0 : cmp source reg combination c0, v0, c1 is NOT allowed";
string PS_161 = 
	"ps_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"cmp r0, c0, v0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_162_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_162_Desc = "ps_3_0 : cmp source reg combination c0, c1, r0 is NOT allowed";
string PS_162 = 
	"ps_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"cmp r0, c0, c1, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_163_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_163_Desc = "ps_3_0 : cmp source reg combination c0, c1, v0 is NOT allowed";
string PS_163 = 
	"ps_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"cmp r0, c0, c1, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_164_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_164_Desc = "ps_3_0 : cmp source reg combination c0, c1, c2 is NOT allowed";
string PS_164 = 
	"ps_3_0 "
	"def c2, 1, 1, 1, 1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"cmp r0, c0, c1, c2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_165_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_165_Desc = "ps_3_0 : cmp source reg combination c0, c1, vPos is NOT allowed";
string PS_165 = 
	"ps_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"cmp r0, c0, c1, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_166_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_166_Desc = "ps_3_0 : cmp source reg combination c0, vPos, c1 is NOT allowed";
string PS_166 = 
	"ps_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"cmp r0, c0, vPos, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_167_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_167_Desc = "ps_3_0 : cmp source reg combination c0, vPos, vPos is NOT allowed";
string PS_167 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"cmp r0, c0, vPos, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_168_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_168_Desc = "ps_3_0 : cmp source reg combination vPos, r0, vPos is NOT allowed";
string PS_168 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, vPos, r0, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_169_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_169_Desc = "ps_3_0 : cmp source reg combination vPos, v0, v1 is NOT allowed";
string PS_169 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"dcl_depth v0 "
	"dcl_tangent v1 "
	"cmp r0, vPos, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_170_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_170_Desc = "ps_3_0 : cmp source reg combination vPos, v0, vPos is NOT allowed";
string PS_170 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"dcl_depth v0 "
	"cmp r0, vPos, v0, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_171_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_171_Desc = "ps_3_0 : cmp source reg combination vPos, c0, c1 is NOT allowed";
string PS_171 = 
	"ps_3_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"cmp r0, vPos, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_172_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_172_Desc = "ps_3_0 : cmp source reg combination vPos, c0, vPos is NOT allowed";
string PS_172 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"cmp r0, vPos, c0, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_173_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_173_Desc = "ps_3_0 : cmp source reg combination vPos, vPos, r0 is NOT allowed";
string PS_173 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"mov r0, c0 "
	"cmp r0, vPos, vPos, r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_174_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_174_Desc = "ps_3_0 : cmp source reg combination vPos, vPos, v0 is NOT allowed";
string PS_174 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"dcl_depth v0 "
	"cmp r0, vPos, vPos, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_175_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_175_Desc = "ps_3_0 : cmp source reg combination vPos, vPos, c0 is NOT allowed";
string PS_175 = 
	"ps_3_0 "
	"def c0, 1, 1, 1, 1 "
	"dcl vPos.xy "
	"cmp r0, vPos, vPos, c0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_176_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_176_Desc = "ps_3_0 : cmp source reg combination vPos, vPos, vPos is NOT allowed";
string PS_176 = 
	"ps_3_0 "
	"dcl vPos.xy "
	"cmp r0, vPos, vPos, vPos "
	"mov oC0, c0 ";

PS_CRITERIA PS_177_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_177_Desc = "ps_3_0 : cmp source reg combination b0, b0, b0 is NOT allowed";
string PS_177 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_178_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_178_Desc = "ps_3_0 : cmp source reg combination b0, b0, i0 is NOT allowed";
string PS_178 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_179_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_179_Desc = "ps_3_0 : cmp source reg combination b0, b0, aL is NOT allowed";
string PS_179 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, b0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_180_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_180_Desc = "ps_3_0 : cmp source reg combination b0, b0, p0 is NOT allowed";
string PS_180 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_181_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_181_Desc = "ps_3_0 : cmp source reg combination b0, b0, s0 is NOT allowed";
string PS_181 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_182_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_182_Desc = "ps_3_0 : cmp source reg combination b0, b0, vFace is NOT allowed";
string PS_182 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, b0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_183_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_183_Desc = "ps_3_0 : cmp source reg combination b0, b0, oC0 is NOT allowed";
string PS_183 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_184_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_184_Desc = "ps_3_0 : cmp source reg combination b0, b0, oDepth is NOT allowed";
string PS_184 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_185_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_185_Desc = "ps_3_0 : cmp source reg combination b0, i0, b0 is NOT allowed";
string PS_185 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_186_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_186_Desc = "ps_3_0 : cmp source reg combination b0, i0, i0 is NOT allowed";
string PS_186 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_187_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_187_Desc = "ps_3_0 : cmp source reg combination b0, i0, aL is NOT allowed";
string PS_187 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, i0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_188_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_188_Desc = "ps_3_0 : cmp source reg combination b0, i0, p0 is NOT allowed";
string PS_188 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_189_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_189_Desc = "ps_3_0 : cmp source reg combination b0, i0, s0 is NOT allowed";
string PS_189 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_190_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_190_Desc = "ps_3_0 : cmp source reg combination b0, i0, vFace is NOT allowed";
string PS_190 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, i0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_191_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_191_Desc = "ps_3_0 : cmp source reg combination b0, i0, oC0 is NOT allowed";
string PS_191 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_192_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_192_Desc = "ps_3_0 : cmp source reg combination b0, i0, oDepth is NOT allowed";
string PS_192 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_193_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_193_Desc = "ps_3_0 : cmp source reg combination b0, aL, b0 is NOT allowed";
string PS_193 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, aL, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_194_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_194_Desc = "ps_3_0 : cmp source reg combination b0, aL, i0 is NOT allowed";
string PS_194 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, aL, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_195_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_195_Desc = "ps_3_0 : cmp source reg combination b0, aL, aL is NOT allowed";
string PS_195 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, aL, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_196_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_196_Desc = "ps_3_0 : cmp source reg combination b0, aL, p0 is NOT allowed";
string PS_196 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, aL, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_197_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_197_Desc = "ps_3_0 : cmp source reg combination b0, aL, s0 is NOT allowed";
string PS_197 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, aL, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_198_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_198_Desc = "ps_3_0 : cmp source reg combination b0, aL, vFace is NOT allowed";
string PS_198 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, aL, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_199_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_199_Desc = "ps_3_0 : cmp source reg combination b0, aL, oC0 is NOT allowed";
string PS_199 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, aL, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_200_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_200_Desc = "ps_3_0 : cmp source reg combination b0, aL, oDepth is NOT allowed";
string PS_200 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, aL, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_201_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_201_Desc = "ps_3_0 : cmp source reg combination b0, p0, b0 is NOT allowed";
string PS_201 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_202_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_202_Desc = "ps_3_0 : cmp source reg combination b0, p0, i0 is NOT allowed";
string PS_202 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_203_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_203_Desc = "ps_3_0 : cmp source reg combination b0, p0, aL is NOT allowed";
string PS_203 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, p0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_204_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_204_Desc = "ps_3_0 : cmp source reg combination b0, p0, p0 is NOT allowed";
string PS_204 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_205_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_205_Desc = "ps_3_0 : cmp source reg combination b0, p0, s0 is NOT allowed";
string PS_205 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_206_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_206_Desc = "ps_3_0 : cmp source reg combination b0, p0, vFace is NOT allowed";
string PS_206 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, p0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_207_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_207_Desc = "ps_3_0 : cmp source reg combination b0, p0, oC0 is NOT allowed";
string PS_207 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_208_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_208_Desc = "ps_3_0 : cmp source reg combination b0, p0, oDepth is NOT allowed";
string PS_208 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_209_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_209_Desc = "ps_3_0 : cmp source reg combination b0, s0, b0 is NOT allowed";
string PS_209 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_210_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_210_Desc = "ps_3_0 : cmp source reg combination b0, s0, i0 is NOT allowed";
string PS_210 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_211_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_211_Desc = "ps_3_0 : cmp source reg combination b0, s0, aL is NOT allowed";
string PS_211 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, s0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_212_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_212_Desc = "ps_3_0 : cmp source reg combination b0, s0, p0 is NOT allowed";
string PS_212 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_213_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_213_Desc = "ps_3_0 : cmp source reg combination b0, s0, s0 is NOT allowed";
string PS_213 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_214_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_214_Desc = "ps_3_0 : cmp source reg combination b0, s0, vFace is NOT allowed";
string PS_214 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, s0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_215_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_215_Desc = "ps_3_0 : cmp source reg combination b0, s0, oC0 is NOT allowed";
string PS_215 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_216_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_216_Desc = "ps_3_0 : cmp source reg combination b0, s0, oDepth is NOT allowed";
string PS_216 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_217_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_217_Desc = "ps_3_0 : cmp source reg combination b0, vFace, b0 is NOT allowed";
string PS_217 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, vFace, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_218_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_218_Desc = "ps_3_0 : cmp source reg combination b0, vFace, i0 is NOT allowed";
string PS_218 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, vFace, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_219_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_219_Desc = "ps_3_0 : cmp source reg combination b0, vFace, aL is NOT allowed";
string PS_219 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, vFace, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_220_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_220_Desc = "ps_3_0 : cmp source reg combination b0, vFace, p0 is NOT allowed";
string PS_220 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, vFace, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_221_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_221_Desc = "ps_3_0 : cmp source reg combination b0, vFace, s0 is NOT allowed";
string PS_221 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, vFace, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_222_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_222_Desc = "ps_3_0 : cmp source reg combination b0, vFace, vFace is NOT allowed";
string PS_222 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, vFace, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_223_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_223_Desc = "ps_3_0 : cmp source reg combination b0, vFace, oC0 is NOT allowed";
string PS_223 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, vFace, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_224_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_224_Desc = "ps_3_0 : cmp source reg combination b0, vFace, oDepth is NOT allowed";
string PS_224 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, vFace, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_225_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_225_Desc = "ps_3_0 : cmp source reg combination b0, oC0, b0 is NOT allowed";
string PS_225 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_226_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_226_Desc = "ps_3_0 : cmp source reg combination b0, oC0, i0 is NOT allowed";
string PS_226 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_227_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_227_Desc = "ps_3_0 : cmp source reg combination b0, oC0, aL is NOT allowed";
string PS_227 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oC0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_228_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_228_Desc = "ps_3_0 : cmp source reg combination b0, oC0, p0 is NOT allowed";
string PS_228 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_229_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_229_Desc = "ps_3_0 : cmp source reg combination b0, oC0, s0 is NOT allowed";
string PS_229 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, b0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_230_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_230_Desc = "ps_3_0 : cmp source reg combination b0, oC0, vFace is NOT allowed";
string PS_230 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oC0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_231_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_231_Desc = "ps_3_0 : cmp source reg combination b0, oC0, oC0 is NOT allowed";
string PS_231 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_232_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_232_Desc = "ps_3_0 : cmp source reg combination b0, oC0, oDepth is NOT allowed";
string PS_232 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_233_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_233_Desc = "ps_3_0 : cmp source reg combination b0, oDepth, b0 is NOT allowed";
string PS_233 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_234_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_234_Desc = "ps_3_0 : cmp source reg combination b0, oDepth, i0 is NOT allowed";
string PS_234 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, b0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_235_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_235_Desc = "ps_3_0 : cmp source reg combination b0, oDepth, aL is NOT allowed";
string PS_235 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oDepth, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_236_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_236_Desc = "ps_3_0 : cmp source reg combination b0, oDepth, p0 is NOT allowed";
string PS_236 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_237_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_237_Desc = "ps_3_0 : cmp source reg combination b0, oDepth, s0 is NOT allowed";
string PS_237 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, b0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_238_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_238_Desc = "ps_3_0 : cmp source reg combination b0, oDepth, vFace is NOT allowed";
string PS_238 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oDepth, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_239_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_239_Desc = "ps_3_0 : cmp source reg combination b0, oDepth, oC0 is NOT allowed";
string PS_239 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_240_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_240_Desc = "ps_3_0 : cmp source reg combination b0, oDepth, oDepth is NOT allowed";
string PS_240 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, b0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_241_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_241_Desc = "ps_3_0 : cmp source reg combination i0, b0, b0 is NOT allowed";
string PS_241 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_242_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_242_Desc = "ps_3_0 : cmp source reg combination i0, b0, i0 is NOT allowed";
string PS_242 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_243_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_243_Desc = "ps_3_0 : cmp source reg combination i0, b0, aL is NOT allowed";
string PS_243 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, b0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_244_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_244_Desc = "ps_3_0 : cmp source reg combination i0, b0, p0 is NOT allowed";
string PS_244 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_245_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_245_Desc = "ps_3_0 : cmp source reg combination i0, b0, s0 is NOT allowed";
string PS_245 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, i0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_246_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_246_Desc = "ps_3_0 : cmp source reg combination i0, b0, vFace is NOT allowed";
string PS_246 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, b0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_247_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_247_Desc = "ps_3_0 : cmp source reg combination i0, b0, oC0 is NOT allowed";
string PS_247 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_248_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_248_Desc = "ps_3_0 : cmp source reg combination i0, b0, oDepth is NOT allowed";
string PS_248 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_249_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_249_Desc = "ps_3_0 : cmp source reg combination i0, i0, b0 is NOT allowed";
string PS_249 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_250_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_250_Desc = "ps_3_0 : cmp source reg combination i0, i0, i0 is NOT allowed";
string PS_250 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_251_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_251_Desc = "ps_3_0 : cmp source reg combination i0, i0, aL is NOT allowed";
string PS_251 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, i0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_252_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_252_Desc = "ps_3_0 : cmp source reg combination i0, i0, p0 is NOT allowed";
string PS_252 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_253_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_253_Desc = "ps_3_0 : cmp source reg combination i0, i0, s0 is NOT allowed";
string PS_253 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, i0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_254_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_254_Desc = "ps_3_0 : cmp source reg combination i0, i0, vFace is NOT allowed";
string PS_254 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, i0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_255_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_255_Desc = "ps_3_0 : cmp source reg combination i0, i0, oC0 is NOT allowed";
string PS_255 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_256_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_256_Desc = "ps_3_0 : cmp source reg combination i0, i0, oDepth is NOT allowed";
string PS_256 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_257_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_257_Desc = "ps_3_0 : cmp source reg combination i0, aL, b0 is NOT allowed";
string PS_257 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, aL, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_258_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_258_Desc = "ps_3_0 : cmp source reg combination i0, aL, i0 is NOT allowed";
string PS_258 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, aL, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_259_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_259_Desc = "ps_3_0 : cmp source reg combination i0, aL, aL is NOT allowed";
string PS_259 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, aL, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_260_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_260_Desc = "ps_3_0 : cmp source reg combination i0, aL, p0 is NOT allowed";
string PS_260 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, aL, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_261_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_261_Desc = "ps_3_0 : cmp source reg combination i0, aL, s0 is NOT allowed";
string PS_261 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, i0, aL, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_262_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_262_Desc = "ps_3_0 : cmp source reg combination i0, aL, vFace is NOT allowed";
string PS_262 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, aL, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_263_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_263_Desc = "ps_3_0 : cmp source reg combination i0, aL, oC0 is NOT allowed";
string PS_263 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, aL, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_264_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_264_Desc = "ps_3_0 : cmp source reg combination i0, aL, oDepth is NOT allowed";
string PS_264 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, aL, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_265_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_265_Desc = "ps_3_0 : cmp source reg combination i0, p0, b0 is NOT allowed";
string PS_265 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_266_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_266_Desc = "ps_3_0 : cmp source reg combination i0, p0, i0 is NOT allowed";
string PS_266 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_267_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_267_Desc = "ps_3_0 : cmp source reg combination i0, p0, aL is NOT allowed";
string PS_267 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, p0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_268_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_268_Desc = "ps_3_0 : cmp source reg combination i0, p0, p0 is NOT allowed";
string PS_268 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_269_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_269_Desc = "ps_3_0 : cmp source reg combination i0, p0, s0 is NOT allowed";
string PS_269 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_270_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_270_Desc = "ps_3_0 : cmp source reg combination i0, p0, vFace is NOT allowed";
string PS_270 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, p0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_271_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_271_Desc = "ps_3_0 : cmp source reg combination i0, p0, oC0 is NOT allowed";
string PS_271 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_272_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_272_Desc = "ps_3_0 : cmp source reg combination i0, p0, oDepth is NOT allowed";
string PS_272 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_273_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_273_Desc = "ps_3_0 : cmp source reg combination i0, s0, b0 is NOT allowed";
string PS_273 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_274_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_274_Desc = "ps_3_0 : cmp source reg combination i0, s0, i0 is NOT allowed";
string PS_274 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_275_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_275_Desc = "ps_3_0 : cmp source reg combination i0, s0, aL is NOT allowed";
string PS_275 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, s0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_276_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_276_Desc = "ps_3_0 : cmp source reg combination i0, s0, p0 is NOT allowed";
string PS_276 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_277_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_277_Desc = "ps_3_0 : cmp source reg combination i0, s0, s0 is NOT allowed";
string PS_277 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_278_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_278_Desc = "ps_3_0 : cmp source reg combination i0, s0, vFace is NOT allowed";
string PS_278 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, s0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_279_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_279_Desc = "ps_3_0 : cmp source reg combination i0, s0, oC0 is NOT allowed";
string PS_279 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_280_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_280_Desc = "ps_3_0 : cmp source reg combination i0, s0, oDepth is NOT allowed";
string PS_280 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_281_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_281_Desc = "ps_3_0 : cmp source reg combination i0, vFace, b0 is NOT allowed";
string PS_281 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, vFace, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_282_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_282_Desc = "ps_3_0 : cmp source reg combination i0, vFace, i0 is NOT allowed";
string PS_282 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, vFace, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_283_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_283_Desc = "ps_3_0 : cmp source reg combination i0, vFace, aL is NOT allowed";
string PS_283 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, vFace, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_284_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_284_Desc = "ps_3_0 : cmp source reg combination i0, vFace, p0 is NOT allowed";
string PS_284 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, vFace, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_285_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_285_Desc = "ps_3_0 : cmp source reg combination i0, vFace, s0 is NOT allowed";
string PS_285 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, vFace, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_286_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_286_Desc = "ps_3_0 : cmp source reg combination i0, vFace, vFace is NOT allowed";
string PS_286 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, vFace, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_287_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_287_Desc = "ps_3_0 : cmp source reg combination i0, vFace, oC0 is NOT allowed";
string PS_287 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, vFace, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_288_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_288_Desc = "ps_3_0 : cmp source reg combination i0, vFace, oDepth is NOT allowed";
string PS_288 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, vFace, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_289_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_289_Desc = "ps_3_0 : cmp source reg combination i0, oC0, b0 is NOT allowed";
string PS_289 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_290_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_290_Desc = "ps_3_0 : cmp source reg combination i0, oC0, i0 is NOT allowed";
string PS_290 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_291_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_291_Desc = "ps_3_0 : cmp source reg combination i0, oC0, aL is NOT allowed";
string PS_291 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oC0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_292_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_292_Desc = "ps_3_0 : cmp source reg combination i0, oC0, p0 is NOT allowed";
string PS_292 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_293_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_293_Desc = "ps_3_0 : cmp source reg combination i0, oC0, s0 is NOT allowed";
string PS_293 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_294_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_294_Desc = "ps_3_0 : cmp source reg combination i0, oC0, vFace is NOT allowed";
string PS_294 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oC0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_295_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_295_Desc = "ps_3_0 : cmp source reg combination i0, oC0, oC0 is NOT allowed";
string PS_295 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_296_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_296_Desc = "ps_3_0 : cmp source reg combination i0, oC0, oDepth is NOT allowed";
string PS_296 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_297_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_297_Desc = "ps_3_0 : cmp source reg combination i0, oDepth, b0 is NOT allowed";
string PS_297 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_298_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_298_Desc = "ps_3_0 : cmp source reg combination i0, oDepth, i0 is NOT allowed";
string PS_298 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_299_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_299_Desc = "ps_3_0 : cmp source reg combination i0, oDepth, aL is NOT allowed";
string PS_299 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oDepth, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_300_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_300_Desc = "ps_3_0 : cmp source reg combination i0, oDepth, p0 is NOT allowed";
string PS_300 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_301_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_301_Desc = "ps_3_0 : cmp source reg combination i0, oDepth, s0 is NOT allowed";
string PS_301 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, i0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_302_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_302_Desc = "ps_3_0 : cmp source reg combination i0, oDepth, vFace is NOT allowed";
string PS_302 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oDepth, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_303_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_303_Desc = "ps_3_0 : cmp source reg combination i0, oDepth, oC0 is NOT allowed";
string PS_303 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_304_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_304_Desc = "ps_3_0 : cmp source reg combination i0, oDepth, oDepth is NOT allowed";
string PS_304 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, i0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_305_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_305_Desc = "ps_3_0 : cmp source reg combination aL, b0, b0 is NOT allowed";
string PS_305 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_306_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_306_Desc = "ps_3_0 : cmp source reg combination aL, b0, i0 is NOT allowed";
string PS_306 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, aL, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_307_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_307_Desc = "ps_3_0 : cmp source reg combination aL, b0, aL is NOT allowed";
string PS_307 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, b0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_308_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_308_Desc = "ps_3_0 : cmp source reg combination aL, b0, p0 is NOT allowed";
string PS_308 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_309_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_309_Desc = "ps_3_0 : cmp source reg combination aL, b0, s0 is NOT allowed";
string PS_309 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_cube s0 "
	"cmp r0, aL, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_310_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_310_Desc = "ps_3_0 : cmp source reg combination aL, b0, vFace is NOT allowed";
string PS_310 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, b0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_311_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_311_Desc = "ps_3_0 : cmp source reg combination aL, b0, oC0 is NOT allowed";
string PS_311 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_312_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_312_Desc = "ps_3_0 : cmp source reg combination aL, b0, oDepth is NOT allowed";
string PS_312 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_313_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_313_Desc = "ps_3_0 : cmp source reg combination aL, i0, b0 is NOT allowed";
string PS_313 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_314_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_314_Desc = "ps_3_0 : cmp source reg combination aL, i0, i0 is NOT allowed";
string PS_314 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_315_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_315_Desc = "ps_3_0 : cmp source reg combination aL, i0, aL is NOT allowed";
string PS_315 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, i0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_316_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_316_Desc = "ps_3_0 : cmp source reg combination aL, i0, p0 is NOT allowed";
string PS_316 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_317_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_317_Desc = "ps_3_0 : cmp source reg combination aL, i0, s0 is NOT allowed";
string PS_317 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, aL, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_318_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_318_Desc = "ps_3_0 : cmp source reg combination aL, i0, vFace is NOT allowed";
string PS_318 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, i0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_319_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_319_Desc = "ps_3_0 : cmp source reg combination aL, i0, oC0 is NOT allowed";
string PS_319 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_320_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_320_Desc = "ps_3_0 : cmp source reg combination aL, i0, oDepth is NOT allowed";
string PS_320 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_321_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_321_Desc = "ps_3_0 : cmp source reg combination aL, aL, b0 is NOT allowed";
string PS_321 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, aL, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_322_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_322_Desc = "ps_3_0 : cmp source reg combination aL, aL, i0 is NOT allowed";
string PS_322 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, aL, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_323_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_323_Desc = "ps_3_0 : cmp source reg combination aL, aL, aL is NOT allowed";
string PS_323 = 
	"ps_3_0 "
	"cmp r0, aL, aL, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_324_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_324_Desc = "ps_3_0 : cmp source reg combination aL, aL, p0 is NOT allowed";
string PS_324 = 
	"ps_3_0 "
	"cmp r0, aL, aL, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_325_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_325_Desc = "ps_3_0 : cmp source reg combination aL, aL, s0 is NOT allowed";
string PS_325 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, aL, aL, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_326_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_326_Desc = "ps_3_0 : cmp source reg combination aL, aL, vFace is NOT allowed";
string PS_326 = 
	"ps_3_0 "
	"cmp r0, aL, aL, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_327_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_327_Desc = "ps_3_0 : cmp source reg combination aL, aL, oC0 is NOT allowed";
string PS_327 = 
	"ps_3_0 "
	"cmp r0, aL, aL, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_328_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_328_Desc = "ps_3_0 : cmp source reg combination aL, aL, oDepth is NOT allowed";
string PS_328 = 
	"ps_3_0 "
	"cmp r0, aL, aL, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_329_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_329_Desc = "ps_3_0 : cmp source reg combination aL, p0, b0 is NOT allowed";
string PS_329 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_330_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_330_Desc = "ps_3_0 : cmp source reg combination aL, p0, i0 is NOT allowed";
string PS_330 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_331_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_331_Desc = "ps_3_0 : cmp source reg combination aL, p0, aL is NOT allowed";
string PS_331 = 
	"ps_3_0 "
	"cmp r0, aL, p0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_332_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_332_Desc = "ps_3_0 : cmp source reg combination aL, p0, p0 is NOT allowed";
string PS_332 = 
	"ps_3_0 "
	"cmp r0, aL, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_333_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_333_Desc = "ps_3_0 : cmp source reg combination aL, p0, s0 is NOT allowed";
string PS_333 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, aL, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_334_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_334_Desc = "ps_3_0 : cmp source reg combination aL, p0, vFace is NOT allowed";
string PS_334 = 
	"ps_3_0 "
	"cmp r0, aL, p0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_335_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_335_Desc = "ps_3_0 : cmp source reg combination aL, p0, oC0 is NOT allowed";
string PS_335 = 
	"ps_3_0 "
	"cmp r0, aL, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_336_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_336_Desc = "ps_3_0 : cmp source reg combination aL, p0, oDepth is NOT allowed";
string PS_336 = 
	"ps_3_0 "
	"cmp r0, aL, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_337_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_337_Desc = "ps_3_0 : cmp source reg combination aL, s0, b0 is NOT allowed";
string PS_337 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, aL, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_338_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_338_Desc = "ps_3_0 : cmp source reg combination aL, s0, i0 is NOT allowed";
string PS_338 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"cmp r0, aL, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_339_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_339_Desc = "ps_3_0 : cmp source reg combination aL, s0, aL is NOT allowed";
string PS_339 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, aL, s0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_340_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_340_Desc = "ps_3_0 : cmp source reg combination aL, s0, p0 is NOT allowed";
string PS_340 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, aL, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_341_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_341_Desc = "ps_3_0 : cmp source reg combination aL, s0, s0 is NOT allowed";
string PS_341 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, aL, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_342_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_342_Desc = "ps_3_0 : cmp source reg combination aL, s0, vFace is NOT allowed";
string PS_342 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, aL, s0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_343_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_343_Desc = "ps_3_0 : cmp source reg combination aL, s0, oC0 is NOT allowed";
string PS_343 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, aL, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_344_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_344_Desc = "ps_3_0 : cmp source reg combination aL, s0, oDepth is NOT allowed";
string PS_344 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, aL, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_345_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_345_Desc = "ps_3_0 : cmp source reg combination aL, vFace, b0 is NOT allowed";
string PS_345 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, vFace, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_346_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_346_Desc = "ps_3_0 : cmp source reg combination aL, vFace, i0 is NOT allowed";
string PS_346 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, vFace, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_347_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_347_Desc = "ps_3_0 : cmp source reg combination aL, vFace, aL is NOT allowed";
string PS_347 = 
	"ps_3_0 "
	"cmp r0, aL, vFace, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_348_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_348_Desc = "ps_3_0 : cmp source reg combination aL, vFace, p0 is NOT allowed";
string PS_348 = 
	"ps_3_0 "
	"cmp r0, aL, vFace, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_349_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_349_Desc = "ps_3_0 : cmp source reg combination aL, vFace, s0 is NOT allowed";
string PS_349 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, aL, vFace, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_350_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_350_Desc = "ps_3_0 : cmp source reg combination aL, vFace, vFace is NOT allowed";
string PS_350 = 
	"ps_3_0 "
	"cmp r0, aL, vFace, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_351_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_351_Desc = "ps_3_0 : cmp source reg combination aL, vFace, oC0 is NOT allowed";
string PS_351 = 
	"ps_3_0 "
	"cmp r0, aL, vFace, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_352_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_352_Desc = "ps_3_0 : cmp source reg combination aL, vFace, oDepth is NOT allowed";
string PS_352 = 
	"ps_3_0 "
	"cmp r0, aL, vFace, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_353_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_353_Desc = "ps_3_0 : cmp source reg combination aL, oC0, b0 is NOT allowed";
string PS_353 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_354_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_354_Desc = "ps_3_0 : cmp source reg combination aL, oC0, i0 is NOT allowed";
string PS_354 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_355_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_355_Desc = "ps_3_0 : cmp source reg combination aL, oC0, aL is NOT allowed";
string PS_355 = 
	"ps_3_0 "
	"cmp r0, aL, oC0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_356_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_356_Desc = "ps_3_0 : cmp source reg combination aL, oC0, p0 is NOT allowed";
string PS_356 = 
	"ps_3_0 "
	"cmp r0, aL, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_357_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_357_Desc = "ps_3_0 : cmp source reg combination aL, oC0, s0 is NOT allowed";
string PS_357 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, aL, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_358_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_358_Desc = "ps_3_0 : cmp source reg combination aL, oC0, vFace is NOT allowed";
string PS_358 = 
	"ps_3_0 "
	"cmp r0, aL, oC0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_359_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_359_Desc = "ps_3_0 : cmp source reg combination aL, oC0, oC0 is NOT allowed";
string PS_359 = 
	"ps_3_0 "
	"cmp r0, aL, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_360_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_360_Desc = "ps_3_0 : cmp source reg combination aL, oC0, oDepth is NOT allowed";
string PS_360 = 
	"ps_3_0 "
	"cmp r0, aL, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_361_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_361_Desc = "ps_3_0 : cmp source reg combination aL, oDepth, b0 is NOT allowed";
string PS_361 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, aL, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_362_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_362_Desc = "ps_3_0 : cmp source reg combination aL, oDepth, i0 is NOT allowed";
string PS_362 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, aL, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_363_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_363_Desc = "ps_3_0 : cmp source reg combination aL, oDepth, aL is NOT allowed";
string PS_363 = 
	"ps_3_0 "
	"cmp r0, aL, oDepth, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_364_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_364_Desc = "ps_3_0 : cmp source reg combination aL, oDepth, p0 is NOT allowed";
string PS_364 = 
	"ps_3_0 "
	"cmp r0, aL, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_365_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_365_Desc = "ps_3_0 : cmp source reg combination aL, oDepth, s0 is NOT allowed";
string PS_365 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, aL, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_366_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_366_Desc = "ps_3_0 : cmp source reg combination aL, oDepth, vFace is NOT allowed";
string PS_366 = 
	"ps_3_0 "
	"cmp r0, aL, oDepth, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_367_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_367_Desc = "ps_3_0 : cmp source reg combination aL, oDepth, oC0 is NOT allowed";
string PS_367 = 
	"ps_3_0 "
	"cmp r0, aL, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_368_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_368_Desc = "ps_3_0 : cmp source reg combination aL, oDepth, oDepth is NOT allowed";
string PS_368 = 
	"ps_3_0 "
	"cmp r0, aL, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_369_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_369_Desc = "ps_3_0 : cmp source reg combination p0, b0, b0 is NOT allowed";
string PS_369 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_370_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_370_Desc = "ps_3_0 : cmp source reg combination p0, b0, i0 is NOT allowed";
string PS_370 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, p0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_371_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_371_Desc = "ps_3_0 : cmp source reg combination p0, b0, aL is NOT allowed";
string PS_371 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, b0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_372_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_372_Desc = "ps_3_0 : cmp source reg combination p0, b0, p0 is NOT allowed";
string PS_372 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_373_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_373_Desc = "ps_3_0 : cmp source reg combination p0, b0, s0 is NOT allowed";
string PS_373 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_cube s0 "
	"cmp r0, p0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_374_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_374_Desc = "ps_3_0 : cmp source reg combination p0, b0, vFace is NOT allowed";
string PS_374 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, b0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_375_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_375_Desc = "ps_3_0 : cmp source reg combination p0, b0, oC0 is NOT allowed";
string PS_375 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_376_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_376_Desc = "ps_3_0 : cmp source reg combination p0, b0, oDepth is NOT allowed";
string PS_376 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_377_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_377_Desc = "ps_3_0 : cmp source reg combination p0, i0, b0 is NOT allowed";
string PS_377 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_378_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_378_Desc = "ps_3_0 : cmp source reg combination p0, i0, i0 is NOT allowed";
string PS_378 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_379_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_379_Desc = "ps_3_0 : cmp source reg combination p0, i0, aL is NOT allowed";
string PS_379 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, i0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_380_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_380_Desc = "ps_3_0 : cmp source reg combination p0, i0, p0 is NOT allowed";
string PS_380 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_381_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_381_Desc = "ps_3_0 : cmp source reg combination p0, i0, s0 is NOT allowed";
string PS_381 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, p0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_382_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_382_Desc = "ps_3_0 : cmp source reg combination p0, i0, vFace is NOT allowed";
string PS_382 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, i0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_383_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_383_Desc = "ps_3_0 : cmp source reg combination p0, i0, oC0 is NOT allowed";
string PS_383 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_384_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_384_Desc = "ps_3_0 : cmp source reg combination p0, i0, oDepth is NOT allowed";
string PS_384 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_385_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_385_Desc = "ps_3_0 : cmp source reg combination p0, aL, b0 is NOT allowed";
string PS_385 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, aL, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_386_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_386_Desc = "ps_3_0 : cmp source reg combination p0, aL, i0 is NOT allowed";
string PS_386 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, aL, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_387_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_387_Desc = "ps_3_0 : cmp source reg combination p0, aL, aL is NOT allowed";
string PS_387 = 
	"ps_3_0 "
	"cmp r0, p0, aL, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_388_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_388_Desc = "ps_3_0 : cmp source reg combination p0, aL, p0 is NOT allowed";
string PS_388 = 
	"ps_3_0 "
	"cmp r0, p0, aL, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_389_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_389_Desc = "ps_3_0 : cmp source reg combination p0, aL, s0 is NOT allowed";
string PS_389 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, p0, aL, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_390_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_390_Desc = "ps_3_0 : cmp source reg combination p0, aL, vFace is NOT allowed";
string PS_390 = 
	"ps_3_0 "
	"cmp r0, p0, aL, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_391_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_391_Desc = "ps_3_0 : cmp source reg combination p0, aL, oC0 is NOT allowed";
string PS_391 = 
	"ps_3_0 "
	"cmp r0, p0, aL, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_392_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_392_Desc = "ps_3_0 : cmp source reg combination p0, aL, oDepth is NOT allowed";
string PS_392 = 
	"ps_3_0 "
	"cmp r0, p0, aL, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_393_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_393_Desc = "ps_3_0 : cmp source reg combination p0, p0, b0 is NOT allowed";
string PS_393 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_394_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_394_Desc = "ps_3_0 : cmp source reg combination p0, p0, i0 is NOT allowed";
string PS_394 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_395_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_395_Desc = "ps_3_0 : cmp source reg combination p0, p0, aL is NOT allowed";
string PS_395 = 
	"ps_3_0 "
	"cmp r0, p0, p0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_396_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_396_Desc = "ps_3_0 : cmp source reg combination p0, p0, p0 is NOT allowed";
string PS_396 = 
	"ps_3_0 "
	"cmp r0, p0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_397_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_397_Desc = "ps_3_0 : cmp source reg combination p0, p0, s0 is NOT allowed";
string PS_397 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, p0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_398_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_398_Desc = "ps_3_0 : cmp source reg combination p0, p0, vFace is NOT allowed";
string PS_398 = 
	"ps_3_0 "
	"cmp r0, p0, p0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_399_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_399_Desc = "ps_3_0 : cmp source reg combination p0, p0, oC0 is NOT allowed";
string PS_399 = 
	"ps_3_0 "
	"cmp r0, p0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_400_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_400_Desc = "ps_3_0 : cmp source reg combination p0, p0, oDepth is NOT allowed";
string PS_400 = 
	"ps_3_0 "
	"cmp r0, p0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_401_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_401_Desc = "ps_3_0 : cmp source reg combination p0, s0, b0 is NOT allowed";
string PS_401 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, p0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_402_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_402_Desc = "ps_3_0 : cmp source reg combination p0, s0, i0 is NOT allowed";
string PS_402 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"cmp r0, p0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_403_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_403_Desc = "ps_3_0 : cmp source reg combination p0, s0, aL is NOT allowed";
string PS_403 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, p0, s0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_404_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_404_Desc = "ps_3_0 : cmp source reg combination p0, s0, p0 is NOT allowed";
string PS_404 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, p0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_405_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_405_Desc = "ps_3_0 : cmp source reg combination p0, s0, s0 is NOT allowed";
string PS_405 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, p0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_406_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_406_Desc = "ps_3_0 : cmp source reg combination p0, s0, vFace is NOT allowed";
string PS_406 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, p0, s0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_407_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_407_Desc = "ps_3_0 : cmp source reg combination p0, s0, oC0 is NOT allowed";
string PS_407 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, p0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_408_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_408_Desc = "ps_3_0 : cmp source reg combination p0, s0, oDepth is NOT allowed";
string PS_408 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, p0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_409_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_409_Desc = "ps_3_0 : cmp source reg combination p0, vFace, b0 is NOT allowed";
string PS_409 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, vFace, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_410_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_410_Desc = "ps_3_0 : cmp source reg combination p0, vFace, i0 is NOT allowed";
string PS_410 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, vFace, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_411_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_411_Desc = "ps_3_0 : cmp source reg combination p0, vFace, aL is NOT allowed";
string PS_411 = 
	"ps_3_0 "
	"cmp r0, p0, vFace, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_412_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_412_Desc = "ps_3_0 : cmp source reg combination p0, vFace, p0 is NOT allowed";
string PS_412 = 
	"ps_3_0 "
	"cmp r0, p0, vFace, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_413_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_413_Desc = "ps_3_0 : cmp source reg combination p0, vFace, s0 is NOT allowed";
string PS_413 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, p0, vFace, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_414_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_414_Desc = "ps_3_0 : cmp source reg combination p0, vFace, vFace is NOT allowed";
string PS_414 = 
	"ps_3_0 "
	"cmp r0, p0, vFace, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_415_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_415_Desc = "ps_3_0 : cmp source reg combination p0, vFace, oC0 is NOT allowed";
string PS_415 = 
	"ps_3_0 "
	"cmp r0, p0, vFace, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_416_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_416_Desc = "ps_3_0 : cmp source reg combination p0, vFace, oDepth is NOT allowed";
string PS_416 = 
	"ps_3_0 "
	"cmp r0, p0, vFace, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_417_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_417_Desc = "ps_3_0 : cmp source reg combination p0, oC0, b0 is NOT allowed";
string PS_417 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_418_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_418_Desc = "ps_3_0 : cmp source reg combination p0, oC0, i0 is NOT allowed";
string PS_418 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_419_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_419_Desc = "ps_3_0 : cmp source reg combination p0, oC0, aL is NOT allowed";
string PS_419 = 
	"ps_3_0 "
	"cmp r0, p0, oC0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_420_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_420_Desc = "ps_3_0 : cmp source reg combination p0, oC0, p0 is NOT allowed";
string PS_420 = 
	"ps_3_0 "
	"cmp r0, p0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_421_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_421_Desc = "ps_3_0 : cmp source reg combination p0, oC0, s0 is NOT allowed";
string PS_421 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, p0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_422_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_422_Desc = "ps_3_0 : cmp source reg combination p0, oC0, vFace is NOT allowed";
string PS_422 = 
	"ps_3_0 "
	"cmp r0, p0, oC0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_423_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_423_Desc = "ps_3_0 : cmp source reg combination p0, oC0, oC0 is NOT allowed";
string PS_423 = 
	"ps_3_0 "
	"cmp r0, p0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_424_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_424_Desc = "ps_3_0 : cmp source reg combination p0, oC0, oDepth is NOT allowed";
string PS_424 = 
	"ps_3_0 "
	"cmp r0, p0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_425_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_425_Desc = "ps_3_0 : cmp source reg combination p0, oDepth, b0 is NOT allowed";
string PS_425 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, p0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_426_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_426_Desc = "ps_3_0 : cmp source reg combination p0, oDepth, i0 is NOT allowed";
string PS_426 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, p0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_427_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_427_Desc = "ps_3_0 : cmp source reg combination p0, oDepth, aL is NOT allowed";
string PS_427 = 
	"ps_3_0 "
	"cmp r0, p0, oDepth, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_428_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_428_Desc = "ps_3_0 : cmp source reg combination p0, oDepth, p0 is NOT allowed";
string PS_428 = 
	"ps_3_0 "
	"cmp r0, p0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_429_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_429_Desc = "ps_3_0 : cmp source reg combination p0, oDepth, s0 is NOT allowed";
string PS_429 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, p0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_430_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_430_Desc = "ps_3_0 : cmp source reg combination p0, oDepth, vFace is NOT allowed";
string PS_430 = 
	"ps_3_0 "
	"cmp r0, p0, oDepth, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_431_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_431_Desc = "ps_3_0 : cmp source reg combination p0, oDepth, oC0 is NOT allowed";
string PS_431 = 
	"ps_3_0 "
	"cmp r0, p0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_432_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_432_Desc = "ps_3_0 : cmp source reg combination p0, oDepth, oDepth is NOT allowed";
string PS_432 = 
	"ps_3_0 "
	"cmp r0, p0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_433_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_433_Desc = "ps_3_0 : cmp source reg combination s0, b0, b0 is NOT allowed";
string PS_433 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, s0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_434_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_434_Desc = "ps_3_0 : cmp source reg combination s0, b0, i0 is NOT allowed";
string PS_434 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, s0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_435_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_435_Desc = "ps_3_0 : cmp source reg combination s0, b0, aL is NOT allowed";
string PS_435 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, s0, b0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_436_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_436_Desc = "ps_3_0 : cmp source reg combination s0, b0, p0 is NOT allowed";
string PS_436 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, s0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_437_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_437_Desc = "ps_3_0 : cmp source reg combination s0, b0, s0 is NOT allowed";
string PS_437 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, s0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_438_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_438_Desc = "ps_3_0 : cmp source reg combination s0, b0, vFace is NOT allowed";
string PS_438 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, s0, b0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_439_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_439_Desc = "ps_3_0 : cmp source reg combination s0, b0, oC0 is NOT allowed";
string PS_439 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, s0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_440_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_440_Desc = "ps_3_0 : cmp source reg combination s0, b0, oDepth is NOT allowed";
string PS_440 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, s0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_441_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_441_Desc = "ps_3_0 : cmp source reg combination s0, i0, b0 is NOT allowed";
string PS_441 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, s0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_442_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_442_Desc = "ps_3_0 : cmp source reg combination s0, i0, i0 is NOT allowed";
string PS_442 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, s0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_443_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_443_Desc = "ps_3_0 : cmp source reg combination s0, i0, aL is NOT allowed";
string PS_443 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, s0, i0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_444_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_444_Desc = "ps_3_0 : cmp source reg combination s0, i0, p0 is NOT allowed";
string PS_444 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, s0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_445_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_445_Desc = "ps_3_0 : cmp source reg combination s0, i0, s0 is NOT allowed";
string PS_445 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, s0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_446_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_446_Desc = "ps_3_0 : cmp source reg combination s0, i0, vFace is NOT allowed";
string PS_446 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, s0, i0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_447_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_447_Desc = "ps_3_0 : cmp source reg combination s0, i0, oC0 is NOT allowed";
string PS_447 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, s0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_448_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_448_Desc = "ps_3_0 : cmp source reg combination s0, i0, oDepth is NOT allowed";
string PS_448 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, s0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_449_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_449_Desc = "ps_3_0 : cmp source reg combination s0, aL, b0 is NOT allowed";
string PS_449 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, s0, aL, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_450_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_450_Desc = "ps_3_0 : cmp source reg combination s0, aL, i0 is NOT allowed";
string PS_450 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, s0, aL, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_451_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_451_Desc = "ps_3_0 : cmp source reg combination s0, aL, aL is NOT allowed";
string PS_451 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, s0, aL, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_452_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_452_Desc = "ps_3_0 : cmp source reg combination s0, aL, p0 is NOT allowed";
string PS_452 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, s0, aL, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_453_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_453_Desc = "ps_3_0 : cmp source reg combination s0, aL, s0 is NOT allowed";
string PS_453 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, s0, aL, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_454_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_454_Desc = "ps_3_0 : cmp source reg combination s0, aL, vFace is NOT allowed";
string PS_454 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, aL, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_455_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_455_Desc = "ps_3_0 : cmp source reg combination s0, aL, oC0 is NOT allowed";
string PS_455 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, aL, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_456_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_456_Desc = "ps_3_0 : cmp source reg combination s0, aL, oDepth is NOT allowed";
string PS_456 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, aL, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_457_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_457_Desc = "ps_3_0 : cmp source reg combination s0, p0, b0 is NOT allowed";
string PS_457 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_cube s0 "
	"cmp r0, s0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_458_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_458_Desc = "ps_3_0 : cmp source reg combination s0, p0, i0 is NOT allowed";
string PS_458 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, s0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_459_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_459_Desc = "ps_3_0 : cmp source reg combination s0, p0, aL is NOT allowed";
string PS_459 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, p0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_460_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_460_Desc = "ps_3_0 : cmp source reg combination s0, p0, p0 is NOT allowed";
string PS_460 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_461_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_461_Desc = "ps_3_0 : cmp source reg combination s0, p0, s0 is NOT allowed";
string PS_461 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_462_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_462_Desc = "ps_3_0 : cmp source reg combination s0, p0, vFace is NOT allowed";
string PS_462 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, p0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_463_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_463_Desc = "ps_3_0 : cmp source reg combination s0, p0, oC0 is NOT allowed";
string PS_463 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_464_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_464_Desc = "ps_3_0 : cmp source reg combination s0, p0, oDepth is NOT allowed";
string PS_464 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_465_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_465_Desc = "ps_3_0 : cmp source reg combination s0, s0, b0 is NOT allowed";
string PS_465 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_cube s0 "
	"cmp r0, s0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_466_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_466_Desc = "ps_3_0 : cmp source reg combination s0, s0, i0 is NOT allowed";
string PS_466 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, s0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_467_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_467_Desc = "ps_3_0 : cmp source reg combination s0, s0, aL is NOT allowed";
string PS_467 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, s0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_468_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_468_Desc = "ps_3_0 : cmp source reg combination s0, s0, p0 is NOT allowed";
string PS_468 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_469_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_469_Desc = "ps_3_0 : cmp source reg combination s0, s0, s0 is NOT allowed";
string PS_469 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_470_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_470_Desc = "ps_3_0 : cmp source reg combination s0, s0, vFace is NOT allowed";
string PS_470 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, s0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_471_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_471_Desc = "ps_3_0 : cmp source reg combination s0, s0, oC0 is NOT allowed";
string PS_471 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_472_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_472_Desc = "ps_3_0 : cmp source reg combination s0, s0, oDepth is NOT allowed";
string PS_472 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_473_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_473_Desc = "ps_3_0 : cmp source reg combination s0, vFace, b0 is NOT allowed";
string PS_473 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_cube s0 "
	"cmp r0, s0, vFace, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_474_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_474_Desc = "ps_3_0 : cmp source reg combination s0, vFace, i0 is NOT allowed";
string PS_474 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, s0, vFace, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_475_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_475_Desc = "ps_3_0 : cmp source reg combination s0, vFace, aL is NOT allowed";
string PS_475 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, vFace, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_476_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_476_Desc = "ps_3_0 : cmp source reg combination s0, vFace, p0 is NOT allowed";
string PS_476 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, vFace, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_477_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_477_Desc = "ps_3_0 : cmp source reg combination s0, vFace, s0 is NOT allowed";
string PS_477 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, vFace, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_478_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_478_Desc = "ps_3_0 : cmp source reg combination s0, vFace, vFace is NOT allowed";
string PS_478 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, vFace, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_479_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_479_Desc = "ps_3_0 : cmp source reg combination s0, vFace, oC0 is NOT allowed";
string PS_479 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, vFace, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_480_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_480_Desc = "ps_3_0 : cmp source reg combination s0, vFace, oDepth is NOT allowed";
string PS_480 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, vFace, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_481_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_481_Desc = "ps_3_0 : cmp source reg combination s0, oC0, b0 is NOT allowed";
string PS_481 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, s0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_482_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_482_Desc = "ps_3_0 : cmp source reg combination s0, oC0, i0 is NOT allowed";
string PS_482 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"cmp r0, s0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_483_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_483_Desc = "ps_3_0 : cmp source reg combination s0, oC0, aL is NOT allowed";
string PS_483 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oC0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_484_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_484_Desc = "ps_3_0 : cmp source reg combination s0, oC0, p0 is NOT allowed";
string PS_484 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_485_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_485_Desc = "ps_3_0 : cmp source reg combination s0, oC0, s0 is NOT allowed";
string PS_485 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_486_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_486_Desc = "ps_3_0 : cmp source reg combination s0, oC0, vFace is NOT allowed";
string PS_486 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oC0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_487_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_487_Desc = "ps_3_0 : cmp source reg combination s0, oC0, oC0 is NOT allowed";
string PS_487 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_488_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_488_Desc = "ps_3_0 : cmp source reg combination s0, oC0, oDepth is NOT allowed";
string PS_488 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_489_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_489_Desc = "ps_3_0 : cmp source reg combination s0, oDepth, b0 is NOT allowed";
string PS_489 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, s0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_490_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_490_Desc = "ps_3_0 : cmp source reg combination s0, oDepth, i0 is NOT allowed";
string PS_490 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"cmp r0, s0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_491_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_491_Desc = "ps_3_0 : cmp source reg combination s0, oDepth, aL is NOT allowed";
string PS_491 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oDepth, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_492_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_492_Desc = "ps_3_0 : cmp source reg combination s0, oDepth, p0 is NOT allowed";
string PS_492 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_493_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_493_Desc = "ps_3_0 : cmp source reg combination s0, oDepth, s0 is NOT allowed";
string PS_493 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_494_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_494_Desc = "ps_3_0 : cmp source reg combination s0, oDepth, vFace is NOT allowed";
string PS_494 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oDepth, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_495_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_495_Desc = "ps_3_0 : cmp source reg combination s0, oDepth, oC0 is NOT allowed";
string PS_495 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, s0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_496_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_496_Desc = "ps_3_0 : cmp source reg combination s0, oDepth, oDepth is NOT allowed";
string PS_496 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, s0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_497_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_497_Desc = "ps_3_0 : cmp source reg combination vFace, b0, b0 is NOT allowed";
string PS_497 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_498_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_498_Desc = "ps_3_0 : cmp source reg combination vFace, b0, i0 is NOT allowed";
string PS_498 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, vFace, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_499_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_499_Desc = "ps_3_0 : cmp source reg combination vFace, b0, aL is NOT allowed";
string PS_499 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, b0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_500_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_500_Desc = "ps_3_0 : cmp source reg combination vFace, b0, p0 is NOT allowed";
string PS_500 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_501_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_501_Desc = "ps_3_0 : cmp source reg combination vFace, b0, s0 is NOT allowed";
string PS_501 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_cube s0 "
	"cmp r0, vFace, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_502_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_502_Desc = "ps_3_0 : cmp source reg combination vFace, b0, vFace is NOT allowed";
string PS_502 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, b0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_503_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_503_Desc = "ps_3_0 : cmp source reg combination vFace, b0, oC0 is NOT allowed";
string PS_503 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_504_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_504_Desc = "ps_3_0 : cmp source reg combination vFace, b0, oDepth is NOT allowed";
string PS_504 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_505_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_505_Desc = "ps_3_0 : cmp source reg combination vFace, i0, b0 is NOT allowed";
string PS_505 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_506_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_506_Desc = "ps_3_0 : cmp source reg combination vFace, i0, i0 is NOT allowed";
string PS_506 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_507_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_507_Desc = "ps_3_0 : cmp source reg combination vFace, i0, aL is NOT allowed";
string PS_507 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, i0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_508_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_508_Desc = "ps_3_0 : cmp source reg combination vFace, i0, p0 is NOT allowed";
string PS_508 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_509_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_509_Desc = "ps_3_0 : cmp source reg combination vFace, i0, s0 is NOT allowed";
string PS_509 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, vFace, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_510_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_510_Desc = "ps_3_0 : cmp source reg combination vFace, i0, vFace is NOT allowed";
string PS_510 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, i0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_511_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_511_Desc = "ps_3_0 : cmp source reg combination vFace, i0, oC0 is NOT allowed";
string PS_511 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_512_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_512_Desc = "ps_3_0 : cmp source reg combination vFace, i0, oDepth is NOT allowed";
string PS_512 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_513_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_513_Desc = "ps_3_0 : cmp source reg combination vFace, aL, b0 is NOT allowed";
string PS_513 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, aL, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_514_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_514_Desc = "ps_3_0 : cmp source reg combination vFace, aL, i0 is NOT allowed";
string PS_514 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, aL, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_515_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_515_Desc = "ps_3_0 : cmp source reg combination vFace, aL, aL is NOT allowed";
string PS_515 = 
	"ps_3_0 "
	"cmp r0, vFace, aL, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_516_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_516_Desc = "ps_3_0 : cmp source reg combination vFace, aL, p0 is NOT allowed";
string PS_516 = 
	"ps_3_0 "
	"cmp r0, vFace, aL, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_517_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_517_Desc = "ps_3_0 : cmp source reg combination vFace, aL, s0 is NOT allowed";
string PS_517 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, vFace, aL, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_518_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_518_Desc = "ps_3_0 : cmp source reg combination vFace, aL, vFace is NOT allowed";
string PS_518 = 
	"ps_3_0 "
	"cmp r0, vFace, aL, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_519_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_519_Desc = "ps_3_0 : cmp source reg combination vFace, aL, oC0 is NOT allowed";
string PS_519 = 
	"ps_3_0 "
	"cmp r0, vFace, aL, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_520_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_520_Desc = "ps_3_0 : cmp source reg combination vFace, aL, oDepth is NOT allowed";
string PS_520 = 
	"ps_3_0 "
	"cmp r0, vFace, aL, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_521_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_521_Desc = "ps_3_0 : cmp source reg combination vFace, p0, b0 is NOT allowed";
string PS_521 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_522_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_522_Desc = "ps_3_0 : cmp source reg combination vFace, p0, i0 is NOT allowed";
string PS_522 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_523_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_523_Desc = "ps_3_0 : cmp source reg combination vFace, p0, aL is NOT allowed";
string PS_523 = 
	"ps_3_0 "
	"cmp r0, vFace, p0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_524_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_524_Desc = "ps_3_0 : cmp source reg combination vFace, p0, p0 is NOT allowed";
string PS_524 = 
	"ps_3_0 "
	"cmp r0, vFace, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_525_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_525_Desc = "ps_3_0 : cmp source reg combination vFace, p0, s0 is NOT allowed";
string PS_525 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, vFace, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_526_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_526_Desc = "ps_3_0 : cmp source reg combination vFace, p0, vFace is NOT allowed";
string PS_526 = 
	"ps_3_0 "
	"cmp r0, vFace, p0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_527_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_527_Desc = "ps_3_0 : cmp source reg combination vFace, p0, oC0 is NOT allowed";
string PS_527 = 
	"ps_3_0 "
	"cmp r0, vFace, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_528_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_528_Desc = "ps_3_0 : cmp source reg combination vFace, p0, oDepth is NOT allowed";
string PS_528 = 
	"ps_3_0 "
	"cmp r0, vFace, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_529_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_529_Desc = "ps_3_0 : cmp source reg combination vFace, s0, b0 is NOT allowed";
string PS_529 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, vFace, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_530_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_530_Desc = "ps_3_0 : cmp source reg combination vFace, s0, i0 is NOT allowed";
string PS_530 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, vFace, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_531_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_531_Desc = "ps_3_0 : cmp source reg combination vFace, s0, aL is NOT allowed";
string PS_531 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, vFace, s0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_532_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_532_Desc = "ps_3_0 : cmp source reg combination vFace, s0, p0 is NOT allowed";
string PS_532 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, vFace, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_533_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_533_Desc = "ps_3_0 : cmp source reg combination vFace, s0, s0 is NOT allowed";
string PS_533 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, vFace, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_534_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_534_Desc = "ps_3_0 : cmp source reg combination vFace, s0, vFace is NOT allowed";
string PS_534 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, vFace, s0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_535_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_535_Desc = "ps_3_0 : cmp source reg combination vFace, s0, oC0 is NOT allowed";
string PS_535 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, vFace, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_536_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_536_Desc = "ps_3_0 : cmp source reg combination vFace, s0, oDepth is NOT allowed";
string PS_536 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, vFace, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_537_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_537_Desc = "ps_3_0 : cmp source reg combination vFace, vFace, b0 is NOT allowed";
string PS_537 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, vFace, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_538_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_538_Desc = "ps_3_0 : cmp source reg combination vFace, vFace, i0 is NOT allowed";
string PS_538 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, vFace, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_539_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_539_Desc = "ps_3_0 : cmp source reg combination vFace, vFace, aL is NOT allowed";
string PS_539 = 
	"ps_3_0 "
	"cmp r0, vFace, vFace, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_540_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_540_Desc = "ps_3_0 : cmp source reg combination vFace, vFace, p0 is NOT allowed";
string PS_540 = 
	"ps_3_0 "
	"cmp r0, vFace, vFace, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_541_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_541_Desc = "ps_3_0 : cmp source reg combination vFace, vFace, s0 is NOT allowed";
string PS_541 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, vFace, vFace, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_542_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_542_Desc = "ps_3_0 : cmp source reg combination vFace, vFace, vFace is NOT allowed";
string PS_542 = 
	"ps_3_0 "
	"cmp r0, vFace, vFace, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_543_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_543_Desc = "ps_3_0 : cmp source reg combination vFace, vFace, oC0 is NOT allowed";
string PS_543 = 
	"ps_3_0 "
	"cmp r0, vFace, vFace, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_544_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_544_Desc = "ps_3_0 : cmp source reg combination vFace, vFace, oDepth is NOT allowed";
string PS_544 = 
	"ps_3_0 "
	"cmp r0, vFace, vFace, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_545_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_545_Desc = "ps_3_0 : cmp source reg combination vFace, oC0, b0 is NOT allowed";
string PS_545 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_546_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_546_Desc = "ps_3_0 : cmp source reg combination vFace, oC0, i0 is NOT allowed";
string PS_546 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_547_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_547_Desc = "ps_3_0 : cmp source reg combination vFace, oC0, aL is NOT allowed";
string PS_547 = 
	"ps_3_0 "
	"cmp r0, vFace, oC0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_548_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_548_Desc = "ps_3_0 : cmp source reg combination vFace, oC0, p0 is NOT allowed";
string PS_548 = 
	"ps_3_0 "
	"cmp r0, vFace, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_549_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_549_Desc = "ps_3_0 : cmp source reg combination vFace, oC0, s0 is NOT allowed";
string PS_549 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, vFace, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_550_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_550_Desc = "ps_3_0 : cmp source reg combination vFace, oC0, vFace is NOT allowed";
string PS_550 = 
	"ps_3_0 "
	"cmp r0, vFace, oC0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_551_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_551_Desc = "ps_3_0 : cmp source reg combination vFace, oC0, oC0 is NOT allowed";
string PS_551 = 
	"ps_3_0 "
	"cmp r0, vFace, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_552_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_552_Desc = "ps_3_0 : cmp source reg combination vFace, oC0, oDepth is NOT allowed";
string PS_552 = 
	"ps_3_0 "
	"cmp r0, vFace, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_553_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_553_Desc = "ps_3_0 : cmp source reg combination vFace, oDepth, b0 is NOT allowed";
string PS_553 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, vFace, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_554_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_554_Desc = "ps_3_0 : cmp source reg combination vFace, oDepth, i0 is NOT allowed";
string PS_554 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, vFace, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_555_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_555_Desc = "ps_3_0 : cmp source reg combination vFace, oDepth, aL is NOT allowed";
string PS_555 = 
	"ps_3_0 "
	"cmp r0, vFace, oDepth, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_556_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_556_Desc = "ps_3_0 : cmp source reg combination vFace, oDepth, p0 is NOT allowed";
string PS_556 = 
	"ps_3_0 "
	"cmp r0, vFace, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_557_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_557_Desc = "ps_3_0 : cmp source reg combination vFace, oDepth, s0 is NOT allowed";
string PS_557 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, vFace, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_558_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_558_Desc = "ps_3_0 : cmp source reg combination vFace, oDepth, vFace is NOT allowed";
string PS_558 = 
	"ps_3_0 "
	"cmp r0, vFace, oDepth, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_559_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_559_Desc = "ps_3_0 : cmp source reg combination vFace, oDepth, oC0 is NOT allowed";
string PS_559 = 
	"ps_3_0 "
	"cmp r0, vFace, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_560_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_560_Desc = "ps_3_0 : cmp source reg combination vFace, oDepth, oDepth is NOT allowed";
string PS_560 = 
	"ps_3_0 "
	"cmp r0, vFace, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_561_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_561_Desc = "ps_3_0 : cmp source reg combination oC0, b0, b0 is NOT allowed";
string PS_561 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_562_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_562_Desc = "ps_3_0 : cmp source reg combination oC0, b0, i0 is NOT allowed";
string PS_562 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, oC0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_563_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_563_Desc = "ps_3_0 : cmp source reg combination oC0, b0, aL is NOT allowed";
string PS_563 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, b0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_564_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_564_Desc = "ps_3_0 : cmp source reg combination oC0, b0, p0 is NOT allowed";
string PS_564 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_565_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_565_Desc = "ps_3_0 : cmp source reg combination oC0, b0, s0 is NOT allowed";
string PS_565 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_2d s0 "
	"cmp r0, oC0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_566_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_566_Desc = "ps_3_0 : cmp source reg combination oC0, b0, vFace is NOT allowed";
string PS_566 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, b0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_567_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_567_Desc = "ps_3_0 : cmp source reg combination oC0, b0, oC0 is NOT allowed";
string PS_567 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_568_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_568_Desc = "ps_3_0 : cmp source reg combination oC0, b0, oDepth is NOT allowed";
string PS_568 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_569_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_569_Desc = "ps_3_0 : cmp source reg combination oC0, i0, b0 is NOT allowed";
string PS_569 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_570_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_570_Desc = "ps_3_0 : cmp source reg combination oC0, i0, i0 is NOT allowed";
string PS_570 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_571_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_571_Desc = "ps_3_0 : cmp source reg combination oC0, i0, aL is NOT allowed";
string PS_571 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, i0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_572_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_572_Desc = "ps_3_0 : cmp source reg combination oC0, i0, p0 is NOT allowed";
string PS_572 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_573_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_573_Desc = "ps_3_0 : cmp source reg combination oC0, i0, s0 is NOT allowed";
string PS_573 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, oC0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_574_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_574_Desc = "ps_3_0 : cmp source reg combination oC0, i0, vFace is NOT allowed";
string PS_574 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, i0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_575_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_575_Desc = "ps_3_0 : cmp source reg combination oC0, i0, oC0 is NOT allowed";
string PS_575 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_576_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_576_Desc = "ps_3_0 : cmp source reg combination oC0, i0, oDepth is NOT allowed";
string PS_576 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_577_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_577_Desc = "ps_3_0 : cmp source reg combination oC0, aL, b0 is NOT allowed";
string PS_577 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, aL, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_578_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_578_Desc = "ps_3_0 : cmp source reg combination oC0, aL, i0 is NOT allowed";
string PS_578 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, aL, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_579_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_579_Desc = "ps_3_0 : cmp source reg combination oC0, aL, aL is NOT allowed";
string PS_579 = 
	"ps_3_0 "
	"cmp r0, oC0, aL, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_580_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_580_Desc = "ps_3_0 : cmp source reg combination oC0, aL, p0 is NOT allowed";
string PS_580 = 
	"ps_3_0 "
	"cmp r0, oC0, aL, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_581_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_581_Desc = "ps_3_0 : cmp source reg combination oC0, aL, s0 is NOT allowed";
string PS_581 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, oC0, aL, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_582_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_582_Desc = "ps_3_0 : cmp source reg combination oC0, aL, vFace is NOT allowed";
string PS_582 = 
	"ps_3_0 "
	"cmp r0, oC0, aL, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_583_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_583_Desc = "ps_3_0 : cmp source reg combination oC0, aL, oC0 is NOT allowed";
string PS_583 = 
	"ps_3_0 "
	"cmp r0, oC0, aL, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_584_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_584_Desc = "ps_3_0 : cmp source reg combination oC0, aL, oDepth is NOT allowed";
string PS_584 = 
	"ps_3_0 "
	"cmp r0, oC0, aL, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_585_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_585_Desc = "ps_3_0 : cmp source reg combination oC0, p0, b0 is NOT allowed";
string PS_585 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_586_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_586_Desc = "ps_3_0 : cmp source reg combination oC0, p0, i0 is NOT allowed";
string PS_586 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_587_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_587_Desc = "ps_3_0 : cmp source reg combination oC0, p0, aL is NOT allowed";
string PS_587 = 
	"ps_3_0 "
	"cmp r0, oC0, p0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_588_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_588_Desc = "ps_3_0 : cmp source reg combination oC0, p0, p0 is NOT allowed";
string PS_588 = 
	"ps_3_0 "
	"cmp r0, oC0, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_589_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_589_Desc = "ps_3_0 : cmp source reg combination oC0, p0, s0 is NOT allowed";
string PS_589 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, oC0, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_590_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_590_Desc = "ps_3_0 : cmp source reg combination oC0, p0, vFace is NOT allowed";
string PS_590 = 
	"ps_3_0 "
	"cmp r0, oC0, p0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_591_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_591_Desc = "ps_3_0 : cmp source reg combination oC0, p0, oC0 is NOT allowed";
string PS_591 = 
	"ps_3_0 "
	"cmp r0, oC0, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_592_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_592_Desc = "ps_3_0 : cmp source reg combination oC0, p0, oDepth is NOT allowed";
string PS_592 = 
	"ps_3_0 "
	"cmp r0, oC0, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_593_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_593_Desc = "ps_3_0 : cmp source reg combination oC0, s0, b0 is NOT allowed";
string PS_593 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_cube s0 "
	"cmp r0, oC0, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_594_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_594_Desc = "ps_3_0 : cmp source reg combination oC0, s0, i0 is NOT allowed";
string PS_594 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"cmp r0, oC0, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_595_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_595_Desc = "ps_3_0 : cmp source reg combination oC0, s0, aL is NOT allowed";
string PS_595 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, oC0, s0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_596_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_596_Desc = "ps_3_0 : cmp source reg combination oC0, s0, p0 is NOT allowed";
string PS_596 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, oC0, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_597_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_597_Desc = "ps_3_0 : cmp source reg combination oC0, s0, s0 is NOT allowed";
string PS_597 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, oC0, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_598_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_598_Desc = "ps_3_0 : cmp source reg combination oC0, s0, vFace is NOT allowed";
string PS_598 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, oC0, s0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_599_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_599_Desc = "ps_3_0 : cmp source reg combination oC0, s0, oC0 is NOT allowed";
string PS_599 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, oC0, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_600_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_600_Desc = "ps_3_0 : cmp source reg combination oC0, s0, oDepth is NOT allowed";
string PS_600 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, oC0, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_601_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_601_Desc = "ps_3_0 : cmp source reg combination oC0, vFace, b0 is NOT allowed";
string PS_601 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, vFace, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_602_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_602_Desc = "ps_3_0 : cmp source reg combination oC0, vFace, i0 is NOT allowed";
string PS_602 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, vFace, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_603_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_603_Desc = "ps_3_0 : cmp source reg combination oC0, vFace, aL is NOT allowed";
string PS_603 = 
	"ps_3_0 "
	"cmp r0, oC0, vFace, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_604_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_604_Desc = "ps_3_0 : cmp source reg combination oC0, vFace, p0 is NOT allowed";
string PS_604 = 
	"ps_3_0 "
	"cmp r0, oC0, vFace, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_605_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_605_Desc = "ps_3_0 : cmp source reg combination oC0, vFace, s0 is NOT allowed";
string PS_605 = 
	"ps_3_0 "
	"dcl_cube s0 "
	"cmp r0, oC0, vFace, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_606_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_606_Desc = "ps_3_0 : cmp source reg combination oC0, vFace, vFace is NOT allowed";
string PS_606 = 
	"ps_3_0 "
	"cmp r0, oC0, vFace, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_607_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_607_Desc = "ps_3_0 : cmp source reg combination oC0, vFace, oC0 is NOT allowed";
string PS_607 = 
	"ps_3_0 "
	"cmp r0, oC0, vFace, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_608_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_608_Desc = "ps_3_0 : cmp source reg combination oC0, vFace, oDepth is NOT allowed";
string PS_608 = 
	"ps_3_0 "
	"cmp r0, oC0, vFace, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_609_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_609_Desc = "ps_3_0 : cmp source reg combination oC0, oC0, b0 is NOT allowed";
string PS_609 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_610_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_610_Desc = "ps_3_0 : cmp source reg combination oC0, oC0, i0 is NOT allowed";
string PS_610 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_611_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_611_Desc = "ps_3_0 : cmp source reg combination oC0, oC0, aL is NOT allowed";
string PS_611 = 
	"ps_3_0 "
	"cmp r0, oC0, oC0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_612_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_612_Desc = "ps_3_0 : cmp source reg combination oC0, oC0, p0 is NOT allowed";
string PS_612 = 
	"ps_3_0 "
	"cmp r0, oC0, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_613_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_613_Desc = "ps_3_0 : cmp source reg combination oC0, oC0, s0 is NOT allowed";
string PS_613 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, oC0, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_614_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_614_Desc = "ps_3_0 : cmp source reg combination oC0, oC0, vFace is NOT allowed";
string PS_614 = 
	"ps_3_0 "
	"cmp r0, oC0, oC0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_615_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_615_Desc = "ps_3_0 : cmp source reg combination oC0, oC0, oC0 is NOT allowed";
string PS_615 = 
	"ps_3_0 "
	"cmp r0, oC0, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_616_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_616_Desc = "ps_3_0 : cmp source reg combination oC0, oC0, oDepth is NOT allowed";
string PS_616 = 
	"ps_3_0 "
	"cmp r0, oC0, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_617_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_617_Desc = "ps_3_0 : cmp source reg combination oC0, oDepth, b0 is NOT allowed";
string PS_617 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oC0, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_618_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_618_Desc = "ps_3_0 : cmp source reg combination oC0, oDepth, i0 is NOT allowed";
string PS_618 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oC0, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_619_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_619_Desc = "ps_3_0 : cmp source reg combination oC0, oDepth, aL is NOT allowed";
string PS_619 = 
	"ps_3_0 "
	"cmp r0, oC0, oDepth, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_620_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_620_Desc = "ps_3_0 : cmp source reg combination oC0, oDepth, p0 is NOT allowed";
string PS_620 = 
	"ps_3_0 "
	"cmp r0, oC0, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_621_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_621_Desc = "ps_3_0 : cmp source reg combination oC0, oDepth, s0 is NOT allowed";
string PS_621 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, oC0, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_622_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_622_Desc = "ps_3_0 : cmp source reg combination oC0, oDepth, vFace is NOT allowed";
string PS_622 = 
	"ps_3_0 "
	"cmp r0, oC0, oDepth, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_623_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_623_Desc = "ps_3_0 : cmp source reg combination oC0, oDepth, oC0 is NOT allowed";
string PS_623 = 
	"ps_3_0 "
	"cmp r0, oC0, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_624_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_624_Desc = "ps_3_0 : cmp source reg combination oC0, oDepth, oDepth is NOT allowed";
string PS_624 = 
	"ps_3_0 "
	"cmp r0, oC0, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_625_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_625_Desc = "ps_3_0 : cmp source reg combination oDepth, b0, b0 is NOT allowed";
string PS_625 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_626_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_626_Desc = "ps_3_0 : cmp source reg combination oDepth, b0, i0 is NOT allowed";
string PS_626 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"cmp r0, oDepth, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_627_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_627_Desc = "ps_3_0 : cmp source reg combination oDepth, b0, aL is NOT allowed";
string PS_627 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, b0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_628_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_628_Desc = "ps_3_0 : cmp source reg combination oDepth, b0, p0 is NOT allowed";
string PS_628 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_629_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_629_Desc = "ps_3_0 : cmp source reg combination oDepth, b0, s0 is NOT allowed";
string PS_629 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, oDepth, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_630_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_630_Desc = "ps_3_0 : cmp source reg combination oDepth, b0, vFace is NOT allowed";
string PS_630 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, b0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_631_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_631_Desc = "ps_3_0 : cmp source reg combination oDepth, b0, oC0 is NOT allowed";
string PS_631 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_632_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_632_Desc = "ps_3_0 : cmp source reg combination oDepth, b0, oDepth is NOT allowed";
string PS_632 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_633_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_633_Desc = "ps_3_0 : cmp source reg combination oDepth, i0, b0 is NOT allowed";
string PS_633 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_634_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_634_Desc = "ps_3_0 : cmp source reg combination oDepth, i0, i0 is NOT allowed";
string PS_634 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_635_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_635_Desc = "ps_3_0 : cmp source reg combination oDepth, i0, aL is NOT allowed";
string PS_635 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, i0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_636_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_636_Desc = "ps_3_0 : cmp source reg combination oDepth, i0, p0 is NOT allowed";
string PS_636 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_637_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_637_Desc = "ps_3_0 : cmp source reg combination oDepth, i0, s0 is NOT allowed";
string PS_637 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_2d s0 "
	"cmp r0, oDepth, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_638_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_638_Desc = "ps_3_0 : cmp source reg combination oDepth, i0, vFace is NOT allowed";
string PS_638 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, i0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_639_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_639_Desc = "ps_3_0 : cmp source reg combination oDepth, i0, oC0 is NOT allowed";
string PS_639 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, i0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_640_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_640_Desc = "ps_3_0 : cmp source reg combination oDepth, i0, oDepth is NOT allowed";
string PS_640 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, i0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_641_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_641_Desc = "ps_3_0 : cmp source reg combination oDepth, aL, b0 is NOT allowed";
string PS_641 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, aL, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_642_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_642_Desc = "ps_3_0 : cmp source reg combination oDepth, aL, i0 is NOT allowed";
string PS_642 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, aL, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_643_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_643_Desc = "ps_3_0 : cmp source reg combination oDepth, aL, aL is NOT allowed";
string PS_643 = 
	"ps_3_0 "
	"cmp r0, oDepth, aL, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_644_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_644_Desc = "ps_3_0 : cmp source reg combination oDepth, aL, p0 is NOT allowed";
string PS_644 = 
	"ps_3_0 "
	"cmp r0, oDepth, aL, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_645_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_645_Desc = "ps_3_0 : cmp source reg combination oDepth, aL, s0 is NOT allowed";
string PS_645 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, oDepth, aL, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_646_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_646_Desc = "ps_3_0 : cmp source reg combination oDepth, aL, vFace is NOT allowed";
string PS_646 = 
	"ps_3_0 "
	"cmp r0, oDepth, aL, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_647_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_647_Desc = "ps_3_0 : cmp source reg combination oDepth, aL, oC0 is NOT allowed";
string PS_647 = 
	"ps_3_0 "
	"cmp r0, oDepth, aL, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_648_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_648_Desc = "ps_3_0 : cmp source reg combination oDepth, aL, oDepth is NOT allowed";
string PS_648 = 
	"ps_3_0 "
	"cmp r0, oDepth, aL, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_649_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_649_Desc = "ps_3_0 : cmp source reg combination oDepth, p0, b0 is NOT allowed";
string PS_649 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, p0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_650_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_650_Desc = "ps_3_0 : cmp source reg combination oDepth, p0, i0 is NOT allowed";
string PS_650 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, p0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_651_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_651_Desc = "ps_3_0 : cmp source reg combination oDepth, p0, aL is NOT allowed";
string PS_651 = 
	"ps_3_0 "
	"cmp r0, oDepth, p0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_652_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_652_Desc = "ps_3_0 : cmp source reg combination oDepth, p0, p0 is NOT allowed";
string PS_652 = 
	"ps_3_0 "
	"cmp r0, oDepth, p0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_653_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_653_Desc = "ps_3_0 : cmp source reg combination oDepth, p0, s0 is NOT allowed";
string PS_653 = 
	"ps_3_0 "
	"dcl_volume s0 "
	"cmp r0, oDepth, p0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_654_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_654_Desc = "ps_3_0 : cmp source reg combination oDepth, p0, vFace is NOT allowed";
string PS_654 = 
	"ps_3_0 "
	"cmp r0, oDepth, p0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_655_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_655_Desc = "ps_3_0 : cmp source reg combination oDepth, p0, oC0 is NOT allowed";
string PS_655 = 
	"ps_3_0 "
	"cmp r0, oDepth, p0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_656_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_656_Desc = "ps_3_0 : cmp source reg combination oDepth, p0, oDepth is NOT allowed";
string PS_656 = 
	"ps_3_0 "
	"cmp r0, oDepth, p0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_657_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_657_Desc = "ps_3_0 : cmp source reg combination oDepth, s0, b0 is NOT allowed";
string PS_657 = 
	"ps_3_0 "
	"defb b0, true "
	"dcl_volume s0 "
	"cmp r0, oDepth, s0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_658_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_658_Desc = "ps_3_0 : cmp source reg combination oDepth, s0, i0 is NOT allowed";
string PS_658 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"cmp r0, oDepth, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_659_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_659_Desc = "ps_3_0 : cmp source reg combination oDepth, s0, aL is NOT allowed";
string PS_659 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, oDepth, s0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_660_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_660_Desc = "ps_3_0 : cmp source reg combination oDepth, s0, p0 is NOT allowed";
string PS_660 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, oDepth, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_661_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_661_Desc = "ps_3_0 : cmp source reg combination oDepth, s0, s0 is NOT allowed";
string PS_661 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, oDepth, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_662_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_662_Desc = "ps_3_0 : cmp source reg combination oDepth, s0, vFace is NOT allowed";
string PS_662 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, oDepth, s0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_663_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_663_Desc = "ps_3_0 : cmp source reg combination oDepth, s0, oC0 is NOT allowed";
string PS_663 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, oDepth, s0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_664_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_664_Desc = "ps_3_0 : cmp source reg combination oDepth, s0, oDepth is NOT allowed";
string PS_664 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, oDepth, s0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_665_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_665_Desc = "ps_3_0 : cmp source reg combination oDepth, vFace, b0 is NOT allowed";
string PS_665 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, vFace, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_666_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_666_Desc = "ps_3_0 : cmp source reg combination oDepth, vFace, i0 is NOT allowed";
string PS_666 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, vFace, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_667_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_667_Desc = "ps_3_0 : cmp source reg combination oDepth, vFace, aL is NOT allowed";
string PS_667 = 
	"ps_3_0 "
	"cmp r0, oDepth, vFace, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_668_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_668_Desc = "ps_3_0 : cmp source reg combination oDepth, vFace, p0 is NOT allowed";
string PS_668 = 
	"ps_3_0 "
	"cmp r0, oDepth, vFace, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_669_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_669_Desc = "ps_3_0 : cmp source reg combination oDepth, vFace, s0 is NOT allowed";
string PS_669 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, oDepth, vFace, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_670_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_670_Desc = "ps_3_0 : cmp source reg combination oDepth, vFace, vFace is NOT allowed";
string PS_670 = 
	"ps_3_0 "
	"cmp r0, oDepth, vFace, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_671_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_671_Desc = "ps_3_0 : cmp source reg combination oDepth, vFace, oC0 is NOT allowed";
string PS_671 = 
	"ps_3_0 "
	"cmp r0, oDepth, vFace, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_672_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_672_Desc = "ps_3_0 : cmp source reg combination oDepth, vFace, oDepth is NOT allowed";
string PS_672 = 
	"ps_3_0 "
	"cmp r0, oDepth, vFace, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_673_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_673_Desc = "ps_3_0 : cmp source reg combination oDepth, oC0, b0 is NOT allowed";
string PS_673 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, oC0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_674_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_674_Desc = "ps_3_0 : cmp source reg combination oDepth, oC0, i0 is NOT allowed";
string PS_674 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, oC0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_675_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_675_Desc = "ps_3_0 : cmp source reg combination oDepth, oC0, aL is NOT allowed";
string PS_675 = 
	"ps_3_0 "
	"cmp r0, oDepth, oC0, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_676_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_676_Desc = "ps_3_0 : cmp source reg combination oDepth, oC0, p0 is NOT allowed";
string PS_676 = 
	"ps_3_0 "
	"cmp r0, oDepth, oC0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_677_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_677_Desc = "ps_3_0 : cmp source reg combination oDepth, oC0, s0 is NOT allowed";
string PS_677 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, oDepth, oC0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_678_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_678_Desc = "ps_3_0 : cmp source reg combination oDepth, oC0, vFace is NOT allowed";
string PS_678 = 
	"ps_3_0 "
	"cmp r0, oDepth, oC0, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_679_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_679_Desc = "ps_3_0 : cmp source reg combination oDepth, oC0, oC0 is NOT allowed";
string PS_679 = 
	"ps_3_0 "
	"cmp r0, oDepth, oC0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_680_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_680_Desc = "ps_3_0 : cmp source reg combination oDepth, oC0, oDepth is NOT allowed";
string PS_680 = 
	"ps_3_0 "
	"cmp r0, oDepth, oC0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_681_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_681_Desc = "ps_3_0 : cmp source reg combination oDepth, oDepth, b0 is NOT allowed";
string PS_681 = 
	"ps_3_0 "
	"defb b0, true "
	"cmp r0, oDepth, oDepth, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_682_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_682_Desc = "ps_3_0 : cmp source reg combination oDepth, oDepth, i0 is NOT allowed";
string PS_682 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"cmp r0, oDepth, oDepth, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_683_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_683_Desc = "ps_3_0 : cmp source reg combination oDepth, oDepth, aL is NOT allowed";
string PS_683 = 
	"ps_3_0 "
	"cmp r0, oDepth, oDepth, aL "
	"mov oC0, c0 ";

PS_CRITERIA PS_684_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_684_Desc = "ps_3_0 : cmp source reg combination oDepth, oDepth, p0 is NOT allowed";
string PS_684 = 
	"ps_3_0 "
	"cmp r0, oDepth, oDepth, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_685_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_685_Desc = "ps_3_0 : cmp source reg combination oDepth, oDepth, s0 is NOT allowed";
string PS_685 = 
	"ps_3_0 "
	"dcl_2d s0 "
	"cmp r0, oDepth, oDepth, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_686_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_686_Desc = "ps_3_0 : cmp source reg combination oDepth, oDepth, vFace is NOT allowed";
string PS_686 = 
	"ps_3_0 "
	"cmp r0, oDepth, oDepth, vFace "
	"mov oC0, c0 ";

PS_CRITERIA PS_687_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_687_Desc = "ps_3_0 : cmp source reg combination oDepth, oDepth, oC0 is NOT allowed";
string PS_687 = 
	"ps_3_0 "
	"cmp r0, oDepth, oDepth, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_688_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_688_Desc = "ps_3_0 : cmp source reg combination oDepth, oDepth, oDepth is NOT allowed";
string PS_688 = 
	"ps_3_0 "
	"cmp r0, oDepth, oDepth, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_689_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_689_Desc = "ps_3_0 : cmp dest r# is allowed";
string PS_689 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_690_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_690_Desc = "ps_3_0 : cmp _pp dest r# is allowed";
string PS_690 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp r0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_691_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_691_Desc = "ps_3_0 : cmp _sat dest r# is allowed";
string PS_691 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_sat r0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_692_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_692_Desc = "ps_3_0 : cmp _centroid dest r# is NOT allowed";
string PS_692 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid r0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_693_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_693_Desc = "ps_3_0 : cmp dest oC# is allowed";
string PS_693 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp oC0, r0, r1, r2 ";

PS_CRITERIA PS_694_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_694_Desc = "ps_3_0 : cmp _pp dest oC# is allowed";
string PS_694 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp oC0, r0, r1, r2 ";

PS_CRITERIA PS_695_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_695_Desc = "ps_3_0 : cmp _centroid dest oC# is NOT allowed";
string PS_695 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid oC0, r0, r1, r2 ";

PS_CRITERIA PS_696_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_696_Desc = "ps_3_0 : cmp dest oDepth is allowed";
string PS_696 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp oDepth, r0.x, r1.x, r2.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_697_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_697_Desc = "ps_3_0 : cmp _pp dest oDepth is allowed";
string PS_697 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp oDepth, r0.x, r1.x, r2.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_698_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_698_Desc = "ps_3_0 : cmp _centroid dest oDepth is NOT allowed";
string PS_698 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid oDepth, r0.x, r1.x, r2.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_699_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_699_Desc = "ps_3_0 : cmp dest v# is NOT allowed";
string PS_699 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp v0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_700_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_700_Desc = "ps_3_0 : cmp _pp dest v# is NOT allowed";
string PS_700 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp v0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_701_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_701_Desc = "ps_3_0 : cmp _centroid dest v# is NOT allowed";
string PS_701 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid v0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_702_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_702_Desc = "ps_3_0 : cmp _sat dest v# is NOT allowed";
string PS_702 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_sat v0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_703_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_703_Desc = "ps_3_0 : cmp dest c# is NOT allowed";
string PS_703 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp c1, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_704_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_704_Desc = "ps_3_0 : cmp _pp dest c# is NOT allowed";
string PS_704 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp c1, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_705_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_705_Desc = "ps_3_0 : cmp _centroid dest c# is NOT allowed";
string PS_705 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid c1, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_706_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_706_Desc = "ps_3_0 : cmp _sat dest c# is NOT allowed";
string PS_706 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_sat c1, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_707_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_707_Desc = "ps_3_0 : cmp dest b# is NOT allowed";
string PS_707 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp b0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_708_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_708_Desc = "ps_3_0 : cmp _pp dest b# is NOT allowed";
string PS_708 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp b0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_709_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_709_Desc = "ps_3_0 : cmp _centroid dest b# is NOT allowed";
string PS_709 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid b0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_710_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_710_Desc = "ps_3_0 : cmp _sat dest b# is NOT allowed";
string PS_710 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_sat b0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_711_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_711_Desc = "ps_3_0 : cmp dest i# is NOT allowed";
string PS_711 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp i0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_712_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_712_Desc = "ps_3_0 : cmp _pp dest i# is NOT allowed";
string PS_712 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp i0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_713_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_713_Desc = "ps_3_0 : cmp _centroid dest i# is NOT allowed";
string PS_713 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid i0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_714_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_714_Desc = "ps_3_0 : cmp _sat dest i# is NOT allowed";
string PS_714 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_sat i0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_715_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_715_Desc = "ps_3_0 : cmp dest aL is NOT allowed";
string PS_715 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp aL, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_716_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_716_Desc = "ps_3_0 : cmp _pp dest aL is NOT allowed";
string PS_716 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp aL, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_717_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_717_Desc = "ps_3_0 : cmp _centroid dest aL is NOT allowed";
string PS_717 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid aL, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_718_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_718_Desc = "ps_3_0 : cmp _sat dest aL is NOT allowed";
string PS_718 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_sat aL, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_719_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_719_Desc = "ps_3_0 : cmp dest p0 is NOT allowed";
string PS_719 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp p0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_720_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_720_Desc = "ps_3_0 : cmp _pp dest p0 is NOT allowed";
string PS_720 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp p0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_721_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_721_Desc = "ps_3_0 : cmp _centroid dest p0 is NOT allowed";
string PS_721 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid p0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_722_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_722_Desc = "ps_3_0 : cmp _sat dest p0 is NOT allowed";
string PS_722 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_sat p0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_723_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_723_Desc = "ps_3_0 : cmp dest s# is NOT allowed";
string PS_723 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp s0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_724_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_724_Desc = "ps_3_0 : cmp _pp dest s# is NOT allowed";
string PS_724 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp s0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_725_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_725_Desc = "ps_3_0 : cmp _centroid dest s# is NOT allowed";
string PS_725 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid s0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_726_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_726_Desc = "ps_3_0 : cmp _sat dest s# is NOT allowed";
string PS_726 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_sat s0, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_727_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_727_Desc = "ps_3_0 : cmp dest vFace is NOT allowed";
string PS_727 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp vFace, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_728_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_728_Desc = "ps_3_0 : cmp _pp dest vFace is NOT allowed";
string PS_728 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp vFace, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_729_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_729_Desc = "ps_3_0 : cmp _centroid dest vFace is NOT allowed";
string PS_729 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid vFace, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_730_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_730_Desc = "ps_3_0 : cmp _sat dest vFace is NOT allowed";
string PS_730 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_sat vFace, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_731_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_731_Desc = "ps_3_0 : cmp dest vPos is NOT allowed";
string PS_731 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp vPos, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_732_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_732_Desc = "ps_3_0 : cmp _pp dest vPos is NOT allowed";
string PS_732 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_pp vPos, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_733_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_733_Desc = "ps_3_0 : cmp _centroid dest vPos is NOT allowed";
string PS_733 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_centroid vPos, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_734_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_734_Desc = "ps_3_0 : cmp _sat dest vPos is NOT allowed";
string PS_734 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp_sat vPos, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_735_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_735_Desc = "ps_3_0 : cmp write mask .r is allowed";
string PS_735 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.r, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_736_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_736_Desc = "ps_3_0 : cmp write mask .g is allowed";
string PS_736 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.g, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_737_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_737_Desc = "ps_3_0 : cmp write mask .b is allowed";
string PS_737 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.b, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_738_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_738_Desc = "ps_3_0 : cmp write mask .a is allowed";
string PS_738 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.a, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_739_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_739_Desc = "ps_3_0 : cmp write mask .rg is allowed";
string PS_739 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.rg, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_740_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_740_Desc = "ps_3_0 : cmp write mask .gb is allowed";
string PS_740 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.gb, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_741_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_741_Desc = "ps_3_0 : cmp write mask .ba is allowed";
string PS_741 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.ba, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_742_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_742_Desc = "ps_3_0 : cmp write mask .rb is allowed";
string PS_742 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.rb, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_743_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_743_Desc = "ps_3_0 : cmp write mask .ra is allowed";
string PS_743 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.ra, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_744_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_744_Desc = "ps_3_0 : cmp write mask .ga is allowed";
string PS_744 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.ga, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_745_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_745_Desc = "ps_3_0 : cmp write mask .rgb is allowed";
string PS_745 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.rgb, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_746_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_746_Desc = "ps_3_0 : cmp write mask .gba is allowed";
string PS_746 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.gba, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_747_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_747_Desc = "ps_3_0 : cmp write mask .rba is allowed";
string PS_747 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.rba, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_748_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_748_Desc = "ps_3_0 : cmp write mask .rgba is allowed";
string PS_748 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.rgba, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_749_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_749_Desc = "ps_3_0 : cmp write mask .yx is NOT allowed";
string PS_749 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.yx, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_750_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_750_Desc = "ps_3_0 : cmp write mask .zx is NOT allowed";
string PS_750 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.zx, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_751_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_751_Desc = "ps_3_0 : cmp write mask .zy is NOT allowed";
string PS_751 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.zy, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_752_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_752_Desc = "ps_3_0 : cmp write mask .wx is NOT allowed";
string PS_752 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.wx, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_753_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_753_Desc = "ps_3_0 : cmp write mask .wz is NOT allowed";
string PS_753 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.wz, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_754_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_754_Desc = "ps_3_0 : cmp write mask .wy is NOT allowed";
string PS_754 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.wy, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_755_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_755_Desc = "ps_3_0 : cmp write mask .zyx is NOT allowed";
string PS_755 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.zyx, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_756_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_756_Desc = "ps_3_0 : cmp write mask .wzy is NOT allowed";
string PS_756 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.wzy, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_757_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_757_Desc = "ps_3_0 : cmp write mask .wzx is NOT allowed";
string PS_757 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.wzx, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_758_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_758_Desc = "ps_3_0 : cmp write mask .wyx is NOT allowed";
string PS_758 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.wyx, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_759_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_759_Desc = "ps_3_0 : cmp write mask .yxw is NOT allowed";
string PS_759 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.yxw, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_760_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_760_Desc = "ps_3_0 : cmp write mask .wzyx is NOT allowed";
string PS_760 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.wzyx, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_761_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_761_Desc = "ps_3_0 : cmp write mask .zxwy is NOT allowed";
string PS_761 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"cmp r0.zxwy, r0, r1, r2 "
	"mov oC0, c0 ";

PS_CRITERIA PS_762_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_762_Desc = "ps_3_0 : cmp is allowed in a 1 level if b0 block";
string PS_762 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	cmp r0, r0, r1, r2 "
	"endif ";

PS_CRITERIA PS_763_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_763_Desc = "ps_3_0 : cmp is allowed in a 8 level if b0 block";
string PS_763 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								cmp r0, r0, r1, r2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_764_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_764_Desc = "ps_3_0 : cmp is allowed in a 16 level if b0 block";
string PS_764 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_765_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_765_Desc = "ps_3_0 : cmp is allowed in a 24 level if b0 block";
string PS_765 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_766_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_766_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level if b0 block";
string PS_766 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_767_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_767_Desc = "ps_3_0 : cmp is allowed in a 1 level if b0 nop else block";
string PS_767 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	cmp r0, r0, r1, r2 "
	"endif ";

PS_CRITERIA PS_768_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_768_Desc = "ps_3_0 : cmp is allowed in a 8 level if b0 nop else block";
string PS_768 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								cmp r0, r0, r1, r2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_769_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_769_Desc = "ps_3_0 : cmp is allowed in a 16 level if b0 nop else block";
string PS_769 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_770_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_770_Desc = "ps_3_0 : cmp is allowed in a 24 level if b0 nop else block";
string PS_770 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_771_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_771_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level if b0 nop else block";
string PS_771 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_772_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_772_Desc = "ps_3_0 : cmp is allowed in a 1 level loop aL, i0 block";
string PS_772 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	cmp r0, r0, r1, r2 "
	"endloop ";

PS_CRITERIA PS_773_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_773_Desc = "ps_3_0 : cmp is allowed in a 2 level loop aL, i0 block";
string PS_773 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_774_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_774_Desc = "ps_3_0 : cmp is allowed in a 3 level loop aL, i0 block";
string PS_774 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			cmp r0, r0, r1, r2 "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_775_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_775_Desc = "ps_3_0 : cmp is allowed in a 4 level loop aL, i0 block";
string PS_775 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				cmp r0, r0, r1, r2 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_776_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_776_Desc = "ps_3_0 : cmp is NOT allowed in a 5 level loop aL, i0 block";
string PS_776 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					cmp r0, r0, r1, r2 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_777_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_777_Desc = "ps_3_0 : cmp is allowed in a 1 level rep i0 block";
string PS_777 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	cmp r0, r0, r1, r2 "
	"endrep ";

PS_CRITERIA PS_778_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_778_Desc = "ps_3_0 : cmp is allowed in a 2 level rep i0 block";
string PS_778 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_779_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_779_Desc = "ps_3_0 : cmp is allowed in a 3 level rep i0 block";
string PS_779 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			cmp r0, r0, r1, r2 "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_780_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_780_Desc = "ps_3_0 : cmp is allowed in a 4 level rep i0 block";
string PS_780 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				cmp r0, r0, r1, r2 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_781_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_781_Desc = "ps_3_0 : cmp is NOT allowed in a 5 level rep i0 block";
string PS_781 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					cmp r0, r0, r1, r2 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_782_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_782_Desc = "ps_3_0 : cmp is allowed in a 1 level if_ge c0.x, c0.y block";
string PS_782 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	cmp r0, r0, r1, r2 "
	"endif ";

PS_CRITERIA PS_783_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_783_Desc = "ps_3_0 : cmp is allowed in a 8 level if_ge c0.x, c0.y block";
string PS_783 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		if_ge c0.x, c0.y "
	"			if_ge c0.x, c0.y "
	"				if_ge c0.x, c0.y "
	"					if_ge c0.x, c0.y "
	"						if_ge c0.x, c0.y "
	"							if_ge c0.x, c0.y "
	"								cmp r0, r0, r1, r2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_784_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_784_Desc = "ps_3_0 : cmp is allowed in a 16 level if_ge c0.x, c0.y block";
string PS_784 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_785_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_785_Desc = "ps_3_0 : cmp is allowed in a 24 level if_ge c0.x, c0.y block";
string PS_785 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_786_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_786_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level if_ge c0.x, c0.y block";
string PS_786 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_787_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_787_Desc = "ps_3_0 : cmp is allowed in a 1 level if_ge c0.x, c0.y nop else block";
string PS_787 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	cmp r0, r0, r1, r2 "
	"endif ";

PS_CRITERIA PS_788_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_788_Desc = "ps_3_0 : cmp is allowed in a 8 level if_ge c0.x, c0.y nop else block";
string PS_788 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		if_ge c0.x, c0.y nop else "
	"			if_ge c0.x, c0.y nop else "
	"				if_ge c0.x, c0.y nop else "
	"					if_ge c0.x, c0.y nop else "
	"						if_ge c0.x, c0.y nop else "
	"							if_ge c0.x, c0.y nop else "
	"								cmp r0, r0, r1, r2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_789_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_789_Desc = "ps_3_0 : cmp is allowed in a 16 level if_ge c0.x, c0.y nop else block";
string PS_789 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_790_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_790_Desc = "ps_3_0 : cmp is allowed in a 24 level if_ge c0.x, c0.y nop else block";
string PS_790 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_791_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_791_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level if_ge c0.x, c0.y nop else block";
string PS_791 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_792_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_792_Desc = "ps_3_0 : cmp is allowed in a 1 level if p0.x block";
string PS_792 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	cmp r0, r0, r1, r2 "
	"endif ";

PS_CRITERIA PS_793_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_793_Desc = "ps_3_0 : cmp is allowed in a 8 level if p0.x block";
string PS_793 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								cmp r0, r0, r1, r2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_794_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_794_Desc = "ps_3_0 : cmp is allowed in a 16 level if p0.x block";
string PS_794 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_795_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_795_Desc = "ps_3_0 : cmp is allowed in a 24 level if p0.x block";
string PS_795 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_796_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_796_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level if p0.x block";
string PS_796 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_797_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_797_Desc = "ps_3_0 : cmp is allowed in a 1 level if p0.x nop else block";
string PS_797 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	cmp r0, r0, r1, r2 "
	"endif ";

PS_CRITERIA PS_798_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_798_Desc = "ps_3_0 : cmp is allowed in a 8 level if p0.x nop else block";
string PS_798 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								cmp r0, r0, r1, r2 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

PS_CRITERIA PS_799_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_799_Desc = "ps_3_0 : cmp is allowed in a 16 level if p0.x nop else block";
string PS_799 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_800_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_800_Desc = "ps_3_0 : cmp is allowed in a 24 level if p0.x nop else block";
string PS_800 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_801_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_801_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level if p0.x nop else block";
string PS_801 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_802_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_802_Desc = "ps_3_0 : cmp is allowed in a 1 level loop aL, i0 break block";
string PS_802 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	cmp r0, r0, r1, r2 "
	"endloop ";

PS_CRITERIA PS_803_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_803_Desc = "ps_3_0 : cmp is NOT allowed in a 8 level loop aL, i0 break block";
string PS_803 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								cmp r0, r0, r1, r2 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_804_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_804_Desc = "ps_3_0 : cmp is NOT allowed in a 16 level loop aL, i0 break block";
string PS_804 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_805_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_805_Desc = "ps_3_0 : cmp is NOT allowed in a 24 level loop aL, i0 break block";
string PS_805 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_806_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_806_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level loop aL, i0 break block";
string PS_806 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_807_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_807_Desc = "ps_3_0 : cmp is allowed in a 1 level rep i0 break block";
string PS_807 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	cmp r0, r0, r1, r2 "
	"endrep ";

PS_CRITERIA PS_808_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_808_Desc = "ps_3_0 : cmp is NOT allowed in a 8 level rep i0 break block";
string PS_808 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								cmp r0, r0, r1, r2 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_809_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_809_Desc = "ps_3_0 : cmp is NOT allowed in a 16 level rep i0 break block";
string PS_809 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_810_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_810_Desc = "ps_3_0 : cmp is NOT allowed in a 24 level rep i0 break block";
string PS_810 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_811_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_811_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level rep i0 break block";
string PS_811 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_812_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_812_Desc = "ps_3_0 : cmp is allowed in a 1 level loop aL, i0 breakp p0.x block";
string PS_812 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	cmp r0, r0, r1, r2 "
	"endloop ";

PS_CRITERIA PS_813_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_813_Desc = "ps_3_0 : cmp is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string PS_813 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								cmp r0, r0, r1, r2 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_814_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_814_Desc = "ps_3_0 : cmp is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string PS_814 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_815_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_815_Desc = "ps_3_0 : cmp is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string PS_815 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_816_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_816_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string PS_816 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_817_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_817_Desc = "ps_3_0 : cmp is allowed in a 1 level rep i0 breakp p0.x block";
string PS_817 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	cmp r0, r0, r1, r2 "
	"endrep ";

PS_CRITERIA PS_818_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_818_Desc = "ps_3_0 : cmp is NOT allowed in a 8 level rep i0 breakp p0.x block";
string PS_818 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								cmp r0, r0, r1, r2 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_819_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_819_Desc = "ps_3_0 : cmp is NOT allowed in a 16 level rep i0 breakp p0.x block";
string PS_819 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_820_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_820_Desc = "ps_3_0 : cmp is NOT allowed in a 24 level rep i0 breakp p0.x block";
string PS_820 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_821_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_821_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level rep i0 breakp p0.x block";
string PS_821 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_822_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_822_Desc = "ps_3_0 : cmp is allowed in a 1 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_822 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	cmp r0, r0, r1, r2 "
	"endloop ";

PS_CRITERIA PS_823_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_823_Desc = "ps_3_0 : cmp is NOT allowed in a 8 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_823 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		loop aL, i0 break_ge c0.x, c0.y "
	"			loop aL, i0 break_ge c0.x, c0.y "
	"				loop aL, i0 break_ge c0.x, c0.y "
	"					loop aL, i0 break_ge c0.x, c0.y "
	"						loop aL, i0 break_ge c0.x, c0.y "
	"							loop aL, i0 break_ge c0.x, c0.y "
	"								cmp r0, r0, r1, r2 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_824_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_824_Desc = "ps_3_0 : cmp is NOT allowed in a 16 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_824 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_825_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_825_Desc = "ps_3_0 : cmp is NOT allowed in a 24 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_825 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_826_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_826_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level loop aL, i0 break_ge c0.x, c0.y block";
string PS_826 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_827_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_827_Desc = "ps_3_0 : cmp is allowed in a 1 level rep i0 break_ge c0.x, c0.y block";
string PS_827 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	cmp r0, r0, r1, r2 "
	"endrep ";

PS_CRITERIA PS_828_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_828_Desc = "ps_3_0 : cmp is NOT allowed in a 8 level rep i0 break_ge c0.x, c0.y block";
string PS_828 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		rep i0 break_ge c0.x, c0.y "
	"			rep i0 break_ge c0.x, c0.y "
	"				rep i0 break_ge c0.x, c0.y "
	"					rep i0 break_ge c0.x, c0.y "
	"						rep i0 break_ge c0.x, c0.y "
	"							rep i0 break_ge c0.x, c0.y "
	"								cmp r0, r0, r1, r2 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_829_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_829_Desc = "ps_3_0 : cmp is NOT allowed in a 16 level rep i0 break_ge c0.x, c0.y block";
string PS_829 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_830_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_830_Desc = "ps_3_0 : cmp is NOT allowed in a 24 level rep i0 break_ge c0.x, c0.y block";
string PS_830 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																								cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_831_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_831_Desc = "ps_3_0 : cmp is NOT allowed in a 25 level rep i0 break_ge c0.x, c0.y block";
string PS_831 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
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
	"																									cmp r0, r0, r1, r2 "
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

PS_CRITERIA PS_832_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_832_Desc = "ps_3_0 : cmp is allowed in a if b0 and if b0 block";
string PS_832 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_833_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_833_Desc = "ps_3_0 : cmp is allowed in a if b0 and if b0 nop else block";
string PS_833 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_834_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_834_Desc = "ps_3_0 : cmp is allowed in a if b0 and loop aL, i0 block";
string PS_834 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_835_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_835_Desc = "ps_3_0 : cmp is allowed in a if b0 and rep i0 block";
string PS_835 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_836_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_836_Desc = "ps_3_0 : cmp is allowed in a if b0 and if_ge c0.x, c0.y block";
string PS_836 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_837_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_837_Desc = "ps_3_0 : cmp is allowed in a if b0 and if_ge c0.x, c0.y nop else block";
string PS_837 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_838_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_838_Desc = "ps_3_0 : cmp is allowed in a if b0 and if p0.x block";
string PS_838 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_839_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_839_Desc = "ps_3_0 : cmp is allowed in a if b0 and if p0.x nop else block";
string PS_839 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_840_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_840_Desc = "ps_3_0 : cmp is allowed in a if b0 and loop aL, i0 break block";
string PS_840 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_841_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_841_Desc = "ps_3_0 : cmp is allowed in a if b0 and rep i0 break block";
string PS_841 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_842_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_842_Desc = "ps_3_0 : cmp is allowed in a if b0 and loop aL, i0 breakp p0.x block";
string PS_842 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_843_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_843_Desc = "ps_3_0 : cmp is allowed in a if b0 and rep i0 breakp p0.x block";
string PS_843 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_844_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_844_Desc = "ps_3_0 : cmp is allowed in a if b0 and loop aL, i0 break_ge c0.x, c0.y block";
string PS_844 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_845_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_845_Desc = "ps_3_0 : cmp is allowed in a if b0 and rep i0 break_ge c0.x, c0.y block";
string PS_845 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_846_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_846_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and if b0 block";
string PS_846 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_847_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_847_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and if b0 nop else block";
string PS_847 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_848_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_848_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and loop aL, i0 block";
string PS_848 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_849_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_849_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and rep i0 block";
string PS_849 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_850_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_850_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and if_ge c0.x, c0.y block";
string PS_850 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_851_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_851_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and if_ge c0.x, c0.y nop else block";
string PS_851 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_852_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_852_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and if p0.x block";
string PS_852 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_853_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_853_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and if p0.x nop else block";
string PS_853 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_854_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_854_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and loop aL, i0 break block";
string PS_854 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_855_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_855_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and rep i0 break block";
string PS_855 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_856_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_856_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string PS_856 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_857_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_857_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string PS_857 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_858_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_858_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and loop aL, i0 break_ge c0.x, c0.y block";
string PS_858 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_859_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_859_Desc = "ps_3_0 : cmp is allowed in a if b0 nop else and rep i0 break_ge c0.x, c0.y block";
string PS_859 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if b0 nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_860_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_860_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and if b0 block";
string PS_860 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_861_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_861_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and if b0 nop else block";
string PS_861 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_862_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_862_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and loop aL, i0 block";
string PS_862 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_863_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_863_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and rep i0 block";
string PS_863 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_864_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_864_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and if_ge c0.x, c0.y block";
string PS_864 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_865_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_865_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and if_ge c0.x, c0.y nop else block";
string PS_865 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_866_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_866_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and if p0.x block";
string PS_866 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_867_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_867_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and if p0.x nop else block";
string PS_867 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_868_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_868_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and loop aL, i0 break block";
string PS_868 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_869_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_869_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and rep i0 break block";
string PS_869 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_870_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_870_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string PS_870 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_871_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_871_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string PS_871 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_872_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_872_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and loop aL, i0 break_ge c0.x, c0.y block";
string PS_872 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_873_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_873_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 and rep i0 break_ge c0.x, c0.y block";
string PS_873 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_874_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_874_Desc = "ps_3_0 : cmp is allowed in a rep i0 and if b0 block";
string PS_874 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_875_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_875_Desc = "ps_3_0 : cmp is allowed in a rep i0 and if b0 nop else block";
string PS_875 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_876_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_876_Desc = "ps_3_0 : cmp is allowed in a rep i0 and loop aL, i0 block";
string PS_876 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_877_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_877_Desc = "ps_3_0 : cmp is allowed in a rep i0 and rep i0 block";
string PS_877 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_878_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_878_Desc = "ps_3_0 : cmp is allowed in a rep i0 and if_ge c0.x, c0.y block";
string PS_878 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_879_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_879_Desc = "ps_3_0 : cmp is allowed in a rep i0 and if_ge c0.x, c0.y nop else block";
string PS_879 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_880_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_880_Desc = "ps_3_0 : cmp is allowed in a rep i0 and if p0.x block";
string PS_880 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_881_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_881_Desc = "ps_3_0 : cmp is allowed in a rep i0 and if p0.x nop else block";
string PS_881 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_882_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_882_Desc = "ps_3_0 : cmp is allowed in a rep i0 and loop aL, i0 break block";
string PS_882 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_883_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_883_Desc = "ps_3_0 : cmp is allowed in a rep i0 and rep i0 break block";
string PS_883 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_884_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_884_Desc = "ps_3_0 : cmp is allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string PS_884 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_885_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_885_Desc = "ps_3_0 : cmp is allowed in a rep i0 and rep i0 breakp p0.x block";
string PS_885 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_886_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_886_Desc = "ps_3_0 : cmp is allowed in a rep i0 and loop aL, i0 break_ge c0.x, c0.y block";
string PS_886 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_887_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_887_Desc = "ps_3_0 : cmp is allowed in a rep i0 and rep i0 break_ge c0.x, c0.y block";
string PS_887 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_888_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_888_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and if b0 block";
string PS_888 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_889_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_889_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and if b0 nop else block";
string PS_889 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_890_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_890_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and loop aL, i0 block";
string PS_890 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_891_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_891_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and rep i0 block";
string PS_891 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_892_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_892_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and if_ge c0.x, c0.y block";
string PS_892 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_893_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_893_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string PS_893 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_894_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_894_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and if p0.x block";
string PS_894 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_895_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_895_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and if p0.x nop else block";
string PS_895 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_896_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_896_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and loop aL, i0 break block";
string PS_896 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_897_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_897_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and rep i0 break block";
string PS_897 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_898_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_898_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_898 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_899_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_899_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and rep i0 breakp p0.x block";
string PS_899 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_900_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_900_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string PS_900 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_901_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_901_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string PS_901 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_902_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_902_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and if b0 block";
string PS_902 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_903_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_903_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and if b0 nop else block";
string PS_903 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_904_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_904_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 block";
string PS_904 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_905_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_905_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and rep i0 block";
string PS_905 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_906_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_906_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and if_ge c0.x, c0.y block";
string PS_906 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_907_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_907_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and if_ge c0.x, c0.y nop else block";
string PS_907 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_908_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_908_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and if p0.x block";
string PS_908 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_909_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_909_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and if p0.x nop else block";
string PS_909 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_910_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_910_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 break block";
string PS_910 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_911_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_911_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and rep i0 break block";
string PS_911 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_912_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_912_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string PS_912 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_913_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_913_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and rep i0 breakp p0.x block";
string PS_913 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_914_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_914_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 break_ge c0.x, c0.y block";
string PS_914 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_915_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_915_Desc = "ps_3_0 : cmp is allowed in a if_ge c0.x, c0.y nop else and rep i0 break_ge c0.x, c0.y block";
string PS_915 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_916_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_916_Desc = "ps_3_0 : cmp is allowed in a if p0.x and if b0 block";
string PS_916 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_917_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_917_Desc = "ps_3_0 : cmp is allowed in a if p0.x and if b0 nop else block";
string PS_917 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_918_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_918_Desc = "ps_3_0 : cmp is allowed in a if p0.x and loop aL, i0 block";
string PS_918 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_919_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_919_Desc = "ps_3_0 : cmp is allowed in a if p0.x and rep i0 block";
string PS_919 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_920_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_920_Desc = "ps_3_0 : cmp is allowed in a if p0.x and if_ge c0.x, c0.y block";
string PS_920 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_921_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_921_Desc = "ps_3_0 : cmp is allowed in a if p0.x and if_ge c0.x, c0.y nop else block";
string PS_921 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_922_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_922_Desc = "ps_3_0 : cmp is allowed in a if p0.x and if p0.x block";
string PS_922 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_923_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_923_Desc = "ps_3_0 : cmp is allowed in a if p0.x and if p0.x nop else block";
string PS_923 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_924_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_924_Desc = "ps_3_0 : cmp is allowed in a if p0.x and loop aL, i0 break block";
string PS_924 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_925_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_925_Desc = "ps_3_0 : cmp is allowed in a if p0.x and rep i0 break block";
string PS_925 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_926_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_926_Desc = "ps_3_0 : cmp is allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string PS_926 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_927_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_927_Desc = "ps_3_0 : cmp is allowed in a if p0.x and rep i0 breakp p0.x block";
string PS_927 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_928_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_928_Desc = "ps_3_0 : cmp is allowed in a if p0.x and loop aL, i0 break_ge c0.x, c0.y block";
string PS_928 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_929_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_929_Desc = "ps_3_0 : cmp is allowed in a if p0.x and rep i0 break_ge c0.x, c0.y block";
string PS_929 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_930_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_930_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and if b0 block";
string PS_930 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_931_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_931_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and if b0 nop else block";
string PS_931 = 
	"ps_3_0 "
	"defb b0, true "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_932_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_932_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and loop aL, i0 block";
string PS_932 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_933_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_933_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and rep i0 block";
string PS_933 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_934_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_934_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and if_ge c0.x, c0.y block";
string PS_934 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_935_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_935_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and if_ge c0.x, c0.y nop else block";
string PS_935 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_936_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_936_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and if p0.x block";
string PS_936 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_937_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_937_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and if p0.x nop else block";
string PS_937 = 
	"ps_3_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endif ";

PS_CRITERIA PS_938_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_938_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and loop aL, i0 break block";
string PS_938 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_939_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_939_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and rep i0 break block";
string PS_939 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_940_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_940_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string PS_940 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_941_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_941_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string PS_941 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_942_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_942_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and loop aL, i0 break_ge c0.x, c0.y block";
string PS_942 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_943_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_943_Desc = "ps_3_0 : cmp is allowed in a if p0.x nop else and rep i0 break_ge c0.x, c0.y block";
string PS_943 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"if p0.x nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_944_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_944_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and if b0 block";
string PS_944 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_945_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_945_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and if b0 nop else block";
string PS_945 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_946_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_946_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_946 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_947_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_947_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and rep i0 block";
string PS_947 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_948_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_948_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and if_ge c0.x, c0.y block";
string PS_948 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_949_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_949_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and if_ge c0.x, c0.y nop else block";
string PS_949 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_950_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_950_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and if p0.x block";
string PS_950 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_951_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_951_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_951 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_952_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_952_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_952 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_953_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_953_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and rep i0 break block";
string PS_953 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_954_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_954_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_954 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_955_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_955_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_955 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_956_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_956_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and loop aL, i0 break_ge c0.x, c0.y block";
string PS_956 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_957_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_957_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break and rep i0 break_ge c0.x, c0.y block";
string PS_957 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_958_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_958_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and if b0 block";
string PS_958 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_959_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_959_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and if b0 nop else block";
string PS_959 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_960_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_960_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and loop aL, i0 block";
string PS_960 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_961_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_961_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and rep i0 block";
string PS_961 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_962_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_962_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and if_ge c0.x, c0.y block";
string PS_962 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_963_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_963_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and if_ge c0.x, c0.y nop else block";
string PS_963 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_964_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_964_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and if p0.x block";
string PS_964 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_965_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_965_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and if p0.x nop else block";
string PS_965 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_966_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_966_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and loop aL, i0 break block";
string PS_966 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_967_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_967_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and rep i0 break block";
string PS_967 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_968_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_968_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_968 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_969_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_969_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_969 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_970_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_970_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and loop aL, i0 break_ge c0.x, c0.y block";
string PS_970 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_971_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_971_Desc = "ps_3_0 : cmp is allowed in a rep i0 break and rep i0 break_ge c0.x, c0.y block";
string PS_971 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_972_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_972_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_972 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_973_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_973_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_973 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_974_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_974_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_974 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_975_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_975_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_975 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_976_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_976_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and if_ge c0.x, c0.y block";
string PS_976 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_977_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_977_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and if_ge c0.x, c0.y nop else block";
string PS_977 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_978_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_978_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_978 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_979_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_979_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_979 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_980_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_980_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_980 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_981_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_981_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_981 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_982_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_982_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_982 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_983_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_983_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_983 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_984_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_984_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_ge c0.x, c0.y block";
string PS_984 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_985_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_985_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 breakp p0.x and rep i0 break_ge c0.x, c0.y block";
string PS_985 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_986_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_986_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_986 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_987_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_987_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_987 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_988_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_988_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_988 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_989_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_989_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_989 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_990_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_990_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and if_ge c0.x, c0.y block";
string PS_990 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_991_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_991_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and if_ge c0.x, c0.y nop else block";
string PS_991 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_992_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_992_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_992 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_993_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_993_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_993 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_994_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_994_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_994 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_995_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_995_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_995 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_996_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_996_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_996 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_997_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_997_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_997 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_998_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_998_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and loop aL, i0 break_ge c0.x, c0.y block";
string PS_998 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_999_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_999_Desc = "ps_3_0 : cmp is allowed in a rep i0 breakp p0.x and rep i0 break_ge c0.x, c0.y block";
string PS_999 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_1000_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1000_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and if b0 block";
string PS_1000 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_1001_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1001_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and if b0 nop else block";
string PS_1001 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_1002_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1002_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 block";
string PS_1002 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_1003_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1003_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 block";
string PS_1003 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_1004_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1004_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y block";
string PS_1004 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_1005_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1005_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string PS_1005 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_1006_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1006_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and if p0.x block";
string PS_1006 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_1007_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1007_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and if p0.x nop else block";
string PS_1007 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_1008_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1008_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 break block";
string PS_1008 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_1009_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1009_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 break block";
string PS_1009 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_1010_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1010_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_1010 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_1011_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1011_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 breakp p0.x block";
string PS_1011 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_1012_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1012_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string PS_1012 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_1013_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1013_Desc = "ps_3_0 : cmp is allowed in a loop aL, i0 break_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string PS_1013 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"loop aL, i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_1014_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1014_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and if b0 block";
string PS_1014 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if b0 "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_1015_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1015_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and if b0 nop else block";
string PS_1015 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if b0 nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_1016_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1016_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 block";
string PS_1016 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_1017_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1017_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 block";
string PS_1017 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_1018_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1018_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y block";
string PS_1018 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_1019_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1019_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string PS_1019 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_1020_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1020_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and if p0.x block";
string PS_1020 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_1021_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1021_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and if p0.x nop else block";
string PS_1021 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		cmp r0, r0, r1, r2 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_1022_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1022_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 break block";
string PS_1022 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_1023_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1023_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 break block";
string PS_1023 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_1024_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1024_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_1024 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_1025_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1025_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 breakp p0.x block";
string PS_1025 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_1026_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1026_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string PS_1026 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_1027_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_1027_Desc = "ps_3_0 : cmp is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string PS_1027 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		cmp r0, r0, r1, r2 "
	"	endrep "
	"endrep ";


