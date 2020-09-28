#include "Test_Include.fx"

int Test_Count = 333;

string TestInfo
<
	string TestType = "VS";
>
= "vs_1_1_inst_mad";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_1_1 : mad source reg combination v0, r0, r1 is allowed";
string VS_001 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_002_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_1_1 : mad source reg combination v0, r0, c0 is allowed";
string VS_002 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, v0, r0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_1_1 : mad source reg combination v0, c0, r0 is allowed";
string VS_003 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, v0, c0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_1_1 : mad source reg combination r0, v0, r1 is allowed";
string VS_004 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, v0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_1_1 : mad source reg combination r0, v0, c0 is allowed";
string VS_005 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, r0, v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_006_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_1_1 : mad source reg combination r0, r1, v0 is allowed";
string VS_006 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, r1, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_007_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_1_1 : mad source reg combination r0, r1, r2 is allowed";
string VS_007 = 
	"vs_1_1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mov r2, c0 "
	"mad r0, r0, r1, r2 "
	"mov oPos, c0 ";

VS_CRITERIA VS_008_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_1_1 : mad source reg combination r0, r1, c0 is allowed";
string VS_008 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, r1, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_009_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_1_1 : mad source reg combination r0, c0, v0 is allowed";
string VS_009 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, r0, c0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_010_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_1_1 : mad source reg combination r0, c0, r1 is allowed";
string VS_010 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, r0, c0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_011_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_1_1 : mad source reg combination c0, v0, r0 is allowed";
string VS_011 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, c0, v0, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_012_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_1_1 : mad source reg combination c0, r0, v0 is allowed";
string VS_012 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"mad r0, c0, r0, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_013_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_1_1 : mad source reg combination c0, r0, r1 is allowed";
string VS_013 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_014_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_1_1 : mad source reg combination -c0, -r0, -r1 is allowed";
string VS_014 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, -c0, -r0, -r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_015_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_1_1 : mad source reg combination -c0, -r0, r1 is allowed";
string VS_015 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, -c0, -r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_016_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_1_1 : mad source reg combination -c0, r0, -r1 is allowed";
string VS_016 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, -c0, r0, -r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_017_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_1_1 : mad source reg combination -c0, r0, r1 is allowed";
string VS_017 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, -c0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_018_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_1_1 : mad source reg combination c0, -r0, -r1 is allowed";
string VS_018 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, -r0, -r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_019_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_1_1 : mad source reg combination c0, -r0, r1 is allowed";
string VS_019 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, -r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_020_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_1_1 : mad source reg combination c0, r0, -r1 is allowed";
string VS_020 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, -r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_021_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_1_1 : mad source reg combination c0, r0, r1 is allowed";
string VS_021 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_022_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_1_1 : mad source reg combination c0.x, r0, r1 is allowed";
string VS_022 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.x, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_023_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_1_1 : mad source reg combination c0.y, r0, r1 is allowed";
string VS_023 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.y, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_024_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_1_1 : mad source reg combination c0.z, r0, r1 is allowed";
string VS_024 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.z, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_025_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_1_1 : mad source reg combination c0.w, r0, r1 is allowed";
string VS_025 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.w, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_026_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_1_1 : mad source reg combination c0.yzxw, r0, r1 is allowed";
string VS_026 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.yzxw, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_027_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_1_1 : mad source reg combination c0.zxyw, r0, r1 is allowed";
string VS_027 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.zxyw, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_028_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_1_1 : mad source reg combination c0.wzyx, r0, r1 is allowed";
string VS_028 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.wzyx, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_029_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_1_1 : mad source reg combination c0.wyxz, r0, r1 is allowed";
string VS_029 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.wyxz, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_030_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_1_1 : mad source reg combination c0.xzyw, r0, r1 is allowed";
string VS_030 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.xzyw, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_031_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_1_1 : mad source reg combination c0.xywz, r0, r1 is allowed";
string VS_031 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.xywz, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_032_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_1_1 : mad source reg combination c0.zyx, r0, r1 is allowed";
string VS_032 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.zyx, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_033_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_1_1 : mad source reg combination c0.xzy, r0, r1 is allowed";
string VS_033 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.xzy, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_034_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_1_1 : mad source reg combination c0.ywx, r0, r1 is allowed";
string VS_034 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.ywx, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_035_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_1_1 : mad source reg combination c0.yx, r0, r1 is allowed";
string VS_035 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.yx, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_036_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_1_1 : mad source reg combination c0.wz, r0, r1 is allowed";
string VS_036 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.wz, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_037_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_1_1 : mad source reg combination c0.zy, r0, r1 is allowed";
string VS_037 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0.zy, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_038_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_1_1 : mad source reg combination c0, r0.x, r1 is allowed";
string VS_038 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.x, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_039_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_1_1 : mad source reg combination c0, r0.y, r1 is allowed";
string VS_039 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.y, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_040_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_1_1 : mad source reg combination c0, r0.z, r1 is allowed";
string VS_040 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.z, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_041_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_1_1 : mad source reg combination c0, r0.w, r1 is allowed";
string VS_041 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.w, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_042_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_1_1 : mad source reg combination c0, r0.yzxw, r1 is allowed";
string VS_042 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.yzxw, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_043_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_1_1 : mad source reg combination c0, r0.zxyw, r1 is allowed";
string VS_043 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.zxyw, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_044_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_1_1 : mad source reg combination c0, r0.wzyx, r1 is allowed";
string VS_044 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.wzyx, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_045_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_1_1 : mad source reg combination c0, r0.wyxz, r1 is allowed";
string VS_045 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.wyxz, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_046_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_1_1 : mad source reg combination c0, r0.xzyw, r1 is allowed";
string VS_046 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.xzyw, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_047_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_1_1 : mad source reg combination c0, r0.xywz, r1 is allowed";
string VS_047 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.xywz, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_048_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_1_1 : mad source reg combination c0, r0.zyx, r1 is allowed";
string VS_048 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.zyx, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_049_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_1_1 : mad source reg combination c0, r0.xzy, r1 is allowed";
string VS_049 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.xzy, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_050_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_1_1 : mad source reg combination c0, r0.ywx, r1 is allowed";
string VS_050 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.ywx, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_051_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_1_1 : mad source reg combination c0, r0.yx, r1 is allowed";
string VS_051 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.yx, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_052_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_1_1 : mad source reg combination c0, r0.wz, r1 is allowed";
string VS_052 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.wz, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_053_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_1_1 : mad source reg combination c0, r0.zy, r1 is allowed";
string VS_053 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0.zy, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_054_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.x is allowed";
string VS_054 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_055_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.y is allowed";
string VS_055 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.y "
	"mov oPos, c0 ";

