#include "Test_Include.fx"

int Test_Count = 95;

string TestInfo
<
	string TestType = "PS";
>
= "ps_1_2_inst_texm3x3spec";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0 is allowed";
string PS_001 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_1_2 : texm3x3spec source reg combination t0_bx2, c0 is allowed";
string PS_002 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0_bx2, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_003_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_1_2 : texm3x3spec source reg combination v0, v0 is NOT allowed";
string PS_003 = 
	"ps_1_2 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, v0, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_004_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_1_2 : texm3x3spec source reg combination v0, r0 is NOT allowed";
string PS_004 = 
	"ps_1_2 "
	"mov r0, c0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, v0, r0 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_1_2 : texm3x3spec source reg combination v0, t0 is NOT allowed";
string PS_005 = 
	"ps_1_2 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_1_2 : texm3x3spec source reg combination r0, v0 is NOT allowed";
string PS_006 = 
	"ps_1_2 "
	"mov r0, c0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, r0, v0 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_1_2 : texm3x3spec source reg combination r0, r0 is NOT allowed";
string PS_007 = 
	"ps_1_2 "
	"mov r0, c0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, r0, r0 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_1_2 : texm3x3spec source reg combination r0, t0 is NOT allowed";
string PS_008 = 
	"ps_1_2 "
	"texcoord t0 "
	"mov r0, c0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, r0, t0 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_1_2 : texm3x3spec source reg combination c0, v0 is NOT allowed";
string PS_009 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, c0, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_1_2 : texm3x3spec source reg combination c0, r0 is NOT allowed";
string PS_010 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, c0, r0 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_1_2 : texm3x3spec source reg combination c0, t0 is NOT allowed";
string PS_011 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, c0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_012_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_1_2 : texm3x3spec source reg combination t0.r, c0 is NOT allowed";
string PS_012 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.r, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_1_2 : texm3x3spec source reg combination t0.g, c0 is NOT allowed";
string PS_013 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.g, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_014_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_1_2 : texm3x3spec source reg combination t0.b, c0 is NOT allowed";
string PS_014 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.b, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_015_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_1_2 : texm3x3spec source reg combination t0.a, c0 is NOT allowed";
string PS_015 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.a, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_1_2 : texm3x3spec source reg combination t0.gbra, c0 is NOT allowed";
string PS_016 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.gbra, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_017_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_1_2 : texm3x3spec source reg combination t0.brga, c0 is NOT allowed";
string PS_017 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.brga, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_018_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_1_2 : texm3x3spec source reg combination t0.abgr, c0 is NOT allowed";
string PS_018 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.abgr, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_1_2 : texm3x3spec source reg combination t0.agrb, c0 is NOT allowed";
string PS_019 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.agrb, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_020_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_1_2 : texm3x3spec source reg combination t0.rbga, c0 is NOT allowed";
string PS_020 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.rbga, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_021_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_1_2 : texm3x3spec source reg combination t0.rgab, c0 is NOT allowed";
string PS_021 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.rgab, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_022_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_1_2 : texm3x3spec source reg combination t0.bgr, c0 is NOT allowed";
string PS_022 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.bgr, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_1_2 : texm3x3spec source reg combination t0.rbg, c0 is NOT allowed";
string PS_023 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.rbg, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_1_2 : texm3x3spec source reg combination t0.gar, c0 is NOT allowed";
string PS_024 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.gar, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_025_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_1_2 : texm3x3spec source reg combination t0.gr, c0 is NOT allowed";
string PS_025 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.gr, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_1_2 : texm3x3spec source reg combination t0.ab, c0 is NOT allowed";
string PS_026 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.ab, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_1_2 : texm3x3spec source reg combination t0.bg, c0 is NOT allowed";
string PS_027 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0.bg, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0_bias is NOT allowed";
string PS_028 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0_bias "
	"mov r0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_1_2 : texm3x3spec source reg combination t0, 1 - c0 is NOT allowed";
string PS_029 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, 1 - c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_1_2 : texm3x3spec source reg combination t0, -c0 is NOT allowed";
string PS_030 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, -c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0_bx2 is NOT allowed";
string PS_031 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0_bx2 "
	"mov r0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_1_2 : texm3x3spec source reg combination t0, -c0_bx2 is NOT allowed";
string PS_032 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, -c0_bx2 "
	"mov r0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.r is NOT allowed";
string PS_033 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.r "
	"mov r0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.g is NOT allowed";
string PS_034 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.g "
	"mov r0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.b is NOT allowed";
string PS_035 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.b "
	"mov r0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.a is NOT allowed";
string PS_036 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.a "
	"mov r0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.gbra is NOT allowed";
string PS_037 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.gbra "
	"mov r0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.brga is NOT allowed";
string PS_038 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.brga "
	"mov r0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.abgr is NOT allowed";
string PS_039 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.abgr "
	"mov r0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.agrb is NOT allowed";
string PS_040 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.agrb "
	"mov r0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.rbga is NOT allowed";
string PS_041 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.rbga "
	"mov r0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.rgab is NOT allowed";
string PS_042 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.rgab "
	"mov r0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.bgr is NOT allowed";
string PS_043 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.bgr "
	"mov r0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.rbg is NOT allowed";
string PS_044 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.rbg "
	"mov r0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.gar is NOT allowed";
string PS_045 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.gar "
	"mov r0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.gr is NOT allowed";
string PS_046 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.gr "
	"mov r0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.ab is NOT allowed";
string PS_047 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.ab "
	"mov r0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_1_2 : texm3x3spec source reg combination t0, c0.bg is NOT allowed";
