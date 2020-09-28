#include "Test_Include.fx"

int Test_Count = 343;

string TestInfo
<
	string TestType = "VS";
>
= "vs_3_sw_inst_mova";

VS_CRITERIA VS_001_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_3_sw : mova source reg combination v0 is allowed";
string VS_001 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0, v0 "
	" ";

VS_CRITERIA VS_002_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_3_sw : mova source reg combination r0 is allowed";
string VS_002 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0 "
	" ";

VS_CRITERIA VS_003_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_3_sw : mova source reg combination c0 is allowed";
string VS_003 = 
	"vs_3_sw "
	"def c0, 1, 1, 1, 1 "
	"mova a0, c0 "
	" ";

VS_CRITERIA VS_004_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_3_sw : mova source reg combination -r0 is allowed";
string VS_004 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, -r0 "
	" ";

VS_CRITERIA VS_005_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_3_sw : mova source reg combination r0_abs is allowed";
string VS_005 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0_abs "
	" ";

VS_CRITERIA VS_006_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_3_sw : mova source reg combination -r0_abs is allowed";
string VS_006 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, -r0_abs "
	" ";

VS_CRITERIA VS_007_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_3_sw : mova source reg combination r0 is allowed";
string VS_007 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0 "
	" ";

VS_CRITERIA VS_008_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_3_sw : mova source reg combination r0.x is allowed";
string VS_008 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.x "
	" ";

VS_CRITERIA VS_009_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_3_sw : mova source reg combination r0.y is allowed";
string VS_009 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.y "
	" ";

VS_CRITERIA VS_010_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_3_sw : mova source reg combination r0.z is allowed";
string VS_010 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.z "
	" ";

VS_CRITERIA VS_011_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_3_sw : mova source reg combination r0.w is allowed";
string VS_011 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.w "
	" ";

VS_CRITERIA VS_012_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_3_sw : mova source reg combination r0.yzxw is allowed";
string VS_012 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.yzxw "
	" ";

VS_CRITERIA VS_013_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_3_sw : mova source reg combination r0.zxyw is allowed";
string VS_013 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.zxyw "
	" ";

VS_CRITERIA VS_014_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_3_sw : mova source reg combination r0.wzyx is allowed";
string VS_014 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.wzyx "
	" ";

VS_CRITERIA VS_015_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_3_sw : mova source reg combination r0.wyxz is allowed";
string VS_015 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.wyxz "
	" ";

VS_CRITERIA VS_016_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_3_sw : mova source reg combination r0.xzyw is allowed";
string VS_016 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.xzyw "
	" ";

VS_CRITERIA VS_017_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_3_sw : mova source reg combination r0.xywz is allowed";
string VS_017 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.xywz "
	" ";

VS_CRITERIA VS_018_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_3_sw : mova source reg combination r0.zyx is allowed";
string VS_018 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.zyx "
	" ";

VS_CRITERIA VS_019_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_3_sw : mova source reg combination r0.xzy is allowed";
string VS_019 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.xzy "
	" ";

VS_CRITERIA VS_020_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_3_sw : mova source reg combination r0.ywx is allowed";
string VS_020 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.ywx "
	" ";

VS_CRITERIA VS_021_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_3_sw : mova source reg combination r0.yx is allowed";
string VS_021 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.yx "
	" ";

VS_CRITERIA VS_022_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_3_sw : mova source reg combination r0.wz is allowed";
string VS_022 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.wz "
	" ";

VS_CRITERIA VS_023_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_3_sw : mova source reg combination r0.zy is allowed";
string VS_023 = 
	"vs_3_sw "
	"mov r0, c0 "
	"mova a0, r0.zy "
	" ";

VS_CRITERIA VS_024_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_3_sw : mova source reg combination a0 is NOT allowed";
string VS_024 = 
	"vs_3_sw "
	"mova a0.x, c0.x "
	"mova a0, a0 "
	" ";

VS_CRITERIA VS_025_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_3_sw : mova source reg combination b0 is NOT allowed";
string VS_025 = 
	"vs_3_sw "
	"defb b0, true "
	"mova a0, b0 "
	" ";

VS_CRITERIA VS_026_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_3_sw : mova source reg combination i0 is NOT allowed";
string VS_026 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"mova a0, i0 "
	" ";

VS_CRITERIA VS_027_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_3_sw : mova source reg combination aL is NOT allowed";
string VS_027 = 
	"vs_3_sw "
	"mova aL.x, c0.x "
	"mova a0, aL "
	" ";

VS_CRITERIA VS_028_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_3_sw : mova source reg combination p0 is NOT allowed";
string VS_028 = 
	"vs_3_sw "
	"mova a0, p0 "
	" ";

VS_CRITERIA VS_029_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_3_sw : mova source reg combination s0 is NOT allowed";
string VS_029 = 
	"vs_3_sw "
	"dcl_cube s0 "
	"mova a0, s0 "
	" ";

VS_CRITERIA VS_030_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_3_sw : mova source reg combination o0 is NOT allowed";
string VS_030 = 
	"vs_3_sw "
	"mova a0, o0 "
	" ";

VS_CRITERIA VS_031_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_3_sw : mova dest a0 is allowed";
string VS_031 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0, v0 "
	" ";

VS_CRITERIA VS_032_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_3_sw : mova _sat dest a0 is NOT allowed";
string VS_032 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova_sat a0, v0 "
	" ";

VS_CRITERIA VS_033_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_3_sw : mova dest v# is NOT allowed";
string VS_033 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova v0, v0 "
	" ";

VS_CRITERIA VS_034_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_3_sw : mova _sat dest v# is NOT allowed";
string VS_034 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova_sat v0, v0 "
	" ";

VS_CRITERIA VS_035_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_3_sw : mova dest r# is NOT allowed";
string VS_035 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova r0, v0 "
	" ";

VS_CRITERIA VS_036_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_3_sw : mova _sat dest r# is NOT allowed";
string VS_036 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova_sat r0, v0 "
	" ";

VS_CRITERIA VS_037_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_3_sw : mova dest c# is NOT allowed";
string VS_037 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova c1, v0 "
	" ";

