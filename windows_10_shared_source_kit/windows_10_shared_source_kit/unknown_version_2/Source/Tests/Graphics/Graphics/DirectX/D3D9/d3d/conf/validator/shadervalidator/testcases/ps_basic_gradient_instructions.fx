#include "Test_Include.fx"

int Test_Count = 7;

string TestInfo
<
    string TestType = "PS";
>
= "PS_basic_gradient_instructions";

string PS_001_Desc = "ps_2_0 : dsx is not supported";
string PS_001 =
    "ps_2_0 "
    "dcl t0 "
    "dsx r0, t0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_001_Criteria = { false, false, -1, -1, -1, -1, -1, 0, -1, 0.f };

string PS_002_Desc = "ps_2_0 : dsy is not supported";
string PS_002 =
    "ps_2_0 "
    "dcl t0 "
    "dsy r0, t0 "
    "mov oC0, r0 ";
PS_CRITERIA PS_002_Criteria = { false, false, -1, -1, -1, -1, -1, 0, -1, 0.f };

string PS_003_Desc = "ps_2_0 : texldd is not supported";
string PS_003 =
    "ps_2_0 "
    "dcl t0 "
    "dcl t1 "
    "dcl t2 "
    "dcl_2d s0 "
    "mov r0, t1 "
    "mov r1, t2 "
    "texldd r2, t0, s0, r0, r1 "
    "mov oC0, r2 ";
PS_CRITERIA PS_003_Criteria = { false, false, -1, -1, -1, -1, -1, 0, -1, 0.f };

string PS_004_Desc = "ps_2_x : texldd / dsx / dsy may be supported";
string PS_004 =
    "ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl t2 "
    "dcl_2d s0 "
    "mov r0, t1 "
    "mov r1, t2 "
    "texldd r2, t0, s0, r0, r1 "
    "dsx r3, t1 "
    "dsy r4, t2 "
    "mad r5, r3, r4, r2 "
    "mov oC0, r5 ";
PS_CRITERIA PS_004_Criteria = { true, false, 0x0200, -1, -1, -1, -1, CAPS_GRADIENTINSTRUCTIONS, -1, 0.f };

string PS_005_Desc = "ps_2_sw : texldd / dsx / dsy are supported";
string PS_005 =
    "ps_2_sw "
    "dcl t0 "
    "dcl t1 "
    "dcl t2 "
    "dcl_2d s0 "
    "mov r0, t1 "
    "mov r1, t2 "
    "texldd r2, t0, s0, r0, r1 "
    "dsx r3, t1 "
    "dsy r4, t2 "
    "mad r5, r3, r4, r2 "
    "mov oC0, r5 ";
PS_CRITERIA PS_005_Criteria = { true, true, -1, -1, -1, -1, -1, -1, -1, 0.f };

string PS_006_Desc = "ps_3_0 : texldd / dsx / dsy are supported";
string PS_006 =
    "ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_texcoord2 v2 "
    "dcl_2d s0 "
    "mov r0, v1 "
    "mov r1, v2 "
    "texldd r2, v0, s0, r0, r1 "
    "dsx r3, v1 "
    "dsy r4, v2 "
    "mad r5, r3, r4, r2 "
    "mov oC0, r5 ";
PS_CRITERIA PS_006_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_007_Desc = "ps_3_sw : texldd / dsx / dsy are supported";
string PS_007 =
    "ps_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_texcoord2 v2 "
    "dcl_2d s0 "
    "mov r0, v1 "
    "mov r1, v2 "
    "texldd r2, v0, s0, r0, r1 "
    "dsx r3, v1 "
    "dsy r4, v2 "
    "mad r5, r3, r4, r2 "
    "mov oC0, r5 ";
PS_CRITERIA PS_007_Criteria = { true, true, -1, -1, -1, -1, -1, 0, -1, 0.f };