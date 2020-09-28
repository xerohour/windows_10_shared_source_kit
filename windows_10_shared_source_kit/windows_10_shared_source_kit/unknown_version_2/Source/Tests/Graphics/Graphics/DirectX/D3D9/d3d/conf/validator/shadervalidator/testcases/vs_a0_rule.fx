#include "Test_Include.fx"

int Test_Count = 180;

string TestInfo
<
    string TestType = "VS";
>
= "VS_a0_rule";

// vs_1_1

VS_CRITERIA VS_001_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_001 =
    "vs_1_1 "
    "add a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_002_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_002 =
    "vs_1_1 "
    "dp3 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_003_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_003 =
    "vs_1_1 "
    "dp4 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_004_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_004 =
    "vs_1_1 "
    "dst a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_005_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_005 =
    "vs_1_1 "
    "dp3 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_006_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_006 =
    "vs_1_1 "
    "exp a0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_007_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_007 =
    "vs_1_1 "
    "expp a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_008_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_008 =
    "vs_1_1 "
    "frc a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_009_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_009 =
    "vs_1_1 "
    "lit a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_010_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_010 =
    "vs_1_1 "
    "log a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_011_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_011 =
    "vs_1_1 "
    "logp a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_012_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_012 =
    "vs_1_1 "
    "mov r0, c0 "
    "mad a0.x, r0, r0, r0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_013_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_013 =
    "vs_1_1 "
    "max a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_014_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_014 =
    "vs_1_1 "
    "min a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_015_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_015 =
    "vs_1_1 "
    "mul a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_016_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_016 =
    "vs_1_1 "
    "rcp a0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_017_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_017 =
    "vs_1_1 "
    "rsq a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_018_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_018 =
    "vs_1_1 "
    "sge a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_019_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_019 =
    "vs_1_1 "
    "slt a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_020_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_1_1 : a0.x can only be written by mov";
string VS_020 =
    "vs_1_1 "
    "sub a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

// vs_2_0

VS_CRITERIA VS_021_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_021 =
    "vs_2_0 "
    "add a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_022_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_022 =
    "vs_2_0 "
    "dp3 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_023_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_023 =
    "vs_2_0 "
    "dp4 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_024_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_024 =
    "vs_2_0 "
    "dst a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_025_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_025 =
    "vs_2_0 "
    "dp3 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_026_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_026 =
    "vs_2_0 "
    "exp a0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_027_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_027 =
    "vs_2_0 "
    "expp a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_028_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_028 =
    "vs_2_0 "
    "frc a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_029_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_029 =
    "vs_2_0 "
    "lit a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_030_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_030 =
    "vs_2_0 "
    "log a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_031_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_031 =
    "vs_2_0 "
    "logp a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_032_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_032 =
    "vs_2_0 "
    "mov r0, c0 "
    "mad a0.x, r0, r0, r0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_033_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_033 =
    "vs_2_0 "
    "max a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_034_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_034 =
    "vs_2_0 "
    "min a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_035_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_035 =
    "vs_2_0 "
    "mul a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_036_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_036 =
    "vs_2_0 "
    "rcp a0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_037_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_037 =
    "vs_2_0 "
    "rsq a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_038_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_038 =
    "vs_2_0 "
    "sge a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_039_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_039 =
    "vs_2_0 "
    "slt a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_040_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_040 =
    "vs_2_0 "
    "sub a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

//-------------

