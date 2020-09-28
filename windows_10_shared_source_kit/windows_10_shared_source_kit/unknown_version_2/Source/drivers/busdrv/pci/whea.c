nd loop aL, i0 breakp p0.x block";
string PS_325 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_326_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_326_Desc = "ps_2_sw : dp3 is allowed in a if p0.x nop else and rep i0 breakp p0.x block";
string PS_326 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_327_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_327_Desc = "ps_2_sw : dp3 is NOT allowed in a if p0.x nop else and loop aL, i0 break_ne c0.x, c0.y block";
string PS_327 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		dp3 r0, v0, v1 "
	"	endloop "
	"endif ";

PS_CRITERIA PS_328_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_328_Desc = "ps_2_sw : dp3 is allowed in a if p0.x nop else and rep i0 break_ne c0.x, c0.y block";
string PS_328 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"if p0.x nop else "
	"	rep i0 break_ne c0.x, c0.y "
	"		dp3 r0, v0, v1 "
	"	endrep "
	"endif ";

PS_CRITERIA PS_329_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_329_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and if b0 block";
string PS_329 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if b0 "
	"		dp3 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_330_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_330_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and if b0 nop else block";
string PS_330 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if b0 nop else "
	"		dp3 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_331_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_331_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and loop aL, i0 block";
string PS_331 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 "
	"		dp3 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_332_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_332_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and rep i0 block";
string PS_332 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 "
	"		dp3 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_333_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_333_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and if_ge c0.x, c0.y block";
string PS_333 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if_ge c0.x, c0.y "
	"		dp3 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_334_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_334_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and if_ge c0.x, c0.y nop else block";
string PS_334 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if_ge c0.x, c0.y nop else "
	"		dp3 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_335_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_335_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and if p0.x block";
string PS_335 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if p0.x "
	"		dp3 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_336_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_336_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and if p0.x nop else block";
string PS_336 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	if p0.x nop else "
	"		dp3 r0, v0, v1 "
	"	endif "
	"endloop ";

PS_CRITERIA PS_337_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_337_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and loop aL, i0 break block";
string PS_337 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break "
	"		dp3 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_338_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_338_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and rep i0 break block";
string PS_338 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 break "
	"		dp3 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_339_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_339_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and loop aL, i0 breakp p0.x block";
string PS_339 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 breakp p0.x "
	"		dp3 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_340_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_340_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and rep i0 breakp p0.x block";
string PS_340 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 breakp p0.x "
	"		dp3 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_341_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_341_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and loop aL, i0 break_ne c0.x, c0.y block";
string PS_341 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	loop aL, i0 break_ne c0.x, c0.y "
	"		dp3 r0, v0, v1 "
	"	endloop "
	"endloop ";

PS_CRITERIA PS_342_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_342_Desc = "ps_2_sw : dp3 is NOT allowed in a loop aL, i0 break and rep i0 break_ne c0.x, c0.y block";
string PS_342 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"loop aL, i0 break "
	"	rep i0 break_ne c0.x, c0.y "
	"		dp3 r0, v0, v1 "
	"	endrep "
	"endloop ";

PS_CRITERIA PS_343_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_343_Desc = "ps_2_sw : dp3 is allowed in a rep i0 break and if b0 block";
string PS_343 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if b0 "
	"		dp3 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_344_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_344_Desc = "ps_2_sw : dp3 is allowed in a rep i0 break and if b0 nop else block";
string PS_344 = 
	"ps_2_sw "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if b0 nop else "
	"		dp3 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_345_Criteria = { false, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_345_Desc = "ps_2_sw : dp3 is NOT allowed in a rep i0 break and loop aL, i0 block";
string PS_345 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	loop aL, i0 "
	"		dp3 r0, v0, v1 "
	"	endloop "
	"endrep ";

PS_CRITERIA PS_346_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_346_Desc = "ps_2_sw : dp3 is allowed in a rep i0 break and rep i0 block";
string PS_346 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	rep i0 "
	"		dp3 r0, v0, v1 "
	"	endrep "
	"endrep ";

PS_CRITERIA PS_347_Criteria = { true, true, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string PS_347_Desc = "ps_2_sw : dp3 is allowed in a rep i0 break and if_ge c0.x, c0.y block";
string PS_347 = 
	"ps_2_sw "
	"defi i0, 0, 0, 0, 0 "
	"dcl v0 "
	"dcl v1 "
	"rep i0 break "
	"	if_ge c0.x, c0.y "
	"		dp3 r0, v0, v1 "
	"	endif "
	"endrep ";

PS_CRITERIA PS_348_Criteria = { true, true, 0x