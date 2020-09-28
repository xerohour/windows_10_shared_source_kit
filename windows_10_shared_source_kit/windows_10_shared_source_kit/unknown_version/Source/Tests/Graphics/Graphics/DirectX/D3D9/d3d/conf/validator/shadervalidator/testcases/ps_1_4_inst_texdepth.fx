#include "Test_Include.fx"

int Test_Count = 59;

string TestInfo
<
	string TestType = "PS";
>
= "ps_1_4_inst_texdepth";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_1_4 : texdepth dest r# is allowed";
string PS_001 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5 "
	"mov r0, c0 ";

PS_CRITERIA PS_002_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_1_4 : texdepth _x2 dest r# is NOT allowed";
string PS_002 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x2 r5 "
	"mov r0, c0 ";

PS_CRITERIA PS_003_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_1_4 : texdepth _x4 dest r# is NOT allowed";
string PS_003 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x4 r5 "
	"mov r0, c0 ";

PS_CRITERIA PS_004_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_1_4 : texdepth _x8 dest r# is NOT allowed";
string PS_004 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x8 r5 "
	"mov r0, c0 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_1_4 : texdepth _d2 dest r# is NOT allowed";
string PS_005 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d2 r5 "
	"mov r0, c0 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_1_4 : texdepth _d4 dest r# is NOT allowed";
string PS_006 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d4 r5 "
	"mov r0, c0 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_1_4 : texdepth _d8 dest r# is NOT allowed";
string PS_007 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d8 r5 "
	"mov r0, c0 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_1_4 : texdepth _sat dest r# is NOT allowed";
string PS_008 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_sat r5 "
	"mov r0, c0 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_1_4 : texdepth dest v# is NOT allowed";
string PS_009 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_1_4 : texdepth _x2 dest v# is NOT allowed";
string PS_010 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x2 v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_1_4 : texdepth _x4 dest v# is NOT allowed";
string PS_011 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x4 v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_012_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_1_4 : texdepth _x8 dest v# is NOT allowed";
string PS_012 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x8 v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_1_4 : texdepth _d2 dest v# is NOT allowed";
string PS_013 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d2 v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_014_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_1_4 : texdepth _d4 dest v# is NOT allowed";
string PS_014 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d4 v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_015_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_1_4 : texdepth _d8 dest v# is NOT allowed";
string PS_015 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d8 v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_1_4 : texdepth _sat dest v# is NOT allowed";
string PS_016 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_sat v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_017_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_1_4 : texdepth dest c# is NOT allowed";
string PS_017 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_018_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_1_4 : texdepth _x2 dest c# is NOT allowed";
string PS_018 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x2 c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_1_4 : texdepth _x4 dest c# is NOT allowed";
string PS_019 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x4 c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_020_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_1_4 : texdepth _x8 dest c# is NOT allowed";
string PS_020 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x8 c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_021_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_1_4 : texdepth _d2 dest c# is NOT allowed";
string PS_021 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d2 c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_022_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_1_4 : texdepth _d4 dest c# is NOT allowed";
string PS_022 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d4 c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_1_4 : texdepth _d8 dest c# is NOT allowed";
string PS_023 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d8 c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_1_4 : texdepth _sat dest c# is NOT allowed";
string PS_024 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_sat c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_025_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_1_4 : texdepth dest t# is NOT allowed";
string PS_025 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_1_4 : texdepth _x2 dest t# is NOT allowed";
string PS_026 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x2 t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_1_4 : texdepth _x4 dest t# is NOT allowed";
string PS_027 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x4 t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_1_4 : texdepth _x8 dest t# is NOT allowed";
string PS_028 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_x8 t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_1_4 : texdepth _d2 dest t# is NOT allowed";
string PS_029 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d2 t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_1_4 : texdepth _d4 dest t# is NOT allowed";
string PS_030 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d4 t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_1_4 : texdepth _d8 dest t# is NOT allowed";
string PS_031 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_d8 t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_1_4 : texdepth _sat dest t# is NOT allowed";
string PS_032 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth_sat t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_033_Criteria = { true, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_1_4 : texdepth write mask .rgba is allowed";
string PS_033 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.rgba "
	"mov r0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_1_4 : texdepth write mask .r is NOT allowed";
string PS_034 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.r "
	"mov r0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_1_4 : texdepth write mask .g is NOT allowed";
string PS_035 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.g "
	"mov r0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_1_4 : texdepth write mask .b is NOT allowed";
string PS_036 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.b "
	"mov r0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_1_4 : texdepth write mask .a is NOT allowed";
string PS_037 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.a "
	"mov r0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_1_4 : texdepth write mask .rg is NOT allowed";
string PS_038 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.rg "
	"mov r0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_1_4 : texdepth write mask .gb is NOT allowed";
string PS_039 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.gb "
	"mov r0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_1_4 : texdepth write mask .ba is NOT allowed";
string PS_040 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.ba "
	"mov r0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_1_4 : texdepth write mask .rb is NOT allowed";
string PS_041 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.rb "
	"mov r0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_1_4 : texdepth write mask .ra is NOT allowed";
string PS_042 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.ra "
	"mov r0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_1_4 : texdepth write mask .ga is NOT allowed";
string PS_043 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.ga "
	"mov r0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_1_4 : texdepth write mask .rgb is NOT allowed";
string PS_044 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.rgb "
	"mov r0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_1_4 : texdepth write mask .gba is NOT allowed";
string PS_045 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.gba "
	"mov r0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_1_4 : texdepth write mask .rba is NOT allowed";
string PS_046 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.rba "
	"mov r0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_1_4 : texdepth write mask .yx is NOT allowed";
string PS_047 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.yx "
	"mov r0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_1_4 : texdepth write mask .zx is NOT allowed";
string PS_048 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.zx "
	"mov r0, c0 ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_1_4 : texdepth write mask .zy is NOT allowed";
string PS_049 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.zy "
	"mov r0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_1_4 : texdepth write mask .wx is NOT allowed";
string PS_050 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.wx "
	"mov r0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_1_4 : texdepth write mask .wz is NOT allowed";
string PS_051 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.wz "
	"mov r0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_1_4 : texdepth write mask .wy is NOT allowed";
string PS_052 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.wy "
	"mov r0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_1_4 : texdepth write mask .zyx is NOT allowed";
string PS_053 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.zyx "
	"mov r0, c0 ";

PS_CRITERIA PS_054_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_1_4 : texdepth write mask .wzy is NOT allowed";
string PS_054 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.wzy "
	"mov r0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_1_4 : texdepth write mask .wzx is NOT allowed";
string PS_055 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.wzx "
	"mov r0, c0 ";

PS_CRITERIA PS_056_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_1_4 : texdepth write mask .wyx is NOT allowed";
string PS_056 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.wyx "
	"mov r0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_1_4 : texdepth write mask .yxw is NOT allowed";
string PS_057 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.yxw "
	"mov r0, c0 ";

PS_CRITERIA PS_058_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_1_4 : texdepth write mask .wzyx is NOT allowed";
string PS_058 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.wzyx "
	"mov r0, c0 ";

PS_CRITERIA PS_059_Criteria = { false, false, 0x0104, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_1_4 : texdepth write mask .zxwy is NOT allowed";
string PS_059 = 
	"ps_1_4 "
	"mov r5, c0 "
	"phase "
	"texdepth r5.zxwy "
	"mov r0, c0 ";


