#include "Test_Include.fx"

int Test_Count = 12;

string TestInfo
<
    string TestType = "PS";
>
= "ps_sinco_dest";

string PS_001_Desc = "ps_3_0 : dest r# is allowed";
string PS_001 =
    "ps_3_0 "
    "sincos r0.xy, c2.x "
    "mov oC0, c0 ";
PS_CRITERIA PS_001_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_002_Desc = "ps_3_0 : dest v# is not allowed";
string PS_002 =
    "ps_3_0 "
    "dcl_texcoord0 v0 "
    "sincos v0.xy, c2.x "
    "mov oC0, c0 ";
PS_CRITERIA PS_002_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_003_Desc = "ps_3_0 : dest c# is not allowed";
string PS_003 =
    "ps_3_0 "
    "sincos c0.xy, c2.x "
    "mov oC0, c0 ";
PS_CRITERIA PS_003_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_004_Desc = "ps_3_0 : dest i# is not allowed";
string PS_004 =
    "ps_3_0 "
    "sincos i0.xy, c2.x "
    "mov oC0, c0 ";
PS_CRITERIA PS_004_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_005_Desc = "ps_3_0 : dest b# is not allowed";
string PS_005 =
    "ps_3_0 "
    "sincos b0.xy, c2.x "
    "mov oC0, c0 ";
PS_CRITERIA PS_005_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_006_Desc = "ps_3_0 : dest p0 is not allowed";
string PS_006 =
    "ps_3_0 "
    "sincos p0.xy, c2.x "
    "mov oC0, c0 ";
PS_CRITERIA PS_006_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_007_Desc = "ps_3_0 : dest s# is not allowed";
string PS_007 =
    "ps_3_0 "
    "dcl_2d s0 "
    "sincos s0.xy, c2.x "
    "mov oC0, c0 ";
PS_CRITERIA PS_007_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_008_Desc = "ps_3_0 : dest vFace# is not allowed";
string PS_008 =
    "ps_3_0 "
    "dcl vFace "
    "sincos vFace.xy, c2.x "
    "mov oC0, c0 ";
PS_CRITERIA PS_008_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_009_Desc = "ps_3_0 : dest vPos# is not allowed";
string PS_009 =
    "ps_3_0 "
    "dcl vPos.xy "
    "sincos vPos.xy, c2.x "
    "mov oC0, c0 ";
PS_CRITERIA PS_009_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_010_Desc = "ps_3_0 : dest aL# is not allowed";
string PS_010 =
    "ps_3_0 "
    "sincos aL.xy, c2.x "
    "mov oC0, c0 ";
PS_CRITERIA PS_010_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };

string PS_011_Desc = "ps_3_0 : dest oC0# is not allowed";
string PS_011 =
    "ps_3_0 "
    "sincos oC0.xy, c2.x ";
PS_CRITERIA PS_011_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };

string PS_012_Desc = "ps_3_0 : dest oDepth# is not allowed";
string PS_012 =
    "ps_3_0 "
    "sincos oDepth.xy, c2.x ";
PS_CRITERIA PS_012_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
