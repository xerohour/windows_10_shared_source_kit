NOT allowed";
string PS_702 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"sincos r4.r, oDepth, s0, p0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_703_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_703_Desc = "ps_2_0 : sincos source reg combination oDepth, s0, s0 is NOT allowed";
string PS_703 = 
	"ps_2_0 "
	"dcl_volume s0 "
	"sincos r4.r, oDepth, s0, s0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_704_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_704_D