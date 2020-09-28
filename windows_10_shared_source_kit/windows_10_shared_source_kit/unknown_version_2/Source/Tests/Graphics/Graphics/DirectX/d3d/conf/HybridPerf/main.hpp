ak block";
string PS_515 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_516_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_516_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_516 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_517_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_517_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_517 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_518_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_518_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break and loop aL, i0 break_lt c0.x, c0.y block";
string PS_518 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_519_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_519_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break and rep i0 break_lt c0.x, c0.y block";
string PS_519 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	rep i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_520_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_520_Desc = "ps_2_x : dp2add is allowed in a rep i0 break and if b0 block";
string PS_520 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if b0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_521_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_521_Desc = "ps_2_x : dp2add is allowed in a rep i0 break and if b0 nop else block";
string PS_521 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if b0 nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_522_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_522_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 break and loop aL, i0 block";
string PS_522 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_523_Criteria = { true, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_523_Desc = "ps_2_x : dp2add is allowed in a rep i0 break and rep i0 block";
string PS_523 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	rep i0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_524_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_524_Desc = "ps_2_x : dp2add is allowed in a rep i0 break and if_ge c0.x, c0.y block";
string PS_524 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if_ge c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_525_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_525_Desc = "ps_2_x : dp2add is allowed in a rep i0 break and if_ge c0.x, c0.y nop else block";
string PS_525 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if_ge c0.x, c0.y nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_526_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_526_Desc = "ps_2_x : dp2add is allowed in a rep i0 break and if p0.x block";
string PS_526 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break "
	"	if p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_527_C