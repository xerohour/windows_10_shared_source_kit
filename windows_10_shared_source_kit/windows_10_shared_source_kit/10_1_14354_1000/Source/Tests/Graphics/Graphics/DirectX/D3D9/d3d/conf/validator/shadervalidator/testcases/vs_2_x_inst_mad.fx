#include "Test_Include.fx"

int Test_Count = 1388;

string TestInfo
<
	string TestType = "VS";
>
= "vs_2_x_inst_mad";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_2_x : mad source reg combination v0, r0, r1 is allowed";
string VS_001 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_002_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_2_x : mad source reg combination v0, r0, c0 is allowed";
string VS_002 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_2_x : mad source reg combination v0, c0, r0 is allowed";
string VS_003 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, c0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_2_x : mad source reg combination r0, v0, r1 is allowed";
string VS_004 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, v0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_2_x : mad source reg combination r0, v0, c0 is allowed";
string VS_005 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, r0, v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_006_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_2_x : mad source reg combination r0, r1, v0 is allowed";
string VS_006 = 
	"vs_2_x "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, r1, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_007_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_2_x : mad source reg combination r0, r1, r2 is allowed";
string VS_007 = 
	"vs_2_x "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"mad r0, r0, r1, r2 "
	"mov oPos, c0 ";

VS_CRITERIA VS_008_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_2_x : mad source reg combination r0, r1, c0 is allowed";
string VS_008 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, r1, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_009_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_2_x : mad source reg combination r0, c0, v0 is allowed";
string VS_009 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, r0, c0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_010_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_2_x : mad source reg combination r0, c0, r1 is allowed";
string VS_010 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, c0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_011_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_2_x : mad source reg combination c0, v0, r0 is allowed";
string VS_011 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, c0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_012_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_2_x : mad source reg combination c0, r0, v0 is allowed";
string VS_012 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, c0, r0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_013_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_2_x : mad source reg combination c0, r0, r1 is allowed";
string VS_013 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_014_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_2_x : mad source reg combination -v0, -r0, -c0 is allowed";
string VS_014 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, -v0, -r0, -c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_015_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_2_x : mad source reg combination -v0, -r0, c0 is allowed";
string VS_015 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, -v0, -r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_016_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_2_x : mad source reg combination -v0, r0, -c0 is allowed";
string VS_016 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, -v0, r0, -c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_017_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_2_x : mad source reg combination -v0, r0, c0 is allowed";
string VS_017 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, -v0, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_018_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_2_x : mad source reg combination v0, -r0, -c0 is allowed";
string VS_018 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, -r0, -c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_019_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_2_x : mad source reg combination v0, -r0, c0 is allowed";
string VS_019 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, -r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_020_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_2_x : mad source reg combination v0, r0, -c0 is allowed";
string VS_020 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, -c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_021_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_2_x : mad source reg combination v0, r0, c0 is allowed";
string VS_021 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_022_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_2_x : mad source reg combination v0.x, r0, c0 is allowed";
string VS_022 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.x, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_023_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_2_x : mad source reg combination v0.y, r0, c0 is allowed";
string VS_023 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.y, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_024_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_2_x : mad source reg combination v0.z, r0, c0 is allowed";
string VS_024 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.z, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_025_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_2_x : mad source reg combination v0.w, r0, c0 is allowed";
string VS_025 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.w, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_026_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_2_x : mad source reg combination v0.yzxw, r0, c0 is allowed";
string VS_026 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.yzxw, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_027_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_2_x : mad source reg combination v0.zxyw, r0, c0 is allowed";
string VS_027 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.zxyw, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_028_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_2_x : mad source reg combination v0.wzyx, r0, c0 is allowed";
string VS_028 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.wzyx, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_029_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_2_x : mad source reg combination v0.wyxz, r0, c0 is allowed";
string VS_029 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.wyxz, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_030_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_2_x : mad source reg combination v0.xzyw, r0, c0 is allowed";
string VS_030 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.xzyw, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_031_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_2_x : mad source reg combination v0.xywz, r0, c0 is allowed";
string VS_031 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.xywz, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_032_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_2_x : mad source reg combination v0.zyx, r0, c0 is allowed";
string VS_032 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.zyx, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_033_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_2_x : mad source reg combination v0.xzy, r0, c0 is allowed";
string VS_033 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.xzy, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_034_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_2_x : mad source reg combination v0.ywx, r0, c0 is allowed";
string VS_034 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.ywx, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_035_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_2_x : mad source reg combination v0.yx, r0, c0 is allowed";
string VS_035 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.yx, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_036_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_2_x : mad source reg combination v0.wz, r0, c0 is allowed";
string VS_036 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.wz, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_037_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_2_x : mad source reg combination v0.zy, r0, c0 is allowed";
string VS_037 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0.zy, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_038_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_2_x : mad source reg combination v0, r0.x, c0 is allowed";
string VS_038 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.x, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_039_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_2_x : mad source reg combination v0, r0.y, c0 is allowed";
string VS_039 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.y, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_040_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_2_x : mad source reg combination v0, r0.z, c0 is allowed";
string VS_040 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.z, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_041_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_2_x : mad source reg combination v0, r0.w, c0 is allowed";
string VS_041 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.w, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_042_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_2_x : mad source reg combination v0, r0.yzxw, c0 is allowed";
string VS_042 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.yzxw, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_043_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_2_x : mad source reg combination v0, r0.zxyw, c0 is allowed";
string VS_043 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.zxyw, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_044_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_2_x : mad source reg combination v0, r0.wzyx, c0 is allowed";
string VS_044 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.wzyx, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_045_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_2_x : mad source reg combination v0, r0.wyxz, c0 is allowed";
string VS_045 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.wyxz, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_046_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_2_x : mad source reg combination v0, r0.xzyw, c0 is allowed";
string VS_046 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.xzyw, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_047_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_2_x : mad source reg combination v0, r0.xywz, c0 is allowed";
string VS_047 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.xywz, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_048_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_2_x : mad source reg combination v0, r0.zyx, c0 is allowed";
string VS_048 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.zyx, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_049_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_2_x : mad source reg combination v0, r0.xzy, c0 is allowed";
string VS_049 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.xzy, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_050_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_2_x : mad source reg combination v0, r0.ywx, c0 is allowed";
string VS_050 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.ywx, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_051_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_2_x : mad source reg combination v0, r0.yx, c0 is allowed";
string VS_051 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.yx, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_052_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_2_x : mad source reg combination v0, r0.wz, c0 is allowed";
string VS_052 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.wz, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_053_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_2_x : mad source reg combination v0, r0.zy, c0 is allowed";
string VS_053 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0.zy, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_054_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_2_x : mad source reg combination v0, r0, c0.x is allowed";
string VS_054 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_055_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_2_x : mad source reg combination v0, r0, c0.y is allowed";
string VS_055 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.y "
	"mov oPos, c0 ";

VS_CRITERIA VS_056_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_2_x : mad source reg combination v0, r0, c0.z is allowed";
string VS_056 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.z "
	"mov oPos, c0 ";

VS_CRITERIA VS_057_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_2_x : mad source reg combination v0, r0, c0.w is allowed";
string VS_057 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.w "
	"mov oPos, c0 ";

VS_CRITERIA VS_058_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_2_x : mad source reg combination v0, r0, c0.yzxw is allowed";
string VS_058 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.yzxw "
	"mov oPos, c0 ";

VS_CRITERIA VS_059_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_2_x : mad source reg combination v0, r0, c0.zxyw is allowed";
string VS_059 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.zxyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_060_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_2_x : mad source reg combination v0, r0, c0.wzyx is allowed";
string VS_060 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.wzyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_061_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_2_x : mad source reg combination v0, r0, c0.wyxz is allowed";
string VS_061 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.wyxz "
	"mov oPos, c0 ";

VS_CRITERIA VS_062_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_2_x : mad source reg combination v0, r0, c0.xzyw is allowed";
string VS_062 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.xzyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_063_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_2_x : mad source reg combination v0, r0, c0.xywz is allowed";
string VS_063 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.xywz "
	"mov oPos, c0 ";

VS_CRITERIA VS_064_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_2_x : mad source reg combination v0, r0, c0.zyx is allowed";
string VS_064 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.zyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_065_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_2_x : mad source reg combination v0, r0, c0.xzy is allowed";
string VS_065 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.xzy "
	"mov oPos, c0 ";

VS_CRITERIA VS_066_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_2_x : mad source reg combination v0, r0, c0.ywx is allowed";
string VS_066 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.ywx "
	"mov oPos, c0 ";

VS_CRITERIA VS_067_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_2_x : mad source reg combination v0, r0, c0.yx is allowed";
string VS_067 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.yx "
	"mov oPos, c0 ";

VS_CRITERIA VS_068_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_2_x : mad source reg combination v0, r0, c0.wz is allowed";
string VS_068 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.wz "
	"mov oPos, c0 ";

VS_CRITERIA VS_069_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_2_x : mad source reg combination v0, r0, c0.zy is allowed";
string VS_069 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0.zy "
	"mov oPos, c0 ";

VS_CRITERIA VS_070_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_2_x : mad source reg combination v0, v1, v2 is NOT allowed";
string VS_070 = 
	"vs_2_x "
	"dcl_color v0 "
	"dcl_depth v1 "
	"dcl_texcoord v2 "
	"mad r0, v0, v1, v2 "
	"mov oPos, c0 ";

VS_CRITERIA VS_071_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_2_x : mad source reg combination v0, v1, r0 is NOT allowed";
string VS_071 = 
	"vs_2_x "
	"dcl_color v0 "
	"dcl_depth v1 "
	"mov r0, c0 "
	"mad r0, v0, v1, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_072_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_2_x : mad source reg combination v0, v1, c0 is NOT allowed";
string VS_072 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"mad r0, v0, v1, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_073_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_2_x : mad source reg combination v0, r0, v1 is NOT allowed";
string VS_073 = 
	"vs_2_x "
	"dcl_color v0 "
	"dcl_depth v1 "
	"mov r0, c0 "
	"mad r0, v0, r0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_074_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_2_x : mad source reg combination v0, c0, v1 is NOT allowed";
string VS_074 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"mad r0, v0, c0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_2_x : mad source reg combination v0, c0, c1 is NOT allowed";
string VS_075 = 
	"vs_2_x "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mad r0, v0, c0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_2_x : mad source reg combination r0, v0, v1 is NOT allowed";
string VS_076 = 
	"vs_2_x "
	"dcl_color v0 "
	"dcl_depth v1 "
	"mov r0, c0 "
	"mad r0, r0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_2_x : mad source reg combination r0, c0, c1 is NOT allowed";
string VS_077 = 
	"vs_2_x "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, r0, c0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_078_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_2_x : mad source reg combination c0, v0, v1 is NOT allowed";
string VS_078 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"dcl_depth v1 "
	"mad r0, c0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_2_x : mad source reg combination c0, v0, c1 is NOT allowed";
string VS_079 = 
	"vs_2_x "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mad r0, c0, v0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_2_x : mad source reg combination c0, r0, c1 is NOT allowed";
string VS_080 = 
	"vs_2_x "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, r0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_2_x : mad source reg combination c0, c1, v0 is NOT allowed";
string VS_081 = 
	"vs_2_x "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"mad r0, c0, c1, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_2_x : mad source reg combination c0, c1, r0 is NOT allowed";
string VS_082 = 
	"vs_2_x "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_083_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_2_x : mad source reg combination c0, c0, c0 is NOT allowed";
string VS_083 = 
	"vs_2_x "
	"def c0, 1, 1, 1, 1 "
	"mad r0, c0, c0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_084_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_2_x : mad source reg combination c0, c1, c2 is NOT allowed";
string VS_084 = 
	"vs_2_x "
	"def c2, 1, 1, 1, 1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mad r0, c0, c1, c2 "
	"mov oPos, c0 ";

