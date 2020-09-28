#include "Test_Include.fx"

int Test_Count = 140;

string TestInfo
<
    string TestType = "PS";
>
= "ps2_x_flow_control_texldp";

//cases with texldp r2, r#, s0

PS_CRITERIA PS_001_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_001_Desc = "ps_2_x : texldp r# is allowed";
string PS_001 =
    "ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "texldp r2, r0, s0 "
    "mov oC0, r2 ";

PS_CRITERIA PS_002_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_002_Desc = "ps_2_x : texldp r# inside if_gt block is NOT allowed";
string PS_002 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_gt c0.x, r0.x "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_003_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_003_Desc = "ps_2_x : texldp r# inside if_lt block is NOT allowed";
string PS_003 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_lt c0.y, r0.y "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_004_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_004_Desc = "ps_2_x : texldp r# inside if_ge block is NOT allowed";
string PS_004 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ge c0.z, r0.z "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_005_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_005_Desc = "ps_2_x : texldp r# inside if_le block is NOT allowed";
string PS_005 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_le c0.w, r0.w "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_006_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_006_Desc = "ps_2_x : texldp r# inside if_eq block is NOT allowed";
string PS_006 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_eq c0.x, r0.y "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_007_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_007_Desc = "ps_2_x : texldp r# inside if_ne block is NOT allowed";
string PS_007 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ne c0.z, r0.w "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_008_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_008_Desc = "ps_2_x : texldp r# inside if p.x block is NOT allowed";
string PS_008 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.x "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_009_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_009_Desc = "ps_2_x : texldp r# inside if !p.y block is NOT allowed";
string PS_009 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.y "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_010_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_010_Desc = "ps_2_x : texldp r# inside if p.z block is NOT allowed";
string PS_010 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.z "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_011_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_011_Desc = "ps_2_x : texldp r# inside if !p.w block is NOT allowed";
string PS_011 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.w "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_012_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_012_Desc = "ps_2_x : texldp r# inside if (true) block is allowed";
string PS_012 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_013_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_013_Desc = "ps_2_x : texldp r# inside if !(true) block is allowed";
string PS_013 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_014_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_014_Desc = "ps_2_x : texldp r# inside if (false) block is allowed";
string PS_014 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_015_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_015_Desc = "ps_2_x : texldp r# inside if !(false) block is allowed";
string PS_015 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	texldp r2, r0, s0 "
    "else "
    "	texldp r2, r1, s0 "
    "endif "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_016_Criteria = { false, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_016_Desc = "ps_2_x : texldp r# inside loop block is allowed";
string PS_016 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_017_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_017_Desc = "ps_2_x : texldp r# inside rep block is allowed";
string PS_017 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_018_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_018_Desc = "ps_2_x : texldp r# inside call block is allowed";
string PS_018 =
	"ps_2_x "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "call l0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, r0, s0 "
    "ret ";
    
PS_CRITERIA PS_019_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_019_Desc = "ps_2_x : texldp r# inside callnz (true) block is allowed";
string PS_019 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, r0, s0 "
    "ret ";
    
PS_CRITERIA PS_020_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_020_Desc = "ps_2_x : texldp r# inside callnz !(true) block is allowed";
string PS_020 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, r0, s0 "
    "ret ";
    
PS_CRITERIA PS_021_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_2_x : texldp r# inside callnz (false) block is allowed";
string PS_021 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, r0, s0 "
    "ret ";
    
PS_CRITERIA PS_022_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_2_x : texldp r# inside callnz !(false) block is allowed";
string PS_022 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, r0, s0 "
    "ret ";
    
PS_CRITERIA PS_023_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_023_Desc = "ps_2_x : texldp r# inside callnz !p.x block is NOT allowed";
string PS_023 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.x "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, r0, s0 "
    "ret ";

PS_CRITERIA PS_024_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_024_Desc = "ps_2_x : texldp r# inside callnz p.y block is NOT allowed";
string PS_024 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.y "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, r0, s0 "
    "ret ";
    
PS_CRITERIA PS_025_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_025_Desc = "ps_2_x : texldp r# inside callnz !p.z block is NOT allowed";
string PS_025 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.z "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, r0, s0 "
    "ret ";
    
PS_CRITERIA PS_026_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_026_Desc = "ps_2_x : texldp r# inside callnz p.w block is NOT allowed";
string PS_026 =
	"ps_2_x "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.w "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, r0, s0 "
    "ret ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_027_Criteria = { false, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_x : texldp r# inside loop block with break is NOT allowed";
string PS_027 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	break "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_028_Criteria = { false, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_x : texldp r# inside loop block with break is NOT allowed";
string PS_028 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_029_Criteria = { false, false, 0x0200, -1, -1, 1, -1, 0, -1, 9.029f };
string PS_029_Desc = "ps_2_x : texldp r# inside rep block with break is NOT allowed";
string PS_029 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	break "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_030_Criteria = { false, false, 0x0200, -1, -1, 1, -1, 0, -1, 9.029f };
string PS_030_Desc = "ps_2_x : texldp r# inside rep block with break is NOT allowed";
string PS_030 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_031_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_2_x : texldp r# inside loop block with break_gt is NOT allowed";
string PS_031 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_032_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_2_x : texldp r# inside loop block with break_gt is NOT allowed";
string PS_032 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_033_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_2_x : texldp r# inside loop block with break_lt is NOT allowed";
string PS_033 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_034_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_2_x : texldp r# inside loop block with break_lt is NOT allowed";
string PS_034 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_035_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_2_x : texldp r# inside loop block with break_ge is NOT allowed";
string PS_035 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_036_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_2_x : texldp r# inside loop block with break_ge is NOT allowed";
string PS_036 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_037_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_2_x : texldp r# inside loop block with break_le is NOT allowed";
string PS_037 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_038_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_2_x : texldp r# inside loop block with break_le is NOT allowed";
string PS_038 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_039_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_2_x : texldp r# inside loop block with break_eq is NOT allowed";
string PS_039 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_040_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_2_x : texldp r# inside loop block with break_eq is NOT allowed";
string PS_040 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_041_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_2_x : texldp r# inside loop block with break_ne is NOT allowed";
string PS_041 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_042_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_2_x : texldp r# inside loop block with break_ne is NOT allowed";
string PS_042 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 ";    
    
PS_CRITERIA PS_043_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_043_Desc = "ps_2_x : texldp r# inside rep block with break_gt is NOT allowed";
string PS_043 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_044_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_044_Desc = "ps_2_x : texldp r# inside rep block with break_gt is NOT allowed";
string PS_044 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_045_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_045_Desc = "ps_2_x : texldp r# inside rep block with break_lt is NOT allowed";
string PS_045 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_046_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_046_Desc = "ps_2_x : texldp r# inside rep block with break_lt is NOT allowed";
string PS_046 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_047_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_047_Desc = "ps_2_x : texldp r# inside rep block with break_ge is NOT allowed";
string PS_047 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_048_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_048_Desc = "ps_2_x : texldp r# inside rep block with break_ge is NOT allowed";
string PS_048 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_049_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_049_Desc = "ps_2_x : texldp r# inside rep block with break_le is NOT allowed";
string PS_049 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_050_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_050_Desc = "ps_2_x : texldp r# inside rep block with break_le is NOT allowed";
string PS_050 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_051_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_051_Desc = "ps_2_x : texldp r# inside rep block with break_eq is NOT allowed";
string PS_051 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_052_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_052_Desc = "ps_2_x : texldp r# inside rep block with break_eq is NOT allowed";
string PS_052 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_053_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_053_Desc = "ps_2_x : texldp r# inside rep block with break_ne is NOT allowed";
string PS_053 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_054_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 9.029f };
string PS_054_Desc = "ps_2_x : texldp r# inside rep block with break_ne is NOT allowed";
string PS_054 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_055_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_055_Desc = "ps_2_x : texldp r# inside loop block with breakp !p.x is NOT allowed";
string PS_055 =
	"ps_2_x "
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
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 "; 

