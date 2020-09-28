#include "Test_Include.fx"

int Test_Count = 57;

string TestInfo
<
	string TestType = "PS";
>
= "ps_2_0_inst_texkill";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_2_0 : texkill dest r# is allowed";
string PS_001 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_2_0 : texkill _pp dest r# is allowed";
string PS_002 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_pp r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_003_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_2_0 : texkill _sat dest r# is NOT allowed";
string PS_003 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_sat r0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_004_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_2_0 : texkill dest t# is allowed";
string PS_004 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_005_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_2_0 : texkill _pp dest t# is allowed";
string PS_005 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_pp t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_2_0 : texkill _sat dest t# is NOT allowed";
string PS_006 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_sat t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_2_0 : texkill dest v# is NOT allowed";
string PS_007 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_2_0 : texkill _pp dest v# is NOT allowed";
string PS_008 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_pp v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_2_0 : texkill _sat dest v# is NOT allowed";
string PS_009 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_sat v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_2_0 : texkill dest c# is NOT allowed";
string PS_010 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_2_0 : texkill _pp dest c# is NOT allowed";
string PS_011 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_pp c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_012_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_2_0 : texkill _sat dest c# is NOT allowed";
string PS_012 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_sat c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_2_0 : texkill dest b# is NOT allowed";
string PS_013 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_014_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_2_0 : texkill _pp dest b# is NOT allowed";
string PS_014 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_pp b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_015_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_2_0 : texkill _sat dest b# is NOT allowed";
string PS_015 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_sat b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_2_0 : texkill dest i# is NOT allowed";
string PS_016 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_017_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_2_0 : texkill _pp dest i# is NOT allowed";
string PS_017 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_pp i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_018_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_2_0 : texkill _sat dest i# is NOT allowed";
string PS_018 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_sat i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_019_Desc = "ps_2_0 : texkill dest p0 is NOT allowed";
string PS_019 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_020_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_020_Desc = "ps_2_0 : texkill _pp dest p0 is NOT allowed";
string PS_020 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_pp p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_021_Desc = "ps_2_0 : texkill _sat dest p0 is NOT allowed";
string PS_021 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_sat p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_2_0 : texkill dest s# is NOT allowed";
string PS_022 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_2_0 : texkill _pp dest s# is NOT allowed";
string PS_023 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_pp s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_2_0 : texkill _sat dest s# is NOT allowed";
string PS_024 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_sat s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_2_0 : texkill dest oC# is NOT allowed";
string PS_025 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill oC0 ";

PS_CRITERIA PS_026_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_2_0 : texkill _pp dest oC# is NOT allowed";
string PS_026 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_pp oC0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_0 : texkill _sat dest oC# is NOT allowed";
string PS_027 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_sat oC0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_0 : texkill dest oDepth is NOT allowed";
string PS_028 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_2_0 : texkill _pp dest oDepth is NOT allowed";
string PS_029 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_pp oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_2_0 : texkill _sat dest oDepth is NOT allowed";
string PS_030 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill_sat oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_2_0 : texkill write mask .rgba is allowed";
string PS_031 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.rgba "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_2_0 : texkill write mask .r is NOT allowed";
string PS_032 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.r "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_2_0 : texkill write mask .g is NOT allowed";
string PS_033 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_2_0 : texkill write mask .b is NOT allowed";
string PS_034 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_2_0 : texkill write mask .a is NOT allowed";
string PS_035 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_2_0 : texkill write mask .rg is NOT allowed";
string PS_036 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.rg "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_2_0 : texkill write mask .gb is NOT allowed";
string PS_037 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.gb "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_2_0 : texkill write mask .ba is NOT allowed";
string PS_038 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.ba "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_2_0 : texkill write mask .rb is NOT allowed";
string PS_039 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.rb "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_2_0 : texkill write mask .ra is NOT allowed";
string PS_040 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.ra "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_2_0 : texkill write mask .ga is NOT allowed";
string PS_041 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.ga "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_2_0 : texkill write mask .rgb is NOT allowed";
string PS_042 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.rgb "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_2_0 : texkill write mask .gba is NOT allowed";
string PS_043 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.gba "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_2_0 : texkill write mask .rba is NOT allowed";
string PS_044 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.rba "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_2_0 : texkill write mask .yx is NOT allowed";
string PS_045 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.yx "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_2_0 : texkill write mask .zx is NOT allowed";
string PS_046 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.zx "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_2_0 : texkill write mask .zy is NOT allowed";
string PS_047 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.zy "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_2_0 : texkill write mask .wx is NOT allowed";
string PS_048 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.wx "
	"mov oC0, c0 ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_2_0 : texkill write mask .wz is NOT allowed";
string PS_049 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.wz "
	"mov oC0, c0 ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_2_0 : texkill write mask .wy is NOT allowed";
string PS_050 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.wy "
	"mov oC0, c0 ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_2_0 : texkill write mask .zyx is NOT allowed";
string PS_051 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.zyx "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_2_0 : texkill write mask .wzy is NOT allowed";
string PS_052 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.wzy "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_2_0 : texkill write mask .wzx is NOT allowed";
string PS_053 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.wzx "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_2_0 : texkill write mask .wyx is NOT allowed";
string PS_054 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.wyx "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_2_0 : texkill write mask .yxw is NOT allowed";
string PS_055 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.yxw "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_2_0 : texkill write mask .wzyx is NOT allowed";
string PS_056 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.wzyx "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_2_0 : texkill write mask .zxwy is NOT allowed";
string PS_057 = 
	"ps_2_0 "
	"dcl t0 "
	"mov r0, c0 "
	"texkill r0.zxwy "
	"mov oC0, c0 ";