VS_CRITERIA VS_056_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.z is allowed";
string VS_056 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.z "
	"mov oPos, c0 ";

VS_CRITERIA VS_057_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.w is allowed";
string VS_057 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.w "
	"mov oPos, c0 ";

VS_CRITERIA VS_058_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.yzxw is allowed";
string VS_058 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.yzxw "
	"mov oPos, c0 ";

VS_CRITERIA VS_059_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.zxyw is allowed";
string VS_059 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.zxyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_060_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.wzyx is allowed";
string VS_060 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.wzyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_061_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.wyxz is allowed";
string VS_061 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.wyxz "
	"mov oPos, c0 ";

VS_CRITERIA VS_062_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.xzyw is allowed";
string VS_062 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.xzyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_063_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.xywz is allowed";
string VS_063 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.xywz "
	"mov oPos, c0 ";

VS_CRITERIA VS_064_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.zyx is allowed";
string VS_064 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.zyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_065_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.xzy is allowed";
string VS_065 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.xzy "
	"mov oPos, c0 ";

VS_CRITERIA VS_066_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.ywx is allowed";
string VS_066 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.ywx "
	"mov oPos, c0 ";

VS_CRITERIA VS_067_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.yx is allowed";
string VS_067 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.yx "
	"mov oPos, c0 ";

VS_CRITERIA VS_068_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.wz is allowed";
string VS_068 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.wz "
	"mov oPos, c0 ";

VS_CRITERIA VS_069_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_1_1 : mad source reg combination c0, r0, r1.zy is allowed";
string VS_069 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, c0, r0, r1.zy "
	"mov oPos, c0 ";

VS_CRITERIA VS_070_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_1_1 : mad source reg combination v0, v1, v2 is NOT allowed";
string VS_070 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"dcl_color v2 "
	"mad r0, v0, v1, v2 "
	"mov oPos, c0 ";

VS_CRITERIA VS_071_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_1_1 : mad source reg combination v0, v1, r0 is NOT allowed";
string VS_071 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mov r0, c0 "
	"mad r0, v0, v1, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_072_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_1_1 : mad source reg combination v0, v1, c0 is NOT allowed";
string VS_072 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mad r0, v0, v1, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_073_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_1_1 : mad source reg combination v0, r0, v1 is NOT allowed";
string VS_073 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mov r0, c0 "
	"mad r0, v0, r0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_074_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_1_1 : mad source reg combination v0, c0, v1 is NOT allowed";
string VS_074 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mad r0, v0, c0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_075_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_1_1 : mad source reg combination v0, c0, c1 is NOT allowed";
string VS_075 = 
	"vs_1_1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mad r0, v0, c0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_076_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_1_1 : mad source reg combination r0, v0, v1 is NOT allowed";
string VS_076 = 
	"vs_1_1 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mov r0, c0 "
	"mad r0, r0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_077_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_1_1 : mad source reg combination r0, c0, c1 is NOT allowed";
string VS_077 = 
	"vs_1_1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, r0, c0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_078_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_1_1 : mad source reg combination c0, v0, v1 is NOT allowed";
string VS_078 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"dcl_fog v1 "
	"mad r0, c0, v0, v1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_079_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_1_1 : mad source reg combination c0, v0, c1 is NOT allowed";
string VS_079 = 
	"vs_1_1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mad r0, c0, v0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_080_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_1_1 : mad source reg combination c0, r0, c1 is NOT allowed";
string VS_080 = 
	"vs_1_1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, r0, c1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_081_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_1_1 : mad source reg combination c0, c1, v0 is NOT allowed";
string VS_081 = 
	"vs_1_1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_depth v0 "
	"mad r0, c0, c1, v0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_082_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_1_1 : mad source reg combination c0, c1, r0 is NOT allowed";
string VS_082 = 
	"vs_1_1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mov r0, c0 "
	"mad r0, c0, c1, r0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_083_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_1_1 : mad source reg combination c0, c1, c2 is NOT allowed";
string VS_083 = 
	"vs_1_1 "
	"def c2, 1, 1, 1, 1 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"mad r0, c0, c1, c2 "
	"mov oPos, c0 ";

