#include "Test_Include.fx"

int Test_Count = 140;

string TestInfo
<
    string TestType = "VS";
>
= "vs3_0_flow_control_texldl";

//cases with texldl r2, r#, s0

VS_CRITERIA VS_001_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_3_sw : texldl r# is allowed";
string VS_001 =
    "vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "texldl r2, r0, s0 "
    "mov o0, r2 ";

VS_CRITERIA VS_002_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_3_sw : texldl r# inside if_gt block is allowed";
string VS_002 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_gt c0.x, r0.x "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_003_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_3_sw : texldl r# inside if_lt block is allowed";
string VS_003 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_lt c0.y, r0.y "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_004_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_3_sw : texldl r# inside if_ge block is allowed";
string VS_004 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ge c0.z, r0.z "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_005_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_3_sw : texldl r# inside if_le block is allowed";
string VS_005 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_le c0.w, r0.w "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_006_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_3_sw : texldl r# inside if_eq block is allowed";
string VS_006 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_eq c0.x, r0.y "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_007_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_3_sw : texldl r# inside if_ne block is allowed";
string VS_007 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ne c0.z, r0.w "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_008_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_3_sw : texldl r# inside if p.x block is allowed";
string VS_008 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.x "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_009_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_3_sw : texldl r# inside if !p.y block is allowed";
string VS_009 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.y "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_010_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_3_sw : texldl r# inside if p.z block is allowed";
string VS_010 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.z "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_011_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_3_sw : texldl r# inside if !p.w block is allowed";
string VS_011 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.w "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_012_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_3_sw : texldl r# inside if (true) block is allowed";
string VS_012 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";
    
VS_CRITERIA VS_013_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_3_sw : texldl r# inside if !(true) block is allowed";
string VS_013 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_014_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_3_sw : texldl r# inside if (false) block is allowed";
string VS_014 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";
    
VS_CRITERIA VS_015_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_3_sw : texldl r# inside if !(false) block is allowed";
string VS_015 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";
    
VS_CRITERIA VS_016_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_3_sw : texldl r# inside loop block is allowed";
string VS_016 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_017_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_3_sw : texldl r# inside rep block is allowed";
string VS_017 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_018_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_3_sw : texldl r# inside call block is allowed";
string VS_018 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "call l0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_019_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_3_sw : texldl r# inside callnz (true) block is allowed";
string VS_019 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_020_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_3_sw : texldl r# inside callnz !(true) block is allowed";
string VS_020 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_021_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_3_sw : texldl r# inside callnz (false) block is allowed";
string VS_021 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_022_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_3_sw : texldl r# inside callnz !(false) block is allowed";
string VS_022 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_023_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_3_sw : texldl r# inside callnz !p.x block is allowed";
string VS_023 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.x "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";

VS_CRITERIA VS_024_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_3_sw : texldl r# inside callnz p.y block is allowed";
string VS_024 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.y "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_025_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_3_sw : texldl r# inside callnz !p.z block is allowed";
string VS_025 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.z "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_026_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_3_sw : texldl r# inside callnz p.w block is allowed";
string VS_026 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.w "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_027_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_3_sw : texldl r# inside loop block with break is allowed";
string VS_027 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_028_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_3_sw : texldl r# inside loop block with break is allowed";
string VS_028 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_029_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_3_sw : texldl r# inside rep block with break is allowed";
string VS_029 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_030_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_3_sw : texldl r# inside rep block with break is allowed";
string VS_030 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_031_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_3_sw : texldl r# inside loop block with break_gt is allowed";
string VS_031 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_032_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_3_sw : texldl r# inside loop block with break_gt is allowed";
string VS_032 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_033_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_3_sw : texldl r# inside loop block with break_lt is allowed";
string VS_033 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_034_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_3_sw : texldl r# inside loop block with break_lt is allowed";
string VS_034 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_035_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_3_sw : texldl r# inside loop block with break_ge is allowed";
string VS_035 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_036_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_3_sw : texldl r# inside loop block with break_ge is allowed";
string VS_036 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_037_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_3_sw : texldl r# inside loop block with break_le is allowed";
string VS_037 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_038_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_3_sw : texldl r# inside loop block with break_le is allowed";
string VS_038 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_039_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_3_sw : texldl r# inside loop block with break_eq is allowed";
string VS_039 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_040_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_3_sw : texldl r# inside loop block with break_eq is allowed";
string VS_040 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_041_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_3_sw : texldl r# inside loop block with break_ne is allowed";
string VS_041 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_042_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_3_sw : texldl r# inside loop block with break_ne is allowed";
string VS_042 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";    
    
