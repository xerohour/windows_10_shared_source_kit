#include "Test_Include.fx"

int Test_Count = 50;

string TestInfo
<
    string TestType = "PS";
>
= "ps_centroid";

//ps_2_0 cases

string PS_001_Desc = "ps_2_0 : dcl_centroid t0 is allowed";
string PS_001 =
    "ps_2_0 "
    "dcl_centroid t0 "
    "dcl_2d s0 "
    "texld r0, t0, s0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.029f };

string PS_002_Desc = "ps_2_0 : dcl_centroid v0 is not allowed";
string PS_002 =
    "ps_2_0 "
    "dcl_centroid v0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_002_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_003_Desc = "ps_2_0 : dcl_2d_centroid s0 is not allowed";
string PS_003 =
    "ps_2_0 "
    "dcl_2d_centroid s0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_003_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_004_Desc = "ps_2_0 : dcl_volume_centroid s0 is not allowed";
string PS_004 =
    "ps_2_0 "
    "dcl_volume_centroid s0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_004_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_005_Desc = "ps_2_0 : dcl_cube_centroid s0 is not allowed";
string PS_005 =
    "ps_2_0 "
    "dcl_cube_centroid s0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_005_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_006_Desc = "ps_2_0 : texld_centroid is not allowed";
string PS_006 =
    "ps_2_0 "
    "dcl t0 "
    "dcl_2d s0 "
    "texld_centroid r0, t0, s0 "
    "mov oC0, t0 ";
PS_CRITERIA PS_006_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_007_Desc = "ps_2_0 : texldb_centroid is not allowed";
string PS_007 =
    "ps_2_0 "
    "dcl t0 "
    "dcl_2d s0 "
    "texldb_centroid r0, t0, s0 "
    "mov oC0, t0 ";
PS_CRITERIA PS_007_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_008_Desc = "ps_2_0 : texldp_centroid is not allowed";
string PS_008 =
    "ps_2_0 "
    "dcl t0 "
    "dcl_2d s0 "
    "texldp_centroid r0, t0, s0 "
    "mov oC0, t0 ";
PS_CRITERIA PS_008_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_009_Desc = "ps_2_0 : texkill_centroid is not allowed";
string PS_009 =
    "ps_2_0 "
    "dcl t0 "
    "mov r0, t0 "
    "texkill_centroid t0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_009_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_010_Desc = "ps_2_0 : add_centroid is not allowed";
string PS_010 =
    "ps_2_0 "
    "dcl t0 "
    "mov r0, t0 "
    "add_centroid r1, t0, t0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_010_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_011_Desc = "ps_2_0 : mov_centroid is not allowed";
string PS_011 =
    "ps_2_0 "
    "dcl t0 "
    "mov r0, t0 "
    "mov_centroid r1, t0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_011_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_012_Desc = "ps_2_0 : pow_centroid is not allowed";
string PS_012 =
    "ps_2_0 "
    "dcl t0 "
    "mov r0, t0 "
    "pow_centroid r0, t0, t0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_012_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

//ps_2_x cases

string PS_013_Desc = "ps_2_x : dcl_centroid t0 is allowed";
string PS_013 =
    "ps_2_x "
    "dcl_centroid t0 "
    "dcl_2d s0 "
    "texld r0, t0, s0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_013_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.029f };

string PS_014_Desc = "ps_2_x : dcl_centroid v0 is not allowed";
string PS_014 =
    "ps_2_x "
    "dcl_centroid v0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_014_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_015_Desc = "ps_2_x : dcl_2d_centroid s0 is not allowed";
string PS_015 =
    "ps_2_x "
    "dcl_2d_centroid s0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_015_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_016_Desc = "ps_2_x : dcl_volume_centroid s0 is not allowed";
string PS_016 =
    "ps_2_x "
    "dcl_volume_centroid s0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_016_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_017_Desc = "ps_2_x : dcl_cube_centroid s0 is not allowed";