VS_CRITERIA VS_085_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_2_x : mad source reg combination a0, a0, a0 is NOT allowed";
string VS_085 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_086_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_2_x : mad source reg combination a0, a0, b0 is NOT allowed";
string VS_086 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_087_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_2_x : mad source reg combination a0, a0, i0 is NOT allowed";
string VS_087 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_088_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_2_x : mad source reg combination a0, a0, aL is NOT allowed";
string VS_088 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_089_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_089_Desc = "vs_2_x : mad source reg combination a0, a0, p0 is NOT allowed";
string VS_089 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_090_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_2_x : mad source reg combination a0, a0, oPos is NOT allowed";
string VS_090 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_091_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_2_x : mad source reg combination a0, a0, oFog is NOT allowed";
string VS_091 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_092_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_2_x : mad source reg combination a0, a0, oPts is NOT allowed";
string VS_092 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_093_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_2_x : mad source reg combination a0, a0, oD0 is NOT allowed";
string VS_093 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_094_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_2_x : mad source reg combination a0, a0, oT0 is NOT allowed";
string VS_094 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_095_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_2_x : mad source reg combination a0, b0, a0 is NOT allowed";
string VS_095 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_096_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_2_x : mad source reg combination a0, b0, b0 is NOT allowed";
string VS_096 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_097_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_2_x : mad source reg combination a0, b0, i0 is NOT allowed";
string VS_097 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_098_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_2_x : mad source reg combination a0, b0, aL is NOT allowed";
string VS_098 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_099_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_099_Desc = "vs_2_x : mad source reg combination a0, b0, p0 is NOT allowed";
string VS_099 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_100_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_2_x : mad source reg combination a0, b0, oPos is NOT allowed";
string VS_100 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_101_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_2_x : mad source reg combination a0, b0, oFog is NOT allowed";
string VS_101 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_102_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_2_x : mad source reg combination a0, b0, oPts is NOT allowed";
string VS_102 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_103_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_2_x : mad source reg combination a0, b0, oD0 is NOT allowed";
string VS_103 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_104_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_2_x : mad source reg combination a0, b0, oT0 is NOT allowed";
string VS_104 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_105_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_2_x : mad source reg combination a0, i0, a0 is NOT allowed";
string VS_105 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_106_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_2_x : mad source reg combination a0, i0, b0 is NOT allowed";
string VS_106 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_107_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_2_x : mad source reg combination a0, i0, i0 is NOT allowed";
string VS_107 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_108_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_2_x : mad source reg combination a0, i0, aL is NOT allowed";
string VS_108 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_109_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_109_Desc = "vs_2_x : mad source reg combination a0, i0, p0 is NOT allowed";
string VS_109 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_110_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_2_x : mad source reg combination a0, i0, oPos is NOT allowed";
string VS_110 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_111_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_2_x : mad source reg combination a0, i0, oFog is NOT allowed";
string VS_111 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_112_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_2_x : mad source reg combination a0, i0, oPts is NOT allowed";
string VS_112 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_113_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_2_x : mad source reg combination a0, i0, oD0 is NOT allowed";
string VS_113 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_114_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_2_x : mad source reg combination a0, i0, oT0 is NOT allowed";
string VS_114 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_115_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_2_x : mad source reg combination a0, aL, a0 is NOT allowed";
string VS_115 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_116_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_2_x : mad source reg combination a0, aL, b0 is NOT allowed";
string VS_116 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_117_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_2_x : mad source reg combination a0, aL, i0 is NOT allowed";
string VS_117 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_118_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_2_x : mad source reg combination a0, aL, aL is NOT allowed";
string VS_118 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_119_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_119_Desc = "vs_2_x : mad source reg combination a0, aL, p0 is NOT allowed";
string VS_119 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_120_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_2_x : mad source reg combination a0, aL, oPos is NOT allowed";
string VS_120 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_121_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_2_x : mad source reg combination a0, aL, oFog is NOT allowed";
string VS_121 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_122_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_2_x : mad source reg combination a0, aL, oPts is NOT allowed";
string VS_122 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_123_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_2_x : mad source reg combination a0, aL, oD0 is NOT allowed";
string VS_123 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_124_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_2_x : mad source reg combination a0, aL, oT0 is NOT allowed";
string VS_124 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_125_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_125_Desc = "vs_2_x : mad source reg combination a0, p0, a0 is NOT allowed";
string VS_125 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_126_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_126_Desc = "vs_2_x : mad source reg combination a0, p0, b0 is NOT allowed";
string VS_126 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_127_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_127_Desc = "vs_2_x : mad source reg combination a0, p0, i0 is NOT allowed";
string VS_127 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_128_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_128_Desc = "vs_2_x : mad source reg combination a0, p0, aL is NOT allowed";
string VS_128 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_129_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_129_Desc = "vs_2_x : mad source reg combination a0, p0, p0 is NOT allowed";
string VS_129 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_130_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_130_Desc = "vs_2_x : mad source reg combination a0, p0, oPos is NOT allowed";
string VS_130 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_131_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_131_Desc = "vs_2_x : mad source reg combination a0, p0, oFog is NOT allowed";
string VS_131 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_132_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_132_Desc = "vs_2_x : mad source reg combination a0, p0, oPts is NOT allowed";
string VS_132 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_133_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_133_Desc = "vs_2_x : mad source reg combination a0, p0, oD0 is NOT allowed";
string VS_133 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_134_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_134_Desc = "vs_2_x : mad source reg combination a0, p0, oT0 is NOT allowed";
string VS_134 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_135_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_2_x : mad source reg combination a0, oPos, a0 is NOT allowed";
string VS_135 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_136_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_2_x : mad source reg combination a0, oPos, b0 is NOT allowed";
string VS_136 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_137_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_2_x : mad source reg combination a0, oPos, i0 is NOT allowed";
string VS_137 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_138_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_2_x : mad source reg combination a0, oPos, aL is NOT allowed";
string VS_138 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_139_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_139_Desc = "vs_2_x : mad source reg combination a0, oPos, p0 is NOT allowed";
string VS_139 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_140_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_2_x : mad source reg combination a0, oPos, oPos is NOT allowed";
string VS_140 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_141_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_2_x : mad source reg combination a0, oPos, oFog is NOT allowed";
string VS_141 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_142_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_2_x : mad source reg combination a0, oPos, oPts is NOT allowed";
string VS_142 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_143_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_2_x : mad source reg combination a0, oPos, oD0 is NOT allowed";
string VS_143 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_144_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_2_x : mad source reg combination a0, oPos, oT0 is NOT allowed";
string VS_144 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_145_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_2_x : mad source reg combination a0, oFog, a0 is NOT allowed";
string VS_145 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_146_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_2_x : mad source reg combination a0, oFog, b0 is NOT allowed";
string VS_146 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_147_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_2_x : mad source reg combination a0, oFog, i0 is NOT allowed";
string VS_147 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_148_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_2_x : mad source reg combination a0, oFog, aL is NOT allowed";
string VS_148 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_149_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_149_Desc = "vs_2_x : mad source reg combination a0, oFog, p0 is NOT allowed";
string VS_149 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_150_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_2_x : mad source reg combination a0, oFog, oPos is NOT allowed";
string VS_150 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_151_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_2_x : mad source reg combination a0, oFog, oFog is NOT allowed";
string VS_151 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_152_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_2_x : mad source reg combination a0, oFog, oPts is NOT allowed";
string VS_152 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_153_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_2_x : mad source reg combination a0, oFog, oD0 is NOT allowed";
string VS_153 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_154_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_2_x : mad source reg combination a0, oFog, oT0 is NOT allowed";
string VS_154 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_155_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_2_x : mad source reg combination a0, oPts, a0 is NOT allowed";
string VS_155 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_156_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_156_Desc = "vs_2_x : mad source reg combination a0, oPts, b0 is NOT allowed";
string VS_156 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_157_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_2_x : mad source reg combination a0, oPts, i0 is NOT allowed";
string VS_157 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_158_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_2_x : mad source reg combination a0, oPts, aL is NOT allowed";
string VS_158 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_159_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_159_Desc = "vs_2_x : mad source reg combination a0, oPts, p0 is NOT allowed";
string VS_159 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_160_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_2_x : mad source reg combination a0, oPts, oPos is NOT allowed";
string VS_160 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_161_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_2_x : mad source reg combination a0, oPts, oFog is NOT allowed";
string VS_161 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_162_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_2_x : mad source reg combination a0, oPts, oPts is NOT allowed";
string VS_162 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_163_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_2_x : mad source reg combination a0, oPts, oD0 is NOT allowed";
string VS_163 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_164_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_2_x : mad source reg combination a0, oPts, oT0 is NOT allowed";
string VS_164 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_165_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_2_x : mad source reg combination a0, oD0, a0 is NOT allowed";
string VS_165 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_166_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_2_x : mad source reg combination a0, oD0, b0 is NOT allowed";
string VS_166 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_167_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_2_x : mad source reg combination a0, oD0, i0 is NOT allowed";
string VS_167 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_168_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_168_Desc = "vs_2_x : mad source reg combination a0, oD0, aL is NOT allowed";
string VS_168 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_169_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_169_Desc = "vs_2_x : mad source reg combination a0, oD0, p0 is NOT allowed";
string VS_169 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_170_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_170_Desc = "vs_2_x : mad source reg combination a0, oD0, oPos is NOT allowed";
string VS_170 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_171_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_171_Desc = "vs_2_x : mad source reg combination a0, oD0, oFog is NOT allowed";
string VS_171 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_172_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_172_Desc = "vs_2_x : mad source reg combination a0, oD0, oPts is NOT allowed";
string VS_172 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_173_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_173_Desc = "vs_2_x : mad source reg combination a0, oD0, oD0 is NOT allowed";
string VS_173 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_174_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_174_Desc = "vs_2_x : mad source reg combination a0, oD0, oT0 is NOT allowed";
string VS_174 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_175_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_175_Desc = "vs_2_x : mad source reg combination a0, oT0, a0 is NOT allowed";
string VS_175 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_176_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_176_Desc = "vs_2_x : mad source reg combination a0, oT0, b0 is NOT allowed";
string VS_176 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, a0, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_177_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_177_Desc = "vs_2_x : mad source reg combination a0, oT0, i0 is NOT allowed";
string VS_177 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, a0, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_178_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_178_Desc = "vs_2_x : mad source reg combination a0, oT0, aL is NOT allowed";
string VS_178 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, a0, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_179_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_179_Desc = "vs_2_x : mad source reg combination a0, oT0, p0 is NOT allowed";
string VS_179 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_180_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_180_Desc = "vs_2_x : mad source reg combination a0, oT0, oPos is NOT allowed";
string VS_180 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_181_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_181_Desc = "vs_2_x : mad source reg combination a0, oT0, oFog is NOT allowed";
string VS_181 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_182_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_182_Desc = "vs_2_x : mad source reg combination a0, oT0, oPts is NOT allowed";
string VS_182 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, a0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_183_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_183_Desc = "vs_2_x : mad source reg combination a0, oT0, oD0 is NOT allowed";
string VS_183 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_184_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_184_Desc = "vs_2_x : mad source reg combination a0, oT0, oT0 is NOT allowed";
string VS_184 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, a0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_185_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_185_Desc = "vs_2_x : mad source reg combination b0, a0, a0 is NOT allowed";
string VS_185 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_186_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_186_Desc = "vs_2_x : mad source reg combination b0, a0, b0 is NOT allowed";
string VS_186 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_187_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_187_Desc = "vs_2_x : mad source reg combination b0, a0, i0 is NOT allowed";
string VS_187 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_188_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_188_Desc = "vs_2_x : mad source reg combination b0, a0, aL is NOT allowed";
string VS_188 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, b0, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_189_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_189_Desc = "vs_2_x : mad source reg combination b0, a0, p0 is NOT allowed";
string VS_189 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_190_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_190_Desc = "vs_2_x : mad source reg combination b0, a0, oPos is NOT allowed";
string VS_190 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_191_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_191_Desc = "vs_2_x : mad source reg combination b0, a0, oFog is NOT allowed";
string VS_191 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_192_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_192_Desc = "vs_2_x : mad source reg combination b0, a0, oPts is NOT allowed";
string VS_192 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_193_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_193_Desc = "vs_2_x : mad source reg combination b0, a0, oD0 is NOT allowed";
string VS_193 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_194_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_194_Desc = "vs_2_x : mad source reg combination b0, a0, oT0 is NOT allowed";
string VS_194 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_195_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_195_Desc = "vs_2_x : mad source reg combination b0, b0, a0 is NOT allowed";
string VS_195 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_196_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_196_Desc = "vs_2_x : mad source reg combination b0, b0, b0 is NOT allowed";
string VS_196 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_197_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_197_Desc = "vs_2_x : mad source reg combination b0, b0, i0 is NOT allowed";
string VS_197 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_198_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_198_Desc = "vs_2_x : mad source reg combination b0, b0, aL is NOT allowed";
string VS_198 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_199_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_199_Desc = "vs_2_x : mad source reg combination b0, b0, p0 is NOT allowed";
string VS_199 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_200_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_200_Desc = "vs_2_x : mad source reg combination b0, b0, oPos is NOT allowed";
string VS_200 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_201_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_201_Desc = "vs_2_x : mad source reg combination b0, b0, oFog is NOT allowed";
string VS_201 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_202_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_202_Desc = "vs_2_x : mad source reg combination b0, b0, oPts is NOT allowed";
string VS_202 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_203_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_203_Desc = "vs_2_x : mad source reg combination b0, b0, oD0 is NOT allowed";
string VS_203 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_204_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_204_Desc = "vs_2_x : mad source reg combination b0, b0, oT0 is NOT allowed";
string VS_204 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_205_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_205_Desc = "vs_2_x : mad source reg combination b0, i0, a0 is NOT allowed";
string VS_205 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_206_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_206_Desc = "vs_2_x : mad source reg combination b0, i0, b0 is NOT allowed";
string VS_206 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_207_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_207_Desc = "vs_2_x : mad source reg combination b0, i0, i0 is NOT allowed";
string VS_207 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_208_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_208_Desc = "vs_2_x : mad source reg combination b0, i0, aL is NOT allowed";
string VS_208 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_209_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_209_Desc = "vs_2_x : mad source reg combination b0, i0, p0 is NOT allowed";
string VS_209 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_210_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_210_Desc = "vs_2_x : mad source reg combination b0, i0, oPos is NOT allowed";
string VS_210 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_211_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_211_Desc = "vs_2_x : mad source reg combination b0, i0, oFog is NOT allowed";
string VS_211 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_212_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_212_Desc = "vs_2_x : mad source reg combination b0, i0, oPts is NOT allowed";
string VS_212 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_213_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_213_Desc = "vs_2_x : mad source reg combination b0, i0, oD0 is NOT allowed";
string VS_213 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_214_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_214_Desc = "vs_2_x : mad source reg combination b0, i0, oT0 is NOT allowed";
string VS_214 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_215_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_215_Desc = "vs_2_x : mad source reg combination b0, aL, a0 is NOT allowed";
string VS_215 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, b0, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_216_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_216_Desc = "vs_2_x : mad source reg combination b0, aL, b0 is NOT allowed";
string VS_216 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_217_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_217_Desc = "vs_2_x : mad source reg combination b0, aL, i0 is NOT allowed";
string VS_217 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_218_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_218_Desc = "vs_2_x : mad source reg combination b0, aL, aL is NOT allowed";
string VS_218 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_219_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_219_Desc = "vs_2_x : mad source reg combination b0, aL, p0 is NOT allowed";
string VS_219 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_220_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_220_Desc = "vs_2_x : mad source reg combination b0, aL, oPos is NOT allowed";
string VS_220 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_221_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_221_Desc = "vs_2_x : mad source reg combination b0, aL, oFog is NOT allowed";
string VS_221 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_222_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_222_Desc = "vs_2_x : mad source reg combination b0, aL, oPts is NOT allowed";
string VS_222 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_223_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_223_Desc = "vs_2_x : mad source reg combination b0, aL, oD0 is NOT allowed";
string VS_223 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_224_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_224_Desc = "vs_2_x : mad source reg combination b0, aL, oT0 is NOT allowed";
string VS_224 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_225_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_225_Desc = "vs_2_x : mad source reg combination b0, p0, a0 is NOT allowed";
string VS_225 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_226_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_226_Desc = "vs_2_x : mad source reg combination b0, p0, b0 is NOT allowed";
string VS_226 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_227_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_227_Desc = "vs_2_x : mad source reg combination b0, p0, i0 is NOT allowed";
string VS_227 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_228_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_228_Desc = "vs_2_x : mad source reg combination b0, p0, aL is NOT allowed";
string VS_228 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_229_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_229_Desc = "vs_2_x : mad source reg combination b0, p0, p0 is NOT allowed";
string VS_229 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_230_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_230_Desc = "vs_2_x : mad source reg combination b0, p0, oPos is NOT allowed";
string VS_230 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_231_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_231_Desc = "vs_2_x : mad source reg combination b0, p0, oFog is NOT allowed";
string VS_231 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_232_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_232_Desc = "vs_2_x : mad source reg combination b0, p0, oPts is NOT allowed";
string VS_232 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_233_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_233_Desc = "vs_2_x : mad source reg combination b0, p0, oD0 is NOT allowed";
string VS_233 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_234_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_234_Desc = "vs_2_x : mad source reg combination b0, p0, oT0 is NOT allowed";
string VS_234 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_235_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_235_Desc = "vs_2_x : mad source reg combination b0, oPos, a0 is NOT allowed";
string VS_235 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_236_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_236_Desc = "vs_2_x : mad source reg combination b0, oPos, b0 is NOT allowed";
string VS_236 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_237_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_237_Desc = "vs_2_x : mad source reg combination b0, oPos, i0 is NOT allowed";
string VS_237 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_238_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_238_Desc = "vs_2_x : mad source reg combination b0, oPos, aL is NOT allowed";
string VS_238 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_239_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_239_Desc = "vs_2_x : mad source reg combination b0, oPos, p0 is NOT allowed";
string VS_239 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_240_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_240_Desc = "vs_2_x : mad source reg combination b0, oPos, oPos is NOT allowed";
string VS_240 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_241_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_241_Desc = "vs_2_x : mad source reg combination b0, oPos, oFog is NOT allowed";
string VS_241 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_242_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_242_Desc = "vs_2_x : mad source reg combination b0, oPos, oPts is NOT allowed";
string VS_242 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_243_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_243_Desc = "vs_2_x : mad source reg combination b0, oPos, oD0 is NOT allowed";
string VS_243 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_244_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_244_Desc = "vs_2_x : mad source reg combination b0, oPos, oT0 is NOT allowed";
string VS_244 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_245_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_245_Desc = "vs_2_x : mad source reg combination b0, oFog, a0 is NOT allowed";
string VS_245 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_246_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_246_Desc = "vs_2_x : mad source reg combination b0, oFog, b0 is NOT allowed";
string VS_246 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_247_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_247_Desc = "vs_2_x : mad source reg combination b0, oFog, i0 is NOT allowed";
string VS_247 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_248_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_248_Desc = "vs_2_x : mad source reg combination b0, oFog, aL is NOT allowed";
string VS_248 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_249_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_249_Desc = "vs_2_x : mad source reg combination b0, oFog, p0 is NOT allowed";
string VS_249 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_250_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_250_Desc = "vs_2_x : mad source reg combination b0, oFog, oPos is NOT allowed";
string VS_250 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_251_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_251_Desc = "vs_2_x : mad source reg combination b0, oFog, oFog is NOT allowed";
string VS_251 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_252_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_252_Desc = "vs_2_x : mad source reg combination b0, oFog, oPts is NOT allowed";
string VS_252 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_253_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_253_Desc = "vs_2_x : mad source reg combination b0, oFog, oD0 is NOT allowed";
string VS_253 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_254_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_254_Desc = "vs_2_x : mad source reg combination b0, oFog, oT0 is NOT allowed";
string VS_254 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_255_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_255_Desc = "vs_2_x : mad source reg combination b0, oPts, a0 is NOT allowed";
string VS_255 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, b0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_256_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_256_Desc = "vs_2_x : mad source reg combination b0, oPts, b0 is NOT allowed";
string VS_256 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_257_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_257_Desc = "vs_2_x : mad source reg combination b0, oPts, i0 is NOT allowed";
string VS_257 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_258_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_258_Desc = "vs_2_x : mad source reg combination b0, oPts, aL is NOT allowed";
string VS_258 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, b0, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_259_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_259_Desc = "vs_2_x : mad source reg combination b0, oPts, p0 is NOT allowed";
string VS_259 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_260_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_260_Desc = "vs_2_x : mad source reg combination b0, oPts, oPos is NOT allowed";
string VS_260 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_261_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_261_Desc = "vs_2_x : mad source reg combination b0, oPts, oFog is NOT allowed";
string VS_261 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_262_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_262_Desc = "vs_2_x : mad source reg combination b0, oPts, oPts is NOT allowed";
string VS_262 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_263_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_263_Desc = "vs_2_x : mad source reg combination b0, oPts, oD0 is NOT allowed";
string VS_263 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_264_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_264_Desc = "vs_2_x : mad source reg combination b0, oPts, oT0 is NOT allowed";
string VS_264 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_265_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_265_Desc = "vs_2_x : mad source reg combination b0, oD0, a0 is NOT allowed";
string VS_265 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_266_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_266_Desc = "vs_2_x : mad source reg combination b0, oD0, b0 is NOT allowed";
string VS_266 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_267_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_267_Desc = "vs_2_x : mad source reg combination b0, oD0, i0 is NOT allowed";
string VS_267 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_268_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_268_Desc = "vs_2_x : mad source reg combination b0, oD0, aL is NOT allowed";
string VS_268 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_269_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_269_Desc = "vs_2_x : mad source reg combination b0, oD0, p0 is NOT allowed";
string VS_269 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_270_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_270_Desc = "vs_2_x : mad source reg combination b0, oD0, oPos is NOT allowed";
string VS_270 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_271_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_271_Desc = "vs_2_x : mad source reg combination b0, oD0, oFog is NOT allowed";
string VS_271 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_272_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_272_Desc = "vs_2_x : mad source reg combination b0, oD0, oPts is NOT allowed";
string VS_272 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_273_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_273_Desc = "vs_2_x : mad source reg combination b0, oD0, oD0 is NOT allowed";
string VS_273 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_274_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_274_Desc = "vs_2_x : mad source reg combination b0, oD0, oT0 is NOT allowed";
string VS_274 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_275_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_275_Desc = "vs_2_x : mad source reg combination b0, oT0, a0 is NOT allowed";
string VS_275 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, b0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_276_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_276_Desc = "vs_2_x : mad source reg combination b0, oT0, b0 is NOT allowed";
string VS_276 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_277_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_277_Desc = "vs_2_x : mad source reg combination b0, oT0, i0 is NOT allowed";
string VS_277 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, b0, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_278_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_278_Desc = "vs_2_x : mad source reg combination b0, oT0, aL is NOT allowed";
string VS_278 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, b0, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_279_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_279_Desc = "vs_2_x : mad source reg combination b0, oT0, p0 is NOT allowed";
string VS_279 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_280_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_280_Desc = "vs_2_x : mad source reg combination b0, oT0, oPos is NOT allowed";
string VS_280 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_281_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_281_Desc = "vs_2_x : mad source reg combination b0, oT0, oFog is NOT allowed";
string VS_281 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_282_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_282_Desc = "vs_2_x : mad source reg combination b0, oT0, oPts is NOT allowed";
string VS_282 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, b0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_283_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_283_Desc = "vs_2_x : mad source reg combination b0, oT0, oD0 is NOT allowed";
string VS_283 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_284_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_284_Desc = "vs_2_x : mad source reg combination b0, oT0, oT0 is NOT allowed";
string VS_284 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, b0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_285_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_285_Desc = "vs_2_x : mad source reg combination i0, a0, a0 is NOT allowed";
string VS_285 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_286_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_286_Desc = "vs_2_x : mad source reg combination i0, a0, b0 is NOT allowed";
string VS_286 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_287_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_287_Desc = "vs_2_x : mad source reg combination i0, a0, i0 is NOT allowed";
string VS_287 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_288_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_288_Desc = "vs_2_x : mad source reg combination i0, a0, aL is NOT allowed";
string VS_288 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, i0, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_289_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_289_Desc = "vs_2_x : mad source reg combination i0, a0, p0 is NOT allowed";
string VS_289 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_290_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_290_Desc = "vs_2_x : mad source reg combination i0, a0, oPos is NOT allowed";
string VS_290 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_291_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_291_Desc = "vs_2_x : mad source reg combination i0, a0, oFog is NOT allowed";
string VS_291 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_292_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_292_Desc = "vs_2_x : mad source reg combination i0, a0, oPts is NOT allowed";
string VS_292 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_293_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_293_Desc = "vs_2_x : mad source reg combination i0, a0, oD0 is NOT allowed";
string VS_293 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_294_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_294_Desc = "vs_2_x : mad source reg combination i0, a0, oT0 is NOT allowed";
string VS_294 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_295_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_295_Desc = "vs_2_x : mad source reg combination i0, b0, a0 is NOT allowed";
string VS_295 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_296_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_296_Desc = "vs_2_x : mad source reg combination i0, b0, b0 is NOT allowed";
string VS_296 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_297_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_297_Desc = "vs_2_x : mad source reg combination i0, b0, i0 is NOT allowed";
string VS_297 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_298_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_298_Desc = "vs_2_x : mad source reg combination i0, b0, aL is NOT allowed";
string VS_298 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_299_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_299_Desc = "vs_2_x : mad source reg combination i0, b0, p0 is NOT allowed";
string VS_299 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_300_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_300_Desc = "vs_2_x : mad source reg combination i0, b0, oPos is NOT allowed";
string VS_300 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_301_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_301_Desc = "vs_2_x : mad source reg combination i0, b0, oFog is NOT allowed";
string VS_301 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_302_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_302_Desc = "vs_2_x : mad source reg combination i0, b0, oPts is NOT allowed";
string VS_302 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_303_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_303_Desc = "vs_2_x : mad source reg combination i0, b0, oD0 is NOT allowed";
string VS_303 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_304_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_304_Desc = "vs_2_x : mad source reg combination i0, b0, oT0 is NOT allowed";
string VS_304 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_305_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_305_Desc = "vs_2_x : mad source reg combination i0, i0, a0 is NOT allowed";
string VS_305 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_306_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_306_Desc = "vs_2_x : mad source reg combination i0, i0, b0 is NOT allowed";
string VS_306 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_307_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_307_Desc = "vs_2_x : mad source reg combination i0, i0, i0 is NOT allowed";
string VS_307 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_308_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_308_Desc = "vs_2_x : mad source reg combination i0, i0, aL is NOT allowed";
string VS_308 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_309_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_309_Desc = "vs_2_x : mad source reg combination i0, i0, p0 is NOT allowed";
string VS_309 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_310_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_310_Desc = "vs_2_x : mad source reg combination i0, i0, oPos is NOT allowed";
string VS_310 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_311_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_311_Desc = "vs_2_x : mad source reg combination i0, i0, oFog is NOT allowed";
string VS_311 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_312_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_312_Desc = "vs_2_x : mad source reg combination i0, i0, oPts is NOT allowed";
string VS_312 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_313_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_313_Desc = "vs_2_x : mad source reg combination i0, i0, oD0 is NOT allowed";
string VS_313 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_314_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_314_Desc = "vs_2_x : mad source reg combination i0, i0, oT0 is NOT allowed";
string VS_314 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_315_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_315_Desc = "vs_2_x : mad source reg combination i0, aL, a0 is NOT allowed";
string VS_315 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, i0, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_316_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_316_Desc = "vs_2_x : mad source reg combination i0, aL, b0 is NOT allowed";
string VS_316 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_317_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_317_Desc = "vs_2_x : mad source reg combination i0, aL, i0 is NOT allowed";
string VS_317 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_318_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_318_Desc = "vs_2_x : mad source reg combination i0, aL, aL is NOT allowed";
string VS_318 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_319_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_319_Desc = "vs_2_x : mad source reg combination i0, aL, p0 is NOT allowed";
string VS_319 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_320_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_320_Desc = "vs_2_x : mad source reg combination i0, aL, oPos is NOT allowed";
string VS_320 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_321_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_321_Desc = "vs_2_x : mad source reg combination i0, aL, oFog is NOT allowed";
string VS_321 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_322_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_322_Desc = "vs_2_x : mad source reg combination i0, aL, oPts is NOT allowed";
string VS_322 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_323_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_323_Desc = "vs_2_x : mad source reg combination i0, aL, oD0 is NOT allowed";
string VS_323 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_324_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_324_Desc = "vs_2_x : mad source reg combination i0, aL, oT0 is NOT allowed";
string VS_324 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_325_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_325_Desc = "vs_2_x : mad source reg combination i0, p0, a0 is NOT allowed";
string VS_325 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_326_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_326_Desc = "vs_2_x : mad source reg combination i0, p0, b0 is NOT allowed";
string VS_326 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_327_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_327_Desc = "vs_2_x : mad source reg combination i0, p0, i0 is NOT allowed";
string VS_327 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_328_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_328_Desc = "vs_2_x : mad source reg combination i0, p0, aL is NOT allowed";
string VS_328 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_329_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_329_Desc = "vs_2_x : mad source reg combination i0, p0, p0 is NOT allowed";
string VS_329 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_330_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_330_Desc = "vs_2_x : mad source reg combination i0, p0, oPos is NOT allowed";
string VS_330 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_331_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_331_Desc = "vs_2_x : mad source reg combination i0, p0, oFog is NOT allowed";
string VS_331 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_332_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_332_Desc = "vs_2_x : mad source reg combination i0, p0, oPts is NOT allowed";
string VS_332 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_333_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_333_Desc = "vs_2_x : mad source reg combination i0, p0, oD0 is NOT allowed";
string VS_333 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_334_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_334_Desc = "vs_2_x : mad source reg combination i0, p0, oT0 is NOT allowed";
string VS_334 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_335_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_335_Desc = "vs_2_x : mad source reg combination i0, oPos, a0 is NOT allowed";
string VS_335 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_336_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_336_Desc = "vs_2_x : mad source reg combination i0, oPos, b0 is NOT allowed";
string VS_336 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_337_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_337_Desc = "vs_2_x : mad source reg combination i0, oPos, i0 is NOT allowed";
string VS_337 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_338_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_338_Desc = "vs_2_x : mad source reg combination i0, oPos, aL is NOT allowed";
string VS_338 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_339_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_339_Desc = "vs_2_x : mad source reg combination i0, oPos, p0 is NOT allowed";
string VS_339 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_340_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_340_Desc = "vs_2_x : mad source reg combination i0, oPos, oPos is NOT allowed";
string VS_340 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_341_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_341_Desc = "vs_2_x : mad source reg combination i0, oPos, oFog is NOT allowed";
string VS_341 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_342_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_342_Desc = "vs_2_x : mad source reg combination i0, oPos, oPts is NOT allowed";
string VS_342 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_343_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_343_Desc = "vs_2_x : mad source reg combination i0, oPos, oD0 is NOT allowed";
string VS_343 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_344_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_344_Desc = "vs_2_x : mad source reg combination i0, oPos, oT0 is NOT allowed";
string VS_344 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_345_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_345_Desc = "vs_2_x : mad source reg combination i0, oFog, a0 is NOT allowed";
string VS_345 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_346_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_346_Desc = "vs_2_x : mad source reg combination i0, oFog, b0 is NOT allowed";
string VS_346 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_347_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_347_Desc = "vs_2_x : mad source reg combination i0, oFog, i0 is NOT allowed";
string VS_347 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_348_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_348_Desc = "vs_2_x : mad source reg combination i0, oFog, aL is NOT allowed";
string VS_348 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_349_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_349_Desc = "vs_2_x : mad source reg combination i0, oFog, p0 is NOT allowed";
string VS_349 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_350_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_350_Desc = "vs_2_x : mad source reg combination i0, oFog, oPos is NOT allowed";
string VS_350 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_351_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_351_Desc = "vs_2_x : mad source reg combination i0, oFog, oFog is NOT allowed";
string VS_351 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_352_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_352_Desc = "vs_2_x : mad source reg combination i0, oFog, oPts is NOT allowed";
string VS_352 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_353_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_353_Desc = "vs_2_x : mad source reg combination i0, oFog, oD0 is NOT allowed";
string VS_353 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_354_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_354_Desc = "vs_2_x : mad source reg combination i0, oFog, oT0 is NOT allowed";
string VS_354 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_355_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_355_Desc = "vs_2_x : mad source reg combination i0, oPts, a0 is NOT allowed";
string VS_355 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, i0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_356_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_356_Desc = "vs_2_x : mad source reg combination i0, oPts, b0 is NOT allowed";
string VS_356 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_357_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_357_Desc = "vs_2_x : mad source reg combination i0, oPts, i0 is NOT allowed";
string VS_357 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_358_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_358_Desc = "vs_2_x : mad source reg combination i0, oPts, aL is NOT allowed";
string VS_358 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, i0, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_359_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_359_Desc = "vs_2_x : mad source reg combination i0, oPts, p0 is NOT allowed";
string VS_359 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_360_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_360_Desc = "vs_2_x : mad source reg combination i0, oPts, oPos is NOT allowed";
string VS_360 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_361_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_361_Desc = "vs_2_x : mad source reg combination i0, oPts, oFog is NOT allowed";
string VS_361 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_362_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_362_Desc = "vs_2_x : mad source reg combination i0, oPts, oPts is NOT allowed";
string VS_362 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_363_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_363_Desc = "vs_2_x : mad source reg combination i0, oPts, oD0 is NOT allowed";
string VS_363 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_364_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_364_Desc = "vs_2_x : mad source reg combination i0, oPts, oT0 is NOT allowed";
string VS_364 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_365_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_365_Desc = "vs_2_x : mad source reg combination i0, oD0, a0 is NOT allowed";
string VS_365 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_366_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_366_Desc = "vs_2_x : mad source reg combination i0, oD0, b0 is NOT allowed";
string VS_366 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_367_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_367_Desc = "vs_2_x : mad source reg combination i0, oD0, i0 is NOT allowed";
string VS_367 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_368_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_368_Desc = "vs_2_x : mad source reg combination i0, oD0, aL is NOT allowed";
string VS_368 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_369_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_369_Desc = "vs_2_x : mad source reg combination i0, oD0, p0 is NOT allowed";
string VS_369 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_370_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_370_Desc = "vs_2_x : mad source reg combination i0, oD0, oPos is NOT allowed";
string VS_370 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_371_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_371_Desc = "vs_2_x : mad source reg combination i0, oD0, oFog is NOT allowed";
string VS_371 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_372_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_372_Desc = "vs_2_x : mad source reg combination i0, oD0, oPts is NOT allowed";
string VS_372 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_373_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_373_Desc = "vs_2_x : mad source reg combination i0, oD0, oD0 is NOT allowed";
string VS_373 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_374_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_374_Desc = "vs_2_x : mad source reg combination i0, oD0, oT0 is NOT allowed";
string VS_374 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_375_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_375_Desc = "vs_2_x : mad source reg combination i0, oT0, a0 is NOT allowed";
string VS_375 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, i0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_376_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_376_Desc = "vs_2_x : mad source reg combination i0, oT0, b0 is NOT allowed";
string VS_376 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_377_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_377_Desc = "vs_2_x : mad source reg combination i0, oT0, i0 is NOT allowed";
string VS_377 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_378_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_378_Desc = "vs_2_x : mad source reg combination i0, oT0, aL is NOT allowed";
string VS_378 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, i0, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_379_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_379_Desc = "vs_2_x : mad source reg combination i0, oT0, p0 is NOT allowed";
string VS_379 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_380_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_380_Desc = "vs_2_x : mad source reg combination i0, oT0, oPos is NOT allowed";
string VS_380 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_381_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_381_Desc = "vs_2_x : mad source reg combination i0, oT0, oFog is NOT allowed";
string VS_381 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_382_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_382_Desc = "vs_2_x : mad source reg combination i0, oT0, oPts is NOT allowed";
string VS_382 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, i0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_383_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_383_Desc = "vs_2_x : mad source reg combination i0, oT0, oD0 is NOT allowed";
string VS_383 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_384_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_384_Desc = "vs_2_x : mad source reg combination i0, oT0, oT0 is NOT allowed";
string VS_384 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, i0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_385_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_385_Desc = "vs_2_x : mad source reg combination aL, a0, a0 is NOT allowed";
string VS_385 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_386_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_386_Desc = "vs_2_x : mad source reg combination aL, a0, b0 is NOT allowed";
string VS_386 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_387_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_387_Desc = "vs_2_x : mad source reg combination aL, a0, i0 is NOT allowed";
string VS_387 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_388_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_388_Desc = "vs_2_x : mad source reg combination aL, a0, aL is NOT allowed";
string VS_388 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_389_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_389_Desc = "vs_2_x : mad source reg combination aL, a0, p0 is NOT allowed";
string VS_389 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_390_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_390_Desc = "vs_2_x : mad source reg combination aL, a0, oPos is NOT allowed";
string VS_390 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_391_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_391_Desc = "vs_2_x : mad source reg combination aL, a0, oFog is NOT allowed";
string VS_391 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_392_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_392_Desc = "vs_2_x : mad source reg combination aL, a0, oPts is NOT allowed";
string VS_392 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_393_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_393_Desc = "vs_2_x : mad source reg combination aL, a0, oD0 is NOT allowed";
string VS_393 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_394_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_394_Desc = "vs_2_x : mad source reg combination aL, a0, oT0 is NOT allowed";
string VS_394 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_395_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_395_Desc = "vs_2_x : mad source reg combination aL, b0, a0 is NOT allowed";
string VS_395 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_396_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_396_Desc = "vs_2_x : mad source reg combination aL, b0, b0 is NOT allowed";
string VS_396 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_397_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_397_Desc = "vs_2_x : mad source reg combination aL, b0, i0 is NOT allowed";
string VS_397 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_398_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_398_Desc = "vs_2_x : mad source reg combination aL, b0, aL is NOT allowed";
string VS_398 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_399_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_399_Desc = "vs_2_x : mad source reg combination aL, b0, p0 is NOT allowed";
string VS_399 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_400_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_400_Desc = "vs_2_x : mad source reg combination aL, b0, oPos is NOT allowed";
string VS_400 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_401_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_401_Desc = "vs_2_x : mad source reg combination aL, b0, oFog is NOT allowed";
string VS_401 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_402_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_402_Desc = "vs_2_x : mad source reg combination aL, b0, oPts is NOT allowed";
string VS_402 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_403_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_403_Desc = "vs_2_x : mad source reg combination aL, b0, oD0 is NOT allowed";
string VS_403 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_404_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_404_Desc = "vs_2_x : mad source reg combination aL, b0, oT0 is NOT allowed";
string VS_404 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_405_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_405_Desc = "vs_2_x : mad source reg combination aL, i0, a0 is NOT allowed";
string VS_405 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_406_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_406_Desc = "vs_2_x : mad source reg combination aL, i0, b0 is NOT allowed";
string VS_406 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_407_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_407_Desc = "vs_2_x : mad source reg combination aL, i0, i0 is NOT allowed";
string VS_407 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_408_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_408_Desc = "vs_2_x : mad source reg combination aL, i0, aL is NOT allowed";
string VS_408 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_409_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_409_Desc = "vs_2_x : mad source reg combination aL, i0, p0 is NOT allowed";
string VS_409 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_410_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_410_Desc = "vs_2_x : mad source reg combination aL, i0, oPos is NOT allowed";
string VS_410 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_411_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_411_Desc = "vs_2_x : mad source reg combination aL, i0, oFog is NOT allowed";
string VS_411 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_412_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_412_Desc = "vs_2_x : mad source reg combination aL, i0, oPts is NOT allowed";
string VS_412 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_413_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_413_Desc = "vs_2_x : mad source reg combination aL, i0, oD0 is NOT allowed";
string VS_413 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_414_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_414_Desc = "vs_2_x : mad source reg combination aL, i0, oT0 is NOT allowed";
string VS_414 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_415_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_415_Desc = "vs_2_x : mad source reg combination aL, aL, a0 is NOT allowed";
string VS_415 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_416_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_416_Desc = "vs_2_x : mad source reg combination aL, aL, b0 is NOT allowed";
string VS_416 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_417_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_417_Desc = "vs_2_x : mad source reg combination aL, aL, i0 is NOT allowed";
string VS_417 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_418_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_418_Desc = "vs_2_x : mad source reg combination aL, aL, aL is NOT allowed";
string VS_418 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_419_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_419_Desc = "vs_2_x : mad source reg combination aL, aL, p0 is NOT allowed";
string VS_419 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_420_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_420_Desc = "vs_2_x : mad source reg combination aL, aL, oPos is NOT allowed";
string VS_420 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_421_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_421_Desc = "vs_2_x : mad source reg combination aL, aL, oFog is NOT allowed";
string VS_421 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_422_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_422_Desc = "vs_2_x : mad source reg combination aL, aL, oPts is NOT allowed";
string VS_422 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_423_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_423_Desc = "vs_2_x : mad source reg combination aL, aL, oD0 is NOT allowed";
string VS_423 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_424_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_424_Desc = "vs_2_x : mad source reg combination aL, aL, oT0 is NOT allowed";
string VS_424 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_425_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_425_Desc = "vs_2_x : mad source reg combination aL, p0, a0 is NOT allowed";
string VS_425 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_426_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_426_Desc = "vs_2_x : mad source reg combination aL, p0, b0 is NOT allowed";
string VS_426 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_427_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_427_Desc = "vs_2_x : mad source reg combination aL, p0, i0 is NOT allowed";
string VS_427 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_428_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_428_Desc = "vs_2_x : mad source reg combination aL, p0, aL is NOT allowed";
string VS_428 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_429_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_429_Desc = "vs_2_x : mad source reg combination aL, p0, p0 is NOT allowed";
string VS_429 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_430_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_430_Desc = "vs_2_x : mad source reg combination aL, p0, oPos is NOT allowed";
string VS_430 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_431_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_431_Desc = "vs_2_x : mad source reg combination aL, p0, oFog is NOT allowed";
string VS_431 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_432_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_432_Desc = "vs_2_x : mad source reg combination aL, p0, oPts is NOT allowed";
string VS_432 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_433_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_433_Desc = "vs_2_x : mad source reg combination aL, p0, oD0 is NOT allowed";
string VS_433 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_434_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_434_Desc = "vs_2_x : mad source reg combination aL, p0, oT0 is NOT allowed";
string VS_434 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_435_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_435_Desc = "vs_2_x : mad source reg combination aL, oPos, a0 is NOT allowed";
string VS_435 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_436_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_436_Desc = "vs_2_x : mad source reg combination aL, oPos, b0 is NOT allowed";
string VS_436 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_437_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_437_Desc = "vs_2_x : mad source reg combination aL, oPos, i0 is NOT allowed";
string VS_437 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_438_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_438_Desc = "vs_2_x : mad source reg combination aL, oPos, aL is NOT allowed";
string VS_438 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_439_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_439_Desc = "vs_2_x : mad source reg combination aL, oPos, p0 is NOT allowed";
string VS_439 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_440_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_440_Desc = "vs_2_x : mad source reg combination aL, oPos, oPos is NOT allowed";
string VS_440 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_441_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_441_Desc = "vs_2_x : mad source reg combination aL, oPos, oFog is NOT allowed";
string VS_441 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_442_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_442_Desc = "vs_2_x : mad source reg combination aL, oPos, oPts is NOT allowed";
string VS_442 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_443_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_443_Desc = "vs_2_x : mad source reg combination aL, oPos, oD0 is NOT allowed";
string VS_443 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_444_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_444_Desc = "vs_2_x : mad source reg combination aL, oPos, oT0 is NOT allowed";
string VS_444 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_445_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_445_Desc = "vs_2_x : mad source reg combination aL, oFog, a0 is NOT allowed";
string VS_445 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_446_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_446_Desc = "vs_2_x : mad source reg combination aL, oFog, b0 is NOT allowed";
string VS_446 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_447_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_447_Desc = "vs_2_x : mad source reg combination aL, oFog, i0 is NOT allowed";
string VS_447 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_448_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_448_Desc = "vs_2_x : mad source reg combination aL, oFog, aL is NOT allowed";
string VS_448 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_449_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_449_Desc = "vs_2_x : mad source reg combination aL, oFog, p0 is NOT allowed";
string VS_449 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_450_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_450_Desc = "vs_2_x : mad source reg combination aL, oFog, oPos is NOT allowed";
string VS_450 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_451_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_451_Desc = "vs_2_x : mad source reg combination aL, oFog, oFog is NOT allowed";
string VS_451 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_452_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_452_Desc = "vs_2_x : mad source reg combination aL, oFog, oPts is NOT allowed";
string VS_452 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_453_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_453_Desc = "vs_2_x : mad source reg combination aL, oFog, oD0 is NOT allowed";
string VS_453 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_454_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_454_Desc = "vs_2_x : mad source reg combination aL, oFog, oT0 is NOT allowed";
string VS_454 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_455_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_455_Desc = "vs_2_x : mad source reg combination aL, oPts, a0 is NOT allowed";
string VS_455 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_456_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_456_Desc = "vs_2_x : mad source reg combination aL, oPts, b0 is NOT allowed";
string VS_456 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_457_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_457_Desc = "vs_2_x : mad source reg combination aL, oPts, i0 is NOT allowed";
string VS_457 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_458_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_458_Desc = "vs_2_x : mad source reg combination aL, oPts, aL is NOT allowed";
string VS_458 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_459_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_459_Desc = "vs_2_x : mad source reg combination aL, oPts, p0 is NOT allowed";
string VS_459 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_460_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_460_Desc = "vs_2_x : mad source reg combination aL, oPts, oPos is NOT allowed";
string VS_460 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_461_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_461_Desc = "vs_2_x : mad source reg combination aL, oPts, oFog is NOT allowed";
string VS_461 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_462_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_462_Desc = "vs_2_x : mad source reg combination aL, oPts, oPts is NOT allowed";
string VS_462 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_463_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_463_Desc = "vs_2_x : mad source reg combination aL, oPts, oD0 is NOT allowed";
string VS_463 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_464_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_464_Desc = "vs_2_x : mad source reg combination aL, oPts, oT0 is NOT allowed";
string VS_464 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_465_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_465_Desc = "vs_2_x : mad source reg combination aL, oD0, a0 is NOT allowed";
string VS_465 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_466_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_466_Desc = "vs_2_x : mad source reg combination aL, oD0, b0 is NOT allowed";
string VS_466 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_467_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_467_Desc = "vs_2_x : mad source reg combination aL, oD0, i0 is NOT allowed";
string VS_467 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_468_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_468_Desc = "vs_2_x : mad source reg combination aL, oD0, aL is NOT allowed";
string VS_468 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_469_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_469_Desc = "vs_2_x : mad source reg combination aL, oD0, p0 is NOT allowed";
string VS_469 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_470_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_470_Desc = "vs_2_x : mad source reg combination aL, oD0, oPos is NOT allowed";
string VS_470 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_471_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_471_Desc = "vs_2_x : mad source reg combination aL, oD0, oFog is NOT allowed";
string VS_471 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_472_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_472_Desc = "vs_2_x : mad source reg combination aL, oD0, oPts is NOT allowed";
string VS_472 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_473_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_473_Desc = "vs_2_x : mad source reg combination aL, oD0, oD0 is NOT allowed";
string VS_473 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_474_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_474_Desc = "vs_2_x : mad source reg combination aL, oD0, oT0 is NOT allowed";
string VS_474 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_475_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_475_Desc = "vs_2_x : mad source reg combination aL, oT0, a0 is NOT allowed";
string VS_475 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, aL, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_476_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_476_Desc = "vs_2_x : mad source reg combination aL, oT0, b0 is NOT allowed";
string VS_476 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, aL, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_477_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_477_Desc = "vs_2_x : mad source reg combination aL, oT0, i0 is NOT allowed";
string VS_477 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, aL, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_478_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_478_Desc = "vs_2_x : mad source reg combination aL, oT0, aL is NOT allowed";
string VS_478 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_479_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_479_Desc = "vs_2_x : mad source reg combination aL, oT0, p0 is NOT allowed";
string VS_479 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_480_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_480_Desc = "vs_2_x : mad source reg combination aL, oT0, oPos is NOT allowed";
string VS_480 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_481_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_481_Desc = "vs_2_x : mad source reg combination aL, oT0, oFog is NOT allowed";
string VS_481 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_482_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_482_Desc = "vs_2_x : mad source reg combination aL, oT0, oPts is NOT allowed";
string VS_482 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, aL, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_483_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_483_Desc = "vs_2_x : mad source reg combination aL, oT0, oD0 is NOT allowed";
string VS_483 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_484_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_484_Desc = "vs_2_x : mad source reg combination aL, oT0, oT0 is NOT allowed";
string VS_484 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, aL, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_485_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_485_Desc = "vs_2_x : mad source reg combination p0, a0, a0 is NOT allowed";
string VS_485 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_486_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_486_Desc = "vs_2_x : mad source reg combination p0, a0, b0 is NOT allowed";
string VS_486 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_487_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_487_Desc = "vs_2_x : mad source reg combination p0, a0, i0 is NOT allowed";
string VS_487 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_488_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_488_Desc = "vs_2_x : mad source reg combination p0, a0, aL is NOT allowed";
string VS_488 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, p0, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_489_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_489_Desc = "vs_2_x : mad source reg combination p0, a0, p0 is NOT allowed";
string VS_489 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_490_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_490_Desc = "vs_2_x : mad source reg combination p0, a0, oPos is NOT allowed";
string VS_490 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_491_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_491_Desc = "vs_2_x : mad source reg combination p0, a0, oFog is NOT allowed";
string VS_491 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_492_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_492_Desc = "vs_2_x : mad source reg combination p0, a0, oPts is NOT allowed";
string VS_492 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_493_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_493_Desc = "vs_2_x : mad source reg combination p0, a0, oD0 is NOT allowed";
string VS_493 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_494_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_494_Desc = "vs_2_x : mad source reg combination p0, a0, oT0 is NOT allowed";
string VS_494 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_495_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_495_Desc = "vs_2_x : mad source reg combination p0, b0, a0 is NOT allowed";
string VS_495 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, p0, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_496_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_496_Desc = "vs_2_x : mad source reg combination p0, b0, b0 is NOT allowed";
string VS_496 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_497_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_497_Desc = "vs_2_x : mad source reg combination p0, b0, i0 is NOT allowed";
string VS_497 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, p0, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_498_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_498_Desc = "vs_2_x : mad source reg combination p0, b0, aL is NOT allowed";
string VS_498 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, p0, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_499_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_499_Desc = "vs_2_x : mad source reg combination p0, b0, p0 is NOT allowed";
string VS_499 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_500_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_500_Desc = "vs_2_x : mad source reg combination p0, b0, oPos is NOT allowed";
string VS_500 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_501_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_501_Desc = "vs_2_x : mad source reg combination p0, b0, oFog is NOT allowed";
string VS_501 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_502_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_502_Desc = "vs_2_x : mad source reg combination p0, b0, oPts is NOT allowed";
string VS_502 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, p0, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_503_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_503_Desc = "vs_2_x : mad source reg combination p0, b0, oD0 is NOT allowed";
string VS_503 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_504_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_504_Desc = "vs_2_x : mad source reg combination p0, b0, oT0 is NOT allowed";
string VS_504 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_505_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_505_Desc = "vs_2_x : mad source reg combination p0, i0, a0 is NOT allowed";
string VS_505 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, p0, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_506_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_506_Desc = "vs_2_x : mad source reg combination p0, i0, b0 is NOT allowed";
string VS_506 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_507_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_507_Desc = "vs_2_x : mad source reg combination p0, i0, i0 is NOT allowed";
string VS_507 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_508_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_508_Desc = "vs_2_x : mad source reg combination p0, i0, aL is NOT allowed";
string VS_508 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, p0, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_509_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_509_Desc = "vs_2_x : mad source reg combination p0, i0, p0 is NOT allowed";
string VS_509 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_510_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_510_Desc = "vs_2_x : mad source reg combination p0, i0, oPos is NOT allowed";
string VS_510 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_511_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_511_Desc = "vs_2_x : mad source reg combination p0, i0, oFog is NOT allowed";
string VS_511 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_512_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_512_Desc = "vs_2_x : mad source reg combination p0, i0, oPts is NOT allowed";
string VS_512 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, p0, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_513_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_513_Desc = "vs_2_x : mad source reg combination p0, i0, oD0 is NOT allowed";
string VS_513 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_514_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_514_Desc = "vs_2_x : mad source reg combination p0, i0, oT0 is NOT allowed";
string VS_514 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_515_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_515_Desc = "vs_2_x : mad source reg combination p0, aL, a0 is NOT allowed";
string VS_515 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, p0, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_516_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_516_Desc = "vs_2_x : mad source reg combination p0, aL, b0 is NOT allowed";
string VS_516 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_517_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_517_Desc = "vs_2_x : mad source reg combination p0, aL, i0 is NOT allowed";
string VS_517 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_518_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_518_Desc = "vs_2_x : mad source reg combination p0, aL, aL is NOT allowed";
string VS_518 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_519_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_519_Desc = "vs_2_x : mad source reg combination p0, aL, p0 is NOT allowed";
string VS_519 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_520_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_520_Desc = "vs_2_x : mad source reg combination p0, aL, oPos is NOT allowed";
string VS_520 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_521_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_521_Desc = "vs_2_x : mad source reg combination p0, aL, oFog is NOT allowed";
string VS_521 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_522_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_522_Desc = "vs_2_x : mad source reg combination p0, aL, oPts is NOT allowed";
string VS_522 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_523_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_523_Desc = "vs_2_x : mad source reg combination p0, aL, oD0 is NOT allowed";
string VS_523 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_524_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_524_Desc = "vs_2_x : mad source reg combination p0, aL, oT0 is NOT allowed";
string VS_524 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_525_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_525_Desc = "vs_2_x : mad source reg combination p0, p0, a0 is NOT allowed";
string VS_525 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_526_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_526_Desc = "vs_2_x : mad source reg combination p0, p0, b0 is NOT allowed";
string VS_526 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_527_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_527_Desc = "vs_2_x : mad source reg combination p0, p0, i0 is NOT allowed";
string VS_527 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_528_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_528_Desc = "vs_2_x : mad source reg combination p0, p0, aL is NOT allowed";
string VS_528 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_529_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_529_Desc = "vs_2_x : mad source reg combination p0, p0, p0 is NOT allowed";
string VS_529 = 
	"vs_2_x "
	"mad r0, p0, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_530_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_530_Desc = "vs_2_x : mad source reg combination p0, p0, oPos is NOT allowed";