//loop is only supported on ps_3_0
PS_CRITERIA PS_056_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_056_Desc = "ps_2_x : texldp r# inside loop block with breakp p.x is NOT allowed";
string PS_056 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	breakp p0.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_057_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_057_Desc = "ps_2_x : texldp r# inside loop block with breakp p.y is NOT allowed";
string PS_057 =
	"ps_2_x "
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
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 "; 

//loop is only supported on ps_3_0
PS_CRITERIA PS_058_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_058_Desc = "ps_2_x : texldp r# inside loop block with breakp !p.y is NOT allowed";
string PS_058 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	breakp !p0.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_059_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_059_Desc = "ps_2_x : texldp r# inside loop block with breakp !p.z is NOT allowed";
string PS_059 =
	"ps_2_x "
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
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 "; 

//loop is only supported on ps_3_0
PS_CRITERIA PS_060_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_060_Desc = "ps_2_x : texldp r# inside loop block with breakp p.z is NOT allowed";
string PS_060 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	breakp p0.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_061_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_061_Desc = "ps_2_x : texldp r# inside loop block with breakp p.w is NOT allowed";
string PS_061 =
	"ps_2_x "
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
    "	texldp r2, r0, s0 "
    "endloop "
    "mov oC0, r2 "; 

//loop is only supported on ps_3_0
PS_CRITERIA PS_062_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_062_Desc = "ps_2_x : texldp r# inside loop block with breakp !p.w is NOT allowed";
string PS_062 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldp r2, r0, s0 "
    "	breakp !p0.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_063_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_063_Desc = "ps_2_x : texldp r# inside rep block with breakp p.x is NOT allowed";
