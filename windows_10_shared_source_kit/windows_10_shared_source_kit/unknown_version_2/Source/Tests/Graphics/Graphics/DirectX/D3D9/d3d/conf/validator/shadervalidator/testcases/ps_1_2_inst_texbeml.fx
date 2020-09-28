#include "Test_Include.fx"

int Test_Count = 73;

string TestInfo
<
	string TestType = "PS";
>
= "ps_1_2_inst_texbeml";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_1_2 : texbeml source reg combination t0 is allowed";
string PS_001 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_1_2 : texbeml source reg combination t0 is allowed";
string PS_002 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_003_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_1_2 : texbeml source reg combination v0 is NOT allowed";
string PS_003 = 
	"ps_1_2 "
	"texbeml t3, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_004_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_1_2 : texbeml source reg combination r0 is NOT allowed";
string PS_004 = 
	"ps_1_2 "
	"mov r0, c0 "
	"texbeml t3, r0 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_1_2 : texbeml source reg combination c0 is NOT allowed";
string PS_005 = 
	"ps_1_2 "
	"def c0, 1, 1, 1, 1 "
	"texbeml t3, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_1_2 : texbeml source reg combination t0_bias is NOT allowed";
string PS_006 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0_bias "
	"mov r0, c0 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_1_2 : texbeml source reg combination 1 - t0 is NOT allowed";
string PS_007 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, 1 - t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_1_2 : texbeml source reg combination -t0 is NOT allowed";
string PS_008 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, -t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_1_2 : texbeml source reg combination t0_bx2 is NOT allowed";
string PS_009 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0_bx2 "
	"mov r0, c0 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_1_2 : texbeml source reg combination -t0_bx2 is NOT allowed";
string PS_010 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, -t0_bx2 "
	"mov r0, c0 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_1_2 : texbeml source reg combination t0.r is NOT allowed";
string PS_011 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.r "
	"mov r0, c0 ";

PS_CRITERIA PS_012_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_1_2 : texbeml source reg combination t0.g is NOT allowed";
string PS_012 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.g "
	"mov r0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_1_2 : texbeml source reg combination t0.b is NOT allowed";
string PS_013 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.b "
	"mov r0, c0 ";

PS_CRITERIA PS_014_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_1_2 : texbeml source reg combination t0.a is NOT allowed";
string PS_014 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.a "
	"mov r0, c0 ";

PS_CRITERIA PS_015_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_1_2 : texbeml source reg combination t0.gbra is NOT allowed";
string PS_015 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.gbra "
	"mov r0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_1_2 : texbeml source reg combination t0.brga is NOT allowed";
string PS_016 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.brga "
	"mov r0, c0 ";

PS_CRITERIA PS_017_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_1_2 : texbeml source reg combination t0.abgr is NOT allowed";
string PS_017 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.abgr "
	"mov r0, c0 ";

PS_CRITERIA PS_018_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_1_2 : texbeml source reg combination t0.agrb is NOT allowed";
string PS_018 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.agrb "
	"mov r0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_1_2 : texbeml source reg combination t0.rbga is NOT allowed";
string PS_019 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.rbga "
	"mov r0, c0 ";

PS_CRITERIA PS_020_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_1_2 : texbeml source reg combination t0.rgab is NOT allowed";
string PS_020 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.rgab "
	"mov r0, c0 ";

PS_CRITERIA PS_021_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_1_2 : texbeml source reg combination t0.bgr is NOT allowed";
string PS_021 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.bgr "
	"mov r0, c0 ";

PS_CRITERIA PS_022_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_1_2 : texbeml source reg combination t0.rbg is NOT allowed";
string PS_022 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.rbg "
	"mov r0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_1_2 : texbeml source reg combination t0.gar is NOT allowed";
string PS_023 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.gar "
	"mov r0, c0 ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_1_2 : texbeml source reg combination t0.gr is NOT allowed";
string PS_024 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.gr "
	"mov r0, c0 ";

PS_CRITERIA PS_025_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_1_2 : texbeml source reg combination t0.ab is NOT allowed";
string PS_025 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.ab "
	"mov r0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_1_2 : texbeml source reg combination t0.bg is NOT allowed";
string PS_026 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0.bg "
	"mov r0, c0 ";

