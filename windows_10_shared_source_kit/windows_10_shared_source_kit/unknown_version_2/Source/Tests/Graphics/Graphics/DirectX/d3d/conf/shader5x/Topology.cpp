    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_084_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_084_Desc = "vs_3_0 : texldl v# inside if (false) block is allowed";
string VS_084 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_085_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_085_Desc = "vs_3_0 : texldl v# inside if !(false) block is allowed";
string VS_085 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_086_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_086_Desc = "vs_3_0 : texldl v# inside loop block is allowed";
string VS_086 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_087_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_087_Desc = "vs_3_0 : texldl v# inside rep block is allowed";
string VS_087 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_088_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_088_Desc = "vs_3_0 : texldl v# inside call block is allowed";
string VS_088 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_089_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_089_Desc = "vs_3_0 : texldl v# inside callnz (true) block is allowed";
string VS_089 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_090_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_090_Desc = "vs_3_0 : texldl v# inside callnz !(true) block is allowed";
string VS_090 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_091_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_091_Desc = "vs_3_0 : texldl v# inside callnz (false) block is allowed";
string VS_091 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_092_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_092_Desc = "vs_3_0 : texldl v# inside callnz !(false) block is allowed";
string VS_092 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_093_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_093_Desc = "vs_3_0 : texldl v# inside callnz !p.x block is allowed";
string VS_093 =
	"vs_3_0 "
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

VS_CRITERIA VS_094_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_094_Desc = "vs_3_0 : texldl v# inside callnz p.y block is allowed";
string VS_094 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_095_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_095_Desc = "vs_3_0 : texldl v# inside callnz !p.z block is allowed";
string VS_095 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_096_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_096_Desc = "vs_3_0 : texldl v# inside callnz p.w block is allowed";
string VS_096 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_097_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_097_Desc = "vs_3_0 : texldl v# inside loop block with break is allowed";
string VS_097 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_098_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_098_Desc = "vs_3_0 : texldl v# inside loop block with break is allowed";
string VS_098 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_099_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_099_Desc = "vs_3_0 : texldl v# inside rep block with break is allowed";
string VS_099 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_100_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_100_Desc = "vs_3_0 : texldl v# inside rep block with break is allowed";
string VS_100 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_101_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_101_Desc = "vs_3_0 : texldl v# inside loop block with break_gt is allowed";
string VS_101 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_102_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_102_Desc = "vs_3_0 : texldl v# inside loop block with break_gt is allowed";
string VS_102 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_103_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_103_Desc = "vs_3_0 : texldl v# inside loop block with break_lt is allowed";
string VS_103 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_104_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_104_Desc = "vs_3_0 : texldl v# inside loop block with break_lt is allowed";
string VS_104 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_105_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_105_Desc = "vs_3_0 : texldl v# inside loop block with break_ge is allowed";
string VS_105 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_106_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_106_Desc = "vs_3_0 : texldl v# inside loop block with break_ge is allowed";
string VS_106 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_107_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_107_Desc = "vs_3_0 : texldl v# inside loop block with break_le is allowed";
string VS_107 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_108_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_108_Desc = "vs_3_0 : texldl v# inside loop block with break_le is allowed";
string VS_108 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_109_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_109_Desc = "vs_3_0 : texldl v# inside loop block with break_eq is allowed";
string VS_109 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_110_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_110_Desc = "vs_3_0 : texldl v# inside loop block with break_eq is allowed";
string VS_110 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_111_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_111_Desc = "vs_3_0 : texldl v# inside loop block with break_ne is allowed";
string VS_111 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_112_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_112_Desc = "vs_3_0 : texldl v# inside loop block with break_ne is allowed";
string VS_112 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_113_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_113_Desc = "vs_3_0 : texldl v# inside rep block with break_gt is allowed";
string VS_113 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_114_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_114_Desc = "vs_3_0 : texldl v# inside rep block with break_gt is allowed";
string VS_114 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_115_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_115_Desc = "vs_3_0 : texldl v# inside rep block with break_lt is allowed";
string VS_115 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_116_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_116_Desc = "vs_3_0 : texldl v# inside rep block with break_lt is allowed";
string VS_116 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_117_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_117_Desc = "vs_3_0 : texldl v# inside rep block with break_ge is allowed";
string VS_117 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_118_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_118_Desc = "vs_3_0 : texldl v# inside rep block with break_ge is allowed";
string VS_118 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_119_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_119_Desc = "vs_3_0 : texldl v# inside rep block with break_le is allowed";
string VS_119 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_120_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_120_Desc = "vs_3_0 : texldl v# inside rep block with break_le is allowed";
string VS_120 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_121_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_121_Desc = "vs_3_0 : texldl v# inside rep block with break_eq is allowed";
string VS_121 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_122_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_122_Desc = "vs_3_0 : texldl v# inside rep block with break_eq is allowed";
string VS_122 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_123_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_123_Desc = "vs_3_0 : texldl v# inside rep block with break_ne is allowed";
string VS_123 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_124_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_124_Desc = "vs_3_0 : texldl v# inside rep block with break_ne is allowed";
string VS_124 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_125_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_125_Desc = "vs_3_0 : texldl v# inside loop block with breakp !p.x is allowed";
string VS_125 =
	"vs_3_0 "
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


VS_CRITERIA VS_126_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_126_Desc = "vs_3_0 : texldl v# inside loop block with breakp p.x is allowed";
string VS_126 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_127_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_127_Desc = "vs_3_0 : texldl v# inside loop block with breakp p.y is allowed";
string VS_127 =
	"vs_3_0 "
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


VS_CRITERIA VS_128_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_128_Desc = "vs_3_0 : texldl v# inside loop block with breakp !p.y is allowed";
string VS_128 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_129_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_129_Desc = "vs_3_0 : texldl v# inside loop block with breakp !p.z is allowed";
string VS_129 =
	"vs_3_0 "
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


VS_CRITERIA VS_130_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_130_Desc = "vs_3_0 : texldl v# inside loop block with breakp p.z is allowed";
string VS_130 =
	"vs_3_0 "
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
    

VS_CRITERIA VS_131_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_131_Desc = "vs_3_0 : texldl v# inside loop block with breakp p.w is allowed";
string VS_131 =
	"vs_3_0 "
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


VS_CRITERIA VS_132_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_132_Desc = "vs_3_0 : texldl v# inside loop block with breakp !p.w is allowed";
string VS_132 =
	"vs_3_0 "
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
    
VS_CRITERIA VS_133_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_133_Desc = "vs_3_0 : texldl v# inside rep block with breakp p.x is allowed";
string VS_133 =
	"vs_3_0 "
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

VS_CRITERIA VS_134_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_134_Desc = "vs_3_0 : texldl v# inside rep block with breakp !p.x is allowed";
string VS_134 =
	"vs_3_0 "
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
    