VS_CRITERIA VS_084_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_1_1 : mad source reg combination a0, a0, a0 is NOT allowed";
string VS_084 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_085_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_1_1 : mad source reg combination a0, a0, oPos is NOT allowed";
string VS_085 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_086_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_1_1 : mad source reg combination a0, a0, oFog is NOT allowed";
string VS_086 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_087_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_1_1 : mad source reg combination a0, a0, oPts is NOT allowed";
string VS_087 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_088_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_1_1 : mad source reg combination a0, a0, oD0 is NOT allowed";
string VS_088 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_089_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_089_Desc = "vs_1_1 : mad source reg combination a0, a0, oT0 is NOT allowed";
string VS_089 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_090_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_1_1 : mad source reg combination a0, oPos, a0 is NOT allowed";
string VS_090 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_091_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_1_1 : mad source reg combination a0, oPos, oPos is NOT allowed";
string VS_091 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_092_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_1_1 : mad source reg combination a0, oPos, oFog is NOT allowed";
string VS_092 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_093_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_1_1 : mad source reg combination a0, oPos, oPts is NOT allowed";
string VS_093 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_094_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_1_1 : mad source reg combination a0, oPos, oD0 is NOT allowed";
string VS_094 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_095_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_1_1 : mad source reg combination a0, oPos, oT0 is NOT allowed";
string VS_095 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_096_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_1_1 : mad source reg combination a0, oFog, a0 is NOT allowed";
string VS_096 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_097_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_1_1 : mad source reg combination a0, oFog, oPos is NOT allowed";
string VS_097 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_098_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_1_1 : mad source reg combination a0, oFog, oFog is NOT allowed";
string VS_098 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_099_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_1_1 : mad source reg combination a0, oFog, oPts is NOT allowed";
string VS_099 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_100_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_1_1 : mad source reg combination a0, oFog, oD0 is NOT allowed";
string VS_100 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_101_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_1_1 : mad source reg combination a0, oFog, oT0 is NOT allowed";
string VS_101 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_102_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_1_1 : mad source reg combination a0, oPts, a0 is NOT allowed";
string VS_102 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_103_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_1_1 : mad source reg combination a0, oPts, oPos is NOT allowed";
string VS_103 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_104_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_1_1 : mad source reg combination a0, oPts, oFog is NOT allowed";
string VS_104 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_105_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_1_1 : mad source reg combination a0, oPts, oPts is NOT allowed";
string VS_105 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_106_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_1_1 : mad source reg combination a0, oPts, oD0 is NOT allowed";
string VS_106 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_107_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_1_1 : mad source reg combination a0, oPts, oT0 is NOT allowed";
string VS_107 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_108_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_1_1 : mad source reg combination a0, oD0, a0 is NOT allowed";
string VS_108 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_109_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_1_1 : mad source reg combination a0, oD0, oPos is NOT allowed";
string VS_109 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_110_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_1_1 : mad source reg combination a0, oD0, oFog is NOT allowed";
string VS_110 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_111_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_1_1 : mad source reg combination a0, oD0, oPts is NOT allowed";
string VS_111 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_112_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_1_1 : mad source reg combination a0, oD0, oD0 is NOT allowed";
string VS_112 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_113_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_1_1 : mad source reg combination a0, oD0, oT0 is NOT allowed";
string VS_113 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_114_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_1_1 : mad source reg combination a0, oT0, a0 is NOT allowed";
string VS_114 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_115_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_1_1 : mad source reg combination a0, oT0, oPos is NOT allowed";
string VS_115 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_116_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_1_1 : mad source reg combination a0, oT0, oFog is NOT allowed";
string VS_116 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_117_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_1_1 : mad source reg combination a0, oT0, oPts is NOT allowed";
string VS_117 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, a0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_118_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_1_1 : mad source reg combination a0, oT0, oD0 is NOT allowed";
string VS_118 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_119_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_1_1 : mad source reg combination a0, oT0, oT0 is NOT allowed";
string VS_119 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, a0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_120_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_1_1 : mad source reg combination oPos, a0, a0 is NOT allowed";
string VS_120 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oPos, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_121_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_1_1 : mad source reg combination oPos, a0, oPos is NOT allowed";
string VS_121 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oPos, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_122_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_1_1 : mad source reg combination oPos, a0, oFog is NOT allowed";
string VS_122 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oPos, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_123_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_1_1 : mad source reg combination oPos, a0, oPts is NOT allowed";
string VS_123 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPos, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_124_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_1_1 : mad source reg combination oPos, a0, oD0 is NOT allowed";
string VS_124 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oPos, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_125_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_1_1 : mad source reg combination oPos, a0, oT0 is NOT allowed";
string VS_125 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oPos, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_126_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_1_1 : mad source reg combination oPos, oPos, a0 is NOT allowed";
string VS_126 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oPos, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_127_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_1_1 : mad source reg combination oPos, oPos, oPos is NOT allowed";
string VS_127 = 
	"vs_1_1 "
	"mad r0, oPos, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_128_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_1_1 : mad source reg combination oPos, oPos, oFog is NOT allowed";
string VS_128 = 
	"vs_1_1 "
	"mad r0, oPos, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_129_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_1_1 : mad source reg combination oPos, oPos, oPts is NOT allowed";
string VS_129 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPos, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_130_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_130_Desc = "vs_1_1 : mad source reg combination oPos, oPos, oD0 is NOT allowed";
string VS_130 = 
	"vs_1_1 "
	"mad r0, oPos, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_131_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_1_1 : mad source reg combination oPos, oPos, oT0 is NOT allowed";
string VS_131 = 
	"vs_1_1 "
	"mad r0, oPos, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_132_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_1_1 : mad source reg combination oPos, oFog, a0 is NOT allowed";
string VS_132 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oPos, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_133_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_1_1 : mad source reg combination oPos, oFog, oPos is NOT allowed";
string VS_133 = 
	"vs_1_1 "
	"mad r0, oPos, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_134_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_1_1 : mad source reg combination oPos, oFog, oFog is NOT allowed";
string VS_134 = 
	"vs_1_1 "
	"mad r0, oPos, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_135_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_1_1 : mad source reg combination oPos, oFog, oPts is NOT allowed";
string VS_135 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPos, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_136_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_1_1 : mad source reg combination oPos, oFog, oD0 is NOT allowed";
string VS_136 = 
	"vs_1_1 "
	"mad r0, oPos, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_137_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_1_1 : mad source reg combination oPos, oFog, oT0 is NOT allowed";
string VS_137 = 
	"vs_1_1 "
	"mad r0, oPos, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_138_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_1_1 : mad source reg combination oPos, oPts, a0 is NOT allowed";
