 sub is allowed in a if p0.x nop else and rep i0 break_eq c0.x, c0.y block";
string VS_365 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"if p0.x nop else "
	"	rep i0 break_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endrep "
	"endif ";

VS_CRITERIA VS_366_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_366_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and if b0 block";
string VS_366 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if b0 "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_367_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_367_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and if b0 nop else block";
string VS_367 = 
	"vs_2_x "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_368_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_368_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and loop aL, i0 block";
string VS_368 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		sub r0, v0, r0 "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_369_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_369_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and rep i0 block";
string VS_369 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		sub r0, v0, r0 "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_370_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_370_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and if_eq c0.x, c0.y block";
string VS_370 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if_eq c0.x, c0.y "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_371_Criteria = { true, false, 0x0200, 24, -1, 1, -1, 0, -1, 0.f };
string VS_371_Desc = "vs_2_x : sub is allowed in a loop a