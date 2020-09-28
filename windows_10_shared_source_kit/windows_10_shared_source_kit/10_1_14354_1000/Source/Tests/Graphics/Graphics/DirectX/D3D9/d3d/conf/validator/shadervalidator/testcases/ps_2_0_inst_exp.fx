#include "Test_Include.fx"

int Test_Count = 80;

string TestInfo
<
	string TestType = "PS";
>
= "ps_2_0_inst_exp";

PS_CRITERIA PS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_2_0 : exp source reg combination c0.r is allowed";
string PS_001 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.r "
	"mov oC0, c0 ";

PS_CRITERIA PS_002_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_002_Desc = "ps_2_0 : exp source reg combination c0.g is allowed";
string PS_002 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_003_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_003_Desc = "ps_2_0 : exp source reg combination c0.b is allowed";
string PS_003 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_004_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_004_Desc = "ps_2_0 : exp source reg combination c0.a is allowed";
string PS_004 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_005_Desc = "ps_2_0 : exp source reg combination b0 is NOT allowed";
string PS_005 = 
	"ps_2_0 "
	"defb b0, true "
	"exp r0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_006_Desc = "ps_2_0 : exp source reg combination i0 is NOT allowed";
string PS_006 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"exp r0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_007_Desc = "ps_2_0 : exp source reg combination p0 is NOT allowed";
string PS_007 = 
	"ps_2_0 "
	"exp r0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_008_Desc = "ps_2_0 : exp source reg combination s0 is NOT allowed";
string PS_008 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"exp r0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_009_Desc = "ps_2_0 : exp source reg combination t0 is NOT allowed";
string PS_009 = 
	"ps_2_0 "
	"dcl t0 "
	"exp r0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_010_Desc = "ps_2_0 : exp source reg combination oC0 is NOT allowed";
string PS_010 = 
	"ps_2_0 "
	"exp r0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_011_Desc = "ps_2_0 : exp source reg combination oDepth is NOT allowed";
string PS_011 = 
	"ps_2_0 "
	"exp r0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_012_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_2_0 : exp source reg combination c0.gbra is NOT allowed";
string PS_012 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.gbra "
	"mov oC0, c0 ";

PS_CRITERIA PS_013_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_2_0 : exp source reg combination c0.brga is NOT allowed";
string PS_013 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.brga "
	"mov oC0, c0 ";

PS_CRITERIA PS_014_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_2_0 : exp source reg combination c0.abgr is NOT allowed";
string PS_014 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.abgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_015_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_2_0 : exp source reg combination c0.agrb is NOT allowed";
string PS_015 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.agrb "
	"mov oC0, c0 ";

PS_CRITERIA PS_016_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_2_0 : exp source reg combination c0.rbga is NOT allowed";
string PS_016 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.rbga "
	"mov oC0, c0 ";

PS_CRITERIA PS_017_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_2_0 : exp source reg combination c0.rgab is NOT allowed";
string PS_017 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.rgab "
	"mov oC0, c0 ";

PS_CRITERIA PS_018_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_2_0 : exp source reg combination c0.bgr is NOT allowed";
string PS_018 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.bgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_019_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_2_0 : exp source reg combination c0.rbg is NOT allowed";
string PS_019 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.rbg "
	"mov oC0, c0 ";

PS_CRITERIA PS_020_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_2_0 : exp source reg combination c0.gar is NOT allowed";
string PS_020 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.gar "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_2_0 : exp source reg combination c0.gr is NOT allowed";
string PS_021 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.gr "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_2_0 : exp source reg combination c0.ab is NOT allowed";
string PS_022 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.ab "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_2_0 : exp source reg combination c0.bg is NOT allowed";
string PS_023 = 
	"ps_2_0 "
	"def c0, 1, 1, 1, 1 "
	"exp r0, c0.bg "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_2_0 : exp dest r# is allowed";
string PS_024 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_2_0 : exp _pp dest r# is allowed";
string PS_025 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_pp r0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_2_0 : exp _sat dest r# is allowed";
string PS_026 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_sat r0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_0 : exp dest v# is NOT allowed";
string PS_027 = 
	"ps_2_0 "
	"dcl v0 "
	"exp v0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_0 : exp _pp dest v# is NOT allowed";
string PS_028 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_pp v0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_2_0 : exp _sat dest v# is NOT allowed";
string PS_029 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_sat v0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_030_Desc = "ps_2_0 : exp dest c# is NOT allowed";
string PS_030 = 
	"ps_2_0 "
	"dcl v0 "
	"exp c1, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_2_0 : exp _pp dest c# is NOT allowed";
string PS_031 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_pp c1, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_2_0 : exp _sat dest c# is NOT allowed";
string PS_032 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_sat c1, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_2_0 : exp dest b# is NOT allowed";
string PS_033 = 
	"ps_2_0 "
	"dcl v0 "
	"exp b0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_2_0 : exp _pp dest b# is NOT allowed";
string PS_034 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_pp b0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_2_0 : exp _sat dest b# is NOT allowed";
string PS_035 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_sat b0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_2_0 : exp dest i# is NOT allowed";
string PS_036 = 
	"ps_2_0 "
	"dcl v0 "
	"exp i0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_2_0 : exp _pp dest i# is NOT allowed";