string VS_138 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPos, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_139_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_1_1 : mad source reg combination oPos, oPts, oPos is NOT allowed";
string VS_139 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPos, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_140_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_1_1 : mad source reg combination oPos, oPts, oFog is NOT allowed";
string VS_140 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPos, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_141_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_1_1 : mad source reg combination oPos, oPts, oPts is NOT allowed";
string VS_141 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPos, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_142_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_1_1 : mad source reg combination oPos, oPts, oD0 is NOT allowed";
string VS_142 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPos, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_143_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_1_1 : mad source reg combination oPos, oPts, oT0 is NOT allowed";
string VS_143 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPos, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_144_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_1_1 : mad source reg combination oPos, oD0, a0 is NOT allowed";
string VS_144 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oPos, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_145_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_1_1 : mad source reg combination oPos, oD0, oPos is NOT allowed";
string VS_145 = 
	"vs_1_1 "
	"mad r0, oPos, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_146_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_1_1 : mad source reg combination oPos, oD0, oFog is NOT allowed";
string VS_146 = 
	"vs_1_1 "
	"mad r0, oPos, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_147_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_1_1 : mad source reg combination oPos, oD0, oPts is NOT allowed";
string VS_147 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPos, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_148_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_1_1 : mad source reg combination oPos, oD0, oD0 is NOT allowed";
string VS_148 = 
	"vs_1_1 "
	"mad r0, oPos, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_149_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_149_Desc = "vs_1_1 : mad source reg combination oPos, oD0, oT0 is NOT allowed";
string VS_149 = 
	"vs_1_1 "
	"mad r0, oPos, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_150_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_1_1 : mad source reg combination oPos, oT0, a0 is NOT allowed";
string VS_150 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oPos, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_151_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_1_1 : mad source reg combination oPos, oT0, oPos is NOT allowed";
string VS_151 = 
	"vs_1_1 "
	"mad r0, oPos, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_152_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_1_1 : mad source reg combination oPos, oT0, oFog is NOT allowed";
string VS_152 = 
	"vs_1_1 "
	"mad r0, oPos, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_153_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_1_1 : mad source reg combination oPos, oT0, oPts is NOT allowed";
string VS_153 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPos, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_154_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_1_1 : mad source reg combination oPos, oT0, oD0 is NOT allowed";
string VS_154 = 
	"vs_1_1 "
	"mad r0, oPos, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_155_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_1_1 : mad source reg combination oPos, oT0, oT0 is NOT allowed";
string VS_155 = 
	"vs_1_1 "
	"mad r0, oPos, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_156_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_156_Desc = "vs_1_1 : mad source reg combination oFog, a0, a0 is NOT allowed";
string VS_156 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oFog, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_157_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_1_1 : mad source reg combination oFog, a0, oPos is NOT allowed";
string VS_157 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oFog, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_158_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_1_1 : mad source reg combination oFog, a0, oFog is NOT allowed";
string VS_158 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oFog, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_159_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_159_Desc = "vs_1_1 : mad source reg combination oFog, a0, oPts is NOT allowed";
string VS_159 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oFog, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_160_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_1_1 : mad source reg combination oFog, a0, oD0 is NOT allowed";
string VS_160 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oFog, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_161_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_1_1 : mad source reg combination oFog, a0, oT0 is NOT allowed";
string VS_161 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oFog, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_162_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_1_1 : mad source reg combination oFog, oPos, a0 is NOT allowed";
string VS_162 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oFog, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_163_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_1_1 : mad source reg combination oFog, oPos, oPos is NOT allowed";
string VS_163 = 
	"vs_1_1 "
	"mad r0, oFog, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_164_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_1_1 : mad source reg combination oFog, oPos, oFog is NOT allowed";
string VS_164 = 
	"vs_1_1 "
	"mad r0, oFog, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_165_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_1_1 : mad source reg combination oFog, oPos, oPts is NOT allowed";
string VS_165 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oFog, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_166_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_1_1 : mad source reg combination oFog, oPos, oD0 is NOT allowed";
string VS_166 = 
	"vs_1_1 "
	"mad r0, oFog, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_167_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_1_1 : mad source reg combination oFog, oPos, oT0 is NOT allowed";
string VS_167 = 
	"vs_1_1 "
	"mad r0, oFog, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_168_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_168_Desc = "vs_1_1 : mad source reg combination oFog, oFog, a0 is NOT allowed";
string VS_168 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oFog, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_169_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_169_Desc = "vs_1_1 : mad source reg combination oFog, oFog, oPos is NOT allowed";
string VS_169 = 
	"vs_1_1 "
	"mad r0, oFog, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_170_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_170_Desc = "vs_1_1 : mad source reg combination oFog, oFog, oFog is NOT allowed";
string VS_170 = 
	"vs_1_1 "
	"mad r0, oFog, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_171_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_171_Desc = "vs_1_1 : mad source reg combination oFog, oFog, oPts is NOT allowed";
string VS_171 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oFog, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_172_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_172_Desc = "vs_1_1 : mad source reg combination oFog, oFog, oD0 is NOT allowed";
string VS_172 = 
	"vs_1_1 "
	"mad r0, oFog, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_173_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_173_Desc = "vs_1_1 : mad source reg combination oFog, oFog, oT0 is NOT allowed";
string VS_173 = 
	"vs_1_1 "
	"mad r0, oFog, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_174_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_174_Desc = "vs_1_1 : mad source reg combination oFog, oPts, a0 is NOT allowed";
string VS_174 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oFog, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_175_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_175_Desc = "vs_1_1 : mad source reg combination oFog, oPts, oPos is NOT allowed";
string VS_175 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oFog, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_176_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_176_Desc = "vs_1_1 : mad source reg combination oFog, oPts, oFog is NOT allowed";
string VS_176 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oFog, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_177_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_177_Desc = "vs_1_1 : mad source reg combination oFog, oPts, oPts is NOT allowed";
string VS_177 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oFog, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_178_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_178_Desc = "vs_1_1 : mad source reg combination oFog, oPts, oD0 is NOT allowed";
string VS_178 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oFog, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_179_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_179_Desc = "vs_1_1 : mad source reg combination oFog, oPts, oT0 is NOT allowed";
string VS_179 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oFog, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_180_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_180_Desc = "vs_1_1 : mad source reg combination oFog, oD0, a0 is NOT allowed";
string VS_180 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oFog, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_181_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_181_Desc = "vs_1_1 : mad source reg combination oFog, oD0, oPos is NOT allowed";
string VS_181 = 
	"vs_1_1 "
	"mad r0, oFog, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_182_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_182_Desc = "vs_1_1 : mad source reg combination oFog, oD0, oFog is NOT allowed";