string PS_063 =
	"ps_2_x "
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
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_064_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_064_Desc = "ps_2_x : texldp r# inside rep block with breakp !p.x is NOT allowed";
string PS_064 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	breakp !p0.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_065_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_065_Desc = "ps_2_x : texldp r# inside rep block with breakp !p.y is NOT allowed";
string PS_065 =
	"ps_2_x "
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
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_066_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_066_Desc = "ps_2_x : texldp r# inside rep block with breakp p.y is NOT allowed";
string PS_066 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	breakp p0.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_067_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_067_Desc = "ps_2_x : texldp r# inside rep block with breakp p.z is NOT allowed";
string PS_067 =
	"ps_2_x "
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
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_068_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_068_Desc = "ps_2_x : texldp r# inside rep block with breakp !p.z is NOT allowed";
string PS_068 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	breakp !p0.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_069_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_069_Desc = "ps_2_x : texldp r# inside rep block with breakp !p.w is NOT allowed";
string PS_069 =
	"ps_2_x "
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
    "	texldp r2, r0, s0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_070_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 9.029f };
string PS_070_Desc = "ps_2_x : texldp r# inside rep block with breakp p.w is NOT allowed";
string PS_070 =
	"ps_2_x "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldp r2, r0, s0 "
    "	breakp p0.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
//cases with texldp r2, t0, s0

PS_CRITERIA PS_071_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_071_Desc = "ps_2_x : texldp t# is allowed";
string PS_071 =
    "ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "texldp r2, t0, s0 "
    "mov oC0, r2 ";