PS_CRITERIA PS_027_Criteria = { true, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_1_2 : texbeml dest t# is allowed";
string PS_027 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_1_2 : texbeml _x2 dest t# is NOT allowed";
string PS_028 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_x2 t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_1_2 : texbeml _x4 dest t# is NOT allowed";
string PS_029 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_x4 t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_1_2 : texbeml _d2 dest t# is NOT allowed";
string PS_030 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_d2 t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_1_2 : texbeml _sat dest t# is NOT allowed";
string PS_031 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_sat t3, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_1_2 : texbeml dest v# is NOT allowed";
string PS_032 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_1_2 : texbeml _x2 dest v# is NOT allowed";
string PS_033 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_x2 v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_1_2 : texbeml _x4 dest v# is NOT allowed";
string PS_034 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_x4 v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_1_2 : texbeml _d2 dest v# is NOT allowed";
string PS_035 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_d2 v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_1_2 : texbeml _sat dest v# is NOT allowed";
string PS_036 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_sat v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_1_2 : texbeml dest r# is NOT allowed";
string PS_037 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml r0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_1_2 : texbeml _x2 dest r# is NOT allowed";
string PS_038 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_x2 r0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_1_2 : texbeml _x4 dest r# is NOT allowed";
string PS_039 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_x4 r0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_1_2 : texbeml _d2 dest r# is NOT allowed";
string PS_040 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_d2 r0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_1_2 : texbeml _sat dest r# is NOT allowed";
string PS_041 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_sat r0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_1_2 : texbeml dest c# is NOT allowed";
string PS_042 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_1_2 : texbeml _x2 dest c# is NOT allowed";
string PS_043 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_x2 c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_1_2 : texbeml _x4 dest c# is NOT allowed";
string PS_044 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_x4 c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_1_2 : texbeml _d2 dest c# is NOT allowed";
string PS_045 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_d2 c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_1_2 : texbeml _sat dest c# is NOT allowed";
string PS_046 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml_sat c1, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_047_Criteria = { true, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_1_2 : texbeml write mask .rgba is allowed";
string PS_047 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.rgba, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_1_2 : texbeml write mask .r is NOT allowed";
string PS_048 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.r, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_1_2 : texbeml write mask .g is NOT allowed";
string PS_049 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.g, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_1_2 : texbeml write mask .b is NOT allowed";
string PS_050 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.b, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_1_2 : texbeml write mask .a is NOT allowed";
string PS_051 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.a, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_1_2 : texbeml write mask .rg is NOT allowed";
string PS_052 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.rg, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_1_2 : texbeml write mask .gb is NOT allowed";
string PS_053 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.gb, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_054_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_1_2 : texbeml write mask .ba is NOT allowed";
string PS_054 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.ba, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_1_2 : texbeml write mask .rb is NOT allowed";
string PS_055 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.rb, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_056_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_1_2 : texbeml write mask .ra is NOT allowed";
string PS_056 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.ra, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_1_2 : texbeml write mask .ga is NOT allowed";
string PS_057 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.ga, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_058_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_1_2 : texbeml write mask .rgb is NOT allowed";
string PS_058 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.rgb, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_1_2 : texbeml write mask .gba is NOT allowed";
string PS_059 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.gba, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_060_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_1_2 : texbeml write mask .rba is NOT allowed";
string PS_060 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.rba, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_061_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_1_2 : texbeml write mask .yx is NOT allowed";
string PS_061 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.yx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_062_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_1_2 : texbeml write mask .zx is NOT allowed";
string PS_062 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.zx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_063_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_1_2 : texbeml write mask .zy is NOT allowed";
string PS_063 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.zy, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_064_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_1_2 : texbeml write mask .wx is NOT allowed";
string PS_064 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.wx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_065_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_1_2 : texbeml write mask .wz is NOT allowed";
string PS_065 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.wz, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_066_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_1_2 : texbeml write mask .wy is NOT allowed";
string PS_066 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.wy, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_067_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_1_2 : texbeml write mask .zyx is NOT allowed";
string PS_067 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.zyx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_1_2 : texbeml write mask .wzy is NOT allowed";
string PS_068 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.wzy, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_069_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_1_2 : texbeml write mask .wzx is NOT allowed";
string PS_069 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.wzx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_1_2 : texbeml write mask .wyx is NOT allowed";
string PS_070 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.wyx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_1_2 : texbeml write mask .yxw is NOT allowed";
string PS_071 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.yxw, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_1_2 : texbeml write mask .wzyx is NOT allowed";
string PS_072 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.wzyx, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, false, 0x0102, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_1_2 : texbeml write mask .zxwy is NOT allowed";
string PS_073 = 
	"ps_1_2 "
	"texcoord t0 "
	"texbeml t3.zxwy, t0 "
	"mov r0, c0 ";


