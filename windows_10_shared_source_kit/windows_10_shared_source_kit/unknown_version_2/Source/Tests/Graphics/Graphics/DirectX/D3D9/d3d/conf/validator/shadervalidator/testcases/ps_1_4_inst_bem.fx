#include "Test_Include.fx"

int Test_Count = 35;

string TestInfo
<
	string TestType = "PS";
>
= "ps_1_4_inst_bem";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_1_4 : bem source reg combination r0, r1 is allowed";
string PS_001 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.rg, r0, r1 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_1_4 : bem source reg combination c0, r0 is allowed";
string PS_002 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"bem r0.rg, c0, r0 ";

PS_CRITERIA PS_003_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_1_4 : bem source reg combination v0, v0 is NOT allowed";
string PS_003 = 
	"ps_1_4 "
	"bem r0.rg, v0, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_004_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_1_4 : bem source reg combination v0, c0 is NOT allowed";
string PS_004 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"bem r0.rg, v0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_1_4 : bem source reg combination v0, t0 is NOT allowed";
string PS_005 = 
	"ps_1_4 "
	"texcrd r0, t0 "
	"bem r0.rg, v0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_1_4 : bem source reg combination t0, v0 is NOT allowed";
string PS_006 = 
	"ps_1_4 "
	"texcrd r0, t0 "
	"bem r0.rg, t0, v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_1_4 : bem source reg combination t0, c0 is NOT allowed";
string PS_007 = 
	"ps_1_4 "
	"def c0, 1, 1, 1, 1 "
	"texcrd r0, t0 "
	"bem r0.rg, t0, c0 "
	"mov r0, c0 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_1_4 : bem source reg combination t0, t0 is NOT allowed";
string PS_008 = 
	"ps_1_4 "
	"texcrd r0, t0 "
	"bem r0.rg, t0, t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_009_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_1_4 : bem write mask .rg is allowed";
string PS_009 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.rg, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_1_4 : bem write mask .r is NOT allowed";
string PS_010 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.r, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_1_4 : bem write mask .g is NOT allowed";
string PS_011 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.g, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_012_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_1_4 : bem write mask .b is NOT allowed";
string PS_012 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.b, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_1_4 : bem write mask .a is NOT allowed";
string PS_013 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.a, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_014_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_1_4 : bem write mask .gb is NOT allowed";
string PS_014 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.gb, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_015_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_1_4 : bem write mask .ba is NOT allowed";
string PS_015 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.ba, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_1_4 : bem write mask .rb is NOT allowed";
string PS_016 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.rb, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_017_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_1_4 : bem write mask .ra is NOT allowed";
string PS_017 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.ra, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_018_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_1_4 : bem write mask .ga is NOT allowed";
string PS_018 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.ga, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_1_4 : bem write mask .rgb is NOT allowed";
string PS_019 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.rgb, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_020_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_1_4 : bem write mask .gba is NOT allowed";
string PS_020 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.gba, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_021_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_1_4 : bem write mask .rba is NOT allowed";
string PS_021 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.rba, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_022_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_1_4 : bem write mask .rgba is NOT allowed";
string PS_022 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.rgba, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_1_4 : bem write mask .yx is NOT allowed";
string PS_023 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.yx, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_1_4 : bem write mask .zx is NOT allowed";
string PS_024 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.zx, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_025_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_1_4 : bem write mask .zy is NOT allowed";
string PS_025 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.zy, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_1_4 : bem write mask .wx is NOT allowed";
string PS_026 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.wx, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_1_4 : bem write mask .wz is NOT allowed";
string PS_027 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.wz, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_1_4 : bem write mask .wy is NOT allowed";
string PS_028 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.wy, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_1_4 : bem write mask .zyx is NOT allowed";
string PS_029 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.zyx, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_1_4 : bem write mask .wzy is NOT allowed";
string PS_030 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.wzy, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_1_4 : bem write mask .wzx is NOT allowed";
string PS_031 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.wzx, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_1_4 : bem write mask .wyx is NOT allowed";
string PS_032 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.wyx, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_1_4 : bem write mask .yxw is NOT allowed";
string PS_033 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.yxw, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_1_4 : bem write mask .wzyx is NOT allowed";
string PS_034 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.wzyx, r0, r1 "
	"mov r0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_1_4 : bem write mask .zxwy is NOT allowed";
string PS_035 = 
	"ps_1_4 "
	"mov r0, c0 "
	"mov r1, c0 "
	"bem r0.zxwy, r0, r1 "
	"mov r0, c0 ";