VS_CRITERIA VS_043_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_3_sw : texldl r# inside rep block with break_gt is allowed";
string VS_043 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_044_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_3_sw : texldl r# inside rep block with break_gt is allowed";
string VS_044 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_045_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_3_sw : texldl r# inside rep block with break_lt is allowed";
string VS_045 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_046_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_3_sw : texldl r# inside rep block with break_lt is allowed";
string VS_046 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_047_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_3_sw : texldl r# inside rep block with break_ge is allowed";
string VS_047 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_048_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_3_sw : texldl r# inside rep block with break_ge is allowed";
string VS_048 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_049_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_3_sw : texldl r# inside rep block with break_le is allowed";
string VS_049 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_050_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_3_sw : texldl r# inside rep block with break_le is allowed";
string VS_050 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_051_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_3_sw : texldl r# inside rep block with break_eq is allowed";
string VS_051 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_052_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_3_sw : texldl r# inside rep block with break_eq is allowed";
string VS_052 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_053_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_3_sw : texldl r# inside rep block with break_ne is allowed";
string VS_053 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_054_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_3_sw : texldl r# inside rep block with break_ne is allowed";
string VS_054 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_055_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_3_sw : texldl r# inside loop block with breakp !p.x is allowed";
string VS_055 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.x "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 "; 

VS_CRITERIA VS_056_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_3_sw : texldl r# inside loop block with breakp p.x is allowed";
string VS_056 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	breakp p0.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_057_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_3_sw : texldl r# inside loop block with breakp p.y is allowed";
string VS_057 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.y "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 "; 

VS_CRITERIA VS_058_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_3_sw : texldl r# inside loop block with breakp !p.y is allowed";
string VS_058 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	breakp !p0.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_059_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_3_sw : texldl r# inside loop block with breakp !p.z is allowed";
string VS_059 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.z "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 "; 

VS_CRITERIA VS_060_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_3_sw : texldl r# inside loop block with breakp p.z is allowed";
string VS_060 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	breakp p0.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_061_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_3_sw : texldl r# inside loop block with breakp p.w is allowed";
string VS_061 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.w "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 "; 

VS_CRITERIA VS_062_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_3_sw : texldl r# inside loop block with breakp !p.w is allowed";
string VS_062 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	breakp !p0.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_063_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_3_sw : texldl r# inside rep block with breakp p.x is allowed";
string VS_063 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.x "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_064_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_3_sw : texldl r# inside rep block with breakp !p.x is allowed";
string VS_064 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	breakp !p0.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_065_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_3_sw : texldl r# inside rep block with breakp !p.y is allowed";
string VS_065 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.y "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_066_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_3_sw : texldl r# inside rep block with breakp p.y is allowed";
string VS_066 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	breakp p0.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_067_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_3_sw : texldl r# inside rep block with breakp p.z is allowed";
string VS_067 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.z "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_068_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_3_sw : texldl r# inside rep block with breakp !p.z is allowed";
string VS_068 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	breakp !p0.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_069_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_3_sw : texldl r# inside rep block with breakp !p.w is allowed";
string VS_069 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.w "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_070_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_3_sw : texldl r# inside rep block with breakp p.w is allowed";
string VS_070 =
	"vs_3_sw "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	breakp p0.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
//cases with texldl r2, t#, s0

VS_CRITERIA VS_071_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_3_sw : texldl v# is allowed";
string VS_071 =
    "vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "texldl r2, v0, s0 "
    "mov o0, r2 ";