VS_CRITERIA VS_038_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_3_sw : mova _sat dest c# is NOT allowed";
string VS_038 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova_sat c1, v0 "
	" ";

VS_CRITERIA VS_039_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_3_sw : mova dest b# is NOT allowed";
string VS_039 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova b0, v0 "
	" ";

VS_CRITERIA VS_040_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_3_sw : mova _sat dest b# is NOT allowed";
string VS_040 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova_sat b0, v0 "
	" ";

VS_CRITERIA VS_041_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_3_sw : mova dest i# is NOT allowed";
string VS_041 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova i0, v0 "
	" ";

VS_CRITERIA VS_042_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_3_sw : mova _sat dest i# is NOT allowed";
string VS_042 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova_sat i0, v0 "
	" ";

VS_CRITERIA VS_043_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_3_sw : mova dest aL is NOT allowed";
string VS_043 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova aL, v0 "
	" ";

VS_CRITERIA VS_044_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_3_sw : mova _sat dest aL is NOT allowed";
string VS_044 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova_sat aL, v0 "
	" ";

VS_CRITERIA VS_045_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_3_sw : mova dest p0 is NOT allowed";
string VS_045 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova p0, v0 "
	" ";

VS_CRITERIA VS_046_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_3_sw : mova _sat dest p0 is NOT allowed";
string VS_046 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova_sat p0, v0 "
	" ";

VS_CRITERIA VS_047_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_3_sw : mova dest s# is NOT allowed";
string VS_047 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova s0, v0 "
	" ";

VS_CRITERIA VS_048_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_3_sw : mova _sat dest s# is NOT allowed";
string VS_048 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova_sat s0, v0 "
	" ";

VS_CRITERIA VS_049_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_3_sw : mova dest o# is NOT allowed";
string VS_049 = 
	"vs_3_sw "
	"dcl_fog o0 "
	"dcl_fog v0 "
	"mova o0, v0 "
	" ";

VS_CRITERIA VS_050_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_3_sw : mova _sat dest o# is NOT allowed";
string VS_050 = 
	"vs_3_sw "
	"dcl_fog o0 "
	"dcl_fog v0 "
	"mova_sat o0, v0 "
	" ";

VS_CRITERIA VS_051_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_3_sw : mova write mask .x is allowed";
string VS_051 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.x, v0 "
	" ";

VS_CRITERIA VS_052_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_3_sw : mova write mask .y is allowed";
string VS_052 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.y, v0 "
	" ";

VS_CRITERIA VS_053_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_3_sw : mova write mask .z is allowed";
string VS_053 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.z, v0 "
	" ";

VS_CRITERIA VS_054_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_3_sw : mova write mask .w is allowed";
string VS_054 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.w, v0 "
	" ";

VS_CRITERIA VS_055_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_3_sw : mova write mask .xy is allowed";
string VS_055 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.xy, v0 "
	" ";

VS_CRITERIA VS_056_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_3_sw : mova write mask .yz is allowed";
string VS_056 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.yz, v0 "
	" ";

VS_CRITERIA VS_057_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_3_sw : mova write mask .zw is allowed";
string VS_057 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.zw, v0 "
	" ";

VS_CRITERIA VS_058_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_3_sw : mova write mask .xz is allowed";
string VS_058 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.xz, v0 "
	" ";

VS_CRITERIA VS_059_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_3_sw : mova write mask .xw is allowed";
string VS_059 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.xw, v0 "
	" ";

VS_CRITERIA VS_060_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_3_sw : mova write mask .yw is allowed";
string VS_060 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.yw, v0 "
	" ";

VS_CRITERIA VS_061_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_3_sw : mova write mask .xyz is allowed";
string VS_061 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.xyz, v0 "
	" ";

VS_CRITERIA VS_062_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_3_sw : mova write mask .yzw is allowed";
string VS_062 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.yzw, v0 "
	" ";

VS_CRITERIA VS_063_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_3_sw : mova write mask .xzw is allowed";
string VS_063 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.xzw, v0 "
	" ";

VS_CRITERIA VS_064_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_3_sw : mova write mask .xyzw is allowed";
string VS_064 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.xyzw, v0 "
	" ";

VS_CRITERIA VS_065_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_3_sw : mova write mask .yx is NOT allowed";
string VS_065 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.yx, v0 "
	" ";

VS_CRITERIA VS_066_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_3_sw : mova write mask .zx is NOT allowed";
string VS_066 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.zx, v0 "
	" ";

VS_CRITERIA VS_067_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_3_sw : mova write mask .zy is NOT allowed";
string VS_067 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.zy, v0 "
	" ";

VS_CRITERIA VS_068_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_3_sw : mova write mask .wx is NOT allowed";
string VS_068 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.wx, v0 "
	" ";

VS_CRITERIA VS_069_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_3_sw : mova write mask .wz is NOT allowed";
string VS_069 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.wz, v0 "
	" ";

VS_CRITERIA VS_070_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_3_sw : mova write mask .wy is NOT allowed";
string VS_070 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.wy, v0 "
	" ";

VS_CRITERIA VS_071_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_3_sw : mova write mask .zyx is NOT allowed";
string VS_071 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.zyx, v0 "
	" ";

VS_CRITERIA VS_072_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_3_sw : mova write mask .wzy is NOT allowed";
string VS_072 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.wzy, v0 "
	" ";

VS_CRITERIA VS_073_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_3_sw : mova write mask .wzx is NOT allowed";
string VS_073 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.wzx, v0 "
	" ";

VS_CRITERIA VS_074_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_3_sw : mova write mask .wyx is NOT allowed";
string VS_074 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.wyx, v0 "
	" ";

VS_CRITERIA VS_075_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_3_sw : mova write mask .yxw is NOT allowed";
string VS_075 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.yxw, v0 "
	" ";

VS_CRITERIA VS_076_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_3_sw : mova write mask .wzyx is NOT allowed";
string VS_076 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.wzyx, v0 "
	" ";

