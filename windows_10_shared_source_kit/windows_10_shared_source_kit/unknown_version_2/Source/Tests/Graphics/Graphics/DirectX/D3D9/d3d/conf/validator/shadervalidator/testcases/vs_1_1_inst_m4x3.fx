wed";
string PS_077 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ne c0.z, r0.w "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_078_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_078_Desc = "ps_3_0 : dsx v# inside if p.x block is allowed";
string PS_078 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.x "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_079_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_079_Desc = "ps_3_0 : dsx v# inside if !p.y block is allowed";
string PS_079 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.y "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_080_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_080_Desc = "ps_3_0 : dsx v# inside if p.z block is allowed";
string PS_080 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.z "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_081_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_081_Desc = "ps_3_0 : dsx v# inside if !p.w block is allowed";
string PS_081 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.w "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_082_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_082_Desc = "ps_3_0 : dsx v# inside if (true) block is allowed";
string PS_082 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_083_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_083_Desc = "ps_3_0 : dsx v# inside if !(true) block is allowed";
string PS_083 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";

PS_CRITERIA PS_084_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_084_Desc = "ps_3_0 : dsx v# inside if (false) block is allowed";
string PS_084 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_085_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_085_Desc = "ps_3_0 : dsx v# inside if !(false) block is allowed";
string PS_085 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	dsx r2, v0 "
    "else "
    "	dsx r2, v1 "
    "endif "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_086_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_086_Desc = "ps_3_0 : dsx v# inside loop block is allowed";
string PS_086 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_087_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_087_Desc = "ps_3_0 : dsx v# inside rep block is allowed";
string PS_087 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_088_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_088_Desc = "ps_3_0 : dsx v# inside call block is allowed";
string PS_088 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "call l0 "
    "mov oC0, r2 "
	"ret "
    "label l0 "
    "	dsx r2, v0 "
    "ret ";
    
PS_CRITERIA PS_089_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_089_Desc = "ps_3_0 : dsx v# inside callnz (true) block is allowed";
string PS_089 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "ret ";
    
PS_CRITERIA PS_090_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_090_Desc = "ps_3_0 : dsx v# inside callnz !(true) block is allowed";
string PS_090 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "ret ";
    
PS_CRITERIA PS_091_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_091_Desc = "ps_3_0 : dsx v# inside callnz (false) block is allowed";
string PS_091 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "ret ";
    
PS_CRITERIA PS_092_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_092_Desc = "ps_3_0 : dsx v# inside callnz !(false) block is allowed";
string PS_092 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "ret ";
    
PS_CRITERIA PS_093_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_093_Desc = "ps_3_0 : dsx v# inside callnz !p.x block is allowed";
string PS_093 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "ret ";

PS_CRITERIA PS_094_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_094_Desc = "ps_3_0 : dsx v# inside callnz p.y block is allowed";
string PS_094 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "ret ";
    
PS_CRITERIA PS_095_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_095_Desc = "ps_3_0 : dsx v# inside callnz !p.z block is allowed";
string PS_095 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "ret ";
    
PS_CRITERIA PS_096_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_096_Desc = "ps_3_0 : dsx v# inside callnz p.w block is allowed";
string PS_096 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "ret ";
    

PS_CRITERIA PS_097_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_097_Desc = "ps_3_0 : dsx v# inside loop block with break is allowed";
string PS_097 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	break "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_098_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_098_Desc = "ps_3_0 : dsx v# inside loop block with break is allowed";
string PS_098 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_099_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_099_Desc = "ps_3_0 : dsx v# inside rep block with break is allowed";
string PS_099 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, v0 "
    "	break "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_100_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_100_Desc = "ps_3_0 : dsx v# inside rep block with break is allowed";
string PS_100 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_101_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_101_Desc = "ps_3_0 : dsx v# inside loop block with break_gt is allowed";
string PS_101 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_102_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_102_Desc = "ps_3_0 : dsx v# inside loop block with break_gt is allowed";
string PS_102 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_103_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_103_Desc = "ps_3_0 : dsx v# inside loop block with break_lt is allowed";
string PS_103 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_104_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_104_Desc = "ps_3_0 : dsx v# inside loop block with break_lt is allowed";
string PS_104 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_105_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_105_Desc = "ps_3_0 : dsx v# inside loop block with break_ge is allowed";
string PS_105 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_106_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_106_Desc = "ps_3_0 : dsx v# inside loop block with break_ge is allowed";
string PS_106 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_107_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_107_Desc = "ps_3_0 : dsx v# inside loop block with break_le is allowed";
string PS_107 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_108_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_108_Desc = "ps_3_0 : dsx v# inside loop block with break_le is allowed";
string PS_108 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_109_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_109_Desc = "ps_3_0 : dsx v# inside loop block with break_eq is allowed";
string PS_109 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_110_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_110_Desc = "ps_3_0 : dsx v# inside loop block with break_eq is allowed";
string PS_110 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_111_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_111_Desc = "ps_3_0 : dsx v# inside loop block with break_ne is allowed";
string PS_111 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_112_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_112_Desc = "ps_3_0 : dsx v# inside loop block with break_ne is allowed";
string PS_112 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 ";    
    
