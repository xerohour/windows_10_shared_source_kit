#include "Test_Include.fx"

int Test_Count = 58;

string TestInfo
<
	string TestType = "VS";
>
= "vs_1_1_inst_log";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_1_1 : log source reg combination r0.x is allowed";
string VS_001 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_002_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_1_1 : log source reg combination r0.y is allowed";
string VS_002 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.y "
	"mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_1_1 : log source reg combination r0.z is allowed";
string VS_003 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.z "
	"mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_1_1 : log source reg combination r0.w is allowed";
string VS_004 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.w "
	"mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_1_1 : log source reg combination a0 is NOT allowed";
string VS_005 = 
	"vs_1_1 "
	"mov a0.x, c0.x "
	"log r0, a0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_006_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_1_1 : log source reg combination oPos is NOT allowed";
string VS_006 = 
	"vs_1_1 "
	"log r0, oPos "
	"mov oPos, c0 ";

VS_CRITERIA VS_007_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_1_1 : log source reg combination oFog is NOT allowed";
string VS_007 = 
	"vs_1_1 "
	"log r0, oFog "
	"mov oPos, c0 ";

VS_CRITERIA VS_008_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_1_1 : log source reg combination oPts is NOT allowed";
string VS_008 = 
	"vs_1_1 "
	"texcoord ts "
	"log r0, oPts "
	"mov oPos, c0 ";

VS_CRITERIA VS_009_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_1_1 : log source reg combination oD0 is NOT allowed";
string VS_009 = 
	"vs_1_1 "
	"log r0, oD0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_010_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_1_1 : log source reg combination oT0 is NOT allowed";
string VS_010 = 
	"vs_1_1 "
	"log r0, oT0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_011_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_1_1 : log source reg combination r0.yzxw is NOT allowed";
string VS_011 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.yzxw "
	"mov oPos, c0 ";

VS_CRITERIA VS_012_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_1_1 : log source reg combination r0.zxyw is NOT allowed";
string VS_012 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.zxyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_013_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_1_1 : log source reg combination r0.wzyx is NOT allowed";
string VS_013 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.wzyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_014_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_1_1 : log source reg combination r0.wyxz is NOT allowed";
string VS_014 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.wyxz "
	"mov oPos, c0 ";

VS_CRITERIA VS_015_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_1_1 : log source reg combination r0.xzyw is NOT allowed";
string VS_015 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.xzyw "
	"mov oPos, c0 ";

VS_CRITERIA VS_016_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_1_1 : log source reg combination r0.xywz is NOT allowed";
string VS_016 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.xywz "
	"mov oPos, c0 ";

VS_CRITERIA VS_017_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_1_1 : log source reg combination r0.zyx is NOT allowed";
string VS_017 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.zyx "
	"mov oPos, c0 ";

VS_CRITERIA VS_018_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_1_1 : log source reg combination r0.xzy is NOT allowed";
string VS_018 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.xzy "
	"mov oPos, c0 ";

VS_CRITERIA VS_019_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_1_1 : log source reg combination r0.ywx is NOT allowed";
string VS_019 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.ywx "
	"mov oPos, c0 ";

VS_CRITERIA VS_020_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_1_1 : log source reg combination r0.yx is NOT allowed";
string VS_020 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.yx "
	"mov oPos, c0 ";

VS_CRITERIA VS_021_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_1_1 : log source reg combination r0.wz is NOT allowed";
string VS_021 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.wz "
	"mov oPos, c0 ";

VS_CRITERIA VS_022_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_1_1 : log source reg combination r0.zy is NOT allowed";
string VS_022 = 
	"vs_1_1 "
	"mov r0, c0 "
	"log r0, r0.zy "
	"mov oPos, c0 ";

VS_CRITERIA VS_023_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_1_1 : log dest r# is allowed";
string VS_023 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_024_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_1_1 : log dest oPos is allowed";
string VS_024 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log oPos, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_025_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_1_1 : log dest oFog is allowed";
string VS_025 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log oFog, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_026_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_1_1 : log dest oPts is allowed";
string VS_026 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log oPts, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_027_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_1_1 : log dest oD# is allowed";
string VS_027 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log oD0, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_028_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_1_1 : log dest oT# is allowed";
string VS_028 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log oT0, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_029_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_1_1 : log dest v# is NOT allowed";
string VS_029 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log v0, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_030_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_1_1 : log dest c# is NOT allowed";
string VS_030 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log c1, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_031_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_1_1 : log dest a0 is NOT allowed";
string VS_031 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log a0, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_032_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_1_1 : log write mask .x is allowed";
string VS_032 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.x, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_033_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_1_1 : log write mask .y is allowed";
string VS_033 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.y, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_034_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_1_1 : log write mask .z is allowed";
string VS_034 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.z, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_035_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_1_1 : log write mask .w is allowed";
string VS_035 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.w, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_036_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_1_1 : log write mask .xy is allowed";
string VS_036 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.xy, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_037_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_1_1 : log write mask .yz is allowed";
string VS_037 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.yz, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_038_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_1_1 : log write mask .zw is allowed";
string VS_038 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.zw, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_039_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_1_1 : log write mask .xz is allowed";
string VS_039 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.xz, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_040_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_1_1 : log write mask .xw is allowed";
string VS_040 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.xw, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_041_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_1_1 : log write mask .yw is allowed";
string VS_041 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.yw, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_042_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_1_1 : log write mask .xyz is allowed";
string VS_042 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.xyz, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_043_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_1_1 : log write mask .yzw is allowed";
string VS_043 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.yzw, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_044_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_1_1 : log write mask .xzw is allowed";
string VS_044 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.xzw, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_045_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_1_1 : log write mask .xyzw is allowed";
string VS_045 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.xyzw, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_046_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_1_1 : log write mask .yx is NOT allowed";
string VS_046 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.yx, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_047_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_1_1 : log write mask .zx is NOT allowed";
string VS_047 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.zx, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_048_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_1_1 : log write mask .zy is NOT allowed";
string VS_048 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.zy, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_049_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_1_1 : log write mask .wx is NOT allowed";
string VS_049 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.wx, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_050_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_1_1 : log write mask .wz is NOT allowed";
string VS_050 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.wz, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_051_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_1_1 : log write mask .wy is NOT allowed";
string VS_051 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.wy, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_052_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_1_1 : log write mask .zyx is NOT allowed";
string VS_052 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.zyx, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_053_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_1_1 : log write mask .wzy is NOT allowed";
string VS_053 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.wzy, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_054_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_1_1 : log write mask .wzx is NOT allowed";
string VS_054 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.wzx, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_055_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_1_1 : log write mask .wyx is NOT allowed";
string VS_055 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.wyx, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_056_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_1_1 : log write mask .yxw is NOT allowed";
string VS_056 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.yxw, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_057_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_1_1 : log write mask .wzyx is NOT allowed";
string VS_057 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.wzyx, v0.x "
	"mov oPos, c0 ";

VS_CRITERIA VS_058_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_1_1 : log write mask .zxwy is NOT allowed";
string VS_058 = 
	"vs_1_1 "
	"dcl_fog v0 "
	"log r0.zxwy, v0.x "
	"mov oPos, c0 ";