string VS_182 = 
	"vs_1_1 "
	"mad r0, oFog, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_183_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_183_Desc = "vs_1_1 : mad source reg combination oFog, oD0, oPts is NOT allowed";
string VS_183 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oFog, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_184_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_184_Desc = "vs_1_1 : mad source reg combination oFog, oD0, oD0 is NOT allowed";
string VS_184 = 
	"vs_1_1 "
	"mad r0, oFog, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_185_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_185_Desc = "vs_1_1 : mad source reg combination oFog, oD0, oT0 is NOT allowed";
string VS_185 = 
	"vs_1_1 "
	"mad r0, oFog, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_186_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_186_Desc = "vs_1_1 : mad source reg combination oFog, oT0, a0 is NOT allowed";
string VS_186 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oFog, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_187_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_187_Desc = "vs_1_1 : mad source reg combination oFog, oT0, oPos is NOT allowed";
string VS_187 = 
	"vs_1_1 "
	"mad r0, oFog, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_188_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_188_Desc = "vs_1_1 : mad source reg combination oFog, oT0, oFog is NOT allowed";
string VS_188 = 
	"vs_1_1 "
	"mad r0, oFog, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_189_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_189_Desc = "vs_1_1 : mad source reg combination oFog, oT0, oPts is NOT allowed";
string VS_189 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oFog, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_190_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_190_Desc = "vs_1_1 : mad source reg combination oFog, oT0, oD0 is NOT allowed";
string VS_190 = 
	"vs_1_1 "
	"mad r0, oFog, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_191_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_191_Desc = "vs_1_1 : mad source reg combination oFog, oT0, oT0 is NOT allowed";
string VS_191 = 
	"vs_1_1 "
	"mad r0, oFog, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_192_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_192_Desc = "vs_1_1 : mad source reg combination oPts, a0, a0 is NOT allowed";
string VS_192 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_193_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_193_Desc = "vs_1_1 : mad source reg combination oPts, a0, oPos is NOT allowed";
string VS_193 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_194_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_194_Desc = "vs_1_1 : mad source reg combination oPts, a0, oFog is NOT allowed";
string VS_194 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_195_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_195_Desc = "vs_1_1 : mad source reg combination oPts, a0, oPts is NOT allowed";
string VS_195 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_196_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_196_Desc = "vs_1_1 : mad source reg combination oPts, a0, oD0 is NOT allowed";
string VS_196 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_197_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_197_Desc = "vs_1_1 : mad source reg combination oPts, a0, oT0 is NOT allowed";
string VS_197 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_198_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_198_Desc = "vs_1_1 : mad source reg combination oPts, oPos, a0 is NOT allowed";
string VS_198 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_199_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_199_Desc = "vs_1_1 : mad source reg combination oPts, oPos, oPos is NOT allowed";
string VS_199 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_200_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_200_Desc = "vs_1_1 : mad source reg combination oPts, oPos, oFog is NOT allowed";
string VS_200 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_201_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_201_Desc = "vs_1_1 : mad source reg combination oPts, oPos, oPts is NOT allowed";
string VS_201 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_202_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_202_Desc = "vs_1_1 : mad source reg combination oPts, oPos, oD0 is NOT allowed";
string VS_202 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_203_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_203_Desc = "vs_1_1 : mad source reg combination oPts, oPos, oT0 is NOT allowed";
string VS_203 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_204_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_204_Desc = "vs_1_1 : mad source reg combination oPts, oFog, a0 is NOT allowed";
string VS_204 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_205_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_205_Desc = "vs_1_1 : mad source reg combination oPts, oFog, oPos is NOT allowed";
string VS_205 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_206_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_206_Desc = "vs_1_1 : mad source reg combination oPts, oFog, oFog is NOT allowed";
string VS_206 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_207_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_207_Desc = "vs_1_1 : mad source reg combination oPts, oFog, oPts is NOT allowed";
string VS_207 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_208_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_208_Desc = "vs_1_1 : mad source reg combination oPts, oFog, oD0 is NOT allowed";
string VS_208 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_209_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_209_Desc = "vs_1_1 : mad source reg combination oPts, oFog, oT0 is NOT allowed";
string VS_209 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_210_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_210_Desc = "vs_1_1 : mad source reg combination oPts, oPts, a0 is NOT allowed";
string VS_210 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_211_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_211_Desc = "vs_1_1 : mad source reg combination oPts, oPts, oPos is NOT allowed";
string VS_211 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_212_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_212_Desc = "vs_1_1 : mad source reg combination oPts, oPts, oFog is NOT allowed";
string VS_212 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_213_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_213_Desc = "vs_1_1 : mad source reg combination oPts, oPts, oPts is NOT allowed";
string VS_213 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_214_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_214_Desc = "vs_1_1 : mad source reg combination oPts, oPts, oD0 is NOT allowed";
string VS_214 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_215_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_215_Desc = "vs_1_1 : mad source reg combination oPts, oPts, oT0 is NOT allowed";
string VS_215 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_216_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_216_Desc = "vs_1_1 : mad source reg combination oPts, oD0, a0 is NOT allowed";
string VS_216 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_217_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_217_Desc = "vs_1_1 : mad source reg combination oPts, oD0, oPos is NOT allowed";
string VS_217 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_218_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_218_Desc = "vs_1_1 : mad source reg combination oPts, oD0, oFog is NOT allowed";
string VS_218 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_219_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_219_Desc = "vs_1_1 : mad source reg combination oPts, oD0, oPts is NOT allowed";
string VS_219 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_220_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_220_Desc = "vs_1_1 : mad source reg combination oPts, oD0, oD0 is NOT allowed";
string VS_220 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_221_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_221_Desc = "vs_1_1 : mad source reg combination oPts, oD0, oT0 is NOT allowed";
string VS_221 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_222_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_222_Desc = "vs_1_1 : mad source reg combination oPts, oT0, a0 is NOT allowed";
string VS_222 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oPts, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_223_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_223_Desc = "vs_1_1 : mad source reg combination oPts, oT0, oPos is NOT allowed";
string VS_223 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_224_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_224_Desc = "vs_1_1 : mad source reg combination oPts, oT0, oFog is NOT allowed";
string VS_224 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_225_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_225_Desc = "vs_1_1 : mad source reg combination oPts, oT0, oPts is NOT allowed";
string VS_225 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_226_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_226_Desc = "vs_1_1 : mad source reg combination oPts, oT0, oD0 is NOT allowed";
string VS_226 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_227_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_227_Desc = "vs_1_1 : mad source reg combination oPts, oT0, oT0 is NOT allowed";
string VS_227 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oPts, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_228_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_228_Desc = "vs_1_1 : mad source reg combination oD0, a0, a0 is NOT allowed";
string VS_228 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oD0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_229_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_229_Desc = "vs_1_1 : mad source reg combination oD0, a0, oPos is NOT allowed";
string VS_229 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oD0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_230_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_230_Desc = "vs_1_1 : mad source reg combination oD0, a0, oFog is NOT allowed";
string VS_230 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oD0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_231_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_231_Desc = "vs_1_1 : mad source reg combination oD0, a0, oPts is NOT allowed";
string VS_231 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oD0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_232_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_232_Desc = "vs_1_1 : mad source reg combination oD0, a0, oD0 is NOT allowed";
string VS_232 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oD0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_233_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_233_Desc = "vs_1_1 : mad source reg combination oD0, a0, oT0 is NOT allowed";
string VS_233 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oD0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_234_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_234_Desc = "vs_1_1 : mad source reg combination oD0, oPos, a0 is NOT allowed";
string VS_234 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oD0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_235_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_235_Desc = "vs_1_1 : mad source reg combination oD0, oPos, oPos is NOT allowed";
string VS_235 = 
	"vs_1_1 "
	"mad r0, oD0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_236_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_236_Desc = "vs_1_1 : mad source reg combination oD0, oPos, oFog is NOT allowed";
