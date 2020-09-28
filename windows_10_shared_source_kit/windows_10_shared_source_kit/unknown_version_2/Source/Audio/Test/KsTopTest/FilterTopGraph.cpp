 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.gbra, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_021_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_021_Desc = "ps_2_0 : add source reg combination r0.brga, r1 is allowed";
string PS_021 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.brga, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_022_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_022_Desc = "ps_2_0 : add source reg combination r0.abgr, r1 is allowed";
string PS_022 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0.abgr, r1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_023_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_023_Desc = "ps_2_0 : add source reg combination r0, r1.r is allowed";
string PS_023 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.r "
	"mov oC0, c0 ";

PS_CRITERIA PS_024_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_024_Desc = "ps_2_0 : add source reg combination r0, r1.g is allowed";
string PS_024 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.g "
	"mov oC0, c0 ";

PS_CRITERIA PS_025_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_025_Desc = "ps_2_0 : add source reg combination r0, r1.b is allowed";
string PS_025 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.b "
	"mov oC0, c0 ";

PS_CRITERIA PS_026_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_026_Desc = "ps_2_0 : add source reg combination r0, r1.a is allowed";
string PS_026 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.a "
	"mov oC0, c0 ";

PS_CRITERIA PS_027_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_027_Desc = "ps_2_0 : add source reg combination r0, r1.gbra is allowed";
string PS_027 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.gbra "
	"mov oC0, c0 ";

PS_CRITERIA PS_028_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_028_Desc = "ps_2_0 : add source reg combination r0, r1.brga is allowed";
string PS_028 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.brga "
	"mov oC0, c0 ";

PS_CRITERIA PS_029_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_029_Desc = "ps_2_0 : add source reg combination r0, r1.abgr is allowed";
string PS_029 = 
	"ps_2_0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"add r0, r0, r1.abgr "
	"mov oC0, c0 ";

PS_CRITERIA PS_030_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 9.109f };
string PS_030_Desc = "ps_2_0 : add source reg combination t0, t0 is NOT allowed";
string PS_030 = 
	"ps_2_0 "
	"dcl t0 "
	"add r0, t0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_031_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_031_Desc = "ps_2_0 : add source reg combination t0, t1 is NOT allowed";
string PS_031 = 
	"ps_2_0 "
	"dcl t0 "
	"dcl t1 "
	"add r0, t0, t1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_032_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_032_Desc = "ps_2_0 : add source reg combination t0, v0 is NOT allowed";
string PS_032 = 
	"ps_2_0 "
	"dcl t0 "
	"dcl v0 "
	"add r0, t0, v0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_033_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_033_Desc = "ps_2_0 : add source reg combination v0, t0 is NOT allowed";
string PS_033 = 
	"ps_2_0 "
	"dcl v0 "
	"dcl t0 "
	"add r0, v0, t0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_034_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_034_Desc = "ps_2_0 : add source reg combination v0, v1 is NOT allowed";
string PS_034 = 
	"ps_2_0 "
	"dcl v0 "
	"dcl v1 "
	"add r0, v0, v1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_035_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_035_Desc = "ps_2_0 : add source reg combination c0, c1 is NOT allowed";
string PS_035 = 
	"ps_2_0 "
	"def c1, 1, 1, 1, 1 "
	"def c0, 1, 1, 1, 1 "
	"add r0, c0, c1 "
	"mov oC0, c0 ";

PS_CRITERIA PS_036_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_036_Desc = "ps_2_0 : add source reg combination b0, b0 is NOT allowed";
string PS_036 = 
	"ps_2_0 "
	"defb b0, true "
	"add r0, b0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_037_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_037_Desc = "ps_2_0 : add source reg combination b0, i0 is NOT allowed";
string PS_037 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"add r0, b0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_038_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_038_Desc = "ps_2_0 : add source reg combination b0, p0 is NOT allowed";
string PS_038 = 
	"ps_2_0 "
	"defb b0, true "
	"add r0, b0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_039_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_039_Desc = "ps_2_0 : add source reg combination b0, s0 is NOT allowed";
string PS_039 = 
	"ps_2_0 "
	"defb b0, true "
	"dcl_cube s0 "
	"add r0, b0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_040_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_040_Desc = "ps_2_0 : add source reg combination b0, oC0 is NOT allowed";
string PS_040 = 
	"ps_2_0 "
	"defb b0, true "
	"add r0, b0, oC0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_041_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_041_Desc = "ps_2_0 : add source reg combination b0, oDepth is NOT allowed";
string PS_041 = 
	"ps_2_0 "
	"defb b0, true "
	"add r0, b0, oDepth "
	"mov oC0, c0 ";

PS_CRITERIA PS_042_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_042_Desc = "ps_2_0 : add source reg combination i0, b0 is NOT allowed";
string PS_042 = 
	"ps_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"add r0, i0, b0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_043_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_043_Desc = "ps_2_0 : add source reg combination i0, i0 is NOT allowed";
string PS_043 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"add r0, i0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_044_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_044_Desc = "ps_2_0 : add source reg combination i0, p0 is NOT allowed";
string PS_044 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"add r0, i0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_045_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_045_Desc = "ps_2_0 : add source reg combination i0, s0 is NOT allowed";
string PS_045 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_cube s0 "
	"add r0, i0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_046_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
st