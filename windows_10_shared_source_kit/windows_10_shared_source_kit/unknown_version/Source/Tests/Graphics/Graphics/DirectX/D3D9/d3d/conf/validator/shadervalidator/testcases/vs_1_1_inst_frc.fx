#include "Test_Include.fx"

int Test_Count = 54;

string TestInfo
<
	string TestType = "VS";
>
= "vs_1_1_inst_frc";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_1_1 : frc source reg combination v0 is allowed";
string VS_001 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.y, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_002_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_1_1 : frc source reg combination r0 is allowed";
string VS_002 = 
	"vs_1_1 "
	"mov r0, c0 "
	"frc r0.y, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_1_1 : frc source reg combination c0 is allowed";
string VS_003 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_1_1 : frc source reg combination -c0 is allowed";
string VS_004 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, -c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_1_1 : frc source reg combination c0 is allowed";
string VS_005 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_006_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_1_1 : frc source reg combination c0.x is allowed";
string VS_006 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_007_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_1_1 : frc source reg combination c0.y is allowed";
string VS_007 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.y "
	"mov oPos, c0 ";

VS_CRITERIA VS_008_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_1_1 : frc source reg combination c0.z is allowed";
string VS_008 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.z "
	"mov oPos, c0 ";

VS_CRITERIA VS_009_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_1_1 : frc source reg combination c0.w is allowed";
string VS_009 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.w "
	"mov oPos, c0 ";

VS_CRITERIA VS_010_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_1_1 : frc source reg combination c0.yzxw is allowed";
string VS_010 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.yzxw "
	"mov oPos, c0 ";

VS_CRITERIA VS_011_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_1_1 : frc source reg combination c0.zxyw is allowed";
string VS_011 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.zxyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_012_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_1_1 : frc source reg combination c0.wzyx is allowed";
string VS_012 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.wzyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_013_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_1_1 : frc source reg combination c0.wyxz is allowed";
string VS_013 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.wyxz "
	"mov oPos, c0 ";

VS_CRITERIA VS_014_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_1_1 : frc source reg combination c0.xzyw is allowed";
string VS_014 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.xzyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_015_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_1_1 : frc source reg combination c0.xywz is allowed";
string VS_015 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.xywz "
	"mov oPos, c0 ";

VS_CRITERIA VS_016_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_1_1 : frc source reg combination c0.zyx is allowed";
string VS_016 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.zyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_017_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_1_1 : frc source reg combination c0.xzy is allowed";
string VS_017 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.xzy "
	"mov oPos, c0 ";

VS_CRITERIA VS_018_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_1_1 : frc source reg combination c0.ywx is allowed";
string VS_018 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.ywx "
	"mov oPos, c0 ";

VS_CRITERIA VS_019_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_1_1 : frc source reg combination c0.yx is allowed";
string VS_019 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.yx "
	"mov oPos, c0 ";

VS_CRITERIA VS_020_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_1_1 : frc source reg combination c0.wz is allowed";
string VS_020 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.wz "
	"mov oPos, c0 ";

VS_CRITERIA VS_021_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_1_1 : frc source reg combination c0.zy is allowed";
string VS_021 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"frc r0.y, c0.zy "
	"mov oPos, c0 ";

VS_CRITERIA VS_022_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_1_1 : frc source reg combination a0 is NOT allowed";
string VS_022 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"frc r0.y, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_023_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_1_1 : frc source reg combination oPos is NOT allowed";
string VS_023 = 
	"vs_1_1 "
	"frc r0.y, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_024_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_1_1 : frc source reg combination oFog is NOT allowed";
string VS_024 = 
	"vs_1_1 "
	"frc r0.y, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_025_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_1_1 : frc source reg combination oPts is NOT allowed";
string VS_025 = 
	"vs_1_1 "
	"texcoord ts "
	"frc r0.y, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_026_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_1_1 : frc source reg combination oD0 is NOT allowed";
string VS_026 = 
	"vs_1_1 "
	"frc r0.y, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_027_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_1_1 : frc source reg combination oT0 is NOT allowed";
string VS_027 = 
	"vs_1_1 "
	"frc r0.y, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_028_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_1_1 : frc write mask .y is allowed";
string VS_028 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.y, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_029_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_1_1 : frc write mask .xy is allowed";
string VS_029 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.xy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_030_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_1_1 : frc write mask .x is NOT allowed";
string VS_030 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.x, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_031_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_1_1 : frc write mask .z is NOT allowed";
string VS_031 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.z, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_032_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_1_1 : frc write mask .w is NOT allowed";
string VS_032 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.w, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_033_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_1_1 : frc write mask .yz is NOT allowed";
string VS_033 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.yz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_034_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_1_1 : frc write mask .zw is NOT allowed";
string VS_034 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.zw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_035_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_1_1 : frc write mask .xz is NOT allowed";
string VS_035 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.xz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_036_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_1_1 : frc write mask .xw is NOT allowed";
string VS_036 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.xw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_037_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_1_1 : frc write mask .yw is NOT allowed";
string VS_037 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.yw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_038_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_1_1 : frc write mask .xyz is NOT allowed";
string VS_038 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.xyz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_039_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_1_1 : frc write mask .yzw is NOT allowed";
string VS_039 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.yzw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_040_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_1_1 : frc write mask .xzw is NOT allowed";
string VS_040 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.xzw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_041_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_1_1 : frc write mask .xyzw is NOT allowed";
string VS_041 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.xyzw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_042_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_1_1 : frc write mask .yx is NOT allowed";
string VS_042 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.yx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_043_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_1_1 : frc write mask .zx is NOT allowed";
string VS_043 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.zx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_044_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_1_1 : frc write mask .zy is NOT allowed";
string VS_044 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.zy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_045_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_1_1 : frc write mask .wx is NOT allowed";
string VS_045 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.wx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_046_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_1_1 : frc write mask .wz is NOT allowed";
string VS_046 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.wz, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_047_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_1_1 : frc write mask .wy is NOT allowed";
string VS_047 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.wy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_048_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_1_1 : frc write mask .zyx is NOT allowed";
string VS_048 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.zyx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_049_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_1_1 : frc write mask .wzy is NOT allowed";
string VS_049 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.wzy, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_050_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_1_1 : frc write mask .wzx is NOT allowed";
string VS_050 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.wzx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_051_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_1_1 : frc write mask .wyx is NOT allowed";
string VS_051 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.wyx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_052_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_1_1 : frc write mask .yxw is NOT allowed";
string VS_052 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.yxw, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_053_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_1_1 : frc write mask .wzyx is NOT allowed";
string VS_053 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.wzyx, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_054_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_1_1 : frc write mask .zxwy is NOT allowed";
string VS_054 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"frc r0.zxwy, v0 "
	"mov oPos, c0 ";