string PS_037 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_pp i0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_2_0 : exp _sat dest i# is NOT allowed";
string PS_038 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_sat i0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_039_Desc = "ps_2_0 : exp dest p0 is NOT allowed";
string PS_039 = 
	"ps_2_0 "
	"dcl v0 "
	"exp p0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_040_Desc = "ps_2_0 : exp _pp dest p0 is NOT allowed";
string PS_040 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_pp p0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_041_Desc = "ps_2_0 : exp _sat dest p0 is NOT allowed";
string PS_041 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_sat p0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_2_0 : exp dest s# is NOT allowed";
string PS_042 = 
	"ps_2_0 "
	"dcl v0 "
	"exp s0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_2_0 : exp _pp dest s# is NOT allowed";
string PS_043 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_pp s0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_2_0 : exp _sat dest s# is NOT allowed";
string PS_044 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_sat s0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_2_0 : exp dest t# is NOT allowed";
string PS_045 = 
	"ps_2_0 "
	"dcl v0 "
	"exp t0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_046_Desc = "ps_2_0 : exp _pp dest t# is NOT allowed";
string PS_046 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_pp t0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_047_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_047_Desc = "ps_2_0 : exp _sat dest t# is NOT allowed";
string PS_047 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_sat t0, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_048_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_048_Desc = "ps_2_0 : exp dest oC# is NOT allowed";
string PS_048 = 
	"ps_2_0 "
	"dcl v0 "
	"exp oC0, v0.x ";

PS_CRITERIA PS_049_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_049_Desc = "ps_2_0 : exp _pp dest oC# is NOT allowed";
string PS_049 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_pp oC0, v0.x ";

PS_CRITERIA PS_050_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_050_Desc = "ps_2_0 : exp _sat dest oC# is NOT allowed";
string PS_050 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_sat oC0, v0.x ";

PS_CRITERIA PS_051_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_051_Desc = "ps_2_0 : exp dest oDepth is NOT allowed";
string PS_051 = 
	"ps_2_0 "
	"dcl v0 "
	"exp oDepth, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_052_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_052_Desc = "ps_2_0 : exp _pp dest oDepth is NOT allowed";
string PS_052 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_pp oDepth, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_053_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_053_Desc = "ps_2_0 : exp _sat dest oDepth is NOT allowed";
string PS_053 = 
	"ps_2_0 "
	"dcl v0 "
	"exp_sat oDepth, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_054_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_054_Desc = "ps_2_0 : exp write mask .r is allowed";
string PS_054 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.r, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_055_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_055_Desc = "ps_2_0 : exp write mask .g is allowed";
string PS_055 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.g, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_056_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_056_Desc = "ps_2_0 : exp write mask .b is allowed";
string PS_056 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.b, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_057_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_057_Desc = "ps_2_0 : exp write mask .a is allowed";
string PS_057 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.a, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_058_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_058_Desc = "ps_2_0 : exp write mask .rg is allowed";
string PS_058 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.rg, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_059_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_059_Desc = "ps_2_0 : exp write mask .gb is allowed";
string PS_059 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.gb, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_060_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_060_Desc = "ps_2_0 : exp write mask .ba is allowed";
string PS_060 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.ba, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_061_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_061_Desc = "ps_2_0 : exp write mask .rb is allowed";
string PS_061 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.rb, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_062_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_062_Desc = "ps_2_0 : exp write mask .ra is allowed";
string PS_062 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.ra, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_063_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_063_Desc = "ps_2_0 : exp write mask .ga is allowed";
string PS_063 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.ga, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_064_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_064_Desc = "ps_2_0 : exp write mask .rgb is allowed";
string PS_064 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.rgb, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_065_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_065_Desc = "ps_2_0 : exp write mask .gba is allowed";
string PS_065 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.gba, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_066_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_066_Desc = "ps_2_0 : exp write mask .rba is allowed";
string PS_066 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.rba, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_067_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_067_Desc = "ps_2_0 : exp write mask .rgba is allowed";
string PS_067 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.rgba, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_068_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_068_Desc = "ps_2_0 : exp write mask .yx is NOT allowed";
string PS_068 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.yx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_069_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_069_Desc = "ps_2_0 : exp write mask .zx is NOT allowed";
string PS_069 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.zx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_070_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_070_Desc = "ps_2_0 : exp write mask .zy is NOT allowed";
string PS_070 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.zy, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_071_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_2_0 : exp write mask .wx is NOT allowed";
string PS_071 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.wx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_072_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_2_0 : exp write mask .wz is NOT allowed";
string PS_072 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.wz, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_073_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_2_0 : exp write mask .wy is NOT allowed";
string PS_073 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.wy, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_074_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_2_0 : exp write mask .zyx is NOT allowed";
string PS_074 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.zyx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_2_0 : exp write mask .wzy is NOT allowed";
string PS_075 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.wzy, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_2_0 : exp write mask .wzx is NOT allowed";
string PS_076 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.wzx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_2_0 : exp write mask .wyx is NOT allowed";
string PS_077 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.wyx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_078_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_2_0 : exp write mask .yxw is NOT allowed";
string PS_078 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.yxw, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_2_0 : exp write mask .wzyx is NOT allowed";
string PS_079 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.wzyx, v0.x "
	"mov oC0, c0 ";

PS_CRITERIA PS_080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_2_0 : exp write mask .zxwy is NOT allowed";
string PS_080 = 
	"ps_2_0 "
	"dcl v0 "
	"exp r0.zxwy, v0.x "
	"mov oC0, c0 ";