string VS_236 = 
	"vs_1_1 "
	"mad r0, oD0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_237_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_237_Desc = "vs_1_1 : mad source reg combination oD0, oPos, oPts is NOT allowed";
string VS_237 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oD0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_238_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_238_Desc = "vs_1_1 : mad source reg combination oD0, oPos, oD0 is NOT allowed";
string VS_238 = 
	"vs_1_1 "
	"mad r0, oD0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_239_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_239_Desc = "vs_1_1 : mad source reg combination oD0, oPos, oT0 is NOT allowed";
string VS_239 = 
	"vs_1_1 "
	"mad r0, oD0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_240_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_240_Desc = "vs_1_1 : mad source reg combination oD0, oFog, a0 is NOT allowed";
string VS_240 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oD0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_241_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_241_Desc = "vs_1_1 : mad source reg combination oD0, oFog, oPos is NOT allowed";
string VS_241 = 
	"vs_1_1 "
	"mad r0, oD0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_242_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_242_Desc = "vs_1_1 : mad source reg combination oD0, oFog, oFog is NOT allowed";
string VS_242 = 
	"vs_1_1 "
	"mad r0, oD0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_243_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_243_Desc = "vs_1_1 : mad source reg combination oD0, oFog, oPts is NOT allowed";
string VS_243 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oD0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_244_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_244_Desc = "vs_1_1 : mad source reg combination oD0, oFog, oD0 is NOT allowed";
string VS_244 = 
	"vs_1_1 "
	"mad r0, oD0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_245_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_245_Desc = "vs_1_1 : mad source reg combination oD0, oFog, oT0 is NOT allowed";
string VS_245 = 
	"vs_1_1 "
	"mad r0, oD0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_246_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_246_Desc = "vs_1_1 : mad source reg combination oD0, oPts, a0 is NOT allowed";
string VS_246 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oD0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_247_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_247_Desc = "vs_1_1 : mad source reg combination oD0, oPts, oPos is NOT allowed";
string VS_247 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oD0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_248_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_248_Desc = "vs_1_1 : mad source reg combination oD0, oPts, oFog is NOT allowed";
string VS_248 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oD0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_249_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_249_Desc = "vs_1_1 : mad source reg combination oD0, oPts, oPts is NOT allowed";
string VS_249 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oD0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_250_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_250_Desc = "vs_1_1 : mad source reg combination oD0, oPts, oD0 is NOT allowed";
string VS_250 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oD0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_251_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_251_Desc = "vs_1_1 : mad source reg combination oD0, oPts, oT0 is NOT allowed";
string VS_251 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oD0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_252_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_252_Desc = "vs_1_1 : mad source reg combination oD0, oD0, a0 is NOT allowed";
string VS_252 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oD0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_253_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_253_Desc = "vs_1_1 : mad source reg combination oD0, oD0, oPos is NOT allowed";
string VS_253 = 
	"vs_1_1 "
	"mad r0, oD0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_254_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_254_Desc = "vs_1_1 : mad source reg combination oD0, oD0, oFog is NOT allowed";
string VS_254 = 
	"vs_1_1 "
	"mad r0, oD0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_255_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_255_Desc = "vs_1_1 : mad source reg combination oD0, oD0, oPts is NOT allowed";
string VS_255 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oD0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_256_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_256_Desc = "vs_1_1 : mad source reg combination oD0, oD0, oD0 is NOT allowed";
string VS_256 = 
	"vs_1_1 "
	"mad r0, oD0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_257_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_257_Desc = "vs_1_1 : mad source reg combination oD0, oD0, oT0 is NOT allowed";
