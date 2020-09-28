L, i0 break and if_eq c0.x, c0.y nop else block";
string VS_371 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if_eq c0.x, c0.y nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_372_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_372_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and if p0.x block";
string VS_372 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_373_Criteria = { true, false, 0x0200, 24, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string VS_373_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and if p0.x nop else block";
string VS_373 = 
	"vs_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl_depth v0 "
	"mov r0, c0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		sub r0, v0, r0 "
	"	endif "
	"endloop ";

VS_CRITERIA VS_374_Criteria = { true, false, 0x0200, 24, -1, 2, -1, 0, -1, 0.f };
string VS_374_Desc = "vs_2_x : sub is allowed in a loop aL, i0 break and loop aL, i0 break block";
string VS_374 = 
	"vs_2_x