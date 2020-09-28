_sw : setp_gt is NOT allowed in a if p0.x nop else and loop aL, i0 break_le c0.x, c0.y block";
string PS_328 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	loop aL, i0 break_le c0.x, c0.y "
	"		setp_gt p0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_329_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_329_Desc = "ps_2_sw : setp_gt is allowed in a if p0.x nop else and rep i0 break_le c0.x, c0.y block";
string PS_