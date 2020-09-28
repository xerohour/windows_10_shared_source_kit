ndrep "
	"endif ";

PS_CRITERIA PS_382_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_382_Desc = "ps_3_0 : sub is allowed in a if p0.x nop else and loop aL, i0 break_eq c0.x, c0.y block";
string PS_382 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	loop aL, i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_383_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_383_Desc = "ps_3_0 : sub is allowed in a if p0.x nop else and rep i0 break_eq c0.x, c0.y block";
string PS_383 = 
	"ps_3_0 "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_384_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_384_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and if b0 block";
string PS_384 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_385_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_385_Desc = "ps_3_0 : sub is allowed in a loop aL, i0 break and if b0 nop else block";
string PS_385 = 
	"ps_3_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_386_Criteria = { true, fals