 and loop aL, i0 break_eq c0.x, c0.y block";
string PS_410 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_411_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_411_Desc = "ps_3_0 : sub is allowed in a rep i0 break and rep i0 break_eq c0.x, c0.y block";
string PS_411 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 break "
	"	rep i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_412_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_412_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and if b0 block";
string PS_412 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_413_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_413_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and if b0 nop else block";
string PS_413 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_414_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_414_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 block";
string PS_414 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_415_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_415_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and rep i0 block";
string PS_415 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_416_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_416_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and if_eq c0.x, c0.y block";
string PS_416 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_417_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_417_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and if_eq c0.x, c0.y nop else block";
string PS_417 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_418_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_418_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_418 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_419_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_419_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_419 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_420_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_420_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_420 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_421_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_421_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_421 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_422_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_422_Desc = "ps_