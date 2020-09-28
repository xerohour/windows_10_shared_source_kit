"
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_534_Criteria = { false, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_534_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_534 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_535_Criteria = { false, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_535_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_535 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_536_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_536_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_536 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_537_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_537_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_537 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_538_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_538_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and if_ge c0.x, c0.y block";
string PS_538 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_ge c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_539_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_539_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and if_ge c0.x, c0.y nop else block";
string PS_539 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_540_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_540_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_540 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_541_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_541_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_541 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_542_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_542_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_542 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_543_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_543_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_543 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_544_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_544_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_544 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_545_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_545_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_545 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_546_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_546_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_lt c0.x, c0.y block";
string PS_546 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_547_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_547_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break_lt c0.x, c0.y block";
string PS_547 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_548_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_548_Desc = "ps_2_x : dp2add is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_548 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_549_Criteria = { true, false, 0x0200, 0, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_549_Desc = "ps_2_x : dp2add is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_549 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_550_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_550_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_550 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_551_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_551_Desc = "ps_2_x : dp2add is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_551 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_552_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_552_Desc = "ps_2_x : dp2add is allowed in a rep i0 breakp p0.x and if_ge c0.x, c0.y block";
string PS_552 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if_ge c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_553_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_553_Desc = "ps_2_x : dp2add is allowed in a rep i0 breakp p0.x and if_ge c0.x, c0.y nop else block";
string PS_553 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if_ge c0.x, c0.y nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_554_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_554_Desc = "ps_2_x : dp2add is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_554 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_555_Criteria = { true, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_555_Desc = "ps_2_x : dp2add is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_555 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_556_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_556_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_556 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_557_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_557_Desc = "ps_2_x : dp2add is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_557 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_558_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_558_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_558 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_559_Criteria = { true, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_559_Desc = "ps_2_x : dp2add is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_559 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_560_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_560_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 breakp p0.x and loop aL, i0 break_lt c0.x, c0.y block";
string PS_560 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_561_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_561_Desc = "ps_2_x : dp2add is allowed in a rep i0 breakp p0.x and rep i0 break_lt c0.x, c0.y block";
string PS_561 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_562_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_562_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and if b0 block";
string PS_562 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if b0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_563_Criteria = { false, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_563_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and if b0 nop else block";
string PS_563 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if b0 nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_564_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_564_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and loop aL, i0 block";
string PS_564 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	loop aL, i0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_565_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_565_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and rep i0 block";
string PS_565 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	rep i0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_566_Criteria = { false, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_566_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and if_ge c0.x, c0.y block";
string PS_566 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_567_Criteria = { false, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_567_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string PS_567 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_568_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_568_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and if p0.x block";
string PS_568 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_569_Criteria = { false, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_569_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and if p0.x nop else block";
string PS_569 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	if p0.x nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endloop ";

PS_CRITERIA PS_570_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_570_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and loop aL, i0 break block";
string PS_570 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_571_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_571_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and rep i0 break block";
string PS_571 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	rep i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_572_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_572_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_572 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_573_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_573_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and rep i0 breakp p0.x block";
string PS_573 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_574_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_574_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and loop aL, i0 break_lt c0.x, c0.y block";
string PS_574 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_575_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_575_Desc = "ps_2_x : dp2add is NOT allowed in a loop aL, i0 break_lt c0.x, c0.y and rep i0 break_lt c0.x, c0.y block";
string PS_575 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"loop aL, i0 break_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_576_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_576_Desc = "ps_2_x : dp2add is allowed in a rep i0 break_lt c0.x, c0.y and if b0 block";
string PS_576 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if b0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_577_Criteria = { true, false, 0x0200, 1, -1, 1, -1, 0, -1, 0.f };
string PS_577_Desc = "ps_2_x : dp2add is allowed in a rep i0 break_lt c0.x, c0.y and if b0 nop else block";
string PS_577 = 
	"ps_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if b0 nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_578_Criteria = { false, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_578_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 block";
string PS_578 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_579_Criteria = { true, false, 0x0200, 1, -1, 2, -1, 0, -1, 0.f };
string PS_579_Desc = "ps_2_x : dp2add is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 block";
string PS_579 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_580_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_580_Desc = "ps_2_x : dp2add is allowed in a rep i0 break_lt c0.x, c0.y and if_ge c0.x, c0.y block";
string PS_580 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if_ge c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_581_Criteria = { true, false, 0x0200, 2, -1, 1, -1, 0, -1, 0.f };
string PS_581_Desc = "ps_2_x : dp2add is allowed in a rep i0 break_lt c0.x, c0.y and if_ge c0.x, c0.y nop else block";
string PS_581 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if_ge c0.x, c0.y nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_582_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_582_Desc = "ps_2_x : dp2add is allowed in a rep i0 break_lt c0.x, c0.y and if p0.x block";
string PS_582 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_583_Criteria = { true, false, 0x0200, 2, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_583_Desc = "ps_2_x : dp2add is allowed in a rep i0 break_lt c0.x, c0.y and if p0.x nop else block";
string PS_583 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	if p0.x nop else "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endif "
	"endrep ";

PS_CRITERIA PS_584_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_584_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 break block";
string PS_584 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_585_Criteria = { true, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_585_Desc = "ps_2_x : dp2add is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 break block";
string PS_585 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_586_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_586_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 breakp p0.x block";
string PS_586 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_587_Criteria = { true, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_587_Desc = "ps_2_x : dp2add is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 breakp p0.x block";
string PS_587 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 breakp p0.x "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_588_Criteria = { false, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_588_Desc = "ps_2_x : dp2add is NOT allowed in a rep i0 break_lt c0.x, c0.y and loop aL, i0 break_lt c0.x, c0.y block";
string PS_588 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	loop aL, i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_589_Criteria = { true, false, 0x0200, 2, -1, 2, -1, 0, -1, 0.f };
string PS_589_Desc = "ps_2_x : dp2add is allowed in a rep i0 break_lt c0.x, c0.y and rep i0 break_lt c0.x, c0.y block";
string PS_589 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"mov r0, c0 "
	"mov r1, c0 "
	"rep i0 break_lt c0.x, c0.y "
	"	rep i0 break_lt c0.x, c0.y "
	"		dp2add r0, v0.x, r0.x, r1.x "
	"	endrep "
	"endrep ";


/******************************Module*Header*******************************
 * Module Name: CDSReset.h
 *
 * Header for Reseting system configuration test component using ChangeDisplaySetting
 *
 * Author: Mai Dao [maithida]
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 **************************************************************************/
#ifndef __TEST_CDS_RESET_H_
#define __TEST_CDS_RESET_H_

#include <windows.h>
#include "TestComponent.h"
#include "PDBCcdConfig.h"

// The possible results when attempting to set a display mode
enum SetDisplayModeResult
{
    DisplayModeNotSupported = 0,    //the specified mode is not supported by the adapter
    DisplayModeSet = 1,             //the mode change was succesful
    DisplayModeFailed = 2,          //the mode change failed
};


void SetupDeviceModeCommandLine(CTestRunner& Runner, const wchar_t* szPath);

UINT FindDisplayModeIndex(std::vector<DEVMODE> &Modes, UINT nWidth, UINT nHeight, UINT nFreq, UINT nRotation);

class CCDSReset : public CTestComponent
{
public:
    CCDSReset();
    ~CCDSReset();

protected:
    virtual void PreWalk(CLogWriter* pLog);
    virtual void PostWalk(CLogWriter* pLog);

private:
    CLogWriter* m_pLog;
    bool m_FailResult;
    // Saves the adapter names
    std::vector<std::wstring> m_AdapterName;
    // Saves display mode that support by the system, and specify by the command line
    std::vector<DEVMODE> m_Modes;
    bool m_ModeChange;

    // Set display modes
    SetDisplayModeResult SetDisplayMode(CLogWriter* pLog,
        const wchar_t* AdapterName, DEVMODE* Mode, UINT dwflags);
};

#endif
// The ActionMacros.h file requires the following three headers
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
                
// Included for friendly macros
#include <ActionMacros.h>

#include <tchar.h>
#include <dxgi.h>

#include <tdh.h>
#include <EventHelper.h>
#include <etwLibEvents.h>

#include <Provider.h>
#include "etwLibEventHelper.h"

#include <D3Dx9math.h>
#include <d3d11_2.h>

#include "WexTestClass.h"
#include <vector>
#include "XperfHelper.h"
#include "DxEtwHelper.h"
#include "SchedPerfCommon.h"

#include <atlbase.h>
#include "GraphicsWorkload.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace WEX::Common;

#define BASIC_RENDER_VENDOR 0x1414
#define BASIC_RENDER_DEVICE 0x8c

class HybridPerf
{
public:
    BEGIN_TEST_CLASS(HybridPerf)
        TEST_CLASS_PROPERTY( L"Kits.Specification", L"System.Fundamentals.Graphics.HybridGraphics.MultiGPU" )
		TEST_CLASS_PROPERTY( L"Kits.RunElevated", L"true" )
    END_TEST_CLASS();

    TEST_CLASS_SETUP(Setup);

    void RunTest(UINT numShaderLoops, UINT numPrimitives, wchar_t* testName);

#if 0
    // Disabling as part of bug 2242775
    BEGIN_TEST_METHOD(FillRate)
		TEST_METHOD_PROPERTY( L"Kits.Description", L"This test verifies whether the discrete GPU is able to rasterize more quickly than the integrated GPU." )

		TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test for hybrid fill rate performance" )
		TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
		TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )
		TEST_METHOD_PROPERTY( L"Kits.Usage", L"This test ensures that the discrete GPU in a hybrid configuration is able to produce pixels with a simple shader faster than the integrated GPU in the system." )

		TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
        TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
        TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
		TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
		TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

		TEST_METHOD_PROPERTY( L"Kits.TestId", L"da83732d-d42e-42ce-ad66-44002bb9a239" )
		TEST_METHOD_PROPERTY( L"Kits.TestId2", L"ecb71287-959e-4895-b343-d7f6eb533d10" )
		TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
		TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
		TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )
        
		TEST_METHOD_PROPERTY( L"Kits.DeploymentItem", L"NTTEST\\windowstest\\graphics\\d3d\\support\\D3DCompiler_Test.dll" )

    END_TEST_METHOD();

    BEGIN_TEST_METHOD(ShaderThroughput)
		TEST_METHOD_PROPERTY( L"Kits.Description", L"This test verifies whether the discrete GPU is able to compute more quickly than the integrated GPU." )

		TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test for hybrid shader throughput performance" )
		TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
		TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )
		TEST_METHOD_PROPERTY( L"Kits.Usage", L"This test ensures that the discrete GPU in a hybrid configuration is able to handle shader invocations faster than the integrated GPU in the system." )

		TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
        TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
        TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
		TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
		TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

		TEST_METHOD_PROPERTY( L"Kits.TestId", L"47597ee0-e421-4477-b605-841137bf360a" )
		TEST_METHOD_PROPERTY( L"Kits.TestId2", L"f77582ce-2189-45ee-8067-a8e8d31fd45e" )
		TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
		TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
		TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

		TEST_METHOD_PROPERTY( L"Kits.DeploymentItem", L"NTTEST\\windowstest\\graphics\\d3d\\support\\D3DCompiler_Test.dll" )

    END_TEST_METHOD();
#endif
};
/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    disk.c

Abstract: 
    
    Implementation of disk device i/o routines using EFI.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "disk.h"

// ------------------------------------------------------------------ Functions

NTSTATUS
DiskFirmwareOpen (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __inout PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    Opens a disk device and collects information, such as disk geometry for the
    device. 
    
Arguments:

    Device - The Disk device to open and initialize.
             
    DeviceEn