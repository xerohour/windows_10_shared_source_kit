ue, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_362_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and if b0 nop else block";
string PS_362 = 
	"ps_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_363_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_363_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_363 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_364_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_364_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and rep i0 block";
string PS_364 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_365_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_365_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and if_ne c0.x, c0.y block";
string PS_365 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_366_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_366_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and if_ne c0.x, c0.y nop else block";
string PS_366 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if_ne c0.x, c0.y nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_367_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_367_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and if p0.x block";
string PS_367 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_368_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_368_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_368 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_369_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_369_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_369 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_370_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_370_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and rep i0 break block";
string PS_370 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_371_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_371_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_371 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_tangent v0 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_372_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_372_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_372 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_373_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_373_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and loop aL, i0 break_ne c0.x, c0.y block";
string PS_373 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 break "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_374_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_374_Desc = "ps_3_sw : m3x3 is allowed in a loop aL, i0 break and rep i0 break_ne c0.x, c0.y block";
string PS_374 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 break "
	"	rep i0 break_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_375_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_375_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 break and if b0 block";
string PS_375 = 
	"ps_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 break "
	"	if b0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_376_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_376_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 break and if b0 nop else block";
string PS_376 = 
	"ps_3_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 break "
	"	if b0 nop else "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_377_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_377_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 break and loop aL, i0 block";
string PS_377 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 break "
	"	loop aL, i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_378_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_378_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 break and rep i0 block";
string PS_378 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 break "
	"	rep i0 "
	"		m3x3 r0.rgb, v0, c0 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_379_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_379_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 break and if_ne c0.x, c0.y block";
string PS_379 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 break "
	"	if_ne c0.x, c0.y "
	"		m3x3 r0.rgb, v0, c0 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_380_Criteria = { true, true, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string PS_380_Desc = "ps_3_sw : m3x3 is allowed in a rep i0 break and if_ne c0.x, c0.y nop else block";
string PS_380 = 
	"ps_3_sw "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 break "
	"	if_ne c0.x, c0.y nop else "
	"		m3x3 r0.r