VS_CRITERIA VS_077_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_3_sw : mova write mask .zxwy is NOT allowed";
string VS_077 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"mova a0.zxwy, v0 "
	" ";

VS_CRITERIA VS_078_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_3_sw : mova is allowed in a 1 level if b0 block";
string VS_078 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"if b0 "
	"	mova a0, v0 "
	"endif ";

VS_CRITERIA VS_079_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_3_sw : mova is allowed in a 8 level if b0 block";
string VS_079 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								mova a0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_080_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_3_sw : mova is allowed in a 16 level if b0 block";
string VS_080 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
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
	"																mova a0, v0 "
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

VS_CRITERIA VS_081_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_3_sw : mova is allowed in a 24 level if b0 block";
string VS_081 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
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
	"																								mova a0, v0 "
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

VS_CRITERIA VS_082_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_3_sw : mova is NOT allowed in a 25 level if b0 block";
string VS_082 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
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
	"																									mova a0, v0 "
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

VS_CRITERIA VS_083_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_3_sw : mova is allowed in a 1 level if b0 nop else block";
string VS_083 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"if b0 nop else "
	"	mova a0, v0 "
	"endif ";

VS_CRITERIA VS_084_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_3_sw : mova is allowed in a 8 level if b0 nop else block";
string VS_084 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								mova a0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_085_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_3_sw : mova is allowed in a 16 level if b0 nop else block";
string VS_085 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
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
	"																mova a0, v0 "
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

VS_CRITERIA VS_086_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_3_sw : mova is allowed in a 24 level if b0 nop else block";
string VS_086 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
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
	"																								mova a0, v0 "
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

VS_CRITERIA VS_087_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_3_sw : mova is NOT allowed in a 25 level if b0 nop else block";
string VS_087 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
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
	"																									mova a0, v0 "
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

VS_CRITERIA VS_088_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_3_sw : mova is allowed in a 1 level loop aL, i0 block";
string VS_088 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	mova a0, v0 "
	"endloop ";

VS_CRITERIA VS_089_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_089_Desc = "vs_3_sw : mova is allowed in a 2 level loop aL, i0 block";
string VS_089 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_090_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_3_sw : mova is allowed in a 3 level loop aL, i0 block";
string VS_090 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			mova a0, v0 "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_091_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_3_sw : mova is allowed in a 4 level loop aL, i0 block";
string VS_091 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				mova a0, v0 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_092_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_3_sw : mova is NOT allowed in a 5 level loop aL, i0 block";
string VS_092 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					mova a0, v0 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_093_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_3_sw : mova is allowed in a 1 level rep i0 block";
string VS_093 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"rep i0 "
	"	mova a0, v0 "
	"endrep ";

VS_CRITERIA VS_094_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_3_sw : mova is allowed in a 2 level rep i0 block";
string VS_094 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_095_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_3_sw : mova is allowed in a 3 level rep i0 block";
string VS_095 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			mova a0, v0 "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_096_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_3_sw : mova is allowed in a 4 level rep i0 block";
string VS_096 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				mova a0, v0 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_097_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_3_sw : mova is NOT allowed in a 5 level rep i0 block";
string VS_097 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					mova a0, v0 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_098_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_3_sw : mova is allowed in a 1 level if_ge c0.x, c0.y block";
string VS_098 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	mova a0, v0 "
	"endif ";

VS_CRITERIA VS_099_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_3_sw : mova is allowed in a 8 level if_ge c0.x, c0.y block";
string VS_099 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		if_ge c0.x, c0.y "
	"			if_ge c0.x, c0.y "
	"				if_ge c0.x, c0.y "
	"					if_ge c0.x, c0.y "
	"						if_ge c0.x, c0.y "
	"							if_ge c0.x, c0.y "
	"								mova a0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_100_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_3_sw : mova is allowed in a 16 level if_ge c0.x, c0.y block";
string VS_100 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																mova a0, v0 "
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

VS_CRITERIA VS_101_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_3_sw : mova is allowed in a 24 level if_ge c0.x, c0.y block";
string VS_101 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																								mova a0, v0 "
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

VS_CRITERIA VS_102_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_3_sw : mova is NOT allowed in a 25 level if_ge c0.x, c0.y block";
string VS_102 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																									mova a0, v0 "
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

VS_CRITERIA VS_103_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_3_sw : mova is allowed in a 1 level if_ge c0.x, c0.y nop else block";
string VS_103 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	mova a0, v0 "
	"endif ";

VS_CRITERIA VS_104_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_3_sw : mova is allowed in a 8 level if_ge c0.x, c0.y nop else block";
string VS_104 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		if_ge c0.x, c0.y nop else "
	"			if_ge c0.x, c0.y nop else "
	"				if_ge c0.x, c0.y nop else "
	"					if_ge c0.x, c0.y nop else "
	"						if_ge c0.x, c0.y nop else "
	"							if_ge c0.x, c0.y nop else "
	"								mova a0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_105_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_3_sw : mova is allowed in a 16 level if_ge c0.x, c0.y nop else block";
string VS_105 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																mova a0, v0 "
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

VS_CRITERIA VS_106_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_3_sw : mova is allowed in a 24 level if_ge c0.x, c0.y nop else block";
string VS_106 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																								mova a0, v0 "
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

VS_CRITERIA VS_107_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_3_sw : mova is NOT allowed in a 25 level if_ge c0.x, c0.y nop else block";
string VS_107 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																									mova a0, v0 "
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

VS_CRITERIA VS_108_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_3_sw : mova is allowed in a 1 level if p0.x block";
string VS_108 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if p0.x "
	"	mova a0, v0 "
	"endif ";

VS_CRITERIA VS_109_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_3_sw : mova is allowed in a 8 level if p0.x block";
string VS_109 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								mova a0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_110_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_3_sw : mova is allowed in a 16 level if p0.x block";
string VS_110 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																mova a0, v0 "
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

VS_CRITERIA VS_111_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_3_sw : mova is allowed in a 24 level if p0.x block";
string VS_111 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																								mova a0, v0 "
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

VS_CRITERIA VS_112_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_3_sw : mova is NOT allowed in a 25 level if p0.x block";
string VS_112 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																									mova a0, v0 "
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