string PS_048 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0.bg "
	"mov r0, c0 ";

PS_CRITERIA PS_049_Criteria = { true, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_1_2 : texm3x3spec dest t# is allowed";
string PS_049 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_1_2 : texm3x3spec _x2 dest t# is NOT allowed";
string PS_050 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_x2 t3, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_1_2 : texm3x3spec _x4 dest t# is NOT allowed";
string PS_051 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_x4 t3, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_1_2 : texm3x3spec _d2 dest t# is NOT allowed";
string PS_052 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_d2 t3, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_1_2 : texm3x3spec _sat dest t# is NOT allowed";
string PS_053 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_sat t3, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_054_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_1_2 : texm3x3spec dest v# is NOT allowed";
string PS_054 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec v0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_1_2 : texm3x3spec _x2 dest v# is NOT allowed";
string PS_055 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_x2 v0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_056_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_1_2 : texm3x3spec _x4 dest v# is NOT allowed";
string PS_056 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_x4 v0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_1_2 : texm3x3spec _d2 dest v# is NOT allowed";
string PS_057 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_d2 v0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_058_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_1_2 : texm3x3spec _sat dest v# is NOT allowed";
string PS_058 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_sat v0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_1_2 : texm3x3spec dest r# is NOT allowed";
string PS_059 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec r0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_060_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_1_2 : texm3x3spec _x2 dest r# is NOT allowed";
string PS_060 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_x2 r0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_061_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_1_2 : texm3x3spec _x4 dest r# is NOT allowed";
string PS_061 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_x4 r0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_062_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_1_2 : texm3x3spec _d2 dest r# is NOT allowed";
string PS_062 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_d2 r0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_063_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_1_2 : texm3x3spec _sat dest r# is NOT allowed";
string PS_063 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_sat r0, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_064_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_1_2 : texm3x3spec dest c# is NOT allowed";
string PS_064 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec c1, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_065_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_1_2 : texm3x3spec _x2 dest c# is NOT allowed";
string PS_065 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_x2 c1, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_066_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_1_2 : texm3x3spec _x4 dest c# is NOT allowed";
string PS_066 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_x4 c1, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_1_2 : texm3x3spec _d2 dest c# is NOT allowed";
string PS_067 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_d2 c1, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_1_2 : texm3x3spec _sat dest c# is NOT allowed";
string PS_068 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec_sat c1, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_069_Criteria = { true, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_1_2 : texm3x3spec write mask .rgba is allowed";
string PS_069 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.rgba, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_1_2 : texm3x3spec write mask .r is NOT allowed";
string PS_070 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.r, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_1_2 : texm3x3spec write mask .g is NOT allowed";
string PS_071 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.g, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_1_2 : texm3x3spec write mask .b is NOT allowed";
string PS_072 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.b, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_1_2 : texm3x3spec write mask .a is NOT allowed";
string PS_073 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.a, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_1_2 : texm3x3spec write mask .rg is NOT allowed";
string PS_074 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.rg, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_1_2 : texm3x3spec write mask .gb is NOT allowed";
string PS_075 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.gb, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_1_2 : texm3x3spec write mask .ba is NOT allowed";
string PS_076 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.ba, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_1_2 : texm3x3spec write mask .rb is NOT allowed";
string PS_077 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.rb, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_1_2 : texm3x3spec write mask .ra is NOT allowed";
string PS_078 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.ra, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_1_2 : texm3x3spec write mask .ga is NOT allowed";
string PS_079 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.ga, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_080_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_1_2 : texm3x3spec write mask .rgb is NOT allowed";
string PS_080 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.rgb, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_081_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_1_2 : texm3x3spec write mask .gba is NOT allowed";
string PS_081 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.gba, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_082_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_1_2 : texm3x3spec write mask .rba is NOT allowed";
string PS_082 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.rba, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_083_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_1_2 : texm3x3spec write mask .yx is NOT allowed";
string PS_083 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.yx, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_084_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_1_2 : texm3x3spec write mask .zx is NOT allowed";
string PS_084 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.zx, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_085_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_1_2 : texm3x3spec write mask .zy is NOT allowed";
string PS_085 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.zy, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_086_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_1_2 : texm3x3spec write mask .wx is NOT allowed";
string PS_086 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.wx, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_087_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_1_2 : texm3x3spec write mask .wz is NOT allowed";
string PS_087 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.wz, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_088_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_1_2 : texm3x3spec write mask .wy is NOT allowed";
string PS_088 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.wy, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_089_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_1_2 : texm3x3spec write mask .zyx is NOT allowed";
string PS_089 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.zyx, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_090_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_1_2 : texm3x3spec write mask .wzy is NOT allowed";
string PS_090 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.wzy, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_091_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_1_2 : texm3x3spec write mask .wzx is NOT allowed";
string PS_091 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.wzx, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_092_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_1_2 : texm3x3spec write mask .wyx is NOT allowed";
string PS_092 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.wyx, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_093_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_1_2 : texm3x3spec write mask .yxw is NOT allowed";
string PS_093 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.yxw, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_094_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_1_2 : texm3x3spec write mask .wzyx is NOT allowed";
string PS_094 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.wzyx, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_095_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_1_2 : texm3x3spec write mask .zxwy is NOT allowed";
string PS_095 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texcoord t0 "
	"texm3x3pad t1, t0 "
	"texm3x3pad t2, t0 "
	"texm3x3spec t3.zxwy, t0, c0 "
	"mov r0, c0 ";