string VS_530 = 
	"vs_2_x "
	"mad r0, p0, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_531_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_531_Desc = "vs_2_x : mad source reg combination p0, p0, oFog is NOT allowed";
string VS_531 = 
	"vs_2_x "
	"mad r0, p0, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_532_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_532_Desc = "vs_2_x : mad source reg combination p0, p0, oPts is NOT allowed";
string VS_532 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_533_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_533_Desc = "vs_2_x : mad source reg combination p0, p0, oD0 is NOT allowed";
string VS_533 = 
	"vs_2_x "
	"mad r0, p0, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_534_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_534_Desc = "vs_2_x : mad source reg combination p0, p0, oT0 is NOT allowed";
string VS_534 = 
	"vs_2_x "
	"mad r0, p0, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_535_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_535_Desc = "vs_2_x : mad source reg combination p0, oPos, a0 is NOT allowed";
string VS_535 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_536_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_536_Desc = "vs_2_x : mad source reg combination p0, oPos, b0 is NOT allowed";
string VS_536 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_537_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_537_Desc = "vs_2_x : mad source reg combination p0, oPos, i0 is NOT allowed";
string VS_537 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_538_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_538_Desc = "vs_2_x : mad source reg combination p0, oPos, aL is NOT allowed";
string VS_538 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_539_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_539_Desc = "vs_2_x : mad source reg combination p0, oPos, p0 is NOT allowed";
string VS_539 = 
	"vs_2_x "
	"mad r0, p0, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_540_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_540_Desc = "vs_2_x : mad source reg combination p0, oPos, oPos is NOT allowed";
string VS_540 = 
	"vs_2_x "
	"mad r0, p0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_541_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_541_Desc = "vs_2_x : mad source reg combination p0, oPos, oFog is NOT allowed";
string VS_541 = 
	"vs_2_x "
	"mad r0, p0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_542_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_542_Desc = "vs_2_x : mad source reg combination p0, oPos, oPts is NOT allowed";
string VS_542 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_543_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_543_Desc = "vs_2_x : mad source reg combination p0, oPos, oD0 is NOT allowed";
string VS_543 = 
	"vs_2_x "
	"mad r0, p0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_544_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_544_Desc = "vs_2_x : mad source reg combination p0, oPos, oT0 is NOT allowed";
string VS_544 = 
	"vs_2_x "
	"mad r0, p0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_545_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_545_Desc = "vs_2_x : mad source reg combination p0, oFog, a0 is NOT allowed";
string VS_545 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_546_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_546_Desc = "vs_2_x : mad source reg combination p0, oFog, b0 is NOT allowed";
string VS_546 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_547_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_547_Desc = "vs_2_x : mad source reg combination p0, oFog, i0 is NOT allowed";
string VS_547 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_548_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_548_Desc = "vs_2_x : mad source reg combination p0, oFog, aL is NOT allowed";
string VS_548 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_549_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_549_Desc = "vs_2_x : mad source reg combination p0, oFog, p0 is NOT allowed";
string VS_549 = 
	"vs_2_x "
	"mad r0, p0, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_550_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_550_Desc = "vs_2_x : mad source reg combination p0, oFog, oPos is NOT allowed";
string VS_550 = 
	"vs_2_x "
	"mad r0, p0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_551_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_551_Desc = "vs_2_x : mad source reg combination p0, oFog, oFog is NOT allowed";
string VS_551 = 
	"vs_2_x "
	"mad r0, p0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_552_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_552_Desc = "vs_2_x : mad source reg combination p0, oFog, oPts is NOT allowed";
string VS_552 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_553_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_553_Desc = "vs_2_x : mad source reg combination p0, oFog, oD0 is NOT allowed";
string VS_553 = 
	"vs_2_x "
	"mad r0, p0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_554_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_554_Desc = "vs_2_x : mad source reg combination p0, oFog, oT0 is NOT allowed";
string VS_554 = 
	"vs_2_x "
	"mad r0, p0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_555_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_555_Desc = "vs_2_x : mad source reg combination p0, oPts, a0 is NOT allowed";
string VS_555 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, p0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_556_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_556_Desc = "vs_2_x : mad source reg combination p0, oPts, b0 is NOT allowed";
string VS_556 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, p0, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_557_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_557_Desc = "vs_2_x : mad source reg combination p0, oPts, i0 is NOT allowed";
string VS_557 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, p0, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_558_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_558_Desc = "vs_2_x : mad source reg combination p0, oPts, aL is NOT allowed";
string VS_558 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, p0, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_559_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_559_Desc = "vs_2_x : mad source reg combination p0, oPts, p0 is NOT allowed";
string VS_559 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_560_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_560_Desc = "vs_2_x : mad source reg combination p0, oPts, oPos is NOT allowed";
string VS_560 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_561_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_561_Desc = "vs_2_x : mad source reg combination p0, oPts, oFog is NOT allowed";
string VS_561 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_562_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_562_Desc = "vs_2_x : mad source reg combination p0, oPts, oPts is NOT allowed";
string VS_562 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_563_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_563_Desc = "vs_2_x : mad source reg combination p0, oPts, oD0 is NOT allowed";
string VS_563 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_564_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_564_Desc = "vs_2_x : mad source reg combination p0, oPts, oT0 is NOT allowed";
string VS_564 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_565_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_565_Desc = "vs_2_x : mad source reg combination p0, oD0, a0 is NOT allowed";
string VS_565 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_566_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_566_Desc = "vs_2_x : mad source reg combination p0, oD0, b0 is NOT allowed";
string VS_566 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_567_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_567_Desc = "vs_2_x : mad source reg combination p0, oD0, i0 is NOT allowed";
string VS_567 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_568_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_568_Desc = "vs_2_x : mad source reg combination p0, oD0, aL is NOT allowed";
string VS_568 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_569_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_569_Desc = "vs_2_x : mad source reg combination p0, oD0, p0 is NOT allowed";
string VS_569 = 
	"vs_2_x "
	"mad r0, p0, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_570_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_570_Desc = "vs_2_x : mad source reg combination p0, oD0, oPos is NOT allowed";
string VS_570 = 
	"vs_2_x "
	"mad r0, p0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_571_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_571_Desc = "vs_2_x : mad source reg combination p0, oD0, oFog is NOT allowed";
string VS_571 = 
	"vs_2_x "
	"mad r0, p0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_572_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_572_Desc = "vs_2_x : mad source reg combination p0, oD0, oPts is NOT allowed";
string VS_572 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_573_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_573_Desc = "vs_2_x : mad source reg combination p0, oD0, oD0 is NOT allowed";
string VS_573 = 
	"vs_2_x "
	"mad r0, p0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_574_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_574_Desc = "vs_2_x : mad source reg combination p0, oD0, oT0 is NOT allowed";
string VS_574 = 
	"vs_2_x "
	"mad r0, p0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_575_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_575_Desc = "vs_2_x : mad source reg combination p0, oT0, a0 is NOT allowed";
string VS_575 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, p0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_576_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_576_Desc = "vs_2_x : mad source reg combination p0, oT0, b0 is NOT allowed";
string VS_576 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, p0, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_577_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_577_Desc = "vs_2_x : mad source reg combination p0, oT0, i0 is NOT allowed";
string VS_577 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, p0, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_578_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_578_Desc = "vs_2_x : mad source reg combination p0, oT0, aL is NOT allowed";
string VS_578 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, p0, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_579_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_579_Desc = "vs_2_x : mad source reg combination p0, oT0, p0 is NOT allowed";
string VS_579 = 
	"vs_2_x "
	"mad r0, p0, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_580_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_580_Desc = "vs_2_x : mad source reg combination p0, oT0, oPos is NOT allowed";
string VS_580 = 
	"vs_2_x "
	"mad r0, p0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_581_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_581_Desc = "vs_2_x : mad source reg combination p0, oT0, oFog is NOT allowed";
string VS_581 = 
	"vs_2_x "
	"mad r0, p0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_582_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_582_Desc = "vs_2_x : mad source reg combination p0, oT0, oPts is NOT allowed";
