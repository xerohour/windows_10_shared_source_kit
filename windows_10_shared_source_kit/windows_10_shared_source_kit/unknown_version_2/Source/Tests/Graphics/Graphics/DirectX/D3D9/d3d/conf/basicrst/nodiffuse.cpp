_0 : sub is allowed in a rep i0 breakp p0.x and loop aL, i0 break block";
string PS_434 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_435_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_435_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and rep i0 break block";
string PS_435 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_436_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_436_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_436 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_437_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_437_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_437 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_438_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_438_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_438 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_439_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_439_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and rep i0 break_eq c0.x, c0.y block";
string PS_439 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_440_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_440_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 block";
string PS_440 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_441_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_441_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and if b0 nop else block";
string PS_441 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_442_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_442_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 block";
string PS_442 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_443_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_443_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and rep i0 block";
string PS_443 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_444_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_444_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and if_eq c0.x, c0.y block";
string PS_444 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_445_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_445_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and if_eq c0.x, c0.y nop else block";
string PS_445 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if_eq c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_446_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_446_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x block";
string PS_446 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_447_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_447_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and if p0.x nop else block";
string PS_447 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_448_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_448_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break_eq c0.x, c0.y and loop aL, i0 break block";
string PS_448 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"loop aL, i0 break_eq c0.x, c0.y "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_449_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_449_Desc = "ps_3_0 : sub is allowed in