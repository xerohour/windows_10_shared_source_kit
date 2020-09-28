A PS_701_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_701_Desc = "ps_2_0 : sincos source reg combination oDepth, s0, i0 is NOT allowed";
string PS_701 = 
	"ps_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_volume s0 "
	"sincos r4.r, oDepth, s0, i0 "
	"mov oC0, c0 ";

PS_CRITERIA PS_702_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_702_Desc = "ps_2_0 : sincos source reg combination oDepth, s0, p0 is 