string VS_582 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, p0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_583_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_583_Desc = "vs_2_x : mad source reg combination p0, oT0, oD0 is NOT allowed";
string VS_583 = 
	"vs_2_x "
	"mad r0, p0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_584_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_584_Desc = "vs_2_x : mad source reg combination p0, oT0, oT0 is NOT allowed";
string VS_584 = 
	"vs_2_x "
	"mad r0, p0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_585_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_585_Desc = "vs_2_x : mad source reg combination oPos, a0, a0 is NOT allowed";
string VS_585 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_586_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_586_Desc = "vs_2_x : mad source reg combination oPos, a0, b0 is NOT allowed";
string VS_586 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, oPos, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_587_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_587_Desc = "vs_2_x : mad source reg combination oPos, a0, i0 is NOT allowed";
string VS_587 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, oPos, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_588_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_588_Desc = "vs_2_x : mad source reg combination oPos, a0, aL is NOT allowed";
string VS_588 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, oPos, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_589_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_589_Desc = "vs_2_x : mad source reg combination oPos, a0, p0 is NOT allowed";
string VS_589 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_590_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_590_Desc = "vs_2_x : mad source reg combination oPos, a0, oPos is NOT allowed";
string VS_590 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_591_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_591_Desc = "vs_2_x : mad source reg combination oPos, a0, oFog is NOT allowed";
string VS_591 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_592_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_592_Desc = "vs_2_x : mad source reg combination oPos, a0, oPts is NOT allowed";
string VS_592 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPos, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_593_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_593_Desc = "vs_2_x : mad source reg combination oPos, a0, oD0 is NOT allowed";
string VS_593 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_594_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_594_Desc = "vs_2_x : mad source reg combination oPos, a0, oT0 is NOT allowed";
string VS_594 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_595_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_595_Desc = "vs_2_x : mad source reg combination oPos, b0, a0 is NOT allowed";
string VS_595 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, oPos, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_596_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_596_Desc = "vs_2_x : mad source reg combination oPos, b0, b0 is NOT allowed";
string VS_596 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_597_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_597_Desc = "vs_2_x : mad source reg combination oPos, b0, i0 is NOT allowed";
string VS_597 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, oPos, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_598_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_598_Desc = "vs_2_x : mad source reg combination oPos, b0, aL is NOT allowed";
string VS_598 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, oPos, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_599_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_599_Desc = "vs_2_x : mad source reg combination oPos, b0, p0 is NOT allowed";
string VS_599 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_600_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_600_Desc = "vs_2_x : mad source reg combination oPos, b0, oPos is NOT allowed";
string VS_600 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_601_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_601_Desc = "vs_2_x : mad source reg combination oPos, b0, oFog is NOT allowed";
string VS_601 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_602_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_602_Desc = "vs_2_x : mad source reg combination oPos, b0, oPts is NOT allowed";
string VS_602 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPos, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_603_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_603_Desc = "vs_2_x : mad source reg combination oPos, b0, oD0 is NOT allowed";
string VS_603 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_604_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_604_Desc = "vs_2_x : mad source reg combination oPos, b0, oT0 is NOT allowed";
string VS_604 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_605_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_605_Desc = "vs_2_x : mad source reg combination oPos, i0, a0 is NOT allowed";
string VS_605 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, oPos, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_606_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_606_Desc = "vs_2_x : mad source reg combination oPos, i0, b0 is NOT allowed";
string VS_606 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_607_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_607_Desc = "vs_2_x : mad source reg combination oPos, i0, i0 is NOT allowed";
string VS_607 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_608_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_608_Desc = "vs_2_x : mad source reg combination oPos, i0, aL is NOT allowed";
string VS_608 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, oPos, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_609_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_609_Desc = "vs_2_x : mad source reg combination oPos, i0, p0 is NOT allowed";
string VS_609 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_610_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_610_Desc = "vs_2_x : mad source reg combination oPos, i0, oPos is NOT allowed";
string VS_610 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_611_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_611_Desc = "vs_2_x : mad source reg combination oPos, i0, oFog is NOT allowed";
string VS_611 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_612_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_612_Desc = "vs_2_x : mad source reg combination oPos, i0, oPts is NOT allowed";
string VS_612 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPos, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_613_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_613_Desc = "vs_2_x : mad source reg combination oPos, i0, oD0 is NOT allowed";
string VS_613 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_614_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_614_Desc = "vs_2_x : mad source reg combination oPos, i0, oT0 is NOT allowed";
string VS_614 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_615_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_615_Desc = "vs_2_x : mad source reg combination oPos, aL, a0 is NOT allowed";
string VS_615 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, oPos, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_616_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_616_Desc = "vs_2_x : mad source reg combination oPos, aL, b0 is NOT allowed";
string VS_616 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, oPos, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_617_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_617_Desc = "vs_2_x : mad source reg combination oPos, aL, i0 is NOT allowed";
string VS_617 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, oPos, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_618_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_618_Desc = "vs_2_x : mad source reg combination oPos, aL, aL is NOT allowed";
string VS_618 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_619_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_619_Desc = "vs_2_x : mad source reg combination oPos, aL, p0 is NOT allowed";
string VS_619 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_620_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_620_Desc = "vs_2_x : mad source reg combination oPos, aL, oPos is NOT allowed";
string VS_620 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_621_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_621_Desc = "vs_2_x : mad source reg combination oPos, aL, oFog is NOT allowed";
string VS_621 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_622_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_622_Desc = "vs_2_x : mad source reg combination oPos, aL, oPts is NOT allowed";
string VS_622 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPos, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_623_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_623_Desc = "vs_2_x : mad source reg combination oPos, aL, oD0 is NOT allowed";
string VS_623 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_624_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_624_Desc = "vs_2_x : mad source reg combination oPos, aL, oT0 is NOT allowed";
string VS_624 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_625_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_625_Desc = "vs_2_x : mad source reg combination oPos, p0, a0 is NOT allowed";
string VS_625 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_626_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_626_Desc = "vs_2_x : mad source reg combination oPos, p0, b0 is NOT allowed";
string VS_626 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_627_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_627_Desc = "vs_2_x : mad source reg combination oPos, p0, i0 is NOT allowed";
string VS_627 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_628_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_628_Desc = "vs_2_x : mad source reg combination oPos, p0, aL is NOT allowed";
string VS_628 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_629_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_629_Desc = "vs_2_x : mad source reg combination oPos, p0, p0 is NOT allowed";
string VS_629 = 
	"vs_2_x "
	"mad r0, oPos, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_630_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_630_Desc = "vs_2_x : mad source reg combination oPos, p0, oPos is NOT allowed";
string VS_630 = 
	"vs_2_x "
	"mad r0, oPos, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_631_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_631_Desc = "vs_2_x : mad source reg combination oPos, p0, oFog is NOT allowed";
string VS_631 = 
	"vs_2_x "
	"mad r0, oPos, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_632_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_632_Desc = "vs_2_x : mad source reg combination oPos, p0, oPts is NOT allowed";
string VS_632 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_633_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_633_Desc = "vs_2_x : mad source reg combination oPos, p0, oD0 is NOT allowed";
string VS_633 = 
	"vs_2_x "
	"mad r0, oPos, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_634_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_634_Desc = "vs_2_x : mad source reg combination oPos, p0, oT0 is NOT allowed";
string VS_634 = 
	"vs_2_x "
	"mad r0, oPos, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_635_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_635_Desc = "vs_2_x : mad source reg combination oPos, oPos, a0 is NOT allowed";
string VS_635 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_636_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_636_Desc = "vs_2_x : mad source reg combination oPos, oPos, b0 is NOT allowed";
string VS_636 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_637_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_637_Desc = "vs_2_x : mad source reg combination oPos, oPos, i0 is NOT allowed";
string VS_637 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_638_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_638_Desc = "vs_2_x : mad source reg combination oPos, oPos, aL is NOT allowed";
string VS_638 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_639_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_639_Desc = "vs_2_x : mad source reg combination oPos, oPos, p0 is NOT allowed";
string VS_639 = 
	"vs_2_x "
	"mad r0, oPos, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_640_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_640_Desc = "vs_2_x : mad source reg combination oPos, oPos, oPos is NOT allowed";
string VS_640 = 
	"vs_2_x "
	"mad r0, oPos, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_641_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_641_Desc = "vs_2_x : mad source reg combination oPos, oPos, oFog is NOT allowed";
string VS_641 = 
	"vs_2_x "
	"mad r0, oPos, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_642_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_642_Desc = "vs_2_x : mad source reg combination oPos, oPos, oPts is NOT allowed";
string VS_642 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_643_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_643_Desc = "vs_2_x : mad source reg combination oPos, oPos, oD0 is NOT allowed";
string VS_643 = 
	"vs_2_x "
	"mad r0, oPos, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_644_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_644_Desc = "vs_2_x : mad source reg combination oPos, oPos, oT0 is NOT allowed";
string VS_644 = 
	"vs_2_x "
	"mad r0, oPos, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_645_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_645_Desc = "vs_2_x : mad source reg combination oPos, oFog, a0 is NOT allowed";
string VS_645 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_646_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_646_Desc = "vs_2_x : mad source reg combination oPos, oFog, b0 is NOT allowed";
string VS_646 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_647_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_647_Desc = "vs_2_x : mad source reg combination oPos, oFog, i0 is NOT allowed";
string VS_647 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_648_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_648_Desc = "vs_2_x : mad source reg combination oPos, oFog, aL is NOT allowed";
string VS_648 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_649_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_649_Desc = "vs_2_x : mad source reg combination oPos, oFog, p0 is NOT allowed";
string VS_649 = 
	"vs_2_x "
	"mad r0, oPos, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_650_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_650_Desc = "vs_2_x : mad source reg combination oPos, oFog, oPos is NOT allowed";
string VS_650 = 
	"vs_2_x "
	"mad r0, oPos, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_651_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_651_Desc = "vs_2_x : mad source reg combination oPos, oFog, oFog is NOT allowed";
string VS_651 = 
	"vs_2_x "
	"mad r0, oPos, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_652_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_652_Desc = "vs_2_x : mad source reg combination oPos, oFog, oPts is NOT allowed";
string VS_652 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_653_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_653_Desc = "vs_2_x : mad source reg combination oPos, oFog, oD0 is NOT allowed";
string VS_653 = 
	"vs_2_x "
	"mad r0, oPos, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_654_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_654_Desc = "vs_2_x : mad source reg combination oPos, oFog, oT0 is NOT allowed";
string VS_654 = 
	"vs_2_x "
	"mad r0, oPos, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_655_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_655_Desc = "vs_2_x : mad source reg combination oPos, oPts, a0 is NOT allowed";
string VS_655 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPos, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_656_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_656_Desc = "vs_2_x : mad source reg combination oPos, oPts, b0 is NOT allowed";
string VS_656 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPos, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_657_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_657_Desc = "vs_2_x : mad source reg combination oPos, oPts, i0 is NOT allowed";
string VS_657 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPos, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_658_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_658_Desc = "vs_2_x : mad source reg combination oPos, oPts, aL is NOT allowed";
string VS_658 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPos, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_659_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_659_Desc = "vs_2_x : mad source reg combination oPos, oPts, p0 is NOT allowed";
string VS_659 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_660_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_660_Desc = "vs_2_x : mad source reg combination oPos, oPts, oPos is NOT allowed";
string VS_660 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_661_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_661_Desc = "vs_2_x : mad source reg combination oPos, oPts, oFog is NOT allowed";
string VS_661 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_662_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_662_Desc = "vs_2_x : mad source reg combination oPos, oPts, oPts is NOT allowed";
string VS_662 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_663_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_663_Desc = "vs_2_x : mad source reg combination oPos, oPts, oD0 is NOT allowed";
string VS_663 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_664_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_664_Desc = "vs_2_x : mad source reg combination oPos, oPts, oT0 is NOT allowed";
string VS_664 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_665_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_665_Desc = "vs_2_x : mad source reg combination oPos, oD0, a0 is NOT allowed";
string VS_665 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_666_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_666_Desc = "vs_2_x : mad source reg combination oPos, oD0, b0 is NOT allowed";
string VS_666 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_667_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_667_Desc = "vs_2_x : mad source reg combination oPos, oD0, i0 is NOT allowed";
string VS_667 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_668_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_668_Desc = "vs_2_x : mad source reg combination oPos, oD0, aL is NOT allowed";
string VS_668 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_669_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_669_Desc = "vs_2_x : mad source reg combination oPos, oD0, p0 is NOT allowed";
string VS_669 = 
	"vs_2_x "
	"mad r0, oPos, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_670_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_670_Desc = "vs_2_x : mad source reg combination oPos, oD0, oPos is NOT allowed";
string VS_670 = 
	"vs_2_x "
	"mad r0, oPos, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_671_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_671_Desc = "vs_2_x : mad source reg combination oPos, oD0, oFog is NOT allowed";
string VS_671 = 
	"vs_2_x "
	"mad r0, oPos, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_672_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_672_Desc = "vs_2_x : mad source reg combination oPos, oD0, oPts is NOT allowed";
string VS_672 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_673_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_673_Desc = "vs_2_x : mad source reg combination oPos, oD0, oD0 is NOT allowed";
string VS_673 = 
	"vs_2_x "
	"mad r0, oPos, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_674_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_674_Desc = "vs_2_x : mad source reg combination oPos, oD0, oT0 is NOT allowed";
string VS_674 = 
	"vs_2_x "
	"mad r0, oPos, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_675_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_675_Desc = "vs_2_x : mad source reg combination oPos, oT0, a0 is NOT allowed";
string VS_675 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oPos, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_676_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_676_Desc = "vs_2_x : mad source reg combination oPos, oT0, b0 is NOT allowed";
string VS_676 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oPos, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_677_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_677_Desc = "vs_2_x : mad source reg combination oPos, oT0, i0 is NOT allowed";
string VS_677 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oPos, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_678_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_678_Desc = "vs_2_x : mad source reg combination oPos, oT0, aL is NOT allowed";
string VS_678 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oPos, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_679_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_679_Desc = "vs_2_x : mad source reg combination oPos, oT0, p0 is NOT allowed";
string VS_679 = 
	"vs_2_x "
	"mad r0, oPos, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_680_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_680_Desc = "vs_2_x : mad source reg combination oPos, oT0, oPos is NOT allowed";
string VS_680 = 
	"vs_2_x "
	"mad r0, oPos, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_681_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_681_Desc = "vs_2_x : mad source reg combination oPos, oT0, oFog is NOT allowed";
string VS_681 = 
	"vs_2_x "
	"mad r0, oPos, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_682_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_682_Desc = "vs_2_x : mad source reg combination oPos, oT0, oPts is NOT allowed";
string VS_682 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPos, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_683_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_683_Desc = "vs_2_x : mad source reg combination oPos, oT0, oD0 is NOT allowed";
string VS_683 = 
	"vs_2_x "
	"mad r0, oPos, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_684_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_684_Desc = "vs_2_x : mad source reg combination oPos, oT0, oT0 is NOT allowed";
string VS_684 = 
	"vs_2_x "
	"mad r0, oPos, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_685_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_685_Desc = "vs_2_x : mad source reg combination oFog, a0, a0 is NOT allowed";
string VS_685 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_686_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_686_Desc = "vs_2_x : mad source reg combination oFog, a0, b0 is NOT allowed";
string VS_686 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, oFog, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_687_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_687_Desc = "vs_2_x : mad source reg combination oFog, a0, i0 is NOT allowed";
string VS_687 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, oFog, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_688_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_688_Desc = "vs_2_x : mad source reg combination oFog, a0, aL is NOT allowed";
string VS_688 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, oFog, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_689_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_689_Desc = "vs_2_x : mad source reg combination oFog, a0, p0 is NOT allowed";
string VS_689 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_690_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_690_Desc = "vs_2_x : mad source reg combination oFog, a0, oPos is NOT allowed";
string VS_690 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_691_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_691_Desc = "vs_2_x : mad source reg combination oFog, a0, oFog is NOT allowed";
string VS_691 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_692_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_692_Desc = "vs_2_x : mad source reg combination oFog, a0, oPts is NOT allowed";
string VS_692 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oFog, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_693_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_693_Desc = "vs_2_x : mad source reg combination oFog, a0, oD0 is NOT allowed";
string VS_693 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_694_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_694_Desc = "vs_2_x : mad source reg combination oFog, a0, oT0 is NOT allowed";
string VS_694 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_695_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_695_Desc = "vs_2_x : mad source reg combination oFog, b0, a0 is NOT allowed";
string VS_695 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, oFog, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_696_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_696_Desc = "vs_2_x : mad source reg combination oFog, b0, b0 is NOT allowed";
string VS_696 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_697_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_697_Desc = "vs_2_x : mad source reg combination oFog, b0, i0 is NOT allowed";
string VS_697 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, oFog, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_698_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_698_Desc = "vs_2_x : mad source reg combination oFog, b0, aL is NOT allowed";
string VS_698 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, oFog, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_699_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_699_Desc = "vs_2_x : mad source reg combination oFog, b0, p0 is NOT allowed";
string VS_699 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_700_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_700_Desc = "vs_2_x : mad source reg combination oFog, b0, oPos is NOT allowed";
string VS_700 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_701_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_701_Desc = "vs_2_x : mad source reg combination oFog, b0, oFog is NOT allowed";
string VS_701 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_702_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_702_Desc = "vs_2_x : mad source reg combination oFog, b0, oPts is NOT allowed";
string VS_702 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oFog, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_703_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_703_Desc = "vs_2_x : mad source reg combination oFog, b0, oD0 is NOT allowed";
string VS_703 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_704_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_704_Desc = "vs_2_x : mad source reg combination oFog, b0, oT0 is NOT allowed";
string VS_704 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_705_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_705_Desc = "vs_2_x : mad source reg combination oFog, i0, a0 is NOT allowed";
string VS_705 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, oFog, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_706_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_706_Desc = "vs_2_x : mad source reg combination oFog, i0, b0 is NOT allowed";
string VS_706 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_707_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_707_Desc = "vs_2_x : mad source reg combination oFog, i0, i0 is NOT allowed";
string VS_707 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_708_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_708_Desc = "vs_2_x : mad source reg combination oFog, i0, aL is NOT allowed";
string VS_708 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, oFog, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_709_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_709_Desc = "vs_2_x : mad source reg combination oFog, i0, p0 is NOT allowed";
string VS_709 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_710_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_710_Desc = "vs_2_x : mad source reg combination oFog, i0, oPos is NOT allowed";
string VS_710 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_711_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_711_Desc = "vs_2_x : mad source reg combination oFog, i0, oFog is NOT allowed";
string VS_711 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_712_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_712_Desc = "vs_2_x : mad source reg combination oFog, i0, oPts is NOT allowed";
string VS_712 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oFog, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_713_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_713_Desc = "vs_2_x : mad source reg combination oFog, i0, oD0 is NOT allowed";
string VS_713 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_714_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_714_Desc = "vs_2_x : mad source reg combination oFog, i0, oT0 is NOT allowed";
string VS_714 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_715_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_715_Desc = "vs_2_x : mad source reg combination oFog, aL, a0 is NOT allowed";
string VS_715 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, oFog, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_716_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_716_Desc = "vs_2_x : mad source reg combination oFog, aL, b0 is NOT allowed";
string VS_716 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, oFog, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_717_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_717_Desc = "vs_2_x : mad source reg combination oFog, aL, i0 is NOT allowed";
string VS_717 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, oFog, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_718_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_718_Desc = "vs_2_x : mad source reg combination oFog, aL, aL is NOT allowed";
string VS_718 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_719_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_719_Desc = "vs_2_x : mad source reg combination oFog, aL, p0 is NOT allowed";
string VS_719 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_720_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_720_Desc = "vs_2_x : mad source reg combination oFog, aL, oPos is NOT allowed";
string VS_720 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_721_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_721_Desc = "vs_2_x : mad source reg combination oFog, aL, oFog is NOT allowed";
string VS_721 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_722_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_722_Desc = "vs_2_x : mad source reg combination oFog, aL, oPts is NOT allowed";
string VS_722 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oFog, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_723_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_723_Desc = "vs_2_x : mad source reg combination oFog, aL, oD0 is NOT allowed";
string VS_723 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_724_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_724_Desc = "vs_2_x : mad source reg combination oFog, aL, oT0 is NOT allowed";
string VS_724 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_725_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_725_Desc = "vs_2_x : mad source reg combination oFog, p0, a0 is NOT allowed";
string VS_725 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_726_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_726_Desc = "vs_2_x : mad source reg combination oFog, p0, b0 is NOT allowed";
string VS_726 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_727_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_727_Desc = "vs_2_x : mad source reg combination oFog, p0, i0 is NOT allowed";
string VS_727 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_728_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_728_Desc = "vs_2_x : mad source reg combination oFog, p0, aL is NOT allowed";
string VS_728 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_729_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_729_Desc = "vs_2_x : mad source reg combination oFog, p0, p0 is NOT allowed";
string VS_729 = 
	"vs_2_x "
	"mad r0, oFog, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_730_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_730_Desc = "vs_2_x : mad source reg combination oFog, p0, oPos is NOT allowed";
string VS_730 = 
	"vs_2_x "
	"mad r0, oFog, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_731_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_731_Desc = "vs_2_x : mad source reg combination oFog, p0, oFog is NOT allowed";
string VS_731 = 
	"vs_2_x "
	"mad r0, oFog, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_732_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_732_Desc = "vs_2_x : mad source reg combination oFog, p0, oPts is NOT allowed";
string VS_732 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_733_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_733_Desc = "vs_2_x : mad source reg combination oFog, p0, oD0 is NOT allowed";
string VS_733 = 
	"vs_2_x "
	"mad r0, oFog, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_734_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_734_Desc = "vs_2_x : mad source reg combination oFog, p0, oT0 is NOT allowed";
string VS_734 = 
	"vs_2_x "
	"mad r0, oFog, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_735_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_735_Desc = "vs_2_x : mad source reg combination oFog, oPos, a0 is NOT allowed";
string VS_735 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_736_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_736_Desc = "vs_2_x : mad source reg combination oFog, oPos, b0 is NOT allowed";
string VS_736 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_737_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_737_Desc = "vs_2_x : mad source reg combination oFog, oPos, i0 is NOT allowed";
string VS_737 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_738_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_738_Desc = "vs_2_x : mad source reg combination oFog, oPos, aL is NOT allowed";
string VS_738 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_739_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_739_Desc = "vs_2_x : mad source reg combination oFog, oPos, p0 is NOT allowed";
string VS_739 = 
	"vs_2_x "
	"mad r0, oFog, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_740_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_740_Desc = "vs_2_x : mad source reg combination oFog, oPos, oPos is NOT allowed";
string VS_740 = 
	"vs_2_x "
	"mad r0, oFog, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_741_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_741_Desc = "vs_2_x : mad source reg combination oFog, oPos, oFog is NOT allowed";
string VS_741 = 
	"vs_2_x "
	"mad r0, oFog, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_742_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_742_Desc = "vs_2_x : mad source reg combination oFog, oPos, oPts is NOT allowed";
string VS_742 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_743_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_743_Desc = "vs_2_x : mad source reg combination oFog, oPos, oD0 is NOT allowed";
string VS_743 = 
	"vs_2_x "
	"mad r0, oFog, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_744_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_744_Desc = "vs_2_x : mad source reg combination oFog, oPos, oT0 is NOT allowed";
string VS_744 = 
	"vs_2_x "
	"mad r0, oFog, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_745_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_745_Desc = "vs_2_x : mad source reg combination oFog, oFog, a0 is NOT allowed";
string VS_745 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_746_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_746_Desc = "vs_2_x : mad source reg combination oFog, oFog, b0 is NOT allowed";
string VS_746 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_747_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_747_Desc = "vs_2_x : mad source reg combination oFog, oFog, i0 is NOT allowed";
string VS_747 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_748_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_748_Desc = "vs_2_x : mad source reg combination oFog, oFog, aL is NOT allowed";
string VS_748 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_749_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_749_Desc = "vs_2_x : mad source reg combination oFog, oFog, p0 is NOT allowed";
string VS_749 = 
	"vs_2_x "
	"mad r0, oFog, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_750_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_750_Desc = "vs_2_x : mad source reg combination oFog, oFog, oPos is NOT allowed";
string VS_750 = 
	"vs_2_x "
	"mad r0, oFog, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_751_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_751_Desc = "vs_2_x : mad source reg combination oFog, oFog, oFog is NOT allowed";
string VS_751 = 
	"vs_2_x "
	"mad r0, oFog, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_752_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_752_Desc = "vs_2_x : mad source reg combination oFog, oFog, oPts is NOT allowed";
