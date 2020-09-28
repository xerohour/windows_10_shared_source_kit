#include "Test_Include.fx"

int Test_Count = 47;

string TestInfo
<
	string TestType = "PS";
>
= "ps_1_3_inst_texkill";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_1_3 : texkill dest t# is allowed";
string PS_001 = 
	"ps_1_3 "
	"texkill t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_002_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_1_3 : texkill _x2 dest t# is NOT allowed";
string PS_002 = 
	"ps_1_3 "
	"texkill_x2 t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_003_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_1_3 : texkill _x4 dest t# is NOT allowed";
string PS_003 = 
	"ps_1_3 "
	"texkill_x4 t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_004_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_1_3 : texkill _d2 dest t# is NOT allowed";
string PS_004 = 
	"ps_1_3 "
	"texkill_d2 t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_1_3 : texkill _sat dest t# is NOT allowed";
string PS_005 = 
	"ps_1_3 "
	"texkill_sat t0 "
	"mov r0, c0 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_1_3 : texkill dest v# is NOT allowed";
string PS_006 = 
	"ps_1_3 "
	"texkill v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_1_3 : texkill _x2 dest v# is NOT allowed";
string PS_007 = 
	"ps_1_3 "
	"texkill_x2 v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_1_3 : texkill _x4 dest v# is NOT allowed";
string PS_008 = 
	"ps_1_3 "
	"texkill_x4 v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_1_3 : texkill _d2 dest v# is NOT allowed";
string PS_009 = 
	"ps_1_3 "
	"texkill_d2 v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_1_3 : texkill _sat dest v# is NOT allowed";
string PS_010 = 
	"ps_1_3 "
	"texkill_sat v0 "
	"mov r0, c0 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_1_3 : texkill dest r# is NOT allowed";
string PS_011 = 
	"ps_1_3 "
	"texkill r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_012_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_1_3 : texkill _x2 dest r# is NOT allowed";
string PS_012 = 
	"ps_1_3 "
	"texkill_x2 r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_1_3 : texkill _x4 dest r# is NOT allowed";
string PS_013 = 
	"ps_1_3 "
	"texkill_x4 r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_014_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_1_3 : texkill _d2 dest r# is NOT allowed";
string PS_014 = 
	"ps_1_3 "
	"texkill_d2 r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_015_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_1_3 : texkill _sat dest r# is NOT allowed";
string PS_015 = 
	"ps_1_3 "
	"texkill_sat r0 "
	"mov r0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_1_3 : texkill dest c# is NOT allowed";
string PS_016 = 
	"ps_1_3 "
	"texkill c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_017_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_1_3 : texkill _x2 dest c# is NOT allowed";
string PS_017 = 
	"ps_1_3 "
	"texkill_x2 c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_018_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_1_3 : texkill _x4 dest c# is NOT allowed";
string PS_018 = 
	"ps_1_3 "
	"texkill_x4 c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_1_3 : texkill _d2 dest c# is NOT allowed";
string PS_019 = 
	"ps_1_3 "
	"texkill_d2 c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_020_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_1_3 : texkill _sat dest c# is NOT allowed";
string PS_020 = 
	"ps_1_3 "
	"texkill_sat c1 "
	"mov r0, c0 ";