string VS_257 = 
	"vs_1_1 "
	"mad r0, oD0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_258_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_258_Desc = "vs_1_1 : mad source reg combination oD0, oT0, a0 is NOT allowed";
string VS_258 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oD0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_259_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_259_Desc = "vs_1_1 : mad source reg combination oD0, oT0, oPos is NOT allowed";
string VS_259 = 
	"vs_1_1 "
	"mad r0, oD0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_260_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_260_Desc = "vs_1_1 : mad source reg combination oD0, oT0, oFog is NOT allowed";
string VS_260 = 
	"vs_1_1 "
	"mad r0, oD0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_261_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_261_Desc = "vs_1_1 : mad source reg combination oD0, oT0, oPts is NOT allowed";
string VS_261 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oD0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_262_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_262_Desc = "vs_1_1 : mad source reg combination oD0, oT0, oD0 is NOT allowed";
string VS_262 = 
	"vs_1_1 "
	"mad r0, oD0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_263_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_263_Desc = "vs_1_1 : mad source reg combination oD0, oT0, oT0 is NOT allowed";
string VS_263 = 
	"vs_1_1 "
	"mad r0, oD0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_264_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_264_Desc = "vs_1_1 : mad source reg combination oT0, a0, a0 is NOT allowed";
string VS_264 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oT0, a0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_265_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_265_Desc = "vs_1_1 : mad source reg combination oT0, a0, oPos is NOT allowed";
string VS_265 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oT0, a0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_266_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_266_Desc = "vs_1_1 : mad source reg combination oT0, a0, oFog is NOT allowed";
string VS_266 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oT0, a0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_267_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_267_Desc = "vs_1_1 : mad source reg combination oT0, a0, oPts is NOT allowed";
string VS_267 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oT0, a0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_268_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_268_Desc = "vs_1_1 : mad source reg combination oT0, a0, oD0 is NOT allowed";
string VS_268 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oT0, a0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_269_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_269_Desc = "vs_1_1 : mad source reg combination oT0, a0, oT0 is NOT allowed";
string VS_269 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oT0, a0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_270_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_270_Desc = "vs_1_1 : mad source reg combination oT0, oPos, a0 is NOT allowed";
string VS_270 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oT0, oPos, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_271_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_271_Desc = "vs_1_1 : mad source reg combination oT0, oPos, oPos is NOT allowed";
string VS_271 = 
	"vs_1_1 "
	"mad r0, oT0, oPos, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_272_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_272_Desc = "vs_1_1 : mad source reg combination oT0, oPos, oFog is NOT allowed";
string VS_272 = 
	"vs_1_1 "
	"mad r0, oT0, oPos, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_273_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_273_Desc = "vs_1_1 : mad source reg combination oT0, oPos, oPts is NOT allowed";
string VS_273 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oT0, oPos, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_274_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_274_Desc = "vs_1_1 : mad source reg combination oT0, oPos, oD0 is NOT allowed";
string VS_274 = 
	"vs_1_1 "
	"mad r0, oT0, oPos, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_275_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_275_Desc = "vs_1_1 : mad source reg combination oT0, oPos, oT0 is NOT allowed";
string VS_275 = 
	"vs_1_1 "
	"mad r0, oT0, oPos, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_276_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_276_Desc = "vs_1_1 : mad source reg combination oT0, oFog, a0 is NOT allowed";
string VS_276 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oT0, oFog, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_277_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_277_Desc = "vs_1_1 : mad source reg combination oT0, oFog, oPos is NOT allowed";
string VS_277 = 
	"vs_1_1 "
	"mad r0, oT0, oFog, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_278_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_278_Desc = "vs_1_1 : mad source reg combination oT0, oFog, oFog is NOT allowed";
string VS_278 = 
	"vs_1_1 "
	"mad r0, oT0, oFog, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_279_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_279_Desc = "vs_1_1 : mad source reg combination oT0, oFog, oPts is NOT allowed";
string VS_279 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oT0, oFog, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_280_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_280_Desc = "vs_1_1 : mad source reg combination oT0, oFog, oD0 is NOT allowed";
string VS_280 = 
	"vs_1_1 "
	"mad r0, oT0, oFog, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_281_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_281_Desc = "vs_1_1 : mad source reg combination oT0, oFog, oT0 is NOT allowed";
string VS_281 = 
	"vs_1_1 "
	"mad r0, oT0, oFog, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_282_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_282_Desc = "vs_1_1 : mad source reg combination oT0, oPts, a0 is NOT allowed";
string VS_282 = 
	"vs_1_1 "
	"texcoord ts "
	"mov a0.x, c0.x "
	"mad r0, oT0, oPts, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_283_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_283_Desc = "vs_1_1 : mad source reg combination oT0, oPts, oPos is NOT allowed";
string VS_283 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oT0, oPts, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_284_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_284_Desc = "vs_1_1 : mad source reg combination oT0, oPts, oFog is NOT allowed";
string VS_284 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oT0, oPts, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_285_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_285_Desc = "vs_1_1 : mad source reg combination oT0, oPts, oPts is NOT allowed";
string VS_285 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oT0, oPts, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_286_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_286_Desc = "vs_1_1 : mad source reg combination oT0, oPts, oD0 is NOT allowed";
string VS_286 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oT0, oPts, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_287_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_287_Desc = "vs_1_1 : mad source reg combination oT0, oPts, oT0 is NOT allowed";
string VS_287 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oT0, oPts, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_288_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_288_Desc = "vs_1_1 : mad source reg combination oT0, oD0, a0 is NOT allowed";
string VS_288 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oT0, oD0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_289_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_289_Desc = "vs_1_1 : mad source reg combination oT0, oD0, oPos is NOT allowed";
string VS_289 = 
	"vs_1_1 "
	"mad r0, oT0, oD0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_290_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_290_Desc = "vs_1_1 : mad source reg combination oT0, oD0, oFog is NOT allowed";
string VS_290 = 
	"vs_1_1 "
	"mad r0, oT0, oD0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_291_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_291_Desc = "vs_1_1 : mad source reg combination oT0, oD0, oPts is NOT allowed";