string VS_752 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_753_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_753_Desc = "vs_2_x : mad source reg combination oFog, oFog, oD0 is NOT allowed";
string VS_753 = 
	"vs_2_x "
	"mad r0, oFog, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_754_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_754_Desc = "vs_2_x : mad source reg combination oFog, oFog, oT0 is NOT allowed";
string VS_754 = 
	"vs_2_x "
	"mad r0, oFog, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_755_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_755_Desc = "vs_2_x : mad source reg combination oFog, oPts, a0 is NOT allowed";
string VS_755 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oFog, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_756_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_756_Desc = "vs_2_x : mad source reg combination oFog, oPts, b0 is NOT allowed";
string VS_756 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oFog, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_757_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_757_Desc = "vs_2_x : mad source reg combination oFog, oPts, i0 is NOT allowed";
string VS_757 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oFog, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_758_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_758_Desc = "vs_2_x : mad source reg combination oFog, oPts, aL is NOT allowed";
string VS_758 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oFog, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_759_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_759_Desc = "vs_2_x : mad source reg combination oFog, oPts, p0 is NOT allowed";
string VS_759 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_760_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_760_Desc = "vs_2_x : mad source reg combination oFog, oPts, oPos is NOT allowed";
string VS_760 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_761_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_761_Desc = "vs_2_x : mad source reg combination oFog, oPts, oFog is NOT allowed";
string VS_761 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_762_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_762_Desc = "vs_2_x : mad source reg combination oFog, oPts, oPts is NOT allowed";
string VS_762 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_763_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_763_Desc = "vs_2_x : mad source reg combination oFog, oPts, oD0 is NOT allowed";
string VS_763 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_764_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_764_Desc = "vs_2_x : mad source reg combination oFog, oPts, oT0 is NOT allowed";
string VS_764 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_765_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_765_Desc = "vs_2_x : mad source reg combination oFog, oD0, a0 is NOT allowed";
string VS_765 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_766_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_766_Desc = "vs_2_x : mad source reg combination oFog, oD0, b0 is NOT allowed";
string VS_766 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_767_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_767_Desc = "vs_2_x : mad source reg combination oFog, oD0, i0 is NOT allowed";
string VS_767 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_768_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_768_Desc = "vs_2_x : mad source reg combination oFog, oD0, aL is NOT allowed";
string VS_768 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_769_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_769_Desc = "vs_2_x : mad source reg combination oFog, oD0, p0 is NOT allowed";
string VS_769 = 
	"vs_2_x "
	"mad r0, oFog, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_770_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_770_Desc = "vs_2_x : mad source reg combination oFog, oD0, oPos is NOT allowed";
string VS_770 = 
	"vs_2_x "
	"mad r0, oFog, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_771_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_771_Desc = "vs_2_x : mad source reg combination oFog, oD0, oFog is NOT allowed";
string VS_771 = 
	"vs_2_x "
	"mad r0, oFog, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_772_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_772_Desc = "vs_2_x : mad source reg combination oFog, oD0, oPts is NOT allowed";
string VS_772 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_773_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_773_Desc = "vs_2_x : mad source reg combination oFog, oD0, oD0 is NOT allowed";
string VS_773 = 
	"vs_2_x "
	"mad r0, oFog, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_774_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_774_Desc = "vs_2_x : mad source reg combination oFog, oD0, oT0 is NOT allowed";
string VS_774 = 
	"vs_2_x "
	"mad r0, oFog, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_775_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_775_Desc = "vs_2_x : mad source reg combination oFog, oT0, a0 is NOT allowed";
string VS_775 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oFog, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_776_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_776_Desc = "vs_2_x : mad source reg combination oFog, oT0, b0 is NOT allowed";
string VS_776 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oFog, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_777_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_777_Desc = "vs_2_x : mad source reg combination oFog, oT0, i0 is NOT allowed";
string VS_777 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oFog, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_778_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_778_Desc = "vs_2_x : mad source reg combination oFog, oT0, aL is NOT allowed";
string VS_778 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oFog, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_779_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_779_Desc = "vs_2_x : mad source reg combination oFog, oT0, p0 is NOT allowed";
string VS_779 = 
	"vs_2_x "
	"mad r0, oFog, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_780_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_780_Desc = "vs_2_x : mad source reg combination oFog, oT0, oPos is NOT allowed";
string VS_780 = 
	"vs_2_x "
	"mad r0, oFog, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_781_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_781_Desc = "vs_2_x : mad source reg combination oFog, oT0, oFog is NOT allowed";
string VS_781 = 
	"vs_2_x "
	"mad r0, oFog, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_782_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_782_Desc = "vs_2_x : mad source reg combination oFog, oT0, oPts is NOT allowed";
string VS_782 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oFog, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_783_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_783_Desc = "vs_2_x : mad source reg combination oFog, oT0, oD0 is NOT allowed";
string VS_783 = 
	"vs_2_x "
	"mad r0, oFog, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_784_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_784_Desc = "vs_2_x : mad source reg combination oFog, oT0, oT0 is NOT allowed";
string VS_784 = 
	"vs_2_x "
	"mad r0, oFog, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_785_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_785_Desc = "vs_2_x : mad source reg combination oPts, a0, a0 is NOT allowed";
string VS_785 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_786_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_786_Desc = "vs_2_x : mad source reg combination oPts, a0, b0 is NOT allowed";
string VS_786 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_787_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_787_Desc = "vs_2_x : mad source reg combination oPts, a0, i0 is NOT allowed";
string VS_787 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_788_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_788_Desc = "vs_2_x : mad source reg combination oPts, a0, aL is NOT allowed";
string VS_788 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, oPts, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_789_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_789_Desc = "vs_2_x : mad source reg combination oPts, a0, p0 is NOT allowed";
string VS_789 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_790_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_790_Desc = "vs_2_x : mad source reg combination oPts, a0, oPos is NOT allowed";
string VS_790 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_791_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_791_Desc = "vs_2_x : mad source reg combination oPts, a0, oFog is NOT allowed";
string VS_791 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_792_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_792_Desc = "vs_2_x : mad source reg combination oPts, a0, oPts is NOT allowed";
string VS_792 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_793_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_793_Desc = "vs_2_x : mad source reg combination oPts, a0, oD0 is NOT allowed";
string VS_793 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_794_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_794_Desc = "vs_2_x : mad source reg combination oPts, a0, oT0 is NOT allowed";
string VS_794 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_795_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_795_Desc = "vs_2_x : mad source reg combination oPts, b0, a0 is NOT allowed";
string VS_795 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_796_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_796_Desc = "vs_2_x : mad source reg combination oPts, b0, b0 is NOT allowed";
string VS_796 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_797_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_797_Desc = "vs_2_x : mad source reg combination oPts, b0, i0 is NOT allowed";
string VS_797 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_798_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_798_Desc = "vs_2_x : mad source reg combination oPts, b0, aL is NOT allowed";
string VS_798 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_799_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_799_Desc = "vs_2_x : mad source reg combination oPts, b0, p0 is NOT allowed";
string VS_799 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_800_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_800_Desc = "vs_2_x : mad source reg combination oPts, b0, oPos is NOT allowed";
string VS_800 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_801_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_801_Desc = "vs_2_x : mad source reg combination oPts, b0, oFog is NOT allowed";
string VS_801 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_802_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_802_Desc = "vs_2_x : mad source reg combination oPts, b0, oPts is NOT allowed";
string VS_802 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_803_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_803_Desc = "vs_2_x : mad source reg combination oPts, b0, oD0 is NOT allowed";
string VS_803 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_804_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_804_Desc = "vs_2_x : mad source reg combination oPts, b0, oT0 is NOT allowed";
string VS_804 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_805_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_805_Desc = "vs_2_x : mad source reg combination oPts, i0, a0 is NOT allowed";
string VS_805 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_806_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_806_Desc = "vs_2_x : mad source reg combination oPts, i0, b0 is NOT allowed";
string VS_806 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_807_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_807_Desc = "vs_2_x : mad source reg combination oPts, i0, i0 is NOT allowed";
string VS_807 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_808_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_808_Desc = "vs_2_x : mad source reg combination oPts, i0, aL is NOT allowed";
string VS_808 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_809_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_809_Desc = "vs_2_x : mad source reg combination oPts, i0, p0 is NOT allowed";
string VS_809 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_810_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_810_Desc = "vs_2_x : mad source reg combination oPts, i0, oPos is NOT allowed";
string VS_810 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_811_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_811_Desc = "vs_2_x : mad source reg combination oPts, i0, oFog is NOT allowed";
string VS_811 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_812_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_812_Desc = "vs_2_x : mad source reg combination oPts, i0, oPts is NOT allowed";
string VS_812 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_813_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_813_Desc = "vs_2_x : mad source reg combination oPts, i0, oD0 is NOT allowed";
string VS_813 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_814_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_814_Desc = "vs_2_x : mad source reg combination oPts, i0, oT0 is NOT allowed";
string VS_814 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_815_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_815_Desc = "vs_2_x : mad source reg combination oPts, aL, a0 is NOT allowed";
string VS_815 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, oPts, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_816_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_816_Desc = "vs_2_x : mad source reg combination oPts, aL, b0 is NOT allowed";
string VS_816 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_817_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_817_Desc = "vs_2_x : mad source reg combination oPts, aL, i0 is NOT allowed";
string VS_817 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_818_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_818_Desc = "vs_2_x : mad source reg combination oPts, aL, aL is NOT allowed";
string VS_818 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_819_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_819_Desc = "vs_2_x : mad source reg combination oPts, aL, p0 is NOT allowed";
string VS_819 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_820_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_820_Desc = "vs_2_x : mad source reg combination oPts, aL, oPos is NOT allowed";
string VS_820 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_821_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_821_Desc = "vs_2_x : mad source reg combination oPts, aL, oFog is NOT allowed";
string VS_821 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_822_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_822_Desc = "vs_2_x : mad source reg combination oPts, aL, oPts is NOT allowed";
string VS_822 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_823_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_823_Desc = "vs_2_x : mad source reg combination oPts, aL, oD0 is NOT allowed";
string VS_823 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_824_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_824_Desc = "vs_2_x : mad source reg combination oPts, aL, oT0 is NOT allowed";
string VS_824 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_825_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_825_Desc = "vs_2_x : mad source reg combination oPts, p0, a0 is NOT allowed";
string VS_825 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_826_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_826_Desc = "vs_2_x : mad source reg combination oPts, p0, b0 is NOT allowed";
string VS_826 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_827_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_827_Desc = "vs_2_x : mad source reg combination oPts, p0, i0 is NOT allowed";
string VS_827 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_828_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_828_Desc = "vs_2_x : mad source reg combination oPts, p0, aL is NOT allowed";
string VS_828 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_829_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_829_Desc = "vs_2_x : mad source reg combination oPts, p0, p0 is NOT allowed";
string VS_829 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_830_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_830_Desc = "vs_2_x : mad source reg combination oPts, p0, oPos is NOT allowed";
string VS_830 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_831_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_831_Desc = "vs_2_x : mad source reg combination oPts, p0, oFog is NOT allowed";
string VS_831 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_832_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_832_Desc = "vs_2_x : mad source reg combination oPts, p0, oPts is NOT allowed";
string VS_832 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_833_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_833_Desc = "vs_2_x : mad source reg combination oPts, p0, oD0 is NOT allowed";
string VS_833 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_834_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_834_Desc = "vs_2_x : mad source reg combination oPts, p0, oT0 is NOT allowed";
string VS_834 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_835_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_835_Desc = "vs_2_x : mad source reg combination oPts, oPos, a0 is NOT allowed";
string VS_835 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_836_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_836_Desc = "vs_2_x : mad source reg combination oPts, oPos, b0 is NOT allowed";
string VS_836 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_837_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_837_Desc = "vs_2_x : mad source reg combination oPts, oPos, i0 is NOT allowed";
string VS_837 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_838_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_838_Desc = "vs_2_x : mad source reg combination oPts, oPos, aL is NOT allowed";
string VS_838 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_839_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_839_Desc = "vs_2_x : mad source reg combination oPts, oPos, p0 is NOT allowed";
string VS_839 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_840_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_840_Desc = "vs_2_x : mad source reg combination oPts, oPos, oPos is NOT allowed";
string VS_840 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_841_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_841_Desc = "vs_2_x : mad source reg combination oPts, oPos, oFog is NOT allowed";
string VS_841 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_842_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_842_Desc = "vs_2_x : mad source reg combination oPts, oPos, oPts is NOT allowed";
string VS_842 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_843_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_843_Desc = "vs_2_x : mad source reg combination oPts, oPos, oD0 is NOT allowed";
string VS_843 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_844_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_844_Desc = "vs_2_x : mad source reg combination oPts, oPos, oT0 is NOT allowed";
string VS_844 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_845_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_845_Desc = "vs_2_x : mad source reg combination oPts, oFog, a0 is NOT allowed";
string VS_845 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_846_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_846_Desc = "vs_2_x : mad source reg combination oPts, oFog, b0 is NOT allowed";
string VS_846 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_847_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_847_Desc = "vs_2_x : mad source reg combination oPts, oFog, i0 is NOT allowed";
string VS_847 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_848_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_848_Desc = "vs_2_x : mad source reg combination oPts, oFog, aL is NOT allowed";
string VS_848 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_849_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_849_Desc = "vs_2_x : mad source reg combination oPts, oFog, p0 is NOT allowed";
string VS_849 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_850_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_850_Desc = "vs_2_x : mad source reg combination oPts, oFog, oPos is NOT allowed";
string VS_850 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_851_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_851_Desc = "vs_2_x : mad source reg combination oPts, oFog, oFog is NOT allowed";
string VS_851 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_852_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_852_Desc = "vs_2_x : mad source reg combination oPts, oFog, oPts is NOT allowed";
string VS_852 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_853_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_853_Desc = "vs_2_x : mad source reg combination oPts, oFog, oD0 is NOT allowed";
string VS_853 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_854_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_854_Desc = "vs_2_x : mad source reg combination oPts, oFog, oT0 is NOT allowed";
string VS_854 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_855_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_855_Desc = "vs_2_x : mad source reg combination oPts, oPts, a0 is NOT allowed";
string VS_855 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_856_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_856_Desc = "vs_2_x : mad source reg combination oPts, oPts, b0 is NOT allowed";
string VS_856 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_857_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_857_Desc = "vs_2_x : mad source reg combination oPts, oPts, i0 is NOT allowed";
string VS_857 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_858_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_858_Desc = "vs_2_x : mad source reg combination oPts, oPts, aL is NOT allowed";
string VS_858 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_859_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_859_Desc = "vs_2_x : mad source reg combination oPts, oPts, p0 is NOT allowed";
string VS_859 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_860_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_860_Desc = "vs_2_x : mad source reg combination oPts, oPts, oPos is NOT allowed";
string VS_860 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_861_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_861_Desc = "vs_2_x : mad source reg combination oPts, oPts, oFog is NOT allowed";
string VS_861 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_862_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_862_Desc = "vs_2_x : mad source reg combination oPts, oPts, oPts is NOT allowed";
string VS_862 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_863_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_863_Desc = "vs_2_x : mad source reg combination oPts, oPts, oD0 is NOT allowed";
string VS_863 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_864_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_864_Desc = "vs_2_x : mad source reg combination oPts, oPts, oT0 is NOT allowed";
string VS_864 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_865_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_865_Desc = "vs_2_x : mad source reg combination oPts, oD0, a0 is NOT allowed";
string VS_865 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_866_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_866_Desc = "vs_2_x : mad source reg combination oPts, oD0, b0 is NOT allowed";
string VS_866 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_867_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_867_Desc = "vs_2_x : mad source reg combination oPts, oD0, i0 is NOT allowed";
string VS_867 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_868_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_868_Desc = "vs_2_x : mad source reg combination oPts, oD0, aL is NOT allowed";
string VS_868 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_869_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_869_Desc = "vs_2_x : mad source reg combination oPts, oD0, p0 is NOT allowed";
string VS_869 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_870_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_870_Desc = "vs_2_x : mad source reg combination oPts, oD0, oPos is NOT allowed";
string VS_870 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_871_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_871_Desc = "vs_2_x : mad source reg combination oPts, oD0, oFog is NOT allowed";
string VS_871 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_872_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_872_Desc = "vs_2_x : mad source reg combination oPts, oD0, oPts is NOT allowed";
string VS_872 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_873_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_873_Desc = "vs_2_x : mad source reg combination oPts, oD0, oD0 is NOT allowed";
string VS_873 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_874_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_874_Desc = "vs_2_x : mad source reg combination oPts, oD0, oT0 is NOT allowed";
string VS_874 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_875_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_875_Desc = "vs_2_x : mad source reg combination oPts, oT0, a0 is NOT allowed";
string VS_875 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oPts, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_876_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_876_Desc = "vs_2_x : mad source reg combination oPts, oT0, b0 is NOT allowed";
string VS_876 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oPts, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_877_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_877_Desc = "vs_2_x : mad source reg combination oPts, oT0, i0 is NOT allowed";
string VS_877 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oPts, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_878_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_878_Desc = "vs_2_x : mad source reg combination oPts, oT0, aL is NOT allowed";
string VS_878 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oPts, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_879_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_879_Desc = "vs_2_x : mad source reg combination oPts, oT0, p0 is NOT allowed";
string VS_879 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_880_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_880_Desc = "vs_2_x : mad source reg combination oPts, oT0, oPos is NOT allowed";
string VS_880 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_881_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_881_Desc = "vs_2_x : mad source reg combination oPts, oT0, oFog is NOT allowed";
string VS_881 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_882_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_882_Desc = "vs_2_x : mad source reg combination oPts, oT0, oPts is NOT allowed";
string VS_882 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_883_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_883_Desc = "vs_2_x : mad source reg combination oPts, oT0, oD0 is NOT allowed";
string VS_883 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_884_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_884_Desc = "vs_2_x : mad source reg combination oPts, oT0, oT0 is NOT allowed";
string VS_884 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oPts, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_885_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_885_Desc = "vs_2_x : mad source reg combination oD0, a0, a0 is NOT allowed";
string VS_885 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_886_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_886_Desc = "vs_2_x : mad source reg combination oD0, a0, b0 is NOT allowed";
string VS_886 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, oD0, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_887_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_887_Desc = "vs_2_x : mad source reg combination oD0, a0, i0 is NOT allowed";
string VS_887 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, oD0, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_888_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_888_Desc = "vs_2_x : mad source reg combination oD0, a0, aL is NOT allowed";
string VS_888 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, oD0, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_889_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_889_Desc = "vs_2_x : mad source reg combination oD0, a0, p0 is NOT allowed";
string VS_889 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_890_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_890_Desc = "vs_2_x : mad source reg combination oD0, a0, oPos is NOT allowed";
string VS_890 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_891_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_891_Desc = "vs_2_x : mad source reg combination oD0, a0, oFog is NOT allowed";
string VS_891 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_892_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_892_Desc = "vs_2_x : mad source reg combination oD0, a0, oPts is NOT allowed";
string VS_892 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oD0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_893_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_893_Desc = "vs_2_x : mad source reg combination oD0, a0, oD0 is NOT allowed";
string VS_893 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_894_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_894_Desc = "vs_2_x : mad source reg combination oD0, a0, oT0 is NOT allowed";
string VS_894 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_895_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_895_Desc = "vs_2_x : mad source reg combination oD0, b0, a0 is NOT allowed";
string VS_895 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, oD0, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_896_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_896_Desc = "vs_2_x : mad source reg combination oD0, b0, b0 is NOT allowed";
string VS_896 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_897_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_897_Desc = "vs_2_x : mad source reg combination oD0, b0, i0 is NOT allowed";
string VS_897 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, oD0, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_898_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_898_Desc = "vs_2_x : mad source reg combination oD0, b0, aL is NOT allowed";
string VS_898 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, oD0, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_899_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_899_Desc = "vs_2_x : mad source reg combination oD0, b0, p0 is NOT allowed";
string VS_899 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_900_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_900_Desc = "vs_2_x : mad source reg combination oD0, b0, oPos is NOT allowed";
string VS_900 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_901_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_901_Desc = "vs_2_x : mad source reg combination oD0, b0, oFog is NOT allowed";
string VS_901 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_902_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_902_Desc = "vs_2_x : mad source reg combination oD0, b0, oPts is NOT allowed";
string VS_902 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oD0, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_903_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_903_Desc = "vs_2_x : mad source reg combination oD0, b0, oD0 is NOT allowed";
string VS_903 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_904_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_904_Desc = "vs_2_x : mad source reg combination oD0, b0, oT0 is NOT allowed";
string VS_904 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_905_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_905_Desc = "vs_2_x : mad source reg combination oD0, i0, a0 is NOT allowed";
string VS_905 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, oD0, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_906_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_906_Desc = "vs_2_x : mad source reg combination oD0, i0, b0 is NOT allowed";
string VS_906 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_907_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_907_Desc = "vs_2_x : mad source reg combination oD0, i0, i0 is NOT allowed";
string VS_907 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_908_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_908_Desc = "vs_2_x : mad source reg combination oD0, i0, aL is NOT allowed";
string VS_908 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, oD0, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_909_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_909_Desc = "vs_2_x : mad source reg combination oD0, i0, p0 is NOT allowed";
string VS_909 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_910_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_910_Desc = "vs_2_x : mad source reg combination oD0, i0, oPos is NOT allowed";
string VS_910 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_911_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_911_Desc = "vs_2_x : mad source reg combination oD0, i0, oFog is NOT allowed";
string VS_911 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_912_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_912_Desc = "vs_2_x : mad source reg combination oD0, i0, oPts is NOT allowed";
string VS_912 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oD0, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_913_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_913_Desc = "vs_2_x : mad source reg combination oD0, i0, oD0 is NOT allowed";
string VS_913 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_914_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_914_Desc = "vs_2_x : mad source reg combination oD0, i0, oT0 is NOT allowed";
string VS_914 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_915_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_915_Desc = "vs_2_x : mad source reg combination oD0, aL, a0 is NOT allowed";
string VS_915 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, oD0, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_916_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_916_Desc = "vs_2_x : mad source reg combination oD0, aL, b0 is NOT allowed";
string VS_916 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, oD0, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_917_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_917_Desc = "vs_2_x : mad source reg combination oD0, aL, i0 is NOT allowed";
string VS_917 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, oD0, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_918_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_918_Desc = "vs_2_x : mad source reg combination oD0, aL, aL is NOT allowed";
string VS_918 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_919_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_919_Desc = "vs_2_x : mad source reg combination oD0, aL, p0 is NOT allowed";
string VS_919 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_920_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_920_Desc = "vs_2_x : mad source reg combination oD0, aL, oPos is NOT allowed";
string VS_920 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_921_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_921_Desc = "vs_2_x : mad source reg combination oD0, aL, oFog is NOT allowed";
string VS_921 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_922_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_922_Desc = "vs_2_x : mad source reg combination oD0, aL, oPts is NOT allowed";
string VS_922 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oD0, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_923_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_923_Desc = "vs_2_x : mad source reg combination oD0, aL, oD0 is NOT allowed";
string VS_923 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_924_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_924_Desc = "vs_2_x : mad source reg combination oD0, aL, oT0 is NOT allowed";
string VS_924 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_925_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_925_Desc = "vs_2_x : mad source reg combination oD0, p0, a0 is NOT allowed";
string VS_925 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_926_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_926_Desc = "vs_2_x : mad source reg combination oD0, p0, b0 is NOT allowed";
string VS_926 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_927_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_927_Desc = "vs_2_x : mad source reg combination oD0, p0, i0 is NOT allowed";
string VS_927 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_928_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_928_Desc = "vs_2_x : mad source reg combination oD0, p0, aL is NOT allowed";
string VS_928 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_929_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_929_Desc = "vs_2_x : mad source reg combination oD0, p0, p0 is NOT allowed";
string VS_929 = 
	"vs_2_x "
	"mad r0, oD0, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_930_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_930_Desc = "vs_2_x : mad source reg combination oD0, p0, oPos is NOT allowed";
string VS_930 = 
	"vs_2_x "
	"mad r0, oD0, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_931_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_931_Desc = "vs_2_x : mad source reg combination oD0, p0, oFog is NOT allowed";
string VS_931 = 
	"vs_2_x "
	"mad r0, oD0, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_932_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_932_Desc = "vs_2_x : mad source reg combination oD0, p0, oPts is NOT allowed";
string VS_932 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_933_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_933_Desc = "vs_2_x : mad source reg combination oD0, p0, oD0 is NOT allowed";
string VS_933 = 
	"vs_2_x "
	"mad r0, oD0, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_934_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_934_Desc = "vs_2_x : mad source reg combination oD0, p0, oT0 is NOT allowed";
string VS_934 = 
	"vs_2_x "
	"mad r0, oD0, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_935_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_935_Desc = "vs_2_x : mad source reg combination oD0, oPos, a0 is NOT allowed";
