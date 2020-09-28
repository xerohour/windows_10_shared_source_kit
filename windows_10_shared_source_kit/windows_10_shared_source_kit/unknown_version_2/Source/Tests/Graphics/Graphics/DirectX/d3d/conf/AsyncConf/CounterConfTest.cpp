3_0 : sub is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_422 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_423_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_423_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_423 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_424_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_424_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break_eq c0.x, c0.y block";
string PS_424 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_425_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_425_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and rep i0 break_eq c0.x, c0.y block";
string PS_425 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_426_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_426_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and if b0 block";
string PS_426 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_427_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_427_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and if b0 nop else block";
string PS_427 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_428_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_428_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and loop aL, i0 block";
string PS_428 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_429_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_429_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and rep i0 block";
string PS_429 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_430_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_430_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y block";
string PS_430 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_431_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_431_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and if_eq c0.x, c0.y nop else block";
string PS_