string VS_291 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oT0, oD0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_292_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_292_Desc = "vs_1_1 : mad source reg combination oT0, oD0, oD0 is NOT allowed";
string VS_292 = 
	"vs_1_1 "
	"mad r0, oT0, oD0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_293_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_293_Desc = "vs_1_1 : mad source reg combination oT0, oD0, oT0 is NOT allowed";
string VS_293 = 
	"vs_1_1 "
	"mad r0, oT0, oD0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_294_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_294_Desc = "vs_1_1 : mad source reg combination oT0, oT0, a0 is NOT allowed";
string VS_294 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"mad r0, oT0, oT0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_295_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_295_Desc = "vs_1_1 : mad source reg combination oT0, oT0, oPos is NOT allowed";
string VS_295 = 
	"vs_1_1 "
	"mad r0, oT0, oT0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_296_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_296_Desc = "vs_1_1 : mad source reg combination oT0, oT0, oFog is NOT allowed";
string VS_296 = 
	"vs_1_1 "
	"mad r0, oT0, oT0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_297_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_297_Desc = "vs_1_1 : mad source reg combination oT0, oT0, oPts is NOT allowed";
string VS_297 = 
	"vs_1_1 "
	"texcoord ts "
	"mad r0, oT0, oT0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_298_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_298_Desc = "vs_1_1 : mad source reg combination oT0, oT0, oD0 is NOT allowed";
string VS_298 = 
	"vs_1_1 "
	"mad r0, oT0, oT0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_299_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_299_Desc = "vs_1_1 : mad source reg combination oT0, oT0, oT0 is NOT allowed";
string VS_299 = 
	"vs_1_1 "
	"mad r0, oT0, oT0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_300_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_300_Desc = "vs_1_1 : mad dest r# is allowed";
string VS_300 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_301_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_301_Desc = "vs_1_1 : mad dest oPos is allowed";
string VS_301 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad oPos, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_302_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_302_Desc = "vs_1_1 : mad dest oD# is allowed";
string VS_302 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad oD0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_303_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_303_Desc = "vs_1_1 : mad dest oT# is allowed";
string VS_303 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad oT0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_304_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_304_Desc = "vs_1_1 : mad dest v# is NOT allowed";
string VS_304 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad v0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_305_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_305_Desc = "vs_1_1 : mad dest c# is NOT allowed";
string VS_305 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad c1, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_306_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_306_Desc = "vs_1_1 : mad dest a0 is NOT allowed";
string VS_306 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad a0, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_307_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_307_Desc = "vs_1_1 : mad write mask .x is allowed";
string VS_307 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.x, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_308_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_308_Desc = "vs_1_1 : mad write mask .y is allowed";
string VS_308 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.y, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_309_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_309_Desc = "vs_1_1 : mad write mask .z is allowed";
string VS_309 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.z, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_310_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_310_Desc = "vs_1_1 : mad write mask .w is allowed";
string VS_310 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.w, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_311_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_311_Desc = "vs_1_1 : mad write mask .xy is allowed";
string VS_311 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xy, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_312_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_312_Desc = "vs_1_1 : mad write mask .yz is allowed";
string VS_312 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.yz, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_313_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_313_Desc = "vs_1_1 : mad write mask .zw is allowed";
string VS_313 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.zw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_314_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_314_Desc = "vs_1_1 : mad write mask .xz is allowed";
string VS_314 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xz, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_315_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_315_Desc = "vs_1_1 : mad write mask .xw is allowed";
string VS_315 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_316_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_316_Desc = "vs_1_1 : mad write mask .yw is allowed";
string VS_316 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.yw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_317_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_317_Desc = "vs_1_1 : mad write mask .xyz is allowed";
string VS_317 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xyz, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_318_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_318_Desc = "vs_1_1 : mad write mask .yzw is allowed";
string VS_318 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.yzw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_319_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_319_Desc = "vs_1_1 : mad write mask .xzw is allowed";
string VS_319 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xzw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_320_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_320_Desc = "vs_1_1 : mad write mask .xyzw is allowed";
string VS_320 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.xyzw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_321_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_321_Desc = "vs_1_1 : mad write mask .yx is NOT allowed";
string VS_321 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.yx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_322_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_322_Desc = "vs_1_1 : mad write mask .zx is NOT allowed";
string VS_322 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.zx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_323_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_323_Desc = "vs_1_1 : mad write mask .zy is NOT allowed";
string VS_323 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.zy, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_324_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_324_Desc = "vs_1_1 : mad write mask .wx is NOT allowed";
string VS_324 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_325_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_325_Desc = "vs_1_1 : mad write mask .wz is NOT allowed";
string VS_325 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wz, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_326_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_326_Desc = "vs_1_1 : mad write mask .wy is NOT allowed";
string VS_326 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wy, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_327_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_327_Desc = "vs_1_1 : mad write mask .zyx is NOT allowed";
string VS_327 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.zyx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_328_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_328_Desc = "vs_1_1 : mad write mask .wzy is NOT allowed";
string VS_328 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wzy, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_329_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_329_Desc = "vs_1_1 : mad write mask .wzx is NOT allowed";
string VS_329 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wzx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_330_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_330_Desc = "vs_1_1 : mad write mask .wyx is NOT allowed";
string VS_330 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wyx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_331_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_331_Desc = "vs_1_1 : mad write mask .yxw is NOT allowed";
string VS_331 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.yxw, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_332_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_332_Desc = "vs_1_1 : mad write mask .wzyx is NOT allowed";
string VS_332 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.wzyx, v0, r0, r1 "
	"mov oPos, c0 ";

VS_CRITERIA VS_333_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_333_Desc = "vs_1_1 : mad write mask .zxwy is NOT allowed";
string VS_333 = 
	"vs_1_1 "
	"dcl_tangent v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"mad r0.zxwy, v0, r0, r1 "
	"mov oPos, c0 ";