string VS_935 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_936_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_936_Desc = "vs_2_x : mad source reg combination oD0, oPos, b0 is NOT allowed";
string VS_936 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_937_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_937_Desc = "vs_2_x : mad source reg combination oD0, oPos, i0 is NOT allowed";
string VS_937 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_938_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_938_Desc = "vs_2_x : mad source reg combination oD0, oPos, aL is NOT allowed";
string VS_938 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_939_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_939_Desc = "vs_2_x : mad source reg combination oD0, oPos, p0 is NOT allowed";
string VS_939 = 
	"vs_2_x "
	"mad r0, oD0, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_940_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_940_Desc = "vs_2_x : mad source reg combination oD0, oPos, oPos is NOT allowed";
string VS_940 = 
	"vs_2_x "
	"mad r0, oD0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_941_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_941_Desc = "vs_2_x : mad source reg combination oD0, oPos, oFog is NOT allowed";
string VS_941 = 
	"vs_2_x "
	"mad r0, oD0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_942_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_942_Desc = "vs_2_x : mad source reg combination oD0, oPos, oPts is NOT allowed";
string VS_942 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_943_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_943_Desc = "vs_2_x : mad source reg combination oD0, oPos, oD0 is NOT allowed";
string VS_943 = 
	"vs_2_x "
	"mad r0, oD0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_944_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_944_Desc = "vs_2_x : mad source reg combination oD0, oPos, oT0 is NOT allowed";
string VS_944 = 
	"vs_2_x "
	"mad r0, oD0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_945_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_945_Desc = "vs_2_x : mad source reg combination oD0, oFog, a0 is NOT allowed";
string VS_945 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_946_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_946_Desc = "vs_2_x : mad source reg combination oD0, oFog, b0 is NOT allowed";
string VS_946 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_947_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_947_Desc = "vs_2_x : mad source reg combination oD0, oFog, i0 is NOT allowed";
string VS_947 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_948_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_948_Desc = "vs_2_x : mad source reg combination oD0, oFog, aL is NOT allowed";
string VS_948 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_949_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_949_Desc = "vs_2_x : mad source reg combination oD0, oFog, p0 is NOT allowed";
string VS_949 = 
	"vs_2_x "
	"mad r0, oD0, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_950_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_950_Desc = "vs_2_x : mad source reg combination oD0, oFog, oPos is NOT allowed";
string VS_950 = 
	"vs_2_x "
	"mad r0, oD0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_951_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_951_Desc = "vs_2_x : mad source reg combination oD0, oFog, oFog is NOT allowed";
string VS_951 = 
	"vs_2_x "
	"mad r0, oD0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_952_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_952_Desc = "vs_2_x : mad source reg combination oD0, oFog, oPts is NOT allowed";
string VS_952 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_953_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_953_Desc = "vs_2_x : mad source reg combination oD0, oFog, oD0 is NOT allowed";
string VS_953 = 
	"vs_2_x "
	"mad r0, oD0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_954_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_954_Desc = "vs_2_x : mad source reg combination oD0, oFog, oT0 is NOT allowed";
string VS_954 = 
	"vs_2_x "
	"mad r0, oD0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_955_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_955_Desc = "vs_2_x : mad source reg combination oD0, oPts, a0 is NOT allowed";
string VS_955 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oD0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_956_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_956_Desc = "vs_2_x : mad source reg combination oD0, oPts, b0 is NOT allowed";
string VS_956 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oD0, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_957_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_957_Desc = "vs_2_x : mad source reg combination oD0, oPts, i0 is NOT allowed";
string VS_957 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oD0, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_958_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_958_Desc = "vs_2_x : mad source reg combination oD0, oPts, aL is NOT allowed";
string VS_958 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oD0, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_959_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_959_Desc = "vs_2_x : mad source reg combination oD0, oPts, p0 is NOT allowed";
string VS_959 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_960_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_960_Desc = "vs_2_x : mad source reg combination oD0, oPts, oPos is NOT allowed";
string VS_960 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_961_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_961_Desc = "vs_2_x : mad source reg combination oD0, oPts, oFog is NOT allowed";
string VS_961 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_962_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_962_Desc = "vs_2_x : mad source reg combination oD0, oPts, oPts is NOT allowed";
string VS_962 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_963_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_963_Desc = "vs_2_x : mad source reg combination oD0, oPts, oD0 is NOT allowed";
string VS_963 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_964_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_964_Desc = "vs_2_x : mad source reg combination oD0, oPts, oT0 is NOT allowed";
string VS_964 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_965_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_965_Desc = "vs_2_x : mad source reg combination oD0, oD0, a0 is NOT allowed";
string VS_965 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_966_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_966_Desc = "vs_2_x : mad source reg combination oD0, oD0, b0 is NOT allowed";
string VS_966 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_967_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_967_Desc = "vs_2_x : mad source reg combination oD0, oD0, i0 is NOT allowed";
string VS_967 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_968_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_968_Desc = "vs_2_x : mad source reg combination oD0, oD0, aL is NOT allowed";
string VS_968 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_969_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_969_Desc = "vs_2_x : mad source reg combination oD0, oD0, p0 is NOT allowed";
string VS_969 = 
	"vs_2_x "
	"mad r0, oD0, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_970_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_970_Desc = "vs_2_x : mad source reg combination oD0, oD0, oPos is NOT allowed";
string VS_970 = 
	"vs_2_x "
	"mad r0, oD0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_971_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_971_Desc = "vs_2_x : mad source reg combination oD0, oD0, oFog is NOT allowed";
string VS_971 = 
	"vs_2_x "
	"mad r0, oD0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_972_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_972_Desc = "vs_2_x : mad source reg combination oD0, oD0, oPts is NOT allowed";
string VS_972 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_973_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_973_Desc = "vs_2_x : mad source reg combination oD0, oD0, oD0 is NOT allowed";
string VS_973 = 
	"vs_2_x "
	"mad r0, oD0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_974_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_974_Desc = "vs_2_x : mad source reg combination oD0, oD0, oT0 is NOT allowed";
string VS_974 = 
	"vs_2_x "
	"mad r0, oD0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_975_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_975_Desc = "vs_2_x : mad source reg combination oD0, oT0, a0 is NOT allowed";
string VS_975 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oD0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_976_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_976_Desc = "vs_2_x : mad source reg combination oD0, oT0, b0 is NOT allowed";
string VS_976 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oD0, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_977_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_977_Desc = "vs_2_x : mad source reg combination oD0, oT0, i0 is NOT allowed";
string VS_977 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oD0, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_978_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_978_Desc = "vs_2_x : mad source reg combination oD0, oT0, aL is NOT allowed";
string VS_978 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oD0, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_979_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_979_Desc = "vs_2_x : mad source reg combination oD0, oT0, p0 is NOT allowed";
string VS_979 = 
	"vs_2_x "
	"mad r0, oD0, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_980_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_980_Desc = "vs_2_x : mad source reg combination oD0, oT0, oPos is NOT allowed";
string VS_980 = 
	"vs_2_x "
	"mad r0, oD0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_981_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_981_Desc = "vs_2_x : mad source reg combination oD0, oT0, oFog is NOT allowed";
string VS_981 = 
	"vs_2_x "
	"mad r0, oD0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_982_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_982_Desc = "vs_2_x : mad source reg combination oD0, oT0, oPts is NOT allowed";
string VS_982 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oD0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_983_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_983_Desc = "vs_2_x : mad source reg combination oD0, oT0, oD0 is NOT allowed";
string VS_983 = 
	"vs_2_x "
	"mad r0, oD0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_984_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_984_Desc = "vs_2_x : mad source reg combination oD0, oT0, oT0 is NOT allowed";
string VS_984 = 
	"vs_2_x "
	"mad r0, oD0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_985_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_985_Desc = "vs_2_x : mad source reg combination oT0, a0, a0 is NOT allowed";
string VS_985 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_986_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_986_Desc = "vs_2_x : mad source reg combination oT0, a0, b0 is NOT allowed";
string VS_986 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, oT0, a0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_987_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_987_Desc = "vs_2_x : mad source reg combination oT0, a0, i0 is NOT allowed";
string VS_987 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, oT0, a0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_988_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_988_Desc = "vs_2_x : mad source reg combination oT0, a0, aL is NOT allowed";
string VS_988 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mova aL.x, c0.x "
	"mad r0, oT0, a0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_989_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_989_Desc = "vs_2_x : mad source reg combination oT0, a0, p0 is NOT allowed";
string VS_989 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, a0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_990_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_990_Desc = "vs_2_x : mad source reg combination oT0, a0, oPos is NOT allowed";
string VS_990 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_991_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_991_Desc = "vs_2_x : mad source reg combination oT0, a0, oFog is NOT allowed";
string VS_991 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_992_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_992_Desc = "vs_2_x : mad source reg combination oT0, a0, oPts is NOT allowed";
string VS_992 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oT0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_993_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_993_Desc = "vs_2_x : mad source reg combination oT0, a0, oD0 is NOT allowed";
string VS_993 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_994_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_994_Desc = "vs_2_x : mad source reg combination oT0, a0, oT0 is NOT allowed";
string VS_994 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_995_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_995_Desc = "vs_2_x : mad source reg combination oT0, b0, a0 is NOT allowed";
string VS_995 = 
	"vs_2_x "
	"defb b0, true "
	"mova a0.x, c0.x "
	"mad r0, oT0, b0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_996_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_996_Desc = "vs_2_x : mad source reg combination oT0, b0, b0 is NOT allowed";
string VS_996 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, b0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_997_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_997_Desc = "vs_2_x : mad source reg combination oT0, b0, i0 is NOT allowed";
string VS_997 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"mad r0, oT0, b0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_998_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_998_Desc = "vs_2_x : mad source reg combination oT0, b0, aL is NOT allowed";
string VS_998 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, oT0, b0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_999_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_999_Desc = "vs_2_x : mad source reg combination oT0, b0, p0 is NOT allowed";
string VS_999 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, b0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1000_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1000_Desc = "vs_2_x : mad source reg combination oT0, b0, oPos is NOT allowed";
string VS_1000 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, b0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_1001_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1001_Desc = "vs_2_x : mad source reg combination oT0, b0, oFog is NOT allowed";
string VS_1001 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, b0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_1002_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1002_Desc = "vs_2_x : mad source reg combination oT0, b0, oPts is NOT allowed";
string VS_1002 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oT0, b0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_1003_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1003_Desc = "vs_2_x : mad source reg combination oT0, b0, oD0 is NOT allowed";
string VS_1003 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, b0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1004_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1004_Desc = "vs_2_x : mad source reg combination oT0, b0, oT0 is NOT allowed";
string VS_1004 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, b0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1005_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1005_Desc = "vs_2_x : mad source reg combination oT0, i0, a0 is NOT allowed";
string VS_1005 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova a0.x, c0.x "
	"mad r0, oT0, i0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1006_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1006_Desc = "vs_2_x : mad source reg combination oT0, i0, b0 is NOT allowed";
string VS_1006 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, i0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1007_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1007_Desc = "vs_2_x : mad source reg combination oT0, i0, i0 is NOT allowed";
string VS_1007 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, i0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1008_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1008_Desc = "vs_2_x : mad source reg combination oT0, i0, aL is NOT allowed";
string VS_1008 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, oT0, i0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_1009_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1009_Desc = "vs_2_x : mad source reg combination oT0, i0, p0 is NOT allowed";
string VS_1009 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, i0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1010_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1010_Desc = "vs_2_x : mad source reg combination oT0, i0, oPos is NOT allowed";
string VS_1010 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, i0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_1011_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1011_Desc = "vs_2_x : mad source reg combination oT0, i0, oFog is NOT allowed";
string VS_1011 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, i0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_1012_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1012_Desc = "vs_2_x : mad source reg combination oT0, i0, oPts is NOT allowed";
string VS_1012 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oT0, i0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_1013_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1013_Desc = "vs_2_x : mad source reg combination oT0, i0, oD0 is NOT allowed";
string VS_1013 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, i0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1014_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1014_Desc = "vs_2_x : mad source reg combination oT0, i0, oT0 is NOT allowed";
string VS_1014 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, i0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1015_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1015_Desc = "vs_2_x : mad source reg combination oT0, aL, a0 is NOT allowed";
string VS_1015 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mova a0.x, c0.x "
	"mad r0, oT0, aL, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1016_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1016_Desc = "vs_2_x : mad source reg combination oT0, aL, b0 is NOT allowed";
string VS_1016 = 
	"vs_2_x "
	"defb b0, true "
	"mova aL.x, c0.x "
	"mad r0, oT0, aL, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1017_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1017_Desc = "vs_2_x : mad source reg combination oT0, aL, i0 is NOT allowed";
string VS_1017 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mova aL.x, c0.x "
	"mad r0, oT0, aL, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1018_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1018_Desc = "vs_2_x : mad source reg combination oT0, aL, aL is NOT allowed";
string VS_1018 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, aL, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_1019_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1019_Desc = "vs_2_x : mad source reg combination oT0, aL, p0 is NOT allowed";
string VS_1019 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, aL, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1020_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1020_Desc = "vs_2_x : mad source reg combination oT0, aL, oPos is NOT allowed";
string VS_1020 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, aL, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_1021_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1021_Desc = "vs_2_x : mad source reg combination oT0, aL, oFog is NOT allowed";
string VS_1021 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, aL, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_1022_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1022_Desc = "vs_2_x : mad source reg combination oT0, aL, oPts is NOT allowed";
string VS_1022 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oT0, aL, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_1023_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1023_Desc = "vs_2_x : mad source reg combination oT0, aL, oD0 is NOT allowed";
string VS_1023 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, aL, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1024_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1024_Desc = "vs_2_x : mad source reg combination oT0, aL, oT0 is NOT allowed";
string VS_1024 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, aL, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1025_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1025_Desc = "vs_2_x : mad source reg combination oT0, p0, a0 is NOT allowed";
string VS_1025 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, p0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1026_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1026_Desc = "vs_2_x : mad source reg combination oT0, p0, b0 is NOT allowed";
string VS_1026 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, p0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1027_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1027_Desc = "vs_2_x : mad source reg combination oT0, p0, i0 is NOT allowed";
string VS_1027 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, p0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1028_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1028_Desc = "vs_2_x : mad source reg combination oT0, p0, aL is NOT allowed";
string VS_1028 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, p0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_1029_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1029_Desc = "vs_2_x : mad source reg combination oT0, p0, p0 is NOT allowed";
string VS_1029 = 
	"vs_2_x "
	"mad r0, oT0, p0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1030_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1030_Desc = "vs_2_x : mad source reg combination oT0, p0, oPos is NOT allowed";
string VS_1030 = 
	"vs_2_x "
	"mad r0, oT0, p0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_1031_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1031_Desc = "vs_2_x : mad source reg combination oT0, p0, oFog is NOT allowed";
string VS_1031 = 
	"vs_2_x "
	"mad r0, oT0, p0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_1032_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1032_Desc = "vs_2_x : mad source reg combination oT0, p0, oPts is NOT allowed";
string VS_1032 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, p0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_1033_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1033_Desc = "vs_2_x : mad source reg combination oT0, p0, oD0 is NOT allowed";
string VS_1033 = 
	"vs_2_x "
	"mad r0, oT0, p0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1034_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1034_Desc = "vs_2_x : mad source reg combination oT0, p0, oT0 is NOT allowed";
string VS_1034 = 
	"vs_2_x "
	"mad r0, oT0, p0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1035_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1035_Desc = "vs_2_x : mad source reg combination oT0, oPos, a0 is NOT allowed";
string VS_1035 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1036_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1036_Desc = "vs_2_x : mad source reg combination oT0, oPos, b0 is NOT allowed";
string VS_1036 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, oPos, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1037_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1037_Desc = "vs_2_x : mad source reg combination oT0, oPos, i0 is NOT allowed";
string VS_1037 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, oPos, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1038_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1038_Desc = "vs_2_x : mad source reg combination oT0, oPos, aL is NOT allowed";
string VS_1038 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, oPos, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_1039_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1039_Desc = "vs_2_x : mad source reg combination oT0, oPos, p0 is NOT allowed";
string VS_1039 = 
	"vs_2_x "
	"mad r0, oT0, oPos, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1040_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1040_Desc = "vs_2_x : mad source reg combination oT0, oPos, oPos is NOT allowed";
string VS_1040 = 
	"vs_2_x "
	"mad r0, oT0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_1041_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1041_Desc = "vs_2_x : mad source reg combination oT0, oPos, oFog is NOT allowed";
string VS_1041 = 
	"vs_2_x "
	"mad r0, oT0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_1042_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1042_Desc = "vs_2_x : mad source reg combination oT0, oPos, oPts is NOT allowed";
string VS_1042 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_1043_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1043_Desc = "vs_2_x : mad source reg combination oT0, oPos, oD0 is NOT allowed";
string VS_1043 = 
	"vs_2_x "
	"mad r0, oT0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1044_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1044_Desc = "vs_2_x : mad source reg combination oT0, oPos, oT0 is NOT allowed";
string VS_1044 = 
	"vs_2_x "
	"mad r0, oT0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1045_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1045_Desc = "vs_2_x : mad source reg combination oT0, oFog, a0 is NOT allowed";
string VS_1045 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1046_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1046_Desc = "vs_2_x : mad source reg combination oT0, oFog, b0 is NOT allowed";
string VS_1046 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, oFog, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1047_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1047_Desc = "vs_2_x : mad source reg combination oT0, oFog, i0 is NOT allowed";
string VS_1047 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, oFog, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1048_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1048_Desc = "vs_2_x : mad source reg combination oT0, oFog, aL is NOT allowed";
string VS_1048 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, oFog, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_1049_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1049_Desc = "vs_2_x : mad source reg combination oT0, oFog, p0 is NOT allowed";
string VS_1049 = 
	"vs_2_x "
	"mad r0, oT0, oFog, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1050_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1050_Desc = "vs_2_x : mad source reg combination oT0, oFog, oPos is NOT allowed";
string VS_1050 = 
	"vs_2_x "
	"mad r0, oT0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_1051_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1051_Desc = "vs_2_x : mad source reg combination oT0, oFog, oFog is NOT allowed";
string VS_1051 = 
	"vs_2_x "
	"mad r0, oT0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_1052_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1052_Desc = "vs_2_x : mad source reg combination oT0, oFog, oPts is NOT allowed";
string VS_1052 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_1053_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1053_Desc = "vs_2_x : mad source reg combination oT0, oFog, oD0 is NOT allowed";
string VS_1053 = 
	"vs_2_x "
	"mad r0, oT0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1054_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1054_Desc = "vs_2_x : mad source reg combination oT0, oFog, oT0 is NOT allowed";
string VS_1054 = 
	"vs_2_x "
	"mad r0, oT0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1055_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1055_Desc = "vs_2_x : mad source reg combination oT0, oPts, a0 is NOT allowed";
string VS_1055 = 
	"vs_2_x "
	"dcl ts "
	"mova a0.x, c0.x "
	"mad r0, oT0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1056_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1056_Desc = "vs_2_x : mad source reg combination oT0, oPts, b0 is NOT allowed";
string VS_1056 = 
	"vs_2_x "
	"defb b0, true "
	"dcl ts "
	"mad r0, oT0, oPts, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1057_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1057_Desc = "vs_2_x : mad source reg combination oT0, oPts, i0 is NOT allowed";
string VS_1057 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl ts "
	"mad r0, oT0, oPts, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1058_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1058_Desc = "vs_2_x : mad source reg combination oT0, oPts, aL is NOT allowed";
string VS_1058 = 
	"vs_2_x "
	"dcl ts "
	"mova aL.x, c0.x "
	"mad r0, oT0, oPts, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_1059_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1059_Desc = "vs_2_x : mad source reg combination oT0, oPts, p0 is NOT allowed";
string VS_1059 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, oPts, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1060_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1060_Desc = "vs_2_x : mad source reg combination oT0, oPts, oPos is NOT allowed";
string VS_1060 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_1061_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1061_Desc = "vs_2_x : mad source reg combination oT0, oPts, oFog is NOT allowed";
string VS_1061 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_1062_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1062_Desc = "vs_2_x : mad source reg combination oT0, oPts, oPts is NOT allowed";
string VS_1062 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_1063_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1063_Desc = "vs_2_x : mad source reg combination oT0, oPts, oD0 is NOT allowed";
string VS_1063 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1064_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1064_Desc = "vs_2_x : mad source reg combination oT0, oPts, oT0 is NOT allowed";
string VS_1064 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1065_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1065_Desc = "vs_2_x : mad source reg combination oT0, oD0, a0 is NOT allowed";
string VS_1065 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1066_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1066_Desc = "vs_2_x : mad source reg combination oT0, oD0, b0 is NOT allowed";
string VS_1066 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, oD0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1067_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1067_Desc = "vs_2_x : mad source reg combination oT0, oD0, i0 is NOT allowed";
string VS_1067 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, oD0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1068_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1068_Desc = "vs_2_x : mad source reg combination oT0, oD0, aL is NOT allowed";
string VS_1068 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, oD0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_1069_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1069_Desc = "vs_2_x : mad source reg combination oT0, oD0, p0 is NOT allowed";
string VS_1069 = 
	"vs_2_x "
	"mad r0, oT0, oD0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1070_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1070_Desc = "vs_2_x : mad source reg combination oT0, oD0, oPos is NOT allowed";
string VS_1070 = 
	"vs_2_x "
	"mad r0, oT0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_1071_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1071_Desc = "vs_2_x : mad source reg combination oT0, oD0, oFog is NOT allowed";
string VS_1071 = 
	"vs_2_x "
	"mad r0, oT0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_1072_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1072_Desc = "vs_2_x : mad source reg combination oT0, oD0, oPts is NOT allowed";
string VS_1072 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_1073_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1073_Desc = "vs_2_x : mad source reg combination oT0, oD0, oD0 is NOT allowed";
string VS_1073 = 
	"vs_2_x "
	"mad r0, oT0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1074_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1074_Desc = "vs_2_x : mad source reg combination oT0, oD0, oT0 is NOT allowed";
string VS_1074 = 
	"vs_2_x "
	"mad r0, oT0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1075_Desc = "vs_2_x : mad source reg combination oT0, oT0, a0 is NOT allowed";
string VS_1075 = 
	"vs_2_x "
	"mova a0.x, c0.x "
	"mad r0, oT0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1076_Desc = "vs_2_x : mad source reg combination oT0, oT0, b0 is NOT allowed";
string VS_1076 = 
	"vs_2_x "
	"defb b0, true "
	"mad r0, oT0, oT0, b0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1077_Desc = "vs_2_x : mad source reg combination oT0, oT0, i0 is NOT allowed";
string VS_1077 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"mad r0, oT0, oT0, i0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1078_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1078_Desc = "vs_2_x : mad source reg combination oT0, oT0, aL is NOT allowed";
string VS_1078 = 
	"vs_2_x "
	"mova aL.x, c0.x "
	"mad r0, oT0, oT0, aL "
	"mov oPos, c0 ";

VS_CRITERIA VS_1079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1079_Desc = "vs_2_x : mad source reg combination oT0, oT0, p0 is NOT allowed";
string VS_1079 = 
	"vs_2_x "
	"mad r0, oT0, oT0, p0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1080_Desc = "vs_2_x : mad source reg combination oT0, oT0, oPos is NOT allowed";
string VS_1080 = 
	"vs_2_x "
	"mad r0, oT0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_1081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1081_Desc = "vs_2_x : mad source reg combination oT0, oT0, oFog is NOT allowed";
string VS_1081 = 
	"vs_2_x "
	"mad r0, oT0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_1082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1082_Desc = "vs_2_x : mad source reg combination oT0, oT0, oPts is NOT allowed";
string VS_1082 = 
	"vs_2_x "
	"dcl ts "
	"mad r0, oT0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_1083_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1083_Desc = "vs_2_x : mad source reg combination oT0, oT0, oD0 is NOT allowed";
string VS_1083 = 
	"vs_2_x "
	"mad r0, oT0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1084_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1084_Desc = "vs_2_x : mad source reg combination oT0, oT0, oT0 is NOT allowed";
string VS_1084 = 
	"vs_2_x "
	"mad r0, oT0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1085_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1085_Desc = "vs_2_x : mad dest r# is allowed";