VS_CRITERIA VS_113_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_3_sw : mova is allowed in a 1 level if p0.x nop else block";
string VS_113 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if p0.x nop else "
	"	mova a0, v0 "
	"endif ";

VS_CRITERIA VS_114_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_3_sw : mova is allowed in a 8 level if p0.x nop else block";
string VS_114 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								mova a0, v0 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_115_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_3_sw : mova is allowed in a 16 level if p0.x nop else block";
string VS_115 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																mova a0, v0 "
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

VS_CRITERIA VS_116_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_3_sw : mova is allowed in a 24 level if p0.x nop else block";
string VS_116 = 
	"vs_3_sw "
	"dcl_fog v0 "
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
	"																								mova a0, v0 "
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

VS_CRITERIA VS_117_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_3_sw : mova is NOT allowed in a 25 level if p0.x nop else block";
string VS_117 = 
	"vs_3_sw "
	"dcl_depth v0 "
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
	"																									mova a0, v0 "
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

VS_CRITERIA VS_118_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_3_sw : mova is allowed in a 1 level loop aL, i0 break block";
string VS_118 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	mova a0, v0 "
	"endloop ";

VS_CRITERIA VS_119_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_3_sw : mova is NOT allowed in a 8 level loop aL, i0 break block";
string VS_119 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								mova a0, v0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_120_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_3_sw : mova is NOT allowed in a 16 level loop aL, i0 break block";
string VS_120 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																mova a0, v0 "
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

VS_CRITERIA VS_121_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_3_sw : mova is NOT allowed in a 24 level loop aL, i0 break block";
string VS_121 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																								mova a0, v0 "
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

VS_CRITERIA VS_122_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_3_sw : mova is NOT allowed in a 25 level loop aL, i0 break block";
string VS_122 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																									mova a0, v0 "
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

VS_CRITERIA VS_123_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_3_sw : mova is allowed in a 1 level rep i0 break block";
string VS_123 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 break "
	"	mova a0, v0 "
	"endrep ";

VS_CRITERIA VS_124_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_3_sw : mova is NOT allowed in a 8 level rep i0 break block";
string VS_124 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								mova a0, v0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_125_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_3_sw : mova is NOT allowed in a 16 level rep i0 break block";
string VS_125 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																mova a0, v0 "
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

VS_CRITERIA VS_126_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_3_sw : mova is NOT allowed in a 24 level rep i0 break block";
string VS_126 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																								mova a0, v0 "
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

VS_CRITERIA VS_127_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_3_sw : mova is NOT allowed in a 25 level rep i0 break block";
string VS_127 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																									mova a0, v0 "
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

VS_CRITERIA VS_128_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_3_sw : mova is allowed in a 1 level loop aL, i0 breakp p0.x block";
string VS_128 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	mova a0, v0 "
	"endloop ";

VS_CRITERIA VS_129_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_3_sw : mova is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string VS_129 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								mova a0, v0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_130_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_130_Desc = "vs_3_sw : mova is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string VS_130 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																mova a0, v0 "
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

VS_CRITERIA VS_131_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_3_sw : mova is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string VS_131 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																								mova a0, v0 "
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

VS_CRITERIA VS_132_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_3_sw : mova is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string VS_132 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																									mova a0, v0 "
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

VS_CRITERIA VS_133_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_3_sw : mova is allowed in a 1 level rep i0 breakp p0.x block";
string VS_133 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	mova a0, v0 "
	"endrep ";

VS_CRITERIA VS_134_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_3_sw : mova is NOT allowed in a 8 level rep i0 breakp p0.x block";
string VS_134 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								mova a0, v0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_135_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_3_sw : mova is NOT allowed in a 16 level rep i0 breakp p0.x block";
string VS_135 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																mova a0, v0 "
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

VS_CRITERIA VS_136_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_3_sw : mova is NOT allowed in a 24 level rep i0 breakp p0.x block";
string VS_136 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																								mova a0, v0 "
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

VS_CRITERIA VS_137_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_3_sw : mova is NOT allowed in a 25 level rep i0 breakp p0.x block";
string VS_137 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
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
	"																									mova a0, v0 "
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

VS_CRITERIA VS_138_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_3_sw : mova is allowed in a 1 level loop aL, i0 break_eq c0.x, c0.y block";
string VS_138 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	mova a0, v0 "
	"endloop ";

VS_CRITERIA VS_139_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_3_sw : mova is NOT allowed in a 8 level loop aL, i0 break_eq c0.x, c0.y block";
string VS_139 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								mova a0, v0 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_140_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_3_sw : mova is NOT allowed in a 16 level loop aL, i0 break_eq c0.x, c0.y block";
string VS_140 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								loop aL, i0 break_eq c0.x, c0.y "
	"									loop aL, i0 break_eq c0.x, c0.y "
	"										loop aL, i0 break_eq c0.x, c0.y "
	"											loop aL, i0 break_eq c0.x, c0.y "
	"												loop aL, i0 break_eq c0.x, c0.y "
	"													loop aL, i0 break_eq c0.x, c0.y "
	"														loop aL, i0 break_eq c0.x, c0.y "
	"															loop aL, i0 break_eq c0.x, c0.y "
	"																mova a0, v0 "
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

VS_CRITERIA VS_141_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_3_sw : mova is NOT allowed in a 24 level loop aL, i0 break_eq c0.x, c0.y block";
string VS_141 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								loop aL, i0 break_eq c0.x, c0.y "
	"									loop aL, i0 break_eq c0.x, c0.y "
	"										loop aL, i0 break_eq c0.x, c0.y "
	"											loop aL, i0 break_eq c0.x, c0.y "
	"												loop aL, i0 break_eq c0.x, c0.y "
	"													loop aL, i0 break_eq c0.x, c0.y "
	"														loop aL, i0 break_eq c0.x, c0.y "
	"															loop aL, i0 break_eq c0.x, c0.y "
	"																loop aL, i0 break_eq c0.x, c0.y "
	"																	loop aL, i0 break_eq c0.x, c0.y "
	"																		loop aL, i0 break_eq c0.x, c0.y "
	"																			loop aL, i0 break_eq c0.x, c0.y "
	"																				loop aL, i0 break_eq c0.x, c0.y "
	"																					loop aL, i0 break_eq c0.x, c0.y "
	"																						loop aL, i0 break_eq c0.x, c0.y "
	"																							loop aL, i0 break_eq c0.x, c0.y "
	"																								mova a0, v0 "
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

