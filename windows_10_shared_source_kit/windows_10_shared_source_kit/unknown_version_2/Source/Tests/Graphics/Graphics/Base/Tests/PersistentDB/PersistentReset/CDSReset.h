2add is NOT allowed in a loop aL, i0 break and if_ge c0.x, c0.y nop else block";
string PS_511 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	if_ge c0.x, c0.y nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_512_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_512_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break and if p0.x block";
string PS_512 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_513_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_513_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_513 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_514_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_514_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_514 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_515_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_515_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break and rep i0 bre