PS_CRITERIA PS_072_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_072_Desc = "ps_2_x : texldp t# inside if_gt block is allowed";
string PS_072 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_gt c0.x, r0.x "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_073_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_073_Desc = "ps_2_x : texldp t# inside if_lt block is allowed";
string PS_073 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_lt c0.y, r0.y "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_074_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_074_Desc = "ps_2_x : texldp t# inside if_ge block is allowed";
string PS_074 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ge c0.z, r0.z "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_075_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_075_Desc = "ps_2_x : texldp t# inside if_le block is allowed";
string PS_075 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_le c0.w, r0.w "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_076_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_076_Desc = "ps_2_x : texldp t# inside if_eq block is allowed";
string PS_076 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_eq c0.x, r0.y "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_077_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_077_Desc = "ps_2_x : texldp t# inside if_ne block is allowed";
string PS_077 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ne c0.z, r0.w "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_078_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_078_Desc = "ps_2_x : texldp t# inside if p.x block is allowed";
string PS_078 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.x "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_079_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_079_Desc = "ps_2_x : texldp t# inside if !p.y block is allowed";
string PS_079 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.y "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_080_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_080_Desc = "ps_2_x : texldp t# inside if p.z block is allowed";
string PS_080 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.z "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_081_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_081_Desc = "ps_2_x : texldp t# inside if !p.w block is allowed";
string PS_081 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.w "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_082_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_2_x : texldp t# inside if (true) block is allowed";
string PS_082 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_083_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_2_x : texldp t# inside if !(true) block is allowed";
string PS_083 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_084_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_2_x : texldp t# inside if (false) block is allowed";
string PS_084 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_085_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_2_x : texldp t# inside if !(false) block is allowed";
string PS_085 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	texldp r2, t0, s0 "
    "else "
    "	texldp r2, t1, s0 "
    "endif "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_086_Criteria = { false, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_2_x : texldp t# inside loop block is allowed";
string PS_086 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_087_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_2_x : texldp t# inside rep block is allowed";
string PS_087 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_088_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_2_x : texldp t# inside call block is allowed";
string PS_088 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "call l0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, t0, s0 "
    "ret ";
    
PS_CRITERIA PS_089_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_2_x : texldp t# inside callnz (true) block is allowed";
string PS_089 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, t0, s0 "
    "ret ";
    
PS_CRITERIA PS_090_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_2_x : texldp t# inside callnz !(true) block is allowed";
string PS_090 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, t0, s0 "
    "ret ";
    
PS_CRITERIA PS_091_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_2_x : texldp t# inside callnz (false) block is allowed";
string PS_091 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, t0, s0 "
    "ret ";
    
PS_CRITERIA PS_092_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_2_x : texldp t# inside callnz !(false) block is allowed";
string PS_092 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, t0, s0 "
    "ret ";
    
PS_CRITERIA PS_093_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_093_Desc = "ps_2_x : texldp t# inside callnz !p.x block is allowed";
string PS_093 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.x "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, t0, s0 "
    "ret ";

PS_CRITERIA PS_094_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_094_Desc = "ps_2_x : texldp t# inside callnz p.y block is allowed";
string PS_094 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.y "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, t0, s0 "
    "ret ";
    
PS_CRITERIA PS_095_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_095_Desc = "ps_2_x : texldp t# inside callnz !p.z block is allowed";
string PS_095 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.z "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, t0, s0 "
    "ret ";
    
PS_CRITERIA PS_096_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_096_Desc = "ps_2_x : texldp t# inside callnz p.w block is allowed";
string PS_096 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.w "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	texldp r2, t0, s0 "
    "ret ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_097_Criteria = { false, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_2_x : texldp t# inside loop block with break is allowed";
string PS_097 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	break "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_098_Criteria = { false, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_2_x : texldp t# inside loop block with break is allowed";
string PS_098 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_099_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_2_x : texldp t# inside rep block with break is allowed";
string PS_099 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	break "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_100_Criteria = { true, false, 0x0200, -1, -1, 1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_2_x : texldp t# inside rep block with break is allowed";
string PS_100 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_101_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_2_x : texldp t# inside loop block with break_gt is allowed";
string PS_101 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_102_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_2_x : texldp t# inside loop block with break_gt is allowed";
string PS_102 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_103_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_2_x : texldp t# inside loop block with break_lt is allowed";
string PS_103 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_104_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_2_x : texldp t# inside loop block with break_lt is allowed";
string PS_104 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_105_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_2_x : texldp t# inside loop block with break_ge is allowed";
string PS_105 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_106_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_2_x : texldp t# inside loop block with break_ge is allowed";
string PS_106 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_107_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_2_x : texldp t# inside loop block with break_le is allowed";
string PS_107 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_108_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_2_x : texldp t# inside loop block with break_le is allowed";
string PS_108 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_109_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_2_x : texldp t# inside loop block with break_eq is allowed";
string PS_109 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_110_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_2_x : texldp t# inside loop block with break_eq is allowed";
string PS_110 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_111_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_2_x : texldp t# inside loop block with break_ne is allowed";
string PS_111 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_112_Criteria = { false, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_2_x : texldp t# inside loop block with break_ne is allowed";
string PS_112 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 ";    
    
PS_CRITERIA PS_113_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_2_x : texldp t# inside rep block with break_gt is allowed";
string PS_113 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_114_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_2_x : texldp t# inside rep block with break_gt is allowed";
string PS_114 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_115_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_2_x : texldp t# inside rep block with break_lt is allowed";
string PS_115 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_116_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_2_x : texldp t# inside rep block with break_lt is allowed";
string PS_116 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_117_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_2_x : texldp t# inside rep block with break_ge is allowed";
string PS_117 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_118_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_2_x : texldp t# inside rep block with break_ge is allowed";
string PS_118 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_119_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_2_x : texldp t# inside rep block with break_le is allowed";
string PS_119 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_120_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_2_x : texldp t# inside rep block with break_le is allowed";
string PS_120 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_121_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_2_x : texldp t# inside rep block with break_eq is allowed";
string PS_121 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_122_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_2_x : texldp t# inside rep block with break_eq is allowed";
string PS_122 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_123_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_2_x : texldp t# inside rep block with break_ne is allowed";
string PS_123 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_124_Criteria = { true, false, 0x0200, 1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_2_x : texldp t# inside rep block with break_ne is allowed";
string PS_124 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_125_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_125_Desc = "ps_2_x : texldp t# inside loop block with breakp !p.x is allowed";
string PS_125 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
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
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 "; 

//loop is only supported on ps_3_0
PS_CRITERIA PS_126_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_126_Desc = "ps_2_x : texldp t# inside loop block with breakp p.x is allowed";
string PS_126 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	breakp p0.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_127_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_127_Desc = "ps_2_x : texldp t# inside loop block with breakp p.y is allowed";
string PS_127 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
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
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 "; 

//loop is only supported on ps_3_0
PS_CRITERIA PS_128_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_128_Desc = "ps_2_x : texldp t# inside loop block with breakp !p.y is allowed";
string PS_128 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	breakp !p0.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_129_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_129_Desc = "ps_2_x : texldp t# inside loop block with breakp !p.z is allowed";
string PS_129 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
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
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 "; 

//loop is only supported on ps_3_0
PS_CRITERIA PS_130_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_130_Desc = "ps_2_x : texldp t# inside loop block with breakp p.z is allowed";
string PS_130 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	breakp p0.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
//loop is only supported on ps_3_0
PS_CRITERIA PS_131_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_131_Desc = "ps_2_x : texldp t# inside loop block with breakp p.w is allowed";
string PS_131 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
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
    "	texldp r2, t0, s0 "
    "endloop "
    "mov oC0, r2 "; 

//loop is only supported on ps_3_0
PS_CRITERIA PS_132_Criteria = { false, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_132_Desc = "ps_2_x : texldp t# inside loop block with breakp !p.w is allowed";
string PS_132 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldp r2, t0, s0 "
    "	breakp !p0.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_133_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_133_Desc = "ps_2_x : texldp t# inside rep block with breakp p.x is allowed";
string PS_133 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
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
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_134_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_134_Desc = "ps_2_x : texldp t# inside rep block with breakp !p.x is allowed";
string PS_134 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	breakp !p0.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_135_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_135_Desc = "ps_2_x : texldp t# inside rep block with breakp !p.y is allowed";
string PS_135 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
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
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_136_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_136_Desc = "ps_2_x : texldp t# inside rep block with breakp p.y is allowed";
string PS_136 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	breakp p0.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_137_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_137_Desc = "ps_2_x : texldp t# inside rep block with breakp p.z is allowed";
string PS_137 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
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
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_138_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_138_Desc = "ps_2_x : texldp t# inside rep block with breakp !p.z is allowed";
string PS_138 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	breakp !p0.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_139_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_139_Desc = "ps_2_x : texldp t# inside rep block with breakp !p.w is allowed";
string PS_139 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
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
    "	texldp r2, t0, s0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_140_Criteria = { true, false, 0x0200, 1, -1, -1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_140_Desc = "ps_2_x : texldp t# inside rep block with breakp p.w is allowed";
string PS_140 =
	"ps_2_x "
    "dcl t0 "
    "dcl t1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldp r2, t0, s0 "
    "	breakp p0.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
    