VS_CRITERIA VS_142_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_3_sw : mova is NOT allowed in a 25 level loop aL, i0 break_eq c0.x, c0.y block";
string VS_142 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		loop aL, i0 break_eq c0.x, c0.y "
	"			loop aL, i0 break_eq c0.x, c0.y "
	"				loop aL, i0 break_eq c0.x, c0.y "
	"					loop aL, i0 break_eq c0.x, c0.y "
	"						loop aL, i0 break_eq c0.x, c0.y "
	"							loop aL, i0 break_eq c0.x, c0.y "
	"								loop aL, i0 break_eq c0.x, c0.y "
	"									loop aL, i0 break_eq c0.x, c0.y "
	"										loop aL, i0 break_eq c0.x, c0.y "
	"											loop aL, i0 break_eq c0.x, c0.y "
	"												loop aL, i0 break_eq c0.x, c0.y "
	"													loop aL, i0 break_eq c0.x, c0.y "
	"														loop aL, i0 break_eq c0.x, c0.y "
	"															loop aL, i0 break_eq c0.x, c0.y "
	"																loop aL, i0 break_eq c0.x, c0.y "
	"																	loop aL, i0 break_eq c0.x, c0.y "
	"																		loop aL, i0 break_eq c0.x, c0.y "
	"																			loop aL, i0 break_eq c0.x, c0.y "
	"																				loop aL, i0 break_eq c0.x, c0.y "
	"																					loop aL, i0 break_eq c0.x, c0.y "
	"																						loop aL, i0 break_eq c0.x, c0.y "
	"																							loop aL, i0 break_eq c0.x, c0.y "
	"																								loop aL, i0 break_eq c0.x, c0.y "
	"																									mova a0, v0 "
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

VS_CRITERIA VS_143_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_3_sw : mova is allowed in a 1 level rep i0 break_eq c0.x, c0.y block";
string VS_143 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	mova a0, v0 "
	"endrep ";

VS_CRITERIA VS_144_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_3_sw : mova is NOT allowed in a 8 level rep i0 break_eq c0.x, c0.y block";
string VS_144 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		rep i0 break_eq c0.x, c0.y "
	"			rep i0 break_eq c0.x, c0.y "
	"				rep i0 break_eq c0.x, c0.y "
	"					rep i0 break_eq c0.x, c0.y "
	"						rep i0 break_eq c0.x, c0.y "
	"							rep i0 break_eq c0.x, c0.y "
	"								mova a0, v0 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_145_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_3_sw : mova is NOT allowed in a 16 level rep i0 break_eq c0.x, c0.y block";
string VS_145 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		rep i0 break_eq c0.x, c0.y "
	"			rep i0 break_eq c0.x, c0.y "
	"				rep i0 break_eq c0.x, c0.y "
	"					rep i0 break_eq c0.x, c0.y "
	"						rep i0 break_eq c0.x, c0.y "
	"							rep i0 break_eq c0.x, c0.y "
	"								rep i0 break_eq c0.x, c0.y "
	"									rep i0 break_eq c0.x, c0.y "
	"										rep i0 break_eq c0.x, c0.y "
	"											rep i0 break_eq c0.x, c0.y "
	"												rep i0 break_eq c0.x, c0.y "
	"													rep i0 break_eq c0.x, c0.y "
	"														rep i0 break_eq c0.x, c0.y "
	"															rep i0 break_eq c0.x, c0.y "
	"																mova a0, v0 "
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

VS_CRITERIA VS_146_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_3_sw : mova is NOT allowed in a 24 level rep i0 break_eq c0.x, c0.y block";
string VS_146 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		rep i0 break_eq c0.x, c0.y "
	"			rep i0 break_eq c0.x, c0.y "
	"				rep i0 break_eq c0.x, c0.y "
	"					rep i0 break_eq c0.x, c0.y "
	"						rep i0 break_eq c0.x, c0.y "
	"							rep i0 break_eq c0.x, c0.y "
	"								rep i0 break_eq c0.x, c0.y "
	"									rep i0 break_eq c0.x, c0.y "
	"										rep i0 break_eq c0.x, c0.y "
	"											rep i0 break_eq c0.x, c0.y "
	"												rep i0 break_eq c0.x, c0.y "
	"													rep i0 break_eq c0.x, c0.y "
	"														rep i0 break_eq c0.x, c0.y "
	"															rep i0 break_eq c0.x, c0.y "
	"																rep i0 break_eq c0.x, c0.y "
	"																	rep i0 break_eq c0.x, c0.y "
	"																		rep i0 break_eq c0.x, c0.y "
	"																			rep i0 break_eq c0.x, c0.y "
	"																				rep i0 break_eq c0.x, c0.y "
	"																					rep i0 break_eq c0.x, c0.y "
	"																						rep i0 break_eq c0.x, c0.y "
	"																							rep i0 break_eq c0.x, c0.y "
	"																								mova a0, v0 "
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

