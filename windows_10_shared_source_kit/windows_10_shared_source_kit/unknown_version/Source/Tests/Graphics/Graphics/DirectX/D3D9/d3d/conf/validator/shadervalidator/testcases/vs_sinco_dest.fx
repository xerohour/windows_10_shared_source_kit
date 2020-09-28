#include "Test_Include.fx"

int Test_Count = 10;

string TestInfo
<
    string TestType = "VS";
>
= "vs_sinco_dest";  // filename of the test template

VS_CRITERIA VS_001_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_3_0 : dest r# is allowed";
string VS_001 =
    "vs_3_0 "
    "dcl_position o0 "
    "mov o0, c0 "
    "sincos r0.xy, c2.x ";

VS_CRITERIA VS_002_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_3_0 : dest v# is not allowed";
string VS_002 =
    "vs_3_0 "
    "dcl_position v0 "
    "dcl_position o0 "
    "mov o0, c0 "
    "sincos v0.xy, c2.x ";

VS_CRITERIA VS_003_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_3_0 : dest c# is not allowed";
string VS_003 =
    "vs_3_0 "
    "dcl_position o0 "
    "mov o0, c0 "
    "sincos c0.xy, c2.x ";

VS_CRITERIA VS_004_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_3_0 : dest i# is not allowed";
string VS_004 =
    "vs_3_0 "
    "dcl_position o0 "
    "mov o0, c0 "
    "sincos i0.xy, c2.x ";

VS_CRITERIA VS_005_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_3_0 : dest b# is not allowed";
string VS_005 =
    "vs_3_0 "
    "dcl_position o0 "
    "mov o0, c0 "
    "sincos b0.xy, c2.x ";

VS_CRITERIA VS_006_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_3_0 : dest p0 is not allowed";
string VS_006 =
    "vs_3_0 "
    "dcl_position o0 "
    "mov o0, c0 "
    "sincos p0.xy, c2.x ";

VS_CRITERIA VS_007_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_3_0 : dest s# is not allowed";
string VS_007 =
    "vs_3_0 "
    "dcl_position o0 "
    "dcl_2d s0 "
    "mov o0, c0 "
    "sincos s0.xy, c2.x ";

VS_CRITERIA VS_008_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_3_0 : dest a0 is not allowed";
string VS_008 =
    "vs_3_0 "
    "dcl_position o0 "
    "mov o0, c0 "
    "sincos a0.xy, c2.x ";

VS_CRITERIA VS_009_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 9.029f };
string VS_009_Desc = "vs_3_0 : dest o# is not allowed";
string VS_009 =
    "vs_3_0 "
    "dcl_position o0 "
    "mov o0, c0 "
    "sincos o0.xy, c2.x ";

VS_CRITERIA VS_010_Criteria = { false, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_3_0 : dest aL# is not allowed";
string VS_010 =
    "vs_3_0 "
    "dcl_position o0 "
    "mov o0, c0 "
    "sincos aL.xy, c2.x ";