VS_CRITERIA VS_041_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_041 =
    "vs_2_0 "
    "abs a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_042_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_042 =
    "vs_2_0 "
    "crs a0.xyz, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_043_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_043 =
    "vs_2_0 "
    "mov r0, c0 "
    "lrp a0, r0, r0, r0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_044_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_044 =
    "vs_2_0 "
    "mov r0, c0 "
    "m3x2 a0.xy, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_045_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_045 =
    "vs_2_0 "
    "mov r0, c0 "
    "m3x3 a0.xyz, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_046_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_046 =
    "vs_2_0 "
    "mov r0, c0 "
    "m3x4 a0, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_047_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_047 =
    "vs_2_0 "
    "mov r0, c0 "
    "m4x3 a0.xyz, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_048_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_048 =
    "vs_2_0 "
    "mov r0, c0 "
    "m4x4 a0, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_049_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_049 =
    "vs_2_0 "
    "nrm a0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_050_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_050 =
    "vs_2_0 "
    "pow a0, c0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_051_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_051 =
    "vs_2_0 "
    "sgn a0, c0, r0, r1 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_052_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_2_0 : a0 can only be written by mova";
string VS_052 =
    "vs_2_0 "
    "mov r0, c0 "
    "sincos a0.xy, r0.x, c1, c2 "
    "mov oPos, c[ a0.x ] ";

// vs_2_x

VS_CRITERIA VS_053_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_2_x : a0 can only be written by mova";
string VS_053 =
    "vs_2_x "
    "add a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_054_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_2_x : a0 can only be written by mova";
string VS_054 =
    "vs_2_x "
    "dp3 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_055_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_2_x : a0 can only be written by mova";
string VS_055 =
    "vs_2_x "
    "dp4 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_056_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_2_x : a0 can only be written by mova";
string VS_056 =
    "vs_2_x "
    "dst a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_057_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_2_x : a0 can only be written by mova";
string VS_057 =
    "vs_2_x "
    "dp3 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_058_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_2_x : a0 can only be written by mova";
string VS_058 =
    "vs_2_x "
    "exp a0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_059_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_2_x : a0 can only be written by mova";
string VS_059 =
    "vs_2_x "
    "expp a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_060_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_2_x : a0 can only be written by mova";
string VS_060 =
    "vs_2_x "
    "frc a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_061_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_2_x : a0 can only be written by mova";
string VS_061 =
    "vs_2_x "
    "lit a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_062_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_2_x : a0 can only be written by mova";
string VS_062 =
    "vs_2_x "
    "log a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_063_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_2_x : a0 can only be written by mova";
string VS_063 =
    "vs_2_x "
    "logp a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_064_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_2_x : a0 can only be written by mova";
string VS_064 =
    "vs_2_x "
    "mov r0, c0 "
    "mad a0.x, r0, r0, r0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_065_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_2_x : a0 can only be written by mova";
string VS_065 =
    "vs_2_x "
    "max a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_066_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_2_x : a0 can only be written by mova";
string VS_066 =
    "vs_2_x "
    "min a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_067_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_2_x : a0 can only be written by mova";
string VS_067 =
    "vs_2_x "
    "mul a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_068_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_2_x : a0 can only be written by mova";
string VS_068 =
    "vs_2_x "
    "rcp a0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_069_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_2_x : a0 can only be written by mova";
string VS_069 =
    "vs_2_x "
    "rsq a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_070_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_2_x : a0 can only be written by mova";
string VS_070 =
    "vs_2_x "
    "sge a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_071_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_2_x : a0 can only be written by mova";
string VS_071 =
    "vs_2_x "
    "slt a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_072_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_2_x : a0 can only be written by mova";
string VS_072 =
    "vs_2_x "
    "sub a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_073_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_2_x : a0 can only be written by mova";
string VS_073 =
    "vs_2_x "
    "abs a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_074_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_2_x : a0 can only be written by mova";
string VS_074 =
    "vs_2_x "
    "crs a0.xyz, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_075_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_2_x : a0 can only be written by mova";
string VS_075 =
    "vs_2_x "
    "mov r0, c0 "
    "lrp a0, r0, r0, r0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_076_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_2_x : a0 can only be written by mova";
string VS_076 =
    "vs_2_x "
    "mov r0, c0 "
    "m3x2 a0.xy, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_077_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_2_x : a0 can only be written by mova";
string VS_077 =
    "vs_2_x "
    "mov r0, c0 "
    "m3x3 a0.xyz, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_078_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_2_x : a0 can only be written by mova";