VS_CRITERIA VS_072_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_3_sw : texldl v# inside if_gt block is allowed";
string VS_072 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_gt c0.x, r0.x "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_073_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_3_sw : texldl v# inside if_lt block is allowed";
string VS_073 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_lt c0.y, r0.y "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_074_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_3_sw : texldl v# inside if_ge block is allowed";
string VS_074 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ge c0.z, r0.z "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_075_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_3_sw : texldl v# inside if_le block is allowed";
string VS_075 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_le c0.w, r0.w "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_076_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_3_sw : texldl v# inside if_eq block is allowed";
string VS_076 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_eq c0.x, r0.y "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_077_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_3_sw : texldl v# inside if_ne block is allowed";
string VS_077 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ne c0.z, r0.w "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_078_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_3_sw : texldl v# inside if p.x block is allowed";
string VS_078 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.x "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_079_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_3_sw : texldl v# inside if !p.y block is allowed";
string VS_079 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.y "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_080_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_3_sw : texldl v# inside if p.z block is allowed";
string VS_080 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.z "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_081_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_3_sw : texldl v# inside if !p.w block is allowed";
string VS_081 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.w "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_082_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_3_sw : texldl v# inside if (true) block is allowed";
string VS_082 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";
    
VS_CRITERIA VS_083_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_3_sw : texldl v# inside if !(true) block is allowed";
string VS_083 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_084_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_3_sw : texldl v# inside if (false) block is allowed";
string VS_084 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";
    
VS_CRITERIA VS_085_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_3_sw : texldl v# inside if !(false) block is allowed";
string VS_085 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";
    

VS_CRITERIA VS_086_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_3_sw : texldl v# inside loop block is allowed";
string VS_086 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_087_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_3_sw : texldl v# inside rep block is allowed";
string VS_087 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_088_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_3_sw : texldl v# inside call block is allowed";
string VS_088 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "call l0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, v0, s0 "
    "ret ";
    
VS_CRITERIA VS_089_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_089_Desc = "vs_3_sw : texldl v# inside callnz (true) block is allowed";
string VS_089 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, v0, s0 "
    "ret ";
    
VS_CRITERIA VS_090_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_3_sw : texldl v# inside callnz !(true) block is allowed";
string VS_090 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, v0, s0 "
    "ret ";
    
VS_CRITERIA VS_091_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_3_sw : texldl v# inside callnz (false) block is allowed";
string VS_091 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, v0, s0 "
    "ret ";
    
VS_CRITERIA VS_092_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_3_sw : texldl v# inside callnz !(false) block is allowed";
string VS_092 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, v0, s0 "
    "ret ";
    
VS_CRITERIA VS_093_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_3_sw : texldl v# inside callnz !p.x block is allowed";
string VS_093 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.x "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, v0, s0 "
    "ret ";

VS_CRITERIA VS_094_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_3_sw : texldl v# inside callnz p.y block is allowed";
string VS_094 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.y "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, v0, s0 "
    "ret ";
    
VS_CRITERIA VS_095_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_3_sw : texldl v# inside callnz !p.z block is allowed";
string VS_095 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.z "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, v0, s0 "
    "ret ";
    
VS_CRITERIA VS_096_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_3_sw : texldl v# inside callnz p.w block is allowed";
string VS_096 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.w "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, v0, s0 "
    "ret ";
    

VS_CRITERIA VS_097_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_3_sw : texldl v# inside loop block with break is allowed";
string VS_097 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	break "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_098_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_3_sw : texldl v# inside loop block with break is allowed";
string VS_098 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_099_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_3_sw : texldl v# inside rep block with break is allowed";
string VS_099 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	break "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_100_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_3_sw : texldl v# inside rep block with break is allowed";
string VS_100 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 ";
    

VS_CRITERIA VS_101_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_3_sw : texldl v# inside loop block with break_gt is allowed";
string VS_101 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_102_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_3_sw : texldl v# inside loop block with break_gt is allowed";
string VS_102 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_103_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_3_sw : texldl v# inside loop block with break_lt is allowed";
string VS_103 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_104_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_3_sw : texldl v# inside loop block with break_lt is allowed";
string VS_104 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_105_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_3_sw : texldl v# inside loop block with break_ge is allowed";
string VS_105 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_106_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_3_sw : texldl v# inside loop block with break_ge is allowed";
string VS_106 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_107_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_3_sw : texldl v# inside loop block with break_le is allowed";
string VS_107 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_108_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_3_sw : texldl v# inside loop block with break_le is allowed";
string VS_108 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_109_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_3_sw : texldl v# inside loop block with break_eq is allowed";
string VS_109 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_110_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_3_sw : texldl v# inside loop block with break_eq is allowed";
string VS_110 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_111_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_3_sw : texldl v# inside loop block with break_ne is allowed";
string VS_111 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_112_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_3_sw : texldl v# inside loop block with break_ne is allowed";
string VS_112 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 ";    
    