PS_CRITERIA PS_113_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_113_Desc = "ps_3_0 : dsx v# inside rep block with break_gt is allowed";
string PS_113 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, v0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_114_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_114_Desc = "ps_3_0 : dsx v# inside rep block with break_gt is allowed";
string PS_114 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_115_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_115_Desc = "ps_3_0 : dsx v# inside rep block with break_lt is allowed";
string PS_115 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, v0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_116_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_116_Desc = "ps_3_0 : dsx v# inside rep block with break_lt is allowed";
string PS_116 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_117_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_117_Desc = "ps_3_0 : dsx v# inside rep block with break_ge is allowed";
string PS_117 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, v0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_118_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_118_Desc = "ps_3_0 : dsx v# inside rep block with break_ge is allowed";
string PS_118 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_119_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_119_Desc = "ps_3_0 : dsx v# inside rep block with break_le is allowed";
string PS_119 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, v0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_120_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_120_Desc = "ps_3_0 : dsx v# inside rep block with break_le is allowed";
string PS_120 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_121_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_121_Desc = "ps_3_0 : dsx v# inside rep block with break_eq is allowed";
string PS_121 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, v0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_122_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_122_Desc = "ps_3_0 : dsx v# inside rep block with break_eq is allowed";
string PS_122 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_123_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_123_Desc = "ps_3_0 : dsx v# inside rep block with break_ne is allowed";
string PS_123 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	dsx r2, v0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_124_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_124_Desc = "ps_3_0 : dsx v# inside rep block with break_ne is allowed";
string PS_124 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_125_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_125_Desc = "ps_3_0 : dsx v# inside loop block with breakp !p.x is allowed";
string PS_125 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 "; 


PS_CRITERIA PS_126_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_126_Desc = "ps_3_0 : dsx v# inside loop block with breakp p.x is allowed";
string PS_126 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	breakp p0.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_127_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_127_Desc = "ps_3_0 : dsx v# inside loop block with breakp p.y is allowed";
string PS_127 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 "; 


PS_CRITERIA PS_128_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_128_Desc = "ps_3_0 : dsx v# inside loop block with breakp !p.y is allowed";
string PS_128 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	breakp !p0.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_129_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_129_Desc = "ps_3_0 : dsx v# inside loop block with breakp !p.z is allowed";
string PS_129 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 "; 


PS_CRITERIA PS_130_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_130_Desc = "ps_3_0 : dsx v# inside loop block with breakp p.z is allowed";
string PS_130 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	breakp p0.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    

PS_CRITERIA PS_131_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_131_Desc = "ps_3_0 : dsx v# inside loop block with breakp p.w is allowed";
string PS_131 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "endloop "
    "mov oC0, r2 "; 


PS_CRITERIA PS_132_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_132_Desc = "ps_3_0 : dsx v# inside loop block with breakp !p.w is allowed";
string PS_132 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	dsx r2, v0 "
    "	breakp !p0.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_133_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_133_Desc = "ps_3_0 : dsx v# inside rep block with breakp p.x is allowed";
string PS_133 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_134_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_134_Desc = "ps_3_0 : dsx v# inside rep block with breakp !p.x is allowed";
string PS_134 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	dsx r2, v0 "
    "	breakp !p0.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_135_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_135_Desc = "ps_3_0 : dsx v# inside rep block with breakp !p.y is allowed";
string PS_135 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_136_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_136_Desc = "ps_3_0 : dsx v# inside rep block with breakp p.y is allowed";
string PS_136 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	dsx r2, v0 "
    "	breakp p0.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_137_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_137_Desc = "ps_3_0 : dsx v# inside rep block with breakp p.z is allowed";
string PS_137 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_138_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_138_Desc = "ps_3_0 : dsx v# inside rep block with breakp !p.z is allowed";
string PS_138 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	dsx r2, v0 "
    "	breakp !p0.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
PS_CRITERIA PS_139_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_139_Desc = "ps_3_0 : dsx v# inside rep block with breakp !p.w is allowed";
string PS_139 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
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
    "	dsx r2, v0 "
    "endrep "
    "mov oC0, r2 "; 

PS_CRITERIA PS_140_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_140_Desc = "ps_3_0 : dsx v# inside rep block with breakp p.w is allowed";
string PS_140 =
	"ps_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_2d s0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	dsx r2, v0 "
    "	breakp p0.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov oC0, r2 ";
    
    #include "Test_Include.fx"

int Test_Count = 127;

string TestInfo
<
	string TestType = "VS";
>
= "vs_1_1_inst_m4x3";

VS_CRITERIA VS_001_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_1_1 : m4x3 source reg combination v0, c0 is allowed";
string VS_001 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"m4x3 r0, v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_002_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_1_1 : m4x3 source reg combination -v0, c0 is allowed";
string VS_002 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"m4x3 r0, -v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_1_1 : m4x3 source reg combination v0, c0 is allowed";
string VS_003 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"m4x3 r0, v0, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_1_1 : m4x3 source reg combination v0.x, c0 is allowed";
string VS_004 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"m4x3 r0, v0.x, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_005_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_1_1 : m4x3 source reg combination v0.y, c0 is allowed";
string VS_005 = 
	"vs_1_1 "
	"def c0, 1, 1, 1, 1 "
	"dcl_color v0 "
	"m4x3 r0, v0.y, c0 "
	"mov oPos, c0 ";

VS_CRITERIA VS_006_Criteria = { true, false, 0x0101, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_1_1 : m4x3 source reg combination v0.z, c0 is allowed";
string VS_006 = 
	"vs_1_1 "
	"def 