string VS_078 =
    "vs_2_x "
    "mov r0, c0 "
    "m3x4 a0, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_079_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_2_x : a0 can only be written by mova";
string VS_079 =
    "vs_2_x "
    "mov r0, c0 "
    "m4x3 a0.xyz, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_080_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_2_x : a0 can only be written by mova";
string VS_080 =
    "vs_2_x "
    "mov r0, c0 "
    "m4x4 a0, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_081_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_2_x : a0 can only be written by mova";
string VS_081 =
    "vs_2_x "
    "nrm a0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_082_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_2_x : a0 can only be written by mova";
string VS_082 =
    "vs_2_x "
    "pow a0, c0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_083_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_2_x : a0 can only be written by mova";
string VS_083 =
    "vs_2_x "
    "sgn a0, c0, r0, r1 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_084_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_2_x : a0 can only be written by mova";
string VS_084 =
    "vs_2_x "
    "mov r0, c0 "
    "sincos a0.xy, r0.x, c1, c2 "
    "mov oPos, c[ a0.x ] ";

// vs_2_sw

VS_CRITERIA VS_085_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_085 =
    "vs_2_sw "
    "add a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_086_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_086 =
    "vs_2_sw "
    "dp3 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_087_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_087 =
    "vs_2_sw "
    "dp4 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_088_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_088 =
    "vs_2_sw "
    "dst a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_089_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_089_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_089 =
    "vs_2_sw "
    "dp3 a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_090_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_090 =
    "vs_2_sw "
    "exp a0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_091_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_091 =
    "vs_2_sw "
    "expp a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_092_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_092 =
    "vs_2_sw "
    "frc a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_093_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_093 =
    "vs_2_sw "
    "lit a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_094_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_094 =
    "vs_2_sw "
    "log a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_095_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_095 =
    "vs_2_sw "
    "logp a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_096_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_096 =
    "vs_2_sw "
    "mov r0, c0 "
    "mad a0.x, r0, r0, r0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_097_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_097 =
    "vs_2_sw "
    "max a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_098_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_098 =
    "vs_2_sw "
    "min a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_099_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_099 =
    "vs_2_sw "
    "mul a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_100_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_100 =
    "vs_2_sw "
    "rcp a0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_101_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_101 =
    "vs_2_sw "
    "rsq a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_102_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_102 =
    "vs_2_sw "
    "sge a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_103_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_103 =
    "vs_2_sw "
    "slt a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_104_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_104 =
    "vs_2_sw "
    "sub a0.x, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_105_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_105 =
    "vs_2_sw "
    "abs a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_106_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_106 =
    "vs_2_sw "
    "crs a0.xyz, c0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_107_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_107 =
    "vs_2_sw "
    "mov r0, c0 "
    "lrp a0, r0, r0, r0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_108_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_108 =
    "vs_2_sw "
    "mov r0, c0 "
    "m3x2 a0.xy, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_109_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_109 =
    "vs_2_sw "
    "mov r0, c0 "
    "m3x3 a0.xyz, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_110_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_110 =
    "vs_2_sw "
    "mov r0, c0 "
    "m3x4 a0, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_111_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_111 =
    "vs_2_sw "
    "mov r0, c0 "
    "m4x3 a0.xyz, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_112_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_112 =
    "vs_2_sw "
    "mov r0, c0 "
    "m4x4 a0, r0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_113_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_113 =
    "vs_2_sw "
    "nrm a0, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_114_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_114 =
    "vs_2_sw "
    "pow a0, c0.x, c0.x "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_115_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_115 =
    "vs_2_sw "
    "sgn a0, c0, r0, r1 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_116_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_2_sw : a0 can only be written by mova";
string VS_116 =
    "vs_2_sw "
    "mov r0, c0 "
    "sincos a0.xy, r0.x, c1, c2 "
    "mov oPos, c[ a0.x ] ";