VS_CRITERIA VS_113_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_3_sw : texldl v# inside rep block with break_gt is allowed";
string VS_113 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_114_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_3_sw : texldl v# inside rep block with break_gt is allowed";
string VS_114 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_115_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_3_sw : texldl v# inside rep block with break_lt is allowed";
string VS_115 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_116_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_3_sw : texldl v# inside rep block with break_lt is allowed";
string VS_116 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_117_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_3_sw : texldl v# inside rep block with break_ge is allowed";
string VS_117 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_118_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_3_sw : texldl v# inside rep block with break_ge is allowed";
string VS_118 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_119_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_3_sw : texldl v# inside rep block with break_le is allowed";
string VS_119 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_120_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_3_sw : texldl v# inside rep block with break_le is allowed";
string VS_120 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_121_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_3_sw : texldl v# inside rep block with break_eq is allowed";
string VS_121 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_122_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_3_sw : texldl v# inside rep block with break_eq is allowed";
string VS_122 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_123_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_3_sw : texldl v# inside rep block with break_ne is allowed";
string VS_123 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_124_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_3_sw : texldl v# inside rep block with break_ne is allowed";
string VS_124 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 ";
    

VS_CRITERIA VS_125_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_3_sw : texldl v# inside loop block with breakp !p.x is allowed";
string VS_125 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.x "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 "; 


VS_CRITERIA VS_126_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_3_sw : texldl v# inside loop block with breakp p.x is allowed";
string VS_126 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	breakp p0.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_127_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_3_sw : texldl v# inside loop block with breakp p.y is allowed";
string VS_127 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.y "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 "; 


VS_CRITERIA VS_128_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_3_sw : texldl v# inside loop block with breakp !p.y is allowed";
string VS_128 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	breakp !p0.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_129_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_3_sw : texldl v# inside loop block with breakp !p.z is allowed";
string VS_129 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.z "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 "; 


VS_CRITERIA VS_130_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_130_Desc = "vs_3_sw : texldl v# inside loop block with breakp p.z is allowed";
string VS_130 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	breakp p0.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_131_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_3_sw : texldl v# inside loop block with breakp p.w is allowed";
string VS_131 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.w "
    "	texldl r2, v0, s0 "
    "endloop "
    "mov o0, r2 "; 


VS_CRITERIA VS_132_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_3_sw : texldl v# inside loop block with breakp !p.w is allowed";
string VS_132 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, v0, s0 "
    "	breakp !p0.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_133_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_3_sw : texldl v# inside rep block with breakp p.x is allowed";
string VS_133 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.x "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_134_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_3_sw : texldl v# inside rep block with breakp !p.x is allowed";
string VS_134 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	breakp !p0.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_135_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_135_Desc = "vs_3_sw : texldl v# inside rep block with breakp !p.y is allowed";
string VS_135 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.y "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_136_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_136_Desc = "vs_3_sw : texldl v# inside rep block with breakp p.y is allowed";
string VS_136 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	breakp p0.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_137_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_137_Desc = "vs_3_sw : texldl v# inside rep block with breakp p.z is allowed";
string VS_137 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.z "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_138_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_138_Desc = "vs_3_sw : texldl v# inside rep block with breakp !p.z is allowed";
string VS_138 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	breakp !p0.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_139_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_139_Desc = "vs_3_sw : texldl v# inside rep block with breakp !p.w is allowed";
string VS_139 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.w "
    "	texldl r2, v0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_140_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_140_Desc = "vs_3_sw : texldl v# inside rep block with breakp p.w is allowed";
string VS_140 =
	"vs_3_sw "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, v0, s0 "
    "	breakp p0.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
    