VS_CRITERIA VS_147_Criteria = { false, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_3_sw : mova is NOT allowed in a 25 level rep i0 break_eq c0.x, c0.y block";
string VS_147 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		rep i0 break_eq c0.x, c0.y "
	"			rep i0 break_eq c0.x, c0.y "
	"				rep i0 break_eq c0.x, c0.y "
	"					rep i0 break_eq c0.x, c0.y "
	"						rep i0 break_eq c0.x, c0.y "
	"							rep i0 break_eq c0.x, c0.y "
	"								rep i0 break_eq c0.x, c0.y "
	"									rep i0 break_eq c0.x, c0.y "
	"										rep i0 break_eq c0.x, c0.y "
	"											rep i0 break_eq c0.x, c0.y "
	"												rep i0 break_eq c0.x, c0.y "
	"													rep i0 break_eq c0.x, c0.y "
	"														rep i0 break_eq c0.x, c0.y "
	"															rep i0 break_eq c0.x, c0.y "
	"																rep i0 break_eq c0.x, c0.y "
	"																	rep i0 break_eq c0.x, c0.y "
	"																		rep i0 break_eq c0.x, c0.y "
	"																			rep i0 break_eq c0.x, c0.y "
	"																				rep i0 break_eq c0.x, c0.y "
	"																					rep i0 break_eq c0.x, c0.y "
	"																						rep i0 break_eq c0.x, c0.y "
	"																							rep i0 break_eq c0.x, c0.y "
	"																								rep i0 break_eq c0.x, c0.y "
	"																									mova a0, v0 "
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

VS_CRITERIA VS_148_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_3_sw : mova is allowed in a if b0 and if b0 block";
string VS_148 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_depth v0 "
	"if b0 "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_149_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_149_Desc = "vs_3_sw : mova is allowed in a if b0 and if b0 nop else block";
string VS_149 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_150_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_3_sw : mova is allowed in a if b0 and loop aL, i0 block";
string VS_150 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_151_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_3_sw : mova is allowed in a if b0 and rep i0 block";
string VS_151 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_152_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_3_sw : mova is allowed in a if b0 and if_ge c0.x, c0.y block";
string VS_152 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_153_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_3_sw : mova is allowed in a if b0 and if_ge c0.x, c0.y nop else block";
string VS_153 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_154_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_3_sw : mova is allowed in a if b0 and if p0.x block";
string VS_154 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_155_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_3_sw : mova is allowed in a if b0 and if p0.x nop else block";
string VS_155 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_156_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_156_Desc = "vs_3_sw : mova is allowed in a if b0 and loop aL, i0 break block";
string VS_156 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_157_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_3_sw : mova is allowed in a if b0 and rep i0 break block";
string VS_157 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_158_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_3_sw : mova is allowed in a if b0 and loop aL, i0 breakp p0.x block";
string VS_158 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_159_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_159_Desc = "vs_3_sw : mova is allowed in a if b0 and rep i0 breakp p0.x block";
string VS_159 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_160_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_3_sw : mova is allowed in a if b0 and loop aL, i0 break_eq c0.x, c0.y block";
string VS_160 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_161_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_3_sw : mova is allowed in a if b0 and rep i0 break_eq c0.x, c0.y block";
string VS_161 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_162_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and if b0 block";
string VS_162 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_163_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and if b0 nop else block";
string VS_163 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_164_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and loop aL, i0 block";
string VS_164 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_165_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and rep i0 block";
string VS_165 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_166_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and if_ge c0.x, c0.y block";
string VS_166 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_167_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and if_ge c0.x, c0.y nop else block";
string VS_167 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_168_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_168_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and if p0.x block";
string VS_168 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_169_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_169_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and if p0.x nop else block";
string VS_169 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_170_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_170_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and loop aL, i0 break block";
string VS_170 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_171_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_171_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and rep i0 break block";
string VS_171 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_172_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_172_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string VS_172 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_173_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_173_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string VS_173 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_174_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_174_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and loop aL, i0 break_eq c0.x, c0.y block";
string VS_174 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_175_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_175_Desc = "vs_3_sw : mova is allowed in a if b0 nop else and rep i0 break_eq c0.x, c0.y block";
string VS_175 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_texcoord v0 "
	"if b0 nop else "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_176_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_176_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and if b0 block";
string VS_176 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_177_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_177_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and if b0 nop else block";
string VS_177 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_178_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_178_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and loop aL, i0 block";
string VS_178 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_179_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_179_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and rep i0 block";
string VS_179 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_180_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_180_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and if_ge c0.x, c0.y block";
string VS_180 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_181_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_181_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and if_ge c0.x, c0.y nop else block";
string VS_181 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_182_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_182_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and if p0.x block";
string VS_182 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_183_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_183_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and if p0.x nop else block";
string VS_183 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_184_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_184_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and loop aL, i0 break block";
string VS_184 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_185_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_185_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and rep i0 break block";
string VS_185 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_186_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_186_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string VS_186 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_187_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_187_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string VS_187 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_188_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_188_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and loop aL, i0 break_eq c0.x, c0.y block";
string VS_188 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_189_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_189_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 and rep i0 break_eq c0.x, c0.y block";
string VS_189 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"loop aL, i0 "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_190_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_190_Desc = "vs_3_sw : mova is allowed in a rep i0 and if b0 block";
string VS_190 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_191_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_191_Desc = "vs_3_sw : mova is allowed in a rep i0 and if b0 nop else block";
string VS_191 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_192_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_192_Desc = "vs_3_sw : mova is allowed in a rep i0 and loop aL, i0 block";
string VS_192 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_193_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_193_Desc = "vs_3_sw : mova is allowed in a rep i0 and rep i0 block";
string VS_193 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_194_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_194_Desc = "vs_3_sw : mova is allowed in a rep i0 and if_ge c0.x, c0.y block";
string VS_194 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_195_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_195_Desc = "vs_3_sw : mova is allowed in a rep i0 and if_ge c0.x, c0.y nop else block";
string VS_195 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_196_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_196_Desc = "vs_3_sw : mova is allowed in a rep i0 and if p0.x block";
string VS_196 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_197_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_197_Desc = "vs_3_sw : mova is allowed in a rep i0 and if p0.x nop else block";
string VS_197 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_198_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_198_Desc = "vs_3_sw : mova is allowed in a rep i0 and loop aL, i0 break block";
string VS_198 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_199_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_199_Desc = "vs_3_sw : mova is allowed in a rep i0 and rep i0 break block";
string VS_199 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_200_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_200_Desc = "vs_3_sw : mova is allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string VS_200 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_201_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_201_Desc = "vs_3_sw : mova is allowed in a rep i0 and rep i0 breakp p0.x block";
string VS_201 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_202_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_202_Desc = "vs_3_sw : mova is allowed in a rep i0 and loop aL, i0 break_eq c0.x, c0.y block";
string VS_202 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_203_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_203_Desc = "vs_3_sw : mova is allowed in a rep i0 and rep i0 break_eq c0.x, c0.y block";
string VS_203 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_204_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_204_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and if b0 block";
string VS_204 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_205_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_205_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and if b0 nop else block";
string VS_205 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_206_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_206_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and loop aL, i0 block";
string VS_206 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_207_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_207_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and rep i0 block";
string VS_207 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_208_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_208_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and if_ge c0.x, c0.y block";
string VS_208 = 
	"vs_3_sw "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_209_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_209_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string VS_209 = 
	"vs_3_sw "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_210_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_210_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and if p0.x block";
string VS_210 = 
	"vs_3_sw "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_211_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_211_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and if p0.x nop else block";
string VS_211 = 
	"vs_3_sw "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_212_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_212_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and loop aL, i0 break block";
string VS_212 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_213_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_213_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and rep i0 break block";
string VS_213 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_214_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_214_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_214 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_215_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_215_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and rep i0 breakp p0.x block";
string VS_215 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_216_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_216_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string VS_216 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_217_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_217_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y and rep i0 break_eq c0.x, c0.y block";
string VS_217 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"if_ge c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_218_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_218_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and if b0 block";
string VS_218 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"if_ge c0.x, c0.y nop else "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_219_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_219_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and if b0 nop else block";
string VS_219 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_color v0 "
	"if_ge c0.x, c0.y nop else "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_220_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_220_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 block";
string VS_220 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_221_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_221_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and rep i0 block";
string VS_221 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_222_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_222_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and if_ge c0.x, c0.y block";
string VS_222 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_223_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_223_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and if_ge c0.x, c0.y nop else block";
string VS_223 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_224_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_224_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and if p0.x block";
string VS_224 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_225_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_225_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and if p0.x nop else block";
string VS_225 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_226_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_226_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 break block";
string VS_226 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_227_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_227_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and rep i0 break block";
string VS_227 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_228_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_228_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string VS_228 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_229_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_229_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and rep i0 breakp p0.x block";
string VS_229 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_230_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_230_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and loop aL, i0 break_eq c0.x, c0.y block";
string VS_230 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_231_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_231_Desc = "vs_3_sw : mova is allowed in a if_ge c0.x, c0.y nop else and rep i0 break_eq c0.x, c0.y block";
string VS_231 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if_ge c0.x, c0.y nop else "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_232_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_232_Desc = "vs_3_sw : mova is allowed in a if p0.x and if b0 block";
string VS_232 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"if p0.x "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_233_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_233_Desc = "vs_3_sw : mova is allowed in a if p0.x and if b0 nop else block";
string VS_233 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"if p0.x "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_234_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_234_Desc = "vs_3_sw : mova is allowed in a if p0.x and loop aL, i0 block";
string VS_234 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if p0.x "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_235_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_235_Desc = "vs_3_sw : mova is allowed in a if p0.x and rep i0 block";
string VS_235 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if p0.x "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_236_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_236_Desc = "vs_3_sw : mova is allowed in a if p0.x and if_ge c0.x, c0.y block";
string VS_236 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if p0.x "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_237_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_237_Desc = "vs_3_sw : mova is allowed in a if p0.x and if_ge c0.x, c0.y nop else block";
string VS_237 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_238_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_238_Desc = "vs_3_sw : mova is allowed in a if p0.x and if p0.x block";
string VS_238 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if p0.x "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_239_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_239_Desc = "vs_3_sw : mova is allowed in a if p0.x and if p0.x nop else block";
string VS_239 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if p0.x "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_240_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_240_Desc = "vs_3_sw : mova is allowed in a if p0.x and loop aL, i0 break block";
string VS_240 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if p0.x "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_241_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_241_Desc = "vs_3_sw : mova is allowed in a if p0.x and rep i0 break block";
string VS_241 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if p0.x "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_242_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_242_Desc = "vs_3_sw : mova is allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string VS_242 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_243_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_243_Desc = "vs_3_sw : mova is allowed in a if p0.x and rep i0 breakp p0.x block";
string VS_243 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_244_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_244_Desc = "vs_3_sw : mova is allowed in a if p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string VS_244 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_245_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_245_Desc = "vs_3_sw : mova is allowed in a if p0.x and rep i0 break_eq c0.x, c0.y block";
string VS_245 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_246_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_246_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and if b0 block";
string VS_246 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"if p0.x nop else "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_247_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_247_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and if b0 nop else block";
string VS_247 = 
	"vs_3_sw "
	"defb b0, true "
	"dcl_fog v0 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_248_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_248_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and loop aL, i0 block";
string VS_248 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_249_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_249_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and rep i0 block";
string VS_249 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"if p0.x nop else "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_250_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_250_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and if_ge c0.x, c0.y block";
string VS_250 = 
	"vs_3_sw "
	"dcl_fog v0 "
	"if p0.x nop else "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_251_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_251_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and if_ge c0.x, c0.y nop else block";
string VS_251 = 
	"vs_3_sw "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_252_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_252_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and if p0.x block";
string VS_252 = 
	"vs_3_sw "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_253_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_253_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and if p0.x nop else block";
string VS_253 = 
	"vs_3_sw "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endif ";

VS_CRITERIA VS_254_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_254_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and loop aL, i0 break block";
string VS_254 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_255_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_255_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and rep i0 break block";
string VS_255 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_256_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_256_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string VS_256 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_257_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_257_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string VS_257 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_258_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_258_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and loop aL, i0 break_eq c0.x, c0.y block";
string VS_258 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_259_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_259_Desc = "vs_3_sw : mova is allowed in a if p0.x nop else and rep i0 break_eq c0.x, c0.y block";
string VS_259 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"if p0.x nop else "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_260_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_260_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and if b0 block";
string VS_260 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_261_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_261_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and if b0 nop else block";
string VS_261 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_262_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_262_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and loop aL, i0 block";
string VS_262 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_263_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_263_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and rep i0 block";
string VS_263 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_264_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_264_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and if_ge c0.x, c0.y block";
string VS_264 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_265_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_265_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and if_ge c0.x, c0.y nop else block";
string VS_265 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_266_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_266_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and if p0.x block";
string VS_266 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_267_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_267_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and if p0.x nop else block";
string VS_267 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_268_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_268_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and loop aL, i0 break block";
string VS_268 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_269_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_269_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and rep i0 break block";
string VS_269 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_270_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_270_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string VS_270 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_271_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_271_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string VS_271 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_272_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_272_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and loop aL, i0 break_eq c0.x, c0.y block";
string VS_272 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_273_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_273_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break and rep i0 break_eq c0.x, c0.y block";
string VS_273 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_274_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_274_Desc = "vs_3_sw : mova is allowed in a rep i0 break and if b0 block";
string VS_274 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_275_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_275_Desc = "vs_3_sw : mova is allowed in a rep i0 break and if b0 nop else block";
string VS_275 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_276_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_276_Desc = "vs_3_sw : mova is allowed in a rep i0 break and loop aL, i0 block";
string VS_276 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_277_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_277_Desc = "vs_3_sw : mova is allowed in a rep i0 break and rep i0 block";
string VS_277 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_278_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_278_Desc = "vs_3_sw : mova is allowed in a rep i0 break and if_ge c0.x, c0.y block";
string VS_278 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_279_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_279_Desc = "vs_3_sw : mova is allowed in a rep i0 break and if_ge c0.x, c0.y nop else block";
string VS_279 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_280_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_280_Desc = "vs_3_sw : mova is allowed in a rep i0 break and if p0.x block";
string VS_280 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_281_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_281_Desc = "vs_3_sw : mova is allowed in a rep i0 break and if p0.x nop else block";
string VS_281 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_282_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_282_Desc = "vs_3_sw : mova is allowed in a rep i0 break and loop aL, i0 break block";
string VS_282 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_283_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_283_Desc = "vs_3_sw : mova is allowed in a rep i0 break and rep i0 break block";
string VS_283 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_284_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_284_Desc = "vs_3_sw : mova is allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string VS_284 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_285_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_285_Desc = "vs_3_sw : mova is allowed in a rep i0 break and rep i0 breakp p0.x block";
string VS_285 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_286_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_286_Desc = "vs_3_sw : mova is allowed in a rep i0 break and loop aL, i0 break_eq c0.x, c0.y block";
string VS_286 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_287_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_287_Desc = "vs_3_sw : mova is allowed in a rep i0 break and rep i0 break_eq c0.x, c0.y block";
string VS_287 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_288_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_288_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and if b0 block";
string VS_288 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_289_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_289_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string VS_289 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_290_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_290_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string VS_290 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_291_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_291_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string VS_291 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_292_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_292_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and if_ge c0.x, c0.y block";
string VS_292 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 breakp p0.x "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_293_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_293_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and if_ge c0.x, c0.y nop else block";
string VS_293 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 breakp p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_294_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_294_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string VS_294 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_295_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_295_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string VS_295 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_296_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_296_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string VS_296 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_297_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_297_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string VS_297 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_298_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_298_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string VS_298 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_299_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_299_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string VS_299 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_300_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_300_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string VS_300 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_301_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_301_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 breakp p0.x and rep i0 break_eq c0.x, c0.y block";
string VS_301 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_302_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_302_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and if b0 block";
string VS_302 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_303_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_303_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string VS_303 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_304_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_304_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string VS_304 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_305_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_305_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and rep i0 block";
string VS_305 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_306_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_306_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and if_ge c0.x, c0.y block";
string VS_306 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_307_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_307_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and if_ge c0.x, c0.y nop else block";
string VS_307 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_308_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_308_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and if p0.x block";
string VS_308 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_309_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_309_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string VS_309 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_310_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_310_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string VS_310 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_311_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_311_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and rep i0 break block";
string VS_311 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_312_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_312_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string VS_312 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_313_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_313_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string VS_313 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_314_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_314_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string VS_314 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_315_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_315_Desc = "vs_3_sw : mova is allowed in a rep i0 breakp p0.x and rep i0 break_eq c0.x, c0.y block";
string VS_315 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_316_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_316_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 block";
string VS_316 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_317_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_317_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 nop else block";
string VS_317 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_318_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_318_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 block";
string VS_318 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_319_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_319_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 block";
string VS_319 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_320_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_320_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and if_ge c0.x, c0.y block";
string VS_320 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_321_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_321_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string VS_321 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_322_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_322_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x block";
string VS_322 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_323_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_323_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x nop else block";
string VS_323 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_324_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_324_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 break block";
string VS_324 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_325_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_325_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 break block";
string VS_325 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_326_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_326_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_326 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_327_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_327_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 breakp p0.x block";
string VS_327 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_328_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_328_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string VS_328 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_329_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_329_Desc = "vs_3_sw : mova is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 break_eq c0.x, c0.y block";
string VS_329 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_330_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_330_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and if b0 block";
string VS_330 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if b0 "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_331_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_331_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and if b0 nop else block";
string VS_331 = 
	"vs_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if b0 nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_332_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_332_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 block";
string VS_332 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_333_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_333_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 block";
string VS_333 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_334_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_334_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and if_ge c0.x, c0.y block";
string VS_334 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_335_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_335_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string VS_335 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_336_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_336_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and if p0.x block";
string VS_336 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if p0.x "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_337_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_337_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and if p0.x nop else block";
string VS_337 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	if p0.x nop else "
	"		mova a0, v0 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_338_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_338_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 break block";
string VS_338 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_339_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_339_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 break block";
string VS_339 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_340_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_340_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_340 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_341_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_341_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 breakp p0.x block";
string VS_341 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_342_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_342_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and loop aL, i0 break_eq c0.x, c0.y block";
string VS_342 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_343_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_343_Desc = "vs_3_sw : mova is allowed in a rep i0 break_eq c0.x, c0.y and rep i0 break_eq c0.x, c0.y block";
string VS_343 = 
	"vs_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"rep i0 break_eq c0.x, c0.y "
	"	rep i0 break_eq c0.x, c0.y "
	"		mova a0, v0 "
	"	endrep "
	"endrep ";