// vs_3_0

VS_CRITERIA VS_117_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_117 =
    "vs_3_0 "
    "dcl_position o0"
    "add a0.x, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_118_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_118 =
    "vs_3_0 "
    "dcl_position o0"
    "dp3 a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_119_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_119 =
    "vs_3_0 "
    "dcl_position o0"
    "dp4 a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_120_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_120 =
    "vs_3_0 "
    "dcl_position o0"
    "dst a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_121_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_121 =
    "vs_3_0 "
    "dcl_position o0"
    "dp3 a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_122_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_122 =
    "vs_3_0 "
    "dcl_position o0"
    "exp a0.x, c0.x "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_123_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_123 =
    "vs_3_0 "
    "dcl_position o0"
    "expp a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_124_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_124 =
    "vs_3_0 "
    "dcl_position o0"
    "frc a0.x, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_125_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_125 =
    "vs_3_0 "
    "dcl_position o0"
    "lit a0.x, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_126_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_126 =
    "vs_3_0 "
    "dcl_position o0"
    "log a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_127_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_127 =
    "vs_3_0 "
    "dcl_position o0"
    "logp a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_128_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_128 =
    "vs_3_0 "
    "dcl_position o0"
    "mov r0, c0 "
    "mad a0.x, r0, r0, r0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_129_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_129 =
    "vs_3_0 "
    "dcl_position o0"
    "max a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_130_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_130_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_130 =
    "vs_3_0 "
    "dcl_position o0"
    "min a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_131_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_131 =
    "vs_3_0 "
    "dcl_position o0"
    "mul a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_132_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_132 =
    "vs_3_0 "
    "dcl_position o0"
    "rcp a0.x, c0.x "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_133_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_133 =
    "vs_3_0 "
    "dcl_position o0"
    "rsq a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_134_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_134 =
    "vs_3_0 "
    "dcl_position o0"
    "sge a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_135_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_135 =
    "vs_3_0 "
    "dcl_position o0"
    "slt a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_136_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_136 =
    "vs_3_0 "
    "dcl_position o0"
    "sub a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_137_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_137 =
    "vs_3_0 "
    "dcl_position o0"
    "abs a0.x, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_138_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_138 =
    "vs_3_0 "
    "dcl_position o0"
    "crs a0.xyz, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_139_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_139 =
    "vs_3_0 "
    "dcl_position o0"
    "mov r0, c0 "
    "lrp a0, r0, r0, r0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_140_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_140 =
    "vs_3_0 "
    "dcl_position o0"
    "mov r0, c0 "
    "m3x2 a0.xy, r0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_141_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_141_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_141 =
    "vs_3_0 "
    "dcl_position o0"
    "mov r0, c0 "
    "m3x3 a0.xyz, r0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_142_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_142_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_142 =
    "vs_3_0 "
    "dcl_position o0"
    "mov r0, c0 "
    "m3x4 a0, r0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_143_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_143_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_143 =
    "vs_3_0 "
    "dcl_position o0"
    "mov r0, c0 "
    "m4x3 a0.xyz, r0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_144_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_144_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_144 =
    "vs_3_0 "
    "dcl_position o0"
    "mov r0, c0 "
    "m4x4 a0, r0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_145_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_145 =
    "vs_3_0 "
    "dcl_position o0"
    "nrm a0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_146_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_146 =
    "vs_3_0 "
    "dcl_position o0"
    "pow a0, c0.x, c0.x "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_147_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_147 =
    "vs_3_0 "
    "dcl_position o0"
    "sgn a0, c0, r0, r1 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_148_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_3_0 : a0 can only be written by mova";
string VS_148 =
    "vs_3_0 "
    "dcl_position o0"
    "mov r0, c0 "
    "sincos a0.xy, r0.x "
    "mov o0, c[ a0.x ] ";

// vs_3_sw