string VS_1085 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1086_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1086_Desc = "vs_2_x : mad dest oPos is allowed";
string VS_1086 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad oPos, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1087_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1087_Desc = "vs_2_x : mad dest oD# is allowed";
string VS_1087 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad oD0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1088_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1088_Desc = "vs_2_x : mad dest oT# is allowed";
string VS_1088 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad oT0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1089_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1089_Desc = "vs_2_x : mad dest v# is NOT allowed";
string VS_1089 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad v0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1090_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1090_Desc = "vs_2_x : mad dest c# is NOT allowed";
string VS_1090 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad c1, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1091_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1091_Desc = "vs_2_x : mad dest a0 is NOT allowed";
string VS_1091 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad a0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1092_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1092_Desc = "vs_2_x : mad dest b# is NOT allowed";
string VS_1092 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad b0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1093_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1093_Desc = "vs_2_x : mad dest i# is NOT allowed";
string VS_1093 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad i0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1094_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1094_Desc = "vs_2_x : mad dest aL is NOT allowed";
string VS_1094 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad aL, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1095_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1095_Desc = "vs_2_x : mad dest p0 is NOT allowed";
string VS_1095 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad p0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1096_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1096_Desc = "vs_2_x : mad write mask .x is allowed";
string VS_1096 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.x, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1097_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1097_Desc = "vs_2_x : mad write mask .y is allowed";
string VS_1097 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.y, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1098_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1098_Desc = "vs_2_x : mad write mask .z is allowed";
string VS_1098 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.z, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1099_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1099_Desc = "vs_2_x : mad write mask .w is allowed";
string VS_1099 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.w, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1100_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1100_Desc = "vs_2_x : mad write mask .xy is allowed";
string VS_1100 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xy, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1101_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1101_Desc = "vs_2_x : mad write mask .yz is allowed";
string VS_1101 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.yz, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1102_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1102_Desc = "vs_2_x : mad write mask .zw is allowed";
string VS_1102 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.zw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1103_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1103_Desc = "vs_2_x : mad write mask .xz is allowed";
string VS_1103 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xz, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1104_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1104_Desc = "vs_2_x : mad write mask .xw is allowed";
string VS_1104 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1105_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1105_Desc = "vs_2_x : mad write mask .yw is allowed";
string VS_1105 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.yw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1106_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1106_Desc = "vs_2_x : mad write mask .xyz is allowed";
string VS_1106 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xyz, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1107_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1107_Desc = "vs_2_x : mad write mask .yzw is allowed";
string VS_1107 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.yzw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1108_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1108_Desc = "vs_2_x : mad write mask .xzw is allowed";
string VS_1108 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xzw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1109_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1109_Desc = "vs_2_x : mad write mask .xyzw is allowed";
string VS_1109 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xyzw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1110_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1110_Desc = "vs_2_x : mad write mask .yx is NOT allowed";
string VS_1110 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.yx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1111_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1111_Desc = "vs_2_x : mad write mask .zx is NOT allowed";
string VS_1111 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.zx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1112_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1112_Desc = "vs_2_x : mad write mask .zy is NOT allowed";
string VS_1112 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.zy, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1113_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1113_Desc = "vs_2_x : mad write mask .wx is NOT allowed";
string VS_1113 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1114_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1114_Desc = "vs_2_x : mad write mask .wz is NOT allowed";
string VS_1114 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wz, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1115_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1115_Desc = "vs_2_x : mad write mask .wy is NOT allowed";
string VS_1115 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wy, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1116_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1116_Desc = "vs_2_x : mad write mask .zyx is NOT allowed";
string VS_1116 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.zyx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1117_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1117_Desc = "vs_2_x : mad write mask .wzy is NOT allowed";
string VS_1117 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wzy, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1118_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1118_Desc = "vs_2_x : mad write mask .wzx is NOT allowed";
string VS_1118 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wzx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1119_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1119_Desc = "vs_2_x : mad write mask .wyx is NOT allowed";
string VS_1119 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wyx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1120_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1120_Desc = "vs_2_x : mad write mask .yxw is NOT allowed";
string VS_1120 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.yxw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1121_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1121_Desc = "vs_2_x : mad write mask .wzyx is NOT allowed";
string VS_1121 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wzyx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1122_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_1122_Desc = "vs_2_x : mad write mask .zxwy is NOT allowed";
string VS_1122 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.zxwy, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_1123_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1123_Desc = "vs_2_x : mad is allowed in a 1 level if b0 block";
string VS_1123 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	mad r0, v0, r0, r1 "
	"endif ";

VS_CRITERIA VS_1124_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1124_Desc = "vs_2_x : mad is allowed in a 8 level if b0 block";
string VS_1124 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	if b0 "
	"		if b0 "
	"			if b0 "
	"				if b0 "
	"					if b0 "
	"						if b0 "
	"							if b0 "
	"								mad r0, v0, r0, r1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_1125_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1125_Desc = "vs_2_x : mad is allowed in a 16 level if b0 block";
string VS_1125 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1126_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1126_Desc = "vs_2_x : mad is NOT allowed in a 24 level if b0 block";
string VS_1126 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1127_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1127_Desc = "vs_2_x : mad is NOT allowed in a 25 level if b0 block";
string VS_1127 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1128_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1128_Desc = "vs_2_x : mad is allowed in a 1 level if b0 nop else block";
string VS_1128 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	mad r0, v0, r0, r1 "
	"endif ";

VS_CRITERIA VS_1129_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1129_Desc = "vs_2_x : mad is allowed in a 8 level if b0 nop else block";
string VS_1129 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		if b0 nop else "
	"			if b0 nop else "
	"				if b0 nop else "
	"					if b0 nop else "
	"						if b0 nop else "
	"							if b0 nop else "
	"								mad r0, v0, r0, r1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_1130_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1130_Desc = "vs_2_x : mad is NOT allowed in a 16 level if b0 nop else block";
