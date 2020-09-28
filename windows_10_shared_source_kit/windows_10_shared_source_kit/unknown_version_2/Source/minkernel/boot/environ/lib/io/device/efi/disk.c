riteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_527_Desc = "ps_2_x : dp2add is allowed in a rep i0 break and if p0.x nop else block";
string PS_527 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if p0.x nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_528_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_528_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 break and loop aL, i0 break block";
string PS_528 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_529_Criteria = { true, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_529_Desc = "ps_2_x : dp2add is allowed in a rep i0 break and rep i0 break block";
string PS_529 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_530_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_530_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 break and loop aL, i0 breakp p0.x block";
string PS_530 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_531_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_531_Desc = "ps_2_x : dp2add is allowed in a rep i0 break and rep i0 breakp p0.x block";
string PS_531 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_532_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_532_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 break and loop aL, i0 break_lt c0.x, c0.y block";
string PS_532 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_533_Criteria = { true, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_533_Desc = "ps_2_x : dp2add is allowed in a rep i0 break and rep i0 break_lt c0.x, c0.y block";
string PS_533 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 break_lt c0.x, c0.y 