PS_CRITERIA PS_021_Criteria = { true, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_1_3 : texkill write mask .rgba is allowed";
string PS_021 = 
	"ps_1_3 "
	"texkill t0.rgba "
	"mov r0, c0 ";

PS_CRITERIA PS_022_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_1_3 : texkill write mask .r is NOT allowed";
string PS_022 = 
	"ps_1_3 "
	"texkill t0.r "
	"mov r0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_1_3 : texkill write mask .g is NOT allowed";
string PS_023 = 
	"ps_1_3 "
	"texkill t0.g "
	"mov r0, c0 ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_1_3 : texkill write mask .b is NOT allowed";
string PS_024 = 
	"ps_1_3 "
	"texkill t0.b "
	"mov r0, c0 ";

PS_CRITERIA PS_025_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_1_3 : texkill write mask .a is NOT allowed";
string PS_025 = 
	"ps_1_3 "
	"texkill t0.a "
	"mov r0, c0 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_1_3 : texkill write mask .rg is NOT allowed";
string PS_026 = 
	"ps_1_3 "
	"texkill t0.rg "
	"mov r0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_1_3 : texkill write mask .gb is NOT allowed";
string PS_027 = 
	"ps_1_3 "
	"texkill t0.gb "
	"mov r0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_1_3 : texkill write mask .ba is NOT allowed";
string PS_028 = 
	"ps_1_3 "
	"texkill t0.ba "
	"mov r0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_1_3 : texkill write mask .rb is NOT allowed";
string PS_029 = 
	"ps_1_3 "
	"texkill t0.rb "
	"mov r0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_1_3 : texkill write mask .ra is NOT allowed";
string PS_030 = 
	"ps_1_3 "
	"texkill t0.ra "
	"mov r0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_1_3 : texkill write mask .ga is NOT allowed";
string PS_031 = 
	"ps_1_3 "
	"texkill t0.ga "
	"mov r0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_1_3 : texkill write mask .rgb is NOT allowed";
string PS_032 = 
	"ps_1_3 "
	"texkill t0.rgb "
	"mov r0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_1_3 : texkill write mask .gba is NOT allowed";
string PS_033 = 
	"ps_1_3 "
	"texkill t0.gba "
	"mov r0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_1_3 : texkill write mask .rba is NOT allowed";
string PS_034 = 
	"ps_1_3 "
	"texkill t0.rba "
	"mov r0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_1_3 : texkill write mask .yx is NOT allowed";
string PS_035 = 
	"ps_1_3 "
	"texkill t0.yx "
	"mov r0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_1_3 : texkill write mask .zx is NOT allowed";
string PS_036 = 
	"ps_1_3 "
	"texkill t0.zx "
	"mov r0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_1_3 : texkill write mask .zy is NOT allowed";
string PS_037 = 
	"ps_1_3 "
	"texkill t0.zy "
	"mov r0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_1_3 : texkill write mask .wx is NOT allowed";
string PS_038 = 
	"ps_1_3 "
	"texkill t0.wx "
	"mov r0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_1_3 : texkill write mask .wz is NOT allowed";
string PS_039 = 
	"ps_1_3 "
	"texkill t0.wz "
	"mov r0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_1_3 : texkill write mask .wy is NOT allowed";
string PS_040 = 
	"ps_1_3 "
	"texkill t0.wy "
	"mov r0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_1_3 : texkill write mask .zyx is NOT allowed";
string PS_041 = 
	"ps_1_3 "
	"texkill t0.zyx "
	"mov r0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_1_3 : texkill write mask .wzy is NOT allowed";
string PS_042 = 
	"ps_1_3 "
	"texkill t0.wzy "
	"mov r0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_1_3 : texkill write mask .wzx is NOT allowed";
string PS_043 = 
	"ps_1_3 "
	"texkill t0.wzx "
	"mov r0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_1_3 : texkill write mask .wyx is NOT allowed";
string PS_044 = 
	"ps_1_3 "
	"texkill t0.wyx "
	"mov r0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_1_3 : texkill write mask .yxw is NOT allowed";
string PS_045 = 
	"ps_1_3 "
	"texkill t0.yxw "
	"mov r0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_1_3 : texkill write mask .wzyx is NOT allowed";
string PS_046 = 
	"ps_1_3 "
	"texkill t0.wzyx "
	"mov r0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0103, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_1_3 : texkill write mask .zxwy is NOT allowed";
string PS_047 = 
	"ps_1_3 "
	"texkill t0.zxwy "
	"mov r0, c0 ";


