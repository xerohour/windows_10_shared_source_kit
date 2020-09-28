431 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_432_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_432_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and if p0.x block";
string PS_432 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_color v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_433_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_433_Desc = "ps_3_0 : sub is allowed in a rep i0 breakp p0.x and if p0.x nop else block";
string PS_433 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_fog v0 "
	"mov r0, c0 "
	"rep i0 breakp p0.x "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_434_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_434_Desc = "ps_3