VS_CRITERIA VS_149_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_149_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_149 =
    "vs_3_sw "
    "dcl_position o0"
    "add a0.x, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_150_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_150 =
    "vs_3_sw "
    "dcl_position o0"
    "dp3 a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_151_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_151 =
    "vs_3_sw "
    "dcl_position o0"
    "dp4 a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_152_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_152 =
    "vs_3_sw "
    "dcl_position o0"
    "dst a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_153_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_153 =
    "vs_3_sw "
    "dcl_position o0"
    "dp3 a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_154_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_154 =
    "vs_3_sw "
    "dcl_position o0"
    "exp a0.x, c0.x "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_155_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_155 =
    "vs_3_sw "
    "dcl_position o0"
    "expp a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_156_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_156_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_156 =
    "vs_3_sw "
    "dcl_position o0"
    "frc a0.x, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_157_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_157 =
    "vs_3_sw "
    "dcl_position o0"
    "lit a0.x, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_158_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_158 =
    "vs_3_sw "
    "dcl_position o0"
    "log a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_159_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_159_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_159 =
    "vs_3_sw "
    "dcl_position o0"
    "logp a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_160_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_160 =
    "vs_3_sw "
    "dcl_position o0"
    "mov r0, c0 "
    "mad a0.x, r0, r0, r0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_161_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_161 =
    "vs_3_sw "
    "dcl_position o0"
    "max a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_162_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_162 =
    "vs_3_sw "
    "dcl_position o0"
    "min a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_163_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_163 =
    "vs_3_sw "
    "dcl_position o0"
    "mul a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_164_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_164 =
    "vs_3_sw "
    "dcl_position o0"
    "rcp a0.x, c0.x "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_165_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_165 =
    "vs_3_sw "
    "dcl_position o0"
    "rsq a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_166_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_166 =
    "vs_3_sw "
    "dcl_position o0"
    "sge a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_167_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_167 =
    "vs_3_sw "
    "dcl_position o0"
    "slt a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_168_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_168_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_168 =
    "vs_3_sw "
    "dcl_position o0"
    "sub a0.x, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_169_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_169_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_169 =
    "vs_3_sw "
    "dcl_position o0"
    "abs a0.x, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_170_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_170_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_170 =
    "vs_3_sw "
    "dcl_position o0"
    "crs a0.xyz, c0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_171_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_171_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_171 =
    "vs_3_sw "
    "dcl_position o0"
    "mov r0, c0 "
    "lrp a0, r0, r0, r0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_172_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_172_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_172 =
    "vs_3_sw "
    "dcl_position o0"
    "mov r0, c0 "
    "m3x2 a0.xy, r0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_173_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_173_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_173 =
    "vs_3_sw "
    "dcl_position o0"
    "mov r0, c0 "
    "m3x3 a0.xyz, r0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_174_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_174_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_174 =
    "vs_3_sw "
    "dcl_position o0"
    "mov r0, c0 "
    "m3x4 a0, r0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_175_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_175_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_175 =
    "vs_3_sw "
    "dcl_position o0"
    "mov r0, c0 "
    "m4x3 a0.xyz, r0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_176_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_176_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_176 =
    "vs_3_sw "
    "dcl_position o0"
    "mov r0, c0 "
    "m4x4 a0, r0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_177_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_177_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_177 =
    "vs_3_sw "
    "dcl_position o0"
    "nrm a0, c0 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_178_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_178_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_178 =
    "vs_3_sw "
    "dcl_position o0"
    "pow a0, c0.x, c0.x "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_179_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_179_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_179 =
    "vs_3_sw "
    "dcl_position o0"
    "sgn a0, c0, r0, r1 "
    "mov o0, c[ a0.x ] ";

VS_CRITERIA VS_180_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_180_Desc = "vs_3_sw : a0 can only be written by mova";
string VS_180 =
    "vs_3_sw "
    "dcl_position o0"
    "mov r0, c0 "
    "sincos a0.xy, r0.x "
    "mov o0, c[ a0.x ] ";