string PS_017 =
    "ps_2_x "
    "dcl_cube_centroid s0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_017_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_018_Desc = "ps_2_x : dsx_centroid is not allowed";
string PS_018 =
    "ps_2_x "
    "dsx_centroid r0, c0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_018_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };

string PS_019_Desc = "ps_2_x : dsy_centroid is not allowed";
string PS_019 =
    "ps_2_x "
    "dsy_centroid r0, c0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_019_Criteria = { false, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };

string PS_020_Desc = "ps_2_x : texld_centroid is not allowed";
string PS_020 =
    "ps_2_x "
    "dcl t0 "
    "dcl_2d s0 "
    "texld_centroid r0, t0, s0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_020_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_021_Desc = "ps_2_x : texldb_centroid is not allowed";
string PS_021 =
    "ps_2_x "
    "dcl t0 "
    "dcl_2d s0 "
    "texldb_centroid r0, t0, s0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_021_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_022_Desc = "ps_2_x : texldp_centroid is not allowed";
string PS_022 =
    "ps_2_x "
    "dcl t0 "
    "dcl_2d s0 "
    "texldp_centroid r0, t0, s0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_022_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_023_Desc = "ps_2_x : texkill_centroid is not allowed";
string PS_023 =
    "ps_2_x "
    "dcl t0 "
    "mov r0, t0 "
    "texkill_centroid t0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_023_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_024_Desc = "ps_2_x : mul_centroid is not allowed";
string PS_024 =
    "ps_2_x "
    "dcl t0 "
    "mul r0, t0 "
    "add_centroid r1, t0, t0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_024_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_025_Desc = "ps_2_x : frc_centroid is not allowed";
string PS_025 =
    "ps_2_x "
    "dcl t0 "
    "mov r0, t0 "
    "frc_centroid r1, t0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_025_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

string PS_026_Desc = "ps_2_x : exp_centroid is not allowed";
string PS_026 =
    "ps_2_x "
    "dcl t0 "
    "mov r0, t0 "
    "exp_centroid r0, t0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_026_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };

//ps_3_0 case

string PS_027_Desc = "ps_3_0 : dcl_texcoord0_centroid is allowed";
string PS_027 =
    "ps_3_0 "
    "dcl_texcoord0_centroid v0 "
    "dcl_2d s0 "
    "texld r0, v0.xy, s0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_027_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_028_Desc = "ps_3_0 : dcl_blendweight_centroid v0 is allowed";
string PS_028 =
    "ps_3_0 "
    "dcl_blendweight_centroid v0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_028_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_029_Desc = "ps_3_0 : dcl_blendindices_centroid v0 is allowed";
string PS_029 =
    "ps_3_0 "
    "dcl_blendindices_centroid v0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_029_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_030_Desc = "ps_3_0 : dcl_normal_centroid v0 is allowed";
string PS_030 =
    "ps_3_0 "
    "dcl_normal_centroid v0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_030_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_031_Desc = "ps_3_0 : dcl_tangent_centroid v0 is allowed";
string PS_031 =
    "ps_3_0 "
    "dcl_tangent_centroid v0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_031_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_032_Desc = "ps_3_0 : dcl_binormal_centroid v0 is allowed";
string PS_032 =
    "ps_3_0 "
    "dcl_binormal_centroid v0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_032_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_033_Desc = "ps_3_0 : dcl_color_centroid v0 is allowed";
string PS_033 =
    "ps_3_0 "
    "dcl_color_centroid v0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_033_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_034_Desc = "ps_3_0 : dcl_fog_centroid v0 is allowed";
string PS_034 =
    "ps_3_0 "
    "dcl_fog_centroid v0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_034_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_035_Desc = "ps_3_0 : dcl_depth_centroid v0 is allowed";
string PS_035 =
    "ps_3_0 "
    "dcl_depth_centroid v0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_035_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_036_Desc = "ps_3_0 : dcl_centroid vFace is not allowed";