string VS_1130 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1131_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1131_Desc = "vs_2_x : mad is NOT allowed in a 24 level if b0 nop else block";
string VS_1131 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1132_Criteria = { false, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1132_Desc = "vs_2_x : mad is NOT allowed in a 25 level if b0 nop else block";
string VS_1132 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1133_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_1133_Desc = "vs_2_x : mad is allowed in a 1 level loop aL, i0 block";
string VS_1133 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	mad r0, v0, r0, r1 "
	"endloop ";

VS_CRITERIA VS_1134_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_1134_Desc = "vs_2_x : mad is allowed in a 2 level loop aL, i0 block";
string VS_1134 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1135_Criteria = { true, false, 0x0200, 0, -1, 3, -1, 0, -1, 0.f };
string VS_1135_Desc = "vs_2_x : mad is allowed in a 3 level loop aL, i0 block";
string VS_1135 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			mad r0, v0, r0, r1 "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1136_Criteria = { true, false, 0x0200, 0, -1, 4, -1, 0, -1, 0.f };
string VS_1136_Desc = "vs_2_x : mad is allowed in a 4 level loop aL, i0 block";
string VS_1136 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				mad r0, v0, r0, r1 "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1137_Criteria = { false, false, 0x0200, 0, -1, 5, -1, 0, -1, 0.f };
string VS_1137_Desc = "vs_2_x : mad is NOT allowed in a 5 level loop aL, i0 block";
string VS_1137 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					mad r0, v0, r0, r1 "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1138_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_1138_Desc = "vs_2_x : mad is allowed in a 1 level rep i0 block";
string VS_1138 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	mad r0, v0, r0, r1 "
	"endrep ";

VS_CRITERIA VS_1139_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_1139_Desc = "vs_2_x : mad is allowed in a 2 level rep i0 block";
string VS_1139 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1140_Criteria = { true, false, 0x0200, 0, -1, 3, -1, 0, -1, 0.f };
string VS_1140_Desc = "vs_2_x : mad is allowed in a 3 level rep i0 block";
string VS_1140 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			mad r0, v0, r0, r1 "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1141_Criteria = { true, false, 0x0200, 0, -1, 4, -1, 0, -1, 0.f };
string VS_1141_Desc = "vs_2_x : mad is allowed in a 4 level rep i0 block";
string VS_1141 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				mad r0, v0, r0, r1 "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1142_Criteria = { false, false, 0x0200, 0, -1, 5, -1, 0, -1, 0.f };
string VS_1142_Desc = "vs_2_x : mad is NOT allowed in a 5 level rep i0 block";
string VS_1142 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					mad r0, v0, r0, r1 "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1143_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1143_Desc = "vs_2_x : mad is allowed in a 1 level if_lt c0.x, c0.y block";
string VS_1143 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	mad r0, v0, r0, r1 "
	"endif ";

VS_CRITERIA VS_1144_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1144_Desc = "vs_2_x : mad is allowed in a 8 level if_lt c0.x, c0.y block";
string VS_1144 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								mad r0, v0, r0, r1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_1145_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1145_Desc = "vs_2_x : mad is allowed in a 16 level if_lt c0.x, c0.y block";
string VS_1145 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								if_lt c0.x, c0.y "
	"									if_lt c0.x, c0.y "
	"										if_lt c0.x, c0.y "
	"											if_lt c0.x, c0.y "
	"												if_lt c0.x, c0.y "
	"													if_lt c0.x, c0.y "
	"														if_lt c0.x, c0.y "
	"															if_lt c0.x, c0.y "
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1146_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1146_Desc = "vs_2_x : mad is allowed in a 24 level if_lt c0.x, c0.y block";
string VS_1146 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								if_lt c0.x, c0.y "
	"									if_lt c0.x, c0.y "
	"										if_lt c0.x, c0.y "
	"											if_lt c0.x, c0.y "
	"												if_lt c0.x, c0.y "
	"													if_lt c0.x, c0.y "
	"														if_lt c0.x, c0.y "
	"															if_lt c0.x, c0.y "
	"																if_lt c0.x, c0.y "
	"																	if_lt c0.x, c0.y "
	"																		if_lt c0.x, c0.y "
	"																			if_lt c0.x, c0.y "
	"																				if_lt c0.x, c0.y "
	"																					if_lt c0.x, c0.y "
	"																						if_lt c0.x, c0.y "
	"																							if_lt c0.x, c0.y "
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1147_Criteria = { false, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1147_Desc = "vs_2_x : mad is NOT allowed in a 25 level if_lt c0.x, c0.y block";
string VS_1147 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		if_lt c0.x, c0.y "
	"			if_lt c0.x, c0.y "
	"				if_lt c0.x, c0.y "
	"					if_lt c0.x, c0.y "
	"						if_lt c0.x, c0.y "
	"							if_lt c0.x, c0.y "
	"								if_lt c0.x, c0.y "
	"									if_lt c0.x, c0.y "
	"										if_lt c0.x, c0.y "
	"											if_lt c0.x, c0.y "
	"												if_lt c0.x, c0.y "
	"													if_lt c0.x, c0.y "
	"														if_lt c0.x, c0.y "
	"															if_lt c0.x, c0.y "
	"																if_lt c0.x, c0.y "
	"																	if_lt c0.x, c0.y "
	"																		if_lt c0.x, c0.y "
	"																			if_lt c0.x, c0.y "
	"																				if_lt c0.x, c0.y "
	"																					if_lt c0.x, c0.y "
	"																						if_lt c0.x, c0.y "
	"																							if_lt c0.x, c0.y "
	"																								if_lt c0.x, c0.y "
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1148_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1148_Desc = "vs_2_x : mad is allowed in a 1 level if_lt c0.x, c0.y nop else block";
string VS_1148 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	mad r0, v0, r0, r1 "
	"endif ";

VS_CRITERIA VS_1149_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1149_Desc = "vs_2_x : mad is allowed in a 8 level if_lt c0.x, c0.y nop else block";
string VS_1149 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								mad r0, v0, r0, r1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_1150_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1150_Desc = "vs_2_x : mad is allowed in a 16 level if_lt c0.x, c0.y nop else block";
string VS_1150 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								if_lt c0.x, c0.y nop else "
	"									if_lt c0.x, c0.y nop else "
	"										if_lt c0.x, c0.y nop else "
	"											if_lt c0.x, c0.y nop else "
	"												if_lt c0.x, c0.y nop else "
	"													if_lt c0.x, c0.y nop else "
	"														if_lt c0.x, c0.y nop else "
	"															if_lt c0.x, c0.y nop else "
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1151_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1151_Desc = "vs_2_x : mad is allowed in a 24 level if_lt c0.x, c0.y nop else block";
string VS_1151 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								if_lt c0.x, c0.y nop else "
	"									if_lt c0.x, c0.y nop else "
	"										if_lt c0.x, c0.y nop else "
	"											if_lt c0.x, c0.y nop else "
	"												if_lt c0.x, c0.y nop else "
	"													if_lt c0.x, c0.y nop else "
	"														if_lt c0.x, c0.y nop else "
	"															if_lt c0.x, c0.y nop else "
	"																if_lt c0.x, c0.y nop else "
	"																	if_lt c0.x, c0.y nop else "
	"																		if_lt c0.x, c0.y nop else "
	"																			if_lt c0.x, c0.y nop else "
	"																				if_lt c0.x, c0.y nop else "
	"																					if_lt c0.x, c0.y nop else "
	"																						if_lt c0.x, c0.y nop else "
	"																							if_lt c0.x, c0.y nop else "
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1152_Criteria = { false, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1152_Desc = "vs_2_x : mad is NOT allowed in a 25 level if_lt c0.x, c0.y nop else block";
string VS_1152 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		if_lt c0.x, c0.y nop else "
	"			if_lt c0.x, c0.y nop else "
	"				if_lt c0.x, c0.y nop else "
	"					if_lt c0.x, c0.y nop else "
	"						if_lt c0.x, c0.y nop else "
	"							if_lt c0.x, c0.y nop else "
	"								if_lt c0.x, c0.y nop else "
	"									if_lt c0.x, c0.y nop else "
	"										if_lt c0.x, c0.y nop else "
	"											if_lt c0.x, c0.y nop else "
	"												if_lt c0.x, c0.y nop else "
	"													if_lt c0.x, c0.y nop else "
	"														if_lt c0.x, c0.y nop else "
	"															if_lt c0.x, c0.y nop else "
	"																if_lt c0.x, c0.y nop else "
	"																	if_lt c0.x, c0.y nop else "
	"																		if_lt c0.x, c0.y nop else "
	"																			if_lt c0.x, c0.y nop else "
	"																				if_lt c0.x, c0.y nop else "
	"																					if_lt c0.x, c0.y nop else "
	"																						if_lt c0.x, c0.y nop else "
	"																							if_lt c0.x, c0.y nop else "
	"																								if_lt c0.x, c0.y nop else "
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1153_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1153_Desc = "vs_2_x : mad is allowed in a 1 level if p0.x block";
string VS_1153 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	mad r0, v0, r0, r1 "
	"endif ";

VS_CRITERIA VS_1154_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1154_Desc = "vs_2_x : mad is allowed in a 8 level if p0.x block";
string VS_1154 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	if p0.x "
	"		if p0.x "
	"			if p0.x "
	"				if p0.x "
	"					if p0.x "
	"						if p0.x "
	"							if p0.x "
	"								mad r0, v0, r0, r1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_1155_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1155_Desc = "vs_2_x : mad is allowed in a 16 level if p0.x block";
string VS_1155 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1156_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1156_Desc = "vs_2_x : mad is allowed in a 24 level if p0.x block";
string VS_1156 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1157_Criteria = { false, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1157_Desc = "vs_2_x : mad is NOT allowed in a 25 level if p0.x block";
string VS_1157 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1158_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1158_Desc = "vs_2_x : mad is allowed in a 1 level if p0.x nop else block";
string VS_1158 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	mad r0, v0, r0, r1 "
	"endif ";

VS_CRITERIA VS_1159_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1159_Desc = "vs_2_x : mad is allowed in a 8 level if p0.x nop else block";
string VS_1159 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		if p0.x nop else "
	"			if p0.x nop else "
	"				if p0.x nop else "
	"					if p0.x nop else "
	"						if p0.x nop else "
	"							if p0.x nop else "
	"								mad r0, v0, r0, r1 "
	"							endif "
	"						endif "
	"					endif "
	"				endif "
	"			endif "
	"		endif "
	"	endif "
	"endif ";

VS_CRITERIA VS_1160_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1160_Desc = "vs_2_x : mad is allowed in a 16 level if p0.x nop else block";
string VS_1160 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1161_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1161_Desc = "vs_2_x : mad is allowed in a 24 level if p0.x nop else block";
string VS_1161 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1162_Criteria = { false, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1162_Desc = "vs_2_x : mad is NOT allowed in a 25 level if p0.x nop else block";
string VS_1162 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1163_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1163_Desc = "vs_2_x : mad is allowed in a 1 level loop aL, i0 break block";
string VS_1163 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	mad r0, v0, r0, r1 "
	"endloop ";

VS_CRITERIA VS_1164_Criteria = { false, false, 0x0200, 24, -1, 8, -1, 0, -1, 0.f };
string VS_1164_Desc = "vs_2_x : mad is NOT allowed in a 8 level loop aL, i0 break block";
string VS_1164 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		loop aL, i0 break "
	"			loop aL, i0 break "
	"				loop aL, i0 break "
	"					loop aL, i0 break "
	"						loop aL, i0 break "
	"							loop aL, i0 break "
	"								mad r0, v0, r0, r1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1165_Criteria = { false, false, 0x0200, 24, -1, 16, -1, 0, -1, 0.f };
string VS_1165_Desc = "vs_2_x : mad is NOT allowed in a 16 level loop aL, i0 break block";
string VS_1165 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1166_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string VS_1166_Desc = "vs_2_x : mad is NOT allowed in a 24 level loop aL, i0 break block";
string VS_1166 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1167_Criteria = { false, false, 0x0200, 24, -1, 25, -1, 0, -1, 0.f };
string VS_1167_Desc = "vs_2_x : mad is NOT allowed in a 25 level loop aL, i0 break block";
string VS_1167 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1168_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1168_Desc = "vs_2_x : mad is allowed in a 1 level rep i0 break block";
string VS_1168 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	mad r0, v0, r0, r1 "
	"endrep ";

VS_CRITERIA VS_1169_Criteria = { false, false, 0x0200, 24, -1, 8, -1, 0, -1, 0.f };
string VS_1169_Desc = "vs_2_x : mad is NOT allowed in a 8 level rep i0 break block";
string VS_1169 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		rep i0 break "
	"			rep i0 break "
	"				rep i0 break "
	"					rep i0 break "
	"						rep i0 break "
	"							rep i0 break "
	"								mad r0, v0, r0, r1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1170_Criteria = { false, false, 0x0200, 24, -1, 16, -1, 0, -1, 0.f };
string VS_1170_Desc = "vs_2_x : mad is NOT allowed in a 16 level rep i0 break block";
string VS_1170 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1171_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string VS_1171_Desc = "vs_2_x : mad is NOT allowed in a 24 level rep i0 break block";
string VS_1171 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1172_Criteria = { false, false, 0x0200, 24, -1, 25, -1, 0, -1, 0.f };
string VS_1172_Desc = "vs_2_x : mad is NOT allowed in a 25 level rep i0 break block";
string VS_1172 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1173_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1173_Desc = "vs_2_x : mad is allowed in a 1 level loop aL, i0 breakp p0.x block";
string VS_1173 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	mad r0, v0, r0, r1 "
	"endloop ";

VS_CRITERIA VS_1174_Criteria = { false, false, 0x0200, 0, -1, 8, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1174_Desc = "vs_2_x : mad is NOT allowed in a 8 level loop aL, i0 breakp p0.x block";
string VS_1174 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		loop aL, i0 breakp p0.x "
	"			loop aL, i0 breakp p0.x "
	"				loop aL, i0 breakp p0.x "
	"					loop aL, i0 breakp p0.x "
	"						loop aL, i0 breakp p0.x "
	"							loop aL, i0 breakp p0.x "
	"								mad r0, v0, r0, r1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1175_Criteria = { false, false, 0x0200, 0, -1, 16, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1175_Desc = "vs_2_x : mad is NOT allowed in a 16 level loop aL, i0 breakp p0.x block";
string VS_1175 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1176_Criteria = { false, false, 0x0200, 0, -1, 24, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1176_Desc = "vs_2_x : mad is NOT allowed in a 24 level loop aL, i0 breakp p0.x block";
string VS_1176 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1177_Criteria = { false, false, 0x0200, 0, -1, 25, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1177_Desc = "vs_2_x : mad is NOT allowed in a 25 level loop aL, i0 breakp p0.x block";
string VS_1177 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1178_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1178_Desc = "vs_2_x : mad is allowed in a 1 level rep i0 breakp p0.x block";
string VS_1178 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	mad r0, v0, r0, r1 "
	"endrep ";

VS_CRITERIA VS_1179_Criteria = { false, false, 0x0200, 0, -1, 8, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1179_Desc = "vs_2_x : mad is NOT allowed in a 8 level rep i0 breakp p0.x block";
string VS_1179 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		rep i0 breakp p0.x "
	"			rep i0 breakp p0.x "
	"				rep i0 breakp p0.x "
	"					rep i0 breakp p0.x "
	"						rep i0 breakp p0.x "
	"							rep i0 breakp p0.x "
	"								mad r0, v0, r0, r1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1180_Criteria = { false, false, 0x0200, 0, -1, 16, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1180_Desc = "vs_2_x : mad is NOT allowed in a 16 level rep i0 breakp p0.x block";
string VS_1180 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1181_Criteria = { false, false, 0x0200, 0, -1, 24, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1181_Desc = "vs_2_x : mad is NOT allowed in a 24 level rep i0 breakp p0.x block";
string VS_1181 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1182_Criteria = { false, false, 0x0200, 0, -1, 25, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1182_Desc = "vs_2_x : mad is NOT allowed in a 25 level rep i0 breakp p0.x block";
string VS_1182 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
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
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1183_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1183_Desc = "vs_2_x : mad is allowed in a 1 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_1183 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	mad r0, v0, r0, r1 "
	"endloop ";

VS_CRITERIA VS_1184_Criteria = { false, false, 0x0200, 24, -1, 8, -1, 0, -1, 0.f };
string VS_1184_Desc = "vs_2_x : mad is NOT allowed in a 8 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_1184 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								mad r0, v0, r0, r1 "
	"							endloop "
	"						endloop "
	"					endloop "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1185_Criteria = { false, false, 0x0200, 24, -1, 16, -1, 0, -1, 0.f };
string VS_1185_Desc = "vs_2_x : mad is NOT allowed in a 16 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_1185 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								loop aL, i0 break_gt c0.x, c0.y "
	"									loop aL, i0 break_gt c0.x, c0.y "
	"										loop aL, i0 break_gt c0.x, c0.y "
	"											loop aL, i0 break_gt c0.x, c0.y "
	"												loop aL, i0 break_gt c0.x, c0.y "
	"													loop aL, i0 break_gt c0.x, c0.y "
	"														loop aL, i0 break_gt c0.x, c0.y "
	"															loop aL, i0 break_gt c0.x, c0.y "
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1186_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string VS_1186_Desc = "vs_2_x : mad is NOT allowed in a 24 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_1186 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								loop aL, i0 break_gt c0.x, c0.y "
	"									loop aL, i0 break_gt c0.x, c0.y "
	"										loop aL, i0 break_gt c0.x, c0.y "
	"											loop aL, i0 break_gt c0.x, c0.y "
	"												loop aL, i0 break_gt c0.x, c0.y "
	"													loop aL, i0 break_gt c0.x, c0.y "
	"														loop aL, i0 break_gt c0.x, c0.y "
	"															loop aL, i0 break_gt c0.x, c0.y "
	"																loop aL, i0 break_gt c0.x, c0.y "
	"																	loop aL, i0 break_gt c0.x, c0.y "
	"																		loop aL, i0 break_gt c0.x, c0.y "
	"																			loop aL, i0 break_gt c0.x, c0.y "
	"																				loop aL, i0 break_gt c0.x, c0.y "
	"																					loop aL, i0 break_gt c0.x, c0.y "
	"																						loop aL, i0 break_gt c0.x, c0.y "
	"																							loop aL, i0 break_gt c0.x, c0.y "
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1187_Criteria = { false, false, 0x0200, 24, -1, 25, -1, 0, -1, 0.f };
string VS_1187_Desc = "vs_2_x : mad is NOT allowed in a 25 level loop aL, i0 break_gt c0.x, c0.y block";
string VS_1187 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_gt c0.x, c0.y "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		loop aL, i0 break_gt c0.x, c0.y "
	"			loop aL, i0 break_gt c0.x, c0.y "
	"				loop aL, i0 break_gt c0.x, c0.y "
	"					loop aL, i0 break_gt c0.x, c0.y "
	"						loop aL, i0 break_gt c0.x, c0.y "
	"							loop aL, i0 break_gt c0.x, c0.y "
	"								loop aL, i0 break_gt c0.x, c0.y "
	"									loop aL, i0 break_gt c0.x, c0.y "
	"										loop aL, i0 break_gt c0.x, c0.y "
	"											loop aL, i0 break_gt c0.x, c0.y "
	"												loop aL, i0 break_gt c0.x, c0.y "
	"													loop aL, i0 break_gt c0.x, c0.y "
	"														loop aL, i0 break_gt c0.x, c0.y "
	"															loop aL, i0 break_gt c0.x, c0.y "
	"																loop aL, i0 break_gt c0.x, c0.y "
	"																	loop aL, i0 break_gt c0.x, c0.y "
	"																		loop aL, i0 break_gt c0.x, c0.y "
	"																			loop aL, i0 break_gt c0.x, c0.y "
	"																				loop aL, i0 break_gt c0.x, c0.y "
	"																					loop aL, i0 break_gt c0.x, c0.y "
	"																						loop aL, i0 break_gt c0.x, c0.y "
	"																							loop aL, i0 break_gt c0.x, c0.y "
	"																								loop aL, i0 break_gt c0.x, c0.y "
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1188_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1188_Desc = "vs_2_x : mad is allowed in a 1 level rep i0 break_gt c0.x, c0.y block";
string VS_1188 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_gt c0.x, c0.y "
	"	mad r0, v0, r0, r1 "
	"endrep ";

VS_CRITERIA VS_1189_Criteria = { false, false, 0x0200, 24, -1, 8, -1, 0, -1, 0.f };
string VS_1189_Desc = "vs_2_x : mad is NOT allowed in a 8 level rep i0 break_gt c0.x, c0.y block";
string VS_1189 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								mad r0, v0, r0, r1 "
	"							endrep "
	"						endrep "
	"					endrep "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1190_Criteria = { false, false, 0x0200, 24, -1, 16, -1, 0, -1, 0.f };
string VS_1190_Desc = "vs_2_x : mad is NOT allowed in a 16 level rep i0 break_gt c0.x, c0.y block";
string VS_1190 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								rep i0 break_gt c0.x, c0.y "
	"									rep i0 break_gt c0.x, c0.y "
	"										rep i0 break_gt c0.x, c0.y "
	"											rep i0 break_gt c0.x, c0.y "
	"												rep i0 break_gt c0.x, c0.y "
	"													rep i0 break_gt c0.x, c0.y "
	"														rep i0 break_gt c0.x, c0.y "
	"															rep i0 break_gt c0.x, c0.y "
	"																mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1191_Criteria = { false, false, 0x0200, 24, -1, 24, -1, 0, -1, 0.f };
string VS_1191_Desc = "vs_2_x : mad is NOT allowed in a 24 level rep i0 break_gt c0.x, c0.y block";
string VS_1191 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								rep i0 break_gt c0.x, c0.y "
	"									rep i0 break_gt c0.x, c0.y "
	"										rep i0 break_gt c0.x, c0.y "
	"											rep i0 break_gt c0.x, c0.y "
	"												rep i0 break_gt c0.x, c0.y "
	"													rep i0 break_gt c0.x, c0.y "
	"														rep i0 break_gt c0.x, c0.y "
	"															rep i0 break_gt c0.x, c0.y "
	"																rep i0 break_gt c0.x, c0.y "
	"																	rep i0 break_gt c0.x, c0.y "
	"																		rep i0 break_gt c0.x, c0.y "
	"																			rep i0 break_gt c0.x, c0.y "
	"																				rep i0 break_gt c0.x, c0.y "
	"																					rep i0 break_gt c0.x, c0.y "
	"																						rep i0 break_gt c0.x, c0.y "
	"																							rep i0 break_gt c0.x, c0.y "
	"																								mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1192_Criteria = { false, false, 0x0200, 24, -1, 25, -1, 0, -1, 0.f };
string VS_1192_Desc = "vs_2_x : mad is NOT allowed in a 25 level rep i0 break_gt c0.x, c0.y block";
string VS_1192 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_gt c0.x, c0.y "
	"	rep i0 break_gt c0.x, c0.y "
	"		rep i0 break_gt c0.x, c0.y "
	"			rep i0 break_gt c0.x, c0.y "
	"				rep i0 break_gt c0.x, c0.y "
	"					rep i0 break_gt c0.x, c0.y "
	"						rep i0 break_gt c0.x, c0.y "
	"							rep i0 break_gt c0.x, c0.y "
	"								rep i0 break_gt c0.x, c0.y "
	"									rep i0 break_gt c0.x, c0.y "
	"										rep i0 break_gt c0.x, c0.y "
	"											rep i0 break_gt c0.x, c0.y "
	"												rep i0 break_gt c0.x, c0.y "
	"													rep i0 break_gt c0.x, c0.y "
	"														rep i0 break_gt c0.x, c0.y "
	"															rep i0 break_gt c0.x, c0.y "
	"																rep i0 break_gt c0.x, c0.y "
	"																	rep i0 break_gt c0.x, c0.y "
	"																		rep i0 break_gt c0.x, c0.y "
	"																			rep i0 break_gt c0.x, c0.y "
	"																				rep i0 break_gt c0.x, c0.y "
	"																					rep i0 break_gt c0.x, c0.y "
	"																						rep i0 break_gt c0.x, c0.y "
	"																							rep i0 break_gt c0.x, c0.y "
	"																								rep i0 break_gt c0.x, c0.y "
	"																									mad r0, v0, r0, r1 "
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

VS_CRITERIA VS_1193_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1193_Desc = "vs_2_x : mad is allowed in a if b0 and if b0 block";
string VS_1193 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1194_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1194_Desc = "vs_2_x : mad is allowed in a if b0 and if b0 nop else block";
string VS_1194 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1195_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_1195_Desc = "vs_2_x : mad is allowed in a if b0 and loop aL, i0 block";
string VS_1195 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1196_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_1196_Desc = "vs_2_x : mad is allowed in a if b0 and rep i0 block";
string VS_1196 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1197_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1197_Desc = "vs_2_x : mad is allowed in a if b0 and if_lt c0.x, c0.y block";
string VS_1197 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1198_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1198_Desc = "vs_2_x : mad is allowed in a if b0 and if_lt c0.x, c0.y nop else block";
string VS_1198 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1199_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1199_Desc = "vs_2_x : mad is allowed in a if b0 and if p0.x block";
string VS_1199 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1200_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1200_Desc = "vs_2_x : mad is allowed in a if b0 and if p0.x nop else block";
string VS_1200 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1201_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1201_Desc = "vs_2_x : mad is allowed in a if b0 and loop aL, i0 break block";
string VS_1201 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1202_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1202_Desc = "vs_2_x : mad is allowed in a if b0 and rep i0 break block";
string VS_1202 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1203_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1203_Desc = "vs_2_x : mad is allowed in a if b0 and loop aL, i0 breakp p0.x block";
string VS_1203 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1204_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1204_Desc = "vs_2_x : mad is allowed in a if b0 and rep i0 breakp p0.x block";
string VS_1204 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1205_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1205_Desc = "vs_2_x : mad is allowed in a if b0 and loop aL, i0 break_gt c0.x, c0.y block";
string VS_1205 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1206_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1206_Desc = "vs_2_x : mad is allowed in a if b0 and rep i0 break_ge c0.x, c0.y block";
string VS_1206 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1207_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1207_Desc = "vs_2_x : mad is allowed in a if b0 nop else and if b0 block";
string VS_1207 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1208_Criteria = { true, false, 0x0200, 0, -1, 0, -1, 0, -1, 0.f };
string VS_1208_Desc = "vs_2_x : mad is allowed in a if b0 nop else and if b0 nop else block";
string VS_1208 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1209_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_1209_Desc = "vs_2_x : mad is allowed in a if b0 nop else and loop aL, i0 block";
string VS_1209 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1210_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_1210_Desc = "vs_2_x : mad is allowed in a if b0 nop else and rep i0 block";
string VS_1210 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1211_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1211_Desc = "vs_2_x : mad is allowed in a if b0 nop else and if_lt c0.x, c0.y block";
string VS_1211 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1212_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1212_Desc = "vs_2_x : mad is allowed in a if b0 nop else and if_lt c0.x, c0.y nop else block";
string VS_1212 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1213_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1213_Desc = "vs_2_x : mad is allowed in a if b0 nop else and if p0.x block";
string VS_1213 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1214_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1214_Desc = "vs_2_x : mad is allowed in a if b0 nop else and if p0.x nop else block";
string VS_1214 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1215_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1215_Desc = "vs_2_x : mad is allowed in a if b0 nop else and loop aL, i0 break block";
string VS_1215 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1216_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1216_Desc = "vs_2_x : mad is allowed in a if b0 nop else and rep i0 break block";
string VS_1216 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1217_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1217_Desc = "vs_2_x : mad is allowed in a if b0 nop else and loop aL, i0 breakp p0.x block";
string VS_1217 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1218_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1218_Desc = "vs_2_x : mad is allowed in a if b0 nop else and rep i0 breakp p0.x block";
string VS_1218 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1219_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1219_Desc = "vs_2_x : mad is allowed in a if b0 nop else and loop aL, i0 break_ge c0.x, c0.y block";
string VS_1219 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1220_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1220_Desc = "vs_2_x : mad is allowed in a if b0 nop else and rep i0 break_ge c0.x, c0.y block";
string VS_1220 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if b0 nop else "
	"	rep i0 break_ge c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1221_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_1221_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and if b0 block";
string VS_1221 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1222_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_1222_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and if b0 nop else block";
string VS_1222 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1223_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_1223_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and loop aL, i0 block";
string VS_1223 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1224_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_1224_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and rep i0 block";
string VS_1224 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1225_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1225_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and if_lt c0.x, c0.y block";
string VS_1225 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1226_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1226_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and if_lt c0.x, c0.y nop else block";
string VS_1226 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1227_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1227_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and if p0.x block";
string VS_1227 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1228_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1228_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and if p0.x nop else block";
string VS_1228 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1229_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1229_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and loop aL, i0 break block";
string VS_1229 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1230_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1230_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and rep i0 break block";
string VS_1230 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1231_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1231_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and loop aL, i0 breakp p0.x block";
string VS_1231 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1232_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1232_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and rep i0 breakp p0.x block";
string VS_1232 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1233_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1233_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and loop aL, i0 break_ge c0.x, c0.y block";
string VS_1233 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1234_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1234_Desc = "vs_2_x : mad is allowed in a loop aL, i0 and rep i0 break_ge c0.x, c0.y block";
string VS_1234 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 "
	"	rep i0 break_ge c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1235_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_1235_Desc = "vs_2_x : mad is allowed in a rep i0 and if b0 block";
string VS_1235 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1236_Criteria = { true, false, 0x0200, 0, -1, 1, -1, 0, -1, 0.f };
string VS_1236_Desc = "vs_2_x : mad is allowed in a rep i0 and if b0 nop else block";
string VS_1236 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1237_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_1237_Desc = "vs_2_x : mad is allowed in a rep i0 and loop aL, i0 block";
string VS_1237 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1238_Criteria = { true, false, 0x0200, 0, -1, 2, -1, 0, -1, 0.f };
string VS_1238_Desc = "vs_2_x : mad is allowed in a rep i0 and rep i0 block";
string VS_1238 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1239_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1239_Desc = "vs_2_x : mad is allowed in a rep i0 and if_lt c0.x, c0.y block";
string VS_1239 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1240_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1240_Desc = "vs_2_x : mad is allowed in a rep i0 and if_lt c0.x, c0.y nop else block";
string VS_1240 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1241_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1241_Desc = "vs_2_x : mad is allowed in a rep i0 and if p0.x block";
string VS_1241 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1242_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1242_Desc = "vs_2_x : mad is allowed in a rep i0 and if p0.x nop else block";
string VS_1242 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1243_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1243_Desc = "vs_2_x : mad is allowed in a rep i0 and loop aL, i0 break block";
string VS_1243 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1244_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1244_Desc = "vs_2_x : mad is allowed in a rep i0 and rep i0 break block";
string VS_1244 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1245_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1245_Desc = "vs_2_x : mad is allowed in a rep i0 and loop aL, i0 breakp p0.x block";
string VS_1245 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1246_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1246_Desc = "vs_2_x : mad is allowed in a rep i0 and rep i0 breakp p0.x block";
string VS_1246 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1247_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1247_Desc = "vs_2_x : mad is allowed in a rep i0 and loop aL, i0 break_lt c0.x, c0.y block";
string VS_1247 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1248_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1248_Desc = "vs_2_x : mad is allowed in a rep i0 and rep i0 break_lt c0.x, c0.y block";
string VS_1248 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 "
	"	rep i0 break_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1249_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1249_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and if b0 block";
string VS_1249 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1250_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1250_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and if b0 nop else block";
string VS_1250 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1251_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1251_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and loop aL, i0 block";
string VS_1251 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1252_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1252_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and rep i0 block";
string VS_1252 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1253_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1253_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and if_lt c0.x, c0.y block";
string VS_1253 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1254_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1254_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string VS_1254 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1255_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1255_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and if p0.x block";
string VS_1255 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1256_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1256_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and if p0.x nop else block";
string VS_1256 = 
	"vs_2_x "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1257_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1257_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and loop aL, i0 break block";
string VS_1257 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1258_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1258_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and rep i0 break block";
string VS_1258 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1259_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1259_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_1259 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1260_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1260_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and rep i0 breakp p0.x block";
string VS_1260 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1261_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1261_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and loop aL, i0 break_lt c0.x, c0.y block";
string VS_1261 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1262_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1262_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y and rep i0 break_lt c0.x, c0.y block";
string VS_1262 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1263_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1263_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and if b0 block";
string VS_1263 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1264_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1264_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and if b0 nop else block";
string VS_1264 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1265_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1265_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 block";
string VS_1265 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1266_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1266_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 block";
string VS_1266 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1267_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1267_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and if_lt c0.x, c0.y block";
string VS_1267 = 
	"vs_2_x "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1268_Criteria = { true, false, 0x0200, 24, -1, 0, -1, 0, -1, 0.f };
string VS_1268_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and if_lt c0.x, c0.y nop else block";
string VS_1268 = 
	"vs_2_x "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1269_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1269_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and if p0.x block";
string VS_1269 = 
	"vs_2_x "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1270_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1270_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and if p0.x nop else block";
string VS_1270 = 
	"vs_2_x "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1271_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1271_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 break block";
string VS_1271 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1272_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1272_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 break block";
string VS_1272 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1273_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1273_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 breakp p0.x block";
string VS_1273 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1274_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1274_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 breakp p0.x block";
string VS_1274 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1275_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1275_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and loop aL, i0 break_le c0.x, c0.y block";
string VS_1275 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1276_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1276_Desc = "vs_2_x : mad is allowed in a if_lt c0.x, c0.y nop else and rep i0 break_le c0.x, c0.y block";
string VS_1276 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if_lt c0.x, c0.y nop else "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1277_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1277_Desc = "vs_2_x : mad is allowed in a if p0.x and if b0 block";
string VS_1277 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1278_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1278_Desc = "vs_2_x : mad is allowed in a if p0.x and if b0 nop else block";
string VS_1278 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1279_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1279_Desc = "vs_2_x : mad is allowed in a if p0.x and loop aL, i0 block";
string VS_1279 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1280_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1280_Desc = "vs_2_x : mad is allowed in a if p0.x and rep i0 block";
string VS_1280 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1281_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1281_Desc = "vs_2_x : mad is allowed in a if p0.x and if_lt c0.x, c0.y block";
string VS_1281 = 
	"vs_2_x "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1282_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1282_Desc = "vs_2_x : mad is allowed in a if p0.x and if_lt c0.x, c0.y nop else block";
string VS_1282 = 
	"vs_2_x "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1283_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1283_Desc = "vs_2_x : mad is allowed in a if p0.x and if p0.x block";
string VS_1283 = 
	"vs_2_x "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1284_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1284_Desc = "vs_2_x : mad is allowed in a if p0.x and if p0.x nop else block";
string VS_1284 = 
	"vs_2_x "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1285_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1285_Desc = "vs_2_x : mad is allowed in a if p0.x and loop aL, i0 break block";
string VS_1285 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1286_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1286_Desc = "vs_2_x : mad is allowed in a if p0.x and rep i0 break block";
string VS_1286 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1287_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1287_Desc = "vs_2_x : mad is allowed in a if p0.x and loop aL, i0 breakp p0.x block";
string VS_1287 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1288_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1288_Desc = "vs_2_x : mad is allowed in a if p0.x and rep i0 breakp p0.x block";
string VS_1288 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1289_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1289_Desc = "vs_2_x : mad is allowed in a if p0.x and loop aL, i0 break_le c0.x, c0.y block";
string VS_1289 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1290_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1290_Desc = "vs_2_x : mad is allowed in a if p0.x and rep i0 break_le c0.x, c0.y block";
string VS_1290 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1291_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1291_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and if b0 block";
string VS_1291 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1292_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1292_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and if b0 nop else block";
string VS_1292 = 
	"vs_2_x "
	"defb b0, true "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1293_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1293_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and loop aL, i0 block";
string VS_1293 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1294_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1294_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and rep i0 block";
string VS_1294 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1295_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1295_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and if_lt c0.x, c0.y block";
string VS_1295 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1296_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1296_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and if_lt c0.x, c0.y nop else block";
string VS_1296 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1297_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1297_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and if p0.x block";
string VS_1297 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1298_Criteria = { true, false, 0x0200, 24, -1, 0, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1298_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and if p0.x nop else block";
string VS_1298 = 
	"vs_2_x "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endif ";

VS_CRITERIA VS_1299_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1299_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and loop aL, i0 break block";
string VS_1299 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1300_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1300_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and rep i0 break block";
string VS_1300 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1301_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1301_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and loop aL, i0 breakp p0.x block";
string VS_1301 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1302_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1302_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string VS_1302 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1303_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1303_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and loop aL, i0 break_eq c0.x, c0.y block";
string VS_1303 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endif ";

VS_CRITERIA VS_1304_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1304_Desc = "vs_2_x : mad is allowed in a if p0.x nop else and rep i0 break_eq c0.x, c0.y block";
string VS_1304 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"if p0.x nop else "
	"	rep i0 break_eq c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_1305_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1305_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and if b0 block";
string VS_1305 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1306_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1306_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and if b0 nop else block";
string VS_1306 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1307_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1307_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and loop aL, i0 block";
string VS_1307 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1308_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1308_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and rep i0 block";
string VS_1308 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1309_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1309_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and if_lt c0.x, c0.y block";
string VS_1309 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1310_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1310_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and if_lt c0.x, c0.y nop else block";
string VS_1310 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1311_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1311_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and if p0.x block";
string VS_1311 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1312_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1312_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and if p0.x nop else block";
string VS_1312 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1313_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1313_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and loop aL, i0 break block";
string VS_1313 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1314_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1314_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and rep i0 break block";
string VS_1314 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1315_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1315_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string VS_1315 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1316_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1316_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string VS_1316 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1317_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1317_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and loop aL, i0 break_ne c0.x, c0.y block";
string VS_1317 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1318_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1318_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break and rep i0 break_ne c0.x, c0.y block";
string VS_1318 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	rep i0 break_ne c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1319_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1319_Desc = "vs_2_x : mad is allowed in a rep i0 break and if b0 block";
string VS_1319 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1320_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1320_Desc = "vs_2_x : mad is allowed in a rep i0 break and if b0 nop else block";
string VS_1320 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1321_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1321_Desc = "vs_2_x : mad is allowed in a rep i0 break and loop aL, i0 block";
string VS_1321 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1322_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1322_Desc = "vs_2_x : mad is allowed in a rep i0 break and rep i0 block";
string VS_1322 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1323_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1323_Desc = "vs_2_x : mad is allowed in a rep i0 break and if_lt c0.x, c0.y block";
string VS_1323 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1324_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1324_Desc = "vs_2_x : mad is allowed in a rep i0 break and if_lt c0.x, c0.y nop else block";
string VS_1324 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1325_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1325_Desc = "vs_2_x : mad is allowed in a rep i0 break and if p0.x block";
string VS_1325 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1326_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1326_Desc = "vs_2_x : mad is allowed in a rep i0 break and if p0.x nop else block";
string VS_1326 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1327_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1327_Desc = "vs_2_x : mad is allowed in a rep i0 break and loop aL, i0 break block";
string VS_1327 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1328_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1328_Desc = "vs_2_x : mad is allowed in a rep i0 break and rep i0 break block";
string VS_1328 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1329_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1329_Desc = "vs_2_x : mad is allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string VS_1329 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1330_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1330_Desc = "vs_2_x : mad is allowed in a rep i0 break and rep i0 breakp p0.x block";
string VS_1330 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1331_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1331_Desc = "vs_2_x : mad is allowed in a rep i0 break and loop aL, i0 break_ne c0.x, c0.y block";
string VS_1331 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1332_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1332_Desc = "vs_2_x : mad is allowed in a rep i0 break and rep i0 break_ne c0.x, c0.y block";
string VS_1332 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 break_ne c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1333_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1333_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and if b0 block";
string VS_1333 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1334_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1334_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string VS_1334 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1335_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1335_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string VS_1335 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1336_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1336_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string VS_1336 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1337_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1337_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and if_lt c0.x, c0.y block";
string VS_1337 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1338_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1338_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and if_lt c0.x, c0.y nop else block";
string VS_1338 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1339_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1339_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string VS_1339 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1340_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1340_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string VS_1340 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1341_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1341_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string VS_1341 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1342_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1342_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string VS_1342 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1343_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1343_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string VS_1343 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1344_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1344_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string VS_1344 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1345_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1345_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_gt c0.x, c0.y block";
string VS_1345 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_gt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1346_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1346_Desc = "vs_2_x : mad is allowed in a loop aL, i0 breakp p0.x and rep i0 break_gt c0.x, c0.y block";
string VS_1346 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_gt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1347_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1347_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and if b0 block";
string VS_1347 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1348_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1348_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string VS_1348 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1349_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1349_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string VS_1349 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1350_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1350_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and rep i0 block";
string VS_1350 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1351_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1351_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and if_lt c0.x, c0.y block";
string VS_1351 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1352_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1352_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and if_lt c0.x, c0.y nop else block";
string VS_1352 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1353_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1353_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and if p0.x block";
string VS_1353 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1354_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1354_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string VS_1354 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1355_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1355_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string VS_1355 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1356_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1356_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and rep i0 break block";
string VS_1356 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1357_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1357_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string VS_1357 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1358_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1358_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string VS_1358 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1359_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1359_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and loop aL, i0 break_le c0.x, c0.y block";
string VS_1359 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1360_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1360_Desc = "vs_2_x : mad is allowed in a rep i0 breakp p0.x and rep i0 break_le c0.x, c0.y block";
string VS_1360 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1361_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1361_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if b0 block";
string VS_1361 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1362_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1362_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if b0 nop else block";
string VS_1362 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1363_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1363_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 block";
string VS_1363 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1364_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1364_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 block";
string VS_1364 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1365_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1365_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if_lt c0.x, c0.y block";
string VS_1365 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1366_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1366_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string VS_1366 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1367_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1367_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if p0.x block";
string VS_1367 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1368_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1368_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and if p0.x nop else block";
string VS_1368 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_1369_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1369_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 break block";
string VS_1369 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1370_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1370_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 break block";
string VS_1370 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1371_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1371_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_1371 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1372_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1372_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 breakp p0.x block";
string VS_1372 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1373_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1373_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and loop aL, i0 break_le c0.x, c0.y block";
string VS_1373 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_1374_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1374_Desc = "vs_2_x : mad is allowed in a loop aL, i0 break_le c0.x, c0.y and rep i0 break_le c0.x, c0.y block";
string VS_1374 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_le c0.x, c0.y "
	"	rep i0 break_le c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_1375_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1375_Desc = "vs_2_x : mad is allowed in a rep i0 break_le c0.x, c0.y and if b0 block";
string VS_1375 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	if b0 "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1376_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1376_Desc = "vs_2_x : mad is allowed in a rep i0 break_le c0.x, c0.y and if b0 nop else block";
string VS_1376 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_le c0.x, c0.y "
	"	if b0 nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1377_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1377_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 block";
string VS_1377 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1378_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1378_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 block";
string VS_1378 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1379_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1379_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and if_lt c0.x, c0.y block";
string VS_1379 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if_lt c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1380_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_1380_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and if_lt c0.x, c0.y nop else block";
string VS_1380 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if_lt c0.x, c0.y nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1381_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1381_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and if p0.x block";
string VS_1381 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if p0.x "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1382_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1382_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and if p0.x nop else block";
string VS_1382 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	if p0.x nop else "
	"		mad r0, v0, r0, r1 "
	"	endif "
	"endrep ";

VS_CRITERIA VS_1383_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1383_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 break block";
string VS_1383 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1384_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1384_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 break block";
string VS_1384 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1385_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1385_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 breakp p0.x block";
string VS_1385 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1386_Criteria = { true, false, 0x0200, 24, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string VS_1386_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 breakp p0.x block";
string VS_1386 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_1387_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1387_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and loop aL, i0 break_ge c0.x, c0.y block";
string VS_1387 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	loop aL, i0 break_ge c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_1388_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_1388_Desc = "vs_2_x : mad is allowed in a rep i0 break_ge c0.x, c0.y and rep i0 break_ge c0.x, c0.y block";
string VS_1388 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_texcoord v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_ge c0.x, c0.y "
	"	rep i0 break_ge c0.x, c0.y "
	"		mad r0, v0, r0, r1 "
	"	endrep "
	"endrep ";


