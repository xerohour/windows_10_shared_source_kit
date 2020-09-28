#include "Test_Include.fx"

int Test_Count = 10;

string TestInfo
<
    string TestType = "VS";
>
= "VS_basic_a0_usage";

// vs_1_1

VS_CRITERIA VS_001_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_1_1";
string VS_001 =
    "vs_1_1 "
    "mov a0.x, c0 "
    "mov oPos, c[ a0.x ] ";

VS_CRITERIA VS_002_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_1_1";
string VS_002 =
    "vs_1_1 "
    "mov a0.y, c0 "
    "mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_1_1";
string VS_003 =
    "vs_1_1 "
    "mov a0.z, c0 "
    "mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { false, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_1_1";
string VS_004 =
    "vs_1_1 "
    "mov a0.w, c0 "
    "mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_1_1";
string VS_005 =
    "vs_1_1 "
    "mov a0, c0 "
    "mov oPos, c0 ";

// vs_2_0

VS_CRITERIA VS_006_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_2_0";
string VS_006 =
    "vs_2_0 "
    "mova a0, c0 "
    "mov oPos.x, c[ a0.x ] "
    "mov oPos.y, c[ a0.y ] "
    "mov oPos.z, c[ a0.z ] "
    "mov oPos.w, c[ a0.w ] ";

// vs_2_x

VS_CRITERIA VS_007_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_2_x";
string VS_007 =
    "vs_2_x "
    "mova a0, c0 "
    "mov oPos.x, c[ a0.x ] "
    "mov oPos.y, c[ a0.y ] "
    "mov oPos.z, c[ a0.z ] "
    "mov oPos.w, c[ a0.w ] ";

// vs_2_sw

VS_CRITERIA VS_008_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_2_sw";
string VS_008 =
    "vs_2_sw "
    "mova a0, c0 "
    "mov oPos.x, c[ a0.x ] "
    "mov oPos.y, c[ a0.y ] "
    "mov oPos.z, c[ a0.z ] "
    "mov oPos.w, c[ a0.w ] ";

// vs_3_0

VS_CRITERIA VS_009_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_3_0";
string VS_009 =
    "vs_3_0 "
    "dcl_position o0 "
    "mova a0, c0 "
    "mov o0.x, c[ a0.x ] "
    "mov o0.y, c[ a0.y ] "
    "mov o0.z, c[ a0.z ] "
    "mov o0.w, c[ a0.w ] ";

// vs_3_sw

VS_CRITERIA VS_010_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_3_sw";
string VS_010 =
    "vs_3_sw "
    "dcl_position o0 "
    "mova a0, c0 "
    "mov o0.x, c[ a0.x ] "
    "mov o0.y, c[ a0.y ] "
    "mov o0.z, c[ a0.z ] "
    "mov o0.w, c[ a0.w ] ";