string PS_036 =
    "ps_3_0 "
    "dcl_centroid vFace "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_036_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };

string PS_037_Desc = "ps_3_0 : dcl_centroid vPos is not allowed";
string PS_037 =
    "ps_3_0 "
    "dcl_centroid vPos.xy "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_037_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };

string PS_038_Desc = "ps_3_0 : dcl_2d_centroid s0 is not allowed";
string PS_038 =
    "ps_3_0 "
    "dcl_2d_centroid s0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_038_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_039_Desc = "ps_3_0 : dcl_volume_centroid s0 is not allowed";
string PS_039 =
    "ps_3_0 "
    "dcl_volume_centroid s0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_039_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_040_Desc = "ps_3_0 : dcl_cube_centroid s0 is not allowed";
string PS_040 =
    "ps_3_0 "
    "dcl_cube_centroid s0 "
    "def c0, 0, 0, 0, 0 "
    "mov oC0, c0 ";
PS_CRITERIA PS_040_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_041_Desc = "ps_3_0 : dsx_centroid is not allowed";
string PS_041 =
    "ps_3_0 "
    "dsx_centroid r0, c0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_041_Criteria = { false, false, 0x0300, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };

string PS_042_Desc = "ps_3_0 : dsy_centroid is not allowed";
string PS_042 =
    "ps_3_0 "
    "dsy_centroid r0, c0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_042_Criteria = { false, false, 0x0300, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };

string PS_043_Desc = "ps_3_0 : texld_centroid is not allowed";
string PS_043 =
    "ps_3_0 "
    "dcl_texcoord0 v0.xy "
    "dcl_2d s0 "
    "texld_centroid r0, v0.xy, s0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_043_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };

string PS_044_Desc = "ps_3_0 : texldb_centroid is not allowed";
string PS_044 =
    "ps_3_0 "
    "dcl_texcoord0 v0.xy "
    "dcl_2d s0 "
    "texldb_centroid r0, v0.xy, s0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_044_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_045_Desc = "ps_3_0 : texldp_centroid is not allowed";
string PS_045 =
    "ps_3_0 "
    "dcl_texcoord0 v0.xy "
    "dcl_2d s0 "
    "texldp_centroid r0, v0.xy, s0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_045_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_046_Desc = "ps_3_0 : texldl_centroid is not allowed";
string PS_046 =
    "ps_3_0 "
    "dcl_texcoord0 v0.xy "
    "dcl_2d s0 "
    "texldl_centroid r0, v0.xy, s0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_046_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_047_Desc = "ps_3_0 : texldd_centroid is not allowed";
string PS_047 =
    "ps_3_0 "
    "dcl_texcoord0 v0.xy "
    "dcl_2d s0 "
    "texldd_centroid r0, v0.xy, s0, r0, r1 "
    "mov oC0, r0 ";
PS_CRITERIA PS_047_Criteria = { false, false, 0x0300, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };

string PS_048_Desc = "ps_3_0 : texkill_centroid is not allowed";
string PS_048 =
    "ps_3_0 "
    "dcl_texcoord0 v0.xy "
    "texkill_centroid r0, v0.xy "
    "mov oC0, r0 ";
PS_CRITERIA PS_048_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_049_Desc = "ps_3_0 : nrm_centroid is not allowed";
string PS_049 =
    "ps_3_0 "
    "dcl_texcoord0 v0.xy "
    "nrm_centroid r0, v0.xy "
    "mov oC0, r0 ";
PS_CRITERIA PS_049_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_050_Desc = "ps_3_0 : mad_centroid is not allowed";
string PS_050 =
    "ps_3_0 "
    "dcl_texcoord0 v0.xy "
    "mad_centroid r0, v0, r1, r2 "
    "mov oC0, r0 ";
PS_CRITERIA PS_050_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
