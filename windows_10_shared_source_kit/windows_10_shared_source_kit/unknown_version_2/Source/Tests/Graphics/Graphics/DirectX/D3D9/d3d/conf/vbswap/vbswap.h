
	"endloop ";

PS_CRITERIA PS_300_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_300_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and if_eq c0.x, c0.y block";
string PS_300 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	if_eq c0.x, c0.y "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_301_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_301_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and if_eq c0.x, c0.y nop else block";
string PS_301 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	if_eq c0.x, c0.y nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_302_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_302_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x block";
string PS_302 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_303_Criteria = { false, false, 0x0200, 1, -1, 1, -1, CAPS_PREDICATION, -1, 0.f };
string PS_303_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and if p0.x nop else block";
string PS_303 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	if p0.x nop else "
	"		abs r0, v0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_304_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_304_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 break block";
string PS_304 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 break "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_305_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_305_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 break block";
string PS_305 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 break "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_306_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_306_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 breakp p0.x block";
string PS_306 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	loop aL, i0 breakp p0.x "
	"		abs r0, v0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_307_Criteria = { false, false, 0x0200, 0, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_307_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and rep i0 breakp p0.x block";
string PS_307 = 
	"ps_2_x "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"loop aL, i0 breakp p0.x "
	"	rep i0 breakp p0.x "
	"		abs r0, v0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_308_Criteria = { false, false, 0x0200, 1, -1, 2, -1, CAPS_PREDICATION, -1, 0.f };
string PS_308_Desc = "ps_2_x : abs is NOT allowed in a loop aL, i0 breakp p0.x and loop aL, i0 b